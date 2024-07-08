/************************************************************
 * File: class_buffers.h                Created: 2022/10/20 *
 *                                Last modified: 2024/06/29 *
 *                                                          *
 * Desc:                                                    *
 *                                                          *
 *  Copyright (c) David William Bull. All rights reserved.  *
 ************************************************************/
#pragma once

#include "Data structures.h"

al16 struct CLASS_BUFFERS {
   ID3D11Device                     *dev;
   ID3D11DeviceContext             **devcon;
   ID3D11Buffer                   *(*const pBuffer)[256]   = (ID3D11Buffer*(*)[256])malloc64(sizeof(ptr[4][256])); // 0 == Vertex, 1 == Index, 2 == Constant, 3 == Structured
   ID3D11ShaderResourceView        **const pSRV            = (ID3D11ShaderResourceView**)malloc64(sizeof(ptr[1024]));
   ID3D11UnorderedAccessView       **const pUAV            = (ID3D11UnorderedAccessView**)malloc64(sizeof(ptr[1024]));
   D3D11_SHADER_RESOURCE_VIEW_DESC  *const srvd            = (D3D11_SHADER_RESOURCE_VIEW_DESC*)zalloc64(sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC[16]));
   D3D11_UNORDERED_ACCESS_VIEW_DESC *const uavd            = (D3D11_UNORDERED_ACCESS_VIEW_DESC*)zalloc64(sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC[16]));
   D3D11_BUFFER_DESC               (*const bd)[256]        = (D3D11_BUFFER_DESC(*)[256])zalloc64(sizeof(D3D11_BUFFER_DESC[4][256])); // 0 == Vertex, 1 == Index, 2 == Constant, 3 == Structured
   ui32                            (*const uiStrides)[272] = (ui32(*)[272])zalloc64(sizeof(ui32[2][272]));
   ui32                            (*const uiOffsets)[272] = (ui32(*)[272])zalloc64(sizeof(ui32[2][272]));
   D3D11_MAPPED_SUBRESOURCE          ms {};
   D3D11_SUBRESOURCE_DATA            srd {};
   si16                              bufferCount[4] {};

   csi32 CreateVertex(ptrc source, cui32 stride, cui32 numVerts, csi16 count) {
      csi32 index = bufferCount[0];

      bd[0][index].ByteWidth           = stride * numVerts;
      bd[0][index].Usage               = D3D11_USAGE_DYNAMIC;
      bd[0][index].BindFlags           = D3D11_BIND_VERTEX_BUFFER;
      bd[0][index].CPUAccessFlags      = D3D11_CPU_ACCESS_WRITE;
      bd[0][index].MiscFlags           = 0;
      bd[0][index].StructureByteStride = stride;
      uiStrides[0][index] = stride;
      uiOffsets[0][index] = 0;

      for (si32 xx = 0; xx < count; xx++) {
         srd.pSysMem = (ui8 *)source + ui64(stride * numVerts * xx);
         Try(stCreateBuf, dev->CreateBuffer(&bd[0][index + xx], &srd, &pBuffer[0][index + xx]), ss_video);
         bufferCount[0]++;
      }
      return index;   // Index of first buffer allocated
   }

   csi32 CreateIndex(cui8 context, ptrc source, cui32 stride, csi16 count) {
      csi32 index = bufferCount[1];

      bd[1][index].ByteWidth           = stride;
      bd[1][index].Usage               = D3D11_USAGE_DYNAMIC;
      bd[1][index].BindFlags           = D3D11_BIND_INDEX_BUFFER;
      bd[1][index].CPUAccessFlags      = D3D11_CPU_ACCESS_WRITE;
      bd[1][index].MiscFlags           = 0;
      bd[1][index].StructureByteStride = stride;

      for (si32 xx = 0; xx < count; xx++) {
         srd.pSysMem = (ui8 *)source + ui64(stride * xx);
         Try(stCreateBuf, dev->CreateBuffer(&bd[1][index + xx], &srd, &pBuffer[1][index + xx]), ss_video);
         bufferCount[1]++;
      }

      return index;   // Index of first buffer allocated
   }

   csi32 CreateConstant(cui8 context, ptrc source, cui32 stride, csi16 count, cui16 slot, cui8 stage, cbool immutable) {
      csi32 index = bufferCount[2];

      bd[2][index].ByteWidth           = stride;
      bd[2][index].Usage               = (immutable ? D3D11_USAGE_IMMUTABLE : D3D11_USAGE_DYNAMIC);
      bd[2][index].BindFlags           = D3D11_BIND_CONSTANT_BUFFER;
      bd[2][index].CPUAccessFlags      = (immutable ? NULL : D3D11_CPU_ACCESS_WRITE);
      bd[2][index].MiscFlags           = NULL;
      bd[2][index].StructureByteStride = stride;

      for (si32 xx = 0; xx < count; xx++) {
         srd.pSysMem = (ui8 *)source + ui64(stride * xx);
         Try(stCreateBuf, dev->CreateBuffer(&bd[2][index + xx], &srd, &pBuffer[2][index + xx]), ss_video);
         bufferCount[2]++;
      }
      if(stage & ae_stage_vertex)
         devcon[context]->VSSetConstantBuffers(slot, count, (ID3D11Buffer *const *)&pBuffer[2][index]);
      if(stage & ae_stage_geometry)
         devcon[context]->GSSetConstantBuffers(slot, count, (ID3D11Buffer *const *)&pBuffer[2][index]);
      if(stage & ae_stage_pixel)
         devcon[context]->PSSetConstantBuffers(slot, count, (ID3D11Buffer *const *)&pBuffer[2][index]);

      return index;   // Index of first buffer allocated
   }

   // BindFlags == D3D11_BIND_SHADER_RESOURCE
   csi32 CreateStructured(cui8 context, cptrc source, cui32 stride, cui32 count, cui8 usageType) {
      csi32 index = bufferCount[3];

      bd[3][index].ByteWidth           = stride * count;
      bd[3][index].Usage               = (D3D11_USAGE)usageType; //(immutable ? D3D11_USAGE_IMMUTABLE : D3D11_USAGE_DYNAMIC);
      bd[3][index].BindFlags           = D3D11_BIND_SHADER_RESOURCE;
      bd[3][index].CPUAccessFlags      = (usageType < 2 ? NULL : D3D11_CPU_ACCESS_WRITE); //(immutable ? NULL : D3D11_CPU_ACCESS_WRITE);
      bd[3][index].MiscFlags           = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
      bd[3][index].StructureByteStride = stride;

      srd.pSysMem = (ui8 *)source;
      Try(stCreateBuf, dev->CreateBuffer(&bd[3][index], &srd, &pBuffer[3][index]), ss_video);
      bufferCount[3]++;

      srvd[0].Format = DXGI_FORMAT_UNKNOWN;
      srvd[0].ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
      srvd[0].Buffer.ElementOffset = 0;
      srvd[0].Buffer.ElementWidth = stride;
      dev->CreateShaderResourceView(pBuffer[3][index], NULL, &pSRV[index]);

      return index;
   }

   // Set custom bind flags
   csi32 CreateStructured(cui8 context, cptrc source, cui32 stride, cui32 count, cui32 bindFlags, cui8 usageType) {
      csi32 index = bufferCount[3];

      bd[3][index].ByteWidth           = stride * count;
      bd[3][index].Usage               = (D3D11_USAGE)usageType; //(immutable ? D3D11_USAGE_IMMUTABLE : D3D11_USAGE_DYNAMIC);
      bd[3][index].BindFlags           = bindFlags;
      bd[3][index].CPUAccessFlags      = (usageType < 2 ? NULL : D3D11_CPU_ACCESS_WRITE); //(immutable ? NULL : D3D11_CPU_ACCESS_WRITE);
      bd[3][index].MiscFlags           = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
      bd[3][index].StructureByteStride = stride;

      srd.pSysMem = (ui8 *)source;
      Try(stCreateBuf, dev->CreateBuffer(&bd[3][index], &srd, &pBuffer[3][index]), ss_video);
      bufferCount[3]++;

      uavd[0].Format = DXGI_FORMAT_UNKNOWN;
      uavd[0].ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
      uavd[0].Buffer.FirstElement = 0;
      uavd[0].Buffer.NumElements = count;
      uavd[0].Buffer.Flags = 0;
      dev->CreateUnorderedAccessView(pBuffer[3][index], &uavd[index], &pUAV[index]);

      srvd[0].Format = DXGI_FORMAT_UNKNOWN;
      srvd[0].ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
      srvd[0].Buffer.ElementOffset = 0;
      srvd[0].Buffer.ElementWidth = stride;
      dev->CreateShaderResourceView(pBuffer[3][index], NULL, &pSRV[index]);

      return index;
   }

   // Update [count] vertices of vertex buffer [index] with data from [source]
   inline void UpdateVertex(cui8 context, cptrc source, cui16 index, cui32 count) {
      Try(stMap, devcon[context]->Map(pBuffer[0][index], NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms), ss_video);

      Stream(source, ms.pData, bd[0][index].StructureByteStride * count);

      devcon[context]->Unmap(pBuffer[0][index], NULL);
   }

   inline void UpdateIndex(cui8 context, cptrc source, cui16 index, si16 numBuffers) {
      do {
         numBuffers--;
         Try(stMap, devcon[context]->Map(pBuffer[1][index + numBuffers], NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms), ss_video);
         memcpy(ms.pData, (ui8 *)source + bd[1][index].ByteWidth * numBuffers, bd[1][index].ByteWidth);
         devcon[context]->Unmap(pBuffer[1][index + numBuffers], NULL);
      } while (numBuffers);
   }

   inline void UpdateConstant(cui8 context, cptrc source, cui16 index, si16 numBuffers) {
      do {
         numBuffers--;
         Try(stMap, devcon[context]->Map(pBuffer[2][index + numBuffers], NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms), ss_video);
         //memcpy(ms.pData, (ui8 *)source + bd[2][index].ByteWidth * numBuffers, bd[2][index].ByteWidth);
         Stream16((ui8 *)source + bd[2][index].ByteWidth * numBuffers, ms.pData, bd[2][index].ByteWidth);
         devcon[context]->Unmap(pBuffer[2][index + numBuffers], NULL);
      } while (numBuffers);
   }

   inline void UpdateStructured(cui8 context, cptrc source, cui16 index, si16 numBuffers) {
      do {
         numBuffers--;
         Try(stMap, devcon[context]->Map(pBuffer[3][index + numBuffers], NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms), ss_video);
         //memcpy(ms.pData, (ui8 *)source + bd[3][index].ByteWidth * numBuffers, bd[3][index].ByteWidth);
         Stream16((ui8 *)source + bd[3][index].ByteWidth * numBuffers, ms.pData, bd[3][index].ByteWidth);
         devcon[context]->Unmap(pBuffer[3][index + numBuffers], NULL);
      } while (numBuffers);
   }

   inline void UpdateStructuredSubresource(cui8 context, cptrc source, cui16 index, cui32 byteCount) {
      const D3D11_BOX boxData = { 0, 0, 0, byteCount, 1, 1};
      devcon[context]->UpdateSubresource(pBuffer[3][index], 0, &boxData, source, 0, 0);
   }

   inline ptrc LockStructuredBeforeUpdate(cui8 context, csi16 index) {
      Try(stMap, devcon[context]->Map(pBuffer[3][index], NULL, D3D11_MAP_WRITE_NO_OVERWRITE, NULL, &ms), ss_video);

      return ms.pData;
   }

   inline void UnlockStructuredAfterUpdate(cui8 context, csi16 index) const { devcon[context]->Unmap(pBuffer[3][index], NULL); }

   inline void SetVertexPrimitive(cui8 context, cui16 index, cui8 slot, cui32 vertexFormat) const {
      devcon[context]->IASetVertexBuffers(slot, 1, &pBuffer[0][index], &uiStrides[0][index], &uiOffsets[0][index]);
      devcon[0]->IASetPrimitiveTopology((D3D11_PRIMITIVE_TOPOLOGY &)vertexFormat);
   }

   inline void SetVertexGroup(cui8 context, cui16 index, csi16 count, cui8 firstSlot, cui32 vertexFormat) const {
      devcon[context]->IASetVertexBuffers(firstSlot, count, &pBuffer[0][index], &uiStrides[0][index], &uiOffsets[0][index]);
      devcon[0]->IASetPrimitiveTopology((D3D11_PRIMITIVE_TOPOLOGY &)vertexFormat);
   }

   inline void SetVertexStagger(cui8 context, cui32 stride, cui32 offset, cui16 index, csi16 count, csi32 vertexFormat) {
      for(ui16 xx = 256; xx < count + 256; xx++) { uiStrides[0][xx] = stride; uiOffsets[0][xx] = offset; }
      devcon[context]->IASetVertexBuffers(index, count, &pBuffer[0][index], &uiStrides[0][256], &uiOffsets[0][256]);
      devcon[0]->IASetPrimitiveTopology((D3D11_PRIMITIVE_TOPOLOGY &)vertexFormat);
   }

   inline void SetIndex(cui8 context, cui16 index, cui8 slot) const { devcon[context]->IASetIndexBuffer(pBuffer[1][index], DXGI_FORMAT_R16_UINT, 0); }

   inline void SetVSR(cui8 context, cui16 index, cui16 slot, cui16 count) const { devcon[context]->VSSetShaderResources(slot, count, &pSRV[index]); }

   inline void SetGSR(cui8 context, cui16 index, cui16 slot, cui16 count) const { devcon[context]->GSSetShaderResources(slot, count, &pSRV[index]); }

   inline void SetPSR(cui8 context, cui16 index, cui16 slot, cui16 count) const { devcon[context]->PSSetShaderResources(slot, count, &pSRV[index]); }

   inline void SetUAV(cui8 context, cui16 index, cui16 slot, cui16 count) const {
      if(slot >= D3D11_1_UAV_SLOT_COUNT) Try(stSetRTandUAV, -1, ss_video);
      const UINT appendConsume = 0x0FFFFFFFFu;
      devcon[context]->OMSetRenderTargetsAndUnorderedAccessViews(0x0FFFFFFFF, NULL, NULL, slot, count, &pUAV[index], &appendConsume);
   }

   void DestroyAllVertex(ui16 bufCount) { do pBuffer[0][--bufferCount[0]]->Release(); while (bufferCount[0] && --bufCount); }

   void DestroyAllIndex(ui16 bufCount) { do pBuffer[1][--bufferCount[1]]->Release(); while (bufferCount[1] && --bufCount); }

   void DestroyAllConstant(ui16 bufCount) { do pBuffer[2][--bufferCount[2]]->Release(); while (bufferCount[2] && --bufCount); }
};
