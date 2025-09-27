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
 * Dependencies: cmath, immintrin, typedefs.h, vector structures.h
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

#define _GEOMETRY_MATH_AVX2_

constexpr cfl32 aeEpsilon = 1.0e-20f;
constexpr si32  rayMissed = static_cast<si32>(0x80000001);

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

/// @brief    Build an AVX2 mask for up to eight floats.
/// @param    count Number of valid lanes in the source array.
/// @return   Mask with sign bits set for active lanes (clamped to [0,8]).
inline ui256 MaskFromCount8(cui32 count) noexcept {
   cui32  laneCount = (count < 8u ? count : 8u);
   cui256 indices   = _mm256_setr_epi32(0, 1, 2, 3, 4, 5, 6, 7);
   cui256 limits    = _mm256_set1_epi32(static_cast<si32>(laneCount));
   return _mm256_cmpgt_epi32(limits, indices);
}

#include "geometry_avx2/geometry_1D.h"
#include "geometry_avx2/geometry_2D.h"
#include "geometry_avx2/geometry_3D.h"
#include "geometry_avx2/geometry_4D.h"

//== Matrix operations

static inline AVXmatrix MatrixRotationRollPitchYaw(cfl32 pitch, cfl32 yaw, cfl32 roll) noexcept {
   cfl32     sx = sinf(pitch), cx = cosf(pitch);
   cfl32     sy = sinf(yaw),   cy = cosf(yaw);
   cfl32     sz = sinf(roll),  cz = cosf(roll);
   AVXmatrix m;
   m.xmm[0] = _mm_setr_ps(cz * cy + sz * sx * sy, sz * cx, -cz * sy + sz * sx * cy, 0.0f);
   m.xmm[1] = _mm_setr_ps(-sz * cy + cz * sx * sy, cz * cx, sz * sy + cz * sx * cy, 0.0f);
   m.xmm[2] = _mm_setr_ps(cx * sy, -sx, cx * cy, 0.0f);
   m.xmm[3] = _mm_setr_ps(0.0f, 0.0f, 0.0f, 1.0f);
   return m;
}

static inline AVXmatrix __vectorcall MatrixLookAtLH(fl32x4 eye, fl32x4 target, fl32x4 up) noexcept {
   cfl32x4   forward  = Normalize3D(_mm_sub_ps(_mm_blend_ps(target, _mm_setzero_ps(), 0b1000), _mm_blend_ps(eye, _mm_setzero_ps(), 0b1000)));
   cfl32x4   right    = Normalize3D(Cross3D(up, forward));
   cfl32x4   cameraUp = Cross3D(forward, right);
   AVXmatrix m;
   m.xmm[0] = _mm_blend_ps(right, _mm_setzero_ps(), 0b1000);
   m.xmm[1] = _mm_blend_ps(cameraUp, _mm_setzero_ps(), 0b1000);
   m.xmm[2] = _mm_blend_ps(forward, _mm_setzero_ps(), 0b1000);
   cfl32x4 eyeVec = _mm_blend_ps(eye, _mm_setzero_ps(), 0b1000);
   cfl32   dotX   = _mm_cvtss_f32(_mm_dp_ps(right, eyeVec, 0x71));
   cfl32   dotY   = _mm_cvtss_f32(_mm_dp_ps(cameraUp, eyeVec, 0x71));
   cfl32   dotZ   = _mm_cvtss_f32(_mm_dp_ps(forward, eyeVec, 0x71));
   m.xmm[3] = _mm_setr_ps(-dotX, -dotY, -dotZ, 1.0f);
   return m;
}

static inline AVXmatrix __vectorcall MatrixMultiply(cAVXmatrix a, cAVXmatrix b) noexcept {
   AVXmatrix result;
   fl32x4    col0   = b.xmm[0];
   fl32x4    col1   = b.xmm[1];
   fl32x4    col2   = b.xmm[2];
   fl32x4    col3   = b.xmm[3];
   _MM_TRANSPOSE4_PS(col0, col1, col2, col3);
   for(ui32 i = 0; i < 4; ++i) {
      cfl32x4 row   = a.xmm[i];
      fl32x4  accum = _mm_mul_ps(_mm_shuffle_ps(row, row, _MM_SHUFFLE(0, 0, 0, 0)), col0);
              accum = _mm_fmadd_ps(_mm_shuffle_ps(row, row, _MM_SHUFFLE(1, 1, 1, 1)), col1, accum);
              accum = _mm_fmadd_ps(_mm_shuffle_ps(row, row, _MM_SHUFFLE(2, 2, 2, 2)), col2, accum);
      result.xmm[i] = _mm_fmadd_ps(_mm_shuffle_ps(row, row, _MM_SHUFFLE(3, 3, 3, 3)), col3, accum);
   }
   return result;
}

