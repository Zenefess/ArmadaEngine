/*
 * File: geometry_3D.h
 *
 * Version: v0.1
 *
 * Owner: David William Bull
 *
 * Created: 2025-09-27
 *
 * Last Modified: 2025-09-27
 *
 * Description: SIMD-accelerated computational geometry helpers for 3D. Includes helpers for Packed AoS vectors.
 *
 * To Do: 1) Wire CPUID dispatch and scalar fallbacks.
 *        2) Add microbenchmarks in docs/.
 *        3) Implement project/intersect/distance helpers.
 *
 * Dependencies: Inherited from parent.
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


//== Non-packed Helpers

inline fl32x4 __vectorcall Dot3D(cfl32x4 lhs, cfl32x4 rhs) noexcept {
   cfl32x4 maskXYZ = _mm_castsi128_ps(_mm_set_epi32(0, -1, -1, -1));
   cfl32x4 ones    = _mm_set1_ps(1.0f);
   cfl32x4 mul     = _mm_mul_ps(_mm_and_ps(lhs, maskXYZ), _mm_and_ps(rhs, maskXYZ));
   cfl32x4 hi      = _mm_movehl_ps(mul, mul);
   cfl32x4 sum     = _mm_fmadd_ps(hi, ones, mul);
   cfl32x4 yLane   = _mm_shuffle_ps(sum, sum, _MM_SHUFFLE(1, 1, 1, 1));
   return _mm_blend_ps(_mm_setzero_ps(), _mm_fmadd_ss(yLane, ones, sum), 0x1);
}

static inline fl32x4 __vectorcall Cross3D(fl32x4 a, fl32x4 b) noexcept {
   cfl32x4 a0    = _mm_blend_ps(a, _mm_setzero_ps(), 0b1000);
   cfl32x4 b0    = _mm_blend_ps(b, _mm_setzero_ps(), 0b1000);
   cfl32x4 a_yzx = _mm_shuffle_ps(a0, a0, _MM_SHUFFLE(3, 0, 2, 1));
   cfl32x4 b_yzx = _mm_shuffle_ps(b0, b0, _MM_SHUFFLE(3, 0, 2, 1));
   cfl32x4 c     = _mm_fmsub_ps(a0, b_yzx, _mm_mul_ps(a_yzx, b0));
   return _mm_blend_ps(_mm_shuffle_ps(c, c, _MM_SHUFFLE(3, 0, 2, 1)), _mm_setzero_ps(), 0b1000);
}

static inline fl32x4 __vectorcall Transform3D(fl32x4 v, cAVXmatrix m) noexcept {
   cfl32x4 vec    = _mm_blend_ps(v, _mm_set1_ps(1.0f), 0b1000);
   fl32x4  result = _mm_mul_ps(_mm_shuffle_ps(vec, vec, _MM_SHUFFLE(0, 0, 0, 0)), m.xmm[0]);
           result = _mm_fmadd_ps(_mm_shuffle_ps(vec, vec, _MM_SHUFFLE(1, 1, 1, 1)), m.xmm[1], result);
           result = _mm_fmadd_ps(_mm_shuffle_ps(vec, vec, _MM_SHUFFLE(2, 2, 2, 2)), m.xmm[2], result);
   return _mm_fmadd_ps(_mm_shuffle_ps(vec, vec, _MM_SHUFFLE(3, 3, 3, 3)), m.xmm[3], result);
}

static inline fl32x4 __vectorcall TransformNormal3D(fl32x4 v, cAVXmatrix m) noexcept {
   cfl32x4 vec    = _mm_blend_ps(v, _mm_setzero_ps(), 0b1000);
   fl32x4  result = _mm_mul_ps(_mm_shuffle_ps(vec, vec, _MM_SHUFFLE(0, 0, 0, 0)), m.xmm[0]);
           result = _mm_fmadd_ps(_mm_shuffle_ps(vec, vec, _MM_SHUFFLE(1, 1, 1, 1)), m.xmm[1], result);
           result = _mm_fmadd_ps(_mm_shuffle_ps(vec, vec, _MM_SHUFFLE(2, 2, 2, 2)), m.xmm[2], result);
   return _mm_blend_ps(result, _mm_setzero_ps(), 0b1000);
}

static inline fl32x4 __vectorcall Normalize3D(fl32x4 v) noexcept {
   cfl32x4 zeroW    = _mm_blend_ps(v, _mm_setzero_ps(), 0b1000);
   cfl32   lengthSq = _mm_cvtss_f32(_mm_dp_ps(zeroW, zeroW, 0x71));
   if(lengthSq <= 0.0f) return zeroW;
   cfl32x4 lengthSqVec = _mm_set1_ps(lengthSq);
   fl32x4  invLen      = _mm_rsqrt_ps(lengthSqVec);
   cfl32x4 half        = _mm_set1_ps(0.5f);
   cfl32x4 three       = _mm_set1_ps(3.0f);
   cfl32x4 invLenSq    = _mm_mul_ps(invLen, invLen);
   invLen = _mm_mul_ps(invLen, _mm_fnmadd_ps(lengthSqVec, invLenSq, three));
   invLen = _mm_mul_ps(invLen, half);
   return _mm_mul_ps(zeroW, invLen);
}

//== Packed Helpers

/// @brief         Add packed 3D vectors (AoS) using AVX2.
///
/// Each vector is stored as {x,y,z} triplets in an array-of-structures layout:
/// [x0, y0, z0, x1, y1, z1, ...].
/// This routine processes two triplets at a time using a 256-bit SIMD register with mask loads/stores. Remaining triplets (if count is odd) are
/// handled with scalar code.
///
/// @param a       Pointer to the first source array of packed 3D floats.
/// @param b       Pointer to the second source array of packed 3D floats.
/// @param out     Pointer to the destination array (may alias a or b).
/// @param count   Number of 3D vectors to process.
inline void Add3D_AoS(cfl32ptr a, cfl32ptr b, fl32ptr out, cui32 count) noexcept {
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

/// @brief         Subtract packed 3D vectors (AoS) using AVX2.
///
/// Each element of out is set to a[i] - b[i], where i indexes 3D triplets. See Add3D_AoS for layout details.
///
/// @param a       Pointer to the first source array of packed 3D floats.
/// @param b       Pointer to the second source array of packed 3D floats.
/// @param out     Pointer to the destination array (may alias a or b).
/// @param count   Number of 3D vectors to process.
inline void Sub3D_AoS(cfl32ptr a, cfl32ptr b, fl32ptr out, cui32 count) noexcept {
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


/// @brief         Dot product of packed 3D vectors (AoS) using AVX2.
///
/// Computes the dot product for each 3D triplet:
/// dot[i] = a_x[i] * b_x[i] + a_y[i] * b_y[i] + a_z[i] * b_z[i].
/// Two triplets are processed per iteration. The result array holds one float per 3D vector.
///
/// @param a       Pointer to the first source array of packed 3D floats.
/// @param b       Pointer to the second source array of packed 3D floats.
/// @param out     Pointer to the destination array of dot products.
/// @param count   Number of 3D vectors to process.
inline void Dot3D_AoS(cfl32ptr a, cfl32ptr b, fl32ptr out, cui32 count) noexcept {
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

/// @brief         Compute 3D cross product (AoS) per packed vector using AVX2.
///
/// For each triplet i, returns a vector
/// (a_y[i]*b_z[i] - a_z[i]*b_y[i],
///  a_z[i]*b_x[i] - a_x[i]*b_z[i],
///  a_x[i]*b_y[i] - a_y[i]*b_x[i]).
/// Two triplets are processed simultaneously. The result array holds three floats per 3D vector.
///
/// @param a       Pointer to the first source array of packed 3D floats.
/// @param b       Pointer to the second source array of packed 3D floats.
/// @param out     Pointer to the destination array of cross products.
/// @param count   Number of 3D vectors to process.
inline void Cross3D_AoS(cfl32ptr a, cfl32ptr b, fl32ptr out, cui32 count) noexcept {
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

/// @brief         Compute lengths of packed 3D vectors (AoS) using AVX2.
///
/// Each result is the Euclidean length sqrt(x^2 + y^2 + z^2) for one 3D triplet. Two triplets are handled per iteration. Results are stored
/// sequentially in the output array.
///
/// @param v       Pointer to the source array of packed 3D floats.
/// @param out     Pointer to the destination array of lengths.
/// @param count   Number of 3D vectors to process.
inline void Length3D_AoS(cfl32ptr v, fl32ptr out, cui32 count) noexcept {
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

/// @brief         Normalize packed 3D vectors (AoS) in place using AVX2.
///
/// Each 3D vector is scaled to unit length. Zero-length vectors remain {0,0,0}. The normalization computes two triplets per iteration, computing
/// lengths and reciprocals on 128-bit halves and broadcasting them across each triplet.
///
/// @param v       Pointer to the array of packed 3D floats to normalize in place.
/// @param count   Number of 3D vectors to process.
inline void Normalize3D_AoS(fl32ptr v, cui32 count) noexcept {
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
