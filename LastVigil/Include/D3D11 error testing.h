/************************************************************
 * File: D3D11 error testing.h          Created: 2023/02/07 *
 *                                Last modified: 2024/06/29 *
 *                                                          *
 * Desc:                                                    *
 *                                                          *
 *                         Copyright (c) David William Bull *
 ************************************************************/
#pragma once

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
cchar stSetRTandUAV[] = "devcon->OMSetRenderTargetsAndUnorderedAccessViews";
