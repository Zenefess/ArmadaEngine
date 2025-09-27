/*
 * File: spinlocks.h                           Created: 2025-09-25
 *                                       Last Modified: 2025-09-26
 *
 * Description: Three spin-lock implementations optimized for
 *              different use cases (low-power, balanced, high-
 *              accuracy) on x86-64 (AVX2) systems.
 *
 * To Do: 1. Tune pause/backoff parameters per CPU architecture.
 *        2. Add optional timeout or deadlock detection if needed.
 *
 * Owner: David William Bull                        Version: v0.1
 * Dependencies: typedefs.h (intrinsics), Windows API (Sleep)
 * ISA: x86-64 (AVX2, SSE2)
 * Thread-safety: Yes (intended for multi-thread use)
 *
 * License: MIT                      Copyright: David William Bull
 */
#pragma once

#ifdef _WIN32
#include <Windows.h>
#endif
#include "typedefs.h"

// Constants
static cui32 SPIN_BACKOFF_MIN     = 16;     // initial pause iterations for backoff
static cui32 SPIN_BACKOFF_MAX     = 1024;   // maximum pause iterations for backoff
static cui32 SPIN_YIELD_THRESHOLD = 10000;  // spin iterations before yielding CPU (low-power)
static cui64 SPIN_CYCLE_THRESHOLD = 100000; // CPU cycle threshold to trigger yield (high-accuracy)

/// SpinLockMin
/// 
/// Spin-lock acquisition optimized for long idle waits. Uses a test-and-test-and-set loop 
/// with periodic pauses and yields to minimize CPU usage during extended waits. After spinning 
/// for a short time, the thread yields its timeslice (using `Sleep(0)` or `Sleep(1)`) to allow 
/// other threads (including lower-priority ones) to run. This is suitable for locks expected to 
/// be held for longer durations or when conserving power/CPU is more important than minimal latency.
/// 
/// @param lock  Pointer to a volatile 32-bit lock flag (0 = unlocked, 1 = locked). The flag should 
///              be initially 0. This function will set the flag to 1 when the lock is acquired.
inline void SpinLockMin(vui32ptr lock) {
   bool firstYield = true;
   ui32 spinCount = 0;
   // Loop until lock is acquired
   for(;;) {
      // Spin-wait while lock is taken, but yield periodically to save power
      while(*lock == 1) {
         _mm_pause();  // short pause to reduce power consumption in tight loop:contentReference[oaicite:4]{index=4}
         if (++spinCount >= SPIN_YIELD_THRESHOLD) {
            // Yield CPU after a threshold of spins (low-power strategy)
            if (firstYield) {
               Sleep(0);   // yield to any thread of equal priority
               firstYield = false;
            } else Sleep(1);   // sleep a minimal time slice to allow lower-priority threads to run
            spinCount = 0;
         }
      }
      // Attempt to acquire the lock (0 -> 1). If successful, exit loop.
      if(_InterlockedCompareExchange((long*)lock, 1, 0) == 0) break;
      // If another thread snatched the lock, yield to avoid busy-waiting in contention
      if(firstYield) {
         Sleep(0);
         firstYield = false;
      } else Sleep(1);
      // Loop continues if lock not acquired
   }
}

/// SpinLock
/// 
/// Spin-lock acquisition with balanced behavior for general short critical sections. This implementation 
/// uses a test-and-test-and-set loop with *exponential backoff* and jitter to handle moderate contention. 
/// Each time the lock acquisition fails, the thread waits using an increasing number of `_mm_pause()` 
/// instructions (up to a limit), potentially randomized, to reduce bus contention and the "thundering herd" effect. 
/// This strategy provides a compromise between CPU usage and latency, making it suitable for typical critical sections.
/// 
/// @param lock  Pointer to a volatile 32-bit lock flag (0 = unlocked, 1 = locked). The flag should 
///              be initially 0. This function will set the flag to 1 when the lock is acquired.
inline void SpinLock(vui32ptr lock) {
   ui32 currentBackoff = SPIN_BACKOFF_MIN;
   // Loop until lock is acquired
   for (;;) {
      // First, spin-wait (read-only) until lock appears free to avoid cache contention
      while (*lock == 1) _mm_pause();
      // Attempt to acquire the lock atomically
      if (_InterlockedCompareExchange((long*)lock, 1, 0) == 0) {
         break;  // acquired successfully
      }
      // Lock was free but another thread got it first -> contention encountered
      // Perform exponential backoff with a bit of randomness to reduce contention:contentReference[oaicite:5]{index=5}:contentReference[oaicite:6]{index=6}
      ui64 tsc = __rdtsc();
      // Random pause count between 1 and currentBackoff (inclusive)
      ui32 pauseCount = (ui32)(tsc & (currentBackoff - 1)) + 1;
      for (ui32 i = 0; i < pauseCount; ++i) _mm_pause();
      // Exponentially increase backoff, up to a maximum
      if (currentBackoff < SPIN_BACKOFF_MAX) {
         currentBackoff <<= 1;  // double the backoff interval
         if (currentBackoff > SPIN_BACKOFF_MAX)
            currentBackoff = SPIN_BACKOFF_MAX;
      }
      // Loop continues to retry acquiring the lock
   }
}

/// SpinLockMax
/// 
/// A highly aggressive spin-lock acquisition optimized for hot-path scenarios with very short critical sections and 
/// frequent contention. This implementation busy-waits in a tight loop with minimal backoff, favoring immediate 
/// lock acquisition at the cost of CPU usage. It includes explicit memory fencing to ensure memory order around the 
/// lock, and uses a timing threshold to avoid pathological starvation. If the spin loop exceeds a cycle threshold 
/// (indicating an unusually long wait, possibly due to priority inversion), the thread yields once to allow the lock holder 
/// to proceed, then resumes aggressive spinning. This lock should be used only when lock hold times are expected to be 
/// extremely brief and minimizing lock latency is critical.
/// 
/// @param lock  Pointer to a volatile 32-bit lock flag (0 = unlocked, 1 = locked). The flag should 
///              be initially 0. This function will set the flag to 1 when the lock is acquired.
inline void SpinLockMax(vui32ptr lock) {
   // Record start time stamp counter for timing-based escape
   const unsigned __int64 startTSC = __rdtsc();
   // Loop until lock is acquired
   for (;;) {
      // Test-and-test-and-set: busy-wait until lock seems free
      while (*lock == 1) {
         _mm_pause();
         // If spinning too long, yield once to avoid live-lock (e.g., priority inversion)
         if (__rdtsc() - startTSC > SPIN_CYCLE_THRESHOLD) Sleep(0);  // brief yield to allow another thread to run if needed
      }
      // Attempt to acquire the lock atomically
      if (_InterlockedCompareExchange((long*)lock, 1, 0) == 0) {
         // Lock acquired – enforce a full memory fence to ensure all prior memory operations 
         // by other threads are visible, and subsequent operations by this thread occur after the lock:
         _mm_mfence();
         break;
      }
      // If acquisition failed (contention at the exact moment of lock release), loop continues.
      // (On failure, we continue the tight spin without expanding backoff, to prioritize latency.)
   }
}
