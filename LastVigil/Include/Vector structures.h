/************************************************************
 * File: Vector structures.h            Created: 2022/12/05 *
 *                                Last modified: 2024/04/04 *
 *                                                          *
 * Notes: 2023/04/27: Added constant vector typedefs.       *
 *        2024/04/04: Added ui24 vector types.              *
 *                                                          *
 *  Copyright (c) David William Bull. All rights reserved.  *
 ************************************************************/
#pragma once

#include "typedefs.h"

struct VEC2Du8 {
   union {
      ui8 _ui8[2];
      struct {
         union { ui8 u, x, size; };
         union { ui8 v, y, intensity; };
      };
   };
};

struct VEC2Ds8 {
   union {
      si8 _si8[2];
      struct {
         union { si8 u, x, size; };
         union { si8 v, y, intensity; };
      };
   };
};

struct VEC2Du16 {
   union {
      ui16 _ui16[2];
      struct {
         union { ui16 u, x; };
         union { ui16 v, y; };
      };
   };
};

struct VEC2Ds16 {
   union {
      si16 _si16[2];
      struct {
         union { si16 u, x; };
         union { si16 v, y; };
      };
   };
};

al2 struct VEC2Du24 {
   union {
      ui24 _ui24[2];
      struct {
         union { ui24 u, x; };
         union { ui24 v, y; };
      };
   };
};

struct VEC2Du32 {
   union {
      ui32 _ui32[2];
      struct {
         union { ui32 u, x; };
         union { ui32 v, y; };
      };
   };
};

struct VEC2Ds32 {
   union {
      si32 _si32[2];
      struct {
         union { si32 u, x, chunk; };
         union { si32 v, y, map; };
      };
   };
};

struct VEC2Du64 {
   union {
      ui64 _ui64[2];
      struct {
         union { ui64 u, x; };
         union { ui64 v, y; };
      };
   };
};

struct VEC2Ds64 {
   union {
      si64 _si64[2];
      struct {
         union { si64 u, x, chunk; };
         union { si64 v, y, map; };
      };
   };
};

struct VEC2Df {
   union {
      fl32 _fl32[2];
      struct {
         union { fl32 u, x; };
         union { fl32 v, y; };
      };
   };
};

struct VEC2Dd {
   union {
      fl64 _fl64[2];
      struct {
         union { fl64 u, x; };
         union { fl64 v, y; };
      };
   };
};

struct VEC3Du8 {
   union {
      ui8 _ui8[3];
      struct {
         union { ui8 r, x; };
         union { ui8 g, y; };
         union { ui8 b, z; };
      };
   };
};

struct VEC3Ds8 {
   union {
      si8 _si8[3];
      struct {
         union { si8 r, x; };
         union { si8 g, y; };
         union { si8 b, z; };
      };
   };
};

struct VEC3Du16 {
   union {
      ui16 _ui16[3];
      struct {
         union { ui16 r, x; };
         union { ui16 g, y; };
         union { ui16 b, z; };
      };
   };
};

struct VEC3Ds16 {
   union {
      si16 _si16[3];
      struct {
         union { si16 r, x; };
         union { si16 g, y; };
         union { si16 b, z; };
      };
   };
};

struct VEC3Du24 {
   union {
      ui24 _ui24[3];
      struct {
         union { ui24 r, x; };
         union { ui24 g, y; };
         union { ui24 b, z; };
      };
   };
};

struct VEC3Du32 {
   union {
      ui32 _ui32[3];
      struct {
         union { ui32 r, x; };
         union { ui32 g, y; };
         union { ui32 b, z; };
      };
   };
};

struct VEC3Ds32 {
   union {
      si32 _si32[3];
      struct {
         union { si32 r, x; };
         union { si32 g, y; };
         union { si32 b, z; };
      };
   };
};

struct VEC3Du64 {
   union {
      ui64 _ui64[3];
      struct {
         union { ui64 r, x; };
         union { ui64 g, y; };
         union { ui64 b, z; };
      };
   };
};

struct VEC3Ds64 {
   union {
      si64 _si64[3];
      struct {
         union { si64 r, x; };
         union { si64 g, y; };
         union { si64 b, z; };
      };
   };
};

struct VEC3Df {
   union {
      fl32 _fl32[3];
      struct {
         union { fl32 r, x; };
         union { fl32 g, y; };
         union { fl32 b, z; };
      };
   };
};

struct VEC3Dd {
   union {
      fl64 _fl64[3];
      struct {
         union { fl64 r, x; };
         union { fl64 g, y; };
         union { fl64 b, z; };
      };
   };
};

