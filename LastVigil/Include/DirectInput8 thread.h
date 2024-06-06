/************************************************************
 * File: DirectInput8 thread.h          Created: 2022/11/01 *
 *                                Last modified: 2024/06/06 *
 *                                                          *
 * Desc:                                                    *
 *                                                          *
 *  Copyright (c) David William Bull. All rights reserved.  *
 ************************************************************/
#pragma once

#define DIRECTINPUT_VERSION  0x0800

#include "typedefs.h"
#include "Data structures.h"
#include <dinput.h>
#include "DirectInput8 keyboard scan codes.h"
#include "DI8 scan code text labels.h"
#include "DI8 error testing.h"

#ifdef _DEBUG
#define NDEBUG 0
#include "comdef.h"
#endif

al4 struct DIMOUSESTATE38 { // 20 bytes
   union {
      ui8  b[8];
      ui32 buttons[2];
   };
   union {
      si32 axis[3];
      struct {
         si32 x;
         si32 y;
         si32 z;
      };
   };
};

al8 struct DIMOUSESTATE48 { // 24 bytes
   union {
      ui8  b[8];
      ui64 buttons;
   };
   union {
      si32 axis[4];
      struct {
         si32 x;
         si32 y;
         si32 z;
         si32 w;
      };
   };
};

typedef al32 union DIJOYSTATEAE { // 80/96 bytes
   struct {
      SSE4Ds32 s;
      SSE4Ds32 t;
      SSE4Du32 p;
      SSE16Du8 b;
   };
   struct {
      SSE4Df32 fs;
      SSE4Df32 ft;
   };
   struct {
      si32 lx;          /* x-axis position              */
      si32 ly;          /* y-axis position              */
      si32 rx;          /* x-axis rotation              */
      si32 ry;          /* y-axis rotation              */
      si32 trigger[2];  /* extra axes positions         */
      si32 slider[2];
      ui32 pov[4];      /* POV directions               */
      ui8  button[32];  /* 32 buttons                   */
   };
#ifdef __AVX__
   si256  ymm[2];
   fl32x8 ymmf[2];
#endif
   si128  xmm[4];
   fl32x4 xmmf[4];
} DIJOYSTATEAE, *LPDIJOYSTATEAE;

DIOBJECTDATAFORMAT ofMouse38[] = {
   { 0, (DWORD)FIELD_OFFSET(DIMOUSESTATE48, b[0]), DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIMOUSESTATE48, b[1]), DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIMOUSESTATE48, b[2]), DIDFT_BUTTON | DIDFT_ANYINSTANCE | 0x080000000, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIMOUSESTATE48, b[3]), DIDFT_BUTTON | DIDFT_ANYINSTANCE | 0x080000000, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIMOUSESTATE48, b[4]), DIDFT_BUTTON | DIDFT_ANYINSTANCE | 0x080000000, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIMOUSESTATE48, b[5]), DIDFT_BUTTON | DIDFT_ANYINSTANCE | 0x080000000, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIMOUSESTATE48, b[6]), DIDFT_BUTTON | DIDFT_ANYINSTANCE | 0x080000000, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIMOUSESTATE48, b[7]), DIDFT_BUTTON | DIDFT_ANYINSTANCE | 0x080000000, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIMOUSESTATE48, x),    DIDFT_AXIS   | DIDFT_ANYINSTANCE, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIMOUSESTATE48, y),    DIDFT_AXIS   | DIDFT_ANYINSTANCE, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIMOUSESTATE48, z),    DIDFT_AXIS   | DIDFT_ANYINSTANCE, 0 }
};

