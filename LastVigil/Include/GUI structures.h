/************************************************************
 * File: GUI structures.h               Created: 2023/01/26 *
 *                                Last modified: 2024/06/24 *
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
constexpr ui8 UI_ALIGN_C  = 0x0;
constexpr ui8 UI_ALIGN_L  = 0x01u;
constexpr ui8 UI_ALIGN_R  = 0x02u;
constexpr ui8 UI_ALIGN_T  = 0x04u;
constexpr ui8 UI_ALIGN_TL = 0x05u;
constexpr ui8 UI_ALIGN_TR = 0x06u;
constexpr ui8 UI_ALIGN_B  = 0x08u;
constexpr ui8 UI_ALIGN_BL = 0x09u;
constexpr ui8 UI_ALIGN_BR = 0x0Au;
constexpr ui8 UI_ELLIPSE  = 0x010u;
constexpr ui8 UI_INVIS    = 0x080u;

// .mods macros
constexpr ui8 UI_NONE    = 0x0;
constexpr ui8 UI_ROT     = 0x01u;
constexpr ui8 UI_TRANS   = 0x02u;
constexpr ui8 UI_SCALE_X = 0x04u;
constexpr ui8 UI_SCALE_Y = 0x08u;
constexpr ui8 UI_SCALE   = 0x0Cu;
constexpr ui8 UI_TRUNC   = 0x010u;
constexpr ui8 UI_COMP    = 0x020u;
constexpr ui8 UI_CTRL_CH = 0x040u;
constexpr ui8 UI_WIDE_CH = 0x080u;
// Defaults for .mods
constexpr ui8 UI_CURSOR    = 0x03u;
constexpr ui8 UI_PANEL     = 0x0Fu;
constexpr ui8 UI_TEXTARRAY = 0x036u;
constexpr ui8 UI_TEXT      = 0x047u;
constexpr ui8 UI_INPUT     = 0x026u;

enum AE_ELEMENT_TYPE : ui8 { aet_text, aet_textArray, aet_panel, aet_button, aet_toggle, aet_scalar, aet_cursor, aet_dial, aet_8, aet_9, aet_10, aet_11, aet_12, aet_13, aet_14, aet_void };
enum AE_REGEN_VERTS  : ui8 { aev_minimum, aev_maximum };

union GUI_INDICES {
   cptrc data;
   struct {
      si32 vertex;
      si32 element;
   };
};

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
   fp16n0_3x16 colour;      // Panel corners / Every 8 characters
   VEC2Df      size;        // Text/Element scale : % of viewspace
   fl32        rotAngle;    // Type!=TextArray: Orientation
   ui32        parentIndex; // GUI_EL_DGS index of parent element; this==No parent
   union {
      struct { // Type==Text|TextArray
         fl32 width;         // Total width of vertex's text in view space
         ui32 charBankOS;    // 0~25==Offset into character bank (div.by 16), 26~31==Vertex's char count
         ui16 alphabetIndex; // Offset into alphabet buffer
         ui8  atlasIndex;    // Runtime index of atlas texture
         ui8  elementType;   // Text=0, Text Array=1, Panel=2, Button=3, Toggle=4, Scalar=5, Cursor=6, Dial=7
                             // 4~5==???, 6==Don't (re)calculate text offsets, 7==Button "down"
      };
      struct { // Type==Scalar|Cursor
         VEC2Df animTime; // Durations of animation cycles
         ui8    animType; // 0==Fade, 1==Zoom, 2==???, 3==Rotate, 4&5==Swing X&Y, 6~7==(Quantum=0, Linear=1, Smoothstep=2, ???=3)
         ui8    RES8[3];
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

al32 struct GUI_ELEMENT { // 96 bytes
   union {
      struct { // View space coordinates
         SSE4Df32 activeCoords; // Absolute coordinates of outer border
         VEC2Df   viewScale;    // Coordinate multipliers to convert view space to panel space
         VEC2Df   activePos;    // Absolute center coordinate of interactable area
      };
      AVX8Df32 coords;
   };
   union {
      ui32ptr     textArray;    // List of each text string's first vertex
      TEXTBUFFER *input;        // Buffer for text input management
      bool        toggleState;  // State of toggle element
      si32        scalarPos;    // Position of 1D scalar; integer
      fl32        fScalarPos;   // Position of 1D scalar; floating-point
      VEC2Ds32    scalarPos2D;  // Position of 2D scalar; integer
      VEC2Df      fScalarPos2D; // Position of 2D scalar; floating-point
   };
   union {
      si32 charCount; // Current number of characters in buffer
      struct {
         si16 textCount; // Total number of entries in .textArray
         si16 textEntry; // Currently active entry
      };
   };
   si32 vertexIndex;    // Geometry data index in GUI vertex array
   ui16 vertexCount[2]; // Number of vertices used; 0==Current, 1==Maximum
   si16 soundBankIndex;
   union {
      ui16 bitField;
      struct {
         ui8 elementType; // 0~3==(Text=0, Text Array=1, Panel=2, Button=3, Toggle=4, Scalar=5, Cursor=6, Dial=7), 4~5==???, 6==Wide characters, 7==Element activated
         ui8 stateBits;   // 8~13==Function busy bits, 14==Visible, 15==Active
      };
   };
   union {
      struct {
         ui32 hover[2]; // Function indices for onHover & offHover
         union {
            ui32 activate[2][2]; // Function indices for onActivate & offActivate via buttons 0 & 1
            struct {
               ui32 activate0[2];
               ui32 activate1[2];
            };
         };
      } func;
      ui32 function[6];
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
   ui32ptr vertex;      // Vertex array of indices to element entries
   ui16    maxVertices; // Maximum element entries
   ui16    vertCount;   // Current element entries
   ui16    maxInputs;   // Maxmimum input bit patterns
   ui16    inputCount;  // Current input bit patterns
   union {              // Array of input combinations
      VEC4Du8 *inputs;
      ui8    (*input)[4];
   };
   union {              // Array of input text labels
      chptr *inputLabels;
      char (*inputLabel)[32];
   };
   union {              // 1-bit element activity array, ~256 vertices
      ui64  mod[4];
      ui128 mod128[2];
      ui256 mod256;
   };
};

// Input data for element creation
al16 struct GUI_EL_DESC { // 144 bytes
   union {
      struct {
         ui32 hover[2]; // Functions for onHover & offHover
         union {
            ui32 activate[2][2]; // Functions for onActivate & offActivate via buttons 0 & 1
            struct {
               ui32 activate0[2]; // Functions for onActivate & offActivate via buttons 0
               ui32 activate1[2]; // Functions for onActivate & offActivate via buttons 0
            };
         };
      } func;
      ui32 function[6] = {};
   };
   union {
      chptr   textPtr = NULL;
      wchptr  wtextPtr;
      chptr  *textArray;
      wchptr *wtextArray;
   };
   struct { // panel, cursor, text
      SSE4Df32 panel  = { 1.0f, 1.0f, 1.0f, 1.0f };
      SSE4Df32 cursor = { 1.0f, 1.0f, 1.0f, 1.0f };
      SSE4Df32 text   = { 1.0f, 1.0f, 1.0f, 1.0f };
   } tint;
   struct { // panel, cursor, text
      VEC2Df panel  = { 1.0f, 1.0f };
      VEC2Df cursor = { 1.0f, 1.0f };
      VEC2Df text   = { 1.0f, 1.0f };
   } size;
   struct { // soundbank, alphabet, spriteLib, panelSprite, cursorSprite
      si16 soundBank    = -1;
      si16 alphabet     = -1;
      ui16 spriteLib    = 0x0FFFFu;
      ui16 panelSprite  = 0x0FFFFu;
      ui16 cursorSprite = 0x0FFFFu;
   } index;
   union {
      struct {
         struct BITFIELD { // align, mods
            ui8 align;
            ui8 mods;
         } panel;
         BITFIELD cursor; // align, mods
         BITFIELD text; // align, mods
      };
      struct {
         ui16 panelBits;
         ui16 cursorBits;
         ui16 textBits;
      };
   };
   VEC2Df viewPos = { 0.0f, 0.0f };
   union {
      si32 charCount = 0;
      si32 stringCount;
   };
   ui32 RES = 0x0CDCDCDCDu;

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

struct UI_PTRS {
   GUI_ELEMENT &element;
   GUI_EL_DGS *vertex;
};

typedef const AE_ELEMENT_TYPE cAE_ELEMENT_TYPE;
typedef const AE_REGEN_VERTS  cAE_REGEN_VERTS;
typedef const GUI_EL_DESC     cGUI_EL_DESC;
typedef const GUI_ELEMENT     cGUI_ELEMENT;
typedef const GUI_INDICES     cGUI_INDICES;
typedef const GUI_SPRITE      cGUI_SPRITE;
typedef const GUI_SPRITE_LIB  cGUI_SPRITE_LIB;
typedef const UI_PTRS         cUI_PTRS;
