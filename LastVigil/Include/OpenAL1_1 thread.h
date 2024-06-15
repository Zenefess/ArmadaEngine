/************************************************************
 * File: OpenAL1_1 thread.h             Created: 2022/11/12 *
 *                                Last modified: 2024/06/15 *
 *                                                          *
 * Desc:                                                    *
 *                                                          *
 * Copyright (c) David William Bull. All rights reserved.   *
 ************************************************************/

#include "typedefs.h"
#include "Data structures.h"
#include <openal\al.h>
#include <openal\alc.h>
#include "OAL11 error testing.h"
#include "OAL1_1 file operations.h"

extern vol GLOBALCOORDS gco;

// Thread status strings
extern cwchptr  stInit;
extern cwchptr  stBusy;
extern cwchptr  stError;
       cwchptrc stOAL11 = L"OpenAL 1.1";

extern al16 wchptr  stThrdStat; // Text strings for thread status
extern al8  HWND    hWnd;       // Main window's handle
extern al8  HWND    hWndDebug;  // Debug window's handle
extern al8  HRESULT hr;
