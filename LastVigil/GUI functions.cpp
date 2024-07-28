/************************************************************
 * File: GUI functions.cpp              Created: 2023/06/13 *
 *                                Last modified: 2024/07/25 *
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

static ui16 interfaceNew;
static ui16 interfaceSave;
static ui16 interfaceLoad;
static ui16 interfaceOptions;
static ui16 interfaceEditors;
static ui16 interfaceExit;
static ui16 interfaceUIEditor;

void __OnHover_Default(cptrc indices) {
   cGUI_INDICES &data = (GUI_INDICES &)indices;

}

void __OffHover_Default(cptrc indices) {
   cGUI_INDICES &data = (GUI_INDICES &)indices;

}

// Set activated button state
void __Activate0_0_Default_Button(cptrc indices) {
   cGUI_INDICES &data = (GUI_INDICES &)indices;

   GUI_ELEMENT &element    = (*(cCLASS_GUIptrc)ptrLib[4]).element[data.element];
   GUI_EL_DGS (&vertex)[2] = (GUI_EL_DGS (&)[2])(*(cCLASS_GUIptrc)ptrLib[4]).element_dgs[data.vertex];

   if(!(element.stateBits & UIE_BUSY0)) {
      element.bitField ^= 0x0480;
      vertex[0].align ^= UI_INVIS;
      vertex[1].align ^= UI_INVIS;
   }
}

// Set deactivated button state
void __Activate0_1_Default_Button(cptrc indices) {
   cGUI_INDICES &data = (GUI_INDICES &)indices;

   GUI_ELEMENT &element    = (*(cCLASS_GUIptrc)ptrLib[4]).element[data.element];
   GUI_EL_DGS (&vertex)[2] = (GUI_EL_DGS (&)[2])(*(cCLASS_GUIptrc)ptrLib[4]).element_dgs[data.vertex];

   if(element.stateBits & UIE_BUSY0) {
      element.bitField ^= 0x0480;
      vertex[0].align ^= UI_INVIS;
      vertex[1].align ^= UI_INVIS;
   }
}

// Show flipped button state
void __Activate0_0_Default_Toggle(cptrc indices) {
   cGUI_INDICES &data = (GUI_INDICES &)indices;

   GUI_ELEMENT &element    = (*(cCLASS_GUIptrc)ptrLib[4]).element[data.element];
   GUI_EL_DGS (&vertex)[2] = (GUI_EL_DGS (&)[2])(*(cCLASS_GUIptrc)ptrLib[4]).element_dgs[data.vertex];

   if(!(element.stateBits & UIE_BUSY0)) {
      element.bitField ^= 0x0480;
      vertex[0].align ^= UI_INVIS;
      vertex[1].align ^= UI_INVIS;
   }
}

// Flip button state
void __Activate0_1_Default_Toggle(cptrc indices) {
   cGUI_INDICES &data = (GUI_INDICES &)indices;

   GUI_ELEMENT &element = (*(cCLASS_GUIptrc)ptrLib[4]).element[data.element];

   if(element.stateBits & UIE_BUSY0) element.bitField ^= 0x0480;
}

// Move scalar relative to pointer
void __Activate0_0_Default_Scalar(cptrc indices) {
   cGUI_INDICES &data = (GUI_INDICES &)indices;

   GUI_ELEMENT &element    = (*(cCLASS_GUIptrc)ptrLib[4]).element[data.element];
   GUI_EL_DGS (&vertex)[2] = (GUI_EL_DGS (&)[2])(*(cCLASS_GUIptrc)ptrLib[4]).element_dgs[data.vertex];

   if(!(element.stateBits & UIE_BUSY0)) {
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

   GUI_ELEMENT &element = (*(cCLASS_GUIptrc)ptrLib[4]).element[data.element];

   if(element.stateBits & UIE_BUSY0) element.stateBits &= ~UIE_BUSY0;
}

// Set scalar to pointer position
void __Activate1_0_Default_Scalar(cptrc indices) {
   cGUI_INDICES &data = (GUI_INDICES &)indices;

   GUI_ELEMENT &element    = (*(cCLASS_GUIptrc)ptrLib[4]).element[data.element];
   GUI_EL_DGS (&vertex)[2] = (GUI_EL_DGS (&)[2])(*(cCLASS_GUIptrc)ptrLib[4]).element_dgs[data.vertex];

   if(!(element.stateBits & UIE_BUSY1))
      element.stateBits |= UIE_BUSY1;
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
void __Activate1_1_Default(cptrc indices) {
   cGUI_INDICES &data = (GUI_INDICES &)indices;

   GUI_ELEMENT &element = (*(cCLASS_GUIptrc)ptrLib[4]).element[data.element];

   if(element.stateBits & UIE_BUSY1)
      element.stateBits &= ~UIE_BUSY1;
}

// Reprocess string
void __Passive_Default_Input(cptrc indices) {
   GUI_ELEMENT (&element)[3] = (GUI_ELEMENT (&)[3])(*(cCLASS_GUIptrc)ptrLib[4]).element[((GUI_INDICES &)indices).element];

   if(element[0].stateBits & UIE_BUSY0) {
      cCLASS_GUI &gui        = *(cCLASS_GUIptrc)ptrLib[4];
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
         vertex[oldVertCount].coords.ymm = null256f;
         vertex[oldVertCount].charBankOS = vertex[oldVertCount].charBankOS & 0x03FFFFFFu;
         vertex[oldVertCount].align |= UI_INVIS;
      } else {
         cui32 vertexCharCount = ((((element[2].charCount - 1) & 31) + 1) << 26);
         if(newVertCount > oldVertCount) {
            vertex[oldVertCount].charBankOS    = (vertex[oldVertCount].charBankOS & 0x03FFFFFFu) | 0x080000000u;
            vertex[newVertCount].origin[0]     = (*(cCLASS_GUIptrc)ptrLib[4]).CalculateTextOrigin(indexVert, indexVert + newVertCount, vertex[0].alphabetIndex);
            vertex[newVertCount].origin[1]     = { 16384.0f, 16384.0f };
            vertex[newVertCount].coords.xmm[1] = _mm_set_ps1(16384.0f);
            vertex[newVertCount].charBankOS    = (vertex[newVertCount].charBankOS & 0x03FFFFFFu) | vertexCharCount;
            vertex[newVertCount].align        &= ~UI_INVIS;
         } else
            vertex[oldVertCount].charBankOS = (vertex[oldVertCount].charBankOS & 0x03FFFFFFu) | vertexCharCount;
      }

      // Focus element's input buffer
      activeTextBuffer = element->input;
      // Make cursor visible and adjust position
      vertex[-1].align &= ~UI_INVIS;

      if(!(vertex[0].align & UI_ALIGN_R)) {
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
         vertex[-1].align |= UI_INVIS;

         activeTextBuffer = 0;
      }
   }
}

// If not active, set element to receive input
void __Activate0_0_Default_Input(cptrc indices) {
   #define vertex2 ((*(cCLASS_GUIptrc)ptrLib[4]).element_dgs[((GUI_INDICES &)indices).vertex + 2])

   GUI_ELEMENT(&element)[3] = (GUI_ELEMENT(&)[3])(*(cCLASS_GUIptrc)ptrLib[4]).element[((GUI_INDICES &)indices).element];
   TEXTBUFFER  &textBuffer  = *element->input;

   if(!(textBuffer.source.bitField & 0x010))
      if(element[0].stateBits & UIE_BUSY0) {
         if(!(textBuffer.source.bitField & 0x08)) {
            //element[2].charCount = textBufferInfo.source.byteOffset >> (element[2].bitField & 0x0800 ? 1 : 0);
            textBuffer.source.bitField |= 0x013;
         }
      } else {
         element[0].stateBits |= UIE_BUSY0;
         //element[1].stateBits |= 0x040;

         textBuffer.source.pCH        =  &(*(cCLASS_GUIptrc)ptrLib[4]).textBuffer[(vertex2.charBankOS & 0x03FFFFFF) << 4];
         textBuffer.source.byteCount  =  element[2].vertexCount[1] << 5;
         textBuffer.source.byteOffset =  element[2].charCount << (element[2].bitField & 0x0800 ? 1 : 0);
         textBuffer.source.bitField   |= 0x09;
      }

   #undef vertex2
}

// If active, confirm change
void __Activate0_1_Default_Input(cptrc indices) {
   GUI_ELEMENT &element = (*(cCLASS_GUIptrc)ptrLib[4]).element[((cGUI_INDICES &)indices).element];

   if(element.stateBits & UIE_BUSY0) element.input->source.bitField &= 0x0F7;
   else element.input->source.bitField &= 0x0EF;
}

// If active, cancel change
void __Activate1_0_Default_Input(cptrc indices) {
   if((*(cCLASS_GUIptrc)ptrLib[4]).element[((cGUI_INDICES &)indices).element].stateBits & UIE_BUSY0)
      (*(cCLASS_GUIptrc)ptrLib[4]).element[((cGUI_INDICES &)indices).element].input->source.bitField = 0x05;
}

// Highlight .textEntry according to pointer position
void __OnHover_Default_Array(cptrc indices) {
   cGUI_INDICES &data = (GUI_INDICES &)indices;

   GUI_ELEMENT (&element)[3] = (GUI_ELEMENT(&)[3])(*(cCLASS_GUIptrc)ptrLib[4]).element[data.element];
   GUI_EL_DGS  (&vertex)[2] = (GUI_EL_DGS(&)[2])(*(cCLASS_GUIptrc)ptrLib[4]).element_dgs[data.vertex + 1u];

   cfl32 offset = (element[2].textEntry >= 0
      ? fl32(element[2].textEntry)
      : truncf((((gcv.scrCoord.y - element[0].activePos.y) * element[0].viewScale.y) + 1.0f + vertex[1].listPos) / element[0].lSpacing));

   vertex[0].viewPos.y = element[0].lSpacing * offset - vertex[1].listPos;
   vertex[0].align &= ~UI_INVIS;
   if((vertex[0].viewPos.y < 0.0f) || (vertex[0].viewPos.y > 2.0f - element[0].lSpacing))
      vertex[0].align |= UI_INVIS;
}

// Remove highlight if no active entry
void __OffHover_Default_Array(cptrc indices) {
   cGUI_INDICES &data = (GUI_INDICES &)indices;

   GUI_ELEMENT &element = (GUI_ELEMENT &)(*(cCLASS_GUIptrc)ptrLib[4]).element[data.element + 2u];
   GUI_EL_DGS  &vertex  = (GUI_EL_DGS &)(*(cCLASS_GUIptrc)ptrLib[4]).element_dgs[data.vertex + 1u];

   if(element.textEntry < 0) vertex.align |= UI_INVIS;
}

// Set active .textEntry if element not busy
void __Activate0_0_Default_Array(cptrc indices) {
   cGUI_INDICES &data = (GUI_INDICES &)indices;

   GUI_ELEMENT(&element)[3] = (GUI_ELEMENT(&)[3])(*(cCLASS_GUIptrc)ptrLib[4]).element[data.element];
   GUI_EL_DGS  &vertex      = (GUI_EL_DGS &)(*(cCLASS_GUIptrc)ptrLib[4]).element_dgs[data.vertex + 2u];

   if(!(element[0].stateBits & UIE_BUSY0)) {
      element[0].bitField ^= 0x0480;
      csi16 entry = si16((((gcv.scrCoord.y - element[0].activePos.y) * element[0].viewScale.y) + 1.0f + vertex.listPos) / element[0].lSpacing);
      if(element[2].textEntry == entry) {
         element[2].textEntry = -1;
         return;
      }
      element[2].textEntry = entry;
   }
}

// Set element to not busy
void __Activate0_1_Default_Array(cptrc indices) {
   cGUI_INDICES &data = (GUI_INDICES &)indices;

   GUI_ELEMENT &element = (*(cCLASS_GUIptrc)ptrLib[4]).element[data.element];

   if(element.stateBits & UIE_BUSY0) element.stateBits &= ~UIE_BUSY0;
}

// Scroll list
void __Activate1_0_Default_Array(cptrc indices) {
   cGUI_INDICES &data = (GUI_INDICES &)indices;

   GUI_ELEMENT &element = (*(cCLASS_GUIptrc)ptrLib[4]).element[data.element];
   //GUI_EL_DGS  &vertex  = (GUI_EL_DGS &)(*(cCLASS_GUIptrc)ptrLib[4]).element_dgs[data.vertex + 2u];
   GUI_EL_DGS (&vertex)[2] = (GUI_EL_DGS (&)[2])(*(cCLASS_GUIptrc)ptrLib[4]).element_dgs[data.vertex + 1u];

   ///--- Drag-scroll entries
   if(!(element.stateBits & UIE_BUSY1)) {
      element.stateBits |= UIE_BUSY1;
      element.scalarPos = gcv.curCoords.y;
   } else {
      csi32 curCoord   = gcv.curCoords.y;
      cfl32 difference = fl32(curCoord - element.scalarPos) / ScrRes.dims[ScrRes.state].dim[1] * element.viewScale.y * 0.125f;

      vertex[0].viewPos.y -= difference;
      vertex[1].listPos   += difference;

      if(vertex[1].listPos < -1.0f)
         vertex[1].listPos = -1.0f;
      else if(vertex[1].listPos > 1.0f)
         vertex[1].listPos = 1.0f;

      if((vertex[0].viewPos.y < 0.0f) || (vertex[0].viewPos.y > 2.0f - element.lSpacing))
         vertex[0].align |= UI_INVIS;
   }
}



/*
 *  User interface menu functions
 */

