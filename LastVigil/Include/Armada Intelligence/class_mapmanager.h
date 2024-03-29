/************************************************************
 * File: class_mapmanager.h             Created: 2022/11/29 *
 *                                Last modified: 2023/05/27 *
 *                                                          *
 * Desc:                                                    *
 *                                                          *
 *  Copyright (c) David William Bull. All rights reserved.  *
 ************************************************************/
#pragma once

#include "pch.h"
#include "Data structures.h"
#include "Map structures.h"
#include "File operations.h"
#include "Common functions.h"

#define MAX_CELL_X 1024
#define MAX_CELL_Y 1024
#define MAX_CELL_Z 1024
#define MAX_CELLS  (MAX_CELL_X * MAX_CELL_Y * MAX_CELL_Z)

#define MAX_MAPS   2097152
#define MAX_WORLDS 512

#define MAX_TABLES   64
#define MAX_ELEMENTS (MAX_TABLES * 256)

typedef cMAPMAN_THREAD_DATA * const cMMTDptr;

vui128 MAPMAN_THREAD_STATUS = {};
static void _MM_Cull_Nonvisible_RasteriseLayer(cVEC4Ds32[2]);
static void _MM_Cull_Nonvisible_Rasterise(cVEC4Ds32[4]);
static void _MM_Cull_Nonvisible_and_Unchanged(ptr);
static void _MM_Cull_Nonvisible_Simple(ptr);
static void _MM_Cull_Nonvisible_Accurate(ptr);
static void _MM_Cull_Unchanged(ptr);

