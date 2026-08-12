/*
 * File: common functions.h
 * Version: v1.1.0
 * Owner: David William Bull
 * Created: 2023-02-02
 * Last Modified: 2026-08-12
 * Description: Inline scalar and SIMD utility functions: constants, swaps, min/max, 2D rotation, sincos, vector tests, and power-of-2 rounding.
 * To Do: 1) Add /// API documentation with @param/@return tags to all public functions (d1)
 *        2) Rename PascalCase macros (RoundUpToNearest, UNLOOPx2/4/8/16) to UPPER_SNAKE per r12
 *        3) Add unit tests for RoundUpToNearest*, AllTrue, AllFalse, Min/Max, and sincos in tests/
 * Dependencies: typedefs.h, vector structures.h, corecrt_math.h, SIMD management.h
 * ISA: Scalar | SSE4.2 | AVX2
 * Thread-safety: Reentrant
 * Reviewers: David William Bull
 * License: MIT  Copyright: David William Bull
 */
#pragma once

#include <typedefs.h>
#include <vector structures.h>
#include <corecrt_math.h>
#include "SIMD management.h"
//#include "string_func_avx2.h"

static_assert(__AVX2__, "GCS a3: x86-64 AVX2+FMA3+BMI2 baseline required; compile with /arch:AVX2 or -mavx2 -mfma -mbmi2");

constexpr cfl32 rcp1p5f   = 2.0f / 3.0f;
constexpr cfl32 rcp3f     = 1.0f / 3.0f;
constexpr cfl32 rcp6f     = 1.0f / 6.0f;
constexpr cfl32 rcp32767f = 1.0f / 32767.0f;
constexpr cfl32 rcp32768f = 1.0f / 32768.0f;
constexpr cfl32 rcp65535f = 1.0f / 65535.0f;
constexpr cfl32 rcp65536f = 1.0f / 65536.0f;

constexpr cfl64 rcp1p5d   = 2.0 / 3.0;
constexpr cfl64 rcp3d     = 1.0 / 3.0;
constexpr cfl64 rcp6d     = 1.0 / 6.0;
constexpr cfl64 rcp32767d = 1.0 / 32767.0;
constexpr cfl64 rcp32768d = 1.0 / 32768.0;
constexpr cfl64 rcp65535d = 1.0 / 65535.0;
constexpr cfl64 rcp65536d = 1.0 / 65536.0;

constexpr cui64 null64 = 0;
constexpr cui64 one64  = 1u;
constexpr cui64 max64  = -1;

constexpr cVEC3Df null3Df = { 0.0f, 0.0f, 0.0f };

constexpr cfl32x4  null128f      = {};
constexpr cfl32x4  ones32x4f     = { .m128_f32 = { 1.0f, 1.0f, 1.0f, 1.0f } };
constexpr cfl32x4  negOnes32x4f  = { .m128_f32 = { -1.0f, -1.0f, -1.0f, -1.0f } };
constexpr cui128   null128       = {};
constexpr cui128   ones32x4      = { .m128i_u32 = { 1, 1, 1, 1 } };
constexpr cui128   max128        = { .m128i_i32 = { -1, -1, -1, -1 } };
constexpr cfl32x8  null256f      = {};
constexpr cfl32x8  ones32x8f     = { .m256_f32 = { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f } };
constexpr cfl32x8  negOnes32x8f  = { .m256_f32 = { -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f } };
constexpr cui256   null256       = {};
constexpr cui256   ones32x8      = { .m256i_u32 = { 1, 1, 1, 1, 1, 1, 1, 1 } };
constexpr cui256   max256        = { .m256i_i32 = { -1, -1, -1, -1, -1, -1, -1, -1 } };
constexpr cfl32x16 null512f      = {};
constexpr cfl32x16 ones32x16f    = { .m512_f32 = { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
                                                   1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f } };
constexpr cfl32x16 negOnes32x16f = { .m512_f32 = { -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,
                                                   -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f } };
constexpr cui512   null512       = {};
constexpr cui512   ones32x16     = { .m512i_u32 = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } };
constexpr cui512   max512        = { .m512i_i32 = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } };

///--- Unloop macros
#define UNLOOPx2(code)  code code
#define UNLOOPx4(code)  code code code code
#define UNLOOPx8(code)  code code code code code code code code
#define UNLOOPx16(code) code code code code code code code code code code code code code code code code
///--- Unloop macros

#ifdef _SYNCHAPI_H_
inline void Idle(cui32 msTime) { if(msTime) Sleep(msTime); else _mm_pause(); }
#endif

inline void swap(ui8&  a, ui8&  b) { ui8  tmp = a; a = b; b = tmp; }
inline void swap(ui16& a, ui16& b) { ui16 tmp = a; a = b; b = tmp; }
inline void swap(ui32& a, ui32& b) { ui32 tmp = a; a = b; b = tmp; }
inline void swap(ui64& a, ui64& b) { ui64 tmp = a; a = b; b = tmp; }
inline void swap(fl32& a, fl32& b) { fl32 tmp = a; a = b; b = tmp; }
inline void swap(fl64& a, fl64& b) { fl64 tmp = a; a = b; b = tmp; }

