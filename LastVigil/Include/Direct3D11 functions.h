/************************************************************
 * File: Direct3D11 functions.h         Created: 2022/10/20 *
 *                                Last modified: 2024/06/17 *
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

inline  void     MessagePump(void);
LRESULT CALLBACK RndrWndProc(HWND, UINT, WPARAM, LPARAM);

al64 struct CLASS_GPU {
   CLASS_FILEOPS &files;

   ID3D11Device  *dev;
   IDXGIDevice4  *dxgiDev;
   IDXGIAdapter3 *dxgiAdapter;

   CLASS_CONFIG   cfg = files;
   CLASS_BUFFERS  buf;
   CLASS_CAM      cam = buf;
   CLASS_LIGHTS   lit;
   CLASS_TEXTURES tex;

   IDXGIFactory5   *factory;
   IDXGISwapChain2 *swapchain;

   ID3D11DeviceContext *devcon[4];

   CLASS_RENDER ren = { cfg, buf, tex };

   ID3D11Texture2D        *pBackBuffer[2];
   ID3D11Texture2D        *pDepthBuffer[2];
   ID3D11RenderTargetView *rtvBackBuffer[2];
   ID3D11DepthStencilView *pDSV;

   DXGI_SWAP_CHAIN_DESC1           scd1 {};
   DXGI_SWAP_CHAIN_FULLSCREEN_DESC scfd {};
   DXGI_MODE_DESC                  md {};
   D3D11_DEPTH_STENCIL_VIEW_DESC   dsvd {};
   D3D11_VIEWPORT                  viewport {};
   D3D11_TEXTURE2D_DESC            td {};
   D3D_FEATURE_LEVEL               d3dFL[5] { (D3D_FEATURE_LEVEL)NULL, D3D_FEATURE_LEVEL_12_1, D3D_FEATURE_LEVEL_12_0, D3D_FEATURE_LEVEL_11_1 };
   DXGI_FORMAT                     bbFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;

//   cui32 flags = (NDEBUG ? NULL : D3D11_CREATE_DEVICE_DEBUG);
   cui32 flags = (NDEBUG ? D3D11_CREATE_DEVICE_SINGLETHREADED : D3D11_CREATE_DEVICE_SINGLETHREADED | D3D11_CREATE_DEVICE_DEBUG);// | D3D11_CREATE_DEVICE_PREVENT_INTERNAL_THREADING_OPTIMIZATIONS);

   ui32  msaaQlevel[33]     = {};
   ui32  uiMSAA             = 1;
   si8   currentWindowState = -1;

///--- Add booleans for selectively enabling each sub-class ---///
   CLASS_GPU(CLASS_FILEOPS &fileOps) : files(fileOps) {
      Try(stCreateDev, D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, flags, &d3dFL[1], 3, D3D11_SDK_VERSION, &dev, &d3dFL[0], &devcon[0]), ss_video);
      Try(stQueryInt, dev->QueryInterface(__uuidof(IDXGIDevice4), (void **)&dxgiDev), ss_video);
      Try(stDevGetPar, dxgiDev->GetParent(__uuidof(IDXGIAdapter3), (void **)&dxgiAdapter), ss_video);
      Try(stAdaGetPar, dxgiAdapter->GetParent(__uuidof(IDXGIFactory4), (void **)&factory), ss_video);

//      dxgiDev->SetGPUThreadPriority(7);

#ifdef AE_PTR_LIB
      ptrLib[0] = &files;
      ptrLib[1] = this;
#endif
      cfg.dev = buf.dev = cam.dev = lit.dev = tex.dev = ren.dev = dev;
      cfg.devcon = buf.devcon = cam.devcon = lit.devcon = tex.devcon = ren.devcon = devcon;
      ren.bufVertex = buf.CreateVertex(&ren.vCube, sizeof(ren.vCube), 1, 1);
      GetCurrentDirectoryW(512, files.pathWorking);
      mset(cfg.shaderIndex, sizeof(ui8[6][CFG_MAX_SHADERS]), ui8(CFG_MAX_SHADERS - 1));   // Default all indices to unused
   }

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

   HANDLE InitialiseBackbuffer(cHWND hWindow, cfl32 width, cfl32 height, cfl32 gamma, cui32 formatBackBuffer, cui32 formatDepthBuffer, cui8 msaa, cui8 msaaQuality, cui8 swapBuffers, cAE_WINDOW_STATE windowState) {
      for(ui8 xx = 1; xx < 33; xx++)
         Try(stCheckMSQL, dev->CheckMultisampleQualityLevels((DXGI_FORMAT)formatBackBuffer, xx, &msaaQlevel[xx]), ss_video);
      scd1.Width       = (UINT)width;
      scd1.Height      = (UINT)height;
      scd1.Format      = (DXGI_FORMAT)formatBackBuffer;
      scd1.Stereo      = 0;
      scd1.SampleDesc  = { msaa, (msaaQuality >= msaaQlevel[msaa] ? msaaQlevel[msaa] : msaaQuality - 1) };
      scd1.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
      scd1.BufferCount = swapBuffers;
      scd1.Scaling     = DXGI_SCALING_NONE;
      scd1.SwapEffect  = (msaa > 1 ? DXGI_SWAP_EFFECT_DISCARD : DXGI_SWAP_EFFECT_FLIP_DISCARD);
//      scd1.SwapEffect  = (msaa > 1 ? DXGI_SWAP_EFFECT_DISCARD : DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL);
      scd1.AlphaMode   = DXGI_ALPHA_MODE_UNSPECIFIED;
//      scd1.Flags       = NULL;
      scd1.Flags       = (windowState <= 1 ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING | DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT : DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
      scfd.RefreshRate      = { 0, 1 };
      scfd.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
      scfd.Scaling          = DXGI_MODE_SCALING_UNSPECIFIED; //STRETCHED;
      scfd.Windowed         = (windowState > 1 ? false : true);

      Try(stCreateSCFH, factory->CreateSwapChainForHwnd(dev, hWindow, &scd1, &scfd, NULL, (IDXGISwapChain1 **)&swapchain), ss_video);
      Try(stMakeWinAss, factory->MakeWindowAssociation(hWindow, DXGI_MWA_NO_ALT_ENTER), ss_video);
      Try(stSetMaxFLat, swapchain->SetMaximumFrameLatency(swapBuffers - 1), ss_video);

      td.Width      = (UINT)width;
      td.Height     = (UINT)height;
      td.MipLevels  = 1;
      td.ArraySize  = 1;
      td.Format     = (DXGI_FORMAT)formatDepthBuffer;
      td.SampleDesc = { msaa, (msaaQuality > msaaQlevel[msaa] ? msaaQlevel[msaa] : msaaQuality) };
      td.Usage      = D3D11_USAGE_DEFAULT;
      td.BindFlags  = D3D11_BIND_DEPTH_STENCIL;
      Try(stCreateTex2D, dev->CreateTexture2D(&td, NULL, &pDepthBuffer[0]), ss_video);

      Try(stResizeTar, swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (ptrptr)&pBackBuffer[0]), ss_video);
      Try(stCreateRTV, dev->CreateRenderTargetView(pBackBuffer[0], NULL, &rtvBackBuffer[0]), ss_video);
      pBackBuffer[0]->Release();

      dsvd.Format             = (DXGI_FORMAT)formatDepthBuffer;
      dsvd.ViewDimension      = D3D11_DSV_DIMENSION_TEXTURE2D;
      dsvd.Texture2D.MipSlice = 0;
      Try(stCreateDSV, dev->CreateDepthStencilView(pDepthBuffer[0], &dsvd, &pDSV), ss_video);

      viewport.TopLeftX = 0;
      viewport.TopLeftY = 0;
      viewport.Width    = width;
      viewport.Height   = height;
      viewport.MinDepth = 0.0f;
      viewport.MaxDepth = 1.0f;
      devcon[0]->RSSetViewports(1, &viewport);

      bbFormat = (DXGI_FORMAT)formatBackBuffer;
      uiMSAA   = msaa;

      return swapchain->GetFrameLatencyWaitableObject();
   };

   HANDLE InitialiseBackbuffer(cHWND hWindow, RESOLUTION &resData, cAE_WINDOW_STATE windowState) {
      cRESDATA &curDimData = resData.dims[windowState];

      for(ui8 xx = 1; xx < 33; xx++)
         Try(stCheckMSQL, dev->CheckMultisampleQualityLevels((DXGI_FORMAT)curDimData.fmtBB, xx, &msaaQlevel[xx]), ss_video);
      scd1.Width       = (UINT)curDimData.width;
      scd1.Height      = (UINT)curDimData.height;
      scd1.Format      = (DXGI_FORMAT)curDimData.fmtBB;
      scd1.Stereo      = 0;
      scd1.SampleDesc  = { (ui32)curDimData.msaa, (curDimData.msaaQ > msaaQlevel[curDimData.msaa] ? msaaQlevel[curDimData.msaa]: curDimData.msaaQ) };
      scd1.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
      scd1.BufferCount = curDimData.buffers;
      scd1.Scaling     = DXGI_SCALING_NONE;
      scd1.SwapEffect  = (curDimData.msaa > 1 ? DXGI_SWAP_EFFECT_DISCARD : DXGI_SWAP_EFFECT_FLIP_DISCARD);
//      scd1.SwapEffect  = (msaa > 1 ? DXGI_SWAP_EFFECT_DISCARD : DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL);
      scd1.AlphaMode   = DXGI_ALPHA_MODE_UNSPECIFIED;
//      scd1.Flags       = NULL;
      scd1.Flags       = (windowState <= 1 ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING | DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT : DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
      scfd.RefreshRate      = { 0, 1 };
      scfd.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
      scfd.Scaling          = DXGI_MODE_SCALING_UNSPECIFIED; //STRETCHED;
      scfd.Windowed         = (windowState > 1 ? false : true);

      Try(stCreateSCFH, factory->CreateSwapChainForHwnd(dev, hWindow, &scd1, &scfd, NULL, (IDXGISwapChain1 **)&swapchain), ss_video);
      Try(stMakeWinAss, factory->MakeWindowAssociation(hWindow, DXGI_MWA_NO_ALT_ENTER), ss_video);
      Try(stSetMaxFLat, swapchain->SetMaximumFrameLatency(curDimData.buffers - 1), ss_video);

      td.Width      = (UINT)curDimData.width;
      td.Height     = (UINT)curDimData.height;
      td.MipLevels  = 1;
      td.ArraySize  = 1;
      td.Format     = (DXGI_FORMAT)curDimData.fmtDB;
      td.SampleDesc = { (ui32)curDimData.msaa, (curDimData.msaaQ > msaaQlevel[curDimData.msaa] ? msaaQlevel[curDimData.msaa] : curDimData.msaaQ) };
      td.Usage      = D3D11_USAGE_DEFAULT;
      td.BindFlags  = D3D11_BIND_DEPTH_STENCIL;
      Try(stCreateTex2D, dev->CreateTexture2D(&td, NULL, &pDepthBuffer[0]), ss_video);

      Try(stResizeTar, swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (ptrptr)&pBackBuffer[0]), ss_video);
      Try(stCreateRTV, dev->CreateRenderTargetView(pBackBuffer[0], NULL, &rtvBackBuffer[0]), ss_video);
      pBackBuffer[0]->Release();

      dsvd.Format             = (DXGI_FORMAT)curDimData.fmtDB;
      dsvd.ViewDimension      = D3D11_DSV_DIMENSION_TEXTURE2D;
      dsvd.Texture2D.MipSlice = 0;
      Try(stCreateDSV, dev->CreateDepthStencilView(pDepthBuffer[0], &dsvd, &pDSV), ss_video);

      viewport.TopLeftX = 0;
      viewport.TopLeftY = 0;
      viewport.Width    = curDimData.width;
      viewport.Height   = curDimData.height;
      viewport.MinDepth = 0.0f;
      viewport.MaxDepth = 1.0f;
      devcon[0]->RSSetViewports(1, &viewport);

      bbFormat = (DXGI_FORMAT)curDimData.fmtBB;
      uiMSAA   = curDimData.msaa;

      resData.state = windowState;

      return swapchain->GetFrameLatencyWaitableObject();
   };

   void ResizeBackbuffer(cRESOLUTION &resData, cAE_WINDOW_STATE windowState) {
      cRESDATA &curDimData = resData.dims[windowState];

      Try(stResizeTar, swapchain->ResizeTarget(&md), ss_video);
      devcon[0]->OMSetRenderTargets(0, NULL, NULL);
      rtvBackBuffer[0]->Release();
      pDSV->Release();
      Try(stSCResizeBuf, swapchain->ResizeBuffers(0, (UINT)curDimData.width, (UINT)curDimData.height, (DXGI_FORMAT)curDimData.fmtBB,
                         (windowState <= 1 ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING | DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT : DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH)), ss_video);

      td.Width      = (UINT)curDimData.width;
      td.Height     = (UINT)curDimData.height;
      td.MipLevels  = 1;
      td.ArraySize  = 1;
      td.Format     = (DXGI_FORMAT)curDimData.fmtDB;
      td.SampleDesc = { (ui32)curDimData.msaa, (curDimData.msaaQ > msaaQlevel[curDimData.msaa] ? msaaQlevel[curDimData.msaa] : curDimData.msaaQ) };
      td.Usage      = D3D11_USAGE_DEFAULT;
      td.BindFlags  = D3D11_BIND_DEPTH_STENCIL;
      Try(stCreateTex2D, dev->CreateTexture2D(&td, NULL, &pDepthBuffer[0]), ss_video);

      Try(stSCGetBuf, swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (ptrptr)&pBackBuffer[0]), ss_video);
      Try(stCreateRTV, dev->CreateRenderTargetView(pBackBuffer[0], NULL, &rtvBackBuffer[0]), ss_video);
      pBackBuffer[0]->Release();

      dsvd.Format             = (DXGI_FORMAT)curDimData.fmtDB;
      dsvd.ViewDimension      = D3D11_DSV_DIMENSION_TEXTURE2D;
      dsvd.Texture2D.MipSlice = 0;
      Try(stCreateDSV, dev->CreateDepthStencilView(pDepthBuffer[0], &dsvd, &pDSV), ss_video);

      viewport.TopLeftX = 0;
      viewport.TopLeftY = 0;
      viewport.Width    = curDimData.width;
      viewport.Height   = curDimData.height;
      viewport.MinDepth = 0.0f;
      viewport.MaxDepth = 1.0f;
      devcon[0]->RSSetViewports(1, &viewport);

      devcon[0]->OMSetRenderTargets(1, &rtvBackBuffer[0], pDSV);

      bbFormat = (DXGI_FORMAT)curDimData.fmtBB;
      uiMSAA   = curDimData.msaa;
   }

   void SetBorderedWindow(cHWND hWindow, RESOLUTION &resData) {
      if(currentWindowState) {
         if(resData.state == 2)
            Try(stSetFullscr, swapchain->SetFullscreenState(false, NULL), ss_video);
         EnableWindow(hWindow, false);
         SetWindowLongPtrW(hWindow, GWL_EXSTYLE, WS_EX_NOREDIRECTIONBITMAP);
         SetWindowLongPtrW(hWindow, GWL_STYLE, WS_CAPTION | WS_MAXIMIZEBOX);
         SetWindowPos(hWindow, HWND_NOTOPMOST, resData.windowOS.x - 8, resData.windowOS.y - 31, (int)resData.window.width + 16,
                      (int)resData.window.height + 39, SWP_DEFERERASE | SWP_NOCOPYBITS | SWP_NOREDRAW | SWP_FRAMECHANGED);
         ShowWindow(hWindow, SW_SHOWNORMAL);
         EnableWindow(hWindow, true);

         md.Width            = (UINT)resData.window.width;
         md.Height           = (UINT)resData.window.height;
         md.RefreshRate      = { 0, 1 };
         md.Format           = bbFormat;
         md.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
         md.Scaling          = DXGI_MODE_SCALING_UNSPECIFIED;
         currentWindowState  = resData.state = 0;

         ResizeBackbuffer(ScrRes, ae_windowed);
      }
   }

   void SetBorderlessWindow(cHWND hWindow, RESOLUTION &resData) {
      if(currentWindowState != 1) {
         if(resData.state == 2)
            Try(stSetFullscr, swapchain->SetFullscreenState(false, NULL), ss_video);
         EnableWindow(hWindow, false);
         SetWindowLongPtrW(hWindow, GWL_EXSTYLE, WS_EX_NOREDIRECTIONBITMAP | WS_EX_OVERLAPPEDWINDOW);
         SetWindowLongPtrW(hWindow, GWL_STYLE, NULL);
         SetWindowPos(hWindow, HWND_TOPMOST, 0, 0, (int)resData.borderless.width, (int)resData.borderless.height, SWP_NOSIZE | SWP_DEFERERASE | SWP_NOCOPYBITS | SWP_NOREDRAW | SWP_FRAMECHANGED);
         ShowWindow(hWindow, SW_SHOWNORMAL);
         EnableWindow(hWindow, true);

         md.Width            = (UINT)resData.borderless.width;
         md.Height           = (UINT)resData.borderless.height;
         md.RefreshRate      = { 0, 1 };
         md.Format           = bbFormat;
         md.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
         md.Scaling          = DXGI_MODE_SCALING_UNSPECIFIED;
         currentWindowState  = resData.state = 1;

         ResizeBackbuffer(ScrRes, ae_borderless);
      }
   }

   void SetFullscreenWindow(cHWND hWindow, RESOLUTION &resData) {
      if(currentWindowState != 2) {
         EnableWindow(hWindow, false);
         SetWindowLongPtrW(hWindow, GWL_EXSTYLE, WS_EX_NOREDIRECTIONBITMAP);
         SetWindowLongPtrW(hWindow, GWL_STYLE, NULL);
         SetWindowPos(hWindow, HWND_TOPMOST, 0, 0, (int)resData.full.width, (int)resData.full.height, SWP_NOMOVE | SWP_NOSIZE | SWP_DEFERERASE | SWP_NOCOPYBITS | SWP_NOREDRAW | SWP_FRAMECHANGED);
         ShowWindow(hWindow, SW_SHOWNORMAL);
         EnableWindow(hWindow, true);
         Try(stSetFullscr, swapchain->SetFullscreenState(true, NULL), ss_video);

         md.Width            = (UINT)resData.full.width;
         md.Height           = (UINT)resData.full.height;
         md.RefreshRate      = { 0, 1 };
         md.Format           = bbFormat;
         md.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
         md.Scaling          = DXGI_MODE_SCALING_UNSPECIFIED;
         currentWindowState  = resData.state = 2;

         ResizeBackbuffer(ScrRes, ae_fullscreen);
      }
   }

   inline void ClearFlipRenderViews(cSSE4Df32 colour) const {
      //al16 static ui32 page;
      devcon[0]->OMSetRenderTargets(1, &rtvBackBuffer[0], pDSV);
      devcon[0]->ClearRenderTargetView(rtvBackBuffer[0], (cfl32ptr)&colour);
      devcon[0]->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
      //page = (page + 1) & 0x01;
   }

   inline void PresentRenderTarget(const ui8 vsyncs) const {   // Add test for vsync enabled
      Try(stSCPresent, (ScrRes.state != 2 ? swapchain->Present(0, DXGI_PRESENT_ALLOW_TEARING) : swapchain->Present(vsyncs, 0)), ss_video);
//      static DXGI_PRESENT_PARAMETERS PP = { 0, NULL, NULL, NULL };
//      Try(stSCPresent, (ScrRes.state != 2 ? swapchain->Present1(0, DXGI_PRESENT_ALLOW_TEARING, &PP) : swapchain->Present1(vsyncs, 0, &PP)));
      sysData.gpu.total.frames++;
      sysData.gpu.frame.prevDrawCalls = sysData.gpu.frame.curDrawCalls;
      sysData.gpu.frame.curDrawCalls = 0;
   }

   inline void DestroyBuffers(void) {
      for(ui8 xx = 0; xx < 2; xx++) {
         if(pDSV) pDSV->Release();
         if(rtvBackBuffer[xx]) rtvBackBuffer[xx]->Release();
         if(pDepthBuffer[xx]) pDepthBuffer[xx]->Release();
         if(pBackBuffer[xx]) pBackBuffer[xx]->Release();
      }
   }
};
