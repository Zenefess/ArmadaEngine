/************************************************************
 * File: class_timer.h                  Created: 2022/10/20 *
 *                                Last modified: 2024/06/29 *
 *                                                          *
 * Desc: Single and multi timer classes.                    *
 *                                                          *
 * MIT license             Copyright (c) David William Bull *
 ************************************************************/
#pragma once

al64 struct TIMER_VARIABLES {
   fl64 dGrandTotal;
   si64 siFrequency;
   fl64 dTotal, dElapsed, dScale, dScaleConst, dScaleAccel, _dScaleAccelTemp;
   si64 siOriginTics, siStartTics, siPrevTics, siCurrentTics, siTotalTics, siElapsedTics, siPauseTics;
   si32 siTotalUpdates, siUpdatesSinceLastReset;
};

al64 struct CLASS_TIMER {
   fl64 dGrandTotal;
   si64 siFrequency;
   fl64 dTotal, dElapsed, dScale, dScaleConst, dScaleAccel, _dScaleAccelTemp;
   si64 siOriginTics, siStartTics, siPrevTics, siCurrentTics, siTotalTics, siElapsedTics, siPauseTics;
   si32 siTotalUpdates, siUpdatesSinceLastReset;

   CLASS_TIMER(void) { Reinitialise(); }
   CLASS_TIMER(cptrptr globalPointer) { Reinitialise(); if(globalPointer) *globalPointer = this; }

   void Reinitialise(void) {
      QueryPerformanceCounter((LARGE_INTEGER *)&siOriginTics);
      dScale = dScaleConst = 1.0;
      siTotalUpdates = siUpdatesSinceLastReset = 0;
      QueryPerformanceFrequency((LARGE_INTEGER *)&siFrequency);
      siTotalTics = siStartTics = siCurrentTics = siOriginTics;
      dTotal = dElapsed = 0.0;
   };

   inline void Update(void) {
      siPrevTics = siCurrentTics;
      QueryPerformanceCounter((LARGE_INTEGER *)&siCurrentTics);
      siTotalTics += (siElapsedTics = siCurrentTics - siPrevTics);
      (dScale += dScaleAccel) = (dScale < 0.0 ? max(dScale, dScaleConst) : min(dScale, dScaleConst));
      dGrandTotal += (dTotal += (dElapsed = (fl64(siElapsedTics) * dScale) / fl64(siFrequency)));
      siUpdatesSinceLastReset++;
      siTotalUpdates++;
   };

   inline void Pause(cfl64 deceleration) {
      siPauseTics = siElapsedTics;
      dScaleAccel = deceleration;
   };

   inline void UnPause(cfl64 acceleration) {
      siPauseTics = 0;
      dScaleAccel = acceleration;
   };

   inline void Freeze(void) {
      _dScaleAccelTemp = dScaleAccel;
      siPauseTics = siCurrentTics;
      dScale      = 0.0;
      dScaleAccel = 0.0;
   };

   inline void Unfreeze(void) {
      dScaleAccel = _dScaleAccelTemp;
      siPauseTics = 0;
      dScale = dScaleConst;
   };

   inline void Reset(cfl64 timeScale) {
      dScale        = dScaleConst = dScaleAccel = timeScale;
      siStartTics   = siCurrentTics;
      dTotal        = dElapsed = 0.0;
      siElapsedTics = siUpdatesSinceLastReset = 0;
   };

   inline void SetScale(cfl64 timeScale) { dScaleConst = timeScale; };

   inline cfl64 GetElapsedTimeScaled(void) const { return dElapsed; };

   inline cfl64 GetTotalTimeScaled(void) const { return dTotal; };

   inline cfl64 GetGrandTotalTimeScaled(void) const { return dGrandTotal; };

   inline cfl64 GetElapsedTimeUnscaled(void) const { return fl64(siElapsedTics) / fl64(siFrequency); };

   inline cfl64 GetTotalTimeUnscaled(void) const { return fl64(siTotalTics) / fl64(siFrequency); };

   inline csi32 GetUpdates(void) const { return siUpdatesSinceLastReset; }

   inline csi32 GetTotalUpdates(void) const { return siTotalUpdates; }

   inline cfl64 UpdatesPerSecondScaled(void) const { return fl64(siUpdatesSinceLastReset) / dElapsed; };

   inline cfl64 AverageUpdatesPerSecondScaled(void) const { return fl64(siTotalUpdates) / dTotal; };
};

