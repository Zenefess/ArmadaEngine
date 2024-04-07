/************************************************************
 * File: gs.map.cells.x8.lq.hlsl        Created: 2023/04/30 *
 *                                Last modified: 2023/05/20 *
 *                                                          *
 * Notes:                                                   *
 *                                                          *
 *  Copyright (c) David William Bull. All rights reserved.  *
 ************************************************************/

#include "common.hlsli"
#include "gs.mesh displacement functions.hlsli"

cbuffer CB_VIEW : register(b0) {   // 144 bytes (9 vectors)
   cmatrix projection;   // Perspective transformation
   cmatrix orthographic; // Orthographic transformation
   cfloat2 guiScale;     // Final X, Y scaling factors for GUI
   cuint2  bitField;     // 0-63==???
};

cbuffer CB_MAIN : register(b1) {   // 80 bytes (5 vectors)
   cmatrix camera;     // Camera transformation
   cuint   frameCount; // Total number of frames presented
   cfloat  frameTime;  // Duration of last frame
   cfloat  secsDelta;  // == (Time elapsed - SecsTotal)
   cuint   st_bf;      // 0-23==secsTotal (Elapsed seconds), 24==Draw transparent sprites, 25-31==???
};

cbuffer MAPDIMS_ICB : register(b2) {   // 48 bytes (3 vectors)
   cint3  iMapDim;    // X, Y, Z cell counts
   cuint  mapDimsE;   // Exponenets of .iMapDim, 24-31==Low byte of .zDim spawning offset
   cint3  iChunkDim;    // X, Y, Z cell counts per chunk
   cuint  chunkDimsE; // Exponenets of .iChunkDim, 24-31==High byte of .zDim spawning offset
   cuint  chunkCount; //   X, Y, Z chunk counts - 1, 24-31==bitFlags
   cuint2 totalCells; // Cells per chunk, Cells per map
   cuint  RES;
}

struct ELEMENT_IMM {   // 16 bytes (1 vector)
   uint2 tc;          // Texture coordinates
   uint   aft_et;      // Animation frame time : 24p8, 24-31==Base transparency
   uint   afoc_tcs_ai; // 0-7==Animation frame offset, 8-15==Animation frame count, 16-23==.tc scalar : 2p6-1, 24-31==Runtime index of atlas texture
};

struct CELL_DYN {   // 16 bytes (1 vector)
   uint eTypes;    // Element type for each layer
   uint eRatios;   // Element ratios for each layer
   uint roughness; // Roughness (lerp noise deviation) for each layer
   uint dens_warp; // Density/viscosity (dictates mesh array median height)
};

struct GOut {   // 48 bytes (3 vectors) <-- Add data for texture blending with adjacent cells?
   float4 pos      : SV_Position;
   float3 position : POSITION;
   uint   ci       : CELLINDEX;   // Cell index
   float2 tex      : TEXCOORD;
   uint2  rot      : ROTATIONS;   // sin & cos of XY rotations
   
   //float3 bw : COLOR;            // Blend weights
   //uint   ei : ELEMENTINDICES;
};

const StructuredBuffer<ELEMENT_IMM> element : register(t0);
      StructuredBuffer<CELL_DYN>    cell    : register(t1);

