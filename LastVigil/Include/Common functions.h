/************************************************************
 * File: Common functions.h             Created: 2023/02/02 *
 *                                    Last mod.: 2023/05/28 *
 *                                                          *
 * Desc:                                                    *
 *                                                          *
 *  Copyright (c) David William Bull. All rights reserved.  *
 ************************************************************/
#pragma once

static cfl32 rcp1_5f   = 2.0f / 3.0f;
static cfl64 rcp1_5d   = 2.0  / 3.0;
static cfl32 rcp3f     = 1.0f / 3.0f;
static cfl64 rcp3d     = 1.0  / 3.0;
static cfl32 rcp6f     = 1.0f / 6.0f;
static cfl64 rcp6d     = 1.0  / 6.0;
static cfl32 rcp32768f = 1.0f / 32768.0f;
static cfl64 rcp32768d = 1.0  / 32768.0;
static cfl32 rcp65535f = 1.0f / 65535.0f;
static cfl64 rcp65535d = 1.0  / 65535.0;
static cfl32 rcp65536f = 1.0f / 65536.0f;
static cfl64 rcp65536d = 1.0  / 65536.0;

#include "typedefs.h"
#include "Vector structures.h"
#include "Fixed-point math.h"
// Move SYSTEM_DATA struct into unique include
#include "Data structures.h"

inline ptrc malloc16(cui64 byteCount) {
	ptrc pointer = _aligned_malloc(byteCount, 16);
	if(pointer) {
		sysData.memory.allocations++;
		sysData.memory.allocated += byteCount;
	}
	return pointer;
}

inline ptrc malloc32(cui64 byteCount) {
	ptrc pointer = _aligned_malloc(byteCount, 32);
	if(pointer) {
		sysData.memory.allocations++;
		sysData.memory.allocated += byteCount;
	}
	return pointer;
}

inline ptrc zalloc16(cui64 byteCount) {
	ptrc pointer = _aligned_malloc(byteCount, 16);
	if(pointer) {
		sysData.memory.allocations++;
		sysData.memory.allocated += byteCount;
		memset(pointer, 0, byteCount);
	}
	return pointer;
}

inline ptrc zalloc32(cui64 byteCount) {
	ptrc pointer = _aligned_malloc(byteCount, 32);
	if(pointer) {
		sysData.memory.allocations++;
		sysData.memory.allocated += byteCount;
		memset(pointer, 0, byteCount);
	}
	return pointer;
}

inline void swap(ui32 a, ui32 b) { a ^= b; b ^= a; a ^= b; }

inline cui32 rand_ui31(cui32 index) {
	cui32 i = (index << 13u) ^ index;

	return (i * (i * i * 15731u + 789221u) + 1376312589u) & 0x07FFFFFFF;
}

inline void transrotate(VEC2Df &coord, cfl32 angle, cfl32 dist) {
	coord.x -= dist * sinf(angle);
	coord.y += dist * cosf(angle);
}

inline cVEC2Df sincos(cfl32 x) {
	cfl32 fSinX = sinf(x);
	return { fSinX, sqrtf(1.0f - fSinX * fSinX) };
}

inline void sincos(cfl32 x, fl32 &sinval, fl32 &cosval) {
	cfl32 fSinX = sinf(x);
	sinval = fSinX;
	cosval = sqrtf(1.0f - fSinX * fSinX);
}

inline void sincos(cfl32 x, fl64 &sinval, fl64 &cosval) {
	cfl64 dSinX = sin(cfl64(x));
	sinval = dSinX;
	cosval = sqrt(1.0 - dSinX * dSinX);
}

inline void sincos(cfl64 x, fl64 &sinval, fl64 &cosval) {
	cfl64 dSinX = sin(x);
	sinval = dSinX;
	cosval = sqrt(1.0 - dSinX * dSinX);
}

inline void mov24(ui8 (&dest)[3], cui32 value) {
	(ui16addr)dest = value & 0x0FFFF;
	dest[2] = (value >> 16) & 0x0FF;
}

inline cfl32 Max(cVEC3Df &vector) { return max(max(vector.x, vector.y), vector.z); }

inline cfl32 Max3(cVEC4Df &vector) { return max(max(vector.x, vector.y), vector.z); }

inline cfl32 Max3(cSSE4Df32 &vector) { return max(max(vector.vector.x, vector.vector.y), vector.vector.z); }

inline cfl32 Max(cVEC4Df &vector) { return max(max(vector.x, vector.y), max(vector.z, vector.w)); }

inline cfl32 Max(cSSE4Df32 &vector) { return max(max(vector.vector.x, vector.vector.y), max(vector.vector.z, vector.vector.w)); }

inline cfl32 V3Magnitute(cfl32x4 vector) { return sqrtf(_mm_dp_ps(vector, vector, 0x071).m128_f32[0]); }

inline void V3NormaliseR(VEC3Df &vector, cfl32 magnitudeReciprocal) {
	vector.x *= magnitudeReciprocal;
	vector.y *= magnitudeReciprocal;
	vector.z *= magnitudeReciprocal;
}

inline void V3NormaliseR(fl32x4 &vector, cfl32 magnitudeReciprocal) {
	vector.m128_f32[0] *= magnitudeReciprocal;
	vector.m128_f32[1] *= magnitudeReciprocal;
	vector.m128_f32[2] *= magnitudeReciprocal;
}


inline void V3Normalise(SSE4Df32 &vector) {
	cfl32 magRCP = 1.0f / sqrtf(_mm_dp_ps(vector.xmm, vector.xmm, 0x071).m128_f32[0]);

	vector.vector.x *= magRCP;
	vector.vector.y *= magRCP;
	vector.vector.z *= magRCP;
}

inline void V3Normalise(fl32x4 &vector) {
	cfl32   mag       = sqrtf(_mm_dp_ps(vector, vector, 0x071).m128_f32[0]);
	cfl32x4 magnitude = _mm_load_ps1(&mag);

	vector = _mm_div_ps(vector, magnitude);
}

/*/ _Precise math_
*   1.0 / sqrt   ==  2893.2030ms  --  3814.0 -> 0.01619234153816938651
*     rsqrt      ==  2.26x faster --  3814.0 -> 0.01616492960118879121 : Deviation of 0.169577%
*
*  1.0f / sqrtf  ==  931.6588ms   --  3814.0 -> 0.01619234122335910797
*  _mm_rsqrt_ss  ==  1.56x faster --  3814.0 -> 0.01619338989257812500 : Deviation of 0.006476%
*
*   _Fast math_
*    1.0 / sqrt  ==  1451.0444ms  --  3814.0 -> 0.01619234153816938651
*      rsqrt     ==  1.13x faster --  3814.0 -> 0.01616492960118879121 : Deviation of 0.169577%
*
*   1.0f / sqrtf ==   524.9123ms  --  3814.0 -> 0.01619234122335910797
*   _mm_rsqrt_ss ==   588.1705ms  --  3814.0 -> 0.01619338989257812500 : Deviation of 0.006476%
*
/*/
inline cfl64 rsqrt(cfl64 number) {
	fl64  y  = number;
	cfl64 x2 = y * 0.5;
	ui64  i  = *(ui64ptrc)&y;

	i = 0x05fe6eb50c7b537a9 - (i >> 1);
	y = *(fl64 * const)&i;
	y = y * (1.5 - (x2 * y * y));

	return y;
}
