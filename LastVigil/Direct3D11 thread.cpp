/************************************************************
 * File: Direct3D11 thread.cpp          Created: 2022/10/12 *
 *                               Code last mod.: 2024/07/04 *
 *                                                          *
 * Desc: Video rendering via Direct3D 11 API.               *
 *                                                          *
 * Copyright (c) David William Bull.   All rights reserved. *
 ************************************************************/

#include "master header.h"
#include "Direct3D11 thread.h"
#include "Direct3D11 functions.h"
#include "colours.h"
#include "Armada Intelligence/class_mapmanager.h"
#include "Armada Intelligence/class_entitymanager.h"
#include "Armada Intelligence/D3D11 helper functions.h"
#include "Armada Intelligence/GUI functions.h"
#include "Armada Intelligence/class_gui.h"

 // For development only. Insert <0,0>~<1,1> into slot#0 for non-texture panels --- !!! Now obsolete !!!
al32 struct GUI_SPRITE_DEFAULTS { cchptrc name; VEC4Df tc; VEC2Df aa; } defaultUISprite[82] = {
   { "Procedural", { 0.0f, 0.0f, 1.0f, 1.0f }, { 1.0f, 1.0f } },
   { "Opaque 2048x88r6", { 0.0f, 0.765625f, 1.0f, 0.80859375f }, { 2004.0f / 2048.0f, 44.0f / 88.0f } },                             // 0
   { "Opaque 1024x392r6", { 0.0f, 0.80859375f, 0.5f, 1.0f }, { 980.0f / 1024.0f, 348.0f / 392.0f } },
   { "Opaque 512x392r6", { 0.5f, 0.80859375f, 0.75f, 1.0f }, { 468.0f / 512.0f, 348.0f / 392.0f } },
   { "Opaque 256x392r6", { 0.75f, 0.80859375f, 0.875f, 1.0f }, { 212.0f / 256.0f, 348.0f / 392.0f } },
   { "Opaque 256x256r6", { 0.875f, 0.80859375f, 1.0f, 0.93359375f }, { 212.0f / 256.0f, 212.0 / 256.0f } },                          // 4
   { "Opaque 256x136r6", { 0.875f, 0.93359375f, 1.0f, 1.0f }, { 212.0f / 256.0f, 92.0f / 136.0f } },

   { "Transparent 1024x512r8", { 0.0f, 0.0f, 0.5f, 0.25f }, { 976.0f / 1024.0f, 464.0f / 512.0f } },
   { "Transparent 1024x256r8", { 0.0f, 0.25f, 0.5f, 0.375f }, { 976.0f / 1024.0f, 208.0f / 256.0f } },
   { "Transparent 1024x128r8", { 0.0f, 0.375f, 0.5f, 0.4375f }, { 976.0f / 1024.0f, 80.0f / 128.0f } },                            // 8
   { "Transparent 1024x64r4", { 0.0f, 0.4375f, 0.5f, 0.46875f }, { 984.0f / 1024.0f, 24.0f / 64.0f } },
   { "Transparent 1024x48r4", { 0.0f, 0.46875f, 0.5f, 0.5f }, { 984.0f / 1024.0f, 8.0f / 48.0f } },
   { "Transparent 512x1024r8", { 0.5f, 0.0f, 0.75f, 0.5f }, { 464.0f / 512.0f, 976.0f / 1024.0f } },
   { "Transparent 512x512r8", { 0.75f, 0.0f, 1.0f, 0.25f }, { 464.0f / 512.0f, 464.0f / 512.0f } },                                  // 12
   { "Transparent 256x512r8", { 0.75f, 0.25f, 0.875f, 0.5f }, { 208.0f / 256.0f, 464.0f / 512.0f } },
   { "Transparent 256x256r8", { 0.875f, 0.25f, 1.0f, 0.375f }, { 208.0f / 256.0f, 208.0f / 256.0f } },
   { "Transparent 256x128r8", { 0.875f, 0.375f, 1.0f, 0.4375f }, { 208.0f / 256.0f, 80.0f / 128.0f } },
   { "Transparent 128x128r8", { 0.875f, 0.4375f, 0.9375f, 0.5f }, { 80.0f / 128.0f, 80.0f / 128.0f } },                            // 16
   { "Transparent 128x64r4", { 0.9375f, 0.4375f, 1.0f, 0.46875f }, { 88.0f / 128.0f, 24.0f / 64.0f } },
   { "Transparent 128x48r4", { 0.9375f, 0.46875f, 1.0f, 0.5f }, { 88.0f / 128.0f, 8.0f / 48.0f } },

   { "Dark 1024x128r8", { 0.0f, 0.625f, 0.5f, 0.6875f }, { 976.0f / 1024.0f, 80.0f / 128.0f } },
   { "Dark 512x128r8", { 0.5f, 0.625f, 0.75f, 0.6875f }, { 464.0f / 512.0f, 80.0f / 128.0f } },                                     // 20
   { "Dark 256x128r8", { 0.75f, 0.625f, 0.875f, 0.6875f }, { 208.0f / 256.0f, 80.0f / 128.0f } },
   { "Dark 1024x96r8", { 0.0f, 0.6875f, 0.5f, 0.734375f }, { 976.0f / 1024.0f, 48.0f / 96.0f } },
   { "Dark 512x96r8", { 0.5f, 0.6875f, 0.75f, 0.734375f }, { 464.0f / 512.0f, 80.0f / 128.0f } },
   { "Dark 256x96r8", { 0.75f, 0.6875f, 0.875f, 0.734375f }, { 208.0f / 256.0f, 48.0f / 96.0f } },                                   // 24
   { "Dark 512x64r4", { 0.0f, 0.734375f, 0.25f, 0.765625f }, { 472.0f / 512.0f, 24.0f / 64.0f } },
   { "Dark 256x64r4", { 0.25f, 0.734375f, 0.375f, 0.765625f }, { 216.0f / 256.0f, 24.0f / 64.0f } },
   { "Dark 128x64r4", { 0.375f, 0.734375f, 0.4375f, 0.765625f }, { 88.0f / 128.0f, 24.0f / 64.0f } },
   { "Dark 504x32r2", { 0.501953125f, 0.736328125f, 0.748046875f, 0.751953125f }, { 480.0f / 504.0f, 12.0f / 32.0f } },              // 28
   { "Dark 248x32r2", { 0.751953125f, 0.736328125f, 0.873046875f, 0.751953125f }, { 224.0f / 248.0f, 12.0f / 32.0f } },
   { "Dark 120x32r2", { 0.876953125f, 0.736328125f, 0.935546875f, 0.751953125f }, { 96.0f / 120.0f, 12.0f / 32.0f } },
   { "Dark 504x24r2", { 0.501953125f, 0.751953125f, 0.748046875f, 0.763671875f }, { 480.0f / 504.0f, 4.0f / 24.0f } },
   { "Dark 248x24r2", { 0.751953125f, 0.751953125f, 0.873046875f, 0.763671875f }, { 224.0f / 248.0f, 4.0f / 24.0f } },              // 32
   { "Dark 120x24r2", { 0.876953125f, 0.751953125f, 0.935546875f, 0.763671875f }, { 96.0f / 120.0f, 4.0f / 24.0f } },

   { "ButtonUp 1024x128r8", { 0.0f, 0.5f, 0.5f, 0.5625f }, { 976.0f / 1024.0f, 80.0f / 128.0f } },
   { "ButtonDn 1024x128r8", { 0.5f, 0.5f, 1.0f, 0.5625f }, { 976.0f / 1024.0f, 80.0f / 128.0f } },
   { "ButtonUp 512x128r8", { 0.0f, 0.5625f, 0.25f, 0.625f }, { 464.0f / 512.0f, 80.0f / 128.0f } },                                 // 36
   { "ButtonDn 512x128r8", { 0.25f, 0.5625f, 0.5f, 0.625f }, { 464.0f / 512.0f, 80.0f / 128.0f } },
   { "ButtonUp 256x128r8", { 0.5f, 0.5625f, 0.625f, 0.625f }, { 208.0f / 256.0f, 80.0f / 128.0f } },
   { "ButtonDn 256x128r8", { 0.625f, 0.5625f, 0.75f, 0.625f }, { 208.0f / 256.0f, 80.0f / 128.0f } },
   { "ButtonUp 128x128r8", { 0.75f, 0.5625f, 0.8125f, 0.625f }, { 80.0f / 128.0f, 80.0f / 128.0f } },                              // 40
   { "ButtonDn 128x128r8", { 0.8125f, 0.5625f, 0.875f, 0.625f }, { 80.0f / 128.0f, 80.0f / 128.0f } },
   { "ButtonUp 128x64r4", { 0.875f, 0.5625f, 0.9375f, 0.59375f }, { 88.0f / 128.0f, 24.0f / 64.0f } },
   { "ButtonDn 128x64r4", { 0.9375f, 0.5625f, 1.0f, 0.59375f }, { 88.0f / 128.0f, 24.0f / 64.0f } },
   { "ButtonUp 64x64r4", { 0.875f, 0.59375f, 0.90625f, 0.625f }, { 24.0f / 64.0f, 24.0f / 64.0f } },                                 // 44
   { "ButtonDn 64x64r4", { 0.90625f, 0.59375f, 0.9375f, 0.625f }, { 24.0f / 64.0f, 24.0f / 64.0f } },
   { "ButtonUp 56x32r2", { 0.939453125f, 0.595703125f, 0.966796875f, 0.611328125f }, { 32.0f / 56.0f, 12.0f / 32.0f } },
   { "ButtonDn 56x32r2", { 0.970703125f, 0.595703125f, 0.998046875f, 0.611328125f }, { 32.0f / 56.0f, 12.0f / 32.0f } },
   { "ButtonUp 56x24r2", { 0.939453125f, 0.611328125f, 0.966796875f, 0.623046875f }, { 32.0f / 56.0f, 4.0f / 24.0f } },             // 48
   { "ButtonDn 56x24r2", { 0.970703125f, 0.611328125f, 0.998046875f, 0.623046875f }, { 32.0f / 56.0f, 4.0f / 24.0f } },

   { "Indicator 128r8", { 0.875f, 0.625f, 0.890625f, 0.6875f }, { 0.5f, 0.75f } },
   { "Indicator 96r8", { 0.890625f, 0.625f, 0.90625f, 0.6875f }, { 0.5f, 64.0f / 96.0f } },
   { "Indicator 80r8", { 0.875f, 0.6875f, 0.890625f, 0.734375f }, { 0.5f, 48.0f / 80.0f } },                                         // 52
   { "Indicator 64r8", { 0.890625f, 0.6875f, 0.90625f, 0.734375f }, { 0.5f, 0.5f } },
   { "Indicator 104r4", { 0.90625f, 0.625f, 0.921875f, 0.6875f }, { 0.25f, 80.0f / 104.0f } },
   { "Indicator 88r4", { 0.921875f, 0.625f, 0.9375f, 0.6875f }, { 0.25f, 64.0f / 88.0f } },
   { "Indicator 72r4", { 0.9375f, 0.625f, 0.953125f, 0.6875f }, { 0.25f, 48.0f / 72.0f } },                                          // 56
   { "Indicator 72r4 #2", { 0.90625f, 0.6875f, 0.921875f, 0.734375f }, { 0.25f, 48.0f / 72.0f } },
   { "Indicator 64r4", { 0.921875f, 0.6875f, 0.9375f, 0.734375f }, { 0.25f, 40.0f / 64.0f } },
   { "Indicator 64r4 #2", { 0.43798828125f, 0.734375f, 0.44970703125f, 0.765625f }, { 0.25f, 40.0f / 64.0f } },
   { "Indicator 48r4", { 0.9375f, 0.6875f, 0.953125f, 0.734375f }, { 0.25f, 24.0f / 48.0f } },                                       // 60
   { "Indicator 48r4 #2", { 0.44970703125f, 0.73828125f, 0.46142578125f, 0.76171875f }, { 0.25f, 24.0f / 48.0f } },
   { "Indicator 84r2", { 0.9541015625f, 0.6357421875f, 0.96875f, 0.6767578125f }, { 0.125f, 60.0f / 84.0f } },
   { "Indicator 52r2", { 0.9541015625f, 0.6982421875f, 0.96875f, 0.7236328125f }, { 0.125f, 32.0f / 52.0f } },
   { "Indicator 52r2 #2", { 0.46142578125f, 0.736328125f, 0.47314453125f, 0.763671875f }, { 0.125f, 32.0f / 52.0f } },               // 64
   { "Indicator 40r2", { 0.9375f, 0.736328125f, 0.953125f, 0.751953125f }, { 0.125f, 20.0f / 40.0f } },
   { "Indicator 36r2", { 0.47314453125f, 0.7412109375f, 0.48486328125f, 0.7587890625f }, { 0.125f, 26.0f / 36.0f } },
   { "Indicator 32r2", { 0.95166015625f, 0.736328125f, 0.96337890625f, 0.751953125f }, { 0.125f, 12.0f / 32.0f } },
   { "Indicator 32r2 #2", { 0.94189453125f, 0.751953125f, 0.94970703125f, 0.763671875f }, { 0.125f, 12.0f / 32.0f } },               // 68
   { "Indicator 24r2", { 0.95361328125f, 0.75390625f, 0.96142578125f, 0.76171875f }, { 0.125f, 4.0f / 24.0f } },
   { "Indicator 34r1", { 0.96533203125f, 0.736328125f, 0.97314453125f, 0.751953125f }, { 0.0625f, 16.0f / 34.0f } },
   { "Indicator 30r1", { 0.97705078125f, 0.73828125f, 0.98486328125f, 0.75f }, { 0.0625f, 12.0f / 30.0f } },
   { "Indicator 26r1", { 0.96533203125f, 0.751953125f, 0.97314453125f, 0.763671875f }, { 0.0625f, 8.0f / 26.0f } },                  // 72
   { "Indicator 22r1", { 0.97802734375f, 0.75390625f, 0.98388671875f, 0.76171875f }, { 0.0625f, 4.0f / 22.0f } },
   { "Indicator 96r8pin #1", { 0.96923828125f, 0.6328125f, 0.98486328125f, 0.6796875f }, { 0.25f, 78.0f / 96.0f } },
   { "Indicator 96r8pin #2", { 0.98486328125f, 0.6328125f, 1.0f, 0.6796875f }, { 0.25f, 78.0f / 96.0f } },
   { "Indicator 64r8pin #1", { 0.96923828125f, 0.6953125f, 0.98486328125f, 0.7265625f }, { 0.25f, 44.0f / 64.0f } },                 // 76
   { "Indicator 64r8pin #2", { 0.984375f, 0.6875f, 1.0f, 0.734375f }, { 0.25f, 44.0f / 64.0f } },
   { "Indicator 56r5pin", { 0.48388671875f, 0.73828125f, 0.49755859375f, 0.76171875f }, { 20.0f / 32.0f, 38.0f / 56.0f } },
   { "Indicator 34r3.5pin", { 0.98876953125f, 0.7373046875f, 0.99658203125f, 0.7509765625f }, { 14.0f / 32.0f, 16.3858f / 34.0f } },
   { "Indicator 26r3.5pin", { 0.98876953125f, 0.7529296875f, 0.99658203125f, 0.7626953125f }, { 14.0f / 32.0f, 8.3858f / 26.0f } }   // 80
};

