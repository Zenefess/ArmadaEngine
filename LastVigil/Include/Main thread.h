/**********************************************************
 * File: Main thread.h                Created: 2008/09/16 *
 *                              Last modified: 2023/04/26 *
 *                                                        *
 * Desc:                                                  *
 *                                                        *
 * Copyright (c) David William Bull. All rights reserved. *
 **********************************************************/
#pragma once

#include "Data structures.h"

/*/
 *  Library of pointers to classes and resources
 *  --------------------------------------------
 *
 * 0==Class: File operations
 * 1==Class: Camera manager
 * 2==Class: Map manager
 * 3==Class: Entity manager
 * 4==Class: GUI manager
 * 5==
 * 6==
 * 7==
/*/
#define AE_PTR_LIB
extern cptr ptrLib[16];

#define AE_SYS_DATA
extern SYSTEM_DATA sysData;

#define AE_D3D11_2
extern vui64     THREAD_LIFE;  // 'Thread active' flags
extern wchptr    stThrdStat;   // Debug output
extern HINSTANCE hInst;        // Current instance's handle
extern HWND      hWnd;         // Main window's handle
extern HANDLE    hErrorOutput; // File handle for error output
extern HRESULT   hr;

extern wchar stErrorFilename[64];
void WriteError(cchptrc, cbool);
