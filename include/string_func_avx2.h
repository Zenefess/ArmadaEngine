/*
 * File: string_func_avx2.h
 *
 * Version: v1.0
 *
 * Owner: David William Bull
 *
 * Created: 2025-09-26
 *
 * Last Modified: 2025-09-26
 *
 * Description: SIMD-accelerated string processing functions.
 *
 * To Do: 1) Wire CPUID dispatch and scalar fallbacks.
 *        2) Add microbenchmarks in docs/.
 *
 * Dependencies: typedefs.h, vector structures.h
 *
 * ISA: AVX2
 *
 * Thread-safety: Reentrant
 *
 * Reviewers: David William Bull
 *
 * License: MIT
 *
 * Copyright: David William Bull
 */
#pragma once

#include <bit>
#include <typedefs.h>
#include <corecrt_math.h>

#define _STRING_FUNCTIONS_AVX2_

/// st_len32
///
/// String length calculation using AVX2 aligned to 32-byte boundaries.
/// @param s  Pointer to the string to be measured.
inline si32 st_len32(cchptr s) {
  // Precondition: s is 32-byte aligned (e.g., allocated with 32B alignment)
  cchptr start = s;
  cui256 zero  = _mm256_setzero_si256();

  for(;;) {
    // Load 32 bytes from memory (aligned load)
    cui256 vec = _mm256_load_si256(reinterpret_cast<cui256ptr>(s));
    // Compare each byte with zero byte (results in 0xFF at positions of match)
    cui256 cmp = _mm256_cmpeq_epi8(vec, zero);
    // Create a bit mask from the comparison (one bit per byte)
    ui32 mask = static_cast<ui32>(_mm256_movemask_epi8(cmp));

    if(mask != 0) {
// A null byte was found in this 32-byte chunk
#if __cplusplus >= 202002L
      size_t offset = std::countr_zero(mask); // index of first 1-bit (C++20)
#else
      csi32 offset = _tzcnt_u32(mask); // index of first 1-bit (GCC/Clang)
#endif
      return (s - start) + offset;
    }

    s += 32; // move to next 32-byte block
  }
}

/// st_len
///
/// String length calculation using AVX2 not aligned to 32-byte boundaries.
/// @param s  Pointer to the string to be measured.
size_t st_len(cchptr s) {
   cchptr start = s;                         // Save the start pointer for length calculation
   cui256 zero  = _mm256_setzero_si256();    // Vector of zero bytes for comparison
   ui64   addr  = reinterpret_cast<ui64>(s); // Numeric address of the string pointer

   // Handle initial misalignment: advance pointer byte-by-byte until 32-byte aligned or at page boundary
   if(addr % 32 != 0) {
      ui64 next_aligned     = (addr + 31) & ~ui64(31); // Calculate next 32-byte boundary
      ui64 len_to_alignment = next_aligned - addr;     // Bytes to reach alignment
      ui64 page_end         = (addr | 0xFFF) + 1;      // End address of current 4KB page (page aligned)

      // Ensure not to cross the page boundary while aligning
      if(next_aligned > page_end) len_to_alignment = page_end - addr;  // Adjust length to page boundary if closer

      // Scan bytes one by one in the misaligned prefix for null terminator
      for(size_t i = 0; i < len_to_alignment; ++i)
         if(s[i] == '\0') return i;  // Null found, return length up to this point

      // Advance pointer and address to aligned boundary or page end after scanning
      s    += len_to_alignment;
      addr += len_to_alignment;
      // Now s is either 32-byte aligned or aligned at a page boundary, safe for vector loads
   }

   // Main loop: process 32 bytes at a time using AVX2 intrinsics to find null terminator
   for(;; s += 32) {
      cui256 vec  = _mm256_loadu_si256(reinterpret_cast<cui256ptr>(s)); // Load 32 bytes from the current position (using unaligned load for safety)
      cui256 cmp  = _mm256_cmpeq_epi8(vec, zero);                       // Compare loaded bytes with zero vector byte-wise
      ui32   mask = static_cast<ui32>(_mm256_movemask_epi8(cmp));       // Convert comparison result to a bitmask, each bit corresponds to a byte in vec

      // If any bit is set, a null byte was found in these 32 bytes
      if(mask != 0) {
         // Find index of first set bit to get location of first zero byte
#if __cplusplus >= 202002L
         ui64 offset = std::countr_zero(mask);
#else
         ui64 offset = _tzcnt_u32(mask);
#endif
         return (s - start) + offset; // Return total length by adding offset within current 32-byte block to bytes scanned so far
      }
   }
}