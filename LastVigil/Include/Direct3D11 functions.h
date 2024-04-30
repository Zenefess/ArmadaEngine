/************************************************************
 * File: Direct3D11 functions.h         Created: 2022/10/20 *
 *                                Last modified: 2022/11/10 *
 *                                                          *
 * Desc:                                                    *
 *                                                          *
 *  Copyright (c) David William Bull. All rights reserved.  *
 ************************************************************/
#pragma once

#include "typedefs.h"
#include "Data structures.h"
#include "File operations.h"
#include "Direct3D11 functions\class_render.h"
#include "Direct3D11 functions\class_config.h"
#include "Direct3D11 functions\class_buffers.h"
#include "Direct3D11 functions\class_cameras.h"
#include "Direct3D11 functions\class_lights.h"
#include "Direct3D11 functions\class_textures.h"
#include "Direct3D11 functions\class_gui.h"

#ifdef _DEBUG
#define NDEBUG 0
#include "comdef.h"
#include "D3D11SDKLayers.h"
#endif

inline  void     MessagePump(void);
LRESULT CALLBACK RndrWndProc(HWND, UINT, WPARAM, LPARAM);

al32 struct CLASS_GPU {
   CLASS_FILEOPS  files;
   CLASS_CONFIG   cfg;
   CLASS_BUFFERS  buf;
   CLASS_CAM      cam;
   CLASS_RENDER   ren;
   CLASS_LIGHTS   lit;
   CLASS_TEXTURES tex;
   CLASS_GUI      gui;

   ID3D11Device           *dev;
   IDXGIDevice4           *dxgiDev;
   IDXGIAdapter3          *dxgiAdapter;
   IDXGIFactory5          *factory;
   ID3D11DeviceContext    *devcon[8];
   ID3D11Texture2D        *pBackBuffer[2], *pDepthBuffer[2];
   ID3D11RenderTargetView *rtvBackBuffer[2];
   ID3D11DepthStencilView *pDSV;
   IDXGISwapChain2        *swapchain;

   DXGI_SWAP_CHAIN_DESC1           scd1 {};
   DXGI_SWAP_CHAIN_FULLSCREEN_DESC scfd {};
   DXGI_MODE_DESC                  md {};
   D3D11_DEPTH_STENCIL_VIEW_DESC   dsvd {};
   D3D11_VIEWPORT                  viewport {};
   D3D11_TEXTURE2D_DESC            td {};
   D3D_FEATURE_LEVEL               d3dFL[5] { (D3D_FEATURE_LEVEL)NULL, D3D_FEATURE_LEVEL_12_1, D3D_FEATURE_LEVEL_12_0, D3D_FEATURE_LEVEL_11_1 };
   DXGI_FORMAT                     bbFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;

//   ui32 flags = (NDEBUG ? NULL : D3D11_CREATE_DEVICE_DEBUG), msaaQuality[33] {}, uiMSAA = 1;
   cui32 flags = (NDEBUG ? D3D11_CREATE_DEVICE_SINGLETHREADED : D3D11_CREATE_DEVICE_SINGLETHREADED | D3D11_CREATE_DEVICE_DEBUG);// | D3D11_CREATE_DEVICE_PREVENT_INTERNAL_THREADING_OPTIMIZATIONS);
   ui32  msaaQuality[33]{}, uiMSAA = 1;
   si8   currentWindowState = -1;

///--- Add booleans for selectively enabling each sub-class ---///
   CLASS_GPU() {
      Try(stCreateDev, D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, flags, &d3dFL[1], 3, D3D11_SDK_VERSION, &dev, &d3dFL[0], &devcon[0]));
      Try(stQueryInt, dev->QueryInterface(__uuidof(IDXGIDevice4), (void **)&dxgiDev));
      Try(stDevGetPar, dxgiDev->GetParent(__uuidof(IDXGIAdapter3), (void **)&dxgiAdapter));
      Try(stAdaGetPar, dxgiAdapter->GetParent(__uuidof(IDXGIFactory4), (void **)&factory));

//      dxgiDev->SetGPUThreadPriority(7);

#ifdef AE_PTR_LIB
      ptrLib[0] = &files;
      ptrLib[1] = this;
#endif
      cfg.files = gui.files = &files;
      cfg.dev = buf.dev = cam.dev = lit.dev = tex.dev = ren.dev = dev;
      cfg.devcon = buf.devcon = cam.devcon = lit.devcon = tex.devcon = ren.devcon = devcon; gui.devcon = devcon;
      cam.buf = ren.buf = gui.buf = &buf;
      ren.cfg = gui.cfg = &cfg;
      ren.tex = gui.tex = &tex;
      ren.gui = &gui;
      gui._Initialise();
      ren.bufVertex = buf.CreateVertex(&ren.vCube, sizeof(ren.vCube), 1, 1);
      GetCurrentDirectoryW(512, files.pathWorking);
      memset(files.uiIndex, 100, sizeof(files.uiIndex));   // Default all indices to unused
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
      if(!hWindow) Try(stCreateWin, -1);
      SetCursor(LoadCursor(NULL, IDC_ARROW));

      return hWindow;
   }

   inline void MouseCursor(const HWND hWnd, cbool visible) const {
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

   HANDLE InitialiseBackbuffer(const HWND hWindow, cfl32 width, cfl32 height, cui32 format, cui8 buffers, cui8 msaa, cbool windowed) {
      for(ui8 xx = 1; xx < 33; xx++)
         Try(stCheckMSQL, dev->CheckMultisampleQualityLevels((DXGI_FORMAT)format, xx, &msaaQuality[xx]));
      scd1.Width = (UINT)width;
      scd1.Height = (UINT)height;
      scd1.Format = (DXGI_FORMAT)format;
      scd1.Stereo = 0;
      scd1.SampleDesc.Count = msaa;
      scd1.SampleDesc.Quality = msaaQuality[msaa] - 1;
      scd1.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
      scd1.BufferCount = buffers;
      scd1.Scaling = DXGI_SCALING_NONE;
      scd1.SwapEffect = (msaa > 1 ? DXGI_SWAP_EFFECT_DISCARD : DXGI_SWAP_EFFECT_FLIP_DISCARD);
//      scd1.SwapEffect = (msaa > 1 ? DXGI_SWAP_EFFECT_DISCARD : DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL);
      scd1.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
//      scd1.Flags = NULL;
      scd1.Flags = (windowed ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING | DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT : DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
      scfd.RefreshRate = { 0, 1 };
      scfd.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
      scfd.Scaling = DXGI_MODE_SCALING_UNSPECIFIED; //STRETCHED;
      scfd.Windowed = windowed;

      Try(stCreateSCFH, factory->CreateSwapChainForHwnd(dev, hWindow, &scd1, &scfd, NULL, (IDXGISwapChain1 **)&swapchain));
      Try(stMakeWinAss, factory->MakeWindowAssociation(hWindow, DXGI_MWA_NO_ALT_ENTER));
      Try(stSetMaxFLat, swapchain->SetMaximumFrameLatency(buffers - 1));

      td.Width = (UINT)width;
      td.Height = (UINT)height;
      td.MipLevels = 1;
      td.ArraySize = 1;
      td.Format = DXGI_FORMAT_D32_FLOAT;
      td.SampleDesc.Count = msaa;
      td.SampleDesc.Quality = msaaQuality[msaa] - 1;
      td.Usage = D3D11_USAGE_DEFAULT;
      td.BindFlags = D3D11_BIND_DEPTH_STENCIL;
      Try(stCreateTex2D, dev->CreateTexture2D(&td, NULL, &pDepthBuffer[0]));

      Try(stResizeTar, swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID *)&pBackBuffer[0]));
      Try(stCreateRTV, dev->CreateRenderTargetView(pBackBuffer[0], NULL, &rtvBackBuffer[0]));
      pBackBuffer[0]->Release();

      dsvd.Format = DXGI_FORMAT_D32_FLOAT;
      dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
      dsvd.Texture2D.MipSlice = 0;
      Try(stCreateDSV, dev->CreateDepthStencilView(pDepthBuffer[0], &dsvd, &pDSV));

      viewport.TopLeftX = 0;
      viewport.TopLeftY = 0;
      viewport.Width = width;
      viewport.Height = height;
      viewport.MinDepth = 0.0f;
      viewport.MaxDepth = 1.0f;
      devcon[0]->RSSetViewports(1, &viewport);

      bbFormat = (DXGI_FORMAT)format;
      uiMSAA   = msaa;

      return swapchain->GetFrameLatencyWaitableObject();
   };

   void ResizeBackbuffer(cfl32 width, cfl32 height, cui32 format, cui8 msaa, cbool windowed) {
      Try(stResizeTar, swapchain->ResizeTarget(&md));
      devcon[0]->OMSetRenderTargets(0, NULL, NULL);
      rtvBackBuffer[0]->Release();
      pDSV->Release();
      Try(stSCResizeBuf, swapchain->ResizeBuffers(0, (UINT)width, (UINT)height, (DXGI_FORMAT)format, scd1.Flags));

      td.Width = (UINT)width;
      td.Height = (UINT)height;
      td.MipLevels = 1;
      td.ArraySize = 1;
      td.Format = DXGI_FORMAT_D32_FLOAT;
      td.SampleDesc.Count = msaa;
      td.SampleDesc.Quality = msaaQuality[msaa] - 1;
      td.Usage = D3D11_USAGE_DEFAULT;
      td.BindFlags = D3D11_BIND_DEPTH_STENCIL;
      Try(stCreateTex2D, dev->CreateTexture2D(&td, NULL, &pDepthBuffer[0]));

      Try(stSCGetBuf, swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID *)&pBackBuffer[0]));
      Try(stCreateRTV, dev->CreateRenderTargetView(pBackBuffer[0], NULL, &rtvBackBuffer[0]));
      pBackBuffer[0]->Release();

      dsvd.Format = DXGI_FORMAT_D32_FLOAT;
      dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
      dsvd.Texture2D.MipSlice = 0;
      Try(stCreateDSV, dev->CreateDepthStencilView(pDepthBuffer[0], &dsvd, &pDSV));

      viewport.TopLeftX = 0;
      viewport.TopLeftY = 0;
      viewport.Width = width;
      viewport.Height = height;
      viewport.MinDepth = 0.0f;
      viewport.MaxDepth = 1.0f;
      devcon[0]->RSSetViewports(1, &viewport);

      devcon[0]->OMSetRenderTargets(1, &rtvBackBuffer[0], pDSV);

      bbFormat = (DXGI_FORMAT)format;
      uiMSAA   = msaa;
   }

   void SetBorderedWindow(const HWND hWindow) {
      if(currentWindowState) {
         if(ScrRes.state == 2) Try(stSetFullscr, swapchain->SetFullscreenState(false, NULL));
         EnableWindow(hWindow, false);
         SetWindowLongPtrW(hWindow, GWL_EXSTYLE, WS_EX_NOREDIRECTIONBITMAP);
         SetWindowLongPtrW(hWindow, GWL_STYLE, WS_SYSMENU | WS_CAPTION | WS_MAXIMIZEBOX);
         SetWindowPos(hWindow, HWND_NOTOPMOST, ScrRes.windowOS.x - 8, ScrRes.windowOS.y - 31, (int)ScrRes.window.w + 16,
                      (int)ScrRes.window.h + 39, SWP_DEFERERASE | SWP_NOCOPYBITS | SWP_NOREDRAW | SWP_FRAMECHANGED);
         ShowWindow(hWindow, SW_SHOWNORMAL);
         EnableWindow(hWindow, true);
         md.Width = (UINT)ScrRes.window.w;
         md.Height = (UINT)ScrRes.window.h;
         md.RefreshRate = { 0, 1 };
         md.Format = bbFormat;
         md.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
         md.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
         currentWindowState = ScrRes.state = 0;
         this->ResizeBackbuffer(ScrRes.window.w, ScrRes.window.h, ScrRes.window.fmtBB, uiMSAA, true);
      }
   }

   void SetBorderlessWindow(const HWND hWindow) {
      if(currentWindowState != 1) {
         if(ScrRes.state == 2) Try(stSetFullscr, swapchain->SetFullscreenState(false, NULL));
         EnableWindow(hWindow, false);
         SetWindowLongPtrW(hWindow, GWL_EXSTYLE, WS_EX_NOREDIRECTIONBITMAP | WS_EX_OVERLAPPEDWINDOW);
         SetWindowLongPtrW(hWindow, GWL_STYLE, NULL);
         SetWindowPos(hWindow, HWND_TOPMOST, 0, 0, int(ScrRes.borderless.w), int(ScrRes.borderless.h),
                      SWP_NOSIZE | SWP_DEFERERASE | SWP_NOCOPYBITS | SWP_NOREDRAW | SWP_FRAMECHANGED);
         ShowWindow(hWindow, SW_SHOWNORMAL);
         EnableWindow(hWindow, true);
         md.Width = UINT(ScrRes.borderless.w);
         md.Height = UINT(ScrRes.borderless.h);
         md.RefreshRate = { 0, 1 };
         md.Format = bbFormat;
         md.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
         md.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
         currentWindowState = ScrRes.state = 1;
         this->ResizeBackbuffer(ScrRes.borderless.w, ScrRes.borderless.h, ScrRes.borderless.fmtBB, uiMSAA, true);
      }
   }

   void SetFullscreenWindow(const HWND hWindow) {
      if(currentWindowState != 2) {
         EnableWindow(hWindow, false);
         SetWindowLongPtrW(hWindow, GWL_EXSTYLE, WS_EX_NOREDIRECTIONBITMAP);
         SetWindowLongPtrW(hWindow, GWL_STYLE, NULL);
         SetWindowPos(hWindow, HWND_TOPMOST, 0, 0, int(ScrRes.full.w), int(ScrRes.full.h),
                      SWP_NOMOVE | SWP_NOSIZE | SWP_DEFERERASE | SWP_NOCOPYBITS | SWP_NOREDRAW | SWP_FRAMECHANGED);
         ShowWindow(hWindow, SW_SHOWNORMAL);
         EnableWindow(hWindow, true);
         Try(stSetFullscr, swapchain->SetFullscreenState(true, NULL));
         md.Width = UINT(ScrRes.full.w);
         md.Height = UINT(ScrRes.full.h);
         md.RefreshRate = { 0, 1 };
         md.Format = bbFormat;
         md.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
         md.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
         currentWindowState = ScrRes.state = 2;
         this->ResizeBackbuffer(ScrRes.full.w, ScrRes.full.h, ScrRes.full.fmtBB, uiMSAA, false);
      }
   }

   inline void ClearFlipRenderViews(cVEC4Df colour) const {
      //al16 static ui32 page;
      devcon[0]->OMSetRenderTargets(1, &rtvBackBuffer[0], pDSV);
      devcon[0]->ClearRenderTargetView(rtvBackBuffer[0], (const float *)&colour);
      devcon[0]->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
      //page = (page + 1) & 0x01;
   }

   inline void PresentRenderTarget(const ui8 vsyncs) const {   // Add test for vsync enabled
      Try(stSCPresent, (ScrRes.state != 2 ? swapchain->Present(0, DXGI_PRESENT_ALLOW_TEARING) : swapchain->Present(vsyncs, 0)));
//      static DXGI_PRESENT_PARAMETERS PP = { 0, NULL, NULL, NULL };
//      Try(stSCPresent, (ScrRes.state != 2 ? swapchain->Present1(0, DXGI_PRESENT_ALLOW_TEARING, &PP) : swapchain->Present1(vsyncs, 0, &PP)));
      sysData.gpu.total.frames++;
      sysData.gpu.frame.prevDrawCalls = sysData.gpu.frame.curDrawCalls;
      sysData.gpu.frame.curDrawCalls = 0;
   }

   inline void DestroyBuffers(void) {
      gui._Deinitialise();
      for(ui8 xx = 0; xx < 2; xx++) {
         if(pDSV) pDSV->Release();
         if(rtvBackBuffer[xx]) rtvBackBuffer[xx]->Release();
         if(pDepthBuffer[xx]) pDepthBuffer[xx]->Release();
         if(pBackBuffer[xx]) pBackBuffer[xx]->Release();
      }
   }
};
