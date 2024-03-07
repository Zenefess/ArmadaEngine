/************************************************************
 * File: gs.gui.hlsl                    Created: 2023/04/09 *
 *                                Last modified: 2023/07/26 *
 *                                                          *
 * Notes: 8 characters per instance.                        *
 *                                                          *
 * 2023/07/02: Moved input data to structured buffer        *
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

struct GOut {	// 44 bytes (2 vectors + 3 scalars)
	float4 pos     : SV_Position;
	float4 col     : COLOR;
	float2 tc      : TEXCOORD;
	uint   ai_type : ATLAS;	// 0-6==Atlas index, 7-10==Type, 11-31==???
};

StructuredBuffer <CHAR_IMM>   alphabet : register(t0); // Character geometry
StructuredBuffer <uint4>      char16   : register(t1); // Text pool
StructuredBuffer <GUI_EL_DYN> element  : register(t2); // Element data

[instance(4)] [maxvertexcount(32)]
void main(in point const VOut input[1], in const uint i : SV_GSInstanceID, inout TriangleStream<GOut> output) {
	const GUI_EL_DYN curElement = element[input[0].index];
	const float2     invScale   = 1.0f / guiScale;

	// Exit if element not visible
	if((curElement.bits & 0x08) || curElement.size.x == 0.0f || curElement.size.y == 0.0f) return;

	// Calculate parent rotation, scaling and translation factors; 3 deep
	float2 parScaling     = 1.0f;
	float2 parTranslation = 0.0f;
	// Does the element have a parent?
   if(curElement.pei != 0x0FFFFFFFF) {
		const GUI_EL_DYN curParent = element[curElement.pei];
		parTranslation += curParent.coords[0].xy;
		parScaling      = curParent.coords[0].zw;
		// Justification
		const float2 adjust = curParent.size - invScale;
		if(curParent.bits & 0x010) { // Origin relative to left of viewport
			parTranslation.x += adjust.x;
		} else if(curParent.bits & 0x020) { // Origin relative to right of viewport
			parTranslation.x -= adjust.x;
		}
		if(curParent.bits & 0x040) { // Origin relative to top of viewport
			parTranslation.y += adjust.y;
			parTranslation.y *= -1.0f;
		} else if(curParent.bits & 0x080) { // Origin relative to bottom of viewport
			parTranslation.y += adjust.y;
		}
		// Does the parent have a parent?
      if(curParent.pei != 0x0FFFFFFFF) {
			const GUI_EL_DYN curParent2 = element[curParent.pei];
			parTranslation	*= parScaling;
			parTranslation += curParent2.coords[0].xy;
			parScaling     *= curParent2.coords[0].zw;
			// Does the parent have a parent?
         if(curParent2.pei != 0x0FFFFFFFF) {
				parTranslation *= parScaling;
				parTranslation += element[curParent2.pei].coords[0].xy;
				parScaling     *= element[curParent2.pei].coords[0].zw;
         }
      }
   }

	const uint   type    = curElement.ind_type & 0x0FF000000;
	const uint   ai_type = ((curElement.ind_type >> 16) & 0x07F) | (type << 7);
	const float2 size    = curElement.size * (curElement.bits & 0x02 ? 1.0f : parScaling);

	// Type==Text
   if(!type) {
		const uint   taOS        = (curElement.taos & 0x0FFFFFFF) + (i >> 1);
		const uint   alphaOS     = curElement.ind_type & 0x0FFFF;
		const uint   arrayStep   = (i & 0x01) << 1;
		const uint2  char16Index = uint2(char16[taOS][arrayStep], char16[taOS][arrayStep + 1]);
      const uint4  chars[2]    = { ((char16Index.xxxx >> shift8888) & 0x0FF) + alphaOS, ((char16Index.yyyy >> shift8888) & 0x0FF) + alphaOS };
		const float4 colour      = float4((uint4(curElement.tint[i >> 1][arrayStep].xx, curElement.tint[i >> 1][arrayStep + 1].xx) >> uint4(0, 16, 0, 16)) & 0x0FFFF) * multi16;

		// Exit if no character(s) or invisible
		if(chars[0].x == alphaOS || colour.a < 0.001f) return;

		// Start point
///--- Discover width of last non-NULL character and replace 2nd "alphabet[chars[0][0]].width" ---///
		const float2 offset       = (1.0f - float2(alphabet[chars[0][0]].width, alphabet[chars[0][0]].width)) * curElement.size.x * 0.5f;
		const float2 adjCoords[4] = input[0].coords;

		float2 coords = adjCoords[i];

		// Justification
      if(curElement.bits & 0x010) { // Origin relative to left of viewport
			coords.x -= offset.x + invScale.x;
      } else if(curElement.bits & 0x020) { // Origin relative to right of viewport
         coords.x -= offset.y + input[0].width - invScale.x;
      } else {
			coords.x -= (input[0].width + offset.x + offset.y) * 0.5f;
      }
		if(curElement.bits & 0x040) { // Origin relative to top of viewport
			coords.y += invScale.y - curElement.size.y;
		} else if(curElement.bits & 0x080) { // Origin relative to bottom of viewport
			coords.y -= invScale.y;
      } else {
			coords.y -= curElement.size.y * 0.5f;
      }
      if(curElement.pei != 0x0FFFFFFFF) {
         coords *= parScaling * guiScale;
         coords += parTranslation;
      }

		[unroll]
		for(uint quad = 0; quad < 2; quad++)
			[unroll]
         for(uint index = 0; index < 4; index++) {
				const uint curChar = chars[quad][index];

				// Exit if null character
				if(curChar == alphaOS) return;

				const float2 pos  = (float2(alphabet[curChar].xos, 0.0f) * size + coords) * guiScale;
				const float2 pos2 = size * guiScale + pos;
				const uint2  chTC = alphabet[curChar].tc;
				const float4 tc   = float4((uint4(chTC.xx, chTC.yy) >> uint4(0, 16, 0, 16)) & 0x0FFFF) * rcp32768;

				const GOut verts[4] = { float4(pos, 0.0f, 1.0f), colour, tc.xw, ai_type,           float4(pos.x, pos2.y, 0.0f, 1.0f), colour, tc.xy, ai_type,
												float4(pos2.x, pos.y, 0.0f, 1.0f), colour, tc.zw, ai_type, float4(pos2, 0.0f, 1.0f), colour, tc.zy, ai_type };

				coords.x += alphabet[curChar].width * size.x;

				// Exit if truncation
				if(coords.x >= 1.0f && curElement.bits & 0x0100) return;

				output.Append(verts[0]);
				output.Append(verts[1]);
				output.Append(verts[2]);
				output.Append(verts[3]);
				output.RestartStrip();
         }
	// Type!=Text
   } else {
		float2 coords = (curElement.coords[0].xy * parScaling + parTranslation);

		// Justification
		const float2 adjust = size - invScale;
      if(curElement.bits & 0x010) { // Origin relative to left of viewport
			coords.x += adjust.x;
      } else if(curElement.bits & 0x020) { // Origin relative to right of viewport
         coords.x -= adjust.x;
      }
		if(curElement.bits & 0x040) { // Origin relative to top of viewport
			coords.y += adjust.y;
			coords.y *= -1.0f;
		} else if(curElement.bits & 0x080) { // Origin relative to bottom of viewport
			coords.y += adjust.y;
		}

		coords -= size;

		const float  stepY  = float(i) * 0.25f;
		const float4 deltas = float4(size * 2.0f, curElement.coords[1].zw - curElement.coords[1].xy);
		const float4 col[4] = { float4((uint4(curElement.tint[0].xx, curElement.tint[0].yy) >> uint4(0, 16, 0, 16)) & 0x0FFFF) * multi16,
										float4((uint4(curElement.tint[0].zz, curElement.tint[0].ww) >> uint4(0, 16, 0, 16)) & 0x0FFFF) * multi16,
										float4((uint4(curElement.tint[1].xx, curElement.tint[1].yy) >> uint4(0, 16, 0, 16)) & 0x0FFFF) * multi16,
										float4((uint4(curElement.tint[1].zz, curElement.tint[1].ww) >> uint4(0, 16, 0, 16)) & 0x0FFFF) * multi16 };

		const float4 colDelta[2] = { col[3] - col[1], col[2] - col[0] };

		[unroll]
		for(float stepX = 0.0f; stepX <= 1.0f; stepX += 0.25f) {
			const float3 step    = float3(stepX, stepY, stepY + 0.25f);
			const float4 pos     = float4((deltas.xy * step.xy + coords) * guiScale, 0.0f, 1.0f);
			const float4 pos2    = float4((deltas.xy * step.xz + coords) * guiScale, 0.0f, 1.0f);
			const float4 cols[2] = { colDelta[0] * step.x + col[1], colDelta[1] * step.x + col[0] };

			const GOut verts[2] = { pos,  (cols[1] - cols[0]) * step.y + cols[0], deltas.zw * step.xy + curElement.coords[1].xy, ai_type,
											pos2, (cols[1] - cols[0]) * step.z + cols[0], deltas.zw * step.xz + curElement.coords[1].xy, ai_type };

			output.Append(verts[0]);
			output.Append(verts[1]);
		}
   }
}
