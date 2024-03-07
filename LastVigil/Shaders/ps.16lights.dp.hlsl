/************************************************************
 * File: ps.16lights.hlsl               Created: 2022/10/27 *
 *                                Last modified: 2022/10/29 *
 *                                                          *
 * Desc:                                                    *
 *                                                          *
 *  Copyright (c) David William Bull. All rights reserved.  *
 ************************************************************/

Texture2D Texture[4] : register(t0);

SamplerState Sampler : register(s0);

struct LIGHT {
	 float4 lightPos; // .w == range
	 float4 lightCol; // .a == flags
};

cbuffer cbLight {
	 LIGHT l[16] : register(b0); // 512 bytes
}

struct GOut {
	float4 pos : SV_Position;
	float3 position : POSITION;
	float3 n : NORMAL;
	float2 tex : TEXCOORD;
	uint   pID : SV_PrimitiveID;
};

static const float3 ambient = { 0.0f, 0.0f, 0.0f };

// (1.0`) 207fps ~ 70% @ 2,040Mhz -- Alpha testing. Unrolled loop.  -- (0.0`) 60fps ~ 100%
// (1.0`) 200fps ~ 49% @ 2,040Mhz -- Unrolled loop.                 -- (0.0`) 51fps ~ 100%
// (1.0`) 200fps ~ 77% @ 2,040Mhz -- Alpha testing                  -- (0.0`) 43fps ~ 100%
// (1.0`) 200fps ~ 51% @ 2,040Mhz                                   -- (0.0`) 34fps ~ 100%

inline float3 ToneMapTexture(float3 texel, float gamma) { return pow(max(0.0f, texel), gamma); };

inline float3 ToneMapResult(float3 colour) {	return colour / max(max(colour.r, colour.g), colour.b); };

