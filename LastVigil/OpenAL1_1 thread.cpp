/************************************************************
 * File: OpenAL1_1 thread.cpp           Created: 2022/11/12 *
 *                           Code last modified: 2022/11/12 *
 *                                                          *
 * Desc:                                                    *
 *                                                          *
 *  Copyright (c) David William Bull. All rights reserved.  *
 ************************************************************/

#include "pch.h"

#include "thread flags.h"
#include "OpenAL1_1 thread.h"

#ifdef _DEBUG
#define NDEBUG 0
#include "comdef.h"
#include "D3D11SDKLayers.h"
#endif

extern vui64 THREAD_LIFE; // 'Thread active' flags

void OpenAL1_1Thread(void* argList) {
   al16 ALCdevice            *ALdev = NULL;
        ALCcontext         *ALdevcon = NULL;
   al16 CLASS_OAL11FILEOPS  files;
        ui64                threadLife = NULL;
        GLOBALCOORDS          gcoLocal {};
   al16 vsi64ptr            gcoPtr = (vsi64ptr)&gco;
   al16 si64ptr             gcoLocalPtr = (si64ptr)&gcoLocal;
        si32                siSound[4096];
        ui8                   uiMaxEAX = 0, uiNumBuffers = 4;

   // Prevent thread from shutting down (after engine reset)
   THREAD_LIFE &= ~AUDIO_THREAD_DIED;

Reinitialise_:

   THREAD_LIFE &= ~AUDIO_THREAD_RESET;

   // Initialisation
   ALdev = alcOpenDevice(NULL);
   if(!ALdev) Try(stOpenDev, -2);
   ALdevcon = alcCreateContext(ALdev, NULL);
   if(!ALdevcon) Try(stCreateCon, -3);
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
   
   ALfloat listenerPos[] = { 0.0f, 0.0f, 0.0f };
   ALfloat listenerVel[] = { 0.0f, 0.0f, 0.0f };
   ALfloat listenerOri[] = { 0.0f, 0.0f, 1.0f, 0.0f ,1.0f ,0.0f };
   alListenerf(AL_GAIN, 1.0f);
   alListenerfv(AL_POSITION, listenerPos);
   alListenerfv(AL_VELOCITY, listenerVel);
   alListenerfv(AL_ORIENTATION, listenerOri);

   // Test loading
   siSound[0] = files.LoadWAV((wchptr)L"sounds\\mirrors.wav");
   //alSourcePlay(siSound[0]);

   // Primary rendering loop
   do {
      threadLife = THREAD_LIFE & AUDIO_THREAD;

      // Read camera's global coordinates
      gcoPtr = (vsi64ptr)&gco;
      gcoLocalPtr = (si64ptr)&gcoLocal;
      memcpy(gcoLocalPtr, (ptr)gcoPtr, sizeof(GLOBALCOORDS));

      alListenerfv(AL_POSITION, gcoLocal.p);
      alListenerfv(AL_VELOCITY, gcoLocal.v);
      alListenerfv(AL_ORIENTATION, gcoLocal.o);

      Sleep(1);
   } while (threadLife & AUDIO_THREAD_ALIVE);

   THREAD_LIFE |= AUDIO_THREAD_DIED;
   //_endthread();
}
