/************************************************************
 * File: LastVigil.cpp                  Created: 2022/10/08 *
 *                           Code last modified: 2023/04/26 *
 *                                                          *
 * Desc: Initial setup, and debug management.               *
 *                                                          *
 *  Copyright (c) David William Bull. All rights reserved.  *
 ************************************************************/
#include "pch.h"
#include "data tracking.h"
#include <stdio.h>
#include "thread flags.h"
#include "typedefs.h"
#include "Data structures.h"
#include "class_timers.h"
#include "Command queue.h"

#ifdef _DEBUG
#define NDEBUG 0
#include "comdef.h"
#include "D3D11SDKLayers.h"
#endif

extern void WorldGenThread(ptr);
extern void Direct3D11Thread(ptr);
extern void OpenAL1_1Thread(ptr);
extern void DirectInput8Thread(ptr);

extern HWND hWnd; // Defined in "Direct3D11 thread.cpp"

#ifdef DATA_TRACKING
SYSTEM_DATA sysData(1024);
#endif

//     COMMAND_MANAGER cmd;
al32 CLASS_TIMER  mainTimer;
     cptr         ptrLib[16];
     wchar        stErrorFilename[64];
 vol GLOBALCOORDS gco {};
     vui64        THREAD_LIFE = 0x0;   // 'Thread active' flags
     wchptr       stThrdStat;          // Debug output
     HINSTANCE    hInst;               // Current instance's handle
     HANDLE       hErrorOutput;        // File handle for error output
     HRESULT      hr;

/*/
 * _tWinMain
 *
 *   Application's main entry point.
/*/
int __cdecl wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPTSTR lpCmdLine, _In_ int nCmdShow) {
   al16 HANDLE hThread[5] {};
        ui64   threadLife;

   hPrevInstance;
   hInst = hInstance;   // Store instance handle globally

   // Initialise timer
   mainTimer.Init();

   FILETIME currentTime;
   GetSystemTimePreciseAsFileTime(&currentTime);
   wsprintfW(stErrorFilename, L"_error logs_/error log %08lX.txt", currentTime.dwLowDateTime);
   hErrorOutput = CreateFile(stErrorFilename, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

   // Prevent thread from shutting down (after engine reset)
   THREAD_LIFE &= ~MAIN_THREAD_DIED;

   THREAD_LIFE |= MAIN_THREAD_ALIVE;

   hThread[0] = GetCurrentThread();
   SetThreadIdealProcessor(hThread[0], 4);
   SetThreadPriority(hThread[0], 0);

   // Begin video rendering thread
   THREAD_LIFE |= VIDEO_THREAD_ALIVE;
   hThread[1] = (HANDLE)_beginthread(Direct3D11Thread, 0, NULL);
   Sleep(250);
   SetThreadIdealProcessor(hThread[1], 0);
   SetThreadPriority(hThread[1], 1);

   // Begin audio rendering thread
   THREAD_LIFE |= AUDIO_THREAD_ALIVE;
   hThread[2] = (HANDLE)_beginthread(OpenAL1_1Thread, 0, NULL);
   Sleep(250);
   SetThreadIdealProcessor(hThread[2], 2);
   SetThreadPriority(hThread[2], 1);

   // Begin input processing thread
   THREAD_LIFE |= INPUT_THREAD_ALIVE;
   hThread[3] = (HANDLE)_beginthread(DirectInput8Thread, 0, NULL);
   Sleep(250);
   SetThreadIdealProcessor(hThread[3], 1);
   SetThreadPriority(hThread[3], 2);

   // Begin world generation thread
   THREAD_LIFE |= GEN_THREAD_ALIVE;
   hThread[4] = (HANDLE)_beginthread(WorldGenThread, 0, NULL);
   Sleep(250);
   SetThreadIdealProcessor(hThread[4], 3);
   SetThreadPriority(hThread[4], 0);

   // Primary application loop
   do {
      threadLife = THREAD_LIFE;

//      mainTimer.Update();

      Sleep(8);
   } while (threadLife & MAIN_THREAD_ALIVE);

   // Request input thread shutdown
   THREAD_LIFE &= ~INPUT_THREAD_ALIVE;
   do {
      threadLife = THREAD_LIFE & INPUT_THREAD;
      Sleep(8);
   } while (!(threadLife & INPUT_THREAD_DIED));

   // Request audio thread shutdown
   THREAD_LIFE &= ~AUDIO_THREAD_ALIVE;
   do {
      threadLife = THREAD_LIFE & AUDIO_THREAD;
      Sleep(8);
   } while (!(threadLife & AUDIO_THREAD_DIED));

   // Request video thread shutdown
   THREAD_LIFE &= ~VIDEO_THREAD_ALIVE;
   do {
      threadLife = THREAD_LIFE & VIDEO_THREAD;
      Sleep(8);
   } while (!(threadLife & VIDEO_THREAD_DIED));

   // Request world generation thread shutdown
   THREAD_LIFE &= ~GEN_THREAD_ALIVE;
   do {
      threadLife = THREAD_LIFE & GEN_THREADS;
      Sleep(8);
   } while (!(threadLife & GEN_THREAD_DIED));

   threadLife = GetFileSize(hErrorOutput, NULL);
   CloseHandle(hErrorOutput);
   if(!threadLife) DeleteFile(stErrorFilename);

   if(THREAD_LIFE & MAIN_THREAD_DIED) return -1;
   else return 0;
}

void WriteError(cchptr const text, cbool fatal) {
   static char  stTimeStamp[32] {};
   static DWORD dwBytes = 0;

   cui32 uiTimeSecs = cui32(mainTimer.siStartTics / mainTimer.siFrequency);
   sprintf(stTimeStamp, "%08lX : ", uiTimeSecs);
   WriteFile(hErrorOutput, stTimeStamp, DWORD(strlen(stTimeStamp)), &dwBytes, NULL);
   WriteFile(hErrorOutput, text, DWORD(strlen(text)), &dwBytes, NULL);
   if(fatal) {
      WriteFile(hErrorOutput, "   *** FATAL ERROR ***\n", 24, &dwBytes, NULL);
      THREAD_LIFE |= MAIN_THREAD_DIED;
      Sleep(1000);
   } else
      WriteFile(hErrorOutput, "\n", 1, &dwBytes, NULL);
}
