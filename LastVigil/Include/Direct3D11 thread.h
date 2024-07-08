 /************************************************************
  * File: Direct3D11 thread.h            Created: 2022/10/12 *
  *                                Last modified: 2024/07/06 *
  *                                                          *
  * Desc:                                                    *
  *                                                          *
  *  Copyright (c) David William Bull. All rights reserved.  *
  ************************************************************/
#pragma once

#include "master header.h"
#include <d3d11_4.h>
#include <directx/d3dx11.h>
#include <DirectXMath.h>
#include <D3Dcompiler.h>
#include <dwmapi.h>
#include "typedefs.h"
#include "Vector structures.h"
#include "Data structures.h"
#include "D3D11 type defines.h"
#include "D3D11 error testing.h"

#define AE_PTR_LIB
extern cptr ptrLib[16];

// Defined in "LastVigil.cpp"
extern SYSTEM_DATA   sysData;
extern wchptr        stThrdStat;
extern HINSTANCE     hInst;      // Current instance's handle
extern HRESULT       hr;
extern CLASS_TIMER   mainTimer;
extern vGLOBALCOORDS gco;

// Defined in "Direct3D11 thread.cpp"
extern vui64      THREAD_LIFE_V;
extern cwchptr    renderWndClass;
extern cwchptr    rndrWndTitle;
extern RESOLUTION ScrRes;

// Defined in "OpenAL1_1 thread.h"
extern SOUND_BANKptrc soundLibrary;

// Defined in "DirectInput8 thread.h"
extern vGLOBALCTRLVARS gcv;

extern VEC4Du16 inputsImmediate;
