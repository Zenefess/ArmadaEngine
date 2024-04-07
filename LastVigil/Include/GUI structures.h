/************************************************************
 * File: GUI structures.h               Created: 2023/01/26 *
 *                                Last modified: 2024/04/04 *
 *                                                          *
 * Desc:                                                    *
 *                                                          *
 *                                                          *
 *  Copyright (c) David William Bull. All rights reserved.  *
 ************************************************************/
#pragma once

#include "typedefs.h"
#include "Vector structures.h"

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
   VEC4Du16 tc; // Texture coordinates : 1p15
   union {      // Size : p16n0.0~1.0
      struct {
         ui16 width;
         ui16 height;
      };
      ui16 s;
      VEC2Du16 size;
   };
   union {      // Offsets : p16n-1.0~1.0
      struct {
         ui16 xos;
         ui16 yos;
      };
      ui16 os;
      VEC2Du16 offset;
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
   AVX16Du16 colour;     // Panel corners / Every 8 characters : p16n0.0~2.0
   VEC2Df size;          // Text/Element scale : % of viewspace
   fl32   rotAngle;      // Orientation
   fl32   width;         // Total width of vertex's text in view space
   ui32   parentIndex;   // GUI_EL_DGS index of parent element; 0x0FFFFFFFF==No parent
   ui32   textArrayOS;   // Offset into text array (div.by 16)
   ui16   alphabetIndex; // Offset into alphabet buffer
   ui8    atlasIndex;    // Runtime index of atlas texture
   ui8    elementType;   // Text=0, Panel=1, Button=2, Toggle=3, Scalar=4, Cursor=5(, Dial=6, 
   ui16   sibling;       // 0~14==First sibling element offset / Sibling count (if bit15 set)
   ui8    orient;        // 0~3==Justificition : left, right, top, bottom
   ui8    mods;          // 0==Rotate, 1==Translate, 2&3==Scaling: X&Y, 4==Invisible, 5==Truncate, 6==Compress, 7==Wide chars
};

al32 struct GUI_ELEMENT { // 128 bytes
   union {
      struct { // View space coordinates
         VEC4Df   activeCoords; // Absolute coordinates of outer border
         VEC2Df   viewScale;    // Coordinate multipliers to convert view space to panel space
         VEC2Du16 activePos;    // Absolute center coordinate of interactable area : p16n0.0~1.0
         VEC2Du16 activeZone;   // Absolute X&Y lengths from center coordinate to inner border : p16n0.0~1.0
      };
      AVX8Df32 coords;
   };
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
      funcptr function[6];
   };
   si32 charCount;      // Current number of characters in buffer
   si32 vertexIndex;    // Geometry data index in GUI vertex array
   ui16 vertexCount[2]; // Number of vertices used; 0==Current, 1==Maximum
   si16 soundBankIndex;
   union {
      ui16 bitField;      // 0~3==(Text=0, Panel=1, Button=2, Scalar=3, Input=4, Cursor=5, Knob=6)
      struct {            // 4~5==???, 6==Wide characters, 7==Button active
         ui8 elementType;
         ui8 stateBits;   // 8~13==Function busy bits, 14==Visible, 15==Active
      };
   };
   union {
      ui128 ui128Var;   // Scratch space
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
