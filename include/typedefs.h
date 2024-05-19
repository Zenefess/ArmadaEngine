/****************************************************************
 * File: typedefs.h                           Created: Jul.2007 *
 *                                    Last modified: 2024/05/18 *
 *                                                              *
 * Desc: Shorthand type defines & composites, and static        *
 *       constant values of common data-type sizes.             *
 *                                                              *
 * Notes: 2023/05/23: Added SSE & AVX vector types              *
 *        2023/06/05: Added ui24 data type                      *
 *        2024/05/02: Added csize_t data type                   *
 *        2024/05/11: Added all (~2) void pointer combinations  *
 *        2024/05/13: Moved ui24 data type to separate file     *
 *        2024/05/18: Added AVX512 vector types                 *
 *                                                              *
 *                             Copyright (c) David William Bull *
 ****************************************************************/
#pragma once

#include <immintrin.h>

#define _COMMON_TYPES_

#define al1  __declspec(align(2))
#define al2  __declspec(align(2))
#define al4  __declspec(align(4))
#define al8  __declspec(align(8))
#define al16 __declspec(align(16))
#define al32 __declspec(align(32))
#define al64 __declspec(align(64))

#define vol volatile

#define $LoopMT   __pragma(loop(hint_parallel(0)))
#define $LoopMT2  __pragma(loop(hint_parallel(2)))
#define $LoopMT4  __pragma(loop(hint_parallel(4)))
#define $LoopMT8  __pragma(loop(hint_parallel(8)))
#define $LoopMT16 __pragma(loop(hint_parallel(16)))

// Standard types
#if !defined(_WINDEF_)
typedef unsigned char    BYTE;
typedef unsigned short   WORD;
typedef unsigned long    DWORD;
#endif
typedef unsigned __int64 QWORD;
typedef unsigned __int8  ui8;
typedef unsigned __int16 ui16;
typedef unsigned __int32 ui32;
typedef unsigned __int64 ui64;
typedef          __m128i ui128;
typedef          __m256i ui256;
typedef          __m512i ui512;
typedef   signed __int8  si8;
typedef   signed __int16 si16;
typedef   signed __int32 si32;
typedef   signed __int64 si64;
typedef          __m128i si128;
typedef          __m256i si256;
typedef          __m512i si512;
typedef       __bfloat16 fl16;
typedef          float   fl32;
typedef          double  fl64;
typedef          __m128  fl32x4;
typedef          __m256  fl32x8;
typedef          __m128d fl64x2;
typedef          __m256d fl64x4;
typedef          __m512  fl32x16;
typedef          __m512d fl64x8;
typedef          wchar_t wchar;

// Constant types
typedef const          bool    cbool;
typedef const unsigned char    cBYTE;
typedef const unsigned short   cWORD;
typedef const unsigned long    cDWORD;
typedef const unsigned __int64 cQWORD;
typedef const          size_t  csize_t;
typedef const unsigned __int8  cui8;
typedef const unsigned __int16 cui16;
typedef const unsigned __int32 cui32;
typedef const unsigned __int64 cui64;
typedef const          __m128i cui128;
typedef const          __m256i cui256;
typedef const          __m512i cui512;
typedef const   signed __int8  csi8;
typedef const   signed __int16 csi16;
typedef const   signed __int32 csi32;
typedef const   signed __int64 csi64;
typedef const          __m128i csi128;
typedef const          __m256i csi256;
typedef const          __m512i csi512;
typedef const       __bfloat16 cfl16;
typedef const          float   cfl32;
typedef const          double  cfl64;
typedef const     long double  cfl80;
typedef const          __m128  cfl32x4;
typedef const          __m256  cfl32x8;
typedef const          __m512  cfl32x16;
typedef const          __m128d cfl64x2;
typedef const          __m256d cfl64x4;
typedef const          __m512d cfl64x8;
typedef const          char    cchar;
typedef const          wchar_t cwchar;

