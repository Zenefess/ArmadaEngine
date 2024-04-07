/************************************************************
 * File: gs.map.cells.hlsl              Created: 2022/12/21 *
 *                                Last modified: 2023/04/17 *
 *                                                          *
 * Notes:                                                   *
 *                                                          *
 *  Copyright (c) David William Bull. All rights reserved.  *
 ************************************************************/

#include "common.hlsli"

cbuffer CB_VIEW : register(b0) {   // 144 bytes (9 vectors)
   const matrix projection;   // Perspective transformation
   const matrix orthographic;   // Orthographic transformation
   const float2 guiScale;      // Final X, Y scaling factors for GUI
   const uint2  bitField;      // 0-63==???
};

cbuffer CB_MAIN : register(b1) {   // 80 bytes (5 vectors)
   const matrix camera;         // Camera transformation
   const uint   frameCount;   // Total number of frames presented
   const float  frameTime;      // Duration of last frame
   const float  secsDelta;      // == (Time elapsed - SecsTotal)
   const uint   st_bf;         // 0-23==secsTotal (Elapsed seconds), 24==Draw transparent sprites, 25-31==???
};

cbuffer MAPDIMS_ICB : register(b2) {   // 48 bytes (3 vectors)
   const int3  iMapDim;      // X, Y, Z cell counts
   const uint  mapDimsE;   // Exponenets of .iMapDim, 24-31==Low byte of .zDim spawning offset
   const int3  iChunkDim;   // X, Y, Z cell counts per chunk
   const uint  chunkDimsE;   // Exponenets of .iChunkDim, 24-31==High byte of .zDim spawning offset
   const uint  chunkCount;   //   X, Y, Z chunk counts, 24-31==bitFlags
   const uint2 totalCells;   // Cells per chunk, Cells per map
   const uint  RES;
}

struct ELEMENT_IMM {   // 16 bytes (1 vector)
   uint2 tc;            // Texture coordinates
   uint   aft_et;         // Animation frame time : 24p8, 24-31==Base transparency
   uint   afoc_tcs_ai;   // 0-7==Animation frame offset, 8-15==Animation frame count, 16-23==.tc scalar : 2p6-1, 24-31==Runtime index of atlas texture
};

struct CELL_DYN {   // 16 bytes (1 vector)
   uint  et;   // Element type for each layer
   uint  er;   // Element ratios for each layer
   uint  end;   // Roughness (lerp noise deviation) for each layer
   float dens;   // Density/viscosity (dictates mesh array median height)
};

struct GOut {   // 48 bytes (3 vectors) <-- Add data for texture blending with adjacent cells?
   float4 pos : SV_Position;
   float3 position : POSITION;
   uint   ci : CELLINDEX;         // Cell index
   float2 tex : TEXCOORD;
   uint2  rot : ROTATIONS;         // sin & cos of XY rotations
   
   //float3 bw : COLOR;            // Blend weights
   //uint   ei : ELEMENTINDICES;
};

const StructuredBuffer<ELEMENT_IMM> element : register(t0);
      StructuredBuffer<CELL_DYN>    cell    : register(t1);

//---!!! Move to "height functions.hlsli" !!!---///
// Calculate Z displacements from 4x4 density array, as well as positon in world space
inline const float3 Displace4x4(in const matrix density, in const float2 position) {
   const float2 fBias_ = (position - 0.5f);
   const float2 fBias  = (0.25f - (fBias_ * fBias_)) * 2.0f;

   const vector vInner = lerp(density[1], density[2], position.x);
   const vector vOuter = lerp((density[1] - density[0]) * 0.5f + density[1], (density[2] - density[3]) * 0.5f + density[2], position.x);
   const vector vXLerp = lerp(vInner, vOuter, fBias.x);
   const float  vMajor = lerp(vXLerp[1], vXLerp[2], position.y);
   const float  vMinor = lerp((vXLerp[1] - vXLerp[0]) * 0.5f + vXLerp[1], (vXLerp[2] - vXLerp[3]) * 0.5f + vXLerp[2], position.y);

   return float3(position, -lerp(vMajor, vMinor, fBias.y));
}

