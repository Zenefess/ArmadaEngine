/************************************************************
 * File: vector structures.h            Created: 2022/12/05 *
 *                                Last modified: 2024/06/08 *
 *                                                          *
 * Notes: 2023/04/27: Added constant vector typedefs.       *
 *        2024/04/04: Added support for 24-bit integers.    *
 *        2024/05/18: Added AVX512 support.                 *
 *                                                          *
 * MIT license.            Copyright (c) David William Bull *
 ************************************************************/
#pragma once

#include "typedefs.h"

#define _VECTOR_STRUCTURES_

union VEC2Du8 {
   ui8 _ui8[2];
   struct {
      union { ui8 u, x, size; };
      union { ui8 v, y, intensity; };
   };
};

union VEC2Ds8 {
   si8 _si8[2];
   struct {
      union { si8 u, x, size; };
      union { si8 v, y, intensity; };
   };
};

union VEC2Du16 {
   ui16 _ui16[2];
   struct {
      union { ui16 u, x; };
      union { ui16 v, y; };
   };
};

union VEC2Ds16 {
   si16 _si16[2];
   struct {
      union { si16 u, x; };
      union { si16 v, y; };
   };
};
#ifdef _24BIT_INTEGERS_
al2 union VEC2Du24 {
   ui24 _ui24[2];
   struct {
      union { ui24 u, x; };
      union { ui24 v, y; };
   };
};

al2 union VEC2Ds24 {
   si24 _si24[2];
   struct {
      union { si24 u, x; };
      union { si24 v, y; };
   };
};
#endif
union VEC2Du32 {
   ui32 _ui32[2];
   struct {
      union { ui32 u, x; };
      union { ui32 v, y; };
   };
};

union VEC2Ds32 {
   si32 _si32[2];
   struct {
      union { si32 u, x, chunk; };
      union { si32 v, y, map; };
   };
};

union VEC2Du64 {
   ui64 _ui64[2];
   struct {
      union { ui64 u, x; };
      union { ui64 v, y; };
   };
};

union VEC2Ds64 {
   si64 _si64[2];
   struct {
      union { si64 u, x, chunk; };
      union { si64 v, y, map; };
   };
};

union VEC2Dh {
   fl16 _si16[2];
   struct {
      union { fl16 u, x; };
      union { fl16 v, y; };
   };
};

union VEC2Df {
   fl32 _fl32[2];
   struct {
      union { fl32 u, x; };
      union { fl32 v, y; };
   };
};

union VEC2Dd {
   fl64 _fl64[2];
   struct {
      union { fl64 u, x; };
      union { fl64 v, y; };
   };
};

union VEC3Du8 {
   ui8 _ui8[3];
   struct {
      union { ui8 r, x; };
      union { ui8 g, y; };
      union { ui8 b, z; };
   };
};

union VEC3Ds8 {
   si8 _si8[3];
   struct {
      union { si8 r, x; };
      union { si8 g, y; };
      union { si8 b, z; };
   };
};

union VEC3Du16 {
   ui16 _ui16[3];
   struct {
      union { ui16 r, x; };
      union { ui16 g, y; };
      union { ui16 b, z; };
   };
};

union VEC3Ds16 {
   si16 _si16[3];
   struct {
      union { si16 r, x; };
      union { si16 g, y; };
      union { si16 b, z; };
   };
};
#ifdef _24BIT_INTEGERS_
union VEC3Du24 {
   ui24 _ui24[3];
   struct {
      union { ui24 r, x; };
      union { ui24 g, y; };
      union { ui24 b, z; };
   };
};

union VEC3Ds24 {
   si24 _si24[3];
   struct {
      union { si24 r, x; };
      union { si24 g, y; };
      union { si24 b, z; };
   };
};
#endif
union VEC3Du32 {
   ui32 _ui32[3];
   struct {
      union { ui32 r, x; };
      union { ui32 g, y; };
      union { ui32 b, z; };
   };
};

union VEC3Ds32 {
   si32 _si32[3];
   struct {
      union { si32 r, x; };
      union { si32 g, y; };
      union { si32 b, z; };
   };
};

union VEC3Du64 {
   ui64 _ui64[3];
   struct {
      union { ui64 r, x; };
      union { ui64 g, y; };
      union { ui64 b, z; };
   };
};

union VEC3Ds64 {
   si64 _si64[3];
   struct {
      union { si64 r, x; };
      union { si64 g, y; };
      union { si64 b, z; };
   };
};

