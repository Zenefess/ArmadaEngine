/*
 * File: spinlocks.h
 * Version: v1.0.0
 * Owner: David William Bull
 * Created: 2026-08-11
 * Last Modified: 2026-08-11
 * Description: User-space spin locks for x86-64 MSVC builds: three acquisition profiles, try-acquire, and full-barrier release.
 * To Do: 1) Tune backoff and yield thresholds per CPU architecture; record results in bench/ per bd1.
 *        2) Add optional acquisition timeout and debug-build deadlock detection.
 *        3) Add a cache-line-padded lock type for lock arrays, to prevent false sharing.
 * Dependencies: typedefs.h, windows.h, intrin.h
 * ISA: AVX2
 * Thread-safety: MT-safe
 * Reviewers: David William Bull
 * License: MIT  Copyright: David William Bull
 */
#pragma once

#include <windows.h>
#include <intrin.h>
#include "typedefs.h"

#pragma intrinsic(_InterlockedCompareExchange, _InterlockedExchange)

// GCS a2/a3 build guards: the CPU baseline is AVX2+FMA3+BMI2, compiled with /arch:AVX2
#ifndef __AVX2__
#error spinlocks.h: compile with /arch:AVX2 -- GCS a2 sets the CPU baseline to AVX2+FMA3+BMI2.
#endif
static_assert(__AVX2__, "GCS a3 build guard: __AVX2__ must be defined and non-zero.");

//== Tuning constants

constexpr cui32 SPIN_BACKOFF_MIN     = 16u;     // Initial ceiling of the randomised backoff window; power of two
constexpr cui32 SPIN_BACKOFF_MAX     = 1024u;   // Final ceiling of the randomised backoff window; power of two
constexpr cui32 SPIN_YIELD_THRESHOLD = 10000u;  // Pause iterations before SpinLockMin yields the CPU
constexpr cui64 SPIN_CYCLE_THRESHOLD = 100000u; // TSC-cycle delta before SpinLockMax's starvation-escape yield

static_assert(SPIN_BACKOFF_MIN && !(SPIN_BACKOFF_MIN & (SPIN_BACKOFF_MIN - 1u)), "SPIN_BACKOFF_MIN must be a power of two: mask-based jitter.");
static_assert(SPIN_BACKOFF_MAX && !(SPIN_BACKOFF_MAX & (SPIN_BACKOFF_MAX - 1u)), "SPIN_BACKOFF_MAX must be a power of two: mask-based jitter.");
static_assert(SPIN_BACKOFF_MIN <= SPIN_BACKOFF_MAX, "Backoff window is inverted.");

//== Lock operations

/// Acquires a spin lock; conserves power and CPU during long waits.
/// Test-and-test-and-set with _mm_pause, escalating to Sleep(0) and then Sleep(1) after SPIN_YIELD_THRESHOLD
/// pause iterations, so ready threads of equal and then lower priority (including a pre-empted lock holder)
/// are able to run. Best when the lock may be held for a long time, or when conserving power and CPU matters
/// more than acquisition latency.
/// @param lock  32-bit lock flag: 0 == unlocked, 1 == locked. Must be initialised to 0 and naturally aligned.
/// @note Acquisition is a full barrier (LOCK CMPXCHG): reads and writes after the call cannot move before it.
/// @note Sleep(1) surrenders the rest of the timeslice; the actual delay is >= the system timer period (~1ms-15.6ms).
inline void SpinLockMin(vui32ptrc lock) {
   ui32 spinCount  = 0;
   bool firstYield = true;

   for(;;) {
      // Read-only wait: no interlocked traffic while the lock is observed held
      while(*lock) {
         _mm_pause();
         if(++spinCount >= SPIN_YIELD_THRESHOLD) {
            if(firstYield) {
               Sleep(0);             // Yield to ready threads of equal priority
               firstYield = false;
            } else Sleep(1);         // Surrender the timeslice; lower-priority threads may run
            spinCount = 0;
         }
      }
      // The lock was observed free; attempt to acquire it: 0 -> 1
      if(_InterlockedCompareExchange((vol long *)lock, 1, 0) == 0) return;
   }
}

/// Acquires a spin lock; balanced latency versus contention behaviour for typical short critical sections.
/// Test-and-test-and-set with bounded, TSC-jittered exponential backoff after each failed acquisition,
/// reducing coherence traffic and thundering-herd retries under moderate contention.
/// @param lock  32-bit lock flag: 0 == unlocked, 1 == locked. Must be initialised to 0 and naturally aligned.
/// @note Acquisition is a full barrier (LOCK CMPXCHG): reads and writes after the call cannot move before it.
inline void SpinLock(vui32ptrc lock) {
   ui32 backoff = SPIN_BACKOFF_MIN;

   for(;;) {
      // Read-only wait: no interlocked traffic while the lock is observed held
      while(*lock) _mm_pause();
      // The lock was observed free; attempt to acquire it: 0 -> 1
      if(_InterlockedCompareExchange((vol long *)lock, 1, 0) == 0) return;
      // Lost the race: pause 1~backoff times, jittered by the TSC, then widen the window up to SPIN_BACKOFF_MAX
      cui32 pauses = (ui32(__rdtsc()) & (backoff - 1u)) + 1u;
      for(ui32 i = 0; i < pauses; ++i) _mm_pause();
      if(backoff < SPIN_BACKOFF_MAX) backoff <<= 1u;
   }
}

/// Acquires a spin lock; minimum-latency acquisition for very short, hot critical sections.
/// Tight test-and-test-and-set with no backoff. If the wait exceeds SPIN_CYCLE_THRESHOLD cycles -- e.g. the
/// holder was pre-empted -- the thread yields once via Sleep(0), re-arms the threshold, and resumes spinning.
/// @param lock  32-bit lock flag: 0 == unlocked, 1 == locked. Must be initialised to 0 and naturally aligned.
/// @note Acquisition is a full barrier (LOCK CMPXCHG): reads and writes after the call cannot move before it;
///       no separate fence is required.
inline void SpinLockMax(vui32ptrc lock) {
   ui64 refTSC = __rdtsc();

   for(;;) {
      // Read-only wait: no interlocked traffic while the lock is observed held
      while(*lock) {
         _mm_pause();
         if(__rdtsc() - refTSC > SPIN_CYCLE_THRESHOLD) {
            Sleep(0);                // Starvation escape: the holder may have been pre-empted
            refTSC = __rdtsc();      // Re-arm the threshold, then resume spinning
         }
      }
      // The lock was observed free; attempt to acquire it: 0 -> 1
      if(_InterlockedCompareExchange((vol long *)lock, 1, 0) == 0) return;
   }
}

/// Attempts to acquire a spin lock without waiting.
/// @param lock  32-bit lock flag: 0 == unlocked, 1 == locked. Must be initialised to 0 and naturally aligned.
/// @return true if the lock was acquired, otherwise false.
/// @note A successful attempt is a full barrier (LOCK CMPXCHG); a failed attempt imposes no ordering.
inline cbool SpinLockTry(vui32ptrc lock) { return _InterlockedCompareExchange((vol long *)lock, 1, 0) == 0; }

/// Releases a spin lock acquired by SpinLockMin, SpinLock, SpinLockMax, or SpinLockTry.
/// @param lock  32-bit lock flag: 1 == locked on entry; 0 == unlocked on return.
/// @note Release is a full barrier (XCHG): every write inside the critical section is globally visible before
///       the flag clears, independent of the /volatile:ms|iso compiler mode. Call only while holding the lock.
inline void SpinUnlock(vui32ptrc lock) { _InterlockedExchange((vol long *)lock, 0); }
