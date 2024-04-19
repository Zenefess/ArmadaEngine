/************************************************************
 * File: GUI functions.cpp              Created: 2023/06/13 *
 *                                Last modified: 2024/04/19 *
 *                                                          *
 * Desc: User interface functions for in-game menus and     *
 *       developer environment.                             *
 *                                                          *
 *  Copyright (c) David William Bull. All rights reserved.  *
 ************************************************************/
#include "pch.h"
#include "data tracking.h"
#include "typedefs.h"
#include "Vector structures.h"
#include "Data structures.h"
#include "thread flags.h"
#include "main thread.h"
#include "Common functions.h"
#include "class_timers.h"
#include "Direct3D11 thread.h"
#include "Direct3D11 functions.h"
#include "Armada Intelligence/class_mapmanager.h"

extern TEXTBUFFER textBufferInfo; // Buffer information for character input

void ProcessGUIInputs(GLOBALCTRLVARS &gcvLocal) {
   CLASS_GPU        &gpu    = *(CLASS_GPU *)ptrLib[5];
   CLASS_MAPMANAGER &mapMan = *(CLASS_MAPMANAGER *)ptrLib[2];
   GUI_DESC         &gd     = *(GUI_DESC *)ptrLib[9];

   cfl32 fElapsedTime = float(mainTimer.GetElapsedTimeScaled());

   si128 &siActiveLayer = gd.returnValues.xmm1;
   si32  &siCell        = gd.returnValues.vector[0].w;
   si32   siChunk       = 0;

   csi32 siWheel = gcvLocal.s32.z[1].x;

   siActiveLayer = gpu.gui.ProcessInputs(gcvLocal, gd.interfaceIndex); // If cursor is over GUI element, process
   if(siActiveLayer.m128i_i32[0] < 0) {
      gd.activeCell.z = siWheel;
      // Locate cell under cursor
      gd.activePlane = gpu.cam.CursorLayerIntersect(siWheel, cvector{ 0.0f, 0.0f, 0.5f }, gcvLocal.curCoords, 0, 0);
      siCell = mapMan.CalcCellIndex(gd.activeCell, 0, 0);
      siChunk = siCell / (*(MAP_DESC *)ptrLib[8]).chunkCells;
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
            mapMan.ModQuadCellDensity(gd.activeCell, -fElapsedTime, 0, 0);
//            gpu.gui.element_dgs[gpu.gui.element[panelElement0].vertexIndex].rotAngle -= fElapsedTime;
//            gpu.gui.element_dgs[gpu.gui.element[textInputEl].vertexIndex].rotAngle -= fElapsedTime;
         }
      }
      // Mouse button 4
      if(gcvLocal.imm.b[16] & 0x010) {
         if(siCell != 0x080000001) {
            mapMan.ModQuadCellDensity(gd.activeCell, fElapsedTime, 0, 0);
//            gpu.gui.element_dgs[gpu.gui.element[panelElement0].vertexIndex].rotAngle += fElapsedTime;
//            gpu.gui.element_dgs[gpu.gui.element[textInputEl].vertexIndex].rotAngle += fElapsedTime;
         }
      }
      // Mouse wheel left
      if(gcvLocal.imm.b[16] & 0x020) {
         if(siCell != 0x080000001) {
            mapMan.ModQuadCellDensity(gd.activeCell, -fElapsedTime, 0, 0);
         }
      }
      // Mouse wheel right
      if(gcvLocal.imm.b[16] & 0x040) {
         if(siCell != 0x080000001) {
            mapMan.ModQuadCellDensity(gd.activeCell, fElapsedTime, 0, 0);
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
      gpu.cam.s[0].fXrot -= gcvLocal.xy[1].y2;
      gpu.cam.s[0].fYrot += gcvLocal.xy[1].x1;
   } else
      if(!(gcvLocal.misc[7] & 0x04)) {
         //gpu.MouseCursor(hWnd, true);
         gcvLocal.misc[7] |= 0x08;
      }
}

void __Activate0_0_Default_Button(cptrc indices) {
   const GUI_INDICES &data = (GUI_INDICES &)indices;

   GUI_ELEMENT &element    = (*(const CLASS_GUI *)ptrLib[4]).element[data.element];
   GUI_EL_DGS (&vertex)[2] = (GUI_EL_DGS (&)[2])(*(const CLASS_GUI *)ptrLib[4]).element_dgs[data.vertex];

   if(!(element.stateBits & 0x04)) {
      element.bitField ^= 0x0480;
      vertex[0].align ^= 0x080;
      vertex[1].align ^= 0x080;
   }
}