static inline AVXmatrix2 __vectorcall BuildViewFrame(fl32x4 eye, fl32x4 dir, fl32x4 up) noexcept {
   cfl32x4    dirN   = Normalize3D(dir);
   cfl32x4    upN    = Normalize3D(up);
   cfl32x4    right  = Normalize3D(Cross3D(upN, dirN));
   cfl32x4    camUp  = Cross3D(dirN, right);
   cfl32x4    eyeVec = _mm_blend_ps(eye, _mm_setzero_ps(), 0b1000);
   AVXmatrix2 frame;
   frame.xmm[0][0] = _mm_blend_ps(right, _mm_setzero_ps(), 0b1000);
   frame.xmm[0][1] = _mm_blend_ps(camUp, _mm_setzero_ps(), 0b1000);
   frame.xmm[0][2] = _mm_blend_ps(dirN, _mm_setzero_ps(), 0b1000);
   cfl32 dotX = _mm_cvtss_f32(_mm_dp_ps(right, eyeVec, 0x71));
   cfl32 dotY = _mm_cvtss_f32(_mm_dp_ps(camUp, eyeVec, 0x71));
   cfl32 dotZ = _mm_cvtss_f32(_mm_dp_ps(dirN, eyeVec, 0x71));
   frame.xmm[0][3] = _mm_setr_ps(-dotX, -dotY, -dotZ, 1.0f);
   frame.xmm[1][0] = _mm_blend_ps(right, _mm_setzero_ps(), 0b1000);
   frame.xmm[1][1] = _mm_blend_ps(camUp, _mm_setzero_ps(), 0b1000);
   frame.xmm[1][2] = _mm_blend_ps(dirN, _mm_setzero_ps(), 0b1000);
   frame.xmm[1][3] = _mm_blend_ps(eyeVec, _mm_setr_ps(0.0f, 0.0f, 0.0f, 1.0f), 0b1000);
   return frame;
}

//== Intersections ==//

//-- Return values are rayMissed if target is beyond horizon

#if !defined(USE_OLD_CODE)
__forceinline void __vectorcall _QuadraticCoefficients(cfl32x4 rayOri, cfl32x4 rayDir, fl32x4 vec[3], fl32 radius) noexcept {
   // Quadratic coefficients for |rayOri + t * rayDir|^2 = radius^2.
   vec[0] = Dot3D(rayDir, rayDir);   vec[1] = Dot3D(rayOri, rayDir);   vec[2] = Dot3D(rayOri, rayOri);
   // Guard against zero-length directions which would make the quadratic degenerate.
   cfl32x4 signMask = _mm_set1_ps(-0.0f);
   cfl32x4 absA     = _mm_andnot_ps(signMask, vec[0]);
   if(_mm_comile_ss(absA, _mm_set1_ps(1.0e-8f))) vec[1] = negOnes32x4f;
   // c -= radius^2 using an FMA to keep the mantissa tight.
   cfl32x4 rSq    = _mm_fmadd_ss(_mm_set_ss(radius), _mm_set_ss(radius), _mm_setzero_ps());
           vec[2] = _mm_sub_ss(vec[2], rSq);
}

inline cfl32 __vectorcall RaySphereIntersect_d(vector rayOri, vector rayDir, fl32 radius) noexcept {
   fl32x4 vec[3];
   _QuadraticCoefficients(rayOri, rayDir, vec, radius);
   cfl32x4 disc = _mm_fnmadd_ss(vec[0], vec[2], _mm_mul_ss(vec[1], vec[1]));
   // Return the discriminant (b^2 - a*c); negative means the ray misses the sphere.
   if(_mm_comilt_ss(disc, _mm_setzero_ps())) return -1.0f;
}