// index==Cell indices
[instance(32)] [maxvertexcount(6)]   // Max 6 verts (1cell/32inst): ?,???fps ... Max 18 verts (4cell/32inst): ???fps ... Max 36 verts (8cell/32inst): ???fps
void main(in point const uint4x4 indices[1] : CELLS, const uint uiStrip : SV_GSInstanceID, inout TriangleStream<GOut> triStream) {
   const uint4x4 index     = indices[0];
   const uint    uiCurCell = index[2][2];
   // Exit if cell is empty or covered
   if(cell[uiCurCell].dens <= 0.0f) if(cell[index[1][1]].dens <= 0.0f) if(cell[index[1][2]].dens <= 0.0f) if(cell[index[2][1]].dens <= 0.0f) return;
   if(cell[uiCurCell].dens > 1.0f) return;
   // Unpack cell constants
   const uint4  uiElement  = (cell[uiCurCell].et >> shift8888) & 0x0FF;
   const uint4  uiElementW = (cell[uiCurCell].er >> shift8888) & 0x0FF;
   // Unpack element transparencies
   const float4 fET = float4((uint4(element[uiElement.x].aft_et, element[uiElement.y].aft_et, element[uiElement.z].aft_et, element[uiElement.w].aft_et) >> 24) & 0x0FF) * 0.00390625f;
   // Sort element indices by weights
   uint4 uiIndexW = uint4(0, 1, 2, 3);
   Sort(uiElementW, uiIndexW);
   // Normalise element weights
   const float4 fElementW = float4(uiElementW) / float(uiElementW.x + uiElementW.y + uiElementW.z + uiElementW.w);
   // Choose between opaque & transparent layer
   uint2 uiIndex   = 4;
   float fElementD = 1.0f;
   uint  uiElIn;
   // Choose greatest transparent & opaque layers
   if(fET[uiIndexW.w] > 0.0f) {
      uiIndex.x = uiIndexW.w;
      fElementD -= fElementW[uiIndexW.w];
   } else uiIndex.y = uiIndexW.w;
   if(fET[uiIndexW.z] > 0.0f) {
      uiIndex.x = uiIndexW.z;
      fElementD -= fElementW[uiIndexW.z];
   } else uiIndex.y = uiIndexW.z;
   if(fET[uiIndexW.y] > 0.0f) {
      uiIndex.x = uiIndexW.y;
      fElementD -= fElementW[uiIndexW.y];
   } else uiIndex.y = uiIndexW.y;
   if(fET[uiIndexW.x] > 0.0f) {
      uiIndex.x = uiIndexW.x;
      fElementD -= fElementW[uiIndexW.x];
   } else uiIndex.y = uiIndexW.x;
   // Assess visibility
   if(bitField.x & 0x01) {
      if(uiIndex.x == 4) return;   // Exit if no transparent layer
      if(fET[uiIndex.x] >= 0.995f) return;   // Exit if all layers are 99.5%~ transparent
      uiElIn = uiElement[uiIndex.x];
      fElementD = 1.0f;
   } else {
      if(uiIndex.y == 4) return;   // Exit if no opaque layer
      uiElIn = uiElement[uiIndex.y];
   }
   // Unpack animation constants and .tc scalar
   const uint4 uiAFOC_TCS = (uint4(element[uiElIn].afoc_tcs_ai, element[uiElIn].afoc_tcs_ai, element[uiElIn].afoc_tcs_ai, element[uiElIn].afoc_tcs_ai) >> uint4(0, 8, 16, 24)) & 0x0FF;
   const float fAFO       = float(uiAFOC_TCS.x) * 0.00390625f;
   const float fAFC       = float(uiAFOC_TCS.y + 1) * 0.00390625f;
   const float fFT        = float(element[uiElIn].aft_et & 0x0FF) * 0.00390625f + float((element[uiElIn].aft_et >> 8) & 0x0FFFF);
   // Unpack element texture scalar & coordinates
   const float  fTCS   = 16.0f / float(uiAFOC_TCS.z + 1);
   const float4 fTC  = float4((element[uiElIn].tc[0] >> uint2(0, 16)) & 0x0FFFF, (element[uiElIn].tc[1] >> uint2(0, 16)) & 0x0FFFF) * 0.000030517578125f;
   // Calculate animation frame offset
   //const float fFrameOS = (fTC.z - fTC.x) * trunc(((totalSecs / fFT) + fAFO) % fAFC);
   const uint  secsTotal = st_bf & 0x0FFFFFF;
   const float fFTDelta  = float(Seconds(secsTotal, secsDelta) * double(rcp(fFT)));
   const float fFrameOS  = (fTC.z - fTC.x) * trunc((fFTDelta + fAFO) % fAFC);
   //
   // Generate triangle stream
   //
   GOut output;
   const matrix mTransform = mul(camera, projection);
   // Calculate cell position in world space
   const uint3  uiChunkDimsE = uint3(chunkDimsE >> uint3(0, 8, 24));
   const uint   uiCurChunk   = uiCurCell >> (uiChunkDimsE.x + uiChunkDimsE.y + uiChunkDimsE.z);
   const uint3  uiChunks     = (chunkCount >> int3(0, 8, 16)) & 0x0FF;
//--- Add exponents of chunk sizes to eliminate divs/mods
   const uint3  uiChunkOS    = uint3(uiCurChunk % uiChunks.x, (uiCurChunk / uiChunks.x) % uiChunks.y, (uiCurChunk / (uiChunks.x * uiChunks.y)) % uiChunks.z);
   const uint3  uiCellOS     = uint3(uiCurCell & uint(iChunkDim.x - 1), (uiCurCell >> uiChunkDimsE.x) & uint(iChunkDim.y - 1), (uiCurCell >> uint(uiChunkDimsE.x + uiChunkDimsE.y)) & uint(iChunkDim.z - 1));
   const float2 fStep        = float2(float((uiStrip >> 0x03) & 0x03) * 0.25f, float(uiStrip & 0x07) * 0.125f);
   const float4 fTStep       = float4(fStep, 0.125f, -0.125f) * fTCS;
   const int3   iPos         = int3(uiChunkOS) * int3(iChunkDim.xyz) + int3(uiCellOS) - float3(iMapDim.xy >> 1, mapDimsE >> 24 + ((chunkDimsE >> 24) << 8));
//   const float  fZOS         = cell[uiCurCell].dens * fElementD;
//   const float3 fPos         = float3(iPos) + float3(fStep.xy, -fZOS);
   const float3 fPos         = float3(iPos);
   // Calculate Texture deltas
   const float4 fTDx = float4(fTC.zw - fTC.xy, fTC.zw - fTC.xy);
   const float4 fTD  = fTDx * fTStep;
   const float2 fTCx = (float2(uiCellOS.xy) * fTCS * fTDx.xy);
   // Calculate mesh vertices
   const matrix mDens  = { cell[index[0][0]].dens, cell[index[0][1]].dens, cell[index[0][2]].dens, cell[index[0][3]].dens,
                           cell[index[1][0]].dens, cell[index[1][1]].dens, cell[index[1][2]].dens, cell[index[1][3]].dens,
                           cell[index[2][0]].dens, cell[index[2][1]].dens, cell[uiCurCell].dens, cell[index[2][3]].dens,
                           cell[index[3][0]].dens, cell[index[3][1]].dens, cell[index[3][2]].dens, cell[index[3][3]].dens };
   const float3 fVert[6] = {
      fPos + Displace4x4(mDens, fStep),                        fPos + Displace4x4(mDens, fStep + float2(0.0f, 0.125f)),
      fPos + Displace4x4(mDens, fStep + float2(0.125f, 0.0f)), fPos + Displace4x4(mDens, fStep + float2(0.125f, 0.125f)),
      fPos + Displace4x4(mDens, fStep + float2(0.25f, 0.0f)),   fPos + Displace4x4(mDens, fStep + float2(0.25f, 0.125f))
   };
   // Calculate vertex specific data
   output.pos = mul(float4(fVert[0], 1.0f), mTransform);
   output.position = fVert[0];
   output.ci = uiCurCell;
   output.tex = (float2(fTCx.x + fTD.x + fFrameOS, fTCx.y + fTD.y) % fTDx.xy) + fTC.xy;
   output.rot = PackXYSinCosFromVerts(fVert[0], fVert[1], fVert[2]);
   triStream.Append(output);
   // Second vertex
   output.pos = mul(float4(fVert[1], 1.0f), mTransform);
   output.position = fVert[1];
   output.tex.y -= fTD.w;
   output.rot = PackXYSinCosFromVerts(fVert[1], fVert[3], fVert[2]);
   triStream.Append(output);
   // Third vertex
   output.pos = mul(float4(fVert[2], 1.0f), mTransform);
   output.position = fVert[2];
   output.tex += fTD.zw;
   output.rot = PackXYSinCosFromVerts(fVert[2], fVert[3], fVert[4]);
   triStream.Append(output);
   // Fourth vertex
   output.pos = mul(float4(fVert[3], 1.0f), mTransform);
   output.position = fVert[3];
   output.tex.y -= fTD.w;
   output.rot = PackXYSinCosFromVerts(fVert[3], fVert[5], fVert[4]);
   triStream.Append(output);
   // Fifth vertex
   output.pos = mul(float4(fVert[4], 1.0f), mTransform);
   output.position = fVert[4];
   output.tex += fTD.zw;
   triStream.Append(output);
   // Sixth vertex
   output.pos = mul(float4(fVert[5], 1.0f), mTransform);
   output.position = fVert[5];
   output.tex.y -= fTD.w;
   triStream.Append(output);
}
