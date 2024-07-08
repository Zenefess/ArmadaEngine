/************************************************************
 * File: GUI functions.cpp              Created: 2023/06/13 *
 *                                Last modified: 2024/06/24 *
 *                                                          *
 * Desc: User interface functions for in-game menus and     *
 *       developer environment.                             *
 *                                                          *
 *  Copyright (c) David William Bull. All rights reserved.  *
 ************************************************************/

#include "master header.h"
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
#include "Armada Intelligence/class_gui.h"
#include "Armada Intelligence/class_mapmanager.h"
#include "colours.h"

extern TEXTBUFFER *activeTextBuffer;

void __OnHover_Default(cptrc indices) {
   cGUI_INDICES &data = (GUI_INDICES &)indices;

}

void __OffHover_Default(cptrc indices) {
   cGUI_INDICES &data = (GUI_INDICES &)indices;

}

// Set activated button state
void __Activate0_0_Default_Button(cptrc indices) {
   cGUI_INDICES &data = (GUI_INDICES &)indices;

   GUI_ELEMENT &element    = (*(cCLASS_GUI *)ptrLib[4]).element[data.element];
   GUI_EL_DGS (&vertex)[2] = (GUI_EL_DGS (&)[2])(*(cCLASS_GUI *)ptrLib[4]).element_dgs[data.vertex];

   if(!(element.stateBits & 0x04)) {
      element.bitField ^= 0x0480;
      vertex[0].align ^= 0x080;
      vertex[1].align ^= 0x080;
   }
}

// Set deactivated button state
void __Activate0_1_Default_Button(cptrc indices) {
   cGUI_INDICES &data = (GUI_INDICES &)indices;

   GUI_ELEMENT &element    = (*(cCLASS_GUI *)ptrLib[4]).element[data.element];
   GUI_EL_DGS (&vertex)[2] = (GUI_EL_DGS (&)[2])(*(cCLASS_GUI *)ptrLib[4]).element_dgs[data.vertex];

   if(element.stateBits & 0x04) {
      element.bitField ^= 0x0480;
      vertex[0].align ^= 0x080;
      vertex[1].align ^= 0x080;
   }
}

// Show flipped button state
void __Activate0_0_Default_Toggle(cptrc indices) {
   cGUI_INDICES &data = (GUI_INDICES &)indices;

   GUI_ELEMENT &element    = (*(cCLASS_GUI *)ptrLib[4]).element[data.element];
   GUI_EL_DGS (&vertex)[2] = (GUI_EL_DGS (&)[2])(*(cCLASS_GUI *)ptrLib[4]).element_dgs[data.vertex];

   if(!(element.stateBits & 0x04)) {
      element.bitField ^= 0x0480;
      vertex[0].align ^= 0x080;
      vertex[1].align ^= 0x080;
   }
}

// Flip button state
void __Activate0_1_Default_Toggle(cptrc indices) {
   cGUI_INDICES &data = (GUI_INDICES &)indices;

   GUI_ELEMENT &element = (*(cCLASS_GUI *)ptrLib[4]).element[data.element];

   if(element.stateBits & 0x04)
      element.bitField ^= 0x0480;
}

// Move scalar relative to pointer
void __Activate0_0_Default_Scalar(cptrc indices) {
   cGUI_INDICES &data = (GUI_INDICES &)indices;

   GUI_ELEMENT &element    = (*(cCLASS_GUI *)ptrLib[4]).element[data.element];
   GUI_EL_DGS (&vertex)[2] = (GUI_EL_DGS (&)[2])(*(cCLASS_GUI *)ptrLib[4]).element_dgs[data.vertex];

   if(!(element.stateBits & 0x04)) {
      element.stateBits |= 0x04;
      element.scalarPos = gcv.curCoords.x;
   } else {
      csi32 curCoord   = gcv.curCoords.x;
      cfl32 difference = fl32(curCoord - element.scalarPos) / ScrRes.dims[ScrRes.state].dim[0];

      element.scalarPos = curCoord;

      vertex[1].viewPos.x += difference * element.viewScale.x * ScrRes.dims[ScrRes.state].aspectI;

      if(vertex[1].viewPos.x < -1.0f)
         vertex[1].viewPos.x = -1.0f;
      else if(vertex[1].viewPos.x > 1.0f)
         vertex[1].viewPos.x = 1.0f;
   }
}

// Stop moving scalar relative to pointer
void __Activate0_1_Default_Scalar(cptrc indices) {
   cGUI_INDICES &data = (GUI_INDICES &)indices;

   GUI_ELEMENT &element = (*(cCLASS_GUI *)ptrLib[4]).element[data.element];

   if(element.stateBits & 0x04)
      element.stateBits &= 0x0FB;
}

