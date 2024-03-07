/************************************************************
 * File: vs.gui.hlsl                    Created: 2023/04/09 *
 *                                Last modified: 2023/07/26 *
 *                                                          *
 * Notes: 32 characters per vertex.                         *
 *                                                          *
 * 2023/07/01: Moved input data to structured buffer        *
 *                                                          *
 * To do: 1)Add rotation 2)Add right justification          *
 *                                                          *
 *  Copyright (c) David William Bull. All rights reserved.  *
 ************************************************************/

#include "common.hlsli"
 
cbuffer CB_VIEW : register(b0) {	// 144 bytes (9 vectors)
	const matrix projection;   // Perspective transformation
	const matrix orthographic; // Orthographic transformation
	const float2 guiScale;     // Final X, Y scaling factors for GUI
	const uint2  bitField;     // 0-63==???
};

struct CHAR_IMM {	// 16 bytes (1 scalar)
	uint2 tc;    // Texture coordinates : 4x(1p15)
	float width; // Relative X dimension
	float xos;   // Relative X offset
};

struct GUI_EL_DYN {	// 96 bytes (24 scalars)
	float4 coords[2];	// Text: view coordinates per 8 characters | Element: View position, bounding space for children, x2 texture coordinates
	uint4  tint[2];	// Text: RGBA values per 8 characters | Element: TL/BL/TR/BR RGBA values : p16n0.0~2.0
	float2 size;		// Scale of text | Size of element
	float  rot;			// Amount of rotation
	uint   pei;			// Parent element's GUI_EL_DYN index; this==No parent, bit31==No scaling
	uint   taos;		// 0~27==Offset into structured text buffer, 28==Truncate string, 29==Compress string, 30=???, 31==???
	uint   ind_type;	// 0~15==Offset into alphabet buffer, 16~23==Runtime index of atlas texture, 24~31==Type
	uint   RES;
	uint   bits;		// 0==NoRot, 1==NoScale, 2==NoTrans, 3==Invisible, 4~7==Justification (L,R,T,B)
};							// 8==Truncate, 9==Compress : String modifiers, 10~31==???

struct VOut {	// 40 bytes (2 vector + 2 scalars)
	float2 coords[4] : COORDS; // Adjusted text coordinates
	float  width     : WIDTH;  // Width of text element
	uint   index     : INDEX;  // GUI_EL_DGS index
};

StructuredBuffer <CHAR_IMM>   alphabet : register(t0); // Character geometry
StructuredBuffer <uint4>      char16   : register(t1); // Text pool
StructuredBuffer <GUI_EL_DYN> element  : register(t2); // Element data

VOut main(in const uint index : INDEX) {	
	const GUI_EL_DYN curElement = element[index];

	float4 coords[2] = curElement.coords;
	float  width = 0.0f;

	// If type==Text
   if(!(curElement.ind_type & 0x0F000000)) {
		const float2 scale      = curElement.size;// * guiScale;
		const uint   alphaOS    = curElement.ind_type & 0x0FFFF;
		const uint4  charLot[2] = { char16[curElement.taos], char16[curElement.taos + 1] };
		const uint4  char4[8]   = { ((charLot[0].xxxx >> shift8888) & 0x0FF) + alphaOS, ((charLot[0].yyyy >> shift8888) & 0x0FF) + alphaOS,
											 ((charLot[0].zzzz >> shift8888) & 0x0FF) + alphaOS, ((charLot[0].wwww >> shift8888) & 0x0FF) + alphaOS,
											 ((charLot[1].xxxx >> shift8888) & 0x0FF) + alphaOS, ((charLot[1].yyyy >> shift8888) & 0x0FF) + alphaOS,
											 ((charLot[1].zzzz >> shift8888) & 0x0FF) + alphaOS, ((charLot[1].wwww >> shift8888) & 0x0FF) + alphaOS };

		uint i;
		// Accumulate sub-total of width
		for(i = 0; i < 4 && char4[0][i] > alphaOS; i++)
			width += alphabet[char4[0][i]].width;
		if(i == 4)
			for(i = 0; i < 4 && char4[1][i] > alphaOS; i++)
				width += alphabet[char4[1][i]].width;
		// If second position not set and character(s) to render, add sub-total
		if((curElement.coords[0].z >= 16000.0f || curElement.coords[0].w >= 16000.0f) && char4[2].x > alphaOS) {
			coords[0].z = width * scale.x + coords[0].x;
			coords[0].w = coords[0].y;
		}
		// Accumulate sub-total of width
		if(i == 4)
			for(i = 0; i < 4 && char4[2][i] > alphaOS; i++)
				width += alphabet[char4[2][i]].width;
		if(i == 4)
			for(i = 0; i < 4 && char4[3][i] > alphaOS; i++)
				width += alphabet[char4[3][i]].width;
		// If third position not set and character(s) to render, add sub-total
      if((curElement.coords[1].x >= 16000.0f || curElement.coords[1].y >= 16000.0f) && char4[4].x > alphaOS) {
         coords[1].x = width * scale.x + coords[0].x;
         coords[1].y = coords[0].y;
      }
		// Accumulate sub-total of width
		if(i == 4)
			for(i = 0; i < 4 && char4[4][i] > alphaOS; i++)
				width += alphabet[char4[4][i]].width;
		if(i == 4)
			for(i = 0; i < 4 && char4[5][i] > alphaOS; i++)
				width += alphabet[char4[5][i]].width;
		// If fourth position not set and character(s) to render, add sub-total
		if((curElement.coords[1].z >= 16000.0f || curElement.coords[1].w >= 16000.0f) && char4[6].z & 0x0FF) {
			coords[1].z = width * scale.x + coords[0].x;
			coords[1].w = coords[0].y;
		}
		// Accumulate sub-total of width
		if(i == 4)
			for(i = 0; i < 4 && char4[4][i] > alphaOS; i++)
				width += alphabet[char4[4][i]].width;
		if(i == 4)
			for(i = 0; i < 4 && char4[5][i] > alphaOS; i++)
				width += alphabet[char4[5][i]].width;
		width *= scale.x * (curElement.pei != 0x0FFFFFFFF ? 2.0f : 1.0f);
		// Add else statements to set unused coords to same as 1st/2nd/3rd?

		// Replicate remaining tint colours if only first is set
//      if(!curElement.tint[0].zw && !curElement.tint[1].xy && !curElement.tint[1].zw) {
//			tint[0].zw = tint[0].xy;
//      }
   }

   const VOut output = { coords[0].xy, coords[0].zw, coords[1].xy, coords[1].zw, width, index };

   return output;
}
