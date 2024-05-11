/************************************************************
 * File: gs.gui.hlsl                    Created: 2023/04/09 *
 * Type: Geometry shader          Last modified: 2024/04/17 *
 *                                                          *
 * Notes: 8 characters per instance.                        *
 *                                                          *
 * 2023/07/02: Moved input data to structured buffer        *
 * 2024/04/01: GUI_EL_DYN SRV+Adj.coords replaced with UAV  *
 * 2024/04/05: Added truncation functionality to text       *
 * 2024/04/09: Added compression functionality to text, and *
 *             rotation functionality to non-text           *
 * 2024/04/16: Added rotation functionality to text         *
 *                                                          *
 * To do: Add elliptical bounding shape                     *
 *        Add wide character support                        *
 *        Add control character support                     *
 *                                                          *
 * Copyright (c) David William Bull.   All rights reserved. *
 ************************************************************/

#include "common.hlsli"
 
cbuffer CB_VIEW : register(b0) { // 160 bytes (10 vectors)
   const matrix projection;   // Perspective transformation
   const matrix orthographic; // Orthographic transformation
   const float2 guiScale;     // Final X & Y scaling factors for GUI
   const uint2  bitField;     // 0-63==???
   const uint4  RES;
};

struct CHAR_IMM { // 16 bytes (1 vector)
   uint2 tc;   // Texture coordinates : 4x(1p15)
   uint  size; // Relative X&Y dimensions : p16n0.0~1.0
   uint  os;   // Relative X&Y offsets : p16n-1.0~1.0
};