// Set scalar to pointer position
void __Activate1_0_Default_Scalar(cptrc indices) {
   cGUI_INDICES &data = (GUI_INDICES &)indices;

   GUI_ELEMENT &element    = (*(cCLASS_GUI *)ptrLib[4]).element[data.element];
   GUI_EL_DGS (&vertex)[2] = (GUI_EL_DGS (&)[2])(*(cCLASS_GUI *)ptrLib[4]).element_dgs[data.vertex];

   if(!(element.stateBits & 0x08))
      element.stateBits |= 0x08;
   else {
      cfl32 curCoord = fl32(gcv.curCoords.x) / ScrRes.dims[ScrRes.state].dim[0];

      vertex[1].viewPos.x = (curCoord - element.activePos.x) * element.viewScale.x * ScrRes.dims[ScrRes.state].aspectI;

      if(vertex[1].viewPos.x < -1.0f)
         vertex[1].viewPos.x = -1.0f;
      else if(vertex[1].viewPos.x > 1.0f)
         vertex[1].viewPos.x = 1.0f;
   }
}

// Stop setting scalar to pointer position
void __Activate1_1_Default_Scalar(cptrc indices) {
   cGUI_INDICES &data = (GUI_INDICES &)indices;

   GUI_ELEMENT &element = (*(cCLASS_GUI *)ptrLib[4]).element[data.element];

   if(element.stateBits & 0x08)
      element.stateBits &= 0x0F7;
}

// Reprocess string
void __Passive_Default_Input(cptrc indices) {
   GUI_ELEMENT (&element)[3] = (GUI_ELEMENT (&)[3])(*(cCLASS_GUI *)ptrLib[4]).element[((GUI_INDICES &)indices).element];

   if(element[0].stateBits & 0x04) {
      cCLASS_GUI &gui        = *(cCLASS_GUI *)ptrLib[4];
      TEXTBUFFER &textBuffer = *element->input;

      csi32 indexVert = ((GUI_INDICES &)indices).vertex + 2;

      GUI_EL_DGSptrc vertex = &gui.element_dgs[indexVert];

      chptrc pCH    = textBuffer.source.pCH;
      cui32  pIMMos = gui.alphabet[vertex[0].alphabetIndex].pIMMos;
      fl32   accum  = 0.0f;
      ui32   i      = 0;

      // Recalculate vertex count
      cui16 oldVertCount = max(0, element[2].charCount - 1) >> 5;
      cui16 newVertCount = min(element[2].vertexCount[1] - 1, max(0, textBuffer.source.byteOffset - 1) >> (element[2].bitField & 0x0800 ? 1 : 0) >> 5);

      element[2].vertexCount[0] = newVertCount + 1;
      element[2].charCount      = textBuffer.source.byteOffset >> (element[2].bitField & 0x0800 ? 1 : 0);

      if(newVertCount < oldVertCount) {
#ifdef __AVX__
         vertex[oldVertCount].coords.ymm = _mm256_setzero_ps();
#else
         vertex[oldVertCount].coords.xmm[0] = _mm_setzero_ps();
         vertex[oldVertCount].coords.xmm[1] = _mm_setzero_ps();
#endif
         vertex[oldVertCount].charBankOS = vertex[oldVertCount].charBankOS & 0x03FFFFFFu;
         vertex[oldVertCount].align |= 0x080;
      } else {
         cui32 vertexCharCount = ((((element[2].charCount - 1) & 31) + 1) << 26);
         if(newVertCount > oldVertCount) {
            vertex[oldVertCount].charBankOS    = (vertex[oldVertCount].charBankOS & 0x03FFFFFFu) | 0x080000000u;
            vertex[newVertCount].origin[0]     = (*(cCLASS_GUI *)ptrLib[4]).CalculateTextOrigin(indexVert, indexVert + newVertCount, vertex[0].alphabetIndex);
            vertex[newVertCount].origin[1]     = { 16384.0f, 16384.0f };
            vertex[newVertCount].coords.xmm[1] = _mm_set_ps1(16384.0f);
            vertex[newVertCount].charBankOS    = (vertex[newVertCount].charBankOS & 0x03FFFFFFu) | vertexCharCount;
            vertex[newVertCount].align        &= 0x07F;
         } else
            vertex[oldVertCount].charBankOS = (vertex[oldVertCount].charBankOS & 0x03FFFFFFu) | vertexCharCount;
      }

      // Focus element's input buffer
      activeTextBuffer = element->input;
      // Make cursor visible and adjust position
      vertex[-1].align &= 0x07F;

      if(!(vertex[0].align & 0x02)) { // If text is not right-aligned
         /// !!! Reduce calc.time by using LAST.origin[0] + remainder chars !!! ///
         while(pCH[i]) accum += gui.alphabet_pIMM[pIMMos + pCH[i++]].width;

         vertex[-1].viewPos.x = accum * vertex[0].size.x;

         if((vertex[0].align & 0x03) == 0) // If text is centre-aligned
            vertex[-1].viewPos.x *= 0.5f;
         else { // Text is left-aligned
            vertex[-1].viewPos.x -= vertex[-2].size.x * 2.0f;
            if(vertex[-1].viewPos.x < -1.0f)
               vertex[-1].viewPos.x = -1.0f;
         }

         if(vertex[-1].viewPos.x > 1.0f)
            vertex[-1].viewPos.x = 1.0f;
      } else
         vertex[-1].viewPos.x = 1.0f;

      // Is the 'confirm' or 'cancel' flag set?
      if(textBuffer.source.bitField & 0x06) {
         textBuffer.source.bitField = 0x010;
         element[0].stateBits &= 0x0FB;
         //element[1].stateBits &= 0x0BF;
         vertex[-1].align |= 0x080;

         activeTextBuffer = 0;
      }
   }
}