union VEC3Dh {
   fl16 _fl16[3];
   struct {
      union { fl16 r, x; };
      union { fl16 g, y; };
      union { fl16 b, z; };
   };
};

union VEC3Df {
   fl32 _fl32[3];
   struct {
      union { fl32 r, x; };
      union { fl32 g, y; };
      union { fl32 b, z; };
   };
};

union VEC3Dd {
   fl64 _fl64[3];
   struct {
      union { fl64 r, x; };
      union { fl64 g, y; };
      union { fl64 b, z; };
   };
};

union VEC4Du8 {
   ui8 _ui8[4];
   struct {
      union { ui8 r, u1, x, x1, xSeg; };
      union { ui8 g, v1, y, y1, xDivs; };
      union { ui8 b, u2, z, x2, ySeg; };
      union { ui8 a, v2, w, y2, yDivs; };
   };
};

union VEC4Ds8 {
   si8 _si8[4];
   struct {
      union { si8 r, u1, x, x1, xSeg; };
      union { si8 g, v1, y, y1, xDivs; };
      union { si8 b, u2, z, x2, ySeg; };
      union { si8 a, v2, w, y2, yDivs; };
   };
};

union VEC4Du16 {
   ui16 _ui16[4];
   struct {
      union { ui16 r, u1, x, x1; };
      union { ui16 g, v1, y, y1; };
      union { ui16 b, u2, z, x2; };
      union { ui16 a, v2, w, y2; };
   };
};

union VEC4Ds16 {
   si16 _si16[4];
   struct {
      union { si16 r, u1, x, x1; };
      union { si16 g, v1, y, y1; };
      union { si16 b, u2, z, x2; };
      union { si16 a, v2, w, y2; };
   };
};
#ifdef _24BIT_INTEGERS_
al4 union VEC4Du24 {
   ui24 _ui24[4];
   struct {
      union { ui24 r, u1, x, x1; };
      union { ui24 g, v1, y, y1; };
      union { ui24 b, u2, z, x2; };
      union { ui24 a, v2, w, y2; };
   };
};

al4 union VEC4Ds24 {
   si24 _si24[4];
   struct {
      union { si24 r, u1, x, x1; };
      union { si24 g, v1, y, y1; };
      union { si24 b, u2, z, x2; };
      union { si24 a, v2, w, y2; };
   };
};
#endif
union VEC4Du32 {
   ui32 _ui32[4];
   struct {
      union { ui32 r, u1, x, x1; };
      union { ui32 g, v1, y, y1; };
      union { ui32 b, u2, z, x2; };
      union { ui32 a, v2, w, y2; };
   };
};

union VEC4Ds32 {
   si32 _si32[4];
   struct {
      union { si32 r, u1, x, x1; };
      union { si32 g, v1, y, y1; };
      union { si32 b, u2, z, x2; };
      union { si32 a, v2, w, y2; };
   };
};

union VEC4Du64 {
   ui64 _ui64[4];
   struct {
      union { ui64 r, u1, x, x1; };
      union { ui64 g, v1, y, y1; };
      union { ui64 b, u2, z, x2; };
      union { ui64 a, v2, w, y2; };
   };
};

union VEC4Ds64 {
   si64 _si64[4];
   struct {
      union { si64 r, u1, x, x1; };
      union { si64 g, v1, y, y1; };
      union { si64 b, u2, z, x2; };
      union { si64 a, v2, w, y2; };
   };
};

union VEC4Dh {
   fl16 _fl16[4];
   struct {
      union { fl16 r, u1, x, x1; };
      union { fl16 g, v1, y, y1; };
      union { fl16 b, u2, z, x2; };
      union { fl16 a, v2, w, y2; };
   };
};

union VEC4Df {
   fl32 _fl32[4];
   struct {
      union { fl32 r, u1, x, x1; };
      union { fl32 g, v1, y, y1; };
      union { fl32 b, u2, z, x2; };
      union { fl32 a, v2, w, y2; };
   };
};

union VEC4Dd {
   fl64 _fl64[4];
   struct {
      union { fl64 r, u1, x, x1; };
      union { fl64 g, v1, y, y1; };
      union { fl64 b, u2, z, x2; };
      union { fl64 a, v2, w, y2; };
   };
};

