/************************************************************
 * File: class_mapmanager.h             Created: 2022/11/29 *
 *                                Last modified: 2024/06/29 *
 *                                                          *
 * Desc:                                                    *
 *                                                          *
 * To do: Change Read/WriteFile to files. equivalents.      *
 *                                                          *
 *  Copyright (c) David William Bull. All rights reserved.  *
 ************************************************************/
#pragma once

#include "master header.h"
#include "Data structures.h"
#include "Map structures.h"
#include "File operations.h"
#include "Common functions.h"

extern vui128 MAPMAN_THREAD_STATUS;

static void _MM_Cull_Nonvisible_RasteriseLayer(cVEC4Ds32[2]);
static void _MM_Cull_Nonvisible_Rasterise(cVEC4Ds32[4]);
static void _MM_Cull_Nonvisible_and_Unchanged(ptr);
static void _MM_Cull_Nonvisible_Simple(ptr);
static void _MM_Cull_Nonvisible_Accurate(ptr);
static void _MM_Cull_Unchanged(ptr);

// Map manager
al16 struct CLASS_MAPMAN {
   CLASS_FILEOPS &files;

   WORLDptrc      world    = (WORLDptr)zalloc64(sizeof(WORLD[MAX_WORLDS]));
   ELEM_TABLEptrc table    = (ELEM_TABLEptr)zalloc64(RoundUpToNearest64(sizeof(ELEM_TABLE[MAX_TABLES])));
   ELEM_TYPEptrc  element  = (ELEM_TYPEptr)zalloc64(RoundUpToNearest64(sizeof(ELEM_TYPE[MAX_ELEMENTS])));
   ELEM_IGSptrc   elem_igs = (ELEM_IGSptr)zalloc64(RoundUpToNearest64(sizeof(ELEM_IGS[MAX_ELEMENTS])));

   wchar (*stPeriodicName)[MAX_TABLES];
   wchar (*stElementName)[MAX_TABLES];

   VEC2Ds32 worldXY;            // Current world cell
   VEC3Ds32 mapXYZ;             // Current map cell
   si32     totalWorlds;        // Number of worlds
   si32     elemTables;         // Number of element tables
   ui32     uiBytes        = 0;
   ui16     uiMapBoundaries;    // Map edge flags: (Per bit... 0:Finite boundaries, 1:Wrap coordinates) 0-4==X axis, 5-9==Y axis, 10-14==Z axis
   ui8      uiPeriodicName = 0;
   ui8      uiElementName  = 0;

   cwchar stMapsDir[10] = L"map_data\\";

   MAPMAN_THREAD_DATA threadData[2];

   CLASS_MAPMAN(CLASS_FILEOPS &fileOpsClass) : files(fileOpsClass) {
#ifdef AE_PTR_LIB
      ptrLib[6] = this;
#endif
}

   inline cMAP_PTRSc Pointers(csi16 worldIndex, csi16 mapIndex) const {
      return { NULL, world[worldIndex].map[mapIndex]->pDGS, world[worldIndex].map[mapIndex]->pDPS, world[worldIndex].map[mapIndex]->pCB };
   }

   inline MAP_DESC *GetMapDescriptor(csi32 mapIndex, csi32 worldIndex) { return &world[worldIndex].map[mapIndex]->desc; }

   inline MAP_DESC *GetMapDescriptor(cID64 mapID) { return &world[mapID.group].map[mapID.index]->desc; }

   cui32 CreatePeriodicTable(wchptrc name, csi32 maxElements, si32 tableIndex) {
      ui8 i = 0;

      // Find first available slot if index is -1
      if(tableIndex == -1) {
         for(; table[i].element && i < MAX_TABLES; i++);
         if(i >= MAX_TABLES) return 0x080000001;   // All periodic table slots occupied
         tableIndex = i;
      }

      csi32 offset = tableIndex << 8;

      table[tableIndex] = { name, &element[offset], &elem_igs[offset], maxElements };

//      for(i = 0; i < maxElements; i++)
//         table[tableIndex].element[i].geometry = &elem_igs[offset + i];

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

   inline void SetElementMiscVars(csi32 tableIndex, csi32 elementIndex, cfl32 decayRate, cf32x2 elecResist, cf32x2 atomDensity, cfl32 acidity) const {
      table[tableIndex].element[elementIndex].edr  = decayRate;
      table[tableIndex].element[elementIndex].er   = elecResist;
      table[tableIndex].element[elementIndex].ad   = atomDensity;
      table[tableIndex].element[elementIndex].acid = acidity;
   }

   inline void SetElementResults(csi32 tableIndex, csi32 elementIndex, cui8 decayResult, cui8 fusionResult) const {
      table[tableIndex].element[elementIndex].ede = decayResult;
      table[tableIndex].element[elementIndex].efe = fusionResult;
   }

   inline void SetElementGeometry(csi32 tableIndex, csi32 elementIndex, cfl32x4 texCoords, cfl32 tcScalar, cfl32 transparency, cfl32 animFrameTime, cui8 animFrameOS, cui8 animFrameCount, cui8 atlasIndex) {
      table[tableIndex].pIGS[elementIndex].tc  = texCoords;
      table[tableIndex].pIGS[elementIndex].aft = animFrameTime;
      table[tableIndex].pIGS[elementIndex].et  = transparency;
      table[tableIndex].pIGS[elementIndex].afo = animFrameOS;
      table[tableIndex].pIGS[elementIndex].afc = animFrameCount - 1;
      (table[tableIndex].pIGS[elementIndex].tcs = tcScalar)--;
      table[tableIndex].pIGS[elementIndex].ai  = atlasIndex;
   }

   cui32 LoadPeriodicTable(wchptrc filename, si32 index) {
      si32 i = 0;
      // Find first available slot if index is -1
      if(index == -1) {
         for(; table[i].element && i < MAX_TABLES; i++);
         if(i >= MAX_TABLES) return 0x080000001;   // All periodic table slots occupied
         index = i;
      }

      wcscpy(files.wstTemp, stMapsDir);
      wcscpy(files.wstTemp + wcslen(stMapsDir), filename);
      HANDLE hElementData = CreateFile(files.wstTemp, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);

      // Read & process tagline
      files.ReadLine(hElementData, files.wstTemp);
//--- To do...
      // Read primary data
      files.ReadLine(hElementData, stPeriodicName[uiPeriodicName]);
      table[index].stName = stPeriodicName[uiPeriodicName++];
      ReadFile(hElementData, &table[index].numElements, sizeof(ui32), (LPDWORD)&uiBytes, NULL);
      // Read element data
      table[index].element = (ELEM_TYPE *)malloc32(sizeof(ELEM_TYPE) * table[index].numElements);
      for(i = 0; i < table[index].numElements; i++) {
         ReadFile(hElementData, stElementName[uiElementName], DWORD(wcslen(table[index].element[i].stName) + 1) * sizeof(wchar), (LPDWORD)&uiBytes, NULL);
         table[index].element[i].stName = stElementName[uiElementName++];
         ReadFile(hElementData, &table[index].element[i].tmp, sizeof(float), (LPDWORD)&uiBytes, NULL);
         ReadFile(hElementData, &table[index].element[i].tbp, sizeof(float), (LPDWORD)&uiBytes, NULL);
         ReadFile(hElementData, &table[index].element[i].tip, sizeof(float), (LPDWORD)&uiBytes, NULL);
         ReadFile(hElementData, &table[index].element[i].tp, sizeof(float), (LPDWORD)&uiBytes, NULL);
         ReadFile(hElementData, &table[index].element[i].eft, sizeof(float), (LPDWORD)&uiBytes, NULL);
         ReadFile(hElementData, &table[index].element[i].edr, sizeof(float), (LPDWORD)&uiBytes, NULL);
         ReadFile(hElementData, &table[index].element[i].er, sizeof(f1p15x2), (LPDWORD)&uiBytes, NULL);
         ReadFile(hElementData, &table[index].element[i].ad, sizeof(f6p10x2), (LPDWORD)&uiBytes, NULL);
         ReadFile(hElementData, &table[index].element[i].acid, sizeof(f6p10), (LPDWORD)&uiBytes, NULL);
         ReadFile(hElementData, &table[index].element[i].ede, sizeof(ui8), (LPDWORD)&uiBytes, NULL);
         ReadFile(hElementData, &table[index].element[i].efe, sizeof(ui8), (LPDWORD)&uiBytes, NULL);
      }
      table[index].pIGS = (ELEM_IGS *)malloc32(sizeof(ELEM_IGS) * table[index].numElements);
      ReadFile(hElementData, table[index].pIGS, sizeof(ELEM_IGS) * table[index].numElements, (LPDWORD)&uiBytes, NULL);

      return index;
   }

   cui32 SavePeriodicTable(wchptrc filename, csi32 index) {
      // Periodic table slot is empty
      if(!table[index].stName) return 0x080000001;

      wcscpy(files.wstTemp, stMapsDir);
      wcscpy(files.wstTemp + wcslen(stMapsDir), filename);
      HANDLE hElementData = CreateFile(files.wstTemp, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

      // Write tag line: 2[Engine].Elements.2[Format version]1[Compression method]
      WriteFile(hElementData, "AE.Elements.01u\0", 16, (LPDWORD)&uiBytes, NULL);
      // Write primary data
      WriteFile(hElementData, table[index].stName, DWORD(wcslen(table[index].stName) + 1) * sizeof(wchar), (LPDWORD)&uiBytes, NULL);
      WriteFile(hElementData, &table[index].numElements, sizeof(ui32), (LPDWORD)&uiBytes, NULL);
      // Write element data
      for(si32 i = 0; i < table[index].numElements; i++) {
         WriteFile(hElementData, table[index].element[i].stName, DWORD(wcslen(table[index].element[i].stName) + 1) * sizeof(wchar), (LPDWORD)&uiBytes, NULL);
         WriteFile(hElementData, &table[index].element[i].tmp, sizeof(float), (LPDWORD)&uiBytes, NULL);
         WriteFile(hElementData, &table[index].element[i].tbp, sizeof(float), (LPDWORD)&uiBytes, NULL);
         WriteFile(hElementData, &table[index].element[i].tip, sizeof(float), (LPDWORD)&uiBytes, NULL);
         WriteFile(hElementData, &table[index].element[i].tp, sizeof(float), (LPDWORD)&uiBytes, NULL);
         WriteFile(hElementData, &table[index].element[i].eft, sizeof(float), (LPDWORD)&uiBytes, NULL);
         WriteFile(hElementData, &table[index].element[i].edr, sizeof(float), (LPDWORD)&uiBytes, NULL);
         WriteFile(hElementData, &table[index].element[i].er, sizeof(f1p15x2), (LPDWORD)&uiBytes, NULL);
         WriteFile(hElementData, &table[index].element[i].ad, sizeof(f6p10x2), (LPDWORD)&uiBytes, NULL);
         WriteFile(hElementData, &table[index].element[i].acid, sizeof(f6p10), (LPDWORD)&uiBytes, NULL);
         WriteFile(hElementData, &table[index].element[i].ede, sizeof(ui8), (LPDWORD)&uiBytes, NULL);
         WriteFile(hElementData, &table[index].element[i].efe, sizeof(ui8), (LPDWORD)&uiBytes, NULL);
      }
      WriteFile(hElementData, table[index].pIGS, sizeof(ELEM_IGS) * table[index].numElements, (LPDWORD)&uiBytes, NULL);

      CloseHandle(hElementData);

      return 0;
   }

   void DestroyPeriodicTable(cui8 index) const { memset(&table[index], 0x0, sizeof(ELEM_TABLE)); }

   cui32 CreateWorld(csi32 worldIndex, csi32 periodicTableIndex, csi32 maxMaps) {
      if(world[worldIndex].map) return 0x080000001;   // world already exists

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
      for(ui32 i = world[worldIndex].totalMaps - 1; i; i--) DestroyMap(worldIndex, i);

      mfree(world[worldIndex].mapMod, world[worldIndex].mapVis, world[worldIndex].map);

      memset(&world[worldIndex], 0x0, sizeof(WORLD));

      return --totalWorlds;
   }

   // Allocate RAM for a map's .wlrv arrays
   inline void CreateSelectionBuffers(csi32 mapIndex, csi32 worldIndex, csi16 averagePerCell) const {
      MAP       &curMap    = *world[worldIndex].map[mapIndex];
      cSSE4Ds32  dimExp_   = { .xmm = _mm_cvtepi16_epi32((ui128 &)curMap.desc.mapDim) };
      cSSE4Df32  dimExp    = { .xmm = _mm_cvtepi32_ps(_mm_mullo_epi32(dimExp_.xmm, dimExp_.xmm)) };
      cui64      arraySize = ui64(sqrtf(dimExp.vector.x + dimExp.vector.y + dimExp.vector.z) * 2.0f);

      curMap.desc.wlrv.cellIndex   = (si32ptr)zalloc32(arraySize * sizeof(si32));
      curMap.desc.wlrv.entityIndex = (ID64ptr)zalloc32(arraySize * (ui64)averagePerCell * sizeof(ID64));
   }

   inline void DestroySelectedCellBuffer(csi32 mapIndex, csi32 worldIndex) const {
      mfree((*world[worldIndex].map[mapIndex]).desc.wlrv.cellIndex, (*world[worldIndex].map[mapIndex]).desc.wlrv.entityIndex);
   }

   // Allocate RAM for a map's .entityList array
   inline void CreateAssociationBuffer(MAP_DESC &md) const { md.entityList = (ID64ptr)salloc((ui64)md.entListDim * (ui64)md.mapCells * sizeof(ID64), 32, max256); }

   inline void DestroyAssociationBuffer(csi32 mapIndex, csi32 worldIndex) const { mdealloc((*world[worldIndex].map[mapIndex]).desc.entityList); }

   cui32 LoadMap(wchptrc filename, csi32 worldIndex, si32 mapIndex) {
      si32 i = 0;
      // Find first available slot if mapIndex is -1
      if(mapIndex == -1)
         for(mapIndex = 0; mapIndex < world[worldIndex].maxMaps && world[worldIndex].map[mapIndex]; mapIndex++);
      // Map slot already occupied, or all slots occupied
      if(mapIndex >= world[worldIndex].maxMaps) return 0x080000001;

      MAP &curMap = *world[worldIndex].map[mapIndex];

      wcscpy(files.wstTemp, stMapsDir);
      wcscpy(files.wstTemp + wcslen(stMapsDir), filename);
      HANDLE hMapData = CreateFile(files.wstTemp, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);

      // Read & process tagline
      files.ReadLine(hMapData, files.wstTemp);
//--- To do...
      // Read critical map information
      files.ReadLine(hMapData, files.wstTemp);
      curMap.desc.stName = (wchptr)malloc32(wcslen(files.wstTemp) + 1);
      wcscpy(curMap.desc.stName, files.wstTemp);
      files.ReadLine(hMapData, files.wstTemp);
      curMap.desc.stInfo = (wchptr)malloc32(wcslen(files.wstTemp) + 1);
      wcscpy(curMap.desc.stInfo, files.wstTemp);
      // Search periodic table array for match
      files.ReadLine(hMapData, files.wstTemp);
      for(i = 0; !wcscmp(files.wstTemp, table[i].stName) || i < MAX_TABLES; i++);
      if(i < MAX_TABLES)
         curMap.desc.ptIndex = i;
      // Not present; attempt to load
      else {
         for(i = 0; table[i].stName || i < MAX_TABLES; i++);
         if(i >= MAX_TABLES)   // No free slots available
            return 0x080000002;
         LoadPeriodicTable(files.wstTemp, i);
         curMap.desc.ptIndex = i;
      }

      ReadFile(hMapData, &curMap.desc.mapDim, sizeof(VEC3Ds16), (LPDWORD)&uiBytes, NULL);
      ReadFile(hMapData, &curMap.desc.chunkDim, sizeof(VEC3Ds16), (LPDWORD)&uiBytes, NULL);
      ReadFile(hMapData, &curMap.desc.zso, sizeof(si16), (LPDWORD)&uiBytes, NULL);
      ReadFile(hMapData, &curMap.oob.vel, sizeof(VEC2Df), (LPDWORD)&uiBytes, NULL);
      ReadFile(hMapData, &curMap.oob.temp, sizeof(fl32), (LPDWORD)&uiBytes, NULL);
      ReadFile(hMapData, &curMap.oob.rad, sizeof(fl32), (LPDWORD)&uiBytes, NULL);
      ReadFile(hMapData, &curMap.oob.elec, sizeof(fl32), (LPDWORD)&uiBytes, NULL);
      curMap.oob.geometry = NULL;
      curMap.oob.pixel    = NULL;
      // Calculate cached values
      csi32 chunkCells  = curMap.desc.chunkDim.x * curMap.desc.chunkDim.y * curMap.desc.chunkDim.z;
      csi32 totalCells  = curMap.desc.mapDim.x * curMap.desc.mapDim.y * curMap.desc.mapDim.z;
      cui32 totalChunks = totalCells / chunkCells;
      
      cVEC3Du8 chunkCount = { ui8(curMap.desc.mapDim.x / curMap.desc.chunkDim.x - 1), ui8(curMap.desc.mapDim.y / curMap.desc.chunkDim.y - 1), ui8(curMap.desc.mapDim.z / curMap.desc.chunkDim.z - 1) };

      curMap.pCB = (MAPDIMS_ICB *)malloc16(sizeof(MAPDIMS_ICB));
      curMap.desc.chunkCells = chunkCells;
      curMap.desc.mapCells   = totalCells;
      curMap.pCB->setMapDims(curMap.desc.mapDim.x - 1, curMap.desc.mapDim.y - 1, curMap.desc.mapDim.z - 1);
      curMap.pCB->setChunkDims(curMap.desc.chunkDim.x - 1, curMap.desc.chunkDim.y - 1, curMap.desc.chunkDim.z - 1);
//      curMap.pCB->totalCells = { ui32(chunkCells), ui32(totalCells) };
//      curMap.pCB->zso        = ui16(curMap.desc.zso);
//      curMap.pCB->chunkCount = chunkCount;
//      curMap.pCB->bitFlags   = 0;
      // Read cell data
      curMap.cell = (CELL *)malloc32(sizeof(CELL) * curMap.desc.mapCells);
      for(ui32 i = 0; i < curMap.desc.mapCells; i++) {
         ReadFile(hMapData, &curMap.cell[i].vel, sizeof(VEC2Df), (LPDWORD)&uiBytes, NULL);
         ReadFile(hMapData, &curMap.cell[i].temp, sizeof(float), (LPDWORD)&uiBytes, NULL);
         ReadFile(hMapData, &curMap.cell[i].rad, sizeof(float), (LPDWORD)&uiBytes, NULL);
         ReadFile(hMapData, &curMap.cell[i].elec, sizeof(float), (LPDWORD)&uiBytes, NULL);
      }
      curMap.pDGS = (CELL_DGS *)malloc32(sizeof(CELL_DGS) * curMap.desc.mapCells);
      ReadFile(hMapData, curMap.pDGS, sizeof(CELL_DGS) * curMap.desc.mapCells, (LPDWORD)&uiBytes, NULL);
      curMap.pDPS = (CELL_DPS *)malloc32(sizeof(CELL_DPS) * curMap.desc.mapCells);
      ReadFile(hMapData, curMap.pDPS, sizeof(CELL_DPS) * curMap.desc.mapCells, (LPDWORD)&uiBytes, NULL);
      curMap.chunkVis = (ui64 *)malloc16(ui64(totalChunks + 7) >> 3);
      curMap.chunkMod = (ui64 *)malloc16(ui64(totalChunks + 7) >> 3);

      return ++world[worldIndex].totalMaps;
   }

   cui32 SaveMap(wchptrc filename, csi32 mapIndex, csi32 worldIndex) {
      // Map slot is empty
      if(!world[worldIndex].map[mapIndex]) return 0x080000001;

      MAP &curMap = *world[worldIndex].map[mapIndex];

      wcscpy(files.wstTemp, stMapsDir);
      wcscpy(files.wstTemp + wcslen(stMapsDir), filename);
      HANDLE hMapData = CreateFile(files.wstTemp, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

      // Write tag line: 2[Engine].4[Frontend].2[Data type].3[Format version]1[Compression method]
      WriteFile(hMapData, "AE.LV01.MD.001u\0", 16, (LPDWORD)&uiBytes, NULL);
      // Write critical map information
      WriteFile(hMapData, curMap.desc.stName, DWORD(wcslen(curMap.desc.stName) + 1) * sizeof(wchar), (LPDWORD)&uiBytes, NULL);
      WriteFile(hMapData, curMap.desc.stInfo, DWORD(wcslen(curMap.desc.stInfo) + 1) * sizeof(wchar), (LPDWORD)&uiBytes, NULL);
      WriteFile(hMapData, table[curMap.desc.ptIndex].stName, DWORD(wcslen(table[curMap.desc.ptIndex].stName) + 1) * sizeof(wchar), (LPDWORD)&uiBytes, NULL);
      WriteFile(hMapData, &curMap.desc.mapDim, sizeof(VEC3Ds16), (LPDWORD)&uiBytes, NULL);
      WriteFile(hMapData, &curMap.desc.chunkDim, sizeof(VEC3Ds16), (LPDWORD)&uiBytes, NULL);
      WriteFile(hMapData, &curMap.desc.zso, sizeof(si16), (LPDWORD)&uiBytes, NULL);
      WriteFile(hMapData, &curMap.oob.vel, sizeof(VEC2Df), (LPDWORD)&uiBytes, NULL);
      WriteFile(hMapData, &curMap.oob.temp, sizeof(float), (LPDWORD)&uiBytes, NULL);
      WriteFile(hMapData, &curMap.oob.rad, sizeof(float), (LPDWORD)&uiBytes, NULL);
      WriteFile(hMapData, &curMap.oob.elec, sizeof(float), (LPDWORD)&uiBytes, NULL);
      // Write cell data
      for(ui32 i = 0; i < curMap.desc.mapCells; i++) {
         WriteFile(hMapData, &curMap.cell[i].vel, sizeof(VEC2Df), (LPDWORD)&uiBytes, NULL);
         WriteFile(hMapData, &curMap.cell[i].temp, sizeof(float), (LPDWORD)&uiBytes, NULL);
         WriteFile(hMapData, &curMap.cell[i].rad, sizeof(float), (LPDWORD)&uiBytes, NULL);
         WriteFile(hMapData, &curMap.cell[i].elec, sizeof(float), (LPDWORD)&uiBytes, NULL);
      }
      WriteFile(hMapData, curMap.pDGS, sizeof(CELL_DGS) * curMap.desc.mapCells, (LPDWORD)&uiBytes, NULL);
      WriteFile(hMapData, curMap.pDPS, sizeof(CELL_DPS) * curMap.desc.mapCells, (LPDWORD)&uiBytes, NULL);

      CloseHandle(hMapData);

      return 0;
   }

   // Map's unique descriptor copied to 'md'
   cui32 CreateMap(MAP_DESC &md, si32 mapIndex, csi32 worldIndex, cui8 openElement, cui8 solidElement) {
      si32 i = 0;
      // Find first available slot if mapIndex is -1
      if(mapIndex == -1) for(mapIndex = 0; mapIndex < world[worldIndex].maxMaps && world[worldIndex].map[mapIndex]; mapIndex++);
      // Map slot already occupied, or all slots occupied
      if(mapIndex >= world[worldIndex].maxMaps) return 0x080000001;

      cui32     yStep       = md.mapDim.y;
      cui32     zStep       = md.mapDim.x * yStep;
      csi32     totalCells  = zStep * md.mapDim.z;
      csi32     chunkCells  = md.chunkDim.x * md.chunkDim.y * md.chunkDim.z;
      csi32     totalChunks = totalCells / chunkCells;
      cVEC3Du16 chunkCount  = { ui16(md.mapDim.x / md.chunkDim.x), ui16(md.mapDim.y / md.chunkDim.y), ui16(md.mapDim.z / md.chunkDim.z) };
      csi32     chunkChOS   = chunkCount.x * chunkCount.y;
      csi32     surfaceChOS = md.zso * chunkChOS;
      csi32     solidChOS   = chunkChOS + surfaceChOS;
///- Enable after adding element table management
      //cui8     atlasIndex  = table[elementTable].element[openElement].geometry->ai;
      cui8      atlasIndex  = 0;

      MAP &curMap = *(world[worldIndex].map[mapIndex] = (MAP *)malloc32(sizeof(MAP)));

      curMap.pCB      = (MAPDIMS_ICB *)malloc16(sizeof(MAPDIMS_ICB));
      curMap.pDGS     = (CELL_DGS *)malloc32(sizeof(CELL_DGS) * totalCells);
      curMap.pDPS     = (CELL_DPS *)malloc32(sizeof(CELL_DPS) * totalCells);
      curMap.cell     = (CELL *)malloc32(sizeof(CELL) * totalCells);
      curMap.chunkVis = (ui64ptr)malloc16(ui64(totalChunks + 7) >> 3);
      curMap.chunkMod = (ui64ptr)malloc16(ui64(totalChunks + 7) >> 3);

      curMap.oob.geometry = NULL;
      curMap.oob.pixel    = NULL;
      curMap.oob.vel      = { 0.0f, 0.0f };
      curMap.oob.temp     = -1.0f;
      curMap.oob.rad      = -1.0f;
      curMap.oob.elec     = -1.0f;

      curMap.desc.wlrv.world = worldIndex;
      curMap.desc.wlrv.map   = mapIndex;
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
      curMap.desc.entListDim = md.entListDim;

      CreateSelectionBuffers(worldIndex, mapIndex, 16);
      CreateAssociationBuffer(curMap.desc);

      curMap.pCB->setMapDims(md.mapDim.x - 1, md.mapDim.y - 1, md.mapDim.z - 1);
      curMap.pCB->setChunkDims(md.chunkDim.x - 1, md.chunkDim.y - 1, md.chunkDim.z - 1);
      curMap.pCB->setMapCells(totalCells - 1);
      curMap.pCB->setChunkCells(chunkCells - 1);
      curMap.pCB->setMapChunks(chunkCount.x - 1, chunkCount.y - 1, chunkCount.z - 1);
      curMap.pCB->setFlag(false);
      curMap.pCB->setSpawnOffset(md.zso - 1);
      curMap.pCB->oobCell = {};

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
            curCell.geometry->dens = 0.0f;
            //curCell.geometry->warp = 0.0f;
            curCell.pixel          = &curMap.pDPS[cellIndex];
            curCell.pixel->pmc     = 1.0f;
            curCell.pixel->gtc     = 1.0f;
            curCell.pixel->gev     = 0.0f;
            curCell.pixel->ems     = 1.0f;
            curCell.pixel->nms     = 1.0f;
            curCell.pixel->rms     = 1.0f;
            curCell.pixel->pms     = 1.0f;
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
//            curCell.geometry->et   = { solidElement, 0, 0, 0 };
            curCell.geometry->et   = { ui8(fl32(cellIndex % 1024) / 256.0f + 1.0f), 0, 0, 0 };
            curCell.geometry->er   = { 255, 0, 0, 0 };
            curCell.geometry->end  = { 0, 0, 0, 0 };
            curCell.geometry->dens = 1.0f;
            //curCell.geometry->warp = 0.0f;
            curCell.pixel          = &curMap.pDPS[cellIndex];
            curCell.pixel->pmc     = 1.0f;
            curCell.pixel->gtc     = 1.0f;
            curCell.pixel->gev     = 0.0f;
            curCell.pixel->ems     = 1.0f;
            curCell.pixel->nms     = 1.0f;
            curCell.pixel->rms     = 1.0f;
            curCell.pixel->pms     = 1.0f;
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
            curCell.geometry->dens = 1.01f;
            //curCell.geometry->warp = 0.0f;
            curCell.pixel          = &curMap.pDPS[cellIndex];
            curCell.pixel->pmc     = 1.0f;
            curCell.pixel->gtc     = 1.0f;
            curCell.pixel->gev     = 0.0f;
            curCell.pixel->ems     = 1.0f;
            curCell.pixel->nms     = 1.0f;
            curCell.pixel->rms     = 1.0f;
            curCell.pixel->pms     = 1.0f;
            curCell.pixel->ai      = atlasIndex;
            curCell.vel            = { 0.0f, 0.0f };
            curCell.temp           = 294.15f;
            curCell.rad            = 0.0f;
            curCell.elec           = 0.0f;
         }
      }

      Copy32(&curMap.desc, &md, sizeof(MAP_DESC));

      world[worldIndex].totalMaps++;

      return mapIndex;
   }

   csi32 DestroyMap(si32 worldIndex, si32 mapIndex) {
      // Map slot is empty
      if(!world[worldIndex].map[mapIndex]) return -1;

      mfree(world[worldIndex].map[mapIndex]->chunkMod, world[worldIndex].map[mapIndex]->chunkVis, world[worldIndex].map[mapIndex]->pDPS, world[worldIndex].map[mapIndex]->pDGS,
            world[worldIndex].map[mapIndex]->cell, world[worldIndex].map[mapIndex]->pCB, world[worldIndex].map[mapIndex]->desc.entityList, world[worldIndex].map[mapIndex]->desc.wlrv.cellIndex,
            world[worldIndex].map[mapIndex]->desc.wlrv.entityIndex, world[worldIndex].map[mapIndex]->desc.stInfo, world[worldIndex].map[mapIndex]->desc.stName, world[worldIndex].map[mapIndex]);

      world[worldIndex].map[mapIndex] = 0;

      return world[worldIndex].totalMaps--;
   }

   inline void SetGlobalMapDescriptor(csi32 mapIndex, csi32 worldIndex) const { ptrLib[14] = &world[worldIndex].map[mapIndex]->desc; }

   inline cui32 CalcCellIndex(cVEC3Ds32 coord, csi32 mapIndex, csi32 worldIndex) const {
      cMAP_DESC &curDesc = world[worldIndex].map[mapIndex]->desc;
      cVEC3Ds32  vCell   = { coord.x + (curDesc.mapDim.x >> 1), coord.y + (curDesc.mapDim.y >> 1), coord.z + curDesc.zso };

      if(vCell.x < 0 || vCell.x >= curDesc.mapDim.x) return 0x080000001;
      if(vCell.y < 0 || vCell.y >= curDesc.mapDim.y) return 0x080000001;
      if(vCell.z < 0 || vCell.z >= curDesc.mapDim.z) return 0x080000001;

      cVEC3Du16 vChunkDim         = (VEC3Du16 &)curDesc.chunkDim;
      cui32     uiChunkCells_XY   = vChunkDim.x * vChunkDim.y;
      cui32     uiChunkCells      = curDesc.chunkCells;
      cui32     uiChunkRowCells   = uiChunkCells * curDesc.chunkCount.x;
      cui32     uiChunkPlaneCells = uiChunkRowCells * curDesc.chunkCount.y;

      cui32 uiCellX = (vCell.x & (vChunkDim.x - 1)) + ((vCell.x / vChunkDim.x) * uiChunkCells);
      cui32 uiCellY = ((vCell.y & (vChunkDim.y - 1)) * vChunkDim.x) + ((vCell.y / vChunkDim.y) * uiChunkRowCells);
      cui32 uiCellZ = ((vCell.z & (vChunkDim.z - 1)) * uiChunkCells_XY) + ((vCell.z / vChunkDim.z) * uiChunkPlaneCells);

      cui32 uiCell = uiCellX + uiCellY + uiCellZ;

      return uiCell < curDesc.mapCells ? uiCell : 0x080000001;
   }

   inline cui32 CalcCellIndex_(cVEC3Ds32 coord, csi32 mapIndex, csi32 worldIndex) const {
      cMAP_DESC &curDesc = world[worldIndex].map[mapIndex]->desc;

      if(coord.x < 0 || coord.x >= curDesc.mapDim.x) return 0x80000001;
      if(coord.y < 0 || coord.y >= curDesc.mapDim.y) return 0x80000001;
      if(coord.z < 0 || coord.z >= curDesc.mapDim.z) return 0x80000001;

      cVEC3Du16 vChunkDim         = (VEC3Du16 &)curDesc.chunkDim;
      cui32     uiChunkCells_XY   = vChunkDim.x * vChunkDim.y;
      cui32     uiChunkCells      = curDesc.chunkCells;
      cui32     uiChunkRowCells   = uiChunkCells * curDesc.chunkCount.x;
      cui32     uiChunkPlaneCells = uiChunkRowCells * curDesc.chunkCount.y;

      cui32 uiCellX = (coord.x & (vChunkDim.x - 1)) + ((coord.x / vChunkDim.x) * uiChunkCells);
      cui32 uiCellY = ((coord.y & (vChunkDim.y - 1)) * vChunkDim.x) + ((coord.y / vChunkDim.y) * uiChunkRowCells);
      cui32 uiCellZ = ((coord.z & (vChunkDim.z - 1)) * uiChunkCells_XY) + ((coord.z / vChunkDim.z) * uiChunkPlaneCells);

      cui32 uiCell = uiCellX + uiCellY + uiCellZ;

      return uiCell < curDesc.mapCells ? uiCell : 0x080000001;
   }

   inline cui32 CalcChunkIndex(cVEC3Ds32 coord, csi32 mapIndex, csi32 worldIndex) const {
      cVEC3Du16 &curCount = world[worldIndex].map[mapIndex]->desc.chunkCount;
      cVEC3Du32  chunk    = { ui32(coord.x + si32(curCount.x >> 1u)), ui32(coord.y + si32(curCount.y >> 1u)), ui32(coord.z + si32(curCount.z >> 1u)) };

      if(chunk.x < 0 || chunk.x >= curCount.x) return 0x080000001;
      if(chunk.y < 0 || chunk.y >= curCount.y) return 0x080000001;
      if(chunk.z < 0 || chunk.z >= curCount.z) return 0x080000001;

      cui32 uiChunks[3] = { chunk.x, chunk.y * curCount.x, chunk.z * (curCount.x * curCount.y) };

      cui32 uiChunk = uiChunks[0] + uiChunks[1] + uiChunks[2];

      return uiChunk < si32(world[worldIndex].map[mapIndex]->desc.mapChunks) ? uiChunk : 0x080000001;
   }


   inline cui32 CalcChunkIndex_(cVEC3Ds32 coord, csi32 mapIndex, csi32 worldIndex) const {
      cVEC3Du16 &curCount = world[worldIndex].map[mapIndex]->desc.chunkCount;

      if(coord.x < 0 || coord.x >= curCount.x) return 0x080000001;
      if(coord.y < 0 || coord.y >= curCount.y) return 0x080000001;
      if(coord.z < 0 || coord.z >= curCount.z) return 0x080000001;

      csi32 siChunks[3] = { coord.x, coord.y * curCount.x, coord.z * (curCount.x * curCount.y) };

      csi32 siChunk = siChunks[0] + siChunks[1] + siChunks[2];

      return siChunk < si32(world[worldIndex].map[mapIndex]->desc.mapChunks) ? siChunk : 0x080000001;
   }

   // Rewrite for automated SIMD selection during compilation
   inline cbool CalcQuadCellIndices(SSE4Ds32 &results, cVEC3Ds32 *coord, csi32 mapIndex, csi32 worldIndex) const {
      static ui128 divq, divr;

      cMAP_DESC &curDesc = world[worldIndex].map[mapIndex]->desc;

      cSSE4Ds32 vCell[3] = { _mm_add_epi32(ui128{.m128i_i32 = { coord[0].x, coord[1].x, coord[2].x, coord[3].x }}, _mm_set1_epi32(curDesc.mapDim.x >> 1)),
                             _mm_add_epi32(ui128{.m128i_i32 = { coord[0].y, coord[1].y, coord[2].y, coord[3].y }}, _mm_set1_epi32(curDesc.mapDim.y >> 1)),
                             _mm_add_epi32(ui128{.m128i_i32 = { coord[0].z, coord[1].z, coord[2].z, coord[3].z }}, _mm_set1_epi32(curDesc.zso)) };

      // If out of bounds
      results.x = results.y = results.z = results.w = -1;
      if(vCell[0].x < 0 || vCell[0].x >= curDesc.mapDim.x) return false;
      if(vCell[1].x < 0 || vCell[1].x >= curDesc.mapDim.y) return false;
      if(vCell[2].x < 0 || vCell[2].x >= curDesc.mapDim.z) return false;

      cSSE4Du32 vChunkDim[3] = { _mm_set1_epi32(curDesc.chunkDim.x), _mm_set1_epi32(curDesc.chunkDim.y), _mm_set1_epi32(curDesc.chunkDim.z) };
      cui32     uiChunkCells_XY   = vChunkDim[0].x * vChunkDim[1].x;
      cui32     uiChunkCells      = curDesc.chunkCells;
      cui32     uiChunkRowCells   = uiChunkCells * curDesc.chunkCount.x;
      cui32     uiChunkPlaneCells = uiChunkRowCells * curDesc.chunkCount.y;

      divq = _mm_divrem_epu32(&divr, vCell[0].xmm, vChunkDim[0].xmm);
      cui128 uiCellX = _mm_add_epi32(divr, _mm_mullo_epi32(divq, _mm_set1_epi32(uiChunkCells)));
      divq = _mm_divrem_epu32(&divr, vCell[1].xmm, vChunkDim[1].xmm);
      cui128 uiCellY = _mm_add_epi32(_mm_mullo_epi32(divr, vChunkDim[0].xmm), _mm_mullo_epi32(divq, _mm_set1_epi32(uiChunkRowCells)));
      divq = _mm_divrem_epu32(&divr, vCell[2].xmm, vChunkDim[2].xmm);
      cui128 uiCellZ = _mm_add_epi32(_mm_mullo_epi32(divr, _mm_set1_epi32(uiChunkCells_XY)), _mm_mullo_epi32(divq, _mm_set1_epi32(uiChunkPlaneCells)));

      cSSE4Du32 uiCell = { _mm_add_epi32(_mm_add_epi32(uiCellX, uiCellY), uiCellZ) };

      if(uiCell.x < curDesc.mapCells) {
         results.x = uiCell.x;
         if(vCell[0].y >= 0) results.y = uiCell.y;
         if(vCell[1].z >= 0) results.z = uiCell.z;
         if(vCell[0].w >= 0 && vCell[1].w >= 0) results.w = uiCell.w;
         return true;
      }
      return false;
   }

   inline void ModQuadCellDensity(cVEC3Ds32 coord, cfl32 densityMod, csi32 mapIndex, csi32 worldIndex) const {
      static SSE4Ds32 vAbove, vBelow, cell;

      cMAP &curMap = *world[worldIndex].map[mapIndex];

      cVEC3Ds32 minus1    = { coord.x - 1, coord.y - 1, coord.z - 1 };
      cVEC3Ds32 coords[4] = { { coord }, { minus1.x, coord.y, coord.z }, { coord.x, minus1.y, coord.z }, { minus1.x, minus1.y, coord.z } };
//      cVEC3Ds32 above[4]  = { { coord.x, coord.y, minus1.z }, { minus1.x, coord.y, minus1.z }, { coord.x, minus1.y, minus1.z }, { minus1.x, minus1.y, minus1.z } };
      cVEC3Ds32 below[4]  = { { coord.x, coord.y, coord.z + 1 }, { minus1.x, coord.y, coord.z + 1 }, { coord.x, minus1.y, coord.z + 1 }, { minus1.x, minus1.y, coord.z + 1 } };

//      CalcQuadCellIndices(vAbove, above, worldIndex, mapIndex);
      CalcQuadCellIndices(vBelow, below, worldIndex, mapIndex);
      if(CalcQuadCellIndices(cell, coords, worldIndex, mapIndex))
         for(ui8 i = 0; i < 4; i++) {
            if(cell._si32[i] == -1) continue;
            cfl32 fDensity     = curMap.cell[cell._si32[i]].geometry->dens + densityMod;
            csi32 chunkIndex   = (cell._si32[i] / curMap.desc.chunkCells);
            cfl32 densityBelow = (vBelow._si32[i] != -1 ? curMap.cell[vBelow._si32[i]].geometry->dens : 0.0f);
            if(fDensity < 0.0f) {
               curMap.cell[cell._si32[i]].geometry->dens = 0;
               //map[mapIndex][cell[i]].geometry->et.x = 0;
               if(vBelow._si32[i] != -1) {
                  if(densityBelow > 1.0f) {
                     curMap.cell[vBelow._si32[i]].geometry->dens = 1.0f;
                     csi32 chunkIndexB = (vBelow._si32[i] / curMap.desc.chunkCells);
                     curMap.chunkMod[chunkIndexB >> 6] |= ui64(0x01) << (chunkIndexB & 0x03F);
                  }
               }
            } else if(fDensity > 1.0f) {
//               if(vAbove._si32[i] != -1) {
//                  map[mapIndex][cell[i]].geometry->dens = 1.01f;
//                  if(cui16 densityAbove = map[mapIndex][vAbove._si32[i]].geometry->dens > 0.0f) {
//                     map[mapIndex][vAbove._si32[i]].geometry->et.x = map[mapIndex][cell[i]].geometry->et.x;
//                     map[mapIndex][vAbove._si32[i]].geometry->er.x = 255;
//                  }
//                  csi32 chunkIndexA = (vAbove._si32[i] / world[mapIndex]->desc.totalCells.x);
//                  world[mapIndex]->chunkMod[chunkIndexA >> 6] |= ui64(0x01) << (chunkIndexA & 0x03F);
//               } else
                  curMap.cell[cell._si32[i]].geometry->dens = 1.0f;
            } else {
               if(curMap.cell[cell._si32[i]].geometry->dens == 0.0f && curMap.cell[cell._si32[i]].geometry->et.x == 0) {
                  curMap.cell[cell._si32[i]].geometry->et.x = (vBelow._si32[i] != -1 ? curMap.cell[vBelow._si32[i]].geometry->et.x : 1);
                  curMap.cell[cell._si32[i]].geometry->er.x = 255;
               }
               curMap.cell[cell._si32[i]].geometry->dens = fDensity;
               if(densityBelow == 1.0f) {
                  curMap.cell[vBelow._si32[i]].geometry->dens = 1.01f;
                  csi32 chunkIndexB = (vBelow._si32[i] / curMap.desc.chunkCells);
                  curMap.chunkMod[chunkIndexB >> 6] |= ui64(0x01) << (chunkIndexB & 0x03F);
               }
            }
            curMap.chunkMod[chunkIndex >> 6] |= ui64(0x01) << (chunkIndex & 0x03F);

            /// --- Debugging purposes ---///
            if((i == 0) && (cell._si32[0] != 0x080000001)) ((MAP_DESC &)ptrLib[14]).RES32 = cell._si32[0];
            /// --- Debugging purposes ---///
         }
   }

   // Fill cell list with the index of every cell the line between endPoints touches
   inline void PopulateCellList(const AVX8Df32 &endPoints, csi32 mapIndex, csi32 worldIndex) const {
      MAP &curMap = *world[worldIndex].map[mapIndex];

      si32ptrc cellList = curMap.desc.wlrv.cellIndex;

      SSE4Df32 coord     = { .xmm = endPoints.xmm0 };
      SSE4Df32 tempCoord = {};
      SSE4Ds32 cellCoord = {};

      cSSE4Df32 diff    = { .xmm = _mm_sub_ps(endPoints.xmm1, endPoints.xmm0) };
      cVEC2Df   delta   = { diff.vector.x / diff.vector.z, diff.vector.y / diff.vector.z };
      cVEC2Df   stepDir = { (delta.x >= 0 ? 1.0f : -1.0f), (delta.y >= 0 ? 1.0f : -1.0f) };

      for(si32 &index = curMap.desc.wlrv.cellCount = 0; coord.vector.z < endPoints.vector.y.z; index++, coord.vector.z++) {
         tempCoord         = coord;
         cellCoord         = { .xmm = _mm_cvtps_epi32(coord.xmm) };
         cellList[index++] = CalcCellIndex((VEC3Ds32 &)cellCoord, worldIndex, mapIndex);

         for(coord.vector.x += delta.x; floor(coord.vector.x) != floor(tempCoord.vector.x); tempCoord.vector.x += stepDir.x)
            cellList[index++] = CalcCellIndex((VEC3Ds32 &)_mm_cvtps_epi32(coord.xmm), worldIndex, mapIndex);
         for(coord.vector.y += delta.y; floor(coord.vector.y) != floor(tempCoord.vector.y); tempCoord.vector.y += stepDir.y)
            cellList[index++] = CalcCellIndex((VEC3Ds32 &)_mm_cvtps_epi32(coord.xmm), worldIndex, mapIndex);
      }
   }

   private : inline void Cull_Nonvisible_and_Unchanged(ptr threadData) {
      CLASS_CAM &camMan = *(CLASS_CAM *)ptrLib[5];

      cMMTDcptrc  data[2] = { (cMMTDcptrc)threadData, (cMMTDcptrc)threadData + 1 };
      cMAPptrc    map     = data[0]->map;

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

      camMan.SetDimsf(map->desc.chunkDim, map->desc.chunkCount, 0);

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
               cui8 visible = camMan.ChunkFrustumIntersect8(chunkOS, 0);

               for(i = 0; i < 8; i++, chunkOS.vector.x++)
                  if(visible & 0x01 << i) {
                     csi32 chunkIndex = CalcChunkIndex((cVEC3Ds32 &)chunkOS, 0, 0);
                     cui32 QWordOS    = chunkIndex >> 6;
                     cui64 bitOS      = ui64(0x01) << (chunkIndex & 0x03F);
                     cfl32 distance = camMan.DistanceFromCamera(chunkOS.xmm, 0);

                     // Change hard limits to LOD scalars
                     if(map->chunkVis[QWordOS] & bitOS) {
                        //                        if(distance < 128.0f)
                        nearCells[nearCount++] = chunkIndex;
                        //                        else if(distance < 512.0f)
                        //                           medCells[medCount++] = chunkIndex;
                        //                        else if(distance < 2048.0f)
                        //                           farCells[farCount++] = chunkIndex;
                     }
                  }
            }
         }
      }

      MAPMAN_THREAD_STATUS.m128i_u64[0] = (modCount << 34) | (nearCount << 4);
      MAPMAN_THREAD_STATUS.m128i_u64[1] = medCount | (farCount << 30);
   }

   public : inline si32 Cull(ui32ptrptrc arrayVisible, ui32ptrc arrayUnchanged, csi32 mapIndex, csi32 worldIndex, csi8 threadCount) {
      static ui8 uiTHREADS = 0;

      if(!arrayVisible) { MAPMAN_THREAD_STATUS.m128i_u8[0] &= 0x0F3; return 0; }

      cui8 threadBits = MAPMAN_THREAD_STATUS.m128i_u8[0];

      if(!(threadBits & 0x01)) threadData[0] = { world[worldIndex].map[mapIndex], arrayVisible };
      if(!(threadBits & 0x02)) threadData[1] = { world[worldIndex].map[mapIndex], arrayUnchanged };

      ///- Stall/skip? if status if 'busy'
      while(MAPMAN_THREAD_STATUS.m128i_u8[0] & 0x03) _mm_pause(); //Sleep(1);
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

   inline void WaitForCulling(ui128 &results, cDWORD sleepDelay) const {
      if(sleepDelay) while(MAPMAN_THREAD_STATUS.m128i_u8[0] & 0x03) Sleep(sleepDelay);
      else while(MAPMAN_THREAD_STATUS.m128i_u8[0] & 0x03) _mm_pause();

      cVEC2Du64 mapManThreadData = (cVEC2Du64 &)MAPMAN_THREAD_STATUS;

      MAPMAN_THREAD_STATUS.m128i_u64[0] &= 0x0C;
      MAPMAN_THREAD_STATUS.m128i_u64[1] = 0;

      results = { .m128i_u32 = { ui32(mapManThreadData.x >> 4) & 0x03FFFFFFF, ui32(mapManThreadData.x >> 34) & 0x03FFFFFFF,
                                 ui32(mapManThreadData.y) & 0x03FFFFFFF, ui32(mapManThreadData.y >> 30) & 0x03FFFFFFF } };
   }
};

