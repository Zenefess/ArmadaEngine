/**************************************************************  
 * File: D3D11 helper functions.h         Created: 2023/05/31 *
 *                                  Last modified: 2024/06/15 *
 *                                                            *
 * Desc:                                                      *
 *                                                            *
 *   Copyright (c) David William Bull. All rights reserved.   *
 **************************************************************/
#pragma once

#include "pch.h"
#include "typedefs.h"
#include "Data structures.h"
#include "File operations.h"
#include "Direct3D11 thread.h"
#include "Direct3D11 functions.h"
#include "Direct3D11 functions/class_buffers.h"
#include "Armada Intelligence/class_mapmanager.h"
#include "Armada Intelligence/class_entitymanager.h"

al16 struct HELPFUNC_MAP {
   CLASS_GPU    &gpu;
   CLASS_MAPMAN &man;

   ui32ptr (*visBuf)[3];  // [MAX_WORLDS][3]
   ui32ptr  *modBuf;      // [MAX_WORLDS]
   si32    (*gpuBuf)[5];  // [MAX_WORLDS][5]
   si32    (*vertBuf)[3]; // [MAX_WORLDS][3]

   ui8      levelsOfDetail = 0;
   SSE4Du32 mapManThreadData {};

   HELPFUNC_MAP(CLASS_GPU &gpuClass, CLASS_MAPMAN &mapMan) : gpu(gpuClass), man(mapMan) {
//      gpuBuf  = (si32 (*)[5])zalloc32(sizeof(si32[5]) * MAX_WORLDS); // Correct allocations? ptrs not correct?
//      vertBuf = (si32 (*)[3])zalloc32(sizeof(si32[3]) * MAX_WORLDS);
//      visBuf  = (ui32ptr (*)[3])zalloc32(sizeof(ui32 (*)[3]) * MAX_WORLDS);
//      modBuf  = (ui32ptr *)zalloc32(sizeof(ui32ptr) * MAX_WORLDS);
      visBuf  = (ui32ptr (*)[3])zalloc32(sizeof(ui32[MAX_WORLDS][3]));
      modBuf  = (ui32ptr *)zalloc32(sizeof(ui32ptr[MAX_WORLDS]));
      gpuBuf  = (si32 (*)[5])zalloc32(sizeof(si32[MAX_WORLDS][5]));
      vertBuf = (si32 (*)[3])zalloc32(sizeof(si32[MAX_WORLDS][3]));
   }

   ~HELPFUNC_MAP(void) { mfree(modBuf, visBuf, vertBuf, gpuBuf); }

   void CreateBuffers(csi16 worldIndex, csi16 mapIndex, csi16 elementTableIndex) {
      cMAP        &map   = *man.world[worldIndex].map[mapIndex];
      cELEM_TABLE &table = man.table[elementTableIndex];
      csi32        count = map.desc.mapChunks;

      for(ui32 i = 0; i <= levelsOfDetail; i++) {
         csi32 subcount = count >> (levelsOfDetail - i);

         visBuf[worldIndex][i]  = (ui32ptr)zalloc16(sizeof(ui32) * subcount);
         vertBuf[worldIndex][i] = gpu.buf.CreateVertex(visBuf[worldIndex][i], sizeof(ui32), subcount, 1);
      }
      modBuf[worldIndex] = (ui32ptr)zalloc16(sizeof(ui32) * count);

      cVEC3Du32 mapDim = { ui32(map.desc.mapDim.x), ui32(map.desc.mapDim.y), ui32(map.desc.mapDim.z) };
      ui32ptrc relIndices = (ui32ptrc)malloc16(map.desc.mapCells * sizeof(ui32));
      for(VEC4Ds32 co = {}; co.z < mapDim.z; ++co.z)
         for(co.y = 0; co.y < mapDim.y; ++co.y)
            for(co.x = 0; co.x < mapDim.x; ++co.x)
               relIndices[co.w++] = man.CalcCellIndex_((VEC3Ds32 &)co, mapIndex, worldIndex);

///--- Group all mapDims_iCB and elements_iGS into master arrays and use offsets in shaders
      gpuBuf[worldIndex][0] = gpu.buf.CreateConstant(0, map.pCB, sizeof(MAPDIMS_ICB), man.world[worldIndex].totalMaps, 2, STAGES_VERTEX_GEOMETRY, true);
      gpuBuf[worldIndex][1] = gpu.buf.CreateStructured(0, table.pIGS, sizeof(ELEM_IGS), table.numElements, USAGE_IMMUTABLE);
      gpuBuf[worldIndex][2] = gpu.buf.CreateStructured(0, relIndices, sizeof(ui32),     map.desc.mapCells, USAGE_IMMUTABLE);
      gpuBuf[worldIndex][3] = gpu.buf.CreateStructured(0, map.pDGS,   sizeof(CELL_DGS), map.desc.mapCells, USAGE_DYNAMIC);
      gpuBuf[worldIndex][4] = gpu.buf.CreateStructured(0, map.pDPS,   sizeof(CELL_DPS), map.desc.mapCells, USAGE_DYNAMIC);

      mfree1(relIndices);
   }

   inline void StartViewCulling(csi32 worldIndex, csi32 mapIndex) const { man.Cull(visBuf[worldIndex], modBuf[worldIndex], worldIndex, mapIndex, 1); }

///--- !!! Expand to 7 LODs !!!
   // Returns counts for { Chunks uploaded, LOD 0 chunks, LOD 1 chunks, LOD 2 chunks }
   cSSE4Du32 WaitThenUploadAndRender(csi16 worldIndex, csi16 mapIndex) {
      cMAP &map = *man.world[worldIndex].map[mapIndex];
      ui32  i;

      // Wait until visibility & modification culling completes
      mapManThreadData = man.WaitForCulling(0);

      // Upload modified cell data
      CELL_DGSptr cellGeoData = (CELL_DGS *)gpu.buf.LockStructuredBeforeUpdate(0, gpuBuf[worldIndex][3]);
      CELL_DPSptr cellPixData = (CELL_DPS *)gpu.buf.LockStructuredBeforeUpdate(0, gpuBuf[worldIndex][4]);
      
      cui64 chunkDGS = sizeof(CELL_DGS) * map.desc.chunkCells;
      cui64 chunkDPS = sizeof(CELL_DPS) * map.desc.chunkCells;

      for(i = 0; i < mapManThreadData.w; i++) {
         csi32 cellIndex = modBuf[worldIndex][i] * map.desc.chunkCells;

         Stream16(&map.pDGS[cellIndex], &cellGeoData[cellIndex], chunkDGS);
         Stream16(&map.pDPS[cellIndex], &cellPixData[cellIndex], chunkDPS);
      }
      gpu.buf.UnlockStructuredAfterUpdate(0, gpuBuf[worldIndex][3]);
      gpu.buf.UnlockStructuredAfterUpdate(0, gpuBuf[worldIndex][4]);

      // Render map
      gpu.devcon[0]->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
      gpu.cfg.SetBlendState(0, 0);
      gpu.cfg.SetDepthStencilState(0, 3, 0);
      gpu.cfg.SetVertexFormat(0, 0);
      gpu.buf.SetGSR(0, gpuBuf[worldIndex][1], 0, 3);
      gpu.buf.SetPSR(0, gpuBuf[worldIndex][4], 0, 1);

      for(i = 0; i <= levelsOfDetail; i++) {
         gpu.buf.SetVertex(0, vertBuf[worldIndex][i], 0);
//         gpu.buf.UpdateVertex(0, visBuf[worldIndex][i], vertBuf[worldIndex][i], 1);
         gpu.buf.UpdateVertex(0, vertBuf[worldIndex][i], mapManThreadData._ui32[i], visBuf[worldIndex][i]);
         gpu.cfg.SetShaderGroup(0, i);
         gpu.devcon[0]->DrawInstanced(map.desc.chunkCells, mapManThreadData._ui32[i], 0, 0); // For use with ?s.map.cells.hlsl
//         gpu.devcon[0]->DrawInstanced(map.desc.chunkCells >> 3, mapManThreadData._ui32[i], 0, 0); // For use with ?s.map.cells.x8.hlsl
         sysData.gpu.total.drawCalls++;
      }

      return mapManThreadData;
   }

   inline void DestroyBuffers(csi16 worldIndex, csi16 mapIndex) {

   }
};