union VEC6Df {
   fl32 _fl32[6];
   struct {
      union { VEC3Df x; };
      union { VEC3Df y; };
   };
   struct {
      VEC2Df x2, y2, z2;
   };
};

union VEC6Dd {
   fl64 _fl64[6];
   struct {
      union { VEC3Dd x; };
      union { VEC3Dd y; };
   };
   struct {
      VEC2Dd x2, y2, z2;
   };
};

union VEC8Du8 {
   ui8 _ui8[8];
   struct {
      union { VEC4Du8 x, p, f, s; };
      union { VEC4Du8 y, o, u, v, r; };
   };
   struct {
      VEC2Du8 x2, y2, z2, w2;
   };
};

union VEC8Ds8 {
   si8 _si8[8];
   struct {
      union { VEC4Ds8 x, p, f, s; };
      union { VEC4Ds8 y, o, u, v, r; };
   };
   struct {
      VEC2Ds8 x2, y2, z2, w2;
   };
};

union VEC8Du32 {
   ui32 _ui32[8];
   struct {
      union { VEC4Du32 x, p, f, s; };
      union { VEC4Du32 y, o, u, v, r; };
   };
   struct {
      VEC2Du32 x2, y2, z2, w2;
   };
};

union VEC8Ds32 {
   si32 _si32[8];
   struct {
      union { VEC4Ds32 x, p, f, s; };
      union { VEC4Ds32 y, o, u, v, r; };
   };
   struct {
      VEC2Ds32 x2, y2, z2, w2;
   };
};

union VEC8Du64 {
   ui64 _ui64[8];
   struct {
      union { VEC4Du64 x, p, f, s; };
      union { VEC4Du64 y, o, u, v, r; };
   };
   struct {
      VEC2Du64 x2, y2, z2, w2;
   };
};

union VEC8Ds64 {
   si64 _si64[8];
   struct {
      union { VEC4Ds64 x, p, f, s; };
      union { VEC4Ds64 y, o, u, v, r; };
   };
   struct {
      VEC2Ds64 x2, y2, z2, w2;
   };
};

union VEC8Dh {
   fl16 _fl16[8];
   struct {
      union { VEC4Dh x, p, f, s; };
      union { VEC4Dh y, o, u, v, r; };
   };
   struct {
      VEC2Dh x2, y2, z2, w2;
   };
};

union VEC8Df {
   fl32 _fl32[8];
   struct {
      union { VEC4Df x, p, f, s; };
      union { VEC4Df y, o, u, v, r; };
    };
   struct {
      VEC2Df x2, y2, z2, w2;
   };
};

union VEC8Dd {
   fl64 _fl64[8];
   struct {
      union { VEC4Dd x, p, f, s; };
      union { VEC4Dd y, o, u, v, r; };
   };
   struct {
      VEC2Dd x2, y2, z2, w2;
   };
};

union VEC16Dh {
   fl16 _fl16[16];
   struct {
      union { VEC4Dh r, u1, x, x1; };
      union { VEC4Dh g, v1, y, y1; };
      union { VEC4Dh b, u2, z, x2; };
      union { VEC4Dh a, v2, w, y2; };
   };
};

// SSE2 & AVX2 vector structs
union SSE2Du32 {
   ui32     _ui32[2];
   __m64    mm;
   VEC2Du32 vector;
   struct {
      union { ui32 u, x; };
      union { ui32 v, y; };
   };
};

union SSE2Ds32 {
   si32     _si32[2];
   __m64    mm;
   VEC2Ds32 vector;
   struct {
      union { si32 u, x; };
      union { si32 v, y; };
   };
};

union SSE2Du64 {
   __m128i  xmm;
   VEC2Du64 vector;
   ui64     _ui64[2];
   struct {
      union { ui64 u, x; };
      union { ui64 v, y; };
   };
};

union SSE2Ds64 {
   __m128i  xmm;
   VEC2Du64 vector;
   si64     _si64[2];
   struct {
      union { si64 u, x; };
      union { si64 v, y; };
   };
};

union SSE2Df32 {
   fl32   _fl32[2];
   __m64  mm;
   VEC2Df vector;
   struct {
      union { fl32 u, x; };
      union { fl32 v, y; };
   };
};

union SSE2Df64 {
   __m128 xmm;
   VEC2Dd vector;
   fl64   _fl64[2];
   struct {
      union { fl64 u, x; };
      union { fl64 v, y; };
   };
};