// Volatile types
typedef vol          bool    vbool;
typedef vol unsigned char    vBYTE;
typedef vol unsigned short   vWORD;
typedef vol unsigned long    vDWORD;
typedef vol unsigned __int64 vQWORD;
typedef vol unsigned __int8  vui8;
typedef vol unsigned __int16 vui16;
typedef vol unsigned __int32 vui32;
typedef vol unsigned __int64 vui64;
typedef vol          __m128i vui128;
typedef vol          __m256i vui256;
typedef vol          __m512i vui512;
typedef vol   signed __int8  vsi8;
typedef vol   signed __int16 vsi16;
typedef vol   signed __int32 vsi32;
typedef vol   signed __int64 vsi64;
typedef vol          __m128i vsi128;
typedef vol          __m256i vsi256;
typedef vol          __m512i vsi512;
typedef vol       __bfloat16 vfl16;
typedef vol          float   vfl32;
typedef vol          double  vfl64;
typedef vol     long double  vfl80;
typedef vol          __m128  vfl32x4;
typedef vol          __m256  vfl32x8;
typedef vol          __m512  vfl32x16;
typedef vol          __m128d vfl64x2;
typedef vol          __m256d vfl64x4;
typedef vol          __m512d vfl64x8;
typedef vol             char vchar;
typedef vol          wchar_t vwchar;

// Void pointer types
typedef void       *               ptr;       // Pointer
typedef void const *               cptr;      // Pointer to constant
typedef void vol   *               vptr;      // Pointer to volatile
typedef void       * const         ptrc;      // Constant pointer
typedef void const * const         cptrc;     // Constant pointer to constant
typedef void vol   * const         vptrc;     // Constant pointer to volatile
typedef void       * vol           ptrv;      // Volatile pointer
typedef void const * vol           cptrv;     // Volatile pointer to constant
typedef void vol   * vol           vptrv;     // Volatile pointer to volatile
typedef void       *       *       ptrptr;    // Pointer to pointer
typedef void const *       *       cptrptr;   // Pointer to pointer to constant
typedef void vol   *       *       vptrptr;   // Pointer to pointer to volatile
typedef void       * const *       ptrcptr;   // Pointer to constant pointer
typedef void const * const *       cptrcptr;  // Pointer to constant pointer to constant
typedef void vol   * const *       vptrcptr;  // Pointer to constant pointer to volatile
typedef void       * vol   *       ptrvptr;   // Pointer to volatile pointer
typedef void const * vol   *       cptrvptr;  // Pointer to volatile pointer to constant
typedef void vol   * vol   *       vptrvptr;  // Pointer to volatile pointer to volatile
typedef void       *       * const ptrptrc;   // Constant pointer to pointer
typedef void const *       * const cptrptrc;  // Constant pointer to pointer to constant
typedef void vol   *       * const vptrptrc;  // Constant pointer to pointer to volatile
typedef void       * const * const ptrcptrc;  // Constant pointer to constant pointer
typedef void const * const * const cptrcptrc; // Constant pointer to constant pointer to constant
typedef void vol   * const * const vptrcptrc; // Constant pointer to constant pointer to volatile
typedef void       * vol   * const ptrvptrc;  // Constant pointer to volatile pointer
typedef void const * vol   * const cptrvptrc; // Constant pointer to volatile pointer to constant
typedef void vol   * vol   * const vptrvptrc; // Constant pointer to volatile pointer to volatile
typedef void       *       * vol   ptrptrv;   // Volatile pointer to pointer
typedef void const *       * vol   cptrptrv;  // Constant pointer to pointer to constant
typedef void vol   *       * vol   vptrptrv;  // Volatile pointer to pointer to volatile
typedef void       * const * vol   ptrcptrv;  // Volatile pointer to constant to pointer
typedef void const * const * vol   cptrcptrv; // Volatile pointer to constant pointer to constant
typedef void vol   * const * vol   vptrcptrv; // Volatile pointer to constant pointer to volatile
typedef void       * vol   * vol   ptrvptrv;  // Volatile pointer to volatile pointer
typedef void const * vol   * vol   cptrvptrv; // Volatile pointer to volatile pointer to constant
typedef void vol   * vol   * vol   vptrvptrv; // Volatile pointer to volatile pointer to volatile

