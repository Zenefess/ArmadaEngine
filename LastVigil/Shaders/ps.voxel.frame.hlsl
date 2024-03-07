/************************************************************
 * File: ps.voxel.frame.hlsl            Created: 2023/01/16 *
 *                                Last modified: 2023/01/17 *
 *                                                          *
 * Notes:                                                   *
 *                                                          *
 *  Copyright (c) David William Bull. All rights reserved.  *
 ************************************************************/

struct VOut {
	float4 pos : SV_Position;
	float2 tex : TEXCOORD;
	float  sec : TIME;
	uint   col : COLOUR;
};

float4 main(in const VOut v) : SV_Target {
	const vector vTint   = vector((v.col >> uint4(0, 8, 16, 24)) & 0x0FF) * 0.0039215686274509803921568627451f;
	const float2 fVRamp  = abs(v.tex * 2.0f - 1.0f);
	const float2 fTexel  = max(0.0f, fVRamp * 2.0f - v.sec);
	const float  fIntens = max(fTexel.x, fTexel.y);
	const vector vOutput	= vector(vTint.rgb, vTint.a * min(1.0f, fIntens));

	clip(vOutput.a - 0.001f);
	return vOutput;
}