// Memory location to write wheel tilt data to
extern si32 &mouseWheelTilt;

// Thread status strings
al16 HWND    hWnd; // Main window's handle
     cwchptr renderWndClass       = L"LV_D3D";
     cwchptr rndrWndTitle         = L"The Last Vigil :: Direct3D 11 render window";
     vui64   THREAD_LIFE_V        = 0; // D3D11 subthread flags
     vui128  MAPMAN_THREAD_STATUS = {};
     vui128  ENTMAN_THREAD_STATUS = {};

// Early develepment only...
RESOLUTION ScrRes = { 3600, 1600, 16.0f / 36.0f, 36.0f / 16.0f, 1.0f, DXGI_FORMAT_R10G10B10A2_UNORM, DXGI_FORMAT_D24_UNORM_S8_UINT, 8, 1, 0,
                      3840, 2160, 9.0f / 16.0f, 16.0f / 9.0f, 1.0f, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_D32_FLOAT, 8, 1, 0,
                      3840, 2160, 9.0f / 16.0f, 16.0f / 9.0f, 2.2f, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_D32_FLOAT, 8, 1, 0,
                      120, 360, 0, 0, 0, 0 };

#include <dxgidebug.h>

void Direct3D11Thread(ptr argList) {
   CLASS_GPU gpu = { files, &ptrLib[2] };
   CLASS_GUI gui = { files, gpu, &ptrLib[4] };

   CLASS_TIMER  frameTimer;
   CLASS_TIMERS animTimer;

   GLOBALCTRLVARS gcvLocal = {};
   GLOBALCOORDS   gcoLocal = {};

   HWND    hRndrWnd;
   ui64    threadLife;
   MARGINS wndMargins     = { -1, -1, -1, -1 };
   ui32    iTotalFrames   = 0;
   ui32    iCurrentFrames = 0;

   // Prevent thread from shutting down (after engine reset)
   THREAD_LIFE &= ~VIDEO_THREAD_DIED;

   // Live debugging for D3D11
   //IDXGIDebug1 *devDebug;
   //hr = DXGIGetDebugInterface1(0, IID_PPV_ARGS(&devDebug));

   hWnd = hRndrWnd = gpu.CreateRenderWindow();

Reinitialise_:
   THREAD_LIFE &= ~VIDEO_THREAD_RESET;

   cHANDLE waitGPU = gpu.InitialiseBackbuffer(hRndrWnd, ScrRes, ae_windowed);

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

   gpu.tex.ConfigData(-1, -1, -1, 8u, D3D11_USAGE_IMMUTABLE, false, false, true, false, false);
   csi32 siTextureSet[2] = { gpu.tex.LoadTextureSet(L"1024", TERRAIN), gpu.tex.LoadTextureSet(L"1024", SPRITE) };
   gpu.tex.SetPS2D(0, siTextureSet[0], TEX_REG_OS_WORLD, 3u);
   gpu.tex.SetPS2D(0, siTextureSet[1], TEX_REG_OS_WORLD + 4u, 3u);

   CLASS_MAPMAN      mapMan(gpu.files);
   CLASS_ENTMAN      entMan;
   CLASS_D3D11HELPER gpuHelper(gpu, mapMan, entMan);
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
   md.stName     = (wchptrc)L"Test map";
   md.stInfo     = (wchptrc)L"Description text.";
   md.ptIndex    = 0;
   md.chunkDim   = { 16, 16, 1 };
   md.mapDim     = { { 1024, 1024, 8 } }; // 256x144x1 -> 4,718,592 triangles -> Approx. 1.68 billion triangles per second @ 4K
   md.zso        = 4;
   md.entListDim = 16;
   csi32 mapID = mapMan.CreateMap(md, -1, 0, 0, 2);
   mapMan.SetGlobalMapDescriptor(mapID, 0); 

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
         entMan.CreateEntity(md, 0, { 0, 0 }, { j - 128.0f, i - 64.0f, -1.5f, 0.0f }, { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.4f, 0.4f, 0.4f, 0.0f });

   gpuHelper.map.CreateBuffers(0, 0, 0);
   gpuHelper.ent.CreateBuffers(0);

   GUI_DESC gd(true);

//   cui32 uiAtlasIndex[2] = { gui.LoadAtlas(L"font.c_a_gatintas.eng", -1), gui.LoadAtlas(L"panels.blue_rust", -1) };
//   cui32 uiAtlasIndex = { gui.LoadAtlas(L"font.c_a_gatintas.eng", -1) };//, gui.LoadAtlas(L"panels.blue_rust", -1) };

//   cui32 GUISprites = gui.CreateSpriteLibrary(0, 82, uiAtlasIndex[1]);
//   for(ui32 i = 0; i < 82; i++)
//      gui.CreateSprite(GUISprites, (GUI_SPRITE &)defaultUISprite[i], true);
//      gui.CreateSprite(GUISprites, (CLASS_GUI::GUI_SPRITE_DEFAULTS &)defaultUISprite[i], true);
//   gui.SaveSpriteLibrary(L"standard.blue_rust.aesl", GUISprites);
//   csi16 alphabetIndex = gui.CreateAlphabet("Bogan", 16, 16, { 0.5f, 0.75f }, 0, uiAtlasIndex);
//   gui.SaveAlphabet(L"main.english.aea", alphabetIndex);
   csi16 spriteLibIndex = gui.LoadSpriteLibrary(L"standard.blue_rust.aesl", -1);
   csi16 alphabetIndex  = gui.LoadAlphabet(L"main.english.aea", -1);

   gui.UploadAlphabetBuffer(0);

   cchptrc selectionBoxArray[10] = { stOnHoverDefault, stOffHoverDefault, stActivate00DefaultButton, stActivate01DefaultButton, stActivate00DefaultToggle, stActivate01DefaultToggle, stActivate00DefaultScalar, stActivate01DefaultScalar, stActivate10DefaultScalar, stActivate11DefaultScalar };

   al32 char textBuffer[4][128] = { "TR-X \1\2\3\4\5\6\7", "<Input list>", "Ya Mum!", "Eshay, bruh? Let's oge!" };

   GUI_EL_DESC elementDesc;

   elementDesc.viewPos     = { 0.0f, 0.0f };
   elementDesc.index       = { -1, alphabetIndex, 0, 13 };
   elementDesc.panel.tint  = c4_white;
   elementDesc.panel.size  = { 1.0f, 1.0f };
   elementDesc.panel.align = UI_ALIGN_C;
   elementDesc.panel.mods  = UI_PANEL;
   elementDesc.text.tint   = c4_white;
   elementDesc.text.size   = { 0.24f, 0.24f };
   elementDesc.text.cArray = (chptr *)selectionBoxArray;
   elementDesc.text.sCount = 10;
   elementDesc.text.align  = UI_ALIGN_TL;
   elementDesc.text.mods   = UI_TEXTARRAY;
   cVEC2Du32 panelElement2 = gui.CreateTextSelectionPanel(elementDesc);
   elementDesc.viewPos     = { 0.0f, 0.0f };
   elementDesc.index       = { -1, -1, 0, 35 };
   elementDesc.panel.tint  = c4_white;
   elementDesc.panel.size  = { 1.0f, 1.0f };
   elementDesc.panel.align = UI_ALIGN_T;
   elementDesc.panel.mods  = UI_PANEL ^ UI_TRANS;
   cui32 panelElement0 = gui.CreateToggle(elementDesc);
   elementDesc.viewPos      = { 0.0f, 0.5f };
   elementDesc.index        = { -1, -1, 0, 21, 76 };
   elementDesc.panel.tint   = c4_white;
   elementDesc.panel.size   = { 2.0f, 2.0f };
   elementDesc.panel.align  = UI_ALIGN_L | UI_ALIGN_T;
   elementDesc.panel.mods   = UI_PANEL;
   elementDesc.cursor.tint  = c4_white;
   elementDesc.cursor.size  = { 2.0f, 2.0f };
   elementDesc.cursor.align = UI_ALIGN_C;
   elementDesc.cursor.mods  = UI_CURSOR;
   cui32 panelElement1 = gui.CreateScalar(elementDesc);
   elementDesc.viewPos     = { 0.0f, 0.25f };
   elementDesc.index       = { -1, -1, 0, 35 };
   elementDesc.panel.tint  = c4_white;
   elementDesc.panel.size  = { 1.0f, 1.0f };
   elementDesc.panel.align = UI_ALIGN_L | UI_ALIGN_T;
   elementDesc.panel.mods  = UI_PANEL;
   cui32 panelElement3 = gui.CreateButton(elementDesc);
   elementDesc.viewPos     = { 0.0f, 0.0f };
   elementDesc.index       = { -1, alphabetIndex };
   elementDesc.text.tint   = c4_white;
   elementDesc.text.size   = { 0.0468757f, 0.0468757f };
   elementDesc.text.cPtr   = textBuffer[0];
   elementDesc.text.cCount = 128;
   elementDesc.text.align  = UI_ALIGN_BL;
   elementDesc.text.mods   = UI_TEXT;
   cui32 textElement = gui.CreateText(elementDesc);
   elementDesc.viewPos     = { 0.0f, 0.0f };
   elementDesc.index       = { -1, alphabetIndex };
   elementDesc.text.tint   = c4_white;
   elementDesc.text.size   = { 0.0468757f, 0.0468757f };
   elementDesc.text.cPtr   = textBuffer[1];
   elementDesc.text.cCount = 128;
   elementDesc.text.align  = UI_ALIGN_TR;
   elementDesc.text.mods   = UI_TEXT ^ UI_TRANS;
   cui32 textInputEl = gui.CreateText(elementDesc);
   elementDesc.viewPos        = { 0.0f, 0.0f };
   elementDesc.index          = { -1, alphabetIndex, 0, 12 };
   elementDesc.panel.tint     = c4_white;
   elementDesc.panel.size     = { 1.0f, 1.0f };
   elementDesc.panel.bStyle   = 1u;
   elementDesc.panel.bSize    = 0.0125f;
   elementDesc.panel.wOpacity = 1.0f - sqrtf(0.5f);
   elementDesc.panel.wIndent  = { 1.0f, 1.0f };
   elementDesc.panel.align    = UI_ALIGN_R;
   elementDesc.panel.mods     = UI_PANEL;
   elementDesc.text.tint      = c4_white;
   elementDesc.text.size      = { 0.125f, 0.125f };
   elementDesc.text.cPtr      = textBuffer[2];
   elementDesc.text.cCount    = 128;
   elementDesc.text.align     = UI_ALIGN_C;
   elementDesc.text.mods      = UI_ROT | UI_TRANS | UI_SCALE_Y;
   cVEC2Du32 textBox = gui.CreateTextPanel(elementDesc);
   elementDesc.viewPos      = { 0.0f, -0.75f };
   elementDesc.index        = { -1, alphabetIndex, 0, 21, 53 };
   elementDesc.panel.tint   = c4_white;
   elementDesc.panel.size   = { 2.0f, 2.0f };
   elementDesc.panel.bStyle = 0;
   elementDesc.panel.align  = UI_ALIGN_C;
   elementDesc.panel.mods   = UI_PANEL;
   elementDesc.cursor.tint  = c4_white;
   elementDesc.cursor.size  = { 1.0f, 1.0f };
   elementDesc.cursor.align = UI_ALIGN_R;
   elementDesc.cursor.mods  = UI_CURSOR;
   elementDesc.text.tint    = c4_white;
   elementDesc.text.size    = { 0.15f, 0.15f };
   elementDesc.text.cPtr    = textBuffer[3];
   elementDesc.text.cCount  = 128;
   elementDesc.text.align   = UI_ALIGN_L;
   elementDesc.text.mods    = UI_INPUT;
   cVEC3Du32 inputBox = gui.CreateInputPanel(elementDesc);
   cui32 textElements[3] = { textElement, textInputEl, textBox.y };

   cui32 interfaceMain = gui.CreateInterface(128, 16);
   gui.AddElementToInterface(panelElement0, interfaceMain);
   gui.AddElementToInterface(panelElement1, interfaceMain);
   gui.AddElementToInterface(panelElement2, interfaceMain);
   gui.AddElementToInterface(panelElement3, interfaceMain);
   gui.AddElementToInterface(textBox, interfaceMain);
   gui.AddElementToInterface(inputBox, interfaceMain);
   gui.AddElementToInterface(textElement, interfaceMain);
   gui.AddElementToInterface(textInputEl, interfaceMain);
   gd.interfaceIndex = interfaceMain;

   //   gpu.files.SaveMap(L"test.aemap", mapMan.world[mapID]);

   gpu.lit.Create(0, 256, 0);
   gpu.lit.SetColour(0, { 0.25f, 0.25f, 0.25f });
   gpu.lit.n[0].pos          = { -256.0f, 256.0f, -256.0f };
   gpu.lit.n[0].range        = 1024.0f;
   gpu.lit.n[0].hl.size      = 0.5f;
   gpu.lit.n[0].hl.intensity = 4.0f;
   gpu.lit.SetColour(1, { 1.0f, 1.0f, 1.0f });
   gpu.lit.n[1].pos          = { 0.0f, 0.0f, -12.0f };
   gpu.lit.n[1].range        = 8.0f;
   gpu.lit.n[1].hl.size      = 0.5f;
   gpu.lit.n[1].hl.intensity = 8.0f;
   gpu.lit.SetColour(2, { 2.0f, 0.001f, 0.001f });
   gpu.lit.n[2].pos          = { 0.0f, 16.0f, -8.0f };
   gpu.lit.n[2].range        = 8.0f;
   gpu.lit.n[2].hl.size      = 1.0f;
   gpu.lit.n[2].hl.intensity = 4.0f;
   gpu.lit.SetColour(3, { 0.001f, 0.001f, 2.0f });
   gpu.lit.n[3].pos          = { 14.0f, -8.0f, -8.0f };
   gpu.lit.n[3].range        = 8.0f;
   gpu.lit.n[3].hl.size      = 1.0f;
   gpu.lit.n[3].hl.intensity = 4.0f;
   gpu.lit.SetColour(4, { 0.001f, 2.0f, 0.001f });
   gpu.lit.n[4].pos          = { -14.0f, -8.0f, -8.0f };
   gpu.lit.n[4].range        = 8.0f;
   gpu.lit.n[4].hl.size      = 1.0f;
   gpu.lit.n[4].hl.intensity = 4.0f;
   gpu.lit.SetColour(5, { 1.0f, 1.0f, 1.0f });
   gpu.lit.n[5].pos          = { -13.0f, 7.0f, -4.0f };
   gpu.lit.n[5].range        = 8.0f;
   gpu.lit.n[5].hl.size      = 0.25f;
   gpu.lit.n[5].hl.intensity = 4.0f;
   gpu.lit.SetColour(6, { 0.7071f, 0.7071f, 0.7071f });
   gpu.lit.n[6].pos          = { 13.0f, 7.0f, -4.0f };
   gpu.lit.n[6].range        = 8.0f;
   gpu.lit.n[6].hl.size      = 1.0f;
   gpu.lit.n[6].hl.intensity = 4.0f;
   gpu.lit.UploadAll(0);

   al16 fl32 fAngle = 0.0f, fScale = 1.0f;
   ui8 bMouseCursor = true;

   gpu.cam.CreateCamera(0, ae_stages_vertex_geometry, ae_stages_vertex_geometry, 1, true);
   gpu.cam.data32[0].fXpos   = 0.0f; gpu.cam.data32[0].fYpos = 0.0f;   gpu.cam.data32[0].fZpos = -192.0f;
   gpu.cam.data32[0].fXrot   = 0.0f; gpu.cam.data32[0].fYrot = 0.0f;   gpu.cam.data32[0].fZrot = 0.0f;
   gpu.cam.data32[0].fZoom   = 1.0f; gpu.cam.data32[0].fSize = 256.0f;
   gpu.cam.data32[0].fWidth  = ScrRes.dims[ScrRes.state].aspectI;
   gpu.cam.data32[0].fHeight = 1.0f;
   gpu.cam.data32[0].fNearZ  = 1.0f;
   gpu.cam.data32[0].fFarZ   = 16384.0f;
   gpu.cam.PrecalculateProjections(0);
   gpu.cam.UploadProjections(0x0, 0);

   // Display the render window
   gpu.SetBorderedWindow(hWnd, ScrRes);
//   gpu.SetBorderlessWindow(hWnd, ScrRes);
//   gpu.SetFullscreenWindow(hWnd, ScrRes);

// Initialise timers
   frameTimer.Reset(1.0);
   cui8 tRibbons = animTimer.Create();
   animTimer.Reset(1.0, tRibbons);
   cui8 tPlane = animTimer.Create();
   animTimer.Reset(7.0, tPlane);

   ui8    index          = 0;
   ui8    step           = 1;
   fl64   dPlaneAnimTime = 0.0;
   VEC3Df vCurPos        = null3Df;
   si128 &siActiveLayer  = (*(MAP_DESC *)ptrLib[14]).mcrv.activeElements.xmm1;
   si32  &siCell         = (*(MAP_DESC *)ptrLib[14]).mcrv.cellIndex[0].vector.w;
   si32   chunk          = 0;

   ui32 uiFrameCount = 0, uiVisChunks = 0;
   fl32 fAvgFrameTime = 0;
   fl64 dTrisPerSec = 0;

   THREAD_LIFE |= VIDEO_THREAD_DONE;

   ///
   /// Primary rendering loop
   ///
   gui.SetResourceBuffers(0, TEX_REG_OS_GUI);
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
      // Trigger input thread to process inputs
      //gd.ProcessInputs(gd.interfaceIndex);
      TriggerInputProcessing;

      // Clamp camera rotation
      if(gpu.cam.data32[0].fXrot < -1.0f) gpu.cam.data32[0].fXrot = -1.0f;
      if(gpu.cam.data32[0].fXrot > 1.0f) gpu.cam.data32[0].fXrot = 1.0f;
      if(gpu.cam.data32[0].fYrot < -1.0f) gpu.cam.data32[0].fYrot = -1.0f;
      if(gpu.cam.data32[0].fYrot > 1.0f) gpu.cam.data32[0].fYrot = 1.0f;

      // Update local copy of global control variables
      Stream64(ptr(&gcv), &gcvLocal, 448u);

      // Update camera
      gpu.cam.MoveCameraRightXZ(gcvLocal.joy[0].s.x2 * fElapsedTime * 32.0f, 0);
      gpu.cam.MoveCameraForwardXZ(gcvLocal.joy[0].s.y2 * fElapsedTime * -32.0f, 0);
      gpu.cam.MoveCameraUpY(gcvLocal.joy[0].t.x * fElapsedTime * -32.0f, 0);
      gpu.cam.TransformCamera(0, false);

      // Begin culling out-of-view entities and map chunks
      gpuHelper.ent.StartViewCulling(0);
      gpuHelper.map.StartViewCulling(0, 0);

      // Upload camera
      gpu.cam.UploadCamera(dTotalTime, fElapsedTime, uiFrameCount++, 0x0, 0);

      // Update audio listener data
      gcoLocal.vel = gcoLocal.pos;
      gcoLocal.pos = gpu.cam.data32[0].pos;
      gcoLocal.ori = gpu.cam.vCamOri;
      
      LockedMoveAndClear(&gcoLocal, &gco, 64); // Forcing volatile copy to avoid glitches

      //WaitForSingleObjectEx(waitGPU, 1000, false);
      gpu.ren.ClearAndFlipOutputImage(c4_black_trans);

      // Entities: Wait until visibility & modification culling completes, then upload modified entity data and render
      cui128 entManThreadData = gpuHelper.ent.WaitThenUploadAndRender(0);

      // Map: Wait until visibility & modification culling completes, then upload modified cell data and render
      cui128 mapManThreadData = gpuHelper.map.WaitThenUploadAndRender(0, 0);
      uiVisChunks = mapManThreadData.m128i_u32[0];

      // Update UI readouts
      snprintf(textBuffer[0], 128, "Pos/Rot: <%.2f, %.2f, %.2f / %.2f, %.2f, %.2f>   Cull<Cells:%.3fms Ent:%.3fms>   Map cells: %d   Entities: %d",
               gpu.cam.data32[0].fXpos, gpu.cam.data32[0].fYpos, gpu.cam.data32[0].fZpos, gpu.cam.data32[0].fXrot, gpu.cam.data32[0].fYrot, gpu.cam.data32[0].fZrot,
               sysData.culling.map.time, sysData.culling.entity.time, sysData.culling.map.vis[0], sysData.culling.entity.vis[0]);
      snprintf(textBuffer[1], 128, "0x%03X 0x%03X 0x%03X 0x%03X", inputsImmediate.x, inputsImmediate.y, inputsImmediate.z, inputsImmediate.w);
//      if(siCell != 0x0CDCDCDCD && siCell != 0x080000001) snprintf(textBoxText, 128, "0x%08X: %.3f", siCell, mapMan.world[0].map[0]->cell[siCell].geometry->dens);
      snprintf(textBuffer[2], 128, "%d:%d:%d", inputBox.z, gui.siGUIElements, gui.uiGUIVerts);

      // Render 3D overlay(s)
      gpu.cfg.SetBlendState(0, 1);
      gpu.cfg.SetDepthStencilState(0, 0, 0);
      if(siActiveLayer.m128i_i32[0] < 0 && siCell != 0x080000001) gpu.ren.QueueDrawVoxel(0, (*(MAP_DESC *)ptrLib[14]).mcrv.activeCell, c4_light_violet);

      // Render GUI
      gui.UpdateText(textBuffer, 128u, textElements, 3u);
      gui.DrawInterface(0, interfaceMain);

      // Display backbuffer
      gpu.ren.QueuePresentOutputImage(0);

      Sleep(0);
   } while (threadLife & VIDEO_THREAD_ALIVE);

   //hr = devDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);

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
