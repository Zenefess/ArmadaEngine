/************************************************************
 * File: class_config.h                 Created: 2022/10/20 *
 *                                Last modified: 2023/04/24 *
 *                                                          *
 * Desc:                                                    *
 *                                                          *
 *  Copyright (c) David William Bull. All rights reserved.  *
 ************************************************************/
#pragma once

al16 struct CLASS_CONFIG {   // malloc pointers and change to "&" where appropriate
   al16 ID3DBlob                *pBlob[6][100] {};
        ID3D11VertexShader      *pVS[100] {};
        ID3D11GeometryShader    *pGS[101] {};
        ID3D11PixelShader       *pPS[100] {};
        ID3D11BlendState        *pBS[10] {};
        ID3D11SamplerState      *pSS[10] {};
        ID3D11InputLayout       *pIL[10] {};
        ID3D11DepthStencilState *pDSS[10] {};
        ID3D11RasterizerState   *pRS[10] {};
        ID3D11Device            *dev;
        ID3D11DeviceContext    **devcon;
        CLASS_FILEOPS           *files;
        D3D11_BLEND_DESC         blendDesc = { 0, 0, 0, D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_INV_SRC_ALPHA, D3D11_BLEND_OP_ADD, D3D11_BLEND_ONE, D3D11_BLEND_ZERO, D3D11_BLEND_OP_ADD, D3D11_COLOR_WRITE_ENABLE_ALL };
//        CHAR                     stShaderModel[3][7] { "vs_5_0", "gs_5_0", "ps_5_0" };

   void LoadAllShaderGroups(void) {
      UINT flags = D3DCOMPILE_ENABLE_STRICTNESS | //D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR | //D3DCOMPILE_WARNINGS_ARE_ERRORS |
#ifdef _DEBUG
                   D3DCOMPILE_OPTIMIZATION_LEVEL0 | D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_DEBUG;
#else
                   D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif

      files->LoadShaderList();
      for(ui16 type = 0; type < 6; type++)
         for(ui16 index = 0; index < files->uiBanks[type]; index++)
            if(files->stShader[type][index]) {
               Try(stReadToBlob, D3DReadFileToBlob(files->stShader[type][index], &pBlob[type][index]));
               switch(type) {
               case 1:
                  if(pBlob[type][index]) Try(stCreateVS, dev->CreateVertexShader(pBlob[type][index]->GetBufferPointer(), pBlob[type][index]->GetBufferSize(), NULL, &pVS[index]));
                  break;
               case 2:
                  if(pBlob[type][index]) Try(stCreateGS, dev->CreateGeometryShader(pBlob[type][index]->GetBufferPointer(), pBlob[type][index]->GetBufferSize(), NULL, &pGS[index]));
                  break;
               case 5:
                  if(pBlob[type][index]) Try(stCreatePS, dev->CreatePixelShader(pBlob[type][index]->GetBufferPointer(), pBlob[type][index]->GetBufferSize(), NULL, &pPS[index]));
                  break;
               }
            }
   }

   inline void SetVertexShader(cui8 context, cui8 shader) const { devcon[context]->VSSetShader(pVS[shader], 0, 0); }

   inline void SetGeometryShader(cui8 context, cui8 shader) const { devcon[context]->GSSetShader(pGS[shader], 0, 0); }

   inline void SetPixelShader(cui8 context, cui8 shader) const { devcon[context]->PSSetShader(pPS[shader], 0, 0); }

   inline void SetShaderGroup(cui8 context, cui8 shaders) const {
      devcon[context]->VSSetShader(pVS[files->uiIndex[1][shaders]], 0, 0);
      devcon[context]->GSSetShader(pGS[files->uiIndex[2][shaders]], 0, 0);
      devcon[context]->PSSetShader(pPS[files->uiIndex[5][shaders]], 0, 0);
   }

   // Returns profile index if successful
   cui32 CreateVertexFormat(si8 profileIndex, si8 vertexShader, si8 inputFormat) {
      static const D3D11_INPUT_ELEMENT_DESC ied[7][8] = {
         // Map format
         { {"INDEX", 0, DXGI_FORMAT_R32_UINT, 0, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1},
            {}, {}, {}, {}, {}, {}, {} },
         // Entity format
         { {"INDEX", 0, DXGI_FORMAT_R32_UINT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {}, {}, {}, {}, {}, {}, {} },
         // GUI entry format
         { {"INDEX", 0, DXGI_FORMAT_R16_UINT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {}, {}, {}, {}, {}, {}, {} },
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

      Try(stCreateIL, dev->CreateInputLayout(ied[inputFormat], numEntries[inputFormat], pBlob[1][vertexShader]->GetBufferPointer(), pBlob[1][vertexShader]->GetBufferSize(), &pIL[i]));

      return i;
   }

   inline void SetVertexFormat(ui8 context, ui8 profile) const { devcon[context]->IASetInputLayout(pIL[profile]); }

   si8 CreateSamplerState(cui8 context, const D3D11_TEXTURE_ADDRESS_MODE addrMode, const D3D11_COMPARISON_FUNC compFunc, D3D11_FILTER filter, csi32 maxAnis) {
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
      if(!pSS[iStates]) Try(stCreateSS, dev->CreateSamplerState(&sd, &pSS[iStates]));
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
      if(!pDSS[iStates]) Try(stCreateDSV, dev->CreateDepthStencilState(&dsd, &pDSS[iStates]));
      return iStates++;
   }

   inline void SetDepthStencilState(cui8 context, csi8 state, cui32 stencilRef) const {
      devcon[context]->OMSetDepthStencilState(pDSS[state], stencilRef);
   }

   void CreateCullingState(void) {
      D3D11_RASTERIZER_DESC rasterDesc[2] = { { D3D11_FILL_SOLID, D3D11_CULL_NONE, false, 0, 0.0f, 0.0f, true, false, false, false },
                                              { D3D11_FILL_WIREFRAME, D3D11_CULL_NONE, false, 0, 0.0f, 0.0f, true, false, false, false } };

      dev->CreateRasterizerState(&rasterDesc[0], &pRS[0]);
      dev->CreateRasterizerState(&rasterDesc[1], &pRS[1]);
   }

   inline void SetCullingState(cui8 context, cui8 state) const { devcon[context]->RSSetState(pRS[state]); }

   void UnloadShaderData(void) const {
      for(ui8 xx = 0; xx < 8; xx++) if(pVS[xx]) pVS[xx]->Release();
      for(ui8 xx = 0; xx < 8; xx++) if(pGS[xx]) pGS[xx]->Release();
      for(ui8 xx = 0; xx < 8; xx++) if(pPS[xx]) pPS[xx]->Release();
   }

   void UnloadBlenderStates(cui8 context) const { for(ui8 xx = 0; xx < 8; xx++) if(pBS[xx]) pBS[xx]->Release(); }

   void UnloadSamplerStates(cui8 context) const { for(ui8 xx = 0; xx < 8; xx++) if(pSS[xx]) pSS[xx]->Release(); }

   void UnloadInputLayouts(cui8 context) const { for(ui8 xx = 0; xx < 8; xx++) if(pIL[xx]) pIL[xx]->Release(); }
};