al16 struct CLASS_TIMERS {
#ifndef MAX_TIMERS
   #define MAX_TIMERS 16
#endif

   TIMER_VARIABLES *const timer = (TIMER_VARIABLES *)zalloc64(RoundUpToNearest64(sizeof(TIMER_VARIABLES[MAX_TIMERS])));

   ui32 uiTimerCount = 0;

   CLASS_TIMERS(void) { ReinitialiseAll(); }
   CLASS_TIMERS(cptrptr globalPointer) { ReinitialiseAll(); if(globalPointer) *globalPointer = this; }

   void Reinitialise(cui8 index) {
      TIMER_VARIABLES &slot = timer[index];

      QueryPerformanceCounter((LARGE_INTEGER *)&slot.siOriginTics);
      slot.dScale = slot.dScaleConst = 1.0;
      slot.siTotalUpdates = slot.siUpdatesSinceLastReset = 0;

      if(!slot.siFrequency) QueryPerformanceFrequency((LARGE_INTEGER *)&slot.siFrequency);

      slot.siTotalTics = slot.siStartTics = slot.siCurrentTics = slot.siOriginTics;
      slot.dTotal = slot.dElapsed = 0.0;
   };

   void ReinitialiseAll(void) {
      for(ui32 index = 0; index < uiTimerCount; ++index) {
         TIMER_VARIABLES &slot = timer[index];

         QueryPerformanceCounter((LARGE_INTEGER *)&slot.siOriginTics);
         slot.dScale = slot.dScaleConst = 1.0;
         slot.siTotalUpdates = slot.siUpdatesSinceLastReset = 0;

         if(!slot.siFrequency) QueryPerformanceFrequency((LARGE_INTEGER *)&slot.siFrequency);

         slot.siTotalTics = slot.siStartTics = slot.siCurrentTics = slot.siOriginTics;
         slot.dTotal = slot.dElapsed = 0.0;
      }
   };

   cui32 Create(void) {
      if(uiTimerCount >= MAX_TIMERS) return 0x0c0000001;
      Reinitialise(uiTimerCount);
      return uiTimerCount++;
   }

   inline void Update(cui8 index) {
      TIMER_VARIABLES &slot = timer[index];

      slot.siPrevTics = slot.siCurrentTics;
      QueryPerformanceCounter((LARGE_INTEGER *)&slot.siCurrentTics);
      slot.siTotalTics += (slot.siElapsedTics = slot.siCurrentTics - slot.siPrevTics);
      (slot.dScale += slot.dScaleAccel) = (slot.dScale < 0.0 ? max(slot.dScale, slot.dScaleConst) : min(slot.dScale, slot.dScaleConst));
      slot.dTotal += (slot.dElapsed = (fl64(slot.siElapsedTics) * slot.dScale) / fl64(slot.siFrequency));
      slot.dGrandTotal += (slot.dTotal += (slot.dElapsed = (fl64(slot.siElapsedTics) * slot.dScale) / fl64(slot.siFrequency)));
      slot.siUpdatesSinceLastReset++;
      slot.siTotalUpdates++;
   };

   inline void UpdateAll(void) {
      QueryPerformanceCounter((LARGE_INTEGER *)&(timer[0].siCurrentTics));
      csi64 curTics = timer[0].siCurrentTics;

      for(ui32 i = 0; i < uiTimerCount; i++) {
         timer[i].siCurrentTics = timer[0].siCurrentTics;
         timer[i].siCurrentTics = curTics;
         timer[i].siTotalTics += (timer[i].siElapsedTics = timer[i].siCurrentTics - timer[i].siPrevTics);
         (timer[i].dScale += timer[i].dScaleAccel) =
            (timer[i].dScale < 0.0 ? max(timer[i].dScale, timer[i].dScaleConst) : min(timer[i].dScale, timer[i].dScaleConst));
         timer[i].dTotal += (timer[i].dElapsed = (fl64(timer[i].siElapsedTics) * timer[i].dScale) / fl64(timer[i].siFrequency));
         timer[i].dGrandTotal += (timer[i].dTotal += (timer[i].dElapsed = (fl64(timer[i].siElapsedTics) * timer[i].dScale) / fl64(timer[i].siFrequency)));
         timer[i].siUpdatesSinceLastReset++;
         timer[i].siTotalUpdates++;
      }
   };

   inline void Pause(cfl64 deceleration, cui8 index) {
      TIMER_VARIABLES &slot = timer[index];

      slot.siPauseTics = slot.siElapsedTics;
      slot.dScaleAccel = deceleration;
   };

   inline void UnPause(cfl64 acceleration, cui8 index) {
      TIMER_VARIABLES &slot = timer[index];

      slot.siPauseTics = 0;
      slot.dScaleAccel = acceleration;
   };

   inline void Freeze(cui8 index) {
      TIMER_VARIABLES &slot = timer[index];

      slot._dScaleAccelTemp = slot.dScaleAccel;
      slot.siPauseTics      = slot.siCurrentTics;
      slot.dScale           = 0.0;
      slot.dScaleAccel      = 0.0;
   };

   inline void Unfreeze(cui8 index) {
      TIMER_VARIABLES &slot = timer[index];

      slot.dScaleAccel = slot._dScaleAccelTemp;
      slot.siPauseTics = 0;
      slot.dScale = slot.dScaleConst;
   };

   inline void Reset(cfl64 timeScale, cui8 index) {
      TIMER_VARIABLES &slot = timer[index];

      slot.dScale        = slot.dScaleConst = slot.dScaleAccel = timeScale;
      slot.siStartTics   = slot.siCurrentTics;
      slot.dElapsed      = 0.0;
      slot.siElapsedTics = slot.siUpdatesSinceLastReset = 0;
   };

   inline void SetScale(cfl64 timeScale, cui8 index) { timer[index].dScaleConst = timeScale; };

   inline cfl64 GetElapsedTimeScaled(cui8 index) const { return timer[index].dElapsed; };

   inline cfl64 GetTotalTimeScaled(cui8 index) const { return timer[index].dTotal; };

   inline cfl64 GetGrandTotalTimeScaled(cui8 index) const { return timer[index].dGrandTotal; };

   inline cfl64 GetElapsedTimeUnscaled(cui8 index) const { return fl64(timer[index].siElapsedTics) / fl64(timer[index].siFrequency); };

   inline cfl64 GetTotalTimeUnscaled(cui8 index) const { return fl64(timer[index].siTotalTics) / fl64(timer[index].siFrequency); };

   inline csi32 GetUpdates(cui8 index) const { return timer[index].siUpdatesSinceLastReset; }

   inline csi32 GetTotalUpdates(cui8 index) const { return timer[index].siTotalUpdates; }

   inline cfl64 UpdatesPerSecondScaled(cui8 index) const { return fl64(timer[index].siUpdatesSinceLastReset) / timer[index].dElapsed; };

   inline cfl64 AverageUpdatesPerSecondScaled(cui8 index) const { return fl64(timer[index].siTotalUpdates) / timer[index].dTotal; };
};