struct VEC4Du8 {
   union {
      ui8 _ui8[4];
      struct {
         union { ui8 r, u1, x, x1, xSeg; };
         union { ui8 g, v1, y, y1, xDivs; };
         union { ui8 b, u2, z, x2, ySeg; };
         union { ui8 a, v2, w, y2, yDivs; };
      };
   };
};

struct VEC4Ds8 {
   union {
      si8 _si8[4];
      struct {
         union { si8 r, u1, x, x1, xSeg; };
         union { si8 g, v1, y, y1, xDivs; };
         union { si8 b, u2, z, x2, ySeg; };
         union { si8 a, v2, w, y2, yDivs; };
      };
   };
};

struct VEC4Du16 {
   union {
      ui16 _ui16[4];
      struct {
         union { ui16 r, u1, x, x1; };
         union { ui16 g, v1, y, y1; };
         union { ui16 b, u2, z, x2; };
         union { ui16 a, v2, w, y2; };
      };
   };
};

struct VEC4Ds16 {
   union {
      si16 _si16[4];
      struct {
         union { si16 r, u1, x, x1; };
         union { si16 g, v1, y, y1; };
         union { si16 b, u2, z, x2; };
         union { si16 a, v2, w, y2; };
      };
   };
};

al4 struct VEC4Du24 {
   union {
      ui24 _ui24[4];
      struct {
         union { ui24 r, u1, x, x1; };
         union { ui24 g, v1, y, y1; };
         union { ui24 b, u2, z, x2; };
         union { ui24 a, v2, w, y2; };
      };
   };
};

struct VEC4Du32 {
   union {
      ui32 _ui32[4];
      struct {
         union { ui32 r, u1, x, x1; };
         union { ui32 g, v1, y, y1; };
         union { ui32 b, u2, z, x2; };
         union { ui32 a, v2, w, y2; };
      };
   };
};

struct VEC4Ds32 {
   union {
      si32 _si32[4];
      struct {
         union { si32 r, u1, x, x1; };
         union { si32 g, v1, y, y1; };
         union { si32 b, u2, z, x2; };
         union { si32 a, v2, w, y2; };
      };
   };
};

struct VEC4Du64 {
   union {
      ui64 _ui64[4];
      struct {
         union { ui64 r, u1, x, x1; };
         union { ui64 g, v1, y, y1; };
         union { ui64 b, u2, z, x2; };
         union { ui64 a, v2, w, y2; };
      };
   };
};

struct VEC4Ds64 {
   union {
      si64 _si64[4];
      struct {
         union { si64 r, u1, x, x1; };
         union { si64 g, v1, y, y1; };
         union { si64 b, u2, z, x2; };
         union { si64 a, v2, w, y2; };
      };
   };
};

struct VEC4Df {
   union {
      fl32 _fl32[4];
      struct {
         union { fl32 r, u1, x, x1; };
         union { fl32 g, v1, y, y1; };
         union { fl32 b, u2, z, x2; };
         union { fl32 a, v2, w, y2; };
      };
   };
};

struct VEC4Dd {
   union {
      fl64 _fl64[4];
      struct {
         union { fl64 r, u1, x, x1; };
         union { fl64 g, v1, y, y1; };
         union { fl64 b, u2, z, x2; };
         union { fl64 a, v2, w, y2; };
      };
   };
};

struct VEC6Df {
   union {
      fl32 _fl32[6];
      struct {
         union { VEC3Df pos, face, s; };
         union { VEC3Df ori, up, vel, rot; };
      };
   };
};

struct VEC6Dd {
   union {
      fl64 _fl64[6];
      struct {
         union { VEC3Dd p, f, s; };
         union { VEC3Dd o, u, v, r; };
      };
   };
};

struct VEC8Df {
   union {
      fl64 _fl32[8];
      struct {
         union { VEC4Df p, f, s; };
         union { VEC4Df o, u, v, r; };
      };
   };
};

// SSE2 & AVX2 vector structs
struct SSE2Du32 {
   union {
      __m64 xmm;
      struct {
         union { ui32 u, x; };
         union { ui32 v, y; };
      };
   };
};

struct SSE2Ds32 {
   union {
      __m64 xmm;
      struct {
         union { si32 u, x; };
         union { si32 v, y; };
      };
   };
};

struct SSE2Du64 {
   union {
      __m128i xmm;
      struct {
         union { ui64 u, x; };
         union { ui64 v, y; };
      };
   };
};

struct SSE2Df32 {
   union {
      __m64 xmm;
      struct {
         union { fl32 u, x; };
         union { fl32 v, y; };
      };
   };
};

struct SSE2Df64 {
   union {
      __m128 xmm;
      struct {
         union { fl64 u, x; };
         union { fl64 v, y; };
      };
   };
};

struct SSE4Du32 {
   union {
      __m128i xmm;
      struct {
         union { ui32 r, u1, x, x1; };
         union { ui32 g, v1, y, y1; };
         union { ui32 b, u2, z, x2; };
         union { ui32 a, v2, w, y2; };
      };
   };
};

