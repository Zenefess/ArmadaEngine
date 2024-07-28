/************************************************************
 * File: LastVigil.cpp                  Created: 2022/10/08 *
 *                           Code last modified: 2024/07/22 *
 *                                                          *
 * Desc: Initial setup, and debug management.               *
 *                                                          *
 * Copyright (c) David William Bull.   All rights reserved. *
 ************************************************************/
#include "master header.h"
#include <stdio.h>
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

#ifdef DATA_TRACKING
SYSTEM_DATA sysData = { 1024u, true };
#endif
//COMMAND_MANAGER cmd;
cptr          ptrLib[16];
CLASS_FILEOPS files       = ptrLib;
CLASS_TIMER   mainTimer   = &ptrLib[1];
vGLOBALCOORDS gco         = {};
THREAD_PROPS  thread      = 5u;    // Thread properties
vui64         THREAD_LIFE = 0x0u;  // 'Thread active' flags
wchar         stErrorFilename[64];
wchptr        stThrdStat;          // Debug output
HINSTANCE     hInst;               // Current instance's handle
HANDLE        hErrorOutput;        // File handle for error output
HRESULT       hr;

int __cdecl wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPTSTR lpCmdLine, _In_ int nCmdShow) {
   ui64         threadLife;

   hPrevInstance;
   hInst = hInstance;   // Store instance handle globally

   FILETIME currentTime;
   GetSystemTimePreciseAsFileTime(&currentTime);
   wsprintfW(stErrorFilename, L"_error logs_/error log %08lX.txt", currentTime.dwLowDateTime);
   ///--- Replace with files. usage
   hErrorOutput = CreateFile(stErrorFilename, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

   thread.idealProcessor[ss_main]     = 0;
   thread.priority[ss_main]           = 0;
   thread.sleepTime[ss_main]          = 1u;
   thread.idealProcessor[ss_input]    = 1u;
   thread.priority[ss_input]          = 2;
   thread.sleepTime[ss_input]         = 1u;
   thread.idealProcessor[ss_video]    = 2u;
   thread.priority[ss_video]          = 1;
   thread.sleepTime[ss_video]         = 1u;
   thread.idealProcessor[ss_audio]    = 3u;
   thread.priority[ss_audio]          = 1;
   thread.sleepTime[ss_audio]         = 1u;
   thread.idealProcessor[ss_worldgen] = 4u;
   thread.priority[ss_worldgen]       = 0;
   thread.sleepTime[ss_worldgen]      = 1u;

   // Prevent thread from shutting down (after engine reset)
   THREAD_LIFE &= ~MAIN_THREAD_DIED;

   THREAD_LIFE |= MAIN_THREAD_ALIVE;

   // Adjust main thread
   thread.handle[ss_main] = GetCurrentThread();
   SetThreadIdealProcessor(thread.handle[ss_main], thread.idealProcessor[ss_main]);
   SetThreadPriority(thread.handle[ss_main], thread.priority[ss_main]);

   // Begin video rendering thread
   THREAD_LIFE |= VIDEO_THREAD_ALIVE;
   thread.handle[ss_video] = (ptr)_beginthread(Direct3D11Thread, 0, NULL);
   Sleep(100);
   SetThreadIdealProcessor(thread.handle[ss_video], thread.idealProcessor[ss_video]);
   SetThreadPriority(thread.handle[ss_video], thread.priority[ss_video]);

   // Begin audio rendering thread
   THREAD_LIFE |= AUDIO_THREAD_ALIVE;
   thread.handle[ss_audio] = (ptr)_beginthread(OpenAL1_1Thread, 0, NULL);
   Sleep(100);
   SetThreadIdealProcessor(thread.handle[ss_audio], thread.idealProcessor[ss_audio]);
   SetThreadPriority(thread.handle[ss_audio], thread.priority[ss_audio]);

   // Begin input processing thread
   THREAD_LIFE |= INPUT_THREAD_ALIVE;
   thread.handle[ss_input] = (ptr)_beginthread(DirectInput8Thread, 0, NULL);
   Sleep(100);
   SetThreadIdealProcessor(thread.handle[ss_input], thread.idealProcessor[ss_input]);
   SetThreadPriority(thread.handle[ss_input], thread.priority[ss_input]);

   // Begin world generation thread
   THREAD_LIFE |= GEN_THREAD_ALIVE;
   thread.handle[ss_worldgen] = (ptr)_beginthread(WorldGenThread, 0, NULL);
   Sleep(100);
   SetThreadIdealProcessor(thread.handle[ss_worldgen], thread.idealProcessor[ss_worldgen]);
   SetThreadPriority(thread.handle[ss_worldgen], thread.priority[ss_worldgen]);

   // Wait for video, audio and input subsystems to finish initialising
   while((THREAD_LIFE & VAI_THREADS_DONE) != VAI_THREADS_DONE) _mm_pause();
   THREAD_LIFE &= ~VAI_THREADS_DONE;

   ///
   /// Primary application loop
   ///
   do {
      threadLife = THREAD_LIFE;

//      mainTimer.Update();

      Idle(thread.sleepTime[ss_main]);
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

void WriteError(cchptrc text, cbool fatal) {
   static char  stTimeStamp[32] {};
   static DWORD dwBytes = 0;

   //cui32 uiTimeSecs = cui32(mainTimer.siStartTics / mainTimer.siFrequency);

   sprintf(stTimeStamp, "%.3f : ", double(mainTimer.siStartTics) / double(mainTimer.siFrequency));

   WriteFile(hErrorOutput, stTimeStamp, DWORD(strlen(stTimeStamp)), &dwBytes, NULL);
   WriteFile(hErrorOutput, text, DWORD(strlen(text)), &dwBytes, NULL);

   if(fatal) {
      WriteFile(hErrorOutput, "   *** FATAL ERROR ***\n", 24, &dwBytes, NULL);

      THREAD_LIFE |= MAIN_THREAD_DIED;
      Sleep(1000);
   } else
      WriteFile(hErrorOutput, "\n", 1, &dwBytes, NULL);
}

inline void Try(cchptrc stEvent, cui32 uiResult, cAE_SUBSYSTEM subsystem) {
   al16 static cchar ae_subsystem[8][20] = { "MAIN:%04X : %s", "INPUT:%04X : %s", "VIDEO:%04X : %s", "AUDIO:%04X : %s", "GUI:%04X : %s", "WORLDGEN:%04X : %s", 0, 0 };
        static char  stDescription[64];

   if(uiResult & 0x080000000) {
      sprintf(stDescription, ae_subsystem[subsystem], uiResult & 0x03FFFFFFF, stEvent);
      WriteError(stDescription, (uiResult & 0x040000000 ? true : false));
   }
}
