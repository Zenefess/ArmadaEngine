/**************************************************************  
 * File: D3D11 helper functions.h         Created: 2023/05/31 *
 *                                  Last modified: 2024/07/09 *
 *                                                            *
 * Desc:                                                      *
 *                                                            *
 *   Copyright (c) David William Bull. All rights reserved.   *
 **************************************************************/
#pragma once

#include "master header.h"
#include "typedefs.h"
#include "Data structures.h"
#include "File operations.h"
#include "Direct3D11 thread.h"
#include "Direct3D11 functions.h"
#include "Direct3D11 functions/class_buffers.h"
#include "Armada Intelligence/class_mapmanager.h"
#include "Armada Intelligence/class_entitymanager.h"

al16 struct HELPFUNC_MAP {
   ui128 mapManThreadData = null128;

   CLASS_GPU    &gpu;
   CLASS_MAPMAN &man;

   declare2d64z(ui32ptr, visBuf,  MAX_WORLDS, MAX_MAP_LOD);
   declare1d64z(ui32ptr, modBuf,  MAX_WORLDS);
   declare2d64z(si32,    gpuBuf,  MAX_WORLDS, 5u);
   declare2d64z(si32,    vertBuf, MAX_WORLDS, MAX_MAP_LOD);

   ui8 levelsOfDetail = 0;

   HELPFUNC_MAP(CLASS_GPU &gpuClass, CLASS_MAPMAN &mapMan) : gpu(gpuClass), man(mapMan) {}

   ~HELPFUNC_MAP(void) { mfree(modBuf, visBuf, vertBuf, gpuBuf); }

   void CreateBuffers(csi16 worldIndex, csi16 mapIndex, csi16 elementTableIndex) {
      cMAP        &map   = *man.world[worldIndex].map[mapIndex];
      cELEM_TABLE &table = man.table[elementTableIndex];
      csi32        count = map.desc.mapChunks;

      for(ui32 i = 0; i <= levelsOfDetail; i++) {
         csi32 subcount = count >> (levelsOfDetail - i);

         visBuf[worldIndex][i]  = zalloc1d16(ui32, subcount);
         vertBuf[worldIndex][i] = gpu.buf.CreateVertex(visBuf[worldIndex][i], sizeof(ui32), subcount, 1u);
      }
      modBuf[worldIndex] = zalloc1d16(ui32, count);

      cVEC3Du32 mapDim = { ui32(map.desc.mapDim.x), ui32(map.desc.mapDim.y), ui32(map.desc.mapDim.z) };
      declare1d16(ui32, relIndices, map.desc.mapCells);

      for(VEC4Du32 co = {}; co.z < mapDim.z; ++co.z)
         for(co.y = 0; co.y < mapDim.y; ++co.y)
            for(co.x = 0; co.x < mapDim.x; ++co.x)
               relIndices[co.w++] = man.CalcCellIndex_((VEC3Ds32 &)co, mapIndex, worldIndex);

///--- Group all mapDims_iCB and elements_iGS into master arrays and use offsets in shaders
      gpuBuf[worldIndex][0] = gpu.buf.CreateConstant(0, map.pCB, sizeof(MAPDIMS_ICB), man.world[worldIndex].totalMaps, 3u, ae_stages_vertex_geometry, true);
      gpuBuf[worldIndex][1] = gpu.buf.CreateStructured(0, table.pIGS, sizeof(ELEM_IGS), table.numElements, ae_buf_immutable);
      gpuBuf[worldIndex][2] = gpu.buf.CreateStructured(0, relIndices, sizeof(ui32),     map.desc.mapCells, ae_buf_immutable);
      gpuBuf[worldIndex][3] = gpu.buf.CreateStructured(0, map.pDGS,   sizeof(CELL_DGS), map.desc.mapCells, ae_buf_dynamic);
      gpuBuf[worldIndex][4] = gpu.buf.CreateStructured(0, map.pDPS,   sizeof(CELL_DPS), map.desc.mapCells, ae_buf_dynamic);

      mfree1(relIndices);
   }

   inline void StartViewCulling(csi32 worldIndex, csi32 mapIndex) const { man.Cull(visBuf[worldIndex], modBuf[worldIndex], worldIndex, mapIndex, 1); }

///--- !!! Expand to 7 LODs !!!
   // Returns counts for { Chunks uploaded, LOD 0 chunks, LOD 1 chunks, LOD 2 chunks }
   cui128 WaitThenUploadAndRender(csi16 worldIndex, csi16 mapIndex) {
      cMAP &map = *man.world[worldIndex].map[mapIndex];
      ui32  i;

      // Wait until visibility & modification culling completes
      man.WaitForCulling(mapManThreadData, 0);

      // Upload modified cell data
      CELL_DGSptrc cellGeoData = (CELL_DGSptr)gpu.buf.LockStructuredBeforeUpdate(0, gpuBuf[worldIndex][3]);
      CELL_DPSptrc cellPixData = (CELL_DPSptr)gpu.buf.LockStructuredBeforeUpdate(0, gpuBuf[worldIndex][4]);
      
      cui64 chunkDGS = sizeof(CELL_DGS) * map.desc.chunkCells;
      cui64 chunkDPS = sizeof(CELL_DPS) * map.desc.chunkCells;

      for(i = 0; i < mapManThreadData.m128i_u32[3]; i++) {
         csi32 cellIndex = modBuf[worldIndex][i] * map.desc.chunkCells;

         Stream16(&map.pDGS[cellIndex], &cellGeoData[cellIndex], chunkDGS);
         Stream16(&map.pDPS[cellIndex], &cellPixData[cellIndex], chunkDPS);
      }
      gpu.buf.UnlockStructuredAfterUpdate(0, gpuBuf[worldIndex][3]);
      gpu.buf.UnlockStructuredAfterUpdate(0, gpuBuf[worldIndex][4]);

      // Render map
      cMAP_PARAMS params = { gpuBuf[worldIndex], vertBuf[worldIndex], visBuf[worldIndex], mapManThreadData.m128i_u32, map.desc.chunkCells, 0 };
      gpu.ren.QueueDrawMap(params);

      return mapManThreadData;
   }

   inline void DestroyBuffers(csi16 worldIndex, csi16 mapIndex) {

   }
};