al16 struct HELPFUNC_ENT {
   CLASS_GPU    &gpu;
   CLASS_ENTMAN &man;

   ui32ptr (*visBuf)[3];
   ui32ptr  *modBuf;
   si32    (*gpuBuf)[4];
   si32    (*vertBuf)[3];

   SSE4Du32 entManThreadData {};
   ui8      levelsOfDetail = 0;

   HELPFUNC_ENT(CLASS_GPU &gpuClass, CLASS_ENTMAN &entMan) : gpu(gpuClass), man(entMan) {
      gpuBuf  = (si32 (*)[4])zalloc32(sizeof(si32[4]) * MAX_ENTITY_GROUPS);
      vertBuf = (si32 (*)[3])zalloc32(sizeof(si32[3]) * MAX_ENTITY_GROUPS);
      visBuf  = (ui32ptr (*)[3])zalloc32(sizeof(ui32 (*)[3]) * MAX_ENTITY_GROUPS);
      modBuf  = (ui32ptrptr)zalloc32(sizeof(ui32ptr) * MAX_ENTITY_GROUPS);
   }

   ~HELPFUNC_ENT() { mfree(modBuf, visBuf, vertBuf, gpuBuf); }

   void CreateBuffers(csi16 groupIndex) {
      OBJECT_GROUP &objGroup = man.objGroup[groupIndex];
      ENTITY_GROUP &entGroup = man.entGroup[groupIndex];

      cui32 count = (entGroup.totalBones + 31) & 0x0FFFFFFE0;  // Round up to nearest multiple of 32

      for(ui32 i = 0; i <= levelsOfDetail; i++) {
         csi32 subcount = count >> (levelsOfDetail - i);

         visBuf[groupIndex][i]  = (ui32ptr)zalloc16(sizeof(ui32) * subcount);
         vertBuf[groupIndex][i] = gpu.buf.CreateVertex(visBuf[groupIndex][i], sizeof(ui32) * 32, (subcount + 31) >> 5, 1);
      }
      modBuf[groupIndex] = (ui32ptr)zalloc16(sizeof(ui32) * count);

      gpuBuf[groupIndex][0] = gpu.buf.CreateStructured(0, objGroup.object, sizeof(OBJECT_IGS), objGroup.totalObjects, USAGE_IMMUTABLE);
      gpuBuf[groupIndex][1] = gpu.buf.CreateStructured(0, objGroup.part, sizeof(PART_IGS), objGroup.totalParts, USAGE_IMMUTABLE);
      gpuBuf[groupIndex][2] = gpu.buf.CreateStructured(0, entGroup.bone_dgs, sizeof(BONE_DGS), entGroup.totalBone_DGS, USAGE_DYNAMIC);
      gpuBuf[groupIndex][3] = gpu.buf.CreateStructured(0, entGroup.spriteO, sizeof(SPRITE_DPS), entGroup.totalSpritesO, USAGE_DYNAMIC);
   }

   inline void StartViewCulling(csi32 groupIndex) const { man.Cull(visBuf[groupIndex], modBuf[groupIndex], groupIndex, 1); }

   ///--- !!! Expand to 7 LODs !!!
   // Returns counts for { Entities uploaded, LOD 0 entities, LOD 1 entities, LOD 2 entities }
   cSSE4Du32 WaitThenUploadAndRender(csi16 groupIndex) {
      const ENTITY_GROUP &group = man.entGroup[groupIndex];

      ui32 i;

      // Wait until visibility & modification culling completes
      entManThreadData = man.WaitForCulling(0);

      // Upload modified entity data
      BONE_DGSptrc   boneGeoData = (BONE_DGSptr)gpu.buf.LockStructuredBeforeUpdate(0, gpuBuf[groupIndex][2]);
      SPRITE_DPSptrc bonePixData = (SPRITE_DPSptr)gpu.buf.LockStructuredBeforeUpdate(0, gpuBuf[groupIndex][3]);

      for(i = 0; i < entManThreadData.w; i++) {
         csi32 boneIndex = modBuf[groupIndex][i];

         Stream64(&group.bone_dgs[boneIndex], &boneGeoData[boneIndex], sizeof(BONE_DGS));
         Stream16(&group.spriteO[boneIndex], &bonePixData[boneIndex], sizeof(SPRITE_DPS));
      }
      gpu.buf.UnlockStructuredAfterUpdate(0, gpuBuf[groupIndex][2]);
      gpu.buf.UnlockStructuredAfterUpdate(0, gpuBuf[groupIndex][3]);

      // Render entities
      gpu.devcon[0]->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
      gpu.cfg.SetBlendState(0, 0);
      gpu.cfg.SetDepthStencilState(0, 3, 0);
      gpu.cfg.SetVertexFormat(0, 1);
      gpu.buf.SetGSR(0, gpuBuf[groupIndex][0], 0, 3);
      gpu.buf.SetPSR(0, gpuBuf[groupIndex][3], 0, 1);

      for(i = 0; i <= levelsOfDetail; i++) {
         cui32 vertCount = entManThreadData._ui32[i] >> 5;
         gpu.buf.SetVertex(0, vertBuf[groupIndex][i], 0);
//         gpu.buf.UpdateVertex(0, visBuf[groupIndex][i], vertBuf[groupIndex][i], 1);
         gpu.buf.UpdateVertex(0, vertBuf[groupIndex][i], vertCount, visBuf[groupIndex][i]);
         gpu.cfg.SetShaderGroup(0, i + 3);
         gpu.devcon[0]->Draw(vertCount, 0);
         sysData.gpu.total.drawCalls++;
      }

      return entManThreadData;
   }

   inline void DestroyBuffers(csi16 groupIndex) {

   }
};

al32 struct CLASS_D3D11HELPER {
   HELPFUNC_MAP map;
   HELPFUNC_ENT ent;

   CLASS_D3D11HELPER(CLASS_GPU &gpuAddr, CLASS_MAPMAN &mapAddr, CLASS_ENTMAN &entAddr) : map(gpuAddr, mapAddr), ent(gpuAddr, entAddr) {}
};