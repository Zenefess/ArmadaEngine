/************************************************************
 * File: Direct3D11 functions.h         Created: 2022/10/20 *
 *                                Last modified: 2024/07/02 *
 *                                                          *
 * Desc:                                                    *
 *                                                          *
 *  Copyright (c) David William Bull. All rights reserved.  *
 ************************************************************/
#pragma once

#include "typedefs.h"
#include "Data structures.h"
#include "File operations.h"
#include "class_render.h"
#include "Direct3D11 functions\class_config.h"
#include "Direct3D11 functions\class_buffers.h"
#include "Direct3D11 functions\class_cameras.h"
#include "Direct3D11 functions\class_lights.h"
#include "Direct3D11 functions\class_textures.h"

#ifdef _DEBUG
#define NDEBUG 0
#include "comdef.h"
#include "D3D11SDKLayers.h"
#endif

 //   cui32 flags = (NDEBUG ? NULL : D3D11_CREATE_DEVICE_DEBUG);
constexpr cui32 flags = (NDEBUG ? D3D11_CREATE_DEVICE_SINGLETHREADED : D3D11_CREATE_DEVICE_SINGLETHREADED | D3D11_CREATE_DEVICE_DEBUG);// | D3D11_CREATE_DEVICE_PREVENT_INTERNAL_THREADING_OPTIMIZATIONS);

inline  void     MessagePump(void);
LRESULT CALLBACK RndrWndProc(HWND, UINT, WPARAM, LPARAM);

al64 struct CLASS_GPU {
   CLASS_FILEOPS &files;

   ID3D11Device *dev; IDXGIDevice4 *dxgiDev; IDXGIAdapter3 *dxgiAdapter;

   CLASS_CONFIG   cfg = files;
   CLASS_BUFFERS  buf;
   CLASS_TEXTURES tex;

   al16 struct _D3D_DATA_BLOCK_ {
      ui32 msaaQlevel[32];
      DXGI_SWAP_CHAIN_DESC1           scd1;
      DXGI_SWAP_CHAIN_FULLSCREEN_DESC scfd;
      DXGI_FORMAT    bbFormat;
      D3D11_VIEWPORT viewport;
      union { D3D_FEATURE_LEVEL d3DFL[4]; struct { D3D_FEATURE_LEVEL x, y, z, w; } vecD3DFL; };
      D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
      DXGI_MODE_DESC       md;
      D3D11_TEXTURE2D_DESC td;
   } &data = *(_D3D_DATA_BLOCK_ *)zalloc16(sizeof(_D3D_DATA_BLOCK_));

   si8 curWinState;

   // 7 bytes spare

   IDXGIFactory5 *factory;

   CLASS_RENDER ren = { cfg, buf, tex };
   CLASS_CAM    cam = buf;
   CLASS_LIGHTS lit;

   ID3D11DeviceContext *devcon[4];

private:
///--- Add booleans for selectively enabling each sub-class? ---///
   inline void _CLASS_GPU_INIT(void) {
      data.vecD3DFL    = { (D3D_FEATURE_LEVEL)0, D3D_FEATURE_LEVEL_12_1, D3D_FEATURE_LEVEL_12_0, D3D_FEATURE_LEVEL_11_1 };
      data.bbFormat    = DXGI_FORMAT_R16G16B16A16_FLOAT;
      curWinState = -1;

      Try(stCreateDev, D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, flags, &data.d3DFL[1], 3, D3D11_SDK_VERSION, &dev, &data.d3DFL[0], &devcon[0]), ss_video);
      Try(stQueryInt, dev->QueryInterface(__uuidof(IDXGIDevice4), (void **)&dxgiDev), ss_video);
      Try(stDevGetPar, dxgiDev->GetParent(__uuidof(IDXGIAdapter3), (void **)&dxgiAdapter), ss_video);
      Try(stAdaGetPar, dxgiAdapter->GetParent(__uuidof(IDXGIFactory4), (void **)&factory), ss_video);

//      dxgiDev->SetGPUThreadPriority(7);

      cfg.dev = buf.dev = cam.dev = lit.dev = tex.dev = ren.dev = dev;
      cfg.devcon = buf.devcon = cam.devcon = lit.devcon = tex.devcon = ren.devcon = devcon;
      ren.bufVertex = buf.CreateVertex(&ren.vCube, sizeof(ren.vCube), 1, 1);
      GetCurrentDirectoryW(512, files.pathWorking);
      mset(cfg.shaderIndex, sizeof(ui8[6][CFG_MAX_SHADERS]), ui8(CFG_MAX_SHADERS - 1));   // Default all indices to unused
   }
