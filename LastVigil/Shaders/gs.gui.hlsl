/************************************************************
 * File: gs.gui.hlsl                    Created: 2023/04/09 *
 *                                Last modified: 2024/04/09 *
 *                                                          *
 * Notes: 8 characters per instance.                        *
 *                                                          *
 * 2023/07/02: Moved input data to structured buffer        *
 * 2024/04/01: GUI_EL_DYN SRV+Adj.coords replaced with UAV  *
 * 2024/04/05: Added truncation functionality to text       *
 * 2024/04/09: Added compression functionality to text      *
 *                                                          *
 * To do: 1)Add rotation                                    *
 *                                                          *
 *  Copyright (c) David William Bull. All rights reserved.  *
 ************************************************************/

#include "common.hlsli"
 
cbuffer CB_VIEW : register(b0) { // 144 bytes (9 vectors)
   const matrix projection;   // Perspective transformation
   const matrix orthographic; // Orthographic transformation
   const float2 guiScale;     // Final X & Y scaling factors for GUI
   const uint2  bitField;     // 0-63==???
};

struct CHAR_IMM { // 16 bytes (1 scalar)
   uint2 tc;    // Texture coordinates : 4x(1p15)
   uint  size; // Relative X&Y dimensions : p16n0.0~1.0
   uint  os;   // Relative X&Y offsets : p-1.0~1.0
};

struct GUI_EL_DYN { // 96 bytes (24 scalars)
   float4 coords[2]; // Text: view coordinates per 8 characters | Element: View position, bounding space for children, x2 texture coordinates
   uint4  tint[2];   // Text: RGBA values per 8 characters | Element: TL/BL/TR/BR RGBA values : p16n0.0~2.0
   float2 size;      // Scale of text | Size of element
   float  rot;       // Amount of rotation
   float  width;     // Total width of vertex's text in view space
   uint   pei;       // Parent element's GUI_EL_DYN index: this==No parent
   uint   taos;      // 0~25==Offset into text array (div.by 16), 26~31==Vertex's char count
   uint   ind_type;  // 0~15==Offset into alphabet buffer, 16~23==Runtime index of alphabet's atlas texture, 24~31==Element type
   uint   seo_bits;  // 0~15==First sibling element offset, 16~19==Justification (L,R,T,B), 20~23==???
};                   // 24==Rotate, 25==Translate, 26&27==Scale: X&Y, 28==Invisible, 29==Truncate, 30==Compress, 31==Wide chars

struct GOut { // 44 bytes (2 vectors + 3 scalars)
   float4 pos     : SV_Position;
   float4 col     : COLOR;
   float2 tc      : TEXCOORD;
   uint   ai_type : ATLAS;       // 0~6==Atlas index, 7~10==Type, 11~31==???
};

StructuredBuffer   <CHAR_IMM>   alphabet : register(t0); // Character geometry
StructuredBuffer   <uint4>      char16   : register(t1); // Text pool
RWStructuredBuffer <GUI_EL_DYN> element  : register(u1); // Element data

