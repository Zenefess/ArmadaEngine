/************************************************************
 * File: gs.map.cells.x8.hlsl           Created: 2023/04/29 *
 * Type: Geometry shader          Last modified: 2023/05/30 *
 *                                                          *
 * Notes:                                                   *
 *                                                          *
 * Copyright (c) David William Bull.   All rights reserved. *
 ************************************************************/

#include "common.hlsli"
#include "gs.mesh displacement functions.hlsli"

cbuffer CB_VIEW : register(b0) { // 160 bytes (10 vectors)
   const matrix projection;   // Perspective transformation
   const matrix orthographic; // Orthographic transformation
   const float2 guiScale;     // Final X & Y scaling factors for GUI
   const uint2  bitField;     // 0-63==???
   const uint4  RES;
};

cbuffer CB_MAIN : register(b1) { // 96 bytes (6 vectors)
   const matrix camera;     // Camera transformation
   const uint   frameCount; // Total number of frames presented
   const float  frameTime;  // Duration of last frame
   const float  secsDelta;  // == (Time elapsed - SecsTotal)
   const uint   st_bf;      // 0-23==secsTotal (Elapsed seconds), 24==Draw transparent sprites, 25-31==???
   const uint4  RES2;
};

cbuffer MAPDIMS_ICB : register(b2) {
   // Map dimensions: X, Y, Z cell counts - 1     --- 30 bits -- [30][..][..][..]
   // Chunk dimensions: X, Y, Z cell counts - 1   --- 18 bits -- [32][16][..][..]
   // Map Cells: Total cells per map - 1          --- 30 bits -- [..][32][14][..]
   // Chunk Cells: Total cells per chunk - 1      --- 18 bits -- [..][..][32][..]
   // Map chunks: X, Y, Z chunk counts - 1        --- 21 bits -- [..][..][..][21]
   // flag                                        ---  1 bits -- [..][..][..][22]
   // zso: Z spawning offset - 1                  --- 10 bits -- [..][..][..][32]
   const uint4 dimData;
}

struct ELEMENT_IMM { // 16 bytes (1 vector)
   uint2 tc;          // Texture coordinates
   uint  aft_et;      // Animation frame time : 24p8, 24-31==Base transparency
   uint  afoc_tcs_ai; // 0-7==Animation frame offset, 8-15==Animation frame count, 16-23==.tc scalar : 2p6-1, 24-30==Runtime index of atlas texture, 31==???
};

struct CELL_DYN { // 16 bytes (1 vector)
   uint eTypes;    // Element type for each layer
   uint eRatios;   // Element ratios for each layer
   uint roughness; // Roughness (lerp noise deviation) for each layer
//   uint dens_warp; // Density/viscosity (dictates mesh array median height)
   float dens; // Density/viscosity (dictates mesh array median height)
};

struct GOut { // 48 bytes (3 vectors) <-- Add data for texture blending with adjacent cells?
   float4 pos      : SV_Position;
   float3 position : POSITION;
   uint   ci       : CELLINDEX;   // Cell index
   float2 tex      : TEXCOORD;
   uint2  rot      : ROTATIONS;   // sin & cos of XY rotations
   
   //float3 bw : COLOR;             // Blend weights
   //uint   ei : ELEMENTINDICES;
};

const StructuredBuffer<ELEMENT_IMM> element : register(t0);
const StructuredBuffer<CELL_DYN>    cell    : register(t1);

