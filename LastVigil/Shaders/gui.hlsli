/************************************************************
 * File: gui.hlsli                      Created: 2024/05/25 *
 * Type: Header file              Last modified: 2024/07/29 *
 *                                                          *
 * Notes:                                                   *
 *                                                          *
 * Copyright (c) David William Bull.   All rights reserved. *
 ************************************************************/

#include "common.hlsli"

#define aet_text           0u
#define aet_textArray      1u
#define aet_panel          2u
#define aet_textPanel      3u
#define aet_textArrayPanel 4u
#define aet_inputPanel     5u
#define aet_dropPanel      6u
#define aet_button         7u
#define aet_textButton     8u
#define aet_toggle         9u
#define aet_scalar         10u
#define aet_cursor         11u
#define aet_dial           12u
#define aet_void           15u

cbuffer CB_VIEW : register(b1) { // 192 bytes (12 vectors)
   const matrix projection;   // Perspective transformation
   const matrix orthographic; // Orthographic transformation
   const float2 guiScale;     // Final X & Y scaling factors for GUI
   const uint2  bitField;     // 0-63==???
//   const uint4  RES;
};

struct CHAR_IMM { // 16 bytes (1 vector)
   uint2 tc;   // Texture coordinates : 4x(1p15)
   uint  size; // Relative X & Y dimensions : p16n0.0~1.0
   uint  os;   // Relative X & Y offsets : p16n-1.0~1.0
};

struct GUI_EL_DYN { // 96 bytes (6 vectors)
   float4 coords[2]; // Text:    View coordinates per 8 characters
                     // Element: View position, bounding space for children, 2 texture coordinates
   uint4  tint[2];   // Text:    RGBA values per 8 characters
                     // Element: TL/BL/TR/BR RGBA values : p16n0.0~3.0
   float2 size;      // Both:    Scaling factors
   float  rot;       // Both:    Amount of rotation
   uint   pei;       // Both:    Parent element's GUI_EL_DYN index: this==No parent
   float  width_at0; // Text:    Total width of vertex's text in view space
                     // Element: First duration of animation cycle
   uint   taos_at1;  // Text:    0~25==Offset into text array (div.by 16), 26~31==Vertex's char count
                     // Element: Second duration of anination cycle (cast from float)
   uint   ind_type;  // Text:    0~15==Offset into alphabet buffer
                     // Element: 0==Fade, 1==Zoom, 2==???, 3==Rotate, 4&5==Swing X&Y, 6~7==(Quantum=0, Linear=1, Smoothstep=2, ???=3), 8~15==???
                     // Both:    16~23==Runtime index of atlas texture, 24~31==Element type
   uint   seo_bits;  // 0~15==First sibling element offset, 16~19==Justification (L,R,T,B), 20==Elliptical bounding space, 21~22==???, 23==Invisible
};                   // 24==Rotate, 25==Translate, 26&27==Scale X&Y, 28==Truncate, 29==Compress, 30==Process control characters, 31==Wide chars

struct GOut { // 48 bytes (3 vectors)
   float4 pos : SV_Position;
   float4 col : COLOR;
   float2 tc  : TEXCOORD;
   uint2  ai  : ATLAS;       // X: 0~7==Atlas index, 8~31==???, Y: 0~15==Border size, 16~31==Window transparency : 2 float16s
};
