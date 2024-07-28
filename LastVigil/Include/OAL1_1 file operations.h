/************************************************************
 * File: OAL1_1 file operations.h       Created: 2022/11/12 *
 *                                Last modified: 2024/07/10 *
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

#ifndef MAX_SOUNDS
#define MAX_SOUNDS 1024
#endif

// .WAV data structures
struct RIFFHEADER {
   char id[4];
   ui32 len;
};

struct WAVEFORMCHUNK {
   char wID[4];
   char fID[4];
   ui32 len;
   ui16 fmt;
   ui16 ch;
   ui32 sr;
   ui32 aBr;
   ui16 aBps;
   ui16 bd;
};

struct DATACHUNK {
   char id[4];
   ui32 len;
};

///  !!!Rewrite to use external CLASS_FILEOPS and malloc!!!
al8 struct CLASS_OAL11FILEOPS {
   CLASS_FILEOPS &files;
   HANDLE         hSound = 0; // [1024] {};
   ptrptrc        pAudioData     = (ptrptrc)malloc64(sizeof(ptr) * MAX_SOUNDS);
   ui32ptrc       alBuffers      = (ui32ptrc)malloc64(sizeof(ptr) * MAX_SOUNDS);
   ui32ptrc       uiSourceObject = (ui32ptrc)malloc64(sizeof(ptr) * MAX_SOUNDS);
   si32           siSoundCount = 0;
   WAVEFORMCHUNK  wcWAV;
   RIFFHEADER     rhWAV;
   DATACHUNK      dcWAV;

   CLASS_OAL11FILEOPS(CLASS_FILEOPS &fileOps) : files(fileOps) {}

   si32 LoadWAV(cwchptrc filename, cwchptrc folder) {
      hSound = files.OpenForReading(filename, folder);
      // Read .WAV header data
      files.Read(hSound, &rhWAV, sizeof(rhWAV));
      files.Read(hSound, &wcWAV, sizeof(wcWAV));
      files.Read(hSound, &dcWAV, sizeof(dcWAV));

      pAudioData[siSoundCount] = malloc16(dcWAV.len);

      files.Read(hSound, pAudioData[siSoundCount], dcWAV.len);
#ifdef DATA_TRACKING
      sysData.storage.bytesRead += files.bytesProcessed;
#endif
      files.CloseFile(hSound);

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

      return uiSourceObject[siSoundCount++];
   }
};