// Pointer to types
typedef unsigned char     *bptr;
typedef unsigned short    *wptr;
typedef unsigned long     *dwptr;
typedef unsigned __int64  *qwptr;
typedef unsigned __int8   *ui8ptr;
typedef unsigned __int8  **ui8ptrptr;
typedef unsigned __int16  *ui16ptr;
typedef unsigned __int16 **ui16ptrptr;
typedef unsigned __int32  *ui32ptr;
typedef unsigned __int32 **ui32ptrptr;
typedef unsigned __int64  *ui64ptr;
typedef unsigned __int64 **ui64ptrptr;
typedef          __m128i  *ui128ptr;
typedef          __m128i **ui128ptrptr;
typedef          __m256i  *ui256ptr;
typedef          __m256i **ui256ptrptr;
typedef          __m512i  *ui512ptr;
typedef          __m512i **ui512ptrptr;
typedef   signed __int8   *si8ptr;
typedef   signed __int8  **si8ptrptr;
typedef   signed __int16  *si16ptr;
typedef   signed __int16 **si16ptrptr;
typedef   signed __int32  *si32ptr;
typedef   signed __int32 **si32ptrptr;
typedef   signed __int64  *si64ptr;
typedef   signed __int64 **si64ptrptr;
typedef          __m128i  *si128ptr;
typedef          __m128i **si128ptrptr;
typedef          __m256i  *si256ptr;
typedef          __m256i **si256ptrptr;
typedef          __m512i  *si512ptr;
typedef          __m512i **si512ptrptr;
typedef       __bfloat16  *fl16ptr;
typedef       __bfloat16 **fl16ptrptr;
typedef          float    *fl32ptr;
typedef          float   **fl32ptrptr;
typedef          double   *fl64ptr;
typedef          double  **fl64ptrptr;
typedef     long double   *fl80ptr;
typedef     long double  **fl80ptrptr;
typedef          __m128   *fl32x4ptr;
typedef          __m128  **fl32x4ptrptr;
typedef          __m256   *fl32x8ptr;
typedef          __m256  **fl32x8ptrptr;
typedef          __m512   *fl32x16ptr;
typedef          __m512  **fl32x16ptrptr;
typedef         __m128d   *fl64x2ptr;
typedef         __m128d  **fl64x2ptrptr;
typedef         __m256d   *fl64x4ptr;
typedef         __m256d  **fl64x4ptrptr;
typedef         __m512d   *fl64x8ptr;
typedef         __m512d  **fl64x8ptrptr;
typedef          char     *chptr;
typedef          char    **stptrptr;
typedef          wchar_t  *wchptr;
typedef          wchar_t **wstptrptr;
#if defined(_FILE_DEFINED)
typedef          FILE     *Fptr;
#endif

// Pointer to constant types
typedef const unsigned char     *cbptr;
typedef const unsigned short    *cwptr;
typedef const unsigned long     *cdwptr;
typedef const unsigned __int64  *cqwptr;
typedef const unsigned __int8   *cui8ptr;
typedef const unsigned __int16  *cui16ptr;
typedef const unsigned __int32  *cui32ptr;
typedef const unsigned __int64  *cui64ptr;
typedef const          __m128i  *cui128ptr;
typedef const          __m256i  *cui256ptr;
typedef const          __m512i  *cui512ptr;
typedef const   signed __int8   *csi8ptr;
typedef const   signed __int16  *csi16ptr;
typedef const   signed __int32  *csi32ptr;
typedef const   signed __int64  *csi64ptr;
typedef const          __m128i  *csi128ptr;
typedef const          __m256i  *csi256ptr;
typedef const          __m512i  *csi512ptr;
typedef const       __bfloat16  *cfl16ptr;
typedef const          float    *cfl32ptr;
typedef const          double   *cfl64ptr;
typedef const     long double   *cfl80ptr;
typedef const          __m128   *cfl32x4ptr;
typedef const          __m256   *cfl32x8ptr;
typedef const          __m512   *cfl32x16ptr;
typedef const         __m128d   *cfl64x2ptr;
typedef const         __m256d   *cfl64x4ptr;
typedef const         __m512d   *cfl64x8ptr;
typedef const          char     *cchptr;
typedef const          char    **cstptr;
typedef const          wchar_t  *cwchptr;
typedef const          wchar_t **cwstptr;