union SSE4Du16 {
   ui16     _ui16[4];
   __m64    xmm;
   VEC4Du16 vector;
   struct {
      union { ui16 u, x; };
      union { ui16 v, y; };
   };
};

union SSE4Ds16 {
   si16     _si16[4];
   __m64    xmm;
   VEC4Du16 vector;
   struct {
      union { si16 u, x; };
      union { si16 v, y; };
   };
};

union SSE4Du32 {
   __m128i  xmm;
   VEC4Du32 vector;
   ui32     _ui32[4];
   struct {
      union { ui32 r, u1, x, x1; };
      union { ui32 g, v1, y, y1; };
      union { ui32 b, u2, z, x2; };
      union { ui32 a, v2, w, y2; };
   };
};

union SSE4Ds32 {
   __m128i  xmm;
   VEC4Ds32 vector;
   si32     _si32[4];
   struct {
      union { si32 r, u1, x, x1; };
      union { si32 g, v1, y, y1; };
      union { si32 b, u2, z, x2; };
      union { si32 a, v2, w, y2; };
   };
};

union SSE4Df32 {
   __m128 xmm;
   VEC4Df vector;
   fl32   _fl32[4];
   struct {
      union { fl32 r, u1, x, x1; };
      union { fl32 g, v1, y, y1; };
      union { fl32 b, u2, z, x2; };
      union { fl32 a, v2, w, y2; };
   };
};

union SSE8Df16 {
   __m128h xmm;
   VEC8Dh  vector[2];
   fl16    _fl16[8];
};

union SSE16Du8 {
   __m128i xmm;
   VEC4Du8 vector[4];
   ui8     _ui8[16];
};

union SSE16Ds8 {
   __m128i xmm;
   VEC4Ds8 vector[4];
   si8     _si8[16];
};

union AVX4Du64 {
   __m256i  ymm;
   __m128i  xmm[2];
   VEC4Du64 vector;
   ui64     _ui64[4];
   struct {
      union { ui64 r, u1, x, x1; };
      union { ui64 g, v1, y, y1; };
      union { ui64 b, u2, z, x2; };
      union { ui64 a, v2, w, y2; };
   };
};

union AVX4Ds64 {
   __m256i  ymm;
   __m128i  xmm[2];
   VEC4Ds64 vector;
   si64     _si64[4];
   struct {
      union { si64 r, u1, x, x1; };
      union { si64 g, v1, y, y1; };
      union { si64 b, u2, z, x2; };
      union { si64 a, v2, w, y2; };
   };
};

union AVX4Df64 {
   __m256 ymm;
   __m128 xmm[2];
   VEC4Dd vector;
   fl64   _fl64[4];
   struct {
      union { fl64 r, u1, x, x1; };
      union { fl64 g, v1, y, y1; };
      union { fl64 b, u2, z, x2; };
      union { fl64 a, v2, w, y2; };
   };
};

union AVX8Du32 {
   __m256i  ymm;
   __m128i  xmm[2];
   VEC8Du32 vector[2];
   ui32     _ui32[8];
   struct {
      union { VEC4Du32 x, p, f, s; };
      union { VEC4Du32 y, o, u, v, r; };
   };
   struct {
      VEC2Du32 x2, y2, z2, w2;
   };
};

union AVX8Ds32 {
   __m256i  ymm;
   __m128i  xmm[2];
   struct { __m128i xmm0, xmm1; };
   VEC8Ds32 vector;
   si32     _si32[8];
   struct {
      union { VEC4Du32 x, p, f, s; };
      union { VEC4Du32 y, o, u, v, r; };
   };
   struct {
      VEC2Du32 x2, y2, z2, w2;
   };
};

union AVX8Df32 {
   __m256 ymm;
   __m128 xmm[2];
   struct { __m128 xmm0, xmm1; };
   VEC8Df vector;
   fl32   _fl[8];
   struct {
      union { VEC4Df x, p, f, s; };
      union { VEC4Df y, o, u, v, r; };
   };
   struct {
      VEC2Df x2, y2, z2, w2;
   };
};

union AVX16Du16 {
   __m256i  ymm;
   __m128i  xmm[2];
   VEC4Du16 vector[4];
   ui16     _ui16[16];
};

union AVX16Ds16 {
   __m256i  ymm;
   __m128i  xmm[2];
   VEC4Ds16 vector[4];
   si16     _si16[16];
};

