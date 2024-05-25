/************************************************************
 * File: class_lights.h                 Created: 2022/10/20 *
 *                                Last modified: 2024/05/23 *
 *                                                          *
 * Desc:                                                    *
 *                                                          *
 * Copyright (c) David William Bull.   All rights reserved. *
 ************************************************************/
#pragma once

#include "Data structures.h"

#define MAX_LIGHTS 1024

al16 struct CLASS_LIGHTS {
   ID3D11Device         *dev;
   ID3D11DeviceContext **devcon;

   CB_LIGHT *n = NULL;

   ID3D11Buffer            *pCB = NULL;
   D3D11_BUFFER_DESC        bd  = {};
   D3D11_SUBRESOURCE_DATA   srd = {};
   D3D11_MAPPED_SUBRESOURCE ms  = {};
   ui16                     totalLights = 0;

   si16 Create(cui8 context, csi16 count, cui8 cBufSlot) {
      if(!n) n = (CB_LIGHT *)zalloc32(sizeof(CB_LIGHT[MAX_LIGHTS]));

      if(totalLights + count >= MAX_LIGHTS) return -1;

      if(pCB) pCB->Release();
      totalLights += count;
      bd.ByteWidth = sizeof(CB_LIGHT) * totalLights;
      bd.Usage = D3D11_USAGE_DYNAMIC;
      bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
      bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
      bd.MiscFlags = 0;
      bd.StructureByteStride = sizeof(CB_LIGHT);
      srd.pSysMem = n;
      Try(stCreateBuf, dev->CreateBuffer(&bd, &srd, &pCB));
      devcon[context]->PSSetConstantBuffers(cBufSlot, 1, (ID3D11Buffer *const *)&pCB);

      return totalLights;
   }

   inline void SetColour(cui16 light, cVEC3Df colour) { n[light].col = colour; }

   inline void UploadAll(cui8 context) {
      Try(stMap, devcon[context]->Map(pCB, 0, D3D11_MAP_WRITE_DISCARD, NULL, &ms));
      memcpy(ms.pData, n, sizeof(CB_LIGHT) * totalLights);
      devcon[context]->Unmap(pCB, 0);
   }

   void DestroyAll(void) {
      mdealloc(n);
      pCB->Release();
      totalLights = 0;
   }
};
