/************************************************************
 * File: D3D11 file operations.h        Created: 2022/11/06 *
 *                                Last modified: 2023/03/24 *
 *                                                          *
 * Desc:                                                    *
 *                                                          *
 *                         Copyright (c) David William Bull *
 ************************************************************/
#pragma once
#pragma warning(disable : 4996)

#include "pch.h"
#include <stdio.h>

al16 struct CLASS_FILEOPS {
   al16 wchptr  pathWorking = (wchar *)_aligned_malloc(sizeof(wchar[512]), 16);
        wchptr  stTemp = (wchar *)_aligned_malloc(sizeof(wchar[4096]), 16);
        wchar (*stShader)[100][256] = NULL;
        ui32    uiShaders = 0, uiBanks[6] {}, uiBytes = 0;
        ui8     uiIndex[6][100] = {};
   al16 LPDWORD pBytes = (LPDWORD)&uiBytes;

   inline csi32 ReadLine(wchar *stDest, HANDLE handle) const {
      static wchar chCurrent = NULL;
             ui32  xx = 0;

      for(xx = 0; xx < 255; xx++) {
         bool bOK = ReadFile(handle, &chCurrent, 1, pBytes, NULL);
         if(!bOK || !uiBytes) {
            stDest[xx] = NULL;
            return 0;
         }
         switch(chCurrent) {
         case '\n':
            if(xx < 2) {
               stDest[0] = 1; stDest[1] = 0; return 1;
            } else
               if(stDest[xx - 1] == '\r') {
                  stDest[xx - 1] = NULL;
                  return 1;
               }
         default:
            stDest[xx] = chCurrent;
         }
      }
      stDest[xx] = NULL;
      return 1;
   }

   csi32 LoadShaderList(void) {
      al16 cwchar stShadersDir[] = L"shaders\\";
           cwchar stExtension[]  = L".cso";

      if(!stShader)
         stShader = (wchar(*)[100][256])malloc16(sizeof(wchar[6][100][256]));

      HANDLE hShaderGroups = CreateFile(L"config\\Shader list.cfg", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);

      // Read (and discard) tag line
      ReadLine(stTemp, hShaderGroups);
      if(wcsncmp(stTemp, L"Stages: ", 8)) Try(stLoadShaders, -1);
      // Read line
      while(ReadLine(stTemp, hShaderGroups))
         if(stTemp[0] >= '0' && stTemp[0] <= '9') {
            ui8 uiBank = ui8(stTemp[0] - '0');
            ui8 xx = 1;
            if(stTemp[1] >= '0' && stTemp[1] <= '9') {
               uiBank *= 10;
               uiBank += ui8(stTemp[1] - '0');
               xx++;
            }
            for(ui8 uiType = 0; xx < 255; xx++) {
               switch(stTemp[xx]) {
               case 'c':   // Compute shader
               case 'C':
                  uiType = 0;
                  break;
               case 'v':   // Vertex shader
               case 'V':
                  uiType = 1;
                  break;
               case 'g':   // Geometry shader
               case 'G':
                  uiType = 2;
                  break;
               case 'd':   // Domain shader
               case 'D':
                  uiType = 3;
                  break;
               case 'm':   // Mesh shader
               case 'M':
                  uiType = 4;
                  break;
               case 'p':   // Pixel shader
               case 'P':
                  uiType = 5;
                  break;
               case 0:
                  xx = 254;
               }
               xx++; if(xx >= 255) break;
               if(stTemp[xx] >= '0' && stTemp[xx] <= '9') {
                  uiIndex[uiType][uiBank] = stTemp[xx] - '0';
                  xx++; if(xx >= 255) break;
                  if(stTemp[xx] >= '0' && stTemp[xx] <= '9') {
                     uiIndex[uiType][uiBank] *= 10;
                     uiIndex[uiType][uiBank] = stTemp[xx] - '0';
                  } else {
                     if(stTemp[xx] == ' ') continue;
                     if(stTemp[xx] == '\t') continue;
                     if(stTemp[xx] == '/') break;
                     if(stTemp[xx] == NULL) break;
                     xx--;
                  }
               } else {
                  if(stTemp[xx] == ' ') continue;
                  if(stTemp[xx] == '\t') continue;
                  if(stTemp[xx] == '/') break;
                  if(stTemp[xx] == NULL) break;
                  xx--;
               }
            }
         } else
            switch(stTemp[0]) {
            case 'v':
            case 'V':
               wcscpy(stShader[1][uiBanks[1]], stShadersDir);
               wcscat(stShader[1][uiBanks[1]], stTemp);
               wcscat(stShader[1][uiBanks[1]], stExtension);
               uiShaders++;
               uiBanks[1]++;
               break;
            case 'g':
            case 'G':
               wcscpy(stShader[2][uiBanks[2]], stShadersDir);
               wcscat(stShader[2][uiBanks[2]], stTemp);
               wcscat(stShader[2][uiBanks[2]], stExtension);
               uiShaders++;
               uiBanks[2]++;
               break;
            case 'p':
            case 'P':
               wcscpy(stShader[5][uiBanks[5]], stShadersDir);
               wcscat(stShader[5][uiBanks[5]], stTemp);
               wcscat(stShader[5][uiBanks[5]], stExtension);
               uiShaders++;
               uiBanks[5]++;
               break;
            case 1:
            case '/':
            case '\r':
            case '\n':
               break;
            default:
               return uiShaders;
               //   Try(VIDEO_THREAD_ERROR);
            }

      CloseHandle(hShaderGroups);

      return uiShaders;
   }

   void _Deinitialise(void) {
      if(stShader) mfree(stShader);
      mfree(stTemp);
      mfree(pathWorking);
   }
};
