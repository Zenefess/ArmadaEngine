/************************************************************
 * File: ps.map.cells.hlsl              Created: 2023/01/05 *
 * Type: Pixel shader             Last modified: 2023/01/13 *
 *                                                          *
 * Desc:                                                    *
 *                                                          *
 * Copyright (c) David William Bull.   All rights reserved. *
 ************************************************************/

#include "common.hlsli"
#include "ps.full register access.hlsli"

struct _LIGHT { // 32 bytes (2 vectors)
   float3 position;
   float  range;
   uint2  col_hl;   // 0-47==RGB: s7p8, 48-55==Highlight size : 0p8-1, 56-63==Highlight intensity : 4p4-1
   uint2  RES;
};

struct LIGHT { // 32 bytes (2 vectors)
   float3 position;
   float  range;
   float3 colour;
   uint   hls_hli; // 0~15==Highlight size : 1p15, 16~31==Highlight intensity : 6p10
};

cbuffer cbLight { // 8,192 bytes (512 vectors)
   const LIGHT l[256] : register(b0);
}

struct CELL_DYN { // 16 bytes (1 vector)
   uint pmc;     // Paint map colour [RGBA] : p8n0.0~1.0
   uint gtc;     // Global tint colour [RGBA] : p8n0.0~1.0
   uint gev_ems; // 0-15==Global emission value : s7p8, Emission map scalar : 7p9
   uint nrps_ai; // 0-7==Normal map scalar : p8n0.0~1.818359375, 8-15==Roughness map scalar, 16-23==Paint map scalar : p8n0.0~1.0, 24-30==Runtime index of atlas texture, 31==???
};

// Texture data
// ------------
// [0] == Diffuse map | [1] == Normal map | [2] == Occlusion, roughness, paint, and emission maps

struct GOut { // 48 bytes (3 vectors) <-- Add data for texture blending with adjacent cells?
   float4 pos      : SV_Position;
   float3 position : POSITION;
   uint   ci       : CELLINDEX;   // Cell index
   float2 tex      : TEXCOORD;
   uint2  rot      : ROTATIONS;   // sin & cos of XY rotations
};

const StructuredBuffer<CELL_DYN> cell : register(t0);

