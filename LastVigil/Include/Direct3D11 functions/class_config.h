/************************************************************
 * File: class_config.h                 Created: 2022/10/20 *
 *                                Last modified: 2024/07/04 *
 *                                                          *
 * Desc:                                                    *
 *                                                          *
 *  Copyright (c) David William Bull. All rights reserved.  *
 ************************************************************/
#pragma once

#include "master header.h"

//#ifdef DATA_TRACKING
//#include "data tracking.h"
//extern SYSTEM_DATA sysData;
//#endif

al32 struct CLASS_CONFIG {   // Rewrite to use files., malloc pointers, and change to "&" where appropriate
   CLASS_FILEOPS &files;

   ID3D11Device         *dev;
   ID3D11DeviceContext **devcon;

   // Allocate all blob and shader pointers packed
   ID3DBlob *(*const pBlob)[CFG_MAX_SHADERS] = (ID3DBlob*(*)[CFG_MAX_SHADERS])zalloc64(RoundUpToNearest64(sizeof(ptr[(CFG_MAX_SHADERS * 9u) + (CFG_MAX_STATES * 5u)])));
   ID3D11VertexShader      **const pVS  = (ID3D11VertexShader **)pBlob + (CFG_MAX_SHADERS * 6u);
   ID3D11PixelShader       **const pPS  = (ID3D11PixelShader**)pVS + CFG_MAX_SHADERS;
   ID3D11GeometryShader    **const pGS  = (ID3D11GeometryShader**)pPS + CFG_MAX_SHADERS;
   ID3D11BlendState        **const pBS  = (ID3D11BlendState**)pGS + CFG_MAX_SHADERS;
   ID3D11SamplerState      **const pSS  = (ID3D11SamplerState**)pBS + CFG_MAX_STATES;
   ID3D11InputLayout       **const pIL  = (ID3D11InputLayout**)pSS + CFG_MAX_STATES;
   ID3D11DepthStencilState **const pDSS = (ID3D11DepthStencilState**)pIL + CFG_MAX_STATES;
   ID3D11RasterizerState   **const pRS  = (ID3D11RasterizerState**)pDSS + CFG_MAX_STATES;

   ui8   (*shaderIndex)[CFG_MAX_SHADERS]       = (ui8(*)[CFG_MAX_SHADERS])malloc64(RoundUpToNearest64(sizeof(ui8[6][CFG_MAX_SHADERS])));
   wchar (*shaderString)[CFG_MAX_SHADERS][256] = (wchar(*)[CFG_MAX_SHADERS][256])malloc64(RoundUpToNearest64(sizeof(wchar[6][CFG_MAX_SHADERS][256])));

   ui32 shaderBanks[6] = {};
   ui32 shaderCount    = 0;

   // 4 bytes spare

   al8 D3D11_BLEND_DESC blendDesc = { 0, 0, 0, D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_INV_SRC_ALPHA, D3D11_BLEND_OP_ADD, D3D11_BLEND_ONE, D3D11_BLEND_ZERO, D3D11_BLEND_OP_ADD, D3D11_COLOR_WRITE_ENABLE_ALL };
//   CHAR             stShaderModel[3][7] { "vs_5_0", "gs_5_0", "ps_5_0" };

   CLASS_CONFIG(CLASS_FILEOPS &fileOps) : files(fileOps) {};

   void LoadAllShaderGroups(void) {
      UINT flags = D3DCOMPILE_ENABLE_STRICTNESS | //D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR | //D3DCOMPILE_WARNINGS_ARE_ERRORS |
#ifdef _DEBUG
                   D3DCOMPILE_OPTIMIZATION_LEVEL0 | D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_DEBUG;
#else
                   D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif

      files.LoadShaderList(shaderIndex, shaderString, shaderBanks, shaderCount);
      for(ui16 type = 0; type < 6; type++)
         for(ui16 index = 0; index < shaderBanks[type]; index++)
            if(shaderString[type][index]) {
               Try(stReadToBlob, D3DReadFileToBlob(shaderString[type][index], &pBlob[type][index]), ss_video);
#ifdef DATA_TRACKING
               sysData.storage.filesOpened++;
               sysData.storage.filesClosed++;
               sysData.storage.bytesRead += pBlob[type][index]->GetBufferSize();
#endif
               switch(type) {
               case 1:
                  if(pBlob[type][index]) Try(stCreateVS, dev->CreateVertexShader(pBlob[type][index]->GetBufferPointer(), pBlob[type][index]->GetBufferSize(), NULL, &pVS[index]), ss_video);
                  break;
               case 2:
                  if(pBlob[type][index]) Try(stCreateGS, dev->CreateGeometryShader(pBlob[type][index]->GetBufferPointer(), pBlob[type][index]->GetBufferSize(), NULL, &pGS[index]), ss_video);
                  break;
               case 5:
                  if(pBlob[type][index]) Try(stCreatePS, dev->CreatePixelShader(pBlob[type][index]->GetBufferPointer(), pBlob[type][index]->GetBufferSize(), NULL, &pPS[index]), ss_video);
                  break;
               }
            }
   }

   inline void SetVertexShader(cui8 context, cui8 shader) const { devcon[context]->VSSetShader(pVS[shader], 0, 0); }

   inline void SetGeometryShader(cui8 context, cui8 shader) const { devcon[context]->GSSetShader(pGS[shader], 0, 0); }

   inline void SetPixelShader(cui8 context, cui8 shader) const { devcon[context]->PSSetShader(pPS[shader], 0, 0); }

   inline void SetShaderGroup(cui8 context, cui8 shaders) const {
      devcon[context]->VSSetShader(pVS[shaderIndex[1][shaders]], 0, 0);
      devcon[context]->GSSetShader(pGS[shaderIndex[2][shaders]], 0, 0);
      devcon[context]->PSSetShader(pPS[shaderIndex[5][shaders]], 0, 0);
   }

   // Returns profile index if successful
   cui32 CreateVertexFormat(csi8 profileIndex, csi8 vertexShader, csi8 inputFormat) {
      static const D3D11_INPUT_ELEMENT_DESC ied[7][8] = {
         // Map format
         { {"INDEX", 0, DXGI_FORMAT_R32_UINT, 0, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1}, {}, {}, {}, {}, {}, {}, {} },
         // Entity format
         { {"INDEX", 0, DXGI_FORMAT_R32_UINT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}, {}, {}, {}, {}, {}, {}, {} },
         // GUI entry format
         { {"INDEX", 0, DXGI_FORMAT_R32_UINT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}, {}, {}, {}, {}, {}, {}, {} },
         // GUI cursor format
         { {"POSITION", 0, DXGI_FORMAT_R32G32B32_SINT, 0, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1},
           {"COLOUR", 0, DXGI_FORMAT_R32_UINT, 0, 12, D3D11_INPUT_PER_INSTANCE_DATA, 1},
           {}, {}, {}, {}, {}, {} },
         // Entity sprite format
         { {"INDEX", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
           {"INDEX", 1, DXGI_FORMAT_R32G32B32A32_UINT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0},
           {"INDEX", 2, DXGI_FORMAT_R32G32B32A32_UINT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
           {"INDEX", 3, DXGI_FORMAT_R32G32B32A32_UINT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0},
           {"INDEX", 4, DXGI_FORMAT_R32G32B32A32_UINT, 0, 64, D3D11_INPUT_PER_VERTEX_DATA, 0},
           {"INDEX", 5, DXGI_FORMAT_R32G32B32A32_UINT, 0, 80, D3D11_INPUT_PER_VERTEX_DATA, 0},
           {"INDEX", 6, DXGI_FORMAT_R32G32B32A32_UINT, 0, 96, D3D11_INPUT_PER_VERTEX_DATA, 0},
           {"INDEX", 7, DXGI_FORMAT_R32G32B32A32_UINT, 0, 112, D3D11_INPUT_PER_VERTEX_DATA, 0} },
         // Spares
         { {}, {}, {}, {}, {}, {}, {}, {} },
         { {}, {}, {}, {}, {}, {}, {}, {} }
      };
      cui8 numEntries[7] = { 1, 1, 1, 2, 8, 0, 0 };
      si8  i;

      if(profileIndex == -1) {
         for(i = 0; pIL[i], i < 10; i++);
         if(i == 10) return 0x080000001;
      }
      i = profileIndex;

      Try(stCreateIL, dev->CreateInputLayout(ied[inputFormat], numEntries[inputFormat], pBlob[1][vertexShader]->GetBufferPointer(), pBlob[1][vertexShader]->GetBufferSize(), &pIL[i]), ss_video);

      return i;
   }

   inline void SetVertexFormat(cui8 context, cui8 profile) const { devcon[context]->IASetInputLayout(pIL[profile]); }

   csi8 CreateSamplerState(cui8 context, const D3D11_TEXTURE_ADDRESS_MODE addrMode, const D3D11_COMPARISON_FUNC compFunc, const D3D11_FILTER filter, csi32 maxAnis) {
      al16 static D3D11_SAMPLER_DESC sd {};
           static si8                iStates {};

      if(iStates >= 8) return -1;
      sd.AddressU = addrMode;
      sd.AddressV = addrMode;
      sd.AddressW = addrMode;
      sd.ComparisonFunc = compFunc;
      sd.Filter = filter;
      sd.MaxAnisotropy = maxAnis;
      sd.MinLOD = 0.0f;
      sd.MaxLOD = D3D11_FLOAT32_MAX;
      sd.MipLODBias = 0.0f;
      if(!pSS[iStates]) Try(stCreateSS, dev->CreateSamplerState(&sd, &pSS[iStates]), ss_video);
      return iStates++;
   }

   inline void SetSamplerStates(cui8 context, cui8 slot, cui8 count, cui8 state) const {
      devcon[context]->PSSetSamplers(slot, count, &pSS[state]);
   }

   void CreateBlendStates(void) {
      // Opaque rendering
      blendDesc.RenderTarget[0].BlendEnable = false;
      blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_MAX;
      blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_MAX;
      blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
      blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
      blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
      blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
      blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
      if(!pBS[0]) dev->CreateBlendState(&blendDesc, &pBS[0]);
      // Transparent rendering
      blendDesc.RenderTarget[0].BlendEnable = true;
      blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
      blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
      blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
      blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
      blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
      blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
      blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
      if(!pBS[1]) dev->CreateBlendState(&blendDesc, &pBS[1]);
      // Transparent additive rendering
      blendDesc.RenderTarget[0].BlendEnable = true;
      blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
      blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
      blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
      blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
      blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
      blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
      blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
      if(!pBS[2]) dev->CreateBlendState(&blendDesc, &pBS[2]);
      // Transparent masked rendering
      blendDesc.RenderTarget[0].BlendEnable = true;
      blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_MAX;
      blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_MAX;
      blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
      blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
      blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
      blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
      blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
      if(!pBS[3]) dev->CreateBlendState(&blendDesc, &pBS[3]);
   }

   inline void SetBlendState(cui8 context, cui8 state) const { devcon[context]->OMSetBlendState(pBS[state], NULL, 0x0FFFFFFFF); }

   si8 CreateDepthStencilState(bool depthEnable, D3D11_DEPTH_WRITE_MASK depthWriteMask, D3D11_COMPARISON_FUNC depthFunc, bool stencilEnable, UINT8 stencilReadMask, UINT8 stencilWriteMask) {
      static D3D11_DEPTH_STENCIL_DESC dsd {};
      static si8                      iStates {};

      if(iStates >= 8) return -1;
      dsd.DepthEnable = depthEnable;
      dsd.DepthWriteMask = depthWriteMask;
      dsd.DepthFunc = depthFunc;
      dsd.StencilEnable = stencilEnable;
      dsd.StencilReadMask = stencilReadMask;
      dsd.StencilWriteMask = stencilWriteMask;
      if(!pDSS[iStates]) Try(stCreateDSV, dev->CreateDepthStencilState(&dsd, &pDSS[iStates]), ss_video);
      return iStates++;
   }

   inline void SetDepthStencilState(cui8 context, csi8 state, cui32 stencilRef) const {
      devcon[context]->OMSetDepthStencilState(pDSS[state], stencilRef);
   }

   void CreateCullingState(void) const {
      D3D11_RASTERIZER_DESC rasterDesc[4] = { { D3D11_FILL_SOLID,     D3D11_CULL_NONE, false, 0, 0.0f, 0.0f, true, false, false, false },
                                              { D3D11_FILL_WIREFRAME, D3D11_CULL_NONE, false, 0, 0.0f, 0.0f, true, false, false, false },
                                              { D3D11_FILL_SOLID,     D3D11_CULL_BACK, false, 0, 0.0f, 0.0f, true, false, false, false },
                                              { D3D11_FILL_WIREFRAME, D3D11_CULL_BACK, false, 0, 0.0f, 0.0f, true, false, false, false } };

      dev->CreateRasterizerState(&rasterDesc[0], &pRS[0]);
      dev->CreateRasterizerState(&rasterDesc[1], &pRS[1]);
      dev->CreateRasterizerState(&rasterDesc[2], &pRS[2]);
      dev->CreateRasterizerState(&rasterDesc[3], &pRS[3]);
   }

   // state: 0==Solid not culled, 1==Wireframe not culled, 2==Solid back culled, 3==Wireframe back culled
   inline void SetCullingState(cui8 context, cui8 state) const { devcon[context]->RSSetState(pRS[state]); }

   void UnloadShaderData(void) const {
      for(ui8 xx = 0; xx < 8; xx++) {
         if(pVS[xx]) pVS[xx]->Release();
         if(pGS[xx]) pGS[xx]->Release();
         if(pPS[xx]) pPS[xx]->Release();
      }
   }

   void UnloadBlenderStates(cui8 context) const { for(ui8 xx = 0; xx < 8; xx++) if(pBS[xx]) pBS[xx]->Release(); }

   void UnloadSamplerStates(cui8 context) const { for(ui8 xx = 0; xx < 8; xx++) if(pSS[xx]) pSS[xx]->Release(); }

   void UnloadInputLayouts(cui8 context) const { for(ui8 xx = 0; xx < 8; xx++) if(pIL[xx]) pIL[xx]->Release(); }
};
