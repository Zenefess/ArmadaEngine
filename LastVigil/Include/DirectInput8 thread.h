/************************************************************
 * File: DirectInput8 thread.h          Created: 2022/11/01 *
 *                                Last modified: 2024/06/15 *
 *                                                          *
 * Desc:                                                    *
 *                                                          *
 *  Copyright (c) David William Bull. All rights reserved.  *
 ************************************************************/
#pragma once

#define DIRECTINPUT_VERSION  0x0800

#include "pch.h"
#include "typedefs.h"
#include "Data structures.h"
#include "class_timers.h"
#include <dinput.h>
#include "DirectInput8 keyboard scan codes.h"
#include "DI8 scan code text labels.h"
#include "DI8 error testing.h"

#ifdef _DEBUG
#define NDEBUG 0
#include "comdef.h"
#endif

al4 struct DIMOUSESTATE38 { // 20/28 bytes
   ui32 RES[2];
   union {
      ui8  b[8];
      ui32 buttons[2];
   };
   union {
      si32 axis[3];
      struct { si32 x, y, z; };
   };
};

al32 struct DIMOUSESTATE48 { // 24/32 bytes
   ui64 RES;
   union {
      ui8  b[8];
      ui64 buttons;
   };
   union {
      struct { si32 x, y, z, w; };
      si32  axis[4];
      si128 xmm;
   };
};

typedef al32 union DIJOYSTATEAE { // 80/96 bytes
   struct {
      SSE4Ds32 s, t;
      SSE4Du32 p;
      SSE16Du8 b;
   };
   struct {
      SSE4Df32 fs, ft;
   };
   struct {
      si32 lx, ly, rx, ry;
      si32 trigger[2], slider[2];
      ui32 pov[4];
      ui8  button[32];
   };
#ifdef __AVX__
   si256  ymm[2];
   fl32x8 ymmf[2];
#endif
   si128  xmm[4];
   fl32x4 xmmf[4];
} DIJOYSTATEAE, *LPDIJOYSTATEAE;

DIOBJECTDATAFORMAT ofMouse38[] = {
   { 0, (DWORD)FIELD_OFFSET(DIMOUSESTATE48, b[0]), 0x000FFFF0C, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIMOUSESTATE48, b[1]), 0x000FFFF0C, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIMOUSESTATE48, b[2]), 0x080FFFF0C, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIMOUSESTATE48, b[3]), 0x080FFFF0C, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIMOUSESTATE48, b[4]), 0x080FFFF0C, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIMOUSESTATE48, b[5]), 0x080FFFF0C, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIMOUSESTATE48, b[6]), 0x080FFFF0C, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIMOUSESTATE48, b[7]), 0x080FFFF0C, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIMOUSESTATE48, x),    0x000FFFF03, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIMOUSESTATE48, y),    0x000FFFF03, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIMOUSESTATE48, z),    0x000FFFF03, 0 }
};

DIOBJECTDATAFORMAT ofGamepadAE[] = {
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, lx),         0x080FFFF03, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, ly),         0x080FFFF03, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, ry),         0x080FFFF03, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, rx),         0x080FFFF03, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, trigger[0]), 0x080FFFF03, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, trigger[1]), 0x080FFFF03, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, slider[0]),  0x080FFFF03, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, slider[1]),  0x080FFFF03, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, pov[0]),     0x080FFFF10, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, pov[1]),     0x080FFFF10, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, pov[2]),     0x080FFFF10, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, pov[3]),     0x080FFFF10, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, button[0]),  0x080FFFF0C, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, button[1]),  0x080FFFF0C, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, button[2]),  0x080FFFF0C, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, button[3]),  0x080FFFF0C, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, button[4]),  0x080FFFF0C, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, button[5]),  0x080FFFF0C, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, button[6]),  0x080FFFF0C, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, button[7]),  0x080FFFF0C, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, button[8]),  0x080FFFF0C, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, button[9]),  0x080FFFF0C, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, button[10]), 0x080FFFF0C, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, button[11]), 0x080FFFF0C, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, button[12]), 0x080FFFF0C, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, button[13]), 0x080FFFF0C, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, button[14]), 0x080FFFF0C, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, button[15]), 0x080FFFF0C, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, button[16]), 0x080FFFF0C, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, button[17]), 0x080FFFF0C, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, button[18]), 0x080FFFF0C, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, button[19]), 0x080FFFF0C, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, button[20]), 0x080FFFF0C, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, button[21]), 0x080FFFF0C, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, button[22]), 0x080FFFF0C, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, button[23]), 0x080FFFF0C, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, button[24]), 0x080FFFF0C, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, button[25]), 0x080FFFF0C, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, button[26]), 0x080FFFF0C, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, button[27]), 0x080FFFF0C, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, button[28]), 0x080FFFF0C, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, button[29]), 0x080FFFF0C, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, button[30]), 0x080FFFF0C, 0 },
   { 0, (DWORD)FIELD_OFFSET(DIJOYSTATEAE, button[31]), 0x080FFFF0C, 0 }
};

DIDATAFORMAT dfMouse38   = { 32, 24, 2, 28, 11, ofMouse38};
DIDATAFORMAT dfGamepadAE = { 32, 24, 1, 96, 44, ofGamepadAE};

// Thread status strings
al16   cwchptr stRawInput = L"DirectInput 8";
extern cwchptr stBusy;
extern cwchptr stError;
extern cwchptr stInit;

vol GLOBALCTRLVARS  gcv;
    GLOBALCTRLVARS  gcvLocal {};
    TEXTBUFFER      textBufferInfo(1024, false); // Buffer information for character input

al16 IDirectInput8       *di8;
     IDirectInputDevice8 *di8Key, *di8Mse, *di8Pad[8];
al64 char                 keyState[2][256];
     DIJOYSTATEAE         padState[2][8];
     DIMOUSESTATE48       mseState[2];
     VEC4Du16             inputsImmediate;
     si32                &mouseWheelTilt = mseState[0].w;
     ui8                  padOrder[8] = { 1, 0, 2, 3, 4, 5, 6, 7 };
     ui32                 padCount = 0;
     fl32                 mouseScale = 0.0003025f;

#ifdef __AVX__
fl32x8 padScale[8] = { { .m256_f32 = { 1.0f / 31233.0f, 1.0f / 31233.0f, 1.0f / 31233.0f, 1.0f / 31233.0f, 1.0f / 32255.0f, 1.0f / 32255.0f, 1.0f / 32767.0f, 1.0f / 32767.0f } },
                       { .m256_f32 = { 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f } },
                       { .m256_f32 = { 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f } },
                       { .m256_f32 = { 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f } },
                       { .m256_f32 = { 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f } },
                       { .m256_f32 = { 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f } },
                       { .m256_f32 = { 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f } },
                       { .m256_f32 = { 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f, 1.0f / 30720.0f } } };

si256 padOS[8] = { { .m256i_i32 = { 32768, 33280, 32767, 32767, 32767, 32767, 32767, 32767 } },
                   { .m256i_i32 = { 31743, 31743, 32767, 32767, 32767, 32767, 32767, 32767 } },
                   { .m256i_i32 = { 31743, 31743, 32767, 32767, 32767, 32767, 32767, 32767 } },
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
