/************************************************************
 * File: OpenAL1_1 thread.h             Created: 2022/11/12 *
 *                                Last modified: 2024/07/12 *
 *                                                          *
 * Desc:                                                    *
 *                                                          *
 * Copyright (c) David William Bull. All rights reserved.   *
 ************************************************************/

#include "master header.h"
#include <openal\al.h>
#include <openal\alc.h>
#include "OAL11 error testing.h"
#include "OAL1_1 file operations.h"

extern vGLOBALCOORDS gco;

// Thread status strings
extern cwchptr  stInit;
extern cwchptr  stBusy;
extern cwchptr  stError;
       cwchptrc stOAL11 = L"OpenAL 1.1";

extern wchptr  stThrdStat; // Text strings for thread status
extern HWND    hWnd;       // Main window's handle
extern HWND    hWndDebug;  // Debug window's handle
extern HRESULT hr;

// Master sound library
SOUND_LIB soundLib(MAX_SOUND_BANKS, 64u);
