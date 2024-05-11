/************************************************************
 * File: Common functions.h             Created: 2023/02/02 *
 *                                    Last mod.: 2024/05/03 *
 *                                                          *
 * Desc:                                                    *
 *                                                          *
 *                         Copyright (c) David William Bull *
 ************************************************************/
#pragma once

#include "typedefs.h"
#include "Vector structures.h"
#include "Memory management.h"
#include "Fixed-point math.h"
#include "Data structures.h"

#define _COMMON_FUNCTIONS_

static cfl32 rcp1_5f   = 2.0f / 3.0f;
static cfl32 rcp3f     = 1.0f / 3.0f;
static cfl32 rcp6f     = 1.0f / 6.0f;
static cfl32 rcp32768f = 1.0f / 32768.0f;
static cfl32 rcp65535f = 1.0f / 65535.0f;
static cfl32 rcp65536f = 1.0f / 65536.0f;

static cfl64 rcp1_5d   = 2.0  / 3.0;
static cfl64 rcp3d     = 1.0  / 3.0;
static cfl64 rcp6d     = 1.0  / 6.0;
static cfl64 rcp32768d = 1.0  / 32768.0;
static cfl64 rcp65535d = 1.0  / 65535.0;
static cfl64 rcp65536d = 1.0  / 65536.0;

static cVEC3Df null3Df = { 0.0f, 0.0f, 0.0f };

static cfl32x4 null128f = { .m128_u64 = { 0, 0 } };
static cui128  null128  = { .m128i_u64 = { 0x0, 0x0 } };
static cui128  max128   = { .m128i_i64 = { -1, -1 } };

static cui256 null256  = { .m256i_u64 = { 0x0, 0x0, 0x0, 0x0 } };
static cui256 ones32x8 = { .m256i_i32 = { 1, 1, 1, 1, 1, 1, 1, 1 } };
static cui256 max256   = { .m256i_i64 = { -1, -1, -1, -1 } };

inline void swap(ui8 &a, ui8 &b) { a ^= b; b ^= a; a ^= b; }

inline void swap(ui16 &a, ui16 &b) { a ^= b; b ^= a; a ^= b; }

inline void swap(ui32 &a, ui32 &b) { a ^= b; b ^= a; a ^= b; }

inline void swap(ui64 &a, ui64 &b) { a ^= b; b ^= a; a ^= b; }

inline void swap(fl32 &a, fl32 &b) {
   (ui32 &)a ^= (ui32 &)b;
   (ui32 &)b ^= (ui32 &)a;
   (ui32 &)a ^= (ui32 &)b;
}

inline void swap(fl64 &a, fl64 &b) {
   (ui64 &)a ^= (ui64 &)b;
   (ui64 &)b ^= (ui64 &)a;
   (ui64 &)a ^= (ui64 &)b;
}

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

inline cVEC2Df sincos(cfl32 angle) {
   cfl32 fSinX = sinf(angle);
   return { fSinX, sqrtf(1.0f - fSinX * fSinX) };
}

inline void sincos(cfl32 angle, fl32 &sinval, fl32 &cosval) {
   cfl32 fSinX = sinf(angle);
   sinval = fSinX;
   cosval = sqrtf(1.0f - fSinX * fSinX);
}

inline void sincos(cfl32 angle, fl64 &sinval, fl64 &cosval) {
   cfl64 dSinX = sin(cfl64(angle));
   sinval = dSinX;
   cosval = sqrt(1.0 - dSinX * dSinX);
}

inline void sincos(cfl64 angle, fl64 &sinval, fl64 &cosval) {
   cfl64 dSinX = sin(angle);
   sinval = dSinX;
   cosval = sqrt(1.0 - dSinX * dSinX);
}

inline void mov24(ui8 (&dest)[3], cui32 value) {
   (ui16 &)dest = value & 0x0FFFF;

   dest[2] = (value >> 16) & 0x0FF;
}

inline void mulV3(VEC3Df &vector, cfl32 multiplier) {
   vector.x *= multiplier;
   vector.y *= multiplier;
   vector.z *= multiplier;
}

inline void mulV3(fl32x4 &vector, cfl32 multiplier) {
   vector = _mm_mul_ps(vector, cfl32x4{ multiplier, multiplier, multiplier, 1.0f });
}

inline cfl32 Max(cVEC3Df &vector) { return max(max(vector.x, vector.y), vector.z); }

inline cfl32 Max3(cVEC4Df &vector) { return max(max(vector.x, vector.y), vector.z); }

inline cfl32 Max3(cSSE4Df32 &vector) { return max(max(vector.vector.x, vector.vector.y), vector.vector.z); }

inline cfl32 Max(cVEC4Df &vector) { return max(max(vector.x, vector.y), max(vector.z, vector.w)); }

inline cfl32 Max(cSSE4Df32 &vector) { return max(max(vector.vector.x, vector.vector.y), max(vector.vector.z, vector.vector.w)); }

inline cfl32 magnituteV3(cfl32x4 vector) { return sqrtf(_mm_dp_ps(vector, vector, 0x071).m128_f32[0]); }

inline void normaliseV3(fl32x4 &vector) {
   cfl32 mag = sqrtf(_mm_dp_ps(vector, vector, 0x071).m128_f32[0]);

   vector = _mm_div_ps(vector, cfl32x4{ mag, mag, mag, 1.0f });
}

inline void normaliseV3(SSE4Df32 &vector) {
   cfl32 mag = sqrtf(_mm_dp_ps(vector.xmm, vector.xmm, 0x071).m128_f32[0]);

   vector.xmm = _mm_div_ps(vector.xmm, cfl32x4{ mag, mag, mag, 1.0f });
}

inline void normaliseV4(fl32x4 &vector) {
   cfl32 mag = sqrtf(_mm_dp_ps(vector, vector, 0x071).m128_f32[0]);

   vector = _mm_div_ps(vector, _mm_set_ps1(mag));
}

inline void normaliseV4(SSE4Df32 &vector) {
   cfl32 mag = sqrtf(_mm_dp_ps(vector.xmm, vector.xmm, 0x071).m128_f32[0]);

   vector.xmm = _mm_div_ps(vector.xmm, _mm_set_ps1(mag));
}

inline cui32 rand_ui31(cui32 index) {
   cui32 i = (index << 13u) ^ index;

   return (i * (i * i * 15731u + 789221u) + 1376312589u) & 0x07FFFFFFF;
}