struct GUI_EL_DYN { // 96 bytes (6 vectors)
   float4 coords[2]; // Text: view coordinates per 8 characters | Element: View position, bounding space for children, x2 texture coordinates
   uint4  tint[2];   // Text: RGBA values per 8 characters | Element: TL/BL/TR/BR RGBA values : p16n0.0~2.0
   float2 size;      // Scale of text | Size of element
   float  rot;       // Amount of rotation
   float  width;     // Total width of vertex's text in view space
   uint   pei;       // Parent element's GUI_EL_DYN index: this==No parent
   uint   taos;      // 0~25==Offset into text array (div.by 16), 26~31==Vertex's char count
   uint   ind_type;  // 0~15==Offset into alphabet buffer, 16~23==Runtime index of alphabet's atlas texture, 24~31==Element type
   uint   seo_bits;  // 0~15==First sibling element offset, 16~19==Justification (L,R,T,B), 20==Elliptical bounding space, 21~22==???, 23==Invisible
};                   // 24==Rotate, 25==Translate, 26&27==Scale: X&Y, 28==Truncate, 29==Compress, 30==Process control characters, 31==Wide chars

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
   if((curBits & 0x080) || curElement.size.x == 0.0f || curElement.size.y == 0.0f) return;

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
         if(curParent.seo_bits & 0x030000) parTranslation.x -= invScale.x - curParent.size.x; // Origin relative to left/right of viewport
         if(curParent.seo_bits & 0x0C0000) parTranslation.y -= invScale.y - curParent.size.y; // Origin relative to top/bottom of viewport
         if(curParent.seo_bits & 0x020000) parTranslation.x *= -1.0f;                         // Flip X coordinate relative to right of viewport
         if(curParent.seo_bits & 0x040000) parTranslation.y *= -1.0f;                         // Flip Y coordinate relative to top of viewport

         // Does the parent have a parent?
         if(curParent.pei != 0x0FFFFFFFF) {
            const GUI_EL_DYN curParent2  = element[curParent.pei];
            const float2     tempScaling = parScaling.xy;

            parScaling.zw *= curParent2.coords[0].zw;

            // 'Scale' bits test
                 if((curParent.seo_bits & 0x0C000000) == 0x04000000) parScaling.xy *= curParent2.coords[0].zz;
            else if((curParent.seo_bits & 0x0C000000) == 0x08000000) parScaling.xy *= curParent2.coords[0].ww;
            else if((curParent.seo_bits & 0x0C000000) == 0x0C000000) parScaling.xy *= curParent2.coords[0].zw;

            // 'Translate' bit test
            if(curParent.seo_bits & 0x02000000) {
               parTranslation += curParent2.coords[0].xy;

               // Justification test
               if(curParent2.seo_bits & 0x030000) parTranslation.x -= invScale.x - curParent2.size.x; // Origin relative to left/right of viewport
               if(curParent2.seo_bits & 0x0C0000) parTranslation.y -= invScale.y - curParent2.size.y; // Origin relative to top/bottom of viewport
               if(curParent2.seo_bits & 0x020000) parTranslation.x *= -1.0f;                          // Flip X coordinate relative to right of viewport
               if(curParent2.seo_bits & 0x040000) parTranslation.y *= -1.0f;                          // Flip Y coordinate relative to top of viewport

               // Does the grandparent have a parent?
               if(curParent2.pei != 0x0FFFFFFFF) {
                  const GUI_EL_DYN curParent3   = element[curParent2.pei];
                  const float2     tempScaling2 = parScaling.xy;

                  parScaling.zw *= curParent3.coords[0].zw;

                  // 'Scale' bits test
                       if((curParent2.seo_bits & 0x0C000000) == 0x04000000) parScaling.xy *= curParent3.coords[0].zz;
                  else if((curParent2.seo_bits & 0x0C000000) == 0x08000000) parScaling.xy *= curParent3.coords[0].ww;
                  else if((curParent2.seo_bits & 0x0C000000) == 0x0C000000) parScaling.xy *= curParent3.coords[0].zw;

                  // 'Translate' bit test
                  if(curParent.seo_bits & 0x02000000) {
                     parTranslation += curParent3.coords[0].xy;

                     // Justification test
                     if(curParent3.seo_bits & 0x030000) parTranslation.x -= invScale.x - curParent3.size.x; // Origin relative to left/right of viewport
                     if(curParent3.seo_bits & 0x0C0000) parTranslation.y -= invScale.y - curParent3.size.y; // Origin relative to top/bottom of viewport
                     if(curParent3.seo_bits & 0x020000) parTranslation.x *= -1.0f;                          // Flip X coordinate relative to right of viewport
                     if(curParent3.seo_bits & 0x040000) parTranslation.y *= -1.0f;                          // Flip Y coordinate relative to top of viewport
                  } else parScaling.xy = tempScaling2;
               }
            } else parScaling.xy = tempScaling;
         }
      } else parScaling.xy = 1.0f;
   }

   const uint type    = curElement.ind_type & 0x0FF000000;
   const uint ai_type = ((curElement.ind_type >> 16) & 0x07F) | (type << 7);
   
   float2 coords = 0.0f;
   float  border = ((curElement.pei != 0x0FFFFFFFF) && (curBits & 0x0200) ? invScale.y : invScale.x);

   // Type!=Text
   [branch] if(type) {
      const float4 size   = float4(curElement.size * parScaling.xy, curElement.size * parScaling.xy / parScaling.zw);
      const float4 col[4] = { float4((uint4(curElement.tint[0].xx, curElement.tint[0].yy) >> uint4(0, 16, 0, 16)) & 0x0FFFF) * multi16,
                              float4((uint4(curElement.tint[0].zz, curElement.tint[0].ww) >> uint4(0, 16, 0, 16)) & 0x0FFFF) * multi16,
                              float4((uint4(curElement.tint[1].xx, curElement.tint[1].yy) >> uint4(0, 16, 0, 16)) & 0x0FFFF) * multi16,
                              float4((uint4(curElement.tint[1].zz, curElement.tint[1].ww) >> uint4(0, 16, 0, 16)) & 0x0FFFF) * multi16 };
      GOut vert[3];

      coords = curElement.coords[0].xy;

      // Justification
      [flatten] if(curBits & 0x03) {  // Origin relative to left/right of viewport
         coords.x += size.z - border;
         [flatten] if(curBits & 0x02) // Origin relative to right of viewport
            coords.x *= -1.0f;
      }
      [flatten] if(curBits & 0x0C) {  // Origin relative to top/bottom of viewport
         coords.y += size.w - invScale.y;
         [flatten] if(curBits & 0x04) // Origin relative to top of viewport
            coords.y *= -1.0f;
      }

      const float4 rotVals = { coords.xy, sin(curElement.rot), cos(curElement.rot)};

      // 'Rotate' && 'translate' bits test: Transrotate around parent's center
      if((curBits & 0x0300) == 0x0300)
         coords = float2((rotVals.x * rotVals.w) - (rotVals.y * rotVals.z), (rotVals.x * rotVals.z) + (rotVals.y * rotVals.w));

      // If parented, scale & translate coordinates to parent space
      if(curElement.pei != 0x0FFFFFFFF) {
         coords *= parScaling.zw;
         coords += parTranslation;
      }

      // Calculate vertices
      vert[0].ai_type = vert[1].ai_type = vert[2].ai_type = ai_type;

      vert[0].pos = vert[1].pos = float4(0.0f, 0.0f, 0.0f, 1.0f);
      vert[2].pos = float4(coords * guiScale, 0.0f, 1.0f);
      vert[2].col = (col[0] + col[1] + col[2] + col[3]) * 0.25f;
      vert[2].tc  = (curElement.coords[1].xy + curElement.coords[1].zw) * 0.5f;

      [flatten] switch(i) { // Only [flatten] compiles with optimisations
      case 0:
         vert[0].pos.xy -= size.xy;
         vert[0].col     = col[0];
         vert[0].tc      = curElement.coords[1].xy;
         vert[1].pos.xy  = float2(-size.x, size.y);
         vert[1].col     = col[2];
         vert[1].tc      = curElement.coords[1].xw;
         break;
      case 1:
         vert[0].pos.xy = float2(-size.x, size.y);
         vert[0].col    = col[2];
         vert[0].tc     = curElement.coords[1].xw;
         vert[1].pos.xy = size.xy;
         vert[1].col    = col[3];
         vert[1].tc     = curElement.coords[1].zw;
         break;
      case 2:
         vert[0].pos.xy = size.xy;
         vert[0].col    = col[3];
         vert[0].tc     = curElement.coords[1].zw;
         vert[1].pos.xy = float2(size.x, -size.y);
         vert[1].col    = col[1];
         vert[1].tc     = curElement.coords[1].zy;
         break;
      case 3:
         vert[0].pos.xy  = float2(size.x, -size.y);
         vert[0].col     = col[1];
         vert[0].tc      = curElement.coords[1].zy;
         vert[1].pos.xy -= size.xy;
         vert[1].col     = col[0];
         vert[1].tc      = curElement.coords[1].xy;
         break;
      }

      // 'Rotate' bits test: Rotate around element center
      if(curBits & 0x0100) {
         const float2 tempPos0 = float2((vert[0].pos.x * rotVals.w) - (vert[0].pos.y * rotVals.z), (vert[0].pos.x * rotVals.z) + (vert[0].pos.y * rotVals.w));
         const float2 tempPos1 = float2((vert[1].pos.x * rotVals.w) - (vert[1].pos.y * rotVals.z), (vert[1].pos.x * rotVals.z) + (vert[1].pos.y * rotVals.w));

         vert[0].pos.xy = tempPos0;
         vert[1].pos.xy = tempPos1;
      }

      vert[0].pos.xy += coords;
      vert[1].pos.xy += coords;
      vert[0].pos.xy *= guiScale;
      vert[1].pos.xy *= guiScale;

      output.Append(vert[0]);
      output.Append(vert[1]);
      output.Append(vert[2]);

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

      float4 vertPos[2];

      // Exit if no character(s) or invisible
      if(chars[0].x == alphaOS || colour.a < 0.001f) return;

      // Calculate total string width and character count
      const uint firstVertex = ((curElement.seo_bits & 0x08000) ? input[0] : input[0] - (curElement.seo_bits & 0x07FFF));
      const uint lastVertex  = firstVertex + (element[firstVertex].seo_bits & 0x07FFF);

      uint2  li = { 0, 0 };
      float2 totalWidth = { element[firstVertex].width, 0.0f };
      uint   totalChars = element[firstVertex].taos >> 26;

      for(uint vi = firstVertex + 1; vi <= lastVertex; vi++) {
         totalWidth += float2(element[vi].width, element[vi - 1].width);
         totalChars += element[vi].taos >> 26;
      }
      
      totalWidth *= 0.5f;

      // 'Compress' bit test
      if((curBits & 0x02000) && (totalWidth.x > 1.0f)) {
         parScaling.xz /= totalWidth.x;
         border        *= totalWidth.x;
      }

      // Start point --- !!! Move character size & dimension transformations to inner loop? !!!
      const float4 size     = float4(curElement.size * parScaling.xy, parScaling.xy / parScaling.zw);
      const float2 offset   = (1.0f - float2(uint2(alphabet[chars[0][0]].size & 0x0FFFF, alphabet[chars[0][0]].size >> 16)) * rcp65535) * curElement.size * 0.5f;
      const float  adjWidth = (totalChars > 32 ? totalWidth.x : curElement.width);

      // Justification
      [flatten] if(curBits & 0x03) {  // Origin relative to left/right of viewport
         coords.x = adjWidth * size.z * 0.5f - border;
         [flatten] if((curBits & 0x02) && !((curBits & 0x02000) && ((totalWidth.x > 1.0f) || (totalChars > 32)))) // Origin relative to right of viewport && not filling viewport width
            coords.x *= -1.0f;
      }
      [flatten] if(curBits & 0x0C) {  // Origin relative to top/bottom of viewport
         coords.y = curElement.size.y * size.w * 0.5f - invScale.y;
         [flatten] if(curBits & 0x04) // Origin relative to top of viewport
            coords.y *= -1.0f;
      }

      coords += element[input[0]].coords[0].xy * size.zw;
      float2 coords2 = ((i & 0x01) ? element[input[0]].coords[i_div2].zw : element[input[0]].coords[i_div2].xy) - element[input[0]].coords[0].xy;

      const float4 rotVals = { coords, sin(curElement.rot), cos(curElement.rot) };

      // 'Rotate' && 'translate' bits test: Transrotate around parent's center
      if((curBits & 0x0300) == 0x0300) {
         coords.x = (rotVals.x * rotVals.w) - (rotVals.y * rotVals.z) * size.z;
         coords.y = (rotVals.x * rotVals.z) + (rotVals.y * rotVals.w) * size.w;
      }

      // Width offset
      vertPos[0].x = (adjWidth * 0.5f - coords2.x + offset.x) * size.z;
      // Second justification test for height offset
           if(curBits & 0x04) vertPos[0].y = (curElement.size.y * 0.5f - offset.y) * size.w; // Origin relative to top of viewport
      else if(curBits & 0x08) vertPos[0].y = (curElement.size.y * 0.5f + offset.y) * size.w; // Origin relative to bottom of viewport
      else                    vertPos[0].y = (curElement.size.y * 0.5f) * size.w;
      // 'Rotate' bit test: rotate anchor vertex around element's center
      if((curBits & 0x0100) == 0x0100) {
         coords.x -= (vertPos[0].x * rotVals.w) - (vertPos[0].y * rotVals.z);
         coords.y -= (vertPos[0].x * rotVals.z) + (vertPos[0].y * rotVals.w);
      } else {
         coords -= vertPos[0].xy;
      }

      // If parented, scale & translate coordinates to parent space
      if(curElement.pei != 0x0FFFFFFFF) {
         coords *= parScaling.zw;
         coords += parTranslation;
      }

      // Generate vertices
      [unroll] for(uint quad = 0; quad < 2; quad++)
         [unroll] for(uint index = 0; index < 4; index++) {
            const uint curChar = chars[quad][index];

            // Exit if null character
            if(curChar == alphaOS) return;

            const float2 os    = float2(uint2(alphabet[curChar].os & 0x0FFFF, alphabet[curChar].os >> 16)) * 0.0000305180437933928435187304493782f - 1.0f;
            const float4 pos12 = { os * size.xy, os * size.xy + float2(0.0f, size.y) };
            const float4 pos34 = pos12 + float4(size.x, 0.0f, size.x, 0.0f);

            vertPos[0] = float4((pos12.x * rotVals.w) - (pos12.y * rotVals.z), (pos12.x * rotVals.z) + (pos12.y * rotVals.w),
                                (pos12.z * rotVals.w) - (pos12.w * rotVals.z), (pos12.z * rotVals.z) + (pos12.w * rotVals.w));
            vertPos[1] = float4((pos34.x * rotVals.w) - (pos34.y * rotVals.z), (pos34.x * rotVals.z) + (pos34.y * rotVals.w),
                                (pos34.z * rotVals.w) - (pos34.w * rotVals.z), (pos34.z * rotVals.z) + (pos34.w * rotVals.w));
            vertPos[0] += coords.xyxy;
            vertPos[0] *= guiScale.xyxy;
            vertPos[1] += coords.xyxy;
            vertPos[1] *= guiScale.xyxy;

            const uint2  chTC = alphabet[curChar].tc;
            const float4 tc   = float4((uint4(chTC.xx, chTC.yy) >> uint4(0, 16, 0, 16)) & 0x0FFFF) * rcp32768;

            const GOut verts[4] = { float4(vertPos[0].xy, 0.0f, 1.0f), colour, tc.xw, ai_type, float4(vertPos[0].zw, 0.0f, 1.0f), colour, tc.xy, ai_type,
                                    float4(vertPos[1].xy, 0.0f, 1.0f), colour, tc.zw, ai_type, float4(vertPos[1].zw, 0.0f, 1.0f), colour, tc.zy, ai_type };

            // 'Truncate' bit tests
            const float limit = parScaling.z * (offset.x + 1.0f);
            if((curBits & 0x01000) && (coords.x + size.x >= limit)) return; // Character exceeds right border?
            [flatten] if((curBits & 0x01000) && (coords.x >= -limit)) {} {  // Character does not exceed left border?
               output.Append(verts[0]);
               output.Append(verts[1]);
               output.Append(verts[2]);
               output.Append(verts[3]);
               output.RestartStrip();
            }

            const float adjSize = float(alphabet[curChar].size & 0x0FFFF) * size.x * rcp65535;
            coords += float2(adjSize * rotVals.w, adjSize * rotVals.z);
         }
   }
}
