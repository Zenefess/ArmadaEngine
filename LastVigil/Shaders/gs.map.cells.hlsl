/************************************************************
 * File: gs.map.cells.hlsl              Created: 2024/06/08 *
 * Type: Geometry shader          Last modified: 2024/06/11 *
 *                                                          *
 * Notes:                                                   *
 *                                                          *
 * Copyright (c) David William Bull.   All rights reserved. *
 ************************************************************/

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

#include "common.hlsli"
#include "gs.mesh displacement functions.hlsli"

cbuffer CB_VIEW : register(b1) { // 160 bytes (10 vectors)
   const matrix perspective;  // Perspective transformation
   const matrix orthographic; // Orthographic transformation
   const float2 guiScale;     // Final X & Y scaling factors for GUI
   const uint2  bitField;     // 0-63==???
   const uint4  RES;
};

cbuffer CB_MAIN : register(b2) { // 96 bytes (6 vectors)
   const matrix camera;     // Camera transformation
   const uint   frameCount; // Total number of frames presented
   const float  frameTime;  // Duration of last frame
   const float  secsDelta;  // == (Time elapsed - SecsTotal)
   const uint   st_bf;      // 0-23==secsTotal (Elapsed seconds), 24==Draw transparent sprites, 25-31==???
   const uint4  RES2;
};
/*
cbuffer MAPDIMS_ICB : register(b2) { // 32 bytes (2 vectors)
   // Map dimensions: X, Y, Z cell counts
   // Map chunks: X, Y, Z chunk counts - 1        --- 21 bits -- [21][..][..][..]
   // zso: Z spawning offset                      --- 11 bits -- [32][..][..][..]
   // Chunk dimensions: X, Y, Z cell counts
   // Out-of-bounds density
   const uint3 mapDim;
   const uint  mapChunk_zso;
   const uint3 chunkDim;
   const uint  oobDens;
}
*/
cbuffer MAPDIMS_ICB : register(b3) { // 32 bytes (2 vectors)
   // Map dimensions: X, Y, Z cell counts - 1     --- 30 bits -- [..][..][..][30]
   // Chunk dimensions: X, Y, Z cell counts - 1   --- 18 bits -- [..][..][16][32]
   // Map Cells: Total cells per map - 1          --- 30 bits -- [..][14][32][..] --- No longer used
   // Chunk Cells: Total cells per chunk - 1      --- 18 bits -- [..][32][..][..] --- No longer used
   // Map chunks: X, Y, Z chunk counts - 1        --- 21 bits -- [21][..][..][..]
   // flag                                        ---  1 bits -- [22][..][..][..] --- No longer used
   // zso: Z spawning offset - 1                  --- 10 bits -- [32][..][..][..]
   const uint4    dimData;
   const CELL_DYN oob; // Out-of-bounds cell data
}

const StructuredBuffer<ELEMENT_IMM> element   : register(t0);
const StructuredBuffer<uint>        cellIndex : register(t1);
const StructuredBuffer<CELL_DYN>    cell      : register(t2);

