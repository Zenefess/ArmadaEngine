/**********************************************************
 * File: typedefs.h                     Created: Jul.2007 *
 *                              Last modified: 2023/06/05 *
 *                                                        *
 * Desc: Shorthand type defines & composites, and static  *
 *       constant values of common data-type sizes.       *
 *                                                        *
 * Notes: 2023/05/23: Added SSE & AVX vector types.       *
 *        2023/06/05: Added ui24 data type.               *
 *                                                        *
 * To do: Swap naming scheme of constant pointer and      *
 *        pointer-to-constant                             *
 *                                                        *
 *                       Copyright (c) David William Bull *
 **********************************************************/
#pragma once

#include <immintrin.h>

#define _COMMON_TYPES_

#define al1  __declspec(align(2))
#define al2  __declspec(align(2))
#define al4  __declspec(align(4))
#define al8  __declspec(align(8))
#define al16 __declspec(align(16))
#define al32 __declspec(align(32))

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
typedef   signed __int8  si8;
typedef   signed __int16 si16;
typedef   signed __int32 si32;
typedef   signed __int64 si64;
typedef          __m128i si128;
typedef          __m256i si256;
typedef       __bfloat16 fl16;
typedef          float   fl32;
typedef          double  fl64;
typedef          __m128  fl32x4;
typedef          __m256  fl32x8;
typedef          __m128d fl64x2;
typedef          __m256d fl64x4;
typedef          wchar_t wchar;

// New pseudo-types
struct ui24 {
private:
   ui8 data[3];

public:
   ui24(void) = default;
   ui24(const ui16 value) { (ui16 &)data = (const ui16 &)value; data[2] = 0; }
   ui24(const si16 value) { (ui16 &)data = (const ui16 &)value; data[2] = 0; }
   ui24(const ui32 value) { (ui16 &)data = (const ui16 &)value; data[2] = ((const ui8 (&)[3])value)[2]; }
   ui24(const si32 value) { (ui16 &)data = (const ui16 &)value; data[2] = ((const ui8 (&)[3])value)[2]; }
   ui24(const fl32 value) {
      const ui32 integer = (const ui32)value;

      (ui16 &)data = (ui16 &)integer;
      data[2] = ((ui8 *)&integer)[2];
   }
   ui24(const fl64 value) {
      const ui32 integer = (const ui32)value;

      (ui16 &)data = (const ui16 &)integer;
      data[2] = ((const ui8 (&)[3])integer)[2];
   }

   operator const ui32(void) const { return (const ui32 &)data & 0x0FFFFFF; }
   operator const fl32(void) const { return fl32((const ui32 &)data & 0x0FFFFFF); }
   operator const fl64(void) const { return fl64((const ui32 &)data & 0x0FFFFFF); }

   inline const bool operator==(const ui24 &value) const {
      if(((const ui32 &)data & 0x0FFFFFF) == ((const ui32 &)value.data & 0x0FFFFFF))
         return true;
      return false;
   }

   inline const bool operator!=(const ui24 &value) const {
      if(((const ui32 &)data & 0x0FFFFFF) != ((const ui32 &)value.data & 0x0FFFFFF))
         return true;
      return false;
   }

   inline const void operator+(const ui24 &value) {
      data[0] += value.data[0];
      data[1] += value.data[1] + (data[0] < value.data[0]);
      data[2] += value.data[2] + (data[1] < value.data[1]);
   };

   inline const ui24 operator+=(const ui24 &value) {
      data[0] += value.data[0];
      data[1] += value.data[1] + (data[0] < value.data[0]);
      data[2] += value.data[2] + (data[1] < value.data[1]);

      return (ui24 &)data;
   };

   inline const void operator-(const ui24 &value) {
      data[0] -= value.data[0];
      data[1] -= value.data[1] + (data[0] > value.data[0]);
      data[2] -= value.data[2] + (data[1] > value.data[1]);
   };

   inline const void operator*(const ui24 &value) {
      const ui32 result = ((const ui32 &)data & 0x0FFFFFF) * ((const ui32 &)value.data & 0x0FFFFFF);

      (ui16 &)data = (const ui16 &)result;
      data[2] = ((const ui8 (&)[3])result)[2];
   };

   inline const void operator/(const ui24 &value) {
      const ui32 result = ((const ui32 &)data & 0x0FFFFFF) / ((const ui32 &)value.data & 0x0FFFFFF);

      (ui16 &)data = (const ui16 &)result;
      data[2] = ((const ui8 (&)[3])result)[2];
   };
};

