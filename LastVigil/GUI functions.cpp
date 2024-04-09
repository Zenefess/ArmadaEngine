/************************************************************
 * File: GUI functions.cpp              Created: 2023/06/13 *
 *                                Last modified: 2024/03/29 *
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

extern TEXTBUFFER textBufferInfo; // Buffer information for character input

void __Activate0_0_Default_Button(cptrc indices) {
   const GUI_INDICES &data = (GUI_INDICES &)indices;

   GUI_ELEMENT &element    = (*(const CLASS_GUI *)ptrLib[4]).element[data.element];
   GUI_EL_DGS (&vertex)[2] = (GUI_EL_DGS (&)[2])(*(const CLASS_GUI *)ptrLib[4]).element_dgs[data.vertex];

   if(!(element.stateBits & 0x04)) {
      element.bitField ^= 0x0480;
      vertex[0].mods ^= 0x010;
      vertex[1].mods ^= 0x010;
   }
}

void __Activate0_1_Default_Button(cptrc indices) {
   const GUI_INDICES &data = (GUI_INDICES &)indices;

   GUI_ELEMENT &element    = (*(const CLASS_GUI *)ptrLib[4]).element[data.element];
   GUI_EL_DGS (&vertex)[2] = (GUI_EL_DGS (&)[2])(*(const CLASS_GUI *)ptrLib[4]).element_dgs[data.vertex];

   if(element.stateBits & 0x04) {
      element.bitField ^= 0x0480;
      vertex[0].mods ^= 0x010;
      vertex[1].mods ^= 0x010;
   }
}

void __Activate0_0_Default_Toggle(cptrc indices) {
   const GUI_INDICES &data = (GUI_INDICES &)indices;

   GUI_ELEMENT &element    = (*(const CLASS_GUI *)ptrLib[4]).element[data.element];
   GUI_EL_DGS (&vertex)[2] = (GUI_EL_DGS (&)[2])(*(const CLASS_GUI *)ptrLib[4]).element_dgs[data.vertex];

   if(!(element.stateBits & 0x04)) {
      element.bitField ^= 0x0400;
      vertex[0].mods ^= 0x010;
      vertex[1].mods ^= 0x010;
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
         vertex[oldVertCount].mods |= 0x010;
      } else if(newVertCount > oldVertCount) {
         vertex[newVertCount].origin[0] = (*(const CLASS_GUI *)ptrLib[4])._CalculateOrigin(indexVert, indexVert + newVertCount, vertex[0].alphabetIndex);
         vertex[newVertCount].origin[1] = { 16384.0f, 16384.0f };
         vertex[newVertCount].origin[2] = { 16384.0f, 16384.0f };
         vertex[newVertCount].origin[3] = { 16384.0f, 16384.0f };
         vertex[newVertCount].mods &= 0x0EF;
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

void MainMenu(cptrc argList) {           // 8th
   const CLASS_GUI &gui = *(const CLASS_GUI *)ptrLib[4];
}

void DeveloperInterface(cptrc argList) { // 3rd
   const CLASS_GUI &gui = *(const CLASS_GUI *)ptrLib[4];
}
