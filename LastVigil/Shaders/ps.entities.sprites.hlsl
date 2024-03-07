/************************************************************
 * File: ps.instanced.sprites.hlsl      Created: 2022/11/21 *
 *                                Last modified: 2023/01/13 *
 *                                                          *
 * Desc:                                                    *
 *                                                          *
 *  Copyright (c) David William Bull. All rights reserved.  *
 ************************************************************/

#include "common.hlsli"
#include "ps.full register access.hlsli"

struct LIGHT {
	float3 position;
	float  range;
	uint2  col_hl;	// 0-47==RGB (s7p8), 48-63==highlight: Size (0p8-1), intensity (4p4-1)
	uint2  RES;
};

cbuffer cbLight {	// 512 bytes
	const LIGHT l[256] : register(b0);
}

struct SPRITE_DYN {	// 16 bytes (1 vector)
	uint pmc;		// Paint map colour [RGBA] : p8n0.0~1.0
	uint gtc;		// Global tint colour [RGBA] : p8n0.0~1.0
	uint gev_ems;	// 0-15==Global emission value : s7p8, Emission map scalar : 7p9
	uint nrps;		// 0-7==Normal map scalar : p8n0.0~1.818359375, 8-15==Roughness map scalar, 16-23==Paint map scalar : p8n0.0~1.0, 24-31==???
};

StructuredBuffer<SPRITE_DYN> sprite : register(t0);

// Texture data
// ------------
// [0] == Diffuse map | [1] == Normal map | [2] == Occlusion, roughness, paint, and emission maps

struct GOut {	// 56 bytes (3 vectors + 2 scalars)
	float4 pos      : SV_Position;
	float3 position : POSITION;
	uint   si       : SPRITEINDEX;
	uint3  rot      : ROTATIONS;   // sin & cos of XYZ rotations
	uint   ai       : ATLASINDEX;  // 0~6==Atlas index
	float2 tex      : TEXCOORD;
};

float4 main(in const GOut g) : SV_Target {
	const uint     index    = g.si;
	const uint     atlas    = g.ai;
	const float3x4 fTexSamp = Sample3of4SpriteLinear(atlas, g.tex);
	// Alpha test
	clip(fTexSamp[0].a - sqrt05f);
	// Unpack paint & global tint colours
	const float2x4 fPTC = float2x4(uint2x4(sprite[index].pmc >> uint4(0, 8, 16, 24), sprite[index].gtc >> uint4(0, 8, 16, 24)) & 0x0FF) * rcp255;
	// Unpack normal, roughness, paint, and emission map scalars
	const float3 fNRP    = float3((sprite[index].nrps >> uint3(0, 8, 16)) & 0x0FF);
	const float4 fScalar = float4(fNRP, float(sprite[index].gev_ems >> 16)) * float4(0.00713082107843137254901960784314f, rcp255, rcp255, 0.00194552529182879377431906614786f);
	// Unpack global emission value, ???
	const float fGEV = uint(sprite[index].gev_ems & 0x0FFFF) * 0.00389105058365758754863813229572f - 127.0f;
	// Sample textures
	const float2 fTexNUV  = fTexSamp[1].xy - 0.5f;
	const float2 fTexNorm = fTexNUV.xy * fScalar.x;
	const float4 fTexORPE = fTexSamp[2];
	const float4 fTexMask = float4(1.0f - fTexORPE.rg, fTexORPE.ba);
	// Unpack sin & cos of rotations for normal map
	const float2x3 fSinCos = float2x3(f16tof32(g.rot & 0x0FFFF), f16tof32(g.rot >> 16));
	// Calculate normal
	const float2 fNormXY  = RotateVector(fTexNorm, fSinCos._13_23.xy);
	const float3 fNormXYZ = normalize(float3(fNormXY.xy, -sqrt(1.0f - (fNormXY.x * fNormXY.x) - (fNormXY.y * fNormXY.y))));
	const float2 fNormXZ  = RotateVector(fNormXYZ.xz, fSinCos._12_22.xy);
	const float3 fNormal  = float3(fNormXZ.x, RotateVector(fNormXYZ.yz, fSinCos._11_21.xy));
	// Calculate texel modifiers
	const float4 fPaint    = lerp(fTexSamp[0] * fPTC[1], fTexSamp[0] * fPTC[0], fTexMask.z * fScalar.z);
	const float  fEmission = (fTexMask.w * 2.0f - 1.0039215686274509803921568627451f) * fScalar.w + fGEV;
	// Calculate light
	float3 fLight = 0.0f, fHighlight = 0.0f;
	[unroll]
	for(uint i = 0; i < 56; i++) {
		// Unpack colour variable
		const float3 fColour = float3(uint3(l[i].col_hl.xxy >> uint3(0, 16, 0)) & 0x0FFFF) * rcp256 - 128.0f;
		// Unpack highlight variables
		const float2 fHL     = float2(uint2((l[i].col_hl.y >> uint2(16, 24)) & 0x0FF) + uint2(257, 1)) * float2(rcp512, rcp16);
		// Process each light
		const float  range   = l[i].range;
		const float3 vToL    = l[i].position - g.position;
		const float  distToL = length(vToL);
		const float  att     = min(1.0f, (distToL / range + (distToL / (range * range))) * 0.5f);
		const float3 ang     = saturate(dot(vToL / distToL, fNormal));
		const float3 lum     = (1.0f - att) * ang;
		const float3 occ     = (1.0f - ang) * fTexMask.x * lum;
		fLight     += max(0.0f, (lum - occ) * fColour);
		fHighlight += max(0.0f, (fTexMask.y - (1.0f - fHL.x)) * lum * fColour * fHL.y);
	}
	const float4 fRet = fPaint * float4(fLight + fEmission + (fHighlight * fScalar.y), 1.0f);
	return fRet;
}
