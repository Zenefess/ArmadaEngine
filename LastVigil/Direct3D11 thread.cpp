/************************************************************
 * File: Direct3D11 thread.cpp          Created: 2022/10/12 *
 *                               Code last mod.: 2023/07/24 *
 *                                                          *
 * Desc: Video rendering via Direct3D 11 API.               *
 *                                                          *
 *  Copyright (c) David William Bull. All rights reserved.  *
 ************************************************************/

#include "pch.h"
#include "resource.h"
#include "typedefs.h"
#include "Vector structures.h"
#include "Data structures.h"
#include "thread flags.h"
#include "main thread.h"
#include "Common functions.h"
#include "class_timers.h"
#include "Direct3D11 thread.h"
#include "Direct3D11 functions.h"
#include "Armada Intelligence/class_mapmanager.h"
#include "Armada Intelligence/class_entitymanager.h"
#include "Armada Intelligence/D3D11 helper functions.h"
#include "Armada Intelligence/GUI functions.h"

 // For development only
al32 struct GUI_SPRITE_DEFAULTS {
	cwchptrc name;
	cVEC4Df tc;
	cVEC2Df aa;
} defaultUISprite[81] = {
	{ L"Opaque 2048x88r6", { 0.0f, 0.765625f, 1.0f, 0.80859375f }, { 2026.0f / 2048.0f, 66.0f / 88.0f } },									// 0
	{ L"Opaque 1024x392r6", { 0.0f, 0.80859375f, 0.5f, 1.0f }, { 1002.0f / 1024.0f, 370.0f / 392.0f } },
	{ L"Opaque 512x392r6", { 0.5f, 0.80859375f, 0.75f, 1.0f }, { 490.0f / 512.0f, 370.0f / 392.0f } },
	{ L"Opaque 256x392r6", { 0.75f, 0.80859375f, 0.875f, 1.0f }, { 234.0f / 256.0f, 370.0f / 392.0f } },
	{ L"Opaque 256x256r6", { 0.875f, 0.80859375f, 1.0f, 0.95703125f }, { 234.0f / 256.0f, 234.0 / 256.0f } },								// 4
	{ L"Opaque 256x136r6", { 0.875f, 0.95703125f, 1.0f, 1.0f }, { 234.0f / 256.0f, 114.0f / 136.0f } },

	{ L"Transparent 1024x512r8", { 0.0f, 0.0f, 0.5f, 0.25f }, { 1000.0f / 1024.0f, 488.0f / 512.0f } },
	{ L"Transparent 1024x256r8", { 0.0f, 0.25f, 0.5f, 0.375f }, { 1000.0f / 1024.0f, 232.0f / 256.0f } },
	{ L"Transparent 1024x128r8", { 0.0f, 0.375f, 0.5f, 0.4375f }, { 1000.0f / 1024.0f, 104.0f / 128.0f } },									// 8
	{ L"Transparent 1024x64r4", { 0.0f, 0.4375f, 0.5f, 0.46875f }, { 1004.0f / 1024.0f, 44.0f / 64.0f } },
	{ L"Transparent 1024x48r4", { 0.0f, 0.46875f, 0.5f, 0.5f }, { 1004.0f / 1024.0f, 28.0f / 48.0f } },
	{ L"Transparent 512x1024r8", { 0.5f, 0.0f, 0.75f, 0.5f }, { 488.0f / 512.0f, 1000.0f / 1024.0f } },
	{ L"Transparent 512x512r8", { 0.75f, 0.0f, 1.0f, 0.25f }, { 488.0f / 512.0f, 488.0f / 512.0f } },											// 12
	{ L"Transparent 256x512r8", { 0.75f, 0.25f, 0.875f, 0.5f }, { 232.0f / 256.0f, 488.0f / 512.0f } },
	{ L"Transparent 256x256r8", { 0.875f, 0.25f, 1.0f, 0.375f }, { 232.0f / 256.0f, 232.0f / 256.0f } },
	{ L"Transparent 256x128r8", { 0.875f, 0.375f, 1.0f, 0.4375f }, { 232.0f / 256.0f, 104.0f / 128.0f } },
	{ L"Transparent 128x128r8", { 0.875f, 0.4375f, 0.9375f, 0.5f }, { 104.0f / 128.0f, 104.0f / 128.0f } },									// 16
	{ L"Transparent 128x64r4", { 0.9375f, 0.4375f, 1.0f, 0.46875f }, { 108.0f / 128.0f, 44.0f / 64.0f } },
	{ L"Transparent 128x48r4", { 0.9375f, 0.46875f, 1.0f, 0.5f }, { 108.0f / 128.0f, 28.0f / 48.0f } },

	{ L"Dark 1024x128r8", { 0.0f, 0.625f, 0.5f, 0.6875f }, { 1000.0f / 1024.0f, 104.0f / 128.0f } },
	{ L"Dark 512x128r8", { 0.5f, 0.625f, 0.75f, 0.6875f }, { 464.0f / 512.0f, 104.0f / 128.0f } },												// 20
	{ L"Dark 256x128r8", { 0.75f, 0.625f, 0.875f, 0.6875f }, { 232.0f / 256.0f, 104.0f / 128.0f } },
	{ L"Dark 1024x96r8", { 0.0f, 0.6875f, 0.5f, 0.734375f }, { 1000.0f / 1024.0f, 72.0f / 96.0f } },
	{ L"Dark 512x96r8", { 0.5f, 0.6875f, 0.75f, 0.734375f }, { 488.0f / 512.0f, 104.0f / 128.0f } },
	{ L"Dark 256x96r8", { 0.75f, 0.6875f, 0.875f, 0.734375f }, { 232.0f / 256.0f, 72.0f / 96.0f } },											// 24
	{ L"Dark 512x64r4", { 0.0f, 0.734375f, 0.25f, 0.765625f }, { 492.0f / 512.0f, 44.0f / 64.0f } },
	{ L"Dark 256x64r4", { 0.25f, 0.734375f, 0.375f, 0.765625f }, { 236.0f / 256.0f, 44.0f / 64.0f } },
	{ L"Dark 128x64r4", { 0.375f, 0.734375f, 0.4375f, 0.765625f }, { 108.0f / 128.0f, 44.0f / 64.0f } },
	{ L"Dark 504x32r2", { 0.501953125f, 0.736328125f, 0.748046875f, 0.751953125f }, { 492.0f / 504.0f, 22.0f / 32.0f } },				// 28
	{ L"Dark 248x32r2", { 0.751953125f, 0.736328125f, 0.873046875f, 0.751953125f }, { 236.0f / 248.0f, 22.0f / 32.0f } },
	{ L"Dark 120x32r2", { 0.876953125f, 0.736328125f, 0.935546875f, 0.751953125f }, { 108.0f / 120.0f, 22.0f / 32.0f } },
	{ L"Dark 504x24r2", { 0.501953125f, 0.751953125f, 0.748046875f, 0.763671875f }, { 492.0f / 504.0f, 14.0f / 24.0f } },
	{ L"Dark 248x24r2", { 0.751953125f, 0.751953125f, 0.873046875f, 0.763671875f }, { 236.0f / 248.0f, 14.0f / 24.0f } },				// 32
	{ L"Dark 120x24r2", { 0.876953125f, 0.751953125f, 0.935546875f, 0.763671875f }, { 108.0f / 120.0f, 14.0f / 24.0f } },

	{ L"ButtonUp 1024x128r8", { 0.0f, 0.5f, 0.5f, 0.5625f }, { 1000.0f / 1024.0f, 104.0f / 128.0f } },
	{ L"ButtonDn 1024x128r8", { 0.5f, 0.5f, 1.0f, 0.5625f }, { 1000.0f / 1024.0f, 104.0f / 128.0f } },
	{ L"ButtonUp 512x128r8", { 0.0f, 0.5625f, 0.25f, 0.625f }, { 488.0f / 512.0f, 104.0f / 128.0f } },											// 36
	{ L"ButtonDn 512x128r8", { 0.25f, 0.5625f, 0.5f, 0.625f }, { 488.0f / 512.0f, 104.0f / 128.0f } },
	{ L"ButtonUp 256x128r8", { 0.5f, 0.5625f, 0.625f, 0.625f }, { 232.0f / 256.0f, 104.0f / 128.0f } },
	{ L"ButtonDn 256x128r8", { 0.625f, 0.5625f, 0.75f, 0.625f }, { 232.0f / 256.0f, 104.0f / 128.0f } },
	{ L"ButtonUp 128x128r8", { 0.75f, 0.5625f, 0.8125f, 0.625f }, { 104.0f / 128.0f, 104.0f / 128.0f } },										// 40
	{ L"ButtonDn 128x128r8", { 0.8125f, 0.5625f, 0.875f, 0.625f }, { 104.0f / 128.0f, 104.0f / 128.0f } },
	{ L"ButtonUp 128x64r4", { 0.875f, 0.5625f, 0.9375f, 0.59375f }, { 108.0f / 128.0f, 44.0f / 64.0f } },
	{ L"ButtonDn 128x64r4", { 0.9375f, 0.5625f, 1.0f, 0.59375f }, { 108.0f / 128.0f, 44.0f / 64.0f } },
	{ L"ButtonUp 64x64r4", { 0.875f, 0.59375f, 0.90625f, 0.625f }, { 44.0f / 64.0f, 44.0f / 64.0f } },											// 44
	{ L"ButtonDn 64x64r4", { 0.90625f, 0.59375f, 0.9375f, 0.625f }, { 44.0f / 64.0f, 44.0f / 64.0f } },
	{ L"ButtonUp 56x32r2", { 0.939453125f, 0.595703125f, 0.966796875f, 0.611328125f }, { 44.0f / 56.0f, 22.0f / 32.0f } },
	{ L"ButtonDn 56x32r2", { 0.970703125f, 0.595703125f, 0.998046875f, 0.611328125f }, { 44.0f / 56.0f, 22.0f / 32.0f } },
	{ L"ButtonUp 56x24r2", { 0.939453125f, 0.611328125f, 0.966796875f, 0.623046875f }, { 44.0f / 56.0f, 14.0f / 24.0f } },				// 48
	{ L"ButtonDn 56x24r2", { 0.970703125f, 0.611328125f, 0.998046875f, 0.623046875f }, { 44.0f / 56.0f, 14.0f / 24.0f } },

	{ L"Indicator 128r8", { 0.875f, 0.625f, 0.890625f, 0.6875f }, { 0.5f, 0.75f } },
	{ L"Indicator 96r8", { 0.890625f, 0.625f, 0.90625f, 0.6875f }, { 0.5f, 64.0f / 96.0f } },
	{ L"Indicator 80r8", { 0.875f, 0.6875f, 0.890625f, 0.734375f }, { 0.5f, 48.0f / 80.0f } },													// 52
	{ L"Indicator 64r8", { 0.890625f, 0.6875f, 0.90625f, 0.734375f }, { 0.5f, 0.5f } },
	{ L"Indicator 104r4", { 0.90625f, 0.625f, 0.921875f, 0.6875f }, { 0.25f, 80.0f / 104.0f } },
	{ L"Indicator 88r4", { 0.921875f, 0.625f, 0.9375f, 0.6875f }, { 0.25f, 64.0f / 88.0f } },
	{ L"Indicator 72r4", { 0.9375f, 0.625f, 0.953125f, 0.6875f }, { 0.25f, 48.0f / 72.0f } },														// 56
	{ L"Indicator 72r4 #2", { 0.90625f, 0.6875f, 0.921875f, 0.734375f }, { 0.25f, 48.0f / 72.0f } },
	{ L"Indicator 64r4", { 0.921875f, 0.6875f, 0.9375f, 0.734375f }, { 0.25f, 40.0f / 64.0f } },
	{ L"Indicator 64r4 #2", { 0.43798828125f, 0.734375f, 0.44970703125f, 0.765625f }, { 0.25f, 40.0f / 64.0f } },
	{ L"Indicator 48r4", { 0.9375f, 0.6875f, 0.953125f, 0.734375f }, { 0.25f, 24.0f / 48.0f } },													// 60
	{ L"Indicator 48r4 #2", { 0.44970703125f, 0.73828125f, 0.46142578125f, 0.76171875f }, { 0.25f, 24.0f / 48.0f } },
	{ L"Indicator 84r2", { 0.9541015625f, 0.6357421875f, 0.96875f, 0.6767578125f }, { 0.125f, 60.0f / 84.0f } },
	{ L"Indicator 52r2", { 0.9541015625f, 0.6982421875f, 0.96875f, 0.7236328125f }, { 0.125f, 32.0f / 52.0f } },
	{ L"Indicator 52r2 #2", { 0.46142578125f, 0.736328125f, 0.47314453125f, 0.763671875f }, { 0.125f, 32.0f / 52.0f } },					// 64
	{ L"Indicator 40r2", { 0.9375f, 0.736328125f, 0.953125f, 0.751953125f }, { 0.125f, 20.0f / 40.0f } },
	{ L"Indicator 36r2", { 0.47314453125f, 0.7412109375f, 0.48486328125f, 0.7587890625f }, { 0.125f, 26.0f / 36.0f } },
	{ L"Indicator 32r2", { 0.95166015625f, 0.736328125f, 0.96337890625f, 0.751953125f }, { 0.125f, 12.0f / 32.0f } },
	{ L"Indicator 32r2 #2", { 0.94189453125f, 0.751953125f, 0.94970703125f, 0.763671875f }, { 0.125f, 12.0f / 32.0f } },					// 68
	{ L"Indicator 24r2", { 0.95361328125f, 0.75390625f, 0.96142578125f, 0.76171875f }, { 0.125f, 4.0f / 24.0f } },
	{ L"Indicator 34r1", { 0.96533203125f, 0.736328125f, 0.97314453125f, 0.751953125f }, { 0.0625f, 16.0f / 34.0f } },
	{ L"Indicator 30r1", { 0.97705078125f, 0.73828125f, 0.98486328125f, 0.75f }, { 0.0625f, 12.0f / 30.0f } },
	{ L"Indicator 26r1", { 0.96533203125f, 0.751953125f, 0.97314453125f, 0.763671875f }, { 0.0625f, 8.0f / 26.0f } },						// 72
	{ L"Indicator 22r1", { 0.97802734375f, 0.75390625f, 0.98388671875f, 0.76171875f }, { 0.0625f, 4.0f / 22.0f } },
	{ L"Indicator 96r8pin #1", { 0.96923828125f, 0.6328125f, 0.98486328125f, 0.6796875f }, { 0.25f, 78.0f / 96.0f } },
	{ L"Indicator 96r8pin #2", { 0.98486328125f, 0.6328125f, 1.0f, 0.6796875f }, { 0.25f, 78.0f / 96.0f } },
	{ L"Indicator 64r8pin #1", { 0.96923828125f, 0.6953125f, 0.98486328125f, 0.7265625f }, { 0.25f, 44.0f / 64.0f } },					// 76
	{ L"Indicator 64r8pin #2", { 0.984375f, 0.6875f, 1.0f, 0.734375f }, { 0.25f, 44.0f / 64.0f } },
	{ L"Indicator 56r5pin", { 0.48388671875f, 0.73828125f, 0.49755859375f, 0.76171875f }, { 20.0f / 32.0f, 38.0f / 56.0f } },
	{ L"Indicator 34r3.5pin", { 0.98876953125f, 0.7373046875f, 0.99658203125f, 0.7509765625f }, { 14.0f / 32.0f, 16.3858f / 34.0f } },
	{ L"Indicator 26r3.5pin", { 0.98876953125f, 0.7529296875f, 0.99658203125f, 0.7626953125f }, { 14.0f / 32.0f, 8.3858f / 26.0f } }	// 80
};

