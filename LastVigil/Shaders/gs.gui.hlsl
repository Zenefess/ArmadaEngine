/************************************************************************************************
 * File: gs.gui.hlsl                                                        Created: 2023/04/09 *
 * Type: Geometry shader                                              Last modified: 2024/07/29 *
 *                                                                                              *
 * Notes: 8 characters per instance.                                                            *
 *                                                                                              *
 * 2023/07/02: Moved input data to structured buffer                                            *
 * 2024/04/01: GUI_EL_DYN SRV+Adj.coords replaced with UAV                                      *
 * 2024/04/05: Added truncation functionality to text                                           *
 * 2024/04/09: Added compression functionality to text, and rotation functionality to non-text  *
 * 2024/04/16: Added rotation functionality to text                                             *
 *                                                                                              *
 * To do: Add elliptical bounding shape                                                         *
 *        Add wide character support                                                            *
 *        Add control character support                                                         *
 *                                                                                              *
 * Copyright (c) David William Bull.                                       All rights reserved. *
 ************************************************************************************************/

#include "gui.hlsli"

StructuredBuffer   <CHAR_IMM>   alphabet : register(t80); // Character geometry
StructuredBuffer   <uint4>      char16   : register(t81); // Text pool
RWStructuredBuffer <GUI_EL_DYN> element  : register(u1);  // Element data