float4 main(in const GOut g) : SV_Target {
   const uint     index    = g.ci;
   const uint     atlas    = cell[index].nrps_ai >> 24;
   const float3x4 fTexSamp = Sample3of4Terrain(uint2(atlas, 1), g.tex);
   // Alpha test
   clip(fTexSamp[0].a - sqrt05f);
   // Unpack paint & global tint colours
   const float2x4 fPTC = float2x4(uint2x4(cell[index].pmc >> uint4(0, 8, 16, 24), cell[index].gtc >> uint4(0, 8, 16, 24)) & 0x0FF) * rcp255;
   // Unpack normal, roughness, paint, and emission map scalars
   const float3 fNRP    = float3((cell[index].nrps_ai >> uint3(0, 8, 16)) & 0x0FF);
   const float4 fScalar = float4(fNRP, float(cell[index].gev_ems >> 16)) * float4(0.00713082107843137254901960784314f, rcp255, rcp255, 0.001953125);
   // Unpack global emission value, ???
   const float fGEV = uint(cell[index].gev_ems & 0x0FFFF) * 0.00390625f - 128.0f;
   // Sample textures
   const float2 fTexNUV  = fTexSamp[1].xy - 0.5f;
   const float2 fTexNorm = fTexNUV.xy * fScalar.x;
   const float4 fTexMask = float4(1.0f - fTexSamp[2].rg, fTexSamp[2].ba);
   // Unpack sin & cos of rotations for normal map
   const float2x2 fSinCos = float2x2(f16tof32(g.rot & 0x0FFFF), f16tof32(g.rot >> 16));
   // Calculate normal
   const float3 fNormXYZ = normalize(float3(fTexNorm.xy, -sqrt(1.0f - (fTexNorm.x * fTexNorm.x) - (fTexNorm.y * fTexNorm.y))));
   const float2 fNormXZ  = RotateVector(fNormXYZ.xz, fSinCos._12_22);
   const float3 fNormal  = float3(fNormXZ.x, RotateVector(fNormXYZ.yz, fSinCos._11_21));
   // Calculate texel modifiers
   const float4 fPaint    = lerp(fTexSamp[0] * fPTC[1], fTexSamp[0] * fPTC[0], fTexMask.z * fScalar.z);
   const float  fEmission = (fTexMask.w * 2.0f - 1.0039215686274509803921568627451f) * fScalar.w + fGEV;
   // Calculate light
   float3 fLight = 0.0f, fHighlight = 0.0f;
   [unroll] for(uint i = 0; i < 32; i += 4) { // (i < 32): No stutter    (i < 40): Minor stuter    (i < 64): stutter    ??? Shader code exceeeding cache ??? Partially unroll
/*
      // Unpack colour variable
      const float4x3 fColour = float4x3(uint4x3(uint3(l[i].col_hl.xxy >> uint3(0, 16, 0)) & 0x0FFFF, uint3(l[i + 1].col_hl.xxy >> uint3(0, 16, 0)) & 0x0FFFF,
                                                uint3(l[i + 2].col_hl.xxy >> uint3(0, 16, 0)) & 0x0FFFF, uint3(l[i + 3].col_hl.xxy >> uint3(0, 16, 0)) & 0x0FFFF)) * rcp256 - 128.0f;
      // Unpack highlight variables
      const float2x4 fHL     = float2x4(uint2x4((l[i].col_hl.y >> uint2(16, 24)) & 0x0FF,     (l[i + 1].col_hl.y >> uint2(16, 24)) & 0x0FF,
                                                (l[i + 2].col_hl.y >> uint2(16, 24)) & 0x0FF, (l[i + 3].col_hl.y >> uint2(16, 24)) & 0x0FF)
                                      + uint2x4(257, 1, 257, 1, 257, 1, 257, 1))
                             * float2x4(rcp512, rcp16, rcp512, rcp16, rcp512, rcp16, rcp512, rcp16);
      // Process each light
      const float4   range   = float4(l[i].range, l[i + 1].range, l[i + 2].range, l[i + 3].range);
      const float4x3 vToL    = float4x3(l[i].position, l[i + 1].position, l[i + 2].position, l[i + 3].position) - float4x3(g.position, g.position, g.position, g.position);
      const float4   distToL = float4(length(vToL[0]), length(vToL[1]), length(vToL[2]), length(vToL[3]));

      const float4 range2 = range * range;
      const float4 att    = min(1.0f, float4(distToL / range + (distToL / (range2))) * 0.5f);

      const float4x3 ang = saturate(float4x3(dot(vToL[0] / distToL[0], fNormal.xyz).xxx, dot(vToL[1] / distToL[1], fNormal.xyz).xxx,
                                             dot(vToL[2] / distToL[2], fNormal.xyz).xxx, dot(vToL[3] / distToL[3], fNormal.xyz).xxx));

      const float4x3 lum  = (1.0f - float4x3(att[0].xxx, att[1].xxx, att[2].xxx, att[3].xxx)) * ang;
      const float4x3 occ  = (1.0f - ang) * float4x3(fTexMask.xxx, fTexMask.xxx, fTexMask.xxx, fTexMask.xxx) * lum;
      const float4x3 l4x3 = max(0.0f, (lum - occ) * fColour);
      
      fLight     += l4x3[0] + l4x3[1] + l4x3[2] + l4x3[3];
      fHighlight += max(0.0f, (fTexMask.y - (1.0f - fHL[0].x)) * lum[0] * fColour[0] * fHL[0].y) + max(0.0f, (fTexMask.y - (1.0f - fHL[0].z)) * lum[1] * fColour[1] * fHL[0].w) +
                    max(0.0f, (fTexMask.y - (1.0f - fHL[1].x)) * lum[2] * fColour[2] * fHL[1].y) + max(0.0f, (fTexMask.y - (1.0f - fHL[1].z)) * lum[3] * fColour[3] * fHL[1].w);
*/
      const uint3 lOS = i + uint3(1u, 2u, 3u);
      // Unpack highlight variables
      const float2x4 fHL     = float2x4(uint2x4((l[i].hls_hli >> uint2(0, 16u)) & 0x0FFFFu,     (l[lOS.x].hls_hli >> uint2(0, 16u)) & 0x0FFFFu,
                                                (l[lOS.y].hls_hli >> uint2(0, 16u)) & 0x0FFFFu, (l[lOS.z].hls_hli >> uint2(0, 16u)) & 0x0FFFFu)
                                      + uint2x4(32768u, 0, 32768u, 0, 32768u, 0, 32768u, 0))
                             * float2x4(rcp65536, rcp4096, rcp65536, rcp4096, rcp65536, rcp4096, rcp65536, rcp4096);
      // Process each light
      const float4   range   = float4(l[i].range, l[lOS.x].range, l[lOS.y].range, l[lOS.z].range);
      const float4x3 vToL    = float4x3(l[i].position, l[lOS.x].position, l[lOS.y].position, l[lOS.z].position) - float4x3(g.position, g.position, g.position, g.position);
      const float4   distToL = float4(length(vToL[0]), length(vToL[1]), length(vToL[2]), length(vToL[3]));

      const float4 range2 = range * range;
      const float4 att    = min(1.0f, float4(distToL / range + (distToL / (range2))) * 0.5f);

      const float4x3 ang = saturate(float4x3(dot(vToL[0] / distToL[0], fNormal.xyz).xxx, dot(vToL[1] / distToL[1], fNormal.xyz).xxx,
                                             dot(vToL[2] / distToL[2], fNormal.xyz).xxx, dot(vToL[3] / distToL[3], fNormal.xyz).xxx));

      const float4x3 lum  = (1.0f - float4x3(att[0].xxx, att[1].xxx, att[2].xxx, att[3].xxx)) * ang;
      const float4x3 occ  = (1.0f - ang) * float4x3(fTexMask.xxx, fTexMask.xxx, fTexMask.xxx, fTexMask.xxx) * lum;
      const float4x3 l4x3 = max(0.0f, (lum - occ) * float4x3(l[i].colour, l[lOS.x].colour, l[lOS.y].colour, l[lOS.z].colour));
      
      fLight     += l4x3[0] + l4x3[1] + l4x3[2] + l4x3[3];
      fHighlight += max(0.0f, (fTexMask.y - (1.0f - fHL[0].x)) * lum[0] * l[i].colour * fHL[0].y) + max(0.0f, (fTexMask.y - (1.0f - fHL[0].z)) * lum[1] * l[lOS.x].colour * fHL[0].w) +
                    max(0.0f, (fTexMask.y - (1.0f - fHL[1].x)) * lum[2] * l[lOS.y].colour * fHL[1].y) + max(0.0f, (fTexMask.y - (1.0f - fHL[1].z)) * lum[3] * l[lOS.z].colour * fHL[1].w);
   }

   return fPaint * float4(fLight + fEmission + (fHighlight * fScalar.y), 1.0f);
}