struct SSE4Ds32 {
   union {
      __m128i xmm;
      VEC4Ds32 vector;
   };
};

struct SSE4Df32 {
   union {
      __m128 xmm;
      VEC4Df vector;
   };
};

struct AVX4Du64 {
   union {
      __m256i ymm;
      __m128i xmm[2];
      struct {
         union { ui64 r, u1, x, x1; };
         union { ui64 g, v1, y, y1; };
         union { ui64 b, u2, z, x2; };
         union { ui64 a, v2, w, y2; };
      };
   };
};

struct AVX4Ds64 {
   union {
      __m256i ymm;
      __m128i xmm[2];
      struct {
         union { si64 r, u1, x, x1; };
         union { si64 g, v1, y, y1; };
         union { si64 b, u2, z, x2; };
         union { si64 a, v2, w, y2; };
      };
   };
};

struct AVX4Df64 {
   union {
      __m256 ymm;
      __m128 xmm[2];
      struct {
         union { fl64 r, u1, x, x1; };
         union { fl64 g, v1, y, y1; };
         union { fl64 b, u2, z, x2; };
         union { fl64 a, v2, w, y2; };
      };
   };
};

struct AVX8Du32 {
   union {
      __m256i ymm;
      __m128i xmm[2];
      struct {
         union { ui32 r, u1, x, x1; };
         union { ui32 g, v1, y, y1; };
         union { ui32 b, u2, z, x2; };
         union { ui32 a, v2, w, y2; };
      } vector[2];
   };
};

struct AVX8Ds32 {
   union {
      __m256i ymm;
      __m128i xmm[2];
      struct { __m128i xmm0, xmm1; };
      VEC4Ds32 vector[2];
      struct { VEC4Ds32 vector0, vector1; };
   };
};

struct AVX8Df32 {
   union {
      __m256 ymm;
      __m128 xmm[2];
      struct { __m128 xmm0, xmm1; };
      VEC4Df vector[2];
      struct { VEC4Df vector0, vector1; };
   };
};

struct AVX16Du16 {
   union {
      __m256i  ymm;
      __m128i  xmm[2];
      VEC4Du16 vector[4];
   };
};

struct AVX16Ds16 {
   union {
      __m256i ymm;
      __m128i xmm[2];
      struct {
         union { si16 r, u1, x, x1; };
         union { si16 g, v1, y, y1; };
         union { si16 b, u2, z, x2; };
         union { si16 a, v2, w, y2; };
      } vector[4];
   };
};

struct AVX16Df16 {
   union {
      __m256bh ymm;
      __m128bh xmm[2];
      struct {
         union { fl16 r, u1, x, x1; };
         union { fl16 g, v1, y, y1; };
         union { fl16 b, u2, z, x2; };
         union { fl16 a, v2, w, y2; };
      } vector[4];
   };
};

struct AVXmatrix {
   union {
      __m256 ymm[2];
      __m128 xmm[4];
      VEC4Df vector[4];
   };
};

// Constant vector types
typedef const VEC2Du8   cVEC2Du8;
typedef const VEC2Ds8   cVEC2Ds8;
typedef const VEC2Du16  cVEC2Du16;
typedef const VEC2Ds16  cVEC2Ds16;
typedef const VEC2Du24  cVEC2Du24;
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
typedef const VEC3Du24  cVEC3Du24;
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
typedef const VEC4Du24  cVEC4Du24;
typedef const VEC4Du32  cVEC4Du32;
typedef const VEC4Ds32  cVEC4Ds32;
typedef const VEC4Du64  cVEC4Du64;
typedef const VEC4Ds64  cVEC4Ds64;
typedef const VEC4Df    cVEC4Df;
typedef const VEC4Dd    cVEC4Dd;
typedef const VEC6Df    cVEC6Df;
typedef const VEC6Dd    cVEC6Dd;
typedef const VEC8Df    cVEC8Df;

typedef const SSE2Du32  cSSE2Du32;
typedef const SSE2Ds32  cSSE2Ds32;
typedef const SSE2Du64  cSSE2Du64;
typedef const SSE2Df32  cSSE2Df32;
typedef const SSE2Df64  cSSE2Df64;
typedef const SSE4Du32  cSSE4Du32;
typedef const SSE4Ds32  cSSE4Ds32;
typedef const SSE4Df32  cSSE4Df32;
typedef const AVX4Df64  cAVX4Df64;
typedef const AVX8Ds32  cAVX8Ds32;
typedef const AVX8Df32  cAVX8Df32;
typedef const AVX16Du16 cAVX16Du16;
typedef const AVXmatrix cAVXmatrix;