// Constant pointers to types
typedef unsigned  __int8  * const ui8ptrc;
typedef unsigned  __int8 ** const ui8ptrptrc;
typedef unsigned __int16  * const ui16ptrc;
typedef unsigned __int16 ** const ui16ptrptrc;
typedef unsigned __int32  * const ui32ptrc;
typedef unsigned __int32 ** const ui32ptrptrc;
typedef unsigned __int64  * const ui64ptrc;
typedef unsigned __int64 ** const ui64ptrptrc;
typedef          __m128i  * const ui128ptrc;
typedef          __m128i ** const ui128ptrptrc;
typedef          __m256i  * const ui256ptrc;
typedef          __m256i ** const ui256ptrptrc;
typedef          __m512i  * const ui512ptrc;
typedef          __m512i ** const ui512ptrptrc;
typedef   signed  __int8  * const si8ptrc;
typedef   signed  __int8 ** const si8ptrptrc;
typedef   signed __int16  * const si16ptrc;
typedef   signed __int16 ** const si16ptrptrc;
typedef   signed __int32  * const si32ptrc;
typedef   signed __int32 ** const si32ptrptrc;
typedef   signed __int64  * const si64ptrc;
typedef   signed __int64 ** const si64ptrptrc;
typedef          __m128i  * const si128ptrc;
typedef          __m128i ** const si128ptrptrc;
typedef          __m256i  * const si256ptrc;
typedef          __m256i ** const si256ptrptrc;
typedef          __m512i  * const si512ptrc;
typedef          __m512i ** const si512ptrptrc;
typedef       __bfloat16  * const fl16ptrc;
typedef       __bfloat16 ** const fl16ptrptrc;
typedef            float  * const fl32ptrc;
typedef            float ** const fl32ptrptrc;
typedef           double  * const fl64ptrc;
typedef           double ** const fl64ptrptrc;
typedef           __m128  * const fl32x4ptrc;
typedef           __m128 ** const fl32x4ptrptrc;
typedef           __m256  * const fl32x8ptrc;
typedef           __m256 ** const fl32x8ptrptrc;
typedef           __m512  * const fl32x16ptrc;
typedef           __m512 ** const fl32x16ptrptrc;
typedef          __m128d  * const fl64x2ptrc;
typedef          __m128d ** const fl64x2ptrptrc;
typedef          __m256d  * const fl64x4ptrc;
typedef          __m256d ** const fl64x4ptrptrc;
typedef          __m512d  * const fl64x8ptrc;
typedef          __m512d ** const fl64x8ptrptrc;
typedef             char  * const chptrc;
typedef             char ** const stptrc;
typedef          wchar_t  * const wchptrc;
typedef          wchar_t ** const wstptrc;

