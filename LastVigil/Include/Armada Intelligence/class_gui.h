/*******************************************************************************  
 * File: class_gui.h                                       Created: 2023/01/26 *
 *                                                   Last modified: 2024/07/08 *
 *                                                                             *
 * Desc:                                                                       *
 *                                                                             *
 * 2024/04/01: GUI_EL_DYN SRV+Adj.coords replaced by UAV                       *
 * 2024/04/17: Modified all 'Create...' functions to use data struct for input *
 * 2024/04/25: Modified .ProcessInputs() to use custom Input mappings          *
 *                                                                             *
 * Copyright (c) David William Bull.                      All rights reserved. *
 *******************************************************************************/
#pragma once

#include "..\master header.h"
#include "Data structures.h"
#include "GUI structures.h"
#include "GUI functions.h"
#include "Input functions.h"
#include "File operations.h"
#include "class_render.h"

al32 struct CLASS_GUI {
   const struct GUI_IO_DATA {
      ui16ptrc soundBank;
      ui8ptrc  atlas;
      ui32ptrc function;
      ui32ptrc charBankOS;

//      GUI_IO_DATA(cui32 maxSoundBanks, cui32 maxAtlas, cui32 maxFunctions) :
//         soundBank(salloc1d16(ui16, (maxSoundBanks * 2u) + maxAtlas + (maxFunctions * 8u), max128)),
//         atlas((ui8ptr)soundBank + maxSoundBanks),
//         function((ui32ptr)atlas + maxAtlas)
//      {}
      GUI_IO_DATA(cui32 maxElements, cui32 maxTextBlocks) :
         soundBank(salloc1d16(ui16, (maxElements * 2u) + maxElements + (maxElements * 4u * 6u) + (maxTextBlocks * 4u), max128)),
         atlas((ui8ptr)soundBank + maxElements),
         function((ui32ptr)atlas + maxElements),
         charBankOS((ui32ptr)function + maxElements)
      {}

      ~GUI_IO_DATA() { mfree(soundBank, atlas, function); }
   };

   CLASS_GPU     &gpu;
   CLASS_FILEOPS &files;

   declare1d64z(FUNCTION,       funcLib,          MAX_GUI_FUNCTIONS);              // Master library of functions & their display name
   declare1d64z(ALPHABET,       alphabet,         MAX_ALPHABETS);                  // 32KB per 1,024 characters
   declare1d64z(CHAR_IMM,       alphabet_pIMM,    MAX_ALPHABETS * MAX_CHARACTERS); // GPU-bound alphabet data
   declare1d64z(GUI_SPRITE_LIB, spriteLib,        MAX_GUI_SPRITE_LIBS);            // Master array of sprite libraries
   declare1d64z(GUI_INTERFACE,  interfaceProfile, MAX_INTERFACES);                 // Master array of interface profiles
   declare1d64s(GUI_ELEMENT,    element,          MAX_GUI_ELEMENTS, max128);       // 96K per 1,024 elements
   declare1d64z(GUI_EL_DGS,     element_dgs,      MAX_GUI_ENTRIES);                // GPU-bound element data

   declare1d64z(ui32, totalVertexList, MAX_GUI_ENTRIES);
   declare1d64s(ui32, curVertexCount,  MAX_INTERFACES, max128);
   declare1d64z(ui16, uiAtlasTexIndex, MAX_ATLAS);
   declare1d64z(char, textBuffer,      MAX_TEXT_BUFFER);

   declare2d64z(ui32,  curVertexList, MAX_INTERFACES, MAX_INTERFACE_VERTS);
   declare2d64z(wchar, stAtlas,       MAX_ATLAS,      MAX_NAME_SIZE);
   declare2d64z(char,  stLanguage,    MAX_ALPHABETS,  MAX_NAME_SIZE);

   cwchptrc stGUIDir = L"ui_elements\\";

   // 28 bytes spare (4 bytes beyond alignment)

   struct {
      union { cui16 bufAlphabet; ui16 _bufAlphabet; };
      union { cui16 bufText;     ui16 _bufText; };
      union { cui16 bufVertex;   ui16 _bufVertex; };
      union { cui16 bufDGS;      ui16 _bufDGS; };
   };
   
   si32 siGUIElements = 0;
   ui32 uiGUIVerts    = 0;
   si32 siTextBankOS  = 0;
   si16 siInterfaces  = 0;
   ui16 uiFunctions   = 0;
   ui16 uiLanguages   = 0;
   ui16 uiAtlas       = 0;

   AVX8Ds32 activeElement = { .ymm = { max256 } };

private:
   inline void _CLASS_GUI_INIT(void) {
      _bufDGS      = gpu.buf.CreateStructured(0, element_dgs, sizeof(GUI_EL_DGS), MAX_GUI_ENTRIES, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS, ae_buf_default);
      _bufAlphabet = gpu.buf.CreateStructured(0, alphabet_pIMM, sizeof(CHAR_IMM), MAX_ALPHABETS * MAX_CHARACTERS, ae_buf_dynamic);
      _bufText     = gpu.buf.CreateStructured(0, textBuffer, 16, MAX_TEXT_BUFFER >> 4, ae_buf_dynamic);
      _bufVertex   = gpu.buf.CreateVertex(curVertexList, sizeof(ui32), MAX_GUI_ENTRIES, 1);

      // Add default UI functions to master library
      funcLib[0]  = { 0,                            "Null" };
      funcLib[1]  = { __OnHover_Default,            stOnHoverDefault };
      funcLib[2]  = { __OffHover_Default,           stOffHoverDefault };
      funcLib[3]  = { __Activate0_0_Default_Button, stActivate00DefaultButton };
      funcLib[4]  = { __Activate0_1_Default_Button, stActivate01DefaultButton };
      funcLib[5]  = { __Activate0_0_Default_Toggle, stActivate00DefaultToggle };
      funcLib[6]  = { __Activate0_1_Default_Toggle, stActivate01DefaultToggle };
      funcLib[7]  = { __Activate0_0_Default_Scalar, stActivate00DefaultScalar };
      funcLib[8]  = { __Activate0_1_Default_Scalar, stActivate01DefaultScalar };
      funcLib[9]  = { __Activate1_0_Default_Scalar, stActivate10DefaultScalar };
      funcLib[10] = { __Activate1_1_Default_Scalar, stActivate11DefaultScalar };
      funcLib[11] = { __Passive_Default_Input,      stPassiveDefaultInput };
      funcLib[12] = { __Activate0_0_Default_Input,  stActivate00DefaultInput };
      funcLib[13] = { __Activate0_1_Default_Input,  stActivate01DefaultInput };
      funcLib[14] = { __Activate1_0_Default_Input,  stActivate10DefaultInput };
      funcLib[15] = { __Passive_Default_Array,      stActivate10DefaultArray };
      funcLib[16] = { __Activate0_0_Default_Array,  stActivate00DefaultArray };
      funcLib[17] = { __Activate0_1_Default_Array,  stActivate01DefaultArray };
      funcLib[18] = { __Activate1_0_Default_Array,  stActivate10DefaultArray };
   }
public:

   CLASS_GUI(CLASS_FILEOPS &fileOps, CLASS_GPU &gpuClass) : files(fileOps), gpu(gpuClass) { _CLASS_GUI_INIT(); }
   CLASS_GUI(CLASS_FILEOPS &fileOps, CLASS_GPU &gpuClass, cptrptr globalPointer) : files(fileOps), gpu(gpuClass) { _CLASS_GUI_INIT(); if(globalPointer) *globalPointer = this; }

   ~CLASS_GUI(void) {
      ui32 i;

      if(element) mdealloc(element);
      if(element_dgs) mdealloc(element_dgs);
//      if(vertexRunLength) mfree(element_dgs);
      if(textBuffer) mdealloc(textBuffer);
      for(i = MAX_ATLAS - 1; i; --i)
         if(uiAtlasTexIndex[i])
            gpu.tex.Unload2D(uiAtlasTexIndex[i]);
//      mfree(vertex_index);
      for(i = siInterfaces - 1; i; --i)
         if(interfaceProfile[i].vertex)
            mdealloc(interfaceProfile[i].vertex);
      mfree(interfaceProfile, stAtlas, stLanguage, alphabet, alphabet_pIMM, spriteLib);
      uiLanguages = uiAtlas = 0;
   }

   // Returns index of function
   cui32 AddFunctionToLibrary(FUNCTION funcInfo) {
      if(uiFunctions < MAX_GUI_FUNCTIONS) funcLib[uiFunctions] = funcInfo;

      return uiFunctions++;
   }

   cui32 CreateAtlas(cwchptrc atlasName, si16 atlasIndex, csi16 textureIndex) {
      if(atlasIndex >= (si16)MAX_ATLAS) return 0x080000002;
      // Find first available slot if index is -1
      if(atlasIndex == -1) {
         ui8 i = 0;
         for(; stAtlas[i][0] && i < MAX_ATLAS; ++i);
         if(i >= MAX_ATLAS) return 0x080000001; // All atlas slots occupied
         atlasIndex = i;
      }
      wcscpy(stAtlas[atlasIndex], atlasName);
      uiAtlasTexIndex[atlasIndex] = textureIndex;
      ++uiAtlas;

      return atlasIndex;
   }

   cui32 LoadAtlas(cwchptrc atlasFilename, si16 atlasIndex) {
      if(atlasIndex >= (si16)MAX_ATLAS) return 0x080000002;
      // Find first available slot if index is -1
      if(atlasIndex == -1) {
         ui8 i = 0;
         for(; stAtlas[i][0] && i < MAX_ATLAS; ++i);
         if(i >= MAX_ATLAS) return 0x080000001; // All atlas slots occupied
         atlasIndex = i;
      }
      wsprintf(files.wstTemp, L"textures\\gui\\%s%s", atlasFilename, L".dds");
      csi32 index = gpu.tex.Load2D(files.wstTemp);
      wcscpy(stAtlas[atlasIndex], atlasFilename);
      uiAtlasTexIndex[atlasIndex] = index;
      ++uiAtlas;

      return atlasIndex;
   }

   cui32 DestroyAtlas(csi16 atlasIndex) {
///--- To do...
   }

   cbool SaveAtlas(cwchptrc filename, cwchptrc folder, csi16 atlasIndex) const {
///--- To do...
   }

   cui32 CreateSpriteLibrary(si32 libraryIndex, csi16 maxSprites, csi16 atlasIndex) const {
      // Find first available slot if index is -1
      if(libraryIndex == -1) {
         ui8 i = 0;
         for(; spriteLib[i].sprite && i < MAX_GUI_SPRITE_LIBS; ++i);
         if(i >= MAX_GUI_SPRITE_LIBS) return 0x080000001; // All atlas slots occupied
         libraryIndex = i;
      }
      spriteLib[libraryIndex] = { (GUI_SPRITE *)zalloc32(sizeof(GUI_SPRITE) * maxSprites), (ui16)atlasIndex, 0, (ui16)maxSprites };

      return libraryIndex;
   }

   cui32 DestroySpriteLibrary(csi32 index) {
///--- To do...
   }

   // Read atlas filename then locates/loads texture. Returns atlas index
   inline cui16 ReadAndLoadAtlas(cHANDLE fileHandle) {
      static al16 wchptrc atlasFilename = (wchptr)malloc16(512u);
      ui16 i = 0;

      files.ReadWideLine(fileHandle, atlasFilename);
      while(wcscmp(atlasFilename, stAtlas[i]) && (i < MAX_ATLAS)) ++i;
      if(i >= MAX_ATLAS) { // Atlas not preloaded
         cui32 atlasSlot = LoadAtlas(atlasFilename, -1);

         if(atlasSlot == 0x080000001) Try("Atlas texture not found", -5, ss_gui);

         i = atlasSlot;
      }

      return i;
   }

   // Returns index of library, or 0x080000001 if all sprite library slots occupied
   csi16 LoadSpriteLibrary(cwchptrc filename, si16 libIndex) {
      chptr textArrayOS = 0;
      ui32  stringCount = 0;
      ui16  index       = 0;

      // Find first available slot if index is -1
      if(libIndex == -1) {
         for(; spriteLib[index].sprite && index < MAX_GUI_SPRITE_LIBS; ++index);
         if(index >= MAX_GUI_SPRITE_LIBS) return 0x08001u;
         libIndex = index;
      }

      cHANDLE hSpriteLibData = files.OpenFileForReading(filename, stGUIDir);

      // Read (and discard) tag line
      if(!files.ReadAndVerify(hSpriteLibData, "AE.SpriteLibrary.01u")) Try("Invalid sprite library file", -4, ss_gui);
      // Read atlas filename then locate/load texture
      index = ReadAndLoadAtlas(hSpriteLibData);
      // Read current & maxmimum counts
      files.ReadFromFile(hSpriteLibData, &spriteLib[libIndex].numSprites, sizeof(ui16[2]));
      // Read total character count of sprite strings
      files.ReadFromFile(hSpriteLibData, &stringCount, 4u);

      spriteLib[libIndex] = { (GUI_SPRITE *)zalloc32(sizeof(GUI_SPRITE) * spriteLib[libIndex].maxSprites), index, spriteLib[libIndex].numSprites, spriteLib[libIndex].maxSprites };
      spriteLib[libIndex].sprite[0].name = (chptr)zalloc32(RoundUpToNearest32(stringCount));

      // Read sprite data
      for(textArrayOS = spriteLib[libIndex].sprite[0].name, index = 0; index < spriteLib[libIndex].numSprites; ++index) {
         spriteLib[libIndex].sprite[index].name = textArrayOS;
         textArrayOS += files.ReadLine(hSpriteLibData, spriteLib[libIndex].sprite[index].name) + 1u;
         files.ReadFromFile(hSpriteLibData, &spriteLib[libIndex].sprite[index].aa, sizeof(fl32[6]));
      };

      files.CloseFile(hSpriteLibData);

      return libIndex;
   }

   // Returns true if successful
   cbool SaveSpriteLibrary(cwchptrc filename, csi32 libIndex) const {
      // Sprite library slot is empty
      if(!spriteLib[libIndex].maxSprites) return false;

      ui32 bytesWritten, charsWritten = 0;

      cHANDLE hSpriteLibData = files.OpenFileForWriting(filename, stGUIDir);
      
      // Write tag line: 2[Engine].SpriteLibrary.2[Format version]1[Compression method]
      bytesWritten = files.WriteToFile(hSpriteLibData, "AE.SpriteLibrary.01u", 21);
      // Write atlas filename to avoid misindexing upon load
      bytesWritten += files.WriteToFile(hSpriteLibData, stAtlas[spriteLib[libIndex].atlasIndex], ui32(wcslen(stAtlas[spriteLib[libIndex].atlasIndex]) << 1u) + 2u);
      // Write current & maximum counts
      bytesWritten += files.WriteToFile(hSpriteLibData, &spriteLib[libIndex].numSprites, sizeof(ui16[2]));
      // Reserved for total character count of sprite strings
      files.WriteToFile(hSpriteLibData, &hSpriteLibData, 4u);
      // Write sprite data
      for(ui32 i = 0; i < spriteLib[libIndex].numSprites; ++i) {
         cui32 stringLength = ui32(strlen(spriteLib[libIndex].sprite[i].name) + 1u);
         charsWritten += stringLength;
         files.WriteToFile(hSpriteLibData, spriteLib[libIndex].sprite[i].name, stringLength);
         files.WriteToFile(hSpriteLibData, &spriteLib[libIndex].sprite[i].aa, sizeof(fl32[6]));
      };
      // Write total character count of sprite strings
      SetFilePointer(hSpriteLibData, bytesWritten, 0, 0);
      files.WriteToFile(hSpriteLibData, &charsWritten, 4u);

      files.CloseFile(hSpriteLibData);

      return true;
   }

   cui32 CreateSprite(csi32 libIndex, chptrc name, cVEC4Df texCoords, cVEC2Df activeArea) const {
      if(spriteLib[libIndex].numSprites >= MAX_GUI_LIB_SPRITES) return 0x080000001;
      spriteLib[libIndex].sprite[spriteLib[libIndex].numSprites] = { name, activeArea, texCoords };

      return spriteLib[libIndex].numSprites++;
   }

   cui32 CreateSprite(csi32 libIndex, GUI_SPRITE spriteData, cbool flipY) const {
      if(spriteLib[libIndex].numSprites >= MAX_GUI_LIB_SPRITES) return 0x080000001;

      if(flipY) {
         cfl32 temp = spriteData.tc.y1;

         spriteData.tc.y1 = spriteData.tc.y2;
         spriteData.tc.y2 = temp;
      }

      spriteLib[libIndex].sprite[spriteLib[libIndex].numSprites] = spriteData;

      return spriteLib[libIndex].numSprites++;
   }

///--- !!! Temporary; for building primary libraries !!!
   al32 struct GUI_SPRITE_DEFAULTS { cchptrc name; VEC4Df tc; VEC2Df aa; };
   cui32 CreateSprite(csi32 libIndex, GUI_SPRITE_DEFAULTS spriteData, cbool flipY) const {
      if(spriteLib[libIndex].numSprites >= MAX_GUI_LIB_SPRITES) return 0x080000001;

      if(flipY) {
         cfl32 temp = spriteData.tc.y1;

         spriteData.tc.y1 = spriteData.tc.y2;
         spriteData.tc.y2 = temp;
      }

      spriteLib[libIndex].sprite[spriteLib[libIndex].numSprites].name = (chptr)spriteData.name;
      spriteLib[libIndex].sprite[spriteLib[libIndex].numSprites].aa = spriteData.aa;
      spriteLib[libIndex].sprite[spriteLib[libIndex].numSprites].tc = spriteData.tc;

      return spriteLib[libIndex].numSprites++;
   }

   cui32 DestroySprite(csi32 libIndex, csi16 spriteIndex) {
///--- To do...
   }

   // Returns index of sprite, or 0x080000001 if all sprite slots occupied
   csi32 LoadSprite(cwchptrc filename, csi32 libIndex, si16 spriteIndex) {
      // Find first available slot if index is -1
      if(spriteIndex == -1) {
         si16 i = 0;
         for(; spriteLib[libIndex].sprite[i].name && i < MAX_GUI_LIB_SPRITES; ++i);
         if(i >= MAX_GUI_LIB_SPRITES) return 0x080000001;
         spriteIndex = i;
      }

      cHANDLE hSpriteData = files.OpenFileForReading(filename, stGUIDir);

      // Read (and discard) tag line
      if(!files.ReadAndVerify(hSpriteData, L"AE.Sprite.01u")) Try("Invalid sprite file", -3, ss_gui);
      // Read data
      files.ReadLine(hSpriteData, spriteLib[libIndex].sprite[spriteIndex].name);
      files.ReadFromFile(hSpriteData, &spriteLib[libIndex].sprite[spriteIndex].tc, sizeof(fl32[6]));

      files.CloseFile(hSpriteData);

      return spriteIndex;
   }

   // Returns true if successful
   cbool SaveSprite(cwchptrc filename, csi32 libIndex, csi16 spriteIndex) const {
      // Sprite slot is empty
      if(!spriteLib[libIndex].sprite[spriteIndex].name) return false;

      cHANDLE hSpriteData = files.OpenFileForWriting(filename, stGUIDir);

      // Write tag line: 2[Engine].Sprite.2[Format version]1[Compression method]
      files.WriteToFile(hSpriteData, "AE.Sprite.01u", 14);
      // Write data
      files.WriteToFile(hSpriteData, spriteLib[libIndex].sprite[spriteIndex].name, ui32(strlen(spriteLib[libIndex].sprite[spriteIndex].name)));
      files.WriteToFile(hSpriteData, &spriteLib[libIndex].sprite[spriteIndex].tc, sizeof(fl32[6]));

      files.CloseFile(hSpriteData);

      return true;
   }

   csi16 CreateAlphabet(cchptrc languageName, csi32 xSegments, csi32 ySegments, cVEC2Df charSize, si16 alphabetIndex, csi16 atlasIndex) {
      // Find first available slot if alphabetIndex is -1
      if(alphabetIndex == -1) {
         si16 i = 0;
         for(; alphabet[i].stLanguage && i < MAX_ALPHABETS; ++i);
         if(i >= MAX_ALPHABETS) return 0x08001u; // All alphabet slots occupied
         alphabetIndex = i;
      }
      if(xSegments < 1 || ySegments < 1) return 0x08002u;
      if(xSegments * ySegments > MAX_CHARACTERS) return 0x08003u;

      cui32 pIMMos = alphabetIndex * MAX_CHARACTERS;
      strcpy(stLanguage[uiLanguages], languageName);
      alphabet[alphabetIndex].stLanguage = stLanguage[uiLanguages++];
      alphabet[alphabetIndex].stAtlasFilename = stAtlas[atlasIndex];
      alphabet[alphabetIndex].atlasIndex = atlasIndex;
      alphabet[alphabetIndex].numChars = xSegments * ySegments;
      alphabet[alphabetIndex].pIMMos = pIMMos;
      for(si32 iY = 0; iY < ySegments; iY++)
         for(si32 iX = 0; iX < xSegments; iX++) {
            // Texture offsets calculated per entry to maintain accuracy
            cui32 i = (iY * xSegments) + iX;
            alphabet_pIMM[pIMMos + i].tc.data16[0] = ui16((iX << 15) / xSegments);
            alphabet_pIMM[pIMMos + i].tc.data16[1] = ui16((iY << 15) / ySegments);
            alphabet_pIMM[pIMMos + i].tc.data16[2] = ui16(((iX + 1) << 15) / xSegments);
            alphabet_pIMM[pIMMos + i].tc.data16[3] = ui16(((iY + 1) << 15) / ySegments);
            alphabet_pIMM[pIMMos + i].width  = i ? charSize.x : 0.0f, 65535.0f;
            alphabet_pIMM[pIMMos + i].height = i ? charSize.y : 0.0f, 65535.0f;
            alphabet_pIMM[pIMMos + i].xos    = 0.0f;
            alphabet_pIMM[pIMMos + i].yos    = 0.0f;
         }

      return alphabetIndex;
   }

   csi16 LoadAlphabet(cwchptrc filename, si16 index) {
      // Find first available slot if index is -1
      if(index == -1) {
         si16 i = 0;
         for(; alphabet[i].stLanguage && i < MAX_ALPHABETS; ++i);
         if(i >= MAX_ALPHABETS) return 0x08001u; // All alphabet slots occupied
         index = i;
      }

      cHANDLE hAlphabetData = files.OpenFileForReading(filename, stGUIDir);

      // Read (and discard) tag line
      if(!files.ReadAndVerify(hAlphabetData, "AE.Alphabet.01u")) Try("Invalid alphabet file", -2, ss_gui);
      // Read primary data
      files.ReadLine(hAlphabetData, stLanguage[uiLanguages]);
      alphabet[uiLanguages].stLanguage = stLanguage[uiLanguages];
      alphabet[uiLanguages].atlasIndex = ReadAndLoadAtlas(hAlphabetData);
      alphabet[uiLanguages].stAtlasFilename = stAtlas[alphabet[uiLanguages].atlasIndex];
      files.ReadFromFile(hAlphabetData, &alphabet[uiLanguages].numChars, sizeof(ui16));
      alphabet[uiLanguages].pIMMos = uiLanguages * MAX_CHARACTERS;
      // Read character data
      files.ReadFromFile(hAlphabetData, &alphabet_pIMM[uiLanguages * MAX_CHARACTERS], sizeof(CHAR_IMM) * alphabet[uiLanguages].numChars);

      CloseHandle(hAlphabetData);
      ++uiLanguages;

      return index;
   }

   // Returns true if successful
   cbool SaveAlphabet(cwchptrc filename, csi16 index) const {
      // Alphabet slot is empty
      if(!alphabet[index].stLanguage) return false;

      cHANDLE hAlphabetData = files.OpenFileForWriting(filename, stGUIDir);

      // Write tag line: 2[Engine].Alphabet.2[Format version]1[Compression method]
      files.WriteToFile(hAlphabetData, "AE.Alphabet.01u", 16);
      // Write primary data
      files.WriteToFile(hAlphabetData, alphabet[index].stLanguage, ui32((strlen(alphabet[index].stLanguage) + 1u) * sizeof(char)));
      files.WriteToFile(hAlphabetData, alphabet[index].stAtlasFilename, ui32((wcslen(alphabet[index].stAtlasFilename) + 1u) * sizeof(wchar)));
      files.WriteToFile(hAlphabetData, &alphabet[index].numChars, sizeof(ui16));
      // Write character data
      files.WriteToFile(hAlphabetData, &alphabet_pIMM[index * MAX_CHARACTERS], sizeof(CHAR_IMM) * alphabet[index].numChars);

      CloseHandle(hAlphabetData);

      return true;
   }

   cui32 DestroyAlphabet(si16 index) const {
///- To do...
   }

   inline cSSE4Df32 CalcActiveCoords(cVEC2Df viewPos, cVEC2Df spriteSize, cVEC2Df activeSize, cui8 alignment) const {
      cVEC2Df indent = { spriteSize.x - activeSize.x, spriteSize.y - activeSize.y };

      SSE4Df32 coords = { viewPos.x, viewPos.y, viewPos.x, viewPos.y };

      if(alignment & 0x01) {
         coords.vector.x1 -= ScrRes.dims[ScrRes.state].aspectI - indent.x;
         coords.vector.x2 += spriteSize.x + activeSize.x - ScrRes.dims[ScrRes.state].aspectI;
      } else if(alignment & 0x02) {
         coords.vector.x1 -= spriteSize.x + activeSize.x - ScrRes.dims[ScrRes.state].aspectI;
         coords.vector.x2 += ScrRes.dims[ScrRes.state].aspectI - indent.x;
      } else {
         coords.vector.x1 -= activeSize.x;
         coords.vector.x2 += activeSize.x;
      }
      if(alignment & 0x04) {
         coords.vector.y1 -= 1.0f - indent.y;
         coords.vector.y2 += spriteSize.y + activeSize.y - 1.0f;
      } else if(alignment & 0x08) {
         coords.vector.y1 -= spriteSize.y + activeSize.y - 1.0f;
         coords.vector.y2 += 1.0f - indent.y;
      } else {
         coords.vector.y1 *= -1.0f;
         coords.vector.y2 *= -1.0f;
         coords.vector.y1 -= activeSize.y;
         coords.vector.y2 += activeSize.y;
      }
      coords.vector.x1 *= ScrRes.dims[ScrRes.state].aspect;
      coords.vector.x2 *= ScrRes.dims[ScrRes.state].aspect;

      return (SSE4Df32 &)_mm_fmadd_ps(coords.xmm, _mm_set_ps1(0.5f), _mm_set_ps1(0.5f));
   }

   inline cVEC2Df CalculateTextOrigin(csi16 firstVert, csi16 curVert, csi16 alphabetIndex) const {
      if(firstVert == curVert) return element_dgs[firstVert].origin[0];

      cui32   uiPrevVert   = ui32(curVert) - 1;
      cVEC2Df uiPrevOrigin = element_dgs[uiPrevVert].origin[0];
      cui32   uiBufIndex   = (element_dgs[uiPrevVert].charBankOS & 0x03FFFFFF) << 4u;
      fl32    fOffset      = 0.0f;

      // Accumulate character widths
      for(ui32 i = 0; (i < 32u) && (textBuffer[uiBufIndex + i] > 0); ++i)
         fOffset += alphabet_pIMM[alphabet[alphabetIndex].pIMMos + textBuffer[uiBufIndex + i]].width;

      return { fOffset * element_dgs[firstVert].size.x + uiPrevOrigin.x, uiPrevOrigin.y };
   };

   // Returns first vertex, or 0x0FFFFFFFF if insufficient text buffer space
   cui32 CreateTextVertices(cGUI_EL_DESC &desc, ui32 textSize, cbool forTextArray) {
      if(textSize <= 0) textSize = ui32(strlen(desc.text.cPtr));

      cui32 firstVertex     = uiGUIVerts;
      cui32 uiTextBlockSize = max(1u, RoundUpToNearest16(desc.text.cCount <= 0 ? textSize : desc.text.cCount));

      // Insufficient text buffer space
      if(uiTextBlockSize > (MAX_TEXT_BUFFER - siTextBankOS)) return 0x0FFFFFFFFu;

      strncpy(&textBuffer[siTextBankOS], desc.text.cPtr, textSize);

      // Zero first character of trailing vertex
      if(!((siTextBankOS + textSize) & 0x0FFFFFFE0u)) textBuffer[(siTextBankOS + textSize)] = 0;

      ui32 i = 0;
      for(element_dgs[firstVertex].origin[0] = desc.viewPos; i < textSize; ++uiGUIVerts, i += 32u) {
         element_dgs[uiGUIVerts].origin[0]     = CalculateTextOrigin(firstVertex, uiGUIVerts, desc.index.alphabet);
//         element_dgs[uiGUIVerts].origin[1]     =
//         element_dgs[uiGUIVerts].origin[2]     =
//         element_dgs[uiGUIVerts].origin[3]     = { 16384.0f, desc.viewPos.y };
         element_dgs[uiGUIVerts].origin[1]     = { 16384.0f, 16384.0f };
         element_dgs[uiGUIVerts].coords.xmm[1] = _mm_set_ps1(16384.0f);
         element_dgs[uiGUIVerts].colour        = desc.text.tint;
         element_dgs[uiGUIVerts].size          = desc.text.size;
         element_dgs[uiGUIVerts].rotAngle      = 0.0f;
         element_dgs[uiGUIVerts].parentIndex   = uiGUIVerts;
         element_dgs[uiGUIVerts].width         = 0.0f;
         element_dgs[uiGUIVerts].charBankOS    = ((i + siTextBankOS) >> 4u) | ((textSize - i > 32u ? 32u : textSize - i) << 26u);
         element_dgs[uiGUIVerts].alphabetIndex = alphabet[desc.index.alphabet].pIMMos;
         element_dgs[uiGUIVerts].atlasIndex    = (ui8 &)alphabet[desc.index.alphabet].atlasIndex * 2u + 80u;
         element_dgs[uiGUIVerts].elementType   = (forTextArray ? aet_textArray : aet_text);
         element_dgs[uiGUIVerts].sibling       = uiGUIVerts - firstVertex;
         element_dgs[uiGUIVerts].align         = desc.text.align;
         element_dgs[uiGUIVerts].mods          = desc.text.mods;
      }
      for(; i < uiTextBlockSize; ++uiGUIVerts, i += 32u) {
#ifdef __AVX__
         element_dgs[uiGUIVerts].coords.ymm    = _mm256_setzero_ps();
#else
         element_dgs[uiGUIVerts].coords.xmm[0] = _mm_setzero_ps();
         element_dgs[uiGUIVerts].coords.xmm[1] = _mm_setzero_ps();
#endif
         element_dgs[uiGUIVerts].colour        = desc.text.tint;
         element_dgs[uiGUIVerts].size          = desc.text.size;
         element_dgs[uiGUIVerts].rotAngle      = 0.0f;
         element_dgs[uiGUIVerts].parentIndex   = uiGUIVerts;
         element_dgs[uiGUIVerts].width         = 0.0f;
         element_dgs[uiGUIVerts].charBankOS    = (i + siTextBankOS) >> 4u;
         element_dgs[uiGUIVerts].alphabetIndex = alphabet[desc.index.alphabet].pIMMos;
         element_dgs[uiGUIVerts].atlasIndex    = (ui8 &)alphabet[desc.index.alphabet].atlasIndex * 2u + 80u;
         element_dgs[uiGUIVerts].elementType   = (forTextArray ? aet_textArray : aet_text);
         element_dgs[uiGUIVerts].sibling       = uiGUIVerts - firstVertex;
         element_dgs[uiGUIVerts].align         = desc.text.align | 0x080u;
         element_dgs[uiGUIVerts].mods          = desc.text.mods;
      }
      element_dgs[firstVertex].sibling = ui16(uiGUIVerts - firstVertex) | 0x08000u;

      siTextBankOS += uiTextBlockSize;

      return firstVertex;
   }

   // Creates unused element entry
   cui32 CreateVoid(void) {
      if(siGUIElements >= MAX_GUI_ELEMENTS) return 0x080000001;

      element[siGUIElements].vertexIndex       = uiGUIVerts;
      element[siGUIElements].vertexCount[0]    = 1;
      element[siGUIElements].vertexCount[1]    = 1;
      element[siGUIElements].soundBankIndex    = -1;
      element[siGUIElements].elementType       = aet_void;
      element[siGUIElements].stateBits         = 0;
      element[siGUIElements].func.hover[0]     = 0;
      element[siGUIElements].func.hover[1]     = 0;
      element[siGUIElements].func.activate0[0] = 0;
      element[siGUIElements].func.activate0[1] = 0;
      element[siGUIElements].func.activate1[0] = 0;
      element[siGUIElements].func.activate1[1] = 0;

      element_dgs[uiGUIVerts].parentIndex = uiGUIVerts;
      element_dgs[uiGUIVerts].elementType = aet_void;
      element_dgs[uiGUIVerts].width       = 0.0f;
      element_dgs[uiGUIVerts++].align     = 0x080;

      return siGUIElements++;
   }

   cui32 CreateText(cGUI_EL_DESC &desc) {
      if(siGUIElements >= MAX_GUI_ELEMENTS) return 0x080000001;

      si16 siTextSize = si16(strlen(desc.text.cPtr));

      if((desc.text.cCount > 0) && (desc.text.cCount < siTextSize)) siTextSize = desc.text.cCount;

      cui32 uiFirstVert = CreateTextVertices(desc, siTextSize, false);

      // Insufficient text buffer space
      if(uiFirstVert == 0x0FFFFFFFFu) return 0x080000002;

      // Calculate width of string view space
///--- Change to iterate & add unique character widths
      cVEC2Df activeSize = { alphabet_pIMM[alphabet[desc.index.alphabet].pIMMos + 1].width * fl32(siTextSize) * desc.text.size.x,
                             alphabet_pIMM[alphabet[desc.index.alphabet].pIMMos + 1].height * desc.text.size.y };

      cSSE4Df32 activeCoords = CalcActiveCoords(desc.viewPos, activeSize, activeSize, desc.text.align);

      element[siGUIElements].activeCoords      = activeCoords;
      element[siGUIElements].activePos         = { (activeCoords.x1 + activeCoords.x2) * 0.5f, (activeCoords.y1 + activeCoords.y2) * 0.5f };
      element[siGUIElements].viewScale         = { 2.0f / activeSize.x, 2.0f / activeSize.y };
      element[siGUIElements].charCount         = siTextSize;
      element[siGUIElements].vertexIndex       = (si32 &)uiFirstVert;
      element[siGUIElements].soundBankIndex    = desc.index.soundBank;
      element[siGUIElements].elementType       = aet_text;
      element[siGUIElements].stateBits         = 0x040;
      element[siGUIElements].func.hover[0]     = desc.func.hover[0];
      element[siGUIElements].func.hover[1]     = desc.func.hover[1];
      element[siGUIElements].func.activate0[0] = desc.func.activate0[0];
      element[siGUIElements].func.activate0[1] = desc.func.activate0[1];
      element[siGUIElements].func.activate1[0] = desc.func.activate1[0];
      element[siGUIElements].func.activate1[1] = desc.func.activate1[1];
      element[siGUIElements].ui128Var          = null128;

      element[siGUIElements].vertexCount[0] = ui16(max(1, (siTextSize + 31) >> 5));
      element[siGUIElements].vertexCount[1] = ui16(uiGUIVerts - uiFirstVert);

      return siGUIElements++;
   }

   cui32 CreateTextList(cGUI_EL_DESC &desc) {
      if(siGUIElements >= MAX_GUI_ELEMENTS) return 0x080000001;

      csi32 textBufferStart = siTextBankOS;
      cui32 GUIVertsOrigin  = uiGUIVerts;
      ui32  curVertex;
      fl32  heightOS;

      GUI_EL_DESC curText;

      Stream32(&desc, &curText, sizeof(GUI_EL_DESC));

      ui16 i;

      element[siGUIElements].textArray = (ui32ptr)malloc16(sizeof(ui32ptr) * desc.text.sCount);

      // Generate all text vertices
      for(i = 0, heightOS = curText.viewPos.y; i < (ui16 &)desc.text.sCount; ++i, heightOS -= curText.text.size.y) {
         curText.viewPos     = { 0.0f, heightOS };
         curText.text.cPtr   = desc.text.cArray[i];
         curText.text.cCount = ui16(strlen(desc.text.cArray[i]));

         curVertex = CreateTextVertices(curText, curText.text.cCount, true);

         if(curVertex == 0x0FFFFFFFFu) break; // Insufficent text buffer space for current string

         element[siGUIElements].textArray[i] = curVertex;
      }

      // Insufficent text buffer space for every string
      if(i < desc.text.sCount) {
         mdealloc(element[siGUIElements].textArray);

         element[siGUIElements].textArray = 0;

         siTextBankOS = textBufferStart;
         uiGUIVerts   = GUIVertsOrigin;

         return 0x08000002;
      }

      ///- CalcActiveCoords?

      //element[siGUIElements].activeCoords      = activeCoords;
      //element[siGUIElements].activePos         = { (activeCoords.x1 + activeCoords.x2) * 0.5f, (activeCoords.y1 + activeCoords.y2) * 0.5f };
      //element[siGUIElements].viewScale         = { 2.0f / activeSize.x, 2.0f / activeSize.y };
      element[siGUIElements].textCount         = i;
      element[siGUIElements].textEntry         = 0;
      element[siGUIElements].vertexIndex       = (si32 &)GUIVertsOrigin;
      element[siGUIElements].soundBankIndex    = desc.index.soundBank;
      element[siGUIElements].elementType       = aet_textArray;
      element[siGUIElements].stateBits         = 0x040;
      element[siGUIElements].func.hover[0]     = desc.func.hover[0];
      element[siGUIElements].func.hover[1]     = desc.func.hover[1];
      element[siGUIElements].func.activate0[0] = desc.func.activate0[0];
      element[siGUIElements].func.activate0[1] = desc.func.activate0[1];
      element[siGUIElements].func.activate1[0] = desc.func.activate1[0];
      element[siGUIElements].func.activate1[1] = desc.func.activate1[1];
      element[siGUIElements].ui128Var          = null128;

      element[siGUIElements].vertexCount[0] = element[siGUIElements].vertexCount[1] = ui16(uiGUIVerts - GUIVertsOrigin);

      return siGUIElements++;
   }

   cui32 CreateCursor(cGUI_EL_DESC &desc) {
      if(siGUIElements >= MAX_GUI_ELEMENTS) return 0x080000001;

      cGUI_SPRITE_LIB &curLib = spriteLib[desc.index.spriteLib];
       GUI_SPRITE     &curSpr = curLib.sprite[desc.index.cursorSprite];

      cVEC2Df spriteSize = { fabsf(curSpr.tc.x2 - curSpr.tc.x1) * desc.cursor.size.x, fabsf(curSpr.tc.y2 - curSpr.tc.y1) * desc.cursor.size.y };
      cVEC2Df activeSize = { spriteSize.x * curSpr.aa.x, spriteSize.y * curSpr.aa.y };
      cVEC4Df viewCoords = { desc.viewPos.x - spriteSize.x, desc.viewPos.y + spriteSize.y, desc.viewPos.x + spriteSize.x, desc.viewPos.y - spriteSize.y };

      cSSE4Df32 activeCoords = CalcActiveCoords(desc.viewPos, spriteSize, activeSize, desc.cursor.align);

      element[siGUIElements].activeCoords      = activeCoords;
      element[siGUIElements].activePos         = { (activeCoords.x1 + activeCoords.x2) * 0.5f, (activeCoords.y1 + activeCoords.y2) * 0.5f };
      element[siGUIElements].viewScale         = { 2.0f / activeSize.x, 2.0f / activeSize.y };
      element[siGUIElements].charCount         = 0;
      element[siGUIElements].vertexIndex       = uiGUIVerts;
      element[siGUIElements].vertexCount[0]    = 1;
      element[siGUIElements].vertexCount[1]    = 1;
      element[siGUIElements].soundBankIndex    = desc.index.soundBank;
      element[siGUIElements].elementType       = aet_cursor;
      element[siGUIElements].stateBits         = 0x040;
      element[siGUIElements].func.hover[0]     = desc.func.hover[0];
      element[siGUIElements].func.hover[1]     = desc.func.hover[1];
      element[siGUIElements].func.activate0[0] = desc.func.activate0[0];
      element[siGUIElements].func.activate0[1] = desc.func.activate0[1];
      element[siGUIElements].func.activate1[0] = desc.func.activate1[0];
      element[siGUIElements].func.activate1[1] = desc.func.activate1[1];
      element[siGUIElements].ui128Var          = null128;
      element_dgs[uiGUIVerts].viewPos      = desc.viewPos;
      element_dgs[uiGUIVerts].indent       = activeSize;
      element_dgs[uiGUIVerts].texCoords    = curSpr.tc;
      element_dgs[uiGUIVerts].colour       = desc.cursor.tint;
      element_dgs[uiGUIVerts].size         = spriteSize;
      element_dgs[uiGUIVerts].rotAngle     = 0.0f;
      element_dgs[uiGUIVerts].parentIndex  = uiGUIVerts;
      element_dgs[uiGUIVerts].animTime     = { 0.0f, 0.0f };
      element_dgs[uiGUIVerts].animType     = 0;
      element_dgs[uiGUIVerts].atlasIndex   = (ui8 &)curLib.atlasIndex * 2u + 80u;
      element_dgs[uiGUIVerts].elementType  = aet_cursor;
      element_dgs[uiGUIVerts].sibling      = 0;
      element_dgs[uiGUIVerts].align        = desc.cursor.align;
      element_dgs[uiGUIVerts++].mods       = desc.cursor.mods;

      return siGUIElements++;
   }

   cui32 CreatePanel(cGUI_EL_DESC &desc) {
      if(siGUIElements >= MAX_GUI_ELEMENTS) return 0x080000001;

      GUI_SPRITE &curSpr = spriteLib[desc.index.spriteLib].sprite[desc.index.panelSprite];

      cVEC2Df spriteSize = { fabsf(curSpr.tc.x2 - curSpr.tc.x1) * desc.panel.size.x, fabsf(curSpr.tc.y2 - curSpr.tc.y1) * desc.panel.size.y };
      cVEC2Df activeSize = { spriteSize.x * curSpr.aa.x * desc.panel.wIndent.x, spriteSize.y * curSpr.aa.y * desc.panel.wIndent.y };
      cVEC4Df viewCoords = { desc.viewPos.x - spriteSize.x, desc.viewPos.y + spriteSize.y, desc.viewPos.x + spriteSize.x, desc.viewPos.y - spriteSize.y };

      cSSE4Df32 activeCoords = CalcActiveCoords(desc.viewPos, spriteSize, activeSize, desc.panel.align);

      element[siGUIElements].activeCoords      = activeCoords;
      element[siGUIElements].activePos         = { (activeCoords.x1 + activeCoords.x2) * 0.5f, (activeCoords.y1 + activeCoords.y2) * 0.5f };
      element[siGUIElements].viewScale         = { 2.0f / activeSize.x, 2.0f / activeSize.y };
      element[siGUIElements].charCount         = 0;
      element[siGUIElements].vertexIndex       = uiGUIVerts;
      element[siGUIElements].vertexCount[0]    = 1;
      element[siGUIElements].vertexCount[1]    = 1;
      element[siGUIElements].soundBankIndex    = desc.index.soundBank;
      element[siGUIElements].elementType       = aet_panel;
      element[siGUIElements].stateBits         = 0x0C0;
      element[siGUIElements].func.hover[0]     = desc.func.hover[0];
      element[siGUIElements].func.hover[1]     = desc.func.hover[1];
      element[siGUIElements].func.activate0[0] = desc.func.activate0[0];
      element[siGUIElements].func.activate0[1] = desc.func.activate0[1];
      element[siGUIElements].func.activate1[0] = desc.func.activate1[0];
      element[siGUIElements].func.activate1[1] = desc.func.activate1[1];
      element[siGUIElements].ui128Var          = null128;
      element_dgs[uiGUIVerts].viewPos      = desc.viewPos;
      element_dgs[uiGUIVerts].indent       = activeSize;
      element_dgs[uiGUIVerts].texCoords    = desc.panel.bStyle ? VEC4Df{ 0.0f, 0.0f, 1.0f, 1.0f } : curSpr.tc;
      element_dgs[uiGUIVerts].colour       = desc.panel.tint;
      element_dgs[uiGUIVerts].size         = spriteSize;
      element_dgs[uiGUIVerts].rotAngle     = 0.0f;
      element_dgs[uiGUIVerts].parentIndex  = uiGUIVerts;
      element_dgs[uiGUIVerts].border       = { desc.panel.bSize, desc.panel.wOpacity };
      element_dgs[uiGUIVerts].borderStyle  = (ui8 &)desc.panel.bStyle;
      element_dgs[uiGUIVerts].atlasIndex   = desc.panel.bStyle ? 0x0FFu : ((ui8 &)spriteLib[desc.index.spriteLib].atlasIndex * 2u + 80u);
      element_dgs[uiGUIVerts].elementType  = aet_panel;
      element_dgs[uiGUIVerts].sibling      = 0;
      element_dgs[uiGUIVerts].align        = desc.panel.align;
      element_dgs[uiGUIVerts++].mods       = desc.panel.mods;

      return siGUIElements++;
   }

   // Button's default sprite index of it's alternate state == [spriteIndex + 1]
   cui32 CreateButton(cGUI_EL_DESC &desc) {
      if(siGUIElements >= MAX_GUI_ELEMENTS) return 0x080000001;

      cGUI_SPRITE_LIB &curLib = spriteLib[desc.index.spriteLib];
       GUI_SPRITE     &curSpr = curLib.sprite[desc.index.panelSprite];

      cVEC2Df spriteSize = { fabsf(curSpr.tc.x2 - curSpr.tc.x1) * desc.panel.size.x, fabsf(curSpr.tc.y2 - curSpr.tc.y1) * desc.panel.size.y };
      cVEC2Df activeSize = { spriteSize.x * curSpr.aa.x, spriteSize.y * curSpr.aa.y };
      cVEC4Df viewCoords = { desc.viewPos.x - spriteSize.x, desc.viewPos.y + spriteSize.y, desc.viewPos.x + spriteSize.x, desc.viewPos.y - spriteSize.y };

      cSSE4Df32 activeCoords = CalcActiveCoords(desc.viewPos, spriteSize, activeSize, desc.panel.align);

      element[siGUIElements].activeCoords      = activeCoords;
      element[siGUIElements].activePos         = { (activeCoords.x1 + activeCoords.x2) * 0.5f, (activeCoords.y1 + activeCoords.y2) * 0.5f };
      element[siGUIElements].viewScale         = { 2.0f / activeSize.x, 2.0f / activeSize.y };
      element[siGUIElements].charCount         = 0;
      element[siGUIElements].vertexIndex       = uiGUIVerts;
      element[siGUIElements].vertexCount[0]    = 2;
      element[siGUIElements].vertexCount[1]    = 2;
      element[siGUIElements].soundBankIndex    = desc.index.soundBank;
      element[siGUIElements].elementType       = aet_button;   // bit7==Button "down"
      element[siGUIElements].stateBits         = 0x0C0;
      element[siGUIElements].func.hover[0]     = desc.func.hover[0];
      element[siGUIElements].func.hover[1]     = desc.func.hover[1];
      element[siGUIElements].func.activate0[0] = (desc.func.activate0[0] ? desc.func.activate0[0] : Activate00DefaultButton);
      element[siGUIElements].func.activate0[1] = (desc.func.activate0[1] ? desc.func.activate0[1] : Activate01DefaultButton);
      element[siGUIElements].func.activate1[0] = desc.func.activate1[0];
      element[siGUIElements].func.activate1[1] = desc.func.activate1[1];
      element[siGUIElements].ui128Var          = null128;
      element_dgs[uiGUIVerts].viewPos      = desc.viewPos;
      element_dgs[uiGUIVerts].indent       = activeSize;
      element_dgs[uiGUIVerts].texCoords    = curSpr.tc;
      element_dgs[uiGUIVerts].colour       = desc.panel.tint;
      element_dgs[uiGUIVerts].size         = spriteSize;
      element_dgs[uiGUIVerts].rotAngle     = 0.0f;
      element_dgs[uiGUIVerts].parentIndex  = uiGUIVerts;
      element_dgs[uiGUIVerts].animTime     = { 0.0f, 0.0f };
      element_dgs[uiGUIVerts].animType     = 0;
      element_dgs[uiGUIVerts].atlasIndex   = (ui8 &)curLib.atlasIndex * 2u + 80u;
      element_dgs[uiGUIVerts].elementType  = aet_button;
      element_dgs[uiGUIVerts].sibling      = 0x08001;
      element_dgs[uiGUIVerts].align        = desc.panel.align;
      element_dgs[uiGUIVerts++].mods       = desc.panel.mods;
      element_dgs[uiGUIVerts].viewPos      = desc.viewPos;
      element_dgs[uiGUIVerts].indent       = activeSize;
      element_dgs[uiGUIVerts].texCoords    = curLib.sprite[desc.index.panelSprite + 1].tc;
      element_dgs[uiGUIVerts].colour       = desc.panel.tint;
      element_dgs[uiGUIVerts].size         = spriteSize;
      element_dgs[uiGUIVerts].rotAngle     = 0.0f;
      element_dgs[uiGUIVerts].parentIndex  = uiGUIVerts;
      element_dgs[uiGUIVerts].animTime     = { 0.0f, 0.0f };
      element_dgs[uiGUIVerts].animType     = 0;
      element_dgs[uiGUIVerts].atlasIndex   = (ui8 &)curLib.atlasIndex;
      element_dgs[uiGUIVerts].elementType  = aet_button;
      element_dgs[uiGUIVerts].sibling      = 1;
      element_dgs[uiGUIVerts].align        = desc.panel.align | 0x080;
      element_dgs[uiGUIVerts++].mods       = desc.panel.mods;

      return siGUIElements++;
   }

   // Button's default sprite index of it's alternate state == [spriteIndex] + 1
   cui32 CreateToggle(cGUI_EL_DESC &desc) {
      if(siGUIElements >= MAX_GUI_ELEMENTS) return 0x080000001;

      const GUI_SPRITE_LIB &curLib = spriteLib[desc.index.spriteLib];
            GUI_SPRITE     &curSpr = curLib.sprite[desc.index.panelSprite];

      cVEC2Df spriteSize = { fabsf(curSpr.tc.x2 - curSpr.tc.x1) * desc.panel.size.x, fabsf(curSpr.tc.y2 - curSpr.tc.y1) * desc.panel.size.y };
      cVEC2Df activeSize = { spriteSize.x * curSpr.aa.x, spriteSize.y * curSpr.aa.y };
      cVEC4Df viewCoords = { desc.viewPos.x - spriteSize.x, desc.viewPos.y + spriteSize.y, desc.viewPos.x + spriteSize.x, desc.viewPos.y - spriteSize.y };

      cSSE4Df32 activeCoords = CalcActiveCoords(desc.viewPos, spriteSize, activeSize, desc.panel.align);

      element[siGUIElements].activeCoords      = activeCoords;
      element[siGUIElements].activePos         = { (activeCoords.x1 + activeCoords.x2) * 0.5f, (activeCoords.y1 + activeCoords.y2) * 0.5f };
      element[siGUIElements].viewScale         = { 2.0f / activeSize.x, 2.0f / activeSize.y };
      element[siGUIElements].charCount         = 0;
      element[siGUIElements].vertexIndex       = uiGUIVerts;
      element[siGUIElements].vertexCount[0]    = 2;
      element[siGUIElements].vertexCount[1]    = 2;
      element[siGUIElements].soundBankIndex    = desc.index.soundBank;
      element[siGUIElements].elementType       = aet_toggle;
      element[siGUIElements].stateBits         = 0x0C0;
      element[siGUIElements].func.hover[0]     = desc.func.hover[0];
      element[siGUIElements].func.hover[1]     = desc.func.hover[1];
      element[siGUIElements].func.activate0[0] = (desc.func.activate0[0] ? desc.func.activate0[0] : Activate00DefaultToggle);
      element[siGUIElements].func.activate0[1] = (desc.func.activate0[1] ? desc.func.activate0[1] : Activate01DefaultToggle);
      element[siGUIElements].func.activate1[0] = desc.func.activate1[0];
      element[siGUIElements].func.activate1[1] = desc.func.activate1[1];
      element[siGUIElements].ui128Var          = null128;
      element_dgs[uiGUIVerts].viewPos      = desc.viewPos;
      element_dgs[uiGUIVerts].indent       = activeSize;
      element_dgs[uiGUIVerts].texCoords    = curSpr.tc;
      element_dgs[uiGUIVerts].colour       = desc.panel.tint;
      element_dgs[uiGUIVerts].size         = spriteSize;
      element_dgs[uiGUIVerts].rotAngle     = 0.0f;
      element_dgs[uiGUIVerts].parentIndex  = uiGUIVerts;
      element_dgs[uiGUIVerts].animTime     = { 0.0f, 0.0f };
      element_dgs[uiGUIVerts].animType     = 0;
      element_dgs[uiGUIVerts].atlasIndex   = (ui8 &)curLib.atlasIndex * 2u + 80u;
      element_dgs[uiGUIVerts].elementType  = aet_toggle;
      element_dgs[uiGUIVerts].sibling      = 0x08001;
      element_dgs[uiGUIVerts].align        = desc.panel.align;
      element_dgs[uiGUIVerts++].mods       = desc.panel.mods;
      element_dgs[uiGUIVerts].viewPos      = desc.viewPos;
      element_dgs[uiGUIVerts].indent       = activeSize;
      element_dgs[uiGUIVerts].texCoords    = curLib.sprite[desc.index.panelSprite + 1].tc;
      element_dgs[uiGUIVerts].colour       = desc.panel.tint;
      element_dgs[uiGUIVerts].size         = spriteSize;
      element_dgs[uiGUIVerts].rotAngle     = 0.0f;
      element_dgs[uiGUIVerts].parentIndex  = uiGUIVerts;
      element_dgs[uiGUIVerts].animTime     = { 0.0f, 0.0f };
      element_dgs[uiGUIVerts].animType     = 0;
      element_dgs[uiGUIVerts].atlasIndex   = (ui8 &)curLib.atlasIndex * 2u + 80u;
      element_dgs[uiGUIVerts].elementType  = aet_toggle;
      element_dgs[uiGUIVerts].sibling      = 1;
      element_dgs[uiGUIVerts].align        = desc.panel.align | 0x080;
      element_dgs[uiGUIVerts++].mods       = desc.panel.mods;

      return siGUIElements++;
   }

   // Returns index of panel. Default index of indicator == [return value] + 1
   // Able to create scalar elements with slider or bar/pool graph, based on .align & .mods bits
   cui32 CreateScalar(cGUI_EL_DESC &desc) {
      if(siGUIElements >= MAX_GUI_ELEMENTS) return 0x080000001;

      cGUI_SPRITE_LIB &curLib = spriteLib[desc.index.spriteLib];
       GUI_SPRITE     &panSpr = curLib.sprite[desc.index.panelSprite];
       GUI_SPRITE     &indSpr = curLib.sprite[desc.index.cursorSprite];

      cVEC2Df spriteSize[2] = { { fabsf(panSpr.tc.x2 - panSpr.tc.x1) * desc.panel.size.x, fabsf(panSpr.tc.y2 - panSpr.tc.y1) * desc.panel.size.y },
                                { fabsf(indSpr.tc.x2 - indSpr.tc.x1) * desc.cursor.size.x, fabsf(indSpr.tc.y2 - indSpr.tc.y1) * desc.cursor.size.y } };
      cVEC2Df activeSize[2] = { { spriteSize[0].x * panSpr.aa.x, spriteSize[0].y * panSpr.aa.y }, { spriteSize[1].x * indSpr.aa.x, spriteSize[1].y * indSpr.aa.y } };
      cVEC4Df viewCoords[2] = { { desc.viewPos.x - spriteSize[0].x, desc.viewPos.y + spriteSize[0].y, desc.viewPos.x + spriteSize[0].x, desc.viewPos.y - spriteSize[0].y },
                                { desc.viewPos.x - spriteSize[1].x, desc.viewPos.y + spriteSize[1].y, desc.viewPos.x + spriteSize[1].x, desc.viewPos.y - spriteSize[1].y } };

      cSSE4Df32 activeCoords[2] = { { CalcActiveCoords(desc.viewPos, spriteSize[0], activeSize[0], desc.panel.align) },
                                    { CalcActiveCoords(desc.viewPos, spriteSize[1], activeSize[1], desc.cursor.align) } };

      element[siGUIElements].activeCoords      = activeCoords[0];
      element[siGUIElements].activePos         = { (activeCoords[0].x1 + activeCoords[0].x2) * 0.5f, (activeCoords[0].y1 + activeCoords[0].y2) * 0.5f};
      element[siGUIElements].viewScale         = { 2.0f / activeSize[0].x, 2.0f / activeSize[0].y };
      element[siGUIElements].scalarPos2D       = { 0, 0 };
      element[siGUIElements].charCount         = 0;
      element[siGUIElements].vertexIndex       = uiGUIVerts;
      element[siGUIElements].vertexCount[0]    = 2;
      element[siGUIElements].vertexCount[1]    = 2;
      element[siGUIElements].soundBankIndex    = desc.index.soundBank;
      element[siGUIElements].elementType       = aet_scalar;
      element[siGUIElements].stateBits         = 0x0C0;
      element[siGUIElements].func.hover[0]     = desc.func.hover[0];
      element[siGUIElements].func.hover[1]     = desc.func.hover[1];
      element[siGUIElements].func.activate0[0] = (desc.func.activate0[0] ? desc.func.activate0[0] : Activate00DefaultScalar);
      element[siGUIElements].func.activate0[1] = (desc.func.activate0[1] ? desc.func.activate0[1] : Activate01DefaultScalar);
      element[siGUIElements].func.activate1[0] = (desc.func.activate1[0] ? desc.func.activate1[0] : Activate10DefaultScalar);
      element[siGUIElements].func.activate1[1] = (desc.func.activate1[1] ? desc.func.activate1[1] : Activate11DefaultScalar);
      element[siGUIElements].ui128Var          = null128;
      element_dgs[uiGUIVerts].viewPos      = desc.viewPos;
      element_dgs[uiGUIVerts].indent       = activeSize[0];
      element_dgs[uiGUIVerts].texCoords    = panSpr.tc;
      element_dgs[uiGUIVerts].colour       = desc.panel.tint;
      element_dgs[uiGUIVerts].size         = spriteSize[0];
      element_dgs[uiGUIVerts].rotAngle     = 0.0f;
      element_dgs[uiGUIVerts].parentIndex  = uiGUIVerts;
      element_dgs[uiGUIVerts].animTime     = { 0.0f, 0.0f };
      element_dgs[uiGUIVerts].animType     = 0;
      element_dgs[uiGUIVerts].atlasIndex   = (ui8 &)curLib.atlasIndex * 2u + 80u;
      element_dgs[uiGUIVerts].elementType  = aet_panel;
      element_dgs[uiGUIVerts].sibling      = 0x08001;
      element_dgs[uiGUIVerts].align        = desc.panel.align;
      element_dgs[uiGUIVerts++].mods       = desc.panel.mods;
      element_dgs[uiGUIVerts].viewPos      = { 0.0f, 0.0f }; // .x range -1.0~1.0
      element_dgs[uiGUIVerts].indent       = activeSize[1];
      element_dgs[uiGUIVerts].texCoords    = indSpr.tc;
      element_dgs[uiGUIVerts].colour       = desc.cursor.tint;
      element_dgs[uiGUIVerts].size         = spriteSize[1];
      element_dgs[uiGUIVerts].rotAngle     = 0.0f;
      element_dgs[uiGUIVerts].parentIndex  = uiGUIVerts - 1;
      element_dgs[uiGUIVerts].animTime     = { 0.0f, 0.0f };
      element_dgs[uiGUIVerts].animType     = 0;
      element_dgs[uiGUIVerts].atlasIndex   = (ui8 &)curLib.atlasIndex * 2u + 80u;
      element_dgs[uiGUIVerts].elementType  = aet_scalar;
      element_dgs[uiGUIVerts].sibling      = 1;
      element_dgs[uiGUIVerts].align        = desc.cursor.align;
      element_dgs[uiGUIVerts++].mods       = desc.cursor.mods;

      return siGUIElements++;
   }

   // Returns indices of panel element and text overlay
   cVEC2Du32 CreateTextPanel(GUI_EL_DESC &desc) {
      if(siGUIElements >= MAX_GUI_ELEMENTS - 1) return { 0x080000001, MAX_GUI_ELEMENTS };
      ui32 i = 0;

      cui32 panel = CreatePanel(desc);

      cui32 tempPtrs[6] = { desc.function[0], desc.function[1], desc.function[2], desc.function[3], desc.function[4], desc.function[5] };
      cVEC2Df tempPos     = desc.viewPos;

      for(i = 0; i < 6; ++i) desc.function[i] = 0;

      desc.viewPos = { 0.0f, 0.0f };
      cui32 overlay = CreateText(desc);

      for(i = 0; i < 6; ++i) desc.function[i] = tempPtrs[i];

      desc.viewPos = tempPos;
      cui32 panelVert = element[panel].vertexIndex;
      cui32 firstVert = element[overlay].vertexIndex;

      for(i = 0; i < element[overlay].vertexCount[1]; ++i) element_dgs[firstVert + i].parentIndex = panelVert;

      return { panel, overlay };
   }

   // Returns indices of panel element and text overlay
   cVEC2Du32 CreateTextButton(GUI_EL_DESC &desc) {
      if(siGUIElements >= MAX_GUI_ELEMENTS - 1) return { 0x080000001, MAX_GUI_ELEMENTS };
      ui32 i = 0;

      cui32 panel = CreateButton(desc);

      cui32   tempPtrs[6] = { desc.function[0], desc.function[1], desc.function[2], desc.function[3], desc.function[4], desc.function[5] };
      cVEC2Df tempPos     = desc.viewPos;

      for(i = 0; i < 6; ++i) desc.function[i] = 0;

      desc.viewPos = { 0.0f, 0.0f };
      cui32 overlay = CreateText(desc);

      for(i = 0; i < 6; ++i) desc.function[i] = tempPtrs[i];

      desc.viewPos = tempPos;
      cui32 panelVert = element[panel].vertexIndex;
      cui32 firstVert = element[overlay].vertexIndex;

      for(i = 0; i < element[overlay].vertexCount[1]; ++i) element_dgs[firstVert + i].parentIndex = panelVert;

      return { panel, overlay };
   }

   // Returns indices of panel element, cursor element, and text overlay
   cVEC3Du32 CreateInputPanel(GUI_EL_DESC &desc) {
      if(siGUIElements >= MAX_GUI_ELEMENTS - 2) return { 0x080000001, MAX_GUI_ELEMENTS, 0 };

      ui32 i = 0;

      cui32 panel = CreatePanel(desc);

      cui32   tempPtrs[6] = { desc.function[0], desc.function[1], desc.function[2], desc.function[3], desc.function[4], desc.function[5] };
      cVEC2Df tempPos     = desc.viewPos;

      for(i = 0; i < 6; ++i) desc.function[i] = 0;

      desc.viewPos = { 0.0f, 0.0f };
      cui32 cursor  = CreateCursor(desc);
      cui32 overlay = CreateText(desc);

      for(i = 0; i < 6; ++i) desc.function[i] = tempPtrs[i];

      desc.viewPos = tempPos;
      cui32 panelVert = element[panel].vertexIndex;
      cui32 firstVert = element[overlay].vertexIndex;

      element_dgs[element[cursor].vertexIndex].parentIndex  = panelVert;
      element_dgs[element[cursor].vertexIndex].align       |= 0x080;

      for(i = 0; i < element[overlay].vertexCount[1]; ++i) element_dgs[firstVert + i].parentIndex = panelVert;

      element[panel].input = new TEXTBUFFER(desc.text.cCount, false);

      element[panel].func.hover[0]     = PassiveDefaultInput;
      element[panel].func.hover[1]     = PassiveDefaultInput;
      element[panel].func.activate0[0] = Activate00DefaultInput;
      element[panel].func.activate0[1] = Activate01DefaultInput;
      element[panel].func.activate1[0] = Activate10DefaultInput;

      element[cursor].stateBits = 0;

      return { panel, cursor, overlay };
   }

   // Returns indices of panel element and text list overlay
   cVEC2Du32 CreateTextSelectionPanel(GUI_EL_DESC &desc) {
      if(siGUIElements >= MAX_GUI_ELEMENTS) return { 0x080000001, MAX_GUI_ELEMENTS };

      ui32 i = 0;

      cui32 panel = CreatePanel(desc);

      cui32   tempPtrs[6] = { desc.function[0], desc.function[1], desc.function[2], desc.function[3], desc.function[4], desc.function[5] };
      cVEC2Df tempPos     = desc.viewPos;

      for(i = 0; i < 6; ++i) desc.function[i] = 0;

      desc.viewPos = { 0.0f, 0.0f };
      cui32 overlay = CreateTextList(desc);

      for(i = 0; i < 6; ++i) desc.function[i] = tempPtrs[i];

      desc.viewPos = tempPos;
      cui32 panelVert = element[panel].vertexIndex;
      cui32 firstVert = element[overlay].vertexIndex;

      for(i = 0; i < element[overlay].vertexCount[1]; ++i) element_dgs[firstVert + i].parentIndex = panelVert;

      element[panel].func.hover[0]     = PassiveDefaultArray;
      element[panel].func.hover[1]     = Null;
      element[panel].func.activate0[0] = Activate00DefaultArray;
      element[panel].func.activate0[1] = Activate01DefaultArray;
      element[panel].func.activate1[0] = Activate10DefaultArray;

      return { panel, overlay };
   }

   void DisableElements(csi16 elementIndex, csi16 count) {
///--- To do...
   }

   csi16 DestroyElements(csi16 elementIndex, csi16 count) {
///--- To do...

      return 0; // Return old index of relocated element
   }

   cbool SaveElement(cwchptrc filename, csi32 elementIndex) const { // Returns false if unsuccessful
      // Element slot is empty
      if(element[elementIndex].vertexIndex == -1) return false;

      cHANDLE hElementData = files.OpenFileForWriting(filename, stGUIDir);
      if(!hElementData) return false;

      // Write tag line: 2[Engine].Element.2[Format version]1[Compression method]
      files.WriteToFile(hElementData, "AE.Element.01u\0", 15);
      // Write primary element data
      files.WriteToFile(hElementData, &element[elementIndex], sizeof(GUI_ELEMENT));
      // Write GUI_EL_DGS data
      files.WriteToFile(hElementData, &element_dgs[element[elementIndex].vertexIndex], sizeof(GUI_EL_DGS) * element[elementIndex].vertexCount[1]);

      files.CloseFile(hElementData);

      return true;
   }

   cbool SaveElements(cwchptrc filename, csi32 firstIndex, si32 count) const { // Returns false if unsuccessful
      si32 index, lastIndex;

      // Element slot is empty
      if(element[firstIndex].vertexIndex == -1) return false;

      cHANDLE hElementData = files.OpenFileForWriting(filename, stGUIDir);
      if(!hElementData) return false;

      // Write tag line: 2[Engine].ElementGroup.2[Format version]1[Compression method]
      files.WriteToFile(hElementData, "AE.ElementGroup.01u\0", 20);

      // Truncate count if unused entry found
      for(index = (lastIndex = firstIndex) + count; (element[index].vertexIndex != -1) && (lastIndex < index); ++lastIndex);
      count = index - lastIndex;
      // Write element count
      files.WriteToFile(hElementData, &count, sizeof(count));
      // Write element data
      files.WriteToFile(hElementData, &element[firstIndex], sizeof(GUI_ELEMENT) * count);
      // Write GUI_EL_DGS data
      for(index = firstIndex; index < lastIndex; ++index)
         files.WriteToFile(hElementData, &element_dgs[element[index].vertexIndex], sizeof(GUI_EL_DGS) * element[index].vertexCount[1]);

      files.CloseFile(hElementData);

      return false;
   }

   cui32 CreateInterface(cui16 maxVertices, ui16 maxInputs) {
      if(siInterfaces >= MAX_INTERFACES) return 0x080000001; // All interface slots occupied

      interfaceProfile[siInterfaces].vertex      = (ui32ptr)zalloc32(sizeof(ui32) * maxVertices);
      interfaceProfile[siInterfaces].vertCount   = 0;
      interfaceProfile[siInterfaces].maxVertices = maxVertices;
      interfaceProfile[siInterfaces].inputs      = zalloc32(sizeof(ui64[8]) * maxInputs);
      interfaceProfile[siInterfaces].inputLabel  = (char (*)[32])zalloc32(sizeof(char (*)[32]) * maxInputs);
      interfaceProfile[siInterfaces].inputCount  = 0;
      interfaceProfile[siInterfaces].maxInputs   = maxInputs;

      return siInterfaces++;
   }

   cui32 CreateInterface(cui16ptrc elementIndices, cui16 elementCount, ui16 maxVertices, ui16 maxInputs) {
      al4 ui16 i, j;

      if(siInterfaces >= MAX_INTERFACES) return 0x080000001; // All interface slots occupied

      for(i = 0, j = 0; i < elementCount; ++i) j += element[elementIndices[i]].vertexCount[1];

      if(maxVertices < j) maxVertices = j;
      if(maxInputs > MAX_INTERFACE_INPUTS) maxVertices = MAX_INTERFACE_INPUTS;

      interfaceProfile[siInterfaces].vertex      = (ui32ptr)zalloc32(sizeof(ui32) * j);
      interfaceProfile[siInterfaces].vertCount   = j;
      interfaceProfile[siInterfaces].maxVertices = maxVertices;

      for(i = 0, j = 0; i < elementCount && j < MAX_INTERFACE_VERTS; ++i) {
         cui16 elementIndex = elementIndices[i];

         if(elementIndex < 0x0FFFFFFFF) {
            csi32 vertexIndex = element[elementIndex].vertexIndex;
            cui16 runLength   = element[elementIndex].vertexCount[1];
            ui16  offset      = 0;

            do interfaceProfile[siInterfaces].vertex[j++] = vertexIndex + offset++;
            while(offset < runLength);
         }
      }

      interfaceProfile[siInterfaces].inputs     = zalloc32(sizeof(ui64[8]) * maxInputs);
      interfaceProfile[siInterfaces].inputLabel = (char (*)[32])zalloc32(sizeof(char (*)[32]) * maxInputs);
      interfaceProfile[siInterfaces].inputCount = 0;
      interfaceProfile[siInterfaces].maxInputs  = maxInputs;

      return siInterfaces++;
   }

   // Returns index of interface, or 0x080000001 if all interface slots occupied
   cui32 LoadInterface(cwchptrc filename, si16 interfaceIndex) {
      // Find first available slot if index is -1
      if(interfaceIndex == -1) {
         si16 ii = 0;
         for(; interfaceProfile[ii].vertCount && ii < MAX_INTERFACES; ++ii);
         if(ii >= MAX_INTERFACES) return 0x080000001;
         interfaceIndex = ii;
      }

      cHANDLE hInterfaceData = files.OpenFileForReading(filename, stGUIDir);

      // Read (and discard) tag line
      if(!files.ReadAndVerify(hInterfaceData, L"AE.Interface.01u")) Try("Invalid interface file", -2, ss_gui);
      // Read primary data

///--- To do...
   }
private:
   inline void WriteVertexData(cHANDLE fileHandle, cui32 vertexIndex, cui32 newParentIndex, cui8 newAtlasIndex) const {
      GUI_EL_DGS curElDGS = element_dgs[vertexIndex];

      curElDGS.parentIndex = newParentIndex;
      curElDGS.atlasIndex  = newAtlasIndex;

      files.WriteToFile(fileHandle, &curElDGS, sizeof(GUI_EL_DGS));
   }
public:
   // Returns false if unsuccessful
   cbool SaveInterface(cwchptrc filename, csi16 interfaceIndex) {
      cGUI_INTERFACE &curProfile = interfaceProfile[interfaceIndex];

      ui32 i, j, k;
      ui32 lowestVertex      = 0x0FFFFFFFFu;
      ui32 newVertexCount    = 0;
      ui32 newParentIndex    = 0;
      ui32 newFunctionIndex  = 0;
      ui32 newCBOSIndex      = 0;
      ui16 newSoundBankIndex = 0;
      ui8  newAtlasIndex     = 0;

      // Interface slot is empty?
      if(curProfile.vertCount == 0) return false;

      cHANDLE hInterfaceData = files.OpenFileForWriting(filename, stGUIDir);
      if(!hInterfaceData) return false;

      // Write tag line: 2[Engine].Interface.2[Format version]1[Compression method]
      files.WriteToFile(hInterfaceData, "AE.Interface.01u\0", 17u);
      // Write current & max counts
      files.WriteToFile(hInterfaceData, &curProfile.maxVertices, 8u);
      // Write input bit patterns and text labels
      files.WriteToFile(hInterfaceData, curProfile.input64, curProfile.inputCount * 4u);
      files.WriteToFile(hInterfaceData, curProfile.inputLabels, curProfile.inputCount * 32u);
      // Count unique .soundBankIndex, .function, .atlasIndex, and text blocks entries
      for(i = 0; i < curProfile.vertCount; ++i) newVertexCount += element[curProfile.vertex[i]].vertexCount[0]; // Total vertex count
      GUI_IO_DATA ioData(curProfile.vertCount, newVertexCount);
      // Discover all unique references for .soundBankIndex, .function, .atlasIndex, and text blocks
      for(i = 0; i < curProfile.vertCount; ++i) {
         cGUI_ELEMENT &curElement = element[curProfile.vertex[i]];
         for(j = 0; (j < newSoundBankIndex) && (ioData.soundBank[j] != curElement.soundBankIndex); ++j);
         if(j >= newSoundBankIndex) ioData.soundBank[newSoundBankIndex++] = curElement.soundBankIndex;
         for(j = 0; j < 6u; ++j) {
            for(k = 0; (k < newFunctionIndex) && (ioData.function[k] != curElement.function[j]); ++k);
            if(k >= newFunctionIndex) ioData.function[newFunctionIndex++] = curElement.function[j];
         }
         for(j = 0; j < curElement.vertexCount[0]; ++j) {
            for(k = 0; (k < newAtlasIndex) && (ioData.atlas[k] != element_dgs[curElement.vertexIndex + j].atlasIndex); ++k);
            if(k >= newAtlasIndex) ioData.atlas[newAtlasIndex++] = element_dgs[curElement.vertexIndex + j].atlasIndex;
         }
         if(curElement.elementType == aet_text)
            for(j = 0; j < curElement.vertexCount[0]; ++j) {
               for(k = 0; (k < newCBOSIndex) && (ioData.charBankOS[k] != (element_dgs[curElement.vertexIndex + j].charBankOS & 0x03FFFFFFu)); ++k);
               if(k >= newCBOSIndex) ioData.charBankOS[newCBOSIndex++] = element_dgs[curElement.vertexIndex + j].charBankOS & 0x03FFFFFFu;
            }
         if(curElement.elementType == aet_textArray) {
            ui32 l;
            for(j = 0; j < (ui32)curElement.textCount; ++j) {
               cui32 curVertex = curElement.textArray[j];
               cui32 endVertex = curVertex + (element_dgs[curVertex].sibling & 0x07FFFu);
               for(k = curVertex; k <= endVertex; ++k)
                  for(l = 0; (l < newCBOSIndex) && (ioData.charBankOS[l] != (element_dgs[k].charBankOS & 0x03FFFFFFu)); ++k)
                     if(l >= newCBOSIndex) ioData.charBankOS[newCBOSIndex++] = element_dgs[k].charBankOS & 0x03FFFFFFu;
            }
         }
      }
      ui32 fileOffset = files.GetFilePosition(hInterfaceData);
      files.WriteToFile(hInterfaceData, &null64, 11u); // Reserve space for soundBank, atlas, function and text block counts
      // Write all .soundBank text names and references
      for(i = 0; i < newSoundBankIndex; ++i) {
         files.WriteToFile(hInterfaceData, &i, 2u);
         files.WriteToFile(hInterfaceData, soundLibrary[ioData.soundBank[i]].label, ui32(strlen(soundLibrary[ioData.soundBank[i]].label) + 1u));
      }
///--- Redo...
      // Write .atlasIndex filenames and references
      for(i = 0; i < curProfile.vertCount; ++i) {
         cGUI_ELEMENT &curElement = element[curProfile.vertex[i]];
         for(j = curElement.vertexIndex; j < ui32(curElement.vertexCount[0] + curElement.vertexIndex); ++j) {
            cui32 curAtlasIndex = (element_dgs[j].atlasIndex - TEX_REG_OS_GUI) >> 1u;
            if(curAtlasIndex != 0x0FFu)
               if(ioData.atlas[curAtlasIndex] == 0x0FFu) {
                  files.WriteToFile(hInterfaceData, stAtlas[curAtlasIndex], ui32((wcslen(stAtlas[curAtlasIndex]) + 1u) << 1u));
                  files.WriteToFile(hInterfaceData, &newAtlasIndex, 1u);
                  ioData.atlas[curAtlasIndex] = newAtlasIndex++;
               }
         }
      }
      // Write all .function text names
      for(i = 0; i < curProfile.vertCount; ++i) {
         cGUI_ELEMENT &curElement = element[curProfile.vertex[i]];
         for(j = 0; j < 6; ++j)
            if(curElement.function[j])
               if(ioData.function[curElement.function[j]] == 0x0FFFFFFFFu) {
                  files.WriteToFile(hInterfaceData, funcLib[curElement.function[j]].label, ui32(strlen(funcLib[curElement.function[j]].label) + 1u));
                  files.WriteToFile(hInterfaceData, &newFunctionIndex, 4u);
                  ioData.function[curElement.function[j]] = newFunctionIndex++;
               }
      }
///--- ...Redo
      // Write all text blocks
///--- To do...
      fileOffset = files.SetFilePosition(hInterfaceData, fileOffset, file_current);
      files.WriteToFile(hInterfaceData, &newSoundBankIndex, 2u);
      files.WriteToFile(hInterfaceData, &newAtlasIndex, 1u);
      files.WriteToFile(hInterfaceData, &newFunctionIndex, 4u);
      files.WriteToFile(hInterfaceData, &newCBOSIndex, 4u);
      files.SetFilePosition(hInterfaceData, fileOffset, file_current);
      // Write elements
      for(i = 0; i < curProfile.vertCount; ++i) {
         GUI_ELEMENT curElement = element[curProfile.vertex[i]];
         curElement.vertexIndex -= (si32)lowestVertex;
         // Write element data
         files.WriteToFile(hInterfaceData, &curElement.coords, 32u);     // Coordinates
         files.WriteToFile(hInterfaceData, &curElement.vertexIndex, 8u); // Vertex indices
         files.WriteToFile(hInterfaceData, &curElement.bitField, 2u);    // Flags
         files.WriteToFile(hInterfaceData, curElement.ptrVar, sizeof(ptr[2]));  // Scratch space
         switch(curElement.elementType & 0x0Fu) {
         case aet_text:
            /// List all unique .charBankOS values; write: .charBankOS as idenitifier followed by 32-char text block
            /// Loop-write vertex data
//            if(textVertex) { // Write .alphabetIndex filename --- !!! Rewrite to use reference indices !!!
//               ui32 i = 0;
//               while((i < uiLanguages) && (curElDGS.alphabetIndex != alphabet[i].pIMMos)) ++i;
//               files.WriteToFile(fileHandle, stLanguage[i], strlen(stLanguage[i]) + 1u);
//            }

            break;
         case aet_textArray:
            break;
         case aet_panel:
         case aet_button:
         case aet_toggle:
         case aet_cursor:
         case aet_dial:
            WriteVertexData(hInterfaceData, curElement.vertexIndex, newParentIndex++, newAtlasIndex);
            break;
         case aet_scalar:
            WriteVertexData(hInterfaceData, curElement.vertexIndex, newParentIndex, newAtlasIndex);
            WriteVertexData(hInterfaceData, curElement.vertexIndex + 1u, newParentIndex, newAtlasIndex);
            newParentIndex += 2;
            break;
         }
      }

      return true;
   }

   cbool DestroyInterface(csi16 interfaceIndex) { // Returns false if interface slot is empty
///--- To do...

      return true;
   }


   cui32 ResizeInterfaceInputs(csi16 interfaceIndex, cVEC4Du8 *inputList, cui16 inputCount, ui16 maxInputs) {
///--- To do...

      return 0;
   }

   cui32 AddElementToInterface(cui32 elementIndex, csi16 interfaceIndex) const {
      if(elementIndex < 0x080000000) {
         interfaceProfile[interfaceIndex].vertex[interfaceProfile[interfaceIndex].vertCount++] = elementIndex;
         RegenVertexBuffer(interfaceIndex, aev_maximum);
      }

      // Return remaining vertex indices
      return interfaceProfile[interfaceIndex].maxVertices - interfaceProfile[interfaceIndex].vertCount;
   }

   cui32 AddElementToInterface(cVEC2Du32 elementIndex, csi16 interfaceIndex) const {
      GUI_INTERFACE &curInterface = interfaceProfile[interfaceIndex];

      if(elementIndex.x < 0x080000000)
         interfaceProfile[interfaceIndex].vertex[interfaceProfile[interfaceIndex].vertCount++] = elementIndex.x;
      if(elementIndex.y < 0x080000000)
         interfaceProfile[interfaceIndex].vertex[interfaceProfile[interfaceIndex].vertCount++] = elementIndex.y;
      if((elementIndex.x < 0x080000000) || (elementIndex.y < 0x080000000))
         RegenVertexBuffer(interfaceIndex, aev_maximum);

      // Return remaining vertex indices
      return interfaceProfile[interfaceIndex].maxVertices - interfaceProfile[interfaceIndex].vertCount;
   }

   cui32 AddElementToInterface(cVEC3Du32 elementIndex, csi16 interfaceIndex) const {
      GUI_INTERFACE &curInterface = interfaceProfile[interfaceIndex];

      if(elementIndex.x < 0x080000000)
         interfaceProfile[interfaceIndex].vertex[interfaceProfile[interfaceIndex].vertCount++] = elementIndex.x;
      if(elementIndex.y < 0x080000000)
         interfaceProfile[interfaceIndex].vertex[interfaceProfile[interfaceIndex].vertCount++] = elementIndex.y;
      if(elementIndex.z < 0x080000000)
         interfaceProfile[interfaceIndex].vertex[interfaceProfile[interfaceIndex].vertCount++] = elementIndex.z;
      if((elementIndex.x < 0x080000000) || (elementIndex.y < 0x080000000) || (elementIndex.z < 0x080000000))
         RegenVertexBuffer(interfaceIndex, aev_maximum);

      // Return remaining vertex indices
      return interfaceProfile[interfaceIndex].maxVertices - interfaceProfile[interfaceIndex].vertCount;
   }

   inline void SetTextArray(cVEC2Du32 elementIndex, si16 value) {
      cGUI_ELEMENT &curElement = element[elementIndex.y];

      if(curElement.elementType != aet_textArray) return;

      element[elementIndex.y].textEntry = value;

      for(ui32 i = 0; i < curElement.vertexCount[1]; ++i)
         element_dgs[curElement.textArray[0] + i].viewPos.y = fl32(value++);
   }

   inline void SetTextArray(cVEC2Du32 elementIndex, fl32 value) {
      cGUI_ELEMENT &curElement = element[elementIndex.y];

      if(curElement.elementType != aet_textArray) return;

      element[elementIndex.y].textEntry = si16(value);

      for(ui32 i = 0; i < curElement.vertexCount[1]; ++i)
         element_dgs[curElement.textArray[0] + i].viewPos.y = value++;
   }

   inline void OffsetTextArray(cVEC2Du32 elementIndex, cfl32 offset) {
      cGUI_ELEMENT &curElement = element[elementIndex.y];

      if(curElement.elementType != aet_textArray) return;

      for(ui32 i = 0; i < curElement.vertexCount[1]; ++i)
         element_dgs[curElement.textArray[0] + i].viewPos.y += offset;

      element[elementIndex.y].textEntry = si16(element_dgs[curElement.textArray[0]].viewPos.y);
   }

   inline void UpdateText(cchptrc text, cui32 elementIndex) {
      GUI_ELEMENT &curElement = element[elementIndex];

      csi32 vertexIndex   = curElement.vertexIndex;
      csi32 textBufferOS  = (element_dgs[vertexIndex].charBankOS & 0x03FFFFFF) << 4;
      csi32 stringLength  = si32(strlen(text));
      csi32 textBlockSize = max(1, (stringLength + 31) >> 5);

      if(textBlockSize >= curElement.vertexCount[1]) {
         curElement.charCount      = min(stringLength, textBlockSize << 5);
         curElement.vertexCount[0] = curElement.vertexCount[1];
         csize_t truncBlock = (size_t(curElement.vertexCount[1]) << 5) - 1;
         strncpy(&textBuffer[textBufferOS], text, truncBlock);
         textBuffer[textBufferOS + truncBlock] = 0;
      } else {
         curElement.charCount      = stringLength;
         curElement.vertexCount[0] = textBlockSize;
         strncpy(&textBuffer[textBufferOS], text, stringLength);
         csi32 deltaIndex = (textBufferOS + stringLength) & 0x01F;
         if(deltaIndex) {
            if(deltaIndex < 24) {
               csi32 textBlockEnd = textBufferOS + (textBlockSize << 5);
               textBuffer[textBlockEnd - 8] = 0;
               if(deltaIndex < 16) {
                  textBuffer[textBlockEnd - 16] = 0;
                  if(deltaIndex < 8)
                     textBuffer[textBlockEnd - 24] = 0;
               }
            }
         } else
            textBuffer[textBufferOS + stringLength] = 0;
      }
      for(ui8 i = 0; i < curElement.vertexCount[0]; ++i) {
         csi32 offset = vertexIndex + i;
         cui32 step   = stringLength - (i << 5);

         element_dgs[offset].align      &= 0x07F;
         element_dgs[offset].charBankOS = (i << 1u) + (textBufferOS >> 4u) + (min(step, 32u) << 26u);
         element_dgs[offset].origin[0]   = CalculateTextOrigin(vertexIndex, offset, element_dgs[offset].alphabetIndex);
         element_dgs[offset].origin[1]   = (step > 8u ? cVEC2Df{ 16384.0f, 16384.0f } : element_dgs[offset].origin[0] );
         element_dgs[offset].origin[2]   = (step > 16u ? cVEC2Df{ 16384.0f, 16384.0f } : element_dgs[offset].origin[1] );
         element_dgs[offset].origin[3]   = (step > 24u ? cVEC2Df{ 16384.0f, 16384.0f } : element_dgs[offset].origin[2] );
      }
      for(ui8 i = textBlockSize; i < curElement.vertexCount[1]; ++i)
         element_dgs[vertexIndex + i].align |= 0x080;
   }

   inline void UpdateText(cchptrc text, cVEC2Du32 elementIndex) {
      GUI_ELEMENT &curElement = element[elementIndex.y];

      csi32 vertexIndex   = curElement.vertexIndex;
      csi32 textBufferOS  = (element_dgs[vertexIndex].charBankOS & 0x03FFFFFF) << 4;
      csi32 stringLength  = si32(strlen(text));
      csi32 textBlockSize = max(1, (stringLength + 31) >> 5);

      if(textBlockSize >= curElement.vertexCount[1]) {
         curElement.charCount      = textBlockSize << 5;
         curElement.vertexCount[0] = curElement.vertexCount[1];
         csize_t truncBlock = (size_t(curElement.vertexCount[1]) << 5) - 1;
         strncpy(&textBuffer[textBufferOS], text, truncBlock);
         textBuffer[textBufferOS + truncBlock] = 0;
      } else {
         curElement.charCount      = stringLength;
         curElement.vertexCount[0] = textBlockSize;
         strncpy(&textBuffer[textBufferOS], text, stringLength);
         csi32 deltaIndex = (textBufferOS + stringLength) & 0x01F;
         if(deltaIndex) {
            if(deltaIndex < 24) {
               csi32 textBlockEnd = textBufferOS + (textBlockSize << 5);
               textBuffer[textBlockEnd - 8] = 0;
               if(deltaIndex < 16) {
                  textBuffer[textBlockEnd - 16] = 0;
                  if(deltaIndex < 8)
                     textBuffer[textBlockEnd - 24] = 0;
               }
            }
         } else
            textBuffer[textBufferOS + stringLength] = 0;
      }
      for(ui8 i = 0; i < curElement.vertexCount[0]; ++i) {
         csi32 offset = vertexIndex + i;
         csi32 step   = stringLength - (i << 5);

         element_dgs[offset].align      &= 0x07F;
         element_dgs[offset].charBankOS = (i << 1u) + (textBufferOS >> 4u) + (min(step, 32u) << 26u);
         element_dgs[offset].origin[0]   = CalculateTextOrigin(vertexIndex, offset, element_dgs[offset].alphabetIndex);
         element_dgs[offset].origin[1]   = (step > 8 ? cVEC2Df{ 16384.0f, 16384.0f } : element_dgs[offset].origin[0] );
         element_dgs[offset].origin[2]   = (step > 16 ? cVEC2Df{ 16384.0f, 16384.0f } : element_dgs[offset].origin[1] );
         element_dgs[offset].origin[3]   = (step > 24 ? cVEC2Df{ 16384.0f, 16384.0f } : element_dgs[offset].origin[2] );
      }
      for(ui8 i = textBlockSize; i < curElement.vertexCount[1]; ++i)
         element_dgs[vertexIndex + i].align |= 0x080;
   }

///--- !!! Rewrite !!!
   inline void UpdateText(cchptrcptrc stringArray, cui32ptrc elementArray, cui32 elementCount) { for(ui32 i = 0; i < elementCount; ++i) UpdateText(stringArray[i], elementArray[i]); }

///--- !!! Rewrite !!!
   inline void UpdateText(cchptrcptrc stringArray, cui32ptrc elementArray, cVEC2Du32 countAndOffset) {
      for(ui32 i = countAndOffset.y; i < (countAndOffset.x + countAndOffset.y); ++i) UpdateText(stringArray[i], elementArray[i]);
   }

   ///--- !!! Rewrite !!!
   inline void UpdateText(cchptrcptrc stringArray, cui32ptrc elementArray, cui32 elementCount, cui32 elementOffset) {
      for(ui32 i = elementOffset; i < (elementCount + elementOffset); ++i) UpdateText(stringArray[i], elementArray[i]);
   }

///--- !!! Rewrite !!!
   inline void UpdateText(ptrc stringArray, cui32 maxCharCount, cui32ptrc elementArray, cui32 elementCount) {
      for(ui32 i = 0; i < elementCount; ++i) UpdateText((i * maxCharCount) + cchptrc(stringArray), elementArray[i]);
   }

///--- !!! Rewrite !!!
   inline void UpdateText(ptrc stringArray, cui32 maxCharCount, cui32ptrc elementArray, cVEC2Du32 countAndOffset) {
      for(ui32 i = countAndOffset.y; i < (countAndOffset.x + countAndOffset.y); ++i) UpdateText((i * maxCharCount) + cchptrc(stringArray), elementArray[i]);
   }

///--- !!! Rewrite !!!
   inline void UpdateText(ptrc stringArray, cui32 maxCharCount, cui32ptrc elementArray, cui32 elementCount, cui32 elementOffset) {
      for(ui32 i = elementOffset; i < (elementCount + elementOffset); ++i) UpdateText((i * maxCharCount) + cchptrc(stringArray), elementArray[i]);
   }

   inline void RegenVertexBuffer(cui32 interfaceIndex, cAE_REGEN_VERTS count) const {
      ui32 i = 0;
      for(ui32 j = 0; j < interfaceProfile[interfaceIndex].vertCount; ++j)
         for(ui32 k = 0; k < element[interfaceProfile[interfaceIndex].vertex[j]].vertexCount[count]; ++k)
            curVertexList[interfaceIndex][i++] = element[interfaceProfile[interfaceIndex].vertex[j]].vertexIndex + k;
      curVertexCount[interfaceIndex] = i;
   }

   inline void RegenTotalVertexBuffer(void) const { for(ui32 i = 0; i < uiGUIVerts; ++i) totalVertexList[i] = i; }

   inline void UploadAlphabetBuffer(cui8 context) const { gpu.buf.UpdateStructured(context, alphabet_pIMM, bufAlphabet, 1); }

///--- Modify for WRITE_NO_OVERWRITE and add mod test ---///
   inline void UploadTextBuffer(cui8 context) const { gpu.buf.UpdateStructured(context, textBuffer, bufText, 1); }

///--- Modify for WRITE_NO_OVERWRITE and add mod test ---///
   inline void UploadEntryBuffer(cui8 context) const { gpu.buf.UpdateStructuredSubresource(context, element_dgs, bufDGS, sizeof(GUI_EL_DGS) * uiGUIVerts); }

   inline void UploadVertexBuffer(cui8 context, cui32 interfaceIndex) const { gpu.buf.UpdateVertex(context, curVertexList[interfaceIndex], bufVertex, RoundUpToNearest4(curVertexCount[interfaceIndex])); }

   inline void UploadTotalVertexBuffer(cui8 context) const { gpu.buf.UpdateVertex(context, totalVertexList, bufVertex, RoundUpToNearest4(uiGUIVerts)); }

   inline void SetResourceBuffers(cui8 context, cui8 startIndex) const {
      gpu.buf.SetVSR(0, bufAlphabet, startIndex, 1);
      gpu.buf.SetVSR(0, bufText, startIndex + 1, 1);
      gpu.buf.SetGSR(0, bufAlphabet, startIndex, 1);
      gpu.buf.SetGSR(0, bufText, startIndex + 1, 1);
      for(ui8 i = 0; i < uiAtlas; ++i)
         gpu.tex.SetPS2D(0, uiAtlasTexIndex[i], i * 2 + startIndex, 1);
   }

   // Use the value in the globally accessible MAP_DESC object
   inline void DrawInterface(cui8 context) const {
      UploadTextBuffer(context);
      UploadEntryBuffer(context);
      UploadVertexBuffer(context, (*(const GUI_DESC *)ptrLib[15]).interfaceIndex);

      gpu.ren.QueueDrawGUI(context, bufVertex, bufDGS, curVertexCount[(*(const GUI_DESC *)ptrLib[15]).interfaceIndex]);
   }

   inline void DrawInterface(cui8 context, cui32 interfaceIndex) const {
      UploadTextBuffer(context);
      UploadEntryBuffer(context);
      UploadVertexBuffer(context, interfaceIndex);

      gpu.ren.QueueDrawGUI(context, bufVertex, bufDGS, curVertexCount[interfaceIndex]);
   }

   inline void DrawAllElements(cui8 context) const {
      UploadTextBuffer(context);
      UploadEntryBuffer(context);
      RegenTotalVertexBuffer();
      UploadTotalVertexBuffer(context);

      gpu.ren.QueueDrawGUI(context, bufVertex, bufDGS, uiGUIVerts);
   }

   // Returns indices of [0] top-most element under cursor, [1] element in use by button 0, and [2] element in use by button 1. If no element, relevant index will be -1.
   inline si128 ProcessInputs(const GLOBALCTRLVARS &ctrlVars, csi16 interfaceIndex) {
      cfl32   scrDim[2] = { ScrRes.dims[ScrRes.state].dim[0], ScrRes.dims[ScrRes.state].dim[1] };
      cfl32   pixPos[2] = { float(ctrlVars.curCoord[0]), float(ctrlVars.curCoord[1]) };
      cVEC2Df curPos    = { (pixPos[0] / scrDim[0]), pixPos[1] / scrDim[1] };

      VEC2Df      adjPos;
      GUI_INDICES indices[8];
      si32        siElement = -1;
      si16        i = 0;
      si16        j = 0;

      for(; i < interfaceProfile[interfaceIndex].vertCount; ++i, j = i & 0x07) {
         csi32 index = interfaceProfile[interfaceIndex].vertex[i];
         // If element inactive
         if(!(element[index].stateBits & 0x080)) continue;

         indices[j].element = index;
         indices[j].vertex = element[index].vertexIndex;

         cSSE4Df32 coords = element[index].activeCoords;

         const GUI_EL_DGS &curElVert = element_dgs[indices[j].vertex + ((element[index].elementType & 0x080) >> 7)];

         adjPos = { (curPos.x - 0.5f) * ScrRes.dims[ScrRes.state].aspectI * 2.0f, curPos.y * 2.0f - 1.0f };

         switch(curElVert.mods & 0x03) {
         case 1: // Rotate around element's center
            transrotate(adjPos, VEC2Df{ (coords.x1 + coords.x2 - 1.0f) * ScrRes.dims[ScrRes.state].aspectI, coords.y1 + coords.y2 - 1.0f }, curElVert.rotAngle);
            break;
         case 2: // Translate from parent's center
            break;
         case 3: // Rotate around parent's center
            transrotate(adjPos, curElVert.rotAngle);
            break;
         }

         adjPos.x *= ScrRes.dims[ScrRes.state].aspect * 0.5f;
         adjPos.y *= 0.5f;
         adjPos.x += 0.5f;
         adjPos.y += 0.5f;

         if(adjPos.x > coords.x1 && adjPos.y > coords.y1 && adjPos.x < coords.x2 && adjPos.y < coords.y2) {
            siElement = index;
            // onHover
            if(!(element[index].stateBits & 0x01) && element[index].function[0])
               funcLib[element[index].func.hover[0]].func1a(indices[j].data);
            // onActivate for button 0
            if(ctrlVars.imm.k[16] & 0x01 && element[index].function[2]) {
               activeElement.xmm0.m128i_i32[0] = index;
               funcLib[element[index].func.activate0[0]].func1a(indices[j].data);
            }
            // onActivate for button 1
            if(ctrlVars.imm.k[16] & 0x02 && element[index].function[4]) {
               activeElement.xmm0.m128i_i32[1] = index;
               funcLib[element[index].func.activate1[0]].func1a(indices[j].data);
            }
         } else
            // offHover
            if(!(element[index].stateBits & 0x02) && element[index].function[1])
               funcLib[element[index].func.hover[1]].func1a(indices[j].data);
         // offActivate for button 0
         if(!(ctrlVars.imm.k[16] & 0x01) && element[index].function[3]) {
            activeElement.xmm0.m128i_i32[0] = -1;
            funcLib[element[index].func.activate0[1]].func1a(indices[j].data);
         }
         // offActivate for button 1
         if(!(ctrlVars.imm.k[16] & 0x02) && element[index].function[5]) {
            activeElement.xmm0.m128i_i32[1] = -1;
            funcLib[element[index].func.activate1[1]].func1a(indices[j].data);
         }
      }

      return { .m128i_i32 = { siElement, activeElement.xmm0.m128i_i32[0], activeElement.xmm0.m128i_i32[1] } };
   }

   // Returns indices of [0~6] element in use by inputMask 0~6, [7] top-most element under cursor. If no element, relevant index will be -1.
   inline cAVX8Ds32 ProcessInputs(INPUT_PROC_DATA &ipd, GLOBALCTRLVARS &ctrlVars, csi16 interfaceIndex) {
      cfl32   scrDim[2] = { ScrRes.dims[ScrRes.state].dim[0], ScrRes.dims[ScrRes.state].dim[1] };
      cfl32   pixPos[2] = { float(ctrlVars.curCoord[0]), float(ctrlVars.curCoord[1]) };
      cVEC2Df curPos    = { (pixPos[0] / scrDim[0]), pixPos[1] / scrDim[1] };

      VEC2Df      adjPos;
      GUI_INDICES indices[8];
      si16        i = 0;
      si16        j = 0;
      ui8         funcCount;
      ui8         nextFuncIndex;

      activeElement.ymm.m256i_i32[7] = -1;

      for(; i < interfaceProfile[interfaceIndex].vertCount; ++i, j = i & 0x07) {
         csi32 index = interfaceProfile[interfaceIndex].vertex[i];

         // If element inactive
         if(!(element[index].stateBits & 0x080)) continue;

         const GUI_EL_DGS &curElVert = element_dgs[element[index].vertexIndex + ((element[index].elementType & 0x080) >> 7)];
         const SSE4Df32    coords    = element[index].activeCoords;

         adjPos = { (curPos.x - 0.5f) * ScrRes.dims[ScrRes.state].aspectI * 2.0f, curPos.y * 2.0f - 1.0f };

         indices[j].element = index;
         indices[j].vertex  = element[index].vertexIndex;

         nextFuncIndex = ipd.input.global; // Offset into UI function list

         switch(curElVert.mods & 0x03) {
         case 1: // Rotate around element's center
            transrotate(adjPos, VEC2Df{ (coords.x1 + coords.x2 - 1.0f) * ScrRes.dims[ScrRes.state].aspectI, coords.y1 + coords.y2 - 1.0f }, curElVert.rotAngle);
            break;
         case 2: // Translate from parent's center
            break;
         case 3: // Rotate around parent's center
            transrotate(adjPos, curElVert.rotAngle);
            break;
         }

         adjPos.x *= ScrRes.dims[ScrRes.state].aspect * 0.5f;
         adjPos.x += 0.5f;
         adjPos.y *= 0.5f;
         adjPos.y += 0.5f;

         if(adjPos.x > coords.x1 && adjPos.y > coords.y1 && adjPos.x < coords.x2 && adjPos.y < coords.y2) {
            activeElement.ymm.m256i_i32[0] = index;

            // onHover
            if(!(element[index].stateBits & 0x01) && element[index].function[0])
               funcLib[element[index].func.hover[0]].func1a(indices[j].data);

            // onActivate for inputMask[0]
            if(ipd.input.ui >= 1 && !AllFalse(ctrlVars.imm.button, ipd.inputMask[0])) {
               activeElement.ymm.m256i_i32[1] = index;
               if(element[index].function[2]) funcLib[element[index].func.activate0[0]].func1a(indices[j].data);
               for(funcCount = 0; funcCount < ipd.funcCount[0].x; funcCount++, nextFuncIndex++)
                  ipd.function[nextFuncIndex](indices[j].data);
            }

            // onActivate for inputMask[1]
            if(ipd.input.ui >= 2 && !AllFalse(ctrlVars.imm.button, ipd.inputMask[1])) {
               activeElement.ymm.m256i_i32[2] = index;
               if(element[index].function[4]) funcLib[element[index].func.activate1[0]].func1a(indices[j].data);
               for(funcCount = 0; funcCount < ipd.funcCount[1].x; funcCount++, nextFuncIndex++)
                  ipd.function[nextFuncIndex](indices[j].data);
            }

            // onActivate for inputMask[2~]
            for(ui8 imCount = 2; imCount < ipd.input.ui; imCount++) {
               if(!AllFalse(ctrlVars.imm.button, ipd.inputMask[imCount])) {
                  if(imCount < 7) activeElement.ymm.m256i_i32[imCount + 1] = index;
                  for(funcCount = 0; funcCount < ipd.funcCount[imCount].x; funcCount++, nextFuncIndex++)
                     ipd.function[nextFuncIndex](indices[j].data);
               }
            }
         } else
            // offHover
            if(!(element[index].stateBits & 0x02) && element[index].function[1])
               funcLib[element[index].func.hover[1]].func1a(indices[j].data);

         // offActivate for inputMask[0]
         if(ipd.input.ui >= 1 && AllFalse(ctrlVars.imm.button, ipd.inputMask[0])) {
            activeElement.ymm.m256i_i32[0] = -1;
            if(element[index].function[3]) funcLib[element[index].func.activate0[1]].func1a(indices[j].data);
            for(funcCount = 0; funcCount < ipd.funcCount[0].y; funcCount++, nextFuncIndex++)
               ipd.function[nextFuncIndex](indices[j].data);
         }

         // offActivate for inputMask[1]
         if(ipd.input.ui >= 2 && AllFalse(ctrlVars.imm.button, ipd.inputMask[1])) {
            activeElement.ymm.m256i_i32[1] = -1;
            if(element[index].function[5]) funcLib[element[index].func.activate1[1]].func1a(indices[j].data);
            for(funcCount = 0; funcCount < ipd.funcCount[1].y; funcCount++, nextFuncIndex++)
               ipd.function[nextFuncIndex](indices[j].data);
         }

         // onActivate for inputMask[2~]
         for(ui8 imCount = 2; imCount < ipd.input.ui; imCount++) {
            if(!AllFalse(ctrlVars.imm.button, ipd.inputMask[imCount])) {
               if(imCount < 7) activeElement.ymm.m256i_i32[imCount + 1] = -1;
               for(funcCount = 0; funcCount < ipd.funcCount[imCount].x; funcCount++, nextFuncIndex++)
                  ipd.function[nextFuncIndex](indices[j].data);
            }
         }
      }

      return activeElement;
   }
};

typedef const CLASS_GUI cCLASS_GUI;
