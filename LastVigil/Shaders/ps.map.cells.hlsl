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

struct LIGHT { // 32 bytes (2 vectors)
   float3 position;
   float  range;
   uint2  col_hl;   // 0-47==RGB: s7p8, 48-55==Highlight size : 0p8-1, 56-63==Highlight intensity : 4p4-1
   uint2  RES;
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

StructuredBuffer<CELL_DYN> cell : register(t0);

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

float4 main(in const GOut g) : SV_Target {
   cuint     index    = g.ci;
   cuint     atlas    = cell[index].nrps_ai >> 24;
   cfloat3x4 fTexSamp = Sample3of4Terrain(uint2(atlas, 1), g.tex);
   // Alpha test
   clip(fTexSamp[0].a - sqrt05f);
   // Unpack paint & global tint colours
   cfloat2x4 fPTC = float2x4(uint2x4(cell[index].pmc >> uint4(0, 8, 16, 24), cell[index].gtc >> uint4(0, 8, 16, 24)) & 0x0FF) * rcp255;
   // Unpack normal, roughness, paint, and emission map scalars
   cfloat3 fNRP    = float3((cell[index].nrps_ai >> uint3(0, 8, 16)) & 0x0FF);
   cfloat4 fScalar = float4(fNRP, float(cell[index].gev_ems >> 16)) * float4(0.00713082107843137254901960784314f, rcp255, rcp255, 0.001953125);
   // Unpack global emission value, ???
   cfloat fGEV = uint(cell[index].gev_ems & 0x0FFFF) * 0.00390625f - 128.0f;
   // Sample textures
   cfloat2 fTexNUV  = fTexSamp[1].xy - 0.5f;
   cfloat2 fTexNorm = fTexNUV.xy * fScalar.x;
   cfloat4 fTexORPE = fTexSamp[2];
   cfloat4 fTexMask = float4(1.0f - fTexORPE.rg, fTexORPE.ba);
   // Unpack sin & cos of rotations for normal map
   cfloat2x2 fSinCos = float2x2(f16tof32(g.rot & 0x0FFFF), f16tof32(g.rot >> 16));
   // Calculate normal
   cfloat2 fNormXY  = fTexNorm;   // RotateVector(fTexNorm, fSinCos._13_23);
   cfloat3 fNormXYZ = normalize(float3(fNormXY.xy, -sqrt(1.0f - (fNormXY.x * fNormXY.x) - (fNormXY.y * fNormXY.y))));
   cfloat2 fNormXZ  = RotateVector(fNormXYZ.xz, fSinCos._12_22);
   cfloat3 fNormal  = float3(fNormXZ.x, RotateVector(fNormXYZ.yz, fSinCos._11_21));
   // Calculate texel modifiers
   cfloat4 fPaint    = lerp(fTexSamp[0] * fPTC[1], fTexSamp[0] * fPTC[0], fTexMask.z * fScalar.z);
   cfloat  fEmission = (fTexMask.w * 2.0f - 1.0039215686274509803921568627451f) * fScalar.w + fGEV;
   // Calculate light
   float3 fLight = 0.0f, fHighlight = 0.0f;
   [unroll]
/*   for(uint i = 0; i < 32; i++) { // (i < 48): No stutter    (i < 64): Minor stuter    (i < 128): stutter    ??? Shader code exceeeding cache ??? Partially unroll
      // Unpack colour variable
      cfloat3 fColour = float3(uint3(l[i].col_hl.xxy >> uint3(0, 16, 0)) & 0x0FFFF) * rcp256 - 128.0f;
      // Unpack highlight variables
      cfloat2 fHL     = float2(uint2((l[i].col_hl.y >> uint2(16, 24)) & 0x0FF) + uint2(257, 1)) * float2(rcp512, rcp16);
      // Process each light
      cfloat  range   = l[i].range;
      cfloat3 vToL    = l[i].position - g.position;
      cfloat  distToL = length(vToL);
      cfloat  att     = min(1.0f, (distToL / range + (distToL / (range * range))) * 0.5f);
      cfloat3 ang     = saturate(dot(vToL / distToL, fNormal));
      cfloat3 lum     = (1.0f - att) * ang;
      cfloat3 occ     = (1.0f - ang) * fTexMask.x * lum;
      fLight     += max(0.0f, (lum - occ) * fColour);
      fHighlight += max(0.0f, (fTexMask.y - (1.0f - fHL.x)) * lum * fColour * fHL.y);*/
   for(uint i = 0; i < 48; i += 4) { // (i < 48): No stutter    (i < 64): Minor stuter    (i < 128): stutter    ??? Shader code exceeeding cache ??? Partially unroll
      // Unpack colour variable
      cfloat4x3 fColour = float4x3(uint4x3(uint3(l[i].col_hl.xxy >> uint3(0, 16, 0)) & 0x0FFFF, uint3(l[i + 1].col_hl.xxy >> uint3(0, 16, 0)) & 0x0FFFF,
                                           uint3(l[i + 2].col_hl.xxy >> uint3(0, 16, 0)) & 0x0FFFF, uint3(l[i + 3].col_hl.xxy >> uint3(0, 16, 0)) & 0x0FFFF)) * rcp256 - 128.0f;
      // Unpack highlight variables
      cfloat2x4 fHL     = float2x4(uint2x4((l[i].col_hl.y >> uint2(16, 24)) & 0x0FF,     (l[i + 1].col_hl.y >> uint2(16, 24)) & 0x0FF,
                                           (l[i + 2].col_hl.y >> uint2(16, 24)) & 0x0FF, (l[i + 3].col_hl.y >> uint2(16, 24)) & 0x0FF)
                                 + uint2x4(257, 1, 257, 1, 257, 1, 257, 1))
                        * float2x4(rcp512, rcp16, rcp512, rcp16, rcp512, rcp16, rcp512, rcp16);
      // Process each light
      cfloat4   range   = float4(l[i].range, l[i + 1].range, l[i + 2].range, l[i + 3].range);
      cfloat4x3 vToL    = float4x3(l[i].position, l[i + 1].position, l[i + 2].position, l[i + 3].position) - float4x3(g.position, g.position, g.position, g.position);
      cfloat4   distToL = float4(length(vToL[0]), length(vToL[1]), length(vToL[2]), length(vToL[3]));

      cfloat4 range2 = range * range;
      cfloat4 att    = min(1.0f, float4(distToL / range + (distToL / (range2))) * 0.5f);

      cfloat4x3 ang = saturate(float4x3(dot(vToL[0] / distToL[0], fNormal.xyz).xxx, dot(vToL[1] / distToL[1], fNormal.xyz).xxx,
                                        dot(vToL[2] / distToL[2], fNormal.xyz).xxx, dot(vToL[3] / distToL[3], fNormal.xyz).xxx));

      cfloat4x3 lum  = (1.0f - float4x3(att[0].xxx, att[1].xxx, att[2].xxx, att[3].xxx)) * ang;
      cfloat4x3 occ  = (1.0f - ang) * fTexMask.x * lum;
      cfloat4x3 l4x3 = max(0.0f, (lum - occ) * fColour);
      
      fLight     += l4x3[0] + l4x3[1] + l4x3[2] + l4x3[3];
      fHighlight += max(0.0f, (fTexMask.y - (1.0f - fHL[0].x)) * lum[0] * fColour[0] * fHL[0].y) + max(0.0f, (fTexMask.y - (1.0f - fHL[0].z)) * lum[1] * fColour[1] * fHL[0].w) +
                    max(0.0f, (fTexMask.y - (1.0f - fHL[1].x)) * lum[2] * fColour[2] * fHL[1].y) + max(0.0f, (fTexMask.y - (1.0f - fHL[1].z)) * lum[3] * fColour[3] * fHL[1].w);
   }

   return fPaint * float4(fLight + fEmission + (fHighlight * fScalar.y), 1.0f);
}
