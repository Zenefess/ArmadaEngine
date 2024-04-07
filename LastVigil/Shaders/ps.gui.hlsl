/************************************************************
 * File: ps.gui.hlsl                    Created: 2023/04/14 *
 *                                Last modified: 2023/04/14 *
 *                                                          *
 * Desc: 8 characters per instance.                         *
 * To do:                                                   *
 *                                                          *
 *  Copyright (c) David William Bull. All rights reserved.  *
 ************************************************************/

#include "common.hlsli"
#include "ps.full register access.hlsli"

struct GOut { // 44 bytes (2 vectors + 3 scalars)
   float4 pos     : SV_Position;
   float4 col     : COLOR;
   float2 tc      : TEXCOORD;
   uint   ai_type : ATLAS;       // 0~6==Atlas index, 7~10==Type, 11~31==???
};

float4 main(in const GOut input) : SV_Target {
   const float4 sample = Sample1of2GUILinear(input.ai_type & 0x07F, input.tc) * input.col;

   clip(sample.a - 0.011f); // Alpha test

   return sample;
//   return sample + float4(0.025f, 0.025f, 0.025f, 0.5f);
}