/*inline static void _Cull_Nonvisible_RasteriseLayer(cVEC4Ds32 lines[2]) {
   extern CLASS_MAPMAN &mapMan;
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
   CLASS_CAM    &camMan = *(CLASS_CAM *)ptrLib[5];
   CLASS_MAPMAN &mapMan = *(CLASS_MAPMAN *)ptrLib[6];

   cMMTDptr  data       = (cMMTDptr)threadData;
   cVEC2Ds32 vScrDim    = { si32(ScrRes.dims[ScrRes.state].w) - 1, si32(ScrRes.dims[ScrRes.state].h) - 1 };
//   cSSE4Ds32 chunkCount = { .vector = { data->map->pCB->chunkCount.x, data->map->pCB->chunkCount.y,
//                                        data->map->pCB->chunkCount.z, data->map->desc.mapChunks } };
   cSSE4Du32 chunkCount = { .vector = { data->map->desc.chunkCells, data->map->desc.mapCells,
                                        data->map->desc.chunkCount.z, data->map->desc.mapChunks } };
   cSSE4Ds32 mapDim     = { .vector = { data->map->desc.mapDim.x >> 1, data->map->desc.mapDim.y >> 1, data->map->desc.mapDim.z >> 1, 1 } };
   cSSE4Ds32 chunkDim   = { .vector = { data->map->desc.chunkDim.x, data->map->desc.chunkDim.y, data->map->desc.chunkDim.z, 1 } };

   SSE4Du32 coords[2];   // Change to AVX8Ds32
   SSE4Du32 vChunk[8];
   ui64     chunks;
   si32     i;

   MAPMAN_THREAD_STATUS.m128i_u8[0] |= 0x04;

   do {
      ///- Stall/skip? if status if 'busy'
      while(!(MAPMAN_THREAD_STATUS.m128i_u8[0] & 0x01)) _mm_pause(); //Sleep(1);

      chunks = 0;

      // Calculate top- & bottom-most cell indices at screen corners
      cfl32x4 camOffset = { 0.0f, 0.0f, -4.0f, 0.0f };
      csi32   zOS[2]    = { 0 - mapDim.vector.z, mapDim.vector.z - 1 };

      (VEC2Ds32 &)vChunk[0] = camMan.CursorLayerIntersect(zOS[0], camOffset, VEC2Ds32{0, vScrDim.y}, 0, 0);
      (VEC2Ds32 &)vChunk[1] = camMan.CursorLayerIntersect(zOS[0], camOffset, vScrDim, 0, 0);
      (VEC2Ds32 &)vChunk[2] = camMan.CursorLayerIntersect(zOS[0], camOffset, VEC2Ds32{0, 0}, 0, 0);
      (VEC2Ds32 &)vChunk[3] = camMan.CursorLayerIntersect(zOS[0], camOffset, VEC2Ds32{vScrDim.x, 0}, 0, 0);
      (VEC2Ds32 &)vChunk[4] = camMan.CursorLayerIntersect(zOS[1], camOffset, VEC2Ds32{0, vScrDim.y}, 0, 0);
      (VEC2Ds32 &)vChunk[5] = camMan.CursorLayerIntersect(zOS[1], camOffset, vScrDim, 0, 0);
      (VEC2Ds32 &)vChunk[6] = camMan.CursorLayerIntersect(zOS[1], camOffset, VEC2Ds32{0, 0}, 0, 0);
      (VEC2Ds32 &)vChunk[7] = camMan.CursorLayerIntersect(zOS[1], camOffset, VEC2Ds32{vScrDim.x, 0}, 0, 0);

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
      cui32     lengths[2] = { coords[0].vector.x2 - coords[0].vector.x1 + 1u, coords[1].vector.x2 - coords[1].vector.x1 + 1u };
      cAVX8Du32 lines[2] = { { .vector = { coords[0].vector.x1, coords[0].vector.y1, vChunk[0].vector.z, lengths[0],
                                           coords[0].vector.x2, coords[0].vector.y2, vChunk[0].vector.z, lengths[0] } },
                             { .vector = { coords[1].vector.x1, coords[1].vector.y1, vChunk[4].vector.z, lengths[1],
                                           coords[1].vector.x2, coords[1].vector.y2, vChunk[4].vector.z, lengths[1] } } };
      // Rasterise prism
      AVX8Df32  accum     = (cAVX8Df32 &)_mm256_cvtepi32_ps((ui256 &)lines);
      AVX8Du32  line      = lines[0];
      cAVX8Du32 sCols     = { .xmm = { _mm_broadcastd_epi32(ui128{.m128i_u32 = lines[1].vector.x.z - lines[0].vector.x.z + 1}),
                                       _mm_broadcastd_epi32(ui128{.m128i_u32 = lines[1].vector.y.z - lines[0].vector.y.z + 1}) } };
      cAVX8Df32 fCols     = { .ymm = _mm256_cvtepi32_ps(sCols.ymm) };
      cAVX8Du32 xyDelta   = { .ymm = _mm256_sub_epi32(lines[1].ymm, lines[0].ymm) };
      cAVX8Du32 signTest  = { .ymm = _mm256_sign_epi32(ones32x8, xyDelta.ymm)};
      cAVX8Du32 signMod   = { .ymm = _mm256_or_epi32(signTest.ymm, ones32x8) };
      cAVX8Du32 sPredelta = { .ymm = _mm256_add_epi32(signMod.ymm, xyDelta.ymm) };
      cAVX8Df32 fPredelta = { .ymm = _mm256_cvtepi32_ps(sPredelta.ymm) };
      cAVX8Df32 delta     = { .ymm = _mm256_div_ps(fPredelta.ymm, fCols.ymm) };

      while(line.vector.x.z <= lines[1].vector.x.z) {
         // Rasterise slice
         for(SSE4Du32 lineX = { .xmm = line.xmm[0] }; lineX.vector.y <= line.vector.y.y; lineX.vector.x = line.vector.x.x, lineX.vector.y++)
            for(cui32 endPoint = lineX.vector.x + lineX.vector.w; lineX.vector.x < endPoint; lineX.vector.x++) {
               cui32 chunkIndex = mapMan.CalcChunkIndex_((cVEC3Ds32 &)lineX, 0, 0);
               cui32 QWordOS    = chunkIndex >> 6;
               cui64 bitOS      = ui64(0x01) << (chunkIndex & 0x03F);

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
   CLASS_CAM    &camMan = *(CLASS_CAM *)ptrLib[5];
   CLASS_MAPMAN &mapMan = *(CLASS_MAPMAN *)ptrLib[6];

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

//   MAPMAN_THREAD_STATUS.m128i_u8[0] &= 0x0FFFFFFFC0000000F;
   MAPMAN_THREAD_STATUS.m128i_u8[0] |= 0x04;

   do {
      ///- Stall/skip? if status if 'busy'
      while(!(MAPMAN_THREAD_STATUS.m128i_u8[0] & 0x01)) _mm_pause(); //Sleep(1);

      camMan.SetDimsf(data->map->desc.chunkDim, data->map->desc.chunkCount, 0);

      nearCount = medCount = farCount = 0;

      // Calculate each chunk's distance-to-camera and reject out-of-view chunks
      // Sort nearest-to-furthest into L.O.D. lists for input assembler
      for(chunkOS.vector.z = mapChunksL.vector.z; chunkOS.vector.z < mapChunksH.vector.z; chunkOS.vector.z++) {
         for(chunkOS.vector.y = mapChunksL.vector.y; chunkOS.vector.y < mapChunksH.vector.y; chunkOS.vector.y++) {
            for(chunkOS.vector.x = mapChunksL.vector.x; chunkOS.vector.x < mapChunksH.vector.x;) {
///--- ??? Swap order of tests ???
               //cSSE4Ds32 cell2 = { .vector = { chunkOS.vector.x + 1, chunkOS.vector.y, chunkOS.vector.z } };
               //cAVX8Ds32 cells = { .xmm = { chunkOS.xmm, cell2.xmm } };
               cui8 visible = camMan.ChunkFrustumIntersect8(chunkOS, 0);

               for(i = 0; i < 8; i++, chunkOS.vector.x++)
                  if(visible & 0x01 << i) {
                     //cSSE4Ds32 coord      = { .vector = { chunkOS.vector.x, chunkOS.vector.y, chunkOS.vector.z } };
                     csi32     chunkIndex = mapMan.CalcChunkIndex((cVEC3Ds32 &)chunkOS, 0, 0);
                     cui32     QWordOS    = chunkIndex >> 6;
                     cui64     bitOS      = ui64(0x01) << (chunkIndex & 0x03F);

                     cfl32 distance = camMan.DistanceFromCamera(chunkOS.xmm, 0);
                     // Change hard limits to LOD scalars
                     if(data->map->chunkVis[QWordOS] & bitOS) {
//                        if(distance < 128.0f)
                           nearCells[nearCount++] = chunkIndex;
//                        else if(distance < 512.0f)
//                           medCells[medCount++] = chunkIndex;
//                        else if(distance < 2048.0f)
//                           farCells[farCount++] = chunkIndex;
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
   cMMTDcptrc data       = (cMMTDcptrc)threadData;
   cui32      chunkCount = data->map->desc.mapChunks;

   al16 ui64 j;
        ui32 i;

//   MAPMAN_THREAD_STATUS &= 0x03FFFFFFFF;
   MAPMAN_THREAD_STATUS.m128i_u8[0] |= 0x08;

   do {
      ///- Stall/skip? if status if 'busy'
      while(!(MAPMAN_THREAD_STATUS.m128i_u8[0] & 0x02)) _mm_pause(); //Sleep(1);

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

   CLASS_CAM    &camMan = *(CLASS_CAM *)ptrLib[5];
   CLASS_MAPMAN &mapMan = *(CLASS_MAPMAN *)ptrLib[6];

   cMMTDcptrc  data[2] = { (cMMTDcptrc)threadData, (cMMTDcptrc)threadData + 1 };
   MAP        *map     = data[0]->map;

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
      ///- Stall/skip? if status if 'busy'
      while(!(MAPMAN_THREAD_STATUS.m128i_u8[0] & 0x03)) _mm_pause(); //Sleep(1);

      QueryPerformanceCounter((LARGE_INTEGER *)&startTics);

      camMan.SetDimsf(map->desc.chunkDim, map->desc.chunkCount, 0);

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
               cui8 visible = camMan.ChunkFrustumIntersect8(chunkOS, 0);

               for(i = 0; i < 8; i++, chunkOS.vector.x++)
                  if(visible & (0x01 << i)) {
                     csi32 chunkIndex = mapMan.CalcChunkIndex((cVEC3Ds32 &)chunkOS, 0, 0);
                     cui32 QWordOS    = chunkIndex >> 6;
                     cui64 bitOS      = (ui64)0x01 << (chunkIndex & 0x03F);

                     cfl32 distance = camMan.DistanceFromCamera(chunkOS.xmm, 0);
                     // Change hard limits to LOD scalars
                     if(map->chunkVis[QWordOS] & bitOS) {
//                        if(distance < 128.0f)
                           nearCells[nearCount++] = chunkIndex;
//                        else if(distance < 512.0f)
//                           medCells[medCount++] = chunkIndex;
//                        else if(distance < 2048.0f)
//                           farCells[farCount++] = chunkIndex;
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
