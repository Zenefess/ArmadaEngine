/************************************************************
 * File: vs.gui.hlsl                    Created: 2023/04/09 *
 * Type: Vertex shader            Last modified: 2024/07/29 *
 *                                                          *
 * Notes: 32 characters per vertex.                         *
 *                                                          *
 * 2023/07/01: Moved input data to structured buffer        *
 * 2024/04/01: GUI_EL_DYN SRV+Adj.coords replaced by UAV    *
 *                                                          *
 * Copyright (c) David William Bull.   All rights reserved. *
 ************************************************************/

#include "gui.hlsli"

StructuredBuffer   <CHAR_IMM>   alphabet : register(t80); // Character geometry
StructuredBuffer   <uint4>      char16   : register(t81); // Text pool
RWStructuredBuffer <GUI_EL_DYN> element  : register(u1);  // Element data

uint main(in const uint index : INDEX) : INDEX {   
   // If invisible
   if(element[index].seo_bits & 0x0800000u) return index;

   // If type!=Text(0)/textArray(1)
   if(((element[index].ind_type >> 24u) & 0x0Fu) > aet_textArray) return index;

   // If (re)processing not needed
   if(element[index].width_at0 > 0.0f) return index;

   const uint  charCount  = element[index].taos_at1 >> 26u;
   const uint  alphaOS    = element[index].ind_type & 0x0FFFFu;
   const uint4 charLot[2] = { char16[element[index].taos_at1 & 0x03FFFFFFu], char16[(element[index].taos_at1 & 0x03FFFFFFu) + 1u] };
   const uint4 char4[8]   = { ((charLot[0].xxxx >> shift8888) & 0x0FFu) + alphaOS, ((charLot[0].yyyy >> shift8888) & 0x0FFu) + alphaOS,
                              ((charLot[0].zzzz >> shift8888) & 0x0FFu) + alphaOS, ((charLot[0].wwww >> shift8888) & 0x0FFu) + alphaOS,
                              ((charLot[1].xxxx >> shift8888) & 0x0FFu) + alphaOS, ((charLot[1].yyyy >> shift8888) & 0x0FFu) + alphaOS,
                              ((charLot[1].zzzz >> shift8888) & 0x0FFu) + alphaOS, ((charLot[1].wwww >> shift8888) & 0x0FFu) + alphaOS };
   uint i = 0x0FFFFFFFF, width = 0;

   // Accumulate sub-total of width
   [flatten] if(++i < charCount) width += alphabet[char4[0][0]].size & 0x0FFFFu;
   [flatten] if(++i < charCount) width += alphabet[char4[0][1]].size & 0x0FFFFu;
   [flatten] if(++i < charCount) width += alphabet[char4[0][2]].size & 0x0FFFFu;
   [flatten] if(++i < charCount) width += alphabet[char4[0][3]].size & 0x0FFFFu;
   [flatten] if(++i < charCount) width += alphabet[char4[1][0]].size & 0x0FFFFu;
   [flatten] if(++i < charCount) width += alphabet[char4[1][1]].size & 0x0FFFFu;
   [flatten] if(++i < charCount) width += alphabet[char4[1][2]].size & 0x0FFFFu;
   [flatten] if(++i < charCount) width += alphabet[char4[1][3]].size & 0x0FFFFu;
   // Set second X position if not set, and character(s) to render
   if((element[index].coords[0].z >= 16000.0f) && (i < charCount)) {
      element[index].coords[0].z = (float(width) * rcp65535) * element[index].size.x + element[index].coords[0].x;
      element[index].coords[0].w = element[index].coords[0].y;
   }
   // Accumulate sub-total of width
   [flatten] if(++i < charCount) width += alphabet[char4[2][0]].size & 0x0FFFFu;
   [flatten] if(++i < charCount) width += alphabet[char4[2][1]].size & 0x0FFFFu;
   [flatten] if(++i < charCount) width += alphabet[char4[2][2]].size & 0x0FFFFu;
   [flatten] if(++i < charCount) width += alphabet[char4[2][3]].size & 0x0FFFFu;
   [flatten] if(++i < charCount) width += alphabet[char4[3][0]].size & 0x0FFFFu;
   [flatten] if(++i < charCount) width += alphabet[char4[3][1]].size & 0x0FFFFu;
   [flatten] if(++i < charCount) width += alphabet[char4[3][2]].size & 0x0FFFFu;
   [flatten] if(++i < charCount) width += alphabet[char4[3][3]].size & 0x0FFFFu;
   // Set third X position if not set, and character(s) to render
   if((element[index].coords[1].x >= 16000.0f) && (i < charCount)) {
      element[index].coords[1].x = (float(width) * rcp65535) * element[index].size.x + element[index].coords[0].x;
      element[index].coords[1].y = element[index].coords[0].y;
   }
   // Accumulate sub-total of width
   [flatten] if(++i < charCount) width += alphabet[char4[4][0]].size & 0x0FFFFu;
   [flatten] if(++i < charCount) width += alphabet[char4[4][1]].size & 0x0FFFFu;
   [flatten] if(++i < charCount) width += alphabet[char4[4][2]].size & 0x0FFFFu;
   [flatten] if(++i < charCount) width += alphabet[char4[4][3]].size & 0x0FFFFu;
   [flatten] if(++i < charCount) width += alphabet[char4[5][0]].size & 0x0FFFFu;
   [flatten] if(++i < charCount) width += alphabet[char4[5][1]].size & 0x0FFFFu;
   [flatten] if(++i < charCount) width += alphabet[char4[5][2]].size & 0x0FFFFu;
   [flatten] if(++i < charCount) width += alphabet[char4[5][3]].size & 0x0FFFFu;
   // Set fourth X position if not set, and character(s) to render
   if((element[index].coords[1].z >= 16000.0f) && (i < charCount)) {
      element[index].coords[1].z = (float(width) * rcp65535) * element[index].size.x + element[index].coords[0].x;
      element[index].coords[1].w = element[index].coords[0].y;
   }
   // Accumulate sub-total of width
   [flatten] if(++i < charCount) width += alphabet[char4[6][0]].size & 0x0FFFFu;
   [flatten] if(++i < charCount) width += alphabet[char4[6][1]].size & 0x0FFFFu;
   [flatten] if(++i < charCount) width += alphabet[char4[6][2]].size & 0x0FFFFu;
   [flatten] if(++i < charCount) width += alphabet[char4[6][3]].size & 0x0FFFFu;
   [flatten] if(++i < charCount) width += alphabet[char4[7][0]].size & 0x0FFFFu;
   [flatten] if(++i < charCount) width += alphabet[char4[7][1]].size & 0x0FFFFu;
   [flatten] if(++i < charCount) width += alphabet[char4[7][2]].size & 0x0FFFFu;
   [flatten] if(++i < charCount) width += alphabet[char4[7][3]].size & 0x0FFFFu;

   element[index].width_at0 = (float(width) * rcp65535) * element[index].size.x;

   return index;
}