union AVX16Df16 {
   __m256bh ymm;
   __m128bh xmm[2];
   VEC16Dh  vector;
   fl16     _fl16[16];
};

union AVX32Du8 {
   __m256i ymm;
   __m128i xmm[2];
   VEC4Du8 vector[8];
   ui8     _ui16[32];
};

union AVX32Ds8 {
   __m256i ymm;
   __m128i xmm[2];
   VEC4Ds8 vector[8];
   si8     _si16[32];
};

union AVX8Du64 {
   __m512i  zmm;
   __m256i  ymm[2];
   __m128i  xmm[4];
   VEC8Du64 vector;
   ui64     _ui64[8];
   struct {
      union { VEC4Du64 x, p, f, s; };
      union { VEC4Du64 y, o, u, v, r; };
   };
   struct {
      VEC2Du64 x2, y2, z2, w2;
   };
};

union AVX8Ds64 {
   __m512i  zmm;
   __m256i  ymm;
   __m128i  xmm[2];
   VEC8Ds64 vector;
   si64     _si64[8];
   struct {
      union { VEC4Ds64 x, p, f, s; };
      union { VEC4Ds64 y, o, u, v, r; };
   };
   struct {
      VEC2Ds64 x2, y2, z2, w2;
   };
};

union AVX8Df64 {
   __m512 zmm;
   __m256 ymm[2];
   __m128 xmm[4];
   VEC8Dd vector;
   fl64   _fl64[4];
   struct {
      union { VEC4Dd x, p, f, s; };
      union { VEC4Dd y, o, u, v, r; };
   };
   struct {
      VEC2Dd x2, y2, z2, w2;
   };
};

union AVX16Du32 {
   __m512i  zmm;
   __m256i  ymm[2];
   __m128i  xmm[4];
   VEC2Du32 vec2D[8];
   VEC8Du32 vec8D[2];
   ui32     _ui32[16];
};

union AVX16Ds32 {
   __m512i  zmm;
   __m256i  ymm[2];
   __m128i  xmm[4];
   VEC2Ds32 vec2D[8];
   VEC8Ds32 vec8D[2];
   si32     _si32[16];
};

union AVX16Df32 {
   __m512 zmm;
   __m256 ymm[2];
   __m128 xmm[4];
   VEC2Df vec2D[8];
   VEC8Df vec8D[2];
   fl32   _fl[16];
};

union AVX32Du16 {
   __m512i  zmm;
   __m256i  ymm[2];
   __m128i  xmm[4];
   VEC4Du16 vector[8];
   ui16     _ui16[32];
};

union AVX32Ds16 {
   __m512i  zmm;
   __m256i  ymm[2];
   __m128i  xmm[4];
   VEC4Ds16 vector[8];
   si16     _si16[32];
};

union AVX32Df16 {
   __m512bh zmm;
   __m256bh ymm[2];
   __m128bh xmm[4];
   VEC16Dh  vector[2];
   fl16     _fl16[32];
};

union AVX64Du8 {
   __m512i zmm;
   __m256i ymm[2];
   __m128i xmm[4];
   VEC4Du8 vector[8];
   ui8     _ui8[64];
};

union AVX64Ds8 {
   __m512i zmm;
   __m256i ymm[2];
   __m128i xmm[4];
   VEC4Ds8 vector[8];
   si8     _si8[64];
};

al32 union AVXmatrix {
   __m256 ymm[2];
   __m128 xmm[4];
   VEC4Df vector[4];
   fl32   fl[16];
};

al64 union AVX512matrix {
   __m512 zmm;
   __m256 ymm[2];
   __m128 xmm[4];
   VEC4Df vector[4];
   fl32   fl[16];
};

