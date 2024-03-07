/************************************************************
 * File: class_buffers.h                Created: 2022/10/20 *
 *                                Last modified: 2023/06/19 *
 *                                                          *
 * Desc:                                                    *
 *                                                          *
 *  Copyright (c) David William Bull. All rights reserved.  *
 ************************************************************/
#pragma once

#include "Data structures.h"

enum AE_BUFFER_USAGE : ui8 {
	USAGE_DEFAULT,
	USAGE_IMMUTABLE,
	USAGE_DYNAMIC,
	USAGE_STAGING
};

enum AE_GPU_STAGE : ui8 {
	STAGE_NULL,
	STAGE_VERTEX,
	STAGE_GEOMETRY,
	STAGES_VERTEX_GEOMETRY,
	STAGE_PIXEL,
	STAGES_VERTEX_PIXEL,
	STAGES_GEOMETRY_PIXEL,
	STAGES_VERTEX_GEOMETRY_PIXEL
};

al32 struct CLASS_BUFFERS {	// malloc pointers and change to "&" where appropriate
	ID3D11Buffer						  *pBuffer[4][256] {}; // 0 == Vertex, 1 == Index, 2 == Constant, 3 == Structured
	ID3D11Device						  *dev;
	ID3D11DeviceContext				 **devcon;
	ID3D11ShaderResourceView		  *pSRV[1024] {};
	D3D11_SHADER_RESOURCE_VIEW_DESC	srvd[16] {};
	D3D11_BUFFER_DESC						bd[4][256] {};
	ui32										uiStrides[2][272] {}, uiOffsets[2][272] {};
	si16										bufferCount[4] {};
	D3D11_MAPPED_SUBRESOURCE			ms {};
	D3D11_SUBRESOURCE_DATA				srd {};

	si32 CreateVertex(void *source, ui32 stride, ui32 numVerts, si16 count) {
		si32 index = bufferCount[0];

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
			Try(stCreateBuf, dev->CreateBuffer(&bd[0][index + xx], &srd, &pBuffer[0][index + xx]));
			bufferCount[0]++;
		}
		return index;	// Index of first buffer allocated
	}

	si32 CreateIndex(ui8 context, void *source, ui32 stride, si16 count) {
		si32 index = bufferCount[1];

		bd[1][index].ByteWidth           = stride;
		bd[1][index].Usage               = D3D11_USAGE_DYNAMIC;
		bd[1][index].BindFlags           = D3D11_BIND_INDEX_BUFFER;
		bd[1][index].CPUAccessFlags      = D3D11_CPU_ACCESS_WRITE;
		bd[1][index].MiscFlags           = 0;
		bd[1][index].StructureByteStride = stride;

		for (si32 xx = 0; xx < count; xx++) {
			srd.pSysMem = (ui8 *)source + ui64(stride * xx);
			Try(stCreateBuf, dev->CreateBuffer(&bd[1][index + xx], &srd, &pBuffer[1][index + xx]));
			bufferCount[1]++;
		}

		return index;	// Index of first buffer allocated
	}

	si32 CreateConstant(ui8 context, void *source, ui32 stride, si16 count, ui16 slot, ui8 stage, bool immutable) {
		si32 index = bufferCount[2];

		bd[2][index].ByteWidth           = stride;
		bd[2][index].Usage               = (immutable ? D3D11_USAGE_IMMUTABLE : D3D11_USAGE_DYNAMIC);
		bd[2][index].BindFlags           = D3D11_BIND_CONSTANT_BUFFER;
		bd[2][index].CPUAccessFlags      = (immutable ? NULL : D3D11_CPU_ACCESS_WRITE);
		bd[2][index].MiscFlags           = NULL;
		bd[2][index].StructureByteStride = stride;

		for (si32 xx = 0; xx < count; xx++) {
			srd.pSysMem = (ui8 *)source + ui64(stride * xx);
			Try(stCreateBuf, dev->CreateBuffer(&bd[2][index + xx], &srd, &pBuffer[2][index + xx]));
			bufferCount[2]++;
		}
		if(stage & STAGE_VERTEX)
			devcon[context]->VSSetConstantBuffers(slot, count, (ID3D11Buffer *const *)&pBuffer[2][index]);
		if(stage & STAGE_GEOMETRY)
			devcon[context]->GSSetConstantBuffers(slot, count, (ID3D11Buffer *const *)&pBuffer[2][index]);
		if(stage & STAGE_PIXEL)
			devcon[context]->PSSetConstantBuffers(slot, count, (ID3D11Buffer *const *)&pBuffer[2][index]);

		return index;	// Index of first buffer allocated
	}

	si32 CreateStructured(ui8 context, cptrc source, ui32 stride, ui32 count, ui8 usageType) {
		si32 index = bufferCount[3];

		bd[3][index].ByteWidth           = stride * count;
		bd[3][index].Usage               = (D3D11_USAGE)usageType; //(immutable ? D3D11_USAGE_IMMUTABLE : D3D11_USAGE_DYNAMIC);
		bd[3][index].BindFlags           = D3D11_BIND_SHADER_RESOURCE;
		bd[3][index].CPUAccessFlags      = (usageType == 1 ? NULL : D3D11_CPU_ACCESS_WRITE); //(immutable ? NULL : D3D11_CPU_ACCESS_WRITE);
		bd[3][index].MiscFlags           = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		bd[3][index].StructureByteStride = stride;

		srd.pSysMem = (ui8 *)source;
		Try(stCreateBuf, dev->CreateBuffer(&bd[3][index], &srd, &pBuffer[3][index]));
		bufferCount[3]++;

		srvd[0].Format = DXGI_FORMAT_UNKNOWN;
		srvd[0].ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		srvd[0].Buffer.ElementOffset = 0;
		srvd[0].Buffer.ElementWidth = stride;
		dev->CreateShaderResourceView(pBuffer[3][index], NULL, &pSRV[index]);

		return index++;
	}

	// Update [count] vertices of vertex buffer [index] with data from [source]
	inline void UpdateVertex(cui8 context, si16 index, ui32 count, cptr source) {
		Try(stMap, devcon[context]->Map(pBuffer[0][index], NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms));
		memcpy(ms.pData, (ui8 *)source, bd[0][index].StructureByteStride * count);
		devcon[context]->Unmap(pBuffer[0][index], NULL);
	}

	// Starting at vertex buffer [index] update [numBuffers] buffers with data from [source]
	inline void UpdateVertex(cui8 context, cptr source, si16 index, si16 numBuffers) {
		do {
			numBuffers--;
			Try(stMap, devcon[context]->Map(pBuffer[0][index + numBuffers], NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms));
			memcpy(ms.pData, (ui8 *)source + bd[0][index].ByteWidth * numBuffers, bd[0][index].ByteWidth);
			devcon[context]->Unmap(pBuffer[0][index + numBuffers], NULL);
		} while (numBuffers);
	}

	inline void UpdateIndex(cui8 context, cptr source, si16 index, si16 numBuffers) {
		do {
			numBuffers--;
			Try(stMap, devcon[context]->Map(pBuffer[1][index + numBuffers], NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms));
			memcpy(ms.pData, (ui8 *)source + bd[1][index].ByteWidth * numBuffers, bd[1][index].ByteWidth);
			devcon[context]->Unmap(pBuffer[1][index + numBuffers], NULL);
		} while (numBuffers);
	}

	inline void UpdateConstant(cui8 context, cptr source, si16 index, si16 numBuffers) {
		do {
			numBuffers--;
			Try(stMap, devcon[context]->Map(pBuffer[2][index + numBuffers], NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms));
			memcpy(ms.pData, (ui8 *)source + bd[2][index].ByteWidth * numBuffers, bd[2][index].ByteWidth);
			devcon[context]->Unmap(pBuffer[2][index + numBuffers], NULL);
		} while (numBuffers);
	}

	inline void UpdateStructured(cui8 context, cptr source, si16 index, si16 numBuffers) {
		do {
			numBuffers--;
			Try(stMap, devcon[context]->Map(pBuffer[3][index + numBuffers], NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms));
			memcpy(ms.pData, (ui8 *)source + bd[3][index].ByteWidth * numBuffers, bd[3][index].ByteWidth);
			devcon[context]->Unmap(pBuffer[3][index + numBuffers], NULL);
		} while (numBuffers);
	}

	inline ptr LockStructuredBeforeUpdate(cui8 context, csi16 index) {
		Try(stMap, devcon[context]->Map(pBuffer[3][index], NULL, D3D11_MAP_WRITE_NO_OVERWRITE, NULL, &ms));

		return ms.pData;
	}

	inline void UnlockStructuredAfterUpdate(cui8 context, csi16 index) const { devcon[context]->Unmap(pBuffer[3][index], NULL); }

	inline void SetVertex(cui8 context, si16 index, ui8 slot) const { devcon[context]->IASetVertexBuffers(slot, 1, &pBuffer[0][index], &uiStrides[0][index], &uiOffsets[0][index]); }

	inline void SetVertexGroup(cui8 context, si16 index, si16 count, ui8 firstSlot) const { devcon[context]->IASetVertexBuffers(firstSlot, count, &pBuffer[0][index], &uiStrides[0][index], &uiOffsets[0][index]); }

	inline void SetVertexStagger(cui8 context, ui32 stride, ui32 offset, si16 index, si16 count) {
		for(ui16 xx = 256; xx < count + 256; xx++) { uiStrides[0][xx] = stride; uiOffsets[0][xx] = offset; }
		devcon[context]->IASetVertexBuffers(index, count, &pBuffer[0][index], &uiStrides[0][256], &uiOffsets[0][256]);
	}

	inline void SetIndex(cui8 context, si16 index, ui8 slot) const { devcon[context]->IASetIndexBuffer(pBuffer[1][index], DXGI_FORMAT_R16_UINT, 0); }

	inline void SetVSR(cui8 context, ui16 index, ui16 slot, ui16 count) const { devcon[context]->VSSetShaderResources(slot, count, &pSRV[index]); }

	inline void SetGSR(cui8 context, ui16 index, ui16 slot, ui16 count) const { devcon[context]->GSSetShaderResources(slot, count, &pSRV[index]); }

	inline void SetPSR(cui8 context, ui16 index, ui16 slot, ui16 count) const { devcon[context]->PSSetShaderResources(slot, count, &pSRV[index]); }

	void DestroyAllVertex(ui16 bufCount) { do pBuffer[0][--bufferCount[0]]->Release(); while (bufferCount[0] && --bufCount); }

	void DestroyAllIndex(ui16 bufCount) { do pBuffer[1][--bufferCount[1]]->Release(); while (bufferCount[1] && --bufCount); }

	void DestroyAllConstant(ui16 bufCount) { do pBuffer[2][--bufferCount[2]]->Release(); while (bufferCount[2] && --bufCount); }
};