// index==Cell indices
[instance(32)] [maxvertexcount(36)]
void main(in point const int4x11 indices[1] : CELLS, const uint instanceID : SV_GSInstanceID, inout TriangleStream<GOut> triStream) {
   const int3  iMapDim    = ((dimData.x >> uint3(0, 10u, 20u)) & 0x03FFu) + 1;
   const uint  zso        = (dimData.w >> 22u) + 1u;
   const int3  iChunkDim  = { ((dimData.x >> 30u) | ((dimData.y << 2u) & 0x03Cu)) + 1, ((dimData.y >> 4u) & 0x03Fu) + 1, ((dimData.y >> 10u) & 0x03Fu) + 1 };
   const bool  bitFlag    = (dimData.w >> 21u) & 0x01u;
   const uint2 totalCells = { (dimData.z >> 14u) + 1u, ((dimData.y >> 16u) | ((dimData.z & 0x03FFu) << 16u)) + 1u };

   const uint    uiCell      = instanceID >> 2;
   const uint    uiStrip     = (instanceID & 0x03) << 1;
   const uint4x4 index       = uint4x4(indices[0][uiCell], indices[0][uiCell + 1], indices[0][uiCell + 2], indices[0][uiCell + 3]);
   const matrix  cellDensity = matrix(cell[index[0][0]].dens, cell[index[0][1]].dens, cell[index[0][2]].dens, cell[index[0][3]].dens,
                                      cell[index[1][0]].dens, cell[index[1][1]].dens, cell[index[1][2]].dens, cell[index[1][3]].dens,
                                      cell[index[2][0]].dens, cell[index[2][1]].dens, cell[index[2][2]].dens, cell[index[2][3]].dens,
                                      cell[index[3][0]].dens, cell[index[3][1]].dens, cell[index[3][2]].dens, cell[index[3][3]].dens);
   // Exit if cell is empty or covered
   if((cellDensity[1][1] == 0.0f && cellDensity[1][2] == 0.0f && cellDensity[2][1] == 0.0f && cellDensity[2][2] == 0.0f) || cellDensity[2][2] > 1.0f) return;
   // Unpack cell constants
   const uint4  uiElement  = (cell[index[2][2]].eTypes >> shift8888) & 0x0FF;
   const uint4  uiElementW = (cell[index[2][2]].eRatios >> shift8888) & 0x0FF;
   const float  fRoughness = 0.0f; // To add...
//   const float2 fTexWarp   = ((cell[index[2][2]].dens_warp >> uint2(16, 24)) & 0x0FF) * rcp256;
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
   const float  fTCS = 16.0f / float(uiAFOC_TCS.z + 1);
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
   const float4x4 mTransform = mul(camera, projection);
   // Calculate cell position in world space
   const uint   uiCurChunk = index[2][2] / totalCells.x;
   const uint3  uiChunks   = ((dimData.w >> uint3(0, 7u, 14u)) & 0x07Fu) + 1u;
   const uint3  uiChunkOS  = uint3(uiCurChunk % uiChunks.x, (uiCurChunk / uiChunks.x) % uiChunks.y, (uiCurChunk / (uiChunks.x * uiChunks.y)) % uiChunks.z);
   const uint3  uiCellOS   = uint3(index[2][2] % asuint(iChunkDim.x), (index[2][2] / asuint(iChunkDim.x)) % asuint(iChunkDim.y), (index[2][2] / uint(iChunkDim.x * iChunkDim.y)) % asuint(iChunkDim.z));
   const float3 fStep      = float3(uiStrip, uiStrip + 1, uiStrip + 2) * 0.125f;
   const float4 fTStep     = float4(0.0f, fStep.x, 0.125f, -0.125f) * fTCS;
   const int3   iPos       = int3(uiChunkOS) * iChunkDim + int3(uiCellOS) - int3(iMapDim.xy >> 1, zso);
   const float3 fPos       = float3(iPos);
   // Calculate Texture deltas
   const float4 fTDx    = float2(fTC.zw - fTC.xy).xyxy;
   const float4 fTD     = fTDx * fTStep;
   const float2 _fTCx   = float2(uiCellOS.xy) * fTCS * fTDx.xy;
   const float2 fTCx[2] = { _fTCx, _fTCx + float2(0.0f, -fTD.w) };
   // Calculate vertex specific data
   [unroll]
   for(uint i = 0; i < 2; i++) {
      // Calculate strip vertices
      const float3x18 fVert = Displace4x4_16x8(cellDensity, fPos, i + uiStrip, 1);
//      const float3x18 fVert = WarpCoord(Displace4x4_16x8(cellDensity, fPos, i + uiStrip, 1), 1.0f);

      // First vertex
      output.pos = mul(float4(fVert[0], 1.0f), mTransform);
      output.position = fVert[0];
      output.ci = index[2][2];
      output.tex = (float2(fTCx[i].x + fTD.x + fFrameOS, fTCx[i].y + fTD.y) % fTDx.xy) + fTC.xy;
      output.rot = PackXYSinCosFromVerts(fVert[0], fVert[1], fVert[2]);
      triStream.Append(output);
      [unroll]
      for(uint j = 1; j < 15; j++) {
         // High vertex
         output.pos = mul(float4(fVert[j], 1.0f), mTransform);
         output.position = fVert[j];
         output.tex.y -= fTD.w;
         output.rot = PackXYSinCosFromVerts(fVert[j], fVert[j + 2], fVert[j + 1]);
         triStream.Append(output);
         // Low vertex
         output.pos = mul(float4(fVert[++j], 1.0f), mTransform);
         output.position = fVert[j];
         output.tex += fTD.zw;
         output.rot = PackXYSinCosFromVerts(fVert[j], fVert[j + 1], fVert[j + 2]);
         triStream.Append(output);
      }
      // Sixteenth vertex
      output.pos = mul(float4(fVert[15], 1.0f), mTransform);
      output.position = fVert[15];
      output.tex.y -= fTD.w;
      output.rot = PackXYSinCosFromVerts(fVert[15], fVert[17], fVert[16]);
      triStream.Append(output);
      // Seventeeth vertex
      output.pos = mul(float4(fVert[16], 1.0f), mTransform);
      output.position = fVert[16];
      output.tex += fTD.zw;
      triStream.Append(output);
      // Eighteenth vertex
      output.pos = mul(float4(fVert[17], 1.0f), mTransform);
      output.position = fVert[17];
      output.tex.y -= fTD.w;
      triStream.Append(output);
      triStream.RestartStrip();
   }
}