// Constant pointers to constant pointers to types
typedef unsigned  __int8 * const * const ui8ptrcptrc;
typedef unsigned __int16 * const * const ui16ptrcptrc;
typedef unsigned __int32 * const * const ui32ptrcptrc;
typedef unsigned __int64 * const * const ui64ptrcptrc;
typedef          __m128i * const * const ui128ptrcptrc;
typedef          __m256i * const * const ui256ptrcptrc;
typedef          __m512i * const * const ui512ptrcptrc;
typedef   signed  __int8 * const * const si8ptrcptrc;
typedef   signed __int16 * const * const si16ptrcptrc;
typedef   signed __int32 * const * const si32ptrcptrc;
typedef   signed __int64 * const * const si64ptrcptrc;
typedef          __m128i * const * const si128ptrcptrc;
typedef          __m256i * const * const si256ptrcptrc;
typedef          __m512i * const * const si512ptrcptrc;
typedef       __bfloat16 * const * const fl16ptrcptrc;
typedef            float * const * const fl32ptrcptrc;
typedef           double * const * const fl64ptrcptrc;
typedef      long double * const * const fl80ptrcptrc;
typedef           __m128 * const * const fl32x4ptrcptrc;
typedef           __m256 * const * const fl32x8ptrcptrc;
typedef           __m512 * const * const fl32x16ptrcptrc;
typedef          __m128d * const * const fl64x2ptrcptrc;
typedef          __m256d * const * const fl64x4ptrcptrc;
typedef          __m512d * const * const fl64x8ptrcptrc;
typedef             char * const * const chptrcptrc;
typedef          wchar_t * const * const wchptrcptrc;

// Constant pointers to constant types
typedef unsigned  __int8 const * const cui8ptrc;
typedef unsigned __int16 const * const cui16ptrc;
typedef unsigned __int32 const * const cui32ptrc;
typedef unsigned __int64 const * const cui64ptrc;
typedef          __m128i const * const cui128ptrc;
typedef          __m256i const * const cui256ptrc;
typedef          __m512i const * const cui512ptrc;
typedef   signed  __int8 const * const csi8ptrc;
typedef   signed __int16 const * const csi16ptrc;
typedef   signed __int32 const * const csi32ptrc;
typedef   signed __int64 const * const csi64ptrc;
typedef          __m128i const * const csi128ptrc;
typedef          __m256i const * const csi256ptrc;
typedef          __m512i const * const csi512ptrc;
typedef       __bfloat16 const * const cfl16ptrc;
typedef            float const * const cfl32ptrc;
typedef           double const * const cfl64ptrc;
typedef      long double const * const cfl80ptrc;
typedef           __m128 const * const cfl32x4ptrc;
typedef           __m256 const * const cfl32x8ptrc;
typedef           __m512 const * const cfl32x16ptrc;
typedef          __m128d const * const cfl64x2ptrc;
typedef          __m256d const * const cfl64x4ptrc;
typedef          __m512d const * const cfl64x8ptrc;
typedef             char const * const cchptrc;
typedef          wchar_t const * const cwchptrc;

// Constant pointers to constant pointers to constant types
typedef unsigned  __int8 const * const * const cui8ptrcptrc;
typedef unsigned __int16 const * const * const cui16ptrcptrc;
typedef unsigned __int32 const * const * const cui32ptrcptrc;
typedef unsigned __int64 const * const * const cui64ptrcptrc;
typedef          __m128i const * const * const cui128ptrcptrc;
typedef          __m256i const * const * const cui256ptrcptrc;
typedef          __m512i const * const * const cui512ptrcptrc;
typedef   signed  __int8 const * const * const csi8ptrcptrc;
typedef   signed __int16 const * const * const csi16ptrcptrc;
typedef   signed __int32 const * const * const csi32ptrcptrc;
typedef   signed __int64 const * const * const csi64ptrcptrc;
typedef          __m128i const * const * const csi128ptrcptrc;
typedef          __m256i const * const * const csi256ptrcptrc;
typedef          __m512i const * const * const csi512ptrcptrc;
typedef       __bfloat16 const * const * const cfl16ptrcptrc;
typedef            float const * const * const cfl32ptrcptrc;
typedef           double const * const * const cfl64ptrcptrc;
typedef      long double const * const * const cfl80ptrcptrc;
typedef           __m128 const * const * const cfl32x4ptrcptrc;
typedef           __m256 const * const * const cfl32x8ptrcptrc;
typedef           __m512 const * const * const cfl32x16ptrcptrc;
typedef          __m128d const * const * const cfl64x2ptrcptrc;
typedef          __m256d const * const * const cfl64x4ptrcptrc;
typedef          __m512d const * const * const cfl64x8ptrcptrc;
typedef             char const * const * const cchptrcptrc;
typedef          wchar_t const * const * const cwchptrcptrc;

