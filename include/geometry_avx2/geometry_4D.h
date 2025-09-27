/*
 * File: geometry_4D.h
 *
 * Version: v0.1
 *
 * Owner: David William Bull
 *
 * Created: 2025-09-27
 *
 * Last Modified: 2025-09-27
 *
 * Description: SIMD-accelerated computational geometry helpers for 4D. Includes helpers for Packed AoS vectors.
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



//== Packed 4D SIMD Helpers (AVX2)

/// @brief         Add packed 4D vectors (AoS) using AVX2.
///
/// Each vector is stored as {x,y,z,w} in an array-of-structures layout:
/// [x0, y0, z0, w0, x1, y1, z1, w1, ...].
/// Two 4D vectors are processed per iteration using a 256-bit SIMD register. Any remaining single vector is handled with scalar code.
///
/// @param a       Pointer to the first source array of packed 4D floats.
/// @param b       Pointer to the second source array of packed 4D floats.
/// @param out     Pointer to the destination array (may alias a or b).
/// @param count   Number of 4D vectors to process.
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

/// @brief         Subtract packed 4D vectors (AoS) using AVX2.
///
/// For each 4D vector i: out[i] = a[i] - b[i].
///
/// @param a       Pointer to the first source array of packed 4D floats.
/// @param b       Pointer to the second source array of packed 4D floats.
/// @param out     Pointer to the destination array (may alias a or b).
/// @param count   Number of 4D vectors to process.
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

/// @brief         Dot product of packed 4D vectors (AoS) using AVX2.
///
/// Computes dot[i] = a_x[i] * b_x[i] + a_y[i] * b_y[i] + a_z[i] * b_z[i] + a_w[i] * b_w[i] for each 4D vector. Two dot products are computed
/// simultaneously. Results are stored sequentially in the out array.
///
/// @param a       Pointer to the first source array of packed 4D floats.
/// @param b       Pointer to the second source array of packed 4D floats.
/// @param out     Pointer to the destination array of dot products.
/// @param count   Number of 4D vectors to process.
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

/// @brief         Compute lengths of packed 4D vectors (AoS) using AVX2.
///
/// Each result is the Euclidean length sqrt(x^2 + y^2 + z^2 + w^2) for one 4D vector.  Two lengths are computed per iteration.  Results are stored
/// sequentially in the out array.
///
/// @param v       Pointer to the source array of packed 4D floats.
/// @param out     Pointer to the destination array of lengths.
/// @param count   Number of 4D vectors to process.
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

/// @brief         Normalize packed 4D vectors (AoS) in place using AVX2.
///
/// Each 4D vector is scaled to unit length. Zero-length vectors remain {0,0,0,0}. Two vectors are processed in each iteration. The input and output
/// buffers must be identical (in place).
///
/// @param v       Pointer to the array of packed 4D floats to normalize.
/// @param count   Number of 4D vectors to process.
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
