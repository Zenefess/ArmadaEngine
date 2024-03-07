/************************************************************
 * File: ps.16lights.hlsl               Created: 2022/11/21 *
 *                                Last modified: 2022/12/07 *
 *                                                          *
 * Desc:                                                    *
 *                                                          *
 *  Copyright (c) David William Bull. All rights reserved.  *
 ************************************************************/

struct LIGHT {
	float3 lightPos;
	float  lightRange;
	float3 lightCol;
	uint   phong;		// 0-15==Highlight size (6p10), 16=31==Highlight intensity (6p10)
};

cbuffer cbLight {	// 512 bytes
	LIGHT l[16] : register(b0);
}

struct SPRITE_DPS {	// 16 bytes
	uint pmc;	// Paint map colour
	uint dmc;	// Occlusion map colour
	uint n_g;	// 0-15==Normal map scale (6p10), 16-31==Global emission value (6p10)
	uint oas;	// 0-15==Occlusion map emission additive (8p8), 16=31==Occlusion map scalar (1p15)
};

StructuredBuffer<SPRITE_DPS> sprite : register(t0);

// [0] == Diffuse map | [1] == Normal map | [2] == Paint & emission maps | [3] == Highlight & occlusion maps
Texture2D Texture[4] : register(t1);

SamplerState Sampler : register(s0);

struct GOut {	// 13 scalars
	float4 pos : SV_Position;
	float3 position : POSITION;
	float2 tex : TEXCOORD;
	uint   i : BLENDINDICES0;		// Sprite index
	uint3  rot : BLENDINCICES1;	// sin & cos of XYZ rotations
};

inline float2 RotateVector(float2 v, float2 dir) { return float2(dot(v, float2(dir.x, -dir.y)), dot(v, dir.yx)); }

float4 main(in GOut g) : SV_Target {
	const float4 fTexSamp = Texture[0].Sample(Sampler, g.tex);
	// Alpha test
	clip(fTexSamp.a - 0.70710678118654752440084436210485f);
	// Unpack paint & occlusion map colours
	const float2x4 fPDC = float2x4((uint2x4(sprite[g.i].pmc >> uint4(0, 8, 16, 24), sprite[g.i].dmc >> uint4(0, 8, 16, 24)) & 0x0FF) + 1) * 0.00390625f;
	// Unpack normal map scale, global emission value, and occlusion additive & scalar
	const float4 fMOD = float4(uint4(sprite[g.i].n_g, sprite[g.i].n_g >> 16, sprite[g.i].oas, sprite[g.i].oas >> 16) & 0x0FFFF) * 0.0009765625f;
	// Sample textures
	const float4 fTexel   = float4(pow(fTexSamp.rgb, 2.2f), fTexSamp.a);
	const float2 fTexNorm = (Texture[1].Sample(Sampler, g.tex).xy - 0.504f) * fMOD.x;
	const float4 fTexMask = float4(Texture[2].Sample(Sampler, g.tex).xy, Texture[3].Sample(Sampler, g.tex).xy);
	// Unpack sin & cos of rotations for normal map
	const float2x3 fSinCos = float2x3(f16tof32(g.rot & 0x0FFFF), f16tof32(g.rot >> 16));
	// Calculate normal
	const float2 fNormXY  = RotateVector(fTexNorm, fSinCos._13_23.xy);
	const float3 fNormXYZ = normalize(float3(fNormXY.xy, -sqrt(1.0f - (fNormXY.x * fNormXY.x) - (fNormXY.y * fNormXY.y))));
	const float2 fNormXZ  = RotateVector(fNormXYZ.xz, fSinCos._12_22.xy);
	const float3 fNormal  = float3(fNormXZ.x, RotateVector(fNormXYZ.yz, fSinCos._11_21.xy));
	// Calculate texel modifiers
	const float  fTexOcc    = fTexMask.w * fMOD.w;
	const float4 fPaint     = lerp(fTexel, fTexel * fPDC[0], fTexMask.x);
	const float  fEmission  = ((fTexMask.y - 0.504f) * 1.985f) + fMOD.y;
	const float  fOccEmiss  = fTexOcc * fMOD.z;
	const float4 fOcclusion = lerp(fPaint, fTexel * fPDC[1], fTexOcc);
	// Calculate light
	float3 fLight = 0.0f, fHighlight = 0.0f;
	[unroll]
	for(uint i = 0; i < 16; i++) {
		// Unpack highlight variables
		const float2 fHL     = float2(uint2(l[i].phong >> 16, l[i].phong) & 0x0FFFF) * float2(0.0009765625f, 0.0009765625f);
		// Process each light
		const float  range   = l[i].lightRange;
		const float3 vToL    = l[i].lightPos.xyz - g.position;
		const float  distToL = length(vToL);
		const float  att     = min(1.0f, ((distToL / range + (distToL / (range * range)))) * 0.5f);
		const float3 lum     = saturate(dot(vToL / distToL, fNormal)) * (1.0f - att) * l[i].lightCol.rgb;
		fLight     += lum;
		fHighlight += pow(max(0.0f, fTexMask.z * fHL.x * lum - (max(0.0f, (1.0f - fHL.y) * fHL.x))), 2.0f);
	}

	return fOcclusion * float4(fLight + fEmission + fOccEmiss + fHighlight * pow(1.0f - fTexOcc, 2.0f), 1.0f);
}
