/************************************************************
 * File: OAL1_1 file operations.h       Created: 2022/11/12 *
 *                                Last modified: 2024/05/06 *
 *                                                          *
 * Notes: 2024/05/06: Added support for data tracking       *
 *                                                          *
 *                         Copyright (c) David William Bull *
 ************************************************************/
#pragma once

#ifdef DATA_TRACKING
#include "data tracking.h"
extern SYSTEM_DATA sysData;
#endif

// .WAV data structures
struct RIFFHEADER {
   char id[4];
   ui32 len;
};

struct WAVEFORMCHUNK {
   char wID[4],
        fID[4];
   ui32 len;
   ui16 fmt,
        ch;
   ui32 sr,
        aBr;
   ui16 aBps,
        bd;
};

struct DATACHUNK {
   char id[4];
   ui32 len;
};

al16 struct CLASS_OAL11FILEOPS {
   al16 HANDLE        hSound = 0; // [1024] {};
        void         *pAudioData[1024] {};
        ALuint        alBuffers[1024] {};
        ui32          uiSourceObject[1024] {}, uiBytes;
        si32          siSoundCount = 0;
   al16 RIFFHEADER    rhWAV;
        WAVEFORMCHUNK wcWAV;
        DATACHUNK     dcWAV;

   si32 LoadWAV(wchptr filename) {
      hSound = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
#ifdef DATA_TRACKING
      sysData.storage.filesOpened++;
#endif
      // Read .WAV header data
      bool bOK = ReadFile(hSound, &rhWAV, sizeof(rhWAV), (LPDWORD)&uiBytes, NULL);
#ifdef DATA_TRACKING
      sysData.storage.bytesRead += uiBytes;
#endif
      bOK = ReadFile(hSound, &wcWAV, sizeof(wcWAV), (LPDWORD)&uiBytes, NULL);
#ifdef DATA_TRACKING
      sysData.storage.bytesRead += uiBytes;
#endif
      bOK = ReadFile(hSound, &dcWAV, sizeof(dcWAV), (LPDWORD)&uiBytes, NULL);
#ifdef DATA_TRACKING
      sysData.storage.bytesRead += uiBytes;
#endif
      pAudioData[siSoundCount] = malloc16(dcWAV.len);
      bOK = ReadFile(hSound, pAudioData[siSoundCount], dcWAV.len, (LPDWORD)&uiBytes, NULL);
#ifdef DATA_TRACKING
      sysData.storage.bytesRead += uiBytes;
#endif
      CloseHandle(hSound);
#ifdef DATA_TRACKING
      sysData.storage.filesClosed++;
#endif

      alGenBuffers(1, &alBuffers[siSoundCount]);
      alBufferData(alBuffers[siSoundCount], 0x01100 + ((wcWAV.ch - 1) << 1) + (wcWAV.bd >> 4), pAudioData[siSoundCount], dcWAV.len, wcWAV.sr);

      alGenSources(1, &uiSourceObject[siSoundCount]);
      alSourcei(uiSourceObject[siSoundCount],AL_BUFFER, alBuffers[siSoundCount]);

      alSourcef(uiSourceObject[siSoundCount], AL_PITCH, 1.0f);
      alSourcef(uiSourceObject[siSoundCount], AL_GAIN, 1.0f);
      alSource3f(uiSourceObject[siSoundCount], AL_POSITION, 0.0f, 0.0f, 0.0f);
      alSource3f(uiSourceObject[siSoundCount], AL_VELOCITY, 0.0f, 0.0f, 0.0f);
      alSourcei(uiSourceObject[siSoundCount], AL_LOOPING, true);
      alSourcef(uiSourceObject[siSoundCount], AL_REFERENCE_DISTANCE, 128.0f);
      alSourcef(uiSourceObject[siSoundCount], AL_MAX_DISTANCE, 16384.0f);

      siSoundCount++;
      return uiSourceObject[siSoundCount - 1];
   }
};