void __Activate0_1_Default_Button(cptrc indices) {
   const GUI_INDICES &data = (GUI_INDICES &)indices;

   GUI_ELEMENT &element    = (*(const CLASS_GUI *)ptrLib[4]).element[data.element];
   GUI_EL_DGS (&vertex)[2] = (GUI_EL_DGS (&)[2])(*(const CLASS_GUI *)ptrLib[4]).element_dgs[data.vertex];

   if(element.stateBits & 0x04) {
      element.bitField ^= 0x0480;
      vertex[0].align ^= 0x080;
      vertex[1].align ^= 0x080;
   }
}

void __Activate0_0_Default_Toggle(cptrc indices) {
   const GUI_INDICES &data = (GUI_INDICES &)indices;

   GUI_ELEMENT &element    = (*(const CLASS_GUI *)ptrLib[4]).element[data.element];
   GUI_EL_DGS (&vertex)[2] = (GUI_EL_DGS (&)[2])(*(const CLASS_GUI *)ptrLib[4]).element_dgs[data.vertex];

   if(!(element.stateBits & 0x04)) {
      element.bitField ^= 0x0400;
      vertex[0].align ^= 0x080;
      vertex[1].align ^= 0x080;
   }
}

void __Activate0_1_Default_Toggle(cptrc indices) {
   const GUI_INDICES &data = (GUI_INDICES &)indices;

   GUI_ELEMENT &element = (*(const CLASS_GUI *)ptrLib[4]).element[data.element];

   if(element.stateBits & 0x04)
      element.bitField ^= 0x0480;
}

void __Activate0_0_Default_Scalar(cptrc indices) {
   const GUI_INDICES &data = (GUI_INDICES &)indices;

   GUI_ELEMENT &element    = (*(const CLASS_GUI *)ptrLib[4]).element[data.element];
   GUI_EL_DGS (&vertex)[2] = (GUI_EL_DGS (&)[2])(*(const CLASS_GUI *)ptrLib[4]).element_dgs[data.vertex];

   if(!(element.stateBits & 0x04)) {
      element.stateBits |= 0x04;
      element.si32Var[0] = gcv.curCoords.x;
   } else {
      csi32 curCoord   = gcv.curCoords.x;
      cfl32 difference = fl32(curCoord - element.si32Var[0]) / ScrRes.dims[ScrRes.state].dim[0];

      element.si32Var[0] = curCoord;

      vertex[1].viewPos.x += difference * element.viewScale.x * ScrRes.dims[ScrRes.state].aspectI;

      if(vertex[1].viewPos.x < -1.0f)
         vertex[1].viewPos.x = -1.0f;
      else if(vertex[1].viewPos.x > 1.0f)
         vertex[1].viewPos.x = 1.0f;
   }
}

void __Activate0_1_Default_Scalar(cptrc indices) {
   const GUI_INDICES &data = (GUI_INDICES &)indices;

   GUI_ELEMENT &element = (*(const CLASS_GUI *)ptrLib[4]).element[data.element];

   if(element.stateBits & 0x04)
      element.stateBits &= 0x0FB;
}

void __Activate1_0_Default_Scalar(cptrc indices) {
   const GUI_INDICES &data = (GUI_INDICES &)indices;

   GUI_ELEMENT &element    = (*(const CLASS_GUI *)ptrLib[4]).element[data.element];
   GUI_EL_DGS (&vertex)[2] = (GUI_EL_DGS (&)[2])(*(const CLASS_GUI *)ptrLib[4]).element_dgs[data.vertex];

   if(!(element.stateBits & 0x08))
      element.stateBits |= 0x08;
   else {
      cfl32 curCoord = fl32(gcv.curCoords.x) / ScrRes.dims[ScrRes.state].dim[0];

      vertex[1].viewPos.x = (curCoord - Float16(element.activePos.x, 1.0f)) * element.viewScale.x * ScrRes.dims[ScrRes.state].aspectI;

      if(vertex[1].viewPos.x < -1.0f)
         vertex[1].viewPos.x = -1.0f;
      else if(vertex[1].viewPos.x > 1.0f)
         vertex[1].viewPos.x = 1.0f;
   }
}