public:
   CLASS_GPU(CLASS_FILEOPS &fileOps) : files(fileOps) { _CLASS_GPU_INIT(); }
   CLASS_GPU(CLASS_FILEOPS &fileOps, cptrptr globalPointer) : files(fileOps) { _CLASS_GPU_INIT(); if(globalPointer) *globalPointer = this; }

   HWND CreateRenderWindow(void) const {
      al8  HWND       hWindow;
           WNDCLASSEX wcex;

      wcex.cbSize = sizeof(WNDCLASSEX);
      wcex.style = 0;
      wcex.lpfnWndProc = RndrWndProc;
      wcex.cbClsExtra = 0;
      wcex.cbWndExtra = 0;
      wcex.hInstance = hInst;
      wcex.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_LASTVIGIL));
      wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
      wcex.hbrBackground = NULL;
      wcex.lpszMenuName = NULL;
      wcex.lpszClassName = renderWndClass;
      wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_LASTVIGIL));

      RegisterClassEx(&wcex);
      hWindow = CreateWindow(renderWndClass, rndrWndTitle, WS_SYSMENU | WS_CAPTION | WS_MAXIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT,
                             (UINT)ScrRes.window.w + 16, (UINT)ScrRes.window.h + 39, NULL, NULL, hInst, NULL);
      if(!hWindow) Try(stCreateWin, -1, ss_video);
      SetCursor(LoadCursor(NULL, IDC_ARROW));

      return hWindow;
   }

   inline void MouseCursor(cHWND hWnd, cbool visible) const {
      static RECT mouseLimits;
      SetCapture(hWnd);
      if(!ScrRes.state)
         mouseLimits = { ScrRes.windowOS.x, ScrRes.windowOS.y, LONG(ScrRes.window.w + ScrRes.windowOS.x), LONG(ScrRes.window.h + ScrRes.windowOS.y) };
      else
         mouseLimits = { 0, 0, LONG(ScrRes.dims[ScrRes.state].w), LONG(ScrRes.dims[ScrRes.state].h)};
      ClipCursor(&mouseLimits);
      if(visible) ShowCursor(true);
      else while(ShowCursor(false) >= 0) ShowCursor(false);
   }

