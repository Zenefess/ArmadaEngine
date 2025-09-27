/*
 * File: geometry_1D.h
 *
 * Version: v0.1
 *
 * Owner: David William Bull
 *
 * Created: 2025-09-27
 *
 * Last Modified: 2025-09-27
 *
 * Description: SIMD-accelerated computational geometry helpers for 1D. Includes helpers for Packed AoS vectors.
 *
 * To Do: 1) Wire CPUID dispatch and scalar fallbacks.
 *        2) Add microbenchmarks in docs/.
 *        3) Implement project/intersect/distance helpers.
 *
 * Dependencies: typedefs.h, vector structures.h
 *
 * ISA: Scalar | AVX2
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

#include <cmath>
#include <immintrin.h>
#include <typedefs.h>
#include <vector structures.h>

/// @brief     Length (magnitude) of 1D vector using AVX2.
/// @param a   SIMD vector containing floats.
/// @return    SIMD vector with absolute values of a.
inline fl32x8 Length1D(fl32x8 a) noexcept { return _mm256_and_ps(a, _mm256_castsi256_ps(_mm256_set1_epi32(0x7FFFFFFF))); }

/// @brief     Normalize 1D vector to unit length using AVX2.
/// @param a   SIMD vector containing floats.
/// @return    SIMD vector with normalized values (1.0 if >0, -1.0 if <0, 0.0 if 0).
inline fl32x8 Normalize1D(fl32x8 a) noexcept {
   fl32x8 zero     = _mm256_setzero_ps();
   fl32x8 pos_mask = _mm256_cmp_ps(a, zero, _CMP_GT_OS);
   fl32x8 neg_mask = _mm256_cmp_ps(a, zero, _CMP_LT_OS);
   fl32x8 one      = _mm256_set1_ps(1.0f);
   fl32x8 neg_one  = _mm256_set1_ps(-1.0f);
   fl32x8 pos      = _mm256_and_ps(pos_mask, one);
   fl32x8 neg      = _mm256_and_ps(neg_mask, neg_one);
   return _mm256_or_ps(pos, neg);
}

/// @brief         Length (magnitude) of packed 1D vectors (AoS) using AVX2.
/// @param v       Pointer to the first element in the packed array.
/// @param count   Number of 1D vectors to process (max 8 per call).
/// @return        SIMD vector containing |v[i]| for the requested lanes.
inline fl32x8 __vectorcall Length1D_AoS(cfl32ptr v, cui32 count) noexcept {
   ui256  mask = MaskFromCount8(count);         // Active-lane mask for maskload/maskstore.
   fl32x8 src  = _mm256_maskload_ps(v, mask);   // Load and zero inactive elements.
   ui256  sign = _mm256_set1_epi32(0x7FFFFFFF); // Clear sign bit to compute |x|.
   fl32x8 mag  = _mm256_and_ps(src, _mm256_castsi256_ps(sign));
   return mag;
}

/// @brief         Normalize packed 1D vectors (AoS) to unit magnitude using AVX2.
/// @param v       Pointer to the first element in the packed array.
/// @param count   Number of 1D vectors to process (max 8 per call).
/// @return        SIMD vector with normalized values for the requested lanes.
inline fl32x8 Normalize1D_AoS(cfl32ptr v, cui32 count) noexcept {
   cui256 mask    = MaskFromCount8(count);       // Mask selects valid lanes.
   fl32x8 src     = _mm256_maskload_ps(v, mask); // Load source and zero inactive lanes.
   fl32x8 zero    = _mm256_setzero_ps();
   fl32x8 ones    = _mm256_set1_ps(1.0f);
   fl32x8 negOnes = _mm256_set1_ps(-1.0f);
   fl32x8 pos     = _mm256_and_ps(_mm256_cmp_ps(src, zero, _CMP_GT_OS), ones);
   fl32x8 neg     = _mm256_and_ps(_mm256_cmp_ps(src, zero, _CMP_LT_OS), negOnes);
   fl32x8 norm    = _mm256_or_ps(pos, neg);
   return norm;
}

/// @brief         Lengths of packed 1D vectors (AoS) using AVX2.
///
/// Processes eight scalars per iteration and writes their absolute values to
/// the output array.  Remaining elements (if count is not a multiple of eight)
/// fall back to scalar code.
///
/// @param vIn     Pointer to the source array of 1D vectors.
/// @param vOut    Pointer to the destination array (may alias vIn).
/// @param count   Number of 1D vectors to process.
inline void Length1D_AoS(cfl32ptr vIn, fl32ptr vOut, cui32 count) noexcept {
   // Mask clears the sign bit to obtain absolute values.
   cfl32x8 mask    = _mm256_castsi256_ps(_mm256_set1_epi32(0x7FFFFFFF));
   ui32    i       = 0;
   cui32   vecStep = count & ~(ui32)7;
   for(; i < vecStep; i += 8) {
      cfl32x8 val  = _mm256_loadu_ps(vIn + i); // Load eight scalars.
      fl32x8  absV = _mm256_and_ps(val, mask); // Clear sign bits.
      _mm256_storeu_ps(vOut + i, absV);        // Store absolute values.
   }
   for(; i < count; ++i) {
      cfl32 a = vIn[i];
      vOut[i] = (a >= 0.0f ? a : -a);
   }
}

/// @brief         Normalize packed 1D vectors (AoS) using AVX2+FMA.
///
/// Generates the sign of each scalar (1 if positive, -1 if negative, 0 if
/// zero).  Eight elements are processed per iteration with AVX2 and FMA; the
/// residual tail is handled scalar.
///
/// @param vIn     Pointer to the source array of 1D vectors.
/// @param vOut    Pointer to the destination array (may alias vIn).
/// @param count   Number of 1D vectors to process.
inline void Normalize1D_AoS(cfl32ptr vIn, fl32ptr vOut, cui32 count) noexcept {
   fl32x8 zero     = _mm256_setzero_ps();   // Zero constant for comparisons.
   fl32x8 one      = _mm256_set1_ps(1.0f);  // +1 broadcast.
   fl32x8 negOne   = _mm256_set1_ps(-1.0f); // -1 broadcast.
   ui32    i       = 0;
   cui32   vecStep = count & ~(ui32)7;
   for(; i < vecStep; i += 8) {
      fl32x8 val     = _mm256_loadu_ps(vIn + i);             // Load eight scalars.
      fl32x8 posMask = _mm256_cmp_ps(val, zero, _CMP_GT_OQ); // Mask for positive inputs.
      fl32x8 negMask = _mm256_cmp_ps(val, zero, _CMP_LT_OQ); // Mask for negative inputs.
      fl32x8 pos     = _mm256_and_ps(posMask, one);          // 1 where positive, 0 otherwise.
      fl32x8 neg     = _mm256_and_ps(negMask, negOne);       // -1 where negative, 0 otherwise.
      fl32x8 norm    = _mm256_fmadd_ps(pos, one, neg);       // Combine signs using FMA.
      _mm256_storeu_ps(vOut + i, norm);                      // Store normalized results.
   }
   for(; i < count; ++i) {
      cfl32 a = vIn[i];
      if(a > 0.0f)      vOut[i] = 1.0f;
      else if(a < 0.0f) vOut[i] = -1.0f;
      else              vOut[i] = 0.0f;
   }
}
