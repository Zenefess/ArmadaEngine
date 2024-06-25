/************************************************************
 * File: class_textures.h               Created: 2022/10/20 *
 *                                Last modified: 2024/06/15 *
 *                                                          *
 * Desc: 4 textures per material.                           *
 *                                                          *
 * Notes: 2024/05/06: Added support for data tracking       *
 *                                                          *
 *  Copyright (c) David William Bull. All rights reserved.  *
 ************************************************************/
#pragma once

#ifdef DATA_TRACKING
#include "data tracking.h"
#endif

enum TEXTYPE { GUI, TERRAIN, SPRITE };

al16 struct CLASS_TEXTURES {
   ID3D11Device              *dev;
   ID3D11DeviceContext      **devcon;
   ID3D11Texture2D          **const pTexture = (ID3D11Texture2D**)malloc64(sizeof(ptr[MAX_TEXTURES]));
   ID3D11ShaderResourceView **const pSRV     = (ID3D11ShaderResourceView**)malloc64(sizeof(ptr[MAX_SRVs]));
   D3DX11_IMAGE_LOAD_INFO     ilo;
   D3D11_TEXTURE2D_DESC       td;
   si32                       index = 0;

   void ConfigData(cDWORD width, cDWORD height, cDWORD depth, cDWORD mipLevels, cDWORD usage, cbool cpuRead, cbool cpuWrite, cbool shaderResource, cbool renderTarget, cbool unorderedAccess) {
      ilo.Width          = width;
      ilo.Height         = height;
      ilo.Depth          = depth;
      ilo.FirstMipLevel  = 0;
      ilo.MipLevels      = mipLevels;
      ilo.Usage          = (D3D11_USAGE)usage;
      ilo.BindFlags      = ((shaderResource & 1) << 3) | ((renderTarget & 1) << 5) | ((unorderedAccess & 1) << 7);
      ilo.CpuAccessFlags = ((cpuRead & 1) << 16) | ((cpuWrite & 1) << 17);
      ilo.MiscFlags      = NULL;
      ilo.Format         = DXGI_FORMAT_FROM_FILE;
      ilo.Filter         = D3DX11_FILTER_TRIANGLE;
      ilo.MipFilter      = D3DX11_FILTER_TRIANGLE;
   }

   si32 Load2D(LPCTSTR filename) {
      if(index > MAX_TEXTURES) return -1;
      D3DX11CreateTextureFromFileW(dev, filename, &ilo, NULL, (ID3D11Resource **)&pTexture[index], &hr);
      if(FAILED(hr)) Try(stTexFromFile, -2, ss_video);
      pTexture[index]->GetDesc(&td);
#ifdef DATA_TRACKING
      sysData.storage.filesOpened++;
      sysData.storage.filesClosed++;
      sysData.storage.bytesRead += td.ArraySize;
#endif
      dev->CreateShaderResourceView(pTexture[index], NULL, &pSRV[index]);
      return index++;
   }

   si32 LoadTextureSet(LPCTSTR atlasName, TEXTYPE type) {
      al16 static cwchar stFilepath[3][20] = { L"textures\\gui\\", L"textures\\terrain\\", L"textures\\sprites\\" };
                  wchar stFilename[3][128];

      if(index > MAX_TEXTURES - 3) return -1;

      wsprintf(stFilename[0], L"%s%s.dif.dds", stFilepath[type], atlasName);
      wsprintf(stFilename[1], L"%s%s.nor.dds", stFilepath[type], atlasName);
      wsprintf(stFilename[2], L"%s%s.mod.dds", stFilepath[type], atlasName);

      D3DX11CreateTextureFromFile(dev, stFilename[0], &ilo, NULL, (ID3D11Resource **)&pTexture[index], &hr);
      if(FAILED(hr)) Try(stTexFromFile, -2, ss_video);
      pTexture[index]->GetDesc(&td);
#ifdef DATA_TRACKING
      sysData.storage.filesOpened++;
      sysData.storage.filesClosed++;
      sysData.storage.bytesRead += td.ArraySize;
#endif
      dev->CreateShaderResourceView(pTexture[index], NULL, &pSRV[index]);
      D3DX11CreateTextureFromFile(dev, stFilename[1], &ilo, NULL, (ID3D11Resource **)&pTexture[++index], &hr);
      if(FAILED(hr)) Try(stTexFromFile, -3, ss_video);
      pTexture[index]->GetDesc(&td);
#ifdef DATA_TRACKING
      sysData.storage.filesOpened++;
      sysData.storage.filesClosed++;
      sysData.storage.bytesRead += td.ArraySize;
#endif
      dev->CreateShaderResourceView(pTexture[index], NULL, &pSRV[index]);
      D3DX11CreateTextureFromFile(dev, stFilename[2], &ilo, NULL, (ID3D11Resource **)&pTexture[++index], &hr);
      if(FAILED(hr)) Try(stTexFromFile, -4, ss_video);
      pTexture[index]->GetDesc(&td);
#ifdef DATA_TRACKING
      sysData.storage.filesOpened++;
      sysData.storage.filesClosed++;
      sysData.storage.bytesRead += td.ArraySize;
#endif
      dev->CreateShaderResourceView(pTexture[index], NULL, &pSRV[index]);
      return index++ - 2;
   }

   inline void SetVS2D(ui8 context, ui16 index, ui16 slot, ui16 count) { devcon[context]->VSSetShaderResources(slot, count, &pSRV[index]); }

   inline void SetGS2D(ui8 context, ui16 index, ui16 slot, ui16 count) { devcon[context]->GSSetShaderResources(slot, count, &pSRV[index]); }

   inline void SetPS2D(ui8 context, ui16 index, ui16 slot, ui16 count) { devcon[context]->PSSetShaderResources(slot, count, &pSRV[index]); }

   inline void Unload2D(si32 index) { pSRV[index]->Release(); pTexture[index]->Release(); }
};
