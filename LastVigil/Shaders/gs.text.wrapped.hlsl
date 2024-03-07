/************************************************************
 * File: gs.text.wrapped.hlsl           Created: 2023/01/23 *
 *                                Last modified: 2023/01/25 *
 *                                                          *
 * Desc:                                                    *
 *                                                          *
 *  Copyright (c) David William Bull. All rights reserved.  *
 ************************************************************/

struct CHAR_IGS {	// 16 bytes
	uint2 tc;		// Texture coordinates : 4x(1p15)
	float width;	// Relative X dimension
	float xos;		// Relative X offset
};

struct VOut {	// 80 bytes
	uint4 offset[4]   : OFFSETS;	// X coord offsets : 16x2(float16)
	uint  origin      : COORDS;	// 0-15==X offset, 16-31==Y offset : float16
	uint 	size        : SIZE;		// 0-15==X scalar, 16-31==Y scalar : float16
	uint  colour      : COLOUR;	// RGBA : 0p8/9/7/8n
	uint  textArrayOS : TEXTOS;	// 0-30==Offset into structured buffer, 31==Y origin set to top of viewport
};
 
struct GOut {
	float4 pos : SV_Position;
	float4 col : COLOUR;
	float2 tc  : TEXCOORD;
};

const StructuredBuffer<CHAR_IGS> alphabet;
		StructuredBuffer<uint4>    char16;	// Text data array

[instance(32)] [maxvertexcount(16)]
void main(in point const VOut input_[1], in const uint i : SV_GSInstanceID, inout TriangleStream<GOut> triStream) {
	if(!(char16[input_[0].textArrayOS & 0x07FFFFFFF].x & 0x0FF)) return;
	const VOut   input  = input_[0];
	const uint   charIn = char16[(input.textArrayOS & 0x07FFFFFFF) + (i >> 2)][i & 0x03];
	const uint4  chars  = (charIn.xxxx >> uint4(0, 8, 16, 24)) & 0x0FF;
	if(!chars.x) return;
	const float4 multis = float4(0.0039215686274509803921568627451f, 0.00195694716242661448140900195695f, 0.00787401574803149606299212598425f, 0.0039215686274509803921568627451f);
	const float4 colour = float4((input.colour >> uint4(0, 8, 17, 24)) & uint4(0x0FF, 0x01FF, 0x07F, 0x0FF)) * multis;
	const float2 size   = float2(f16tof32(input.size & 0x0FFFF), f16tof32(input.size >> 16));
			float2 origin = float2(f16tof32(input.origin & 0x0FFFF), f16tof32(input.origin >> 16));
	if(input.textArrayOS & 0x080000000) origin.y -= size.y;

	const float2 pos0  = float2(f16tof32((input.offset[i >> 3][(i & 0x06) >> 1] >> ((i & 0x01) << 4)) & 0x0FFFF) * size.x + origin.x, origin.y);
	const float2 pos02 = float2(alphabet[chars[0]].width, 1.0f) * size + pos0;
	const uint2  chTC0 = alphabet[chars[0]].tc;
	const float4 tc0   = float4((uint4(chTC0.xx, chTC0.yy) >> uint4(0, 16, 0, 16)) & 0x0FFFF) * 0.000030517578125f;

	const GOut output0[4] = { float4(pos0, 0.0f, 1.0f), colour, tc0.xw,
									  float4(pos0.x, pos02.y, 0.0f, 1.0f), colour, tc0.xy,
									  float4(pos02.x, pos0.y, 0.0f, 1.0f), colour, tc0.zw,
									  float4(pos02, 0.0f, 1.0f), colour, tc0.zy };
	triStream.Append(output0[0]);
	triStream.Append(output0[1]);
	triStream.Append(output0[2]);
	triStream.Append(output0[3]);
	triStream.RestartStrip();

	const float2 pos1  = float2(alphabet[chars[1]].xos * size.x + pos02.x, origin.y);
	const float2 pos12 = float2(alphabet[chars[1]].width, 1.0f) * size + pos1;
	const uint2  chTC1 = alphabet[chars[1]].tc;
	const float4 tc1   = float4((uint4(chTC1.xx, chTC1.yy) >> uint4(0, 16, 0, 16)) & 0x0FFFF) * 0.000030517578125f;

	const GOut output1[4] = { float4(pos1, 0.0f, 1.0f), colour, tc1.xw,
									  float4(pos1.x, pos12.y, 0.0f, 1.0f), colour, tc1.xy,
									  float4(pos12.x, pos1.y, 0.0f, 1.0f), colour, tc1.zw,
									  float4(pos12, 0.0f, 1.0f), colour, tc1.zy };
	triStream.Append(output1[0]);
	triStream.Append(output1[1]);
	triStream.Append(output1[2]);
	triStream.Append(output1[3]);
	triStream.RestartStrip();

	const float2 pos2  = float2(alphabet[chars[2]].xos * size.x + pos12.x, origin.y);
	const float2 pos22 = float2(alphabet[chars[2]].width, 1.0f) * size + pos2;
	const uint2  chTC2 = alphabet[chars[2]].tc;
	const float4 tc2   = float4((uint4(chTC2.xx, chTC2.yy) >> uint4(0, 16, 0, 16)) & 0x0FFFF) * 0.000030517578125f;

	const GOut output2[4] = { float4(pos2, 0.0f, 1.0f), colour, tc2.xw,
									  float4(pos2.x, pos22.y, 0.0f, 1.0f), colour, tc2.xy,
									  float4(pos22.x, pos2.y, 0.0f, 1.0f), colour, tc2.zw,
									  float4(pos22, 0.0f, 1.0f), colour, tc2.zy };
	triStream.Append(output2[0]);
	triStream.Append(output2[1]);
	triStream.Append(output2[2]);
	triStream.Append(output2[3]);
	triStream.RestartStrip();

	const float2 pos3  = float2(alphabet[chars[3]].xos * size.x + pos22.x, origin.y);
	const float2 pos32 = float2(alphabet[chars[3]].width, 1.0f) * size + pos3;
	const uint2  chTC3 = alphabet[chars[3]].tc;
	const float4 tc3   = float4((uint4(chTC3.xx, chTC3.yy) >> uint4(0, 16, 0, 16)) & 0x0FFFF) * 0.000030517578125f;

	const GOut output3[4] = { float4(pos3, 0.0f, 1.0f), colour, tc3.xw,
									  float4(pos3.x, pos32.y, 0.0f, 1.0f), colour, tc3.xy,
									  float4(pos32.x, pos3.y, 0.0f, 1.0f), colour, tc3.zw,
									  float4(pos32, 0.0f, 1.0f), colour, tc3.zy };
	triStream.Append(output3[0]);
	triStream.Append(output3[1]);
	triStream.Append(output3[2]);
	triStream.Append(output3[3]);
	triStream.RestartStrip();
}