/*******************************************************************************  
 * File: class_gui.h                                       Created: 2023/01/26 *
 *                                                   Last modified: 2024/05/24 *
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

#include "pch.h"
#include "Data structures.h"
#include "GUI structures.h"
#include "Armada Intelligence/GUI functions.h"
#include "Armada Intelligence/Input functions.h"
#include "File operations.h"

#define MAX_ATLAS            48
#define MAX_ALPHABETS        48
#define MAX_CHARACTERS       1024
#define MAX_NAME_SIZE        64
#define MAX_TEXT_BUFFER      32768 // 32KB VRAM
#define MAX_GUI_SPRITE_LIBS  16
#define MAX_GUI_LIB_SPRITES  256
#define MAX_GUI_ENTRIES      2048  // 128KB VRAM
#define MAX_GUI_ELEMENTS     1024
#define MAX_INTERFACES       32
#define MAX_INTERFACE_VERTS  256
#define MAX_INTERFACE_INPUTS 256

extern TEXTBUFFER textBufferInfo; // Buffer information for character input

al32 struct CLASS_GUI {
   D3D11_MAPPED_SUBRESOURCE ms {};

   al16 ID3D11DeviceContext **devcon;

   CLASS_FILEOPS  *files;
   CLASS_CONFIG   *cfg;
   CLASS_TEXTURES *tex;
   CLASS_BUFFERS  *buf;
   GUI_INTERFACE  *interfaceProfile;
   ui16           *element_vert;
   GUI_EL_DGS     *element_dgs;
   GUI_ELEMENT    *element;
   CHAR_IMM       *alphabet_pIMM;
   ALPHABET       *alphabet; // 16KB per 1024 characters
   GUI_SPRITE_LIB *spriteLib;

   wchar  (*stLanguage)[MAX_ALPHABETS], (*stAtlas)[MAX_ATLAS];
   chptr    textBuffer; ///--- Change to array of pointers, allocated on demand? ---///
   LPDWORD  pBytes = (LPDWORD)&uiBytes;
   cwchar   stGUIDir[13] = L"ui_elements\\";

   AVX8Ds32 activeElement = { .ymm = { .m256i_i32 = {-1, -1, -1, -1, -1, -1, -1, -1} } };
   ui32     uiBytes;
   si32     siTextBufferOS = 0, siGUIElements = 0, siGUIVerts = 0;
   si16     bufDGS, bufAlphabet, bufText, bufVertex;
   si16     siAtlasTexIndex[MAX_ATLAS] = {};
   si16     siInterfaces = 0;
   ui8      uiLanguages = 0, uiAtlas = 0;

#ifdef AE_PTR_LIB
   CLASS_GUI(void) { ptrLib[4] = this; }
#endif

   void _Initialise(void) { ///- Add input variables for setting max values (-1 for defaults)
      spriteLib        = (GUI_SPRITE_LIB *)zalloc32(sizeof(GUI_SPRITE_LIB[MAX_GUI_SPRITE_LIBS]));
      alphabet_pIMM    = (CHAR_IMM *)zalloc32(sizeof(CHAR_IMM[MAX_ALPHABETS][MAX_CHARACTERS]));
      alphabet         = (ALPHABET *)zalloc32(sizeof(ALPHABET[MAX_ALPHABETS]));
      stLanguage       = (wchar(*)[MAX_ALPHABETS])zalloc32(sizeof(wchar[MAX_ALPHABETS][MAX_NAME_SIZE]));
      stAtlas          = (wchar(*)[MAX_ATLAS])zalloc32(sizeof(wchar[MAX_ATLAS][MAX_NAME_SIZE]));
      interfaceProfile = (GUI_INTERFACE *)zalloc32(sizeof(GUI_INTERFACE[MAX_INTERFACES]));
      textBuffer       = (chptr)zalloc32(sizeof(char[MAX_TEXT_BUFFER]));
      element_vert     = (ui16ptr)zalloc32(sizeof(ui16[MAX_GUI_ENTRIES]));
      element_dgs      = (GUI_EL_DGS *)zalloc32(sizeof(GUI_EL_DGS[MAX_GUI_ENTRIES]));
      element          = (GUI_ELEMENT *)zalloc32(sizeof(GUI_ELEMENT[MAX_GUI_ELEMENTS]));
      bufDGS           = buf->CreateStructured(0, element_dgs, sizeof(GUI_EL_DGS), MAX_GUI_ENTRIES, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS, USAGE_DEFAULT);
      bufAlphabet      = buf->CreateStructured(0, alphabet_pIMM, sizeof(CHAR_IMM), MAX_ALPHABETS * MAX_CHARACTERS, USAGE_DYNAMIC);
      bufText          = buf->CreateStructured(0, textBuffer, 16, MAX_TEXT_BUFFER >> 4, USAGE_DYNAMIC);
      bufVertex        = buf->CreateVertex(element_vert, sizeof(ui16), MAX_GUI_ENTRIES, 1);
   }

   void _Deinitialise(void) {
      ui32 i;

      if(element) mdealloc(element);
      if(element_dgs) mdealloc(element_dgs);
//      if(vertexRunLength) mfree(element_dgs);
      if(textBuffer) mdealloc(textBuffer);
      for(i = MAX_ATLAS; i; i++)
         if(siAtlasTexIndex[i])
            tex->Unload2D(siAtlasTexIndex[i]);
//      mfree(vertex_index);
      for(i = siInterfaces; i; i++)
         if(interfaceProfile[i].vertex)
            mdealloc(interfaceProfile[i].vertex);
      mfree(interfaceProfile, stAtlas, stLanguage, alphabet, alphabet_pIMM, spriteLib);
      uiLanguages = uiAtlas = 0;
   }

   cui32 CreateAtlas(cwchptrc atlasName, si16 atlasIndex, csi16 textureIndex) {
      // Find first available slot if index is -1
      if(atlasIndex == -1) {
         ui8 i = 0;
         for(; stAtlas[i] && i < MAX_ATLAS; i++);
         if(i >= MAX_ATLAS) return 0x080000001; // All atlas slots occupied
         atlasIndex = i;
      }
      wcscpy(stAtlas[atlasIndex], atlasName);
      siAtlasTexIndex[uiAtlas++] = textureIndex;
//      if(atlasIndex >= uiAtlas) uiAtlas++;

      return atlasIndex;
   }

   cui32 LoadAtlas(cwchptrc atlasFilename, si16 atlasIndex) {
      // Find first available slot if index is -1
      if(atlasIndex == -1) {
         ui8 i = 0;
         for(; stAtlas[i] && i < MAX_ATLAS; i++);
         if(i >= MAX_ATLAS) return 0x080000001; // All atlas slots occupied
         atlasIndex = i;
      }
      wsprintf(files->stTemp, L"textures\\gui\\%s%s", atlasFilename, L".dds");
      csi32 index = tex->Load2D(files->stTemp);
      wcscpy(stAtlas[atlasIndex], atlasFilename);
      siAtlasTexIndex[uiAtlas++] = index;
//      if(atlasIndex >= uiAtlas) uiAtlas++;

      return atlasIndex;
   }

   cui32 DestroyAtlas(csi16 atlasIndex) {
///--- To do...
   }

   cui32 CreateSpriteLibrary(si32 libraryIndex, csi16 maxSprites, csi16 atlasIndex) const {
      // Find first available slot if index is -1
      if(libraryIndex == -1) {
         ui8 i = 0;
         for(; spriteLib[i].maxSprites && i < MAX_GUI_SPRITE_LIBS; i++);
         if(i >= MAX_GUI_SPRITE_LIBS) return 0x080000001; // All atlas slots occupied
         libraryIndex = i;
      }
      spriteLib[libraryIndex] = { (GUI_SPRITE *)zalloc32(sizeof(GUI_SPRITE) * maxSprites), (ui16)atlasIndex, 0, (ui16)maxSprites };

      return libraryIndex;
   }

   cui32 DestroySpriteLibrary(csi32 index) {
///--- To do...
   }

   cui32 CreateSprite(csi32 libIndex, wchptrc name, VEC4Df texCoords, cVEC2Df activeArea) const {
      ui16 &numSprites = spriteLib[libIndex].numSprites;

      if(numSprites >= MAX_GUI_LIB_SPRITES) return 0x080000001;
      spriteLib[libIndex].sprite[numSprites] = { name, texCoords, activeArea };

      return numSprites++;
   }

   cui32 CreateSprite(csi32 libIndex, GUI_SPRITE spriteData, cbool flipY) const {
      ui16 &numSprites = spriteLib[libIndex].numSprites;

      if(numSprites >= MAX_GUI_LIB_SPRITES) return 0x080000001;

      if(flipY) {
         const fl32 temp  = spriteData.tc.y1;
         spriteData.tc.y1 = spriteData.tc.y2;
         spriteData.tc.y2 = temp;
      }

      spriteLib[libIndex].sprite[numSprites] = spriteData;

      return numSprites++;
   }

   cui32 DestroySprite(csi32 libIndex, csi16 spriteIndex) {
///--- To do...
   }

   cui16 CreateAlphabet(cwchptrc languageName, csi32 xSegments, csi32 ySegments, cVEC2Df charSize, si16 alphabetIndex, csi16 atlasIndex) {
      // Find first available slot if alphabetIndex is -1
      if(alphabetIndex == -1) {
         si16 i = 0;
         for(; alphabet[i].stLanguage && i < MAX_ALPHABETS; i++);
         if(i >= MAX_ALPHABETS) return 0x08001; // All alphabet slots occupied
         alphabetIndex = i;
      }
      if(xSegments < 1 || ySegments < 1) return 0x08002;
      if(xSegments * ySegments > MAX_CHARACTERS) return 0x08003;

      cui32 pIMMos = alphabetIndex * MAX_CHARACTERS;
      wcscpy(stLanguage[uiLanguages], languageName);
      alphabet[alphabetIndex].stLanguage = stLanguage[uiLanguages++];
      alphabet[alphabetIndex].stAtlasFilename = stAtlas[atlasIndex];
      alphabet[alphabetIndex].atlasIndex = atlasIndex;
      alphabet[alphabetIndex].numChars = xSegments * ySegments;
      alphabet[alphabetIndex].pIMMos = pIMMos;
      for(si32 iY = 0; iY < ySegments; iY++)
         for(si32 iX = 0; iX < xSegments; iX++) {
            // Texture offsets calculated per entry to maintain accuracy
            cui32 i = iX + (iY * xSegments);
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

   cui32 LoadAlphabet(cwchptrc filename, si16 index) {
      // Find first available slot if index is -1
      if(index == -1) {
         si16 i = 0;
         for(; alphabet[i].stLanguage && i < MAX_ALPHABETS; i++);
         if(i >= MAX_ALPHABETS) return 0x080000001; // All alphabet slots occupied
         index = i;
      }

      wcscpy(files->stTemp, stGUIDir);
      wcscpy(files->stTemp + wcslen(stGUIDir), filename);
      HANDLE hAlphabetData = CreateFile(files->stTemp, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, 0);

      // Read (and discard) tag line
      files->ReadLine(files->stTemp, hAlphabetData);
      if(wcsncmp(files->stTemp, L"AE.Alphabet.01u", 15)) Try(stLoadAlpha, -2);
      // Read primary data
      files->ReadLine(stLanguage[uiLanguages], hAlphabetData);
      files->ReadLine(stAtlas[uiAtlas], hAlphabetData);

///--- Add searching and/or loading of atlas texture

      alphabet[uiLanguages].atlasIndex = siAtlasTexIndex[uiAtlas++];
      ReadFile(hAlphabetData, &alphabet[uiLanguages].numChars, sizeof(ui16), pBytes, 0);
      alphabet[uiLanguages].pIMMos = uiLanguages * MAX_CHARACTERS;
      // Read character data
      ReadFile(hAlphabetData, &alphabet_pIMM[uiLanguages * MAX_CHARACTERS], sizeof(CHAR_IMM) * alphabet[uiLanguages++].numChars, pBytes, 0);

      CloseHandle(hAlphabetData);

      return index;
   }

   cui32 SaveAlphabet(cwchptrc filename, si16 index) const {
      // Alphabet slot is empty
      if(!alphabet[index].stLanguage) return 0x080000001;

      wcscpy(files->stTemp, stGUIDir);
      wcscpy(files->stTemp + wcslen(stGUIDir), filename);
      HANDLE hAlphabetData = CreateFile(files->stTemp, GENERIC_WRITE, FILE_SHARE_WRITE, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

      // Write tag line: 2[Engine].Alphabet.2[Format version]1[Compression method]
      WriteFile(hAlphabetData, "AE.Alphabet.01u\0", 16, pBytes, 0);
      // Write primary data
      WriteFile(hAlphabetData, alphabet[index].stLanguage, DWORD(wcslen(alphabet[index].stLanguage) + 1) * sizeof(wchar), pBytes, 0);
      WriteFile(hAlphabetData, alphabet[index].stAtlasFilename, DWORD(wcslen(alphabet[index].stAtlasFilename) + 1) * sizeof(wchar), pBytes, 0);
      WriteFile(hAlphabetData, &alphabet[index].numChars, sizeof(ui16), pBytes, 0);
      // Write character data
      WriteFile(hAlphabetData, &alphabet_pIMM[index * MAX_CHARACTERS], sizeof(CHAR_IMM) * alphabet[index].numChars, pBytes, 0);

      CloseHandle(hAlphabetData);

      return 0;
   }

   cui32 UnloadAlphabet(si16 index) const {
///- To do...
   }

   inline cVEC2Df _CalculateOrigin(csi16 firstVert, csi16 curVert, csi16 alphabetIndex) const {
      if(firstVert == curVert) return element_dgs[firstVert].origin[0];

      cui32   uiPrevVert   = curVert - 1;
      cVEC2Df uiPrevOrigin = element_dgs[uiPrevVert].origin[0];
      cui32   uiBufIndex   = element_dgs[uiPrevVert].textArrayOS & 0x03FFFFFF;
      fl32    fOffset      = 0.0f;

      // Accumulate character widths
      for(ui8 i = 0; i < 32 && textBuffer[uiBufIndex + i]; i++)
         fOffset += alphabet_pIMM[alphabet[alphabetIndex].pIMMos + textBuffer[uiBufIndex + i]].width;

      return { fOffset * element_dgs[firstVert].size.x + uiPrevOrigin.x, uiPrevOrigin.y };
   };

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

   // Returns element index and vertex count. Set .charCount to ~0 to automatically determine max string length   ---   !!! Rewrite properly, ya lazy cunt !!!
   cui32 CreateText(const GUI_EL_DESC &desc) {
      if(desc.charCount <= 0) { // !!! Rewrite properly, ya lazy cunt !!!
         csi32 siTextSize      = si32(strlen(desc.textPtr));
         csi32 siTextBlockSize = max(1, (siTextSize + 15) & 0x0EFFFFFF0); // Round up to nearest multiple of 16
         csi32 siFirstVert     = siGUIVerts;

         if(siGUIElements >= MAX_GUI_ELEMENTS) return { 0x080000001 };
         if(siTextBlockSize > (MAX_TEXT_BUFFER - siTextBufferOS)) return { 0x080000002 };

         csi32 siTextEnd = siTextBufferOS + siTextSize;

         strncpy(&textBuffer[siTextBufferOS], desc.textPtr, siTextSize);
         if(!(siTextEnd & 0x0FFFFFFE0)) textBuffer[siTextEnd] = 0;

         // Calculate width of string view space   ---   Change to iterate & add unique character widths
         cVEC2Df activeSize = { alphabet_pIMM[alphabet[desc.index.alphabet].pIMMos + 1].width * fl32(siTextSize) * desc.size.text.x,
                                alphabet_pIMM[alphabet[desc.index.alphabet].pIMMos + 1].height * desc.size.text.y };

         cSSE4Df32 activeCoords = CalcActiveCoords(desc.viewPos, activeSize, activeSize, desc.text.align);

         element[siGUIElements].activeCoords      = activeCoords;
         element[siGUIElements].activePos         = { (activeCoords.x1 + activeCoords.x2) * 0.5f, (activeCoords.y1 + activeCoords.y2) * 0.5f };
         element[siGUIElements].viewScale         = { 2.0f / activeSize.x, 2.0f / activeSize.y };
         element[siGUIElements].vertexIndex       = siGUIVerts;
         element[siGUIElements].elementType       = 0;
         element[siGUIElements].func.hover[0]     = desc.func.hover[0];
         element[siGUIElements].func.hover[1]     = desc.func.hover[1];
         element[siGUIElements].func.activate0[0] = desc.func.activate0[0];
         element[siGUIElements].func.activate0[1] = desc.func.activate0[1];
         element[siGUIElements].func.activate1[0] = desc.func.activate1[0];
         element[siGUIElements].func.activate1[1] = desc.func.activate1[1];
         element[siGUIElements].soundBankIndex    = desc.index.soundBank;
         element[siGUIElements].stateBits         = 0x040;
         element[siGUIElements].charCount         = (ui16)siTextSize;

         element_dgs[siFirstVert].origin[0] = desc.viewPos;
         for(si32 i = 0; i < siTextBlockSize; siGUIVerts++, i += 32) {
            element_dgs[siGUIVerts].origin[0]     = _CalculateOrigin(siFirstVert, siGUIVerts, desc.index.alphabet);
            element_dgs[siGUIVerts].origin[1]     = { 16384.0f, 16384.0f };
            element_dgs[siGUIVerts].origin[2]     = { 16384.0f, 16384.0f };
            element_dgs[siGUIVerts].origin[3]     = { 16384.0f, 16384.0f };
            element_dgs[siGUIVerts].colour        = desc.tint.text.xmm;
            element_dgs[siGUIVerts].size          = desc.size.text;
            element_dgs[siGUIVerts].rotAngle      = 0.0f;
            element_dgs[siGUIVerts].width         = 0.0f;
            element_dgs[siGUIVerts].parentIndex   = 0x0FFFFFFFF; // Change to iterate & add unique character widths
            element_dgs[siGUIVerts].textArrayOS   = ((i + siTextBufferOS) >> 4) | ((siTextSize - i > 32 ? 32 : siTextSize - i) << 26);
            element_dgs[siGUIVerts].alphabetIndex = alphabet[desc.index.alphabet].pIMMos;
            element_dgs[siGUIVerts].atlasIndex    = (ui8)alphabet[desc.index.alphabet].atlasIndex;
            element_dgs[siGUIVerts].elementType   = 0;
            element_dgs[siGUIVerts].sibling       = siGUIVerts - siFirstVert;
            element_dgs[siGUIVerts].align         = desc.text.align;
            element_dgs[siGUIVerts].mods          = desc.text.mods;
         }
         element_dgs[siFirstVert].sibling = (siGUIVerts - siFirstVert) | 0x08000;

         siTextBufferOS += siTextBlockSize;
         element[siGUIElements].vertexCount[0] = element[siGUIElements].vertexCount[1] = ui16(siGUIVerts - siFirstVert);

         return (ui32)siGUIElements++;
      } else { // !!! Rewrite properly, ya lazy cunt !!!
         if(siGUIElements >= MAX_GUI_ELEMENTS) return { 0x080000001 };

         si32 siTextLength = si32(strlen(desc.textPtr));
         if(siTextLength > desc.charCount) siTextLength = desc.charCount;

         csi32 siTextSize = siTextLength;
         csi32 siTextBlockSize = max(1, (desc.charCount + 15) & 0x0EFFFFFF0); // Round up to nearest multiple of 16

         if(siTextBlockSize > (MAX_TEXT_BUFFER - siTextBufferOS)) return { 0x080000002 };

         csi32 siFirstVert = siGUIVerts;
         csi32 siTextEnd = siTextBufferOS + siTextSize;

         strncpy(&textBuffer[siTextBufferOS], desc.textPtr, siTextSize);
         if(!(siTextEnd & 0x0FFFFFFE0)) textBuffer[siTextEnd] = 0;

         // Calculate width of string view space   ---   Change to iterate & add unique character widths
         cVEC2Df activeSize = { alphabet_pIMM[alphabet[desc.index.alphabet].pIMMos + 1].width * fl32(siTextLength) * desc.size.text.x,
                                alphabet_pIMM[alphabet[desc.index.alphabet].pIMMos + 1].height * desc.size.text.y };

         cSSE4Df32 activeCoords = CalcActiveCoords(desc.viewPos, activeSize, activeSize, desc.text.align);

         element[siGUIElements].activeCoords      = activeCoords;
         element[siGUIElements].activePos         = { (activeCoords.x1 + activeCoords.x2) * 0.5f, (activeCoords.y1 + activeCoords.y2) * 0.5f };
         element[siGUIElements].viewScale         = { 2.0f / activeSize.x, 2.0f / activeSize.y };
         element[siGUIElements].vertexIndex       = siGUIVerts;
         element[siGUIElements].elementType       = 0;
         element[siGUIElements].func.hover[0]     = desc.func.hover[0];
         element[siGUIElements].func.hover[1]     = desc.func.hover[1];
         element[siGUIElements].func.activate0[0] = desc.func.activate0[0];
         element[siGUIElements].func.activate0[1] = desc.func.activate0[1];
         element[siGUIElements].func.activate1[0] = desc.func.activate1[0];
         element[siGUIElements].func.activate1[1] = desc.func.activate1[1];
         element[siGUIElements].soundBankIndex    = desc.index.soundBank;
         element[siGUIElements].stateBits         = 0x040;
         element[siGUIElements].charCount         = (ui16)siTextSize;

         si32 i = 0;
         for(element_dgs[siFirstVert].origin[0]   = desc.viewPos; i < siTextLength; siGUIVerts++, i += 32) {
            element_dgs[siGUIVerts].origin[0]     = _CalculateOrigin(siFirstVert, siGUIVerts, desc.index.alphabet);
            element_dgs[siGUIVerts].origin[1]     = { 16384.0f, 16384.0f };
            element_dgs[siGUIVerts].origin[2]     = { 16384.0f, 16384.0f };
            element_dgs[siGUIVerts].origin[3]     = { 16384.0f, 16384.0f };
            element_dgs[siGUIVerts].colour        = desc.tint.text.xmm;
            element_dgs[siGUIVerts].size          = desc.size.text;
            element_dgs[siGUIVerts].rotAngle      = 0.0f;
            element_dgs[siGUIVerts].width         = 0.0f;
            element_dgs[siGUIVerts].parentIndex   = 0x0FFFFFFFF;
            element_dgs[siGUIVerts].textArrayOS   = ((i + siTextBufferOS) >> 4) | ((siTextSize - i > 32 ? 32 : siTextSize - i) << 26);
            element_dgs[siGUIVerts].alphabetIndex = alphabet[desc.index.alphabet].pIMMos;
            element_dgs[siGUIVerts].atlasIndex    = (ui8)alphabet[desc.index.alphabet].atlasIndex;
            element_dgs[siGUIVerts].elementType   = 0;
            element_dgs[siGUIVerts].sibling       = siGUIVerts - siFirstVert;
            element_dgs[siGUIVerts].align         = desc.text.align;
            element_dgs[siGUIVerts].mods          = desc.text.mods;
         }
         for(; i < siTextBlockSize; siGUIVerts++, i += 32) {
            element_dgs[siGUIVerts].origin[0]     = { 0.0f, 0.0f };
            element_dgs[siGUIVerts].origin[1]     = { 0.0f, 0.0f };
            element_dgs[siGUIVerts].origin[2]     = { 0.0f, 0.0f };
            element_dgs[siGUIVerts].origin[3]     = { 0.0f, 0.0f };
            element_dgs[siGUIVerts].colour        = desc.tint.text.xmm;
            element_dgs[siGUIVerts].size          = desc.size.text;
            element_dgs[siGUIVerts].rotAngle      = 0.0f;
            element_dgs[siGUIVerts].width         = 0.0f;
            element_dgs[siGUIVerts].parentIndex   = 0x0FFFFFFFF;
            element_dgs[siGUIVerts].textArrayOS   = ((i + siTextBufferOS) >> 4);
            element_dgs[siGUIVerts].alphabetIndex = alphabet[desc.index.alphabet].pIMMos;
            element_dgs[siGUIVerts].atlasIndex    = (ui8)alphabet[desc.index.alphabet].atlasIndex;
            element_dgs[siGUIVerts].elementType   = 0;
            element_dgs[siGUIVerts].sibling       = siGUIVerts - siFirstVert;
            element_dgs[siGUIVerts].align         = desc.text.align | 0x080;
            element_dgs[siGUIVerts].mods          = desc.text.mods;
         }
         element_dgs[siFirstVert].sibling = (siGUIVerts - siFirstVert) | 0x08000;

         siTextBufferOS += siTextBlockSize;
         element[siGUIElements].vertexCount[0] = ui16(max(1, (siTextSize + 31) >> 5));
         element[siGUIElements].vertexCount[1] = ui16(siGUIVerts - siFirstVert);

         return (ui32)siGUIElements++;
      }
   }

   cui32 CreateCursor(const GUI_EL_DESC &desc) {
      if(siGUIElements >= MAX_GUI_ELEMENTS) return 0x080000001;

      const GUI_SPRITE_LIB &curLib = spriteLib[desc.index.spriteLib];
            GUI_SPRITE     &curSpr = curLib.sprite[desc.index.cursorSprite];

      cVEC2Df spriteSize = { fabsf(curSpr.tc.x2 - curSpr.tc.x1) * desc.size.cursor.x, fabsf(curSpr.tc.y2 - curSpr.tc.y1) * desc.size.cursor.y };
      cVEC2Df activeSize = { spriteSize.x * curSpr.aa.x, spriteSize.y * curSpr.aa.y };
      cVEC4Df viewCoords = { desc.viewPos.x - spriteSize.x, desc.viewPos.y + spriteSize.y, desc.viewPos.x + spriteSize.x, desc.viewPos.y - spriteSize.y };

      cSSE4Df32 activeCoords = CalcActiveCoords(desc.viewPos, spriteSize, activeSize, desc.cursor.align);

      element[siGUIElements].vertexCount[0] = element[siGUIElements].vertexCount[1] = 1;

      element[siGUIElements].activeCoords      = activeCoords;
      element[siGUIElements].activePos         = { (activeCoords.x1 + activeCoords.x2) * 0.5f, (activeCoords.y1 + activeCoords.y2) * 0.5f };
      element[siGUIElements].viewScale         = { 2.0f / activeSize.x, 2.0f / activeSize.y };
      element[siGUIElements].vertexIndex       = siGUIVerts;
      element[siGUIElements].elementType       = 5;
      element[siGUIElements].func.hover[0]     = desc.func.hover[0];
      element[siGUIElements].func.hover[1]     = desc.func.hover[1];
      element[siGUIElements].func.activate0[0] = desc.func.activate0[0];
      element[siGUIElements].func.activate0[1] = desc.func.activate0[1];
      element[siGUIElements].func.activate1[0] = desc.func.activate1[0];
      element[siGUIElements].func.activate1[1] = desc.func.activate1[1];
      element[siGUIElements].soundBankIndex    = desc.index.soundBank;
      element[siGUIElements].stateBits        = 0x040;
      element[siGUIElements].activeCoords     = CalcActiveCoords(desc.viewPos, spriteSize, activeSize, desc.cursor.align);
      element_dgs[siGUIVerts].viewPos      = desc.viewPos;
      element_dgs[siGUIVerts].indent       = activeSize;
      element_dgs[siGUIVerts].texCoords    = curSpr.tc;
      element_dgs[siGUIVerts].colour       = desc.tint.cursor.xmm;
      element_dgs[siGUIVerts].size         = spriteSize;
      element_dgs[siGUIVerts].rotAngle     = 0.0f;
      element_dgs[siGUIVerts].width        = 0.0f;
      element_dgs[siGUIVerts].parentIndex  = 0x0FFFFFFFF;
      element_dgs[siGUIVerts].atlasIndex   = (ui8)curLib.atlasIndex;
      element_dgs[siGUIVerts].elementType  = 5;
      element_dgs[siGUIVerts].sibling      = 0;
      element_dgs[siGUIVerts].align        = desc.cursor.align;
      element_dgs[siGUIVerts++].mods       = desc.cursor.mods;

      return siGUIElements++;
   }

   cui32 CreatePanel(const GUI_EL_DESC &desc) {
      if(siGUIElements >= MAX_GUI_ELEMENTS) return 0x080000001;

      const GUI_SPRITE_LIB &curLib = spriteLib[desc.index.spriteLib];
            GUI_SPRITE     &curSpr = curLib.sprite[desc.index.panelSprite];

      cVEC2Df spriteSize = { fabsf(curSpr.tc.x2 - curSpr.tc.x1) * desc.size.panel.x, fabsf(curSpr.tc.y2 - curSpr.tc.y1) * desc.size.panel.y };
      cVEC2Df activeSize = { spriteSize.x * curSpr.aa.x, spriteSize.y * curSpr.aa.y };
      cVEC4Df viewCoords = { desc.viewPos.x - spriteSize.x, desc.viewPos.y + spriteSize.y, desc.viewPos.x + spriteSize.x, desc.viewPos.y - spriteSize.y };

      cSSE4Df32 activeCoords = CalcActiveCoords(desc.viewPos, spriteSize, activeSize, desc.panel.align);

      element[siGUIElements].vertexCount[0] = element[siGUIElements].vertexCount[1] = 1;

      element[siGUIElements].activeCoords      = activeCoords;
      element[siGUIElements].activePos         = { (activeCoords.x1 + activeCoords.x2) * 0.5f, (activeCoords.y1 + activeCoords.y2) * 0.5f };
//      element[siGUIElements].activeZone        = { (1.0f - curSpr.aa.x) * spriteSize.x, (1.0f - curSpr.aa.y) * spriteSize.y };
      element[siGUIElements].viewScale         = { 2.0f / activeSize.x, 2.0f / activeSize.y };
      element[siGUIElements].vertexIndex       = siGUIVerts;
      element[siGUIElements].elementType       = 1;
      element[siGUIElements].func.hover[0]     = desc.func.hover[0];
      element[siGUIElements].func.hover[1]     = desc.func.hover[1];
      element[siGUIElements].func.activate0[0] = desc.func.activate0[0];
      element[siGUIElements].func.activate0[1] = desc.func.activate0[1];
      element[siGUIElements].func.activate1[0] = desc.func.activate1[0];
      element[siGUIElements].func.activate1[1] = desc.func.activate1[1];
      element[siGUIElements].soundBankIndex    = desc.index.soundBank;
      element[siGUIElements].stateBits         = 0x0C0;
      element_dgs[siGUIVerts].viewPos      = desc.viewPos;
      element_dgs[siGUIVerts].indent       = activeSize;
      element_dgs[siGUIVerts].texCoords    = curSpr.tc;
      element_dgs[siGUIVerts].colour       = desc.tint.panel.xmm;
      element_dgs[siGUIVerts].size         = spriteSize;
      element_dgs[siGUIVerts].rotAngle     = 0.0f;
      element_dgs[siGUIVerts].width        = 0.0f;
      element_dgs[siGUIVerts].parentIndex  = 0x0FFFFFFFF;
      element_dgs[siGUIVerts].atlasIndex   = (ui8)curLib.atlasIndex;
      element_dgs[siGUIVerts].elementType  = 1;
      element_dgs[siGUIVerts].sibling      = 0;
      element_dgs[siGUIVerts].align        = desc.panel.align;
      element_dgs[siGUIVerts++].mods       = desc.panel.mods;

      return siGUIElements++;
   }

   // Button's default sprite index of it's alternate state == [spriteIndex + 1]
   cui32 CreateButton(const GUI_EL_DESC &desc) {
      if(siGUIElements >= MAX_GUI_ELEMENTS) return 0x080000001;

      const GUI_SPRITE_LIB &curLib = spriteLib[desc.index.spriteLib];
            GUI_SPRITE     &curSpr = curLib.sprite[desc.index.panelSprite];

      cVEC2Df spriteSize = { fabsf(curSpr.tc.x2 - curSpr.tc.x1) * desc.size.panel.x, fabsf(curSpr.tc.y2 - curSpr.tc.y1) * desc.size.panel.y };
      cVEC2Df activeSize = { spriteSize.x * curSpr.aa.x, spriteSize.y * curSpr.aa.y };
      cVEC4Df viewCoords = { desc.viewPos.x - spriteSize.x, desc.viewPos.y + spriteSize.y, desc.viewPos.x + spriteSize.x, desc.viewPos.y - spriteSize.y };

      cSSE4Df32 activeCoords = CalcActiveCoords(desc.viewPos, spriteSize, activeSize, desc.panel.align);

      element[siGUIElements].vertexCount[0] = element[siGUIElements].vertexCount[1] = 2;

      element[siGUIElements].activeCoords      = activeCoords;
      element[siGUIElements].activePos         = { (activeCoords.x1 + activeCoords.x2) * 0.5f, (activeCoords.y1 + activeCoords.y2) * 0.5f };
//      element[siGUIElements].activeZone        = { (1.0f - curSpr.aa.x) * spriteSize.x, (1.0f - curSpr.aa.y) * spriteSize.y };
      element[siGUIElements].viewScale         = { 2.0f / activeSize.x, 2.0f / activeSize.y };
      element[siGUIElements].vertexIndex       = siGUIVerts;
      element[siGUIElements].elementType       = 2;   // bit7==Button "down"
      element[siGUIElements].func.hover[0]     = desc.func.hover[0];
      element[siGUIElements].func.hover[1]     = desc.func.hover[1];
      element[siGUIElements].func.activate0[0] = (desc.func.activate0[0] ? desc.func.activate0[0] : __Activate0_0_Default_Button);
      element[siGUIElements].func.activate0[1] = (desc.func.activate0[1] ? desc.func.activate0[1] : __Activate0_1_Default_Button);
      element[siGUIElements].func.activate1[0] = desc.func.activate1[0];
      element[siGUIElements].func.activate1[1] = desc.func.activate1[1];
      element[siGUIElements].soundBankIndex    = desc.index.soundBank;
      element[siGUIElements].stateBits         = 0x0C0;
      element_dgs[siGUIVerts].viewPos      = desc.viewPos;
      element_dgs[siGUIVerts].indent       = activeSize;
      element_dgs[siGUIVerts].texCoords    = curSpr.tc;
      element_dgs[siGUIVerts].colour       = desc.tint.panel.xmm;
      element_dgs[siGUIVerts].size         = spriteSize;
      element_dgs[siGUIVerts].rotAngle     = 0.0f;
      element_dgs[siGUIVerts].width        = 0.0f;
      element_dgs[siGUIVerts].parentIndex  = 0x0FFFFFFFF;
      element_dgs[siGUIVerts].atlasIndex   = (ui8)curLib.atlasIndex;
      element_dgs[siGUIVerts].elementType  = 2;
      element_dgs[siGUIVerts].sibling      = 0x08001;
      element_dgs[siGUIVerts].align        = desc.panel.align;
      element_dgs[siGUIVerts++].mods       = desc.panel.mods;
      element_dgs[siGUIVerts].viewPos      = desc.viewPos;
      element_dgs[siGUIVerts].indent       = activeSize;
      element_dgs[siGUIVerts].texCoords    = curLib.sprite[desc.index.panelSprite + 1].tc;
      element_dgs[siGUIVerts].colour       = desc.tint.panel.xmm;
      element_dgs[siGUIVerts].size         = spriteSize;
      element_dgs[siGUIVerts].rotAngle     = 0.0f;
      element_dgs[siGUIVerts].width        = 0.0f;
      element_dgs[siGUIVerts].parentIndex  = 0x0FFFFFFFF;
      element_dgs[siGUIVerts].atlasIndex   = (ui8)curLib.atlasIndex;
      element_dgs[siGUIVerts].elementType  = 2;
      element_dgs[siGUIVerts].sibling      = 1;
      element_dgs[siGUIVerts].align        = desc.panel.align | 0x080;
      element_dgs[siGUIVerts++].mods       = desc.panel.mods;

      return siGUIElements++;
   }

   // Button's default sprite index of it's alternate state == [spriteIndex] + 1
   cui32 CreateToggle(const GUI_EL_DESC &desc) {
      if(siGUIElements >= MAX_GUI_ELEMENTS) return 0x080000001;

      const GUI_SPRITE_LIB &curLib = spriteLib[desc.index.spriteLib];
            GUI_SPRITE     &curSpr = curLib.sprite[desc.index.panelSprite];

      cVEC2Df spriteSize = { fabsf(curSpr.tc.x2 - curSpr.tc.x1) * desc.size.panel.x, fabsf(curSpr.tc.y2 - curSpr.tc.y1) * desc.size.panel.y };
      cVEC2Df activeSize = { spriteSize.x * curSpr.aa.x, spriteSize.y * curSpr.aa.y };
      cVEC4Df viewCoords = { desc.viewPos.x - spriteSize.x, desc.viewPos.y + spriteSize.y, desc.viewPos.x + spriteSize.x, desc.viewPos.y - spriteSize.y };

      cSSE4Df32 activeCoords = CalcActiveCoords(desc.viewPos, spriteSize, activeSize, desc.panel.align);

      element[siGUIElements].vertexCount[0] = element[siGUIElements].vertexCount[1] = 2;

      element[siGUIElements].activeCoords      = activeCoords;
      element[siGUIElements].activePos         = { (activeCoords.x1 + activeCoords.x2) * 0.5f, (activeCoords.y1 + activeCoords.y2) * 0.5f };
//      element[siGUIElements].activeZone        = { (1.0f - curSpr.aa.x) * spriteSize.x, (1.0f - curSpr.aa.y) * spriteSize.y };
      element[siGUIElements].viewScale         = { 2.0f / activeSize.x, 2.0f / activeSize.y };
      element[siGUIElements].vertexIndex       = siGUIVerts;
      element[siGUIElements].elementType       = 3;   // bit7==Button "toggled"
      element[siGUIElements].func.hover[0]     = desc.func.hover[0];
      element[siGUIElements].func.hover[1]     = desc.func.hover[1];
      element[siGUIElements].func.activate0[0] = (desc.func.activate0[0] ? desc.func.activate0[0] : __Activate0_0_Default_Toggle);
      element[siGUIElements].func.activate0[1] = (desc.func.activate0[1] ? desc.func.activate0[1] : __Activate0_1_Default_Toggle);
      element[siGUIElements].func.activate1[0] = desc.func.activate1[0];
      element[siGUIElements].func.activate1[1] = desc.func.activate1[1];
      element[siGUIElements].soundBankIndex    = desc.index.soundBank;
      element[siGUIElements].stateBits         = 0x0C0;
      element_dgs[siGUIVerts].viewPos      = desc.viewPos;
      element_dgs[siGUIVerts].indent       = activeSize;
      element_dgs[siGUIVerts].texCoords    = curSpr.tc;
      element_dgs[siGUIVerts].colour       = desc.tint.panel.xmm;
      element_dgs[siGUIVerts].size         = spriteSize;
      element_dgs[siGUIVerts].rotAngle     = 0.0f;
      element_dgs[siGUIVerts].width        = 0.0f;
      element_dgs[siGUIVerts].parentIndex  = 0x0FFFFFFFF;
      element_dgs[siGUIVerts].atlasIndex   = (ui8)curLib.atlasIndex;
      element_dgs[siGUIVerts].elementType  = 3;
      element_dgs[siGUIVerts].sibling      = 0x08001;
      element_dgs[siGUIVerts].align        = desc.panel.align;
      element_dgs[siGUIVerts++].mods       = desc.panel.mods;
      element_dgs[siGUIVerts].viewPos      = desc.viewPos;
      element_dgs[siGUIVerts].indent       = activeSize;
      element_dgs[siGUIVerts].texCoords    = curLib.sprite[desc.index.panelSprite + 1].tc;
      element_dgs[siGUIVerts].colour       = desc.tint.panel.xmm;
      element_dgs[siGUIVerts].size         = spriteSize;
      element_dgs[siGUIVerts].rotAngle     = 0.0f;
      element_dgs[siGUIVerts].width        = 0.0f;
      element_dgs[siGUIVerts].parentIndex  = 0x0FFFFFFFF;
      element_dgs[siGUIVerts].atlasIndex   = (ui8)curLib.atlasIndex;
      element_dgs[siGUIVerts].elementType  = 3;
      element_dgs[siGUIVerts].sibling      = 1;
      element_dgs[siGUIVerts].align        = desc.panel.align | 0x080;
      element_dgs[siGUIVerts++].mods       = desc.panel.mods;

      return siGUIElements++;
   }

   // Returns index of panel. Default index of indicator == [return value] + 1
   // Able to create scalar elements with slider or bar/pool graph, based on .align & .mods bits
   cui32 CreateScalar(const GUI_EL_DESC &desc) {
      if(siGUIElements >= MAX_GUI_ELEMENTS) return 0x080000001;

      const GUI_SPRITE_LIB &curLib = spriteLib[desc.index.spriteLib];
            GUI_SPRITE     &panSpr = curLib.sprite[desc.index.panelSprite];
            GUI_SPRITE     &indSpr = curLib.sprite[desc.index.cursorSprite];

      cVEC2Df spriteSize[2] = { { fabsf(panSpr.tc.x2 - panSpr.tc.x1) * desc.size.panel.x, fabsf(panSpr.tc.y2 - panSpr.tc.y1) * desc.size.panel.y },
                                { fabsf(indSpr.tc.x2 - indSpr.tc.x1) * desc.size.cursor.x, fabsf(indSpr.tc.y2 - indSpr.tc.y1) * desc.size.cursor.y } };
      cVEC2Df activeSize[2] = { { spriteSize[0].x * panSpr.aa.x, spriteSize[0].y * panSpr.aa.y }, { spriteSize[1].x * indSpr.aa.x, spriteSize[1].y * indSpr.aa.y } };
      cVEC4Df viewCoords[2] = { { desc.viewPos.x - spriteSize[0].x, desc.viewPos.y + spriteSize[0].y, desc.viewPos.x + spriteSize[0].x, desc.viewPos.y - spriteSize[0].y },
                                { desc.viewPos.x - spriteSize[1].x, desc.viewPos.y + spriteSize[1].y, desc.viewPos.x + spriteSize[1].x, desc.viewPos.y - spriteSize[1].y } };

      cSSE4Df32 activeCoords[2] = { { CalcActiveCoords(desc.viewPos, spriteSize[0], activeSize[0], desc.panel.align) },
                                    { CalcActiveCoords(desc.viewPos, spriteSize[1], activeSize[1], desc.cursor.align) } };

      element[siGUIElements].activeCoords      = activeCoords[0];
      element[siGUIElements].activePos         = { (activeCoords[0].x1 + activeCoords[0].x2) * 0.5f, (activeCoords[0].y1 + activeCoords[0].y2) * 0.5f};
      element[siGUIElements].viewScale         = { 2.0f / activeSize[0].x, 2.0f / activeSize[0].y };
      element[siGUIElements].func.hover[0]     = desc.func.hover[0];
      element[siGUIElements].func.hover[1]     = desc.func.hover[1];
      element[siGUIElements].func.activate0[0] = (desc.func.activate0[0] ? desc.func.activate0[0] : __Activate0_0_Default_Scalar);
      element[siGUIElements].func.activate0[1] = (desc.func.activate0[1] ? desc.func.activate0[1] : __Activate0_1_Default_Scalar);
      element[siGUIElements].func.activate1[0] = (desc.func.activate1[0] ? desc.func.activate1[0] : __Activate1_0_Default_Scalar);
      element[siGUIElements].func.activate1[1] = (desc.func.activate1[1] ? desc.func.activate1[1] : __Activate1_1_Default_Scalar);
      element[siGUIElements].soundBankIndex    = desc.index.soundBank;
      element[siGUIElements].vertexCount[0]    = element[siGUIElements].vertexCount[1] = 2;
      element[siGUIElements].vertexIndex       = siGUIVerts;
      element[siGUIElements].elementType       = 4;
      element[siGUIElements].si32Var[0]        = 0;
      element[siGUIElements].stateBits         = 0x0C0;
      element_dgs[siGUIVerts].viewPos      = desc.viewPos;
      element_dgs[siGUIVerts].indent       = activeSize[0];
      element_dgs[siGUIVerts].texCoords    = panSpr.tc;
      element_dgs[siGUIVerts].colour       = desc.tint.panel.xmm;
      element_dgs[siGUIVerts].size         = spriteSize[0];
      element_dgs[siGUIVerts].rotAngle     = 0.0f;
      element_dgs[siGUIVerts].width        = 0.0f;
      element_dgs[siGUIVerts].parentIndex  = 0x0FFFFFFFF;
      element_dgs[siGUIVerts].atlasIndex   = (ui8)curLib.atlasIndex;
      element_dgs[siGUIVerts].elementType  = 1;
      element_dgs[siGUIVerts].sibling      = 0x08001;
      element_dgs[siGUIVerts].align        = desc.panel.align;
      element_dgs[siGUIVerts++].mods       = desc.panel.mods;
      element_dgs[siGUIVerts].viewPos      = { 0.0f, 0.0f }; // .x range -1.0~1.0
      element_dgs[siGUIVerts].indent       = activeSize[1];
      element_dgs[siGUIVerts].texCoords    = indSpr.tc;
      element_dgs[siGUIVerts].colour       = desc.tint.cursor.xmm;
      element_dgs[siGUIVerts].size         = spriteSize[1];
      element_dgs[siGUIVerts].rotAngle     = 0.0f;
      element_dgs[siGUIVerts].width        = 0.0f;
      element_dgs[siGUIVerts].parentIndex  = siGUIVerts - 1;
      element_dgs[siGUIVerts].atlasIndex   = (ui8)curLib.atlasIndex;
      element_dgs[siGUIVerts].elementType  = 4;
      element_dgs[siGUIVerts].sibling      = 1;
      element_dgs[siGUIVerts].align        = desc.cursor.align;
      element_dgs[siGUIVerts++].mods       = desc.cursor.mods;

      return siGUIElements++;
   }

   // Returns indices of panel element and text overlay
   cVEC2Du32 CreateTextPanel(GUI_EL_DESC &desc) {
      if(siGUIElements >= MAX_GUI_ELEMENTS - 1) return { 0x080000001, MAX_GUI_ELEMENTS };

      cui32 panel = CreatePanel(desc);

      funcptr tempPtrs[6] = { desc.function[0], desc.function[1], desc.function[2], desc.function[3], desc.function[4], desc.function[5] };
      cVEC2Df tempPos     = desc.viewPos;

      for(ui8 i = 0; i < 6; i++)
         desc.function[i] = 0;

      desc.viewPos = { 0.0f, 0.0f };
      cui32 overlay = CreateText(desc);

      for(ui8 i = 0; i < 6; i++)
         desc.function[i] = tempPtrs[i];

      desc.viewPos = tempPos;
      cui32 panelVert = element[panel].vertexIndex;
      cui32 firstVert = element[overlay].vertexIndex;

      for(ui32 i = 0; i < element[overlay].vertexCount[1]; i++)
         element_dgs[firstVert + i].parentIndex = panelVert;

      return { panel, overlay };
   }

   // Returns indices of panel element and text overlay
   cVEC2Du32 CreateTextButton(GUI_EL_DESC &desc) {
      if(siGUIElements >= MAX_GUI_ELEMENTS - 1) return { 0x080000001, MAX_GUI_ELEMENTS };

      cui32 panel = CreateButton(desc);

      ui32 i = 0;

      funcptr tempPtrs[6] = { desc.function[0], desc.function[1], desc.function[2], desc.function[3], desc.function[4], desc.function[5] };
      cVEC2Df tempPos     = desc.viewPos;

      for(i = 0; i < 6; i++) desc.function[i] = 0;

      desc.viewPos = { 0.0f, 0.0f };
      cui32 overlay = CreateText(desc);

      for(i = 0; i < 6; i++) desc.function[i] = tempPtrs[i];

      desc.viewPos = tempPos;
      cui32 panelVert = element[panel].vertexIndex;
      cui32 firstVert = element[overlay].vertexIndex;

      for(i = 0; i < element[overlay].vertexCount[1]; i++) element_dgs[firstVert + i].parentIndex = panelVert;

      return { panel, overlay };
   }

   // Returns indices of panel element, cursor element, and text overlay
   cVEC3Du32 CreateInputPanel(GUI_EL_DESC &desc) {
      if(siGUIElements >= MAX_GUI_ELEMENTS - 2) return { 0x080000001, MAX_GUI_ELEMENTS, 0 };

      cui32 panel = CreatePanel(desc);

      fl32 accum = 0.0f;
      ui32 i = 0;

      for(; desc.textPtr[i]; i++) accum += alphabet_pIMM[alphabet[desc.index.alphabet].pIMMos + desc.textPtr[i]].width;

      funcptr tempPtrs[6] = { desc.function[0], desc.function[1], desc.function[2], desc.function[3], desc.function[4], desc.function[5] };
      cVEC2Df tempPos = desc.viewPos;

      for(i = 0; i < 6; i++) desc.function[i] = 0;

      desc.viewPos = { 0.0f, 0.0f };
      cui32 cursor = CreateCursor(desc);
      cui32 overlay = CreateText(desc);

      for(i = 0; i < 6; i++) desc.function[i] = tempPtrs[i];
      desc.viewPos = tempPos;

      cui32 panelVert = element[panel].vertexIndex;
      cui32 firstVert = element[overlay].vertexIndex;

      element_dgs[element[cursor].vertexIndex].parentIndex = panelVert;
      element_dgs[element[cursor].vertexIndex].align |= 0x080;

      for(i = 0; i < element[overlay].vertexCount[1]; i++) element_dgs[firstVert + i].parentIndex = panelVert;

      element[panel].func.activate0[0] = __Activate0_0_Default_Input;
      element[panel].func.hover[0]     = __Passive_Default_Input;
      element[panel].func.hover[1]     = __Passive_Default_Input;

      element[cursor].stateBits = 0;

      return { panel, cursor, overlay };
   }

   inline void DisableElements(csi16 elementIndex, csi16 count) {
///- To do...
   }

   inline si16 DestroyElements(csi16 elementIndex, csi16 count) {
///- To do...

      return 0; // Return old index of relocated element
   }

   cui32 CreateInterface(cui16 maxVertices, ui16 maxInputs) {
      if(siInterfaces >= MAX_INTERFACES) return 0x080000001; // All interface slots occupied

      interfaceProfile[siInterfaces].vertex      = (ui16ptr)zalloc32(sizeof(ui16) * maxVertices);
      interfaceProfile[siInterfaces].vertCount   = 0;
      interfaceProfile[siInterfaces].maxVertices = maxVertices;
      interfaceProfile[siInterfaces].inputs      = (VEC4Du8 *)zalloc32(sizeof(VEC4Du8) * maxInputs);
      interfaceProfile[siInterfaces].inputLabel  = (char (*)[32])zalloc32(sizeof(char (*)[32]) * maxInputs);
      interfaceProfile[siInterfaces].inputCount  = 0;
      interfaceProfile[siInterfaces].maxInputs   = maxInputs;

      return siInterfaces++;
   }

   cui32 CreateInterface(cui16ptrc elementIndices, cui16 elementCount, ui16 maxVertices, ui16 maxInputs) {
      al4 ui16 i, j;

      if(siInterfaces >= MAX_INTERFACES) return 0x080000001; // All interface slots occupied

      for(i = 0, j = 0; i < elementCount; i++) j += element[elementIndices[i]].vertexCount[1];

      if(maxVertices < j) maxVertices = j;
      if(maxInputs > MAX_INTERFACE_INPUTS) maxVertices = MAX_INTERFACE_INPUTS;

      interfaceProfile[siInterfaces].vertex      = (ui16ptr)zalloc32(sizeof(ui16) * j);
      interfaceProfile[siInterfaces].vertCount   = j;
      interfaceProfile[siInterfaces].maxVertices = maxVertices;

      for(i = 0, j = 0; i < elementCount && j < MAX_INTERFACE_VERTS; i++) {
         cui16 elementIndex = elementIndices[i];

         if(elementIndex < 0x0FFFFFFFF) {
            csi32 vertexIndex = element[elementIndex].vertexIndex;
            cui16 runLength   = element[elementIndex].vertexCount[1];
            ui16  offset      = 0;

            do interfaceProfile[siInterfaces].vertex[j++] = vertexIndex + offset++;
            while(offset < runLength);
         }
      }

      interfaceProfile[siInterfaces].inputs     = (VEC4Du8 *)zalloc32(sizeof(VEC4Du8) * maxInputs);
      interfaceProfile[siInterfaces].inputLabel = (char (*)[32])zalloc32(sizeof(char (*)[32]) * maxInputs);
      interfaceProfile[siInterfaces].inputCount = 0;
      interfaceProfile[siInterfaces].maxInputs  = maxInputs;

      return siInterfaces++;
   }

   cui32 AddElementToInterface(cui32 elementIndex, csi16 interfaceIndex) const {
      if(elementIndex < 0x0FFFFFFFF) {
         csi32 vertexIndex = element[elementIndex].vertexIndex;
         cui16 runLength   = element[elementIndex].vertexCount[1];
         ui16  offset      = 0;

         GUI_INTERFACE &curInterface = interfaceProfile[interfaceIndex];

         while(offset < runLength && curInterface.vertCount < curInterface.maxVertices)
            curInterface.vertex[curInterface.vertCount++] = vertexIndex + offset++;
      }

      // Return remaining vertex indices
      return interfaceProfile[interfaceIndex].maxVertices - interfaceProfile[interfaceIndex].vertCount;
   }

   cui32 AddElementToInterface(cVEC2Du32 elementIndex, csi16 interfaceIndex) const {
      GUI_INTERFACE &curInterface = interfaceProfile[interfaceIndex];

      if(elementIndex.x < 0x0FFFFFFFF) {
         csi32 vertexIndex = element[elementIndex.x].vertexIndex;
         cui16 runLength   = element[elementIndex.x].vertexCount[1];
         ui16  offset      = 0;

         while(offset < runLength && curInterface.vertCount < curInterface.maxVertices)
            curInterface.vertex[curInterface.vertCount++] = vertexIndex + offset++;
      }
      if(elementIndex.y < 0x0FFFFFFFF) {
         csi32 vertexIndex = element[elementIndex.y].vertexIndex;
         cui16 runLength   = element[elementIndex.y].vertexCount[1];
         ui16  offset      = 0;

         while(offset < runLength && curInterface.vertCount < curInterface.maxVertices)
            curInterface.vertex[curInterface.vertCount++] = vertexIndex + offset++;
      }

      // Return remaining vertex indices
      return interfaceProfile[interfaceIndex].maxVertices - interfaceProfile[interfaceIndex].vertCount;
   }

   cui32 AddElementToInterface(cVEC3Du32 elementIndex, csi16 interfaceIndex) const {
      GUI_INTERFACE &curInterface = interfaceProfile[interfaceIndex];

      if(elementIndex.x < 0x0FFFFFFFF) {
         csi32 vertexIndex = element[elementIndex.x].vertexIndex;
         cui16 runLength   = element[elementIndex.x].vertexCount[1];
         ui16  offset      = 0;

         while(offset < runLength && curInterface.vertCount < curInterface.maxVertices)
            curInterface.vertex[curInterface.vertCount++] = vertexIndex + offset++;
      }
      if(elementIndex.y < 0x0FFFFFFFF) {
         csi32 vertexIndex = element[elementIndex.y].vertexIndex;
         cui16 runLength   = element[elementIndex.y].vertexCount[1];
         ui16  offset      = 0;

         while(offset < runLength && curInterface.vertCount < curInterface.maxVertices)
            curInterface.vertex[curInterface.vertCount++] = vertexIndex + offset++;
      }
      if(elementIndex.z < 0x0FFFFFFFF) {
         csi32 vertexIndex = element[elementIndex.z].vertexIndex;
         cui16 runLength   = element[elementIndex.z].vertexCount[1];
         ui16  offset      = 0;

         while(offset < runLength && curInterface.vertCount < curInterface.maxVertices)
            curInterface.vertex[curInterface.vertCount++] = vertexIndex + offset++;
      }

      // Return remaining vertex indices
      return interfaceProfile[interfaceIndex].maxVertices - interfaceProfile[interfaceIndex].vertCount;
   }

   cui32 ResizeInterfaceInputs(csi16 interfaceIndex, cVEC4Du8 *inputList, cui16 inputCount, ui16 maxInputs) {
///--- To do...

      return 0;
   }

   inline void UpdateText(cchptr text, ui32 elementIndex) {
      GUI_ELEMENT &curElement = element[elementIndex];

      csi32 vertexIndex   = curElement.vertexIndex;
      csi32 textBufferOS  = (element_dgs[vertexIndex].textArrayOS & 0x03FFFFFF) << 4;
      csi32 stringLength  = si32(strlen(text));
      csi32 textBlockSize = max(1, (stringLength + 31) >> 5);

      if(textBlockSize >= curElement.vertexCount[1]) {
         curElement.charCount      = min(stringLength, textBlockSize << 5);
         curElement.vertexCount[0] = curElement.vertexCount[1];
         const size_t truncBlock = (size_t(curElement.vertexCount[1]) << 5) - 1;
         strncpy(&textBuffer[textBufferOS], text, truncBlock);
         textBuffer[textBufferOS + truncBlock] = 0;
      } else {
         curElement.charCount      = stringLength;
         curElement.vertexCount[0] = textBlockSize;
         strncpy(&textBuffer[textBufferOS], text, stringLength);
         textBuffer[textBufferOS + stringLength] = 0;
         csi32 deltaIndex = (textBufferOS + stringLength) & 0x01F;
         if(deltaIndex && deltaIndex < 24) {
            csi32 textBlockEnd = textBufferOS + (textBlockSize << 5);
            textBuffer[textBlockEnd - 8] = 0;
            if(deltaIndex < 16) {
               textBuffer[textBlockEnd - 16] = 0;
               if(deltaIndex < 8)
                  textBuffer[textBlockEnd - 24] = 0;
            }
         }
      }
      for(ui8 i = 0; i < curElement.vertexCount[0]; i++) {
         csi32 offset = vertexIndex + i;
         csi32 step   = stringLength - (i << 5);

         element_dgs[offset].align     &= 0x07F;
         element_dgs[offset].textArrayOS = (element_dgs[offset].textArrayOS & 0x03FFFFFF) | (min(step, 32) << 26);
         element_dgs[offset].origin[0]   = _CalculateOrigin(vertexIndex, offset, element_dgs[offset].alphabetIndex);
         element_dgs[offset].origin[1]   = (step > 8 ? cVEC2Df{ 16384.0f, 16384.0f } : element_dgs[offset].origin[0] );
         element_dgs[offset].origin[2]   = (step > 16 ? cVEC2Df{ 16384.0f, 16384.0f } : element_dgs[offset].origin[1] );
         element_dgs[offset].origin[3]   = (step > 24 ? cVEC2Df{ 16384.0f, 16384.0f } : element_dgs[offset].origin[2] );
      }
      for(ui8 i = textBlockSize; i < curElement.vertexCount[1]; i++)
         element_dgs[vertexIndex + i].align |= 0x080;
   }

   inline void UpdateText(cchptr text, cVEC2Du32 elementIndex) {
      GUI_ELEMENT &curElement = element[elementIndex.y];

      csi32 vertexIndex   = curElement.vertexIndex;
      csi32 textBufferOS  = (element_dgs[vertexIndex].textArrayOS & 0x03FFFFFF) << 4;
      csi32 stringLength  = si32(strlen(text));
      csi32 textBlockSize = max(1, (stringLength + 31) >> 5);

      if(textBlockSize >= curElement.vertexCount[1]) {
         curElement.charCount      = textBlockSize << 5;
         curElement.vertexCount[0] = curElement.vertexCount[1];
         const size_t truncBlock = (size_t(curElement.vertexCount[1]) << 5) - 1;
         strncpy(&textBuffer[textBufferOS], text, truncBlock);
         textBuffer[textBufferOS + truncBlock] = 0;
      } else {
         curElement.charCount      = stringLength;
         curElement.vertexCount[0] = textBlockSize;
         strncpy(&textBuffer[textBufferOS], text, stringLength);
         textBuffer[textBufferOS + stringLength] = 0;
         csi32 deltaIndex = (textBufferOS + stringLength) & 0x01F;
         if(deltaIndex && deltaIndex < 24) {
            csi32 textBlockEnd = textBufferOS + (textBlockSize << 5);
            textBuffer[textBlockEnd - 8] = 0;
            if(deltaIndex < 16) {
               textBuffer[textBlockEnd - 16] = 0;
               if(deltaIndex < 8)
                  textBuffer[textBlockEnd - 24] = 0;
            }
         }
      }
      for(ui8 i = 0; i < curElement.vertexCount[0]; i++) {
         csi32 offset = vertexIndex + i;
         csi32 step   = stringLength - (i << 5);

         element_dgs[offset].align     &= 0x07F;
         element_dgs[offset].textArrayOS = (element_dgs[offset].textArrayOS & 0x03FFFFFF) | (min(step, 32) << 26);
         element_dgs[offset].origin[0]   = _CalculateOrigin(vertexIndex, offset, element_dgs[offset].alphabetIndex);
         element_dgs[offset].origin[1]   = (step > 8 ? cVEC2Df{ 16384.0f, 16384.0f } : element_dgs[offset].origin[0] );
         element_dgs[offset].origin[2]   = (step > 16 ? cVEC2Df{ 16384.0f, 16384.0f } : element_dgs[offset].origin[1] );
         element_dgs[offset].origin[3]   = (step > 24 ? cVEC2Df{ 16384.0f, 16384.0f } : element_dgs[offset].origin[2] );
      }
      for(ui8 i = textBlockSize; i < curElement.vertexCount[1]; i++)
         element_dgs[vertexIndex + i].align |= 0x080;
   }

   void UploadAlphabetBuffer(cui8 context) {
      Try(stMap, devcon[context]->Map(buf->pBuffer[3][bufAlphabet], 0, D3D11_MAP_WRITE_DISCARD, 0, &ms));
      memcpy(ms.pData, alphabet_pIMM, sizeof(CHAR_IMM) * uiLanguages * MAX_CHARACTERS);
      devcon[context]->Unmap(buf->pBuffer[3][bufAlphabet], 0);
   }

///--- Modify for WRITE_NO_OVERWRITE and add mod test ---///
   void UploadTextBuffer(cui8 context) {
      Try(stMap, devcon[context]->Map(buf->pBuffer[3][bufText], 0, D3D11_MAP_WRITE_DISCARD, 0, &ms));
      memcpy(ms.pData, textBuffer, siTextBufferOS);
      devcon[context]->Unmap(buf->pBuffer[3][bufText], 0);
   }

///--- Modify for WRITE_NO_OVERWRITE and add mod test ---///
   inline void UploadEntryBuffer(cui8 context) {
      devcon[context]->UpdateSubresource(buf->pBuffer[3][bufDGS], 0, NULL, element_dgs, sizeof(GUI_EL_DGS), sizeof(GUI_EL_DGS));
   }

   void UploadVertexBuffer(cui8 context, csi16 interfaceIndex) {
      Try(stMap, devcon[context]->Map(buf->pBuffer[0][bufVertex], 0, D3D11_MAP_WRITE_DISCARD, 0, &ms));
      memcpy(ms.pData, interfaceProfile[interfaceIndex].vertex, sizeof(ui16) * interfaceProfile[interfaceIndex].vertCount);
      devcon[context]->Unmap(buf->pBuffer[0][bufVertex], 0);
   }

   csi16 SetResources(cui8 context, cui8 startIndex) const {
      cui32 c = uiAtlas;

      buf->SetUAV(0, bufDGS, 1, 1);
      buf->SetVSR(0, bufAlphabet, startIndex, 1);
      buf->SetVSR(0, bufText, startIndex + 1, 1);
      buf->SetGSR(0, bufAlphabet, startIndex, 1);
      buf->SetGSR(0, bufText, startIndex + 1, 1);
      for(ui8 i = 0; i < c; i++)
         tex->SetPS2D(0, siAtlasTexIndex[i], i * 2 + 80, 1);

      cfg->SetShaderGroup(context, 4);
      cfg->SetVertexFormat(context, 2);
      buf->SetVertex(context, bufDGS, 0);
      devcon[0]->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

      return startIndex + 2; // Next available index
   }

   inline void DrawElements(void) const {
      devcon[0]->Draw(siGUIVerts, 0);
      sysData.gpu.total.drawCalls++;
   }

   // Use the value in the globally accessible MAP_DESC object
   inline void DrawInterface(cui8 context) {
      UploadVertexBuffer(context, (*(const GUI_DESC *)ptrLib[15]).interfaceIndex);
      buf->SetVertex(context, bufVertex, 0);
      devcon[0]->Draw(interfaceProfile[(*(const GUI_DESC *)ptrLib[15]).interfaceIndex].vertCount, 0);
      sysData.gpu.total.drawCalls++;
   }

   inline void DrawInterface(cui8 context, csi16 interfaceIndex) {
      UploadVertexBuffer(context, interfaceIndex);
      buf->SetVertex(context, bufVertex, 0);
      devcon[0]->Draw(interfaceProfile[interfaceIndex].vertCount, 0);
      sysData.gpu.total.drawCalls++;
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
               element[index].func.hover[0](indices[j].data);
            // onActivate for button 0
            if(ctrlVars.imm.b[16] & 0x01 && element[index].function[2]) {
               activeElement.xmm0.m128i_i32[0] = index;
               element[index].func.activate0[0](indices[j].data);
            }
            // onActivate for button 1
            if(ctrlVars.imm.b[16] & 0x02 && element[index].function[4]) {
               activeElement.xmm0.m128i_i32[1] = index;
               element[index].func.activate1[0](indices[j].data);
            }
         } else
            // offHover
            if(!(element[index].stateBits & 0x02) && element[index].function[1])
               element[index].func.hover[1](indices[j].data);
         // offActivate for button 0
         if(!(ctrlVars.imm.b[16] & 0x01) && element[index].function[3]) {
            activeElement.xmm0.m128i_i32[0] = -1;
            element[index].func.activate0[1](indices[j].data);
         }
         // offActivate for button 1
         if(!(ctrlVars.imm.b[16] & 0x02) && element[index].function[5]) {
            activeElement.xmm0.m128i_i32[1] = -1;
            element[index].func.activate1[1](indices[j].data);
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
         adjPos.y *= 0.5f;
         adjPos.x += 0.5f;
         adjPos.y += 0.5f;

         if(adjPos.x > coords.x1 && adjPos.y > coords.y1 && adjPos.x < coords.x2 && adjPos.y < coords.y2) {
            activeElement.ymm.m256i_i32[0] = index;

            // onHover
            if(!(element[index].stateBits & 0x01) && element[index].function[0])
               element[index].func.hover[0](indices[j].data);

            // onActivate for inputMask[0]
            if(ipd.input.ui >= 1 && !AllFalse(ctrlVars.imm.button, ipd.inputMask[0])) {
               activeElement.ymm.m256i_i32[1] = index;
               if(element[index].function[2]) element[index].func.activate0[0](indices[j].data);
               for(funcCount = 0; funcCount < ipd.funcCount[0].x; funcCount++, nextFuncIndex++)
                  ipd.function[nextFuncIndex](indices[j].data);
            }

            // onActivate for inputMask[1]
            if(ipd.input.ui >= 2 && !AllFalse(ctrlVars.imm.button, ipd.inputMask[1])) {
               activeElement.ymm.m256i_i32[2] = index;
               if(element[index].function[4]) element[index].func.activate1[0](indices[j].data);
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
               element[index].func.hover[1](indices[j].data);

         // offActivate for inputMask[0]
         if(ipd.input.ui >= 1 && AllFalse(ctrlVars.imm.button, ipd.inputMask[0])) {
            activeElement.ymm.m256i_i32[0] = -1;
            if(element[index].function[3]) element[index].func.activate0[1](indices[j].data);
            for(funcCount = 0; funcCount < ipd.funcCount[0].y; funcCount++, nextFuncIndex++)
               ipd.function[nextFuncIndex](indices[j].data);
         }

         // offActivate for inputMask[1]
         if(ipd.input.ui >= 2 && AllFalse(ctrlVars.imm.button, ipd.inputMask[1])) {
            activeElement.ymm.m256i_i32[1] = -1;
            if(element[index].function[5]) element[index].func.activate1[1](indices[j].data);
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
