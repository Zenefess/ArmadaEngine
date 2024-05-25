/************************************************************
 * File: vs.gui.hlsl                    Created: 2023/04/09 *
 * Type: Vertex shader            Last modified: 2024/05/25 *
 *                                                          *
 * Notes: 32 characters per vertex.                         *
 *                                                          *
 * 2023/07/01: Moved input data to structured buffer        *
 * 2024/04/01: GUI_EL_DYN SRV+Adj.coords replaced by UAV    *
 *                                                          *
 * Copyright (c) David William Bull.   All rights reserved. *
 ************************************************************/

#include "gui.hlsli"

uint main(in const uint index : INDEX) : INDEX {   
   const GUI_EL_DYN curElement = element[index];

   // If type==Text
   if(!(curElement.ind_type & 0x0F000000)) {
      const float scale      = curElement.size.x;
      const uint  alphaOS    = curElement.ind_type & 0x0FFFF;
      const uint4 charLot[2] = { char16[curElement.taos_at1 & 0x03FFFFFF], char16[(curElement.taos_at1 & 0x03FFFFFF) + 1] };
      const uint4 char4[8]   = { ((charLot[0].xxxx >> shift8888) & 0x0FF) + alphaOS, ((charLot[0].yyyy >> shift8888) & 0x0FF) + alphaOS,
                                 ((charLot[0].zzzz >> shift8888) & 0x0FF) + alphaOS, ((charLot[0].wwww >> shift8888) & 0x0FF) + alphaOS,
                                 ((charLot[1].xxxx >> shift8888) & 0x0FF) + alphaOS, ((charLot[1].yyyy >> shift8888) & 0x0FF) + alphaOS,
                                 ((charLot[1].zzzz >> shift8888) & 0x0FF) + alphaOS, ((charLot[1].wwww >> shift8888) & 0x0FF) + alphaOS };

      uint i;
      // Accumulate sub-total of width
      for(i = 0, element[index].width_at0 = 0.0f; i < 4 && char4[0][i] > alphaOS; i++)
         element[index].width_at0 += float(alphabet[char4[0][i]].size & 0x0FFFF) * rcp65535;
      if(i == 4)
         for(i = 0; i < 4 && char4[1][i] > alphaOS; i++)
            element[index].width_at0 += float(alphabet[char4[1][i]].size & 0x0FFFF) * rcp65535;
      // If second position not set and character(s) to render, add sub-total
      if((curElement.coords[0].z >= 16000.0f || curElement.coords[0].w >= 16000.0f) && char4[2].x > alphaOS) {
         element[index].coords[0].z = element[index].width_at0 * scale.x + element[index].coords[0].x;
         element[index].coords[0].w = element[index].coords[0].y;
      }
      // Accumulate sub-total of width
      if(i == 4)
         for(i = 0; i < 4 && char4[2][i] > alphaOS; i++)
            element[index].width_at0 += float(alphabet[char4[2][i]].size & 0x0FFFF) * rcp65535;
      if(i == 4)
         for(i = 0; i < 4 && char4[3][i] > alphaOS; i++)
            element[index].width_at0 += float(alphabet[char4[3][i]].size & 0x0FFFF) * rcp65535;
      // If third position not set and character(s) to render, add sub-total
      if((curElement.coords[1].x >= 16000.0f || curElement.coords[1].y >= 16000.0f) && char4[4].x > alphaOS) {
         element[index].coords[1].x = element[index].width_at0 * scale.x + element[index].coords[0].x;
         element[index].coords[1].y = element[index].coords[0].y;
      }
      // Accumulate sub-total of width
      if(i == 4)
         for(i = 0; i < 4 && char4[4][i] > alphaOS; i++)
            element[index].width_at0 += float(alphabet[char4[4][i]].size & 0x0FFFF) * rcp65535;
      if(i == 4)
         for(i = 0; i < 4 && char4[5][i] > alphaOS; i++)
            element[index].width_at0 += float(alphabet[char4[5][i]].size & 0x0FFFF) * rcp65535;
      // If fourth position not set and character(s) to render, add sub-total
      if((curElement.coords[1].z >= 16000.0f || curElement.coords[1].w >= 16000.0f) && char4[6].x > alphaOS) {
         element[index].coords[1].z = element[index].width_at0 * scale.x + element[index].coords[0].x;
         element[index].coords[1].w = element[index].coords[0].y;
      }
      // Accumulate sub-total of width
      if(i == 4)
         for(i = 0; i < 4 && char4[6][i] > alphaOS; i++)
            element[index].width_at0 += float(alphabet[char4[6][i]].size & 0x0FFFF) * rcp65535;
      if(i == 4)
         for(i = 0; i < 4 && char4[7][i] > alphaOS; i++)
            element[index].width_at0 += float(alphabet[char4[7][i]].size & 0x0FFFF) * rcp65535;

      element[index].width_at0 *= scale.x;
      
      // Add else statements to set unused coords to same as 1st/2nd/3rd?

      // Replicate remaining tint colours if only first is set
//      if(!curElement.tint[0].zw && !curElement.tint[1].xy && !curElement.tint[1].zw) {
//         tint[0].zw = tint[0].xy;
//      }
   }

   return index;
}
