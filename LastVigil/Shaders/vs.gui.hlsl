/************************************************************
 * File: vs.gui.hlsl                    Created: 2023/04/09 *
 *                                Last modified: 2024/04/03 *
 *                                                          *
 * Notes: 32 characters per vertex.                         *
 *                                                          *
 * 2023/07/01: Moved input data to structured buffer        *
 * 2024/04/01: GUI_EL_DYN SRV+Adj.coords replaced by UAV    *
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
   uint  size; // Relative X & Y dimensions : p16n0.0~1.0
   uint  os;   // Relative X & Y offsets : p-1.0~1.0
};

struct GUI_EL_DYN { // 96 bytes (24 scalars)
   float4 coords[2]; // Text: view coordinates per 8 characters | Element: View position, bounding space for children, x2 texture coordinates
   uint4  tint[2];   // Text: RGBA values per 8 characters | Element: TL/BL/TR/BR RGBA values : p16n0.0~2.0
   float2 size;      // Scale of text | Size of element
   float  rot;       // Amount of rotation
   float  width;     // Total width of vertex's text in view space
   uint   pei;       // Parent element's GUI_EL_DYN index: this==No parent
   uint   taos;      // 0~27==Offset into structured text buffer
   uint   ind_type;  // 0~15==Offset into alphabet buffer, 16~23==Runtime index of alphabet's atlas texture, 24~31==Element type
   uint   seo_bits;  // 0~15==First sibling element offset, 16~19==Justification (L,R,T,B), 20~23==???
};                   // 24==No rotation, 25==NoTranslation, 26&27==No scaling: X&Y, 28==Invisible, 29==Truncate, 30==Compress, 31==Wide chars

StructuredBuffer   <CHAR_IMM>   alphabet : register(t0); // Character geometry
StructuredBuffer   <uint4>      char16   : register(t1); // Text pool
RWStructuredBuffer <GUI_EL_DYN> element  : register(u1); // Element data

uint main(in const uint index : INDEX) : INDEX {   
   const GUI_EL_DYN curElement = element[index];

   // If type==Text
   if(!(curElement.ind_type & 0x0F000000)) {
      const float scale      = curElement.size.x;// * guiScale;
      const uint  alphaOS    = curElement.ind_type & 0x0FFFF;
      const uint4 charLot[2] = { char16[curElement.taos], char16[(curElement.taos) + 1] };
      const uint4 char4[8]   = { ((charLot[0].xxxx >> shift8888) & 0x0FF) + alphaOS, ((charLot[0].yyyy >> shift8888) & 0x0FF) + alphaOS,
                                 ((charLot[0].zzzz >> shift8888) & 0x0FF) + alphaOS, ((charLot[0].wwww >> shift8888) & 0x0FF) + alphaOS,
                                 ((charLot[1].xxxx >> shift8888) & 0x0FF) + alphaOS, ((charLot[1].yyyy >> shift8888) & 0x0FF) + alphaOS,
                                 ((charLot[1].zzzz >> shift8888) & 0x0FF) + alphaOS, ((charLot[1].wwww >> shift8888) & 0x0FF) + alphaOS };

      uint  i;
      // Accumulate sub-total of width
      for(i = 0, element[index].width = 0.0f; i < 4 && char4[0][i] > alphaOS; i++)
         element[index].width += float(alphabet[char4[0][i]].size & 0x0FFFF) * rcp65535;
      if(i == 4)
         for(i = 0; i < 4 && char4[1][i] > alphaOS; i++)
            element[index].width += float(alphabet[char4[1][i]].size & 0x0FFFF) * rcp65535;
      // If second position not set and character(s) to render, add sub-total
      if((curElement.coords[0].z >= 16000.0f || curElement.coords[0].w >= 16000.0f) && char4[2].x > alphaOS) {
         element[index].coords[0].z = element[index].width * scale.x + element[index].coords[0].x;
         element[index].coords[0].w = element[index].coords[0].y;
      }
      // Accumulate sub-total of width
      if(i == 4)
         for(i = 0; i < 4 && char4[2][i] > alphaOS; i++)
            element[index].width += float(alphabet[char4[2][i]].size & 0x0FFFF) * rcp65535;
      if(i == 4)
         for(i = 0; i < 4 && char4[3][i] > alphaOS; i++)
            element[index].width += float(alphabet[char4[3][i]].size & 0x0FFFF) * rcp65535;
      // If third position not set and character(s) to render, add sub-total
      if((curElement.coords[1].x >= 16000.0f || curElement.coords[1].y >= 16000.0f) && char4[4].x > alphaOS) {
         element[index].coords[1].x = element[index].width * scale.x + element[index].coords[0].x;
         element[index].coords[1].y = element[index].coords[0].y;
      }
      // Accumulate sub-total of width
      if(i == 4)
         for(i = 0; i < 4 && char4[4][i] > alphaOS; i++)
            element[index].width += float(alphabet[char4[4][i]].size & 0x0FFFF) * rcp65535;
      if(i == 4)
         for(i = 0; i < 4 && char4[5][i] > alphaOS; i++)
            element[index].width += float(alphabet[char4[5][i]].size & 0x0FFFF) * rcp65535;
      // If fourth position not set and character(s) to render, add sub-total
      if((curElement.coords[1].z >= 16000.0f || curElement.coords[1].w >= 16000.0f) && char4[6].x > alphaOS) {
         element[index].coords[1].z = element[index].width * scale.x + element[index].coords[0].x;
         element[index].coords[1].w = element[index].coords[0].y;
      }
      // Accumulate sub-total of width
      if(i == 4)
         for(i = 0; i < 4 && char4[6][i] > alphaOS; i++)
            element[index].width += float(alphabet[char4[6][i]].size & 0x0FFFF) * rcp65535;
      if(i == 4)
         for(i = 0; i < 4 && char4[7][i] > alphaOS; i++)
            element[index].width += float(alphabet[char4[7][i]].size & 0x0FFFF) * rcp65535;

      element[index].width *= scale.x;
      
      // Add else statements to set unused coords to same as 1st/2nd/3rd?

      // Replicate remaining tint colours if only first is set
//      if(!curElement.tint[0].zw && !curElement.tint[1].xy && !curElement.tint[1].zw) {
//         tint[0].zw = tint[0].xy;
//      }
   }

   return index;
}
