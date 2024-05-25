/************************************************************
 * File: GUI structures.h               Created: 2023/01/26 *
 *                                Last modified: 2024/05/23 *
 *                                                          *
 * Desc:                                                    *
 *                                                          *
 * 2024/04/18: Added GUI_DESC struct                        *
 *                                                          *
 *  Copyright (c) David William Bull. All rights reserved.  *
 ************************************************************/
#pragma once

#include "typedefs.h"
#include "Vector structures.h"

// .align macros
#define UI_ALIGN_C 0x0
#define UI_ALIGN_L 0x01u
#define UI_ALIGN_R 0x02u
#define UI_ALIGN_T 0x04u
#define UI_ALIGN_B 0x08u
#define UI_ELLIPSE 0x010u
#define UI_INVIS   0x080u

// .mods macros
#define UI_NONE    0x0
#define UI_ROT     0x01u
#define UI_TRANS   0x02u
#define UI_SCALE_X 0x04u
#define UI_SCALE_Y 0x08u
#define UI_SCALE   0x0Cu
#define UI_TRUNC   0x010u
#define UI_COMP    0x020u
#define UI_CTRL_CH 0x040u
#define UI_WIDE_CH 0x080u
// Defaults for .mods
#define UI_CURSOR  0x03u
#define UI_PANEL   0x0Fu
#define UI_TEXT    0x047u

al8 struct GUI_SPRITE { // 32 bytes
   wchptr name;
   VEC4Df tc;   // Texture coordinates
   VEC2Df aa;   // Active area %
};

al16 struct GUI_SPRITE_LIB { // 16 bytes
   GUI_SPRITE *sprite;
   ui16        atlasIndex;
   ui16        numSprites;
   ui16        maxSprites;
   ui16        RES;
};

al16 struct CHAR_IMM { // 16 bytes
   f1p15x4 tc; // Texture coordinates : 1p15
   union {      // Size : p16n0.0~1.0
      struct {
         fp16n0_1 width;
         fp16n0_1 height;
      };
      fp16n0_1   s[2];
      fp16n0_1x2 size;
   };
   union {      // Offsets : p16n-1.0~1.0
      struct {
         fp16n_1_1 xos;
         fp16n_1_1 yos;
      };
      fp16n_1_1   os[2];
      fp16n_1_1x2 offset;
   };
};

al16 struct ALPHABET { // 32 bytes
   wchptr stLanguage;
   wchptr stAtlasFilename; // Filename of atlas texture
   ui32   pIMMos;          // Offset into array for GPU's shaders
   ui16   numChars;
   ui16   atlasIndex;      // Index of atlas texture
   ui64   RES;
};

al32 struct GUI_EL_DGS { // 96 bytes (24 scalars)   ///--- Rewrite to remove redundants ---///
   union {
      VEC2Df origin[4];    // Position per 8 characters in parent/view space
      struct {
         VEC2Df viewPos;   // Position in parent/view space
         VEC2Df indent;    // Bounding space for children : %
         VEC4Df texCoords; // Texture coordinates
      };
      AVX8Df32 coords;
   };
   fp16n0_3x16 colour;      // Panel corners / Every 8 characters : p16n0.0~3.0
   VEC2Df      size;        // Text/Element scale : % of viewspace
   fl32        rotAngle;    // Orientation
   ui32        parentIndex; // GUI_EL_DGS index of parent element; 0x0FFFFFFFF==No parent
   union {
      struct { // Type==Text
         fl32 width;         // Total width of vertex's text in view space
         ui32 textArrayOS;   // 0~25==Offset into text array (div.by 16), 28~31==Vertex's char count
         ui16 alphabetIndex; // Offset into alphabet buffer
         ui8  atlasIndex;    // Runtime index of atlas texture
         ui8  elementType;   // Text=0, Panel=1, Button=2, Toggle=3, Scalar=4, Cursor=5, Dial=6
      };
      struct { // Type==Scalar|Cursor
         VEC2Df animTime; // Durations of animation cycles
         ui16   RES16;
         ui8    animType; // 0==Fade, 1==Zoom, 2==???, 3==Rotate, 4&5==Swing X&Y, 6~7==(Quantum=0, Linear=1, Smoothstep=2, ???=3)
         ui8    RES_eT;
      };
   };
   ui16 sibling; // 0~14==First sibling element offset / Sibling count (if bit15 set)
   union {
      ui16 bitField;
      struct {
         ui8 align; // 0~3==Justification (L,R,T,B), 4==Elliptical bounding space, 5~6==???, 7==Invisible
         ui8 mods;  // 0==Rotate, 1==Translate, 2&3==Scale X&Y, Text=(4==Truncate, 5==Compress, 6==Process control characters, 7==Wide chars)
      };
   };
};

