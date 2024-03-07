/************************************************************
 * File: GUI structures.h               Created: 2023/01/26 *
 *                                Last modified: 2023/07/08 *
 *                                                          *
 * Desc:                                                    *
 *                                                          *
 *                                                          *
 *  Copyright (c) David William Bull. All rights reserved.  *
 ************************************************************/
#pragma once

#include "typedefs.h"
#include "Vector structures.h"

al8 struct GUI_SPRITE {	// 32 bytes
	wchptr name;
	VEC4Df tc;	// Texture coordinates
	VEC2Df aa;	// Active area %
};

al16 struct GUI_SPRITE_LIB {	// 16 bytes
	GUI_SPRITE *sprite;
	ui16        atlasIndex;
	ui16        numSprites;
	ui16        maxSprites;
	ui16        RES;
};

al16 struct CHAR_IMM {	// 16 bytes
	VEC4Du16 tc;
	float    width;
	float    xos;
};

al16 struct ALPHABET {	// 32 bytes
	wchptr stLanguage;
	wchptr stAtlasFilename;	// Filename of atlas texture
	ui32   pIMMos;				// Offset into array for GPU's shaders
	ui16   numChars;
	ui16   atlasIndex;		// Index of atlas texture
	ui64   RES;
};

al32 struct GUI_EL_DGS {	// 96 bytes (24 scalars)	///--- Rewrite to remove redundants ---///
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
	ui32   parentIndex;   // GUI_EL_DGS index of parent element; 0x0FFFFFFFF==No parent
	ui32   textArrayOS;   // Offset into text array: Multiple of 16
	ui16   alphabetIndex; // Offset into alphabet buffer
	ui8    atlasIndex;    // Runtime index of atlas texture
	ui8    elementType;	 // Text=0, Panel=1, Button=2, Toggle=3, Scalar=4, Cursor=5, Tab=6
	ui32   RES;
	union {
		ui32 bitField;     // 0==NoRotation, 1==NoScaling, 2==NoTranslation, 3==Invisible
		struct {           // 4~7==Justificition : left, right, top, bottom
			ui8 orient;     // 8==Truncate, 9==Compress : String modifiers
			ui8 textMod;    // 10~31==???
			ui8 RES16;
		};
	};
};

al32 struct GUI_ELEMENT {	// 128 bytes
	union {
		struct {
			VEC4Df activeCoords;	// Absolute coordinates of interactable area in view space
			VEC2Df activePos;		// Absolute center coordinate of interactable area in view space
			VEC2Df viewScale;		// Coordinate multipliers to convert view space to panel space
		};
		AVX8Df32 coords;
	};
	union {
		struct {
			funcptr hover[2];				// Functions for onHover & offHover
			union {
				funcptr activate[2][2];	// Functions for onActivate & offActivate via buttons 0 & 1
				struct {
					funcptr activate0[2];
					funcptr activate1[2];
				};
			};
		} func;
		funcptr function[6];
	};
	si32 charCount;		// Current number of characters in buffer
	si32 vertexIndex;		// Geometry data index in GUI vertex array
	ui16 vertexCount[2];	// Number of vertices used; 0==Current, 1==Maximum
	si16 soundBankIndex;
	union {
		ui16 bitField;			// 0~3==(Text=0, Panel=1, Button=2, Scalar=3, Input=4, Cursor=5, Tab=6)
		struct {					// 4~6==???, 7==Button active
			ui8 elementType;	// 8~13==Function busy bits, 14==Visible, 15==Active
			ui8 stateBits;
		};
	};
	union {
		ui128 ui128Var;	// Scratch space
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

al32 struct GUI_INTERFACE {	// 64 bytes
	ui16 *vertex;      // Vertex array of indices to element entries
	si16  maxVertices;
	si16  vertCount;
	si16  maxInputs;
	si16  inputCount;
	union {            // Array of input combinations
		VEC4Du8 *inputs;
		ui8    (*input)[4];
	};
	union {            // Array of input text labels
		chptr *inputLabels;
		char (*inputLabel)[32];
	};
	union {            // 1-bit element activity array, ~256 vertices
		ui64  mod[4];
		ui128 mod128[2];
		ui256 mod256;
	};
};
