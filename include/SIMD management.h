/************************************************************
 * File: SIMD management.h              Created: 2025/09/18 *
 *                                Last modified: 2025/09/27 *
 *                                                          *
 * Desc:                                                    *
 *                                                          *
 * MIT license             Copyright (c) David William Bull *
 ************************************************************/
#pragma once

#include <immintrin.h>

namespace simd {
   inline fl32x4 __vectorcall fmadd_ps(fl32x4 a, fl32x4 b, fl32x4 c) noexcept {
#if defined(__FMA__) || defined(__AVX2__)
      return _mm_fmadd_ps(a, b, c);
#else
      return _mm_add_ps(_mm_mul_ps(a, b), c);
#endif
   }
}

///== Intrinsics macros
#if !defined(__FMA__) && !defined(__AVX2__)
#define _mm_abs_pd(input)  (fl64x2&)_mm_and_epi64(_mm_set1_epi64x(0x07FFFFFFFFFFFFFFF), (si128&)(input))

#define _mm256_abs_pd(input)  _mm256_and_pd((fl64x4&)_mm256_set1_epi64x(0x07FFFFFFFFFFFFFFF), (input))

#ifndef _mm_fmadd_ps
#define _mm_fmadd_ps(a, b, c) _mm_add_ps(_mm_mul_ps((a), (b)), (c))
#endif

#ifndef _mm_fmsub_ps
#define _mm_fmsub_ps(a, b, c) _mm_sub_ps(_mm_mul_ps((a), (b)), (c))
#endif

#ifndef _mm_fnmadd_ps
#define _mm_fnmadd_ps(a, b, c) _mm_sub_ps((c), _mm_mul_ps((a), (b)))
#endif

#ifndef _mm256_fmadd_ps
#define _mm256_fmadd_ps(a, b, c) _mm256_add_ps(_mm256_mul_ps((a), (b)), (c))
#endif

#ifndef _mm256_fnmadd_ps
#define _mm256_fnmadd_ps(a, b, c) _mm256_sub_ps((c), _mm256_mul_ps((a), (b)))
#endif
#endif
///== Intrinsics macros