void __Activate0_1_New_Button(cptrc indices) {
   __Activate0_1_Default_Button(indices);
   (*(GUI_DESCptrc)ptrLib[15]).SetNextInterface(interfaceNew, 0.25f, 0.25f, UIPT_FADE);
}

void __Activate0_1_Save_Button(cptrc indices) {
   __Activate0_1_Default_Button(indices);
   (*(GUI_DESCptrc)ptrLib[15]).SetNextInterface(interfaceSave, 0.25f, 0.25f, UIPT_FADE);
}

void __Activate0_1_Load_Button(cptrc indices) {
   __Activate0_1_Default_Button(indices);
   (*(GUI_DESCptrc)ptrLib[15]).SetNextInterface(interfaceLoad, 0.25f, 0.25f, UIPT_FADE);
}

void __Activate0_1_Editors_Button(cptrc indices) {
   __Activate0_1_Default_Button(indices);
   (*(GUI_DESCptrc)ptrLib[15]).SetNextInterface(interfaceEditors, 0.25f, 0.25f, UIPT_FADE);
}

void __Activate0_1_Options_Button(cptrc indices) {
   __Activate0_1_Default_Button(indices);
   (*(GUI_DESCptrc)ptrLib[15]).SetNextInterface(interfaceOptions, 0.25f, 0.25f, UIPT_FADE);
}

