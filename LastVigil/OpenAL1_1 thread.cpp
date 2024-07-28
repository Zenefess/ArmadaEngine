/************************************************************
 * File: OpenAL1_1 thread.cpp           Created: 2022/11/12 *
 *                           Code last modified: 2024/07/20 *
 *                                                          *
 * Desc:                                                    *
 *                                                          *
 * Copyright (c) David William Bull.   All rights reserved. *
 ************************************************************/

#include "master header.h"
#include "OpenAL1_1 thread.h"

#ifdef _DEBUG
#define NDEBUG 0
#include "comdef.h"
#include "D3D11SDKLayers.h"
#endif

void OpenAL1_1Thread(void* argList) {
   GLOBALCOORDS        gcoLocal     = {};
   CLASS_OAL11FILEOPS  sndFiles     = files;
   ALCdevice          *ALdev        = NULL;
   ALCcontext         *ALdevcon     = NULL;
   ui64                threadLife   = NULL;
   si32ptrc            siSound      = (si32ptrc)malloc32(sizeof(si32) * MAX_SOUNDS);
   ui8                 uiMaxEAX     = 0;
   ui8                 uiNumBuffers = 4;

   // Prevent thread from shutting down (after engine reset)
   THREAD_LIFE &= ~AUDIO_THREAD_DIED;

Reinitialise_:

   THREAD_LIFE &= ~AUDIO_THREAD_RESET;

   // Initialisation
   ALdev = alcOpenDevice(NULL);
   if(!ALdev) Try(stOpenDev, -2, ss_audio);
   ALdevcon = alcCreateContext(ALdev, NULL);
   if(!ALdevcon) Try(stCreateCon, -3, ss_audio);
   bool bOK = alcMakeContextCurrent(ALdevcon);

   if(alIsExtensionPresent("EAX2.0")) uiMaxEAX = 2;
   if(alIsExtensionPresent("EAX3.0")) uiMaxEAX = 3;
   if(alIsExtensionPresent("EAX4.0")) uiMaxEAX = 4;
   if(alIsExtensionPresent("EAX5.0")) uiMaxEAX = 5;
   ALenum err = alGetError();

   // Check number of available voices
   ALCint nummono, numstereo;
   alcGetIntegerv(ALdev, ALC_MONO_SOURCES, 1, &nummono);
   alcGetIntegerv(ALdev, ALC_STEREO_SOURCES, 1, &numstereo);
   
   VEC3Df listenerPos = null3Df;
   VEC3Df listenerVel = null3Df;
   VEC6Df listenerOri = { 0.0f, 0.0f, 1.0f, 0.0f ,1.0f ,0.0f };
   alListenerf(AL_GAIN, 1.0f);
   alListenerfv(AL_POSITION, listenerPos._fl32);
   alListenerfv(AL_VELOCITY, listenerVel._fl32);
   alListenerfv(AL_ORIENTATION, listenerOri._fl32);

   // Test loading
   siSound[0] = sndFiles.LoadWAV(L"mirrors.wav", L"sounds");
   //alSourcePlay(siSound[0]);

   THREAD_LIFE |= AUDIO_THREAD_DONE;

   ///
   /// Primary rendering loop
   ///
   do {
      threadLife = THREAD_LIFE & AUDIO_THREAD;

      // Read camera's global coordinates
      Copy64(&gco, &gcoLocal, sizeof(GLOBALCOORDS));

      alListenerfv(AL_POSITION, gcoLocal.p);
      alListenerfv(AL_VELOCITY, gcoLocal.v);
      alListenerfv(AL_ORIENTATION, gcoLocal.o);

      Idle(thread.sleepTime[ss_audio]);
   } while (threadLife & AUDIO_THREAD_ALIVE);

   THREAD_LIFE |= AUDIO_THREAD_DIED;
   //_endthread();
}