DIOBJECTDATAFORMAT ofGamepadAE[] = {
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, lx),         DIDFT_AXIS   | DIDFT_ANYINSTANCE | 0x080000000, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, ly),         DIDFT_AXIS   | DIDFT_ANYINSTANCE | 0x080000000, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, ry),         DIDFT_AXIS   | DIDFT_ANYINSTANCE | 0x080000000, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, rx),         DIDFT_AXIS   | DIDFT_ANYINSTANCE | 0x080000000, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, trigger[0]), DIDFT_AXIS   | DIDFT_ANYINSTANCE | 0x080000000, 0},
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, trigger[1]), DIDFT_AXIS   | DIDFT_ANYINSTANCE | 0x080000000, 0},
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, slider[0]),  DIDFT_AXIS   | DIDFT_ANYINSTANCE | 0x080000000, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, slider[1]),  DIDFT_AXIS   | DIDFT_ANYINSTANCE | 0x080000000, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, pov[0]),     DIDFT_POV    | DIDFT_ANYINSTANCE | 0x080000000, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, pov[1]),     DIDFT_POV    | DIDFT_ANYINSTANCE | 0x080000000, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, pov[2]),     DIDFT_POV    | DIDFT_ANYINSTANCE | 0x080000000, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, pov[3]),     DIDFT_POV    | DIDFT_ANYINSTANCE | 0x080000000, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, button[0]),  DIDFT_BUTTON | DIDFT_ANYINSTANCE | 0x080000000, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, button[1]),  DIDFT_BUTTON | DIDFT_ANYINSTANCE | 0x080000000, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, button[2]),  DIDFT_BUTTON | DIDFT_ANYINSTANCE | 0x080000000, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, button[3]),  DIDFT_BUTTON | DIDFT_ANYINSTANCE | 0x080000000, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, button[4]),  DIDFT_BUTTON | DIDFT_ANYINSTANCE | 0x080000000, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, button[5]),  DIDFT_BUTTON | DIDFT_ANYINSTANCE | 0x080000000, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, button[6]),  DIDFT_BUTTON | DIDFT_ANYINSTANCE | 0x080000000, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, button[7]),  DIDFT_BUTTON | DIDFT_ANYINSTANCE | 0x080000000, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, button[8]),  DIDFT_BUTTON | DIDFT_ANYINSTANCE | 0x080000000, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, button[9]),  DIDFT_BUTTON | DIDFT_ANYINSTANCE | 0x080000000, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, button[10]), DIDFT_BUTTON | DIDFT_ANYINSTANCE | 0x080000000, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, button[11]), DIDFT_BUTTON | DIDFT_ANYINSTANCE | 0x080000000, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, button[12]), DIDFT_BUTTON | DIDFT_ANYINSTANCE | 0x080000000, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, button[13]), DIDFT_BUTTON | DIDFT_ANYINSTANCE | 0x080000000, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, button[14]), DIDFT_BUTTON | DIDFT_ANYINSTANCE | 0x080000000, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, button[15]), DIDFT_BUTTON | DIDFT_ANYINSTANCE | 0x080000000, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, button[16]), DIDFT_BUTTON | DIDFT_ANYINSTANCE | 0x080000000, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, button[17]), DIDFT_BUTTON | DIDFT_ANYINSTANCE | 0x080000000, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, button[18]), DIDFT_BUTTON | DIDFT_ANYINSTANCE | 0x080000000, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, button[19]), DIDFT_BUTTON | DIDFT_ANYINSTANCE | 0x080000000, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, button[20]), DIDFT_BUTTON | DIDFT_ANYINSTANCE | 0x080000000, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, button[21]), DIDFT_BUTTON | DIDFT_ANYINSTANCE | 0x080000000, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, button[22]), DIDFT_BUTTON | DIDFT_ANYINSTANCE | 0x080000000, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, button[23]), DIDFT_BUTTON | DIDFT_ANYINSTANCE | 0x080000000, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, button[24]), DIDFT_BUTTON | DIDFT_ANYINSTANCE | 0x080000000, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, button[25]), DIDFT_BUTTON | DIDFT_ANYINSTANCE | 0x080000000, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, button[26]), DIDFT_BUTTON | DIDFT_ANYINSTANCE | 0x080000000, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, button[27]), DIDFT_BUTTON | DIDFT_ANYINSTANCE | 0x080000000, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, button[28]), DIDFT_BUTTON | DIDFT_ANYINSTANCE | 0x080000000, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, button[29]), DIDFT_BUTTON | DIDFT_ANYINSTANCE | 0x080000000, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, button[30]), DIDFT_BUTTON | DIDFT_ANYINSTANCE | 0x080000000, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, button[31]), DIDFT_BUTTON | DIDFT_ANYINSTANCE | 0x080000000, 0 }
};

DIDATAFORMAT dfMouse38   = { sizeof(DIDATAFORMAT), sizeof(DIOBJECTDATAFORMAT), DIDF_RELAXIS, sizeof(DIMOUSESTATE38), (sizeof(ofMouse38) / sizeof(ofMouse38[0])), ofMouse38};
DIDATAFORMAT dfGamepadAE = { sizeof(DIDATAFORMAT), sizeof(DIOBJECTDATAFORMAT), DIDF_ABSAXIS, sizeof(DIJOYSTATEAE), (sizeof(ofGamepadAE) / sizeof(ofGamepadAE[0])), ofGamepadAE};

// Thread status strings
al16   cwchptr stRawInput = L"DirectInput 8";
extern cwchptr stBusy;
extern cwchptr stError;
extern cwchptr stInit;
extern wchptr  stThrdStat;  // Text strings for thread status

extern vui64 THREAD_LIFE;   // 'Thread active' flags

extern HINSTANCE hInst;     // Current instance's handle
extern HWND      hWnd;      // Main window's handle
extern HWND      hWndDebug; // Debug window's handle
extern HRESULT   hr;

vol GLOBALCTRLVARS  gcv;
    GLOBALCTRLVARS  gcvLocal {};
    GLOBALCTRLVARS *gcvLocalPtr;
    TEXTBUFFER      textBufferInfo(1024, false); // Buffer information for character input

al16 IDirectInput8       *di8;
     IDirectInputDevice8 *di8Key, *di8Mse, *di8Pad[8];
al32 char                 keyState[2][256];
al8  DIMOUSESTATE48       mseState[2];
     DIJOYSTATEAE         padState[2][8];
     VEC4Du16             inputsImmediate;
     si32                &mouseWheelTilt = mseState[0].w;
     ui8                  padOrder[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };
     ui32                 padCount = 0;
     fl32                 mouseScale = 0.0003025f;

