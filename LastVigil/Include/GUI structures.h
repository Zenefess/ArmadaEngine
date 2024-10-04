/************************************************************
 * File: GUI structures.h               Created: 2023/01/26 *
 *                                Last modified: 2024/07/30 *
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

// Vertex .align flags
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

// Vertex .mods flags
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

// Element .stateBits flags
constexpr ui8 UIE_BUSYON  = 0x01u;
constexpr ui8 UIE_BUSYOFF = 0x02u;
constexpr ui8 UIE_BUSY0   = 0x04u;
constexpr ui8 UIE_BUSY1   = 0x08u;
constexpr ui8 UIE_RES2    = 0x010u;
constexpr ui8 UIE_RES3    = 0x020u;
constexpr ui8 UIE_VISIBLE = 0x040u;
constexpr ui8 UIE_ACTIVE  = 0x080u;

// Profile .transitionMods flags
constexpr ui8 UIPT_DELAY   = 0x01u;
constexpr ui8 UIPT_OVERLAY = 0x02u;
constexpr ui8 UIPT_FADE    = 0x04u;

enum AE_REGEN_VERTS  : ui8 { aev_minimum, aev_maximum };
enum AE_ELEMENT_TYPE : ui8 { aet_text, aet_textArray, aet_panel, aet_textPanel,
                             aet_textArrayPanel, aet_inputPanel, aet_dropPanel, aet_button,
                             aet_textButton, aet_toggle, aet_scalar, aet_cursor,
                             aet_dial, aet_13, aet_14, aet_void };

union GUI_INDICES {
   cptrc data;
   struct {
      si32 vertex;
      si32 element;
   };
};

al32 struct GUI_SPRITE { // 32 bytes
   chptr  name;
   VEC2Df aa;   // Active area %
   VEC4Df tc;   // Texture coordinates
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
   chptr  stLanguage;
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
   fp16n0_3x16 colour; // Panel corners / Every 8 characters
   VEC2Df      size;   // Text/Element scale : % of viewspace
   union {
      fl32 rotAngle; // Type!=TextArray: Orientation
      fl32 listPos;  // Type==TextArray: Scroll offset
   };
   ui32 parentIndex; // GUI_EL_DGS index of parent element; this==No parent
   union {
      struct { // Type==Text|TextArray
         fl32 width;         // Total width of vertex's text in view space
         ui32 charBankOS;    // 0~25==Offset into character bank (div.by 16), 26~31==Vertex's char count
         ui16 alphabetIndex; // Offset into alphabet buffer
         ui8  RES8[2];
      };
      struct { // Type!=Text|TextArray
         union {
            VEC2Df animTime; // Durations of animation cycles
            VEC2Df border;   // x==Size, y==window opacity
         };
         ui8 animType;    // 0==Fade, 1==Zoom, 2==???, 3==Rotate, 4&5==Swing X&Y, 6~7==(Quantum=0, Linear=1, Smoothstep=2, ???=3)
         ui8 borderStyle; // 0~7==???
         ui8 atlasIndex;  // Runtime index of atlas texture
         ui8 elementType; // Text=0, Text Array=1, Panel=2, Button=3, Toggle=4, Scalar=5, Cursor=6, Dial=7
      };                  // 4~5==???, 6==Don't (re)calculate text offsets, 7==Button "down"
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
      ui32        parent;       // Parent element
      bool        toggleState;  // State of toggle element
      si32        scalarPos;    // Position of 1D scalar; integer
      fl32        fScalarPos;   // Position of 1D scalar; floating-point
      VEC2Ds32    scalarPos2D;  // Position of 2D scalar; integer
      VEC2Df      fScalarPos2D; // Position of 2D scalar; floating-point
   };
   union {
      fl32 lSpacing;  // Text list vertical spacing size
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

al16 struct GUI_INTERFACE { // 48 bytes
   union { chptr label; cchptr cLabel; }; // Interface name
   ui32ptr vertex; // Vertex array of indices to element entries
   union {         // Array of input combinations
      ui512  *input512;
      ui256 (*input256)[2];
      ui128 (*input128)[4];
      ui64  (*input64)[8];
      ptr     inputs;
   };
   union { // Array of input text labels
      chptr  inputLabels;
      char (*inputLabel)[32];
   };
   union { cUNIPTR mod; UNIPTR _mod; }; // Activity array; 1 bit per element
   ui16 maxVertices; // Maximum element entries
   ui16 maxInputs;   // Maxmimum input bit patterns
   ui16 vertCount;   // Current element entries
   ui16 inputCount;  // Current input bit patterns

private:
   void Init(cui16 maxElement, cui16 maxInput) {
      vertex      = zalloc1d32(ui32, maxElement);
      inputs      = zalloc1d32(ui64, maxInput * 8u);
      inputLabel  = zalloc2d32(char, maxInput, 32u);
      _mod        = zalloc1d16(ui8, maxElement >> 3u);
      maxVertices = maxElement;
      maxInputs   = maxInput;
      vertCount   = 0;
      inputCount  = 0;
   }

public:
   GUI_INTERFACE(cui16 maxElements, cui16 maxInputs) {
      Init(maxElements, maxInputs); cLabel = 0;
   };

   void Initialise(chptrc name, cui16 maxElements, cui16 maxInputs) {
      Init(maxElements, maxInputs); label = name;
   };

   void Initialise(cchptrc name, cui16 maxElements, cui16 maxInputs) {
      Init(maxElements, maxInputs); cLabel = name;
   };
};

// Properties for element types
struct GUI_PCT_PROPS {
   fl32x4 tint;
   VEC2Df size;
   union {
      ptr     p;       // Raw pointer
      chptr   cPtr;    // String
      wchptr  wPtr;    // Wide string
      chptr  *cArray;  // String array
      wchptr *wArray;  // Wide string array
      VEC2Df  wIndent; // Window indent (active area) multiplier
   };
   union {
      si32 cCount; // Character count
      si32 sCount; // String count
   };
   fl32 wOpacity; // Window opacity
   union {
      fl32 bSize;    // Border size
      fl32 lSpacing; // Text list vertical spacing multiplier
   };
   ui16 bStyle; // Border style
   union {
      struct {
         ui8 align; // Alignment flags
         ui8 mods;  // Modifier flags
      };
      ui16 flags;
   };
};

// Input data for element creation
al32 struct GUI_EL_DESC { // 190 of 192 bytes
   union {
      struct {
         ui32 hover[2]; // Functions for onHover & offHover
         union {
            ui32 activate[2][2]; // Functions for onActivate & offActivate via buttons 0 & 1
            struct {
               ui32 activate0[2]; // Functions for onActivate & offActivate via buttons 0
               ui32 activate1[2]; // Functions for onActivate & offActivate via buttons 1
            };
         };
      } func;
      ui32 function[6];
   };
   VEC2Df viewPos;
   union {
      GUI_PCT_PROPS panel; // Tint, Size, N/A, N/A, Window opacity, Border size, Border style, Alignment, Modifiers
      GUI_PCT_PROPS button; // Tint, Size, N/A, N/A, Window opacity, Border size, Border style, Alignment, Modifiers
   };
   GUI_PCT_PROPS cursor; // Tint, Size, N/A, N/A, Window opacity, Border size, Border style, Alignment, Modifiers
   GUI_PCT_PROPS text; // Tint, Size, String (array) pointer, Character/string count, N/A, N/A, N/A, Alignment, Modifiers
   struct { // soundbank, alphabet, spriteLib, panelSprite, cursorSprite
      ui16 soundBank;
      si16 alphabet;
      ui16 spriteLib;
      ui16 panelSprite;
      ui16 cursorSprite;
   } index;
   ui8 RES[2];
   union _AE_UI_EL_I {
      ui32 data;
      _AE_UI_EL_I(void) {}
      _AE_UI_EL_I(cui32 input) { data = input; }
      _AE_UI_EL_I(cVEC2Du32 input) { data = input.x; }
      _AE_UI_EL_I(cVEC3Du32 input) { data = input.x; }
   } parent;

   GUI_EL_DESC(void) {
      for(ui8 i = 0; i < 6; ++i)
         function[i] = 0;
      viewPos = { 0.0f, 0.0f };
      panel.tint     = { 1.0f, 1.0f, 1.0f, 1.0f };
      panel.size     = { 1.0f, 1.0f };
      panel.wIndent  = { 1.0f, 1.0f };
      panel.wOpacity = 0.0f;
      panel.bSize    = 1.0f;
      panel.bStyle   = 0;
      panel.align    = UI_ALIGN_C;
      panel.mods     = UI_PANEL;
      cursor.tint     = { 1.0f, 1.0f, 1.0f, 1.0f };
      cursor.size     = { 1.0f, 1.0f };
      cursor.wIndent  = { 1.0f, 1.0f };
      cursor.wOpacity = 0.0f;
      cursor.bSize    = 0.125f;
      cursor.bStyle   = 1;
      cursor.align    = UI_ALIGN_C;
      cursor.mods     = UI_CURSOR;
      text.tint     = { 1.0f, 1.0f, 1.0f, 1.0f };
      text.size     = { 1.0f, 1.0f };
      text.p        = 0;
      text.cCount   = 0;
      text.lSpacing = 1.0f;
      text.align    = UI_ALIGN_C;
      text.mods     = UI_TEXT;
      index.soundBank    = -1;
      index.alphabet     = -1;
      index.spriteLib    = 0x0FFFFu;
      index.panelSprite  = 0x0FFFFu;
      index.cursorSprite = 0x0FFFFu;
   }
};

// Defined in "DirectInput8 thread.h"
extern vGLOBALCTRLVARS gcv;

#define TriggerInputProcessing gcv.misc[7] |= 0x080

// Passing-in true when declaring will add it to the master library of pointers.
// Set .interfaceIndex before executing "gcv.misc[7] |= 0x080;"
al32 struct GUI_DESC {
   ui32 interfaceIndex   = 0;    // Currently active interface
   ui32 defaultInterface = 0;    // Default user interface index
   ui32 prevInterface    = 0;    // User interface transitioned from
   ui32 nextInterface    = 0;    // User interface to transition to
   fl32 transitionTime   = 0.0f; // Period of time (in seconds) to transition between user interfaces
   fl32 elapsedTime      = 0.0f; // Current elapsed time (in seconds) of transition
   fl32 noInputTime      = 0.0f; // Period of time GUI input is inactive during interface transitions
   ui8  transitionMods   = 0;    // 0==Delayed, 1==Overlaid, 2==Fade to, 3~7==???
//   ui8  RES[3];

   GUI_DESC(cbool makeGlobal) { if(makeGlobal) ptrLib[15] = this; }

   inline GUI_DESC &operator=(GUI_DESC &input) {
#ifdef __AVX__
      return (GUI_DESC &)(*(ui256ptrc)this = _mm256_load_si256((cui256ptrc)&input));
#else
      ((ui128ptrc)this)[0] = _mm_load_si128((cui128ptrc)&input);
      ((ui128ptrc)this)[1] = _mm_load_si128((cui128ptrc)&input + 1u);
      return *this;
#endif
   }

   inline void SetNextInterface(cui32 interfaceIndex, cfl32 duration, cfl32 inputDelay, cui8 flags) {
      transitionMods = flags;
      noInputTime    = inputDelay;
      transitionTime = duration;
      nextInterface  = interfaceIndex;
   }

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

typedef const AE_ELEMENT_TYPE        cAE_ELEMENT_TYPE;
typedef const AE_REGEN_VERTS         cAE_REGEN_VERTS;
typedef const ALPHABET               cALPHABET;
typedef       ALPHABET       *       ALPHABETptr;
typedef const ALPHABET       *       cALPHABETptr;
typedef       ALPHABET       * const ALPHABETptrc;
typedef const ALPHABET       * const cALPHABETptrc;
typedef const CHAR_IMM               cCHAR_IMM;
typedef       CHAR_IMM       *       CHAR_IMMptr;
typedef const CHAR_IMM       *       cCHAR_IMMptr;
typedef       CHAR_IMM       * const CHAR_IMMptrc;
typedef const CHAR_IMM       * const cCHAR_IMMptrc;
typedef const GUI_DESC               cGUI_DESC;
typedef       GUI_DESC       *       GUI_DESCptr;
typedef const GUI_DESC       *       cGUI_DESCptr;
typedef       GUI_DESC       * const GUI_DESCptrc;
typedef const GUI_DESC       * const cGUI_DESCptrc;
typedef const GUI_EL_DESC            cGUI_EL_DESC;
typedef       GUI_EL_DESC    *       GUI_EL_DESCptr;
typedef const GUI_EL_DESC    *       cGUI_EL_DESCptr;
typedef       GUI_EL_DESC    * const GUI_EL_DESCptrc;
typedef const GUI_EL_DESC    * const cGUI_EL_DESCptrc;
typedef const GUI_EL_DGS             cGUI_EL_DGS;
typedef       GUI_EL_DGS     * const GUI_EL_DGSptrc;
typedef const GUI_EL_DGS     * const cGUI_EL_DGSptrc;
typedef const GUI_ELEMENT            cGUI_ELEMENT;
typedef       GUI_ELEMENT    *       GUI_ELEMENTptr;
typedef const GUI_ELEMENT    *       cGUI_ELEMENTptr;
typedef       GUI_ELEMENT    * const GUI_ELEMENTptrc;
typedef const GUI_ELEMENT    * const cGUI_ELEMENTptrc;
typedef const GUI_INDICES            cGUI_INDICES;
typedef const GUI_INTERFACE          cGUI_INTERFACE;
typedef       GUI_INTERFACE  *       GUI_INTERFACEptr;
typedef const GUI_INTERFACE  *       cGUI_INTERFACEptr;
typedef       GUI_INTERFACE  * const GUI_INTERFACEptrc;
typedef const GUI_INTERFACE  * const cGUI_INTERFACEptrc;
typedef const GUI_SPRITE             cGUI_SPRITE;
typedef       GUI_SPRITE     *       GUI_SPRITEptr;
typedef const GUI_SPRITE     *       cGUI_SPRITEptr;
typedef       GUI_SPRITE     * const GUI_SPRITEptrc;
typedef const GUI_SPRITE     * const cGUI_SPRITEptrc;
typedef const GUI_SPRITE_LIB         cGUI_SPRITE_LIB;
typedef       GUI_SPRITE_LIB *       GUI_SPRITE_LIBptr;
typedef const GUI_SPRITE_LIB *       cGUI_SPRITE_LIBptr;
typedef       GUI_SPRITE_LIB * const GUI_SPRITE_LIBptrc;
typedef const GUI_SPRITE_LIB * const cGUI_SPRITE_LIBptrc;
typedef const UI_PTRS                cUI_PTRS;