inline void transrotate(VEC2Df &coord, cfl32 angle, cfl32 dist) {
   coord.x -= dist * sinf(angle);
   coord.y += dist * cosf(angle);
}

inline void transrotate(VEC2Df &coord, cfl32 angle) {
   cVEC2Df temp = coord;
   cVEC2Df rots = { sinf(angle), cosf(angle) };

   coord = { (temp.x * rots.y) - (temp.y * rots.x), (temp.x * rots.x) + (temp.y * rots.y) };
}

inline void transrotate(VEC2Df &coord, cVEC2Df origin, cfl32 angle) {
   cVEC2Df temp = { coord.x - origin.x, coord.y - origin.y };
   cVEC2Df rots = { sinf(angle), cosf(angle) };

   coord = { (temp.x * rots.y) - (temp.y * rots.x) + origin.x,
             (temp.x * rots.x) + (temp.y * rots.y) + origin.y };
}

inline cVEC2Df sincos(cfl32 angle) {   return { sinf(angle), cosf(angle) }; }

inline void sincos(cfl32 angle, fl32 &sinval, fl32 &cosval) {
   sinval = sinf(angle);
   cosval = cosf(angle);
}

inline void sincos(cfl32 angle, fl64 &sinval, fl64 &cosval) {
   sinval = sin(cfl64(angle));
   cosval = cos(cfl64(angle));
}

inline void sincos(cfl64 angle, fl64 &sinval, fl64 &cosval) {
   sinval = sin(angle);
   cosval = cos(angle);
}

inline void mov24(ui8 (&dest)[3], cui32 value) { (ui16 &)dest = (ui16 &)value;   dest[2] = ((ui8 (&)[3])value)[2]; }

inline void mulV3(VEC3Df &vector, cfl32 multiplier) { vector.x *= multiplier;   vector.y *= multiplier;   vector.z *= multiplier; }
inline void mulV3(fl32x4 &vector, cfl32 multiplier) { vector = _mm_mul_ps(vector, cfl32x4{ multiplier, multiplier, multiplier, 1.0f }); }

inline constexpr cui8  Min(cui8  a, cui8  b) { return a < b ? a : b; }
inline constexpr cui16 Min(cui16 a, cui16 b) { return a < b ? a : b; }
inline constexpr cui32 Min(cui32 a, cui32 b) { return a < b ? a : b; }
inline constexpr cui64 Min(cui64 a, cui64 b) { return a < b ? a : b; }
inline constexpr csi8  Min(csi8  a, csi8  b) { return a < b ? a : b; }
inline constexpr csi16 Min(csi16 a, csi16 b) { return a < b ? a : b; }
inline constexpr csi32 Min(csi32 a, csi32 b) { return a < b ? a : b; }
inline constexpr csi64 Min(csi64 a, csi64 b) { return a < b ? a : b; }
inline constexpr cfl32 Min(cfl32 a, cfl32 b) { return a < b ? a : b; }
inline constexpr cfl64 Min(cfl64 a, cfl64 b) { return a < b ? a : b; }

inline constexpr cui8  Max(cui8  a, cui8  b) { return a > b ? a : b; }
inline constexpr cui16 Max(cui16 a, cui16 b) { return a > b ? a : b; }
inline constexpr cui32 Max(cui32 a, cui32 b) { return a > b ? a : b; }
inline constexpr cui64 Max(cui64 a, cui64 b) { return a > b ? a : b; }
inline constexpr csi8  Max(csi8  a, csi8  b) { return a > b ? a : b; }
inline constexpr csi16 Max(csi16 a, csi16 b) { return a > b ? a : b; }
inline constexpr csi32 Max(csi32 a, csi32 b) { return a > b ? a : b; }
inline constexpr csi64 Max(csi64 a, csi64 b) { return a > b ? a : b; }
inline constexpr cfl32 Max(cfl32 a, cfl32 b) { return a > b ? a : b; }
inline constexpr cfl64 Max(cfl64 a, cfl64 b) { return a > b ? a : b; }

inline cfl32 Max (cVEC3Df &vector)   { return Max(Max(vector.x, vector.y), vector.z); }
inline cfl32 Max (cVEC4Df& vector)   { return Max(Max(vector.x, vector.y), Max(vector.z, vector.w)); }
inline cfl32 Max (cSSE4Df32& vector) { return Max(Max(vector.vector.x, vector.vector.y), Max(vector.vector.z, vector.vector.w)); }
inline cfl32 Max3(cVEC4Df &vector)   { return Max(Max(vector.x, vector.y), vector.z); }
inline cfl32 Max3(cSSE4Df32 &vector) { return Max(Max(vector.vector.x, vector.vector.y), vector.vector.z); }

