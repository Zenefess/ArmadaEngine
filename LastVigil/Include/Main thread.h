/**********************************************************
 * File: Main thread.h                Created: 2008/09/16 *
 *                              Last modified: 2024/04/18 *
 *                                                        *
 * Desc:                                                  *
 *                                                        *
 * Copyright (c) David William Bull. All rights reserved. *
 **********************************************************/

/*/
 *  Library of pointers to classes and resources
 *  --------------------------------------------
 *
 *  0==Class: File operations
 *  1==Class: Camera manager
 *  2==Class: Map manager
 *  3==Class: Entity manager
 *  4==Class: GUI manager
 *  5==Class: GPU manager
 *  6==
 *  7==
 *  8==Active MAP_DESC information
 *  9==Active GUI_DESC information
 * 10==
 * 11==
 * 12==
 * 13==
 * 14==
 * 15==
/*/
#define AE_PTR_LIB
extern cptr ptrLib[16];

#ifdef DATA_TRACKING
#include "data tracking.h"
extern SYSTEM_DATA sysData;
#endif

#define AE_D3D11_2
extern vui64     THREAD_LIFE;  // 'Thread active' flags
extern wchptr    stThrdStat;   // Debug output
extern HINSTANCE hInst;        // Current instance's handle
extern HWND      hWnd;         // Main window's handle
extern HANDLE    hErrorOutput; // File handle for error output
extern HRESULT   hr;
extern wchar     stErrorFilename[64];

extern void WriteError(cchptrc, cbool);