inline cfl32 RaySphereIntersect(vector rayOri, vector rayDir, fl32 radius) noexcept {
   fl32x4 vec[3];
   _QuadraticCoefficients(rayOri, rayDir, vec, radius);
   // discriminant = b^2 - a*c with one fused negative multiply-add.
   cfl32x4 disc = _mm_fnmadd_ss(vec[0], vec[2], _mm_mul_ss(vec[1], vec[1]));
   if(_mm_comilt_ss(disc, _mm_setzero_ps())) return -1.0f;

   // Prepare 1/a via Newton-Raphson refined reciprocal.
   cfl32x4 rcpInit  = _mm_rcp_ps(vec[0]);
   cfl32x4 two      = _mm_set1_ps(2.0f);
   cfl32x4 invA     = _mm_mul_ps(rcpInit, _mm_fnmadd_ps(vec[0], rcpInit, two));
   // Solve the quadratic for the nearest positive t.
   cfl32x4 sqrtDisc = _mm_sqrt_ss(disc);
   cfl32x4 negB     = _mm_sub_ss(_mm_setzero_ps(), vec[1]);
   cfl32x4 tNearVec = _mm_mul_ss(_mm_sub_ss(negB, sqrtDisc), invA);
   cfl32x4 tFarVec  = _mm_mul_ss(_mm_add_ss(negB, sqrtDisc), invA);
   cfl32   tNear    = _mm_cvtss_f32(tNearVec);
   if(tNear > 0.0f) return tNear;

   cfl32 tFar = _mm_cvtss_f32(tFarVec);
   return (tFar > 0.0f) ? tFar : -1.0f;
}

inline fl32 __vectorcall RayPlaneIntersect_d(cfl32x4 rayOri, cfl32x4 rayDir, cfl32x4 planeOri, cfl32x4 planeDir) noexcept {
   // Dot(rayDir, planeDir) -> denominator; early out when parallel.
   cfl32x4 denomVec = Dot3D(planeDir, rayDir);
   cfl32   denom    = _mm_cvtss_f32(denomVec);
   if(denom != 0.0f) {
      // Project the plane origin difference along the plane normal.
      cfl32x4 diff     = _mm_sub_ps(planeOri, rayOri);
      cfl32x4 numer    = Dot3D(diff, planeDir);
      cfl32x4 rcpInit  = _mm_rcp_ps(denomVec);
      // One Newton-Raphson step keeps the reciprocal accurate without a full divide.
      cfl32x4 two      = _mm_set1_ps(2.0f);
      cfl32x4 denomRcp = _mm_mul_ps(rcpInit, _mm_fnmadd_ps(denomVec, rcpInit, two));
      cfl32   hitParam = _mm_cvtss_f32(_mm_mul_ss(numer, denomRcp));
      if(hitParam > 0.0f) return hitParam;
   }
   // No forward intersection: return denominator for compatibility with legacy callers.
   return denom;
}

inline fl32 RayPlaneIntersect(cfl32x4 rayOri, cfl32x4 rayDir, cfl32x4 planeOri, cfl32x4 planeDir) noexcept {
   // denom = dot(rayDir, planeDir); near-zero indicates parallel or degenerate.
   cfl32x4 denomVec = Dot3D(planeDir, rayDir);
   cfl32x4 signMask = _mm_set1_ps(-0.0f);
   cfl32x4 absDen   = _mm_andnot_ps(signMask, denomVec);

   if(_mm_comile_ss(absDen, _mm_set1_ps(1.0e-8f))) return -1.0f;
   // Project the ray origin to the plane normal to compute numer.
   cfl32x4 diff    = _mm_sub_ps(planeOri, rayOri);
   cfl32x4 numer   = Dot3D(diff, planeDir);
   // t = numer / denom using a refined reciprocal to avoid scalar divides.
   cfl32x4 rcpInit = _mm_rcp_ps(denomVec);
   cfl32x4 two     = _mm_set1_ps(2.0f);
   cfl32x4 denomR  = _mm_mul_ps(rcpInit, _mm_fnmadd_ps(denomVec, rcpInit, two));
   cfl32x4 tVec    = _mm_mul_ss(numer, denomR);

   cfl32 t = _mm_cvtss_f32(tVec);
   return (t > 0.0f) ? t : -1.0f;
}