#ifdef __AVX__
fl32x8 padScale[8] = { { .m256_f32 = { 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f } },
                       { .m256_f32 = { 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f } },
                       { .m256_f32 = { 1.0f / 31233.0f, 1.0f / 31233.0f, 1.0f / 31233.0f, 1.0f / 31233.0f, 1.0f / 32255.0f, 1.0f / 32255.0f, 1.0f / 32767.0f, 1.0f / 32767.0f } },
                       { .m256_f32 = { 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f } },
                       { .m256_f32 = { 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f } },
                       { .m256_f32 = { 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f } },
                       { .m256_f32 = { 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f } },
                       { .m256_f32 = { 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f } } };

si256 padOS[8] = { { .m256i_i32 = { 31743, 31743, 32767, 32767, 32767, 32767, 32767, 32767 } },
                   { .m256i_i32 = { 31743, 31743, 32767, 32767, 32767, 32767, 32767, 32767 } },
                   { .m256i_i32 = { 32768, 33280, 32767, 32767, 32767, 32767, 32767, 32767 } },
                   { .m256i_i32 = { 31743, 31743, 32767, 32767, 32767, 32767, 32767, 32767 } },
                   { .m256i_i32 = { 31743, 31743, 32767, 32767, 32767, 32767, 32767, 32767 } },
                   { .m256i_i32 = { 31743, 31743, 32767, 32767, 32767, 32767, 32767, 32767 } },
                   { .m256i_i32 = { 31743, 31743, 32767, 32767, 32767, 32767, 32767, 32767 } },
                   { .m256i_i32 = { 31743, 31743, 32767, 32767, 32767, 32767, 32767, 32767 } } };
#else
fl32x4 padScale[8][2] = { { { .m128_f32 = { rcp32767f, rcp32767f, rcp32767f, rcp32767f } }, { .m128_f32 = { rcp32767f, rcp32767f, rcp32767f, rcp32767f } } },
                          { { .m128_f32 = { rcp32767f, rcp32767f, rcp32767f, rcp32767f } }, { .m128_f32 = { rcp32767f, rcp32767f, rcp32767f, rcp32767f } } },
                          { { .m128_f32 = { rcp32767f, rcp32767f, rcp32767f, rcp32767f } }, { .m128_f32 = { rcp32767f, rcp32767f, rcp32767f, rcp32767f } } },
                          { { .m128_f32 = { rcp32767f, rcp32767f, rcp32767f, rcp32767f } }, { .m128_f32 = { rcp32767f, rcp32767f, rcp32767f, rcp32767f } } },
                          { { .m128_f32 = { rcp32767f, rcp32767f, rcp32767f, rcp32767f } }, { .m128_f32 = { rcp32767f, rcp32767f, rcp32767f, rcp32767f } } },
                          { { .m128_f32 = { rcp32767f, rcp32767f, rcp32767f, rcp32767f } }, { .m128_f32 = { rcp32767f, rcp32767f, rcp32767f, rcp32767f } } },
                          { { .m128_f32 = { rcp32767f, rcp32767f, rcp32767f, rcp32767f } }, { .m128_f32 = { rcp32767f, rcp32767f, rcp32767f, rcp32767f } } },
                          { { .m128_f32 = { rcp32767f, rcp32767f, rcp32767f, rcp32767f } }, { .m128_f32 = { rcp32767f, rcp32767f, rcp32767f, rcp32767f } } } };

si128 padOS[8][2] = { { { .m128i_i32 = { 31743, 31743, 32767, 32767 } }, { { .m128i_i32 = { 32767, 32767, 32767, 32767 } } },
                      { { .m128i_i32 = { 31743, 31743, 32767, 32767 } }, { { .m128i_i32 = { 32767, 32767, 32767, 32767 } } },
                      { { .m128i_i32 = { 31743, 31743, 32767, 32767 } }, { { .m128i_i32 = { 32767, 32767, 32767, 32767 } } },
                      { { .m128i_i32 = { 31743, 31743, 32767, 32767 } }, { { .m128i_i32 = { 32767, 32767, 32767, 32767 } } },
                      { { .m128i_i32 = { 31743, 31743, 32767, 32767 } }, { { .m128i_i32 = { 32767, 32767, 32767, 32767 } } },
                      { { .m128i_i32 = { 31743, 31743, 32767, 32767 } }, { { .m128i_i32 = { 32767, 32767, 32767, 32767 } } },
                      { { .m128i_i32 = { 31743, 31743, 32767, 32767 } }, { { .m128i_i32 = { 32767, 32767, 32767, 32767 } } },
                      { { .m128i_i32 = { 31743, 31743, 32767, 32767 } }, { { .m128i_i32 = { 32767, 32767, 32767, 32767 } } } };
#endif

AVX16Ds32 padDeadZone = { ._si32 = { -1535, 1535, -1535, 1535, -1535, 1535, -1535, 1535, -3, 3, -3, 3, -3, 3, -3, 3 } };
AVX8Df32  padShaping  = { ._fl   = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f } };