al32 struct GUI_ELEMENT { // 128 bytes
   union {
      struct { // View space coordinates
         SSE4Df32 activeCoords; // Absolute coordinates of outer border
         VEC2Df   viewScale;    // Coordinate multipliers to convert view space to panel space
         VEC2Df   activePos;    // Absolute center coordinate of interactable area : p16n0.0~1.0
      };
      AVX8Df32 coords;
   };
   union {
      struct {
         funcptr hover[2]; // Functions for onHover & offHover
         union {
            funcptr activate[2][2]; // Functions for onActivate & offActivate via buttons 0 & 1
            struct {
               funcptr activate0[2];
               funcptr activate1[2];
            };
         };
      } func;
      funcptr function[6];
   };
   si32 charCount;      // Current number of characters in buffer
   si32 vertexIndex;    // Geometry data index in GUI vertex array
   ui16 vertexCount[2]; // Number of vertices used; 0==Current, 1==Maximum
   si16 soundBankIndex;
   union {
      ui16 bitField;
      struct {
         ui8 elementType; // 0~3==(Text=0, Panel=1, Button=2, Scalar=3, Input=4, Cursor=5, Dial=6), 4~5==???, 6==Wide characters, 7==Element actived
         ui8 stateBits;   // 8~13==Function busy bits, 14==Visible, 15==Active
      };
   };
   union { // Scratch space
      ui128 ui128Var;
      si128 si128Var;
      ptr   ptrVar[2];
      fl64  fl64Var[2];
      ui64  ui64Var[2];
      si64  si64Var[2];
      fl32  fl32Var[4];
      ui32  ui32Var[4];
      si32  si32Var[4];
      ui16  ui16Var[8];
      si16  si16Var[8];
      ui8   ui8Var[16];
      si8   si8Var[16];
   };
};

al32 struct GUI_INTERFACE { // 64 bytes
   ui16 *vertex;       // Vertex array of indices to element entries
   si16  maxVertices;
   si16  vertCount;
   si16  maxInputs;
   si16  inputCount;
   union {             // Array of input combinations
      VEC4Du8 *inputs;
      ui8    (*input)[4];
   };
   union {             // Array of input text labels
      chptr *inputLabels;
      char (*inputLabel)[32];
   };
   union {             // 1-bit element activity array, ~256 vertices
      ui64  mod[4];
      ui128 mod128[2];
      ui256 mod256;
   };
};

// Input data for element creation
al32 struct GUI_EL_DESC { // 160 bytes
   union {
      struct {
         funcptr hover[2];          // Functions for onHover & offHover
         union {
            funcptr activate[2][2]; // Functions for onActivate & offActivate via buttons 0 & 1
            struct {
               funcptr activate0[2];
               funcptr activate1[2];
            };
         };
      } func;
      funcptr function[6] = {};
   };
   union {
      chptr  textPtr = NULL;
      wchptr wtextPtr;
   };
   VEC2Df viewPos = { 0.0f, 0.0f };
   struct {
      SSE4Df32 panel  = { 1.0f, 1.0f, 1.0f, 1.0f };
      SSE4Df32 cursor = { 1.0f, 1.0f, 1.0f, 1.0f };
      SSE4Df32 text   = { 1.0f, 1.0f, 1.0f, 1.0f };
   } tint;
   struct {
      VEC2Df panel  = { 1.0f, 1.0f };
      VEC2Df cursor = { 1.0f, 1.0f };
      VEC2Df text   = { 1.0f, 1.0f };
   } size;
   struct {
      si16 soundBank    = -1;
      si16 alphabet     = -1;
      ui16 spriteLib    = 0x0FFFF;
      ui16 panelSprite  = 0x0FFFF;
      ui16 cursorSprite = 0x0FFFF;
   } index;
   union {
      struct {
         struct BITFIELD {
            ui8 align;
            ui8 mods;
         } panel;
         BITFIELD cursor;
         BITFIELD text;
      };
      struct {
         ui16 panelBits;
         ui16 cursorBits;
         ui16 textBits;
      };
   };
   si32 charCount = 0;
   ui32 RES       = 0x0CDCDCDCD;

   GUI_EL_DESC(void) {
      textBits   = UI_TEXT;
      panelBits  = UI_PANEL;
      cursorBits = UI_CURSOR;
   }
};

#define TriggerInputProcessing gcv.misc[7] |= 0x080

// Passing-in true when decalring will set it as globally accessible.
// Set .interfaceIndex before executing "gcv.misc[7] |= 0x080;"
al32 struct GUI_DESC {
   ui32 interfaceIndex   = 0;    // Currently active interface
   ui32 defaultInterface = 0;    // Default user interface index
   ui32 prevInterface    = 0;    // User interface transitioned from
   ui32 nextInterface    = 0;    // User interface to transition to
   fl32 transitionTime   = 0.0f; // Period of time (in seconds) to transition between user interfaces
   fl32 elapsedTime      = 0.0f; // Current elapsed time (in seconds) of transition
   ui16 noInputTime      = 0;    // Period of time (in milliseconds) GUI input is inactive between interface transitions
   ui8  transitionMods   = 0;    // 0==Delayed, 1==Overlaid, 2==Fade in, 3==Fade out, 4~7==???
   ui8  RES[5];

   GUI_DESC(cbool makeGlobal) { if(makeGlobal) ptrLib[15] = this; }

   // Requires GLOBALCTRLVARS 'gcv' global variable
   void ProcessInputs(void) const { gcv.misc[7] |= 0x080; }

   // If(nextInterface!=interfaceIndex ? Transition to nextInterface)
   // If(Transited ? interfaceIndex=nextInterface)
   // If(nextInterface==-1 ? Reverse transition to prevInterface)

   // Automatic management of transition states for user interfaces
   void ProcessTransition(void) {
      // Transition to next interface
      if(nextInterface != interfaceIndex) {

      // Reverse transition from current interface
      } else if(nextInterface == 0x0FFFFFFFF) {

      // Confirm new interface
      } if(elapsedTime >= transitionTime) {

      }
   }
};
