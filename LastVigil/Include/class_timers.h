/************************************************************
 * File: class_timer.h                  Created: 2022/10/20 *
 *                                Last modified: 2023/05/13 *
 *                                                          *
 * Desc: Single and multi timer classes.                    *
 *                                                          *
 *  Copyright (c) David William Bull. All rights reserved.  *
 ************************************************************/
#pragma once

al32 struct CLASS_TIMER {
   double dGrandTotal, dTotal, dElapsed, dScale, dScaleConst, dScaleAccel, _dScaleAccelTemp;
   si64   siFrequency, siOriginTics, siStartTics, siPrevTics, siCurrentTics, siTotalTics, siElapsedTics, siPauseTics;
   si32   siTotalUpdates, siUpdatesSinceLastReset;

   void Init(void) {
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
      dGrandTotal += (dTotal += (dElapsed = (double(siElapsedTics) * dScale) / double(siFrequency)));
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

   inline void UnFreeze(void) {
      dScaleAccel = _dScaleAccelTemp;
      siPauseTics = 0;
      dScale = dScaleConst;
   };

   inline void Reset(cfl64 timeScale) {
      dScale = dScaleConst = dScaleAccel = timeScale;
      siStartTics = siCurrentTics;
      dTotal = dElapsed = 0.0;
      siElapsedTics = siUpdatesSinceLastReset = 0;
   };

   inline void Rescale(double timeScale) {
      dScaleConst = timeScale;
   };

   inline cfl64 GetElapsedTimeScaled(void) const {
      return dElapsed;
   };

   inline cfl64 GetTotalTimeScaled(void) const {
      return dTotal;
   };

   inline cfl64 GetGrandTotalTimeScaled(void) const {
      return dGrandTotal;
   };

   inline cfl64 GetElapsedTimeUnscaled(void) const {
      return fl64(siElapsedTics) / fl64(siFrequency);
   };

   inline cfl64 GetTotalTimeUnscaled(void) const {
      return fl64(siTotalTics) / fl64(siFrequency);
   };

   inline csi32 GetUpdates(void) const {
      return siUpdatesSinceLastReset;
   }

   inline csi32 GetTotalUpdates(void) const {
      return siTotalUpdates;
   }

   inline cfl64 UpdatesPerSecondScaled(void) const {
      return fl64(siUpdatesSinceLastReset) / dElapsed;
   };

   inline cfl64 AverageUpdatesPerSecondScaled(void) const {
      return fl64(siTotalUpdates) / dTotal;
   };
};

al16 struct CLASS_TIMERS {
   #define MAX_TIMERS 16

   struct TIMER_VARIABLES {
      fl64 dTotal, dElapsed, dScale, dScaleConst, dScaleAccel, _dScaleAccelTemp;
      si64 siOriginTics, siStartTics, siPrevTics, siCurrentTics, siTotalTics, siElapsedTics, siPauseTics;
      si32 siTotalUpdates, siUpdatesSinceLastReset;
   } timer[MAX_TIMERS] {};

   si64 siFrequency;
   si32 uiTimerCount = 0;

   ui32 Init(void) {
      TIMER_VARIABLES &slot = timer[uiTimerCount];

      if(uiTimerCount >= MAX_TIMERS) return 0x0c0000001;

      QueryPerformanceCounter((LARGE_INTEGER *)&slot.siOriginTics);
      slot.dScale = slot.dScaleConst = 1.0;
      slot.siTotalUpdates = slot.siUpdatesSinceLastReset = 0;

      if(!siFrequency) QueryPerformanceFrequency((LARGE_INTEGER *)&siFrequency);

      slot.siTotalTics = slot.siStartTics = slot.siCurrentTics = slot.siOriginTics;
      slot.dTotal = slot.dElapsed = 0.0;

      return uiTimerCount++;
   };

   inline void Update(si8 index) {
      TIMER_VARIABLES &slot = timer[uiTimerCount];

      slot.siPrevTics = slot.siCurrentTics;
      QueryPerformanceCounter((LARGE_INTEGER *)&slot.siCurrentTics);
      slot.siTotalTics += (slot.siElapsedTics = slot.siCurrentTics - slot.siPrevTics);
      (slot.dScale += slot.dScaleAccel) = (slot.dScale < 0.0 ? max(slot.dScale, slot.dScaleConst) : min(slot.dScale, slot.dScaleConst));
      slot.dTotal += (slot.dElapsed = (double(slot.siElapsedTics) * slot.dScale) / double(siFrequency));
      slot.siUpdatesSinceLastReset++;
      slot.siTotalUpdates++;
   };

   inline void UpdateAll() {
      QueryPerformanceCounter((LARGE_INTEGER *)&(timer[0].siCurrentTics));
      csi64 curTics = timer[0].siCurrentTics;

      for(ui8 i = 0; i < uiTimerCount; i++) {
         timer[i].siCurrentTics = curTics;
         timer[i].siTotalTics += (timer[i].siElapsedTics = timer[i].siCurrentTics - timer[i].siPrevTics);
         (timer[i].dScale += timer[i].dScaleAccel) =
            (timer[i].dScale < 0.0 ? max(timer[i].dScale, timer[i].dScaleConst) : min(timer[i].dScale, timer[i].dScaleConst));
         timer[i].dTotal += (timer[i].dElapsed = (double(timer[i].siElapsedTics) * timer[i].dScale) / double(siFrequency));
         timer[i].siUpdatesSinceLastReset++;
         timer[i].siTotalUpdates++;
      }
   };

   inline void Pause(double deceleration, ui8 index) {
      TIMER_VARIABLES &slot = timer[index];

      slot.siPauseTics = slot.siElapsedTics;
      slot.dScaleAccel = deceleration;
   };

   inline void UnPause(double acceleration, ui8 index) {
      TIMER_VARIABLES &slot = timer[index];

      slot.siPauseTics = 0;
      slot.dScaleAccel = acceleration;
   };

   inline void Freeze(ui8 index) {
      TIMER_VARIABLES &slot = timer[index];

      slot._dScaleAccelTemp = slot.dScaleAccel;
      slot.siPauseTics      = slot.siCurrentTics;
      slot.dScale           = 0.0;
      slot.dScaleAccel      = 0.0;
   };

   inline void UnFreeze(ui8 index) {
      TIMER_VARIABLES &slot = timer[index];

      slot.dScaleAccel = slot._dScaleAccelTemp;
      slot.siPauseTics = 0;
      slot.dScale = slot.dScaleConst;
   };

   inline void Reset(double timeScale, ui8 index) {
      TIMER_VARIABLES &slot = timer[index];

      slot.dScale = slot.dScaleConst = slot.dScaleAccel = timeScale;
      slot.siStartTics = slot.siCurrentTics;
      slot.dElapsed = 0.0;
      slot.siElapsedTics = slot.siUpdatesSinceLastReset = 0;
   };

   inline void Rescale(double timeScale, ui8 index) {
      timer[index].dScaleConst = timeScale;
   };

   inline double GetElapsedTimeScaled(ui8 index) const {
      return timer[index].dElapsed;
   };

   inline double GetTotalTimeScaled(ui8 index) const {
      return timer[index].dTotal;
   };

   inline double GetElapsedTimeUnscaled(ui8 index) const {
      return double(timer[index].siElapsedTics) / double(siFrequency);
   };

   inline double GetTotalTimeUnscaled(ui8 index) const {
      return double(timer[index].siTotalTics) / double(siFrequency);
   };

   inline si32 GetUpdates(ui8 index) const {
      return timer[index].siUpdatesSinceLastReset;
   }

   inline si32 GetTotalUpdates(ui8 index) const {
      return timer[index].siTotalUpdates;
   }

   inline double UpdatesPerSecondScaled(ui8 index) const {
      return double(timer[index].siUpdatesSinceLastReset) / timer[index].dElapsed;
   };

   inline double AverageUpdatesPerSecondScaled(ui8 index) const {
      return double(timer[index].siTotalUpdates) / timer[index].dTotal;
   };
};