// Constant types
typedef const          bool    cbool;
typedef const unsigned char    cBYTE;
typedef const unsigned short   cWORD;
typedef const unsigned long    cDWORD;
typedef const unsigned __int64 cQWORD;
typedef const unsigned __int8  cui8;
typedef const unsigned __int16 cui16;
typedef const             ui24 cui24;
typedef const unsigned __int32 cui32;
typedef const unsigned __int64 cui64;
typedef const          __m128i cui128;
typedef const          __m256i cui256;
typedef const   signed __int8  csi8;
typedef const   signed __int16 csi16;
typedef const   signed __int32 csi32;
typedef const   signed __int64 csi64;
typedef const          __m128i csi128;
typedef const          __m256i csi256;
typedef const       __bfloat16 cfl16;
typedef const          float   cfl32;
typedef const          double  cfl64;
typedef const          __m128  cfl32x4;
typedef const          __m256  cfl32x8;
typedef const          __m256  cflx8;
typedef const          __m128d cfl64x2;
typedef const          __m256d cfl64x4;
typedef const          float   cfl32;
typedef const          double  cfl64;
typedef const     long double  cfl80;
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
typedef vol             ui24 vui24;
typedef vol unsigned __int32 vui32;
typedef vol unsigned __int64 vui64;
typedef vol          __m128i vui128;
typedef vol          __m256i vui256;
typedef vol   signed __int8  vsi8;
typedef vol   signed __int16 vsi16;
typedef vol   signed __int32 vsi32;
typedef vol   signed __int64 vsi64;
typedef vol          __m128i vsi128;
typedef vol          __m256i vsi256;
typedef vol       __bfloat16 vfl16;
typedef vol          float   vfl32;
typedef vol          double  vfl64;
typedef vol          __m128  vfl32x4;
typedef vol          __m256  vfl32x8;
typedef vol          __m128d vfl64x2;
typedef vol          __m256d vfl64x4;
typedef vol          float   vfl32;
typedef vol          double  vfl64;
typedef vol     long double  vfl80;
typedef vol          wchar_t vwchar;

// Pointer types
typedef          void     *ptr;
typedef          void    **aptr;
typedef unsigned char     *bptr;
typedef unsigned short    *wptr;
typedef unsigned long     *dwptr;
typedef unsigned __int64  *qwptr;
typedef unsigned __int8   *ui8ptr;
typedef unsigned __int16  *ui16ptr;
typedef             ui24  *ui24ptr;
typedef unsigned __int32  *ui32ptr;
typedef unsigned __int32 **ui32aptr;
typedef unsigned __int64  *ui64ptr;
typedef unsigned __int64 **ui64aptr;
typedef   signed __int8   *si8ptr;
typedef   signed __int16  *si16ptr;
typedef   signed __int32  *si32ptr;
typedef   signed __int64  *si64ptr;
typedef       __bfloat16  *fl16ptr;
typedef          float    *fl32ptr;
typedef          double   *fl64ptr;
typedef     long double   *fl80ptr;
typedef          char     *chptr;
typedef          char    **stptr;
typedef          wchar_t  *wchptr;
typedef          wchar_t **wstptr;
#if defined(_FILE_DEFINED)
typedef          FILE     *Fptr;
#endif

// Function pointer types
typedef void (*func)(void);
typedef void (*funcptr)(const void * const);

// Pointer arrays
struct PTR2 { ptr p0; ptr p1; };
struct PTR4 { ptr p0; ptr p1; ptr p2; ptr p3; };

// Pointer-to-constant types
typedef const          void     *cptr;
typedef const          void    **captr;
typedef const unsigned char     *cbptr;
typedef const unsigned short    *cwptr;
typedef const unsigned long     *cdwptr;
typedef const unsigned __int64  *cqwptr;
typedef const unsigned __int8   *cui8ptr;
typedef const unsigned __int16  *cui16ptr;
typedef const             ui24  *cui24ptr;
typedef const unsigned __int32  *cui32ptr;
typedef const unsigned __int64  *cui64ptr;
typedef const   signed __int8   *csi8ptr;
typedef const   signed __int16  *csi16ptr;
typedef const   signed __int32  *csi32ptr;
typedef const   signed __int64  *csi64ptr;
typedef const          char     *cchptr;
typedef const          char    **cstptr;
typedef const          wchar_t  *cwchptr;
typedef const          wchar_t **cwstptr;
typedef const       __bfloat16  *cfl16ptr;
typedef const          float    *cfl32ptr;
typedef const          double   *cfl64ptr;
typedef const     long double   *cfl80ptr;