void __Activate1_1_Default_Scalar(cptrc indices) {
   const GUI_INDICES &data = (GUI_INDICES &)indices;

   GUI_ELEMENT &element = (*(const CLASS_GUI *)ptrLib[4]).element[data.element];

   if(element.stateBits & 0x08)
      element.stateBits &= 0x0F7;
}

void __Activate0_0_Default_Input(cptrc indices) {
   const GUI_INDICES &data = (GUI_INDICES &)indices;

   GUI_ELEMENT (&element)[3] = (GUI_ELEMENT (&)[3])(*(const CLASS_GUI *)ptrLib[4]).element[data.element];
   GUI_EL_DGS  (&vertex)[3]  = (GUI_EL_DGS (&)[3])(*(const CLASS_GUI *)ptrLib[4]).element_dgs[data.vertex];

   if(!(element[0].stateBits & 0x04)) {
      element[0].stateBits |= 0x04;

      textBufferInfo.source.pCH        =  &(*(const CLASS_GUI *)ptrLib[4]).textBuffer[(vertex[2].textArrayOS & 0x03FFFFFF) << 4];
      textBufferInfo.source.byteCount  =  element[2].vertexCount[1] << 5;
      textBufferInfo.source.byteOffset =  element[2].charCount << (element[2].bitField & 0x0800 ? 1 : 0);
      textBufferInfo.source.bitField   |= 0x01;
   }
}

void __Passive_Default_Input(cptrc indices) {
   const GUI_INDICES &data = (GUI_INDICES &)indices;
   csi32 indexVert = data.vertex + 2;

   GUI_ELEMENT (&element)[3] = (GUI_ELEMENT (&)[3])(*(const CLASS_GUI *)ptrLib[4]).element[data.element];
   GUI_EL_DGS *const vertex  = &(*(const CLASS_GUI *)ptrLib[4]).element_dgs[indexVert];

   if(element[0].stateBits & 0x04) {
      // Recalculate vertex count
      cui16 oldVertCount = max(0, element[2].charCount - 1) >> 5;
      cui16 newVertCount = max(0, textBufferInfo.source.byteOffset - 1) >> (element[2].bitField & 0x0800 ? 1 : 0) >> 5;

      element[2].vertexCount[0] = newVertCount + 1;
      element[2].charCount = textBufferInfo.source.byteOffset >> (element[2].bitField & 0x0800 ? 1 : 0);

      if(newVertCount < oldVertCount) {
         vertex[oldVertCount].origin[0] = { 0.0f, 0.0f };
         vertex[oldVertCount].origin[1] = { 0.0f, 0.0f };
         vertex[oldVertCount].origin[2] = { 0.0f, 0.0f };
         vertex[oldVertCount].origin[3] = { 0.0f, 0.0f };
         vertex[oldVertCount].align |= 0x080;
      } else if(newVertCount > oldVertCount) {
         vertex[newVertCount].origin[0] = (*(const CLASS_GUI *)ptrLib[4])._CalculateOrigin(indexVert, indexVert + newVertCount, vertex[0].alphabetIndex);
         vertex[newVertCount].origin[1] = { 16384.0f, 16384.0f };
         vertex[newVertCount].origin[2] = { 16384.0f, 16384.0f };
         vertex[newVertCount].origin[3] = { 16384.0f, 16384.0f };
         vertex[newVertCount].align &= 0x07F;
      }

      // Is the 'confirm' flag set?
      if(textBufferInfo.source.bitField & 0x2) {

         textBufferInfo.source.bitField = 0;
         element[0].stateBits &= 0x0FB;
      }
      // Is the 'cancel' flag set?
         if(textBufferInfo.source.bitField & 0x4) {

         textBufferInfo.source.bitField = 0;
         element[0].stateBits &= 0x0FB;
      }
   }
}

void __OnHover_Default(cptrc indices) {
   const GUI_INDICES &data = (GUI_INDICES &)indices;

}

void __OffHover_Default(cptrc indices) {
   const GUI_INDICES &data = (GUI_INDICES &)indices;

}

/*
 *  User interfaces
 */