// If not active, set element to receive input
void __Activate0_0_Default_Input(cptrc indices) {
   #define vertex2 ((*(cCLASS_GUI *)ptrLib[4]).element_dgs[((GUI_INDICES &)indices).vertex + 2])

   GUI_ELEMENT(&element)[3] = (GUI_ELEMENT(&)[3])(*(cCLASS_GUI *)ptrLib[4]).element[((GUI_INDICES &)indices).element];
   TEXTBUFFER  &textBuffer  = *element->input;

   if(!(textBuffer.source.bitField & 0x010))
      if(element[0].stateBits & 0x04) {
         if(!(textBuffer.source.bitField & 0x08)) {
            //element[2].charCount = textBufferInfo.source.byteOffset >> (element[2].bitField & 0x0800 ? 1 : 0);
            textBuffer.source.bitField |= 0x013;
         }
      } else {
         element[0].stateBits |= 0x04;
         //element[1].stateBits |= 0x040;

         textBuffer.source.pCH        =  &(*(cCLASS_GUI *)ptrLib[4]).textBuffer[(vertex2.charBankOS & 0x03FFFFFF) << 4];
         textBuffer.source.byteCount  =  element[2].vertexCount[1] << 5;
         textBuffer.source.byteOffset =  element[2].charCount << (element[2].bitField & 0x0800 ? 1 : 0);
         textBuffer.source.bitField   |= 0x09;
      }

   #undef vertex2
}

// If active, confirm change
void __Activate0_1_Default_Input(cptrc indices) {
   if((*(cCLASS_GUI *)ptrLib[4]).element[((cGUI_INDICES &)indices).element].stateBits & 0x04)
      (*(cCLASS_GUI *)ptrLib[4]).element[((cGUI_INDICES &)indices).element].input->source.bitField &= 0x0F7;
   else
      (*(cCLASS_GUI *)ptrLib[4]).element[((cGUI_INDICES &)indices).element].input->source.bitField &= 0x0EF;
}

// If active, cancel change
void __Activate1_0_Default_Input(cptrc indices) {
   if((*(cCLASS_GUI *)ptrLib[4]).element[((cGUI_INDICES &)indices).element].stateBits & 0x04)
      (*(cCLASS_GUI *)ptrLib[4]).element[((cGUI_INDICES &)indices).element].input->source.bitField = 0x05;
}

// Highlight .textEntry according to pointer position
void __Passive_Default_Array(cptrc indices) {
   cGUI_INDICES &data = (GUI_INDICES &)indices;

   GUI_ELEMENT   &element = (*(cCLASS_GUI *)ptrLib[4]).element[data.element];
   GUI_EL_DGSptrc vertex  = (GUI_EL_DGSptrc)&(*(cCLASS_GUI *)ptrLib[4]).element_dgs[data.vertex];


}

// Set active .textEntry if element not busy
void __Activate0_0_Default_Array(cptrc indices) {
   cGUI_INDICES &data = (GUI_INDICES &)indices;

   GUI_ELEMENT   &element = (*(cCLASS_GUI *)ptrLib[4]).element[data.element];
   GUI_EL_DGSptrc vertex  = (GUI_EL_DGSptrc)&(*(cCLASS_GUI *)ptrLib[4]).element_dgs[data.vertex];


}