inline fl32 __vectorcall RayBoxIntersect(cfl32x4 rayOri, cfl32x4 rayDir, cfl32x4 boxMin, cfl32x4 boxMax) noexcept {
   // Mask ensures w lanes never contaminate horizontal reductions.
   cfl32x4 maskXYZ  = _mm_castsi128_ps(_mm_set_epi32(0, -1, -1, -1));
   cfl32x4 ori      = _mm_and_ps(rayOri, maskXYZ);
   cfl32x4 dir      = _mm_and_ps(rayDir, maskXYZ);
   cfl32x4 b0       = _mm_and_ps(boxMin, maskXYZ);
   cfl32x4 b1       = _mm_and_ps(boxMax, maskXYZ);
   // Reject rays parallel to a slab while being outside that slab.
   cfl32x4 outside  = _mm_or_ps(_mm_cmplt_ps(ori, b0), _mm_cmpgt_ps(ori, b1));
   cfl32x4 parallel = _mm_cmpeq_ps(dir, _mm_setzero_ps());

   if((_mm_movemask_ps(_mm_and_ps(outside, parallel)) & 0x7) != 0) return -1.0f;
   // Compute refined reciprocals for the direction to avoid slow divides.
   cfl32x4 rcpInit  = _mm_rcp_ps(dir);
   cfl32x4 two      = _mm_set1_ps(2.0f);
   cfl32x4 invDir   = _mm_mul_ps(rcpInit, _mm_fnmadd_ps(dir, rcpInit, two));
   // Vectorise both min/max slabs so we can resolve them in one AVX2 pass.
   cfl32x8 invDir8  = _mm256_broadcast_ps(&invDir);
   cfl32x8 ori8     = _mm256_broadcast_ps(&ori);
   cfl32x8 bounds   = _mm256_insertf128_ps(_mm256_castps128_ps256(b0), b1, 1);
   // (bound - ori) * invDir using FMA for both halves simultaneously.
   cfl32x8 oriTerm  = _mm256_mul_ps(ori8, invDir8);
   cfl32x8 slabs    = _mm256_fmsub_ps(bounds, invDir8, oriTerm);
   cfl32x4 t0       = _mm256_castps256_ps128(slabs);
   cfl32x4 t1       = _mm256_extractf128_ps(slabs, 1);
   cfl32x4 swapMask = _mm_cmplt_ps(invDir, _mm_setzero_ps());
   fl32x4  tMin     = _mm_blendv_ps(t0, t1, swapMask);
   fl32x4  tMax     = _mm_blendv_ps(t1, t0, swapMask);
   // Force w lanes to sentinel values so the horizontal reductions stay within xyz.
   cfl32x4 huge     = _mm_set1_ps(3.402823466e+38F);
   cfl32x4 negHuge  = _mm_sub_ps(_mm_setzero_ps(), huge);
           tMin     = _mm_blend_ps(tMin, negHuge, 0x8);
           tMax     = _mm_blend_ps(tMax, huge, 0x8);
   // Horizontal reductions: entry is the max of the slab minima, exit is the min of maxima.
   fl32x4  tEnterV  = _mm_max_ps(tMin, _mm_shuffle_ps(tMin, tMin, _MM_SHUFFLE(2, 1, 0, 3)));
           tEnterV  = _mm_max_ps(tEnterV, _mm_shuffle_ps(tEnterV, tEnterV, _MM_SHUFFLE(1, 0, 3, 2)));
   fl32x4  tExitV   = _mm_min_ps(tMax, _mm_shuffle_ps(tMax, tMax, _MM_SHUFFLE(2, 1, 0, 3)));
           tExitV   = _mm_min_ps(tExitV, _mm_shuffle_ps(tExitV, tExitV, _MM_SHUFFLE(1, 0, 3, 2)));
   cfl32   tEnter   = _mm_cvtss_f32(tEnterV);
   cfl32   tExit    = _mm_cvtss_f32(tExitV);
   cfl32   thresh   = (tEnter > 0.0f) ? tEnter : 0.0f;
   if(tExit < thresh) return -1.0f;

   return (tEnter >= 0.0f) ? tEnter : tExit;
}