// Constant vector types
typedef const VEC2Du8   cVEC2Du8;
typedef const VEC2Ds8   cVEC2Ds8;
typedef const VEC2Du16  cVEC2Du16;
typedef const VEC2Ds16  cVEC2Ds16;
#ifdef _24BIT_INTEGERS_
typedef const VEC2Du24  cVEC2Du24;
typedef const VEC2Ds24  cVEC2Ds24;
#endif
typedef const VEC2Du32  cVEC2Du32;
typedef const VEC2Ds32  cVEC2Ds32;
typedef const VEC2Du64  cVEC2Du64;
typedef const VEC2Ds64  cVEC2Ds64;
typedef const VEC2Df    cVEC2Df;
typedef const VEC2Dd    cVEC2Dd;
typedef const VEC3Du8   cVEC3Du8;
typedef const VEC3Ds8   cVEC3Ds8;
typedef const VEC3Du16  cVEC3Du16;
typedef const VEC3Ds16  cVEC3Ds16;
#ifdef _24BIT_INTEGERS_
typedef const VEC3Du24  cVEC3Du24;
typedef const VEC3Ds24  cVEC3Ds24;
#endif
typedef const VEC3Du32  cVEC3Du32;
typedef const VEC3Ds32  cVEC3Ds32;
typedef const VEC3Du64  cVEC3Du64;
typedef const VEC3Ds64  cVEC3Ds64;
typedef const VEC3Df    cVEC3Df;
typedef const VEC3Dd    cVEC3Dd;
typedef const VEC4Du8   cVEC4Du8;
typedef const VEC4Ds8   cVEC4Ds8;
typedef const VEC4Du16  cVEC4Du16;
typedef const VEC4Ds16  cVEC4Ds16;
#ifdef _24BIT_INTEGERS_
typedef const VEC4Du24  cVEC4Du24;
typedef const VEC4Ds24  cVEC4Ds24;
#endif
typedef const VEC4Du32  cVEC4Du32;
typedef const VEC4Ds32  cVEC4Ds32;
typedef const VEC4Du64  cVEC4Du64;
typedef const VEC4Ds64  cVEC4Ds64;
typedef const VEC4Df    cVEC4Df;
typedef const VEC4Dd    cVEC4Dd;
typedef const VEC6Df    cVEC6Df;
typedef const VEC6Dd    cVEC6Dd;
typedef const VEC8Du8   cVEC8Du8;
typedef const VEC8Ds8   cVEC8Ds8;
typedef const VEC8Du32  cVEC8Du32;
typedef const VEC8Ds32  cVEC8Ds32;
typedef const VEC8Dh    cVEC8Dh;
typedef const VEC8Df    cVEC8Df;
typedef const VEC8Dd    cVEC8Dd;
typedef const VEC16Dh   cVEC16Dh;

typedef const SSE2Du32  cSSE2Du32;
typedef const SSE2Ds32  cSSE2Ds32;
typedef const SSE2Du64  cSSE2Du64;
typedef const SSE2Df32  cSSE2Df32;
typedef const SSE2Df64  cSSE2Df64;
typedef const SSE4Du32  cSSE4Du32;
typedef const SSE4Ds32  cSSE4Ds32;
typedef const SSE4Df32  cSSE4Df32;
typedef const SSE8Df16  cSSE8Df16;
typedef const AVX4Du64  cAVX4Du64;
typedef const AVX4Ds64  cAVX4Ds64;
typedef const AVX4Df64  cAVX4Df64;
typedef const AVX8Du32  cAVX8Du32;
typedef const AVX8Ds32  cAVX8Ds32;
typedef const AVX8Du64  cAVX8Du64;
typedef const AVX8Ds64  cAVX8Ds64;
typedef const AVX8Df32  cAVX8Df32;
typedef const AVX8Df64  cAVX8Df64;
typedef const AVX16Du16 cAVX16Du16;
typedef const AVX16Ds16 cAVX16Ds16;
typedef const AVX16Du32 cAVX16Du32;
typedef const AVX16Ds32 cAVX16Ds32;
typedef const AVX16Df16 cAVX16Df16;
typedef const AVX16Df32 cAVX16Df32;
typedef const AVX32Du8  cAVX32Du8;
typedef const AVX32Ds8  cAVX32Ds8;
typedef const AVX32Du16 cAVX32Du16;
typedef const AVX32Ds16 cAVX32Ds16;
typedef const AVX32Df16 cAVX32Df16;
typedef const AVX64Du8  cAVX64Du8;
typedef const AVX64Ds8  cAVX64Ds8;

typedef const AVXmatrix    cAVXmatrix;
typedef const AVX512matrix cAVX512matrix;

