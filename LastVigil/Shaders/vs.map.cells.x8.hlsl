/************************************************************
 * File: vs.map.cells.x8.hlsl           Created: 2023/04/29 *
 *                                Last modified: 2023/05/10 *
 *                                                          *
 * Desc:                                                    *
 *                                                          *
 *  Copyright (c) David William Bull. All rights reserved.  *
 ************************************************************/

#include "common.hlsli"

cbuffer MAPDIMS_ICB : register(b2) {
	cint3  iMapDim;    // X, Y, Z cell counts																	!!! Max dim of 1,024 === 66 bits wasted !!!
	cuint  mapDimsE;   // Exponenets of .iMapDim, 24-31==Low byte of .zDim spawning offset		!!! Max exp of 10 === 12 bits wasted !!!
	cint3  iChunkDim;  // X, Y, Z cell counts per chunk													!!! Max dim of 64 === 78 bits wasted !!!
	cuint  chunkDimsE; // Exponenets of .iChunkDim, 24-31==High byte of .zDim spawning offset	!!! Max exp of 6 === 15 bits wasted : Max offset of 1,024 === 6 bits wasted !!!
	cuint  chunkCount; //	X, Y, Z chunk counts - 1, 24-31==bitFlags									!!! Max counts of 128 === 3 bits wasted !!!
	cuint2 totalCells; // Cells per chunk, Cells per map													!!! Max sizes of 64x64x64 & 1,024x1,024x1,024 === 16 bits wasted !!!
	cuint  RES;        //																							!!! Total bits wasted: 178 (22 bytes + 5 bits) !!!
}

int4x11 main(in cuint chunk : INDEX, cuint _cell : SV_VertexID) : CELLS {
	cuint3 uiChunks    = ((chunkCount >> int3(0, 8, 16)) & 0x0FF) + 1;
	cuint3 uiChunksA   = uiChunks - 1;
	cuint3 uiCellsA    = iChunkDim - 1;
	cuint  uiChunkXY   = iChunkDim.x * iChunkDim.y;
	cint3  iChunkOS    = int3(chunk & uiChunksA.x, (chunk / uiChunks.x) & uiChunksA.y, (chunk / (uiChunks.x * uiChunks.y)) & uiChunksA.z);
	cuint  cell        = _cell << 3;
	cint   i1stEntry   = (chunk * (uiChunkXY * iChunkDim.z)) + cell - 2;
	cint3  iCellOS     = int3(cell & uiCellsA.x, (cell / iChunkDim.x) & uiCellsA.y, (cell / uiChunkXY) & uiCellsA.z);
	cuint2 uiChunkStep = uint2(totalCells.x * uiChunks.x, iChunkDim.x * uiCellsA.y);

	int4    rows = int4(iChunkDim.x << 1, iChunkDim.x, 0, -iChunkDim.x);
	int4x11 index;

	if(iChunkOS.y == 0) {
		switch(iCellOS.y) {
		case 0:
			rows.xy = 0;
			break;
		case 1:
			rows.x = iChunkDim.x;
			break;
		default:
			if(iCellOS.y >= uiCellsA.y)
				rows.w = uiChunkStep.y - uiChunkStep.x;
			break;
		}
	} else if(iChunkOS.y >= uiChunksA.y) {
		switch(iCellOS.y) {
		case 0:
			rows.xy += uiChunkStep.x - totalCells.x;
			break;
		case 1:
			rows.x += uiChunkStep.x - totalCells.x;
			break;
		default:
			if(iCellOS.y >= uiCellsA.y)
				rows.w = 0;
			break;
		}
	} else {
		switch(iCellOS.y) {
		case 0:
			rows.xy += uiChunkStep.x - totalCells.x;
			break;
		case 1:
			rows.x += uiChunkStep.x - totalCells.x;
			break;
		default:
			if(iCellOS.y >= uiCellsA.y)
				rows.w = uiChunkStep.y - uiChunkStep.x;
			break;
		}
	}

	[unroll]
	for(int i = 0; i < 11; i++)
		index[i] = i1stEntry.xxxx + i.xxxx - rows;

	if(iChunkOS.x == 0) {
		switch(iCellOS.x) {
		case 0:
			index[0] += 2;
			index[1] += 1;
			break;
		case 1:
			index[0] += 1;
			break;
		default:
			if(iCellOS.x >= uiCellsA.x - 7)
				index[10] += int(uiChunkStep.y);
			break;
		}
	} else {
		if(iChunkOS.x >= uiChunksA.x) {
			switch(iCellOS.x) {
			case 0:
				index[0] -= uiChunkStep.y;
				index[1] -= uiChunkStep.y;
				break;
			case 1:
				index[0] -= uiChunkStep.y;
				break;
			default:
				if(iCellOS.x >= uiCellsA.x - 7)
					index[10] -= 1;
				break;
			}
		} else {
			switch(iCellOS.x) {
			case 0:
				index[0] -= uiChunkStep.y;
				index[1] -= uiChunkStep.y;
				break;
			case 1:
				index[0] -= uiChunkStep.y;
				break;
			default:
				if(iCellOS.x >= uiCellsA.x - 7)
					index[10] += int(uiChunkStep.y);
				break;
			}
		}
	}

	return index;
}