// Pointers to volatile types
typedef vol unsigned char     *vbptr;
typedef vol unsigned short    *vwptr;
typedef vol unsigned long     *vdwptr;
typedef vol unsigned __int64  *vqwptr;
typedef vol unsigned __int8   *vui8ptr;
typedef vol unsigned __int16  *vui16ptr;
typedef vol unsigned __int32  *vui32ptr;
typedef vol unsigned __int64  *vui64ptr;
typedef vol          __m128i  *vui128ptr;
typedef vol          __m256i  *vui256ptr;
typedef vol          __m512i  *vui512ptr;
typedef vol   signed __int8   *vsi8ptr;
typedef vol   signed __int16  *vsi16ptr;
typedef vol   signed __int32  *vsi32ptr;
typedef vol   signed __int64  *vsi64ptr;
typedef vol          __m128i  *vsi128ptr;
typedef vol          __m256i  *vsi256ptr;
typedef vol          __m512i  *vsi512ptr;
typedef vol       __bfloat16  *vfl16ptr;
typedef vol          float    *vfl32ptr;
typedef vol          double   *vfl64ptr;
typedef vol     long double   *vfl80ptr;
typedef vol          __m128   *vfl32x4ptr;
typedef vol          __m256   *vfl32x8ptr;
typedef vol          __m512   *vfl32x16ptr;
typedef vol         __m128d   *vfl64x2ptr;
typedef vol         __m256d   *vfl64x4ptr;
typedef vol         __m512d   *vfl64x8ptr;
typedef vol          char     *vchptr;
typedef vol          char    **vstptr;
typedef vol          wchar_t  *vwchptr;
typedef vol          wchar_t **vwstptr;

// Function pointer types
typedef void (*func)(void);
typedef void (*funcptr)(const void * const);

// Pointer arrays
struct PTR2 { ptr p0; ptr p1; };
struct PTR4 { ptr p0; ptr p1; ptr p2; ptr p3; };

// Data-type sizes
al4 static const int PTR_SIZE  = sizeof(void *),
                     REG_SIZE  = sizeof(void *),
                     CHAR_SIZE = sizeof(char),
                     WCH_SIZE  = sizeof(wchar_t),
                     INT_SIZE  = sizeof(int),
                     LONG_SIZE = sizeof(long),
                     LLNG_SIZE = sizeof(long long),
                     FL16_SIZE = sizeof(__bfloat16),
                     FL32_SIZE = sizeof(float),
                     FL64_SIZE = sizeof(double),
                     FL80_SIZE = sizeof(long double),
                     M64_SIZE  = sizeof(__m64),
                     M128_SIZE = sizeof(__m128),
                     M256_SIZE = sizeof(__m256),
                     M512_SIZE = sizeof(__m512);
#if defined(_WINCON_)
al4 static const int CHI_SIZE = sizeof(_CHAR_INFO);
#endif

// Pointer array macros
#define defpa(dataType, dimension, varName) dataType (*varName)[dimension]
#define defpa2(dataType, dimension1, dimension2, varName) dataType (*varName)[dimension1][dimension2]
#define defp1a1(dataType, dimension1, dimension2, varName) dataType (*varName[dimension1])[dimension2]

#define refpa(dataType, dimension) (dataType (*)[dimension])
#define refpa2(dataType, dimension1, dimesnion2) (dataType (*)[dimension1][dimension2])
#define refp1a1(dataType, dimension1, dimension2) (dataType (*[dimension1])[dimension2])