al16 struct HELPFUNC_ENT {
   ui128 entManThreadData;

   CLASS_GPU    &gpu;
   CLASS_ENTMAN &man;

   declare2d64z(ui32ptr, visBuf,  MAX_ENTITY_GROUPS, MAX_ENT_LOD);
   declare1d64z(ui32ptr, modBuf,  MAX_ENTITY_GROUPS);
   declare2d64z(si32,    gpuBuf,  MAX_ENTITY_GROUPS, 4u);
   declare2d64z(si32,    vertBuf, MAX_ENTITY_GROUPS, MAX_ENT_LOD);

   ui8 levelsOfDetail = 0;

   HELPFUNC_ENT(CLASS_GPU &gpuClass, CLASS_ENTMAN &entMan) : gpu(gpuClass), man(entMan) {}

   ~HELPFUNC_ENT() { mfree(modBuf, visBuf, vertBuf, gpuBuf); }

   void CreateBuffers(csi16 groupIndex) {
      OBJECT_GROUP &objGroup = man.objGroup[groupIndex];
      ENTITY_GROUP &entGroup = man.entGroup[groupIndex];

      cui32 count = RoundUpToNearest32(entGroup.totalBones);

      for(ui32 i = 0; i <= levelsOfDetail; i++) {
         csi32 subcount = count >> (levelsOfDetail - i);

         visBuf[groupIndex][i]  = zalloc1d16(ui32, subcount);
         vertBuf[groupIndex][i] = gpu.buf.CreateVertex(visBuf[groupIndex][i], sizeof(ui32) * 32, (subcount + 31) >> 5, 1);
      }
      modBuf[groupIndex] = zalloc1d16(ui32, count);

      gpuBuf[groupIndex][0] = gpu.buf.CreateStructured(0, objGroup.object, sizeof(OBJECT_IGS), objGroup.totalObjects, ae_buf_immutable);
      gpuBuf[groupIndex][1] = gpu.buf.CreateStructured(0, objGroup.part, sizeof(PART_IGS), objGroup.totalParts, ae_buf_immutable);
      gpuBuf[groupIndex][2] = gpu.buf.CreateStructured(0, entGroup.bone_dgs, sizeof(BONE_DGS), entGroup.totalBone_DGS, ae_buf_dynamic);
      gpuBuf[groupIndex][3] = gpu.buf.CreateStructured(0, entGroup.spriteO, sizeof(SPRITE_DPS), entGroup.totalSpritesO, ae_buf_dynamic);
   }

   inline void StartViewCulling(csi32 groupIndex) const { man.Cull(visBuf[groupIndex], modBuf[groupIndex], groupIndex, 1); }

   ///--- !!! Expand to 7 LODs !!!
   // Returns counts for { Entities uploaded, LOD 0 entities, LOD 1 entities, LOD 2 entities }
   cui128 WaitThenUploadAndRender(csi16 groupIndex) {
      cENTITY_GROUP &group = man.entGroup[groupIndex];
      ui32           i;

      // Wait until visibility & modification culling completes
      man.WaitForCulling(entManThreadData, 0);

      // Upload modified entity data
      BONE_DGSptrc   boneGeoData = (BONE_DGSptr)gpu.buf.LockStructuredBeforeUpdate(0, gpuBuf[groupIndex][2]);
      SPRITE_DPSptrc bonePixData = (SPRITE_DPSptr)gpu.buf.LockStructuredBeforeUpdate(0, gpuBuf[groupIndex][3]);

      for(i = 0; i < entManThreadData.m128i_u32[3]; i++) {
         csi32 boneIndex = modBuf[groupIndex][i];

         Stream64(&group.bone_dgs[boneIndex], &boneGeoData[boneIndex], sizeof(BONE_DGS));
         Stream16(&group.spriteO[boneIndex], &bonePixData[boneIndex], sizeof(SPRITE_DPS));
      }
      gpu.buf.UnlockStructuredAfterUpdate(0, gpuBuf[groupIndex][2]);
      gpu.buf.UnlockStructuredAfterUpdate(0, gpuBuf[groupIndex][3]);

      // Render entities
      cENT_PARAMS params = { gpuBuf[groupIndex], vertBuf[groupIndex], visBuf[groupIndex], entManThreadData.m128i_u32, levelsOfDetail, 0 };
      gpu.ren.QueueDrawEntities(params);

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
