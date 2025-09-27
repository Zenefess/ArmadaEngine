/*
 * File: geometry_func_avx2.h
 *
 * Version: v0.1
 *
 * Owner: David William Bull
 *
 * Created: 2025-09-26
 *
 * Last Modified: 2025-09-26
 *
 * Description: SIMD-accelerated computational geometry helpers for 1D~4D. Includes helpers for Packed AoS vectors.
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
#include "typedefs.h"
#include "vector structures.h"

constexpr cfl32 aeEpsilon = 1.0e-20f;

//== Non-SIMD functions ==//

/// @brief     Length (magnitude) of a 1D vector.
/// @param a   The value.
/// @return    |a| (absolute value of a).
inline constexpr fl32 Length1D(cfl32 a) noexcept { return (a >= 0.0f ? a : -a); } // Use fabsf from <cmath> or similar, or multiply by sign.

/// @brief     Normalize a 1D vector to unit length.
/// @param a   The value to normalize.
/// @return    1.0 if a > 0, -1.0 if a < 0, or 0.0 if a == 0.
inline constexpr fl32 Normalize1D(cfl32 a) noexcept { if(a > 0.0f) return 1.0f;   if(a < 0.0f) return -1.0f;   return 0.0f; }

//== AVX2-optimized functions ==//

/// @brief     Length (magnitude) of 1D vector using AVX2.
/// @param a   SIMD vector containing floats.
/// @return    SIMD vector with absolute values of a.
inline constexpr fl32x8 Length1D(fl32x8 a) noexcept { return _mm256_and_ps(a, _mm256_castsi256_ps(_mm256_set1_epi32(0x7FFFFFFF))); }

/// @brief     Normalize 1D vector to unit length using AVX2.
/// @param a   SIMD vector containing floats.
/// @return    SIMD vector with normalized values (1.0 if >0, -1.0 if <0, 0.0 if 0).
inline constexpr fl32x8 Normalize1D(fl32x8 a) noexcept {
   fl32x8 zero     = _mm256_setzero_ps();
   fl32x8 pos_mask = _mm256_cmp_ps(a, zero, _CMP_GT_OS);
   fl32x8 neg_mask = _mm256_cmp_ps(a, zero, _CMP_LT_OS);
   fl32x8 one      = _mm256_set1_ps(1.0f);
   fl32x8 neg_one  = _mm256_set1_ps(-1.0f);
   fl32x8 pos      = _mm256_and_ps(pos_mask, one);
   fl32x8 neg      = _mm256_and_ps(neg_mask, neg_one);
   return _mm256_or_ps(pos, neg);
}

//== Packed 2D SIMD Helpers (AVX2)

/// @brief Add packed 2D vectors (AoS) using AVX2.
///
/// Each vector is stored as {x,y} pairs in an array-of-structures layout:
/// [x0, y0, x1, y1, x2, y2, x3, y3, ...].  This routine processes four pairs
/// at a time using a single 256-bit register.  Remaining pairs (if count is
/// not a multiple of four) are handled with scalar code.
///
/// @param a   Pointer to the first source array of packed 2D floats.
/// @param b   Pointer to the second source array of packed 2D floats.
/// @param out Pointer to the destination array (may alias a or b).
/// @param count Number of 2D vectors to process.
inline constexpr void Add2D_AoS(cfl32ptr a, cfl32ptr b, fl32ptr out, cui32 count) noexcept {
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

/// @brief Subtract packed 2D vectors (AoS) using AVX2.
///
/// Each element of out is set to a[i] - b[i], where i indexes 2D vector pairs.
/// See Add2D_AoS_AVX2 for layout details.
///
/// @param a       Pointer to the first source array of packed 2D floats.
/// @param b       Pointer to the second source array of packed 2D floats.
/// @param out     Pointer to the destination array (may alias a or b).
/// @param count   Number of 2D vectors to process.
inline constexpr void Sub2D_AoS(cfl32ptr a, cfl32ptr b, fl32ptr out, cui32 count) noexcept {
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

/// @brief Compute dot products of packed 2D vectors (AoS) using AVX2.
///
/// Computes the dot product for each 2D pair: dot[i] = a_x[i] * b_x[i] + a_y[i] * b_y[i].
/// Four pairs are processed per iteration using horizontal addition.  The result
/// array holds one float per 2D vector.
///
/// @param a       Pointer to the first source array of packed 2D floats.
/// @param b       Pointer to the second source array of packed 2D floats.
/// @param out     Pointer to the destination array of dot products.
/// @param count   Number of 2D vectors to process.
inline constexpr void Dot2D_AoS(cfl32ptr a, cfl32ptr b, fl32ptr out, cui32 count) noexcept {
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

/// @brief Compute 2D scalar cross product (z-component) per packed vector using AVX2.
///
/// For each pair i, returns a_x[i] * b_y[i] - a_y[i] * b_x[i].  Four pairs are processed
/// simultaneously.  The result array holds one float per 2D vector.
///
/// @param a       Pointer to the first source array of packed 2D floats.
/// @param b       Pointer to the second source array of packed 2D floats.
/// @param out     Pointer to the destination array of cross products.
/// @param count   Number of 2D vectors to process.
inline constexpr void Cross2D_AoS(cfl32ptr a, cfl32ptr b, fl32ptr out, cui32 count) noexcept {
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

/// @brief Compute lengths of packed 2D vectors (AoS) using AVX2.
///
/// Each result is the Euclidean length sqrt(x^2 + y^2) for one 2D pair.  Four pairs
/// are handled per iteration.  Results are stored sequentially in the output array.
///
/// @param v       Pointer to the source array of packed 2D floats.
/// @param out     Pointer to the destination array of lengths.
/// @param count   Number of 2D vectors to process.
inline constexpr void Length2DAoSAVX2(cfl32ptr v, fl32ptr out, cui32 count) noexcept {
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

/// @brief Normalize packed 2D vectors (AoS) in place using AVX2+FMA.
///
/// Each 2D vector is scaled to unit length.  Zero-length vectors remain {0,0}.
/// The normalization uses a single iteration of Newton–Raphson refinement on the
/// reciprocal square root to improve accuracy.  Four pairs are processed at a time.
///
/// @param v       Pointer to the array of packed 2D floats to normalize in place.
/// @param count   Number of 2D vectors to process.
inline constexpr void Normalize2D_AoS(fl32ptr v, cui32 count) noexcept {
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

//== Packed 3D SIMD Helpers (AVX2)

/// @brief Add packed 3D vectors (AoS) using AVX2.
///
/// Each vector is stored as {x,y,z} triplets in an array-of-structures layout:
/// [x0, y0, z0, x1, y1, z1, ...].  This routine processes two triplets
/// at a time using a 256-bit SIMD register with mask loads/stores.  Remaining
/// triplets (if count is odd) are handled with scalar code.
///
/// @param a       Pointer to the first source array of packed 3D floats.
/// @param b       Pointer to the second source array of packed 3D floats.
/// @param out     Pointer to the destination array (may alias a or b).
/// @param count   Number of 3D vectors to process.
inline constexpr void Add3D_AoS(cfl32ptr a, cfl32ptr b, fl32ptr out, cui32 count) noexcept {
   ui32   i         = 0;
   // Number of pairs of 3D vectors (two per iteration)
   cui32  pairCount = count & ~(ui32)1;
   // Mask to load/store two 3D vectors (x,y,z,0, x,y,z,0)
   cui256 MASKLOAD  = _mm256_setr_epi32(-1, -1, -1, 0, -1, -1, -1, 0);
   for(; i < pairCount; i += 2) {
      ui32   idx = i * 3;
      fl32x8 av  = _mm256_maskload_ps(a + idx, MASKLOAD);
      fl32x8 bv  = _mm256_maskload_ps(b + idx, MASKLOAD);
      fl32x8 sum = _mm256_add_ps(av, bv);
      _mm256_maskstore_ps(out + idx, MASKLOAD, sum);
   }
   // Handle leftover vector (if count is odd)
   for(; i < count; ++i) {
      ui32 idx = i * 3;
      out[idx + 0] = a[idx + 0] + b[idx + 0];
      out[idx + 1] = a[idx + 1] + b[idx + 1];
      out[idx + 2] = a[idx + 2] + b[idx + 2];
   }
}

/// @brief Subtract packed 3D vectors (AoS) using AVX2.
///
/// Each element of out is set to a[i] - b[i], where i indexes 3D triplets.
/// See Add3DAoSAVX2 for layout details.
///
/// @param a   Pointer to the first source array of packed 3D floats.
/// @param b   Pointer to the second source array of packed 3D floats.
/// @param out Pointer to the destination array (may alias a or b).
/// @param count Number of 3D vectors to process.
inline constexpr void Sub3D_AoS(cfl32ptr a, cfl32ptr b, fl32ptr out, cui32 count) noexcept {
   ui32   i         = 0;
   cui32  pairCount = count & ~(ui32)1;
   cui256 MASKLOAD  = _mm256_setr_epi32(-1, -1, -1, 0, -1, -1, -1, 0);
   for(; i < pairCount; i += 2) {
      ui32   idx  = i * 3;
      fl32x8 av   = _mm256_maskload_ps(a + idx, MASKLOAD);
      fl32x8 bv   = _mm256_maskload_ps(b + idx, MASKLOAD);
      fl32x8 diff = _mm256_sub_ps(av, bv);
      _mm256_maskstore_ps(out + idx, MASKLOAD, diff);
   }
   for(; i < count; ++i) {
      ui32 idx = i * 3;
      out[idx + 0] = a[idx + 0] - b[idx + 0];
      out[idx + 1] = a[idx + 1] - b[idx + 1];
      out[idx + 2] = a[idx + 2] - b[idx + 2];
   }
}


/// @brief Dot product of packed 3D vectors (AoS) using AVX2.
///
/// Computes the dot product for each 3D triplet: dot[i] = a_x[i] * b_x[i] +
/// a_y[i] * b_y[i] + a_z[i] * b_z[i].  Two triplets are processed per
/// iteration.  The result array holds one float per 3D vector.
///
/// @param a       Pointer to the first source array of packed 3D floats.
/// @param b       Pointer to the second source array of packed 3D floats.
/// @param out     Pointer to the destination array of dot products.
/// @param count   Number of 3D vectors to process.
inline constexpr void Dot3D_AoS(cfl32ptr a, cfl32ptr b, fl32ptr out, cui32 count) noexcept {
   ui32   i         = 0;
   cui32  pairCount = count & ~(ui32)1;
   cui256 MASKLOAD  = _mm256_setr_epi32(-1, -1, -1, 0, -1, -1, -1, 0);
   for(; i < pairCount; i += 2) {
      ui32   idx  = i * 3;
      fl32x8 av   = _mm256_maskload_ps(a + idx, MASKLOAD);
      fl32x8 bv   = _mm256_maskload_ps(b + idx, MASKLOAD);
      fl32x8 prod = _mm256_mul_ps(av, bv);
      // Horizontal add to sum pairs of components
      fl32x8 sum  = _mm256_hadd_ps(prod, prod);
      fl32x4 low  = _mm256_castps256_ps128(sum);
      // low: [x0+y0, z0, x1+y1, z1]
      fl32x4 dp  = _mm_hadd_ps(low, low);
      _mm_storel_pi((__m64 *)(out + i), dp);
   }
   for(; i < count; ++i) {
      ui32 idx = i * 3;
      out[i] = a[idx + 0] * b[idx + 0] + a[idx + 1] * b[idx + 1] + a[idx + 2] * b[idx + 2];
   }
}

/// @brief Compute 3D cross product (AoS) per packed vector using AVX2.
///
/// For each triplet i, returns a vector
/// (a_y[i]*b_z[i] - a_z[i]*b_y[i],
///  a_z[i]*b_x[i] - a_x[i]*b_z[i],
///  a_x[i]*b_y[i] - a_y[i]*b_x[i]).
/// Two triplets are processed simultaneously.  The result array holds
/// three floats per 3D vector.
///
/// @param a    Pointer to the first source array of packed 3D floats.
/// @param b    Pointer to the second source array of packed 3D floats.
/// @param out  Pointer to the destination array of cross products.
/// @param count Number of 3D vectors to process.
inline constexpr void Cross3D_AoS(cfl32ptr a, cfl32ptr b, fl32ptr out, cui32 count) noexcept {
   ui32   i         = 0;
   cui32  pairCount = count & ~(ui32)1;
   cui256 MASKLOAD  = _mm256_setr_epi32(-1, -1, -1, 0, -1, -1, -1, 0);
   // Permutation indices to rotate (y,z,x) and (z,x,y) within each pair
   cui256 IDX1      = _mm256_setr_epi32(1, 2, 0, 3, 5, 6, 4, 7);
   cui256 IDX2      = _mm256_setr_epi32(2, 0, 1, 3, 6, 4, 5, 7);
   for(; i < pairCount; i += 2) {
      ui32   idx   = i * 3;
      fl32x8 av    = _mm256_maskload_ps(a + idx, MASKLOAD);
      fl32x8 bv    = _mm256_maskload_ps(b + idx, MASKLOAD);
      fl32x8 a1    = _mm256_permutevar8x32_ps(av, IDX1);
      fl32x8 b1    = _mm256_permutevar8x32_ps(bv, IDX2);
      fl32x8 a2    = _mm256_permutevar8x32_ps(av, IDX2);
      fl32x8 b2    = _mm256_permutevar8x32_ps(bv, IDX1);
      // Compute cross = a1*b1 - a2*b2 using fused multiply-subtract
      fl32x8 cross = _mm256_fmsub_ps(a1, b1, _mm256_mul_ps(a2, b2));
      _mm256_maskstore_ps(out + idx, MASKLOAD, cross);
   }
   for(; i < count; ++i) {
      ui32 idx = i * 3;
      fl32 ax  = a[idx + 0];
      fl32 ay  = a[idx + 1];
      fl32 az  = a[idx + 2];
      fl32 bx  = b[idx + 0];
      fl32 by  = b[idx + 1];
      fl32 bz  = b[idx + 2];
      out[idx + 0] = ay * bz - az * by;
      out[idx + 1] = az * bx - ax * bz;
      out[idx + 2] = ax * by - ay * bx;
   }
}

/// @brief Compute lengths of packed 3D vectors (AoS) using AVX2.
///
/// Each result is the Euclidean length sqrt(x^2 + y^2 + z^2) for one 3D triplet.
/// Two triplets are handled per iteration.  Results are stored sequentially
/// in the output array.
///
/// @param v    Pointer to the source array of packed 3D floats.
/// @param out  Pointer to the destination array of lengths.
/// @param count Number of 3D vectors to process.
inline constexpr void Length3D_AoS(cfl32ptr v, fl32ptr out, cui32 count) noexcept {
   ui32   i         = 0;
   cui32  pairCount = count & ~(ui32)1;
   cui256 MASKLOAD  = _mm256_setr_epi32(-1, -1, -1, 0, -1, -1, -1, 0);
   for(; i < pairCount; i += 2) {
      ui32   idx = i * 3;
      fl32x8 av  = _mm256_maskload_ps(v + idx, MASKLOAD);
      fl32x8 sq  = _mm256_mul_ps(av, av);
      fl32x8 sum = _mm256_hadd_ps(sq, sq);
      fl32x4 low = _mm256_castps256_ps128(sum);
      fl32x4 dp  = _mm_hadd_ps(low, low);
      fl32x4 len = _mm_sqrt_ps(dp);
      _mm_storel_pi((__m64 *)(out + i), len);
   }
   for(; i < count; ++i) {
      ui32 idx = i * 3;
      fl32 x   = v[idx + 0];
      fl32 y   = v[idx + 1];
      fl32 z   = v[idx + 2];
      out[i] = static_cast<fl32>(std::sqrt(static_cast<fl64>(x * x + y * y + z * z)));
   }
}

/// @brief Normalize packed 3D vectors (AoS) in place using AVX2.
///
/// Each 3D vector is scaled to unit length.  Zero-length vectors remain {0,0,0}.
/// The normalization computes two triplets per iteration, computing lengths
/// and reciprocals on 128-bit halves and broadcasting them across each triplet.
///
/// @param v    Pointer to the array of packed 3D floats to normalize in place.
/// @param count Number of 3D vectors to process.
inline constexpr void Normalize3D_AoS(fl32ptr v, cui32 count) noexcept {
   cui256 MASKLOAD  = _mm256_setr_epi32(-1, -1, -1, 0, -1, -1, -1, 0);
   ui32   i         = 0;
   cui32  pairCount = count & ~(ui32)1;
   for(; i < pairCount; i += 2) {
      ui32   idx    = i * 3;
      fl32x8 av     = _mm256_maskload_ps(v + idx, MASKLOAD);
      // Compute squared lengths for each triplet
      fl32x8 sq     = _mm256_mul_ps(av, av);
      fl32x8 sum    = _mm256_hadd_ps(sq, sq);
      fl32x4 low    = _mm256_castps256_ps128(sum);
      fl32x4 dp     = _mm_hadd_ps(low, low);
      // Compute square roots (lengths)
      fl32x4 len    = _mm_sqrt_ps(dp);
      // Extract lengths for each half
      fl32 len0     = _mm_cvtss_f32(len);
      fl32 len1     = _mm_cvtss_f32(_mm_shuffle_ps(len, len, _MM_SHUFFLE(0, 0, 0, 1)));
      // Compute inverse lengths with zero-check
      fl32 inv0     = (len0 > aeEpsilon) ? (1.0f / len0) : 0.0f;
      fl32 inv1     = (len1 > aeEpsilon) ? (1.0f / len1) : 0.0f;
      // Build broadcast vectors [inv0, inv0, inv0, 0] and [inv1, inv1, inv1, 0]
      fl32x4 invLo  = _mm_set_ps(0.0f, inv0, inv0, inv0);
      fl32x4 invHi  = _mm_set_ps(0.0f, inv1, inv1, inv1);
      fl32x8 invVec = _mm256_insertf128_ps(_mm256_castps128_ps256(invLo), invHi, 1);
      // Multiply by inverse lengths
      fl32x8 nv     = _mm256_mul_ps(av, invVec);
      // Store normalized vectors
      _mm256_maskstore_ps(v + idx, MASKLOAD, nv);
   }
   for(; i < count; ++i) {
      ui32 idx = i * 3;
      fl32 x = v[idx + 0];
      fl32 y = v[idx + 1];
      fl32 z = v[idx + 2];
      fl32 len2 = x * x + y * y + z * z;
      if(len2 > aeEpsilon * aeEpsilon) {
         fl32 invLen = 1.0f / static_cast<fl32>(std::sqrt(static_cast<fl64>(len2)));
         v[idx + 0] = x * invLen;
         v[idx + 1] = y * invLen;
         v[idx + 2] = z * invLen;
      } else {
         v[idx + 0] = 0.0f;
         v[idx + 1] = 0.0f;
         v[idx + 2] = 0.0f;
      }
   }
}
                                                
//== Packed 4D SIMD Helpers (AVX2)

/// @brief Add packed 4D vectors (AoS) using AVX2.
///
/// Each vector is stored as {x,y,z,w} in an array-of-structures layout:
/// [x0, y0, z0, w0, x1, y1, z1, w1, ...].  Two 4D vectors are processed
/// per iteration using a 256-bit SIMD register.  Any remaining single
/// vector is handled with scalar code.
///
/// @param a    Pointer to the first source array of packed 4D floats.
/// @param b    Pointer to the second source array of packed 4D floats.
/// @param out  Pointer to the destination array (may alias a or b).
/// @param count Number of 4D vectors to process.
inline void Add4D_AoS(cfl32ptr a, cfl32ptr b, fl32ptr out, cui32 count) {
   ui32  i         = 0;
   // Process pairs of vectors (two per iteration)
   cui32 pairCount = count & ~(ui32)1;
   for(; i < pairCount; i += 2) {
      ui32   idx = i << 2; // i * 4
      fl32x8 av  = _mm256_loadu_ps(a + idx);
      fl32x8 bv  = _mm256_loadu_ps(b + idx);
      fl32x8 sum = _mm256_add_ps(av, bv);
      _mm256_storeu_ps(out + idx, sum);
   }
   // Handle any trailing vector
   for(; i < count; ++i) {
      ui32 idx = i << 2;
      out[idx + 0] = a[idx + 0] + b[idx + 0];
      out[idx + 1] = a[idx + 1] + b[idx + 1];
      out[idx + 2] = a[idx + 2] + b[idx + 2];
      out[idx + 3] = a[idx + 3] + b[idx + 3];
   }
}

/// @brief Subtract packed 4D vectors (AoS) using AVX2.
///
/// For each 4D vector i: out[i] = a[i] - b[i].
///
/// @param a    Pointer to the first source array of packed 4D floats.
/// @param b    Pointer to the second source array of packed 4D floats.
/// @param out  Pointer to the destination array (may alias a or b).
/// @param count Number of 4D vectors to process.
inline void Sub4D_AoS(cfl32ptr a, cfl32ptr b, fl32ptr out, cui32 count) {
   ui32  i         = 0;
   cui32 pairCount = count & ~(ui32)1;
   for(; i < pairCount; i += 2) {
      ui32 idx = i << 2;
      fl32x8 av = _mm256_loadu_ps(a + idx);
      fl32x8 bv = _mm256_loadu_ps(b + idx);
      fl32x8 diff = _mm256_sub_ps(av, bv);
      _mm256_storeu_ps(out + idx, diff);
   }
   for(; i < count; ++i) {
      ui32 idx = i << 2;
      out[idx + 0] = a[idx + 0] - b[idx + 0];
      out[idx + 1] = a[idx + 1] - b[idx + 1];
      out[idx + 2] = a[idx + 2] - b[idx + 2];
      out[idx + 3] = a[idx + 3] - b[idx + 3];
   }
}

/// @brief Dot product of packed 4D vectors (AoS) using AVX2.
///
/// Computes dot[i] = a_x[i] * b_x[i] + a_y[i] * b_y[i] + a_z[i] * b_z[i] +
/// a_w[i] * b_w[i] for each 4D vector.  Two dot products are computed
/// simultaneously.  Results are stored sequentially in the out array.
///
/// @param a     Pointer to the first source array of packed 4D floats.
/// @param b     Pointer to the second source array of packed 4D floats.
/// @param out   Pointer to the destination array of dot products.
/// @param count Number of 4D vectors to process.
inline void Dot4D_AoS(cfl32ptr a, cfl32ptr b, fl32ptr out, cui32 count) {
   ui32  i         = 0;
   cui32 pairCount = count & ~(ui32)1;
   for(; i < pairCount; i += 2) {
      ui32   idx  = i << 2;
      fl32x8 av   = _mm256_loadu_ps(a + idx);
      fl32x8 bv   = _mm256_loadu_ps(b + idx);
      fl32x8 prod = _mm256_mul_ps(av, bv);
      // Sum components in each 4D vector via horizontal adds
      fl32x8 t0   = _mm256_hadd_ps(prod, prod);
      fl32x8 t1   = _mm256_hadd_ps(t0, t0);
      fl32x4 lo   = _mm256_castps256_ps128(t1);
      // lo = [dot0, dot1, dot0, dot1]; store first two
      _mm_storel_pi((__m64 *)(out + i), lo);
   }
   for(; i < count; ++i) {
      ui32 idx = i << 2;
      out[i] = a[idx + 0] * b[idx + 0] + a[idx + 1] * b[idx + 1] + a[idx + 2] * b[idx + 2] + a[idx + 3] * b[idx + 3];
   }
}

/// @brief Compute lengths of packed 4D vectors (AoS) using AVX2.
///
/// Each result is the Euclidean length sqrt(x^2 + y^2 + z^2 + w^2) for one
/// 4D vector.  Two lengths are computed per iteration.  Results are
/// stored sequentially in the out array.
///
/// @param v     Pointer to the source array of packed 4D floats.
/// @param out   Pointer to the destination array of lengths.
/// @param count Number of 4D vectors to process.
inline void Length4D_AoS(cfl32ptr v, fl32ptr out, cui32 count) {
   ui32  i         = 0;
   cui32 pairCount = count & ~(ui32)1;
   for(; i < pairCount; i += 2) {
      ui32   idx = i << 2;
      fl32x8 av  = _mm256_loadu_ps(v + idx);
      fl32x8 sq  = _mm256_mul_ps(av, av);
      fl32x8 t0  = _mm256_hadd_ps(sq, sq);
      fl32x8 t1  = _mm256_hadd_ps(t0, t0);
      fl32x4 lo  = _mm256_castps256_ps128(t1);
      fl32x4 len = _mm_sqrt_ps(lo);
      _mm_storel_pi((__m64 *)(out + i), len);
   }
   for(; i < count; ++i) {
      ui32 idx = i << 2;
      fl64 sum = static_cast<fl64>(v[idx + 0]) * v[idx + 0] + static_cast<fl64>(v[idx + 1]) * v[idx + 1]
               + static_cast<fl64>(v[idx + 2]) * v[idx + 2] + static_cast<fl64>(v[idx + 3]) * v[idx + 3];
      out[i] = static_cast<fl32>(std::sqrt(sum));
   }
}

/// @brief Normalize packed 4D vectors (AoS) in place using AVX2.
///
/// Each 4D vector is scaled to unit length.  Zero-length vectors remain
/// {0,0,0,0}.  Two vectors are processed in each iteration.  The input
/// and output buffers must be identical (in place).
///
/// @param v     Pointer to the array of packed 4D floats to normalize.
/// @param count Number of 4D vectors to process.
inline void Normalize4D_AoS(fl32ptr v, cui32 count) {
   ui32  i         = 0;
   cui32 pairCount = count & ~(ui32)1;
   for(; i < pairCount; i += 2) {
      ui32   idx    = i << 2;
      fl32x8 av     = _mm256_loadu_ps(v + idx);
      fl32x8 sq     = _mm256_mul_ps(av, av);
      fl32x8 t0     = _mm256_hadd_ps(sq, sq);
      fl32x8 t1     = _mm256_hadd_ps(t0, t0);
      fl32x4 lo     = _mm256_castps256_ps128(t1);
      fl32x4 len    = _mm_sqrt_ps(lo);
      // Extract lengths for two vectors
      fl32   len0   = _mm_cvtss_f32(len);
      fl32   len1   = _mm_cvtss_f32(_mm_shuffle_ps(len, len, _MM_SHUFFLE(0, 0, 0, 1)));
      // Compute inverse lengths with zero-check
      fl32   inv0   = (len0 > aeEpsilon) ? (1.0f / len0) : 0.0f;
      fl32   inv1   = (len1 > aeEpsilon) ? (1.0f / len1) : 0.0f;
      // Broadcast inverses to each 4D segment
      fl32x4 invLo  = _mm_set_ps(inv0, inv0, inv0, inv0);
      fl32x4 invHi  = _mm_set_ps(inv1, inv1, inv1, inv1);
      fl32x8 invVec = _mm256_insertf128_ps(_mm256_castps128_ps256(invLo), invHi, 1);
      fl32x8 nv     = _mm256_mul_ps(av, invVec);
      _mm256_storeu_ps(v + idx, nv);
   }
   for(; i < count; ++i) {
      ui32 idx = i << 2;
      fl64 sum = static_cast<fl64>(v[idx + 0]) * v[idx + 0] + static_cast<fl64>(v[idx + 1]) * v[idx + 1]
               + static_cast<fl64>(v[idx + 2]) * v[idx + 2] + static_cast<fl64>(v[idx + 3]) * v[idx + 3];
      if(sum > static_cast<fl64>(aeEpsilon) * aeEpsilon) {
         fl32 invLen = static_cast<fl32>(1.0 / std::sqrt(sum));
         v[idx + 0] = v[idx + 0] * invLen;
         v[idx + 1] = v[idx + 1] * invLen;
         v[idx + 2] = v[idx + 2] * invLen;
         v[idx + 3] = v[idx + 3] * invLen;
      } else {
         v[idx + 0] = 0.0f;
         v[idx + 1] = 0.0f;
         v[idx + 2] = 0.0f;
         v[idx + 3] = 0.0f;
      }
   }
}