void __Activate0_1_Exit_Button(cptrc indices) {
   __Activate0_1_Default_Button(indices);
   (*(GUI_DESCptrc)ptrLib[15]).SetNextInterface(interfaceExit, 0.125f, 0.125f, UIPT_OVERLAY | UIPT_FADE);
}

void __Activate0_1_UI_Editor_Button(cptrc indices) {
   __Activate0_1_Default_Button(indices);
   (*(GUI_DESCptrc)ptrLib[15]).SetNextInterface(interfaceUIEditor, 0.25f, 0.25f, UIPT_FADE);
   ///--- Signal worldgen thread to execute editor environment for UI
}

/*
 *  User interface menus
 *
 * Temporary; remove all after menus are completed
 */

// ArgList == VEC4Du16(soundBank, alphabet, spriteLib, panelSprite)
void TempMenuGen(cptrc argList) {
   static union {
      VEC2Du32 element[8];
      struct {
         VEC2Du32 buttonNew;
         VEC2Du32 buttonSave;
         VEC2Du32 buttonLoad;
         VEC2Du32 buttonOptions;
         VEC2Du32 buttonEditors;
         VEC2Du32 buttonExit;
      };
   };

   static bool firstRun = true;

   const struct INPUT_INDICES {
      ui16 soundBank;
      si16 alphabet;
      ui16 spriteLib;
      ui16 panelSprite;
   } &index = (const INPUT_INDICES &)argList;

   CLASS_GUI &gui = *(CLASS_GUIptrc)ptrLib[4];

   if(firstRun) {
      GUI_EL_DESC elementDesc;

///--- Main Menu
      elementDesc.viewPos = { 0.0f, -0.25f };
      elementDesc.index   = { index.soundBank, index.alphabet, index.spriteLib, index.panelSprite };
      elementDesc.panel.tint   = c4_white;
      elementDesc.panel.size   = { 0.0625f, 0.3125f };
      elementDesc.panel.bStyle = 0;
      elementDesc.panel.align  = UI_ALIGN_BL;
      elementDesc.panel.mods   = UI_NONE;
      elementDesc.text.tint   = c4_white;
      elementDesc.text.size   = { 1.0f, 1.0f };
      elementDesc.text.cPtr   = (chptr)"New game";
      elementDesc.text.cCount = (si32)strlen(elementDesc.text.cPtr);
      elementDesc.text.align  = UI_ALIGN_C;
      elementDesc.text.mods   = UI_ROT | UI_TRANS | UI_SCALE_X;
      elementDesc.func.activate0[1] = Activate01NewButton;
      buttonNew = gui.CreateTextButton(elementDesc);

      elementDesc.viewPos = { 0.0625f, 0.25f };
      elementDesc.text.cPtr   = (chptr)"Save game";
      elementDesc.text.cCount = (si32)strlen(elementDesc.text.cPtr);
      elementDesc.func.activate0[1] = Activate01SaveButton;
      buttonSave = gui.CreateTextButton(elementDesc);

      elementDesc.viewPos = { 0.0625f, 0.1875f };
      elementDesc.text.cPtr   = (chptr)"Load game";
      elementDesc.text.cCount = (si32)strlen(elementDesc.text.cPtr);
      elementDesc.func.activate0[1] = Activate01LoadButton;
      buttonLoad = gui.CreateTextButton(elementDesc);

      elementDesc.viewPos = { 0.0625f, 0.125f };
      elementDesc.text.cPtr   = (chptr)"Options";
      elementDesc.text.cCount = (si32)strlen(elementDesc.text.cPtr);
      elementDesc.func.activate0[1] = Activate01OptionsButton;
      buttonOptions = gui.CreateTextButton(elementDesc);

      elementDesc.viewPos = { 0.0625f, 0.0625f };
      elementDesc.text.cPtr   = (chptr)"Editors";
      elementDesc.text.cCount = (si32)strlen(elementDesc.text.cPtr);
      elementDesc.func.activate0[1] = Activate01EditorsButton;
      buttonEditors = gui.CreateTextButton(elementDesc);

      elementDesc.viewPos = { 0.03125f, 0.03125f };
      elementDesc.index   = { index.soundBank, index.alphabet, index.spriteLib, 38u };
      elementDesc.text.cPtr   = (chptr)"Exit";
      elementDesc.text.cCount = (si32)strlen(elementDesc.text.cPtr);
      elementDesc.func.activate0[1] = Activate01ExitButton;
      buttonExit = gui.CreateTextButton(elementDesc);

      interfaceNew = gui.CreateInterface("Main Menu", -1, 6u, 2u);
      gui.AddToInterfaceInput(interfaceNew, 0, "Primary input", aei_mouse_button_1);
      gui.AddElementToInterface(buttonNew, interfaceNew);
      gui.AddElementToInterface(buttonSave, interfaceNew);
      gui.AddElementToInterface(buttonLoad, interfaceNew);
      gui.AddElementToInterface(buttonOptions, interfaceNew);
      gui.AddElementToInterface(buttonEditors, interfaceNew);
      gui.AddElementToInterface(buttonExit, interfaceNew);

      gui.SaveInterface(L"MainMenu", interfaceNew);
///--- Main Menu

///--- Editors (UI, AI, Map, Entity, Mission, ???)
      elementDesc.viewPos = { 0.0f, 0.125f };
      elementDesc.index   = { index.soundBank, index.alphabet, index.spriteLib, index.panelSprite };
      elementDesc.panel.tint     = { 0.7071f, 0.7071f, 0.7071f, 1.0f };
      elementDesc.panel.size     = { 1.0f, 1.0f };
      elementDesc.panel.bStyle   = 1u;
      elementDesc.panel.bSize    = 0.0125f;
      elementDesc.panel.wOpacity = 0.2929f;
      elementDesc.panel.wIndent  = { 1.0f, 1.0f };
      elementDesc.panel.align    = UI_ALIGN_T;
      elementDesc.text.cPtr   = (chptr)"Editors";
      elementDesc.text.cCount = (si32)strlen(elementDesc.text.cPtr);
      element[0] = gui.CreateTextPanel(elementDesc);

      elementDesc.viewPos = { 0.0625f, 0.3125f };
      elementDesc.index   = { index.soundBank, index.alphabet, index.spriteLib, index.panelSprite };
      elementDesc.panel.tint     = c4_white;
      elementDesc.panel.bStyle   = 0;
      elementDesc.panel.bSize    = 0.0f;
      elementDesc.panel.wOpacity = 1.0;
      elementDesc.panel.align    = UI_ALIGN_BL;
      elementDesc.text.cPtr   = (chptr)"User interface";
      elementDesc.text.cCount = (si32)strlen(elementDesc.text.cPtr);
      elementDesc.func.activate0[1] = Activate01UIEditorButton;
      element[1] = gui.CreateTextButton(elementDesc);

      elementDesc.viewPos = { 0.0625f, 0.25f };
      elementDesc.text.cPtr   = (chptr)"Artificial Intelligence";
      elementDesc.text.cCount = (si32)strlen(elementDesc.text.cPtr);
      element[2] = gui.CreateTextButton(elementDesc);

      elementDesc.viewPos = { 0.0625f, 0.1875f };
      elementDesc.text.cPtr   = (chptr)"Entity";
      elementDesc.text.cCount = (si32)strlen(elementDesc.text.cPtr);
      element[3] = gui.CreateTextButton(elementDesc);

      elementDesc.viewPos = { 0.0625f, 0.125f };
      elementDesc.text.cPtr   = (chptr)"Map";
      elementDesc.text.cCount = (si32)strlen(elementDesc.text.cPtr);
      element[4] = gui.CreateTextButton(elementDesc);

      elementDesc.viewPos = { 0.0625f, 0.0625f };
      elementDesc.text.cPtr   = (chptr)"Mission";
      elementDesc.text.cCount = (si32)strlen(elementDesc.text.cPtr);
      element[5] = gui.CreateTextButton(elementDesc);

      elementDesc.viewPos = { 0.03125f, 0.03125f };
      elementDesc.text.cPtr   = (chptr)"Main menu";
      elementDesc.text.cCount = (si32)strlen(elementDesc.text.cPtr);
      element[6] = gui.CreateTextButton(elementDesc);

      interfaceEditors = gui.CreateInterface("Editors menu", -1, 7u, 2u);
      gui.AddToInterfaceInput(interfaceEditors, 0, "Primary input", aei_mouse_button_1);
      gui.AddElementToInterface(element[0], interfaceEditors);
      gui.AddElementToInterface(element[1], interfaceEditors);
      gui.AddElementToInterface(element[2], interfaceEditors);
      gui.AddElementToInterface(element[3], interfaceEditors);
      gui.AddElementToInterface(element[4], interfaceEditors);
      gui.AddElementToInterface(element[5], interfaceEditors);
      gui.AddElementToInterface(element[6], interfaceEditors);
///--- Editors

///--- User interface editor
      // Panel for element creation options
      cchptrc listOptionsElements[8] = { "Text", "Text array", "Panel", "Button", "Toggle", "Scalar", "Cursor", "Dial" };
      elementDesc.viewPos = { 0.0f, 0.0f };
      elementDesc.index   = { 0, index.alphabet, 0, 0, 0 };
      elementDesc.panel.tint     = c4_white;
      elementDesc.panel.size     = { 0.0625f, 0.125f };
      elementDesc.panel.bStyle   = 1u;
      elementDesc.panel.bSize    = 0.0625f;
      elementDesc.panel.wIndent  = { 0.0625f, 0.03125f };
      elementDesc.panel.wOpacity = 0.2929f;
      elementDesc.panel.align    = UI_ALIGN_C;
      elementDesc.panel.mods     = UI_PANEL;
      elementDesc.cursor.tint     = c4_white;
      elementDesc.cursor.size     = { 1.0f, 1.0f };
      elementDesc.cursor.bStyle   = 1u;
      elementDesc.cursor.bSize    = 0.125f;
      elementDesc.cursor.wOpacity = 0.2929f;
      elementDesc.text.tint     = c4_white;
      elementDesc.text.size     = { 0.24f, 0.24f };
      elementDesc.text.cArray   = (chptrptr)listOptionsElements;
      elementDesc.text.sCount   = 8;
      elementDesc.text.lSpacing = 1.375f;
      elementDesc.text.align    = UI_ALIGN_TL;
      elementDesc.text.mods     = UI_TEXTARRAY;
      cVEC3Du32 element0 = gui.CreateTextSelectionPanel(elementDesc);
      // Panel for element properties
      /// Create overlapping & conditionally visible elements

      // Panel for vertex properties
      /// Create overlapping & conditionally visible elements

///--- User interface editor

      firstRun = false;
   }


}