inline fl32 __vectorcall RayRotBoxIntersect(cfl32x4 rayOri, cfl32x4 rayDir, cfl32x4 boxOri, cmatrix boxRot, cfl32x4 halfExtent) noexcept {
   // Transform ray into box-local space using the rotation rows as orthonormal axes.
   cfl32x4 maskXYZ     = _mm_castsi128_ps(_mm_set_epi32(0, -1, -1, -1));
   cfl32x4 localOri    = _mm_and_ps(_mm_sub_ps(rayOri, boxOri), maskXYZ);
   cfl32x4 axisX       = _mm_and_ps(boxRot.xmm[0], maskXYZ);
   cfl32x4 axisY       = _mm_and_ps(boxRot.xmm[1], maskXYZ);
   cfl32x4 axisZ       = _mm_and_ps(boxRot.xmm[2], maskXYZ);
   cfl32x4 oriX        = Dot3D(localOri, axisX);
   cfl32x4 oriY        = Dot3D(localOri, axisY);
   cfl32x4 oriZ        = Dot3D(localOri, axisZ);
   cfl32x4 dirX        = Dot3D(rayDir, axisX);
   cfl32x4 dirY        = Dot3D(rayDir, axisY);
   cfl32x4 dirZ        = Dot3D(rayDir, axisZ);
   cfl32x4 splatZ      = _mm_shuffle_ps(oriZ, oriZ, _MM_SHUFFLE(0, 0, 0, 0));
   fl32x4  localOrigin = _mm_movelh_ps(oriX, oriY);
           localOrigin = _mm_blend_ps(localOrigin, splatZ, 0x4);
           localOrigin = _mm_blend_ps(localOrigin, _mm_setzero_ps(), 0x8);
   cfl32x4 splatDirZ   = _mm_shuffle_ps(dirZ, dirZ, _MM_SHUFFLE(0, 0, 0, 0));
   fl32x4  localDir    = _mm_movelh_ps(dirX, dirY);
           localDir    = _mm_blend_ps(localDir, splatDirZ, 0x4);
           localDir    = _mm_blend_ps(localDir, _mm_setzero_ps(), 0x8);
   cfl32x4 extent      = _mm_and_ps(halfExtent, _mm_castsi128_ps(_mm_set_epi32(0, -1, -1, -1)));
   cfl32x4 negExtent   = _mm_sub_ps(_mm_setzero_ps(), extent);

   return RayBoxIntersect(localOrigin, localDir, negExtent, extent);
}

// Returns truncated X&Y coordinates
inline VEC2Ds32 __vectorcall RayLayerIntersect(cfl32x4 rayOri, cfl32x4 rayDir, si32 layer) noexcept {
   cfl32 coordZ = fl32(layer);
   // z lanes
   cfl32x4 oz   = _mm_shuffle_ps(rayOri, rayOri, _MM_SHUFFLE(2, 2, 2, 2));
   cfl32x4 dz   = _mm_shuffle_ps(rayDir, rayDir, _MM_SHUFFLE(2, 2, 2, 2));
   cfl32   oriZ = _mm_cvtss_f32(oz);
   cfl32   dirZ = _mm_cvtss_f32(dz);

   if(dirZ == 0.0f) return { rayMissed, rayMissed };

   // (optional: rcp + 1 NR step if profiling favors it)
   cfl32 t = (coordZ - oriZ) / dirZ;
   if(t <= 0.0f) return { rayMissed, rayMissed };

   cfl32x4 T   = _mm_set1_ps(t);
   cfl32x4 hit = simd::fmadd_ps(rayDir, T, rayOri); // dir * t + ori
   // floor(x,y) -> int
   cfl32x4 flo = _mm_floor_ps(hit); // SSE4.1
   cui128  ixy = _mm_cvttps_epi32(flo);
   csi32   ix  = _mm_cvtsi128_si32(ixy);    // x
   csi32   iy  = _mm_extract_epi32(ixy, 1); // y

   return { ix, iy };
}

