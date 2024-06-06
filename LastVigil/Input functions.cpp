/************************************************************
* File: Input functions.cpp            Created: 2024/04/22 *
*                                Last modified: 2024/06/05 *
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

// Scalar inputs to be processed via NULL input pattern in each stage
void ProcessInputs(INPUT_PROC_DATA &ipd, GLOBALCTRLVARS &ctrlVars) {
   static ui16 imCount, imLast;
   static ui8  funcCount;

   CLASS_GPU &gpu = *(CLASS_GPU *)ptrLib[1];
   MAP_DESC  &md  = *(MAP_DESC *)ptrLib[14];

   imLast = ipd.input.global;

   // Populate intersection lists  --  Thread? Not required until 'Process world space inputs'
   md.mcrv.activeLocations.xmm0 = gpu.cam.data[0].pos32.xmm;
   md.mcrv.activeLocation1.z    = fl32(md.mapDim.z - md.zso);
   gpu.cam.CursorLayerIntersect(md.mcrv.locationOS[1], ctrlVars.curCoords, gpu.cam.currentCamProj);
   (*(CLASS_MAPMAN *)ptrLib[6]).PopulateCellList(md.mcrv.activeLocations, md.wlrv.world, md.wlrv.map);
   (*(CLASS_ENTMAN *)ptrLib[7]).PopulateEntityList(md, ctrlVars.curCoords, gpu.cam.currentCamProj);

   // Process global action inputs
   for(imCount = 0; imCount < imLast; imCount++)
      if(!AllFalse(ctrlVars.imm.button, ipd.inputMask[imCount]))
         for(funcCount = 0; funcCount < ipd.funcCount[imCount].x; funcCount++)
            ipd.function[funcCount](NULL);

   // Execute relevant GUI functions
   md.mcrv.activeElements = gpu.gui.ProcessInputs(ipd, ctrlVars, (*(GUI_DESC *)ptrLib[15]).interfaceIndex);

   // Process world space inputs
   for(imLast = (imCount += ipd.input.ui) + ipd.input.world; imCount < imLast; imCount++)
      if(!AllFalse(ctrlVars.imm.button, ipd.inputMask[imCount]))
         for(funcCount = 0; funcCount < ipd.funcCount[imCount].x; funcCount++)
            ipd.function[funcCount](NULL);
}

/// !!! For testing purposes !!!
void ProcessInputs(GLOBALCTRLVARS &ctrlVars) {
   CLASS_GPU    &gpu    = *(CLASS_GPU *)ptrLib[1]; // Replace with separate cam. and gui. references?
   CLASS_MAPMAN &mapMan = *(CLASS_MAPMAN *)ptrLib[6];
   MAP_DESC     &md     = *(MAP_DESC *)ptrLib[14];

   si128 &siActiveLayer = md.mcrv.activeElements.xmm1;
   si32  &siCell        = md.mcrv.cellIndex[0].vector.w;

   cfl32 fElapsedTime = fl32(mainTimer.GetElapsedTimeScaled());
   csi32 siWheel      = ctrlVars.mouse.z;

   si32 siChunk = 0;

   siActiveLayer = gpu.gui.ProcessInputs(ctrlVars, (*(GUI_DESC *)ptrLib[15]).interfaceIndex); // If cursor is over GUI element, process
   if(siActiveLayer.m128i_i32[0] < 0) {
      // Locate cell under cursor
      md.mcrv.activeCell.z = siWheel;
      md.mcrv.activePlane  = gpu.cam.CursorLayerIntersect(siWheel, cfl32x4{ 0.0f, 0.0f, 0.5f }, ctrlVars.curCoords, 0, 0);

      siCell  = mapMan.CalcCellIndex(md.mcrv.activeCell, 0, 0);
      siChunk = siCell / md.chunkCells;

      // Mouse button 0
      if(siActiveLayer.m128i_i32[1] < 0 && ctrlVars.imm.k[16] & 0x01) {
         if(siCell != 0x080000001) {
            mapMan.world[0].map[0]->cell[siCell].pixel->gev += fElapsedTime * 8.0f;
            mapMan.world[0].map[0]->chunkMod[siChunk >> 6] |= ui64(0x01) << (siChunk & 0x03F);
         }
      }
      // Mouse button 1
      if(siActiveLayer.m128i_i32[2] < 0 && ctrlVars.imm.k[16] & 0x02) {
         if(siCell != 0x080000001) {
            mapMan.world[0].map[0]->cell[siCell].pixel->gev -= fElapsedTime * 8.0f;
            mapMan.world[0].map[0]->chunkMod[siChunk >> 6] |= ui64(0x01) << (siChunk & 0x03F);
         }
      }
      // Mouse button 3
      if(ctrlVars.imm.k[16] & 0x08) {
         if(siCell != 0x080000001) {
            mapMan.ModQuadCellDensity(md.mcrv.activeCell, -fElapsedTime, 0, 0);
            //            gpu.gui.element_dgs[gpu.gui.element[panelElement0].vertexIndex].rotAngle -= fElapsedTime;
            //            gpu.gui.element_dgs[gpu.gui.element[textInputEl].vertexIndex].rotAngle -= fElapsedTime;
         }
      }
      // Mouse button 4
      if(ctrlVars.imm.k[16] & 0x010) {
         if(siCell != 0x080000001) {
            mapMan.ModQuadCellDensity(md.mcrv.activeCell, fElapsedTime, 0, 0);
            //            gpu.gui.element_dgs[gpu.gui.element[panelElement0].vertexIndex].rotAngle += fElapsedTime;
            //            gpu.gui.element_dgs[gpu.gui.element[textInputEl].vertexIndex].rotAngle += fElapsedTime;
         }
      }
      // Mouse wheel left
      if(ctrlVars.imm.k[16] & 0x020) {
         if(siCell != 0x080000001) {
            mapMan.ModQuadCellDensity(md.mcrv.activeCell, -fElapsedTime, 0, 0);
         }
      }
      // Mouse wheel right
      if(ctrlVars.imm.k[16] & 0x040) {
         if(siCell != 0x080000001) {
            mapMan.ModQuadCellDensity(md.mcrv.activeCell, fElapsedTime, 0, 0);
         }
      }
      if(ctrlVars.imm.k[4] & 0x01)
         if(siCell != 0x080000001) {
            mapMan.world[0].map[0]->cell[siCell].geometry->et.x = (ctrlVars.misc[1] & 0x03u) + 1u;
            mapMan.world[0].map[0]->chunkMod[siChunk >> 6] |= ui64(0x01u) << (siChunk & 0x03Fu);
         }
   }
   // Mouse button 2
   if(ctrlVars.imm.k[16] & 0x04) {
      if(ctrlVars.misc[7] & 0x08) {
         //gpu.MouseCursor(hWnd, false);
         ctrlVars.misc[7] &= 0x0F7;
      }
      gpu.cam.data[0].fXrot -= ctrlVars.mouse.y;
      gpu.cam.data[0].fYrot += ctrlVars.mouse.x;
   } else
      if(!(ctrlVars.misc[7] & 0x04)) {
         //gpu.MouseCursor(hWnd, true);
         ctrlVars.misc[7] |= 0x08;
      }
   // Gamepad 0x02 input
   gpu.cam.data[0].fXrot -= ctrlVars.joy[2].s.x1 * fElapsedTime;
   gpu.cam.data[0].fYrot += ctrlVars.joy[2].s.y1 * fElapsedTime;
}