float4 main(in GOut g) : SV_Target {
	float4 Texel = Texture[0].Sample(Sampler, g.tex);

//    clip(Texel.a - 0.70710678118654752440084436210485f);
//    clip(Texel.a - 0.25f);
	 clip(Texel.a - 0.01f);

//	return Texel;
//}
	 
	float3 result = { 0.0f, 0.0f, 0.0f };

	float3 lCol = l[0].lightCol.rgb;
	float range = l[0].lightPos.w;
	float3 vToL = l[0].lightPos.xyz - g.position;
	float distToL = length(vToL);
	float att = min(1.0f, (distToL / range + distToL / (range * range)) * 0.5f);
	float3 lum = saturate(dot(vToL / distToL, g.n)) * lCol;
	result += lum * (1.0f - att);

	lCol = l[1].lightCol.rgb;
	range = l[1].lightPos.w;
	vToL = l[1].lightPos.xyz - g.position;
	distToL = length(vToL);
	att = min(1.0f, (distToL / range + distToL / (range * range)) * 0.5f);
	lum = saturate(dot(vToL / distToL, g.n)) * lCol;
	result += lum * (1.0f - att);

	lCol = l[2].lightCol.rgb;
	range = l[2].lightPos.w;
	vToL = l[2].lightPos.xyz - g.position;
	distToL = length(vToL);
	att = min(1.0f, (distToL / range + distToL / (range * range)) * 0.5f);
	lum = saturate(dot(vToL / distToL, g.n)) * lCol;
	result += lum * (1.0f - att);

	lCol = l[3].lightCol.rgb;
	range = l[3].lightPos.w;
	vToL = l[3].lightPos.xyz - g.position;
	distToL = length(vToL);
	att = min(1.0f, (distToL / range + distToL / (range * range)) * 0.5f);
	lum = saturate(dot(vToL / distToL, g.n)) * lCol;
	result += lum * (1.0f - att);

	lCol = l[4].lightCol.rgb;
	range = l[4].lightPos.w;
	vToL = l[4].lightPos.xyz - g.position;
	distToL = length(vToL);
	att = min(1.0f, (distToL / range + distToL / (range * range)) * 0.5f);
	lum = saturate(dot(vToL / distToL, g.n)) * lCol;
	result += lum * (1.0f - att);

	lCol = l[5].lightCol.rgb;
	range = l[5].lightPos.w;
	vToL = l[5].lightPos.xyz - g.position;
	distToL = length(vToL);
	att = min(1.0f, (distToL / range + distToL / (range * range)) * 0.5f);
	lum = saturate(dot(vToL / distToL, g.n)) * lCol;
	result += lum * (1.0f - att);

	lCol = l[6].lightCol.rgb;
	range = l[6].lightPos.w;
	vToL = l[6].lightPos.xyz - g.position;
	distToL = length(vToL);
	att = min(1.0f, (distToL / range + distToL / (range * range)) * 0.5f);
	lum = saturate(dot(vToL / distToL, g.n)) * lCol;
	result += lum * (1.0f - att);

	lCol = l[7].lightCol.rgb;
	range = l[7].lightPos.w;
	vToL = l[7].lightPos.xyz - g.position;
	distToL = length(vToL);
	att = min(1.0f, (distToL / range + distToL / (range * range)) * 0.5f);
	lum = saturate(dot(vToL / distToL, g.n)) * lCol;
	result += lum * (1.0f - att);

	 lCol = l[8].lightCol.rgb;
	 range = l[8].lightPos.w;
	 vToL = l[8].lightPos.xyz - g.position;
	 distToL = length(vToL);
	 att = min(1.0f, (distToL / range + distToL / (range * range)) * 0.5f);
	 lum = saturate(dot(vToL / distToL, g.n)) * lCol;
	 result += lum * (1.0f - att);

	 lCol = l[9].lightCol.rgb;
	 range = l[9].lightPos.w;
	 vToL = l[9].lightPos.xyz - g.position;
	 distToL = length(vToL);
	 att = min(1.0f, (distToL / range + distToL / (range * range)) * 0.5f);
	 lum = saturate(dot(vToL / distToL, g.n)) * lCol;
	 result += lum * (1.0f - att);

	 lCol = l[10].lightCol.rgb;
	 range = l[10].lightPos.w;
	 vToL = l[10].lightPos.xyz - g.position;
	 distToL = length(vToL);
	 att = min(1.0f, (distToL / range + distToL / (range * range)) * 0.5f);
	 lum = saturate(dot(vToL / distToL, g.n)) * lCol;
	 result += lum * (1.0f - att);

	 lCol = l[11].lightCol.rgb;
	 range = l[11].lightPos.w;
	 vToL = l[11].lightPos.xyz - g.position;
	 distToL = length(vToL);
	 att = min(1.0f, (distToL / range + distToL / (range * range)) * 0.5f);
	 lum = saturate(dot(vToL / distToL, g.n)) * lCol;
	 result += lum * (1.0f - att);

	 lCol = l[12].lightCol.rgb;
	 range = l[12].lightPos.w;
	 vToL = l[12].lightPos.xyz - g.position;
	 distToL = length(vToL);
	 att = min(1.0f, (distToL / range + distToL / (range * range)) * 0.5f);
	 lum = saturate(dot(vToL / distToL, g.n)) * lCol;
	 result += lum * (1.0f - att);

	 lCol = l[13].lightCol.rgb;
	 range = l[13].lightPos.w;
	 vToL = l[13].lightPos.xyz - g.position;
	 distToL = length(vToL);
	 att = min(1.0f, (distToL / range + distToL / (range * range)) * 0.5f);
	 lum = saturate(dot(vToL / distToL, g.n)) * lCol;
	 result += lum * (1.0f - att);

	 lCol = l[14].lightCol.rgb;
	 range = l[14].lightPos.w;
	 vToL = l[14].lightPos.xyz - g.position;
	 distToL = length(vToL);
	 att = min(1.0f, (distToL / range + distToL / (range * range)) * 0.5f);
	 lum = saturate(dot(vToL / distToL, g.n)) * lCol;
	 result += lum * (1.0f - att);

	 lCol = l[15].lightCol.rgb;
	 range = l[15].lightPos.w;
	 vToL = l[15].lightPos.xyz - g.position;
	 distToL = length(vToL);
	 att = min(1.0f, (distToL / range + distToL / (range * range)) * 0.5f);
	 lum = saturate(dot(vToL / distToL, g.n)) * lCol;
	 result += lum * (1.0f - att);

//	return float4((ambient + result) * Texel.rgb, Texel.a);
	return float4(ToneMapResult((ambient + result) * ToneMapTexture(Texel.rgb, 2.2)), Texel.a);
}