// Input == Relative cell index
[instance(32)] [maxvertexcount(6)]
void main(in point const uint relIndex[1] : CELL, const uint uiStrip : SV_GSInstanceID, inout TriangleStream<GOut> triStream) {
   const uint3 mapDim   = ((dimData.xxx >> uint3(0, 10u, 20u)) & 0x03FFu.xxx) + 1u.xxx; // dimData.MapDimensions: X, Y, Z cell counts
   const uint  absIndex = cellIndex[relIndex[0]];
   // Calculate cell position in world space
   const int3  relCoord = asint((relIndex[0].xxx >> uint3(0, 10u, 20u)) & 0x03FFu.xxx);

   // Calculate 4x4 matrix of indices; test for out-of-bounds
   const uint mapDimXY = mapDim.x * mapDim.y;
   matrix cellDensity;
   [unroll] for(int2 step = 0; step.y < 4; ++step.y)
      [unroll] for(step.x = 0; step.x < 4; ++step.x) {
         const int2 adjIndex = relCoord.xy + step - 2;
         cellDensity[step.x][step.y] = ((adjIndex.x >= 0) || (adjIndex.x < asint(mapDim.x)) || (adjIndex.y >= 0) || (adjIndex.y < asint(mapDim.y))
                                       ? cell[cellIndex[(asuint(relCoord.z) * mapDimXY) + (asuint(adjIndex.y) * mapDim.x) + asuint(adjIndex.x)]].dens
                                       : oob.dens);
      }

   // Exit if cell is empty or covered
   if((cellDensity[1][1] <= 0.0f && cellDensity[1][2] <= 0.0f && cellDensity[2][1] <= 0.0f && cellDensity[2][2] <= 0.0f) || cellDensity[2][2] > 1.0f) return;
   // Unpack cell constants
   const uint4  uiElement  = (cell[absIndex].eTypes >> shift8888) & 0x0FF;
   const uint4  uiElementW = (cell[absIndex].eRatios >> shift8888) & 0x0FF;
   const float  fRoughness = 0.0f; // To add...
//   const float2 fTexWarp   = ((cell[index[2][2]].dens_warp >> uint2(16, 24)) & 0x0FF) * rcp256;
   // Unpack element transparencies
   const float4 fET = float4((uint4(element[uiElement.x].aft_et, element[uiElement.y].aft_et, element[uiElement.z].aft_et, element[uiElement.w].aft_et) >> 24) & 0x0FF) * 0.00390625f;
   // Sort element indices by weights
   uint4 uiIndexW = { 0, 1u, 2u, 3u };
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
      if(uiIndex.x == 4) return; // Exit if no transparent layer
      if(fET[uiIndex.x] >= 0.995f) return; // Exit if all layers are 99.5%~ transparent
      uiElIn = uiElement[uiIndex.x];
      fElementD = 1.0f;
   } else {
      if(uiIndex.y == 4) return; // Exit if no opaque layer
      uiElIn = uiElement[uiIndex.y];
   }
   // Unpack animation constants and .tc scalar
   const uint4 uiAFOC_TCS = (uint4(element[uiElIn].afoc_tcs_ai, element[uiElIn].afoc_tcs_ai, element[uiElIn].afoc_tcs_ai, element[uiElIn].afoc_tcs_ai) >> uint4(0, 8, 16, 24)) & 0x0FF;
   const float fAFO       = float(uiAFOC_TCS.x) * 0.00390625f;
   const float fAFC       = float(uiAFOC_TCS.y + 1) * 0.00390625f;
   const float fFT        = float(element[uiElIn].aft_et & 0x0FF) * 0.00390625f + float((element[uiElIn].aft_et >> 8) & 0x0FFFF);
   // Unpack element texture scalar & coordinates
   const float  fTCS  = 16.0f / float(uiAFOC_TCS.z + 1);
   const float4 fTC   = float4((element[uiElIn].tc[0] >> uint2(0, 16)) & 0x0FFFF, (element[uiElIn].tc[1] >> uint2(0, 16)) & 0x0FFFF) * 0.000030517578125f;
   const float2 fStep = float2(float((uiStrip >> 0x03u) & 0x03u) * 0.25f, float(uiStrip & 0x07u) * 0.125f);
   // Calculate animation frame offset
   //const float fFrameOS = (fTC.z - fTC.x) * trunc(((totalSecs / fFT) + fAFO) % fAFC);
   const uint   secsTotal = st_bf & 0x0FFFFFF;
   const float  fFTDelta  = float(Seconds(secsTotal, secsDelta) * double(rcp(fFT)));
   const float  fFrameOS  = (fTC.z - fTC.x) * trunc((fFTDelta + fAFO) % fAFC);
   const float4 fTStep    = float4(fStep, 0.125f, -0.125f) * fTCS;
   //
   // Generate triangle stream
   //
   GOut output;
   const float4x4 mTransform = mul(camera, perspective);
   // Calculate Texture deltas
   const float4 fTDx = float2(fTC.zw - fTC.xy).xyxy;
   const float4 fTD  = fTDx * fTStep;
   const float2 fTCx = float2(relCoord.xy) * fTCS * fTDx.xy;
   // Calculate mesh vertices
   const float3x6 fVert = Displace4x4_16x8(cellDensity, float3(relCoord - int3(asint(mapDim.xy >> 1u.xx), asint(dimData.w >> 22u) + 1u)), fStep, 1); // dimData.zSpawningOffset
   // Calculate vertex specific data
   output.pos = mul(float4(fVert[0], 1.0f), mTransform);
   output.position = fVert[0];
   output.ci = absIndex;
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