inline cbool AllTrue(cui128 source, cui128 compare) { return _mm_testc_si128(source, compare); }

inline cbool AllTrue(cui128 source[3], cui128 compare[3]) {
   return (_mm_testc_si128(source[0], compare[0]) & _mm_testc_si128(source[1], compare[1]) & _mm_testc_si128(source[2], compare[2]));
}

inline cbool AllTrue(cui256 source, cui256 compare) { return _mm256_testc_si256(source, compare); }

inline cbool AllTrue(cui256 source[2], cui256 compare[2]) {
   return (_mm256_testc_si256(source[0], compare[0]) & _mm256_testc_si256(source[1], compare[1]));
}

inline cbool AllTrue(cui512 &source, cui512 &compare) {
   return (_mm256_testc_si256(((cui256ptr)&source)[0], ((cui256ptr)&compare)[0]) &
           _mm256_testc_si256(((cui256ptr)&source)[1], ((cui256ptr)&compare)[1]));
}

inline cbool AllFalse(cui128 source, cui128 compare) { return _mm_testz_si128(source, compare); }

inline cbool AllFalse(cui128 source[3], cui128 compare[3]) {
   return (_mm_testz_si128(source[0], compare[0]) & _mm_testz_si128(source[1], compare[1]) & _mm_testz_si128(source[2], compare[2]));
}

inline cbool AllFalse(cui256 source, cui256 compare) { return _mm256_testz_si256(source, compare); }

inline cbool AllFalse(cui256 source[2], cui256 compare[2]) {
   return (_mm256_testz_si256(source[0], compare[0]) & _mm256_testz_si256(source[1], compare[1]));
}

inline cbool AllFalse(cui512 &source, cui512 &compare) {
   return (_mm256_testz_si256(((cui256ptr)&source)[0], ((cui256ptr)&compare)[0]) &
           _mm256_testz_si256(((cui256ptr)&source)[1], ((cui256ptr)&compare)[1]));
}

#define RoundUpToNearest(x, A) (((x) + ((A) - 1)) & ~((A) - 1))
inline csi32 RoundUpToNearest4(csi32 input)  { return (input + 3)     & 0x0FFFFFFFC; }
inline cui32 RoundUpToNearest4(cui32 input)  { return (input + 3u)    & 0x0FFFFFFFCu; }
inline csi64 RoundUpToNearest4(csi64 input)  { return (input + 3ll)   & 0x0FFFFFFFFFFFFFFFCll; }
inline cui64 RoundUpToNearest4(cui64 input)  { return (input + 3ull)  & 0x0FFFFFFFFFFFFFFFCull; }
inline csi32 RoundUpToNearest8(csi32 input)  { return (input + 7)     & 0x0FFFFFFF8; }
inline cui32 RoundUpToNearest8(cui32 input)  { return (input + 7u)    & 0x0FFFFFFF8u; }
inline csi64 RoundUpToNearest8(csi64 input)  { return (input + 7ll)   & 0x0FFFFFFFFFFFFFFF8ll; }
inline cui64 RoundUpToNearest8(cui64 input)  { return (input + 7ull)  & 0x0FFFFFFFFFFFFFFF8ull; }
inline csi32 RoundUpToNearest16(csi32 input) { return (input + 15)    & 0x0FFFFFFF0; }
inline cui32 RoundUpToNearest16(cui32 input) { return (input + 15u)   & 0x0FFFFFFF0u; }
inline csi64 RoundUpToNearest16(csi64 input) { return (input + 15ll)  & 0x0FFFFFFFFFFFFFFF0ll; }
inline cui64 RoundUpToNearest16(cui64 input) { return (input + 15ull) & 0x0FFFFFFFFFFFFFFF0ull; }
inline csi32 RoundUpToNearest32(csi32 input) { return (input + 31)    & 0x0FFFFFFE0; }
inline cui32 RoundUpToNearest32(cui32 input) { return (input + 31u)   & 0x0FFFFFFE0u; }
inline csi64 RoundUpToNearest32(csi64 input) { return (input + 31ll)  & 0x0FFFFFFFFFFFFFFE0ll; }
inline cui64 RoundUpToNearest32(cui64 input) { return (input + 31ull) & 0x0FFFFFFFFFFFFFFE0ull; }
inline csi32 RoundUpToNearest64(csi32 input) { return (input + 63)    & 0x0FFFFFFC0; }
inline cui32 RoundUpToNearest64(cui32 input) { return (input + 63u)   & 0x0FFFFFFC0u; }
inline csi64 RoundUpToNearest64(csi64 input) { return (input + 63ll)  & 0x0FFFFFFFFFFFFFFC0ll; }
inline cui64 RoundUpToNearest64(cui64 input) { return (input + 63ull) & 0x0FFFFFFFFFFFFFFC0ull; }