// index==Cell indices
[instance(32)] [maxvertexcount(4)]
void main(in point cint4x11 indices[1] : CELLS, cuint instanceID : SV_GSInstanceID, inout TriangleStream<GOut> triStream) {
   cint4    _index[11]  = indices[0];
   cuint    uiCell      = instanceID >> 2;
   cuint    uiStrip      = instanceID & 0x03;
   cuint4x4 index       = uint4x4(_index[uiCell], _index[uiCell + 1], _index[uiCell + 2], _index[uiCell + 3]);
   cuint    uiCurCell   = index[2][2];
   cmatrix  cellDensity = matrix(uint4x4(cell[index[0][0]].dens_warp, cell[index[0][1]].dens_warp, cell[index[0][2]].dens_warp, cell[index[0][3]].dens_warp,
                                         cell[index[1][0]].dens_warp, cell[index[1][1]].dens_warp, cell[index[1][2]].dens_warp, cell[index[1][3]].dens_warp,
                                         cell[index[2][0]].dens_warp, cell[index[2][1]].dens_warp, cell[uiCurCell].dens_warp,   cell[index[2][3]].dens_warp,
                                         cell[index[3][0]].dens_warp, cell[index[3][1]].dens_warp, cell[index[3][2]].dens_warp, cell[index[3][3]].dens_warp)) * rcp65534;
   // Exit if cell is empty or covered
   if((cellDensity[1][1] == 0.0f && cellDensity[1][2] == 0.0f && cellDensity[2][1] == 0.0f && cellDensity[2][2] == 0.0f) || cellDensity[2][2] > 1.0f) return;
   // Unpack cell constants
   cuint4  uiElement  = (cell[uiCurCell].eTypes >> shift8888) & 0x0FF;
   cuint4  uiElementW = (cell[uiCurCell].eRatios >> shift8888) & 0x0FF;
   cfloat  fRoughness = 0.0f; // To add...
   cfloat2 fTexWarp   = ((cell[uiCurCell].dens_warp >> uint2(16, 24)) & 0x0FF) * rcp256;
   // Unpack element transparencies
   cfloat4 fET = float4((uint4(element[uiElement.x].aft_et, element[uiElement.y].aft_et, element[uiElement.z].aft_et, element[uiElement.w].aft_et) >> 24) & 0x0FF) * 0.00390625f;
   // Sort element indices by weights
   uint4 uiIndexW = uint4(0, 1, 2, 3);
   Sort(uiElementW, uiIndexW);
   // Normalise element weights
   cfloat4 fElementW = float4(uiElementW) / float(uiElementW.x + uiElementW.y + uiElementW.z + uiElementW.w);
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
   cuint4 uiAFOC_TCS = (uint4(element[uiElIn].afoc_tcs_ai, element[uiElIn].afoc_tcs_ai, element[uiElIn].afoc_tcs_ai, element[uiElIn].afoc_tcs_ai) >> uint4(0, 8, 16, 24)) & 0x0FF;
   cfloat fAFO       = float(uiAFOC_TCS.x) * 0.00390625f;
   cfloat fAFC       = float(uiAFOC_TCS.y + 1) * 0.00390625f;
   cfloat fFT        = float(element[uiElIn].aft_et & 0x0FF) * 0.00390625f + float((element[uiElIn].aft_et >> 8) & 0x0FFFF);
   // Unpack element texture scalar & coordinates
   cfloat  fTCS = 16.0f / float(uiAFOC_TCS.z + 1);
   cfloat4 fTC  = float4((element[uiElIn].tc[0] >> uint2(0, 16)) & 0x0FFFF, (element[uiElIn].tc[1] >> uint2(0, 16)) & 0x0FFFF) * 0.000030517578125f;
   // Calculate animation frame offset
   //cfloat fFrameOS = (fTC.z - fTC.x) * trunc(((totalSecs / fFT) + fAFO) % fAFC);
   cuint  secsTotal = st_bf & 0x0FFFFFF;
   cfloat fFTDelta  = float(Seconds(secsTotal, secsDelta) * double(rcp(fFT)));
   cfloat fFrameOS  = (fTC.z - fTC.x) * trunc((fFTDelta + fAFO) % fAFC);
   //
   // Generate triangle stream
   //
   GOut output;
   cmatrix mTransform = mul(camera, projection);
   // Calculate cell position in world space
   cuint   uiCurChunk = uiCurCell / totalCells.x;
   cuint3  uiChunks   = ((chunkCount >> int3(0, 8, 16)) & 0x0FF) + 1;
   cuint3  uiChunkOS  = uint3(uiCurChunk % uiChunks.x, (uiCurChunk / uiChunks.x) % uiChunks.y, (uiCurChunk / (uiChunks.x * uiChunks.y)) % uiChunks.z);
   cuint3  uiCellOS   = uint3(uiCurCell & uint(iChunkDim.x - 1), (uiCurCell / uint(iChunkDim.x)) & uint(iChunkDim.y - 1), (uiCurCell / uint(iChunkDim.x * iChunkDim.y)) & uint(iChunkDim.z - 1));
   cfloat2 fStep      = float2(float(uiStrip & 0x01), float(uiStrip >> 1)) * 0.5f;
   cfloat4 fTStep     = float4(fStep, 0.5f, -0.5f) * fTCS;
   cint3   iPos       = int3(uiChunkOS) * int3(iChunkDim.xyz) + int3(uiCellOS) - float3(iMapDim.xy >> 1, mapDimsE >> 24 + ((chunkDimsE >> 24) << 8));
   cfloat3 fPos       = float3(iPos);
   // Calculate Texture deltas
   cfloat4 fTDx = float2(fTC.zw - fTC.xy).xyxy;
   cfloat4 fTD  = fTDx * fTStep;
   cfloat2 fTCx = float2(uiCellOS.xy) * fTCS * fTDx.xy;
   // Calculate mesh vertices
   cfloat3 fVert[4] = { fPos + Displace4x4(cellDensity, fStep, 1),                      fPos + Displace4x4(cellDensity, fStep + float2(0.0f, 0.5f), 1),
                        fPos + Displace4x4(cellDensity, fStep + float2(0.5f, 0.0f), 1), fPos + Displace4x4(cellDensity, fStep + float2(0.5f, 0.5f), 1) };

   // First vertex
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
   output.rot = PackXYSinCosFromVerts(fVert[0], fVert[3], fVert[2]);
   triStream.Append(output);
   // Third vertex
   output.pos = mul(float4(fVert[2], 1.0f), mTransform);
   output.position = fVert[2];
   output.tex += fTD.zw;
   triStream.Append(output);
   // Fourth vertex
   output.pos = mul(float4(fVert[3], 1.0f), mTransform);
   output.position = fVert[3];
   output.tex.y -= fTD.w;
   triStream.Append(output);
   triStream.RestartStrip();
}
