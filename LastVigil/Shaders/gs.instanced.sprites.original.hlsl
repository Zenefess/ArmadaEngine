/************************************************************
 * File: gs.sprites.hlsl                Created: 2022/10/28 *
 *                                Last modified: 2022/10/30 *
 *                                                          *
 * Desc:                                                    *
 *                                                          *
 *  Copyright (c) David William Bull. All rights reserved.  *
 ************************************************************/

cbuffer CB_PROJ {
	matrix camera;
//	uint   totalTics[2];
//	float  totalSecs;
//	float  elapsedSecs;
};

/*  Reduced packet size -- 256x256 max atlas segments
	 -------------------
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
	uint   pID : SV_PrimitiveID;
};

[maxvertexcount(4)]

void main(point VOut gin[1], uint pID : SV_PrimitiveID, inout TriangleStream<GOut> triStream) {
	GOut output;

	const	uint	 bits		= gin[0].bits;	 
	const	uint	 ySegs	= (bits & 0x0FF000000) >> 24u;
	const	uint	 _yOS		= (bits & 0x000FF0000) >> 16u;
	const	float	 yOS		= 1.0f - float(_yOS) / float(ySegs);
	const	float	 yOSd		= rcp(float(ySegs));
	const	uint	 xSegs	= (bits & 0x00000FF00) >> 8u;
	const	uint	 _xOS		= (bits & 0x0000000FF);
	const	float	 xOS		= float(_xOS) / float(xSegs);
	const	float	 xOSd		= rcp(float(xSegs));
			float2 v;

	output.pID = pID;
	output.n = float3( 0.0f, 0.0f, -1.0f );
	
	output.position = gin[0].position;	// Translate
	v.x = -gin[0].r_s.y; v.y = -gin[0].r_s.z;	// Scale
	output.tex = float2(xOS, yOS);
	output.position.x += v.x * cos(gin[0].r_s.x) - v.y * sin(gin[0].r_s.x);	// Rotate
	output.position.y += v.x * sin(gin[0].r_s.x) + v.y * cos(gin[0].r_s.x);
	output.pos = mul(float4(output.position, 1.0f), camera);	// Transform
	triStream.Append(output);
	
	output.position = gin[0].position;
	v.x = -gin[0].r_s.y; v.y = gin[0].r_s.z;
	output.tex = float2(xOS, yOS - yOSd);
	output.position.x += v.x * cos(gin[0].r_s.x) - v.y * sin(gin[0].r_s.x);
	output.position.y += v.x * sin(gin[0].r_s.x) + v.y * cos(gin[0].r_s.x);
	output.pos = mul(float4(output.position, 1.0f), camera);
	triStream.Append(output);
	
	output.position = gin[0].position;
	v.x = gin[0].r_s.y; v.y = -gin[0].r_s.z;
	output.tex = float2(xOS + xOSd, yOS);
	output.position.x += v.x * cos(gin[0].r_s.x) - v.y * sin(gin[0].r_s.x);
	output.position.y += v.y * sin(gin[0].r_s.x) + v.y * cos(gin[0].r_s.x);
	output.pos = mul(float4(output.position, 1.0f), camera);
	triStream.Append(output);
	
	output.position = gin[0].position;
	v.x = gin[0].r_s.y; v.y = gin[0].r_s.z;
	output.tex = float2(xOS + xOSd, yOS - yOSd);
	output.position.x += v.x * cos(gin[0].r_s.x) - v.y * sin(gin[0].r_s.x);
	output.position.y += v.y * sin(gin[0].r_s.x) + v.y * cos(gin[0].r_s.x);
	output.pos = mul(float4(output.position, 1.0f), camera);
	triStream.Append(output);
}