// Returns truncated X&Y coordinates
inline VEC2Ds32 __vectorcall RayLayerIntersect(cfl32x8 rayOriDir, si32 layer) noexcept {
   cfl32x8 oriPack = _mm256_permute2f128_ps(rayOriDir, rayOriDir, 0x00);
   cfl32x8 dirPack = _mm256_permute2f128_ps(rayOriDir, rayOriDir, 0x11);
   cfl32x4 rayOri  = _mm256_castps256_ps128(oriPack);
   cfl32x4 rayDir  = _mm256_castps256_ps128(dirPack);
   cfl32x4 oriZ    = _mm_shuffle_ps(rayOri, rayOri, _MM_SHUFFLE(2, 2, 2, 2));
   cfl32x4 dirZ    = _mm_shuffle_ps(rayDir, rayDir, _MM_SHUFFLE(2, 2, 2, 2));
   cfl32   coordZ  = fl32(layer);
   cfl32   oz      = _mm_cvtss_f32(oriZ);
   cfl32   dz      = _mm_cvtss_f32(dirZ);

   if(dz == 0.0f) return { rayMissed, rayMissed };

   cfl32 t = (coordZ - oz) / dz;
   if(t <= 0.0f) return { rayMissed, rayMissed };

   cfl32x8 tVec    = _mm256_set1_ps(t);
   cfl32x8 hitPack = _mm256_fmadd_ps(dirPack, tVec, oriPack);
   cfl32x4 hit     = _mm256_castps256_ps128(hitPack);
   cfl32x4 flo     = _mm_floor_ps(hit);
   cui128  ixy     = _mm_cvttps_epi32(flo);
   csi32   ix      = _mm_cvtsi128_si32(ixy);
   csi32   iy      = _mm_extract_epi32(ixy, 1);

   return { ix, iy };
}
#else
inline cfl32 __vectorcall RaySphereIntersect(vector rayOri, vector rayDir, fl32 radius) noexcept {
   // Calculate the a, b, and c coefficients
   cfl32 a = _mm_dp_ps(rayDir, rayDir, 0x071).m128_f32[0];
   cfl32 b = _mm_dp_ps(rayDir, rayOri, 0x071).m128_f32[0];
   cfl32 c = _mm_dp_ps(rayOri, rayOri, 0x071).m128_f32[0] - (radius * radius);
   // Returns discriminant; if result is negative the picking ray missed the sphere
   return b - (a * c);
}

inline fl32 __vectorcall RayPlaneIntersect(cfl32x4 rayOri, cfl32x4 rayDir, cfl32x4 planeOri, cfl32x4 planeDir) noexcept {
   cfl32x4 vDenom = DX::XMVector3Dot(planeDir, rayDir);
   if(vDenom.m128_f32[0] != 0.0f) {
      cfl32x4 vDiff = DX::XMVectorSubtract(planeOri, rayOri);
      cfl32x4 vT    = DX::XMVector3Dot(vDiff, planeDir);
      cfl32   fT    = vT.m128_f32[0] / vDenom.m128_f32[0];

      if(fT > 0.0f) return fT;
   }
   return vDenom.m128_f32[0];
}

inline VEC2Ds32 __vectorcall RayLayerIntersect(cfl32x4 rayOri, cfl32x4 rayDir, si32 layer) noexcept {
   cfl32x4 planeDir = { 0.0f, 0.0f, 1.0f };
   cfl32x4 planeOri = { 0.0f, 0.0f, fl32(layer)};
   cfl32x4 vDenom   = DX::XMVector3Dot(planeDir, rayDir);

   if(vDenom.m128_f32[0] != 0.0f) {
      cfl32x4 vDiff = DX::XMVectorSubtract(planeOri, rayOri);
      cfl32x4 vT    = DX::XMVector3Dot(vDiff, planeDir);
      cfl32   fT    = vT.m128_f32[0] / vDenom.m128_f32[0];

      if(fT > 0.0f) {
         //--- Calculate 2D coordinate
         cVEC2Df   fStep = { rayDir.m128_f32[0] * fT, rayDir.m128_f32[1] * fT };
         cVEC2Ds32 vCell = { si32(floorf(rayOri.m128_f32[0] + fStep.x)), si32(floorf(rayOri.m128_f32[1] + fStep.y)) };

         return vCell;
      }
   }
   return { rayMissed, rayMissed };
}
#endif
