/************************************************************
 * File: gui.hlsli                      Created: 2024/05/25 *
 * Type: Header file              Last modified: 2024/05/25 *
 *                                                          *
 * Notes:                                                   *
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
   uint  size; // Relative X & Y dimensions : p16n0.0~1.0
   uint  os;   // Relative X & Y offsets : p16n-1.0~1.0
};

struct GUI_EL_DYN { // 96 bytes (6 vectors)
   float4 coords[2]; // Text:    View coordinates per 8 characters
                     // Element: View position, bounding space for children, x2 texture coordinates
   uint4  tint[2];   // Text:    RGBA values per 8 characters
                     // Element: TL/BL/TR/BR RGBA values : p16n0.0~3.0
   float2 size;      // Scaling factors
   float  rot;       // Amount of rotation
   uint   pei;       // Parent element's GUI_EL_DYN index: this==No parent
   float  width_at0; // Text:    Total width of vertex's text in view space
                     // Element: First duration of animation cycle
   uint   taos_at1;  // Text:    0~25==Offset into text array (div.by 16), 26~31==Vertex's char count
                     // Element: Second duration of anination cycle (cast from float)
   uint   ind_type;  // Text:    0~15==Offset into alphabet buffer, 16~23==Runtime index of alphabet's atlas texture, 24~31==Element type
                     // Element: 0~15==???, 16==Fade, 17==Zoom, 18==???, 19==Rotate, 20&21==Swing X&Y, 22~23==(Quantum=0, Linear=1, Smoothstep=2, ???=3), 24~31==Element type
   uint   seo_bits;  // 0~15==First sibling element offset, 16~19==Justification (L,R,T,B), 20==Elliptical bounding space, 21~22==???, 23==Invisible
};                   // 24==Rotate, 25==Translate, 26&27==Scale X&Y, 28==Truncate, 29==Compress, 30==Process control characters, 31==Wide chars

struct GOut { // 44 bytes (2 vectors + 3 scalars)
   float4 pos     : SV_Position;
   float4 col     : COLOR;
   float2 tc      : TEXCOORD;
   uint   ai_type : ATLAS;       // 0~6==Atlas index, 7~10==Type, 11~31==???
};

StructuredBuffer   <CHAR_IMM>   alphabet : register(t0); // Character geometry
StructuredBuffer   <uint4>      char16   : register(t1); // Text pool
RWStructuredBuffer <GUI_EL_DYN> element  : register(u1); // Element data