///--- !!! Rewrite based on new version below !!!
   HANDLE InitialiseBackbuffer(cHWND hWindow, cfl32 width, cfl32 height, cfl32 gamma, cui32 formatBackBuffer, cui32 formatDepthBuffer, cui8 msaa, cui8 msaaQuality, cui8 swapBuffers, cAE_WINDOW_STATE windowState) {
      for(ui8 xx = 1; xx < 33; xx++)
         Try(stCheckMSQL, dev->CheckMultisampleQualityLevels((DXGI_FORMAT)formatBackBuffer, xx, &data.msaaQlevel[xx]), ss_video);
      data.scd1.Width       = (UINT)width;
      data.scd1.Height      = (UINT)height;
      data.scd1.Format      = (DXGI_FORMAT)formatBackBuffer;
      data.scd1.Stereo      = 0;
      data.scd1.SampleDesc  = { msaa, (msaaQuality >= data.msaaQlevel[msaa] ? data.msaaQlevel[msaa] : msaaQuality - 1) };
      data.scd1.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
      data.scd1.BufferCount = swapBuffers;
      data.scd1.Scaling     = DXGI_SCALING_NONE;
      data.scd1.SwapEffect  = (msaa > 1 ? DXGI_SWAP_EFFECT_DISCARD : DXGI_SWAP_EFFECT_FLIP_DISCARD);
//      data.scd1.SwapEffect  = (msaa > 1 ? DXGI_SWAP_EFFECT_DISCARD : DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL);
      data.scd1.AlphaMode   = DXGI_ALPHA_MODE_UNSPECIFIED;
//      data.scd1.Flags       = NULL;
      data.scd1.Flags       = (windowState <= 1 ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING | DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT : DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
      data.scfd.RefreshRate      = { 0, 1 };
      data.scfd.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
      data.scfd.Scaling          = DXGI_MODE_SCALING_UNSPECIFIED; //STRETCHED;
      data.scfd.Windowed         = (windowState > 1 ? false : true);

      Try(stCreateSCFH, factory->CreateSwapChainForHwnd(dev, hWindow, &data.scd1, &data.scfd, NULL, (IDXGISwapChain1 **)&ren.swapchain), ss_video);
      Try(stMakeWinAss, factory->MakeWindowAssociation(hWindow, DXGI_MWA_NO_ALT_ENTER), ss_video);
      Try(stSetMaxFLat, ren.swapchain->SetMaximumFrameLatency(swapBuffers - 1), ss_video);

      data.td.Width      = (UINT)width;
      data.td.Height     = (UINT)height;
      data.td.MipLevels  = 1;
      data.td.ArraySize  = 1;
      data.td.Format     = (DXGI_FORMAT)formatDepthBuffer;
      data.td.SampleDesc = { msaa, (msaaQuality > data.msaaQlevel[msaa] ? data.msaaQlevel[msaa] : msaaQuality) };
      data.td.Usage      = D3D11_USAGE_DEFAULT;
      data.td.BindFlags  = D3D11_BIND_DEPTH_STENCIL;
      Try(stCreateTex2D, dev->CreateTexture2D(&data.td, NULL, &ren.pDepthBuffer[0]), ss_video);

      Try(stResizeTar, ren.swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (ptrptr)&ren.pBackBuffer[0]), ss_video);
      Try(stCreateRTV, dev->CreateRenderTargetView(ren.pBackBuffer[0], NULL, &ren.rtvBackBuffer), ss_video);
      ren.pBackBuffer[0]->Release();

      data.dsvd.Format             = (DXGI_FORMAT)formatDepthBuffer;
      data.dsvd.ViewDimension      = D3D11_DSV_DIMENSION_TEXTURE2D;
      data.dsvd.Texture2D.MipSlice = 0;
      Try(stCreateDSV, dev->CreateDepthStencilView(ren.pDepthBuffer[0], &data.dsvd, &ren.pDSV), ss_video);

      data.viewport.TopLeftX = 0;
      data.viewport.TopLeftY = 0;
      data.viewport.Width    = width;
      data.viewport.Height   = height;
      data.viewport.MinDepth = 0.0f;
      data.viewport.MaxDepth = 1.0f;
      devcon[0]->RSSetViewports(1, &data.viewport);

      data.bbFormat      = (DXGI_FORMAT)formatBackBuffer;
      ren.msaaLevel = msaa;

      return ren.swapchain->GetFrameLatencyWaitableObject();
   };

   // Multisampling supported
   HANDLE InitialiseBackbuffer(cHWND hWindow, RESOLUTION &resData, cAE_WINDOW_STATE windowState) {
      cRESDATA &curDimData = resData.dims[windowState];

      for(ui8 xx = 1; xx <= 32; xx++)
         Try(stCheckMSQL, dev->CheckMultisampleQualityLevels((DXGI_FORMAT)curDimData.fmtBB, xx, &data.msaaQlevel[xx - 1]), ss_video);

      data.scd1.Width       = (UINT)curDimData.width;
      data.scd1.Height      = (UINT)curDimData.height;
      data.scd1.Format      = (DXGI_FORMAT)curDimData.fmtBB;
      data.scd1.Stereo      = 0;
      data.scd1.SampleDesc  = { (ui32)curDimData.msaa, (curDimData.msaaQ > data.msaaQlevel[curDimData.msaa - 1] ? data.msaaQlevel[curDimData.msaa - 1] : curDimData.msaaQ) };
      data.scd1.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
      data.scd1.BufferCount = curDimData.buffers;
      data.scd1.Scaling     = (curDimData.msaa > 1 ? DXGI_SCALING_STRETCH : DXGI_SCALING_NONE);
      data.scd1.SwapEffect  = (curDimData.msaa > 1 ? DXGI_SWAP_EFFECT_DISCARD : DXGI_SWAP_EFFECT_FLIP_DISCARD);
      data.scd1.AlphaMode   = DXGI_ALPHA_MODE_UNSPECIFIED;
      data.scd1.Flags       = (windowState <= 1 ? curDimData.msaa > 1 ? 0 : DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING | DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT : DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);

      data.scfd.RefreshRate      = (DXGI_RATIONAL &)(VEC2Du32)resData.refreshRate;
      data.scfd.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
      data.scfd.Scaling          = DXGI_MODE_SCALING_UNSPECIFIED; //STRETCHED;
      data.scfd.Windowed         = (windowState > 1 ? false : true);

      Try(stCreateSCFH, factory->CreateSwapChainForHwnd(dev, hWindow, &data.scd1, &data.scfd, NULL, (IDXGISwapChain1 **)&ren.swapchain), ss_video);
      Try(stMakeWinAss, factory->MakeWindowAssociation(hWindow, DXGI_MWA_NO_ALT_ENTER), ss_video);
      Try(stSetMaxFLat, ren.swapchain->SetMaximumFrameLatency(curDimData.buffers - 1), ss_video);

      data.td.Width      = (UINT)curDimData.width;
      data.td.Height     = (UINT)curDimData.height;
      data.td.MipLevels  = 1u;
      data.td.ArraySize  = 1u;
      data.td.Format     = (DXGI_FORMAT)curDimData.fmtDB;
      data.td.SampleDesc = { (ui32)curDimData.msaa, (curDimData.msaaQ > data.msaaQlevel[curDimData.msaa - 1] ? data.msaaQlevel[curDimData.msaa - 1] : curDimData.msaaQ) };
      data.td.Usage      = D3D11_USAGE_DEFAULT;
      data.td.BindFlags  = D3D11_BIND_DEPTH_STENCIL;
      Try(stCreateTex2D, dev->CreateTexture2D(&data.td, NULL, &ren.pDepthBuffer[0]), ss_video);

      Try(stResizeTar, ren.swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (ptrptr)&ren.pBackBuffer[0]), ss_video);
      Try(stCreateRTV, dev->CreateRenderTargetView(ren.pBackBuffer[0], NULL, &ren.rtvBackBuffer), ss_video);
      ren.pBackBuffer[0]->Release();

      data.dsvd.Format             = (DXGI_FORMAT)curDimData.fmtDB;
      data.dsvd.ViewDimension      = (curDimData.msaa > 1 ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D);
      data.dsvd.Texture2D.MipSlice = 0;
      Try(stCreateDSV, dev->CreateDepthStencilView(ren.pDepthBuffer[0], &data.dsvd, &ren.pDSV), ss_video);

      data.viewport.TopLeftX = 0;
      data.viewport.TopLeftY = 0;
      data.viewport.Width    = curDimData.width;
      data.viewport.Height   = curDimData.height;
      data.viewport.MinDepth = 0.0f;
      data.viewport.MaxDepth = 1.0f;
      devcon[0]->RSSetViewports(1, &data.viewport);

      data.bbFormat      = (DXGI_FORMAT)curDimData.fmtBB;
      ren.msaaLevel = curDimData.msaa;

      resData.state = windowState;

      return ren.swapchain->GetFrameLatencyWaitableObject();
   };

   void ResizeBackbuffer(cRESOLUTION &resData, cAE_WINDOW_STATE windowState) {
      cRESDATA &curDimData = resData.dims[windowState];

      Try(stResizeTar, ren.swapchain->ResizeTarget(&data.md), ss_video);
      devcon[0]->OMSetRenderTargets(0, NULL, NULL);
      ren.rtvBackBuffer->Release();
      ren.pDSV->Release();
      Try(stSCResizeBuf, ren.swapchain->ResizeBuffers(0, (UINT)curDimData.width, (UINT)curDimData.height, (DXGI_FORMAT)curDimData.fmtBB,
                         (windowState <= 1 ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING | DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT : DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH)), ss_video);

      data.td.Width      = (UINT)curDimData.width;
      data.td.Height     = (UINT)curDimData.height;
      data.td.MipLevels  = 1;
      data.td.ArraySize  = 1;
      data.td.Format     = (DXGI_FORMAT)curDimData.fmtDB;
      data.td.SampleDesc = { (ui32)curDimData.msaa, (curDimData.msaaQ > data.msaaQlevel[curDimData.msaa] ? data.msaaQlevel[curDimData.msaa] : curDimData.msaaQ) };
      data.td.Usage      = D3D11_USAGE_DEFAULT;
      data.td.BindFlags  = D3D11_BIND_DEPTH_STENCIL;
      Try(stCreateTex2D, dev->CreateTexture2D(&data.td, NULL, &ren.pDepthBuffer[0]), ss_video);

      Try(stSCGetBuf, ren.swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (ptrptr)&ren.pBackBuffer[0]), ss_video);
      Try(stCreateRTV, dev->CreateRenderTargetView(ren.pBackBuffer[0], NULL, &ren.rtvBackBuffer), ss_video);
      ren.pBackBuffer[0]->Release();

      data.dsvd.Format             = (DXGI_FORMAT)curDimData.fmtDB;
      data.dsvd.ViewDimension      = (curDimData.msaa > 1 ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D);
      data.dsvd.Texture2D.MipSlice = 0;
      Try(stCreateDSV, dev->CreateDepthStencilView(ren.pDepthBuffer[0], &data.dsvd, &ren.pDSV), ss_video);

      data.viewport.TopLeftX = 0;
      data.viewport.TopLeftY = 0;
      data.viewport.Width    = curDimData.width;
      data.viewport.Height   = curDimData.height;
      data.viewport.MinDepth = 0.0f;
      data.viewport.MaxDepth = 1.0f;
      devcon[0]->RSSetViewports(1, &data.viewport);

      devcon[0]->OMSetRenderTargets(1, &ren.rtvBackBuffer, ren.pDSV);

      data.bbFormat      = (DXGI_FORMAT)curDimData.fmtBB;
      ren.msaaLevel = curDimData.msaa;
   }

   void SetBorderedWindow(cHWND hWindow, RESOLUTION &resData) {
      if(curWinState) {
         if(resData.state == 2)
            Try(stSetFullscr, ren.swapchain->SetFullscreenState(false, NULL), ss_video);
         EnableWindow(hWindow, false);
         SetWindowLongPtrW(hWindow, GWL_EXSTYLE, WS_EX_NOREDIRECTIONBITMAP);
         SetWindowLongPtrW(hWindow, GWL_STYLE, WS_CAPTION | WS_MAXIMIZEBOX);
         SetWindowPos(hWindow, HWND_NOTOPMOST, resData.windowOS.x - 8, resData.windowOS.y - 31, (int)resData.window.width + 16,
                      (int)resData.window.height + 39, SWP_DEFERERASE | SWP_NOCOPYBITS | SWP_NOREDRAW | SWP_FRAMECHANGED);
         ShowWindow(hWindow, SW_SHOWNORMAL);
         EnableWindow(hWindow, true);

         data.md.Width            = (UINT)resData.window.width;
         data.md.Height           = (UINT)resData.window.height;
         data.md.RefreshRate      = { 0, 1 };
         data.md.Format           = data.bbFormat;
         data.md.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
         data.md.Scaling          = DXGI_MODE_SCALING_UNSPECIFIED;
         curWinState         = resData.state = 0;

         ResizeBackbuffer(ScrRes, ae_windowed);
      }
   }

   void SetBorderlessWindow(cHWND hWindow, RESOLUTION &resData) {
      if(curWinState != 1) {
         if(resData.state == 2)
            Try(stSetFullscr, ren.swapchain->SetFullscreenState(false, NULL), ss_video);
         EnableWindow(hWindow, false);
         SetWindowLongPtrW(hWindow, GWL_EXSTYLE, WS_EX_NOREDIRECTIONBITMAP | WS_EX_OVERLAPPEDWINDOW);
         SetWindowLongPtrW(hWindow, GWL_STYLE, NULL);
         SetWindowPos(hWindow, HWND_TOPMOST, 0, 0, (int)resData.borderless.width, (int)resData.borderless.height, SWP_NOSIZE | SWP_DEFERERASE | SWP_NOCOPYBITS | SWP_NOREDRAW | SWP_FRAMECHANGED);
         ShowWindow(hWindow, SW_SHOWNORMAL);
         EnableWindow(hWindow, true);

         data.md.Width            = (UINT)resData.borderless.width;
         data.md.Height           = (UINT)resData.borderless.height;
         data.md.RefreshRate      = { 0, 1 };
         data.md.Format           = data.bbFormat;
         data.md.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
         data.md.Scaling          = DXGI_MODE_SCALING_UNSPECIFIED;
         curWinState         = resData.state = 1;

         ResizeBackbuffer(ScrRes, ae_borderless);
      }
   }

   void SetFullscreenWindow(cHWND hWindow, RESOLUTION &resData) {
      if(curWinState != 2) {
         EnableWindow(hWindow, false);
         SetWindowLongPtrW(hWindow, GWL_EXSTYLE, WS_EX_NOREDIRECTIONBITMAP);
         SetWindowLongPtrW(hWindow, GWL_STYLE, NULL);
         SetWindowPos(hWindow, HWND_TOPMOST, 0, 0, (int)resData.full.width, (int)resData.full.height, SWP_NOMOVE | SWP_NOSIZE | SWP_DEFERERASE | SWP_NOCOPYBITS | SWP_NOREDRAW | SWP_FRAMECHANGED);
         ShowWindow(hWindow, SW_SHOWNORMAL);
         EnableWindow(hWindow, true);
         Try(stSetFullscr, ren.swapchain->SetFullscreenState(true, NULL), ss_video);

         data.md.Width            = (UINT)resData.full.width;
         data.md.Height           = (UINT)resData.full.height;
         data.md.RefreshRate      = { 0, 1 };
         data.md.Format           = data.bbFormat;
         data.md.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
         data.md.Scaling          = DXGI_MODE_SCALING_UNSPECIFIED;
         curWinState         = resData.state = 2;

         ResizeBackbuffer(ScrRes, ae_fullscreen);
      }
   }

   inline void DestroyBuffers(void) {
      for(ui8 xx = 0; xx < 2; xx++) {
         if(ren.pDSV) ren.pDSV->Release();
         if(ren.pDepthBuffer[xx]) ren.pDepthBuffer[xx]->Release();
         if(ren.pBackBuffer[xx]) ren.pBackBuffer[xx]->Release();
      }
      if(ren.rtvBackBuffer) ren.rtvBackBuffer->Release();
   }
};