// Pointer-to-volatile types
typedef vol          void     *vptr;
typedef vol          void    **vaptr;
typedef vol unsigned char     *vbptr;
typedef vol unsigned short    *vwptr;
typedef vol unsigned long     *vdwptr;
typedef vol unsigned __int64  *vqwptr;
typedef vol unsigned __int8   *vui8ptr;
typedef vol unsigned __int16  *vui16ptr;
typedef vol             ui24  *vui24ptr;
typedef vol unsigned __int32  *vui32ptr;
typedef vol unsigned __int64  *vui64ptr;
typedef vol   signed __int8   *vsi8ptr;
typedef vol   signed __int16  *vsi16ptr;
typedef vol   signed __int32  *vsi32ptr;
typedef vol   signed __int64  *vsi64ptr;
typedef vol          char     *vchptr;
typedef vol          char    **vstptr;
typedef vol          wchar_t  *vwchptr;
typedef vol          wchar_t **vwstptr;
typedef vol       __bfloat16  *vfl16ptr;
typedef vol          float    *vfl32ptr;
typedef vol          double   *vfl64ptr;
typedef vol     long double   *vfl80ptr;

// Constant-pointer types
typedef             void  * const ptrc;
typedef             void ** const aptrc;
typedef             char  * const chptrc;
typedef          wchar_t  * const wchptrc;
typedef unsigned  __int8  * const ui8ptrc;
typedef             ui24  * const ui24ptrc;
typedef unsigned __int32  * const ui32ptrc;
typedef unsigned __int32 ** const ui32aptrc;
typedef unsigned __int64  * const ui64ptrc;
typedef unsigned __int64 ** const ui64aptrc;
typedef   signed  __int8  * const si8ptrc;
typedef   signed __int32  * const si32ptrc;
typedef   signed __int32 ** const si32aptrc;
typedef   signed __int64  * const si64ptrc;
typedef   signed __int64 ** const si64aptrc;

// Constant-pointer-to-constant types
typedef const             void  * const cptrc;
typedef const             void ** const captrc;
typedef const unsigned __int16  * const cui16ptrc;
typedef const             char  * const cchptrc;
typedef const          wchar_t  * const cwchptrc;

// Volatile-pointer types
typedef void  * vol ptrv;
typedef void ** vol aptrv;

// Address types
typedef unsigned char     &baddr;
typedef unsigned short    &waddr;
typedef unsigned long     &dwaddr;
typedef unsigned __int64  &qwaddr;
typedef unsigned __int8   &ui8addr;
typedef unsigned __int16  &ui16addr;
typedef             ui24  &ui24addr;
typedef unsigned __int32  &ui32addr;
typedef unsigned __int64  &ui64addr;
typedef   signed __int8   &si8addr;
typedef   signed __int16  &si16addr;
typedef   signed __int32  &si32addr;
typedef   signed __int64  &si64addr;
typedef          char     &chaddr;
typedef          char    &&staddr;
typedef          wchar_t  &wchaddr;
typedef          wchar_t &&wstaddr;
typedef       __bfloat16  &fl16addr;
typedef          float    &fl32addr;
typedef          double   &fl64addr;
typedef     long double   &fl80addr;

// 64-bit register size automation...
typedef       unsigned __int64  uint;
typedef         signed __int64  sint;
typedef const unsigned __int64  cuint;
typedef const   signed __int64  csint;
typedef vol   unsigned __int64  vuint;
typedef vol     signed __int64  vsint;
typedef vol     signed __int64 *vsintptr;

// Data-type sizes
al4 static const int PTR_SIZE  = sizeof(void *),
                     REG_SIZE  = sizeof(void *),
                     CHAR_SIZE = sizeof(char),
                     WCH_SIZE  = sizeof(wchar_t),
                     INT_SIZE  = sizeof(int),
                     LONG_SIZE = sizeof(long),
                     FL16_SIZE = sizeof(__bfloat16),
                     FL32_SIZE = sizeof(float),
                     FL64_SIZE = sizeof(double),
                     FL80_SIZE = sizeof(long double),
                     M64_SIZE  = sizeof(__m64),
                     M128_SIZE = sizeof(__m128),
                     M256_SIZE = sizeof(__m256);
#if defined(_WINCON_)
al4 static const int CHI_SIZE = sizeof(_CHAR_INFO);
#endif