[instance(4)] [maxvertexcount(32)]
void main(in point const uint input[1] : INDEX, in const uint i : SV_GSInstanceID, inout TriangleStream<GOut> output) {
   const GUI_EL_DYN curElement = element[input[0]];
   
   const float2 invScale = 1.0f / guiScale;
   const uint   curBits  = curElement.seo_bits >> 16;

   // Exit if element not visible
   if((curBits & 0x01000) || curElement.size.x == 0.0f || curElement.size.y == 0.0f) return;

   // Calculate parent rotation, scaling and translation factors; 3 deep
   float4 parScaling     = 1.0f; // .xy==Scale, .zw==Scaled translation
   float2 parTranslation = 0.0f;

   // Does the element have a parent?
   [branch] if(curElement.pei != 0x0FFFFFFFF) {
      const GUI_EL_DYN curParent = element[curElement.pei];

      parScaling.zw = curParent.coords[0].zw;

      // 'Scale' bits test
           if((curBits & 0x0C00) == 0x0400) parScaling.xy = curParent.coords[0].zz;
      else if((curBits & 0x0C00) == 0x0800) parScaling.xy = curParent.coords[0].ww;
      else if((curBits & 0x0C00) == 0x0C00) parScaling.xy = curParent.coords[0].zw;

      // 'Translate' bit test
      [branch] if(curBits & 0x0200) {
         parTranslation = curParent.coords[0].xy;

         // Justification test
         const float2 adjust = invScale - curParent.size;

              if(curParent.seo_bits & 0x010000) parTranslation.x -= adjust.x; // Origin relative to left of viewport
         else if(curParent.seo_bits & 0x020000) parTranslation.x += adjust.x; // Origin relative to right of viewport
              if(curParent.seo_bits & 0x040000) parTranslation.y += adjust.y; // Origin relative to top of viewport
         else if(curParent.seo_bits & 0x080000) parTranslation.y -= adjust.y; // Origin relative to bottom of viewport

         // Does the parent have a parent?
         if(curParent.pei != 0x0FFFFFFFF) {
            const GUI_EL_DYN curParent2  = element[curParent.pei];
            const float2     tempScaling = parScaling.xy;

            parScaling.zw *= curParent2.coords[0].zw;

            // 'Scale' bits test
                 if((curParent.seo_bits & 0x04000000) == 0x04000000) parScaling.xy *= curParent2.coords[0].zz;
            else if((curParent.seo_bits & 0x08000000) == 0x08000000) parScaling.xy *= curParent2.coords[0].ww;
            else if((curParent.seo_bits & 0x0C000000) == 0x0C000000) parScaling.xy *= curParent2.coords[0].zw;

            // 'Translate' bit test
            if(curParent.seo_bits & 0x02000000) {
               parTranslation += curParent2.coords[0].xy;

               // Justification test
               const float2 adjust2 = invScale - curParent2.size;

                    if(curParent2.seo_bits & 0x010000) parTranslation.x -= adjust2.x; // Origin relative to left of viewport
               else if(curParent2.seo_bits & 0x020000) parTranslation.x += adjust2.x; // Origin relative to right of viewport
                    if(curParent2.seo_bits & 0x040000) parTranslation.y += adjust2.y; // Origin relative to top of viewport
               else if(curParent2.seo_bits & 0x080000) parTranslation.y -= adjust2.y; // Origin relative to bottom of viewport

               // Does the grandparent have a parent?
               if(curParent2.pei != 0x0FFFFFFFF) {
                  const GUI_EL_DYN curParent3   = element[curParent2.pei];
                  const float2     tempScaling2 = parScaling.xy;

                  parScaling.zw *= curParent3.coords[0].zw;

                  // 'Scale' bits test
                       if((curParent2.seo_bits & 0x04000000) == 0x04000000) parScaling.xy *= curParent3.coords[0].zz;
                  else if((curParent2.seo_bits & 0x08000000) == 0x08000000) parScaling.xy *= curParent3.coords[0].ww;
                  else if((curParent2.seo_bits & 0x0C000000) == 0x0C000000) parScaling.xy *= curParent3.coords[0].zw;

                  // 'Translate' bit test
                  if(curParent.seo_bits & 0x02000000) {
                     parTranslation += curParent2.coords[0].xy;

                     // Justification test
                     const float2 adjust3 = invScale - curParent3.size;

                          if(curParent3.seo_bits & 0x010000) parTranslation.x -= adjust3.x; // Origin relative to left of viewport
                     else if(curParent3.seo_bits & 0x020000) parTranslation.x += adjust3.x; // Origin relative to right of viewport
                          if(curParent3.seo_bits & 0x040000) parTranslation.y += adjust3.y; // Origin relative to top of viewport
                     else if(curParent3.seo_bits & 0x080000) parTranslation.y -= adjust3.y; // Origin relative to bottom of viewport
                  } else parScaling.xy = tempScaling2;
               }
            } else parScaling.xy = tempScaling;
         }
      } else parScaling.xy = 1.0f;
   }

   const uint  type    = curElement.ind_type & 0x0FF000000;
   const uint  ai_type = ((curElement.ind_type >> 16) & 0x07F) | (type << 7);
   
   float  border = ((curElement.pei != 0x0FFFFFFFF) && (curBits & 0x0200) ? invScale.y : invScale.x);
   float2 coords;

   // Type!=Text
   [branch] if(type) {
      const float2 size = curElement.size + curElement.size;

      coords = curElement.coords[0].xy;

      // Justification
           if(curBits & 0x01) coords.x -= border;                                     // Origin relative to left of viewport
      else if(curBits & 0x02) { coords.x -= border - size.x; coords.x *= -1.0f; }     // Origin relative to right of viewport
      else                    coords.x -= curElement.size.x;
           if(curBits & 0x04) { coords.y -= invScale.y - size.y; coords.y *= -1.0f; } // Origin relative to top of viewport
      else if(curBits & 0x08) coords.y -= invScale.y;                                 // Origin relative to bottom of viewport
      else                    coords.y -= curElement.size.y;

      // If parented, scale & translate coordinates to parent space
      if(curElement.pei != 0x0FFFFFFFF) {
         coords *= parScaling.xy;
         coords += parTranslation;
      }

      const float  stepY  = float(i) * 0.25f;
      const float4 deltas = float4(curElement.size * 2.0f, curElement.coords[1].zw - curElement.coords[1].xy);
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
      
      return;
   // Type==Text
   } else {
      const uint   i_div2      = i >> 1;
      const uint   taOS        = (curElement.taos & 0x03FFFFFF) + (i_div2);
      const uint   alphaOS     = curElement.ind_type & 0x0FFFF;
      const uint   arrayStep   = (i & 0x01) << 1;
      const uint2  char16Index = uint2(char16[taOS][arrayStep], char16[taOS][arrayStep + 1]);
      const uint4  chars[2]    = { ((char16Index.xxxx >> shift8888) & 0x0FF) + alphaOS, ((char16Index.yyyy >> shift8888) & 0x0FF) + alphaOS };
      const float4 colour      = float4((uint4(curElement.tint[i_div2][arrayStep].xx, curElement.tint[i_div2][arrayStep + 1].xx) >> uint4(0, 16, 0, 16)) & 0x0FFFF) * multi16;

      // Exit if no character(s) or invisible
      if(chars[0].x == alphaOS || colour.a < 0.001f) return;

      // Calculate total string width and character count
      const uint firstVertex = ((curElement.seo_bits & 0x08000) ? input[0] : input[0] - (curElement.seo_bits & 0x07FFF));
      const uint lastVertex  = firstVertex + (element[firstVertex].seo_bits & 0x07FFF);

      float totalWidth = element[firstVertex].width;
      uint  totalChars = element[firstVertex].taos >> 26;

      for(uint vi = firstVertex + 1; vi <= lastVertex; vi++) {
         totalWidth += element[vi].width;
         totalChars += element[vi].taos >> 26;
      }
      
      totalWidth *= 0.5f;

      // 'Compress' bit test
      if((curBits & 0x04000) && (totalWidth > 1.0f)) {
         parScaling.xz /= totalWidth;
         border        *= totalWidth;
      }

      // Start point --- !!! Move character size & dimension transformations to inner loop? !!!
      const float4 size     = float4(curElement.size * parScaling.xy, parScaling.xy / parScaling.zw);
      const float2 offset   = (1.0f - float2(uint2(alphabet[chars[0][0]].size & 0x0FFFF, alphabet[chars[0][0]].size >> 16)) * rcp65535) * curElement.size * 0.5f;
      const float  adjWidth = (totalChars > 32 ? totalWidth : curElement.width);

      coords = ((i & 0x01) ? element[input[0]].coords[i_div2].zw : element[input[0]].coords[i_div2].xy) * size.zw;

      // Justification test
           if((curBits & 0x01) || ((curBits & 0x04000) && ((totalWidth > 1.0f) || (totalChars > 32))))
                              coords.x -= offset.x * size.z + border;                           // Origin relative to left of viewport
      else if(curBits & 0x02) coords.x -= (offset.x + adjWidth) * size.z - border;              // Origin relative to right of viewport
      else                    coords.x -= (adjWidth * 0.5f + offset.x) * size.z;
           if(curBits & 0x04) coords.y += (offset.y - curElement.size.y) * size.w + invScale.y; // Origin relative to top of viewport
      else if(curBits & 0x08) coords.y -= offset.y * size.w + invScale.y;                       // Origin relative to bottom of viewport
      else                    coords.y -= curElement.size.y * size.w * 0.5f;

      // If parented, scale & translate coordinates to parent space
      if(curElement.pei != 0x0FFFFFFFF) {
         coords *= parScaling.zw;
         coords += parTranslation;
      }

      // Generate vertices
      [unroll]
      for(uint quad = 0; quad < 2; quad++)
         [unroll]
         for(uint index = 0; index < 4; index++) {
            const uint curChar = chars[quad][index];

            // Exit if null character
            if(curChar == alphaOS) return;

            const float2 os   = float2(uint2(alphabet[curChar].os & 0x0FFFF, alphabet[curChar].os >> 16)) * 0.0000305180437933928435187304493782f - 1.0f;
            const float2 pos  = (os * size.xy + coords) * guiScale;
            const float2 pos2 = size.xy * guiScale + pos;
            const uint2  chTC = alphabet[curChar].tc;
            const float4 tc   = float4((uint4(chTC.xx, chTC.yy) >> uint4(0, 16, 0, 16)) & 0x0FFFF) * rcp32768;

            const GOut verts[4] = { float4(pos, 0.0f, 1.0f), colour, tc.xw, ai_type,           float4(pos.x, pos2.y, 0.0f, 1.0f), colour, tc.xy, ai_type,
                                    float4(pos2.x, pos.y, 0.0f, 1.0f), colour, tc.zw, ai_type, float4(pos2, 0.0f, 1.0f), colour, tc.zy, ai_type };

            // 'Truncate' bit tests
            const float limit = parScaling.z * (offset.x + 1.0f);
            if((curBits & 0x02000) && (coords.x + size.x >= limit)) return; // Character exceeds right border?
            [flatten] if((curBits & 0x02000) && (coords.x >= -limit)) {} {  // Character does not exceed left border?
               output.Append(verts[0]);
               output.Append(verts[1]);
               output.Append(verts[2]);
               output.Append(verts[3]);
               output.RestartStrip();
            }

            coords.x += float(alphabet[curChar].size & 0x0FFFF) * size.x * rcp65535;
         }
   }
}
