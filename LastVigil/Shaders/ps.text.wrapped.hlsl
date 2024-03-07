/************************************************************
 * File: ps.text.wrapped.hlsl           Created: 2023/01/25 *
 *                                Last modified: 2023/01/25 *
 *                                                          *
 * Desc:                                                    *
 *                                                          *
 *  Copyright (c) David William Bull. All rights reserved.  *
 ************************************************************/

struct GOut {
	float4 pos : SV_Position;
	float4 col : COLOUR;
	float2 tc  : TEXCOORD;
};

const Texture2D Texture : register(t4);

const SamplerState Sampler : register(s1);

float4 main(in const GOut input) : SV_Target {
	const float4 texSamp = float4(input.col.rgb, input.col.a * Texture.Sample(Sampler, input.tc).x);
	// Alpha test
	clip(texSamp.a - 0.011f);

	return texSamp;
}
