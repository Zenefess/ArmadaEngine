/************************************************************
 * File: File operations.h              Created: 2022/11/06 *
 *                                Last modified: 2024/07/10 *
 *                                                          *
 * Notes: 2024/05/06: Added support for data tracking       *
 *                                                          *
 * To do: Add WriteLine functions                           *
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

extern inline void Try(cchptrc stEvent, cui32 uiResult, cAE_SUBSYSTEM subsystem);

enum ENUM_FILE_POS : ui8 { file_begin, file_current, file_end };

al32 struct CLASS_FILEOPS {
   cchar   stLoadShaders[32] = "Invalid shader list.cfg file";
   wchptrc pathWorking       = (wchptr)malloc64(sizeof(wchar[512]));
   union {
      chptrc  stTemp;
      wchptrc wstTemp = (wchptr)malloc64(sizeof(wchar[4096]));
   };
   ui32  uiBytes        = 0;
   DWORD bytesProcessed = 0;

   CLASS_FILEOPS(void) {}
   CLASS_FILEOPS(cptrptrc globalPointer) { if(globalPointer) *globalPointer = this; }

   inline cHANDLE OpenForReading(cwchptrc filename, cwchptrc folder) const {
      csize_t folderLength = wcslen(folder);

      ///--- Rewrite to use wsprintf
      wcscpy(wstTemp, folder);
      wcscpy(wstTemp + folderLength, L"\\");
      wcscpy(wstTemp + folderLength + 1, filename);
      cHANDLE hFile = CreateFile(wstTemp, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, 0);
      if(!hFile) return hFile;
#ifdef DATA_TRACKING
      ++sysData.storage.filesOpened;
#endif
      return hFile;
   }

   inline cHANDLE OpenForWriting(cwchptrc filename, cwchptrc folder) const {
      csize_t folderLength = wcslen(folder);

      ///--- Rewrite to use wsprintf
      wcscpy(wstTemp, folder);
      wcscpy(wstTemp + folderLength, L"\\");
      wcscpy(wstTemp + folderLength + 1, filename);
      cHANDLE hFile = CreateFile(wstTemp, GENERIC_WRITE, FILE_SHARE_WRITE, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
      if(!hFile) return hFile;
#ifdef DATA_TRACKING
      ++sysData.storage.filesOpened;
#endif
      return hFile;
   }

   inline cbool CloseFile(cHANDLE file) const {
      return CloseHandle(file);
#ifdef DATA_TRACKING
      ++sysData.storage.filesClosed;
#endif
   }

   // Returns current position in file
   inline cui32 GetPosition(cHANDLE file) const { return SetFilePointer(file, 0, 0, file_current); }

   // Sets current position in file
   inline cui32 SetPosition(cHANDLE file, cui32 offset, ENUM_FILE_POS startPos) const {
      return SetFilePointer(file, offset, 0, startPos);
   }

   inline cui32 Read(cHANDLE file, ptrc dest, cui32 bytesToRead) {
      ReadFile(file, dest, bytesToRead, &bytesProcessed, 0);
#ifdef DATA_TRACKING
      sysData.storage.bytesRead += bytesProcessed;
#endif
      return bytesProcessed;
   }

   inline cui32 Write(cHANDLE file, cptrc data, cui32 bytesToWrite) {
      WriteFile(file, data, bytesToWrite, &bytesProcessed, 0);
#ifdef DATA_TRACKING
      sysData.storage.bytesWritten += bytesProcessed;
#endif
      return bytesProcessed;
   }

   // Returns number of bytes read
   inline cui32 ReadLine(cHANDLE file, chptrc stDest) const {
      static char chCurrent = 0;

      ui32 xx = 0;

      for(xx = 0; xx < 255u; xx++) {
         cbool bOK = ReadFile(file, &chCurrent, 1, (LPDWORD)&uiBytes, 0);
         if(!bOK || !uiBytes || !chCurrent) break;
         if(chCurrent == '/n')
            if(xx < 2u) {
               stDest[0] = 1; stDest[1] = 0; return 1;
            } else {
               if(stDest[xx - 1] == '\r') {
                  stDest[xx - 1] = 0;
                  return 1u;
               }
            }
         else stDest[xx] = chCurrent;
      }
      stDest[xx] = 0;
#ifdef DATA_TRACKING
      sysData.storage.bytesRead += xx;
#endif
      return xx;
   }

   // Returns number of bytes read
   inline cui32 ReadLine(cHANDLE file, wchptrc stDest) const {
      static wchar chCurrent = 0;

      ui32 xx = 0;

      for(xx = 0; xx < 255u; xx++) {
         cbool bOK = ReadFile(file, &chCurrent, 1, (LPDWORD)&uiBytes, 0);
         if(!bOK || !uiBytes || !chCurrent) {
            break;
         }
         switch(chCurrent) {
         case L'\n':
            if(xx < 2u) {
               stDest[0] = 1; stDest[1] = 0; return 1;
            } else
               if(stDest[xx - 1] == L'\r') {
                  stDest[xx - 1] = 0;
                  return 1;
               }
         default:
            stDest[xx] = chCurrent;
         }
      }
      stDest[xx] = 0;
#ifdef DATA_TRACKING
      sysData.storage.bytesRead += xx;
#endif
      return xx;
   }

   // Returns number of bytes read
   inline cui32 ReadWideLine(cHANDLE file, chptrc stDest) const {
      static char chCurrent[2] = { 0, 0 };

      ui32 xx = 0;

      for(xx = 0; xx < 255u; xx++) {
         cbool bOK = ReadFile(file, &chCurrent, 2, (LPDWORD)&uiBytes, 0);
         if(!bOK || !uiBytes || !chCurrent) {
            break;
         }
         switch(chCurrent[0]) {
         case '\n':
            if(xx < 2u) {
               stDest[0] = 1; stDest[1] = 0; return 1;
            } else
               if(stDest[xx - 1] == '\r') {
                  stDest[xx - 1] = 0;
                  return 1u;
               }
         default:
            stDest[xx] = chCurrent[0];
         }
      }
      stDest[xx] = 0;
#ifdef DATA_TRACKING
      sysData.storage.bytesRead += xx;
#endif
      return xx;
   }

   // Returns number of bytes read
   inline cui32 ReadWideLine(cHANDLE file, wchptrc stDest) const {
      static wchar chCurrent = 0;

      ui32 xx = 0;

      for(xx = 0; xx < 255u; xx++) {
         cbool bOK = ReadFile(file, &chCurrent, 2, (LPDWORD)&uiBytes, 0);
         if(!bOK || !uiBytes || !chCurrent) {
            break;
         }
         switch(chCurrent) {
         case L'\n':
            if(xx < 2u) {
               stDest[0] = 1; stDest[1] = 0; return 1;
            } else
               if(stDest[xx - 1] == L'\r') {
                  stDest[xx - 1] = 0;
                  return 1;
               }
         default:
            stDest[xx] = chCurrent;
         }
      }
      stDest[xx] = 0;
#ifdef DATA_TRACKING
      sysData.storage.bytesRead += xx;
#endif
      return xx;
   }

   // Returns true if identical
   inline cbool ReadAndVerify(cHANDLE file, cchptrc string) const {
      ReadLine(file, stTemp);
      return (strncmp(stTemp, string, strlen(string)) ? false : true);
   }

   // Returns true if identical
   inline cbool ReadAndVerify(cHANDLE file, cwchptrc string) const {
      ReadWideLine(file, wstTemp);
      return (wcsncmp(wstTemp, string, wcslen(string)) ? false : true);
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
      uiBytes = ReadLine(hShaderGroups, wstTemp);
#ifdef DATA_TRACKING
      sysData.storage.bytesRead += uiBytes;
#endif
      if(wcsncmp(wstTemp, L"Stages: ", 8)) Try(stLoadShaders, -1, ss_video);
      // Read line
      while(uiBytes = ReadLine(hShaderGroups, wstTemp)) {
#ifdef DATA_TRACKING
         sysData.storage.bytesRead += uiBytes;
#endif
         if(wstTemp[0] >= '0' && wstTemp[0] <= '9') {
            ui8 uiBank = ui8(wstTemp[0] - '0');
            ui8 xx = 1;
            if(wstTemp[1] >= '0' && wstTemp[1] <= '9') {
               uiBank *= 10;
               uiBank += ui8(wstTemp[1] - '0');
               xx++;
            }
            for(ui8 uiType = 0; xx < 255; xx++) {
               switch(wstTemp[xx]) {
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
               if(wstTemp[xx] >= '0' && wstTemp[xx] <= '9') {
                  uiIndex[uiType][uiBank] = wstTemp[xx] - '0';
                  xx++; if(xx >= 255) break;
                  if(wstTemp[xx] >= '0' && wstTemp[xx] <= '9') {
                     uiIndex[uiType][uiBank] *= 10;
                     uiIndex[uiType][uiBank] = wstTemp[xx] - '0';
                  } else {
                     if(wstTemp[xx] == ' ') continue;
                     if(wstTemp[xx] == '\t') continue;
                     if(wstTemp[xx] == '/') break;
                     if(wstTemp[xx] == NULL) break;
                     xx--;
                  }
               } else {
                  if(wstTemp[xx] == ' ') continue;
                  if(wstTemp[xx] == '\t') continue;
                  if(wstTemp[xx] == '/') break;
                  if(wstTemp[xx] == NULL) break;
                  xx--;
               }
            }
         } else
            switch(wstTemp[0]) {
            case 'v':
            case 'V':
               wcscpy(stShader[1][uiBanks[1]], stShadersDir);
               wcscat(stShader[1][uiBanks[1]], wstTemp);
               wcscat(stShader[1][uiBanks[1]], stExtension);
               uiShaders++;
               uiBanks[1]++;
               break;
            case 'g':
            case 'G':
               wcscpy(stShader[2][uiBanks[2]], stShadersDir);
               wcscat(stShader[2][uiBanks[2]], wstTemp);
               wcscat(stShader[2][uiBanks[2]], stExtension);
               uiShaders++;
               uiBanks[2]++;
               break;
            case 'p':
            case 'P':
               wcscpy(stShader[5][uiBanks[5]], stShadersDir);
               wcscat(stShader[5][uiBanks[5]], wstTemp);
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
      mfree(wstTemp, pathWorking);
   }
};