// Volatile vector types
typedef vol VEC2Du8   vVEC2Du8;
typedef vol VEC2Ds8   vVEC2Ds8;
typedef vol VEC2Du16  vVEC2Du16;
typedef vol VEC2Ds16  vVEC2Ds16;
#ifdef _24BIT_INTEGERS_
typedef vol VEC2Du24  vVEC2Du24;
typedef vol VEC2Ds24  vVEC2Ds24;
#endif
typedef vol VEC2Du32  vVEC2Du32;
typedef vol VEC2Ds32  vVEC2Ds32;
typedef vol VEC2Du64  vVEC2Du64;
typedef vol VEC2Ds64  vVEC2Ds64;
typedef vol VEC2Df    vVEC2Df;
typedef vol VEC2Dd    vVEC2Dd;
typedef vol VEC3Du8   vVEC3Du8;
typedef vol VEC3Ds8   vVEC3Ds8;
typedef vol VEC3Du16  vVEC3Du16;
typedef vol VEC3Ds16  vVEC3Ds16;
#ifdef _24BIT_INTEGERS_
typedef vol VEC3Du24  vVEC3Du24;
typedef vol VEC3Ds24  vVEC3Ds24;
#endif
typedef vol VEC3Du32  vVEC3Du32;
typedef vol VEC3Ds32  vVEC3Ds32;
typedef vol VEC3Du64  vVEC3Du64;
typedef vol VEC3Ds64  vVEC3Ds64;
typedef vol VEC3Df    vVEC3Df;
typedef vol VEC3Dd    vVEC3Dd;
typedef vol VEC4Du8   vVEC4Du8;
typedef vol VEC4Ds8   vVEC4Ds8;
typedef vol VEC4Du16  vVEC4Du16;
typedef vol VEC4Ds16  vVEC4Ds16;
#ifdef _24BIT_INTEGERS_
typedef vol VEC4Du24  vVEC4Du24;
typedef vol VEC4Ds24  vVEC4Ds24;
#endif
typedef vol VEC4Du32  vVEC4Du32;
typedef vol VEC4Ds32  vVEC4Ds32;
typedef vol VEC4Du64  vVEC4Du64;
typedef vol VEC4Ds64  vVEC4Ds64;
typedef vol VEC4Df    vVEC4Df;
typedef vol VEC4Dd    vVEC4Dd;
typedef vol VEC6Df    vVEC6Df;
typedef vol VEC6Dd    vVEC6Dd;
typedef vol VEC8Du32  vVEC8Du32;
typedef vol VEC8Ds32  vVEC8Ds32;
typedef vol VEC8Dh    vVEC8Dh;
typedef vol VEC8Df    vVEC8Df;
typedef vol VEC8Dd    vVEC8Dd;
typedef vol VEC16Dh   vVEC16Dh;

typedef vol SSE2Du32  vSSE2Du32;
typedef vol SSE2Ds32  vSSE2Ds32;
typedef vol SSE2Du64  vSSE2Du64;
typedef vol SSE2Df32  vSSE2Df32;
typedef vol SSE2Df64  vSSE2Df64;
typedef vol SSE4Du32  vSSE4Du32;
typedef vol SSE4Ds32  vSSE4Ds32;
typedef vol SSE4Df32  vSSE4Df32;
typedef vol SSE8Df16  vSSE8Df16;
typedef vol AVX4Du64  vAVX4Du64;
typedef vol AVX4Ds64  vAVX4Ds64;
typedef vol AVX4Df64  vAVX4Df64;
typedef vol AVX8Du32  vAVX8Du32;
typedef vol AVX8Ds32  vAVX8Ds32;
typedef vol AVX8Du64  vAVX8Du64;
typedef vol AVX8Ds64  vAVX8Ds64;
typedef vol AVX8Df32  vAVX8Df32;
typedef vol AVX8Df64  vAVX8Df64;
typedef vol AVX16Du16 vAVX16Du16;
typedef vol AVX16Ds16 vAVX16Ds16;
typedef vol AVX16Du32 vAVX16Du32;
typedef vol AVX16Ds32 vAVX16Ds32;
typedef vol AVX16Df16 vAVX16Df16;
typedef vol AVX16Df32 vAVX16Df32;
typedef vol AVX32Du8  vAVX32Du8;
typedef vol AVX32Ds8  vAVX32Ds8;
typedef vol AVX32Du16 vAVX32Du16;
typedef vol AVX32Ds16 vAVX32Ds16;
typedef vol AVX32Df16 vAVX32Df16;
typedef vol AVX64Du8  vAVX64Du8;
typedef vol AVX64Ds8  vAVX64Ds8;

typedef vol AVXmatrix    vAVXmatrix;
typedef vol AVX512matrix vAVX512matrix;