// Map manager
al32 struct CLASS_MAPMANAGER {
	CLASS_FILEOPS files;

	cwchar stMapsDir[10] = L"map_data\\";

	MAPMAN_THREAD_DATA *threadData = (MAPMAN_THREAD_DATA *)zalloc32(sizeof(MAPMAN_THREAD_DATA) * 2);

	WORLD      *world;
	ELEM_TABLE *table;
	ELEM_TYPE  *element;
	ELEM_IGS   *elem_igs;

	si32 totalWorlds;
	si32 elemTables;

	wchar      (*stPeriodicName)[MAX_TABLES];
	wchar      (*stElementName)[MAX_TABLES];
	LPDWORD      pBytes = (LPDWORD)&uiBytes;
	VEC2Ds32     worldXY;			// Current world cell
	VEC3Ds32     mapXYZ;				// Current map cell
	ui32         uiBytes;			// Bytes written
	ui16         uiMapBoundaries;	// Map edge flags: (Per bit... 0:Finite boundaries, 1:Wrap coordinates) 0-4==X axis, 5-9==Y axis, 10-14==Z axis
	ui8          uiPeriodicName = 0, uiElementName = 0;

	CLASS_MAPMANAGER(void) {
#ifdef AE_PTR_LIB
		ptrLib[2] = this;
#endif
		world    = (WORLD *)zalloc32(sizeof(WORLD[MAX_WORLDS]));
		table    = (ELEM_TABLE *)zalloc32(sizeof(ELEM_TABLE[MAX_TABLES]));
		element  = (ELEM_TYPE *)zalloc16(sizeof(ELEM_TYPE[MAX_ELEMENTS]));
		elem_igs = (ELEM_IGS *)zalloc32(sizeof(ELEM_IGS[MAX_ELEMENTS]));
}

	inline MAP_PTRS const Pointers(csi16 worldIndex, csi16 mapIndex) const {
		return { NULL, world[worldIndex].map[mapIndex]->pDGS, world[worldIndex].map[mapIndex]->pDPS, world[worldIndex].map[mapIndex]->pCB };
	}

	cui32 CreatePeriodicTable(wchptrc name, csi32 maxElements, si32 tableIndex) {
		ui8 i = 0;

		// Find first available slot if index is -1
		if(tableIndex == -1) {
			for(; table[i].element && i < MAX_TABLES; i++);
			if(i >= MAX_TABLES) return 0x080000001;	// All periodic table slots occupied
			tableIndex = i;
		}

		csi32 offset = tableIndex << 8;

		table[tableIndex] = { name, &element[offset], &elem_igs[offset], maxElements };

//		for(i = 0; i < maxElements; i++)
//			table[tableIndex].element[i].geometry = &elem_igs[offset + i];

		return tableIndex;
	}

	inline cui32 InsertElement(csi32 tableIndex, csi32 elementIndex) {
	}

	inline cui32 DeleteElement(csi32 tableIndex, csi32 elementIndex) {
	}

	inline void SetElement(csi32 tableIndex, csi32 elementIndex, ELEM_TYPE &elementData) const { table[tableIndex].element[elementIndex] = elementData; }

	inline void SetElementName(csi32 tableIndex, csi32 elementIndex, wchptrc name) const { table[tableIndex].element[elementIndex].stName = name; }

	inline void SetElementTemps(csi32 tableIndex, csi32 elementIndex, cfl32 meltingPoint, cfl32 boilingPoint, cfl32 ignitionPoint, cfl32 propagation, cfl32 fusion) const {
		table[tableIndex].element[elementIndex].tmp = meltingPoint;
		table[tableIndex].element[elementIndex].tbp = boilingPoint;
		table[tableIndex].element[elementIndex].tip = ignitionPoint;
		table[tableIndex].element[elementIndex].tp  = propagation;
		table[tableIndex].element[elementIndex].eft = fusion;
	}

	inline void SetElementMiscVars(csi32 tableIndex, csi32 elementIndex, cfl32 decayRate, cVEC2Df elecResist, cVEC2Df atomDensity, cfl32 acidity) const {
		table[tableIndex].element[elementIndex].edr  = decayRate;
		table[tableIndex].element[elementIndex].er   = Fix16x2(elecResist, 32768.0f);
		table[tableIndex].element[elementIndex].ad   = Fix16x2(atomDensity, 1024.0f);
		table[tableIndex].element[elementIndex].acid = Fix16(acidity, 1024.0f);
	}

	inline void SetElementResults(csi32 tableIndex, csi32 elementIndex, cui8 decayResult, cui8 fusionResult) const {
		table[tableIndex].element[elementIndex].ede = decayResult;
		table[tableIndex].element[elementIndex].efe = fusionResult;
	}

	inline void SetElementGeometry(csi32 tableIndex, csi32 elementIndex, cVEC4Df texCoords, cfl32 tcScalar, cfl32 transparency, cfl32 animFrameTime, cui8 animFrameOS, cui8 animFrameCount, cui8 atlasIndex) {
		table[tableIndex].pIGS[elementIndex].tc  = Fix16x4(texCoords, 32768.0f);
		table[tableIndex].pIGS[elementIndex].aft = Fix16p8(animFrameTime);
		table[tableIndex].pIGS[elementIndex].et  = Fix8(transparency, 255.0f);
		table[tableIndex].pIGS[elementIndex].afo = animFrameOS;
		table[tableIndex].pIGS[elementIndex].afc = animFrameCount - 1;
		table[tableIndex].pIGS[elementIndex].tcs = Fix16(tcScalar, 16.0f) - 1;
		table[tableIndex].pIGS[elementIndex].ai  = atlasIndex;
	}

	cui32 LoadPeriodicTable(wchptrc filename, si32 index) {
		si32 i = 0;
		// Find first available slot if index is -1
		if(index == -1) {
			for(; table[i].element && i < MAX_TABLES; i++);
			if(i >= MAX_TABLES) return 0x080000001;	// All periodic table slots occupied
			index = i;
		}

		wcscpy(files.stTemp, stMapsDir);
		wcscpy(files.stTemp + wcslen(stMapsDir), filename);
		HANDLE hElementData = CreateFile(files.stTemp, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);

		// Read & process tagline
		files.ReadLine(files.stTemp, hElementData);
//--- To do...
		// Read primary data
		files.ReadLine(stPeriodicName[uiPeriodicName], hElementData);
		table[index].stName = stPeriodicName[uiPeriodicName++];
		ReadFile(hElementData, &table[index].numElements, sizeof(ui32), pBytes, NULL);
		// Read element data
		table[index].element = (ELEM_TYPE *)malloc32(sizeof(ELEM_TYPE) * table[index].numElements);
		for(i = 0; i < table[index].numElements; i++) {
			ReadFile(hElementData, stElementName[uiElementName], DWORD(wcslen(table[index].element[i].stName) + 1) * sizeof(wchar), pBytes, NULL);
			table[index].element[i].stName = stElementName[uiElementName++];
			ReadFile(hElementData, &table[index].element[i].tmp, sizeof(float), pBytes, NULL);
			ReadFile(hElementData, &table[index].element[i].tbp, sizeof(float), pBytes, NULL);
			ReadFile(hElementData, &table[index].element[i].tip, sizeof(float), pBytes, NULL);
			ReadFile(hElementData, &table[index].element[i].tp, sizeof(float), pBytes, NULL);
			ReadFile(hElementData, &table[index].element[i].eft, sizeof(float), pBytes, NULL);
			ReadFile(hElementData, &table[index].element[i].edr, sizeof(float), pBytes, NULL);
			ReadFile(hElementData, &table[index].element[i].er, sizeof(VEC2Du16), pBytes, NULL);
			ReadFile(hElementData, &table[index].element[i].ad, sizeof(VEC2Du16), pBytes, NULL);
			ReadFile(hElementData, &table[index].element[i].acid, sizeof(ui16), pBytes, NULL);
			ReadFile(hElementData, &table[index].element[i].ede, sizeof(ui8), pBytes, NULL);
			ReadFile(hElementData, &table[index].element[i].efe, sizeof(ui8), pBytes, NULL);
		}
		table[index].pIGS = (ELEM_IGS *)malloc32(sizeof(ELEM_IGS) * table[index].numElements);
		ReadFile(hElementData, table[index].pIGS, sizeof(ELEM_IGS) * table[index].numElements, pBytes, NULL);

		return index;
	}

	cui32 SavePeriodicTable(wchptrc filename, csi32 index) {
		// Periodic table slot is empty
		if(!table[index].stName) return 0x080000001;

		wcscpy(files.stTemp, stMapsDir);
		wcscpy(files.stTemp + wcslen(stMapsDir), filename);
		HANDLE hElementData = CreateFile(files.stTemp, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

		// Write tag line: 2[Engine].Elements.2[Format version]1[Compression method]
		WriteFile(hElementData, "AE.Elements.01u\0", 16, pBytes, NULL);
		// Write primary data
		WriteFile(hElementData, table[index].stName, DWORD(wcslen(table[index].stName) + 1) * sizeof(wchar), pBytes, NULL);
		WriteFile(hElementData, &table[index].numElements, sizeof(ui32), pBytes, NULL);
		// Write element data
		for(si32 i = 0; i < table[index].numElements; i++) {
			WriteFile(hElementData, table[index].element[i].stName, DWORD(wcslen(table[index].element[i].stName) + 1) * sizeof(wchar), pBytes, NULL);
			WriteFile(hElementData, &table[index].element[i].tmp, sizeof(float), pBytes, NULL);
			WriteFile(hElementData, &table[index].element[i].tbp, sizeof(float), pBytes, NULL);
			WriteFile(hElementData, &table[index].element[i].tip, sizeof(float), pBytes, NULL);
			WriteFile(hElementData, &table[index].element[i].tp, sizeof(float), pBytes, NULL);
			WriteFile(hElementData, &table[index].element[i].eft, sizeof(float), pBytes, NULL);
			WriteFile(hElementData, &table[index].element[i].edr, sizeof(float), pBytes, NULL);
			WriteFile(hElementData, &table[index].element[i].er, sizeof(VEC2Du16), pBytes, NULL);
			WriteFile(hElementData, &table[index].element[i].ad, sizeof(VEC2Du16), pBytes, NULL);
			WriteFile(hElementData, &table[index].element[i].acid, sizeof(ui16), pBytes, NULL);
			WriteFile(hElementData, &table[index].element[i].ede, sizeof(ui8), pBytes, NULL);
			WriteFile(hElementData, &table[index].element[i].efe, sizeof(ui8), pBytes, NULL);
		}
		WriteFile(hElementData, table[index].pIGS, sizeof(ELEM_IGS) * table[index].numElements, pBytes, NULL);

		CloseHandle(hElementData);

		return 0;
	}

	void DestroyPeriodicTable(cui8 index) const { memset(&table[index], 0x0, sizeof(ELEM_TABLE)); }

	cui32 CreateWorld(csi32 worldIndex, csi32 periodicTableIndex, csi32 maxMaps) {
		if(world[worldIndex].map) return 0x080000001;	// world already exists

		WORLD &curWorld = world[worldIndex];

		curWorld.maxMaps  = max(maxMaps, MAX_MAPS);
		curWorld.map      = (MAP **)zalloc32(sizeof(MAP *) * maxMaps);
		curWorld.mapVis   = (ui64ptr)zalloc32(sizeof(ui64) * ((maxMaps + 7) >> 3));
		curWorld.mapMod   = (ui64ptr)zalloc32(sizeof(ui64) * ((maxMaps + 7) >> 3));
		curWorld.perTable = &table[periodicTableIndex];

		return ++totalWorlds;
	}

	cui32 ResizeWorld(csi32 worldIndex, csi32 maxMaps) {
//--- To do...
		if(maxMaps > world[worldIndex].maxMaps) return 0x080000001;

		return maxMaps;
	}

	cui32 DestroyWorld(csi32 worldIndex) {
		for(ui32 i = world[worldIndex].totalMaps - 1; i; i--) 
			DestroyMap(worldIndex, i);
		_aligned_free(world[worldIndex].mapMod);
		_aligned_free(world[worldIndex].mapVis);
		_aligned_free(world[worldIndex].map);
		memset(&world[worldIndex], 0x0, sizeof(WORLD));

		return --totalWorlds;
	}


	cui32 LoadMap(wchptrc filename, csi32 worldIndex, si32 mapIndex) {
		si32 i = 0;
		// Find first available slot if mapIndex is -1
		if(mapIndex == -1)
			for(mapIndex = 0; mapIndex < world[worldIndex].maxMaps && world[worldIndex].map[mapIndex]; mapIndex++);
		// Map slot already occupied, or all slots occupied
		if(mapIndex >= world[worldIndex].maxMaps) return 0x080000001;

		MAP &curMap = *world[worldIndex].map[mapIndex];

		wcscpy(files.stTemp, stMapsDir);
		wcscpy(files.stTemp + wcslen(stMapsDir), filename);
		HANDLE hMapData = CreateFile(files.stTemp, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);

		// Read & process tagline
		files.ReadLine(files.stTemp, hMapData);
//--- To do...
		// Read critical map information
		files.ReadLine(files.stTemp, hMapData);
		curMap.desc.stName = (wchptr)malloc32(wcslen(files.stTemp) + 1);
		wcscpy(curMap.desc.stName, files.stTemp);
		files.ReadLine(files.stTemp, hMapData);
		curMap.desc.stInfo = (wchptr)malloc32(wcslen(files.stTemp) + 1);
		wcscpy(curMap.desc.stInfo, files.stTemp);
		// Search periodic table array for match
		files.ReadLine(files.stTemp, hMapData);
		for(i = 0; !wcscmp(files.stTemp, table[i].stName) || i < MAX_TABLES; i++);
		if(i < MAX_TABLES)
			curMap.desc.ptIndex = i;
		// Not present; attempt to load
		else {
			for(i = 0; table[i].stName || i < MAX_TABLES; i++);
			if(i >= MAX_TABLES)	// No free slots available
				return 0x080000002;
			LoadPeriodicTable(files.stTemp, i);
			curMap.desc.ptIndex = i;
		}

		ReadFile(hMapData, &curMap.desc.mapDim, sizeof(VEC3Ds16), pBytes, NULL);
		ReadFile(hMapData, &curMap.desc.chunkDim, sizeof(VEC3Ds16), pBytes, NULL);
		ReadFile(hMapData, &curMap.desc.zso, sizeof(si16), pBytes, NULL);
		ReadFile(hMapData, &curMap.oob.vel, sizeof(VEC2Df), pBytes, NULL);
		ReadFile(hMapData, &curMap.oob.temp, sizeof(float), pBytes, NULL);
		ReadFile(hMapData, &curMap.oob.rad, sizeof(float), pBytes, NULL);
		ReadFile(hMapData, &curMap.oob.elec, sizeof(float), pBytes, NULL);
		curMap.oob.geometry	= NULL;
		curMap.oob.pixel		= NULL;
		// Calculate cached values
		csi32 chunkCells  = curMap.desc.chunkDim.x * curMap.desc.chunkDim.y * curMap.desc.chunkDim.z;
		csi32 totalCells  = curMap.desc.mapDim.x * curMap.desc.mapDim.y * curMap.desc.mapDim.z;
		cui32 totalChunks = totalCells / chunkCells;
		
		cVEC3Du8 chunkCount = { ui8(curMap.desc.mapDim.x / curMap.desc.chunkDim.x),
										ui8(curMap.desc.mapDim.y / curMap.desc.chunkDim.y),
										ui8(curMap.desc.mapDim.z / curMap.desc.chunkDim.z) };

		curMap.pCB = (MAPDIMS_ICB *)malloc16(sizeof(MAPDIMS_ICB));
		curMap.desc.chunkCells      = chunkCells;
		curMap.desc.mapCells        = totalCells;
		curMap.pCB->mapDims         = { curMap.desc.mapDim.x, curMap.desc.mapDim.y, curMap.desc.mapDim.z };
		curMap.pCB->chunkDims       = { curMap.desc.chunkDim.x, curMap.desc.chunkDim.y, curMap.desc.chunkDim.z };
		curMap.pCB->totalCells      = { ui32(chunkCells), ui32(totalCells) };
		curMap.pCB->zsoL            = ui8(curMap.desc.zso & 0x0FF);
		curMap.pCB->zsoH            = ui8(curMap.desc.zso >> 8);
		curMap.pCB->chunkCount      = chunkCount;
		curMap.pCB->bitFlags        = 0;
		curMap.desc.chunkCellsE     = 31 - __lzcnt(chunkCells);
		curMap.desc.chunkRowCellE   = curMap.desc.chunkCellsE + 31 - chunkCount.x;
		curMap.desc.chunkPlaneCellE = curMap.desc.chunkRowCellE + 31 - chunkCount.y;
		curMap.pCB->mapDimsE        = { ui8(31 - __lzcnt(curMap.desc.mapDim.x)), ui8(31 - __lzcnt(curMap.desc.mapDim.x)), ui8(31 - __lzcnt(curMap.desc.mapDim.x)) };
		curMap.pCB->chunkDimsE      = { ui8(31 - __lzcnt(curMap.desc.chunkDim.x)), ui8(31 - __lzcnt(curMap.desc.chunkDim.y)), ui8(31 - __lzcnt(curMap.desc.chunkDim.z)) };
		// Read cell data
		curMap.cell = (CELL *)malloc32(sizeof(CELL) * curMap.desc.mapCells);
		for(si32 i = 0; i < curMap.desc.mapCells; i++) {
			ReadFile(hMapData, &curMap.cell[i].vel, sizeof(VEC2Df), pBytes, NULL);
			ReadFile(hMapData, &curMap.cell[i].temp, sizeof(float), pBytes, NULL);
			ReadFile(hMapData, &curMap.cell[i].rad, sizeof(float), pBytes, NULL);
			ReadFile(hMapData, &curMap.cell[i].elec, sizeof(float), pBytes, NULL);
		}
		curMap.pDGS = (CELL_DGS *)malloc32(sizeof(CELL_DGS) * curMap.desc.mapCells);
		ReadFile(hMapData, curMap.pDGS, sizeof(CELL_DGS) * curMap.desc.mapCells, pBytes, NULL);
		curMap.pDPS = (CELL_DPS *)malloc32(sizeof(CELL_DPS) * curMap.desc.mapCells);
		ReadFile(hMapData, curMap.pDPS, sizeof(CELL_DPS) * curMap.desc.mapCells, pBytes, NULL);
		curMap.chunkVis = (ui64 *)malloc16(ui64(totalChunks + 7) >> 3);
		curMap.chunkMod = (ui64 *)malloc16(ui64(totalChunks + 7) >> 3);

		return ++world[worldIndex].totalMaps;
	}

	cui32 SaveMap(wchptrc filename, csi32 worldIndex, csi32 mapIndex) {
		// Map slot is empty
		if(!world[worldIndex].map[mapIndex]) return 0x080000001;

		MAP &curMap = *world[worldIndex].map[mapIndex];

		wcscpy(files.stTemp, stMapsDir);
		wcscpy(files.stTemp + wcslen(stMapsDir), filename);
		HANDLE hMapData = CreateFile(files.stTemp, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

		// Write tag line: 2[Engine].4[Frontend].2[Data type].3[Format version]1[Compression method]
		WriteFile(hMapData, "AE.LV01.MD.001u\0", 16, pBytes, NULL);
		// Write critical map information
		WriteFile(hMapData, curMap.desc.stName, DWORD(wcslen(curMap.desc.stName) + 1) * sizeof(wchar), pBytes, NULL);
		WriteFile(hMapData, curMap.desc.stInfo, DWORD(wcslen(curMap.desc.stInfo) + 1) * sizeof(wchar), pBytes, NULL);
		WriteFile(hMapData, table[curMap.desc.ptIndex].stName, DWORD(wcslen(table[curMap.desc.ptIndex].stName) + 1) * sizeof(wchar), pBytes, NULL);
		WriteFile(hMapData, &curMap.desc.mapDim, sizeof(VEC3Ds16), pBytes, NULL);
		WriteFile(hMapData, &curMap.desc.chunkDim, sizeof(VEC3Ds16), pBytes, NULL);
		WriteFile(hMapData, &curMap.desc.zso, sizeof(si16), pBytes, NULL);
		WriteFile(hMapData, &curMap.oob.vel, sizeof(VEC2Df), pBytes, NULL);
		WriteFile(hMapData, &curMap.oob.temp, sizeof(float), pBytes, NULL);
		WriteFile(hMapData, &curMap.oob.rad, sizeof(float), pBytes, NULL);
		WriteFile(hMapData, &curMap.oob.elec, sizeof(float), pBytes, NULL);
		// Write cell data
		for(si32 i = 0; i < curMap.desc.mapCells; i++) {
			WriteFile(hMapData, &curMap.cell[i].vel, sizeof(VEC2Df), pBytes, NULL);
			WriteFile(hMapData, &curMap.cell[i].temp, sizeof(float), pBytes, NULL);
			WriteFile(hMapData, &curMap.cell[i].rad, sizeof(float), pBytes, NULL);
			WriteFile(hMapData, &curMap.cell[i].elec, sizeof(float), pBytes, NULL);
		}
		WriteFile(hMapData, curMap.pDGS, sizeof(CELL_DGS) * curMap.desc.mapCells, pBytes, NULL);
		WriteFile(hMapData, curMap.pDPS, sizeof(CELL_DPS) * curMap.desc.mapCells, pBytes, NULL);

		CloseHandle(hMapData);

		return 0;
	}

	cui32 CreateMap(MAP_DESC &md, csi32 worldIndex, si32 mapIndex, cui8 openElement, cui8 solidElement) {	// Currently only compatible with chunk depth of 1
		si32 i = 0;
		// Find first available slot if mapIndex is -1
		if(mapIndex == -1)
			for(mapIndex = 0; mapIndex < world[worldIndex].maxMaps && world[worldIndex].map[mapIndex]; mapIndex++);
		// Map slot already occupied, or all slots occupied
		if(mapIndex >= world[worldIndex].maxMaps) return 0x080000001;

		cui32     yStep       = md.mapDim.y;
		cui32     zStep       = md.mapDim.x * yStep;
		csi32     totalCells  = zStep * md.mapDim.z;
		csi32     chunkCells  = md.chunkDim.x * md.chunkDim.y * md.chunkDim.z;
		csi32     totalChunks = totalCells / chunkCells;
		cVEC3Ds16 chunkCount  = { si16(md.mapDim.x / md.chunkDim.x), si16(md.mapDim.y / md.chunkDim.y), si16(md.mapDim.z / md.chunkDim.z) };
		csi32     chunkChOS   = chunkCount.x * chunkCount.y;
		csi32     surfaceChOS = md.zso * chunkChOS;
		csi32     solidChOS   = chunkChOS + surfaceChOS;
///- Enable after adding element table management
		//cui8     atlasIndex  = table[elementTable].element[openElement].geometry->ai;
		cui8      atlasIndex  = 0;

		md.totals     = { chunkCells, totalCells, totalChunks };
		md.chunkCount = chunkCount;

		MAP &curMap = *(world[worldIndex].map[mapIndex] = (MAP *)malloc32(sizeof(MAP)));

		curMap.pCB      = (MAPDIMS_ICB *)malloc16(sizeof(MAPDIMS_ICB));
		curMap.pDGS     = (CELL_DGS *)malloc32(sizeof(CELL_DGS) * totalCells);
		curMap.pDPS     = (CELL_DPS *)malloc32(sizeof(CELL_DPS) * totalCells);
		curMap.cell     = (CELL *)malloc32(sizeof(CELL) * totalCells);
		curMap.chunkVis = (ui64 *)malloc16(ui64(totalChunks + 7) >> 3);
		curMap.chunkMod = (ui64 *)malloc16(ui64(totalChunks + 7) >> 3);
		curMap.oob.geometry = NULL;
		curMap.oob.pixel    = NULL;
		curMap.oob.vel      = { 0.0f, 0.0f };
		curMap.oob.temp     = -1.0f;
		curMap.oob.rad      = -1.0f;
		curMap.oob.elec     = -1.0f;
		curMap.desc.stName     = md.stName;
		curMap.desc.stInfo     = md.stInfo;
		curMap.desc.chunkCells = chunkCells;
		curMap.desc.mapCells   = totalCells;
		curMap.desc.mapChunks  = totalChunks;
		curMap.desc.zso        = md.zso;
		curMap.desc.ptIndex    = md.ptIndex;
		curMap.desc.mapDim     = md.mapDim;
		curMap.desc.chunkDim   = md.chunkDim;
		curMap.desc.chunkCount = chunkCount;
		curMap.pCB->mapDims     = { md.mapDim.x, md.mapDim.y, md.mapDim.z };
		curMap.pCB->chunkDims   = { md.chunkDim.x, md.chunkDim.y, md.chunkDim.z };
		curMap.pCB->zsoL        = ui8(md.zso & 0x0FF);
		curMap.pCB->zsoH        = ui8(md.zso >> 8);
		curMap.pCB->totalCells  = { ui32(chunkCells), ui32(totalCells) };
		curMap.pCB->totalChunks = totalChunks;
		curMap.pCB->chunkCount  = { ui8(chunkCount.x - 1), ui8(chunkCount.y - 1), ui8(chunkCount.z - 1) };
		curMap.pCB->bitFlags    = 0;

		curMap.desc.chunkCellsE		= 31 - __lzcnt(chunkCells);
		curMap.desc.chunkRowCellE		= curMap.desc.chunkCellsE + 31 - chunkCount.x;
		curMap.desc.chunkPlaneCellE	= curMap.desc.chunkRowCellE + 31 - chunkCount.y;
		curMap.pCB->mapDimsE			= { ui8(31 - __lzcnt(md.mapDim.x)), ui8(31 - __lzcnt(md.mapDim.x)), ui8(31 - __lzcnt(md.mapDim.x)) };
		curMap.pCB->chunkDimsE			= { ui8(31 - __lzcnt(md.chunkDim.x)), ui8(31 - __lzcnt(md.chunkDim.y)), ui8(31 - __lzcnt(md.chunkDim.z)) };
		md.chunkCellsE     = curMap.desc.chunkCellsE;
		md.chunkRowCellE   = curMap.desc.chunkRowCellE;
		md.chunkPlaneCellE = curMap.desc.chunkPlaneCellE;

		si32 chunkIndex = 0;
		$LoopMT4 for(; chunkIndex < surfaceChOS; chunkIndex++) {
			cui64 bitOS   = ui64(0x01) << (chunkIndex & 0x03F);
			cui32 QWordOS = chunkIndex >> 6;
			curMap.chunkVis[QWordOS] |= bitOS;

			for(si32 i = 0; i < chunkCells; i++) {
				csi32  cellIndex = chunkIndex * chunkCells + i;
				CELL  &curCell   = world[worldIndex].map[mapIndex]->cell[cellIndex];

				curCell.geometry       = &curMap.pDGS[cellIndex];
				curCell.geometry->et   = { openElement, 0, 0, 0 };
				curCell.geometry->er   = { 255, 0, 0, 0 };
				curCell.geometry->end  = { 0, 0, 0, 0 };
				curCell.geometry->dens = 0;
				curCell.geometry->warp = { 0, 0 };
				curCell.pixel          = &curMap.pDPS[cellIndex];
				curCell.pixel->pmc     = Fix8x4({ 1.0f, 1.0f, 1.0f, 1.0f });
				curCell.pixel->gtc     = Fix8x4({ 1.0f, 1.0f, 1.0f, 1.0f });
				curCell.pixel->gev     = Fix16(0.0f, -127.0f, 255.0f);
				curCell.pixel->ems     = Fix16(1.0f, 514.0f);
				curCell.pixel->nms     = Fix8(1.0f);
				curCell.pixel->rms     = Fix8(1.0f);
				curCell.pixel->pms     = Fix8(1.0f);
				curCell.pixel->ai      = atlasIndex;
				curCell.vel            = { 0.0f, 0.0f };
				curCell.temp           = 294.15f;
				curCell.rad            = 0.0f;
				curCell.elec           = 0.0f;
			}
		}
		$LoopMT4 for(; chunkIndex < solidChOS; chunkIndex++) {
			cui64 bitOS   = ui64(0x01) << (chunkIndex & 0x03F);
			cui32 QWordOS = chunkIndex >> 6;
			curMap.chunkVis[QWordOS] |= bitOS;

			for(si32 i = 0; i < chunkCells; i++) {
				csi32 cellIndex  = chunkIndex * chunkCells + i;
				CELL  &curCell   = world[worldIndex].map[mapIndex]->cell[cellIndex];

				curCell.geometry       = &curMap.pDGS[cellIndex];
				curCell.geometry->et   = { solidElement, 0, 0, 0 };
				curCell.geometry->er   = { 255, 0, 0, 0 };
				curCell.geometry->end  = { 0, 0, 0, 0 };
				curCell.geometry->dens = 0x0FFFE;
				curCell.geometry->warp = { 0, 0 };
				curCell.pixel          = &curMap.pDPS[cellIndex];
				curCell.pixel->pmc     = Fix8x4({ 1.0f, 1.0f, 1.0f, 1.0f });
				curCell.pixel->gtc     = Fix8x4({ 1.0f, 1.0f, 1.0f, 1.0f });
				curCell.pixel->gev     = Fix16(0.0f, -127.0f, 255.0f);
				curCell.pixel->ems     = Fix16(1.0f, 514.0f);
				curCell.pixel->nms     = Fix8(1.0f);
				curCell.pixel->rms     = Fix8(1.0f);
				curCell.pixel->pms     = Fix8(1.0f);
				curCell.pixel->ai      = atlasIndex;
				curCell.vel            = { 0.0f, 0.0f };
				curCell.temp           = 294.15f;
				curCell.rad            = 0.0f;
				curCell.elec           = 0.0f;
			}
		}
		$LoopMT4 for(; chunkIndex < totalChunks; chunkIndex++) {
			cui64 bitOS   = ui64(0x01) << (chunkIndex & 0x03F);
			cui32 QWordOS = chunkIndex >> 6;
			curMap.chunkVis[QWordOS] |= bitOS;

			for(si32 i = 0; i < chunkCells; i++) {
				csi32 cellIndex  = chunkIndex * chunkCells + i;
				CELL  &curCell   = world[worldIndex].map[mapIndex]->cell[cellIndex];

				curCell.geometry       = &curMap.pDGS[cellIndex];
				curCell.geometry->et   = { solidElement, 0, 0, 0 };
				curCell.geometry->er   = { 255, 0, 0, 0 };
				curCell.geometry->end  = { 0, 0, 0, 0 };
				curCell.geometry->dens = 0x0FFFF;
				curCell.geometry->warp = { 0, 0 };
				curCell.pixel          = &curMap.pDPS[cellIndex];
				curCell.pixel->pmc     = Fix8x4({ 1.0f, 1.0f, 1.0f, 1.0f });
				curCell.pixel->gtc     = Fix8x4({ 1.0f, 1.0f, 1.0f, 1.0f });
				curCell.pixel->gev     = Fix16(0.0f, -127.0f, 255.0f);
				curCell.pixel->ems     = Fix16(1.0f, 514.0f);
				curCell.pixel->nms     = Fix8(1.0f);
				curCell.pixel->rms     = Fix8(1.0f);
				curCell.pixel->pms     = Fix8(1.0f);
				curCell.pixel->ai      = atlasIndex;
				curCell.vel            = { 0.0f, 0.0f };
				curCell.temp           = 294.15f;
				curCell.rad            = 0.0f;
				curCell.elec           = 0.0f;
			}
		}
		return ++world[worldIndex].totalMaps;
	}

	csi32 DestroyMap(si32 worldIndex, si32 mapIndex) {
		// Map slot is empty
		if(!world[worldIndex].map[mapIndex]) return -1;

		_aligned_free(world[worldIndex].map[mapIndex]->chunkMod);
		_aligned_free(world[worldIndex].map[mapIndex]->chunkVis);
		_aligned_free(world[worldIndex].map[mapIndex]->pDPS);
		_aligned_free(world[worldIndex].map[mapIndex]->pDGS);
		_aligned_free(world[worldIndex].map[mapIndex]->cell);
		_aligned_free(world[worldIndex].map[mapIndex]->pCB);
		_aligned_free(world[worldIndex].map[mapIndex]->desc.stInfo);
		_aligned_free(world[worldIndex].map[mapIndex]->desc.stName);
		_aligned_free(world[worldIndex].map[mapIndex]);
		world[worldIndex].map[mapIndex] = 0;

		return --world[worldIndex].totalMaps;
	}

	inline csi32 CalcCellIndex(cVEC3Ds32 coord, csi32 worldIndex, csi32 mapIndex) {
		cMAP_DESC &curDesc = world[worldIndex].map[mapIndex]->desc;
		cVEC3Ds32  vCell   = { coord.x + (curDesc.mapDim.x >> 1), coord.y + (curDesc.mapDim.y >> 1), coord.z + curDesc.zso };

		if(vCell.x < 0 || vCell.x >= curDesc.mapDim.x) return 0x080000001;
		if(vCell.y < 0 || vCell.y >= curDesc.mapDim.y) return 0x080000001;
		if(vCell.z < 0 || vCell.z >= curDesc.mapDim.z) return 0x080000001;

		cVEC3Du16 vChunkDim         = (VEC3Du16 &)curDesc.chunkDim;
		cui32     uiChunkCells_XY   = vChunkDim.x * vChunkDim.y;
		cui32     uiChunkCells      = curDesc.chunkCells;
		cui32     uiChunkRowCells   = uiChunkCells * (curDesc.chunkCount.x);
		cui32     uiChunkPlaneCells = uiChunkRowCells * (curDesc.chunkCount.y);

		cui32 uiCellX = (vCell.x & (vChunkDim.x - 1)) + ((vCell.x / vChunkDim.x) * uiChunkCells);
		cui32 uiCellY = ((vCell.y & (vChunkDim.y - 1)) * vChunkDim.x) + ((vCell.y / vChunkDim.y) * uiChunkRowCells);
		cui32 uiCellZ = ((vCell.z & (vChunkDim.z - 1)) * uiChunkCells_XY) + ((vCell.z / vChunkDim.z) * uiChunkPlaneCells);

		cui32 uiCell = uiCellX + uiCellY + uiCellZ;

		return (uiCell < world[worldIndex].map[mapIndex]->pCB->totalCells.y ? uiCell : 0x080000001);
	}

	inline csi32 CalcCellIndex_(cVEC3Ds32 coord, csi32 worldIndex, csi32 mapIndex) {
		cMAP_DESC &curDesc = world[worldIndex].map[mapIndex]->desc;

		if(coord.x < 0 || coord.x >= curDesc.mapDim.x) return 0x80000001;
		if(coord.y < 0 || coord.y >= curDesc.mapDim.y) return 0x80000001;
		if(coord.z < 0 || coord.z >= curDesc.mapDim.z) return 0x80000001;

		cVEC3Du16 vChunkDim         = (VEC3Du16 &)curDesc.chunkDim;
		cui32     uiChunkCells_XY   = vChunkDim.x * vChunkDim.y;
		cui32     uiChunkCells      = curDesc.chunkCells;
		cui32     uiChunkRowCells   = uiChunkCells * (curDesc.chunkCount.x);
		cui32     uiChunkPlaneCells = uiChunkRowCells * (curDesc.chunkCount.y);

		cui32 uiCellX = (coord.x & (vChunkDim.x - 1)) + ((coord.x / vChunkDim.x) * uiChunkCells);
		cui32 uiCellY = ((coord.y & (vChunkDim.y - 1)) * vChunkDim.x) + ((coord.y / vChunkDim.y) * uiChunkRowCells);
		cui32 uiCellZ = ((coord.z & (vChunkDim.z - 1)) * uiChunkCells_XY) + ((coord.z / vChunkDim.z) * uiChunkPlaneCells);

		cui32 uiCell = uiCellX + uiCellY + uiCellZ;

		return (uiCell < world[worldIndex].map[mapIndex]->pCB->totalCells.y ? uiCell : 0x080000001);
	}

	inline csi32 CalcChunkIndex(cVEC3Ds32 coord, csi32 worldIndex, csi32 mapIndex) {
		cVEC3Ds16 &curCount = world[worldIndex].map[mapIndex]->desc.chunkCount;
		cVEC3Ds32  chunk    = { coord.x + (curCount.x >> 1), coord.y + (curCount.y >> 1), coord.z + (curCount.z >> 1) };

		if(chunk.x < 0 || chunk.x >= curCount.x) return 0x080000001;
		if(chunk.y < 0 || chunk.y >= curCount.y) return 0x080000001;
		if(chunk.z < 0 || chunk.z >= curCount.z) return 0x080000001;

		csi32 siChunks[3] = { chunk.x, chunk.y * (curCount.x), chunk.z * (curCount.x * curCount.y) };

		csi32 siChunk = siChunks[0] + siChunks[1] + siChunks[2];

		return (siChunk < si32(world[worldIndex].map[mapIndex]->pCB->totalChunks) ? siChunk : 0x080000001);
	}


	inline csi32 CalcChunkIndex_(cVEC3Ds32 coord, csi32 worldIndex, csi32 mapIndex) {
		cVEC3Ds16 &curCount = world[worldIndex].map[mapIndex]->desc.chunkCount;

		if(coord.x < 0 || coord.x >= curCount.x) return 0x080000001;
		if(coord.y < 0 || coord.y >= curCount.y) return 0x080000001;
		if(coord.z < 0 || coord.z >= curCount.z) return 0x080000001;

		csi32 siChunks[3] = { coord.x, coord.y * (curCount.x), coord.z * (curCount.x * curCount.y) };

		csi32 siChunk = siChunks[0] + siChunks[1] + siChunks[2];

		return (siChunk < si32(world[worldIndex].map[mapIndex]->pCB->totalChunks) ? siChunk : 0x080000001);
	}

	inline cbool CalcQuadCellIndices(VEC4Ds32 &results, cVEC3Ds32 *coord, csi32 worldIndex, csi32 mapIndex) {
		cMAP_DESC &curDesc = world[worldIndex].map[mapIndex]->desc;

		cVEC3Ds32 vCell[4] = { coord[0].x + (curDesc.mapDim.x >> 1), coord[0].y + (curDesc.mapDim.y >> 1), coord[0].z + (curDesc.zso),
									  coord[1].x + (curDesc.mapDim.x >> 1), coord[1].y + (curDesc.mapDim.y >> 1), coord[1].z + (curDesc.zso),
									  coord[2].x + (curDesc.mapDim.x >> 1), coord[2].y + (curDesc.mapDim.y >> 1), coord[2].z + (curDesc.zso),
									  coord[3].x + (curDesc.mapDim.x >> 1), coord[3].y + (curDesc.mapDim.y >> 1), coord[3].z + (curDesc.zso) };

		// If out of bounds
		results.x = results.y = results.z = results.w = -1;
		if(vCell[0].x < 0 || vCell[0].x >= curDesc.mapDim.x) return false;
		if(vCell[0].y < 0 || vCell[0].y >= curDesc.mapDim.y) return false;
		if(vCell[0].z < 0 || vCell[0].z >= curDesc.mapDim.z) return false;

		cVEC3Du16 vChunkDim         = (VEC3Du16 &)curDesc.chunkDim;
		cui32     uiChunkCells_XY   = vChunkDim.x * vChunkDim.y;
		cui32     uiChunkCells      = curDesc.chunkCells;
		cui32     uiChunkRowCells   = uiChunkCells * (world[worldIndex].map[mapIndex]->pCB->chunkCount.x + 1);
		cui32     uiChunkPlaneCells = uiChunkRowCells * (world[worldIndex].map[mapIndex]->pCB->chunkCount.y + 1);

		cui32 uiCellX[4] = { (vCell[0].x & (vChunkDim.x - 1)) + ((vCell[0].x / vChunkDim.x) * uiChunkCells),
									(vCell[1].x & (vChunkDim.x - 1)) + ((vCell[1].x / vChunkDim.x) * uiChunkCells),
									(vCell[2].x & (vChunkDim.x - 1)) + ((vCell[2].x / vChunkDim.x) * uiChunkCells),
									(vCell[3].x & (vChunkDim.x - 1)) + ((vCell[3].x / vChunkDim.x) * uiChunkCells) };
		cui32 uiCellY[4] = { ((vCell[0].y & (vChunkDim.y - 1)) * vChunkDim.x) + ((vCell[0].y / vChunkDim.y) * uiChunkRowCells),
									((vCell[1].y & (vChunkDim.y - 1)) * vChunkDim.x) + ((vCell[1].y / vChunkDim.y) * uiChunkRowCells),
									((vCell[2].y & (vChunkDim.y - 1)) * vChunkDim.x) + ((vCell[2].y / vChunkDim.y) * uiChunkRowCells),
									((vCell[3].y & (vChunkDim.y - 1)) * vChunkDim.x) + ((vCell[3].y / vChunkDim.y) * uiChunkRowCells) };
		cui32 uiCellZ[4] = { ((vCell[0].z & (vChunkDim.z - 1)) * uiChunkCells_XY) + ((vCell[0].z / vChunkDim.z) * uiChunkPlaneCells),
									((vCell[1].z & (vChunkDim.z - 1)) * uiChunkCells_XY) + ((vCell[1].z / vChunkDim.z) * uiChunkPlaneCells),
									((vCell[2].z & (vChunkDim.z - 1)) * uiChunkCells_XY) + ((vCell[2].z / vChunkDim.z) * uiChunkPlaneCells),
									((vCell[3].z & (vChunkDim.z - 1)) * uiChunkCells_XY) + ((vCell[3].z / vChunkDim.z) * uiChunkPlaneCells) };

		cui32 uiCell[4]  = { uiCellX[0] + uiCellY[0] + uiCellZ[0], uiCellX[1] + uiCellY[1] + uiCellZ[1],
									uiCellX[2] + uiCellY[2] + uiCellZ[2], uiCellX[3] + uiCellY[3] + uiCellZ[3] };

		if(uiCell[0] < world[worldIndex].map[mapIndex]->pCB->totalCells.y) {
			results.x = uiCell[0];
			if(vCell[1].x >= 0) results.y = uiCell[1];
			if(vCell[2].y >= 0) results.z = uiCell[2];
			if(vCell[3].x >= 0 && vCell[3].y >= 0) results.w = uiCell[3];
			return true;
		}
		return false;
	}

	inline void ModQuadCellDensity(cVEC3Ds32 coord, cfl32 densityMod, csi32 worldIndex, csi32 mapIndex) {
		cMAP &curMap = *world[worldIndex].map[mapIndex];

		si32 cell[4];

		cVEC3Ds32 coords[4] = { { coord }, { coord.x - 1, coord.y, coord.z }, { coord.x, coord.y - 1, coord.z }, { coord.x - 1, coord.y - 1, coord.z } };
		cVEC3Ds32 above[4]  = { { coord.x, coord.y, coord.z - 1 }, { coord.x - 1, coord.y, coord.z - 1 }, { coord.x, coord.y - 1, coord.z - 1 }, { coord.x - 1, coord.y - 1, coord.z - 1 } };
		cVEC3Ds32 below[4]  = { { coord.x, coord.y, coord.z + 1 }, { coord.x - 1, coord.y, coord.z + 1 }, { coord.x, coord.y - 1, coord.z + 1 }, { coord.x - 1, coord.y - 1, coord.z + 1 } };

		if(CalcQuadCellIndices((VEC4Ds32 &)cell, coords, worldIndex, mapIndex))
			for(ui8 i = 0; i < 4; i++) {
				if(cell[i] == -1) continue;
				cfl32 fDensity     = float(curMap.cell[cell[i]].geometry->dens) * 0.000015259254737998596148564104129154f + densityMod;
				csi32 siAbove      = CalcCellIndex(above[i], worldIndex, mapIndex);
				csi32 siBelow      = CalcCellIndex(below[i], worldIndex, mapIndex);
				csi32 chunkIndex   = (cell[i] / curMap.desc.chunkCells);
				cui16 densityBelow = (siBelow != -1 ? curMap.cell[siBelow].geometry->dens : 0);
				if(fDensity < 0.0f) {
					curMap.cell[cell[i]].geometry->dens = 0;
					//map[mapIndex][cell[i]].geometry->et.x = 0;
					if(siBelow != -1) {
						if(densityBelow == 0x0FFFF) {
							curMap.cell[siBelow].geometry->dens = 0x0FFFE;
							csi32 chunkIndex = (siBelow / curMap.desc.chunkCells);
							curMap.chunkMod[chunkIndex >> 6] |= ui64(0x01) << (chunkIndex & 0x03F);
						}
					}
				} else if(fDensity > 1.0f) {
//					if(siAbove != -1) {
//						map[mapIndex][cell[i]].geometry->dens = 0x0FFFF;
//						if(cui16 densityAbove = map[mapIndex][siAbove].geometry->dens > 0) {
//							map[mapIndex][siAbove].geometry->et.x = map[mapIndex][cell[i]].geometry->et.x;
//							map[mapIndex][siAbove].geometry->er.x = 255;
//						}
//						csi32 chunkIndex = (siAbove / world[mapIndex]->desc.totalCells.x);
//						world[mapIndex]->chunkMod[chunkIndex >> 6] |= ui64(0x01) << (chunkIndex & 0x03F);
//					} else
						curMap.cell[cell[i]].geometry->dens = 0x0FFFE;
				} else {
					if(curMap.cell[cell[i]].geometry->dens == 0 && curMap.cell[cell[i]].geometry->et.x == 0) {
						curMap.cell[cell[i]].geometry->et.x = (siBelow != -1 ? curMap.cell[siBelow].geometry->et.x : 1);
						curMap.cell[cell[i]].geometry->er.x = 255;
					}
					curMap.cell[cell[i]].geometry->dens = Fix16(fDensity, 65534.0f);
					if(densityBelow == 0x0FFFE) {
						curMap.cell[siBelow].geometry->dens = 0x0FFFF;
						csi32 chunkIndex = (siBelow / curMap.desc.chunkCells);
						curMap.chunkMod[chunkIndex >> 6] |= ui64(0x01) << (chunkIndex & 0x03F);
					}
				}
				curMap.chunkMod[chunkIndex >> 6] |= ui64(0x01) << (chunkIndex & 0x03F);
			}
	}

	private : inline void Cull_Nonvisible_and_Unchanged(ptr threadData) {
		CLASS_CAMERAS * const camMan = (CLASS_CAMERAS *)ptrLib[1];

		cMMTDptr  data[2] = { (cMMTDptr)threadData, (cMMTDptr)threadData + 1 };
		MAP      *map     = data[0]->map;

		cSSE4Ds32 mapChunksH = { .vector = { map->desc.chunkCount.x >> 1, map->desc.chunkCount.y >> 1, map->desc.chunkCount.z >> 1, 1 } };
		cSSE4Ds32 mapChunksL = { .vector = { mapChunksH.vector.x - map->desc.chunkCount.x, mapChunksH.vector.y - map->desc.chunkCount.y,
														 mapChunksH.vector.z - map->desc.chunkCount.z, 1 } };

		cui32 chunkCount = map->desc.mapChunks;

		SSE4Ds32 chunkOS   = {};
		ui32ptrc nearCells = data[0]->chunkVis[0];
		ui32ptrc medCells  = data[0]->chunkVis[1];
		ui32ptrc farCells  = data[0]->chunkVis[2];
		ui32ptrc modCells  = data[1]->chunkMod;

		ui64 modCount, nearCount, medCount, farCount;
		ui32 i;

		camMan->SetDimsf(map->desc.chunkDim, map->desc.chunkCount, 0);

		nearCount = medCount = farCount = 0;
		modCount = 0;

		for(i = 0; i < chunkCount; i++) {
			cui64 bitOS   = ui64(0x01) << (i & 0x03F);
			cui32 qwordOS = i >> 6;

			if(map->chunkMod[qwordOS] & bitOS) {
				modCells[modCount++] = i;
				map->chunkMod[qwordOS] ^= bitOS;
			}
		}

		// Calculate each chunk's distance-to-camera and reject out-of-view chunks
		// Sort nearest-to-furthest into L.O.D. lists for input assembler
		for(chunkOS.vector.z = mapChunksL.vector.z; chunkOS.vector.z < mapChunksH.vector.z; chunkOS.vector.z++) {
			for(chunkOS.vector.y = mapChunksL.vector.y; chunkOS.vector.y < mapChunksH.vector.y; chunkOS.vector.y++) {
				for(chunkOS.vector.x = mapChunksL.vector.x; chunkOS.vector.x < mapChunksH.vector.x;) {
					cui8 visible = camMan->ChunkFrustumIntersect8(chunkOS, 0);

					for(i = 0; i < 8; i++, chunkOS.vector.x++)
						if(visible & 0x01 << i) {
							csi32 chunkIndex = CalcChunkIndex((cVEC3Ds32 &)chunkOS, 0, 0);
							cui32 QWordOS    = chunkIndex >> 6;
							cui64 bitOS      = ui64(0x01) << (chunkIndex & 0x03F);
							cfl32 distance = camMan->DistanceFromCamera(chunkOS.xmm, 0);

							// Change hard limits to LOD scalars
							if(map->chunkVis[QWordOS] & bitOS) {
								//								if(distance < 128.0f)
								nearCells[nearCount++] = chunkIndex;
								//								else if(distance < 512.0f)
								//									medCells[medCount++] = chunkIndex;
								//								else if(distance < 2048.0f)
								//									farCells[farCount++] = chunkIndex;
							}
						}
				}
			}
		}

		MAPMAN_THREAD_STATUS.m128i_u64[0] = (modCount << 34) | (nearCount << 4);
		MAPMAN_THREAD_STATUS.m128i_u64[1] = medCount | (farCount << 30);
	}

	public : inline si32 Cull(ui32aptrc arrayVisible, ui32ptrc arrayUnchanged, csi32 worldIndex, csi32 mapIndex, csi8 threadCount) {
		static ui8 uiTHREADS = 0;

		if(!arrayVisible) { MAPMAN_THREAD_STATUS.m128i_u8[0] &= 0x0F3; return 0; }

		cui8 threadBits = MAPMAN_THREAD_STATUS.m128i_u8[0];

		if(!(threadBits & 0x01)) threadData[0] = { world[worldIndex].map[mapIndex], arrayVisible };
		if(!(threadBits & 0x02)) threadData[1] = { world[worldIndex].map[mapIndex], arrayUnchanged };

		///- Stall/skip? if status if 'busy'
		while(MAPMAN_THREAD_STATUS.m128i_u8[0] & 0x03) Sleep(1);
		//if(bits) return si32(0x080000000 | bits);
		//MAPMAN_THREAD_STATUS.m128i_u8[0] &= 0x0FC;

		switch(threadCount) {
		case 0:
			Cull_Nonvisible_and_Unchanged(&threadData[0]);
			return 3;
		case 1:
			MAPMAN_THREAD_STATUS.m128i_u8[0] |= 0x03;
			if(!(uiTHREADS & 0x03)) {
				HANDLE thread0 = (HANDLE)_beginthread(_MM_Cull_Nonvisible_and_Unchanged, 0, &threadData[0]);
				SetThreadIdealProcessor(thread0, 4);
				SetThreadPriority(thread0, -1);
				uiTHREADS |= 0x03;
			}
			return 3;
		case 2:
			MAPMAN_THREAD_STATUS.m128i_u8[0] |= 0x03;
			if(!(uiTHREADS & 0x03)) {
				//HANDLE thread0 = (HANDLE)_beginthread(_MM_Cull_Nonvisible_Simple, 0, &threadData[0]);
				HANDLE thread0 = (HANDLE)_beginthread(_MM_Cull_Nonvisible_Accurate, 0, &threadData[0]);
				SetThreadPriority(thread0, -2);
				HANDLE thread1 = (HANDLE)_beginthread(_MM_Cull_Unchanged, 0, &threadData[1]);
				SetThreadPriority(thread1, -1);
				uiTHREADS |= 0x03;
			}
			return 3;
		case -1:
			MAPMAN_THREAD_STATUS.m128i_u8[0] |= 0x01;
			if(!(uiTHREADS & 0x01)) {
				//HANDLE thread0 = (HANDLE)_beginthread(_MM_Cull_Nonvisible_Simple, 0, &threadData[0]);
				HANDLE thread0 = (HANDLE)_beginthread(_MM_Cull_Nonvisible_Accurate, 0, &threadData[0]);
				SetThreadPriority(thread0, -1);
				uiTHREADS |= 0x01;
			}
			return 1;
		case -2:
			MAPMAN_THREAD_STATUS.m128i_u8[0] |= 0x02;
			if(!(uiTHREADS & 0x02)) {
				HANDLE thread1 = (HANDLE)_beginthread(_MM_Cull_Unchanged, 0, &threadData[1]);
				SetThreadPriority(thread1, -1);
				uiTHREADS |= 0x02;
			}
			return 2;
		}

		MAPMAN_THREAD_STATUS.m128i_u8[0] &= 0x0F3;

		return 0;
	}

	inline cVEC4Du32 WaitForCulling(cDWORD sleepDelay) const {
		while(MAPMAN_THREAD_STATUS.m128i_u8[0] & 0x03) Sleep(sleepDelay);

		cVEC2Du64 mapManThreadData = (cVEC2Du64 &)MAPMAN_THREAD_STATUS;

		MAPMAN_THREAD_STATUS.m128i_u64[0] &= 0x0C;
		MAPMAN_THREAD_STATUS.m128i_u64[1] = 0;

		return { ui32(mapManThreadData.x >> 4) & 0x03FFFFFFF, ui32(mapManThreadData.x >> 34) & 0x03FFFFFFF,
					ui32(mapManThreadData.y) & 0x03FFFFFFF, ui32(mapManThreadData.y >> 30) & 0x03FFFFFFF };
	}
};

/*inline static void _Cull_Nonvisible_RasteriseLayer(cVEC4Ds32 lines[2]) {
	extern CLASS_MAPMANAGER &mapMan;
	extern cMMTDptr          data;
	extern ui64              chunks;

	VEC4Ds32 line     = lines[0];
	float    accum[2] = { 0.0f, 0.0f };
	cui32    rows     = lines[1].y - lines[0].y + 1;
	cfl32   delta[2] = { float(lines[1].x - lines[0].x + (lines[0].x <= lines[1].x ? 1: -1)) / float(rows),
								 float(lines[1].w - lines[0].w + (lines[0].w <= lines[1].w ? 1: -1)) / float(rows) };

	while(line.y < lines[1].y) {
		accum[0] += delta[0];
		accum[1] += delta[1];
		++line.y;
		line.x = si32(accum[0]);
		line.w = si32(accum[1]);

		for(ui32 i = 0; i < line.w; i++) {
			line.x += i;
			csi32 chunkIndex = mapMan.CalcChunkIndex((VEC3Ds32 &)line, 0);
			cui32 byteOS     = chunkIndex >> 6;
			cui64 bitOS      = ui64(0x01) << (chunkIndex & 0x03F);
			if(data->map->chunkVis[byteOS] & bitOS) {
				data->buf[chunks++] = chunkIndex;
				data->map->chunkVis[byteOS] ^= bitOS;
			}
		}
	}
}

static void _Cull_Nonvisible_Rasterise(cVEC4Ds32 lines[4]) {
	VEC4Ds32 line[2]  = { lines[0], lines[1] };
	float    accum[6] = { 0.0f, 0.0f, 0.0f, 0.0f };
	cui32    cols[2]  = { lines[2].z - lines[0].z + 1, lines[3].z - lines[1].z + 1 };
	cfl32   delta[6] = { float(lines[2].x - lines[0].x + (lines[3].x <= lines[1].x ? 1 : -1)) / float(cols[0]),
		float(lines[2].y - lines[0].y + (lines[3].y <= lines[1].y ? 1 : -1)) / float(cols[0]),
		float(lines[2].w - lines[0].w + (lines[2].w <= lines[0].w ? 1 : -1)) / float(cols[0]),
		float(lines[3].x - lines[1].x + (lines[3].x <= lines[1].x ? 1 : -1)) / float(cols[1]),
		float(lines[3].y - lines[1].y + (lines[3].y <= lines[1].y ? 1 : -1)) / float(cols[1]),
		float(lines[3].w - lines[1].w + (lines[3].w <= lines[1].w ? 1 : -1)) / float(cols[1]) };

	while(line[0].z < lines[2].z) {
		accum[0] += delta[0];
		accum[1] += delta[1];
		accum[2] += delta[2];
		accum[3] += delta[3];
		accum[4] += delta[4];
		accum[5] += delta[5];
		++line[0].z;
		++line[1].z;
		line[0].x = si32(accum[0]);
		line[0].y = si32(accum[1]);
		line[0].w = si32(accum[2]);
		line[1].x = si32(accum[3]);
		line[1].y = si32(accum[4]);
		line[1].w = si32(accum[5]);

		_Cull_Nonvisible_RasteriseLayer(line);
	}
}*/

static void _MM_Cull_Nonvisible_Simple(ptr threadData) {
	CLASS_CAMERAS    * const camMan = (CLASS_CAMERAS *)ptrLib[1];
	CLASS_MAPMANAGER * const mapMan = (CLASS_MAPMANAGER *)ptrLib[2];

	cMMTDptr  data       = (cMMTDptr)threadData;
	cVEC2Ds32 vScrDim    = { si32(ScrRes.dims[ScrRes.state].w) - 1, si32(ScrRes.dims[ScrRes.state].h) - 1 };
	cSSE4Ds32 chunkCount = { .vector = { data->map->pCB->chunkCount.x, data->map->pCB->chunkCount.y,
													 data->map->pCB->chunkCount.z, data->map->desc.mapChunks } };
	cSSE4Ds32 mapDim     = { .vector = { data->map->desc.mapDim.x >> 1, data->map->desc.mapDim.y >> 1, data->map->desc.mapDim.z >> 1, 1 } };
	cSSE4Ds32 chunkDim   = { .vector = { data->map->desc.chunkDim.x, data->map->desc.chunkDim.y, data->map->desc.chunkDim.z, 1 } };

	SSE4Ds32 coords[2];	// Change to AVX8Ds32
	SSE4Ds32 vChunk[8];
	ui64     chunks;
	si32     i;

	MAPMAN_THREAD_STATUS.m128i_u8[0] |= 0x04;

	do {
		while(!(MAPMAN_THREAD_STATUS.m128i_u8[0] & 0x01)) Sleep(1);

		chunks = 0;

		// Calculate top- & bottom-most cell indices at screen corners
		cfl32x4 camOffset = { 0.0f, 0.0f, -4.0f, 0.0f };
		csi32   zOS[2]    = { 0 - mapDim.vector.z, mapDim.vector.z - 1 };

		(VEC2Ds32 &)vChunk[0] = camMan->CursorLayerIntersect(zOS[0], camOffset, VEC2Ds32{0, vScrDim.y}, 0, 0);
		(VEC2Ds32 &)vChunk[1] = camMan->CursorLayerIntersect(zOS[0], camOffset, vScrDim, 0, 0);
		(VEC2Ds32 &)vChunk[2] = camMan->CursorLayerIntersect(zOS[0], camOffset, VEC2Ds32{0, 0}, 0, 0);
		(VEC2Ds32 &)vChunk[3] = camMan->CursorLayerIntersect(zOS[0], camOffset, VEC2Ds32{vScrDim.x, 0}, 0, 0);
		(VEC2Ds32 &)vChunk[4] = camMan->CursorLayerIntersect(zOS[1], camOffset, VEC2Ds32{0, vScrDim.y}, 0, 0);
		(VEC2Ds32 &)vChunk[5] = camMan->CursorLayerIntersect(zOS[1], camOffset, vScrDim, 0, 0);
		(VEC2Ds32 &)vChunk[6] = camMan->CursorLayerIntersect(zOS[1], camOffset, VEC2Ds32{0, 0}, 0, 0);
		(VEC2Ds32 &)vChunk[7] = camMan->CursorLayerIntersect(zOS[1], camOffset, VEC2Ds32{vScrDim.x, 0}, 0, 0);

		// Calculate chunk derivatives
		for(i = 0; i < 8; i++) {
			vChunk[i].vector.z = (i & 0x04);
			vChunk[i].xmm = _mm_div_epi32(_mm_add_epi32(vChunk[i].xmm, mapDim.xmm), chunkDim.xmm);
		}

		// Expand coordinates to axis-aligned prism
		coords[0].vector = { vChunk[0].vector.x, vChunk[0].vector.y, vChunk[0].vector.x, vChunk[0].vector.y };
		coords[1].vector = { vChunk[4].vector.x, vChunk[4].vector.y, vChunk[4].vector.x, vChunk[4].vector.y };
		// Select top-most vertex and other same-side vertex, then set coords[side]
		// Expand coordinates on opposite side to axis-alignment
		// !!! Replace following for loop !!!
		// !!! Does not work with constrainment. Rely on 0x080000001 for out-of-bounds exclusion. !!!

		for(i = 1; i < 4; i++) {
			if(coords[0].vector.x1 > vChunk[i].vector.x) {
				coords[0].vector.x1 = vChunk[i].vector.x;
				coords[1].vector.x1 = vChunk[i + 4].vector.x;
			}
			if(coords[0].vector.x2 < vChunk[i].vector.x) {
				coords[0].vector.x2 = vChunk[i].vector.x;
				coords[1].vector.x2 = vChunk[i + 4].vector.x;
			}
			if(coords[0].vector.y1 > vChunk[i].vector.y) {
				coords[0].vector.y1 = vChunk[i].vector.y;
				coords[1].vector.y1 = vChunk[i + 4].vector.y;
			}
			if(coords[0].vector.y2 < vChunk[i].vector.y) {
				coords[0].vector.y2 = vChunk[i].vector.y;
				coords[1].vector.y2 = vChunk[i + 4].vector.y;
			}
		}

		// Constrain coordinates to map limits
		if(coords[0].vector.x1 < 0) coords[0].vector.x1 = 0;
		if(coords[0].vector.y1 < 0) coords[0].vector.y1 = 0;
		if(coords[0].vector.x2 > chunkCount.vector.x) coords[0].vector.x2 = chunkCount.vector.x;
		if(coords[0].vector.y2 > chunkCount.vector.y) coords[0].vector.y2 = chunkCount.vector.y;
		if(coords[1].vector.x1 < 0) coords[1].vector.x1 = 0;
		if(coords[1].vector.y1 < 0) coords[1].vector.y1 = 0;
		if(coords[1].vector.x2 > chunkCount.vector.x) coords[1].vector.x2 = chunkCount.vector.x;
		if(coords[1].vector.y2 > chunkCount.vector.y) coords[1].vector.y2 = chunkCount.vector.y;
		// Calculate run lengths
		csi32     lengths[2] = { coords[0].vector.x2 - coords[0].vector.x1 + 1, coords[1].vector.x2 - coords[1].vector.x1 + 1 };
		cAVX8Ds32 lines[2]   = { { .vector0 = { coords[0].vector.x1, coords[0].vector.y1, vChunk[0].vector.z, lengths[0] },
											.vector1 = { coords[0].vector.x2, coords[0].vector.y2, vChunk[0].vector.z, lengths[0] } },
										 { .vector0 = { coords[1].vector.x1, coords[1].vector.y1, vChunk[4].vector.z, lengths[1] },
											.vector1 = { coords[1].vector.x2, coords[1].vector.y2, vChunk[4].vector.z, lengths[1] } } };
		// Rasterise prism
		AVX8Df32  accum     = (cAVX8Df32 &)_mm256_cvtepi32_ps((si256 &)lines);
		AVX8Ds32  line      = lines[0];
		cAVX8Ds32 sCols     = { .xmm0 = _mm_broadcastd_epi32(si128{.m128i_i32 = lines[1].vector0.z - lines[0].vector0.z + 1}),
										.xmm1 = _mm_broadcastd_epi32(si128{.m128i_i32 = lines[1].vector1.z - lines[0].vector1.z + 1}) };
		cAVX8Df32 fCols     = { .ymm = _mm256_cvtepi32_ps(sCols.ymm) };
		cAVX8Ds32 xyDelta   = { .ymm = _mm256_sub_epi32(lines[1].ymm, lines[0].ymm) };
		cAVX8Ds32 ones      = { .ymm = { .m256i_i32 = { 1, 1, 1, 1, 1, 1, 1, 1 } } };
		cAVX8Ds32 signTest  = { .ymm = _mm256_sign_epi32(ones.ymm, xyDelta.ymm)};
		cAVX8Ds32 signMod   = { .ymm = _mm256_or_epi32(signTest.ymm, ones.ymm) };
		cAVX8Ds32 sPredelta = { .ymm = _mm256_add_epi32(signMod.ymm, xyDelta.ymm) };
		cAVX8Df32 fPredelta = { .ymm = _mm256_cvtepi32_ps(sPredelta.ymm) };
		cAVX8Df32 delta     = { .ymm = _mm256_div_ps(fPredelta.ymm, fCols.ymm) };

		while(line.vector[0].z <= lines[1].vector0.z) {
			// Rasterise slice
			for(SSE4Ds32 lineX = { .xmm = line.xmm[0] }; lineX.vector.y <= line.vector[1].y; lineX.vector.x = line.vector[0].x, lineX.vector.y++)
				for(csi32 endPoint = lineX.vector.x + lineX.vector.w; lineX.vector.x < endPoint; lineX.vector.x++) {
					csi32     chunkIndex = mapMan->CalcChunkIndex_((cVEC3Ds32 &)lineX, 0, 0);
					cui32     QWordOS    = chunkIndex >> 6;
					cui64     bitOS      = ui64(0x01) << (chunkIndex & 0x03F);

					if(chunkIndex != 0x080000001 && data->map->chunkVis[QWordOS] & bitOS)
						data->chunkVis[0][chunks++] = chunkIndex;
				}
			accum.ymm = _mm256_add_ps(delta.ymm, accum.ymm);
			line.ymm = _mm256_cvtps_epi32(accum.ymm);
		}

		MAPMAN_THREAD_STATUS.m128i_u64[0] ^= (chunks << 34) | 0x01;
	} while(MAPMAN_THREAD_STATUS.m128i_u8[0] & 0x04);
}

static void _MM_Cull_Nonvisible_Accurate(ptr threadData) {
	CLASS_CAMERAS    * const camMan = (CLASS_CAMERAS *)ptrLib[1];
	CLASS_MAPMANAGER * const mapMan = (CLASS_MAPMANAGER *)ptrLib[2];

	cMMTDptr  data       = (cMMTDptr)threadData;
	cSSE4Ds32 mapChunksH = { .vector = { data->map->desc.chunkCount.x >> 1, data->map->desc.chunkCount.y >> 1, data->map->desc.chunkCount.z >> 1, 1 } };
	cSSE4Ds32 mapChunksL = { .vector = { mapChunksH.vector.x - data->map->desc.chunkCount.x, mapChunksH.vector.y - data->map->desc.chunkCount.y,
													 mapChunksH.vector.z - data->map->desc.chunkCount.z, 1 } };

	SSE4Ds32 chunkOS   = {};
	ui32ptrc nearCells = data->chunkVis[0];
	ui32ptrc medCells  = data->chunkVis[1];
	ui32ptrc farCells  = data->chunkVis[2];

	ui64 nearCount, medCount, farCount;
	ui8  i;

//	MAPMAN_THREAD_STATUS.m128i_u8[0] &= 0x0FFFFFFFC0000000F;
	MAPMAN_THREAD_STATUS.m128i_u8[0] |= 0x04;

	do {
		while(!(MAPMAN_THREAD_STATUS.m128i_u8[0] & 0x01)) Sleep(1);

		camMan->SetDimsf(data->map->desc.chunkDim, data->map->desc.chunkCount, 0);

		nearCount = medCount = farCount = 0;

		// Calculate each chunk's distance-to-camera and reject out-of-view chunks
		// Sort nearest-to-furthest into L.O.D. lists for input assembler
		for(chunkOS.vector.z = mapChunksL.vector.z; chunkOS.vector.z < mapChunksH.vector.z; chunkOS.vector.z++) {
			for(chunkOS.vector.y = mapChunksL.vector.y; chunkOS.vector.y < mapChunksH.vector.y; chunkOS.vector.y++) {
				for(chunkOS.vector.x = mapChunksL.vector.x; chunkOS.vector.x < mapChunksH.vector.x;) {
///--- ??? Swap order of tests ???
					//cSSE4Ds32 cell2 = { .vector = { chunkOS.vector.x + 1, chunkOS.vector.y, chunkOS.vector.z } };
					//cAVX8Ds32 cells = { .xmm = { chunkOS.xmm, cell2.xmm } };
					cui8 visible = camMan->ChunkFrustumIntersect8(chunkOS, 0);

					for(i = 0; i < 8; i++, chunkOS.vector.x++)
						if(visible & 0x01 << i) {
							//cSSE4Ds32 coord      = { .vector = { chunkOS.vector.x, chunkOS.vector.y, chunkOS.vector.z } };
							csi32     chunkIndex = mapMan->CalcChunkIndex((cVEC3Ds32 &)chunkOS, 0, 0);
							cui32     QWordOS    = chunkIndex >> 6;
							cui64     bitOS      = ui64(0x01) << (chunkIndex & 0x03F);

							cfl32 distance = camMan->DistanceFromCamera(chunkOS.xmm, 0);
							// Change hard limits to LOD scalars
							if(data->map->chunkVis[QWordOS] & bitOS) {
//								if(distance < 128.0f)
									nearCells[nearCount++] = chunkIndex;
//								else if(distance < 512.0f)
//									medCells[medCount++] = chunkIndex;
//								else if(distance < 2048.0f)
//									farCells[farCount++] = chunkIndex;
							}
						}
				}
			}
		}

		MAPMAN_THREAD_STATUS.m128i_u64[0] = (nearCount << 34) | 0x01;
		MAPMAN_THREAD_STATUS.m128i_u64[1] = (farCount << 30) | medCount;
	} while(MAPMAN_THREAD_STATUS.m128i_u8[0] & 0x04);
}

static void _MM_Cull_Unchanged(ptr threadData) {
	cMMTDptr data       = (cMMTDptr)threadData;
	cui32    chunkCount = data->map->desc.mapChunks;

	al16 ui64 j;
	     ui32 i;

//	MAPMAN_THREAD_STATUS &= 0x03FFFFFFFF;
	MAPMAN_THREAD_STATUS.m128i_u8[0] |= 0x08;

	do {
		while(!(MAPMAN_THREAD_STATUS.m128i_u8[0] & 0x02)) Sleep(1);

		for(i = 0, j = 0; i < chunkCount; i++) {
			cui64 bitOS  = ui64(0x01) << (i & 0x03F);
			cui32 qwordOS = i >> 6;

			if(data->map->chunkMod[qwordOS] & bitOS) {
				data->chunkMod[j++] = i;
				data->map->chunkMod[qwordOS] ^= bitOS;
			}
		}

		MAPMAN_THREAD_STATUS.m128i_u64[0] ^= (j << 4) | 0x02;
	} while(MAPMAN_THREAD_STATUS.m128i_u8[0] & 0x08);
}

static void _MM_Cull_Nonvisible_and_Unchanged(ptr threadData) {
		static si64 frequencyTics, startTics, endTics;
		QueryPerformanceFrequency((LARGE_INTEGER *)&frequencyTics);

	CLASS_CAMERAS    * const camMan = (CLASS_CAMERAS *)ptrLib[1];
	CLASS_MAPMANAGER * const mapMan = (CLASS_MAPMANAGER *)ptrLib[2];

	cMMTDptr  data[2] = { (cMMTDptr)threadData, (cMMTDptr)threadData + 1 };
	MAP      *map     = data[0]->map;

	cSSE4Ds32 mapChunksH = { .vector = { map->desc.chunkCount.x >> 1, map->desc.chunkCount.y >> 1, (map->desc.chunkCount.z + 1) >> 1, 1 } };
	cSSE4Ds32 mapChunksL = { .vector = { mapChunksH.vector.x - map->desc.chunkCount.x, mapChunksH.vector.y - map->desc.chunkCount.y,
													 mapChunksH.vector.z - map->desc.chunkCount.z, 1 } };

	cui32 chunkCount = map->desc.mapChunks;

	SSE4Ds32 chunkOS   = {};
	ui32ptrc nearCells = data[0]->chunkVis[0];
	ui32ptrc medCells  = data[0]->chunkVis[1];
	ui32ptrc farCells  = data[0]->chunkVis[2];
	ui32ptrc modCells  = data[1]->chunkMod;

	ui64 modCount, nearCount, medCount, farCount;
	ui32 i;

	MAPMAN_THREAD_STATUS.m128i_u8[0] |= 0x0C;

	do {
		while(!(MAPMAN_THREAD_STATUS.m128i_u8[0] & 0x03)) Sleep(1);

		QueryPerformanceCounter((LARGE_INTEGER *)&startTics);

		camMan->SetDimsf(map->desc.chunkDim, map->desc.chunkCount, 0);

		nearCount = medCount = farCount = 0;
		modCount = 0;

		for(i = 0; i < chunkCount; i++) {
			cui64 bitOS   = (ui64)0x01 << (i & 0x03F);
			cui32 qwordOS = i >> 6;

			if(map->chunkMod[qwordOS] & bitOS) {
				modCells[modCount++] = i;
				map->chunkMod[qwordOS] ^= bitOS;
			}
		}

		// Calculate each chunk's distance-to-camera and reject out-of-view chunks
		// Sort nearest-to-furthest into L.O.D. lists for input assembler
		for(chunkOS.vector.z = mapChunksL.vector.z; chunkOS.vector.z < mapChunksH.vector.z; chunkOS.vector.z++) {
			for(chunkOS.vector.y = mapChunksL.vector.y; chunkOS.vector.y < mapChunksH.vector.y; chunkOS.vector.y++) {
				for(chunkOS.vector.x = mapChunksL.vector.x; chunkOS.vector.x < mapChunksH.vector.x;) {
					cui8 visible = camMan->ChunkFrustumIntersect8(chunkOS, 0);

					for(i = 0; i < 8; i++, chunkOS.vector.x++)
						if(visible & (0x01 << i)) {
							csi32 chunkIndex = mapMan->CalcChunkIndex((cVEC3Ds32 &)chunkOS, 0, 0);
							cui32 QWordOS    = chunkIndex >> 6;
							cui64 bitOS      = (ui64)0x01 << (chunkIndex & 0x03F);

							cfl32 distance = camMan->DistanceFromCamera(chunkOS.xmm, 0);
							// Change hard limits to LOD scalars
							if(map->chunkVis[QWordOS] & bitOS) {
//								if(distance < 128.0f)
									nearCells[nearCount++] = chunkIndex;
//								else if(distance < 512.0f)
//									medCells[medCount++] = chunkIndex;
//								else if(distance < 2048.0f)
//									farCells[farCount++] = chunkIndex;
							}
						}
				}
			}
		}

		MAPMAN_THREAD_STATUS.m128i_u64[0] = (medCount << 34) | (nearCount << 4) | 0x0C;
		MAPMAN_THREAD_STATUS.m128i_u64[1] = (modCount << 30) | farCount;

			QueryPerformanceCounter((LARGE_INTEGER *)&endTics);
			sysData.culling.map.time   = double(endTics - startTics) / double(frequencyTics) * 1000.0;
			sysData.culling.map.mod    = (ui32)modCount;
			sysData.culling.map.vis[0] = (ui32)nearCount;
			sysData.culling.map.vis[1] = (ui32)medCount;
			sysData.culling.map.vis[2] = (ui32)farCount;
	} while(MAPMAN_THREAD_STATUS.m128i_u64[0] & 0x0C);
}