// Memory location to write wheel tilt data to
extern si32 &mouseWheelTilt;

// Thread status strings
al16 cwchptr renderWndClass = L"LV_D3D";
     cwchptr rndrWndTitle   = L"The Last Vigil :: Direct3D 11 render window";
     vui64   THREAD_LIFE_V  = 0;	// D3D11 subthread flags

// Early develepment only...
RESOLUTION ScrRes = { 3200, 1800, 9.0f / 16.0f, 16.0f / 9.0f, 1.0f, DXGI_FORMAT_R10G10B10A2_UNORM, DXGI_FORMAT_R24_UNORM_X8_TYPELESS, 1, -1, 8, 0,
							 3840, 2160, 9.0f / 16.0f, 16.0f / 9.0f, 1.0f, DXGI_FORMAT_R10G10B10A2_UNORM, DXGI_FORMAT_D32_FLOAT, 1, -1, 8, 0,
							 3840, 2160, 9.0f / 16.0f, 16.0f / 9.0f, 2.2f, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_D32_FLOAT, 1, -1, 8, 0,
							 320, 160, 0, 0, 0 };

void Direct3D11Thread(ptr argList) {
	CLASS_GPU gpu;

	CLASS_TIMER  frameTimer;
	CLASS_TIMERS animTimer;

	GLOBALCTRLVARS gcvLocal {};
	GLOBALCOORDS   gcoLocal {};

	vsi64ptr const gcoPtr = (vsi64ptr)&gco;
	si64ptr  const gcoLocalPtr = (si64ptr)&gcoLocal;

	HWND     hRndrWnd;
	ui64     threadLife;
	MARGINS  wndMargins { -1, -1, -1, -1 };
	ui32     iTotalFrames = 0, iCurrentFrames = 0;

	cui32 sizes[8] = {
		sizeof(gpu),
		sizeof(gpu.gui),
		sizeof(MAP),
		sizeof(MAP_DESC),
		sizeof(CELL),
		sizeof(GUI_SPRITE),
		sizeof(GUI_ELEMENT),
		sizeof(GUI_EL_DGS)
	};

	// Prevent thread from shutting down (after engine reset)
	THREAD_LIFE &= ~VIDEO_THREAD_DIED;

	hWnd = hRndrWnd = gpu.CreateRenderWindow();

Reinitialise_:
	THREAD_LIFE &= ~VIDEO_THREAD_RESET;

	HANDLE waitGPU = gpu.InitialiseBackbuffer(hRndrWnd, ScrRes.window.w, ScrRes.window.h, ScrRes.window.fmtBB, 8, 1, true);
//	gpu.InitialiseBackbuffer(hRndrWnd, ScrRes.borderless.w, ScrRes.borderless.h, ScrRes.borderless.fmtBB, 8, 1, true);
//	gpu.InitialiseBackbuffer(hRndrWnd, ScrRes.full.w, ScrRes.full.h, ScrRes.full.fmtBB, 8, 1, false);
	gpu.cfg.LoadAllShaderGroups();
	gpu.cfg.CreateVertexFormat(0, 0, 0);
	gpu.cfg.CreateVertexFormat(1, 1, 4);
	gpu.cfg.CreateVertexFormat(2, 2, 2);
	gpu.cfg.CreateVertexFormat(3, 3, 3);

	gpu.cfg.CreateSamplerState(0, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_COMPARISON_ALWAYS, D3D11_FILTER_MIN_MAG_MIP_POINT, 16);
	gpu.cfg.CreateSamplerState(0, D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_COMPARISON_ALWAYS, D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR, 16);
	gpu.cfg.CreateSamplerState(0, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_COMPARISON_ALWAYS, D3D11_FILTER_ANISOTROPIC, 16);
	gpu.cfg.SetSamplerStates(0, 0, 3, 0);
	gpu.cfg.CreateBlendStates();
	gpu.cfg.CreateDepthStencilState(false, D3D11_DEPTH_WRITE_MASK_ZERO, D3D11_COMPARISON_NEVER, false, NULL, NULL);
	gpu.cfg.CreateDepthStencilState(false, D3D11_DEPTH_WRITE_MASK_ALL, D3D11_COMPARISON_LESS, false, NULL, NULL);
	gpu.cfg.CreateDepthStencilState(true, D3D11_DEPTH_WRITE_MASK_ZERO, D3D11_COMPARISON_NEVER, false, NULL, NULL);
	gpu.cfg.CreateDepthStencilState(true, D3D11_DEPTH_WRITE_MASK_ALL, D3D11_COMPARISON_LESS, false, NULL, NULL);
	gpu.cfg.CreateCullingState();
	gpu.cfg.SetCullingState(0, 0);

	si32 siTextureSet[4];
	gpu.tex.ConfigData(-1, -1, -1, 8, D3D11_USAGE_IMMUTABLE, false, false, true, false, false);
	siTextureSet[0] = gpu.tex.LoadTextureSet(L"1024", SPRITE);
	siTextureSet[1] = gpu.tex.LoadTextureSet(L"1024", TERRAIN);
	gpu.tex.SetPS2D(0, siTextureSet[0], 20, 3);
	gpu.tex.SetPS2D(0, siTextureSet[1], 16, 3);

	CLASS_MAPMANAGER    mapMan;
	CLASS_ENTITYMANAGER entMan;
	CLASS_D3D11HELPER   gpuHelper(gpu, mapMan, entMan);

	// Test map
	mapMan.CreatePeriodicTable((wchptrc)L"Main periodic table", 5, 0);
	mapMan.SetElementName(0, 0, (wchptrc)L"Air");
	mapMan.SetElementTemps(0, 0, 0.0f, 100.0f, 1000.0f, 1.0f, 10000.0f);
	mapMan.SetElementMiscVars(0, 0, 0.0f, { 0.0f, 0.5f }, { 1.125f, 1.0f }, 0.0f);
	mapMan.SetElementResults(0, 0, 0, 0);
	mapMan.SetElementGeometry(0, 0, { 0.0f, 0.0f, 0.0f, 0.0f }, 1.0f, 1.0f, 1.0f, 0, 0, 16);
	mapMan.SetElementName(0, 1, (wchptrc)L"Rough rock");
	mapMan.SetElementTemps(0, 1, 0.0f, 100.0f, 1000.0f, 1.0f, 10000.0f);
	mapMan.SetElementMiscVars(0, 1, 0.0f, { 0.0f, 0.5f }, { 0.875f, 1.0f }, 0.0f);
	mapMan.SetElementResults(0, 1, 0, 0);
	mapMan.SetElementGeometry(0, 1, { 0.0f, 0.0f, 0.5f, 0.5f }, 1.0f, 0.0f, 1.0f, 0, 0, 16);
	mapMan.SetElementName(0, 2, (wchptrc)L"_element_002_");
	mapMan.SetElementTemps(0, 2, 0.0f, 100.0f, 1000.0f, 1.0f, 10000.0f);
	mapMan.SetElementMiscVars(0, 1, 0.0f, { 0.0f, 0.5f }, { 0.875f, 1.0f }, 0.0f);
	mapMan.SetElementResults(0, 2, 0, 0);
	mapMan.SetElementGeometry(0, 2, { 0.5f, 0.0f, 1.0f, 0.5f }, 1.0f, 0.0f, 1.0f, 0, 0, 16);
	mapMan.SetElementName(0, 3, (wchptrc)L"_element_003_");
	mapMan.SetElementTemps(0, 3, 0.0f, 100.0f, 1000.0f, 1.0f, 10000.0f);
	mapMan.SetElementMiscVars(0, 1, 0.0f, { 0.0f, 0.5f }, { 0.875f, 1.0f }, 0.0f);
	mapMan.SetElementResults(0, 3, 0, 0);
	mapMan.SetElementGeometry(0, 3, { 0.0f, 0.5f, 0.5f, 1.0f }, 1.0f, 0.0f, 1.0f, 0, 0, 16);
	mapMan.SetElementName(0, 4, (wchptrc)L"_element_004_");
	mapMan.SetElementTemps(0, 4, 0.0f, 100.0f, 1000.0f, 1.0f, 10000.0f);
	mapMan.SetElementMiscVars(0, 1, 0.0f, { 0.0f, 0.5f }, { 0.875f, 1.0f }, 0.0f);
	mapMan.SetElementResults(0, 4, 0, 0);
	mapMan.SetElementGeometry(0, 4, { 0.5f, 0.5f, 1.0f, 1.0f }, 1.0f, 0.0f, 1.0f, 0, 0, 16);

	mapMan.CreateWorld(0, 0, 1);

	MAP_DESC md;
	md.stName = (wchptrc)L"Test map";
	md.stInfo = (wchptrc)L"Description text.";
	md.ptIndex = 0;
	md.chunkDim = { 16, 16, 1 };
	md.mapDim = { { 1024, 1024, 8 } }; // 256x144x1 -> 4,718,592 triangles -> Approx. 1.68 billion triangles per second @ 4K
	md.zso = 4;
	si32 mapID = mapMan.CreateMap(md, 0, -1, 0, 2);

	csi32 numEntities = 1024;
	csi32 numParts    = 32;

	entMan.CreateObjectGroup((wchptrc)L"Test group", (wchptrc)L"No description", 1, numParts);
	entMan.CreateEntityGroup((wchptrc)L"Test group", (wchptrc)L"No description", numEntities, numEntities * numParts);
	entMan.CreateObject(0, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.5f, 0.5f }, 20, numParts);
	for(si32 k = 1; k < numParts; k++)
		entMan.SetPart(0, k, { (float(k & 7) - 3.5f) * 0.9375f * 0.25f, (float(k >> 3) - 1.5f) * 0.9375f * 0.25f, -0.25f }, { 0.0f, 0.0f, 0.0f },
							{ 0.2f, 0.2f }, { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f }, { 0.5f, 0.0f, 1.0f, 0.5f }, 20);
	for(float i = 0.0f; i < 4.0f; i++)
		for(float j = 0.0f; j < 256.0f; j++)
			entMan.CreateEntity(0, { 0, 0 }, { j - 128.0f, i - 64.0f, -1.5f }, { 0.0f, 0.0f, 0.0f }, { 0.4f, 0.4f, 0.4f });

	gpuHelper.map.CreateBuffers(0, 0, 0);
	gpuHelper.ent.CreateBuffers(0);

	cui32 uiAtlasIndex[2] = { gpu.gui.LoadAtlas(L"font.c_a_gatintas.eng", 80), gpu.gui.LoadAtlas(L"panels.blue_rust", 82) };

	cui32 GUISprites = gpu.gui.CreateSpriteLibrary(0, 81, uiAtlasIndex[1]);
	for(ui32 i = 0; i < 81; i++)
		gpu.gui.CreateSprite(GUISprites, (GUI_SPRITE &)defaultUISprite[i], true);

	cui32 panelElement0 = gpu.gui.CreateToggle(0, 34, { 0.0f, 0.0f }, { 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, 0x05);
	cui32 panelElement1 = gpu.gui.CreateButton(0, 34, { 0.0f, 0.25f }, { 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, 0x05);
	cui32 panelElement2 = gpu.gui.CreateScalar(0, 20, 75, { 0.0f, 0.5f }, { 2.0f, 2.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, 0x05);
	gpu.gui.element[panelElement0].func.hover[0] = onHover;
	gpu.gui.element[panelElement0].func.hover[1] = offHover;

	cui32 alphabetIndex = gpu.gui.CreateAlphabet(L"Bogan", 16, 16, 0.5f, 0, uiAtlasIndex[0]);
	gpu.gui.UploadAlphabetBuffer(0);
	char  textBuffer[128] = "TR-X \1\2\3\4\5\6\7";
	csi32 textBufferSize  = si32(strlen(textBuffer));
	cui32 textElement = gpu.gui.CreateText(textBuffer, 128, alphabetIndex, { 0.0f, 0.0f }, { 0.0468757f, 0.0468757f }, { 1.0f, 1.0f, 1.0f, 1.0f }, 0x05);
	char textInputs[128] = "<Input list>";
	cui32 textInputEl = gpu.gui.CreateText(textInputs, 128, alphabetIndex, { 0.0f, 0.0f }, { 0.0468757f, 0.0468757f }, { 1.0f, 1.0f, 1.0f, 1.0f }, 0x06);
	char textBoxText[128] = "Ya Mum!";
	cVEC2Du32 textBox = gpu.gui.CreateTextBox(textBoxText, 128, alphabetIndex, { 0.25f, 0.25f }, { 1.0f, 1.0f, 1.0f, 1.0f }, 0, 12, { 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f }, 0x02, 0x0);

	cui32 interfaceMain = gpu.gui.CreateInterface(128, 16);
	gpu.gui.AddElementToInterface(panelElement0, interfaceMain);
	gpu.gui.AddElementToInterface(panelElement1, interfaceMain);
	gpu.gui.AddElementToInterface(panelElement2, interfaceMain);
	gpu.gui.AddElementToInterface(textElement, interfaceMain);
	gpu.gui.AddElementToInterface(textInputEl, interfaceMain);
	gpu.gui.AddElementToInterface(textBox, interfaceMain);

	//	gpu.files.SaveAlphabetData(L"font.main.eng.cfg", english);
	//	gpu.files.SaveMap(L"test.aemap", mapMan.world[mapID]);

	gpu.lit.Create(0, 256, 0);
	gpu.lit.SetColour(0, { 0.25f, 0.25f, 0.25f });
	gpu.lit.n[0].pos          = { -256.0f, 256.0f, -256.0f };
	gpu.lit.n[0].range        = 1024.0f;
	gpu.lit.n[0].hl.size      = 127;
	gpu.lit.n[0].hl.intensity = 63;
	gpu.lit.SetColour(1, { 1.0f, 1.0f, 1.0f });
	gpu.lit.n[1].pos          = { 0.0f, 0.0f, -12.0f };
	gpu.lit.n[1].range        = 8.0f;
	gpu.lit.n[1].hl.size      = 127;
	gpu.lit.n[1].hl.intensity = 127;
	gpu.lit.SetColour(2, { 2.0f, 0.001f, 0.001f });
	gpu.lit.n[2].pos          = { 0.0f, 16.0f, -8.0f };
	gpu.lit.n[2].range        = 8.0f;
	gpu.lit.n[2].hl.size      = 255;
	gpu.lit.n[2].hl.intensity = 63;
	gpu.lit.SetColour(3, { 0.001f, 0.001f, 2.0f });
	gpu.lit.n[3].pos          = { 14.0f, -8.0f, -8.0f };
	gpu.lit.n[3].range        = 8.0f;
	gpu.lit.n[3].hl.size      = 255;
	gpu.lit.n[3].hl.intensity = 63;
	gpu.lit.SetColour(4, { 0.001f, 2.0f, 0.001f });
	gpu.lit.n[4].pos          = { -14.0f, -8.0f, -8.0f };
	gpu.lit.n[4].range        = 8.0f;
	gpu.lit.n[4].hl.size      = 255;
	gpu.lit.n[4].hl.intensity = 63;
	gpu.lit.SetColour(5, { 1.0f, 1.0f, 1.0f });
	gpu.lit.n[5].pos          = { -13.0f, 7.0f, -4.0f };
	gpu.lit.n[5].range        = 8.0f;
	gpu.lit.n[5].hl.size      = 63;
	gpu.lit.n[5].hl.intensity = 63;
	gpu.lit.SetColour(6, { 0.7071f, 0.7071f, 0.7071f });
	gpu.lit.n[6].pos          = { 13.0f, 7.0f, -4.0f };
	gpu.lit.n[6].range        = 8.0f;
	gpu.lit.n[6].hl.size      = 255;
	gpu.lit.n[6].hl.intensity = 63;
	gpu.lit.UploadAll(0);

	al16 fl32 fAngle = 0.0f, fScale = 1.0f;
	ui8 bMouseCursor = true;

	gpu.cam.CreateCamera(0, STAGES_VERTEX_GEOMETRY, 0);
	gpu.cam.s[0].fXpos = 0.0f; gpu.cam.s[0].fYpos = 0.0f; gpu.cam.s[0].fZpos = -256.0f;
	gpu.cam.s[0].fXrot = 0.0f; gpu.cam.s[0].fYrot = 0.0f; gpu.cam.s[0].fZrot = 0.0f;
	gpu.cam.s[0].fZoom = 1.0f; gpu.cam.s[0].fSize = 256.0f;
	gpu.cam.s[0].fWidth  = ScrRes.dims[ScrRes.state].aspectI;
	gpu.cam.s[0].fHeight = 1.0f;
	gpu.cam.s[0].fNearZ = 1.0f;
	gpu.cam.s[0].fFarZ = 16384.0f;

	// Display the render window
	gpu.SetBorderedWindow(hWnd);
//	gpu.SetBorderlessWindow(hWnd);
//	gpu.SetFullscreenWindow(hWnd);

// Initialise timer
	frameTimer.Init();
	frameTimer.Reset(1.0);
	ui8 tRibbons = animTimer.Init();
	animTimer.Reset(1.0, tRibbons);
	ui8 tPlane = animTimer.Init();
	animTimer.Reset(7.0, tPlane);
	ui8 index = 0, step = 1;
	double dPlaneAnimTime = 0.0;
	VEC3Df vCurPos {};
	si32 siCell = 0, siChunk = 0;
	al16 union { VEC3Ds32 sivActiveCell {}; VEC2Ds32 sivActivePlane; };
	ui32 uiFrameCount = 0, uiVisChunks = 0;
	fl32 fAvgFrameTime = 0;
	fl64 dTrisPerSec = 0;
	// Primary rendering loop
	do {
		threadLife = THREAD_LIFE;
		if (threadLife & VIDEO_THREAD_RESET) {
			// Shutdown Direct3D11 HAL then goto initialisation.
			gpu.lit.DestroyAll();
			DestroyWindow(hWnd);
			goto Reinitialise_;
		}
		MessagePump();

		// Update timers
		mainTimer.Update();
		frameTimer.Update();
		animTimer.UpdateAll();
		cfl64 dTotalTime        = mainTimer.GetTotalTimeScaled();
		cfl32 fElapsedTime      = float(mainTimer.GetElapsedTimeScaled());
		cfl64 dCurrentFrameTime = frameTimer.GetTotalTimeScaled();
		if (dCurrentFrameTime >= 1.0) {
			sysData.gpu.frame.rate = frameTimer.UpdatesPerSecondScaled();
			sysData.gpu.frame.time = frameTimer.GetElapsedTimeScaled();
			frameTimer.Reset(1.0);
			dTrisPerSec = double(uiVisChunks) * sysData.gpu.frame.time * 0.000000128;
		}
		dPlaneAnimTime += animTimer.GetElapsedTimeScaled(tPlane);
		if(dPlaneAnimTime > 1.0) {
			if (index > 5) step = -1;
			else if (index < 1) step = 1;
			index += step;
			dPlaneAnimTime = 0.0;
		}
		// Read controls; create function in DI8 thread to do the same
		memcpy(&gcvLocal, (void *)&gcv, sizeof(GLOBALCTRLVARS));
		gcv.misc[7] |= 0x080;
		csi32 siWheel = gcvLocal.s32.z[1].x;

		csi128 siActiveLayer = gpu.gui.ProcessInputs(gcvLocal, interfaceMain);	// If cursor is over GUI element, process
		if(siActiveLayer.m128i_i32[0] < 0) {
			sivActiveCell.z = siWheel;
			// Locate cell under cursor
			sivActivePlane = gpu.cam.CursorLayerIntersect(siWheel, cvector{0.0f, 0.0f, 0.5f}, gcvLocal.curCoords, 0, 0);
			siCell = mapMan.CalcCellIndex(sivActiveCell, 0, 0);
			siChunk = siCell / md.chunkCells;
			// Mouse button 0
			if(siActiveLayer.m128i_i32[1] < 0 && gcvLocal.imm.b[16] & 0x01) {
				if(siCell != 0x080000001) {
					mapMan.world[0].map[0]->cell[siCell].pixel->gev += ui16(fElapsedTime * 2048.0f);
					mapMan.world[0].map[0]->chunkMod[siChunk >> 6] |= ui64(0x01) << (siChunk & 0x03F);
				}
			}
			// Mouse button 1
			if(siActiveLayer.m128i_i32[2] < 0 && gcvLocal.imm.b[16] & 0x02) {
				if(siCell != 0x080000001) {
					mapMan.world[0].map[0]->cell[siCell].pixel->gev -= ui16(fElapsedTime * 2048.0f);
					mapMan.world[0].map[0]->chunkMod[siChunk >> 6] |= ui64(0x01) << (siChunk & 0x03F);
				}
			}
			// Mouse button 3
			if(gcvLocal.imm.b[16] & 0x08) {
				if(siCell != 0x080000001) {
					mapMan.ModQuadCellDensity(sivActiveCell, -fElapsedTime, 0, 0);
				}
			}
			// Mouse button 4
			if(gcvLocal.imm.b[16] & 0x010) {
				if(siCell != 0x080000001) {
					mapMan.ModQuadCellDensity(sivActiveCell, fElapsedTime, 0, 0);
				}
			}
			// Mouse wheel left
			if(gcvLocal.imm.b[16] & 0x020) {
				if(siCell != 0x080000001) {
					mapMan.ModQuadCellDensity(sivActiveCell, -fElapsedTime, 0, 0);
				}
			}
			// Mouse wheel right
			if(gcvLocal.imm.b[16] & 0x040) {
				if(siCell != 0x080000001) {
					mapMan.ModQuadCellDensity(sivActiveCell, fElapsedTime, 0, 0);
				}
			}
			if(gcvLocal.imm.b[4] & 0x01)
				if(siCell != 0x080000001) {
					mapMan.world[0].map[0]->cell[siCell].geometry->et.x = (gcvLocal.misc[1] & 0x03) + 1;
					mapMan.world[0].map[0]->chunkMod[siChunk >> 6] |= ui64(0x01) << (siChunk & 0x03F);
				}
		}
		// Mouse button 2
		if(gcvLocal.imm.b[16] & 0x04) {
			if(bMouseCursor) {
//					gpu.MouseCursor(hWnd, false);
				bMouseCursor = false;
			}
			gpu.cam.s[0].fXrot -= gcvLocal.xy[1].y2;
			gpu.cam.s[0].fYrot += gcvLocal.xy[1].x1;
		} else
			if(!bMouseCursor) {
//					gpu.MouseCursor(hWnd, true);
				bMouseCursor = true;
			}

		// Clamp camera rotation
		if(gpu.cam.s[0].fXrot < -1.0f) gpu.cam.s[0].fXrot = -1.0f;
		if(gpu.cam.s[0].fXrot > 1.0f) gpu.cam.s[0].fXrot = 1.0f;
		if(gpu.cam.s[0].fYrot < -1.0f) gpu.cam.s[0].fYrot = -1.0f;
		if(gpu.cam.s[0].fYrot > 1.0f) gpu.cam.s[0].fYrot = 1.0f;

		// Update camera
		gpu.cam.MoveCameraForwardXZ((gcvLocal.z[0].x - gcvLocal.z[0].y) * fElapsedTime * 32.0f, 0);
		gpu.cam.MoveCameraRightXZ((gcvLocal.xy[0].x2 - gcvLocal.xy[0].x1) * fElapsedTime * 16.0f, 0);
		gpu.cam.MoveCameraUpY((gcvLocal.xy[0].y1 - gcvLocal.xy[0].y2) * fElapsedTime * 16.0f, 0);
		gpu.cam.TransformProjections(0);
		gpu.cam.TransformCamera(0, 0);

		// Cull out-of-view entities and map chunks
		gpuHelper.ent.StartViewCulling(0);
		gpuHelper.map.StartViewCulling(0, 0);

		// Upload camera
		gpu.cam.UploadProjections(NULL, 0);
		gpu.cam.UploadCamera(dTotalTime, fElapsedTime, uiFrameCount++, NULL, 0);

		// Update audio listener data
		gcoLocal.vel.x = gcoLocal.pos.x;
		gcoLocal.vel.y = gcoLocal.pos.y;
		gcoLocal.vel.z = gcoLocal.pos.z;
		gcoLocal.pos.x = gpu.cam.s[0].fXpos;
		gcoLocal.pos.y = -gpu.cam.s[0].fYpos;
		gcoLocal.pos.z = gpu.cam.s[0].fZpos;
		gcoLocal.ori.face.x = gpu.cam.vCamDir.m128_f32[0];
		gcoLocal.ori.face.y = -gpu.cam.vCamDir.m128_f32[1];
		gcoLocal.ori.face.z = gpu.cam.vCamDir.m128_f32[2];
		gcoLocal.ori.up.x = gpu.cam.vCamUp.m128_f32[0];
		gcoLocal.ori.up.y = -gpu.cam.vCamUp.m128_f32[1];
		gcoLocal.ori.up.z = gpu.cam.vCamUp.m128_f32[2];
		// Forcing volatile copy to avoid glitches 
		for(ui8 xx = 0; xx < sizeof(GLOBALCOORDS) >> 3; xx++)
			gcoPtr[xx] = _InterlockedExchange64(&gcoLocalPtr[xx], 0);

		//WaitForSingleObjectEx(waitGPU, 1000, false);
		gpu.ClearFlipRenderViews(VEC4Df { 0.0f, 0.0f, 0.0f, 0.0f });

		// Entities: Wait until visibility & modification culling completes, then upload modified entity data and render
		cVEC4Du32 entManThreadData = gpuHelper.ent.WaitThenUploadAndRender(0);

		// Map: Wait until visibility & modification culling completes, then upload modified cell data and render
		cVEC4Du32 mapManThreadData = gpuHelper.map.WaitThenUploadAndRender(0, 0);
		uiVisChunks = mapManThreadData.x;

		// Render 3D overlays
		gpu.cfg.SetBlendState(0, 1);
		gpu.cfg.SetDepthStencilState(0, 0, 0);
		if(siActiveLayer.m128i_i32[0] < 0 && siCell != 0x080000001) gpu.ren.DrawVoxel(sivActiveCell, { 223, 191, 255, 255 });

		// Render GUI
		snprintf(textBuffer, 128, "Pos: <%.3f, %.3f, %.3f>   Rot: <%.3f, %.3f, %.3f>   Cull time: %.3fms   Mouse tilt: %d   Entities: %d",
					gpu.cam.s[0].fXpos, gpu.cam.s[0].fYpos, gpu.cam.s[0].fZpos, gpu.cam.s[0].fXrot, gpu.cam.s[0].fYrot, gpu.cam.s[0].fZrot,
					sysData.culling.entity.time, gcvLocal.s32.z[1].y, sysData.culling.entity.vis[0]);
		gpu.gui.UpdateText(textBuffer, textElement);
		snprintf(textInputs, 128, "0x0%02X 0x0%02X 0x0%02X 0x0%02X", inputsImmediate.x, inputsImmediate.y, inputsImmediate.z, inputsImmediate.w);
		gpu.gui.UpdateText(textInputs, textInputEl);
		snprintf(textBoxText, 128, "Ya Mum!");
		gpu.gui.UpdateText(textBoxText, textBox.y);
		gpu.gui.UploadTextBuffer(0);
		gpu.gui.UploadEntryBuffer(0);
		gpu.gui.SetResources(0, 0, 2);
		gpu.gui.DrawInterface(0, interfaceMain);

		// Display backbuffer
		gpu.PresentRenderTarget(0);

		Sleep(0);
	} while (threadLife & VIDEO_THREAD_ALIVE);

	mapMan.Cull(0, 0, 0, 0, 0);
	gpu.lit.DestroyAll();
	gpu.cfg.UnloadShaderData();
	DestroyWindow(hWnd);

	Sleep(8);
	THREAD_LIFE |= VIDEO_THREAD_DIED;
	//_endthread();
}

inline void MessagePump(void) {
	static al16 MSG msg {};

	if(PeekMessage(&msg, NULL, WM_MOUSEHWHEEL, WM_MOUSEHWHEEL, PM_REMOVE))
		DispatchMessage(&msg);
	else
		mouseWheelTilt = 0;
	if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		DispatchMessage(&msg);
}

LRESULT CALLBACK RndrWndProc(HWND hWndw, UINT message, WPARAM wParam, LPARAM lParam) {

	switch (message) {
	case WM_MOUSEHWHEEL:
		mouseWheelTilt = (si32)((si16 (&)[2])wParam)[1];
		return 0;
	case WM_DESTROY:
	case WM_CLOSE:
	case WM_QUIT:
		THREAD_LIFE &= ~MAIN_THREAD_ALIVE;
		return 0;
	default:
		return DefWindowProc(hWndw, message, wParam, lParam);
	}
}
