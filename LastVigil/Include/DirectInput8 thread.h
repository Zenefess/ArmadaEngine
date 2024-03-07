/************************************************************
 * File: DirectInput8 thread.h          Created: 2022/11/01 *
 *                                Last modified: 2022/11/04 *
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

DIOBJECTDATAFORMAT ofMouse38[] = {
   { &GUID_Button, (DWORD)FIELD_OFFSET(DIMOUSESTATE48, b[0]), DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
   { &GUID_Button, (DWORD)FIELD_OFFSET(DIMOUSESTATE48, b[1]), DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
   { &GUID_Button, (DWORD)FIELD_OFFSET(DIMOUSESTATE48, b[2]), DIDFT_BUTTON | DIDFT_ANYINSTANCE | 0x080000000, 0 },
   { &GUID_Button, (DWORD)FIELD_OFFSET(DIMOUSESTATE48, b[3]), DIDFT_BUTTON | DIDFT_ANYINSTANCE | 0x080000000, 0 },
   { &GUID_Button, (DWORD)FIELD_OFFSET(DIMOUSESTATE48, b[4]), DIDFT_BUTTON | DIDFT_ANYINSTANCE | 0x080000000, 0 },
   { &GUID_Button, (DWORD)FIELD_OFFSET(DIMOUSESTATE48, b[5]), DIDFT_BUTTON | DIDFT_ANYINSTANCE | 0x080000000, 0 },
   { &GUID_Button, (DWORD)FIELD_OFFSET(DIMOUSESTATE48, b[6]), DIDFT_BUTTON | DIDFT_ANYINSTANCE | 0x080000000, 0 },
   { &GUID_Button, (DWORD)FIELD_OFFSET(DIMOUSESTATE48, b[7]), DIDFT_BUTTON | DIDFT_ANYINSTANCE | 0x080000000, 0 },
   { &GUID_XAxis,  (DWORD)FIELD_OFFSET(DIMOUSESTATE48, x),    DIDFT_AXIS   | DIDFT_ANYINSTANCE, 0 },
   { &GUID_YAxis,  (DWORD)FIELD_OFFSET(DIMOUSESTATE48, y),    DIDFT_AXIS   | DIDFT_ANYINSTANCE, 0 },
   { &GUID_ZAxis,  (DWORD)FIELD_OFFSET(DIMOUSESTATE48, z),    DIDFT_AXIS   | DIDFT_ANYINSTANCE, 0 }
};

DIDATAFORMAT dfMouse38 = { sizeof(DIDATAFORMAT), sizeof(DIOBJECTDATAFORMAT), DIDF_RELAXIS, sizeof(DIMOUSESTATE38),
                           (sizeof(ofMouse38) / sizeof(DIOBJECTDATAFORMAT)), ofMouse38 };

// Thread status strings
al16   cwchptr stRawInput = L"DirectInput 8";
extern cwchptr stBusy;
extern cwchptr stError;
extern cwchptr stInit;
extern wchptr  stThrdStat; // Text strings for thread status

extern HINSTANCE hInst;     // Current instance's handle
extern HWND      hWnd;      // Main window's handle
extern HWND      hWndDebug; // Debug window's handle
extern HRESULT   hr;

vol GLOBALCTRLVARS  gcv;
    GLOBALCTRLVARS *gcvLocalPtr;

al16 IDirectInput8       *di8;
     IDirectInputDevice8 *di8Key, *di8Mse, *di8Pad;
     char                 keyState[2][256];
     DIMOUSESTATE48       mseState[2];
al4  si32                &mouseWheelTilt = mseState[0].w;
     VEC4Du8              inputsImmediate;
