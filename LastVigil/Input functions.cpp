/************************************************************
* File: Input functions.cpp            Created: 2024/04/22 *
*                                Last modified: 2024/04/30 *
*                                                          *
* Desc:                                                    *
*                                                          *
*  Copyright (c) David William Bull. All rights reserved.  *
************************************************************/

#include "pch.h"
#include "data tracking.h"
#include "Common functions.h"
#include "class_timers.h"
#include "Direct3D11 thread.h"
#include "Direct3D11 functions.h"
#include "Armada Intelligence/class_mapmanager.h"
#include "Armada Intelligence/class_entitymanager.h"
#include "Armada Intelligence/Input functions.h"

void ProcessInputs(INPUT_PROC_DATA &ipd, GLOBALCTRLVARS &gcvLocal) {
   CLASS_GPU    &gpu    = *(CLASS_GPU *)ptrLib[1];
   CLASS_MAPMAN &mapMan = *(CLASS_MAPMAN *)ptrLib[6];
   MAP_DESC     &md     = *(MAP_DESC *)ptrLib[14];
   GUI_DESC     &gd     = *(GUI_DESC *)ptrLib[15];

   AVX8Ds32 &activeElements = md.mcrv.activeElements;
   AVX8Df32 &activeLocs     = md.mcrv.activeLocations;
   VEC3Df   &endPoint       = (VEC3Df &)activeLocs.vector1;

   // Process global action inputs


   // Execute relevant GUI functions
   activeElements = gpu.gui.ProcessInputs(ipd, gcvLocal, gd.interfaceIndex);

   // Create intersection test lists: Discover last cell under cursor and interpolate from cell containing camera, then discover & sort entities under cursor from precalculated cell lists
   activeLocs.vector0 = (VEC4Df &)gpu.cam.data[0].pos32;
   endPoint           = gpu.cam.CursorLayerIntersect(md.mapDim.z - md.zso, gcvLocal.curCoords, cVEC2Du8{0, 0});
   // Build cell list
   mapMan.PopulateCellList(activeLocs, 0, 0);
   // Build entity list
   //entMan.PopulateEntityList(groupList, 0, 0);
   // Process world space inputs
   
}

void ProcessInputs(GLOBALCTRLVARS &gcvLocal) {
   CLASS_GPU    &gpu    = *(CLASS_GPU *)ptrLib[1]; // Replace with separate cam. and gui. references?
   CLASS_MAPMAN &mapMan = *(CLASS_MAPMAN *)ptrLib[6];
   MAP_DESC     &md     = *(MAP_DESC *)ptrLib[14];

   cfl32 fElapsedTime = float(mainTimer.GetElapsedTimeScaled());
   csi32 siWheel      = gcvLocal.mouse.z;

   si128 &siActiveLayer = md.mcrv.activeElements.xmm1;
   si32  &siCell        = md.mcrv.cellIndex[0].vector.w;
   si32   siChunk       = 0;

   siActiveLayer = gpu.gui.ProcessInputs(gcvLocal, (*(GUI_DESC *)ptrLib[15]).interfaceIndex); // If cursor is over GUI element, process
   if(siActiveLayer.m128i_i32[0] < 0) {
      // Locate cell under cursor
      md.mcrv.activeCell.z = siWheel;
      md.mcrv.activePlane  = gpu.cam.CursorLayerIntersect(siWheel, cfl32x4{ 0.0f, 0.0f, 0.5f }, gcvLocal.curCoords, 0, 0);

      siCell  = mapMan.CalcCellIndex(md.mcrv.activeCell, 0, 0);
      siChunk = siCell / md.chunkCells;

      // Mouse button 0
      if(siActiveLayer.m128i_i32[1] < 0 && gcvLocal.imm.b[16] & 0x01) {
         if(siCell != 0x080000001) {
            mapMan.world[0].map[0]->cell[siCell].pixel->gev += ui16(fElapsedTime * 2048.0f);
            mapMan.world[0].map[0]->chunkMod[siChunk >> 6] |= ui64(0x01) << (siChunk & 0x03F);
         }
      }
      // Mouse button 1
      if(siActiveLayer.m128i_i32[2] < 0 && gcvLocal.imm.b[16] & 0x02) {
         if(siCell != 0x080000001) {
            mapMan.world[0].map[0]->cell[siCell].pixel->gev -= ui16(fElapsedTime * 2048.0f);
            mapMan.world[0].map[0]->chunkMod[siChunk >> 6] |= ui64(0x01) << (siChunk & 0x03F);
         }
      }
      // Mouse button 3
      if(gcvLocal.imm.b[16] & 0x08) {
         if(siCell != 0x080000001) {
            mapMan.ModQuadCellDensity(md.mcrv.activeCell, -fElapsedTime, 0, 0);
            //            gpu.gui.element_dgs[gpu.gui.element[panelElement0].vertexIndex].rotAngle -= fElapsedTime;
            //            gpu.gui.element_dgs[gpu.gui.element[textInputEl].vertexIndex].rotAngle -= fElapsedTime;
         }
      }
      // Mouse button 4
      if(gcvLocal.imm.b[16] & 0x010) {
         if(siCell != 0x080000001) {
            mapMan.ModQuadCellDensity(md.mcrv.activeCell, fElapsedTime, 0, 0);
            //            gpu.gui.element_dgs[gpu.gui.element[panelElement0].vertexIndex].rotAngle += fElapsedTime;
            //            gpu.gui.element_dgs[gpu.gui.element[textInputEl].vertexIndex].rotAngle += fElapsedTime;
         }
      }
      // Mouse wheel left
      if(gcvLocal.imm.b[16] & 0x020) {
         if(siCell != 0x080000001) {
            mapMan.ModQuadCellDensity(md.mcrv.activeCell, -fElapsedTime, 0, 0);
         }
      }
      // Mouse wheel right
      if(gcvLocal.imm.b[16] & 0x040) {
         if(siCell != 0x080000001) {
            mapMan.ModQuadCellDensity(md.mcrv.activeCell, fElapsedTime, 0, 0);
         }
      }
      if(gcvLocal.imm.b[4] & 0x01)
         if(siCell != 0x080000001) {
            mapMan.world[0].map[0]->cell[siCell].geometry->et.x = (gcvLocal.misc[1] & 0x03) + 1;
            mapMan.world[0].map[0]->chunkMod[siChunk >> 6] |= ui64(0x01) << (siChunk & 0x03F);
         }
   }
   // Mouse button 2
   if(gcvLocal.imm.b[16] & 0x04) {
      if(gcvLocal.misc[7] & 0x08) {
         //gpu.MouseCursor(hWnd, false);
         gcvLocal.misc[7] &= 0x0F7;
      }
      gpu.cam.data[0].fXrot -= gcvLocal.mouse.y;
      gpu.cam.data[0].fYrot += gcvLocal.mouse.x;
   } else
      if(!(gcvLocal.misc[7] & 0x04)) {
         //gpu.MouseCursor(hWnd, true);
         gcvLocal.misc[7] |= 0x08;
      }
}
