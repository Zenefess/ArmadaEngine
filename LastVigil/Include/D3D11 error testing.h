/************************************************************
 * File: D3D11 error testing.h          Created: 2023/02/07 *
 *                                Last modified: 2023/02/07 *
 *                                                          *
 * Desc:                                                    *
 *                                                          *
 *  Copyright (c) David William Bull. All rights reserved.  *
 ************************************************************/
#pragma once

#include "pch.h"
#include "typedefs.h"
#include <stdio.h>

cchar stCreateWin[]   = "CreateWindow";
cchar stReadToBlob[]  = "D3DReadFileToBlob";
cchar stCreateDev[]   = "D3D11CreateDevice";
cchar stTexFromFile[] = "D3DX11CreateTextureFromFile";
cchar stCheckMSQL[]   = "dev->CheckMultisampleQualityLevels";
cchar stCreateBuf[]   = "dev->CreateBuffer";
cchar stCreateCS[]    = "dev->CreateComputeShader";
cchar stCreateDSV[]   = "dev->CreateDepthStencilView";
cchar stCreateGS[]    = "dev->CreateGeometryShader";
cchar stCreateIL[]    = "dev->CreateInputLayout";
cchar stCreatePS[]    = "dev->CreatePixelShader";
cchar stCreateRTV[]   = "dev->CreateRenderTargetView";
cchar stCreateSS[]    = "dev->CreateSamplerState";
cchar stCreateTex2D[] = "dev->CreateTexture2D";
cchar stCreateVS[]    = "dev->CreateVertexShader";
cchar stQueryInt[]    = "dev->QueryInterface";
cchar stMap[]         = "devcon->Map";
cchar stAdaGetPar[]   = "dxgiAdapter->GetParent";
cchar stDevGetPar[]   = "dxgiDev->GetParent";
cchar stCreateSCFH[]  = "factory->CreateSwapChainForHwnd";
cchar stMakeWinAss[]  = "factory->MakeWindowAssociation";
cchar stSCGetBuf[]    = "swapchain->GetBuffer";
cchar stSCPresent[]   = "swapchain->Present";
cchar stResizeTar[]   = "swapchain->ResizeTarget";
cchar stSCResizeBuf[] = "swapchain->ResizeBuffers";
cchar stSetFullscr[]  = "swapchain->SetFullscreenState";
cchar stSetMaxFLat[]  = "swapchain->SetMaximumFrameLatency";

cchar stLoadAlpha[]   = "Invalid alphabet file";
cchar stLoadShaders[] = "Invalid shader.cfg file";

extern vui64       THREAD_LIFE;
extern HANDLE      hErrorOutput;
extern CLASS_TIMER mainTimer;

extern void WriteError(cchptr, bool);

inline static void Try(cchptr stEvent, ui32 uiResult) {
   static char stDescription[64];

   if(uiResult & 0x080000000) {
      sprintf(stDescription, "VIDEO:%04X : %s", uiResult & 0x03FFFFFFF, stEvent);
      WriteError(stDescription, (uiResult & 0x040000000 ? true : false));
   }
}
