/************************************************************
 * File: gs.instanced.billboards.hlsl   Created: 2022/10/28 *
 *                                    Last mod.: 2022/10/31 *
 *                                                          *
 * Desc: Geometry shader. Generates billboarded sprites     *
 *       from point lists.                                  *
 *                                                          *
 *  Copyright (c) David William Bull. All rights reserved.  *
 ************************************************************/

cbuffer CB_PROJ {
	matrix camera;
	matrix projection;
//	uint   totalTics[2];
//	float  totalSecs;
//	float  elapsedSecs;
};

/* Reduced packet size -- 256x256 max atlas segments
	-------------------------------------------------
FLOAT3	Sprite location							// 12 bytes
FLOAT		Rotation										// 20 bytes
FLOAT2	Scale											// 24 bytes
UINT														// 28 bytes
	Fixed8p00	Texture X segment
	Fixed8p00	Texture X total segments
	Fixed8p00	Texture Y segment
	Fixed8p00	Texture Y total segments
UINT														// 32 bytes
	Fixed3p00	Squadron generation method
	Fixed7p00	Sprite stride
	Fixed8p14	X/Y distance between sprites
*/

struct VOut {
	float3 position : POSITION;
	float3 r_s : NORMAL;
	uint   bits : BLENDINDICES;
};

struct GOut {
	float4 pos : SV_Position;
	float3 position : POSITION;
	float3 n : NORMAL;
	float2 tex : TEXCOORD;
};

inline float2 RotateVector(float2 v, float2 dir) { return float2(dot(v, float2(dir.x, -dir.y)), dot(v, dir.yx)); }

//[instance(4)]
[maxvertexcount(4)]
void main(point VOut gin[1], uint pID : SV_PrimitiveID, inout TriangleStream<GOut> triStream) {
//void main(point VOut gin[1], uint pID : SV_PrimitiveID, uint iID : SV_GSInstanceID, inout TriangleStream<GOut> triStream) {
	const uint4  uiTV = (gin[0].bits >> uint4(0, 8, 16, 24)) & 0x0FF;
	const float4 fTV  = float4(int4(uiTV));
			float4 fTC  = float4(fTV.x, 1.0f - fTV.z, 1.0, -1.0) / fTV.ywyw;
			float4 fRot = gin[0].r_s.yzyz;
			float2 fDir; sincos(gin[0].r_s.x, fDir.y, fDir.x);
			GOut   output;

	fTC.zw += fTC.xy;
	fRot.w = -fRot.w;
	fRot.xy = RotateVector(fRot.xy, fDir);
	fRot.zw = RotateVector(fRot.zw, fDir);
	
	output.n = float3( 0.0f, 0.0f, -1.0f );

	output.position = gin[0].position;
	float4 prePos = mul(float4(output.position, 1.0f), camera);	// Rotate & scale
	prePos -= float4(fRot.xy, 0.0f, -1.0f);
	output.pos = mul(prePos, projection);
	output.tex = fTC.xy;
	triStream.Append(output);
	
	output.position = gin[0].position;
	prePos = mul(float4(output.position, 1.0f), camera);
	prePos -= float4(fRot.zw, 0.0f, -1.0f);
	output.pos = mul(prePos, projection);
	output.tex = fTC.xw;
	triStream.Append(output);
	
	output.position = gin[0].position;
	prePos = mul(float4(output.position, 1.0f), camera);
	prePos += float4(fRot.zw, 0.0f, 1.0f);
	output.pos = mul(prePos, projection);
	output.tex = fTC.zy;
	triStream.Append(output);
	
	output.position = gin[0].position;
	prePos = mul(float4(output.position, 1.0f), camera);
	prePos += float4(fRot.xy, 0.0f, 1.0f);
	output.pos = mul(prePos, projection);
	output.tex = fTC.zw;
	triStream.Append(output);
}
