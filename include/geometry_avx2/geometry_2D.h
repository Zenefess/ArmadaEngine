/*
 * File: geometry_2D.h
 *
 * Version: v0.1
 *
 * Owner: David William Bull
 *
 * Created: 2025-09-27
 *
 * Last Modified: 2025-09-27
 *
 * Description: SIMD-accelerated computational geometry helpers for 2D. Includes helpers for Packed AoS vectors.
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


 
//== Packed 2D SIMD Helpers (AVX2)

/// @brief         Add packed 2D vectors (AoS) using AVX2.
///
/// Each vector is stored as {x,y} pairs in an array-of-structures layout: [x0, y0, x1, y1, x2, y2, x3, y3, ...]. This routine processes four pairs
/// at a time using a single 256-bit register.  Remaining pairs (if count is not a multiple of four) are handled with scalar code.
///
/// @param a       Pointer to the first source array of packed 2D floats.
/// @param b       Pointer to the second source array of packed 2D floats.
/// @param out     Pointer to the destination array (may alias a or b).
/// @param count   Number of 2D vectors to process.
inline void Add2D_AoS(cfl32ptr a, cfl32ptr b, fl32ptr out, cui32 count) noexcept {
   ui32  i         = 0;
   cui32 quadCount = count & ~(ui32)3;
   for(; i < quadCount; i += 4) {
      cfl32x8 av = _mm256_loadu_ps(a + (i << 1));
      cfl32x8 bv = _mm256_loadu_ps(b + (i << 1));
      cfl32x8 rv = _mm256_add_ps(av, bv);
      _mm256_storeu_ps(out + (i << 1), rv);
   }
   for(; i < count; ++i) {
      out[(i << 1) + 0] = a[(i << 1) + 0] + b[(i << 1) + 0];
      out[(i << 1) + 1] = a[(i << 1) + 1] + b[(i << 1) + 1];
   }
}

/// @brief         Subtract packed 2D vectors (AoS) using AVX2.
///
/// Each element of out is set to a[i] - b[i], where i indexes 2D vector pairs. See Add2D_AoS for layout details.
///
/// @param a       Pointer to the first source array of packed 2D floats.
/// @param b       Pointer to the second source array of packed 2D floats.
/// @param out     Pointer to the destination array (may alias a or b).
/// @param count   Number of 2D vectors to process.
inline void Sub2D_AoS(cfl32ptr a, cfl32ptr b, fl32ptr out, cui32 count) noexcept {
   ui32  i         = 0;
   cui32 quadCount = count & ~(ui32)3;
   for(; i < quadCount; i += 4) {
      cfl32x8 av = _mm256_loadu_ps(a + (i << 1));
      cfl32x8 bv = _mm256_loadu_ps(b + (i << 1));
      cfl32x8 rv = _mm256_sub_ps(av, bv);
      _mm256_storeu_ps(out + (i << 1), rv);
   }
   for(; i < count; ++i) {
      out[(i << 1) + 0] = a[(i << 1) + 0] - b[(i << 1) + 0];
      out[(i << 1) + 1] = a[(i << 1) + 1] - b[(i << 1) + 1];
   }
}

/// @brief         Compute dot products of packed 2D vectors (AoS) using AVX2.
///
/// Computes the dot product for each 2D pair: dot[i] = a_x[i] * b_x[i] + a_y[i] * b_y[i]. Four pairs are processed per iteration using horizontal
/// addition. The result array holds one float per 2D vector.
///
/// @param a       Pointer to the first source array of packed 2D floats.
/// @param b       Pointer to the second source array of packed 2D floats.
/// @param out     Pointer to the destination array of dot products.
/// @param count   Number of 2D vectors to process.
inline void Dot2D_AoS(cfl32ptr a, cfl32ptr b, fl32ptr out, cui32 count) noexcept {
   ui32  i         = 0;
   cui32 quadCount = count & ~(ui32)3;
   for(; i < quadCount; i += 4) {
      cfl32x8 av   = _mm256_loadu_ps(a + (i << 1));
      cfl32x8 bv   = _mm256_loadu_ps(b + (i << 1));
      cfl32x8 prod = _mm256_mul_ps(av, bv);
      cfl32x8 sum  = _mm256_hadd_ps(prod, prod);
      cfl32x4 low  = _mm256_castps256_ps128(sum);
      _mm_storeu_ps(out + i, low);
   }
   for(; i < count; ++i)
      out[i] = a[(i << 1) + 0] * b[(i << 1) + 0] + a[(i << 1) + 1] * b[(i << 1) + 1];
}

/// @brief         Compute 2D scalar cross product (z-component) per packed vector using AVX2.
///
/// For each pair i, returns a_x[i] * b_y[i] - a_y[i] * b_x[i]. Four pairs are processed simultaneously. The result array holds one float per 2D
/// vector.
///
/// @param a       Pointer to the first source array of packed 2D floats.
/// @param b       Pointer to the second source array of packed 2D floats.
/// @param out     Pointer to the destination array of cross products.
/// @param count   Number of 2D vectors to process.
inline void Cross2D_AoS(cfl32ptr a, cfl32ptr b, fl32ptr out, cui32 count) noexcept {
   // Permutation to swap x/y within each pair: [1,0,3,2,5,4,7,6]
   cui256 SWAP_IDX  = _mm256_setr_epi32(1,0,3,2,5,4,7,6);
   ui32   i         = 0;
   cui32  quadCount = count & ~(ui32)3;
   for(; i < quadCount; i += 4) {
      fl32x8 av    = _mm256_loadu_ps(a + (i << 1));
      fl32x8 bv    = _mm256_loadu_ps(b + (i << 1));
      fl32x8 bswap = _mm256_permutevar8x32_ps(bv, SWAP_IDX);
      fl32x8 prod  = _mm256_mul_ps(av, bswap);
      fl32x8 diff  = _mm256_hsub_ps(prod, prod);
      fl32x4 low   = _mm256_castps256_ps128(diff);
      _mm_storeu_ps(out + i, low);
   }
   for(; i < count; ++i) { out[i] = a[(i << 1) + 0] * b[(i << 1) + 1] - a[(i << 1) + 1] * b[(i << 1) + 0]; }
}

/// @brief         Compute lengths of packed 2D vectors (AoS) using AVX2.
///
/// Each result is the Euclidean length sqrt(x^2 + y^2) for one 2D pair. Four pairs are handled per iteration.  Results are stored sequentially in
/// the output array.
///
/// @param v       Pointer to the source array of packed 2D floats.
/// @param out     Pointer to the destination array of lengths.
/// @param count   Number of 2D vectors to process.
inline void Length2DAoSAVX2(cfl32ptr v, fl32ptr out, cui32 count) noexcept {
   ui32  i         = 0;
   cui32 quadCount = count & ~(ui32)3;
   for(; i < quadCount; i += 4) {
      fl32x8 vv  = _mm256_loadu_ps(v + (i << 1));
      fl32x8 sq  = _mm256_mul_ps(vv, vv);
      fl32x8 sum = _mm256_hadd_ps(sq, sq);
      fl32x8 len = _mm256_sqrt_ps(sum);
      fl32x4 low = _mm256_castps256_ps128(len);
      _mm_storeu_ps(out + i, low);
   }
   for(; i < count; ++i) {
      cfl32 x = v[(i << 1) + 0];
      cfl32 y = v[(i << 1) + 1];
      out[i] = static_cast<fl32>(sqrt(static_cast<fl64>(x * x + y * y)));
   }
}

/// @brief         Normalize packed 2D vectors (AoS) in place using AVX2+FMA.
///
/// Each 2D vector is scaled to unit length. Zero-length vectors remain {0,0}. The normalization uses a single iteration of Newton–Raphson refinement
/// on the reciprocal square root to improve accuracy.  Four pairs are processed at a time.
///
/// @param v       Pointer to the array of packed 2D floats to normalize in place.
/// @param count   Number of 2D vectors to process.
inline void Normalize2D_AoS(fl32ptr v, cui32 count) noexcept {
   cfl32x8 vEps      = _mm256_set1_ps(aeEpsilon);
   cui256  REPL_IDX  = _mm256_setr_epi32(0,0,1,1,2,2,3,3);
   ui32    i         = 0;
   cui32   quadCount = count & ~(ui32)3;
   for(; i < quadCount; i += 4) {
      fl32x8 vv     = _mm256_loadu_ps(v + (i << 1));
      fl32x8 sq     = _mm256_mul_ps(vv, vv);
      fl32x8 sum    = _mm256_hadd_ps(sq, sq);      // [len2_0,len2_1,len2_2,len2_3]*2
      fl32x4 len4   = _mm256_castps256_ps128(sum); // lower 4 values
      // Approximate reciprocal sqrt for four lengths
      fl32x4 inv4   = _mm_rsqrt_ps(len4);
      // Refine using Newton–Raphson: inv = inv * (1.5 - 0.5 * len * inv^2)
      fl32x4 inv4Sq = _mm_mul_ps(inv4, inv4);
      fl32x4 half   = _mm_set1_ps(0.5f);
      fl32x4 three  = _mm_set1_ps(1.5f);
      fl32x4 factor = _mm_sub_ps(three, _mm_mul_ps(_mm_mul_ps(len4, inv4Sq), half));
      inv4 = _mm_mul_ps(inv4, factor);
      // Broadcast to 256-bit and replicate per pair
      fl32x8 inv8  = _mm256_insertf128_ps(_mm256_castps128_ps256(inv4), inv4, 1);
      inv8 = _mm256_permutevar8x32_ps(inv8, REPL_IDX);
      // Replicate len2 as well to build mask
      fl32x8 len8  = _mm256_insertf128_ps(_mm256_castps128_ps256(len4), len4, 1);
      len8 = _mm256_permutevar8x32_ps(len8, REPL_IDX);
      // Multiply to normalize
      fl32x8 nv    = _mm256_mul_ps(vv, inv8);
      // Zero vectors with len^2 <= aeEpsilon
      fl32x8 mask  = _mm256_cmp_ps(len8, vEps, _CMP_GT_OQ);
      fl32x8 outv  = _mm256_and_ps(nv, mask);
      _mm256_storeu_ps(v + (i << 1), outv);
   }
   for(; i < count; ++i) {
      fl32 x  = v[(i << 1) + 0];
      fl32 y  = v[(i << 1) + 1];
      fl32 len2 = x * x + y * y;
      if(len2 > aeEpsilon) {
         fl32 inv = 1.0f / static_cast<fl32>(std::sqrt(static_cast<fl64>(len2)));
         v[(i << 1) + 0] = x * inv;
         v[(i << 1) + 1] = y * inv;
      } else {
         v[(i << 1) + 0] = 0.0f;
         v[(i << 1) + 1] = 0.0f;
      }
   }
}