void MainMenu(cptrc argList) {
   static bool firstRun = true;

   const struct INPUT_INDICES {
      si16 alphabet;
      ui16 spriteLib;
      ui16 panelSprite;
   } &index = (const INPUT_INDICES &)argList;

   CLASS_GUI &gui = *(CLASS_GUI *)ptrLib[4];
   GUI_DESC  &gd  = *(GUI_DESC *)ptrLib[9];

   union { VEC2Du32 button[8]; struct { VEC2Du32 buttonOptions, buttonEditors, buttonExit; }; };

   ui32 interfaceMain;

   if(firstRun) {
      GUI_EL_DESC elementDesc;

      elementDesc.viewPos    = { 0.0f, -0.25f };
      elementDesc.index      = { index.alphabet, index.spriteLib, index.panelSprite };
      elementDesc.size       = { { 0.125f, 0.125f }, { 1.0f, 1.0f } };
      elementDesc.colour     = { { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f } };
      elementDesc.text       = (chptr)"Options";
      elementDesc.charCount  = 7;
      elementDesc.panelAlign = UI_ALIGN_L;
      elementDesc.panelMods  = UI_NONE;
      elementDesc.textAlign  = UI_ALIGN_C;
      elementDesc.textMods   = UI_ROT | UI_TRANS | UI_SCALE_X;
      buttonOptions = gui.CreateTextButton(elementDesc);
      elementDesc.viewPos    = { 0.0f, -0.5f };
      elementDesc.index      = { index.alphabet, index.spriteLib, index.panelSprite };
      elementDesc.size       = { { 0.125f, 0.125f }, { 1.0f, 1.0f } };
      elementDesc.colour     = { { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f } };
      elementDesc.text       = (chptr)"Editors";
      elementDesc.charCount  = 7;
      elementDesc.panelAlign = UI_ALIGN_L;
      elementDesc.panelMods  = UI_NONE;
      elementDesc.textAlign  = UI_ALIGN_C;
      elementDesc.textMods   = UI_ROT | UI_TRANS | UI_SCALE_X;
      buttonEditors = gui.CreateTextButton(elementDesc);
      elementDesc.viewPos    = { 0.0f, 0.0f };
      elementDesc.index      = { index.alphabet, index.spriteLib, index.panelSprite };
      elementDesc.size       = { { 0.125f, 0.125f }, { 1.0f, 1.0f } };
      elementDesc.colour     = { { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f } };
      elementDesc.text       = (chptr)"Exit";
      elementDesc.charCount  = 4;
      elementDesc.panelAlign = UI_ALIGN_L | UI_ALIGN_B;
      elementDesc.panelMods  = UI_NONE;
      elementDesc.textAlign  = UI_ALIGN_C;
      elementDesc.textMods   = UI_ROT | UI_TRANS | UI_SCALE_X;
      buttonExit = gui.CreateTextButton(elementDesc);

      interfaceMain = gui.CreateInterface(128, 16);
      gui.AddElementToInterface(buttonOptions, interfaceMain);
      gui.AddElementToInterface(buttonEditors, interfaceMain);
      gui.AddElementToInterface(buttonExit, interfaceMain);

      firstRun = false;
   }
}

/*
 *  Editor menus
 */

void GUI_Editor(cptrc argList) {
   const CLASS_GUI &gui = *(const CLASS_GUI *)ptrLib[4];

}

/*
 *  Game menus
 */

void PauseMenu(cptrc argList) {          // 5th
   const CLASS_GUI &gui = *(const CLASS_GUI *)ptrLib[4];
}

void AudioMenu(cptrc argList) {          // 4th
   const CLASS_GUI &gui = *(const CLASS_GUI *)ptrLib[4];
}

void VideoMenu(cptrc argList) {          // 2nd
   const CLASS_GUI &gui = *(const CLASS_GUI *)ptrLib[4];
}

void ControlsMenu(cptrc argList) {       // 1st
   const CLASS_GUI &gui = *(const CLASS_GUI *)ptrLib[4];


}

void GameplayMenu(cptrc argList) {       // 7th
   const CLASS_GUI &gui = *(const CLASS_GUI *)ptrLib[4];
}

void OptionsMenu(cptrc argList) {        // 6th
   const CLASS_GUI &gui = *(const CLASS_GUI *)ptrLib[4];
}

void DeveloperInterface(cptrc argList) { // 3rd
   const CLASS_GUI &gui = *(const CLASS_GUI *)ptrLib[4];
}