/*
 *  Editor menus
 */

void Editor_GUI(cptrc argList) {
   cCLASS_GUI &gui = *(cCLASS_GUIptrc)ptrLib[4];

}

/*
 *  Game menus
 */

void MenuPause(cptrc argList) {          // 5th
   cCLASS_GUI &gui = *(cCLASS_GUIptrc)ptrLib[4];
}

void MenuAudio(cptrc argList) {          // 4th
   cCLASS_GUI &gui = *(cCLASS_GUIptrc)ptrLib[4];
}

void MenuVideo(cptrc argList) {          // 2nd
   cCLASS_GUI &gui = *(cCLASS_GUIptrc)ptrLib[4];
}

void MenuControls(cptrc argList) {       // 1st
   cCLASS_GUI &gui = *(cCLASS_GUIptrc)ptrLib[4];


}

void MenuGameplay(cptrc argList) {       // 7th
   cCLASS_GUI &gui = *(cCLASS_GUIptrc)ptrLib[4];
}

void MenuOptions(cptrc argList) {        // 6th
   cCLASS_GUI &gui = *(cCLASS_GUIptrc)ptrLib[4];
}

void InterfaceDeveloper(cptrc argList) { // 3rd
   cCLASS_GUI &gui = *(cCLASS_GUIptrc)ptrLib[4];
}