[instance(4)] [maxvertexcount(32)]
void main(in point const uint input[1] : INDEX, in const uint i : SV_GSInstanceID, inout TriangleStream<GOut> output) {
   const GUI_EL_DYN curElement = element[input[0]];
   const uint type = (curElement.ind_type >> 24u) & 0x0Fu;

   // Type==void
   if(type == aet_void) return;
   
   const float2 invScale = 1.0f / guiScale;
   const uint   curBits  = curElement.seo_bits >> 16u;

   // Exit if element not visible
   if((curBits & 0x080u) || curElement.size.x == 0.0f || curElement.size.y == 0.0f) return;

   // Calculate parent rotation, scaling and translation factors; 3 deep
   float4 parScaling     = 1.0f; // .xy==Scale, .zw==Scaled translation
   float2 parTranslation = 0.0f;

   // Does the element have a parent?
   [branch] if(curElement.pei != input[0]) {
      const GUI_EL_DYN curParent = element[curElement.pei];

      parScaling.zw = curParent.coords[0].zw;

      // 'Scale' bits test
           if((curBits & 0x0C00u) == 0x0400u) parScaling.xy = curParent.coords[0].zz;
      else if((curBits & 0x0C00u) == 0x0800u) parScaling.xy = curParent.coords[0].ww;
      else if((curBits & 0x0C00u) == 0x0C00u) parScaling.xy = curParent.coords[0].zw;

      // 'Translate' bit test
      [branch] if(curBits & 0x0200u) {
         parTranslation = curParent.coords[0].xy;

         // Justification test
         if(curParent.seo_bits & 0x030000u) parTranslation.x -= invScale.x - curParent.size.x; // Origin relative to left/right of viewport
         if(curParent.seo_bits & 0x0C0000u) parTranslation.y -= invScale.y - curParent.size.y; // Origin relative to top/bottom of viewport
         if(curParent.seo_bits & 0x020000u) parTranslation.x *= -1.0f;                         // Flip X coordinate relative to right of viewport
         if(curParent.seo_bits & 0x040000u) parTranslation.y *= -1.0f;                         // Flip Y coordinate relative to top of viewport

         // Does the parent have a parent?
         if(curParent.pei != curElement.pei) {
            const GUI_EL_DYN curParent2  = element[curParent.pei];
            const float2     tempScaling = parScaling.xy;

            parScaling.zw *= curParent2.coords[0].zw;

            // 'Scale' bits test
                 if((curParent.seo_bits & 0x0C000000u) == 0x04000000u) parScaling.xy *= curParent2.coords[0].zz;
            else if((curParent.seo_bits & 0x0C000000u) == 0x08000000u) parScaling.xy *= curParent2.coords[0].ww;
            else if((curParent.seo_bits & 0x0C000000u) == 0x0C000000u) parScaling.xy *= curParent2.coords[0].zw;

            // 'Translate' bit test
            if(curParent.seo_bits & 0x02000000u) {
               parTranslation += curParent2.coords[0].xy;

               // Justification test
               if(curParent2.seo_bits & 0x030000u) parTranslation.x -= invScale.x - curParent2.size.x; // Origin relative to left/right of viewport
               if(curParent2.seo_bits & 0x0C0000u) parTranslation.y -= invScale.y - curParent2.size.y; // Origin relative to top/bottom of viewport
               if(curParent2.seo_bits & 0x020000u) parTranslation.x *= -1.0f;                          // Flip X coordinate relative to right of viewport
               if(curParent2.seo_bits & 0x040000u) parTranslation.y *= -1.0f;                          // Flip Y coordinate relative to top of viewport

               // Does the grandparent have a parent?
               if(curParent2.pei != curParent.pei) {
                  const GUI_EL_DYN curParent3   = element[curParent2.pei];
                  const float2     tempScaling2 = parScaling.xy;

                  parScaling.zw *= curParent3.coords[0].zw;

                  // 'Scale' bits test
                       if((curParent2.seo_bits & 0x0C000000u) == 0x04000000u) parScaling.xy *= curParent3.coords[0].zz;
                  else if((curParent2.seo_bits & 0x0C000000u) == 0x08000000u) parScaling.xy *= curParent3.coords[0].ww;
                  else if((curParent2.seo_bits & 0x0C000000u) == 0x0C000000u) parScaling.xy *= curParent3.coords[0].zw;

                  // 'Translate' bit test
                  if(curParent.seo_bits & 0x02000000u) {
                     parTranslation += curParent3.coords[0].xy;

                     // Justification test
                     if(curParent3.seo_bits & 0x030000u) parTranslation.x -= invScale.x - curParent3.size.x; // Origin relative to left/right of viewport
                     if(curParent3.seo_bits & 0x0C0000u) parTranslation.y -= invScale.y - curParent3.size.y; // Origin relative to top/bottom of viewport
                     if(curParent3.seo_bits & 0x020000u) parTranslation.x *= -1.0f;                          // Flip X coordinate relative to right of viewport
                     if(curParent3.seo_bits & 0x040000u) parTranslation.y *= -1.0f;                          // Flip Y coordinate relative to top of viewport
                  } else parScaling.xy = tempScaling2;
               }
            } else parScaling.xy = tempScaling;
         }
      } else parScaling.xy = 1.0f;
   }

   const uint ai = (curElement.ind_type >> 16u) & 0x0FFu;
   
   float2 coords = 0.0f;
   float  border = ((curElement.pei != input[0]) && (curBits & 0x0200u) ? invScale.y : invScale.x);

   // Type!=text(0)/textArray(1)
   [branch] if(type > aet_textArray) {
      const float4 size   = float4(curElement.size * parScaling.xy, curElement.size * parScaling.xy / parScaling.zw);
      const float4 col[4] = { float4((uint4(curElement.tint[0].xx, curElement.tint[0].yy) >> uint4(0, 16u, 0, 16u)) & 0x0FFFFu) * rcp21845,
                              float4((uint4(curElement.tint[0].zz, curElement.tint[0].ww) >> uint4(0, 16u, 0, 16u)) & 0x0FFFFu) * rcp21845,
                              float4((uint4(curElement.tint[1].xx, curElement.tint[1].yy) >> uint4(0, 16u, 0, 16u)) & 0x0FFFFu) * rcp21845,
                              float4((uint4(curElement.tint[1].zz, curElement.tint[1].ww) >> uint4(0, 16u, 0, 16u)) & 0x0FFFFu) * rcp21845 };
      GOut vert[3];

      coords = curElement.coords[0].xy;

      // Justification
      [flatten] if((type == aet_scalar) || (type == aet_cursor)) { // Type==Scalar or Cursor
         [flatten] if(curBits & 0x01u) coords.x -= size.z; // Align origin to left
         [flatten] if(curBits & 0x02u) coords.x += size.z; // Align origin to right
         [flatten] if(curBits & 0x04u) coords.y += size.w; // Align origin to top
         [flatten] if(curBits & 0x08u) coords.y -= size.w; // Align origin to bottom
      } else {
         [flatten] if(curBits & 0x03u) { // Origin relative to left/right of viewport
            coords.x += size.z - border;
            [flatten] if(curBits & 0x02u) coords.x *= -1.0f; // Origin relative to right of viewport
         }
         [flatten] if(curBits & 0x0Cu) { // Origin relative to right of viewport
            coords.y += size.w - invScale.y;
            [flatten] if(curBits & 0x04u) coords.y *= -1.0f; // Origin relative to right of viewport
         }
      }

      const float4 rotVals = { coords.xy, sin(curElement.rot), cos(curElement.rot)};

      // 'Rotate' && 'translate' bits test: Transrotate around parent's center
      if((curBits & 0x0300u) == 0x0300u)
         coords = float2((rotVals.x * rotVals.w) - (rotVals.y * rotVals.z), (rotVals.x * rotVals.z) + (rotVals.y * rotVals.w));

      // If parented, scale & translate coordinates to parent space
      if(curElement.pei != input[0]) {
         coords *= parScaling.zw;
         coords += parTranslation;
      }

      // Calculate vertices
      vert[0].ai.x = vert[1].ai.x = vert[2].ai.x = ai | (curElement.ind_type & 0x0FF00u) | (f32tof16(curElement.width_at0 * (curElement.size.y / curElement.size.x)) << 16u);
      vert[0].ai.y = vert[1].ai.y = vert[2].ai.y = f32tof16(asfloat(curElement.taos_at1)) | (f32tof16(curElement.width_at0) << 16u);

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
      if(curBits & 0x0100u) {
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
   // Type==text(0)/textArray(1)
   } else {
      const uint   i_div2      = i >> 1u;
      const uint   taOS        = (curElement.taos_at1 & 0x03FFFFFFu) + (i_div2);
      const uint   alphaOS     = curElement.ind_type & 0x0FFFFu;
      const uint   arrayStep   = (i & 0x01u) << 1u;
      const uint2  char16Index = uint2(char16[taOS][arrayStep], char16[taOS][arrayStep + 1u]);
      const uint4  chars[2]    = { ((char16Index.xxxx >> shift8888) & 0x0FFu) + alphaOS, ((char16Index.yyyy >> shift8888) & 0x0FFu) + alphaOS };
      const float4 colour      = float4((uint4(curElement.tint[i_div2][arrayStep].xx, curElement.tint[i_div2][arrayStep + 1u].xx) >> uint4(0, 16u, 0, 16u)) & 0x0FFFFu) * rcp21845;

      float4 vertPos[2];

      // Exit if no character(s) or invisible
      if(chars[0].x == alphaOS || colour.a < 0.001f) return;

      // Calculate total string width and character count
      const uint firstVertex = ((curElement.seo_bits & 0x08000u) ? input[0] : input[0] - (curElement.seo_bits & 0x07FFFu));
      const uint lastVertex  = firstVertex + (element[firstVertex].seo_bits & 0x07FFFu);

      float totalWidth = element[firstVertex].width_at0;
      uint  totalChars = element[firstVertex].taos_at1 >> 26u;

      for(uint vi = firstVertex + 1; vi < lastVertex; ++vi) {
         totalWidth += element[vi].width_at0;
         totalChars += element[vi].taos_at1 >> 26u;
      }

      totalWidth *= 0.5f;

      // 'Compress' bit test
      if((curBits & 0x02000u) && (totalWidth > 1.0f)) {
         parScaling.xz /= totalWidth;
         border        *= totalWidth;
      }

      // Start point --- !!! Move character size & dimension transformations to inner loop? !!!
      const float4 size     = float4(curElement.size * parScaling.xy, parScaling.xy / parScaling.zw);
      const float2 offset   = (1.0f - float2(uint2(alphabet[chars[0][0]].size & 0x0FFFFu, alphabet[chars[0][0]].size >> 16u)) * rcp65535) * curElement.size * 0.5f;
      const float  adjWidth = (totalChars > 32u ? totalWidth : curElement.width_at0);

      // Adjust to scroll position if textArray
      if(type == aet_textArray) coords.y -= element[curElement.pei + 2u].rot;
      
      // Justification
      const bool fillView = (curBits & 0x02000u) && ((totalWidth > 1.0f) || (totalChars > 32u));
      [flatten] if(curBits & 0x03u || fillView) {  // Origin relative to left/right of viewport, and wider than viewport
         coords.x = adjWidth * size.z * 0.5f - border;
         [flatten] if((curBits & 0x02u) && !fillView) // Origin relative to right of viewport, and not wider than viewport
            coords.x *= -1.0f;
      }
      [flatten] if(curBits & 0x0Cu) {  // Origin relative to top/bottom of viewport
         coords.y += curElement.size.y * size.w * 0.5f - invScale.y;
         [flatten] if(curBits & 0x04u) // Origin relative to top of viewport
            coords.y *= -1.0f;
      }

      coords += element[input[0]].coords[0].xy * size.zw;
      float2 coords2 = ((i & 0x01u) ? element[input[0]].coords[i_div2].z : element[input[0]].coords[i_div2].x) - element[input[0]].coords[0].x;

//      const float4 rotVals = { coords, sin(curElement.rot), cos(curElement.rot) };
      const float4 rotVals = { coords, (type == aet_textArray ? float2(0.0f, 1.0f) : float2(sin(curElement.rot), cos(curElement.rot))) };

      // 'Rotate' && 'translate' bits test: Transrotate around parent's center
      if((curBits & 0x0300u) == 0x0300u) {
         coords.x = (rotVals.x * rotVals.w) - (rotVals.y * rotVals.z) * size.z;
         coords.y = (rotVals.x * rotVals.z) + (rotVals.y * rotVals.w) * size.w;
      }

      // Width offset
      vertPos[0].x = (adjWidth * 0.5f - coords2.x + offset.x) * size.z;
      // Second justification test for height offset
           if(curBits & 0x04u) vertPos[0].y = (curElement.size.y * 0.5f - offset.y) * size.w; // Origin relative to top of viewport
      else if(curBits & 0x08u) vertPos[0].y = (curElement.size.y * 0.5f + offset.y) * size.w; // Origin relative to bottom of viewport
      else                     vertPos[0].y = (curElement.size.y * 0.5f) * size.w;
      // 'Rotate' bit test: rotate anchor vertex around element's center
      if((curBits & 0x0100u) == 0x0100u) {
         coords.x -= (vertPos[0].x * rotVals.w) - (vertPos[0].y * rotVals.z);
         coords.y -= (vertPos[0].x * rotVals.z) + (vertPos[0].y * rotVals.w);
      } else
         coords -= vertPos[0].xy;

      // If parented, scale & translate coordinates to parent space
      if(curElement.pei != input[0]) {
         coords *= parScaling.zw;
         coords += parTranslation;
      }

      // Generate vertices
      [unroll] for(uint quad = 0; quad < 2u; quad++)
         [unroll] for(uint index = 0; index < 4u; index++) {
            const uint curChar = chars[quad][index];

            // Exit if null character
            if(curChar == alphaOS) return;

            // 'Truncate' bit tests
            const float2 limit = parScaling.zw * (offset.xy + 1.00001f.xx);
            if(curBits & 0x01000u) {
               if(!(curBits & 0x02000u) && ((coords.x + size.x > limit.x) || (coords.x < -limit.x))) return;
               if((coords.y + size.y > limit.y) || (coords.y < -limit.y)) return;
            }

            const float2 os    = float2(uint2(alphabet[curChar].os & 0x0FFFFu, alphabet[curChar].os >> 16u)) * 0.0000305180437933928435187304493782f - 1.0f;
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
            const float4 tc   = float4((uint4(chTC.xx, chTC.yy) >> uint4(0, 16u, 0, 16u)) & 0x0FFFFu) * rcp32768;

            const GOut verts[4] = { float4(vertPos[0].xy, 0.0f, 1.0f), colour, tc.xw, ai, 2u, float4(vertPos[0].zw, 0.0f, 1.0f), colour, tc.xy, ai, 2u,
                                    float4(vertPos[1].xy, 0.0f, 1.0f), colour, tc.zw, ai, 2u, float4(vertPos[1].zw, 0.0f, 1.0f), colour, tc.zy, ai, 2u };

            output.Append(verts[0]);
            output.Append(verts[1]);
            output.Append(verts[2]);
            output.Append(verts[3]);
            output.RestartStrip();

            const float adjSize = float(alphabet[curChar].size & 0x0FFFFu) * size.x * rcp65535;
            coords += adjSize.xx * rotVals.wz;
         }
   }
}
