/************************************************************
 * File: ps.gui.hlsl                    Created: 2023/04/14 *
 * Type: Pixel shader             Last modified: 2024/06/27 *
 *                                                          *
 * Desc:                                                    *
 *                                                          *
 * To do:                                                   *
 *                                                          *
 * Copyright (c) David William Bull.   All rights reserved. *
 ************************************************************/

#include "gui.hlsli"
#include "ps.full register access.hlsli"

float4 main(in const GOut input) : SV_Target {
   const uint borderStyle = (input.ai.x >> 8u) & 0x0FFu;
   [flatten] if(!borderStyle)
      return Sample1of2GUI(uint2(input.ai.x & 0x0FFu, 2u), input.tc) * input.col;
   else {
      const float2 adjTC = abs(input.tc * 2.0f.xx - 1.0f.xx);
      const float2 bs    = f16tof32(input.ai >> 16u);
      const float2 bsRCP = rcp(bs);

      const float2 basic  = 1.0f - adjTC;
      const float2 fadeIn = min(basic * bsRCP, 1.0f);
      const float2 temp5  = min(fadeIn, 1.0f - fadeIn) * 2.0f;
      const float  style5 = min(temp5.x, temp5.y);

      const float wo = f16tof32(input.ai.y);

      switch (borderStyle) {
      default: // Solid border
         return ((basic.x < bs.x) || (basic.y < bs.y)) ? 1.0f.xxxx : float4(0.0f.xxx, wo);
      case 2: // Solid window
         return ((basic.x < bs.x) || (basic.y < bs.y)) ? float4(0.0f.xxx, wo) : 1.0f.xxxx;
      case 3: // Fade to window
         const float2 temp3  = 1.0f - (fadeIn * (1.0f - wo));
         const float  style3 = max(temp3.x, temp3.y);

         return style3 > wo ? float4(1.0f.xxx, style3) : float4(0.0f.xxx, wo);
      case 4: // Fade from window
         const float style4 = min(fadeIn.x, fadeIn.y);

         return style4 < 1.0f ? float4(1.0f.xxx, style4) : float4(0.0f.xxx, wo);
      case 5: // Solid corners
         return style5 > wo ? float4(style5.xxx, 1.0f) : float4(0.0f.xxx, wo);
      case 6: // Soft corners
         return style5 > wo ? float4(1.0f.xxx, style5) : float4(0.0f.xxx, wo);
      }
   }
}
