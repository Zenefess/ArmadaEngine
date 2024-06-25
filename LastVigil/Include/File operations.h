/************************************************************
 * File: File operations.h              Created: 2022/11/06 *
 *                                Last modified: 2024/06/21 *
 *                                                          *
 * Notes: 2024/05/06: Added support for data tracking       *
 *                                                          *
 *                         Copyright (c) David William Bull *
 ************************************************************/
#pragma once
#pragma warning(disable : 4996)

#ifdef DATA_TRACKING
#include "data tracking.h"
#endif

#include <stdio.h>
#include "typedefs.h"
#include "class_timers.h"

extern vui64       THREAD_LIFE;
extern HANDLE      hErrorOutput;
extern CLASS_TIMER mainTimer;

enum AE_SUBSYSTEM : ui8 { ss_video, ss_audio, ss_input, ss_gui };

typedef const AE_SUBSYSTEM cAE_SUBSYSTEM;

extern inline void Try(cchptrc stEvent, cui32 uiResult, cAE_SUBSYSTEM subsystem);

al32 struct CLASS_FILEOPS {
   cchar   stLoadShaders[32] = "Invalid shader list.cfg file";
   wchptrc pathWorking       = (wchptr)malloc16(sizeof(wchar[512]));
   wchptrc stTemp            = (wchptr)malloc16(sizeof(wchar[4096]));
   ui32    uiBytes           = 0;
   DWORD   bytesProcessed    = 0;

   inline cHANDLE OpenFileForReading(cwchptrc filename, cwchptrc folder) const {
      csize_t folderLength = wcslen(folder);

      wcscpy(stTemp, folder);
      wcscpy(stTemp + folderLength, L"\\");
      wcscpy(stTemp + folderLength + 1, filename);
      cHANDLE hFile = CreateFile(stTemp, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, 0);
      if(!hFile) return hFile;
#ifdef DATA_TRACKING
      sysData.storage.filesOpened++;
#endif
      return hFile;
   }

   inline cHANDLE OpenFileForWriting(cwchptrc filename, cwchptrc folder) const {
      csize_t folderLength = wcslen(folder);

      wcscpy(stTemp, folder);
      wcscpy(stTemp + folderLength, L"\\");
      wcscpy(stTemp + folderLength + 1, filename);
      cHANDLE hFile = CreateFile(stTemp, GENERIC_WRITE, FILE_SHARE_WRITE, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
      if(!hFile) return hFile;
#ifdef DATA_TRACKING
      sysData.storage.filesOpened++;
#endif
      return hFile;
   }

   inline cui32 ReadFromFile(cHANDLE file, ptrc dest, cui32 bytesToRead) {
      ReadFile(file, dest, bytesToRead, &bytesProcessed, 0);
#ifdef DATA_TRACKING
      sysData.storage.bytesRead += bytesProcessed;
#endif
      return bytesProcessed;
   }

   inline cui32 WriteToFile(cHANDLE file, cptrc data, cui32 bytesToWrite) {
      WriteFile(file, data, bytesToWrite, &bytesProcessed, 0);
#ifdef DATA_TRACKING
      sysData.storage.bytesWritten += bytesProcessed;
#endif
      return bytesProcessed;
   }

   // Returns number of bytes read
   inline cui32 ReadLine(cHANDLE file, wchar *stDest) const {
      static wchar chCurrent = NULL;

      ui32 xx = 0;

      for(xx = 0; xx < 255; xx++) {
         cbool bOK = ReadFile(file, &chCurrent, 1, (LPDWORD)&uiBytes, NULL);
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
#ifdef DATA_TRACKING
      sysData.storage.bytesRead += uiBytes;
#endif
      return uiBytes;
   }

   // Returns true if identical
   inline cbool ReadAndVerify(cHANDLE file, cwchptrc string) const {
      ReadLine(file, stTemp);
      return (wcsncmp(stTemp, string, wcslen(string)) ? true : false);
   }

   inline cbool CloseFile(cHANDLE file) const {
      return CloseHandle(file);
#ifdef DATA_TRACKING
      sysData.storage.filesClosed++;
#endif
   }

   // Returns number of shaders loaded. 0x080000001 if shader list file does not exist
   cui32 LoadShaderList(ui8 (*const uiIndex)[CFG_MAX_SHADERS], wchar(*const stShader)[CFG_MAX_SHADERS][256], ui32ptrc uiBanks, ui32 &uiShaders) {
      cwchar  stShadersDir[] = L"shaders\\";
      cwchar  stExtension[]  = L".cso";
      cHANDLE hShaderGroups  = CreateFile(L"config\\Shader list.cfg", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);

      if(!hShaderGroups) return 0x080000001;
#ifdef DATA_TRACKING
      sysData.storage.filesOpened++;
#endif

      // Read (and discard) tag line
      uiBytes = ReadLine(hShaderGroups, stTemp);
#ifdef DATA_TRACKING
      sysData.storage.bytesRead += uiBytes;
#endif
      if(wcsncmp(stTemp, L"Stages: ", 8)) Try(stLoadShaders, -1, ss_video);
      // Read line
      while(uiBytes = ReadLine(hShaderGroups, stTemp)) {
#ifdef DATA_TRACKING
         sysData.storage.bytesRead += uiBytes;
#endif
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
      }
      CloseHandle(hShaderGroups);

#ifdef DATA_TRACKING
      sysData.storage.filesClosed++;
#endif
      return uiShaders;
   }

   void _Deinitialise(void) const {
      mfree(stTemp, pathWorking);
   }
};