// Set element to not busy
void __Activate0_1_Default_Array(cptrc indices) {
   cGUI_INDICES &data = (GUI_INDICES &)indices;

   GUI_ELEMENT   &element = (*(cCLASS_GUI *)ptrLib[4]).element[data.element];
   GUI_EL_DGSptrc vertex  = (GUI_EL_DGSptrc)&(*(cCLASS_GUI *)ptrLib[4]).element_dgs[data.vertex];


}

// Scroll list
void __Activate1_0_Default_Array(cptrc indices) {
   cGUI_INDICES &data = (GUI_INDICES &)indices;

   GUI_ELEMENT   &element = (*(cCLASS_GUI *)ptrLib[4]).element[data.element];
   GUI_EL_DGSptrc vertex  = (GUI_EL_DGSptrc)&(*(cCLASS_GUI *)ptrLib[4]).element_dgs[data.vertex];


}



/*
 *  User interfaces menus
 */

void MainMenu(cptrc argList) {
   static union {
      VEC2Du32 button[8];
      struct {
         VEC2Du32 buttonOptions;
         VEC2Du32 buttonEditors;
         VEC2Du32 buttonExit;
      };
   };

   static bool firstRun = true;

   const struct INPUT_INDICES {
      si16 soundBank;
      si16 alphabet;
      ui16 spriteLib;
      ui16 panelSprite;
   } &index = (const INPUT_INDICES &)argList;

   CLASS_GUI &gui = *(CLASS_GUI *)ptrLib[4];

   ui32 interfaceMain;

   if(firstRun) {
      GUI_EL_DESC elementDesc;

      elementDesc.viewPos = { 0.0f, -0.25f };
      elementDesc.index   = { index.soundBank, index.alphabet, index.spriteLib, index.panelSprite };
      elementDesc.panel.tint  = c4_white;
      elementDesc.panel.size  = { 0.125f, 0.125f };
      elementDesc.panel.align = UI_ALIGN_L;
      elementDesc.panel.mods  = UI_NONE;
      elementDesc.text.tint   = c4_white;
      elementDesc.text.size   = { 1.0f, 1.0f };
      elementDesc.text.cPtr   = (chptr)"Options";
      elementDesc.text.cCount = 7;
      elementDesc.text.align  = UI_ALIGN_C;
      elementDesc.text.mods   = UI_ROT | UI_TRANS | UI_SCALE_X;
      buttonOptions = gui.CreateTextButton(elementDesc);

      elementDesc.viewPos = { 0.0f, -0.5f };
      elementDesc.text.cPtr = (chptr)"Editors";
      buttonEditors = gui.CreateTextButton(elementDesc);

      elementDesc.viewPos = { 0.0f, 0.0f };
      elementDesc.text.cPtr   = (chptr)"Exit";
      elementDesc.text.cCount = 4;
      elementDesc.panel.align = UI_ALIGN_L | UI_ALIGN_B;
      buttonExit = gui.CreateTextButton(elementDesc);

      interfaceMain = gui.CreateInterface(128, 16);
      gui.AddElementToInterface(buttonOptions, interfaceMain);
      gui.AddElementToInterface(buttonEditors, interfaceMain);
      gui.AddElementToInterface(buttonExit, interfaceMain);

      (*(GUI_DESC *)ptrLib[15]).defaultInterface = interfaceMain;

      firstRun = false;
   }


}

/*
 *  Editor menus
 */

void GUI_Editor(cptrc argList) {
   cCLASS_GUI &gui = *(cCLASS_GUI *)ptrLib[4];

}

/*
 *  Game menus
 */

void PauseMenu(cptrc argList) {          // 5th
   cCLASS_GUI &gui = *(cCLASS_GUI *)ptrLib[4];
}

void AudioMenu(cptrc argList) {          // 4th
   cCLASS_GUI &gui = *(cCLASS_GUI *)ptrLib[4];
}

void VideoMenu(cptrc argList) {          // 2nd
   cCLASS_GUI &gui = *(cCLASS_GUI *)ptrLib[4];
}

void ControlsMenu(cptrc argList) {       // 1st
   cCLASS_GUI &gui = *(cCLASS_GUI *)ptrLib[4];


}

void GameplayMenu(cptrc argList) {       // 7th
   cCLASS_GUI &gui = *(cCLASS_GUI *)ptrLib[4];
}

void OptionsMenu(cptrc argList) {        // 6th
   cCLASS_GUI &gui = *(cCLASS_GUI *)ptrLib[4];
}

void DeveloperInterface(cptrc argList) { // 3rd
   cCLASS_GUI &gui = *(cCLASS_GUI *)ptrLib[4];
}
