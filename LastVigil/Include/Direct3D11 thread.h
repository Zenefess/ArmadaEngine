 /************************************************************
  * File: Direct3D11 thread.h            Created: 2022/10/12 *
  *                                Last modified: 2022/11/01 *
  *                                                          *
  * Desc:                                                    *
  *                                                          *
  *  Copyright (c) David William Bull. All rights reserved.  *
  ************************************************************/
#pragma once

#include "pch.h"
#include <d3d11_4.h>
#include <dxgi1_2.h>
#include <directx/d3dx11.h>
#include <directx/d3dx10.h>
#include <DirectXMath.h>
#include <D3Dcompiler.h>
#include <dwmapi.h>
#include "typedefs.h"
#include "Vector structures.h"
#include "Data structures.h"
#include "D3D11 type defines.h"
#include "D3D11 error testing.h"

extern SYSTEM_DATA sysData;

extern cwchptr    renderWndClass;
extern cwchptr    rndrWndTitle;
extern vui64      THREAD_LIFE_V;
extern RESOLUTION ScrRes;

extern cwchptr stBusy;
extern cwchptr stError;
extern cwchptr stInit;
extern wchptr  stThrdStat;	// Text string for thread status

extern HINSTANCE hInst;			// Current instance's handle
extern HWND      hWnd;			// Main window's handle
extern HWND      hWndDebug;		// Debug window's handle
extern HRESULT   hr;

extern vol GLOBALCTRLVARS gcv;
extern vol GLOBALCOORDS   gco;
extern     CLASS_TIMER    mainTimer;

#define AE_PTR_LIB
extern cptr ptrLib[16];

extern VEC4Du8 inputsImmediate;
