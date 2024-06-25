/**********************************************************
 * File: Main thread.h                Created: 2008/09/16 *
 *                              Last modified: 2024/06/20 *
 *                                                        *
 * Desc:                                                  *
 *                                                        *
 * Copyright (c) David William Bull. All rights reserved. *
 **********************************************************/
#pragma once

/*/
 *  Library of pointers to classes and resources
 *  --------------------------------------------
 *
 *  0==Class: File operations
 *  1==Class: GPU manager
 *  2==
 *  3==
 *  4==Class: GUI manager
 *  5==Class: Camera manager
 *  6==Class: Map manager
 *  7==Class: Entity manager
 *  8==
 *  9==
 * 10==
 * 11==
 * 12==
 * 13==
 * 14==Active MAP_DESC information
 * 15==Active GUI_DESC information
/*/
#define AE_PTR_LIB
extern cptr ptrLib[16];

#define AE_D3D11_2
extern SYSTEM_DATA   sysData;      // Automated tracking of system data
extern CLASS_FILEOPS files;        // File handler
extern CLASS_TIMER   mainTimer;    // Primary (single) timer
extern vui64         THREAD_LIFE;  // 'Thread active' flags
extern wchptr        stThrdStat;   // Debug output
extern HINSTANCE     hInst;        // Current instance's handle
extern HWND          hWnd;         // Main window's handle
extern HWND          hWndDebug;    // Debug window's handle
extern HANDLE        hErrorOutput; // File handle for error output
extern HRESULT       hr;
extern wchar         stErrorFilename[64];
extern vGLOBALCOORDS gco;

extern inline void Try(cchptrc stEvent, cui32 uiResult, cAE_SUBSYSTEM subsystem);
extern        void WriteError(cchptrc, cbool);
