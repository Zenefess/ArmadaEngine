/**********************************************************************
 * File: Fixed-point data types.h                 Created: 2024/05/11 *
 *                                          Last modified: 2024/05/23 *
 *                                                                    *
 * Desc: Provides sizes of 8, 16, 24, and 32 bits. All sizes have     *
 *       support for fixed, normalised, and custom value ranges.      *
 *       There are, also, 4-element vectors of the 8 & 16-bit sizes.  *
 *       All data types are listed at the end of this file within     *
 *       typedefs of their constant counterparts.                     *
 *                                                                    *
 * Notes: Define FPDT_NO_CUSTOM before including this file to exclude *
 *        declarations of variables & functions for types with        *
 *        user-definable ranges.                                      *
 *        If using those data types, add fpdtInitCustom; to the       *
 *        global space of one of your project's .c/.cpp source files. *
 *        SSE3 support required. AVX2 and AVX512 support optional.    *
 *                                                                    *
 * MIT license.                     Copyright (c) David William Bull. *
 **********************************************************************/
#pragma once

#include <immintrin.h>
#include "typedefs.h"

// External _FPDT_DATA_ to be declared in a main .c/.cpp file, required for data types with a user-definable range
#define fpdtInitCustom al16 _FPDT_DATA_ _fpdt_data_

// For 2-scalar return values
typedef
#ifdef _VECTOR_STRUCTURES_
SSE2Df32
#else
al8 union { struct { fl32 u, v; } struct { fl32 x, y; }; fl32 _fl[2]; }
#endif
f32x2;

// For 3-scalar return values
typedef
#ifdef _VECTOR_STRUCTURES_
VEC3Du16
#else
union { struct { fs7p8 x, y, z; } struct { fs7p8 r, g, b; }; }
#endif
i16x3;

// For 32-byte return values
typedef
#ifdef _VECTOR_STRUCTURES_
AVX8Df32
#else
union { fl32x8 ymm; fl32x4 xmm[2]; fl32 _fl[8]; }
#endif
f32x8;

typedef
#ifdef _VECTOR_STRUCTURES_
AVX16Du16
#else
union { csi256 ymm; csi128 xmm[2]; cui16 _u16[16]; }
#endif
i16x16;

typedef
#ifdef _VECTOR_STRUCTURES_
AVX4Du64
#else
union { csi256 ymm; csi128 xmm[2]; cui64 _u64[4]; }
#endif
i64x4;

// For 64-byte return values
typedef
#ifdef _VECTOR_STRUCTURES_
AVX16Df32
#else
union { fl32x16 zmm; fl32x8 ymm[2]; fl32x4 xmm[4]; fl32 _fl[16]; }
#endif
f32x16;

typedef
#ifdef _VECTOR_STRUCTURES_
AVX16Ds32
#else
union { csi512 zmm; csi256 ymm[2]; csi128 xmm[4]; csi64 _s64[16]; cui64 _u64[16]; }
#endif
i32x16;

typedef const f32x2  cf32x2;  // For 2-scalar return values
typedef const i16x3  ci16x3;  // For 3-scalar return values
typedef const i64x4  ci64x4;  // For 32-byte packed integer return values
typedef const f32x8  cf32x8;  // For 32-byte packed floating-point return values
typedef const i16x16 ci16x16; // For 32-byte packed integer return values
typedef const i32x16 ci32x16; // For 64-byte packed integer return values
typedef const f32x16 cf32x16; // For 64-byte packed floating-point return values

#define _FIXED_POINT_DATA_TYPES_

#ifdef __AVX2__
#ifdef __AVX512F__
#define _FPDT_AVX512_
#endif
#define _FPDT_AVX2_
#endif

#ifdef _FPDT_AVX512_

static cfl32x16 _fpdt_65535div3fx16    = _mm512_set1_ps(21845.0f);
static cfl32x16 _fpdt_2div65535fx16    = _mm512_set1_ps(3.0f / 65535.0f);
static cui512   _fpdt_shuffle16sAVX512 = { .m512i_u64 = { 0x00D0C090805040100u, 0x01D1C191815141110u, 0x02D2C292825242120u, 0x03D3C393835343130u, 0, 0, 0, 0 } };

#endif

#ifdef _FPDT_AVX2_

static cfl32x8 _fpdt_65535div3fx8   = _mm256_set1_ps(21845.0f);
static cfl32x8 _fpdt_rcp255fx8      = _mm256_set1_ps(1.0f / 255.0f);
static cfl32x8 _fpdt_3div65535fx8   = _mm256_set1_ps(3.0f / 65535.0f);
static cui256  _fpdt_1x16           = _mm256_set1_epi16(0x01);
static cui128  _fpdt_shuffle16sAVX0 = { .m128i_u64 = { 0x00D0C090805040100u, 0 } };
static cui128  _fpdt_shuffle16sAVX1 = { .m128i_u64 = { 0x01D1C191815141110u, 0 } };

#endif

static cfl32x4 _fpdt_32768fx4     = _mm_set_ps1(32768.0f);
static cfl32x4 _fpdt_65535div2fx4 = _mm_set_ps1(32767.5f);
static cfl32x4 _fpdt_65535fx4     = _mm_set_ps1(65535.0f);
static cfl32x4 _fpdt_256fx4       = _mm_set_ps1(256.0f);
static cfl32x4 _fpdt_255fx4       = _mm_set_ps1(255.0f);
static cfl32x4 _fpdt_128fx4       = _mm_set_ps1(128.0f);
static cfl32x4 _fpdt_rcp255fx4    = _mm_set_ps1(1.0f / 255.0f);
static cfl32x4 _fpdt_rcp256fx4    = _mm_set_ps1(1.0f / 256.0f);
static cfl32x4 _fpdt_rcp32768fx4  = _mm_set_ps1(1.0f / 32768.0f);
static cfl32x4 _fpdt_rcp65535fx4  = _mm_set_ps1(1.0f / 65535.0f);
static cfl32x4 _fpdt_2div65535fx4 = _mm_set_ps1(2.0f / 65535.0f);
static cui128  _fpdt_1x8          = _mm_set1_epi16(0x01);
static cui128  _fpdt_shuffle8s    = { .m128i_u64 = { 0x0000000000C080400u, 0 } };
static cui128  _fpdt_shuffle16s   = { .m128i_u64 = { 0x00D0C090805040100u, 0 } };

static cfl32 _fpdt_65535div2f   = 32767.5f;
static cfl32 _fpdt_65535div128f = 65535.0f / 128.0f;
static cfl32 _fpdt_65535div256f = 65535.0f / 256.0f;
static cfl32 _fpdt_rcp16f       = 1.0f / 16.0f;
static cfl32 _fpdt_rcp128f      = 1.0f / 128.0f;
static cfl32 _fpdt_rcp255f      = 1.0f / 255.0f;
static cfl32 _fpdt_rcp256f      = 1.0f / 256.0f;
static cfl32 _fpdt_rcp512f      = 1.0f / 512.0f;
static cfl32 _fpdt_rcp1024f     = 1.0f / 1024.0f;
static cfl32 _fpdt_rcp4096f     = 1.0f / 4096.0f;
static cfl32 _fpdt_rcp16384f    = 1.0f / 16384.0f;
static cfl32 _fpdt_rcp32768f    = 1.0f / 32768.0f;
static cfl32 _fpdt_rcp65535f    = 1.0f / 65535.0f;
static cfl32 _fpdt_rcp65536f    = 1.0f / 65536.0f;
static cfl32 _fpdt_rcp2p24_1f   = 1.0f / 16777215.0f;
static cfl32 _fpdt_rcp2p24f     = 1.0f / 16777216.0f;
static cfl32 _fpdt_3div65535f   = 3.0f / 65535.0f;
static cfl32 _fpdt_2div65535f   = 2.0f / 65535.0f;
static cfl32 _fpdt_2div2p24_1f  = 2.0f / 16777215.0f;
static cfl32 _fpdt_128div65535f = 128.0f / 65535.0f;
static cfl32 _fpdt_256div65535f = 256.0f / 65535.0f;

static cfl64 _fpdt_rcp65536   = 1.0 / 65536.0;
static cfl64 _fpdt_rcp2p32_1  = 1.0 / 4294967295.0;
static cfl64 _fpdt_rcp2p32    = 1.0 / 4294967296.0;
static cfl64 _fpdt_2div2p32_1 = 2.0 / 4294967295.0;

#ifndef FPDT_NO_CUSTOM

// Data for types with a user-defineable range
struct _FPDT_DATA_ {
   fl32 origin8 = -1.0f;
   fl32 range8 = 1.0f;
   fl32 maxDivRange8 = 255.0f;
   fl32 rangeDivMax8 = _fpdt_rcp255f;
   fl32 origin16 = -1.0f;
   fl32 range16 = 1.0f;
   fl32 maxDivRange16 = 65535.0f;
   fl32 rangeDivMax16 = _fpdt_rcp65535f;
#ifdef _24BIT_INTEGERS_
   fl32 origin24 = -1.0f;
   fl32 range24 = 1.0f;
   fl32 maxDivRange24 = 16777215.0f;
   fl32 rangeDivMax24 = _fpdt_rcp2p24_1f;
#endif
   fl64 origin32 = -1.0;
   fl64 range32 = 1.0;
   fl64 maxDivRange32 = 4294967295.0;
   fl64 rangeDivMax32 = _fpdt_rcp2p32_1;
};

extern _FPDT_DATA_ _fpdt_data_;

// Normalised 8-bit : User-defineable decimal range
struct fp8n {
   typedef const fp8n cfp8n;

   ui8 data;

   inline cfp8n toFixed(cfl32 &value) const { return (cfp8n &)ui8((value - _fpdt_data_.origin8) * _fpdt_data_.maxDivRange8); }
   inline cfl32 toFloat(void) const { return fl32(data) * _fpdt_data_.rangeDivMax8 + _fpdt_data_.origin8; };
   inline cfl32 toFloat(cfp8n &value) const { return fl32(value.data) * _fpdt_data_.rangeDivMax8 + _fpdt_data_.origin8; };
   inline cui8 toFixedMod(cfp8n &value) const { return (value.data - ui8(_fpdt_data_.origin8 * _fpdt_data_.maxDivRange8)); }
   inline cui8 toFixedMod(cfl32 &value) const { return ui8(value * _fpdt_data_.maxDivRange8); }

   fp8n(void) = default;
   fp8n(cfl32 floor, cfl32 ceiling) {
      _fpdt_data_.origin8      = floor;
      _fpdt_data_.range8       = ceiling - floor;
      _fpdt_data_.maxDivRange8 = 255.0f / _fpdt_data_.range8;
      _fpdt_data_.rangeDivMax8 = _fpdt_data_.range8 / 255.0f;
   };
   fp8n(cfl32 value, cfl32 floor, cfl32 ceiling) {
      _fpdt_data_.origin8      = floor;
      _fpdt_data_.range8       = ceiling - floor;
      _fpdt_data_.maxDivRange8 = 255.0f / _fpdt_data_.range8;
      _fpdt_data_.rangeDivMax8 = _fpdt_data_.range8 / 255.0f;

      data = ui8((value - floor) * _fpdt_data_.maxDivRange8);
   };
   fp8n(cui8 value) { data = value; }
   fp8n(cui16 value) { data = toFixed(fl32(value) * _fpdt_data_.rangeDivMax16 + _fpdt_data_.origin16).data; }
   fp8n(cui32 value) { data = toFixed(fl32(fl64(value) * _fpdt_data_.rangeDivMax32 + _fpdt_data_.origin32)).data; }
   fp8n(csi32 value) { data = (ui8 &)value; }
   fp8n(cfl32 value) { data = toFixed(value).data; }
   fp8n(cfl64 value) { data = ui8((value - fl64(_fpdt_data_.origin8)) * (255.0 / fl64(_fpdt_data_.range8))); }

   operator ptr(void) const { return *this; }
   operator cfl32(void) const { return fl32(data) * _fpdt_data_.rangeDivMax8 + _fpdt_data_.origin8; };

   inline cfp8n &operator&(void) const { return *this; }
   inline cfp8n operator~(void) const { return (cfp8n &)~data; }
   inline cfp8n operator-(void) const { return (cfp8n &)~data; }
   inline cbool operator!(void) const { return data == 0; }

   inline cfp8n operator++(void) { data++; return *this; }
   inline cfp8n operator++(int) { cui8 temp = data++; return (cfp8n &)temp; }
   inline cfp8n operator--(void) { data--; return *this; }
   inline cfp8n operator--(int) { cui8 temp = data--; return (cfp8n &)temp; }

   inline cfp8n operator<<(csi32 &value) const { return (cfp8n &)(data << value); }
   inline cfp8n operator>>(csi32 &value) const { return (cfp8n &)(data >> value); }
   inline cfp8n operator<<=(csi32 &value) { return (cfp8n &)(data <<= value); }
   inline cfp8n operator>>=(csi32 &value) { return (cfp8n &)(data >>= value); }

   inline cbool operator==(cfp8n &value) const { return data == value.data; }
   inline cbool operator!=(cfp8n &value) const { return data != value.data; }
   inline cbool operator>=(cfp8n &value) const { return data >= value.data; }
   inline cbool operator<=(cfp8n &value) const { return data <= value.data; }
   inline cbool operator>(cfp8n &value) const { return data > value.data; }
   inline cbool operator<(cfp8n &value) const { return data < value.data; }
   inline cbool operator||(cfp8n &value) const { return data || value.data; }
   inline cbool operator&&(cfp8n &value) const { return data && value.data; }

   inline cfp8n operator+(cfp8n &value) const { return (cfp8n &)(data + toFixedMod(value)); }
   inline cfp8n operator-(cfp8n &value) const { return (cfp8n &)(data - toFixedMod(value)); }
   inline cfp8n operator*(cfp8n &value) const { return toFixed(toFloat() * toFloat(value)); }
   inline cfp8n operator/(cfp8n &value) const { return toFixed(toFloat() / toFloat(value)); }
   inline cfp8n operator%(cfp8n &value) const { return (cfp8n &)(data % toFixedMod(value)); }
   inline cfp8n operator&(cfp8n &value) const { return (cfp8n &)(data & value.data); }
   inline cfp8n operator|(cfp8n &value) const { return (cfp8n &)(data | value.data); }
   inline cfp8n operator^(cfp8n &value) const { return (cfp8n &)(data ^ value.data); }
   inline cfp8n operator+=(cfp8n &value) { return (cfp8n &)(data += toFixedMod(value)); }
   inline cfp8n operator-=(cfp8n &value) { return (cfp8n &)(data -= toFixedMod(value)); }
   inline cfp8n operator*=(cfp8n &value) { return *this = toFixed(toFloat() * toFloat(value)); }
   inline cfp8n operator/=(cfp8n &value) { return *this = toFixed(toFloat() / toFloat(value)); }
   inline cfp8n operator%=(cfp8n &value) { return (cfp8n &)(data %= toFixedMod(value)); }
   inline cfp8n operator&=(cfp8n &value) { return (cfp8n &)(data &= value.data); }
   inline cfp8n operator|=(cfp8n &value) { return (cfp8n &)(data |= value.data); }
   inline cfp8n operator^=(cfp8n &value) { return (cfp8n &)(data ^= value.data); }

   inline cbool operator==(cfl32 &value) const { return data == toFixed(value).data; }
   inline cbool operator!=(cfl32 &value) const { return data != toFixed(value).data; }
   inline cbool operator>=(cfl32 &value) const { return data <= toFixed(value).data; }
   inline cbool operator<=(cfl32 &value) const { return data >= toFixed(value).data; }
   inline cbool operator>(cfl32 &value) const { return data < toFixed(value).data; }
   inline cbool operator<(cfl32 &value) const { return data > toFixed(value).data; }
   inline cbool operator||(cfl32 &value) const { return data || toFixed(value).data; }
   inline cbool operator&&(cfl32 &value) const { return data && toFixed(value).data; }

   inline cfl32 operator+(cfl32 &value) const { return toFloat() + value; }
   inline cfl32 operator-(cfl32 &value) const { return toFloat() - value; }
   inline cfl32 operator*(cfl32 &value) const { return toFloat() * value; }
   inline cfl32 operator/(cfl32 &value) const { return toFloat() / value; }
   inline cfp8n operator%(cfl32 &value) const { return (cfp8n &)(data % toFixed(value).data); }
   inline cfp8n operator&(cfl32 &value) const { return (cfp8n &)(data & toFixed(value).data); }
   inline cfp8n operator|(cfl32 &value) const { return (cfp8n &)(data | toFixed(value).data); }
   inline cfp8n operator^(cfl32 &value) const { return (cfp8n &)(data ^ toFixed(value).data); }
   inline cfl32 operator+=(cfl32 &value) { cfl32 temp = toFloat() + value; *this = toFixed(temp); return temp; }
   inline cfl32 operator-=(cfl32 &value) { cfl32 temp = toFloat() - value; *this = toFixed(temp); return temp; }
   inline cfl32 operator*=(cfl32 &value) { cfl32 temp = toFloat() * value; *this = toFixed(temp); return temp; }
   inline cfl32 operator/=(cfl32 &value) { cfl32 temp = toFloat() / value; *this = toFixed(temp); return temp; }
   inline cfp8n operator%=(cfl32 &value) { return (cfp8n &)(data %= toFixed(value).data); }
   inline cfp8n operator&=(cfl32 &value) { return (cfp8n &)(data &= toFixed(value).data); }
   inline cfp8n operator|=(cfl32 &value) { return (cfp8n &)(data |= toFixed(value).data); }
   inline cfp8n operator^=(cfl32 &value) { return (cfp8n &)(data ^= toFixed(value).data); }
};

// 4x normalised 8-bit : User-defineable decimal range
struct fp8nx4 {
   typedef const fp8n cfp8n; typedef const fp8nx4 cfp8nx4;

   union { ui32 data32; fp8n data[4]; ui8 data8[4]; };

   inline cfp8n toFixed(cfl32 &value) const { return (cfp8n &)ui8((value - _fpdt_data_.origin8) * _fpdt_data_.maxDivRange8); }
   inline cfp8nx4 toFixed4(cfl32x4 &value) const { return (cfp8nx4 &)_mm_shuffle_epi8(_mm_cvttps_epi32(_mm_mul_ps(_mm_sub_ps(value, _mm_set_ps1(_fpdt_data_.origin8)), _mm_set_ps1(_fpdt_data_.maxDivRange8))), _fpdt_shuffle8s); }
   inline cfl32x4 toFloat4(void) const {
      return _mm_add_ps(_mm_mul_ps(_mm_cvtepu32_ps(_mm_cvtepu8_epi32(_mm_cvtsi32_si128(data32))), _mm_set_ps1(_fpdt_data_.rangeDivMax8)), _mm_set_ps1(_fpdt_data_.origin8));
   };
   inline cfl32x4 toFloat4(cfp8nx4 &value) const {
      return _mm_add_ps(_mm_mul_ps(_mm_cvtepu32_ps(_mm_cvtepu8_epi32(_mm_cvtsi32_si128(value.data32))), _mm_set_ps1(_fpdt_data_.rangeDivMax8)), _mm_set_ps1(_fpdt_data_.origin8));
   };
   inline cfp8nx4 toFixed4Mod(cfp8nx4 &value) const {
      cfp8nx4 temp = (cfp8nx4 &)_mm_shuffle_epi8(_mm_cvttps_epi32(_mm_mul_ps(_mm_set_ps1(_fpdt_data_.origin8), _mm_set_ps1(_fpdt_data_.maxDivRange8))), _fpdt_shuffle8s);
      return cfp8nx4{ ui8(data8[0] - temp.data8[0]), ui8(data8[1] - temp.data8[1]), ui8(data8[2] - temp.data8[2]), ui8(data8[3] - temp.data8[3]) };
   }
   inline cfp8nx4 toFixed4Mod(cfl32x4 &value) const {
      return (cfp8nx4 &)_mm_shuffle_epi8(_mm_cvttps_epi32(_mm_mul_ps(value, _mm_set_ps1(_fpdt_data_.maxDivRange8))), _fpdt_shuffle8s);
   }

   fp8nx4(void) = default;
   fp8nx4(cfl32 floor, cfl32 ceiling) {
      _fpdt_data_.origin8      = floor;
      _fpdt_data_.range8       = ceiling - floor;
      _fpdt_data_.maxDivRange8 = 255.0f / _fpdt_data_.range8;
      _fpdt_data_.rangeDivMax8 = _fpdt_data_.range8 / 255.0f;
   };
   fp8nx4(cfp8n value, cui8 index) { data[index] = value.data; }
   fp8nx4(cui8 value, cui8 index) { data8[index] = value; }
   fp8nx4(csi32 value, cui8 index) { data8[index] = (ui8 &)value; }
   fp8nx4(cfl32 value, cui8 index) { data8[index] = toFixed(value).data; }
   fp8nx4(cfp8n (&value)[4]) { data32 = (ui32 &)value[0]; }
   fp8nx4(cui8 (&value)[4]) { data32 = (ui32 &)value[0]; }
   fp8nx4(cfp8n value) { data[0] = data[1] = data[2] = data[3] = value; }
   fp8nx4(cui8 value) { data8[0] = data8[1] = data8[2] = data8[3] = value; }
   fp8nx4(cui32 value) { data32 = value; }
   fp8nx4(csi32 value) { data32 = (ui32 &)value; }
   fp8nx4(cfl32 value) { cfp8n temp = toFixed(value); data8[0] = data8[1] = data8[2] = data8[3] = temp.data; }
   fp8nx4(cfl32x4 value) { *this = toFixed4(value); }
#ifdef _VECTOR_STRUCTURES_
   fp8nx4(cVEC4Df value) { cSSE4Df32 temp = { .vector = value }; *this = toFixed4(temp.xmm); }
   fp8nx4(cSSE4Df32 value) { *this = toFixed4(value.xmm); }
#endif
   fp8nx4(cfp8n value0, cfp8n value1, cfp8n value2, cfp8n value3) { data[0] = value0; data[1] = value1; data[2] = value2; data[3] = value3; }
   fp8nx4(cui8 value0, cui8 value1, cui8 value2, cui8 value3) { data8[0] = value0; data8[1] = value1; data8[2] = value2; data8[3] = value3; }
   fp8nx4(csi32 value0, csi32 value1, csi32 value2, csi32 value3) { data8[0] = (ui8 &)value0; data8[1] = (ui8 &)value1; data8[2] = (ui8 &)value2; data8[3] = (ui8 &)value3; }
   fp8nx4(cfl32 value, cui8 index, cfl32 floor, cfl32 ceiling) {
      _fpdt_data_.origin8      = floor;
      _fpdt_data_.range8       = ceiling - floor;
      _fpdt_data_.maxDivRange8 = 255.0f / _fpdt_data_.range8;
      _fpdt_data_.rangeDivMax8 = _fpdt_data_.range8 / 255.0f;
      data8[index] = ui8((value - floor) * _fpdt_data_.maxDivRange8);
   };
   fp8nx4(cfl32x4 value, cfl32 floor, cfl32 ceiling) {
      _fpdt_data_.origin8      = floor;
      _fpdt_data_.range8       = ceiling - floor;
      _fpdt_data_.maxDivRange8 = 255.0f / _fpdt_data_.range8;
      _fpdt_data_.rangeDivMax8 = _fpdt_data_.range8 / 255.0f;
      data32 = toFixed4(value).data32;
   }

   operator ptr(void) const { return *this; }
   operator cfl32x4(void) const { return _mm_add_ps(_mm_mul_ps(_mm_cvtepu32_ps(_mm_cvtepu8_epi32(_mm_cvtsi32_si128(data32))), _mm_set_ps1(_fpdt_data_.rangeDivMax8)), _mm_set_ps1(_fpdt_data_.origin8)); }

   inline cfp8nx4 &operator&(void) const { return *this; }
   inline cfp8nx4 operator~(void) const { return (cfp8nx4 &)(data32 ^ 0x0FFFFFFFFu); }
   inline cfp8nx4 operator-(void) const { return (cfp8nx4 &)(data32 ^ 0x0FFFFFFFFu); }
   inline cbool operator!(void) const { return data32 == 0; }

   inline cfp8nx4 operator++(void) { data[0]++; data[1]++; data[2]++; data[3]++; return *this; }
   inline cfp8nx4 operator++(int) { cfp8nx4 temp = { data[0]++, data[1]++, data[2]++, data[3]++ }; return (cfp8nx4 &)temp; }
   inline cfp8nx4 operator--(void) { data[0]--; data[1]--; data[2]--; data[3]--; return *this; }
   inline cfp8nx4 operator--(int) { cfp8nx4 temp = { data[0]--, data[1]--, data[2]--, data[3]-- }; return (cfp8nx4 &)temp; }

   inline cfp8nx4 operator<<(csi32 &value) const { return fp8nx4{ data8[0] << value, data8[1] << value, data8[2] << value, data8[3] << value }; }
   inline cfp8nx4 operator>>(csi32 &value) const { return fp8nx4{ data8[0] >> value, data8[1] >> value, data8[2] >> value, data8[3] >> value }; }
   inline cfp8nx4 operator<<=(csi32 &value) { return fp8nx4{ data8[0] <<= value, data8[1] <<= value, data8[2] <<= value, data8[3] <<= value }; }
   inline cfp8nx4 operator>>=(csi32 &value) { return fp8nx4{ data8[0] >>= value, data8[1] >>= value, data8[2] >>= value, data8[3] >>= value }; }

// Each of the lower 4 bits represents the test result per variable
   inline cui8 operator==(cfp8nx4 &value) const { return (data8[0] == value.data8[0]) | ((data8[1] == value.data8[1]) << 1) | ((data8[2] == value.data8[2]) << 2) | ((data8[3] == value.data8[3]) << 3); }
   inline cui8 operator!=(cfp8nx4 &value) const { return (data8[0] != value.data8[0]) | ((data8[1] != value.data8[1]) << 1) | ((data8[2] != value.data8[2]) << 2) | ((data8[3] != value.data8[3]) << 3); }
   inline cui8 operator>=(cfp8nx4 &value) const { return (data8[0] >= value.data8[0]) | ((data8[1] >= value.data8[1]) << 1) | ((data8[2] >= value.data8[2]) << 2) | ((data8[3] >= value.data8[3]) << 3); }
   inline cui8 operator<=(cfp8nx4 &value) const { return (data8[0] <= value.data8[0]) | ((data8[1] <= value.data8[1]) << 1) | ((data8[2] <= value.data8[2]) << 2) | ((data8[3] <= value.data8[3]) << 3); }
   inline cui8 operator>(cfp8nx4 &value) const { return (data8[0] > value.data8[0]) | ((data8[1] > value.data8[1]) << 1) | ((data8[2] > value.data8[2]) << 2) | ((data8[3] > value.data8[3]) << 3); }
   inline cui8 operator<(cfp8nx4 &value) const { return (data8[0] < value.data8[0]) | ((data8[1] < value.data8[1]) << 1) | ((data8[2] < value.data8[2]) << 2) | ((data8[3] < value.data8[3]) << 3); }
   inline cui8 operator||(cfp8nx4 &value) const { return (data8[0] || value.data8[0]) | ((data8[1] || value.data8[1]) << 1) | ((data8[2] || value.data8[2]) << 2) | ((data8[3] || value.data8[3]) << 3); }
   inline cui8 operator&&(cfp8nx4 &value) const { return (data8[0] && value.data8[0]) | ((data8[1] && value.data8[1]) << 1) | ((data8[2] && value.data8[2]) << 2) | ((data8[3] && value.data8[3]) << 3); }

   inline cfp8nx4 operator+(cfp8nx4 &value) const { return (cfp8nx4 &)_mm_add_epi8(_mm_cvtsi32_si128(data32), _mm_cvtsi32_si128(value.data32)); }
   inline cfp8nx4 operator-(cfp8nx4 &value) const { return (cfp8nx4 &)_mm_sub_epi8(_mm_cvtsi32_si128(data32), _mm_cvtsi32_si128(value.data32)); }
   inline cfp8nx4 operator*(cfp8nx4 &value) const { return toFixed4(_mm_mul_ps(toFloat4(), toFloat4(value))); }
   inline cfp8nx4 operator/(cfp8nx4 &value) const { return toFixed4(_mm_div_ps(toFloat4(), toFloat4(value))); }
   inline cfp8nx4 operator%(cfp8nx4 &value) const { cfp8nx4 temp = toFixed4Mod(value); return cfp8nx4{ data8[0] % temp.data8[0], data8[1] % temp.data8[1], data8[2] % temp.data8[2], data8[3] % temp.data8[3] }; }
   inline cfp8nx4 operator&(cfp8nx4 &value) const { return (cfp8nx4 &)(data32 & value.data32); }
   inline cfp8nx4 operator|(cfp8nx4 &value) const { return (cfp8nx4 &)(data32 | value.data32); }
   inline cfp8nx4 operator^(cfp8nx4 &value) const { return (cfp8nx4 &)(data32 ^ value.data32); }
   inline cfp8nx4 operator+=(cfp8nx4 &value) { data32 = (ui32 &)_mm_add_epi8(_mm_cvtsi32_si128(data32), _mm_cvtsi32_si128(value.data32)); return *this; }
   inline cfp8nx4 operator-=(cfp8nx4 &value) { data32 = (ui32 &)_mm_sub_epi8(_mm_cvtsi32_si128(data32), _mm_cvtsi32_si128(value.data32)); return *this; }
   inline cfp8nx4 operator*=(cfp8nx4 &value) { return *this = toFixed4(_mm_mul_ps(toFloat4(), toFloat4(value))); }
   inline cfp8nx4 operator/=(cfp8nx4 &value) { return *this = toFixed4(_mm_div_ps(toFloat4(), toFloat4(value))); }
   inline cfp8nx4 operator%=(cfp8nx4 &value) { cfp8nx4 temp = toFixed4Mod(value); return cfp8nx4{ data8[0] %= temp.data8[0], data8[1] %= temp.data8[1], data8[2] %= temp.data8[2], data8[3] %= temp.data8[3] }; }
   inline cfp8nx4 operator&=(cfp8nx4 &value) { return (cfp8nx4 &)(data32 &= value.data32); }
   inline cfp8nx4 operator|=(cfp8nx4 &value) { return (cfp8nx4 &)(data32 |= value.data32); }
   inline cfp8nx4 operator^=(cfp8nx4 &value) { return (cfp8nx4 &)(data32 ^= value.data32); }

// Each of the lower 4 bits represents the test result per variable
   inline cui8 operator==(cfl32x4 &value) const {
      cfp8nx4 temp = toFixed4(value);
      return (data8[0] == temp.data8[0]) | ((data8[1] == temp.data8[1]) << 1) | ((data8[2] == temp.data8[2]) << 2) | ((data8[3] == temp.data8[3]) << 3);
   }
   inline cui8 operator!=(cfl32x4 &value) const {
      cfp8nx4 temp = toFixed4(value);
      return (data8[0] != temp.data8[0]) | ((data8[1] != temp.data8[1]) << 1) | ((data8[2] != temp.data8[2]) << 2) | ((data8[3] != temp.data8[3]) << 3);
   }
   inline cui8 operator>=(cfl32x4 &value) const {
      cfp8nx4 temp = toFixed4(value);
      return (data8[0] >= temp.data8[0]) | ((data8[1] >= temp.data8[1]) << 1) | ((data8[2] >= temp.data8[2]) << 2) | ((data8[3] >= temp.data8[3]) << 3);
   }
   inline cui8 operator<=(cfl32x4 &value) const {
      cfp8nx4 temp = toFixed4(value);
      return (data8[0] <= temp.data8[0]) | ((data8[1] <= temp.data8[1]) << 1) | ((data8[2] <= temp.data8[2]) << 2) | ((data8[3] <= temp.data8[3]) << 3);
   }
   inline cui8 operator>(cfl32x4 &value) const {
      cfp8nx4 temp = toFixed4(value);
      return (data8[0] > temp.data8[0]) | ((data8[1] > temp.data8[1]) << 1) | ((data8[2] > temp.data8[2]) << 2) | ((data8[3] > temp.data8[3]) << 3);
   }
   inline cui8 operator<(cfl32x4 &value) const {
      cfp8nx4 temp = toFixed4(value);
      return (data8[0] < temp.data8[0]) | ((data8[1] < temp.data8[1]) << 1) | ((data8[2] < temp.data8[2]) << 2) | ((data8[3] < temp.data8[3]) << 3);
   }
   inline cui8 operator||(cfl32x4 &value) const {
      cfp8nx4 temp = toFixed4(value);
      return (data8[0] || temp.data8[0]) | ((data8[1] || temp.data8[1]) << 1) | ((data8[2] || temp.data8[2]) << 2) | ((data8[3] || temp.data8[3]) << 3);
   }
   inline cui8 operator&&(cfl32x4 &value) const {
      cfp8nx4 temp = toFixed4(value);
      return (data8[0] && temp.data8[0]) | ((data8[1] && temp.data8[1]) << 1) | ((data8[2] && temp.data8[2]) << 2) | ((data8[3] && temp.data8[3]) << 3);
   }

   inline cfp8nx4 operator+(cfl32x4 &value) const { return toFixed4(_mm_add_ps(toFloat4(), value)); }
   inline cfp8nx4 operator-(cfl32x4 &value) const { return toFixed4(_mm_sub_ps(toFloat4(), value)); }
   inline cfp8nx4 operator*(cfl32x4 &value) const { return toFixed4(_mm_mul_ps(toFloat4(), value)); }
   inline cfp8nx4 operator/(cfl32x4 &value) const { return toFixed4(_mm_div_ps(toFloat4(), value)); }
   inline cfp8nx4 operator%(cfl32x4 &value) const {
      cfp8nx4 temp = toFixed4(value);
      return fp8nx4{ data8[0] % temp.data8[0], data8[1] % temp.data8[1], data8[2] % temp.data8[2], data8[3] % temp.data8[3] };
   }
   inline cfp8nx4 operator&(cfl32x4 &value) const { return (cfp8nx4 &)(data32 & toFixed4(value).data32); }
   inline cfp8nx4 operator|(cfl32x4 &value) const { return (cfp8nx4 &)(data32 | toFixed4(value).data32); }
   inline cfp8nx4 operator^(cfl32x4 &value) const { return (cfp8nx4 &)(data32 ^ toFixed4(value).data32); }
   inline cfp8nx4 operator+=(cfl32x4 &value) { return *this = toFixed4(_mm_add_ps(toFloat4(), value)); }
   inline cfp8nx4 operator-=(cfl32x4 &value) { return *this = toFixed4(_mm_sub_ps(toFloat4(), value)); }
   inline cfp8nx4 operator*=(cfl32x4 &value) { return *this = toFixed4(_mm_mul_ps(toFloat4(), value)); }
   inline cfp8nx4 operator/=(cfl32x4 &value) { return *this = toFixed4(_mm_div_ps(toFloat4(), value)); }
   inline cfp8nx4 operator%=(cfl32x4 &value) {
      cfp8nx4 temp = toFixed4(value);
      return *this = fp8nx4{ data8[0] % temp.data8[0], data8[1] % temp.data8[1], data8[2] % temp.data8[2], data8[3] % temp.data8[3] };
   }
   inline cfp8nx4 operator&=(cfl32x4 &value) { (cfp8nx4 &)(data32 &= toFixed4(value).data32); }
   inline cfp8nx4 operator|=(cfl32x4 &value) { (cfp8nx4 &)(data32 |= toFixed4(value).data32); }
   inline cfp8nx4 operator^=(cfl32x4 &value) { (cfp8nx4 &)(data32 ^= toFixed4(value).data32); }
};

// Normalised 16-bit : User-defineable decimal range
struct fp16n {
   typedef const fp16n cfp16n;

   ui16 data;

   inline cfp16n toFixed(cfl32 &value) const { return (cfp16n &)ui16((value - _fpdt_data_.origin16) * _fpdt_data_.maxDivRange16); }
   inline cfl32 toFloat(void) const { return fl32(data) * _fpdt_data_.rangeDivMax16 + _fpdt_data_.origin16; };
   inline cfl32 toFloat(cfp16n &value) const { return fl32(value.data) * _fpdt_data_.rangeDivMax16 + _fpdt_data_.origin16; };
   inline cui16 toFixedMod(cfp16n &value) const { return (value.data - ui16(_fpdt_data_.origin16 * _fpdt_data_.maxDivRange16)); }
   inline cui16 toFixedMod(cfl32 &value) const { return ui16(value * _fpdt_data_.maxDivRange16); }

   fp16n(void) {};
   fp16n(cfl32 floor, cfl32 ceiling) {
      _fpdt_data_.origin16      = floor;
      _fpdt_data_.range16       = ceiling - floor;
      _fpdt_data_.maxDivRange16 = 65535.0f / _fpdt_data_.range16;
      _fpdt_data_.rangeDivMax16 = _fpdt_data_.range16 / 65535.0f;
   };
   fp16n(cfl32 value, cfl32 floor, cfl32 ceiling) {
      _fpdt_data_.origin16      = floor;
      _fpdt_data_.range16       = ceiling - floor;
      _fpdt_data_.maxDivRange16 = 65535.0f / _fpdt_data_.range16;
      _fpdt_data_.rangeDivMax16 = _fpdt_data_.range16 / 65535.0f;

      data = ui16((value - floor) * _fpdt_data_.maxDivRange16);
   };
   fp16n(cui8 value) { data = toFixed(fl32(value) * _fpdt_data_.rangeDivMax8 + _fpdt_data_.origin8).data; }
   fp16n(cui16 value) { data = value; }
   fp16n(cui32 value) { data = toFixed(fl32(fl64(value) * _fpdt_data_.rangeDivMax32 + _fpdt_data_.origin32)).data; }
   fp16n(csi32 value) { data = (ui16 &)value; }
   fp16n(cfl32 value) { data = toFixed(value).data; }
   fp16n(cfl64 value) { data = ui16((value - fl64(_fpdt_data_.origin16)) * (65535.0 / fl64(_fpdt_data_.range16))); }

   operator ptr(void) const { return *this; }
   operator cfl32(void) const { return fl32(data) * _fpdt_data_.rangeDivMax16 + _fpdt_data_.origin16; };

   inline cfp16n &operator&(void) const { return *this; }
   inline cfp16n operator~(void) const { return (cfp16n &)~data; }
   inline cfp16n operator-(void) const { return (cfp16n &)~data; }
   inline cbool operator!(void) const { return data == 0; }

   inline cfp16n operator++(void) { data++; return *this; }
   inline cfp16n operator++(int) { cui16 temp = data++; return (cfp16n &)temp; }
   inline cfp16n operator--(void) { data--; return *this; }
   inline cfp16n operator--(int) { cui16 temp = data--; return (cfp16n &)temp; }

   inline cbool operator==(csi32 &value) const { return data == (ui16 &)data; }
   inline cbool operator!=(csi32 &value) const { return data != (ui16 &)data; }
   inline cbool operator>=(csi32 &value) const { return data >= (ui16 &)data; }
   inline cbool operator<=(csi32 &value) const { return data <= (ui16 &)data; }
   inline cbool operator>(csi32 &value) const { return data > (ui16 &)data; }
   inline cbool operator<(csi32 &value) const { return data < (ui16 &)data; }
   inline cbool operator||(csi32 &value) const { return data || (ui16 &)data; }
   inline cbool operator&&(csi32 &value) const { return data && (ui16 &)data; }

   inline cfp16n operator<<(csi32 &value) const { return (cfp16n &)(data << value); }
   inline cfp16n operator>>(csi32 &value) const { return (cfp16n &)(data >> value); }
   inline cfp16n operator<<=(csi32 &value) { return (cfp16n &)(data <<= value); }
   inline cfp16n operator>>=(csi32 &value) { return (cfp16n &)(data >>= value); }

   inline cbool operator==(cfp16n &value) const { return data == value.data; }
   inline cbool operator!=(cfp16n &value) const { return data != value.data; }
   inline cbool operator>=(cfp16n &value) const { return data >= value.data; }
   inline cbool operator<=(cfp16n &value) const { return data <= value.data; }
   inline cbool operator>(cfp16n &value) const { return data > value.data; }
   inline cbool operator<(cfp16n &value) const { return data < value.data; }
   inline cbool operator||(cfp16n &value) const { return data || value.data; }
   inline cbool operator&&(cfp16n &value) const { return data && value.data; }

   inline cfp16n operator+(cfp16n &value) const { return (cfp16n &)(data + toFixedMod(value)); }
   inline cfp16n operator-(cfp16n &value) const { return (cfp16n &)(data - toFixedMod(value)); }
   inline cfp16n operator*(cfp16n &value) const { return toFixed(toFloat() * toFloat(value)); }
   inline cfp16n operator/(cfp16n &value) const { return toFixed(toFloat() / toFloat(value)); }
   inline cfp16n operator%(cfp16n &value) const { return (cfp16n &)(data % toFixedMod(value)); }
   inline cfp16n operator&(cfp16n &value) const { return (cfp16n &)(data & value.data); }
   inline cfp16n operator|(cfp16n &value) const { return (cfp16n &)(data | value.data); }
   inline cfp16n operator^(cfp16n &value) const { return (cfp16n &)(data ^ value.data); }
   inline cfp16n operator+=(cfp16n &value) { return (cfp16n &)(data += toFixedMod(value)); }
   inline cfp16n operator-=(cfp16n &value) { return (cfp16n &)(data -= toFixedMod(value)); }
   inline cfp16n operator*=(cfp16n &value) { return *this = toFixed(toFloat() * toFloat(value)); }
   inline cfp16n operator/=(cfp16n &value) { return *this = toFixed(toFloat() / toFloat(value)); }
   inline cfp16n operator%=(cfp16n &value) { return (cfp16n &)(data %= toFixedMod(value)); }
   inline cfp16n operator&=(cfp16n &value) { return (cfp16n &)(data &= value.data); }
   inline cfp16n operator|=(cfp16n &value) { return (cfp16n &)(data |= value.data); }
   inline cfp16n operator^=(cfp16n &value) { return (cfp16n &)(data ^= value.data); }

   inline cbool operator==(cfl32 &value) const { return data == toFixed(value).data; }
   inline cbool operator!=(cfl32 &value) const { return data != toFixed(value).data; }
   inline cbool operator>=(cfl32 &value) const { return data <= toFixed(value).data; }
   inline cbool operator<=(cfl32 &value) const { return data >= toFixed(value).data; }
   inline cbool operator>(cfl32 &value) const { return data < toFixed(value).data; }
   inline cbool operator<(cfl32 &value) const { return data > toFixed(value).data; }
   inline cbool operator||(cfl32 &value) const { return data || toFixed(value).data; }
   inline cbool operator&&(cfl32 &value) const { return data && toFixed(value).data; }

   inline cfl32 operator+(cfl32 &value) const { return toFloat() + value; }
   inline cfl32 operator-(cfl32 &value) const { return toFloat() - value; }
   inline cfl32 operator*(cfl32 &value) const { return toFloat() * value; }
   inline cfl32 operator/(cfl32 &value) const { return toFloat() / value; }
   inline cfp16n operator%(cfl32 &value) const { return (cfp16n &)(data % toFixed(value).data); }
   inline cfp16n operator&(cfl32 &value) const { return (cfp16n &)(data & toFixed(value).data); }
   inline cfp16n operator|(cfl32 &value) const { return (cfp16n &)(data | toFixed(value).data); }
   inline cfp16n operator^(cfl32 &value) const { return (cfp16n &)(data ^ toFixed(value).data); }
   inline cfl32 operator+=(cfl32 &value) { cfl32 temp = toFloat() + value; *this = toFixed(temp); return temp; }
   inline cfl32 operator-=(cfl32 &value) { cfl32 temp = toFloat() - value; *this = toFixed(temp); return temp; }
   inline cfl32 operator*=(cfl32 &value) { cfl32 temp = toFloat() * value; *this = toFixed(temp); return temp; }
   inline cfl32 operator/=(cfl32 &value) { cfl32 temp = toFloat() / value; *this = toFixed(temp); return temp; }
   inline cfp16n operator%=(cfl32 &value) { return (cfp16n &)(data %= toFixed(value).data); }
   inline cfp16n operator&=(cfl32 &value) { return (cfp16n &)(data &= toFixed(value).data); }
   inline cfp16n operator|=(cfl32 &value) { return (cfp16n &)(data |= toFixed(value).data); }
   inline cfp16n operator^=(cfl32 &value) { return (cfp16n &)(data ^= toFixed(value).data); }
};

// 4x normalised 16-bit : User-defineable decimal range
struct fp16nx4 {
   typedef const fp16n cfp16n; typedef const fp16nx4 cfp16nx4;

   union { ui64 data64; fp16n data[4]; ui16 data16[4]; };

   inline cfp16n toFixed(cfl32 &value) const { return (cfp16n &)ui16((value - _fpdt_data_.origin16) * _fpdt_data_.maxDivRange16); }
   inline cfp16nx4 toFixed4(cfl32x4 &value) const {
      return (cfp16nx4 &)_mm_shuffle_epi8(_mm_cvttps_epi32( _mm_mul_ps(_mm_sub_ps(value, _mm_set_ps1(_fpdt_data_.origin16)), _mm_set_ps1(_fpdt_data_.maxDivRange16))), _fpdt_shuffle16s);
   }
   inline cfl32x4 toFloat4(void) const {
      return _mm_add_ps(_mm_mul_ps(_mm_cvtepu32_ps(_mm_cvtepu16_epi32(_mm_cvtsi64_si128(data64))), _mm_set_ps1(_fpdt_data_.rangeDivMax16)), _mm_set_ps1(_fpdt_data_.origin16));
   };
   inline cfl32x4 toFloat4(cfp16nx4 &value) const {
      return _mm_add_ps(_mm_mul_ps(_mm_cvtepu32_ps(_mm_cvtepu16_epi32(_mm_cvtsi64_si128(value.data64))), _mm_set_ps1(_fpdt_data_.rangeDivMax16)), _mm_set_ps1(_fpdt_data_.origin16));
   };
   inline cfp16nx4 toFixed4Mod(cfp16nx4 &value) const {
      cfp16nx4 temp = (cfp16nx4 &)_mm_shuffle_epi8(_mm_cvttps_epi32(_mm_mul_ps(_mm_set_ps1(_fpdt_data_.origin16), _mm_set_ps1(_fpdt_data_.maxDivRange16))), _fpdt_shuffle16s);
      return cfp16nx4{ ui16(data16[0] - temp.data16[0]), ui16(data16[1] - temp.data16[1]), ui16(data16[2] - temp.data16[2]), ui16(data16[3] - temp.data16[3]) };
   }
   inline cfp16nx4 toFixed4Mod(cfl32x4 &value) const {
      return (cfp16nx4 &)_mm_shuffle_epi8(_mm_cvttps_epi32(_mm_mul_ps(value, _mm_set_ps1(_fpdt_data_.maxDivRange16))), _fpdt_shuffle16s);
   }

   fp16nx4(void) = default;
   fp16nx4(cfl32 floor, cfl32 ceiling) {
      _fpdt_data_.origin16      = floor;
      _fpdt_data_.range16       = ceiling - floor;
      _fpdt_data_.maxDivRange16 = 65535.0f / _fpdt_data_.range16;
      _fpdt_data_.rangeDivMax16 = _fpdt_data_.range16 / 65535.0f;
   };
   fp16nx4(cfp16n value, cui8 index) { data[index] = value.data; }
   fp16nx4(cui16 value, cui8 index) { data16[index] = value; }
   fp16nx4(csi32 value, cui8 index) { data16[index] = (ui16 &)value; }
   fp16nx4(cfl32 value, cui8 index) { data16[index] = ui16((value - _fpdt_data_.origin16) * _fpdt_data_.maxDivRange16); }
   fp16nx4(cfp16n (&value)[4]) { data64 = (ui64 &)value[0]; }
   fp16nx4(cui16 (&value)[4]) { data64 = (ui64 &)value[0]; }
   fp16nx4(cfp16n value) { data[0] = data[1] = data[2] = data[3] = value; }
   fp16nx4(cui16 value) { data16[0] = data16[1] = data16[2] = data16[3] = value; }
   fp16nx4(csi32 value) { data16[0] = (ui16 &)value; data16[1] = (ui16 &)value; data16[2] = (ui16 &)value; data16[3] = (ui16 &)value; }
   fp16nx4(cfl32 value) { cfp16n temp = toFixed(value); data16[0] = data16[1] = data16[2] = data16[3] = temp.data; }
   fp16nx4(cui64 value) { data64 = value; }
   fp16nx4(csi64 value) { data64 = (ui64 &)value; }
   fp16nx4(cfl32x4 value) { *this = toFixed4(value); }
#ifdef _VECTOR_STRUCTURES_
   fp16nx4(cVEC4Df value) { cSSE4Df32 temp = { .vector = value }; *this = toFixed4(temp.xmm); }
   fp16nx4(cSSE4Df32 value) { *this = toFixed4(value.xmm); }
#endif
   fp16nx4(cfp16n value0, cfp16n value1, cfp16n value2, cfp16n value3) { data[0] = value0; data[1] = value1; data[2] = value2; data[3] = value3; }
   fp16nx4(cui16 value0, cui16 value1, cui16 value2, cui16 value3) { data16[0] = value0; data16[1] = value1; data16[2] = value2; data16[3] = value3; }
   fp16nx4(csi32 value0, csi32 value1, csi32 value2, csi32 value3) { data16[0] = (ui16 &)value0; data16[1] = (ui16 &)value1; data16[2] = (ui16 &)value2; data16[3] = (ui16 &)value3; }
   fp16nx4(cfl32 value, cui8 index, cfl32 floor, cfl32 ceiling) {
      _fpdt_data_.origin16      = floor;
      _fpdt_data_.range16       = ceiling - floor;
      _fpdt_data_.maxDivRange16 = 65535.0f / _fpdt_data_.range16;
      _fpdt_data_.rangeDivMax16 = _fpdt_data_.range16 / 65535.0f;
      data16[index] = ui16((value - floor) * _fpdt_data_.maxDivRange16);
   };
   fp16nx4(cfl32x4 value, cfl32 floor, cfl32 ceiling) {
      _fpdt_data_.origin16      = floor;
      _fpdt_data_.range16       = ceiling - floor;
      _fpdt_data_.maxDivRange16 = 65535.0f / _fpdt_data_.range16;
      _fpdt_data_.rangeDivMax16 = _fpdt_data_.range16 / 65535.0f;
      data64 = toFixed4(value).data64;
   }

   operator ptr(void) const { return *this; }
   operator cfl32x4(void) const { return _mm_add_ps(_mm_mul_ps(_mm_cvtepu32_ps(_mm_cvtepu16_epi32(_mm_cvtsi64_si128(data64))), _mm_set_ps1(_fpdt_data_.rangeDivMax16)), _mm_set_ps1(_fpdt_data_.origin16)); }

   inline cfp16nx4 &operator&(void) const { return *this; }
   inline cfp16nx4 operator~(void) const { return (cfp16nx4 &)(data64 ^ 0x0FFFFFFFFFFFFFFFFu); }
   inline cfp16nx4 operator-(void) const { return (cfp16nx4 &)(data64 ^ 0x0FFFFFFFFFFFFFFFFu); }
   inline cbool operator!(void) const { return data64 == 0; }

   inline cfp16nx4 operator++(void) { data[0]++; data[1]++; data[2]++; data[3]++; return *this; }
   inline cfp16nx4 operator++(int) { cfp16nx4 temp = { data[0]++, data[1]++, data[2]++, data[3]++ }; return (cfp16nx4 &)temp; }
   inline cfp16nx4 operator--(void) { data[0]--; data[1]--; data[2]--; data[3]--; return *this; }
   inline cfp16nx4 operator--(int) { cfp16nx4 temp = { data[0]--, data[1]--, data[2]--, data[3]-- }; return (cfp16nx4 &)temp; }

   inline cfp16nx4 operator<<(csi32 &value) const { return fp16nx4{ data16[0] << value, data16[1] << value, data16[2] << value, data16[3] << value }; }
   inline cfp16nx4 operator>>(csi32 &value) const { return fp16nx4{ data16[0] >> value, data16[1] >> value, data16[2] >> value, data16[3] >> value }; }
   inline cfp16nx4 operator<<=(csi32 &value) { return fp16nx4{ data16[0] <<= value, data16[1] <<= value, data16[2] <<= value, data16[3] <<= value }; }
   inline cfp16nx4 operator>>=(csi32 &value) { return fp16nx4{ data16[0] >>= value, data16[1] >>= value, data16[2] >>= value, data16[3] >>= value }; }

// Each of the lower 4 bits represents the test result per variable
   inline cui8 operator==(cfp16nx4 &value) const { return (data16[0] == value.data16[0]) | ((data16[1] == value.data16[1]) << 1) | ((data16[2] == value.data16[2]) << 2) | ((data16[3] == value.data16[3]) << 3); }
   inline cui8 operator!=(cfp16nx4 &value) const { return (data16[0] != value.data16[0]) | ((data16[1] != value.data16[1]) << 1) | ((data16[2] != value.data16[2]) << 2) | ((data16[3] != value.data16[3]) << 3); }
   inline cui8 operator>=(cfp16nx4 &value) const { return (data16[0] >= value.data16[0]) | ((data16[1] >= value.data16[1]) << 1) | ((data16[2] >= value.data16[2]) << 2) | ((data16[3] >= value.data16[3]) << 3); }
   inline cui8 operator<=(cfp16nx4 &value) const { return (data16[0] <= value.data16[0]) | ((data16[1] <= value.data16[1]) << 1) | ((data16[2] <= value.data16[2]) << 2) | ((data16[3] <= value.data16[3]) << 3); }
   inline cui8 operator>(cfp16nx4 &value) const { return (data16[0] > value.data16[0]) | ((data16[1] > value.data16[1]) << 1) | ((data16[2] > value.data16[2]) << 2) | ((data16[3] > value.data16[3]) << 3); }
   inline cui8 operator<(cfp16nx4 &value) const { return (data16[0] < value.data16[0]) | ((data16[1] < value.data16[1]) << 1) | ((data16[2] < value.data16[2]) << 2) | ((data16[3] < value.data16[3]) << 3); }
   inline cui8 operator||(cfp16nx4 &value) const { return (data16[0] || value.data16[0]) | ((data16[1] || value.data16[1]) << 1) | ((data16[2] || value.data16[2]) << 2) | ((data16[3] || value.data16[3]) << 3); }
   inline cui8 operator&&(cfp16nx4 &value) const { return (data16[0] && value.data16[0]) | ((data16[1] && value.data16[1]) << 1) | ((data16[2] && value.data16[2]) << 2) | ((data16[3] && value.data16[3]) << 3); }

   inline cfp16nx4 operator+(cfp16nx4 &value) const { return (cfp16nx4 &)_mm_add_epi8(_mm_cvtsi64_si128(data64), _mm_cvtsi64_si128(value.data64)); }
   inline cfp16nx4 operator-(cfp16nx4 &value) const { return (cfp16nx4 &)_mm_sub_epi8(_mm_cvtsi64_si128(data64), _mm_cvtsi64_si128(value.data64)); }
   inline cfp16nx4 operator*(cfp16nx4 &value) const { return toFixed4(_mm_mul_ps(toFloat4(), toFloat4(value))); }
   inline cfp16nx4 operator/(cfp16nx4 &value) const { return toFixed4(_mm_div_ps(toFloat4(), toFloat4(value))); }
   inline cfp16nx4 operator%(cfp16nx4 &value) const { cfp16nx4 temp = toFixed4Mod(value); return cfp16nx4{ data16[0] % temp.data16[0], data16[1] % temp.data16[1], data16[2] % temp.data16[2], data16[3] % temp.data16[3] }; }
   inline cfp16nx4 operator&(cfp16nx4 &value) const { return (cfp16nx4 &)(data64 & value.data64); }
   inline cfp16nx4 operator|(cfp16nx4 &value) const { return (cfp16nx4 &)(data64 | value.data64); }
   inline cfp16nx4 operator^(cfp16nx4 &value) const { return (cfp16nx4 &)(data64 ^ value.data64); }
   inline cfp16nx4 operator+=(cfp16nx4 &value) { data64 = (ui64 &)_mm_add_epi8(_mm_cvtsi64_si128(data64), _mm_cvtsi64_si128(value.data64)); return *this; }
   inline cfp16nx4 operator-=(cfp16nx4 &value) { data64 = (ui64 &)_mm_sub_epi8(_mm_cvtsi64_si128(data64), _mm_cvtsi64_si128(value.data64)); return *this; }
   inline cfp16nx4 operator*=(cfp16nx4 &value) { return *this = toFixed4(_mm_mul_ps(toFloat4(), toFloat4(value))); }
   inline cfp16nx4 operator/=(cfp16nx4 &value) { return *this = toFixed4(_mm_div_ps(toFloat4(), toFloat4(value))); }
   inline cfp16nx4 operator%=(cfp16nx4 &value) { cfp16nx4 temp = toFixed4Mod(value); return cfp16nx4{ data16[0] %= temp.data16[0], data16[1] %= temp.data16[1], data16[2] %= temp.data16[2], data16[3] %= temp.data16[3] }; }
   inline cfp16nx4 operator&=(cfp16nx4 &value) { return (cfp16nx4 &)(data64 &= value.data64); }
   inline cfp16nx4 operator|=(cfp16nx4 &value) { return (cfp16nx4 &)(data64 |= value.data64); }
   inline cfp16nx4 operator^=(cfp16nx4 &value) { return (cfp16nx4 &)(data64 ^= value.data64); }

// Each of the lower 4 bits represents the test result per variable
   inline cui8 operator==(cfl32x4 &value) const {
      cfp16nx4 temp = toFixed4(value);
      return (data16[0] == temp.data16[0]) | ((data16[1] == temp.data16[1]) << 1) | ((data16[2] == temp.data16[2]) << 2) | ((data16[3] == temp.data16[3]) << 3);
   }
   inline cui8 operator!=(cfl32x4 &value) const {
      cfp16nx4 temp = toFixed4(value);
      return (data16[0] != temp.data16[0]) | ((data16[1] != temp.data16[1]) << 1) | ((data16[2] != temp.data16[2]) << 2) | ((data16[3] != temp.data16[3]) << 3);
   }
   inline cui8 operator>=(cfl32x4 &value) const {
      cfp16nx4 temp = toFixed4(value);
      return (data16[0] >= temp.data16[0]) | ((data16[1] >= temp.data16[1]) << 1) | ((data16[2] >= temp.data16[2]) << 2) | ((data16[3] >= temp.data16[3]) << 3);
   }
   inline cui8 operator<=(cfl32x4 &value) const {
      cfp16nx4 temp = toFixed4(value);
      return (data16[0] <= temp.data16[0]) | ((data16[1] <= temp.data16[1]) << 1) | ((data16[2] <= temp.data16[2]) << 2) | ((data16[3] <= temp.data16[3]) << 3);
   }
   inline cui8 operator>(cfl32x4 &value) const {
      cfp16nx4 temp = toFixed4(value);
      return (data16[0] > temp.data16[0]) | ((data16[1] > temp.data16[1]) << 1) | ((data16[2] > temp.data16[2]) << 2) | ((data16[3] > temp.data16[3]) << 3);
   }
   inline cui8 operator<(cfl32x4 &value) const {
      cfp16nx4 temp = toFixed4(value);
      return (data16[0] < temp.data16[0]) | ((data16[1] < temp.data16[1]) << 1) | ((data16[2] < temp.data16[2]) << 2) | ((data16[3] < temp.data16[3]) << 3);
   }
   inline cui8 operator||(cfl32x4 &value) const {
      cfp16nx4 temp = toFixed4(value);
      return (data16[0] || temp.data16[0]) | ((data16[1] || temp.data16[1]) << 1) | ((data16[2] || temp.data16[2]) << 2) | ((data16[3] || temp.data16[3]) << 3);
   }
   inline cui8 operator&&(cfl32x4 &value) const {
      cfp16nx4 temp = toFixed4(value);
      return (data16[0] && temp.data16[0]) | ((data16[1] && temp.data16[1]) << 1) | ((data16[2] && temp.data16[2]) << 2) | ((data16[3] && temp.data16[3]) << 3);
   }

   inline cfp16nx4 operator+(cfl32x4 &value) const { return toFixed4(_mm_add_ps(toFloat4(), value)); }
   inline cfp16nx4 operator-(cfl32x4 &value) const { return toFixed4(_mm_sub_ps(toFloat4(), value)); }
   inline cfp16nx4 operator*(cfl32x4 &value) const { return toFixed4(_mm_mul_ps(toFloat4(), value)); }
   inline cfp16nx4 operator/(cfl32x4 &value) const { return toFixed4(_mm_div_ps(toFloat4(), value)); }
   inline cfp16nx4 operator%(cfl32x4 &value) const {
      cfp16nx4 temp = toFixed4Mod(value);
      return fp16nx4{ data16[0] % temp.data16[0], data16[1] % temp.data16[1], data16[2] % temp.data16[2], data16[3] % temp.data16[3] };
   }
   inline cfp16nx4 operator&(cfl32x4 &value) const { return (cfp16nx4 &)(data64 & toFixed4(value).data64); }
   inline cfp16nx4 operator|(cfl32x4 &value) const { return (cfp16nx4 &)(data64 | toFixed4(value).data64); }
   inline cfp16nx4 operator^(cfl32x4 &value) const { return (cfp16nx4 &)(data64 ^ toFixed4(value).data64); }
   inline cfp16nx4 operator+=(cfl32x4 &value) { return *this = toFixed4(_mm_add_ps(toFloat4(), value)); }
   inline cfp16nx4 operator-=(cfl32x4 &value) { return *this = toFixed4(_mm_sub_ps(toFloat4(), value)); }
   inline cfp16nx4 operator*=(cfl32x4 &value) { return *this = toFixed4(_mm_mul_ps(toFloat4(), value)); }
   inline cfp16nx4 operator/=(cfl32x4 &value) { return *this = toFixed4(_mm_div_ps(toFloat4(), value)); }
   inline cfp16nx4 operator%=(cfl32x4 &value) {
      cfp16nx4 temp = toFixed4Mod(value);
      return *this = fp16nx4{ data16[0] % temp.data16[0], data16[1] % temp.data16[1], data16[2] % temp.data16[2], data16[3] % temp.data16[3] };
   }
   inline cfp16nx4 operator&=(cfl32x4 &value) { (cfp16nx4 &)(data64 &= toFixed4(value).data64); }
   inline cfp16nx4 operator|=(cfl32x4 &value) { (cfp16nx4 &)(data64 |= toFixed4(value).data64); }
   inline cfp16nx4 operator^=(cfl32x4 &value) { (cfp16nx4 &)(data64 ^= toFixed4(value).data64); }
};

#ifdef _24BIT_INTEGERS_

// Normalised 24-bit : User-defineable decimal range
struct fp24n {
   typedef const fp24n cfp24n;

   ui24 data;

   inline cui24 toFixed(cfl32 &value) const { return ui24((value - _fpdt_data_.origin24) * _fpdt_data_.maxDivRange24); }
   inline cfl32 toFloat(void) const { return fl32(data) * _fpdt_data_.rangeDivMax24 + _fpdt_data_.origin24; };
   inline cfl32 toFloat(cfp24n &value) const { return fl32(value.data) * _fpdt_data_.rangeDivMax24 + _fpdt_data_.origin24; };
   inline cui24 toFixedMod(cfp24n &value) const { return (value.data - ui24(_fpdt_data_.origin24 * _fpdt_data_.maxDivRange24)); }
   inline cui24 toFixedMod(cfl32 &value) const { return ui24(value * _fpdt_data_.maxDivRange24); }

   fp24n(void) = default;
   fp24n(cfl32 floor, cfl32 ceiling) {
      _fpdt_data_.origin24      = floor;
      _fpdt_data_.range24       = ceiling - floor;
      _fpdt_data_.maxDivRange24 = 16777215.0f / _fpdt_data_.range24;
      _fpdt_data_.rangeDivMax24 = _fpdt_data_.range24 / 16777215.0f;
   };
   fp24n(cfl32 value, cfl32 floor, cfl32 ceiling) {
      _fpdt_data_.origin24      = floor;
      _fpdt_data_.range24       = ceiling - floor;
      _fpdt_data_.maxDivRange24 = 16777215.0f / _fpdt_data_.range24;
      _fpdt_data_.rangeDivMax24 = _fpdt_data_.range24 / 16777215.0f;

      data = ui24((value - floor) * _fpdt_data_.maxDivRange24);
   };
   fp24n(cui8 value) { data = toFixed(fl32(value) * _fpdt_data_.rangeDivMax8 + _fpdt_data_.origin8); }
   fp24n(cui16 value) { data = toFixed(fl32(value) * _fpdt_data_.rangeDivMax16 + _fpdt_data_.origin16); }
   fp24n(cui24 value) { data = value; }
   fp24n(cui32 value) { data = toFixed(fl32(fl64(value) * _fpdt_data_.rangeDivMax32 + _fpdt_data_.origin32)); }
   fp24n(csi32 value) { data = (ui24 &)value; }
   fp24n(cfl32 value) { data = toFixed(value); }

   operator ptr(void) const { return *this; }
   operator cfl32(void) const { return fl32(data) * _fpdt_data_.rangeDivMax24 + _fpdt_data_.origin24; };

   inline cfp24n &operator&(void) const { return *this; }
   inline cfp24n operator~(void) const { return (cfp24n &)~data; }
   inline cfp24n operator-(void) const { return (cfp24n &)~data; }
   inline cbool operator!(void) const { return data == 0; }

   inline cfp24n operator++(void) { data++; return *this; }
   inline cfp24n operator++(int) { cui24 temp = data++; return (cfp24n &)temp; }
   inline cfp24n operator--(void) { data--; return *this; }
   inline cfp24n operator--(int) { cui24 temp = data--; return (cfp24n &)temp; }

   inline cbool operator==(csi32 &value) const { return data == (ui24 &)data; }
   inline cbool operator!=(csi32 &value) const { return data != (ui24 &)data; }
   inline cbool operator>=(csi32 &value) const { return data >= (ui24 &)data; }
   inline cbool operator<=(csi32 &value) const { return data <= (ui24 &)data; }
   inline cbool operator>(csi32 &value) const { return data > (ui24 &)data; }
   inline cbool operator<(csi32 &value) const { return data < (ui24 &)data; }
   inline cbool operator||(csi32 &value) const { return data || (ui24 &)data; }
   inline cbool operator&&(csi32 &value) const { return data && (ui24 &)data; }

   inline cfp24n operator<<(csi32 &value) const { return (cfp24n &)(data << value); }
   inline cfp24n operator>>(csi32 &value) const { return (cfp24n &)(data >> value); }
   inline cfp24n operator<<=(csi32 &value) { return (cfp24n &)(data <<= value); }
   inline cfp24n operator>>=(csi32 &value) { return (cfp24n &)(data >>= value); }

   inline cbool operator==(cfp24n &value) const { return data == value.data; }
   inline cbool operator!=(cfp24n &value) const { return data != value.data; }
   inline cbool operator>=(cfp24n &value) const { return data >= value.data; }
   inline cbool operator<=(cfp24n &value) const { return data <= value.data; }
   inline cbool operator>(cfp24n &value) const { return data > value.data; }
   inline cbool operator<(cfp24n &value) const { return data < value.data; }
   inline cbool operator||(cfp24n &value) const { return data || value.data; }
   inline cbool operator&&(cfp24n &value) const { return data && value.data; }

   inline cfp24n operator+(cfp24n &value) const { return (cfp24n &)(data + toFixedMod(value)); }
   inline cfp24n operator-(cfp24n &value) const { return (cfp24n &)(data - toFixedMod(value)); }
   inline cfp24n operator*(cfp24n &value) const { return toFixed(toFloat() * toFloat(value)); }
   inline cfp24n operator/(cfp24n &value) const { return toFixed(toFloat() / toFloat(value)); }
   inline cfp24n operator%(cfp24n &value) const { return (cfp24n &)(data % toFixedMod(value)); }
   inline cfp24n operator&(cfp24n &value) const { return (cfp24n &)(data & value.data); }
   inline cfp24n operator|(cfp24n &value) const { return (cfp24n &)(data | value.data); }
   inline cfp24n operator^(cfp24n &value) const { return (cfp24n &)(data ^ value.data); }
   inline cfp24n operator+=(cfp24n &value) { return (cfp24n &)(data += toFixedMod(value)); }
   inline cfp24n operator-=(cfp24n &value) { return (cfp24n &)(data -= toFixedMod(value)); }
   inline cfp24n operator*=(cfp24n &value) { return *this = toFixed(toFloat() * toFloat(value)); }
   inline cfp24n operator/=(cfp24n &value) { return *this = toFixed(toFloat() / toFloat(value)); }
   inline cfp24n operator%=(cfp24n &value) { return (cfp24n &)(data %= toFixedMod(value)); }
   inline cfp24n operator&=(cfp24n &value) { return (cfp24n &)(data &= value.data); }
   inline cfp24n operator|=(cfp24n &value) { return (cfp24n &)(data |= value.data); }
   inline cfp24n operator^=(cfp24n &value) { return (cfp24n &)(data ^= value.data); }

   inline cbool operator==(cfl32 &value) const { return data == toFixed(value); }
   inline cbool operator!=(cfl32 &value) const { return data != toFixed(value); }
   inline cbool operator>=(cfl32 &value) const { return data <= toFixed(value); }
   inline cbool operator<=(cfl32 &value) const { return data >= toFixed(value); }
   inline cbool operator>(cfl32 &value) const { return data < toFixed(value); }
   inline cbool operator<(cfl32 &value) const { return data > toFixed(value); }
   inline cbool operator||(cfl32 &value) const { return data || toFixed(value); }
   inline cbool operator&&(cfl32 &value) const { return data && toFixed(value); }

   inline cfl32 operator+(cfl32 &value) const { return toFloat() + value; }
   inline cfl32 operator-(cfl32 &value) const { return toFloat() - value; }
   inline cfl32 operator*(cfl32 &value) const { return toFloat() * value; }
   inline cfl32 operator/(cfl32 &value) const { return toFloat() / value; }
   inline cfp24n operator%(cfl32 &value) const { return (cfp24n &)(data % toFixed(value)); }
   inline cfp24n operator&(cfl32 &value) const { return (cfp24n &)(data & toFixed(value)); }
   inline cfp24n operator|(cfl32 &value) const { return (cfp24n &)(data | toFixed(value)); }
   inline cfp24n operator^(cfl32 &value) const { return (cfp24n &)(data ^ toFixed(value)); }
   inline cfl32 operator+=(cfl32 &value) { cfl32 temp = toFloat() + value; *this = toFixed(temp); return temp; }
   inline cfl32 operator-=(cfl32 &value) { cfl32 temp = toFloat() - value; *this = toFixed(temp); return temp; }
   inline cfl32 operator*=(cfl32 &value) { cfl32 temp = toFloat() * value; *this = toFixed(temp); return temp; }
   inline cfl32 operator/=(cfl32 &value) { cfl32 temp = toFloat() / value; *this = toFixed(temp); return temp; }
   inline cfp24n operator%=(cfl32 &value) { return (cfp24n &)(data %= toFixed(value)); }
   inline cfp24n operator&=(cfl32 &value) { return (cfp24n &)(data &= toFixed(value)); }
   inline cfp24n operator|=(cfl32 &value) { return (cfp24n &)(data |= toFixed(value)); }
   inline cfp24n operator^=(cfl32 &value) { return (cfp24n &)(data ^= toFixed(value)); }
};

#endif // #ifdef _24BIT_INTEGERS_

// Normalised 32-bit : User-defineable decimal range
struct fp32n {
   typedef const fp32n cfp32n;

   ui32 data;

   inline cui32 toFixed(cfl64 &value) const { return ui32((value - _fpdt_data_.origin32) * _fpdt_data_.maxDivRange32); }
   inline cfl64 toFloat(void) const { return fl64(data) * _fpdt_data_.rangeDivMax32 + _fpdt_data_.origin32; };
   inline cfl64 toFloat(cfp32n &value) const { return fl64(value.data) * _fpdt_data_.rangeDivMax32 + _fpdt_data_.origin32; };
   inline cui32 toFixedMod(cfp32n &value) const { return (value.data - ui32(_fpdt_data_.origin32 * _fpdt_data_.maxDivRange32)); }
   inline cui32 toFixedMod(cfl64 &value) const { return ui32(value * _fpdt_data_.maxDivRange32); }

   fp32n(void) = default;
   fp32n(cfl64 floor, cfl64 ceiling) {
      _fpdt_data_.origin32      = floor;
      _fpdt_data_.range32       = ceiling - floor;
      _fpdt_data_.maxDivRange32 = 4294967296.0 / _fpdt_data_.range32;
      _fpdt_data_.rangeDivMax32 = _fpdt_data_.range32 / 4294967296.0;
   };
   fp32n(cfl64 value, cfl64 floor, cfl64 ceiling) {
      _fpdt_data_.origin32      = floor;
      _fpdt_data_.range32       = ceiling - floor;
      _fpdt_data_.maxDivRange32 = 4294967296.0 / _fpdt_data_.range32;
      _fpdt_data_.rangeDivMax32 = _fpdt_data_.range32 / 4294967296.0;

      data = ui32((value - floor) * _fpdt_data_.maxDivRange32);
   };
   fp32n(cui8 value) { data = toFixed(fl32(value) * _fpdt_data_.rangeDivMax8 + _fpdt_data_.origin8); }
   fp32n(cui16 value) { data = toFixed(fl32(value) * _fpdt_data_.rangeDivMax16 + _fpdt_data_.origin16); }
#ifdef _24BIT_INTEGERS_
   fp32n(cui24 value) { data = toFixed(fl32(value) * _fpdt_data_.rangeDivMax24 + _fpdt_data_.origin24); }
#endif
   fp32n(cui32 value) { data = value; }
   fp32n(csi32 value) { data = (ui32 &)value; }
   fp32n(cfl64 value) { data = toFixed(value); }

   operator ptr(void) const { return *this; }
   operator cfl64(void) const { return fl64(data) * _fpdt_data_.rangeDivMax32 + _fpdt_data_.origin32; };

   inline cfp32n &operator&(void) const { return *this; }
   inline cfp32n operator~(void) const { return (cfp32n &)~data; }
   inline cfp32n operator-(void) const { return (cfp32n &)~data; }
   inline cbool operator!(void) const { return data == 0; }

   inline cfp32n operator++(void) { data++; return *this; }
   inline cfp32n operator++(int) { cui32 temp = data++; return (cfp32n &)temp; }
   inline cfp32n operator--(void) { data--; return *this; }
   inline cfp32n operator--(int) { cui32 temp = data--; return (cfp32n &)temp; }

   inline cbool operator==(csi32 &value) const { return data == (ui32 &)data; }
   inline cbool operator!=(csi32 &value) const { return data != (ui32 &)data; }
   inline cbool operator>=(csi32 &value) const { return data >= (ui32 &)data; }
   inline cbool operator<=(csi32 &value) const { return data <= (ui32 &)data; }
   inline cbool operator>(csi32 &value) const { return data > (ui32 &)data; }
   inline cbool operator<(csi32 &value) const { return data < (ui32 &)data; }
   inline cbool operator||(csi32 &value) const { return data || (ui32 &)data; }
   inline cbool operator&&(csi32 &value) const { return data && (ui32 &)data; }

   inline cfp32n operator<<(csi32 &value) const { return (cfp32n &)(data << value); }
   inline cfp32n operator>>(csi32 &value) const { return (cfp32n &)(data >> value); }
   inline cfp32n operator<<=(csi32 &value) { return (cfp32n &)(data <<= value); }
   inline cfp32n operator>>=(csi32 &value) { return (cfp32n &)(data >>= value); }

   inline cbool operator==(cfp32n &value) const { return data == value.data; }
   inline cbool operator!=(cfp32n &value) const { return data != value.data; }
   inline cbool operator>=(cfp32n &value) const { return data >= value.data; }
   inline cbool operator<=(cfp32n &value) const { return data <= value.data; }
   inline cbool operator>(cfp32n &value) const { return data > value.data; }
   inline cbool operator<(cfp32n &value) const { return data < value.data; }
   inline cbool operator||(cfp32n &value) const { return data || value.data; }
   inline cbool operator&&(cfp32n &value) const { return data && value.data; }

   inline cfp32n operator+(cfp32n &value) const { return (cfp32n &)(data + toFixedMod(value)); }
   inline cfp32n operator-(cfp32n &value) const { return (cfp32n &)(data - toFixedMod(value)); }
   inline cfp32n operator*(cfp32n &value) const { return toFixed(toFloat() * toFloat(value)); }
   inline cfp32n operator/(cfp32n &value) const { return toFixed(toFloat() / toFloat(value)); }
   inline cfp32n operator%(cfp32n &value) const { return (cfp32n &)(data % toFixedMod(value)); }
   inline cfp32n operator&(cfp32n &value) const { return (cfp32n &)(data & value.data); }
   inline cfp32n operator|(cfp32n &value) const { return (cfp32n &)(data | value.data); }
   inline cfp32n operator^(cfp32n &value) const { return (cfp32n &)(data ^ value.data); }
   inline cfp32n operator+=(cfp32n &value) { return (cfp32n &)(data += toFixedMod(value)); }
   inline cfp32n operator-=(cfp32n &value) { return (cfp32n &)(data -= toFixedMod(value)); }
   inline cfp32n operator*=(cfp32n &value) { return data = toFixed(toFloat() * toFloat(value)); }
   inline cfp32n operator/=(cfp32n &value) { return data = toFixed(toFloat() / toFloat(value)); }
   inline cfp32n operator%=(cfp32n &value) { return (cfp32n &)(data %= toFixedMod(value)); }
   inline cfp32n operator&=(cfp32n &value) { return (cfp32n &)(data &= value.data); }
   inline cfp32n operator|=(cfp32n &value) { return (cfp32n &)(data |= value.data); }
   inline cfp32n operator^=(cfp32n &value) { return (cfp32n &)(data ^= value.data); }

   inline cbool operator==(cfl64 &value) const { return data == toFixed(value); }
   inline cbool operator!=(cfl64 &value) const { return data != toFixed(value); }
   inline cbool operator>=(cfl64 &value) const { return data <= toFixed(value); }
   inline cbool operator<=(cfl64 &value) const { return data >= toFixed(value); }
   inline cbool operator>(cfl64 &value) const { return data < toFixed(value); }
   inline cbool operator<(cfl64 &value) const { return data > toFixed(value); }
   inline cbool operator||(cfl64 &value) const { return data || toFixed(value); }
   inline cbool operator&&(cfl64 &value) const { return data && toFixed(value); }

   inline cfl64 operator+(cfl64 &value) const { return toFloat() + value; }
   inline cfl64 operator-(cfl64 &value) const { return toFloat() - value; }
   inline cfl64 operator*(cfl64 &value) const { return toFloat() * value; }
   inline cfl64 operator/(cfl64 &value) const { return toFloat() / value; }
   inline cfp32n operator%(cfl64 &value) const { return (cfp32n &)(data % toFixed(value)); }
   inline cfp32n operator&(cfl64 &value) const { return (cfp32n &)(data & toFixed(value)); }
   inline cfp32n operator|(cfl64 &value) const { return (cfp32n &)(data | toFixed(value)); }
   inline cfp32n operator^(cfl64 &value) const { return (cfp32n &)(data ^ toFixed(value)); }
   inline cfl64 operator+=(cfl64 &value) { cfl64 temp = toFloat() + value; *this = toFixed(temp); return temp; }
   inline cfl64 operator-=(cfl64 &value) { cfl64 temp = toFloat() - value; *this = toFixed(temp); return temp; }
   inline cfl64 operator*=(cfl64 &value) { cfl64 temp = toFloat() * value; *this = toFixed(temp); return temp; }
   inline cfl64 operator/=(cfl64 &value) { cfl64 temp = toFloat() / value; *this = toFixed(temp); return temp; }
   inline cfp32n operator%=(cfl64 &value) { return (cfp32n &)(data %= toFixed(value)); }
   inline cfp32n operator&=(cfl64 &value) { return (cfp32n &)(data &= toFixed(value)); }
   inline cfp32n operator|=(cfl64 &value) { return (cfp32n &)(data |= toFixed(value)); }
   inline cfp32n operator^=(cfl64 &value) { return (cfp32n &)(data ^= toFixed(value)); }
};

#endif // #ifndef FPDT_NO_CUSTOM

// 8-bit, 0.8 : Decimal range of 0.0~0.99609375
struct f0p8 {
   typedef const f0p8 cf0p8;

   ui8 data;

   inline cui8 toFixed(cfl32 &value) const { return ui8(value * 256.0f); }
   inline cfl32 toFloat(void) const { return cfl32(data) * _fpdt_rcp256f; }
   inline cfl32 toFloat(cf0p8 &value) const { return fl32(value.data) * _fpdt_rcp256f; }

   f0p8(void) = default;
   f0p8(const fp8n value) { data = toFixed(fl32(value) * _fpdt_data_.rangeDivMax8 + _fpdt_data_.origin8); }
   f0p8(const fp16n value) { data = toFixed(fl32(value) * _fpdt_data_.rangeDivMax16 + _fpdt_data_.origin16); }
#ifdef _24BIT_INTEGERS_
   f0p8(const fp24n value) { data = toFixed(fl32(value) * _fpdt_data_.rangeDivMax24 + _fpdt_data_.origin24); }
#endif
   f0p8(const fp32n value) { data = toFixed(fl32(fl64(value) * _fpdt_data_.rangeDivMax32 + _fpdt_data_.origin32)); }
   f0p8(cui8 value) { data = value; }
   f0p8(csi32 value) { data = (ui8 &)value; }
   f0p8(csi64 value) { data = (ui8 &)value; }
   f0p8(cfl32 value) { data = toFixed(value); }

   operator ptr(void) const { return *this; }
   operator cfl32(void) const { return cfl32(data) * _fpdt_rcp256f; }

   inline cf0p8 &operator&(void) const { return *this; }
   inline cf0p8 operator~(void) const { return (cf0p8 &)~data; }
   inline cf0p8 operator-(void) const { return (cf0p8 &)~data; }
   inline cbool operator!(void) const { return data == 0; }

   inline cf0p8 operator++(void) { data++; return *this; }
   inline cf0p8 operator++(int) { cui8 temp = data++; return (cf0p8 &)temp; }
   inline cf0p8 operator--(void) { data--; return *this; }
   inline cf0p8 operator--(int) { cui8 temp = data--; return (cf0p8 &)temp; }

   inline cbool operator==(csi32 &value) const { return data == (ui8 &)data; }
   inline cbool operator!=(csi32 &value) const { return data != (ui8 &)data; }
   inline cbool operator>=(csi32 &value) const { return data >= (ui8 &)data; }
   inline cbool operator<=(csi32 &value) const { return data <= (ui8 &)data; }
   inline cbool operator>(csi32 &value) const { return data > (ui8 &)data; }
   inline cbool operator<(csi32 &value) const { return data < (ui8 &)data; }
   inline cbool operator||(csi32 &value) const { return data || (ui8 &)data; }
   inline cbool operator&&(csi32 &value) const { return data && (ui8 &)data; }

   inline cf0p8 operator<<(csi32 &value) const { return (cf0p8 &)(data << value); }
   inline cf0p8 operator>>(csi32 &value) const { return (cf0p8 &)(data >> value); }
   inline cf0p8 operator<<=(csi32 &value) { return (cf0p8 &)(data <<= value); }
   inline cf0p8 operator>>=(csi32 &value) { return (cf0p8 &)(data >>= value); }

   inline cbool operator==(cf0p8 &value) const { return data == value.data; }
   inline cbool operator!=(cf0p8 &value) const { return data != value.data; }
   inline cbool operator>=(cf0p8 &value) const { return data >= value.data; }
   inline cbool operator<=(cf0p8 &value) const { return data <= value.data; }
   inline cbool operator>(cf0p8 &value) const { return data > value.data; }
   inline cbool operator<(cf0p8 &value) const { return data < value.data; }
   inline cbool operator||(cf0p8 &value) const { return data || value.data; }
   inline cbool operator&&(cf0p8 &value) const { return data && value.data; }

   inline cf0p8 operator+(cf0p8 &value) const { return (cf0p8 &)(data + value.data); }
   inline cf0p8 operator-(cf0p8 &value) const { return (cf0p8 &)(data - value.data); }
   inline cf0p8 operator*(cf0p8 &value) const { return toFixed(toFloat() * toFloat(value)); }
   inline cf0p8 operator/(cf0p8 &value) const { return toFixed(toFloat() / toFloat(value)); }
   inline cf0p8 operator%(cf0p8 &value) const { return (cf0p8 &)(data % value.data); }
   inline cf0p8 operator&(cf0p8 &value) const { return (cf0p8 &)(data & value.data); }
   inline cf0p8 operator|(cf0p8 &value) const { return (cf0p8 &)(data | value.data); }
   inline cf0p8 operator^(cf0p8 &value) const { return (cf0p8 &)(data ^ value.data); }
   inline cf0p8 operator+=(cf0p8 &value) { return (cf0p8 &)(data += value.data); }
   inline cf0p8 operator-=(cf0p8 &value) { return (cf0p8 &)(data -= value.data); }
   inline cf0p8 operator*=(cf0p8 &value) { return data = toFixed(toFloat() * toFloat(value)); }
   inline cf0p8 operator/=(cf0p8 &value) { return data = toFixed(toFloat() / toFloat(value)); }
   inline cf0p8 operator%=(cf0p8 &value) { return (cf0p8 &)(data %= value.data); }
   inline cf0p8 operator&=(cf0p8 &value) { return (cf0p8 &)(data &= value.data); }
   inline cf0p8 operator|=(cf0p8 &value) { return (cf0p8 &)(data |= value.data); }
   inline cf0p8 operator^=(cf0p8 &value) { return (cf0p8 &)(data ^= value.data); }

   inline cfl32 operator+(cfl32 &value) const { return toFloat() + value; }
   inline cfl32 operator-(cfl32 &value) const { return toFloat() - value; }
   inline cfl32 operator*(cfl32 &value) const { return toFloat() * value; }
   inline cfl32 operator/(cfl32 &value) const { return toFloat() / value; }
   inline cf0p8 operator%(cfl32 &value) const { return (cf0p8 &)(data % toFixed(value)); }
   inline cf0p8 operator&(cfl32 &value) const { return (cf0p8 &)(data & toFixed(value)); }
   inline cf0p8 operator|(cfl32 &value) const { return (cf0p8 &)(data | toFixed(value)); }
   inline cf0p8 operator^(cfl32 &value) const { return (cf0p8 &)(data ^ toFixed(value)); }
   inline cfl32 operator+=(cfl32 &value) { cfl32 temp = toFloat() + value; *this = toFixed(temp); return temp; }
   inline cfl32 operator-=(cfl32 &value) { cfl32 temp = toFloat() - value; *this = toFixed(temp); return temp; }
   inline cfl32 operator*=(cfl32 &value) { cfl32 temp = toFloat() * value; *this = toFixed(temp); return temp; }
   inline cfl32 operator/=(cfl32 &value) { cfl32 temp = toFloat() / value; *this = toFixed(temp); return temp; }
   inline cf0p8 operator%=(cfl32 &value) { return (cf0p8 &)(data %= toFixed(value)); }
   inline cf0p8 operator&=(cfl32 &value) { return (cf0p8 &)(data &= toFixed(value)); }
   inline cf0p8 operator|=(cfl32 &value) { return (cf0p8 &)(data |= toFixed(value)); }
   inline cf0p8 operator^=(cfl32 &value) { return (cf0p8 &)(data ^= toFixed(value)); }

   inline cbool operator==(cfl32 &value) const { return data == toFixed(value); }
   inline cbool operator!=(cfl32 &value) const { return data != toFixed(value); }
   inline cbool operator>=(cfl32 &value) const { return data <= toFixed(value); }
   inline cbool operator<=(cfl32 &value) const { return data >= toFixed(value); }
   inline cbool operator>(cfl32 &value) const { return data < toFixed(value); }
   inline cbool operator<(cfl32 &value) const { return data > toFixed(value); }
   inline cbool operator||(cfl32 &value) const { return data || toFixed(value); }
   inline cbool operator&&(cfl32 &value) const { return data && toFixed(value); }
};

// 8-bit, 1.7 : Decimal range of 0.0~1.9921875
struct f1p7 {
   typedef const f1p7 cf1p7;

   ui8 data;

   inline cui8 toFixed(cfl32 &value) const { return ui8(value * 128.0f); }
   inline cfl32 toFloat(void) const { return cfl32(data) * _fpdt_rcp128f; }
   inline cfl32 toFloat(cf1p7 &value) const { return fl32(value.data) * _fpdt_rcp128f; }

   f1p7(void) = default;
   f1p7(const fp8n value) { data = toFixed(fl32(value) * _fpdt_data_.rangeDivMax8 + _fpdt_data_.origin8); }
   f1p7(const fp16n value) { data = toFixed(fl32(value) * _fpdt_data_.rangeDivMax16 + _fpdt_data_.origin16); }
#ifdef _24BIT_INTEGERS_
   f1p7(const fp24n value) { data = toFixed(fl32(value) * _fpdt_data_.rangeDivMax24 + _fpdt_data_.origin24); }
#endif
   f1p7(const fp32n value) { data = toFixed(fl32(fl64(value) * _fpdt_data_.rangeDivMax32 + _fpdt_data_.origin32)); }
   f1p7(cui8 value) { data = value; }
   f1p7(csi32 value) { data = (ui8 &)value; }
   f1p7(csi64 value) { data = (ui8 &)value; }
   f1p7(cfl32 value) { data = toFixed(value); }

   operator ptr(void) const { return *this; }
   operator cfl32(void) const { return cfl32(data) * _fpdt_rcp128f; }

   inline cf1p7 &operator&(void) const { return *this; }
   inline cf1p7 operator~(void) const { return (cf1p7 &)~data; }
   inline cf1p7 operator-(void) const { return (cf1p7 &)~data; }
   inline cbool operator!(void) const { return data == 0; }

   inline cf1p7 operator++(void) { data++; return *this; }
   inline cf1p7 operator++(int) { cui8 temp = data++; return (cf1p7 &)temp; }
   inline cf1p7 operator--(void) { data--; return *this; }
   inline cf1p7 operator--(int) { cui8 temp = data--; return (cf1p7 &)temp; }

   inline cbool operator==(csi32 &value) const { return data == (ui8 &)data; }
   inline cbool operator!=(csi32 &value) const { return data != (ui8 &)data; }
   inline cbool operator>=(csi32 &value) const { return data >= (ui8 &)data; }
   inline cbool operator<=(csi32 &value) const { return data <= (ui8 &)data; }
   inline cbool operator>(csi32 &value) const { return data > (ui8 &)data; }
   inline cbool operator<(csi32 &value) const { return data < (ui8 &)data; }
   inline cbool operator||(csi32 &value) const { return data || (ui8 &)data; }
   inline cbool operator&&(csi32 &value) const { return data && (ui8 &)data; }

   inline cf1p7 operator<<(csi32 &value) const { return (cf1p7 &)(data << value); }
   inline cf1p7 operator>>(csi32 &value) const { return (cf1p7 &)(data >> value); }
   inline cf1p7 operator<<=(csi32 &value) { return (cf1p7 &)(data <<= value); }
   inline cf1p7 operator>>=(csi32 &value) { return (cf1p7 &)(data >>= value); }

   inline cbool operator==(cf1p7 &value) const { return data == value.data; }
   inline cbool operator!=(cf1p7 &value) const { return data != value.data; }
   inline cbool operator>=(cf1p7 &value) const { return data >= value.data; }
   inline cbool operator<=(cf1p7 &value) const { return data <= value.data; }
   inline cbool operator>(cf1p7 &value) const { return data > value.data; }
   inline cbool operator<(cf1p7 &value) const { return data < value.data; }
   inline cbool operator||(cf1p7 &value) const { return data || value.data; }
   inline cbool operator&&(cf1p7 &value) const { return data && value.data; }

   inline cf1p7 operator+(cf1p7 &value) const { return (cf1p7 &)(data + value.data); }
   inline cf1p7 operator-(cf1p7 &value) const { return (cf1p7 &)(data - value.data); }
   inline cf1p7 operator*(cf1p7 &value) const { return toFixed(toFloat() * toFloat(value)); }
   inline cf1p7 operator/(cf1p7 &value) const { return toFixed(toFloat() / toFloat(value)); }
   inline cf1p7 operator%(cf1p7 &value) const { return (cf1p7 &)(data % value.data); }
   inline cf1p7 operator&(cf1p7 &value) const { return (cf1p7 &)(data & value.data); }
   inline cf1p7 operator|(cf1p7 &value) const { return (cf1p7 &)(data | value.data); }
   inline cf1p7 operator^(cf1p7 &value) const { return (cf1p7 &)(data ^ value.data); }
   inline cf1p7 operator+=(cf1p7 &value) { return (cf1p7 &)(data += value.data); }
   inline cf1p7 operator-=(cf1p7 &value) { return (cf1p7 &)(data -= value.data); }
   inline cf1p7 operator*=(cf1p7 &value) { return data = toFixed(toFloat() * toFloat(value)); }
   inline cf1p7 operator/=(cf1p7 &value) { return data = toFixed(toFloat() / toFloat(value)); }
   inline cf1p7 operator%=(cf1p7 &value) { return (cf1p7 &)(data %= value.data); }
   inline cf1p7 operator&=(cf1p7 &value) { return (cf1p7 &)(data &= value.data); }
   inline cf1p7 operator|=(cf1p7 &value) { return (cf1p7 &)(data |= value.data); }
   inline cf1p7 operator^=(cf1p7 &value) { return (cf1p7 &)(data ^= value.data); }

   inline cfl32 operator+(cfl32 &value) const { return toFloat() + value; }
   inline cfl32 operator-(cfl32 &value) const { return toFloat() - value; }
   inline cfl32 operator*(cfl32 &value) const { return toFloat() * value; }
   inline cfl32 operator/(cfl32 &value) const { return toFloat() / value; }
   inline cf1p7 operator%(cfl32 &value) const { return (cf1p7 &)(data % toFixed(value)); }
   inline cf1p7 operator&(cfl32 &value) const { return (cf1p7 &)(data & toFixed(value)); }
   inline cf1p7 operator|(cfl32 &value) const { return (cf1p7 &)(data | toFixed(value)); }
   inline cf1p7 operator^(cfl32 &value) const { return (cf1p7 &)(data ^ toFixed(value)); }
   inline cfl32 operator+=(cfl32 &value) { cfl32 temp = toFloat() + value; *this = toFixed(temp); return temp; }
   inline cfl32 operator-=(cfl32 &value) { cfl32 temp = toFloat() - value; *this = toFixed(temp); return temp; }
   inline cfl32 operator*=(cfl32 &value) { cfl32 temp = toFloat() * value; *this = toFixed(temp); return temp; }
   inline cfl32 operator/=(cfl32 &value) { cfl32 temp = toFloat() / value; *this = toFixed(temp); return temp; }
   inline cf1p7 operator%=(cfl32 &value) { return (cf1p7 &)(data %= toFixed(value)); }
   inline cf1p7 operator&=(cfl32 &value) { return (cf1p7 &)(data &= toFixed(value)); }
   inline cf1p7 operator|=(cfl32 &value) { return (cf1p7 &)(data |= toFixed(value)); }
   inline cf1p7 operator^=(cfl32 &value) { return (cf1p7 &)(data ^= toFixed(value)); }

   inline cbool operator==(cfl32 &value) const { return data == toFixed(value); }
   inline cbool operator!=(cfl32 &value) const { return data != toFixed(value); }
   inline cbool operator>=(cfl32 &value) const { return data <= toFixed(value); }
   inline cbool operator<=(cfl32 &value) const { return data >= toFixed(value); }
   inline cbool operator>(cfl32 &value) const { return data < toFixed(value); }
   inline cbool operator<(cfl32 &value) const { return data > toFixed(value); }
   inline cbool operator||(cfl32 &value) const { return data || toFixed(value); }
   inline cbool operator&&(cfl32 &value) const { return data && toFixed(value); }
};

// 8-bit, 4.4 : Decimal range of 0.0~15.9375
struct f4p4 {
   typedef const f4p4 cf4p4;

   ui8 data;

   inline cui8 toFixed(cfl32 &value) const { return ui8(value * 16.0f); }
   inline cfl32 toFloat(void) const { return cfl32(data) * _fpdt_rcp16f; }
   inline cfl32 toFloat(cf4p4 &value) const { return fl32(value.data) * _fpdt_rcp16f; }

   f4p4(void) = default;
   f4p4(const fp8n value) { data = toFixed(fl32(value) * _fpdt_data_.rangeDivMax8 + _fpdt_data_.origin8); }
   f4p4(const fp16n value) { data = toFixed(fl32(value) * _fpdt_data_.rangeDivMax16 + _fpdt_data_.origin16); }
#ifdef _24BIT_INTEGERS_
   f4p4(const fp24n value) { data = toFixed(fl32(value) * _fpdt_data_.rangeDivMax24 + _fpdt_data_.origin24); }
#endif
   f4p4(const fp32n value) { data = toFixed(fl32(fl64(value) * _fpdt_data_.rangeDivMax32 + _fpdt_data_.origin32)); }
   f4p4(cui8 value) { data = value; }
   f4p4(csi32 value) { data = (ui8 &)value; }
   f4p4(csi64 value) { data = (ui8 &)value; }
   f4p4(cfl32 value) { data = toFixed(value); }

   operator ptr(void) const { return *this; }
   operator cfl32(void) const { return cfl32(data) * _fpdt_rcp16f; }

   inline cf4p4 &operator&(void) const { return *this; }
   inline cf4p4 operator~(void) const { return (cf4p4 &)~data; }
   inline cf4p4 operator-(void) const { return (cf4p4 &)~data; }
   inline cbool operator!(void) const { return data == 0; }

   inline cf4p4 operator++(void) { data++; return *this; }
   inline cf4p4 operator++(int) { cui8 temp = data++; return (cf4p4 &)temp; }
   inline cf4p4 operator--(void) { data--; return *this; }
   inline cf4p4 operator--(int) { cui8 temp = data--; return (cf4p4 &)temp; }

   inline cbool operator==(csi32 &value) const { return data == (ui8 &)data; }
   inline cbool operator!=(csi32 &value) const { return data != (ui8 &)data; }
   inline cbool operator>=(csi32 &value) const { return data >= (ui8 &)data; }
   inline cbool operator<=(csi32 &value) const { return data <= (ui8 &)data; }
   inline cbool operator>(csi32 &value) const { return data > (ui8 &)data; }
   inline cbool operator<(csi32 &value) const { return data < (ui8 &)data; }
   inline cbool operator||(csi32 &value) const { return data || (ui8 &)data; }
   inline cbool operator&&(csi32 &value) const { return data && (ui8 &)data; }

   inline cf4p4 operator<<(csi32 &value) const { return (cf4p4 &)(data << value); }
   inline cf4p4 operator>>(csi32 &value) const { return (cf4p4 &)(data >> value); }
   inline cf4p4 operator<<=(csi32 &value) { return (cf4p4 &)(data <<= value); }
   inline cf4p4 operator>>=(csi32 &value) { return (cf4p4 &)(data >>= value); }

   inline cbool operator==(cf4p4 &value) const { return data == value.data; }
   inline cbool operator!=(cf4p4 &value) const { return data != value.data; }
   inline cbool operator>=(cf4p4 &value) const { return data >= value.data; }
   inline cbool operator<=(cf4p4 &value) const { return data <= value.data; }
   inline cbool operator>(cf4p4 &value) const { return data > value.data; }
   inline cbool operator<(cf4p4 &value) const { return data < value.data; }
   inline cbool operator||(cf4p4 &value) const { return data || value.data; }
   inline cbool operator&&(cf4p4 &value) const { return data && value.data; }

   inline cf4p4 operator+(cf4p4 &value) const { return (cf4p4 &)(data + value.data); }
   inline cf4p4 operator-(cf4p4 &value) const { return (cf4p4 &)(data - value.data); }
   inline cf4p4 operator*(cf4p4 &value) const { return toFixed(toFloat() * toFloat(value)); }
   inline cf4p4 operator/(cf4p4 &value) const { return toFixed(toFloat() / toFloat(value)); }
   inline cf4p4 operator%(cf4p4 &value) const { return (cf4p4 &)(data % value.data); }
   inline cf4p4 operator&(cf4p4 &value) const { return (cf4p4 &)(data & value.data); }
   inline cf4p4 operator|(cf4p4 &value) const { return (cf4p4 &)(data | value.data); }
   inline cf4p4 operator^(cf4p4 &value) const { return (cf4p4 &)(data ^ value.data); }
   inline cf4p4 operator+=(cf4p4 &value) { return (cf4p4 &)(data += value.data); }
   inline cf4p4 operator-=(cf4p4 &value) { return (cf4p4 &)(data -= value.data); }
   inline cf4p4 operator*=(cf4p4 &value) { return data = toFixed(toFloat() * toFloat(value)); }
   inline cf4p4 operator/=(cf4p4 &value) { return data = toFixed(toFloat() / toFloat(value)); }
   inline cf4p4 operator%=(cf4p4 &value) { return (cf4p4 &)(data %= value.data); }
   inline cf4p4 operator&=(cf4p4 &value) { return (cf4p4 &)(data &= value.data); }
   inline cf4p4 operator|=(cf4p4 &value) { return (cf4p4 &)(data |= value.data); }
   inline cf4p4 operator^=(cf4p4 &value) { return (cf4p4 &)(data ^= value.data); }

   inline cfl32 operator+(cfl32 &value) const { return toFloat() + value; }
   inline cfl32 operator-(cfl32 &value) const { return toFloat() - value; }
   inline cfl32 operator*(cfl32 &value) const { return toFloat() * value; }
   inline cfl32 operator/(cfl32 &value) const { return toFloat() / value; }
   inline cf4p4 operator%(cfl32 &value) const { return (cf4p4 &)(data % toFixed(value)); }
   inline cf4p4 operator&(cfl32 &value) const { return (cf4p4 &)(data & toFixed(value)); }
   inline cf4p4 operator|(cfl32 &value) const { return (cf4p4 &)(data | toFixed(value)); }
   inline cf4p4 operator^(cfl32 &value) const { return (cf4p4 &)(data ^ toFixed(value)); }
   inline cfl32 operator+=(cfl32 &value) { cfl32 temp = toFloat() + value; *this = toFixed(temp); return temp; }
   inline cfl32 operator-=(cfl32 &value) { cfl32 temp = toFloat() - value; *this = toFixed(temp); return temp; }
   inline cfl32 operator*=(cfl32 &value) { cfl32 temp = toFloat() * value; *this = toFixed(temp); return temp; }
   inline cfl32 operator/=(cfl32 &value) { cfl32 temp = toFloat() / value; *this = toFixed(temp); return temp; }
   inline cf4p4 operator%=(cfl32 &value) { return (cf4p4 &)(data %= toFixed(value)); }
   inline cf4p4 operator&=(cfl32 &value) { return (cf4p4 &)(data &= toFixed(value)); }
   inline cf4p4 operator|=(cfl32 &value) { return (cf4p4 &)(data |= toFixed(value)); }
   inline cf4p4 operator^=(cfl32 &value) { return (cf4p4 &)(data ^= toFixed(value)); }

   inline cbool operator==(cfl32 &value) const { return data == toFixed(value); }
   inline cbool operator!=(cfl32 &value) const { return data != toFixed(value); }
   inline cbool operator>=(cfl32 &value) const { return data <= toFixed(value); }
   inline cbool operator<=(cfl32 &value) const { return data >= toFixed(value); }
   inline cbool operator>(cfl32 &value) const { return data < toFixed(value); }
   inline cbool operator<(cfl32 &value) const { return data > toFixed(value); }
   inline cbool operator||(cfl32 &value) const { return data || toFixed(value); }
   inline cbool operator&&(cfl32 &value) const { return data && toFixed(value); }
};

// Normalised 8-bit : Decimal range of 0.0~1.0
struct fp8n0_1 {
   typedef const fp8n0_1 cfp8n0_1;

   ui8 data;

   inline cui8 toFixed(cfl32 &value) const { return ui8(value * 255.0f); }
   inline cfl32 toFloat(void) const { return cfl32(data) * _fpdt_rcp255f; }
   inline cfl32 toFloat(cfp8n0_1 &value) const { return fl32(value.data) * _fpdt_rcp255f; }

   fp8n0_1(void) = default;
   fp8n0_1(const fp8n value) { data = toFixed(fl32(value) * _fpdt_data_.rangeDivMax8 + _fpdt_data_.origin8); }
   fp8n0_1(const fp16n value) { data = toFixed(fl32(value) * _fpdt_data_.rangeDivMax16 + _fpdt_data_.origin16); }
#ifdef _24BIT_INTEGERS_
   fp8n0_1(const fp24n value) { data = toFixed(fl32(value) * _fpdt_data_.rangeDivMax24 + _fpdt_data_.origin24); }
#endif
   fp8n0_1(const fp32n value) { data = toFixed(fl32(fl64(value) * _fpdt_data_.rangeDivMax32 + _fpdt_data_.origin32)); }
   fp8n0_1(cui8 value) { data = value; }
   fp8n0_1(csi32 value) { data = (ui8 &)value; }
   fp8n0_1(csi64 value) { data = (ui8 &)value; }
   fp8n0_1(cfl32 value) { data = toFixed(value); }

   operator ptr(void) const { return *this; }
   operator cfl32(void) const { return cfl32(data) * _fpdt_rcp255f; }

   inline cfp8n0_1 &operator&(void) const { return *this; }
   inline cfp8n0_1 operator~(void) const { return (cfp8n0_1 &)~data; }
   inline cfp8n0_1 operator-(void) const { return (cfp8n0_1 &)~data; }
   inline cbool operator!(void) const { return data == 0; }

   inline cfp8n0_1 operator++(void) { data++; return *this; }
   inline cfp8n0_1 operator++(int) { cui8 temp = data++; return (cfp8n0_1 &)temp; }
   inline cfp8n0_1 operator--(void) { data--; return *this; }
   inline cfp8n0_1 operator--(int) { cui8 temp = data--; return (cfp8n0_1 &)temp; }

   inline cbool operator==(csi32 &value) const { return data == (ui8 &)data; }
   inline cbool operator!=(csi32 &value) const { return data != (ui8 &)data; }
   inline cbool operator>=(csi32 &value) const { return data >= (ui8 &)data; }
   inline cbool operator<=(csi32 &value) const { return data <= (ui8 &)data; }
   inline cbool operator>(csi32 &value) const { return data > (ui8 &)data; }
   inline cbool operator<(csi32 &value) const { return data < (ui8 &)data; }
   inline cbool operator||(csi32 &value) const { return data || (ui8 &)data; }
   inline cbool operator&&(csi32 &value) const { return data && (ui8 &)data; }

   inline cfp8n0_1 operator<<(csi32 &value) const { return (cfp8n0_1 &)(data << value); }
   inline cfp8n0_1 operator>>(csi32 &value) const { return (cfp8n0_1 &)(data >> value); }
   inline cfp8n0_1 operator<<=(csi32 &value) { return (cfp8n0_1 &)(data <<= value); }
   inline cfp8n0_1 operator>>=(csi32 &value) { return (cfp8n0_1 &)(data >>= value); }

   inline cbool operator==(cfp8n0_1 &value) const { return data == value.data; }
   inline cbool operator!=(cfp8n0_1 &value) const { return data != value.data; }
   inline cbool operator>=(cfp8n0_1 &value) const { return data >= value.data; }
   inline cbool operator<=(cfp8n0_1 &value) const { return data <= value.data; }
   inline cbool operator>(cfp8n0_1 &value) const { return data > value.data; }
   inline cbool operator<(cfp8n0_1 &value) const { return data < value.data; }
   inline cbool operator||(cfp8n0_1 &value) const { return data || value.data; }
   inline cbool operator&&(cfp8n0_1 &value) const { return data && value.data; }

   inline cfp8n0_1 operator+(cfp8n0_1 &value) const { return (cfp8n0_1 &)(data + value.data); }
   inline cfp8n0_1 operator-(cfp8n0_1 &value) const { return (cfp8n0_1 &)(data - value.data); }
   inline cfp8n0_1 operator*(cfp8n0_1 &value) const { return toFixed(toFloat() * toFloat(value)); }
   inline cfp8n0_1 operator/(cfp8n0_1 &value) const { return toFixed(toFloat() / toFloat(value)); }
   inline cfp8n0_1 operator%(cfp8n0_1 &value) const { return (cfp8n0_1 &)(data % value.data); }
   inline cfp8n0_1 operator&(cfp8n0_1 &value) const { return (cfp8n0_1 &)(data & value.data); }
   inline cfp8n0_1 operator|(cfp8n0_1 &value) const { return (cfp8n0_1 &)(data | value.data); }
   inline cfp8n0_1 operator^(cfp8n0_1 &value) const { return (cfp8n0_1 &)(data ^ value.data); }
   inline cfp8n0_1 operator+=(cfp8n0_1 &value) { return (cfp8n0_1 &)(data += value.data); }
   inline cfp8n0_1 operator-=(cfp8n0_1 &value) { return (cfp8n0_1 &)(data -= value.data); }
   inline cfp8n0_1 operator*=(cfp8n0_1 &value) { return data = toFixed(toFloat() * toFloat(value)); }
   inline cfp8n0_1 operator/=(cfp8n0_1 &value) { return data = toFixed(toFloat() / toFloat(value)); }
   inline cfp8n0_1 operator%=(cfp8n0_1 &value) { return (cfp8n0_1 &)(data %= value.data); }
   inline cfp8n0_1 operator&=(cfp8n0_1 &value) { return (cfp8n0_1 &)(data &= value.data); }
   inline cfp8n0_1 operator|=(cfp8n0_1 &value) { return (cfp8n0_1 &)(data |= value.data); }
   inline cfp8n0_1 operator^=(cfp8n0_1 &value) { return (cfp8n0_1 &)(data ^= value.data); }

   inline cfl32 operator+(cfl32 &value) const { return toFloat() + value; }
   inline cfl32 operator-(cfl32 &value) const { return toFloat() - value; }
   inline cfl32 operator*(cfl32 &value) const { return toFloat() * value; }
   inline cfl32 operator/(cfl32 &value) const { return toFloat() / value; }
   inline cfp8n0_1 operator%(cfl32 &value) const { return (cfp8n0_1 &)(data % toFixed(value)); }
   inline cfp8n0_1 operator&(cfl32 &value) const { return (cfp8n0_1 &)(data & toFixed(value)); }
   inline cfp8n0_1 operator|(cfl32 &value) const { return (cfp8n0_1 &)(data | toFixed(value)); }
   inline cfp8n0_1 operator^(cfl32 &value) const { return (cfp8n0_1 &)(data ^ toFixed(value)); }
   inline cfl32 operator+=(cfl32 &value) { cfl32 temp = toFloat() + value; *this = toFixed(temp); return temp; }
   inline cfl32 operator-=(cfl32 &value) { cfl32 temp = toFloat() - value; *this = toFixed(temp); return temp; }
   inline cfl32 operator*=(cfl32 &value) { cfl32 temp = toFloat() * value; *this = toFixed(temp); return temp; }
   inline cfl32 operator/=(cfl32 &value) { cfl32 temp = toFloat() / value; *this = toFixed(temp); return temp; }
   inline cfp8n0_1 operator%=(cfl32 &value) { return (cfp8n0_1 &)(data %= toFixed(value)); }
   inline cfp8n0_1 operator&=(cfl32 &value) { return (cfp8n0_1 &)(data &= toFixed(value)); }
   inline cfp8n0_1 operator|=(cfl32 &value) { return (cfp8n0_1 &)(data |= toFixed(value)); }
   inline cfp8n0_1 operator^=(cfl32 &value) { return (cfp8n0_1 &)(data ^= toFixed(value)); }

   inline cbool operator==(cfl32 &value) const { return data == toFixed(value); }
   inline cbool operator!=(cfl32 &value) const { return data != toFixed(value); }
   inline cbool operator>=(cfl32 &value) const { return data <= toFixed(value); }
   inline cbool operator<=(cfl32 &value) const { return data >= toFixed(value); }
   inline cbool operator>(cfl32 &value) const { return data < toFixed(value); }
   inline cbool operator<(cfl32 &value) const { return data > toFixed(value); }
   inline cbool operator||(cfl32 &value) const { return data || toFixed(value); }
   inline cbool operator&&(cfl32 &value) const { return data && toFixed(value); }
};

// 4x normalised 8-bit : Decimal ranges of 0.0~1.0
struct fp8n0_1x4 {
   typedef const fp8n0_1 cfp8n0_1; typedef const fp8n0_1x4 cfp8n0_1x4;

   union { ui32 data32; fp8n0_1 data[4]; ui8 data8[4]; };

   fp8n0_1x4(void) = default;
   fp8n0_1x4(fp8n0_1 value, cui8 index) { data[index] = value; }
   fp8n0_1x4(cui8 value, cui8 index) { data8[index] = value; }
   fp8n0_1x4(cfl32 value, cui8 index) { data8[index] = ui8(value * 255.0f); }
   fp8n0_1x4(cfp8n0_1 (&value)[4]) { data32 = (ui32 &)value[0]; }
   fp8n0_1x4(cui8 (&value)[4]) { data32 = (ui32 &)value[0]; }
   fp8n0_1x4(cfp8n0_1 value) { data[0] = data[1] = data[2] = data[3] = value; }
   fp8n0_1x4(cui8 value) { data8[0] = data8[1] = data8[2] = data8[3] = value; }
   fp8n0_1x4(cui32 value) { data32 = value; }
   fp8n0_1x4(cfl32 value) { cui8 temp = ui8(value * 255.0f); data8[0] = data8[1] = data8[2] = data8[3] = temp; }
   fp8n0_1x4(cui8 value0, cui8 value1, cui8 value2, cui8 value3) { data8[0] = value0; data8[1] = value1; data8[2] = value2; data8[3] = value3; }
   fp8n0_1x4(cfl32x4 value) { data32 = (ui32 &)_mm_shuffle_epi8(_mm_cvttps_epi32(_mm_mul_ps(value, _fpdt_255fx4)), _fpdt_shuffle8s); }
#ifdef _VECTOR_STRUCTURES_
   fp8n0_1x4(cVEC4Df value) { cSSE4Df32 temp = { .vector = value }; data32 = (ui32 &)_mm_shuffle_epi8(_mm_cvttps_epi32(_mm_mul_ps(temp.xmm, _fpdt_255fx4)), _fpdt_shuffle8s); }
   fp8n0_1x4(cSSE4Df32 value) { data32 = (ui32 &)_mm_shuffle_epi8(_mm_cvttps_epi32(_mm_mul_ps(value.xmm, _fpdt_255fx4)), _fpdt_shuffle8s); }
#endif

   operator ptr(void) const { return *this; }
   operator cfl32x4(void) const { return _mm_mul_ps(_mm_cvtepu32_ps(_mm_cvtepu8_epi32(_mm_cvtsi32_si128(data32))), _fpdt_rcp255fx4); }

   inline cfp8n0_1x4 operator<<(csi32 &value) const { return fp8n0_1x4{ ui8(data8[0] << value), ui8(data8[1] << value), ui8(data8[2] << value), ui8(data8[3] << value) }; }
   inline cfp8n0_1x4 operator>>(csi32 &value) const { return fp8n0_1x4{ ui8(data8[0] >> value), ui8(data8[1] >> value), ui8(data8[2] >> value), ui8(data8[3] >> value) }; }
   inline cfp8n0_1x4 operator<<=(csi32 &value) { return fp8n0_1x4{ data8[0] <<= value, data8[1] <<= value, data8[2] <<= value, data8[3] <<= value }; }
   inline cfp8n0_1x4 operator>>=(csi32 &value) { return fp8n0_1x4{ data8[0] >>= value, data8[1] >>= value, data8[2] >>= value, data8[3] >>= value }; }

   inline cfp8n0_1x4 operator++(void) { data8[0]++; data8[1]++; data8[2]++; data8[3]++; return *this; }
   inline cfp8n0_1x4 operator++(int) { al4 cui8 temp[4] = { data8[0]++, data8[1]++, data8[2]++, data8[3]++ }; return (cfp8n0_1x4 &)temp; }
   inline cfp8n0_1x4 operator--(void) { data8[0]--; data8[1]--; data8[2]--; data8[3]--; return *this; }
   inline cfp8n0_1x4 operator--(int) { al4 cui8 temp[4] = { data8[0]--, data8[1]--, data8[2]--, data8[3]-- }; return (cfp8n0_1x4 &)temp; }

   inline cfp8n0_1x4 operator+(cfp8n0_1x4 &value) const { return data32 + value.data32; }
   inline cfp8n0_1x4 operator-(cfp8n0_1x4 &value) const { return data32 - value.data32; }
   inline cfp8n0_1x4 operator+(cfl32x4 &value) const { return data32 + (ui32 &)_mm_shuffle_epi8(_mm_cvttps_epi32(_mm_mul_ps(value, _fpdt_255fx4)), _fpdt_shuffle8s); }
   inline cfp8n0_1x4 operator-(cfl32x4 &value) const { return data32 - (ui32 &)_mm_shuffle_epi8(_mm_cvttps_epi32(_mm_mul_ps(value, _fpdt_255fx4)), _fpdt_shuffle8s); }

   inline cfp8n0_1x4 operator+=(cfp8n0_1x4 &value) { return (data32 += value.data32); }
   inline cfp8n0_1x4 operator-=(cfp8n0_1x4 &value) { return (data32 -= value.data32); }
   inline cfp8n0_1x4 operator+=(cfl32x4 &value) { return (data32 += (ui32 &)_mm_shuffle_epi8(_mm_cvttps_epi32(_mm_mul_ps(value, _fpdt_255fx4)), _fpdt_shuffle8s)); }
   inline cfp8n0_1x4 operator-=(cfl32x4 &value) { return (data32 -= (ui32 &)_mm_shuffle_epi8(_mm_cvttps_epi32(_mm_mul_ps(value, _fpdt_255fx4)), _fpdt_shuffle8s)); }
};

// 16-bit, 0.16 : Decimal range of 0.0~0.9999847412109375
struct f0p16 {
   typedef const f0p16 cf0p16;

   ui16 data;

   inline cui16 toFixed(cfl32 &value) const { return ui16(value * 65536.0f); }
   inline cfl32 toFloat(void) const { return cfl32(data) * _fpdt_rcp65536f; }
   inline cfl32 toFloat(cf0p16 &value) const { return fl32(value.data) * _fpdt_rcp65536f; }

   f0p16(void) = default;
   f0p16(const fp8n value) { data = toFixed(fl32(value) * _fpdt_data_.rangeDivMax8 + _fpdt_data_.origin8); }
   f0p16(const fp16n value) { data = toFixed(fl32(value) * _fpdt_data_.rangeDivMax16 + _fpdt_data_.origin16); }
#ifdef _24BIT_INTEGERS_
   f0p16(const fp24n value) { data = toFixed(fl32(value) * _fpdt_data_.rangeDivMax24 + _fpdt_data_.origin24); }
#endif
   f0p16(const fp32n value) { data = toFixed(fl32(fl64(value) * _fpdt_data_.rangeDivMax32 + _fpdt_data_.origin32)); }
   f0p16(cui16 value) { data = value; }
   f0p16(csi32 value) { data = (ui16 &)value; }
   f0p16(csi64 value) { data = (ui16 &)value; }
   f0p16(cfl32 value) { data = toFixed(value); }

   operator ptr(void) const { return *this; }
   operator cfl32(void) const { return cfl32(data) * _fpdt_rcp65536f; }

   inline cf0p16 &operator&(void) const { return *this; }
   inline cf0p16 operator~(void) const { return (cf0p16 &)~data; }
   inline cf0p16 operator-(void) const { return (cf0p16 &)~data; }
   inline cbool operator!(void) const { return data == 0; }

   inline cf0p16 operator++(void) { data++; return *this; }
   inline cf0p16 operator++(int) { cui16 temp = data++; return (cf0p16 &)temp; }
   inline cf0p16 operator--(void) { data--; return *this; }
   inline cf0p16 operator--(int) { cui16 temp = data--; return (cf0p16 &)temp; }

   inline cbool operator==(csi32 &value) const { return data == (ui16 &)data; }
   inline cbool operator!=(csi32 &value) const { return data != (ui16 &)data; }
   inline cbool operator>=(csi32 &value) const { return data >= (ui16 &)data; }
   inline cbool operator<=(csi32 &value) const { return data <= (ui16 &)data; }
   inline cbool operator>(csi32 &value) const { return data > (ui16 &)data; }
   inline cbool operator<(csi32 &value) const { return data < (ui16 &)data; }
   inline cbool operator||(csi32 &value) const { return data || (ui16 &)data; }
   inline cbool operator&&(csi32 &value) const { return data && (ui16 &)data; }

   inline cf0p16 operator<<(csi32 &value) const { return (cf0p16 &)(data << value); }
   inline cf0p16 operator>>(csi32 &value) const { return (cf0p16 &)(data >> value); }
   inline cf0p16 operator<<=(csi32 &value) { return (cf0p16 &)(data <<= value); }
   inline cf0p16 operator>>=(csi32 &value) { return (cf0p16 &)(data >>= value); }

   inline cbool operator==(cf0p16 &value) const { return data == value.data; }
   inline cbool operator!=(cf0p16 &value) const { return data != value.data; }
   inline cbool operator>=(cf0p16 &value) const { return data >= value.data; }
   inline cbool operator<=(cf0p16 &value) const { return data <= value.data; }
   inline cbool operator>(cf0p16 &value) const { return data > value.data; }
   inline cbool operator<(cf0p16 &value) const { return data < value.data; }
   inline cbool operator||(cf0p16 &value) const { return data || value.data; }
   inline cbool operator&&(cf0p16 &value) const { return data && value.data; }

   inline cf0p16 operator+(cf0p16 &value) const { return (cf0p16 &)(data + value.data); }
   inline cf0p16 operator-(cf0p16 &value) const { return (cf0p16 &)(data - value.data); }
   inline cf0p16 operator*(cf0p16 &value) const { return toFixed(toFloat() * toFloat(value)); }
   inline cf0p16 operator/(cf0p16 &value) const { return toFixed(toFloat() / toFloat(value)); }
   inline cf0p16 operator%(cf0p16 &value) const { return (cf0p16 &)(data % value.data); }
   inline cf0p16 operator&(cf0p16 &value) const { return (cf0p16 &)(data & value.data); }
   inline cf0p16 operator|(cf0p16 &value) const { return (cf0p16 &)(data | value.data); }
   inline cf0p16 operator^(cf0p16 &value) const { return (cf0p16 &)(data ^ value.data); }
   inline cf0p16 operator+=(cf0p16 &value) { return (cf0p16 &)(data += value.data); }
   inline cf0p16 operator-=(cf0p16 &value) { return (cf0p16 &)(data -= value.data); }
   inline cf0p16 operator*=(cf0p16 &value) { return data = toFixed(toFloat() * toFloat(value)); }
   inline cf0p16 operator/=(cf0p16 &value) { return data = toFixed(toFloat() / toFloat(value)); }
   inline cf0p16 operator%=(cf0p16 &value) { return (cf0p16 &)(data %= value.data); }
   inline cf0p16 operator&=(cf0p16 &value) { return (cf0p16 &)(data &= value.data); }
   inline cf0p16 operator|=(cf0p16 &value) { return (cf0p16 &)(data |= value.data); }
   inline cf0p16 operator^=(cf0p16 &value) { return (cf0p16 &)(data ^= value.data); }

   inline cfl32 operator+(cfl32 &value) const { return toFloat() + value; }
   inline cfl32 operator-(cfl32 &value) const { return toFloat() - value; }
   inline cfl32 operator*(cfl32 &value) const { return toFloat() * value; }
   inline cfl32 operator/(cfl32 &value) const { return toFloat() / value; }
   inline cf0p16 operator%(cfl32 &value) const { return (cf0p16 &)(data % toFixed(value)); }
   inline cf0p16 operator&(cfl32 &value) const { return (cf0p16 &)(data & toFixed(value)); }
   inline cf0p16 operator|(cfl32 &value) const { return (cf0p16 &)(data | toFixed(value)); }
   inline cf0p16 operator^(cfl32 &value) const { return (cf0p16 &)(data ^ toFixed(value)); }
   inline cfl32 operator+=(cfl32 &value) { cfl32 temp = toFloat() + value; *this = toFixed(temp); return temp; }
   inline cfl32 operator-=(cfl32 &value) { cfl32 temp = toFloat() - value; *this = toFixed(temp); return temp; }
   inline cfl32 operator*=(cfl32 &value) { cfl32 temp = toFloat() * value; *this = toFixed(temp); return temp; }
   inline cfl32 operator/=(cfl32 &value) { cfl32 temp = toFloat() / value; *this = toFixed(temp); return temp; }
   inline cf0p16 operator%=(cfl32 &value) { return (cf0p16 &)(data %= toFixed(value)); }
   inline cf0p16 operator&=(cfl32 &value) { return (cf0p16 &)(data &= toFixed(value)); }
   inline cf0p16 operator|=(cfl32 &value) { return (cf0p16 &)(data |= toFixed(value)); }
   inline cf0p16 operator^=(cfl32 &value) { return (cf0p16 &)(data ^= toFixed(value)); }

   inline cbool operator==(cfl32 &value) const { return data == toFixed(value); }
   inline cbool operator!=(cfl32 &value) const { return data != toFixed(value); }
   inline cbool operator>=(cfl32 &value) const { return data <= toFixed(value); }
   inline cbool operator<=(cfl32 &value) const { return data >= toFixed(value); }
   inline cbool operator>(cfl32 &value) const { return data < toFixed(value); }
   inline cbool operator<(cfl32 &value) const { return data > toFixed(value); }
   inline cbool operator||(cfl32 &value) const { return data || toFixed(value); }
   inline cbool operator&&(cfl32 &value) const { return data && toFixed(value); }
};

// 16-bit, signed 1.14 : Decimal range of -2.0~1.99993896484375
struct fs1p14 {
   typedef const fs1p14 cfs1p14;

   ui16 data;

   inline cui16 toFixed(cfl32 &value) const { return ui16((value + 2.0f) * 16384.0f); }
   inline cfl32 toFloat(void) const { return cfl32(data) * _fpdt_rcp16384f - 2.0f; }
   inline cfl32 toFloat(cfs1p14 &value) const { return fl32(value.data) * _fpdt_rcp16384f - 2.0f; }

   fs1p14(void) = default;
   fs1p14(const fp8n value) { data = toFixed(fl32(value) * _fpdt_data_.rangeDivMax8 + _fpdt_data_.origin8); }
   fs1p14(const fp16n value) { data = toFixed(fl32(value) * _fpdt_data_.rangeDivMax16 + _fpdt_data_.origin16); }
#ifdef _24BIT_INTEGERS_
   fs1p14(const fp24n value) { data = toFixed(fl32(value) * _fpdt_data_.rangeDivMax24 + _fpdt_data_.origin24); }
#endif
   fs1p14(const fp32n value) { data = toFixed(fl32(fl64(value) * _fpdt_data_.rangeDivMax32 + _fpdt_data_.origin32)); }
   fs1p14(cui16 value) { data = value; }
   fs1p14(csi32 value) { data = (ui16 &)value; }
   fs1p14(csi64 value) { data = (ui16 &)value; }
   fs1p14(cfl32 value) { data = toFixed(value); }

   operator ptr(void) const { return *this; }
   operator cfl32(void) const { return cfl32(data) * _fpdt_rcp16384f - 2.0f; }

   inline cfs1p14 &operator&(void) const { return *this; }
   inline cfs1p14 operator~(void) const { return (cfs1p14 &)~data; }
   inline cfs1p14 operator-(void) const { return (cfs1p14 &)~data; }
   inline cbool operator!(void) const { return data == 0; }

   inline cfs1p14 operator++(void) { data++; return *this; }
   inline cfs1p14 operator++(int) { cui16 temp = data++; return (cfs1p14 &)temp; }
   inline cfs1p14 operator--(void) { data--; return *this; }
   inline cfs1p14 operator--(int) { cui16 temp = data--; return (cfs1p14 &)temp; }

   inline cbool operator==(csi32 &value) const { return data == (ui16 &)data; }
   inline cbool operator!=(csi32 &value) const { return data != (ui16 &)data; }
   inline cbool operator>=(csi32 &value) const { return data >= (ui16 &)data; }
   inline cbool operator<=(csi32 &value) const { return data <= (ui16 &)data; }
   inline cbool operator>(csi32 &value) const { return data > (ui16 &)data; }
   inline cbool operator<(csi32 &value) const { return data < (ui16 &)data; }
   inline cbool operator||(csi32 &value) const { return data || (ui16 &)data; }
   inline cbool operator&&(csi32 &value) const { return data && (ui16 &)data; }

   inline cfs1p14 operator<<(csi32 &value) const { return (cfs1p14 &)(data << value); }
   inline cfs1p14 operator>>(csi32 &value) const { return (cfs1p14 &)(data >> value); }
   inline cfs1p14 operator<<=(csi32 &value) { return (cfs1p14 &)(data <<= value); }
   inline cfs1p14 operator>>=(csi32 &value) { return (cfs1p14 &)(data >>= value); }

   inline cbool operator==(cfs1p14 &value) const { return data == value.data; }
   inline cbool operator!=(cfs1p14 &value) const { return data != value.data; }
   inline cbool operator>=(cfs1p14 &value) const { return data >= value.data; }
   inline cbool operator<=(cfs1p14 &value) const { return data <= value.data; }
   inline cbool operator>(cfs1p14 &value) const { return data > value.data; }
   inline cbool operator<(cfs1p14 &value) const { return data < value.data; }
   inline cbool operator||(cfs1p14 &value) const { return data || value.data; }
   inline cbool operator&&(cfs1p14 &value) const { return data && value.data; }

   inline cfs1p14 operator+(cfs1p14 &value) const { return (cfs1p14 &)(data + value.data - 32768); }
   inline cfs1p14 operator-(cfs1p14 &value) const { return (cfs1p14 &)(data - value.data - 32768); }
   inline cfs1p14 operator*(cfs1p14 &value) const { return toFixed(toFloat() * toFloat(value)); }
   inline cfs1p14 operator/(cfs1p14 &value) const { return toFixed(toFloat() / toFloat(value)); }
   inline cfs1p14 operator%(cfs1p14 &value) const { return (cfs1p14 &)(data % value.data); }
   inline cfs1p14 operator&(cfs1p14 &value) const { return (cfs1p14 &)(data & value.data); }
   inline cfs1p14 operator|(cfs1p14 &value) const { return (cfs1p14 &)(data | value.data); }
   inline cfs1p14 operator^(cfs1p14 &value) const { return (cfs1p14 &)(data ^ value.data); }
   inline cfs1p14 operator+=(cfs1p14 &value) { return (cfs1p14 &)(data += value.data - 32768); }
   inline cfs1p14 operator-=(cfs1p14 &value) { return (cfs1p14 &)(data -= value.data - 32768); }
   inline cfs1p14 operator*=(cfs1p14 &value) { return data = toFixed(toFloat() * toFloat(value)); }
   inline cfs1p14 operator/=(cfs1p14 &value) { return data = toFixed(toFloat() / toFloat(value)); }
   inline cfs1p14 operator%=(cfs1p14 &value) { return (cfs1p14 &)(data %= value.data); }
   inline cfs1p14 operator&=(cfs1p14 &value) { return (cfs1p14 &)(data &= value.data); }
   inline cfs1p14 operator|=(cfs1p14 &value) { return (cfs1p14 &)(data |= value.data); }
   inline cfs1p14 operator^=(cfs1p14 &value) { return (cfs1p14 &)(data ^= value.data); }

   inline cfl32 operator+(cfl32 &value) const { return toFloat() + value; }
   inline cfl32 operator-(cfl32 &value) const { return toFloat() - value; }
   inline cfl32 operator*(cfl32 &value) const { return toFloat() * value; }
   inline cfl32 operator/(cfl32 &value) const { return toFloat() / value; }
   inline cfs1p14 operator%(cfl32 &value) const { return (cfs1p14 &)(data % toFixed(value)); }
   inline cfs1p14 operator&(cfl32 &value) const { return (cfs1p14 &)(data & toFixed(value)); }
   inline cfs1p14 operator|(cfl32 &value) const { return (cfs1p14 &)(data | toFixed(value)); }
   inline cfs1p14 operator^(cfl32 &value) const { return (cfs1p14 &)(data ^ toFixed(value)); }
   inline cfl32 operator+=(cfl32 &value) { cfl32 temp = toFloat() + value; *this = toFixed(temp); return temp; }
   inline cfl32 operator-=(cfl32 &value) { cfl32 temp = toFloat() - value; *this = toFixed(temp); return temp; }
   inline cfl32 operator*=(cfl32 &value) { cfl32 temp = toFloat() * value; *this = toFixed(temp); return temp; }
   inline cfl32 operator/=(cfl32 &value) { cfl32 temp = toFloat() / value; *this = toFixed(temp); return temp; }
   inline cfs1p14 operator%=(cfl32 &value) { return (cfs1p14 &)(data %= toFixed(value)); }
   inline cfs1p14 operator&=(cfl32 &value) { return (cfs1p14 &)(data &= toFixed(value)); }
   inline cfs1p14 operator|=(cfl32 &value) { return (cfs1p14 &)(data |= toFixed(value)); }
   inline cfs1p14 operator^=(cfl32 &value) { return (cfs1p14 &)(data ^= toFixed(value)); }

   inline cbool operator==(cfl32 &value) const { return data == toFixed(value); }
   inline cbool operator!=(cfl32 &value) const { return data != toFixed(value); }
   inline cbool operator>=(cfl32 &value) const { return data <= toFixed(value); }
   inline cbool operator<=(cfl32 &value) const { return data >= toFixed(value); }
   inline cbool operator>(cfl32 &value) const { return data < toFixed(value); }
   inline cbool operator<(cfl32 &value) const { return data > toFixed(value); }
   inline cbool operator||(cfl32 &value) const { return data || toFixed(value); }
   inline cbool operator&&(cfl32 &value) const { return data && toFixed(value); }
};

// 16-bit, 1.15 : Decimal range of 0.0~1.999969482421875
struct f1p15 {
   typedef const f1p15 cf1p15;

   ui16 data;

   inline cui16 toFixed(cfl32 &value) const { return ui16(value * 32768.0f); }
   inline cfl32 toFloat(void) const { return cfl32(data) * _fpdt_rcp32768f; }
   inline cfl32 toFloat(cf1p15 &value) const { return fl32(value.data) * _fpdt_rcp32768f; }

   f1p15(void) = default;
   f1p15(const fp8n value) { data = toFixed(fl32(value) * _fpdt_data_.rangeDivMax8 + _fpdt_data_.origin8); }
   f1p15(const fp16n value) { data = toFixed(fl32(value) * _fpdt_data_.rangeDivMax16 + _fpdt_data_.origin16); }
#ifdef _24BIT_INTEGERS_
   f1p15(const fp24n value) { data = toFixed(fl32(value) * _fpdt_data_.rangeDivMax24 + _fpdt_data_.origin24); }
#endif
   f1p15(const fp32n value) { data = toFixed(fl32(fl64(value) * _fpdt_data_.rangeDivMax32 + _fpdt_data_.origin32)); }
   f1p15(cui16 value) { data = value; }
   f1p15(csi32 value) { data = (ui16 &)value; }
   f1p15(csi64 value) { data = (ui16 &)value; }
   f1p15(cfl32 value) { data = toFixed(value); }

   operator ptr(void) const { return *this; }
   operator cfl32(void) const { return cfl32(data) * _fpdt_rcp32768f; }

   inline cf1p15 &operator&(void) const { return *this; }
   inline cf1p15 operator~(void) const { return (cf1p15 &)~data; }
   inline cf1p15 operator-(void) const { return (cf1p15 &)~data; }
   inline cbool operator!(void) const { return data == 0; }

   inline cf1p15 operator++(void) { data++; return *this; }
   inline cf1p15 operator++(int) { cui16 temp = data++; return (cf1p15 &)temp; }
   inline cf1p15 operator--(void) { data--; return *this; }
   inline cf1p15 operator--(int) { cui16 temp = data--; return (cf1p15 &)temp; }

   inline cbool operator==(csi32 &value) const { return data == (ui16 &)data; }
   inline cbool operator!=(csi32 &value) const { return data != (ui16 &)data; }
   inline cbool operator>=(csi32 &value) const { return data >= (ui16 &)data; }
   inline cbool operator<=(csi32 &value) const { return data <= (ui16 &)data; }
   inline cbool operator>(csi32 &value) const { return data > (ui16 &)data; }
   inline cbool operator<(csi32 &value) const { return data < (ui16 &)data; }
   inline cbool operator||(csi32 &value) const { return data || (ui16 &)data; }
   inline cbool operator&&(csi32 &value) const { return data && (ui16 &)data; }

   inline cf1p15 operator<<(csi32 &value) const { return (cf1p15 &)(data << value); }
   inline cf1p15 operator>>(csi32 &value) const { return (cf1p15 &)(data >> value); }
   inline cf1p15 operator<<=(csi32 &value) { return (cf1p15 &)(data <<= value); }
   inline cf1p15 operator>>=(csi32 &value) { return (cf1p15 &)(data >>= value); }

   inline cbool operator==(cf1p15 &value) const { return data == value.data; }
   inline cbool operator!=(cf1p15 &value) const { return data != value.data; }
   inline cbool operator>=(cf1p15 &value) const { return data >= value.data; }
   inline cbool operator<=(cf1p15 &value) const { return data <= value.data; }
   inline cbool operator>(cf1p15 &value) const { return data > value.data; }
   inline cbool operator<(cf1p15 &value) const { return data < value.data; }
   inline cbool operator||(cf1p15 &value) const { return data || value.data; }
   inline cbool operator&&(cf1p15 &value) const { return data && value.data; }

   inline cf1p15 operator+(cf1p15 &value) const { return (cf1p15 &)(data + value.data); }
   inline cf1p15 operator-(cf1p15 &value) const { return (cf1p15 &)(data - value.data); }
   inline cf1p15 operator*(cf1p15 &value) const { return toFixed(toFloat() * toFloat(value)); }
   inline cf1p15 operator/(cf1p15 &value) const { return toFixed(toFloat() / toFloat(value)); }
   inline cf1p15 operator%(cf1p15 &value) const { return (cf1p15 &)(data % value.data); }
   inline cf1p15 operator&(cf1p15 &value) const { return (cf1p15 &)(data & value.data); }
   inline cf1p15 operator|(cf1p15 &value) const { return (cf1p15 &)(data | value.data); }
   inline cf1p15 operator^(cf1p15 &value) const { return (cf1p15 &)(data ^ value.data); }
   inline cf1p15 operator+=(cf1p15 &value) { return (cf1p15 &)(data += value.data); }
   inline cf1p15 operator-=(cf1p15 &value) { return (cf1p15 &)(data -= value.data); }
   inline cf1p15 operator*=(cf1p15 &value) { return data = toFixed(toFloat() * toFloat(value)); }
   inline cf1p15 operator/=(cf1p15 &value) { return data = toFixed(toFloat() / toFloat(value)); }
   inline cf1p15 operator%=(cf1p15 &value) { return (cf1p15 &)(data %= value.data); }
   inline cf1p15 operator&=(cf1p15 &value) { return (cf1p15 &)(data &= value.data); }
   inline cf1p15 operator|=(cf1p15 &value) { return (cf1p15 &)(data |= value.data); }
   inline cf1p15 operator^=(cf1p15 &value) { return (cf1p15 &)(data ^= value.data); }

   inline cfl32 operator+(cfl32 &value) const { return toFloat() + value; }
   inline cfl32 operator-(cfl32 &value) const { return toFloat() - value; }
   inline cfl32 operator*(cfl32 &value) const { return toFloat() * value; }
   inline cfl32 operator/(cfl32 &value) const { return toFloat() / value; }
   inline cf1p15 operator%(cfl32 &value) const { return (cf1p15 &)(data % toFixed(value)); }
   inline cf1p15 operator&(cfl32 &value) const { return (cf1p15 &)(data & toFixed(value)); }
   inline cf1p15 operator|(cfl32 &value) const { return (cf1p15 &)(data | toFixed(value)); }
   inline cf1p15 operator^(cfl32 &value) const { return (cf1p15 &)(data ^ toFixed(value)); }
   inline cfl32 operator+=(cfl32 &value) { cfl32 temp = toFloat() + value; *this = toFixed(temp); return temp; }
   inline cfl32 operator-=(cfl32 &value) { cfl32 temp = toFloat() - value; *this = toFixed(temp); return temp; }
   inline cfl32 operator*=(cfl32 &value) { cfl32 temp = toFloat() * value; *this = toFixed(temp); return temp; }
   inline cfl32 operator/=(cfl32 &value) { cfl32 temp = toFloat() / value; *this = toFixed(temp); return temp; }
   inline cf1p15 operator%=(cfl32 &value) { return (cf1p15 &)(data %= toFixed(value)); }
   inline cf1p15 operator&=(cfl32 &value) { return (cf1p15 &)(data &= toFixed(value)); }
   inline cf1p15 operator|=(cfl32 &value) { return (cf1p15 &)(data |= toFixed(value)); }
   inline cf1p15 operator^=(cfl32 &value) { return (cf1p15 &)(data ^= toFixed(value)); }

   inline cbool operator==(cfl32 &value) const { return data == toFixed(value); }
   inline cbool operator!=(cfl32 &value) const { return data != toFixed(value); }
   inline cbool operator>=(cfl32 &value) const { return data <= toFixed(value); }
   inline cbool operator<=(cfl32 &value) const { return data >= toFixed(value); }
   inline cbool operator>(cfl32 &value) const { return data < toFixed(value); }
   inline cbool operator<(cfl32 &value) const { return data > toFixed(value); }
   inline cbool operator||(cfl32 &value) const { return data || toFixed(value); }
   inline cbool operator&&(cfl32 &value) const { return data && toFixed(value); }
};

// 16-bit, 6.10 : Decimal range of 0.0~63.9990234375
struct f6p10 {
   typedef const f6p10 cf6p10;

   ui16 data;

   inline cui16 toFixed(cfl32 &value) const { return ui16(value * 1024.0f); }
   inline cfl32 toFloat(void) const { return cfl32(data) * _fpdt_rcp1024f; }
   inline cfl32 toFloat(cf6p10 &value) const { return fl32(value.data) * _fpdt_rcp1024f; }

   f6p10(void) = default;
   f6p10(const fp8n value) { data = toFixed(fl32(value) * _fpdt_data_.rangeDivMax8 + _fpdt_data_.origin8); }
   f6p10(const fp16n value) { data = toFixed(fl32(value) * _fpdt_data_.rangeDivMax16 + _fpdt_data_.origin16); }
#ifdef _24BIT_INTEGERS_
   f6p10(const fp24n value) { data = toFixed(fl32(value) * _fpdt_data_.rangeDivMax24 + _fpdt_data_.origin24); }
#endif
   f6p10(const fp32n value) { data = toFixed(fl32(fl64(value) * _fpdt_data_.rangeDivMax32 + _fpdt_data_.origin32)); }
   f6p10(cui16 value) { data = value; }
   f6p10(csi32 value) { data = (ui16 &)value; }
   f6p10(csi64 value) { data = (ui16 &)value; }
   f6p10(cfl32 value) { data = toFixed(value); }

   operator ptr(void) const { return *this; }
   operator cfl32(void) const { return cfl32(data) * _fpdt_rcp1024f; }

   inline cf6p10 &operator&(void) const { return *this; }
   inline cf6p10 operator~(void) const { return (cf6p10 &)~data; }
   inline cf6p10 operator-(void) const { return (cf6p10 &)~data; }
   inline cbool operator!(void) const { return data == 0; }

   inline cf6p10 operator++(void) { data++; return *this; }
   inline cf6p10 operator++(int) { cui16 temp = data++; return (cf6p10 &)temp; }
   inline cf6p10 operator--(void) { data--; return *this; }
   inline cf6p10 operator--(int) { cui16 temp = data--; return (cf6p10 &)temp; }

   inline cbool operator==(csi32 &value) const { return data == (ui16 &)data; }
   inline cbool operator!=(csi32 &value) const { return data != (ui16 &)data; }
   inline cbool operator>=(csi32 &value) const { return data >= (ui16 &)data; }
   inline cbool operator<=(csi32 &value) const { return data <= (ui16 &)data; }
   inline cbool operator>(csi32 &value) const { return data > (ui16 &)data; }
   inline cbool operator<(csi32 &value) const { return data < (ui16 &)data; }
   inline cbool operator||(csi32 &value) const { return data || (ui16 &)data; }
   inline cbool operator&&(csi32 &value) const { return data && (ui16 &)data; }

   inline cf6p10 operator<<(csi32 &value) const { return (cf6p10 &)(data << value); }
   inline cf6p10 operator>>(csi32 &value) const { return (cf6p10 &)(data >> value); }
   inline cf6p10 operator<<=(csi32 &value) { return (cf6p10 &)(data <<= value); }
   inline cf6p10 operator>>=(csi32 &value) { return (cf6p10 &)(data >>= value); }

   inline cbool operator==(cf6p10 &value) const { return data == value.data; }
   inline cbool operator!=(cf6p10 &value) const { return data != value.data; }
   inline cbool operator>=(cf6p10 &value) const { return data >= value.data; }
   inline cbool operator<=(cf6p10 &value) const { return data <= value.data; }
   inline cbool operator>(cf6p10 &value) const { return data > value.data; }
   inline cbool operator<(cf6p10 &value) const { return data < value.data; }
   inline cbool operator||(cf6p10 &value) const { return data || value.data; }
   inline cbool operator&&(cf6p10 &value) const { return data && value.data; }

   inline cf6p10 operator+(cf6p10 &value) const { return (cf6p10 &)(data + value.data); }
   inline cf6p10 operator-(cf6p10 &value) const { return (cf6p10 &)(data - value.data); }
   inline cf6p10 operator*(cf6p10 &value) const { return toFixed(toFloat() * toFloat(value)); }
   inline cf6p10 operator/(cf6p10 &value) const { return toFixed(toFloat() / toFloat(value)); }
   inline cf6p10 operator%(cf6p10 &value) const { return (cf6p10 &)(data % value.data); }
   inline cf6p10 operator&(cf6p10 &value) const { return (cf6p10 &)(data & value.data); }
   inline cf6p10 operator|(cf6p10 &value) const { return (cf6p10 &)(data | value.data); }
   inline cf6p10 operator^(cf6p10 &value) const { return (cf6p10 &)(data ^ value.data); }
   inline cf6p10 operator+=(cf6p10 &value) { return (cf6p10 &)(data += value.data); }
   inline cf6p10 operator-=(cf6p10 &value) { return (cf6p10 &)(data -= value.data); }
   inline cf6p10 operator*=(cf6p10 &value) { return data = toFixed(toFloat() * toFloat(value)); }
   inline cf6p10 operator/=(cf6p10 &value) { return data = toFixed(toFloat() / toFloat(value)); }
   inline cf6p10 operator%=(cf6p10 &value) { return (cf6p10 &)(data %= value.data); }
   inline cf6p10 operator&=(cf6p10 &value) { return (cf6p10 &)(data &= value.data); }
   inline cf6p10 operator|=(cf6p10 &value) { return (cf6p10 &)(data |= value.data); }
   inline cf6p10 operator^=(cf6p10 &value) { return (cf6p10 &)(data ^= value.data); }

   inline cfl32 operator+(cfl32 &value) const { return toFloat() + value; }
   inline cfl32 operator-(cfl32 &value) const { return toFloat() - value; }
   inline cfl32 operator*(cfl32 &value) const { return toFloat() * value; }
   inline cfl32 operator/(cfl32 &value) const { return toFloat() / value; }
   inline cf6p10 operator%(cfl32 &value) const { return (cf6p10 &)(data % toFixed(value)); }
   inline cf6p10 operator&(cfl32 &value) const { return (cf6p10 &)(data & toFixed(value)); }
   inline cf6p10 operator|(cfl32 &value) const { return (cf6p10 &)(data | toFixed(value)); }
   inline cf6p10 operator^(cfl32 &value) const { return (cf6p10 &)(data ^ toFixed(value)); }
   inline cfl32 operator+=(cfl32 &value) { cfl32 temp = toFloat() + value; *this = toFixed(temp); return temp; }
   inline cfl32 operator-=(cfl32 &value) { cfl32 temp = toFloat() - value; *this = toFixed(temp); return temp; }
   inline cfl32 operator*=(cfl32 &value) { cfl32 temp = toFloat() * value; *this = toFixed(temp); return temp; }
   inline cfl32 operator/=(cfl32 &value) { cfl32 temp = toFloat() / value; *this = toFixed(temp); return temp; }
   inline cf6p10 operator%=(cfl32 &value) { return (cf6p10 &)(data %= toFixed(value)); }
   inline cf6p10 operator&=(cfl32 &value) { return (cf6p10 &)(data &= toFixed(value)); }
   inline cf6p10 operator|=(cfl32 &value) { return (cf6p10 &)(data |= toFixed(value)); }
   inline cf6p10 operator^=(cfl32 &value) { return (cf6p10 &)(data ^= toFixed(value)); }

   inline cbool operator==(cfl32 &value) const { return data == toFixed(value); }
   inline cbool operator!=(cfl32 &value) const { return data != toFixed(value); }
   inline cbool operator>=(cfl32 &value) const { return data <= toFixed(value); }
   inline cbool operator<=(cfl32 &value) const { return data >= toFixed(value); }
   inline cbool operator>(cfl32 &value) const { return data < toFixed(value); }
   inline cbool operator<(cfl32 &value) const { return data > toFixed(value); }
   inline cbool operator||(cfl32 &value) const { return data || toFixed(value); }
   inline cbool operator&&(cfl32 &value) const { return data && toFixed(value); }
};

// 16-bit, signed 7.8 : Decimal range of -128.0~127.99609375
struct fs7p8 {
   typedef const fs7p8 cfs7p8;

   ui16 data;

   inline cui16 toFixed(cfl32 &value) const { return ui16((value + 128.0f) * 256.0f); }
   inline cfl32 toFloat(void) const { return cfl32(data) * _fpdt_rcp256f - 128.0f; }
   inline cfl32 toFloat(cfs7p8 &value) const { return fl32(value.data) * _fpdt_rcp256f - 128.0f; }

   fs7p8(void) = default;
   fs7p8(const fp8n value) { data = toFixed(fl32(value) * _fpdt_data_.rangeDivMax8 + _fpdt_data_.origin8); }
   fs7p8(const fp16n value) { data = toFixed(fl32(value) * _fpdt_data_.rangeDivMax16 + _fpdt_data_.origin16); }
#ifdef _24BIT_INTEGERS_
   fs7p8(const fp24n value) { data = toFixed(fl32(value) * _fpdt_data_.rangeDivMax24 + _fpdt_data_.origin24); }
#endif
   fs7p8(const fp32n value) { data = toFixed(fl32(fl64(value) * _fpdt_data_.rangeDivMax32 + _fpdt_data_.origin32)); }
   fs7p8(cui16 value) { data = value; }
   fs7p8(csi32 value) { data = (ui16 &)value; }
   fs7p8(csi64 value) { data = (ui16 &)value; }
   fs7p8(cfl32 value) { data = toFixed(value); }

   operator ptr(void) const { return *this; }
   operator cfl32(void) const { return cfl32(data) * _fpdt_rcp256f - 128.0f; }

   inline cfs7p8 &operator&(void) const { return *this; }
   inline cfs7p8 operator~(void) const { return (cfs7p8 &)~data; }
   inline cfs7p8 operator-(void) const { return (cfs7p8 &)~data; }
   inline cbool operator!(void) const { return data == 0; }

   inline cfs7p8 operator++(void) { data++; return *this; }
   inline cfs7p8 operator++(int) { cui16 temp = data++; return (cfs7p8 &)temp; }
   inline cfs7p8 operator--(void) { data--; return *this; }
   inline cfs7p8 operator--(int) { cui16 temp = data--; return (cfs7p8 &)temp; }

   inline cbool operator==(csi32 &value) const { return data == (ui16 &)data; }
   inline cbool operator!=(csi32 &value) const { return data != (ui16 &)data; }
   inline cbool operator>=(csi32 &value) const { return data >= (ui16 &)data; }
   inline cbool operator<=(csi32 &value) const { return data <= (ui16 &)data; }
   inline cbool operator>(csi32 &value) const { return data > (ui16 &)data; }
   inline cbool operator<(csi32 &value) const { return data < (ui16 &)data; }
   inline cbool operator||(csi32 &value) const { return data || (ui16 &)data; }
   inline cbool operator&&(csi32 &value) const { return data && (ui16 &)data; }

   inline cfs7p8 operator<<(csi32 &value) const { return (cfs7p8 &)(data << value); }
   inline cfs7p8 operator>>(csi32 &value) const { return (cfs7p8 &)(data >> value); }
   inline cfs7p8 operator<<=(csi32 &value) { return (cfs7p8 &)(data <<= value); }
   inline cfs7p8 operator>>=(csi32 &value) { return (cfs7p8 &)(data >>= value); }

   inline cbool operator==(cfs7p8 &value) const { return data == value.data; }
   inline cbool operator!=(cfs7p8 &value) const { return data != value.data; }
   inline cbool operator>=(cfs7p8 &value) const { return data >= value.data; }
   inline cbool operator<=(cfs7p8 &value) const { return data <= value.data; }
   inline cbool operator>(cfs7p8 &value) const { return data > value.data; }
   inline cbool operator<(cfs7p8 &value) const { return data < value.data; }
   inline cbool operator||(cfs7p8 &value) const { return data || value.data; }
   inline cbool operator&&(cfs7p8 &value) const { return data && value.data; }

   inline cfs7p8 operator+(cfs7p8 &value) const { return (cfs7p8 &)(data + value.data - 32768); }
   inline cfs7p8 operator-(cfs7p8 &value) const { return (cfs7p8 &)(data - value.data - 32768); }
   inline cfs7p8 operator*(cfs7p8 &value) const { return toFixed(toFloat() * toFloat(value)); }
   inline cfs7p8 operator/(cfs7p8 &value) const { return toFixed(toFloat() / toFloat(value)); }
   inline cfs7p8 operator%(cfs7p8 &value) const { return (cfs7p8 &)(data % value.data); }
   inline cfs7p8 operator&(cfs7p8 &value) const { return (cfs7p8 &)(data & value.data); }
   inline cfs7p8 operator|(cfs7p8 &value) const { return (cfs7p8 &)(data | value.data); }
   inline cfs7p8 operator^(cfs7p8 &value) const { return (cfs7p8 &)(data ^ value.data); }
   inline cfs7p8 operator+=(cfs7p8 &value) { return (cfs7p8 &)(data += value.data - 32768); }
   inline cfs7p8 operator-=(cfs7p8 &value) { return (cfs7p8 &)(data -= value.data - 32768); }
   inline cfs7p8 operator*=(cfs7p8 &value) { return data = toFixed(toFloat() * toFloat(value)); }
   inline cfs7p8 operator/=(cfs7p8 &value) { return data = toFixed(toFloat() / toFloat(value)); }
   inline cfs7p8 operator%=(cfs7p8 &value) { return (cfs7p8 &)(data %= value.data); }
   inline cfs7p8 operator&=(cfs7p8 &value) { return (cfs7p8 &)(data &= value.data); }
   inline cfs7p8 operator|=(cfs7p8 &value) { return (cfs7p8 &)(data |= value.data); }
   inline cfs7p8 operator^=(cfs7p8 &value) { return (cfs7p8 &)(data ^= value.data); }

   inline cfl32 operator+(cfl32 &value) const { return toFloat() + value; }
   inline cfl32 operator-(cfl32 &value) const { return toFloat() - value; }
   inline cfl32 operator*(cfl32 &value) const { return toFloat() * value; }
   inline cfl32 operator/(cfl32 &value) const { return toFloat() / value; }
   inline cfs7p8 operator%(cfl32 &value) const { return (cfs7p8 &)(data % toFixed(value)); }
   inline cfs7p8 operator&(cfl32 &value) const { return (cfs7p8 &)(data & toFixed(value)); }
   inline cfs7p8 operator|(cfl32 &value) const { return (cfs7p8 &)(data | toFixed(value)); }
   inline cfs7p8 operator^(cfl32 &value) const { return (cfs7p8 &)(data ^ toFixed(value)); }
   inline cfl32 operator+=(cfl32 &value) { cfl32 temp = toFloat() + value; *this = toFixed(temp); return temp; }
   inline cfl32 operator-=(cfl32 &value) { cfl32 temp = toFloat() - value; *this = toFixed(temp); return temp; }
   inline cfl32 operator*=(cfl32 &value) { cfl32 temp = toFloat() * value; *this = toFixed(temp); return temp; }
   inline cfl32 operator/=(cfl32 &value) { cfl32 temp = toFloat() / value; *this = toFixed(temp); return temp; }
   inline cfs7p8 operator%=(cfl32 &value) { return (cfs7p8 &)(data %= toFixed(value)); }
   inline cfs7p8 operator&=(cfl32 &value) { return (cfs7p8 &)(data &= toFixed(value)); }
   inline cfs7p8 operator|=(cfl32 &value) { return (cfs7p8 &)(data |= toFixed(value)); }
   inline cfs7p8 operator^=(cfl32 &value) { return (cfs7p8 &)(data ^= toFixed(value)); }

   inline cbool operator==(cfl32 &value) const { return data == toFixed(value); }
   inline cbool operator!=(cfl32 &value) const { return data != toFixed(value); }
   inline cbool operator>=(cfl32 &value) const { return data <= toFixed(value); }
   inline cbool operator<=(cfl32 &value) const { return data >= toFixed(value); }
   inline cbool operator>(cfl32 &value) const { return data < toFixed(value); }
   inline cbool operator<(cfl32 &value) const { return data > toFixed(value); }
   inline cbool operator||(cfl32 &value) const { return data || toFixed(value); }
   inline cbool operator&&(cfl32 &value) const { return data && toFixed(value); }
};

// 16-bit, 7.9 : Decimal range of 0.0~127.998046875
struct f7p9 {
   typedef const f7p9 cf7p9;

   ui16 data;

   inline cui16 toFixed(cfl32 &value) const { return ui16(value * 512.0f); }
   inline cfl32 toFloat(void) const { return cfl32(data) * _fpdt_rcp512f; }
   inline cfl32 toFloat(cf7p9 &value) const { return fl32(value.data) * _fpdt_rcp512f; }

   f7p9(void) = default;
   f7p9(const fp8n value) { data = toFixed(fl32(value) * _fpdt_data_.rangeDivMax8 + _fpdt_data_.origin8); }
   f7p9(const fp16n value) { data = toFixed(fl32(value) * _fpdt_data_.rangeDivMax16 + _fpdt_data_.origin16); }
#ifdef _24BIT_INTEGERS_
   f7p9(const fp24n value) { data = toFixed(fl32(value) * _fpdt_data_.rangeDivMax24 + _fpdt_data_.origin24); }
#endif
   f7p9(const fp32n value) { data = toFixed(fl32(fl64(value) * _fpdt_data_.rangeDivMax32 + _fpdt_data_.origin32)); }
   f7p9(cui16 value) { data = value; }
   f7p9(csi32 value) { data = (ui16 &)value; }
   f7p9(csi64 value) { data = (ui16 &)value; }
   f7p9(cfl32 value) { data = toFixed(value); }

   operator ptr(void) const { return *this; }
   operator cfl32(void) const { return cfl32(data) * _fpdt_rcp512f; }

   inline cf7p9 &operator&(void) const { return *this; }
   inline cf7p9 operator~(void) const { return (cf7p9 &)~data; }
   inline cf7p9 operator-(void) const { return (cf7p9 &)~data; }
   inline cbool operator!(void) const { return data == 0; }

   inline cf7p9 operator++(void) { data++; return *this; }
   inline cf7p9 operator++(int) { cui16 temp = data++; return (cf7p9 &)temp; }
   inline cf7p9 operator--(void) { data--; return *this; }
   inline cf7p9 operator--(int) { cui16 temp = data--; return (cf7p9 &)temp; }

   inline cbool operator==(csi32 &value) const { return data == (ui16 &)data; }
   inline cbool operator!=(csi32 &value) const { return data != (ui16 &)data; }
   inline cbool operator>=(csi32 &value) const { return data >= (ui16 &)data; }
   inline cbool operator<=(csi32 &value) const { return data <= (ui16 &)data; }
   inline cbool operator>(csi32 &value) const { return data > (ui16 &)data; }
   inline cbool operator<(csi32 &value) const { return data < (ui16 &)data; }
   inline cbool operator||(csi32 &value) const { return data || (ui16 &)data; }
   inline cbool operator&&(csi32 &value) const { return data && (ui16 &)data; }

   inline cf7p9 operator<<(csi32 &value) const { return (cf7p9 &)(data << value); }
   inline cf7p9 operator>>(csi32 &value) const { return (cf7p9 &)(data >> value); }
   inline cf7p9 operator<<=(csi32 &value) { return (cf7p9 &)(data <<= value); }
   inline cf7p9 operator>>=(csi32 &value) { return (cf7p9 &)(data >>= value); }

   inline cbool operator==(cf7p9 &value) const { return data == value.data; }
   inline cbool operator!=(cf7p9 &value) const { return data != value.data; }
   inline cbool operator>=(cf7p9 &value) const { return data >= value.data; }
   inline cbool operator<=(cf7p9 &value) const { return data <= value.data; }
   inline cbool operator>(cf7p9 &value) const { return data > value.data; }
   inline cbool operator<(cf7p9 &value) const { return data < value.data; }
   inline cbool operator||(cf7p9 &value) const { return data || value.data; }
   inline cbool operator&&(cf7p9 &value) const { return data && value.data; }

   inline cf7p9 operator+(cf7p9 &value) const { return (cf7p9 &)(data + value.data); }
   inline cf7p9 operator-(cf7p9 &value) const { return (cf7p9 &)(data - value.data); }
   inline cf7p9 operator*(cf7p9 &value) const { return toFixed(toFloat() * toFloat(value)); }
   inline cf7p9 operator/(cf7p9 &value) const { return toFixed(toFloat() / toFloat(value)); }
   inline cf7p9 operator%(cf7p9 &value) const { return (cf7p9 &)(data % value.data); }
   inline cf7p9 operator&(cf7p9 &value) const { return (cf7p9 &)(data & value.data); }
   inline cf7p9 operator|(cf7p9 &value) const { return (cf7p9 &)(data | value.data); }
   inline cf7p9 operator^(cf7p9 &value) const { return (cf7p9 &)(data ^ value.data); }
   inline cf7p9 operator+=(cf7p9 &value) { return (cf7p9 &)(data += value.data); }
   inline cf7p9 operator-=(cf7p9 &value) { return (cf7p9 &)(data -= value.data); }
   inline cf7p9 operator*=(cf7p9 &value) { return data = toFixed(toFloat() * toFloat(value)); }
   inline cf7p9 operator/=(cf7p9 &value) { return data = toFixed(toFloat() / toFloat(value)); }
   inline cf7p9 operator%=(cf7p9 &value) { return (cf7p9 &)(data %= value.data); }
   inline cf7p9 operator&=(cf7p9 &value) { return (cf7p9 &)(data &= value.data); }
   inline cf7p9 operator|=(cf7p9 &value) { return (cf7p9 &)(data |= value.data); }
   inline cf7p9 operator^=(cf7p9 &value) { return (cf7p9 &)(data ^= value.data); }

   inline cfl32 operator+(cfl32 &value) const { return toFloat() + value; }
   inline cfl32 operator-(cfl32 &value) const { return toFloat() - value; }
   inline cfl32 operator*(cfl32 &value) const { return toFloat() * value; }
   inline cfl32 operator/(cfl32 &value) const { return toFloat() / value; }
   inline cf7p9 operator%(cfl32 &value) const { return (cf7p9 &)(data % toFixed(value)); }
   inline cf7p9 operator&(cfl32 &value) const { return (cf7p9 &)(data & toFixed(value)); }
   inline cf7p9 operator|(cfl32 &value) const { return (cf7p9 &)(data | toFixed(value)); }
   inline cf7p9 operator^(cfl32 &value) const { return (cf7p9 &)(data ^ toFixed(value)); }
   inline cfl32 operator+=(cfl32 &value) { cfl32 temp = toFloat() + value; *this = toFixed(temp); return temp; }
   inline cfl32 operator-=(cfl32 &value) { cfl32 temp = toFloat() - value; *this = toFixed(temp); return temp; }
   inline cfl32 operator*=(cfl32 &value) { cfl32 temp = toFloat() * value; *this = toFixed(temp); return temp; }
   inline cfl32 operator/=(cfl32 &value) { cfl32 temp = toFloat() / value; *this = toFixed(temp); return temp; }
   inline cf7p9 operator%=(cfl32 &value) { return (cf7p9 &)(data %= toFixed(value)); }
   inline cf7p9 operator&=(cfl32 &value) { return (cf7p9 &)(data &= toFixed(value)); }
   inline cf7p9 operator|=(cfl32 &value) { return (cf7p9 &)(data |= toFixed(value)); }
   inline cf7p9 operator^=(cfl32 &value) { return (cf7p9 &)(data ^= toFixed(value)); }

   inline cbool operator==(cfl32 &value) const { return data == toFixed(value); }
   inline cbool operator!=(cfl32 &value) const { return data != toFixed(value); }
   inline cbool operator>=(cfl32 &value) const { return data <= toFixed(value); }
   inline cbool operator<=(cfl32 &value) const { return data >= toFixed(value); }
   inline cbool operator>(cfl32 &value) const { return data < toFixed(value); }
   inline cbool operator<(cfl32 &value) const { return data > toFixed(value); }
   inline cbool operator||(cfl32 &value) const { return data || toFixed(value); }
   inline cbool operator&&(cfl32 &value) const { return data && toFixed(value); }
};

// 16-bit, 8.8 : Decimal range of 0.0~255.99609375
struct f8p8 {
   typedef const f8p8 cf8p8;

   ui16 data;

   inline cui16 toFixed(cfl32 &value) const { return ui16(value * 256.0f); }
   inline cfl32 toFloat(void) const { return cfl32(data) * _fpdt_rcp256f; }
   inline cfl32 toFloat(cf8p8 &value) const { return fl32(value.data) * _fpdt_rcp256f; }

   f8p8(void) = default;
   f8p8(const fp8n value) { data = toFixed(fl32(value) * _fpdt_data_.rangeDivMax8 + _fpdt_data_.origin8); }
   f8p8(const fp16n value) { data = toFixed(fl32(value) * _fpdt_data_.rangeDivMax16 + _fpdt_data_.origin16); }
#ifdef _24BIT_INTEGERS_
   f8p8(const fp24n value) { data = toFixed(fl32(value) * _fpdt_data_.rangeDivMax24 + _fpdt_data_.origin24); }
#endif
   f8p8(const fp32n value) { data = toFixed(fl32(fl64(value) * _fpdt_data_.rangeDivMax32 + _fpdt_data_.origin32)); }
   f8p8(cui16 value) { data = value; }
   f8p8(csi32 value) { data = (ui16 &)value; }
   f8p8(csi64 value) { data = (ui16 &)value; }
   f8p8(cfl32 value) { data = toFixed(value); }

   operator ptr(void) const { return *this; }
   operator cfl32(void) const { return cfl32(data) * _fpdt_rcp256f; }

   inline cf8p8 &operator&(void) const { return *this; }
   inline cf8p8 operator~(void) const { return (cf8p8 &)~data; }
   inline cf8p8 operator-(void) const { return (cf8p8 &)~data; }
   inline cbool operator!(void) const { return data == 0; }

   inline cf8p8 operator++(void) { data++; return *this; }
   inline cf8p8 operator++(int) { cui16 temp = data++; return (cf8p8 &)temp; }
   inline cf8p8 operator--(void) { data--; return *this; }
   inline cf8p8 operator--(int) { cui16 temp = data--; return (cf8p8 &)temp; }

   inline cbool operator==(csi32 &value) const { return data == (ui16 &)data; }
   inline cbool operator!=(csi32 &value) const { return data != (ui16 &)data; }
   inline cbool operator>=(csi32 &value) const { return data >= (ui16 &)data; }
   inline cbool operator<=(csi32 &value) const { return data <= (ui16 &)data; }
   inline cbool operator>(csi32 &value) const { return data > (ui16 &)data; }
   inline cbool operator<(csi32 &value) const { return data < (ui16 &)data; }
   inline cbool operator||(csi32 &value) const { return data || (ui16 &)data; }
   inline cbool operator&&(csi32 &value) const { return data && (ui16 &)data; }

   inline cf8p8 operator<<(csi32 &value) const { return (cf8p8 &)(data << value); }
   inline cf8p8 operator>>(csi32 &value) const { return (cf8p8 &)(data >> value); }
   inline cf8p8 operator<<=(csi32 &value) { return (cf8p8 &)(data <<= value); }
   inline cf8p8 operator>>=(csi32 &value) { return (cf8p8 &)(data >>= value); }

   inline cbool operator==(cf8p8 &value) const { return data == value.data; }
   inline cbool operator!=(cf8p8 &value) const { return data != value.data; }
   inline cbool operator>=(cf8p8 &value) const { return data >= value.data; }
   inline cbool operator<=(cf8p8 &value) const { return data <= value.data; }
   inline cbool operator>(cf8p8 &value) const { return data > value.data; }
   inline cbool operator<(cf8p8 &value) const { return data < value.data; }
   inline cbool operator||(cf8p8 &value) const { return data || value.data; }
   inline cbool operator&&(cf8p8 &value) const { return data && value.data; }

   inline cf8p8 operator+(cf8p8 &value) const { return (cf8p8 &)(data + value.data); }
   inline cf8p8 operator-(cf8p8 &value) const { return (cf8p8 &)(data - value.data); }
   inline cf8p8 operator*(cf8p8 &value) const { return toFixed(toFloat() * toFloat(value)); }
   inline cf8p8 operator/(cf8p8 &value) const { return toFixed(toFloat() / toFloat(value)); }
   inline cf8p8 operator%(cf8p8 &value) const { return (cf8p8 &)(data % value.data); }
   inline cf8p8 operator&(cf8p8 &value) const { return (cf8p8 &)(data & value.data); }
   inline cf8p8 operator|(cf8p8 &value) const { return (cf8p8 &)(data | value.data); }
   inline cf8p8 operator^(cf8p8 &value) const { return (cf8p8 &)(data ^ value.data); }
   inline cf8p8 operator+=(cf8p8 &value) { return (cf8p8 &)(data += value.data); }
   inline cf8p8 operator-=(cf8p8 &value) { return (cf8p8 &)(data -= value.data); }
   inline cf8p8 operator*=(cf8p8 &value) { return data = toFixed(toFloat() * toFloat(value)); }
   inline cf8p8 operator/=(cf8p8 &value) { return data = toFixed(toFloat() / toFloat(value)); }
   inline cf8p8 operator%=(cf8p8 &value) { return (cf8p8 &)(data %= value.data); }
   inline cf8p8 operator&=(cf8p8 &value) { return (cf8p8 &)(data &= value.data); }
   inline cf8p8 operator|=(cf8p8 &value) { return (cf8p8 &)(data |= value.data); }
   inline cf8p8 operator^=(cf8p8 &value) { return (cf8p8 &)(data ^= value.data); }

   inline cfl32 operator+(cfl32 &value) const { return toFloat() + value; }
   inline cfl32 operator-(cfl32 &value) const { return toFloat() - value; }
   inline cfl32 operator*(cfl32 &value) const { return toFloat() * value; }
   inline cfl32 operator/(cfl32 &value) const { return toFloat() / value; }
   inline cf8p8 operator%(cfl32 &value) const { return (cf8p8 &)(data % toFixed(value)); }
   inline cf8p8 operator&(cfl32 &value) const { return (cf8p8 &)(data & toFixed(value)); }
   inline cf8p8 operator|(cfl32 &value) const { return (cf8p8 &)(data | toFixed(value)); }
   inline cf8p8 operator^(cfl32 &value) const { return (cf8p8 &)(data ^ toFixed(value)); }
   inline cfl32 operator+=(cfl32 &value) { cfl32 temp = toFloat() + value; *this = toFixed(temp); return temp; }
   inline cfl32 operator-=(cfl32 &value) { cfl32 temp = toFloat() - value; *this = toFixed(temp); return temp; }
   inline cfl32 operator*=(cfl32 &value) { cfl32 temp = toFloat() * value; *this = toFixed(temp); return temp; }
   inline cfl32 operator/=(cfl32 &value) { cfl32 temp = toFloat() / value; *this = toFixed(temp); return temp; }
   inline cf8p8 operator%=(cfl32 &value) { return (cf8p8 &)(data %= toFixed(value)); }
   inline cf8p8 operator&=(cfl32 &value) { return (cf8p8 &)(data &= toFixed(value)); }
   inline cf8p8 operator|=(cfl32 &value) { return (cf8p8 &)(data |= toFixed(value)); }
   inline cf8p8 operator^=(cfl32 &value) { return (cf8p8 &)(data ^= toFixed(value)); }

   inline cbool operator==(cfl32 &value) const { return data == toFixed(value); }
   inline cbool operator!=(cfl32 &value) const { return data != toFixed(value); }
   inline cbool operator>=(cfl32 &value) const { return data <= toFixed(value); }
   inline cbool operator<=(cfl32 &value) const { return data >= toFixed(value); }
   inline cbool operator>(cfl32 &value) const { return data < toFixed(value); }
   inline cbool operator<(cfl32 &value) const { return data > toFixed(value); }
   inline cbool operator||(cfl32 &value) const { return data || toFixed(value); }
   inline cbool operator&&(cfl32 &value) const { return data && toFixed(value); }
};

// Normalised 16-bit : Decimal range of 0.0~1.0
struct fp16n0_1 {
   typedef const fp16n0_1 cfp16n0_1;

   ui16 data;

   inline cui16 toFixed(cfl32 &value) const { return ui16(value * 65535.0f); }
   inline cfl32 toFloat(void) const { return cfl32(data) * _fpdt_rcp65535f; }
   inline cfl32 toFloat(cfp16n0_1 &value) const { return fl32(value.data) * _fpdt_rcp65535f; }

   fp16n0_1(void) = default;
   fp16n0_1(const fp8n value) { data = toFixed(fl32(value) * _fpdt_data_.rangeDivMax8 + _fpdt_data_.origin8); }
   fp16n0_1(const fp16n value) { data = toFixed(fl32(value) * _fpdt_data_.rangeDivMax16 + _fpdt_data_.origin16); }
#ifdef _24BIT_INTEGERS_
   fp16n0_1(const fp24n value) { data = toFixed(fl32(value) * _fpdt_data_.rangeDivMax24 + _fpdt_data_.origin24); }
#endif
   fp16n0_1(const fp32n value) { data = toFixed(fl32(fl64(value) * _fpdt_data_.rangeDivMax32 + _fpdt_data_.origin32)); }
   fp16n0_1(cui16 value) { data = value; }
   fp16n0_1(csi32 value) { data = (ui16 &)value; }
   fp16n0_1(csi64 value) { data = (ui16 &)value; }
   fp16n0_1(cfl32 value) { data = toFixed(value); }

   operator ptr(void) const { return *this; }
   operator cfl32(void) const { return cfl32(data) * _fpdt_rcp65535f; }

   inline cfp16n0_1 &operator&(void) const { return *this; }
   inline cfp16n0_1 operator~(void) const { return (cfp16n0_1 &)~data; }
   inline cfp16n0_1 operator-(void) const { return (cfp16n0_1 &)~data; }
   inline cbool operator!(void) const { return data == 0; }

   inline cfp16n0_1 operator++(void) { data++; return *this; }
   inline cfp16n0_1 operator++(int) { cui16 temp = data++; return (cfp16n0_1 &)temp; }
   inline cfp16n0_1 operator--(void) { data--; return *this; }
   inline cfp16n0_1 operator--(int) { cui16 temp = data--; return (cfp16n0_1 &)temp; }

   inline cbool operator==(csi32 &value) const { return data == (ui16 &)data; }
   inline cbool operator!=(csi32 &value) const { return data != (ui16 &)data; }
   inline cbool operator>=(csi32 &value) const { return data >= (ui16 &)data; }
   inline cbool operator<=(csi32 &value) const { return data <= (ui16 &)data; }
   inline cbool operator>(csi32 &value) const { return data > (ui16 &)data; }
   inline cbool operator<(csi32 &value) const { return data < (ui16 &)data; }
   inline cbool operator||(csi32 &value) const { return data || (ui16 &)data; }
   inline cbool operator&&(csi32 &value) const { return data && (ui16 &)data; }

   inline cfp16n0_1 operator<<(csi32 &value) const { return (cfp16n0_1 &)(data << value); }
   inline cfp16n0_1 operator>>(csi32 &value) const { return (cfp16n0_1 &)(data >> value); }
   inline cfp16n0_1 operator<<=(csi32 &value) { return (cfp16n0_1 &)(data <<= value); }
   inline cfp16n0_1 operator>>=(csi32 &value) { return (cfp16n0_1 &)(data >>= value); }

   inline cbool operator==(cfp16n0_1 &value) const { return data == value.data; }
   inline cbool operator!=(cfp16n0_1 &value) const { return data != value.data; }
   inline cbool operator>=(cfp16n0_1 &value) const { return data >= value.data; }
   inline cbool operator<=(cfp16n0_1 &value) const { return data <= value.data; }
   inline cbool operator>(cfp16n0_1 &value) const { return data > value.data; }
   inline cbool operator<(cfp16n0_1 &value) const { return data < value.data; }
   inline cbool operator||(cfp16n0_1 &value) const { return data || value.data; }
   inline cbool operator&&(cfp16n0_1 &value) const { return data && value.data; }

   inline cfp16n0_1 operator+(cfp16n0_1 &value) const { return (cfp16n0_1 &)(data + value.data); }
   inline cfp16n0_1 operator-(cfp16n0_1 &value) const { return (cfp16n0_1 &)(data - value.data); }
   inline cfp16n0_1 operator*(cfp16n0_1 &value) const { return toFixed(toFloat() * toFloat(value)); }
   inline cfp16n0_1 operator/(cfp16n0_1 &value) const { return toFixed(toFloat() / toFloat(value)); }
   inline cfp16n0_1 operator%(cfp16n0_1 &value) const { return (cfp16n0_1 &)(data % value.data); }
   inline cfp16n0_1 operator&(cfp16n0_1 &value) const { return (cfp16n0_1 &)(data & value.data); }
   inline cfp16n0_1 operator|(cfp16n0_1 &value) const { return (cfp16n0_1 &)(data | value.data); }
   inline cfp16n0_1 operator^(cfp16n0_1 &value) const { return (cfp16n0_1 &)(data ^ value.data); }
   inline cfp16n0_1 operator+=(cfp16n0_1 &value) { return (cfp16n0_1 &)(data += value.data); }
   inline cfp16n0_1 operator-=(cfp16n0_1 &value) { return (cfp16n0_1 &)(data -= value.data); }
   inline cfp16n0_1 operator*=(cfp16n0_1 &value) { return data = toFixed(toFloat() * toFloat(value)); }
   inline cfp16n0_1 operator/=(cfp16n0_1 &value) { return data = toFixed(toFloat() / toFloat(value)); }
   inline cfp16n0_1 operator%=(cfp16n0_1 &value) { return (cfp16n0_1 &)(data %= value.data); }
   inline cfp16n0_1 operator&=(cfp16n0_1 &value) { return (cfp16n0_1 &)(data &= value.data); }
   inline cfp16n0_1 operator|=(cfp16n0_1 &value) { return (cfp16n0_1 &)(data |= value.data); }
   inline cfp16n0_1 operator^=(cfp16n0_1 &value) { return (cfp16n0_1 &)(data ^= value.data); }

   inline cfl32 operator+(cfl32 &value) const { return toFloat() + value; }
   inline cfl32 operator-(cfl32 &value) const { return toFloat() - value; }
   inline cfl32 operator*(cfl32 &value) const { return toFloat() * value; }
   inline cfl32 operator/(cfl32 &value) const { return toFloat() / value; }
   inline cfp16n0_1 operator%(cfl32 &value) const { return (cfp16n0_1 &)(data % toFixed(value)); }
   inline cfp16n0_1 operator&(cfl32 &value) const { return (cfp16n0_1 &)(data & toFixed(value)); }
   inline cfp16n0_1 operator|(cfl32 &value) const { return (cfp16n0_1 &)(data | toFixed(value)); }
   inline cfp16n0_1 operator^(cfl32 &value) const { return (cfp16n0_1 &)(data ^ toFixed(value)); }
   inline cfl32 operator+=(cfl32 &value) { cfl32 temp = toFloat() + value; *this = toFixed(temp); return temp; }
   inline cfl32 operator-=(cfl32 &value) { cfl32 temp = toFloat() - value; *this = toFixed(temp); return temp; }
   inline cfl32 operator*=(cfl32 &value) { cfl32 temp = toFloat() * value; *this = toFixed(temp); return temp; }
   inline cfl32 operator/=(cfl32 &value) { cfl32 temp = toFloat() / value; *this = toFixed(temp); return temp; }
   inline cfp16n0_1 operator%=(cfl32 &value) { return (cfp16n0_1 &)(data %= toFixed(value)); }
   inline cfp16n0_1 operator&=(cfl32 &value) { return (cfp16n0_1 &)(data &= toFixed(value)); }
   inline cfp16n0_1 operator|=(cfl32 &value) { return (cfp16n0_1 &)(data |= toFixed(value)); }
   inline cfp16n0_1 operator^=(cfl32 &value) { return (cfp16n0_1 &)(data ^= toFixed(value)); }

   inline cbool operator==(cfl32 &value) const { return data == toFixed(value); }
   inline cbool operator!=(cfl32 &value) const { return data != toFixed(value); }
   inline cbool operator>=(cfl32 &value) const { return data <= toFixed(value); }
   inline cbool operator<=(cfl32 &value) const { return data >= toFixed(value); }
   inline cbool operator>(cfl32 &value) const { return data < toFixed(value); }
   inline cbool operator<(cfl32 &value) const { return data > toFixed(value); }
   inline cbool operator||(cfl32 &value) const { return data || toFixed(value); }
   inline cbool operator&&(cfl32 &value) const { return data && toFixed(value); }
};

// Normalised 16-bit : Decimal range of 0.0~2.0
struct fp16n0_2 {
   typedef const fp16n0_2 cfp16n0_2;

   ui16 data;

   inline cui16 toFixed(cfl32 &value) const { return ui16(value * 32767.5f); }
   inline cfl32 toFloat(void) const { return cfl32(data) * _fpdt_2div65535f; }
   inline cfl32 toFloat(cfp16n0_2 &value) const { return fl32(value.data) * _fpdt_2div65535f; }

   fp16n0_2(void) = default;
   fp16n0_2(const fp8n value) { data = toFixed(fl32(value) * _fpdt_data_.rangeDivMax8 + _fpdt_data_.origin8); }
   fp16n0_2(const fp16n value) { data = toFixed(fl32(value) * _fpdt_data_.rangeDivMax16 + _fpdt_data_.origin16); }
#ifdef _24BIT_INTEGERS_
   fp16n0_2(const fp24n value) { data = toFixed(fl32(value) * _fpdt_data_.rangeDivMax24 + _fpdt_data_.origin24); }
#endif
   fp16n0_2(const fp32n value) { data = toFixed(fl32(fl64(value) * _fpdt_data_.rangeDivMax32 + _fpdt_data_.origin32)); }
   fp16n0_2(cui16 value) { data = value; }
   fp16n0_2(csi32 value) { data = (ui16 &)value; }
   fp16n0_2(csi64 value) { data = (ui16 &)value; }
   fp16n0_2(cfl32 value) { data = toFixed(value); }

   operator ptr(void) const { return *this; }
   operator cfl32(void) const { return cfl32(data) * _fpdt_2div65535f; }

   inline cfp16n0_2 &operator&(void) const { return *this; }
   inline cfp16n0_2 operator~(void) const { return (cfp16n0_2 &)~data; }
   inline cfp16n0_2 operator-(void) const { return (cfp16n0_2 &)~data; }
   inline cbool operator!(void) const { return data == 0; }

   inline cfp16n0_2 operator++(void) { data++; return *this; }
   inline cfp16n0_2 operator++(int) { cui16 temp = data++; return (cfp16n0_2 &)temp; }
   inline cfp16n0_2 operator--(void) { data--; return *this; }
   inline cfp16n0_2 operator--(int) { cui16 temp = data--; return (cfp16n0_2 &)temp; }

   inline cbool operator==(csi32 &value) const { return data == (ui16 &)data; }
   inline cbool operator!=(csi32 &value) const { return data != (ui16 &)data; }
   inline cbool operator>=(csi32 &value) const { return data >= (ui16 &)data; }
   inline cbool operator<=(csi32 &value) const { return data <= (ui16 &)data; }
   inline cbool operator>(csi32 &value) const { return data > (ui16 &)data; }
   inline cbool operator<(csi32 &value) const { return data < (ui16 &)data; }
   inline cbool operator||(csi32 &value) const { return data || (ui16 &)data; }
   inline cbool operator&&(csi32 &value) const { return data && (ui16 &)data; }

   inline cfp16n0_2 operator<<(csi32 &value) const { return (cfp16n0_2 &)(data << value); }
   inline cfp16n0_2 operator>>(csi32 &value) const { return (cfp16n0_2 &)(data >> value); }
   inline cfp16n0_2 operator<<=(csi32 &value) { return (cfp16n0_2 &)(data <<= value); }
   inline cfp16n0_2 operator>>=(csi32 &value) { return (cfp16n0_2 &)(data >>= value); }

   inline cbool operator==(cfp16n0_2 &value) const { return data == value.data; }
   inline cbool operator!=(cfp16n0_2 &value) const { return data != value.data; }
   inline cbool operator>=(cfp16n0_2 &value) const { return data >= value.data; }
   inline cbool operator<=(cfp16n0_2 &value) const { return data <= value.data; }
   inline cbool operator>(cfp16n0_2 &value) const { return data > value.data; }
   inline cbool operator<(cfp16n0_2 &value) const { return data < value.data; }
   inline cbool operator||(cfp16n0_2 &value) const { return data || value.data; }
   inline cbool operator&&(cfp16n0_2 &value) const { return data && value.data; }

   inline cfp16n0_2 operator+(cfp16n0_2 &value) const { return (cfp16n0_2 &)(data + value.data); }
   inline cfp16n0_2 operator-(cfp16n0_2 &value) const { return (cfp16n0_2 &)(data - value.data); }
   inline cfp16n0_2 operator*(cfp16n0_2 &value) const { return toFixed(toFloat() * toFloat(value)); }
   inline cfp16n0_2 operator/(cfp16n0_2 &value) const { return toFixed(toFloat() / toFloat(value)); }
   inline cfp16n0_2 operator%(cfp16n0_2 &value) const { return (cfp16n0_2 &)(data % value.data); }
   inline cfp16n0_2 operator&(cfp16n0_2 &value) const { return (cfp16n0_2 &)(data & value.data); }
   inline cfp16n0_2 operator|(cfp16n0_2 &value) const { return (cfp16n0_2 &)(data | value.data); }
   inline cfp16n0_2 operator^(cfp16n0_2 &value) const { return (cfp16n0_2 &)(data ^ value.data); }
   inline cfp16n0_2 operator+=(cfp16n0_2 &value) { return (cfp16n0_2 &)(data += value.data); }
   inline cfp16n0_2 operator-=(cfp16n0_2 &value) { return (cfp16n0_2 &)(data -= value.data); }
   inline cfp16n0_2 operator*=(cfp16n0_2 &value) { return data = toFixed(toFloat() * toFloat(value)); }
   inline cfp16n0_2 operator/=(cfp16n0_2 &value) { return data = toFixed(toFloat() / toFloat(value)); }
   inline cfp16n0_2 operator%=(cfp16n0_2 &value) { return (cfp16n0_2 &)(data %= value.data); }
   inline cfp16n0_2 operator&=(cfp16n0_2 &value) { return (cfp16n0_2 &)(data &= value.data); }
   inline cfp16n0_2 operator|=(cfp16n0_2 &value) { return (cfp16n0_2 &)(data |= value.data); }
   inline cfp16n0_2 operator^=(cfp16n0_2 &value) { return (cfp16n0_2 &)(data ^= value.data); }

   inline cfl32 operator+(cfl32 &value) const { return toFloat() + value; }
   inline cfl32 operator-(cfl32 &value) const { return toFloat() - value; }
   inline cfl32 operator*(cfl32 &value) const { return toFloat() * value; }
   inline cfl32 operator/(cfl32 &value) const { return toFloat() / value; }
   inline cfp16n0_2 operator%(cfl32 &value) const { return (cfp16n0_2 &)(data % toFixed(value)); }
   inline cfp16n0_2 operator&(cfl32 &value) const { return (cfp16n0_2 &)(data & toFixed(value)); }
   inline cfp16n0_2 operator|(cfl32 &value) const { return (cfp16n0_2 &)(data | toFixed(value)); }
   inline cfp16n0_2 operator^(cfl32 &value) const { return (cfp16n0_2 &)(data ^ toFixed(value)); }
   inline cfl32 operator+=(cfl32 &value) { cfl32 temp = toFloat() + value; *this = toFixed(temp); return temp; }
   inline cfl32 operator-=(cfl32 &value) { cfl32 temp = toFloat() - value; *this = toFixed(temp); return temp; }
   inline cfl32 operator*=(cfl32 &value) { cfl32 temp = toFloat() * value; *this = toFixed(temp); return temp; }
   inline cfl32 operator/=(cfl32 &value) { cfl32 temp = toFloat() / value; *this = toFixed(temp); return temp; }
   inline cfp16n0_2 operator%=(cfl32 &value) { return (cfp16n0_2 &)(data %= toFixed(value)); }
   inline cfp16n0_2 operator&=(cfl32 &value) { return (cfp16n0_2 &)(data &= toFixed(value)); }
   inline cfp16n0_2 operator|=(cfl32 &value) { return (cfp16n0_2 &)(data |= toFixed(value)); }
   inline cfp16n0_2 operator^=(cfl32 &value) { return (cfp16n0_2 &)(data ^= toFixed(value)); }

   inline cbool operator==(cfl32 &value) const { return data == toFixed(value); }
   inline cbool operator!=(cfl32 &value) const { return data != toFixed(value); }
   inline cbool operator>=(cfl32 &value) const { return data <= toFixed(value); }
   inline cbool operator<=(cfl32 &value) const { return data >= toFixed(value); }
   inline cbool operator>(cfl32 &value) const { return data < toFixed(value); }
   inline cbool operator<(cfl32 &value) const { return data > toFixed(value); }
   inline cbool operator||(cfl32 &value) const { return data || toFixed(value); }
   inline cbool operator&&(cfl32 &value) const { return data && toFixed(value); }
};

// Normalised 16-bit : Decimal range of 0.0~3.0
struct fp16n0_3 {
   typedef const fp16n0_3 cfp16n0_3;

   ui16 data;

   inline cui16 toFixed(cfl32 &value) const { return ui16(value * 21845.0f); }
   inline cfl32 toFloat(void) const { return cfl32(data) * _fpdt_3div65535f; }
   inline cfl32 toFloat(cfp16n0_3 &value) const { return fl32(value.data) * _fpdt_3div65535f; }

   fp16n0_3(void) = default;
   fp16n0_3(const fp8n value) { data = toFixed(fl32(value) * _fpdt_data_.rangeDivMax8 + _fpdt_data_.origin8); }
   fp16n0_3(const fp16n value) { data = toFixed(fl32(value) * _fpdt_data_.rangeDivMax16 + _fpdt_data_.origin16); }
#ifdef _24BIT_INTEGERS_
   fp16n0_3(const fp24n value) { data = toFixed(fl32(value) * _fpdt_data_.rangeDivMax24 + _fpdt_data_.origin24); }
#endif
   fp16n0_3(const fp32n value) { data = toFixed(fl32(fl64(value) * _fpdt_data_.rangeDivMax32 + _fpdt_data_.origin32)); }
   fp16n0_3(cui16 value) { data = value; }
   fp16n0_3(csi32 value) { data = (ui16 &)value; }
   fp16n0_3(csi64 value) { data = (ui16 &)value; }
   fp16n0_3(cfl32 value) { data = toFixed(value); }

   operator ptr(void) const { return *this; }
   operator cfl32(void) const { return cfl32(data) * _fpdt_3div65535f; }

   inline cfp16n0_3 &operator&(void) const { return *this; }
   inline cfp16n0_3 operator~(void) const { return (cfp16n0_3 &)~data; }
   inline cfp16n0_3 operator-(void) const { return (cfp16n0_3 &)~data; }
   inline cbool operator!(void) const { return data == 0; }

   inline cfp16n0_3 operator++(void) { data++; return *this; }
   inline cfp16n0_3 operator++(int) { cui16 temp = data++; return (cfp16n0_3 &)temp; }
   inline cfp16n0_3 operator--(void) { data--; return *this; }
   inline cfp16n0_3 operator--(int) { cui16 temp = data--; return (cfp16n0_3 &)temp; }

   inline cbool operator==(csi32 &value) const { return data == (ui16 &)data; }
   inline cbool operator!=(csi32 &value) const { return data != (ui16 &)data; }
   inline cbool operator>=(csi32 &value) const { return data >= (ui16 &)data; }
   inline cbool operator<=(csi32 &value) const { return data <= (ui16 &)data; }
   inline cbool operator>(csi32 &value) const { return data > (ui16 &)data; }
   inline cbool operator<(csi32 &value) const { return data < (ui16 &)data; }
   inline cbool operator||(csi32 &value) const { return data || (ui16 &)data; }
   inline cbool operator&&(csi32 &value) const { return data && (ui16 &)data; }

   inline cfp16n0_3 operator<<(csi32 &value) const { return (cfp16n0_3 &)(data << value); }
   inline cfp16n0_3 operator>>(csi32 &value) const { return (cfp16n0_3 &)(data >> value); }
   inline cfp16n0_3 operator<<=(csi32 &value) { return (cfp16n0_3 &)(data <<= value); }
   inline cfp16n0_3 operator>>=(csi32 &value) { return (cfp16n0_3 &)(data >>= value); }

   inline cbool operator==(cfp16n0_3 &value) const { return data == value.data; }
   inline cbool operator!=(cfp16n0_3 &value) const { return data != value.data; }
   inline cbool operator>=(cfp16n0_3 &value) const { return data >= value.data; }
   inline cbool operator<=(cfp16n0_3 &value) const { return data <= value.data; }
   inline cbool operator>(cfp16n0_3 &value) const { return data > value.data; }
   inline cbool operator<(cfp16n0_3 &value) const { return data < value.data; }
   inline cbool operator||(cfp16n0_3 &value) const { return data || value.data; }
   inline cbool operator&&(cfp16n0_3 &value) const { return data && value.data; }

   inline cfp16n0_3 operator+(cfp16n0_3 &value) const { return (cfp16n0_3 &)(data + value.data); }
   inline cfp16n0_3 operator-(cfp16n0_3 &value) const { return (cfp16n0_3 &)(data - value.data); }
   inline cfp16n0_3 operator*(cfp16n0_3 &value) const { return toFixed(toFloat() * toFloat(value)); }
   inline cfp16n0_3 operator/(cfp16n0_3 &value) const { return toFixed(toFloat() / toFloat(value)); }
   inline cfp16n0_3 operator%(cfp16n0_3 &value) const { return (cfp16n0_3 &)(data % value.data); }
   inline cfp16n0_3 operator&(cfp16n0_3 &value) const { return (cfp16n0_3 &)(data & value.data); }
   inline cfp16n0_3 operator|(cfp16n0_3 &value) const { return (cfp16n0_3 &)(data | value.data); }
   inline cfp16n0_3 operator^(cfp16n0_3 &value) const { return (cfp16n0_3 &)(data ^ value.data); }
   inline cfp16n0_3 operator+=(cfp16n0_3 &value) { return (cfp16n0_3 &)(data += value.data); }
   inline cfp16n0_3 operator-=(cfp16n0_3 &value) { return (cfp16n0_3 &)(data -= value.data); }
   inline cfp16n0_3 operator*=(cfp16n0_3 &value) { return data = toFixed(toFloat() * toFloat(value)); }
   inline cfp16n0_3 operator/=(cfp16n0_3 &value) { return data = toFixed(toFloat() / toFloat(value)); }
   inline cfp16n0_3 operator%=(cfp16n0_3 &value) { return (cfp16n0_3 &)(data %= value.data); }
   inline cfp16n0_3 operator&=(cfp16n0_3 &value) { return (cfp16n0_3 &)(data &= value.data); }
   inline cfp16n0_3 operator|=(cfp16n0_3 &value) { return (cfp16n0_3 &)(data |= value.data); }
   inline cfp16n0_3 operator^=(cfp16n0_3 &value) { return (cfp16n0_3 &)(data ^= value.data); }

   inline cfl32 operator+(cfl32 &value) const { return toFloat() + value; }
   inline cfl32 operator-(cfl32 &value) const { return toFloat() - value; }
   inline cfl32 operator*(cfl32 &value) const { return toFloat() * value; }
   inline cfl32 operator/(cfl32 &value) const { return toFloat() / value; }
   inline cfp16n0_3 operator%(cfl32 &value) const { return (cfp16n0_3 &)(data % toFixed(value)); }
   inline cfp16n0_3 operator&(cfl32 &value) const { return (cfp16n0_3 &)(data & toFixed(value)); }
   inline cfp16n0_3 operator|(cfl32 &value) const { return (cfp16n0_3 &)(data | toFixed(value)); }
   inline cfp16n0_3 operator^(cfl32 &value) const { return (cfp16n0_3 &)(data ^ toFixed(value)); }
   inline cfl32 operator+=(cfl32 &value) { cfl32 temp = toFloat() + value; *this = toFixed(temp); return temp; }
   inline cfl32 operator-=(cfl32 &value) { cfl32 temp = toFloat() - value; *this = toFixed(temp); return temp; }
   inline cfl32 operator*=(cfl32 &value) { cfl32 temp = toFloat() * value; *this = toFixed(temp); return temp; }
   inline cfl32 operator/=(cfl32 &value) { cfl32 temp = toFloat() / value; *this = toFixed(temp); return temp; }
   inline cfp16n0_3 operator%=(cfl32 &value) { return (cfp16n0_3 &)(data %= toFixed(value)); }
   inline cfp16n0_3 operator&=(cfl32 &value) { return (cfp16n0_3 &)(data &= toFixed(value)); }
   inline cfp16n0_3 operator|=(cfl32 &value) { return (cfp16n0_3 &)(data |= toFixed(value)); }
   inline cfp16n0_3 operator^=(cfl32 &value) { return (cfp16n0_3 &)(data ^= toFixed(value)); }

   inline cbool operator==(cfl32 &value) const { return data == toFixed(value); }
   inline cbool operator!=(cfl32 &value) const { return data != toFixed(value); }
   inline cbool operator>=(cfl32 &value) const { return data <= toFixed(value); }
   inline cbool operator<=(cfl32 &value) const { return data >= toFixed(value); }
   inline cbool operator>(cfl32 &value) const { return data < toFixed(value); }
   inline cbool operator<(cfl32 &value) const { return data > toFixed(value); }
   inline cbool operator||(cfl32 &value) const { return data || toFixed(value); }
   inline cbool operator&&(cfl32 &value) const { return data && toFixed(value); }
};

// Normalised 16-bit : Decimal range of 0.0~128.0
struct fp16n0_128 {
   typedef const fp16n0_128 cfp16n0_128;

   ui16 data;

   inline cui16 toFixed(cfl32 &value) const { return ui16(value * _fpdt_65535div128f); }
   inline cfl32 toFloat(void) const { return cfl32(data) * _fpdt_128div65535f; }
   inline cfl32 toFloat(cfp16n0_128 &value) const { return fl32(value.data) * _fpdt_128div65535f; }

   fp16n0_128(void) = default;
   fp16n0_128(const fp8n value) { data = toFixed(fl32(value) * _fpdt_data_.rangeDivMax8 + _fpdt_data_.origin8); }
   fp16n0_128(const fp16n value) { data = toFixed(fl32(value) * _fpdt_data_.rangeDivMax16 + _fpdt_data_.origin16); }
#ifdef _24BIT_INTEGERS_
   fp16n0_128(const fp24n value) { data = toFixed(fl32(value) * _fpdt_data_.rangeDivMax24 + _fpdt_data_.origin24); }
#endif
   fp16n0_128(const fp32n value) { data = toFixed(fl32(fl64(value) * _fpdt_data_.rangeDivMax32 + _fpdt_data_.origin32)); }
   fp16n0_128(cui16 value) { data = value; }
   fp16n0_128(csi32 value) { data = (ui16 &)value; }
   fp16n0_128(csi64 value) { data = (ui16 &)value; }
   fp16n0_128(cfl32 value) { data = toFixed(value); }

   operator ptr(void) const { return *this; }
   operator cfl32(void) const { return cfl32(data) * _fpdt_128div65535f; }

   inline cfp16n0_128 &operator&(void) const { return *this; }
   inline cfp16n0_128 operator~(void) const { return (cfp16n0_128 &)~data; }
   inline cfp16n0_128 operator-(void) const { return (cfp16n0_128 &)~data; }
   inline cbool operator!(void) const { return data == 0; }

   inline cfp16n0_128 operator++(void) { data++; return *this; }
   inline cfp16n0_128 operator++(int) { cui16 temp = data++; return (cfp16n0_128 &)temp; }
   inline cfp16n0_128 operator--(void) { data--; return *this; }
   inline cfp16n0_128 operator--(int) { cui16 temp = data--; return (cfp16n0_128 &)temp; }

   inline cbool operator==(csi32 &value) const { return data == (ui16 &)data; }
   inline cbool operator!=(csi32 &value) const { return data != (ui16 &)data; }
   inline cbool operator>=(csi32 &value) const { return data >= (ui16 &)data; }
   inline cbool operator<=(csi32 &value) const { return data <= (ui16 &)data; }
   inline cbool operator>(csi32 &value) const { return data > (ui16 &)data; }
   inline cbool operator<(csi32 &value) const { return data < (ui16 &)data; }
   inline cbool operator||(csi32 &value) const { return data || (ui16 &)data; }
   inline cbool operator&&(csi32 &value) const { return data && (ui16 &)data; }

   inline cfp16n0_128 operator<<(csi32 &value) const { return (cfp16n0_128 &)(data << value); }
   inline cfp16n0_128 operator>>(csi32 &value) const { return (cfp16n0_128 &)(data >> value); }
   inline cfp16n0_128 operator<<=(csi32 &value) { return (cfp16n0_128 &)(data <<= value); }
   inline cfp16n0_128 operator>>=(csi32 &value) { return (cfp16n0_128 &)(data >>= value); }

   inline cbool operator==(cfp16n0_128 &value) const { return data == value.data; }
   inline cbool operator!=(cfp16n0_128 &value) const { return data != value.data; }
   inline cbool operator>=(cfp16n0_128 &value) const { return data >= value.data; }
   inline cbool operator<=(cfp16n0_128 &value) const { return data <= value.data; }
   inline cbool operator>(cfp16n0_128 &value) const { return data > value.data; }
   inline cbool operator<(cfp16n0_128 &value) const { return data < value.data; }
   inline cbool operator||(cfp16n0_128 &value) const { return data || value.data; }
   inline cbool operator&&(cfp16n0_128 &value) const { return data && value.data; }

   inline cfp16n0_128 operator+(cfp16n0_128 &value) const { return (cfp16n0_128 &)(data + value.data); }
   inline cfp16n0_128 operator-(cfp16n0_128 &value) const { return (cfp16n0_128 &)(data - value.data); }
   inline cfp16n0_128 operator*(cfp16n0_128 &value) const { return toFixed(toFloat() * toFloat(value)); }
   inline cfp16n0_128 operator/(cfp16n0_128 &value) const { return toFixed(toFloat() / toFloat(value)); }
   inline cfp16n0_128 operator%(cfp16n0_128 &value) const { return (cfp16n0_128 &)(data % value.data); }
   inline cfp16n0_128 operator&(cfp16n0_128 &value) const { return (cfp16n0_128 &)(data & value.data); }
   inline cfp16n0_128 operator|(cfp16n0_128 &value) const { return (cfp16n0_128 &)(data | value.data); }
   inline cfp16n0_128 operator^(cfp16n0_128 &value) const { return (cfp16n0_128 &)(data ^ value.data); }
   inline cfp16n0_128 operator+=(cfp16n0_128 &value) { return (cfp16n0_128 &)(data += value.data); }
   inline cfp16n0_128 operator-=(cfp16n0_128 &value) { return (cfp16n0_128 &)(data -= value.data); }
   inline cfp16n0_128 operator*=(cfp16n0_128 &value) { return data = toFixed(toFloat() * toFloat(value)); }
   inline cfp16n0_128 operator/=(cfp16n0_128 &value) { return data = toFixed(toFloat() / toFloat(value)); }
   inline cfp16n0_128 operator%=(cfp16n0_128 &value) { return (cfp16n0_128 &)(data %= value.data); }
   inline cfp16n0_128 operator&=(cfp16n0_128 &value) { return (cfp16n0_128 &)(data &= value.data); }
   inline cfp16n0_128 operator|=(cfp16n0_128 &value) { return (cfp16n0_128 &)(data |= value.data); }
   inline cfp16n0_128 operator^=(cfp16n0_128 &value) { return (cfp16n0_128 &)(data ^= value.data); }

   inline cfl32 operator+(cfl32 &value) const { return toFloat() + value; }
   inline cfl32 operator-(cfl32 &value) const { return toFloat() - value; }
   inline cfl32 operator*(cfl32 &value) const { return toFloat() * value; }
   inline cfl32 operator/(cfl32 &value) const { return toFloat() / value; }
   inline cfp16n0_128 operator%(cfl32 &value) const { return (cfp16n0_128 &)(data % toFixed(value)); }
   inline cfp16n0_128 operator&(cfl32 &value) const { return (cfp16n0_128 &)(data & toFixed(value)); }
   inline cfp16n0_128 operator|(cfl32 &value) const { return (cfp16n0_128 &)(data | toFixed(value)); }
   inline cfp16n0_128 operator^(cfl32 &value) const { return (cfp16n0_128 &)(data ^ toFixed(value)); }
   inline cfl32 operator+=(cfl32 &value) { cfl32 temp = toFloat() + value; *this = toFixed(temp); return temp; }
   inline cfl32 operator-=(cfl32 &value) { cfl32 temp = toFloat() - value; *this = toFixed(temp); return temp; }
   inline cfl32 operator*=(cfl32 &value) { cfl32 temp = toFloat() * value; *this = toFixed(temp); return temp; }
   inline cfl32 operator/=(cfl32 &value) { cfl32 temp = toFloat() / value; *this = toFixed(temp); return temp; }
   inline cfp16n0_128 operator%=(cfl32 &value) { return (cfp16n0_128 &)(data %= toFixed(value)); }
   inline cfp16n0_128 operator&=(cfl32 &value) { return (cfp16n0_128 &)(data &= toFixed(value)); }
   inline cfp16n0_128 operator|=(cfl32 &value) { return (cfp16n0_128 &)(data |= toFixed(value)); }
   inline cfp16n0_128 operator^=(cfl32 &value) { return (cfp16n0_128 &)(data ^= toFixed(value)); }

   inline cbool operator==(cfl32 &value) const { return data == toFixed(value); }
   inline cbool operator!=(cfl32 &value) const { return data != toFixed(value); }
   inline cbool operator>=(cfl32 &value) const { return data <= toFixed(value); }
   inline cbool operator<=(cfl32 &value) const { return data >= toFixed(value); }
   inline cbool operator>(cfl32 &value) const { return data < toFixed(value); }
   inline cbool operator<(cfl32 &value) const { return data > toFixed(value); }
   inline cbool operator||(cfl32 &value) const { return data || toFixed(value); }
   inline cbool operator&&(cfl32 &value) const { return data && toFixed(value); }
};

// Normalised 16-bit : Decimal range of -1.0~1.0
struct fp16n_1_1 {
   typedef const fp16n_1_1 cfp16n_1_1;

   ui16 data;

   inline cui16 toFixed(cfl32 &value) const { return ui16((value + 1.0f) * _fpdt_65535div2f); }
   inline cfl32 toFloat(void) const { return cfl32(data) * _fpdt_2div65535f - 1.0f; }
   inline cfl32 toFloat(cfp16n_1_1 &value) const { return fl32(value.data) * _fpdt_2div65535f - 1.0f; }

   fp16n_1_1(void) = default;
   fp16n_1_1(const fp8n value) { data = toFixed(fl32(value) * _fpdt_data_.rangeDivMax8 + _fpdt_data_.origin8); }
   fp16n_1_1(const fp16n value) { data = toFixed(fl32(value) * _fpdt_data_.rangeDivMax16 + _fpdt_data_.origin16); }
#ifdef _24BIT_INTEGERS_
   fp16n_1_1(const fp24n value) { data = toFixed(fl32(value) * _fpdt_data_.rangeDivMax24 + _fpdt_data_.origin24); }
#endif
   fp16n_1_1(const fp32n value) { data = toFixed(fl32(fl64(value) * _fpdt_data_.rangeDivMax32 + _fpdt_data_.origin32)); }
   fp16n_1_1(cui16 value) { data = value; }
   fp16n_1_1(csi32 value) { data = (ui16 &)value; }
   fp16n_1_1(csi64 value) { data = (ui16 &)value; }
   fp16n_1_1(cfl32 value) { data = toFixed(value); }

   operator ptr(void) const { return *this; }
   operator cfl32(void) const { return cfl32(data) * _fpdt_2div65535f - 1.0f; }

   inline cfp16n_1_1 &operator&(void) const { return *this; }
   inline cfp16n_1_1 operator~(void) const { return (cfp16n_1_1 &)~data; }
   inline cfp16n_1_1 operator-(void) const { return (cfp16n_1_1 &)~data; }
   inline cbool operator!(void) const { return data == 0; }

   inline cfp16n_1_1 operator++(void) { data++; return *this; }
   inline cfp16n_1_1 operator++(int) { cui16 temp = data++; return (cfp16n_1_1 &)temp; }
   inline cfp16n_1_1 operator--(void) { data--; return *this; }
   inline cfp16n_1_1 operator--(int) { cui16 temp = data--; return (cfp16n_1_1 &)temp; }

   inline cbool operator==(csi32 &value) const { return data == data; }
   inline cbool operator!=(csi32 &value) const { return data != data; }
   inline cbool operator>=(csi32 &value) const { return data >= data; }
   inline cbool operator<=(csi32 &value) const { return data <= data; }
   inline cbool operator>(csi32 &value) const { return data > data; }
   inline cbool operator<(csi32 &value) const { return data < data; }
   inline cbool operator||(csi32 &value) const { return data || data; }
   inline cbool operator&&(csi32 &value) const { return data && data; }

   inline cfp16n_1_1 operator<<(csi32 &value) const { return (cfp16n_1_1 &)(data << value); }
   inline cfp16n_1_1 operator>>(csi32 &value) const { return (cfp16n_1_1 &)(data >> value); }
   inline cfp16n_1_1 operator<<=(csi32 &value) { return (cfp16n_1_1 &)(data <<= value); }
   inline cfp16n_1_1 operator>>=(csi32 &value) { return (cfp16n_1_1 &)(data >>= value); }

   inline cbool operator==(cfp16n_1_1 &value) const { return data == value.data; }
   inline cbool operator!=(cfp16n_1_1 &value) const { return data != value.data; }
   inline cbool operator>=(cfp16n_1_1 &value) const { return data >= value.data; }
   inline cbool operator<=(cfp16n_1_1 &value) const { return data <= value.data; }
   inline cbool operator>(cfp16n_1_1 &value) const { return data > value.data; }
   inline cbool operator<(cfp16n_1_1 &value) const { return data < value.data; }
   inline cbool operator||(cfp16n_1_1 &value) const { return data || value.data; }
   inline cbool operator&&(cfp16n_1_1 &value) const { return data && value.data; }

   inline cfp16n_1_1 operator+(cfp16n_1_1 &value) const { return (cfp16n_1_1 &)(data + value.data - 32767); }
   inline cfp16n_1_1 operator-(cfp16n_1_1 &value) const { return (cfp16n_1_1 &)(data - value.data - 32767); }
   inline cfp16n_1_1 operator*(cfp16n_1_1 &value) const { return toFixed(toFloat() * toFloat(value)); }
   inline cfp16n_1_1 operator/(cfp16n_1_1 &value) const { return toFixed(toFloat() / toFloat(value)); }
   inline cfp16n_1_1 operator%(cfp16n_1_1 &value) const { return (cfp16n_1_1 &)(data % value.data); }
   inline cfp16n_1_1 operator&(cfp16n_1_1 &value) const { return (cfp16n_1_1 &)(data & value.data); }
   inline cfp16n_1_1 operator|(cfp16n_1_1 &value) const { return (cfp16n_1_1 &)(data | value.data); }
   inline cfp16n_1_1 operator^(cfp16n_1_1 &value) const { return (cfp16n_1_1 &)(data ^ value.data); }
   inline cfp16n_1_1 operator+=(cfp16n_1_1 &value) { return (cfp16n_1_1 &)(data += value.data - 32767); }
   inline cfp16n_1_1 operator-=(cfp16n_1_1 &value) { return (cfp16n_1_1 &)(data -= value.data - 32767); }
   inline cfp16n_1_1 operator*=(cfp16n_1_1 &value) { return data = toFixed(toFloat() * toFloat(value)); }
   inline cfp16n_1_1 operator/=(cfp16n_1_1 &value) { return data = toFixed(toFloat() / toFloat(value)); }
   inline cfp16n_1_1 operator%=(cfp16n_1_1 &value) { return (cfp16n_1_1 &)(data %= value.data); }
   inline cfp16n_1_1 operator&=(cfp16n_1_1 &value) { return (cfp16n_1_1 &)(data &= value.data); }
   inline cfp16n_1_1 operator|=(cfp16n_1_1 &value) { return (cfp16n_1_1 &)(data |= value.data); }
   inline cfp16n_1_1 operator^=(cfp16n_1_1 &value) { return (cfp16n_1_1 &)(data ^= value.data); }

   inline cfl32 operator+(cfl32 &value) const { return toFloat() + value; }
   inline cfl32 operator-(cfl32 &value) const { return toFloat() - value; }
   inline cfl32 operator*(cfl32 &value) const { return toFloat() * value; }
   inline cfl32 operator/(cfl32 &value) const { return toFloat() / value; }
   inline cfp16n_1_1 operator%(cfl32 &value) const { return (cfp16n_1_1 &)(data % toFixed(value)); }
   inline cfp16n_1_1 operator&(cfl32 &value) const { return (cfp16n_1_1 &)(data & toFixed(value)); }
   inline cfp16n_1_1 operator|(cfl32 &value) const { return (cfp16n_1_1 &)(data | toFixed(value)); }
   inline cfp16n_1_1 operator^(cfl32 &value) const { return (cfp16n_1_1 &)(data ^ toFixed(value)); }
   inline cfl32 operator+=(cfl32 &value) { cfl32 temp = toFloat() + value; *this = toFixed(temp); return temp; }
   inline cfl32 operator-=(cfl32 &value) { cfl32 temp = toFloat() - value; *this = toFixed(temp); return temp; }
   inline cfl32 operator*=(cfl32 &value) { cfl32 temp = toFloat() * value; *this = toFixed(temp); return temp; }
   inline cfl32 operator/=(cfl32 &value) { cfl32 temp = toFloat() / value; *this = toFixed(temp); return temp; }
   inline cfp16n_1_1 operator%=(cfl32 &value) { return (cfp16n_1_1 &)(data %= toFixed(value)); }
   inline cfp16n_1_1 operator&=(cfl32 &value) { return (cfp16n_1_1 &)(data &= toFixed(value)); }
   inline cfp16n_1_1 operator|=(cfl32 &value) { return (cfp16n_1_1 &)(data |= toFixed(value)); }
   inline cfp16n_1_1 operator^=(cfl32 &value) { return (cfp16n_1_1 &)(data ^= toFixed(value)); }

   inline cbool operator==(cfl32 &value) const { return data == toFixed(value); }
   inline cbool operator!=(cfl32 &value) const { return data != toFixed(value); }
   inline cbool operator>=(cfl32 &value) const { return data <= toFixed(value); }
   inline cbool operator<=(cfl32 &value) const { return data >= toFixed(value); }
   inline cbool operator>(cfl32 &value) const { return data < toFixed(value); }
   inline cbool operator<(cfl32 &value) const { return data > toFixed(value); }
   inline cbool operator||(cfl32 &value) const { return data || toFixed(value); }
   inline cbool operator&&(cfl32 &value) const { return data && toFixed(value); }
};

// Normalised 16-bit : Decimal range of -128.0~128.0
struct fp16n_128_128 {
   typedef const fp16n_128_128 cfp16n_128_128;

   ui16 data;

   inline cui16 toFixed(cfl32 &value) const { return ui16(value * _fpdt_65535div256f); }
   inline cfl32 toFloat(void) const { return cfl32(data) * _fpdt_256div65535f; }
   inline cfl32 toFloat(cfp16n_128_128 &value) const { return fl32(value.data) * _fpdt_256div65535f; }

   fp16n_128_128(void) = default;
   fp16n_128_128(const fp8n value) { data = toFixed(fl32(value) * _fpdt_data_.rangeDivMax8 + _fpdt_data_.origin8); }
   fp16n_128_128(const fp16n value) { data = toFixed(fl32(value) * _fpdt_data_.rangeDivMax16 + _fpdt_data_.origin16); }
#ifdef _24BIT_INTEGERS_
   fp16n_128_128(const fp24n value) { data = toFixed(fl32(value) * _fpdt_data_.rangeDivMax24 + _fpdt_data_.origin24); }
#endif
   fp16n_128_128(const fp32n value) { data = toFixed(fl32(fl64(value) * _fpdt_data_.rangeDivMax32 + _fpdt_data_.origin32)); }
   fp16n_128_128(cui16 value) { data = value; }
   fp16n_128_128(csi32 value) { data = (ui16 &)value; }
   fp16n_128_128(csi64 value) { data = (ui16 &)value; }
   fp16n_128_128(cfl32 value) { data = toFixed(value); }

   operator ptr(void) const { return *this; }
   operator cfl32(void) const { return cfl32(data) * _fpdt_256div65535f; }

   inline cfp16n_128_128 &operator&(void) const { return *this; }
   inline cfp16n_128_128 operator~(void) const { return (cfp16n_128_128 &)~data; }
   inline cfp16n_128_128 operator-(void) const { return (cfp16n_128_128 &)~data; }
   inline cbool operator!(void) const { return data == 0; }

   inline cfp16n_128_128 operator++(void) { data++; return *this; }
   inline cfp16n_128_128 operator++(int) { cui16 temp = data++; return (cfp16n_128_128 &)temp; }
   inline cfp16n_128_128 operator--(void) { data--; return *this; }
   inline cfp16n_128_128 operator--(int) { cui16 temp = data--; return (cfp16n_128_128 &)temp; }

   inline cbool operator==(csi32 &value) const { return data == data; }
   inline cbool operator!=(csi32 &value) const { return data != data; }
   inline cbool operator>=(csi32 &value) const { return data >= data; }
   inline cbool operator<=(csi32 &value) const { return data <= data; }
   inline cbool operator>(csi32 &value) const { return data > data; }
   inline cbool operator<(csi32 &value) const { return data < data; }
   inline cbool operator||(csi32 &value) const { return data || data; }
   inline cbool operator&&(csi32 &value) const { return data && data; }

   inline cfp16n_128_128 operator<<(csi32 &value) const { return (cfp16n_128_128 &)(data << value); }
   inline cfp16n_128_128 operator>>(csi32 &value) const { return (cfp16n_128_128 &)(data >> value); }
   inline cfp16n_128_128 operator<<=(csi32 &value) { return (cfp16n_128_128 &)(data <<= value); }
   inline cfp16n_128_128 operator>>=(csi32 &value) { return (cfp16n_128_128 &)(data >>= value); }

   inline cbool operator==(cfp16n_128_128 &value) const { return data == value.data; }
   inline cbool operator!=(cfp16n_128_128 &value) const { return data != value.data; }
   inline cbool operator>=(cfp16n_128_128 &value) const { return data >= value.data; }
   inline cbool operator<=(cfp16n_128_128 &value) const { return data <= value.data; }
   inline cbool operator>(cfp16n_128_128 &value) const { return data > value.data; }
   inline cbool operator<(cfp16n_128_128 &value) const { return data < value.data; }
   inline cbool operator||(cfp16n_128_128 &value) const { return data || value.data; }
   inline cbool operator&&(cfp16n_128_128 &value) const { return data && value.data; }

   inline cfp16n_128_128 operator+(cfp16n_128_128 &value) const { return (cfp16n_128_128 &)(data + value.data - 32767); }
   inline cfp16n_128_128 operator-(cfp16n_128_128 &value) const { return (cfp16n_128_128 &)(data - value.data - 32767); }
   inline cfp16n_128_128 operator*(cfp16n_128_128 &value) const { return toFixed(toFloat() * toFloat(value)); }
   inline cfp16n_128_128 operator/(cfp16n_128_128 &value) const { return toFixed(toFloat() / toFloat(value)); }
   inline cfp16n_128_128 operator%(cfp16n_128_128 &value) const { return (cfp16n_128_128 &)(data % value.data); }
   inline cfp16n_128_128 operator&(cfp16n_128_128 &value) const { return (cfp16n_128_128 &)(data & value.data); }
   inline cfp16n_128_128 operator|(cfp16n_128_128 &value) const { return (cfp16n_128_128 &)(data | value.data); }
   inline cfp16n_128_128 operator^(cfp16n_128_128 &value) const { return (cfp16n_128_128 &)(data ^ value.data); }
   inline cfp16n_128_128 operator+=(cfp16n_128_128 &value) { return (cfp16n_128_128 &)(data += value.data - 32767); }
   inline cfp16n_128_128 operator-=(cfp16n_128_128 &value) { return (cfp16n_128_128 &)(data -= value.data - 32767); }
   inline cfp16n_128_128 operator*=(cfp16n_128_128 &value) { return data = toFixed(toFloat() * toFloat(value)); }
   inline cfp16n_128_128 operator/=(cfp16n_128_128 &value) { return data = toFixed(toFloat() / toFloat(value)); }
   inline cfp16n_128_128 operator%=(cfp16n_128_128 &value) { return (cfp16n_128_128 &)(data %= value.data); }
   inline cfp16n_128_128 operator&=(cfp16n_128_128 &value) { return (cfp16n_128_128 &)(data &= value.data); }
   inline cfp16n_128_128 operator|=(cfp16n_128_128 &value) { return (cfp16n_128_128 &)(data |= value.data); }
   inline cfp16n_128_128 operator^=(cfp16n_128_128 &value) { return (cfp16n_128_128 &)(data ^= value.data); }

   inline cfl32 operator+(cfl32 &value) const { return toFloat() + value; }
   inline cfl32 operator-(cfl32 &value) const { return toFloat() - value; }
   inline cfl32 operator*(cfl32 &value) const { return toFloat() * value; }
   inline cfl32 operator/(cfl32 &value) const { return toFloat() / value; }
   inline cfp16n_128_128 operator%(cfl32 &value) const { return (cfp16n_128_128 &)(data % toFixed(value)); }
   inline cfp16n_128_128 operator&(cfl32 &value) const { return (cfp16n_128_128 &)(data & toFixed(value)); }
   inline cfp16n_128_128 operator|(cfl32 &value) const { return (cfp16n_128_128 &)(data | toFixed(value)); }
   inline cfp16n_128_128 operator^(cfl32 &value) const { return (cfp16n_128_128 &)(data ^ toFixed(value)); }
   inline cfl32 operator+=(cfl32 &value) { cfl32 temp = toFloat() + value; *this = toFixed(temp); return temp; }
   inline cfl32 operator-=(cfl32 &value) { cfl32 temp = toFloat() - value; *this = toFixed(temp); return temp; }
   inline cfl32 operator*=(cfl32 &value) { cfl32 temp = toFloat() * value; *this = toFixed(temp); return temp; }
   inline cfl32 operator/=(cfl32 &value) { cfl32 temp = toFloat() / value; *this = toFixed(temp); return temp; }
   inline cfp16n_128_128 operator%=(cfl32 &value) { return (cfp16n_128_128 &)(data %= toFixed(value)); }
   inline cfp16n_128_128 operator&=(cfl32 &value) { return (cfp16n_128_128 &)(data &= toFixed(value)); }
   inline cfp16n_128_128 operator|=(cfl32 &value) { return (cfp16n_128_128 &)(data |= toFixed(value)); }
   inline cfp16n_128_128 operator^=(cfl32 &value) { return (cfp16n_128_128 &)(data ^= toFixed(value)); }

   inline cbool operator==(cfl32 &value) const { return data == toFixed(value); }
   inline cbool operator!=(cfl32 &value) const { return data != toFixed(value); }
   inline cbool operator>=(cfl32 &value) const { return data <= toFixed(value); }
   inline cbool operator<=(cfl32 &value) const { return data >= toFixed(value); }
   inline cbool operator>(cfl32 &value) const { return data < toFixed(value); }
   inline cbool operator<(cfl32 &value) const { return data > toFixed(value); }
   inline cbool operator||(cfl32 &value) const { return data || toFixed(value); }
   inline cbool operator&&(cfl32 &value) const { return data && toFixed(value); }
};

// 3x 16-bit, signed 7.8 : Decimal range of -128.0~127.99609375
struct fs7p8x3 {
   typedef const fs7p8 cfs7p8; typedef const fs7p8x3 cfs7p8x3;

   union { fs7p8 data[3]; ui16 data16[3]; struct { fs7p8 r, g, b; }; i16x3 data48; };

   inline cui16 toFixed(cfl32 &value) const { return ui16((value + 128.0f) * 256.0f); }
   inline ci16x3 toFixed3(cfl32x4 &value) const { cui64 data64 = (ui64 &)_mm_shuffle_epi8(_mm_cvttps_epi32(_mm_mul_ps(_mm_add_ps(value, _fpdt_128fx4), _fpdt_256fx4)), _fpdt_shuffle16s); return *(i16x3 *)&data64; }
   inline cfl32 toFloat(cui8 index) const { return cfl32(data16[index]) * _fpdt_rcp256f - 128.0f; }
   inline cfl32 toFloat(cfs7p8 &value) const { return fl32(value.data) * _fpdt_rcp256f - 128.0f; }
   inline cfl32x4 toFloat3(void) const { return _mm_sub_ps(_mm_mul_ps(_mm_cvtepu32_ps(_mm_cvtepu8_epi32(_mm_cvtsi64_si128((si64 &)*this))), _fpdt_rcp256fx4), _fpdt_128fx4); } // 4th element is undefined
   inline cfl32x4 toFloat3(cfs7p8x3 value) const { return _mm_sub_ps(_mm_mul_ps(_mm_cvtepu32_ps(_mm_cvtepu8_epi32(_mm_cvtsi64_si128((si64 &)value))), _fpdt_rcp256fx4), _fpdt_128fx4); } // 4th element is undefined

   fs7p8x3(void) = default;
   fs7p8x3(cfs7p8 value, cui16 index) { data[index] = value; }
   fs7p8x3(cui16 value, cui16 index) { data16[index] = value; }
   fs7p8x3(cfl32 value, cui16 index) { data16[index] = toFixed(value); }
   fs7p8x3(cfs7p8 (&value)[3]) { data[0] = value[0]; data[1] = value[1]; data[2] = value[2]; }
   fs7p8x3(cui16 (&value)[3]) { data16[0] = value[0]; data16[1] = value[1]; data16[2] = value[2]; }
   fs7p8x3(cfs7p8 value) { data[0] = data[1] = data[2] = value; }
   fs7p8x3(cui16 value) { data16[0] = data16[1] = data16[2] = value; }
   fs7p8x3(cui16 value0, cui16 value1, cui16 value2) { data16[0] = value0; data16[1] = value1; data16[2] = value2; }
   fs7p8x3(cfl32 value) { cui16 temp = toFixed(value); data16[0] = data16[1] = data16[2] = temp; }
   fs7p8x3(cfl32x4 value) { data48 = toFixed3(value); } // 4th element is ignored
#ifdef _VECTOR_STRUCTURES_
   fs7p8x3(cVEC3Df value) { cSSE4Df32 temp = { .vector = (VEC4Df &)value }; data48 = toFixed3(temp.xmm); }
   fs7p8x3(cSSE4Df32 value) { data48 = toFixed3(value.xmm); } // 4th element is ignored
#endif
   operator ptr(void) const { return *this; }
   operator cfl32x4(void) const { return toFloat3(); } // 4th element is undefined

   inline cfs7p8x3 &operator&(void) const { return *this; }
   inline cfs7p8x3 &operator&(cui16 (&value)[3]) const { return (cfs7p8x3 &)value; }

   inline cfs7p8x3 operator++(void) { data16[0]++; data16[1]++; data16[2]++; return *this; }
   inline cfs7p8x3 operator++(int) { cui16 temp[3] = { data16[0]++, data16[1]++, data16[2]++ }; return (cfs7p8x3 &)temp; }
   inline cfs7p8x3 operator--(void) { data16[0]--; data16[1]--; data16[2]--; return *this; }
   inline cfs7p8x3 operator--(int) { cui16 temp[4] = { data16[0]--, data16[1]--, data16[2]-- }; return (cfs7p8x3 &)temp; }

   inline cfs7p8x3 operator<<(csi32 &value) const { return cfs7p8x3{ ui16(data16[0] << value), ui16(data16[1] << value), ui16(data16[2] << value) }; }
   inline cfs7p8x3 operator>>(csi32 &value) const { return cfs7p8x3{ ui16(data16[0] >> value), ui16(data16[1] >> value), ui16(data16[2] >> value) }; }
   inline cfs7p8x3 operator<<=(csi32 &value) { return cfs7p8x3{ data16[0] <<= value, data16[1] <<= value, data16[2] <<= value }; }
   inline cfs7p8x3 operator>>=(csi32 &value) { return cfs7p8x3{ data16[0] >>= value, data16[1] >>= value, data16[2] >>= value }; }

   inline cfs7p8x3 operator+(cfs7p8x3 &value) const { return cfs7p8x3{ ui16(data16[0] + value.data16[0]), ui16(data16[1] + value.data16[1]), ui16(data16[2] + value.data16[2]) }; }
   inline cfs7p8x3 operator-(cfs7p8x3 &value) const { return cfs7p8x3{ ui16(data16[0] - value.data16[0]), ui16(data16[1] - value.data16[1]), ui16(data16[2] - value.data16[2]) }; }
   inline cfs7p8x3 operator+=(cfs7p8x3 &value) { return cfs7p8x3{ data16[0] += value.data16[0], data16[1] += value.data16[1], data16[2] += value.data16[2]}; }
   inline cfs7p8x3 operator-=(cfs7p8x3 &value) { return cfs7p8x3{ data16[0] -= value.data16[0], data16[1] -= value.data16[1], data16[2] -= value.data16[2]}; }

   inline cfs7p8x3 operator+(cfl32x4 &value) const { ci16x3 temp = toFixed3(_mm_add_ps(value, toFloat3())); return *(cfs7p8x3 *)&temp; }
   inline cfs7p8x3 operator-(cfl32x4 &value) const { ci16x3 temp = toFixed3(_mm_sub_ps(value, toFloat3())); return *(cfs7p8x3 *)&temp; }
   inline cfs7p8x3 operator+=(cfl32x4 &value) { data48 = toFixed3(_mm_add_ps(value, toFloat3())); return *this; }
   inline cfs7p8x3 operator-=(cfl32x4 &value) { data48 = toFixed3(_mm_sub_ps(value, toFloat3())); return *this; }
};

// 4x 16-bit, 1.15 : Decimal range of 0.0~1.999969482421875
struct f1p15x4 {
   typedef const f1p15 cf1p15; typedef const f1p15x4 cf1p15x4;

   union { ui64 data64; f1p15 data[4]; ui16 data16[4]; };

   f1p15x4(void) = default;
   f1p15x4(cf1p15 value, cui16 index) { data[index] = value; }
   f1p15x4(cui16 value, cui16 index) { data16[index] = value; }
   f1p15x4(cfl32 value, cui16 index) { data16[index] = ui16(value * 32768.0f); }
   f1p15x4(cf1p15 (&value)[4]) { data64 = (ui64 &)value; }
   f1p15x4(cui16 (&value)[4]) { data64 = (ui64 &)value; }
   f1p15x4(cf1p15 value) { data[0] = data[1] = data[2] = data[3] = value; }
   f1p15x4(cui16 value) { data16[0] = data16[1] = data16[2] = data16[3] = value; }
   f1p15x4(cui64 value) { data64 = value; }
   f1p15x4(cui16 value0, cui16 value1, cui16 value2, cui16 value3) { data16[0] = value0; data16[1] = value1; data16[2] = value2; data16[3] = value3; }
   f1p15x4(cfl32 value) { cui16 temp = ui16(value * 65535.0f); data16[0] = data16[1] = data16[2] = data16[3] = temp; }
   f1p15x4(cfl32x4 value) { data64 = (ui64 &)_mm_shuffle_epi8(_mm_cvttps_epi32(_mm_mul_ps(value, _fpdt_32768fx4)), _fpdt_shuffle16s); }
#ifdef _VECTOR_STRUCTURES_
   f1p15x4(cVEC4Df value) { cSSE4Df32 temp = { .vector = value }; data64 = (ui64 &)_mm_shuffle_epi8(_mm_cvttps_epi32(_mm_mul_ps(temp.xmm, _fpdt_32768fx4)), _fpdt_shuffle16s); }
   f1p15x4(cSSE4Df32 value) { data64 = (ui64 &)_mm_shuffle_epi8(_mm_cvttps_epi32(_mm_mul_ps(value.xmm, _fpdt_32768fx4)), _fpdt_shuffle16s); }
#endif
   operator ptr(void) const { return *this; }
   operator cfl32x4(void) const { return _mm_mul_ps(_mm_cvtepu32_ps(_mm_cvtepu8_epi32(_mm_cvtsi64_si128(data64))), _fpdt_rcp255fx4); }

   inline cf1p15x4 &operator&(void) const { return *this; }
   inline cf1p15x4 &operator&(cui64 &value) const { return (cf1p15x4 &)value; }

   inline cf1p15x4 operator++(void) { data16[0]++; data16[1]++; data16[2]++; data16[3]++; return *this; }
   inline cf1p15x4 operator++(int) { al8 cui16 temp[4] = { data16[0]++, data16[1]++, data16[2]++, data16[3]++ }; return (cf1p15x4 &)temp; }
   inline cf1p15x4 operator--(void) { data16[0]--; data16[1]--; data16[2]--; data16[3]--; return *this; }
   inline cf1p15x4 operator--(int) { al8 cui16 temp[4] = { data16[0]--, data16[1]--, data16[2]--, data16[3]-- }; return (cf1p15x4 &)temp; }

   inline cf1p15x4 operator<<(csi32 &value) const { return cf1p15x4{ ui16(data16[0] << value), ui16(data16[1] << value), ui16(data16[2] << value), ui16(data16[3] << value) }; }
   inline cf1p15x4 operator>>(csi32 &value) const { return cf1p15x4{ ui16(data16[0] >> value), ui16(data16[1] >> value), ui16(data16[2] >> value), ui16(data16[3] >> value) }; }
   inline cf1p15x4 operator<<=(csi32 &value) { return cf1p15x4{ data16[0] <<= value, data16[1] <<= value, data16[2] <<= value, data16[3] <<= value }; }
   inline cf1p15x4 operator>>=(csi32 &value) { return cf1p15x4{ data16[0] >>= value, data16[1] >>= value, data16[2] >>= value, data16[3] >>= value }; }

   inline cf1p15x4 operator+(cf1p15x4 &value) const { return (cf1p15x4 &)_mm_add_epi16(_mm_cvtsi64_si128(data64), _mm_cvtsi64_si128(value.data64)); }
   inline cf1p15x4 operator-(cf1p15x4 &value) const { return (cf1p15x4 &)_mm_sub_epi16(_mm_cvtsi64_si128(data64), _mm_cvtsi64_si128(value.data64)); }
   inline cf1p15x4 operator+=(cf1p15x4 &value) { data64 = (ui64 &)_mm_add_epi16(_mm_cvtsi64_si128(data64), _mm_cvtsi64_si128(value.data64)); return *this;  }
   inline cf1p15x4 operator-=(cf1p15x4 &value) { data64 = (ui64 &)_mm_sub_epi16(_mm_cvtsi64_si128(data64), _mm_cvtsi64_si128(value.data64)); return *this;  }

   inline cf1p15x4 operator+(cfl32x4 &value) const { return data64 + (ui64 &)_mm_shuffle_epi8(_mm_cvttps_epi32(_mm_mul_ps(value, _fpdt_32768fx4)), _fpdt_shuffle16s); }
   inline cf1p15x4 operator-(cfl32x4 &value) const { return data64 - (ui64 &)_mm_shuffle_epi8(_mm_cvttps_epi32(_mm_mul_ps(value, _fpdt_32768fx4)), _fpdt_shuffle16s); }
   inline cf1p15x4 operator+=(cfl32x4 &value) { return (data64 += (ui64 &)_mm_shuffle_epi8(_mm_cvttps_epi32(_mm_mul_ps(value, _fpdt_32768fx4)), _fpdt_shuffle16s)); }
   inline cf1p15x4 operator-=(cfl32x4 &value) { return (data64 -= (ui64 &)_mm_shuffle_epi8(_mm_cvttps_epi32(_mm_mul_ps(value, _fpdt_32768fx4)), _fpdt_shuffle16s)); }
};

// 4x normalised 16-bit : Decimal ranges of 0.0~1.0
struct fp16n0_1x4 {
   typedef const fp16n0_1 cfp16n0_1; typedef const fp16n0_1x4 cfp16n0_1x4;

   union { ui64 data64; fp16n0_1 data[4]; ui16 data16[4]; };

   fp16n0_1x4(void) = default;
   fp16n0_1x4(fp16n0_1 value, cui16 index) { data[index] = value; }
   fp16n0_1x4(cui8 value, cui16 index) { data16[index] = value; }
   fp16n0_1x4(cfl32 value, cui16 index) { data16[index] = ui16(value * 65535.0f); }
   fp16n0_1x4(fp16n0_1 (&value)[4]) { data64 = (ui64 &)value; }
   fp16n0_1x4(cui16 (&value)[4]) { data64 = (ui64 &)value; }
   fp16n0_1x4(cfp16n0_1 value) { data[0] = data[1] = data[2] = data[3] = value; }
   fp16n0_1x4(cui16 value) { data16[0] = data16[1] = data16[2] = data16[3] = value; }
   fp16n0_1x4(cui64 value) { data64 = value; }
   fp16n0_1x4(cui16 value0, cui16 value1, cui16 value2, cui16 value3) { data16[0] = value0; data16[1] = value1; data16[2] = value2; data16[3] = value3; }
   fp16n0_1x4(cfl32 value) { cui16 temp = ui16(value * 65535.0f); data16[0] = data16[1] = data16[2] = data16[3] = temp; }
   fp16n0_1x4(cfl32x4 value) { data64 = (ui64 &)_mm_shuffle_epi8(_mm_cvttps_epi32(_mm_mul_ps(value, _fpdt_65535fx4)), _fpdt_shuffle16s); }
#ifdef _VECTOR_STRUCTURES_
   fp16n0_1x4(cVEC4Df value) { cSSE4Df32 temp = { .vector = value }; data64 = (ui64 &)_mm_shuffle_epi8(_mm_cvttps_epi32(_mm_mul_ps(temp.xmm, _fpdt_65535fx4)), _fpdt_shuffle16s); }
   fp16n0_1x4(cSSE4Df32 value) { data64 = (ui64 &)_mm_shuffle_epi8(_mm_cvttps_epi32(_mm_mul_ps(value.xmm, _fpdt_65535fx4)), _fpdt_shuffle16s); }
#endif

   operator cui64(void) const { return data64; }
   operator cfl32x4(void) const { return _mm_mul_ps(_mm_cvtepu32_ps(_mm_cvtepu8_epi32(_mm_cvtsi64_si128(data64))), _fpdt_rcp255fx4); }

   inline cfp16n0_1x4 operator++(void) { data16[0]++; data16[1]++; data16[2]++; data16[3]++; return *this; }
   inline cfp16n0_1x4 operator++(int) { al8 cui16 temp[4] = { data16[0]++, data16[1]++, data16[2]++, data16[3]++ }; return (cfp16n0_1x4 &)temp; }
   inline cfp16n0_1x4 operator--(void) { data16[0]--; data16[1]--; data16[2]--; data16[3]--; return *this; }
   inline cfp16n0_1x4 operator--(int) { al8 cui16 temp[4] = { data16[0]--, data16[1]--, data16[2]--, data16[3]-- }; return (cfp16n0_1x4 &)temp; }

   inline cfp16n0_1x4 operator<<(csi32 &value) const { return cfp16n0_1x4{ ui16(data16[0] << value), ui16(data16[1] << value), ui16(data16[2] << value), ui16(data16[3] << value) }; }
   inline cfp16n0_1x4 operator>>(csi32 &value) const { return cfp16n0_1x4{ ui16(data16[0] >> value), ui16(data16[1] >> value), ui16(data16[2] >> value), ui16(data16[3] >> value) }; }
   inline cfp16n0_1x4 operator<<=(csi32 &value) { return cfp16n0_1x4{ ui16(data16[0] <<= value), ui16(data16[1] <<= value), ui16(data16[2] <<= value), ui16(data16[3] <<= value) }; }
   inline cfp16n0_1x4 operator>>=(csi32 &value) { return cfp16n0_1x4{ ui16(data16[0] >>= value), ui16(data16[1] >>= value), ui16(data16[2] >>= value), ui16(data16[3] >>= value) }; }

   inline cfp16n0_1x4 operator+(cfp16n0_1x4 &value) const { return (cfp16n0_1x4 &)_mm_add_epi16(_mm_cvtsi64_si128(data64), _mm_cvtsi64_si128(value.data64)); }
   inline cfp16n0_1x4 operator-(cfp16n0_1x4 &value) const { return (cfp16n0_1x4 &)_mm_add_epi16(_mm_cvtsi64_si128(data64), _mm_cvtsi64_si128(value.data64)); }
   inline cfp16n0_1x4 operator+(cfl32x4 &value) const { return data64 + (ui64 &)_mm_shuffle_epi8(_mm_cvttps_epi32(_mm_mul_ps(value, _fpdt_65535fx4)), _fpdt_shuffle16s); }
   inline cfp16n0_1x4 operator-(cfl32x4 &value) const { return data64 - (ui64 &)_mm_shuffle_epi8(_mm_cvttps_epi32(_mm_mul_ps(value, _fpdt_65535fx4)), _fpdt_shuffle16s); }

   inline cfp16n0_1x4 operator+=(cfp16n0_1x4 &value) { data64 = (ui64 &)_mm_add_epi16(_mm_cvtsi64_si128(data64), _mm_cvtsi64_si128(value.data64)); return *this; }
   inline cfp16n0_1x4 operator-=(cfp16n0_1x4 &value) { data64 = (ui64 &)_mm_sub_epi16(_mm_cvtsi64_si128(data64), _mm_cvtsi64_si128(value.data64)); return *this; }
   inline cfp16n0_1x4 operator+=(cfl32x4 &value) { return (data64 += (ui64 &)_mm_shuffle_epi8(_mm_cvttps_epi32(_mm_mul_ps(value, _fpdt_65535fx4)), _fpdt_shuffle16s)); }
   inline cfp16n0_1x4 operator-=(cfl32x4 &value) { return (data64 -= (ui64 &)_mm_shuffle_epi8(_mm_cvttps_epi32(_mm_mul_ps(value, _fpdt_65535fx4)), _fpdt_shuffle16s)); }
};

// 16x normalised 16-bit : Decimal ranges of 0.0~3.0
struct fp16n0_3x16 {
   typedef const fp16n0_3 cfp16n0_3; typedef const fp16n0_3x16 cfp16n0_3x16;
   typedef fp16n0_3 (& fp16n0_3_16)[16]; typedef const fp16n0_3_16 cfp16n0_3_16;
   
   union { ui256 data256; ui128 data128[2]; ui64 data64[4]; fp16n0_3 data[16]; ui16 data16[16]; i16x16 data16x16; };

   inline cui16 toFixed(cfl32 &value) const { return ui16(value * 21845.0f); }
   inline cfl32 toFloat(csi32 &index) const { return fl32(data16[index]) * _fpdt_3div65535f; }
   inline cfl32 toFloat(cfp16n0_3 &value) const { return fl32(value.data) * _fpdt_3div65535f; }
   inline ci16x16 toFixed16(cf32x16 &value) const {
#ifdef _FPDT_AVX512_
      return (ui256 &)_mm512_shuffle_epi8(_mm512_cvttps_epi32(_mm512_mul_ps(value.zmm, _fpdt_65535div2fx16)), _fpdt_shuffle16sAVX512);
#else
#ifdef _FPDT_AVX2_
      ci32x16 precalc = { .ymm = { _mm256_cvttps_epi32(_mm256_mul_ps(value.ymm[0], _fpdt_65535div3fx8)), _mm256_cvttps_epi32(_mm256_mul_ps(value.ymm[1], _fpdt_65535div3fx8)) } };
      ci64x4 result = { ._ui64 = {
         (ui64 &)_mm_shuffle_epi8(precalc.xmm[0], _fpdt_shuffle16sAVX0),
         (ui64 &)_mm_shuffle_epi8(precalc.xmm[1], _fpdt_shuffle16sAVX1),
         (ui64 &)_mm_shuffle_epi8(precalc.xmm[2], _fpdt_shuffle16sAVX0),
         (ui64 &)_mm_shuffle_epi8(precalc.xmm[3], _fpdt_shuffle16sAVX1)
      } };
      return (ci16x16 &)result;
#else
      ci16x16 temp = { .m256i_u64 = {
         (ui64 &)_mm_shuffle_epi8(_mm_cvttps_epi32(_mm_mul_ps(value.xmm[0], _fpdt_65535div2fx4)), _fpdt_shuffle16s),
         (ui64 &)_mm_shuffle_epi8(_mm_cvttps_epi32(_mm_mul_ps(value.xmm[1], _fpdt_65535div2fx4)), _fpdt_shuffle16s),
         (ui64 &)_mm_shuffle_epi8(_mm_cvttps_epi32(_mm_mul_ps(value.xmm[2], _fpdt_65535div2fx4)), _fpdt_shuffle16s),
         (ui64 &)_mm_shuffle_epi8(_mm_cvttps_epi32(_mm_mul_ps(value.xmm[3], _fpdt_65535div2fx4)), _fpdt_shuffle16s)
      } };
      return temp;
#endif
#endif
   }
   inline cf32x16 toFloat16(void) const {
#ifdef _FPDT_AVX512_
      return (cf32x16 &)_mm512_mul_ps(_mm512_cvtepu32_ps(_mm512_cvtepu16_epi32(data256)), _fpdt_2div65535fx16);
#else
#ifdef _FPDT_AVX2_
      cf32x16 temp = { .ymm = {
         _mm256_mul_ps(_mm256_cvtepi32_ps(_mm256_cvtepu16_epi32(data128[0])), _fpdt_3div65535fx8),
         _mm256_mul_ps(_mm256_cvtepi32_ps(_mm256_cvtepu16_epi32(data128[1])), _fpdt_3div65535fx8)
      } };
#else
      cf32x16 temp = { .xmm = {
         _mm_mul_ps(_mm_cvtepu32_ps(_mm_cvtepu16_epi32(data128[0])), _fpdt_2div65535fx4),
         _mm_mul_ps(_mm_cvtepu32_ps(_mm_cvtepu16_epi32(ui128(data64[1]))), _fpdt_2div65535fx4),
         _mm_mul_ps(_mm_cvtepu32_ps(_mm_cvtepu16_epi32(data128[1])), _fpdt_2div65535fx4),
         _mm_mul_ps(_mm_cvtepu32_ps(_mm_cvtepu16_epi32(ui128(data64[3]))), _fpdt_2div65535fx4)
      } };
#endif
      return temp;
#endif
   }

   fp16n0_3x16(void) = default;
   fp16n0_3x16(cfp16n0_3 value, cui8 index) { data[index] = value; }
   fp16n0_3x16(cui8 value, cui8 index) { data16[index] = value; }
   fp16n0_3x16(cfl32 value, cui8 index) { data16[index] = ui16(value * 21845.0f); }
   fp16n0_3x16(cui128 value, cui8 index) { data128[index] = value; }
   fp16n0_3x16(cui128 value) { data128[0] = data128[1] = value; }
   fp16n0_3x16(cui128 (&value)[2]) { data128[0] = value[0]; data128[1] = value[1]; }
   fp16n0_3x16(cf32x16 value) { data16x16 = toFixed16(value); }
#ifndef _FPDT_AVX2_
   fp16n0_3x16(cfp16n0_2_16 value) { data256 = (ui256 &)value; }
   fp16n0_3x16(cui16 (&value)[16]) { data256 = (ui256 &)value; }
   fp16n0_3x16(cfp16n0_2 value) { data256 = _mm256_set1_epi16(value.data); }
   fp16n0_3x16(cui16 value) { data256 = _mm256_set1_epi16(value); }
   fp16n0_3x16(cfl32 value) { cui16 temp = ui16(value * 32767.5f); data256 = _mm256_set1_epi16(temp); }
   fp16n0_3x16(cui256 value) { data256 = value; }
   fp16n0_3x16(cfl32x4 &value) {
#ifdef _FPDT_AVX512_
      cf32x16 temp = { .zmm = _mm512_broadcast_f32x4(value) };
#else
      cf32x8 half = _mm256_broadcast_ps(&value);
      cf32x16 temp = { .ymm = { half, half } };
#endif
      data256 = (cfp16n0_3x16 &)toFixed16(temp);
   }
#else
   fp16n0_3x16(cfp16n0_3_16 value) { data128[0] = (ui128 &)value[0]; data128[1] = ((ui128(&)[2])value)[1]; }
   fp16n0_3x16(cui16 (&value)[16]) { data128[0] = (ui128 &)value[0]; data128[1] = ((ui128(&)[2])value)[1]; }
   fp16n0_3x16(cfp16n0_3 value) { data128[0] = data128[1] = _mm_set1_epi16(value.data); }
   fp16n0_3x16(cui16 value) { data128[0] = data128[1] = _mm_set1_epi16(value); }
   fp16n0_3x16(cfl32 value) { cui16 temp = ui16(value * 21845.0f); data128[0] = data128[1] = _mm_set1_epi16(temp); }
   fp16n0_3x16(cfl32x4 &value) { cf32x16 temp = { .xmm = { value, value, value, value } }; data16x16 = toFixed16(temp); }
#endif

   operator ptr(void) const { return *this; }
   operator cf32x16(void) const { return toFloat16(); }

   inline cfp16n0_3x16 &operator&(void) const { return *this; }

   inline cfp16n0_3 operator[](cui8 index) const { return data[index]; }
   inline cui16 operator[](cui16 index) const { return data16[index]; }
   inline cfl32 operator[](csi32 index) const { return fl32(data16[index]) * _fpdt_3div65535f; }
#ifdef _FPDT_AVX2_
   inline cfp16n0_3x16 operator++(void) { data256 = _mm256_add_epi16(data256, _fpdt_1x16); return *this; }
   inline cfp16n0_3x16 operator++(int) { cui256 temp = data256; data256 = _mm256_add_epi16(data256, _fpdt_1x16); return (cfp16n0_3x16 &)temp; }
   inline cfp16n0_3x16 operator--(void) { data256 = _mm256_sub_epi16(data256, _fpdt_1x16); return *this; }
   inline cfp16n0_3x16 operator--(int) { cui256 temp = data256; data256 = _mm256_sub_epi16(data256, _fpdt_1x16); return (cfp16n0_3x16 &)temp; }

   inline cfp16n0_3x16 operator<<(csi32 &value) const { return (cfp16n0_3x16 &)_mm256_slli_epi16(data256, value); }
   inline cfp16n0_3x16 operator>>(csi32 &value) const { return (cfp16n0_3x16 &)_mm256_srli_epi16(data256, value); }
   inline cfp16n0_3x16 operator<<=(csi32 &value) { return (cfp16n0_3x16 &)(data256 = _mm256_slli_epi16(data256, value)); }
   inline cfp16n0_3x16 operator>>=(csi32 &value) { return (cfp16n0_3x16 &)(data256 = _mm256_srli_epi16(data256, value)); }

   inline cfp16n0_3x16 operator+(cfp16n0_3x16 &value) const { return (cfp16n0_3x16 &)_mm256_add_epi16(data256, value.data256); }
   inline cfp16n0_3x16 operator-(cfp16n0_3x16 &value) const { return (cfp16n0_3x16 &)_mm256_sub_epi16(data256, value.data256); }
   inline cfp16n0_3x16 operator*(cfp16n0_3x16 &value) const {
      cf32x16 temp = toFloat16(), input = value.toFloat16();
      cf32x16 temp2 = {
#ifdef _FPDT_AVX512_
         .zmm = _mm512_mul_ps(temp.zmm, input.zmm)
#else
         .ymm = { _mm256_mul_ps(temp.ymm[0], input.ymm[0]), _mm256_mul_ps(temp.ymm[1], input.ymm[1])}
#endif
      };
      return (cfp16n0_3x16 &)toFixed16(temp2);
   }
   inline cfp16n0_3x16 operator/(cfp16n0_3x16 &value) const {
      cf32x16 temp = toFloat16(), input = value.toFloat16();
      cf32x16 temp2 = {
#ifdef _FPDT_AVX512_
         .zmm = _mm512_div_ps(temp.zmm, input.zmm)
#else
         .ymm = { _mm256_div_ps(temp.ymm[0], input.ymm[0]), _mm256_div_ps(temp.ymm[1], input.ymm[1])}
#endif
      };
      return (cfp16n0_3x16 &)toFixed16(temp2);
   }
   inline cfp16n0_3x16 operator+=(cfp16n0_3x16 &value) { data256 = _mm256_add_epi16(data256, value.data256); return *this; }
   inline cfp16n0_3x16 operator-=(cfp16n0_3x16 &value) { data256 = _mm256_sub_epi16(data256, value.data256); return *this; }
   inline cfp16n0_3x16 operator*=(cfp16n0_3x16 &value) {
      cf32x16 temp = toFloat16(), input = value.toFloat16();
      cf32x16 temp2 = {
#ifdef _FPDT_AVX512_
         .zmm = _mm512_mul_ps(temp.zmm, input.zmm)
#else
         .ymm = { _mm256_mul_ps(temp.ymm[0], input.ymm[0]), _mm256_mul_ps(temp.ymm[1], input.ymm[1])}
#endif
      };
      data16x16 = toFixed16(temp2);
      return *this;
   }
   inline cfp16n0_3x16 operator/(cfp16n0_3x16 &value) {
      cf32x16 temp = toFloat16(), input = value.toFloat16();
      cf32x16 temp2 = {
#ifdef _FPDT_AVX512_
         .zmm = _mm512_div_ps(temp.zmm, input.zmm)
#else
         .ymm = { _mm256_div_ps(temp.ymm[0], input.ymm[0]), _mm256_div_ps(temp.ymm[1], input.ymm[1])}
#endif
      };
      data16x16 = toFixed16(temp2);
      return *this;
   }

   inline cfp16n0_3x16 operator+(cfl32 &value) const { return (cfp16n0_3x16 &)(_mm256_add_epi16(data256, _mm256_set1_epi16(toFixed(value)))); }
   inline cfp16n0_3x16 operator-(cfl32 &value) const { return (cfp16n0_3x16 &)(_mm256_sub_epi16(data256, _mm256_set1_epi16(toFixed(value)))); }
   inline cfp16n0_3x16 operator*(cfl32 &value) const {
      cfl32x8 input = _mm256_set1_ps(value);
      cf32x16 temp = toFloat16();
      cf32x16 temp2 = { .ymm = { _mm256_mul_ps(temp.ymm[0], input), _mm256_mul_ps(temp.ymm[1], input) } };
      return (cfp16n0_3x16 &)toFixed16(temp2);
   }
   inline cfp16n0_3x16 operator/(cfl32 &value) const {
      cfl32x8 input = _mm256_set1_ps(value);
      cf32x16 temp = toFloat16();
      cf32x16 temp2 = { .ymm = { _mm256_div_ps(temp.ymm[0], input), _mm256_div_ps(temp.ymm[1], input) } };
      return (cfp16n0_3x16 &)toFixed16(temp2);
   }
   inline cfp16n0_3x16 operator+=(cfl32 &value) { data256 = _mm256_add_epi16(data256, _mm256_set1_epi16(toFixed(value))); return *this; }
   inline cfp16n0_3x16 operator-=(cfl32 &value) { data256 = _mm256_sub_epi16(data256, _mm256_set1_epi16(toFixed(value))); return *this; }
   inline cfp16n0_3x16 operator*=(cfl32 &value) {
      cfl32x8 input = _mm256_set1_ps(value);
      cf32x16 temp = toFloat16();
      cf32x16 temp2 = { .ymm = { _mm256_mul_ps(temp.ymm[0], input), _mm256_mul_ps(temp.ymm[1], input) } };
      data16x16 = toFixed16(temp2);
      return *this;
   }
   inline cfp16n0_3x16 operator/=(cfl32 &value) {
      cfl32x8 input = _mm256_set1_ps(value);
      cf32x16 temp = toFloat16();
      cf32x16 temp2 = { .ymm = { _mm256_div_ps(temp.ymm[0], input), _mm256_div_ps(temp.ymm[1], input) } };
      data16x16 = toFixed16(temp2);
      return *this;
   }

   inline cfp16n0_3x16 operator+(cf32x16 &value) const { return (cfp16n0_3x16 &)_mm256_add_epi16(data256, toFixed16(value).ymm); }
   inline cfp16n0_3x16 operator-(cf32x16 &value) const { return (cfp16n0_3x16 &)_mm256_sub_epi16(data256, toFixed16(value).ymm); }
   inline cfp16n0_3x16 operator*(cf32x16 &value) const {
      cf32x16 temp = toFloat16();
      cf32x16 temp2 = { .ymm = { _mm256_mul_ps(temp.ymm[0], value.ymm[0]), _mm256_mul_ps(temp.ymm[1], value.ymm[1]) } };
      return (cfp16n0_3x16 &)toFixed16(temp2);
   }
   inline cfp16n0_3x16 operator/(cf32x16 &value) const {
      cf32x16 temp = toFloat16();
      cf32x16 temp2 = { .ymm = { _mm256_div_ps(temp.ymm[0], value.ymm[0]), _mm256_div_ps(temp.ymm[1], value.ymm[1]) } };
      return (cfp16n0_3x16 &)toFixed16(temp2);
   }
   inline cfp16n0_3x16 operator+=(cf32x16 &value) { data256 = _mm256_add_epi16(data256, toFixed16(value).ymm); return *this; }
   inline cfp16n0_3x16 operator-=(cf32x16 &value) { data256 = _mm256_sub_epi16(data256, toFixed16(value).ymm); return *this; }
   inline cfp16n0_3x16 operator*=(cf32x16 &value) {
      cf32x16 temp = toFloat16();
      cf32x16 temp2 = { .ymm = { _mm256_mul_ps(temp.ymm[0], value.ymm[0]), _mm256_mul_ps(temp.ymm[1], value.ymm[1]) } };
      data16x16 = toFixed16(temp2);
      return *this;
   }
   inline cfp16n0_3x16 operator/=(cf32x16 &value) {
      cf32x16 temp = toFloat16();
      cf32x16 temp2 = { .ymm = { _mm256_div_ps(temp.ymm[0], value.ymm[0]), _mm256_div_ps(temp.ymm[1], value.ymm[1]) } };
      data16x16 = toFixed16(temp2);
      return *this;
   }
#else
   inline cfp16n0_3x16 operator++(void) { data128[0] = _mm_add_epi16(data128[0], _fpdt_1x8); data128[1] = _mm_add_epi16(data128[1], _fpdt_1x8); return *this; }
   inline cfp16n0_3x16 operator++(int) {
      ci16x16 temp = { .xmm = { data128[0], data128[1] } };
      data128[0] = _mm_add_epi16(data128[0], _fpdt_1x8);
      data128[1] = _mm_add_epi16(data128[1], _fpdt_1x8);
      return (cfp16n0_3x16 &)temp;
   }
   inline cfp16n0_3x16 operator--(void) { data128[0] = _mm_sub_epi16(data128[0], _fpdt_1x8); data128[1] = _mm_sub_epi16(data128[1], _fpdt_1x8); return *this; }
   inline cfp16n0_3x16 operator--(int) {
      ci16x16 temp = { .xmm = { data128[0], data128[1] } };
      data128[0] = _mm_sub_epi16(data128[0], _fpdt_1x8);
      data128[1] = _mm_sub_epi16(data128[1], _fpdt_1x8);
      return (cfp16n0_3x16 &)temp;
   }

   inline cfp16n0_3x16 operator<<(csi32 &value) const { ci16x16 temp = { .xmm = { _mm_slli_epi16(data128[0], value), _mm_slli_epi16(data128[1], value) } }; return (cfp16n0_3x16 &)temp; }
   inline cfp16n0_3x16 operator>>(csi32 &value) const { ci16x16 temp = { .xmm = { _mm_srli_epi16(data128[0], value), _mm_srli_epi16(data128[1], value) } }; return (cfp16n0_3x16 &)temp; }
   inline cfp16n0_3x16 operator<<=(csi32 &value) { data128[0] = _mm_slli_epi16(data128[0], value); data128[1] = _mm_slli_epi16(data128[1], value); return *this; }
   inline cfp16n0_3x16 operator>>=(csi32 &value) { data128[0] = _mm_srli_epi16(data128[0], value); data128[1] = _mm_srli_epi16(data128[1], value); return *this; }

   inline cfp16n0_3x16 operator+(cfp16n0_3x16 &value) const {
      ci16x16 temp = { .xmm = { _mm_add_epi16(data128[0], value.data128[0]), _mm_add_epi16(data128[1], value.data128[1]) } };
      return (cfp16n0_3x16 &)temp;
   }
   inline cfp16n0_3x16 operator-(cfp16n0_3x16 &value) const {
      ci16x16 temp = { .xmm = { _mm_sub_epi16(data128[0], value.data128[0]), _mm_sub_epi16(data128[1], value.data128[1]) } };
      return (cfp16n0_3x16 &)temp;
   }
   inline cfp16n0_3x16 operator*(cfp16n0_3x16 &value) const {
      cf32x16 temp = toFloat16(), input = value.toFloat16();
      cf32x16 temp2 = { .ymm = { _mm256_mul_ps(temp.ymm[0], input.ymm[0]), _mm256_mul_ps(temp.ymm[1], input.ymm[1])} };
      return (cfp16n0_3x16 &)toFixed16(temp2);
   }
   inline cfp16n0_3x16 operator/(cfp16n0_3x16 &value) const {cf32x16 temp = toFloat16(), input = value.toFloat16();
      cf32x16 temp2 = { .ymm = { _mm256_div_ps(temp.ymm[0], input.ymm[0]), _mm256_div_ps(temp.ymm[1], input.ymm[1])} };
      return (cfp16n0_3x16 &)toFixed16(temp2);
   }
   inline cfp16n0_3x16 operator+=(cfp16n0_3x16 &value) { data128[0] = _mm_add_epi16(data128[0], value.data128[0]); data128[1] = _mm_add_epi16(data128[1], value.data128[1]); return *this; }
   inline cfp16n0_3x16 operator-=(cfp16n0_3x16 &value) { data128[0] = _mm_sub_epi16(data128[0], value.data128[0]); data128[1] = _mm_sub_epi16(data128[1], value.data128[1]); return *this; }
   inline cfp16n0_3x16 operator*=(cfp16n0_3x16 &value) {
      cf32x16 temp = toFloat16(), input = value.toFloat16();
      cf32x16 temp2 = { .ymm = { _mm256_mul_ps(temp.ymm[0], input.ymm[0]), _mm256_mul_ps(temp.ymm[1], input.ymm[1])} };
      data16x16 = toFixed16(temp2);
      return *this;
   }
   inline cfp16n0_3x16 operator/=(cfp16n0_3x16 &value) {
      cf32x16 temp = toFloat16(), input = value.toFloat16();
      cf32x16 temp2 = { .ymm = { _mm256_div_ps(temp.ymm[0], input.ymm[0]), _mm256_div_ps(temp.ymm[1], input.ymm[1])} };
      data16x16 = toFixed16(temp2);
      return *this;
   }

   inline cfp16n0_3x16 operator+(cfl32 &value) const {
      cui128 input = _mm_set1_epi16(toFixed(value));
      ci16x16 temp = { .xmm = { _mm_add_epi16(data128[0], input), _mm_add_epi16(data128[1], input) } };
      return (cfp16n0_2 &)temp;
   }
   inline cfp16n0_3x16 operator-(cfl32 &value) const {
      cui128 input = _mm_set1_epi16(toFixed(value));
      ci16x16 temp = { .xmm = { _mm_sub_epi16(data128[0], input), _mm_sub_epi16(data128[1], input) } };
      return (cfp16n0_2 &)temp;
   }
   inline cfp16n0_3x16 operator*(cfl32 &value) const {
      cfl32x4 input = _mm_set_ps1(value);
      cf32x16 temp = toFloat16();
      cf32x16 temp2 = { .xmm = { _mm_mul_ps(temp.xmm[0], input), _mm_mul_ps(temp.xmm[1], input), _mm_mul_ps(temp.xmm[2], input), _mm_mul_ps(temp.xmm[3], input) } };
      return (cfp16n0_3x16 &)toFixed16(temp2);
   }
   inline cfp16n0_3x16 operator/(cfl32 &value) const {
      cfl32x4 input = _mm_set_ps1(value);
      cf32x16 temp = toFloat16();
      cf32x16 temp2 = { .xmm = { _mm_div_ps(temp.xmm[0], input), _mm_div_ps(temp.xmm[1], input), _mm_div_ps(temp.xmm[2], input), _mm_div_ps(temp.xmm[3], input) } };
      return (cfp16n0_3x16 &)toFixed16(temp2);
   }
   inline cfp16n0_3x16 operator+=(cfl32 &value) { cui128 input = _mm_set1_epi16(toFixed(value)); data128[0] = _mm_add_epi16(data128[0], input); data128[1] = _mm_add_epi16(data128[1], input); return *this; }
   inline cfp16n0_3x16 operator-=(cfl32 &value) { cui128 input = _mm_set1_epi16(toFixed(value)); data128[0] = _mm_sub_epi16(data128[0], input); data128[1] = _mm_sub_epi16(data128[1], input); return *this; }
   inline cfp16n0_3x16 operator*=(cfl32 &value) {
      cfl32x4 input = _mm_set_ps1(value);
      cf32x16 temp = toFloat16();
      cf32x16 temp2 = { .xmm = { _mm_mul_ps(temp.xmm[0], input), _mm_mul_ps(temp.xmm[1], input), _mm_mul_ps(temp.xmm[2], input), _mm_mul_ps(temp.xmm[3], input) } };
      data16x16 = toFixed16(temp2);
      return *this;
   }
   inline cfp16n0_3x16 operator/=(cfl32 &value) {
      cfl32x4 input = _mm_set_ps1(value);
      cf32x16 temp = toFloat16();
      cf32x16 temp2 = { .xmm = { _mm_div_ps(temp.xmm[0], input), _mm_div_ps(temp.xmm[1], input), _mm_div_ps(temp.xmm[2], input), _mm_div_ps(temp.xmm[3], input) } };
      data16x16 = toFixed16(temp2);
      return *this;
   }

   inline cfp16n0_3x16 operator+(cf32x16 &value) const {
      ci16x16 input = toFixed16(value);
      ci16x16 temp = { .xmm = { _mm_add_epi16(data128[0], input.xmm[0]), _mm_add_epi16(data128[1], input.xmm[1])}};
      return (cfp16n0_2 &)temp;
   }
   inline cfp16n0_3x16 operator-(cf32x16 &value) const {
      ci16x16 input = toFixed16(value);
      ci16x16 temp = { .xmm = { _mm_sub_epi16(data128[0], input.xmm[0]), _mm_sub_epi16(data128[1], input.xmm[1])}};
      return (cfp16n0_2 &)temp;
   }
   inline cfp16n0_3x16 operator*(cf32x16 &value) const {
      cf32x16 temp = toFloat16();
      cf32x16 temp2 = { .xmm = { _mm_mul_ps(temp.xmm[0], value.xmm[0]), _mm_mul_ps(temp.xmm[1], value.xmm[1]), _mm_mul_ps(temp.xmm[2], value.xmm[2]), _mm_mul_ps(temp.xmm[3], value.xmm[3])}};
      return (cfp16n0_3x16 &)toFixed16(temp2);
   }
   inline cfp16n0_3x16 operator/(cf32x16 &value) const {
      cf32x16 temp = toFloat16();
      cf32x16 temp2 = { .ymm = { _mm256_div_ps(temp.ymm[0], value.ymm[0]), _mm256_div_ps(temp.ymm[1], value.ymm[1]) } };
      return (cfp16n0_3x16 &)toFixed16(temp2);
   }
   inline cfp16n0_3x16 operator+=(cf32x16 &value) { ci16x16 temp = toFixed16(value); data128[0] = _mm_add_epi16(data128[0], temp.xmm[0]); data128[1] = _mm_add_epi16(data128[1], temp.xmm[1]); return *this; }
   inline cfp16n0_3x16 operator-=(cf32x16 &value) { ci16x16 temp = toFixed16(value); data128[0] = _mm_sub_epi16(data128[0], temp.xmm[0]); data128[1] = _mm_sub_epi16(data128[1], temp.xmm[1]); return *this; }
   inline cfp16n0_3x16 operator*=(cf32x16 &value) {
      cf32x16 temp = toFloat16();
      cf32x16 temp2 = { .xmm = { _mm_mul_ps(temp.xmm[0], value.xmm[0]), _mm_mul_ps(temp.xmm[1], value.xmm[1]), _mm_mul_ps(temp.xmm[2], value.xmm[2]), _mm_mul_ps(temp.xmm[3], value.xmm[3])}};
      data16x16 = toFixed16(temp2);
      return *this;
   }
   inline cfp16n0_3x16 operator/=(cf32x16 &value) {
      cf32x16 temp = toFloat16();
      cf32x16 temp2 = { .ymm = { _mm256_div_ps(temp.ymm[0], value.ymm[0]), _mm256_div_ps(temp.ymm[1], value.ymm[1]) } };
      data16x16 = toFixed16(temp2);
      return *this;
   }
#endif
};

#ifdef _24BIT_INTEGERS_

// 24-bit, 0.24 : Decimal range of 0.0~0.999999940395355224609375
struct f0p24 {
   typedef const f0p24 cf0p24;

   ui24 data;

   inline cui24 toFixed(cfl32 &value) const { return ui24(value * 16777216.0f); }
   inline cfl32 toFloat(void) const { return cfl32(data) * _fpdt_rcp2p24f; }
   inline cfl32 toFloat(cf0p24 &value) const { return fl32(value.data) * _fpdt_rcp2p24f; }

   f0p24(void) = default;
   f0p24(const fp8n value) { data = toFixed(fl32(value) * _fpdt_data_.rangeDivMax8 + _fpdt_data_.origin8); }
   f0p24(const fp16n value) { data = toFixed(fl32(value) * _fpdt_data_.rangeDivMax16 + _fpdt_data_.origin16); }
   f0p24(const fp24n value) { data = toFixed(fl32(value) * _fpdt_data_.rangeDivMax24 + _fpdt_data_.origin24); }
   f0p24(const fp32n value) { data = toFixed(fl32(fl64(value) * _fpdt_data_.rangeDivMax32 + _fpdt_data_.origin32)); }
   f0p24(cui24 value) { data = value; }
   f0p24(csi32 value) { data = (ui24 &)value; }
   f0p24(csi64 value) { data = (ui24 &)value; }
   f0p24(cfl32 value) { data = toFixed(value); }

   operator ptr(void) const { return *this; }
   operator cfl32(void) const { return cfl32(data) * _fpdt_rcp2p24f; }

   inline cf0p24 &operator&(void) const { return *this; }
   inline cf0p24 operator~(void) const { return (cf0p24 &)~data; }
   inline cf0p24 operator-(void) const { return (cf0p24 &)~data; }
   inline cbool operator!(void) const { return data == 0; }

   inline cf0p24 operator++(void) { data++; return *this; }
   inline cf0p24 operator++(int) { cui24 temp = data++; return (cf0p24 &)temp; }
   inline cf0p24 operator--(void) { data--; return *this; }
   inline cf0p24 operator--(int) { cui24 temp = data--; return (cf0p24 &)temp; }

   inline cbool operator==(csi32 &value) const { return data == (ui24 &)value; }
   inline cbool operator!=(csi32 &value) const { return data != (ui24 &)value; }
   inline cbool operator>=(csi32 &value) const { return data >= (ui24 &)value; }
   inline cbool operator<=(csi32 &value) const { return data <= (ui24 &)value; }
   inline cbool operator>(csi32 &value) const { return data > (ui24 &)value; }
   inline cbool operator<(csi32 &value) const { return data < (ui24 &)value; }
   inline cbool operator||(csi32 &value) const { return data || (ui24 &)value; }
   inline cbool operator&&(csi32 &value) const { return data && (ui24 &)value; }

   inline cf0p24 operator<<(csi32 &value) const { return (cf0p24 &)(data << value); }
   inline cf0p24 operator>>(csi32 &value) const { return (cf0p24 &)(data >> value); }
   inline cf0p24 operator<<=(csi32 &value) { return (cf0p24 &)(data <<= value); }
   inline cf0p24 operator>>=(csi32 &value) { return (cf0p24 &)(data >>= value); }

   inline cbool operator==(cf0p24 &value) const { return data == value.data; }
   inline cbool operator!=(cf0p24 &value) const { return data != value.data; }
   inline cbool operator>=(cf0p24 &value) const { return data >= value.data; }
   inline cbool operator<=(cf0p24 &value) const { return data <= value.data; }
   inline cbool operator>(cf0p24 &value) const { return data > value.data; }
   inline cbool operator<(cf0p24 &value) const { return data < value.data; }
   inline cbool operator||(cf0p24 &value) const { return data || value.data; }
   inline cbool operator&&(cf0p24 &value) const { return data && value.data; }

   inline cf0p24 operator+(cf0p24 &value) const { return (cf0p24 &)(data + value.data); }
   inline cf0p24 operator-(cf0p24 &value) const { return (cf0p24 &)(data - value.data); }
   inline cf0p24 operator*(cf0p24 &value) const { return toFixed(toFloat() * toFloat(value)); }
   inline cf0p24 operator/(cf0p24 &value) const { return toFixed(toFloat() / toFloat(value)); }
   inline cf0p24 operator%(cf0p24 &value) const { return (cf0p24 &)(data % value.data); }
   inline cf0p24 operator&(cf0p24 &value) const { return (cf0p24 &)(data & value.data); }
   inline cf0p24 operator|(cf0p24 &value) const { return (cf0p24 &)(data | value.data); }
   inline cf0p24 operator^(cf0p24 &value) const { return (cf0p24 &)(data ^ value.data); }
   inline cf0p24 operator+=(cf0p24 &value) { return (cf0p24 &)(data += value.data); }
   inline cf0p24 operator-=(cf0p24 &value) { return (cf0p24 &)(data -= value.data); }
   inline cf0p24 operator*=(cf0p24 &value) { return data = toFixed(toFloat() * toFloat(value)); }
   inline cf0p24 operator/=(cf0p24 &value) { return data = toFixed(toFloat() / toFloat(value)); }
   inline cf0p24 operator%=(cf0p24 &value) { return (cf0p24 &)(data %= value.data); }
   inline cf0p24 operator&=(cf0p24 &value) { return (cf0p24 &)(data &= value.data); }
   inline cf0p24 operator|=(cf0p24 &value) { return (cf0p24 &)(data |= value.data); }
   inline cf0p24 operator^=(cf0p24 &value) { return (cf0p24 &)(data ^= value.data); }

   inline cfl32 operator+(cfl32 &value) const { return toFloat() + value; }
   inline cfl32 operator-(cfl32 &value) const { return toFloat() - value; }
   inline cfl32 operator*(cfl32 &value) const { return toFloat() * value; }
   inline cfl32 operator/(cfl32 &value) const { return toFloat() / value; }
   inline cf0p24 operator%(cfl32 &value) const { return (cf0p24 &)(data % toFixed(value)); }
   inline cf0p24 operator&(cfl32 &value) const { return (cf0p24 &)(data & toFixed(value)); }
   inline cf0p24 operator|(cfl32 &value) const { return (cf0p24 &)(data | toFixed(value)); }
   inline cf0p24 operator^(cfl32 &value) const { return (cf0p24 &)(data ^ toFixed(value)); }
   inline cfl32 operator+=(cfl32 &value) { cfl32 temp = toFloat() + value; *this = toFixed(temp); return temp; }
   inline cfl32 operator-=(cfl32 &value) { cfl32 temp = toFloat() - value; *this = toFixed(temp); return temp; }
   inline cfl32 operator*=(cfl32 &value) { cfl32 temp = toFloat() * value; *this = toFixed(temp); return temp; }
   inline cfl32 operator/=(cfl32 &value) { cfl32 temp = toFloat() / value; *this = toFixed(temp); return temp; }
   inline cf0p24 operator%=(cfl32 &value) { return (cf0p24 &)(data %= toFixed(value)); }
   inline cf0p24 operator&=(cfl32 &value) { return (cf0p24 &)(data &= toFixed(value)); }
   inline cf0p24 operator|=(cfl32 &value) { return (cf0p24 &)(data |= toFixed(value)); }
   inline cf0p24 operator^=(cfl32 &value) { return (cf0p24 &)(data ^= toFixed(value)); }

   inline cbool operator==(cfl32 &value) const { return data == toFixed(value); }
   inline cbool operator!=(cfl32 &value) const { return data != toFixed(value); }
   inline cbool operator>=(cfl32 &value) const { return data <= toFixed(value); }
   inline cbool operator<=(cfl32 &value) const { return data >= toFixed(value); }
   inline cbool operator>(cfl32 &value) const { return data < toFixed(value); }
   inline cbool operator<(cfl32 &value) const { return data > toFixed(value); }
   inline cbool operator||(cfl32 &value) const { return data || toFixed(value); }
   inline cbool operator&&(cfl32 &value) const { return data && toFixed(value); }
};

// 24-bit, 8.16 : Decimal range of 0.0~255.9999847412109375
struct f8p16 {
   typedef const f8p16 cf8p16;

   ui24 data;

   inline cui24 toFixed(cfl32 &value) const { return ui24(value * 65536.0f); }
   inline cfl32 toFloat(void) const { return cfl32(data) * _fpdt_rcp65536f; }
   inline cfl32 toFloat(cf8p16 &value) const { return fl32(value.data) * _fpdt_rcp65536f; }

   f8p16(void) = default;
   f8p16(const fp8n value) { data = toFixed(fl32(value) * _fpdt_data_.rangeDivMax8 + _fpdt_data_.origin8); }
   f8p16(const fp16n value) { data = toFixed(fl32(value) * _fpdt_data_.rangeDivMax16 + _fpdt_data_.origin16); }
   f8p16(const fp24n value) { data = toFixed(fl32(value) * _fpdt_data_.rangeDivMax24 + _fpdt_data_.origin24); }
   f8p16(const fp32n value) { data = toFixed(fl32(fl64(value) * _fpdt_data_.rangeDivMax32 + _fpdt_data_.origin32)); }
   f8p16(cui24 value) { data = value; }
   f8p16(csi32 value) { data = (ui24 &)value; }
   f8p16(csi64 value) { data = (ui24 &)value; }
   f8p16(cfl32 value) { data = toFixed(value); }

   operator ptr(void) const { return *this; }
   operator cfl32(void) const { return cfl32(data) * _fpdt_rcp65536f; }

   inline cf8p16 &operator&(void) const { return *this; }
   inline cf8p16 operator~(void) const { return (cf8p16 &)~data; }
   inline cf8p16 operator-(void) const { return (cf8p16 &)~data; }
   inline cbool operator!(void) const { return data == 0; }

   inline cf8p16 operator++(void) { data++; return *this; }
   inline cf8p16 operator++(int) { cui24 temp = data++; return (cf8p16 &)temp; }
   inline cf8p16 operator--(void) { data--; return *this; }
   inline cf8p16 operator--(int) { cui24 temp = data--; return (cf8p16 &)temp; }

   inline cbool operator==(csi32 &value) const { return data == (ui24 &)value; }
   inline cbool operator!=(csi32 &value) const { return data != (ui24 &)value; }
   inline cbool operator>=(csi32 &value) const { return data >= (ui24 &)value; }
   inline cbool operator<=(csi32 &value) const { return data <= (ui24 &)value; }
   inline cbool operator>(csi32 &value) const { return data > (ui24 &)value; }
   inline cbool operator<(csi32 &value) const { return data < (ui24 &)value; }
   inline cbool operator||(csi32 &value) const { return data || (ui24 &)value; }
   inline cbool operator&&(csi32 &value) const { return data && (ui24 &)value; }

   inline cf8p16 operator<<(csi32 &value) const { return (cf8p16 &)(data << value); }
   inline cf8p16 operator>>(csi32 &value) const { return (cf8p16 &)(data >> value); }
   inline cf8p16 operator<<=(csi32 &value) { return (cf8p16 &)(data <<= value); }
   inline cf8p16 operator>>=(csi32 &value) { return (cf8p16 &)(data >>= value); }

   inline cbool operator==(cf8p16 &value) const { return data == value.data; }
   inline cbool operator!=(cf8p16 &value) const { return data != value.data; }
   inline cbool operator>=(cf8p16 &value) const { return data >= value.data; }
   inline cbool operator<=(cf8p16 &value) const { return data <= value.data; }
   inline cbool operator>(cf8p16 &value) const { return data > value.data; }
   inline cbool operator<(cf8p16 &value) const { return data < value.data; }
   inline cbool operator||(cf8p16 &value) const { return data || value.data; }
   inline cbool operator&&(cf8p16 &value) const { return data && value.data; }

   inline cf8p16 operator+(cf8p16 &value) const { return (cf8p16 &)(data + value.data); }
   inline cf8p16 operator-(cf8p16 &value) const { return (cf8p16 &)(data - value.data); }
   inline cf8p16 operator*(cf8p16 &value) const { return toFixed(toFloat() * toFloat(value)); }
   inline cf8p16 operator/(cf8p16 &value) const { return toFixed(toFloat() / toFloat(value)); }
   inline cf8p16 operator%(cf8p16 &value) const { return (cf8p16 &)(data % value.data); }
   inline cf8p16 operator&(cf8p16 &value) const { return (cf8p16 &)(data & value.data); }
   inline cf8p16 operator|(cf8p16 &value) const { return (cf8p16 &)(data | value.data); }
   inline cf8p16 operator^(cf8p16 &value) const { return (cf8p16 &)(data ^ value.data); }
   inline cf8p16 operator+=(cf8p16 &value) { return (cf8p16 &)(data += value.data); }
   inline cf8p16 operator-=(cf8p16 &value) { return (cf8p16 &)(data -= value.data); }
   inline cf8p16 operator*=(cf8p16 &value) { return data = toFixed(toFloat() * toFloat(value)); }
   inline cf8p16 operator/=(cf8p16 &value) { return data = toFixed(toFloat() / toFloat(value)); }
   inline cf8p16 operator%=(cf8p16 &value) { return (cf8p16 &)(data %= value.data); }
   inline cf8p16 operator&=(cf8p16 &value) { return (cf8p16 &)(data &= value.data); }
   inline cf8p16 operator|=(cf8p16 &value) { return (cf8p16 &)(data |= value.data); }
   inline cf8p16 operator^=(cf8p16 &value) { return (cf8p16 &)(data ^= value.data); }

   inline cfl32 operator+(cfl32 &value) const { return toFloat() + value; }
   inline cfl32 operator-(cfl32 &value) const { return toFloat() - value; }
   inline cfl32 operator*(cfl32 &value) const { return toFloat() * value; }
   inline cfl32 operator/(cfl32 &value) const { return toFloat() / value; }
   inline cf8p16 operator%(cfl32 &value) const { return (cf8p16 &)(data % toFixed(value)); }
   inline cf8p16 operator&(cfl32 &value) const { return (cf8p16 &)(data & toFixed(value)); }
   inline cf8p16 operator|(cfl32 &value) const { return (cf8p16 &)(data | toFixed(value)); }
   inline cf8p16 operator^(cfl32 &value) const { return (cf8p16 &)(data ^ toFixed(value)); }
   inline cfl32 operator+=(cfl32 &value) { cfl32 temp = toFloat() + value; *this = toFixed(temp); return temp; }
   inline cfl32 operator-=(cfl32 &value) { cfl32 temp = toFloat() - value; *this = toFixed(temp); return temp; }
   inline cfl32 operator*=(cfl32 &value) { cfl32 temp = toFloat() * value; *this = toFixed(temp); return temp; }
   inline cfl32 operator/=(cfl32 &value) { cfl32 temp = toFloat() / value; *this = toFixed(temp); return temp; }
   inline cf8p16 operator%=(cfl32 &value) { return (cf8p16 &)(data %= toFixed(value)); }
   inline cf8p16 operator&=(cfl32 &value) { return (cf8p16 &)(data &= toFixed(value)); }
   inline cf8p16 operator|=(cfl32 &value) { return (cf8p16 &)(data |= toFixed(value)); }
   inline cf8p16 operator^=(cfl32 &value) { return (cf8p16 &)(data ^= toFixed(value)); }

   inline cbool operator==(cfl32 &value) const { return data == toFixed(value); }
   inline cbool operator!=(cfl32 &value) const { return data != toFixed(value); }
   inline cbool operator>=(cfl32 &value) const { return data <= toFixed(value); }
   inline cbool operator<=(cfl32 &value) const { return data >= toFixed(value); }
   inline cbool operator>(cfl32 &value) const { return data < toFixed(value); }
   inline cbool operator<(cfl32 &value) const { return data > toFixed(value); }
   inline cbool operator||(cfl32 &value) const { return data || toFixed(value); }
   inline cbool operator&&(cfl32 &value) const { return data && toFixed(value); }
};

// 24-bit, 12.12 : Decimal range of 0.0~4095.999755859375
struct f12p12 {
   typedef const f12p12 cf12p12;

   ui24 data;

   inline cui24 toFixed(cfl32 &value) const { return ui24(value * 4096.0f); }
   inline cfl32 toFloat(void) const { return cfl32(data) * _fpdt_rcp4096f; }
   inline cfl32 toFloat(cf12p12 &value) const { return fl32(value.data) * _fpdt_rcp4096f; }

   f12p12(void) = default;
   f12p12(const fp8n value) { data = toFixed(fl32(value) * _fpdt_data_.rangeDivMax8 + _fpdt_data_.origin8); }
   f12p12(const fp16n value) { data = toFixed(fl32(value) * _fpdt_data_.rangeDivMax16 + _fpdt_data_.origin16); }
   f12p12(const fp24n value) { data = toFixed(fl32(value) * _fpdt_data_.rangeDivMax24 + _fpdt_data_.origin24); }
   f12p12(const fp32n value) { data = toFixed(fl32(fl64(value) * _fpdt_data_.rangeDivMax32 + _fpdt_data_.origin32)); }
   f12p12(cui24 value) { data = value; }
   f12p12(csi32 value) { data = (ui24 &)value; }
   f12p12(csi64 value) { data = (ui24 &)value; }
   f12p12(cfl32 value) { data = toFixed(value); }

   operator ptr(void) const { return *this; }
   operator cfl32(void) const { return cfl32(data) * _fpdt_rcp4096f; }

   inline cf12p12 &operator&(void) const { return *this; }
   inline cf12p12 operator~(void) const { return (cf12p12 &)~data; }
   inline cf12p12 operator-(void) const { return (cf12p12 &)~data; }
   inline cbool operator!(void) const { return data == 0; }

   inline cf12p12 operator++(void) { data++; return *this; }
   inline cf12p12 operator++(int) { cui24 temp = data++; return (cf12p12 &)temp; }
   inline cf12p12 operator--(void) { data--; return *this; }
   inline cf12p12 operator--(int) { cui24 temp = data--; return (cf12p12 &)temp; }

   inline cbool operator==(csi32 &value) const { return data == (ui24 &)value; }
   inline cbool operator!=(csi32 &value) const { return data != (ui24 &)value; }
   inline cbool operator>=(csi32 &value) const { return data >= (ui24 &)value; }
   inline cbool operator<=(csi32 &value) const { return data <= (ui24 &)value; }
   inline cbool operator>(csi32 &value) const { return data > (ui24 &)value; }
   inline cbool operator<(csi32 &value) const { return data < (ui24 &)value; }
   inline cbool operator||(csi32 &value) const { return data || (ui24 &)value; }
   inline cbool operator&&(csi32 &value) const { return data && (ui24 &)value; }

   inline cf12p12 operator<<(csi32 &value) const { return (cf12p12 &)(data << value); }
   inline cf12p12 operator>>(csi32 &value) const { return (cf12p12 &)(data >> value); }
   inline cf12p12 operator<<=(csi32 &value) { return (cf12p12 &)(data <<= value); }
   inline cf12p12 operator>>=(csi32 &value) { return (cf12p12 &)(data >>= value); }

   inline cbool operator==(cf12p12 &value) const { return data == value.data; }
   inline cbool operator!=(cf12p12 &value) const { return data != value.data; }
   inline cbool operator>=(cf12p12 &value) const { return data >= value.data; }
   inline cbool operator<=(cf12p12 &value) const { return data <= value.data; }
   inline cbool operator>(cf12p12 &value) const { return data > value.data; }
   inline cbool operator<(cf12p12 &value) const { return data < value.data; }
   inline cbool operator||(cf12p12 &value) const { return data || value.data; }
   inline cbool operator&&(cf12p12 &value) const { return data && value.data; }

   inline cf12p12 operator+(cf12p12 &value) const { return (cf12p12 &)(data + value.data); }
   inline cf12p12 operator-(cf12p12 &value) const { return (cf12p12 &)(data - value.data); }
   inline cf12p12 operator*(cf12p12 &value) const { return toFixed(toFloat() * toFloat(value)); }
   inline cf12p12 operator/(cf12p12 &value) const { return toFixed(toFloat() / toFloat(value)); }
   inline cf12p12 operator%(cf12p12 &value) const { return (cf12p12 &)(data % value.data); }
   inline cf12p12 operator&(cf12p12 &value) const { return (cf12p12 &)(data & value.data); }
   inline cf12p12 operator|(cf12p12 &value) const { return (cf12p12 &)(data | value.data); }
   inline cf12p12 operator^(cf12p12 &value) const { return (cf12p12 &)(data ^ value.data); }
   inline cf12p12 operator+=(cf12p12 &value) { return (cf12p12 &)(data += value.data); }
   inline cf12p12 operator-=(cf12p12 &value) { return (cf12p12 &)(data -= value.data); }
   inline cf12p12 operator*=(cf12p12 &value) { return data = toFixed(toFloat() * toFloat(value)); }
   inline cf12p12 operator/=(cf12p12 &value) { return data = toFixed(toFloat() / toFloat(value)); }
   inline cf12p12 operator%=(cf12p12 &value) { return (cf12p12 &)(data %= value.data); }
   inline cf12p12 operator&=(cf12p12 &value) { return (cf12p12 &)(data &= value.data); }
   inline cf12p12 operator|=(cf12p12 &value) { return (cf12p12 &)(data |= value.data); }
   inline cf12p12 operator^=(cf12p12 &value) { return (cf12p12 &)(data ^= value.data); }

   inline cfl32 operator+(cfl32 &value) const { return toFloat() + value; }
   inline cfl32 operator-(cfl32 &value) const { return toFloat() - value; }
   inline cfl32 operator*(cfl32 &value) const { return toFloat() * value; }
   inline cfl32 operator/(cfl32 &value) const { return toFloat() / value; }
   inline cf12p12 operator%(cfl32 &value) const { return (cf12p12 &)(data % toFixed(value)); }
   inline cf12p12 operator&(cfl32 &value) const { return (cf12p12 &)(data & toFixed(value)); }
   inline cf12p12 operator|(cfl32 &value) const { return (cf12p12 &)(data | toFixed(value)); }
   inline cf12p12 operator^(cfl32 &value) const { return (cf12p12 &)(data ^ toFixed(value)); }
   inline cfl32 operator+=(cfl32 &value) { cfl32 temp = toFloat() + value; *this = toFixed(temp); return temp; }
   inline cfl32 operator-=(cfl32 &value) { cfl32 temp = toFloat() - value; *this = toFixed(temp); return temp; }
   inline cfl32 operator*=(cfl32 &value) { cfl32 temp = toFloat() * value; *this = toFixed(temp); return temp; }
   inline cfl32 operator/=(cfl32 &value) { cfl32 temp = toFloat() / value; *this = toFixed(temp); return temp; }
   inline cf12p12 operator%=(cfl32 &value) { return (cf12p12 &)(data %= toFixed(value)); }
   inline cf12p12 operator&=(cfl32 &value) { return (cf12p12 &)(data &= toFixed(value)); }
   inline cf12p12 operator|=(cfl32 &value) { return (cf12p12 &)(data |= toFixed(value)); }
   inline cf12p12 operator^=(cfl32 &value) { return (cf12p12 &)(data ^= toFixed(value)); }

   inline cbool operator==(cfl32 &value) const { return data == toFixed(value); }
   inline cbool operator!=(cfl32 &value) const { return data != toFixed(value); }
   inline cbool operator>=(cfl32 &value) const { return data <= toFixed(value); }
   inline cbool operator<=(cfl32 &value) const { return data >= toFixed(value); }
   inline cbool operator>(cfl32 &value) const { return data < toFixed(value); }
   inline cbool operator<(cfl32 &value) const { return data > toFixed(value); }
   inline cbool operator||(cfl32 &value) const { return data || toFixed(value); }
   inline cbool operator&&(cfl32 &value) const { return data && toFixed(value); }
};

// 24-bit, 16.8 : Decimal range of 0.0~65535.99609375
struct f16p8 {
   typedef const f16p8 cf16p8;

   ui24 data;

   inline cui24 toFixed(cfl32 &value) const { return ui24(value * 256.0f); }
   inline cfl32 toFloat(void) const { return cfl32(data) * _fpdt_rcp256f; }
   inline cfl32 toFloat(cf16p8 &value) const { return fl32(value.data) * _fpdt_rcp256f; }

   f16p8(void) = default;
   f16p8(const fp8n value) { data = toFixed(fl32(value) * _fpdt_data_.rangeDivMax8 + _fpdt_data_.origin8); }
   f16p8(const fp16n value) { data = toFixed(fl32(value) * _fpdt_data_.rangeDivMax16 + _fpdt_data_.origin16); }
   f16p8(const fp24n value) { data = toFixed(fl32(value) * _fpdt_data_.rangeDivMax24 + _fpdt_data_.origin24); }
   f16p8(const fp32n value) { data = toFixed(fl32(fl64(value) * _fpdt_data_.rangeDivMax32 + _fpdt_data_.origin32)); }
   f16p8(cui24 value) { data = value; }
   f16p8(csi32 value) { data = (ui24 &)value; }
   f16p8(csi64 value) { data = (ui24 &)value; }
   f16p8(cfl32 value) { data = toFixed(value); }

   operator ptr(void) const { return *this; }
   operator cfl32(void) const { return cfl32(data) * _fpdt_rcp256f; }

   inline cf16p8 &operator&(void) const { return *this; }
   inline cf16p8 operator~(void) const { return (cf16p8 &)~data; }
   inline cf16p8 operator-(void) const { return (cf16p8 &)~data; }
   inline cbool operator!(void) const { return data == 0; }

   inline cf16p8 operator++(void) { data++; return *this; }
   inline cf16p8 operator++(int) { cui24 temp = data++; return (cf16p8 &)temp; }
   inline cf16p8 operator--(void) { data--; return *this; }
   inline cf16p8 operator--(int) { cui24 temp = data--; return (cf16p8 &)temp; }

   inline cbool operator==(csi32 &value) const { return data == (ui24 &)value; }
   inline cbool operator!=(csi32 &value) const { return data != (ui24 &)value; }
   inline cbool operator>=(csi32 &value) const { return data >= (ui24 &)value; }
   inline cbool operator<=(csi32 &value) const { return data <= (ui24 &)value; }
   inline cbool operator>(csi32 &value) const { return data > (ui24 &)value; }
   inline cbool operator<(csi32 &value) const { return data < (ui24 &)value; }
   inline cbool operator||(csi32 &value) const { return data || (ui24 &)value; }
   inline cbool operator&&(csi32 &value) const { return data && (ui24 &)value; }

   inline cf16p8 operator<<(csi32 &value) const { return (cf16p8 &)(data << value); }
   inline cf16p8 operator>>(csi32 &value) const { return (cf16p8 &)(data >> value); }
   inline cf16p8 operator<<=(csi32 &value) { return (cf16p8 &)(data <<= value); }
   inline cf16p8 operator>>=(csi32 &value) { return (cf16p8 &)(data >>= value); }

   inline cbool operator==(cf16p8 &value) const { return data == value.data; }
   inline cbool operator!=(cf16p8 &value) const { return data != value.data; }
   inline cbool operator>=(cf16p8 &value) const { return data >= value.data; }
   inline cbool operator<=(cf16p8 &value) const { return data <= value.data; }
   inline cbool operator>(cf16p8 &value) const { return data > value.data; }
   inline cbool operator<(cf16p8 &value) const { return data < value.data; }
   inline cbool operator||(cf16p8 &value) const { return data || value.data; }
   inline cbool operator&&(cf16p8 &value) const { return data && value.data; }

   inline cf16p8 operator+(cf16p8 &value) const { return (cf16p8 &)(data + value.data); }
   inline cf16p8 operator-(cf16p8 &value) const { return (cf16p8 &)(data - value.data); }
   inline cf16p8 operator*(cf16p8 &value) const { return toFixed(toFloat() * toFloat(value)); }
   inline cf16p8 operator/(cf16p8 &value) const { return toFixed(toFloat() / toFloat(value)); }
   inline cf16p8 operator%(cf16p8 &value) const { return (cf16p8 &)(data % value.data); }
   inline cf16p8 operator&(cf16p8 &value) const { return (cf16p8 &)(data & value.data); }
   inline cf16p8 operator|(cf16p8 &value) const { return (cf16p8 &)(data | value.data); }
   inline cf16p8 operator^(cf16p8 &value) const { return (cf16p8 &)(data ^ value.data); }
   inline cf16p8 operator+=(cf16p8 &value) { return (cf16p8 &)(data += value.data); }
   inline cf16p8 operator-=(cf16p8 &value) { return (cf16p8 &)(data -= value.data); }
   inline cf16p8 operator*=(cf16p8 &value) { return data = toFixed(toFloat() * toFloat(value)); }
   inline cf16p8 operator/=(cf16p8 &value) { return data = toFixed(toFloat() / toFloat(value)); }
   inline cf16p8 operator%=(cf16p8 &value) { return (cf16p8 &)(data %= value.data); }
   inline cf16p8 operator&=(cf16p8 &value) { return (cf16p8 &)(data &= value.data); }
   inline cf16p8 operator|=(cf16p8 &value) { return (cf16p8 &)(data |= value.data); }
   inline cf16p8 operator^=(cf16p8 &value) { return (cf16p8 &)(data ^= value.data); }

   inline cfl32 operator+(cfl32 &value) const { return toFloat() + value; }
   inline cfl32 operator-(cfl32 &value) const { return toFloat() - value; }
   inline cfl32 operator*(cfl32 &value) const { return toFloat() * value; }
   inline cfl32 operator/(cfl32 &value) const { return toFloat() / value; }
   inline cf16p8 operator%(cfl32 &value) const { return (cf16p8 &)(data % toFixed(value)); }
   inline cf16p8 operator&(cfl32 &value) const { return (cf16p8 &)(data & toFixed(value)); }
   inline cf16p8 operator|(cfl32 &value) const { return (cf16p8 &)(data | toFixed(value)); }
   inline cf16p8 operator^(cfl32 &value) const { return (cf16p8 &)(data ^ toFixed(value)); }
   inline cfl32 operator+=(cfl32 &value) { cfl32 temp = toFloat() + value; *this = toFixed(temp); return temp; }
   inline cfl32 operator-=(cfl32 &value) { cfl32 temp = toFloat() - value; *this = toFixed(temp); return temp; }
   inline cfl32 operator*=(cfl32 &value) { cfl32 temp = toFloat() * value; *this = toFixed(temp); return temp; }
   inline cfl32 operator/=(cfl32 &value) { cfl32 temp = toFloat() / value; *this = toFixed(temp); return temp; }
   inline cf16p8 operator%=(cfl32 &value) { return (cf16p8 &)(data %= toFixed(value)); }
   inline cf16p8 operator&=(cfl32 &value) { return (cf16p8 &)(data &= toFixed(value)); }
   inline cf16p8 operator|=(cfl32 &value) { return (cf16p8 &)(data |= toFixed(value)); }
   inline cf16p8 operator^=(cfl32 &value) { return (cf16p8 &)(data ^= toFixed(value)); }

   inline cbool operator==(cfl32 &value) const { return data == toFixed(value); }
   inline cbool operator!=(cfl32 &value) const { return data != toFixed(value); }
   inline cbool operator>=(cfl32 &value) const { return data <= toFixed(value); }
   inline cbool operator<=(cfl32 &value) const { return data >= toFixed(value); }
   inline cbool operator>(cfl32 &value) const { return data < toFixed(value); }
   inline cbool operator<(cfl32 &value) const { return data > toFixed(value); }
   inline cbool operator||(cfl32 &value) const { return data || toFixed(value); }
   inline cbool operator&&(cfl32 &value) const { return data && toFixed(value); }
};

// Normalised 24-bit : Decimal range of 0.0~1.0
struct fp24n0_1 {
   typedef const fp24n0_1 cfp24n0_1;

   ui24 data;

   inline cui24 toFixed(cfl32 &value) const { return ui24(value * 16777215.0f); }
   inline cfl32 toFloat(void) const { return cfl32(data) * _fpdt_rcp2p24_1f; }
   inline cfl32 toFloat(cfp24n0_1 &value) const { return fl32(value.data) * _fpdt_rcp2p24_1f; }

   fp24n0_1(void) = default;
   fp24n0_1(const fp8n value) { data = toFixed(fl32(value) * _fpdt_data_.rangeDivMax8 + _fpdt_data_.origin8); }
   fp24n0_1(const fp16n value) { data = toFixed(fl32(value) * _fpdt_data_.rangeDivMax16 + _fpdt_data_.origin16); }
   fp24n0_1(const fp24n value) { data = toFixed(fl32(value) * _fpdt_data_.rangeDivMax24 + _fpdt_data_.origin24); }
   fp24n0_1(const fp32n value) { data = toFixed(fl32(fl64(value) * _fpdt_data_.rangeDivMax32 + _fpdt_data_.origin32)); }
   fp24n0_1(cui24 value) { data = value; }
   fp24n0_1(csi32 value) { data = (ui24 &)value; }
   fp24n0_1(csi64 value) { data = (ui24 &)value; }
   fp24n0_1(cfl32 value) { data = toFixed(value); }

   operator ptr(void) const { return *this; }
   operator cfl32(void) const { return cfl32(data) * _fpdt_rcp2p24_1f; }

   inline cfp24n0_1 &operator&(void) const { return *this; }
   inline cfp24n0_1 operator~(void) const { return (cfp24n0_1 &)~data; }
   inline cfp24n0_1 operator-(void) const { return (cfp24n0_1 &)~data; }
   inline cbool operator!(void) const { return data == 0; }

   inline cfp24n0_1 operator++(void) { data++; return *this; }
   inline cfp24n0_1 operator++(int) { cui24 temp = data++; return (cfp24n0_1 &)temp; }
   inline cfp24n0_1 operator--(void) { data--; return *this; }
   inline cfp24n0_1 operator--(int) { cui24 temp = data--; return (cfp24n0_1 &)temp; }

   inline cbool operator==(csi32 &value) const { return data == (ui24 &)value; }
   inline cbool operator!=(csi32 &value) const { return data != (ui24 &)value; }
   inline cbool operator>=(csi32 &value) const { return data >= (ui24 &)value; }
   inline cbool operator<=(csi32 &value) const { return data <= (ui24 &)value; }
   inline cbool operator>(csi32 &value) const { return data > (ui24 &)value; }
   inline cbool operator<(csi32 &value) const { return data < (ui24 &)value; }
   inline cbool operator||(csi32 &value) const { return data || (ui24 &)value; }
   inline cbool operator&&(csi32 &value) const { return data && (ui24 &)value; }

   inline cfp24n0_1 operator<<(csi32 &value) const { return (cfp24n0_1 &)(data << value); }
   inline cfp24n0_1 operator>>(csi32 &value) const { return (cfp24n0_1 &)(data >> value); }
   inline cfp24n0_1 operator<<=(csi32 &value) { return (cfp24n0_1 &)(data <<= value); }
   inline cfp24n0_1 operator>>=(csi32 &value) { return (cfp24n0_1 &)(data >>= value); }

   inline cbool operator==(cfp24n0_1 &value) const { return data == value.data; }
   inline cbool operator!=(cfp24n0_1 &value) const { return data != value.data; }
   inline cbool operator>=(cfp24n0_1 &value) const { return data >= value.data; }
   inline cbool operator<=(cfp24n0_1 &value) const { return data <= value.data; }
   inline cbool operator>(cfp24n0_1 &value) const { return data > value.data; }
   inline cbool operator<(cfp24n0_1 &value) const { return data < value.data; }
   inline cbool operator||(cfp24n0_1 &value) const { return data || value.data; }
   inline cbool operator&&(cfp24n0_1 &value) const { return data && value.data; }

   inline cfp24n0_1 operator+(cfp24n0_1 &value) const { return (cfp24n0_1 &)(data + value.data); }
   inline cfp24n0_1 operator-(cfp24n0_1 &value) const { return (cfp24n0_1 &)(data - value.data); }
   inline cfp24n0_1 operator*(cfp24n0_1 &value) const { return toFixed(toFloat() * toFloat(value)); }
   inline cfp24n0_1 operator/(cfp24n0_1 &value) const { return toFixed(toFloat() / toFloat(value)); }
   inline cfp24n0_1 operator%(cfp24n0_1 &value) const { return (cfp24n0_1 &)(data % value.data); }
   inline cfp24n0_1 operator&(cfp24n0_1 &value) const { return (cfp24n0_1 &)(data & value.data); }
   inline cfp24n0_1 operator|(cfp24n0_1 &value) const { return (cfp24n0_1 &)(data | value.data); }
   inline cfp24n0_1 operator^(cfp24n0_1 &value) const { return (cfp24n0_1 &)(data ^ value.data); }
   inline cfp24n0_1 operator+=(cfp24n0_1 &value) { return (cfp24n0_1 &)(data += value.data); }
   inline cfp24n0_1 operator-=(cfp24n0_1 &value) { return (cfp24n0_1 &)(data -= value.data); }
   inline cfp24n0_1 operator*=(cfp24n0_1 &value) { return data = toFixed(toFloat() * toFloat(value)); }
   inline cfp24n0_1 operator/=(cfp24n0_1 &value) { return data = toFixed(toFloat() / toFloat(value)); }
   inline cfp24n0_1 operator%=(cfp24n0_1 &value) { return (cfp24n0_1 &)(data %= value.data); }
   inline cfp24n0_1 operator&=(cfp24n0_1 &value) { return (cfp24n0_1 &)(data &= value.data); }
   inline cfp24n0_1 operator|=(cfp24n0_1 &value) { return (cfp24n0_1 &)(data |= value.data); }
   inline cfp24n0_1 operator^=(cfp24n0_1 &value) { return (cfp24n0_1 &)(data ^= value.data); }

   inline cfl32 operator+(cfl32 &value) const { return toFloat() + value; }
   inline cfl32 operator-(cfl32 &value) const { return toFloat() - value; }
   inline cfl32 operator*(cfl32 &value) const { return toFloat() * value; }
   inline cfl32 operator/(cfl32 &value) const { return toFloat() / value; }
   inline cfp24n0_1 operator%(cfl32 &value) const { return (cfp24n0_1 &)(data % toFixed(value)); }
   inline cfp24n0_1 operator&(cfl32 &value) const { return (cfp24n0_1 &)(data & toFixed(value)); }
   inline cfp24n0_1 operator|(cfl32 &value) const { return (cfp24n0_1 &)(data | toFixed(value)); }
   inline cfp24n0_1 operator^(cfl32 &value) const { return (cfp24n0_1 &)(data ^ toFixed(value)); }
   inline cfl32 operator+=(cfl32 &value) { cfl32 temp = toFloat() + value; *this = toFixed(temp); return temp; }
   inline cfl32 operator-=(cfl32 &value) { cfl32 temp = toFloat() - value; *this = toFixed(temp); return temp; }
   inline cfl32 operator*=(cfl32 &value) { cfl32 temp = toFloat() * value; *this = toFixed(temp); return temp; }
   inline cfl32 operator/=(cfl32 &value) { cfl32 temp = toFloat() / value; *this = toFixed(temp); return temp; }
   inline cfp24n0_1 operator%=(cfl32 &value) { return (cfp24n0_1 &)(data %= toFixed(value)); }
   inline cfp24n0_1 operator&=(cfl32 &value) { return (cfp24n0_1 &)(data &= toFixed(value)); }
   inline cfp24n0_1 operator|=(cfl32 &value) { return (cfp24n0_1 &)(data |= toFixed(value)); }
   inline cfp24n0_1 operator^=(cfl32 &value) { return (cfp24n0_1 &)(data ^= toFixed(value)); }

   inline cbool operator==(cfl32 &value) const { return data == toFixed(value); }
   inline cbool operator!=(cfl32 &value) const { return data != toFixed(value); }
   inline cbool operator>=(cfl32 &value) const { return data <= toFixed(value); }
   inline cbool operator<=(cfl32 &value) const { return data >= toFixed(value); }
   inline cbool operator>(cfl32 &value) const { return data < toFixed(value); }
   inline cbool operator<(cfl32 &value) const { return data > toFixed(value); }
   inline cbool operator||(cfl32 &value) const { return data || toFixed(value); }
   inline cbool operator&&(cfl32 &value) const { return data && toFixed(value); }
};

// Normalised 24-bit : Decimal range of -1.0~1.0
struct fp24n_1_1 {
   typedef const fp24n_1_1 cfp24n_1_1;

   ui24 data;

   inline cui24 toFixed(cfl32 &value) const { return ui24((value + 1.0f) * 8388607.5f); }
   inline cfl32 toFloat(void) const { return cfl32(data) * _fpdt_2div2p24_1f - 1.0f; }
   inline cfl32 toFloat(cfp24n_1_1 &value) const { return fl32(value.data) * _fpdt_2div2p24_1f - 1.0f; }

   fp24n_1_1(void) = default;
   fp24n_1_1(const fp8n value) { data = toFixed(fl32(value) * _fpdt_data_.rangeDivMax8 + _fpdt_data_.origin8); }
   fp24n_1_1(const fp16n value) { data = toFixed(fl32(value) * _fpdt_data_.rangeDivMax16 + _fpdt_data_.origin16); }
   fp24n_1_1(const fp24n value) { data = toFixed(fl32(value) * _fpdt_data_.rangeDivMax24 + _fpdt_data_.origin24); }
   fp24n_1_1(const fp32n value) { data = toFixed(fl32(fl64(value) * _fpdt_data_.rangeDivMax32 + _fpdt_data_.origin32)); }
   fp24n_1_1(cui24 value) { data = value; }
   fp24n_1_1(csi32 value) { data = (ui32 &)value; }
   fp24n_1_1(csi64 value) { data = (ui32 &)value; }
   fp24n_1_1(cfl32 value) { data = toFixed(value); }

   operator ptr(void) const { return *this; }
   operator cfl32(void) const { return cfl32(data) * _fpdt_2div2p24_1f - 1.0f; }

   inline cfp24n_1_1 &operator&(void) const { return *this; }
   inline cfp24n_1_1 operator~(void) const { return (cfp24n_1_1 &)~data; }
   inline cfp24n_1_1 operator-(void) const { return (cfp24n_1_1 &)~data; }
   inline cbool operator!(void) const { return data == 0; }

   inline cfp24n_1_1 operator++(void) { data++; return *this; }
   inline cfp24n_1_1 operator++(int) { cui24 temp = data++; return (cfp24n_1_1 &)temp; }
   inline cfp24n_1_1 operator--(void) { data--; return *this; }
   inline cfp24n_1_1 operator--(int) { cui24 temp = data--; return (cfp24n_1_1 &)temp; }

   inline cbool operator==(csi32 &value) const { return data == (ui24 &)value; }
   inline cbool operator!=(csi32 &value) const { return data != (ui24 &)value; }
   inline cbool operator>=(csi32 &value) const { return data >= (ui24 &)value; }
   inline cbool operator<=(csi32 &value) const { return data <= (ui24 &)value; }
   inline cbool operator>(csi32 &value) const { return data > (ui24 &)value; }
   inline cbool operator<(csi32 &value) const { return data < (ui24 &)value; }
   inline cbool operator||(csi32 &value) const { return data || (ui24 &)value; }
   inline cbool operator&&(csi32 &value) const { return data && (ui24 &)value; }

   inline cfp24n_1_1 operator<<(csi32 &value) const { return (cfp24n_1_1 &)(data << value); }
   inline cfp24n_1_1 operator>>(csi32 &value) const { return (cfp24n_1_1 &)(data >> value); }
   inline cfp24n_1_1 operator<<=(csi32 &value) { return (cfp24n_1_1 &)(data <<= value); }
   inline cfp24n_1_1 operator>>=(csi32 &value) { return (cfp24n_1_1 &)(data >>= value); }

   inline cbool operator==(cfp24n_1_1 &value) const { return data == value.data; }
   inline cbool operator!=(cfp24n_1_1 &value) const { return data != value.data; }
   inline cbool operator>=(cfp24n_1_1 &value) const { return data >= value.data; }
   inline cbool operator<=(cfp24n_1_1 &value) const { return data <= value.data; }
   inline cbool operator>(cfp24n_1_1 &value) const { return data > value.data; }
   inline cbool operator<(cfp24n_1_1 &value) const { return data < value.data; }
   inline cbool operator||(cfp24n_1_1 &value) const { return data || value.data; }
   inline cbool operator&&(cfp24n_1_1 &value) const { return data && value.data; }

   inline cfp24n_1_1 operator+(cfp24n_1_1 &value) const { return (cfp24n_1_1 &)(data + value.data - 8388607); }
   inline cfp24n_1_1 operator-(cfp24n_1_1 &value) const { return (cfp24n_1_1 &)(data - value.data - 8388607); }
   inline cfp24n_1_1 operator*(cfp24n_1_1 &value) const { return (cfp24n_1_1 &)toFixed(toFloat() * toFloat(value)); }
   inline cfp24n_1_1 operator/(cfp24n_1_1 &value) const { return (cfp24n_1_1 &)toFixed(toFloat() / toFloat(value)); }
   inline cfp24n_1_1 operator%(cfp24n_1_1 &value) const { return (cfp24n_1_1 &)(data % value.data); }
   inline cfp24n_1_1 operator&(cfp24n_1_1 &value) const { return (cfp24n_1_1 &)(data & value.data); }
   inline cfp24n_1_1 operator|(cfp24n_1_1 &value) const { return (cfp24n_1_1 &)(data | value.data); }
   inline cfp24n_1_1 operator^(cfp24n_1_1 &value) const { return (cfp24n_1_1 &)(data ^ value.data); }
   inline cfp24n_1_1 operator+=(cfp24n_1_1 &value) { return (cfp24n_1_1 &)(data += value.data - 8388607); }
   inline cfp24n_1_1 operator-=(cfp24n_1_1 &value) { return (cfp24n_1_1 &)(data -= value.data - 8388607); }
   inline cfp24n_1_1 operator*=(cfp24n_1_1 &value) { return (cfp24n_1_1 &)(data = toFixed(toFloat() * toFloat(value))); }
   inline cfp24n_1_1 operator/=(cfp24n_1_1 &value) { return (cfp24n_1_1 &)(data = toFixed(toFloat() / toFloat(value))); }
   inline cfp24n_1_1 operator%=(cfp24n_1_1 &value) { return (cfp24n_1_1 &)(data %= value.data); }
   inline cfp24n_1_1 operator&=(cfp24n_1_1 &value) { return (cfp24n_1_1 &)(data &= value.data); }
   inline cfp24n_1_1 operator|=(cfp24n_1_1 &value) { return (cfp24n_1_1 &)(data |= value.data); }
   inline cfp24n_1_1 operator^=(cfp24n_1_1 &value) { return (cfp24n_1_1 &)(data ^= value.data); }

   inline cfl32 operator+(cfl32 &value) const { return toFloat() + value; }
   inline cfl32 operator-(cfl32 &value) const { return toFloat() - value; }
   inline cfl32 operator*(cfl32 &value) const { return toFloat() * value; }
   inline cfl32 operator/(cfl32 &value) const { return toFloat() / value; }
   inline cfp24n_1_1 operator%(cfl32 &value) const { return (cfp24n_1_1 &)(data % toFixed(value)); }
   inline cfp24n_1_1 operator&(cfl32 &value) const { return (cfp24n_1_1 &)(data & toFixed(value)); }
   inline cfp24n_1_1 operator|(cfl32 &value) const { return (cfp24n_1_1 &)(data | toFixed(value)); }
   inline cfp24n_1_1 operator^(cfl32 &value) const { return (cfp24n_1_1 &)(data ^ toFixed(value)); }
   inline cfl32 operator+=(cfl32 &value) { cfl32 temp = toFloat() + value; data = toFixed(temp); return temp; }
   inline cfl32 operator-=(cfl32 &value) { cfl32 temp = toFloat() - value; data = toFixed(temp); return temp; }
   inline cfl32 operator*=(cfl32 &value) { cfl32 temp = toFloat() * value; data = toFixed(temp); return temp; }
   inline cfl32 operator/=(cfl32 &value) { cfl32 temp = toFloat() / value; data = toFixed(temp); return temp; }
   inline cfp24n_1_1 operator%=(cfl32 &value) { return (cfp24n_1_1 &)(data %= toFixed(value)); }
   inline cfp24n_1_1 operator&=(cfl32 &value) { return (cfp24n_1_1 &)(data &= toFixed(value)); }
   inline cfp24n_1_1 operator|=(cfl32 &value) { return (cfp24n_1_1 &)(data |= toFixed(value)); }
   inline cfp24n_1_1 operator^=(cfl32 &value) { return (cfp24n_1_1 &)(data ^= toFixed(value)); }

   inline cbool operator==(cfl32 &value) const { return data == toFixed(value); }
   inline cbool operator!=(cfl32 &value) const { return data != toFixed(value); }
   inline cbool operator>=(cfl32 &value) const { return data <= toFixed(value); }
   inline cbool operator<=(cfl32 &value) const { return data >= toFixed(value); }
   inline cbool operator>(cfl32 &value) const { return data < toFixed(value); }
   inline cbool operator<(cfl32 &value) const { return data > toFixed(value); }
   inline cbool operator||(cfl32 &value) const { return data || toFixed(value); }
   inline cbool operator&&(cfl32 &value) const { return data && toFixed(value); }
};

#endif

// 32-bit, 0.32 : Decimal range of 0.0~0.99999999976716935634613037109375
struct f0p32 {
   typedef const f0p32 cf0p32;

   ui32 data;

   inline cui32 toFixed(cfl64 &value) const { return ui32(value * 4294967296.0); }
   inline cfl64 toFloat(void) const { return cfl64(data) * _fpdt_rcp2p32; }
   inline cfl64 toFloat(cf0p32 &value) const { return fl64(value.data) * _fpdt_rcp2p32; }

   f0p32(void) = default;
   f0p32(const fp8n value) { data = toFixed(fl64(value) * _fpdt_data_.rangeDivMax8 + _fpdt_data_.origin8); }
   f0p32(const fp16n value) { data = toFixed(fl64(value) * _fpdt_data_.rangeDivMax16 + _fpdt_data_.origin16); }
#ifdef _24BIT_INTEGERS_
   f0p32(const fp24n value) { data = toFixed(fl64(value) * _fpdt_data_.rangeDivMax24 + _fpdt_data_.origin24); }
#endif
   f0p32(const fp32n value) { data = toFixed(fl64(value) * _fpdt_data_.rangeDivMax32 + _fpdt_data_.origin32); }
   f0p32(cui32 value) { data = value; }
   f0p32(csi32 value) { data = (ui32 &)value; }
   f0p32(csi64 value) { data = (ui32 &)value; }
   f0p32(cfl64 value) { data = toFixed(value); }

   operator ptr(void) const { return *this; }
   operator cfl64(void) const { return cfl64(data) * _fpdt_rcp2p32; }

   inline cf0p32 &operator&(void) const { return *this; }
   inline cf0p32 operator~(void) const { return (cf0p32 &)~data; }
   inline cf0p32 operator-(void) const { return (cf0p32 &)~data; }
   inline cbool operator!(void) const { return data == 0; }

   inline cf0p32 operator++(void) { data++; return *this; }
   inline cf0p32 operator++(int) { cui32 temp = data++; return (cf0p32 &)temp; }
   inline cf0p32 operator--(void) { data--; return *this; }
   inline cf0p32 operator--(int) { cui32 temp = data--; return (cf0p32 &)temp; }

   inline cbool operator==(csi32 &value) const { return data == (ui32 &)value; }
   inline cbool operator!=(csi32 &value) const { return data != (ui32 &)value; }
   inline cbool operator>=(csi32 &value) const { return data >= (ui32 &)value; }
   inline cbool operator<=(csi32 &value) const { return data <= (ui32 &)value; }
   inline cbool operator>(csi32 &value) const { return data > (ui32 &)value; }
   inline cbool operator<(csi32 &value) const { return data < (ui32 &)value; }
   inline cbool operator||(csi32 &value) const { return data || (ui32 &)value; }
   inline cbool operator&&(csi32 &value) const { return data && (ui32 &)value; }

   inline cf0p32 operator<<(csi32 &value) const { return (cf0p32 &)(data << value); }
   inline cf0p32 operator>>(csi32 &value) const { return (cf0p32 &)(data >> value); }
   inline cf0p32 operator<<=(csi32 &value) { return (cf0p32 &)(data <<= value); }
   inline cf0p32 operator>>=(csi32 &value) { return (cf0p32 &)(data >>= value); }

   inline cbool operator==(cf0p32 &value) const { return data == value.data; }
   inline cbool operator!=(cf0p32 &value) const { return data != value.data; }
   inline cbool operator>=(cf0p32 &value) const { return data >= value.data; }
   inline cbool operator<=(cf0p32 &value) const { return data <= value.data; }
   inline cbool operator>(cf0p32 &value) const { return data > value.data; }
   inline cbool operator<(cf0p32 &value) const { return data < value.data; }
   inline cbool operator||(cf0p32 &value) const { return data || value.data; }
   inline cbool operator&&(cf0p32 &value) const { return data && value.data; }

   inline cf0p32 operator+(cf0p32 &value) const { return (cf0p32 &)(data + value.data); }
   inline cf0p32 operator-(cf0p32 &value) const { return (cf0p32 &)(data - value.data); }
   inline cf0p32 operator*(cf0p32 &value) const { return toFixed(toFloat() * toFloat(value)); }
   inline cf0p32 operator/(cf0p32 &value) const { return toFixed(toFloat() / toFloat(value)); }
   inline cf0p32 operator%(cf0p32 &value) const { return (cf0p32 &)(data % value.data); }
   inline cf0p32 operator&(cf0p32 &value) const { return (cf0p32 &)(data & value.data); }
   inline cf0p32 operator|(cf0p32 &value) const { return (cf0p32 &)(data | value.data); }
   inline cf0p32 operator^(cf0p32 &value) const { return (cf0p32 &)(data ^ value.data); }
   inline cf0p32 operator+=(cf0p32 &value) { return (cf0p32 &)(data += value.data); }
   inline cf0p32 operator-=(cf0p32 &value) { return (cf0p32 &)(data -= value.data); }
   inline cf0p32 operator*=(cf0p32 &value) { return data = toFixed(toFloat() * toFloat(value)); }
   inline cf0p32 operator/=(cf0p32 &value) { return data = toFixed(toFloat() / toFloat(value)); }
   inline cf0p32 operator%=(cf0p32 &value) { return (cf0p32 &)(data %= value.data); }
   inline cf0p32 operator&=(cf0p32 &value) { return (cf0p32 &)(data &= value.data); }
   inline cf0p32 operator|=(cf0p32 &value) { return (cf0p32 &)(data |= value.data); }
   inline cf0p32 operator^=(cf0p32 &value) { return (cf0p32 &)(data ^= value.data); }

   inline cfl64 operator+(cfl64 &value) const { return toFloat() + value; }
   inline cfl64 operator-(cfl64 &value) const { return toFloat() - value; }
   inline cfl64 operator*(cfl64 &value) const { return toFloat() * value; }
   inline cfl64 operator/(cfl64 &value) const { return toFloat() / value; }
   inline cf0p32 operator%(cfl64 &value) const { return (cf0p32 &)(data % toFixed(value)); }
   inline cf0p32 operator&(cfl64 &value) const { return (cf0p32 &)(data & toFixed(value)); }
   inline cf0p32 operator|(cfl64 &value) const { return (cf0p32 &)(data | toFixed(value)); }
   inline cf0p32 operator^(cfl64 &value) const { return (cf0p32 &)(data ^ toFixed(value)); }
   inline cfl64 operator+=(cfl64 &value) { cfl64 temp = toFloat() + value; *this = toFixed(temp); return temp; }
   inline cfl64 operator-=(cfl64 &value) { cfl64 temp = toFloat() - value; *this = toFixed(temp); return temp; }
   inline cfl64 operator*=(cfl64 &value) { cfl64 temp = toFloat() * value; *this = toFixed(temp); return temp; }
   inline cfl64 operator/=(cfl64 &value) { cfl64 temp = toFloat() / value; *this = toFixed(temp); return temp; }
   inline cf0p32 operator%=(cfl64 &value) { return (cf0p32 &)(data %= toFixed(value)); }
   inline cf0p32 operator&=(cfl64 &value) { return (cf0p32 &)(data &= toFixed(value)); }
   inline cf0p32 operator|=(cfl64 &value) { return (cf0p32 &)(data |= toFixed(value)); }
   inline cf0p32 operator^=(cfl64 &value) { return (cf0p32 &)(data ^= toFixed(value)); }

   inline cbool operator==(cfl64 &value) const { return data == toFixed(value); }
   inline cbool operator!=(cfl64 &value) const { return data != toFixed(value); }
   inline cbool operator>=(cfl64 &value) const { return data <= toFixed(value); }
   inline cbool operator<=(cfl64 &value) const { return data >= toFixed(value); }
   inline cbool operator>(cfl64 &value) const { return data < toFixed(value); }
   inline cbool operator<(cfl64 &value) const { return data > toFixed(value); }
   inline cbool operator||(cfl64 &value) const { return data || toFixed(value); }
   inline cbool operator&&(cfl64 &value) const { return data && toFixed(value); }
};

// 32-bit, 16.16 : Decimal range of 0.0~65535.9999847412109375
struct f16p16 {
   typedef const f16p16 cf16p16;

   ui32 data;

   inline cui32 toFixed(cfl64 &value) const { return ui32(value * 65536.0); }
   inline cfl64 toFloat(void) const { return cfl64(data) * _fpdt_rcp65536; }
   inline cfl64 toFloat(cf16p16 &value) const { return fl64(value.data) * _fpdt_rcp65536; }

   f16p16(void) = default;
   f16p16(const fp8n value) { data = toFixed(fl64(value) * _fpdt_data_.rangeDivMax8 + _fpdt_data_.origin8); }
   f16p16(const fp16n value) { data = toFixed(fl64(value) * _fpdt_data_.rangeDivMax16 + _fpdt_data_.origin16); }
#ifdef _24BIT_INTEGERS_
   f16p16(const fp24n value) { data = toFixed(fl64(value) * _fpdt_data_.rangeDivMax24 + _fpdt_data_.origin24); }
#endif
   f16p16(const fp32n value) { data = toFixed(fl64(value) * _fpdt_data_.rangeDivMax32 + _fpdt_data_.origin32); }
   f16p16(cui32 value) { data = value; }
   f16p16(csi32 value) { data = (ui32 &)value; }
   f16p16(csi64 value) { data = (ui32 &)value; }
   f16p16(cfl64 value) { data = toFixed(value); }

   operator ptr(void) const { return *this; }
   operator cfl64(void) const { return cfl64(data) * _fpdt_rcp65536; }

   inline cf16p16 &operator&(void) const { return *this; }
   inline cf16p16 operator~(void) const { return (cf16p16 &)~data; }
   inline cf16p16 operator-(void) const { return (cf16p16 &)~data; }
   inline cbool operator!(void) const { return data == 0; }

   inline cf16p16 operator++(void) { data++; return *this; }
   inline cf16p16 operator++(int) { cui32 temp = data++; return (cf16p16 &)temp; }
   inline cf16p16 operator--(void) { data--; return *this; }
   inline cf16p16 operator--(int) { cui32 temp = data--; return (cf16p16 &)temp; }

   inline cbool operator==(csi32 &value) const { return data == (ui32 &)value; }
   inline cbool operator!=(csi32 &value) const { return data != (ui32 &)value; }
   inline cbool operator>=(csi32 &value) const { return data >= (ui32 &)value; }
   inline cbool operator<=(csi32 &value) const { return data <= (ui32 &)value; }
   inline cbool operator>(csi32 &value) const { return data > (ui32 &)value; }
   inline cbool operator<(csi32 &value) const { return data < (ui32 &)value; }
   inline cbool operator||(csi32 &value) const { return data || (ui32 &)value; }
   inline cbool operator&&(csi32 &value) const { return data && (ui32 &)value; }

   inline cf16p16 operator<<(csi32 &value) const { return (cf16p16 &)(data << value); }
   inline cf16p16 operator>>(csi32 &value) const { return (cf16p16 &)(data >> value); }
   inline cf16p16 operator<<=(csi32 &value) { return (cf16p16 &)(data <<= value); }
   inline cf16p16 operator>>=(csi32 &value) { return (cf16p16 &)(data >>= value); }

   inline cbool operator==(cf16p16 &value) const { return data == value.data; }
   inline cbool operator!=(cf16p16 &value) const { return data != value.data; }
   inline cbool operator>=(cf16p16 &value) const { return data >= value.data; }
   inline cbool operator<=(cf16p16 &value) const { return data <= value.data; }
   inline cbool operator>(cf16p16 &value) const { return data > value.data; }
   inline cbool operator<(cf16p16 &value) const { return data < value.data; }
   inline cbool operator||(cf16p16 &value) const { return data || value.data; }
   inline cbool operator&&(cf16p16 &value) const { return data && value.data; }

   inline cf16p16 operator+(cf16p16 &value) const { return (cf16p16 &)(data + value.data); }
   inline cf16p16 operator-(cf16p16 &value) const { return (cf16p16 &)(data - value.data); }
   inline cf16p16 operator*(cf16p16 &value) const { return toFixed(toFloat() * toFloat(value)); }
   inline cf16p16 operator/(cf16p16 &value) const { return toFixed(toFloat() / toFloat(value)); }
   inline cf16p16 operator%(cf16p16 &value) const { return (cf16p16 &)(data % value.data); }
   inline cf16p16 operator&(cf16p16 &value) const { return (cf16p16 &)(data & value.data); }
   inline cf16p16 operator|(cf16p16 &value) const { return (cf16p16 &)(data | value.data); }
   inline cf16p16 operator^(cf16p16 &value) const { return (cf16p16 &)(data ^ value.data); }
   inline cf16p16 operator+=(cf16p16 &value) { return (cf16p16 &)(data += value.data); }
   inline cf16p16 operator-=(cf16p16 &value) { return (cf16p16 &)(data -= value.data); }
   inline cf16p16 operator*=(cf16p16 &value) { return data = toFixed(toFloat() * toFloat(value)); }
   inline cf16p16 operator/=(cf16p16 &value) { return data = toFixed(toFloat() / toFloat(value)); }
   inline cf16p16 operator%=(cf16p16 &value) { return (cf16p16 &)(data %= value.data); }
   inline cf16p16 operator&=(cf16p16 &value) { return (cf16p16 &)(data &= value.data); }
   inline cf16p16 operator|=(cf16p16 &value) { return (cf16p16 &)(data |= value.data); }
   inline cf16p16 operator^=(cf16p16 &value) { return (cf16p16 &)(data ^= value.data); }

   inline cfl64 operator+(cfl64 &value) const { return toFloat() + value; }
   inline cfl64 operator-(cfl64 &value) const { return toFloat() - value; }
   inline cfl64 operator*(cfl64 &value) const { return toFloat() * value; }
   inline cfl64 operator/(cfl64 &value) const { return toFloat() / value; }
   inline cf16p16 operator%(cfl64 &value) const { return (cf16p16 &)(data % toFixed(value)); }
   inline cf16p16 operator&(cfl64 &value) const { return (cf16p16 &)(data & toFixed(value)); }
   inline cf16p16 operator|(cfl64 &value) const { return (cf16p16 &)(data | toFixed(value)); }
   inline cf16p16 operator^(cfl64 &value) const { return (cf16p16 &)(data ^ toFixed(value)); }
   inline cfl64 operator+=(cfl64 &value) { cfl64 temp = toFloat() + value; *this = toFixed(temp); return temp; }
   inline cfl64 operator-=(cfl64 &value) { cfl64 temp = toFloat() - value; *this = toFixed(temp); return temp; }
   inline cfl64 operator*=(cfl64 &value) { cfl64 temp = toFloat() * value; *this = toFixed(temp); return temp; }
   inline cfl64 operator/=(cfl64 &value) { cfl64 temp = toFloat() / value; *this = toFixed(temp); return temp; }
   inline cf16p16 operator%=(cfl64 &value) { return (cf16p16 &)(data %= toFixed(value)); }
   inline cf16p16 operator&=(cfl64 &value) { return (cf16p16 &)(data &= toFixed(value)); }
   inline cf16p16 operator|=(cfl64 &value) { return (cf16p16 &)(data |= toFixed(value)); }
   inline cf16p16 operator^=(cfl64 &value) { return (cf16p16 &)(data ^= toFixed(value)); }

   inline cbool operator==(cfl64 &value) const { return data == toFixed(value); }
   inline cbool operator!=(cfl64 &value) const { return data != toFixed(value); }
   inline cbool operator>=(cfl64 &value) const { return data <= toFixed(value); }
   inline cbool operator<=(cfl64 &value) const { return data >= toFixed(value); }
   inline cbool operator>(cfl64 &value) const { return data < toFixed(value); }
   inline cbool operator<(cfl64 &value) const { return data > toFixed(value); }
   inline cbool operator||(cfl64 &value) const { return data || toFixed(value); }
   inline cbool operator&&(cfl64 &value) const { return data && toFixed(value); }
};

// Normalised 32-bit : Decimal range of 0.0~1.0
struct fp32n0_1 {
   typedef const fp32n0_1 cfp32n0_1;

   ui32 data;

   inline cui32 toFixed(cfl64 &value) const { return ui32(value * 4294967295.0); }
   inline cfl64 toFloat(void) const { return cfl64(data) * _fpdt_rcp2p32_1; }
   inline cfl64 toFloat(cfp32n0_1 &value) const { return fl64(value.data) * _fpdt_rcp2p32_1; }

   fp32n0_1(void) = default;
   fp32n0_1(const fp8n value) { data = toFixed(fl64(value) * _fpdt_data_.rangeDivMax8 + _fpdt_data_.origin8); }
   fp32n0_1(const fp16n value) { data = toFixed(fl64(value) * _fpdt_data_.rangeDivMax16 + _fpdt_data_.origin16); }
#ifdef _24BIT_INTEGERS_
   fp32n0_1(const fp24n value) { data = toFixed(fl64(value) * _fpdt_data_.rangeDivMax24 + _fpdt_data_.origin24); }
#endif
   fp32n0_1(const fp32n value) { data = toFixed(fl64(value) * _fpdt_data_.rangeDivMax32 + _fpdt_data_.origin32); }
   fp32n0_1(cui32 value) { data = value; }
   fp32n0_1(csi32 value) { data = (ui32 &)value; }
   fp32n0_1(csi64 value) { data = (ui32 &)value; }
   fp32n0_1(cfl64 value) { data = toFixed(value); }

   operator ptr(void) const { return *this; }
   operator cfl64(void) const { return cfl64(data) * _fpdt_rcp2p32_1; }

   inline cfp32n0_1 &operator&(void) const { return *this; }
   inline cfp32n0_1 operator~(void) const { return (cfp32n0_1 &)~data; }
   inline cfp32n0_1 operator-(void) const { return (cfp32n0_1 &)~data; }
   inline cbool operator!(void) const { return data == 0; }

   inline cfp32n0_1 operator++(void) { data++; return *this; }
   inline cfp32n0_1 operator++(int) { cui32 temp = data++; return (cfp32n0_1 &)temp; }
   inline cfp32n0_1 operator--(void) { data--; return *this; }
   inline cfp32n0_1 operator--(int) { cui32 temp = data--; return (cfp32n0_1 &)temp; }

   inline cbool operator==(csi32 &value) const { return data == (ui32 &)value; }
   inline cbool operator!=(csi32 &value) const { return data != (ui32 &)value; }
   inline cbool operator>=(csi32 &value) const { return data >= (ui32 &)value; }
   inline cbool operator<=(csi32 &value) const { return data <= (ui32 &)value; }
   inline cbool operator>(csi32 &value) const { return data > (ui32 &)value; }
   inline cbool operator<(csi32 &value) const { return data < (ui32 &)value; }
   inline cbool operator||(csi32 &value) const { return data || (ui32 &)value; }
   inline cbool operator&&(csi32 &value) const { return data && (ui32 &)value; }

   inline cfp32n0_1 operator<<(csi32 &value) const { return (cfp32n0_1 &)(data << value); }
   inline cfp32n0_1 operator>>(csi32 &value) const { return (cfp32n0_1 &)(data >> value); }
   inline cfp32n0_1 operator<<=(csi32 &value) { return (cfp32n0_1 &)(data <<= value); }
   inline cfp32n0_1 operator>>=(csi32 &value) { return (cfp32n0_1 &)(data >>= value); }

   inline cbool operator==(cfp32n0_1 &value) const { return data == value.data; }
   inline cbool operator!=(cfp32n0_1 &value) const { return data != value.data; }
   inline cbool operator>=(cfp32n0_1 &value) const { return data >= value.data; }
   inline cbool operator<=(cfp32n0_1 &value) const { return data <= value.data; }
   inline cbool operator>(cfp32n0_1 &value) const { return data > value.data; }
   inline cbool operator<(cfp32n0_1 &value) const { return data < value.data; }
   inline cbool operator||(cfp32n0_1 &value) const { return data || value.data; }
   inline cbool operator&&(cfp32n0_1 &value) const { return data && value.data; }

   inline cfp32n0_1 operator+(cfp32n0_1 &value) const { return (cfp32n0_1 &)(data + value.data); }
   inline cfp32n0_1 operator-(cfp32n0_1 &value) const { return (cfp32n0_1 &)(data - value.data); }
   inline cfp32n0_1 operator*(cfp32n0_1 &value) const { return toFixed(toFloat() * toFloat(value)); }
   inline cfp32n0_1 operator/(cfp32n0_1 &value) const { return toFixed(toFloat() / toFloat(value)); }
   inline cfp32n0_1 operator%(cfp32n0_1 &value) const { return (cfp32n0_1 &)(data % value.data); }
   inline cfp32n0_1 operator&(cfp32n0_1 &value) const { return (cfp32n0_1 &)(data & value.data); }
   inline cfp32n0_1 operator|(cfp32n0_1 &value) const { return (cfp32n0_1 &)(data | value.data); }
   inline cfp32n0_1 operator^(cfp32n0_1 &value) const { return (cfp32n0_1 &)(data ^ value.data); }
   inline cfp32n0_1 operator+=(cfp32n0_1 &value) { return (cfp32n0_1 &)(data += value.data); }
   inline cfp32n0_1 operator-=(cfp32n0_1 &value) { return (cfp32n0_1 &)(data -= value.data); }
   inline cfp32n0_1 operator*=(cfp32n0_1 &value) { return data = toFixed(toFloat() * toFloat(value)); }
   inline cfp32n0_1 operator/=(cfp32n0_1 &value) { return data = toFixed(toFloat() / toFloat(value)); }
   inline cfp32n0_1 operator%=(cfp32n0_1 &value) { return (cfp32n0_1 &)(data %= value.data); }
   inline cfp32n0_1 operator&=(cfp32n0_1 &value) { return (cfp32n0_1 &)(data &= value.data); }
   inline cfp32n0_1 operator|=(cfp32n0_1 &value) { return (cfp32n0_1 &)(data |= value.data); }
   inline cfp32n0_1 operator^=(cfp32n0_1 &value) { return (cfp32n0_1 &)(data ^= value.data); }

   inline cfl64 operator+(cfl64 &value) const { return toFloat() + value; }
   inline cfl64 operator-(cfl64 &value) const { return toFloat() - value; }
   inline cfl64 operator*(cfl64 &value) const { return toFloat() * value; }
   inline cfl64 operator/(cfl64 &value) const { return toFloat() / value; }
   inline cfp32n0_1 operator%(cfl64 &value) const { return (cfp32n0_1 &)(data % toFixed(value)); }
   inline cfp32n0_1 operator&(cfl64 &value) const { return (cfp32n0_1 &)(data & toFixed(value)); }
   inline cfp32n0_1 operator|(cfl64 &value) const { return (cfp32n0_1 &)(data | toFixed(value)); }
   inline cfp32n0_1 operator^(cfl64 &value) const { return (cfp32n0_1 &)(data ^ toFixed(value)); }
   inline cfl64 operator+=(cfl64 &value) { cfl64 temp = toFloat() + value; *this = toFixed(temp); return temp; }
   inline cfl64 operator-=(cfl64 &value) { cfl64 temp = toFloat() - value; *this = toFixed(temp); return temp; }
   inline cfl64 operator*=(cfl64 &value) { cfl64 temp = toFloat() * value; *this = toFixed(temp); return temp; }
   inline cfl64 operator/=(cfl64 &value) { cfl64 temp = toFloat() / value; *this = toFixed(temp); return temp; }
   inline cfp32n0_1 operator%=(cfl64 &value) { return (cfp32n0_1 &)(data %= toFixed(value)); }
   inline cfp32n0_1 operator&=(cfl64 &value) { return (cfp32n0_1 &)(data &= toFixed(value)); }
   inline cfp32n0_1 operator|=(cfl64 &value) { return (cfp32n0_1 &)(data |= toFixed(value)); }
   inline cfp32n0_1 operator^=(cfl64 &value) { return (cfp32n0_1 &)(data ^= toFixed(value)); }

   inline cbool operator==(cfl64 &value) const { return data == toFixed(value); }
   inline cbool operator!=(cfl64 &value) const { return data != toFixed(value); }
   inline cbool operator>=(cfl64 &value) const { return data <= toFixed(value); }
   inline cbool operator<=(cfl64 &value) const { return data >= toFixed(value); }
   inline cbool operator>(cfl64 &value) const { return data < toFixed(value); }
   inline cbool operator<(cfl64 &value) const { return data > toFixed(value); }
   inline cbool operator||(cfl64 &value) const { return data || toFixed(value); }
   inline cbool operator&&(cfl64 &value) const { return data && toFixed(value); }
};

// Normalised 32-bit : Decimal range of -1.0~1.0
struct fp32n_1_1 {
   typedef const fp32n_1_1 cfp32n_1_1;

   ui32 data;

   inline cui32 toFixed(cfl64 &value) const { return ui32((value + 1.0) * 2147483647.5); }
   inline cfl64 toFloat(void) const { return cfl64(data) * _fpdt_2div2p32_1; }
   inline cfl64 toFloat(cfp32n_1_1 &value) const { return fl64(value.data) * _fpdt_2div2p32_1; }

   fp32n_1_1(void) = default;
   fp32n_1_1(const fp8n value) { data = toFixed(fl64(value) * _fpdt_data_.rangeDivMax8 + _fpdt_data_.origin8); }
   fp32n_1_1(const fp16n value) { data = toFixed(fl64(value) * _fpdt_data_.rangeDivMax16 + _fpdt_data_.origin16); }
#ifdef _24BIT_INTEGERS_
   fp32n_1_1(const fp24n value) { data = toFixed(fl64(value) * _fpdt_data_.rangeDivMax24 + _fpdt_data_.origin24); }
#endif
   fp32n_1_1(const fp32n value) { data = toFixed(fl64(value) * _fpdt_data_.rangeDivMax32 + _fpdt_data_.origin32); }
   fp32n_1_1(cui32 value) { data = value; }
   fp32n_1_1(csi32 value) { data = (ui32 &)value; }
   fp32n_1_1(csi64 value) { data = (ui32 &)value; }
   fp32n_1_1(cfl64 value) { data = toFixed(value); }

   operator ptr(void) const { return *this; }
   operator cfl64(void) const { return cfl64(data) * _fpdt_2div2p32_1; }

   inline cfp32n_1_1 &operator&(void) const { return *this; }
   inline cfp32n_1_1 operator~(void) const { return (cfp32n_1_1 &)~data; }
   inline cfp32n_1_1 operator-(void) const { return (cfp32n_1_1 &)~data; }
   inline cbool operator!(void) const { return data == 0; }

   inline cfp32n_1_1 operator++(void) { data++; return *this; }
   inline cfp32n_1_1 operator++(int) { cui32 temp = data++; return (cfp32n_1_1 &)temp; }
   inline cfp32n_1_1 operator--(void) { data--; return *this; }
   inline cfp32n_1_1 operator--(int) { cui32 temp = data--; return (cfp32n_1_1 &)temp; }

   inline cbool operator==(csi32 &value) const { return data == (ui32 &)value; }
   inline cbool operator!=(csi32 &value) const { return data != (ui32 &)value; }
   inline cbool operator>=(csi32 &value) const { return data >= (ui32 &)value; }
   inline cbool operator<=(csi32 &value) const { return data <= (ui32 &)value; }
   inline cbool operator>(csi32 &value) const { return data > (ui32 &)value; }
   inline cbool operator<(csi32 &value) const { return data < (ui32 &)value; }
   inline cbool operator||(csi32 &value) const { return data || (ui32 &)value; }
   inline cbool operator&&(csi32 &value) const { return data && (ui32 &)value; }

   inline cfp32n_1_1 operator<<(csi32 &value) const { return (cfp32n_1_1 &)(data << value); }
   inline cfp32n_1_1 operator>>(csi32 &value) const { return (cfp32n_1_1 &)(data >> value); }
   inline cfp32n_1_1 operator<<=(csi32 &value) { return (cfp32n_1_1 &)(data <<= value); }
   inline cfp32n_1_1 operator>>=(csi32 &value) { return (cfp32n_1_1 &)(data >>= value); }

   inline cbool operator==(cfp32n_1_1 &value) const { return data == value.data; }
   inline cbool operator!=(cfp32n_1_1 &value) const { return data != value.data; }
   inline cbool operator>=(cfp32n_1_1 &value) const { return data >= value.data; }
   inline cbool operator<=(cfp32n_1_1 &value) const { return data <= value.data; }
   inline cbool operator>(cfp32n_1_1 &value) const { return data > value.data; }
   inline cbool operator<(cfp32n_1_1 &value) const { return data < value.data; }
   inline cbool operator||(cfp32n_1_1 &value) const { return data || value.data; }
   inline cbool operator&&(cfp32n_1_1 &value) const { return data && value.data; }

   inline cfp32n_1_1 operator+(cfp32n_1_1 &value) const { return (cfp32n_1_1 &)(data + value.data - 2147483647); }
   inline cfp32n_1_1 operator-(cfp32n_1_1 &value) const { return (cfp32n_1_1 &)(data - value.data - 2147483647); }
   inline cfp32n_1_1 operator*(cfp32n_1_1 &value) const { return toFixed(toFloat() * toFloat(value)); }
   inline cfp32n_1_1 operator/(cfp32n_1_1 &value) const { return toFixed(toFloat() / toFloat(value)); }
   inline cfp32n_1_1 operator%(cfp32n_1_1 &value) const { return (cfp32n_1_1 &)(data % value.data); }
   inline cfp32n_1_1 operator&(cfp32n_1_1 &value) const { return (cfp32n_1_1 &)(data & value.data); }
   inline cfp32n_1_1 operator|(cfp32n_1_1 &value) const { return (cfp32n_1_1 &)(data | value.data); }
   inline cfp32n_1_1 operator^(cfp32n_1_1 &value) const { return (cfp32n_1_1 &)(data ^ value.data); }
   inline cfp32n_1_1 operator+=(cfp32n_1_1 &value) { return (cfp32n_1_1 &)(data += value.data - 2147483647); }
   inline cfp32n_1_1 operator-=(cfp32n_1_1 &value) { return (cfp32n_1_1 &)(data -= value.data - 2147483647); }
   inline cfp32n_1_1 operator*=(cfp32n_1_1 &value) { return data = toFixed(toFloat() * toFloat(value)); }
   inline cfp32n_1_1 operator/=(cfp32n_1_1 &value) { return data = toFixed(toFloat() / toFloat(value)); }
   inline cfp32n_1_1 operator%=(cfp32n_1_1 &value) { return (cfp32n_1_1 &)(data %= value.data); }
   inline cfp32n_1_1 operator&=(cfp32n_1_1 &value) { return (cfp32n_1_1 &)(data &= value.data); }
   inline cfp32n_1_1 operator|=(cfp32n_1_1 &value) { return (cfp32n_1_1 &)(data |= value.data); }
   inline cfp32n_1_1 operator^=(cfp32n_1_1 &value) { return (cfp32n_1_1 &)(data ^= value.data); }

   inline cfl64 operator+(cfl64 &value) const { return toFloat() + value; }
   inline cfl64 operator-(cfl64 &value) const { return toFloat() - value; }
   inline cfl64 operator*(cfl64 &value) const { return toFloat() * value; }
   inline cfl64 operator/(cfl64 &value) const { return toFloat() / value; }
   inline cfp32n_1_1 operator%(cfl64 &value) const { return (cfp32n_1_1 &)(data % toFixed(value)); }
   inline cfp32n_1_1 operator&(cfl64 &value) const { return (cfp32n_1_1 &)(data & toFixed(value)); }
   inline cfp32n_1_1 operator|(cfl64 &value) const { return (cfp32n_1_1 &)(data | toFixed(value)); }
   inline cfp32n_1_1 operator^(cfl64 &value) const { return (cfp32n_1_1 &)(data ^ toFixed(value)); }
   inline cfl64 operator+=(cfl64 &value) { cfl64 temp = toFloat() + value; *this = toFixed(temp); return temp; }
   inline cfl64 operator-=(cfl64 &value) { cfl64 temp = toFloat() - value; *this = toFixed(temp); return temp; }
   inline cfl64 operator*=(cfl64 &value) { cfl64 temp = toFloat() * value; *this = toFixed(temp); return temp; }
   inline cfl64 operator/=(cfl64 &value) { cfl64 temp = toFloat() / value; *this = toFixed(temp); return temp; }
   inline cfp32n_1_1 operator%=(cfl64 &value) { return (cfp32n_1_1 &)(data %= toFixed(value)); }
   inline cfp32n_1_1 operator&=(cfl64 &value) { return (cfp32n_1_1 &)(data &= toFixed(value)); }
   inline cfp32n_1_1 operator|=(cfl64 &value) { return (cfp32n_1_1 &)(data |= toFixed(value)); }
   inline cfp32n_1_1 operator^=(cfl64 &value) { return (cfp32n_1_1 &)(data ^= toFixed(value)); }

   inline cbool operator==(cfl64 &value) const { return data == toFixed(value); }
   inline cbool operator!=(cfl64 &value) const { return data != toFixed(value); }
   inline cbool operator>=(cfl64 &value) const { return data <= toFixed(value); }
   inline cbool operator<=(cfl64 &value) const { return data >= toFixed(value); }
   inline cbool operator>(cfl64 &value) const { return data < toFixed(value); }
   inline cbool operator<(cfl64 &value) const { return data > toFixed(value); }
   inline cbool operator||(cfl64 &value) const { return data || toFixed(value); }
   inline cbool operator&&(cfl64 &value) const { return data && toFixed(value); }
};

// 2x 8-bit, 0.8 : Decimal range of 0.0~0.99609375
struct f0p8x2 { f0p8 x, y; f0p8x2(cfl32 value) { x = y = value; } f0p8x2(cfl32 valX, cfl32 valY) { x = valX; y = valY; } f0p8x2(cf32x2 value) { x = value.x; y = value.y; } };
// 2x 8-bit, 1.7 : Decimal range of 0.0~1.9921875
struct f1p7x2 { f1p7 x, y; f1p7x2(cfl32 value) { x = y = value; } f1p7x2(cfl32 valX, cfl32 valY) { x = valX; y = valY; } f1p7x2(cf32x2 value) { x = value.x; y = value.y; } };
// 2x 8-bit, 4.4 : Decimal range of 0.0~15.9375
struct f4p4x2 { f4p4 x, y; f4p4x2(cfl32 value) { x = y = value; } f4p4x2(cfl32 valX, cfl32 valY) { x = valX; y = valY; } f4p4x2(cf32x2 value) { x = value.x; y = value.y; } };
// 2x Normalised 8-bit : Decimal range of 0.0~1.0
struct fp8n0_1x2 { fp8n0_1 x, y; fp8n0_1x2(cfl32 value) { x = y = value; } fp8n0_1x2(cfl32 valX, cfl32 valY) { x = valX; y = valY; } fp8n0_1x2(cf32x2 value) { x = value.x; y = value.y; } };
// 2x 16-bit, 0.16 : Decimal range of 0.0~0.9999847412109375
struct f0p16x2 { f0p16 x, y; f0p16x2(cfl32 value) { x = y = value; } f0p16x2(cfl32 valX, cfl32 valY) { x = valX; y = valY; } f0p16x2(cf32x2 value) { x = value.x; y = value.y; } };
// 2x 16-bit, signed 1.14 : Decimal range of -2.0~1.99993896484375
struct fs1p14x2 { fs1p14 x, y; fs1p14x2(cfl32 value) { x = y = value; } fs1p14x2(cfl32 valX, cfl32 valY) { x = valX; y = valY; } fs1p14x2(cf32x2 value) { x = value.x; y = value.y; } };
// 2x 16-bit, 1.15 : Decimal range of 0.0~1.999969482421875
struct f1p15x2 { f1p15 x, y; f1p15x2(cfl32 value) { x = y = value; } f1p15x2(cfl32 valX, cfl32 valY) { x = valX; y = valY; } f1p15x2(cf32x2 value) { x = value.x; y = value.y; } };
// 2x 16-bit, 6.10 : Decimal range of 0.0~63.9990234375
struct f6p10x2 { f6p10 x, y; f6p10x2(cfl32 value) { x = y = value; } f6p10x2(cfl32 valX, cfl32 valY) { x = valX; y = valY; } f6p10x2(cf32x2 value) { x = value.x; y = value.y; } };
// 2x 16-bit, signed 7.8 : Decimal range of -128.0~127.99609375
struct fs7p8x2 { fs7p8 x, y; fs7p8x2(cfl32 value) { x = y = value; } fs7p8x2(cfl32 valX, cfl32 valY) { x = valX; y = valY; } fs7p8x2(cf32x2 value) { x = value.x; y = value.y; } };
// 2x 16-bit, 7.9 : Decimal range of 0.0~127.998046875
struct f7p9x2 { f7p9 x, y; f7p9x2(cfl32 value) { x = y = value; } f7p9x2(cfl32 valX, cfl32 valY) { x = valX; y = valY; } f7p9x2(cf32x2 value) { x = value.x; y = value.y; } };
// 2x 16-bit, 8.8 : Decimal range of 0.0~255.99609375
struct f8p8x2 { f8p8 x, y; f8p8x2(cfl32 value) { x = y = value; } f8p8x2(cfl32 valX, cfl32 valY) { x = valX; y = valY; } f8p8x2(cf32x2 value) { x = value.x; y = value.y; } };
// 2x Normalised 16-bit : Decimal range of 0.0~1.0
struct fp16n0_1x2 { fp16n0_1 x, y; fp16n0_1x2(cfl32 value) { x = y = value; } fp16n0_1x2(cfl32 valX, cfl32 valY) { x = valX; y = valY; } fp16n0_1x2(cf32x2 value) { x = value.x; y = value.y; } };
// 2x Normalised 16-bit : Decimal range of -1.0~1.0
struct fp16n_1_1x2 { fp16n_1_1 x, y; fp16n_1_1x2(cfl32 value) { x = y = value; } fp16n_1_1x2(cfl32 valX, cfl32 valY) { x = valX; y = valY; } fp16n_1_1x2(cf32x2 value) { x = value.x; y = value.y; } };

#ifndef FPDT_NO_CUSTOM

typedef const fp8n    cfp8n;    // Normalised 8-bit : User-defineable decimal range
typedef const fp8nx4  cfp8nx4;  // 4x normalised 8-bit : User-defineable decimal range
typedef const fp16n   cfp16n;   // Normalised 16-bit : User-defineable decimal range
typedef const fp16nx4 cfp16nx4; // 4x normalised 16-bit : User-defineable decimal range
#ifdef _24BIT_INTEGERS_
typedef const fp24n   cfp24n;   // Normalised 24-bit : User-defineable decimal range
#endif
typedef const fp32n   cfp32n;   // Normalised 32-bit : User-defineable decimal range

typedef const fp8n    vfp8n;    // Normalised 8-bit : User-defineable decimal range
typedef const fp8nx4  vfp8nx4;  // 4x normalised 8-bit : User-defineable decimal range
typedef const fp16n   vfp16n;   // Normalised 16-bit : User-defineable decimal range
typedef const fp16nx4 vfp16nx4; // 4x normalised 16-bit : User-defineable decimal range
#ifdef _24BIT_INTEGERS_
typedef const fp24n   vfp24n;   // Normalised 24-bit : User-defineable decimal range
#endif
typedef const fp32n   vfp32n;   // Normalised 32-bit : User-defineable decimal range

#endif

typedef const f0p8      cf0p8;      // 8-bit, 0.8 : Decimal range of 0.0~0.99609375
typedef const f0p8x2    cf0p8x2;    // 2x 8-bit, 0.8 : Decimal range of 0.0~0.99609375
typedef const f1p7      cf1p7;      // 8-bit, 1.7 : Decimal range of 0.0~1.9921875
typedef const f1p7x2    cf1p7x2;    // 2x 8-bit, 1.7 : Decimal range of 0.0~1.9921875
typedef const f4p4      cf4p4;      // 8-bit, 4.4 : Decimal range of 0.0~15.9375
typedef const f4p4x2    cf4p4x2;    // 2x 8-bit, 4.4 : Decimal range of 0.0~15.9375
typedef const fp8n0_1   cfp8n0_1;   // Normalised 8-bit : Decimal range of 0.0~1.0
typedef const fp8n0_1x2 cfp8n0_1x2; // 2x Normalised 8-bit : Decimal range of 0.0~1.0
typedef const fp8n0_1x4 cfp8n0_1x4; // 4x normalised 8-bit : Decimal ranges of 0.0~1.0

typedef const f0p8      vf0p8;      // 8-bit, 0.8 : Decimal range of 0.0~0.99609375
typedef const f0p8x2    vf0p8x2;    // 2x 8-bit, 0.8 : Decimal range of 0.0~0.99609375
typedef const f1p7      vf1p7;      // 8-bit, 1.7 : Decimal range of 0.0~1.9921875
typedef const f1p7x2    vf1p7x2;    // 2x 8-bit, 1.7 : Decimal range of 0.0~1.9921875
typedef const f4p4      vf4p4;      // 8-bit, 4.4 : Decimal range of 0.0~15.9375
typedef const f4p4x2    vf4p4x2;    // 2x 8-bit, 4.4 : Decimal range of 0.0~15.9375
typedef const fp8n0_1   vfp8n0_1;   // Normalised 8-bit : Decimal range of 0.0~1.0
typedef const fp8n0_1x2 vfp8n0_1x2; // 2x Normalised 8-bit : Decimal range of 0.0~1.0
typedef const fp8n0_1x4 vfp8n0_1x4; // 4x normalised 8-bit : Decimal ranges of 0.0~1.0

typedef const f0p16         cf0p16;         // 16-bit, 0.16 : Decimal range of 0.0~0.9999847412109375
typedef const f0p16x2       cf0p16x2;       // 2x 16-bit, 0.16 : Decimal range of 0.0~0.9999847412109375
typedef const fs1p14        cfs1p14;        // 16-bit, signed 1.14 : Decimal range of -2.0~1.99993896484375
typedef const fs1p14x2      cfs1p14x2;      // 2x 16-bit, signed 1.14 : Decimal range of -2.0~1.99993896484375
typedef const f1p15         cf1p15;         // 16-bit, 1.15 : Decimal range of 0.0~1.999969482421875
typedef const f1p15x2       cf1p15x2;       // 2x 16-bit, 1.15 : Decimal range of 0.0~1.999969482421875
typedef const f1p15x4       cf1p15x4;       // 4x 16-bit, 1.15 : Decimal range of 0.0~1.999969482421875
typedef const f6p10         cf6p10;         // 16-bit, 6.10 : Decimal range of 0.0~63.9990234375
typedef const f6p10x2       cf6p10x2;       // 2x 16-bit, 6.10 : Decimal range of 0.0~63.9990234375
typedef const fs7p8         cfs7p8;         // 16-bit, signed 7.8 : Decimal range of -128.0~127.99609375
typedef const fs7p8x2       cfs7p8x2;       // 2x 16-bit, signed 7.8 : Decimal range of -128.0~127.99609375
typedef const fs7p8x3       cfs7p8x3;       // 3x 16-bit, signed 7.8 : Decimal range of -128.0~127.99609375
typedef const f7p9          cf7p9;          // 16-bit, 7.9 : Decimal range of 0.0~127.998046875
typedef const f7p9x2        cf7p9x2;        // 2x 16-bit, 7.9 : Decimal range of 0.0~127.998046875
typedef const f8p8          cf8p8;          // 16-bit, 8.8 : Decimal range of 0.0~255.99609375
typedef const f8p8x2        cf8p8x2;        // 2x 16-bit, 8.8 : Decimal range of 0.0~255.99609375
typedef const fp16n_1_1     cfp16n_1_1;     // Normalised 16-bit : Decimal range of -1.0~1.0
typedef const fp16n0_1      cfp16n0_1;      // Normalised 16-bit : Decimal range of 0.0~1.0
typedef const fp16n0_1x2    cfp16n0_1x2;    // 2x Normalised 16-bit : Decimal range of 0.0~1.0
typedef const fp16n0_1x4    cfp16n0_1x4;    // 4x normalised 16-bit : Decimal ranges of 0.0~1.0
typedef const fp16n0_2      cfp16n0_2;      // Normalised 16-bit : Decimal range of 0.0~2.0
typedef const fp16n0_3      cfp16n0_3;      // Normalised 16-bit : Decimal range of 0.0~3.0
typedef const fp16n0_3x16   cfp16n0_3x16;   // 16x Normalised 16-bit : Decimal range of 0.0~3.0
typedef const fp16n0_128    cfp16n0_128;    // Normalised 16-bit : Decimal range of 0.0~128.0
typedef const fp16n_128_128 cfp16n_128_128; // Normalised 16-bit : Decimal range of -128.0~128.0

typedef const f0p16         vf0p16;         // 16-bit, 0.16 : Decimal range of 0.0~0.9999847412109375
typedef const f0p16x2       vf0p16x2;       // 2x 16-bit, 0.16 : Decimal range of 0.0~0.9999847412109375
typedef const fs1p14        vfs1p14;        // 16-bit, signed 1.14 : Decimal range of -2.0~1.99993896484375
typedef const fs1p14x2      vfs1p14x2;      // 2x 16-bit, signed 1.14 : Decimal range of -2.0~1.99993896484375
typedef const f1p15         vf1p15;         // 16-bit, 1.15 : Decimal range of 0.0~1.999969482421875
typedef const f1p15x2       vf1p15x2;       // 2x 16-bit, 1.15 : Decimal range of 0.0~1.999969482421875
typedef const f1p15x4       vf1p15x4;       // 4x 16-bit, 1.15 : Decimal range of 0.0~1.999969482421875
typedef const f6p10         vf6p10;         // 16-bit, 6.10 : Decimal range of 0.0~63.9990234375
typedef const f6p10x2       vf6p10x2;       // 2x 16-bit, 6.10 : Decimal range of 0.0~63.9990234375
typedef const fs7p8         vfs7p8;         // 16-bit, signed 7.8 : Decimal range of -128.0~127.99609375
typedef const fs7p8x2       vfs7p8x2;       // 2x 16-bit, signed 7.8 : Decimal range of -128.0~127.99609375
typedef const fs7p8x3       vfs7p8x3;       // 3x 16-bit, signed 7.8 : Decimal range of -128.0~127.99609375
typedef const f7p9          vf7p9;          // 16-bit, 7.9 : Decimal range of 0.0~127.998046875
typedef const f7p9x2        vf7p9x2;        // 2x 16-bit, 7.9 : Decimal range of 0.0~127.998046875
typedef const f8p8          vf8p8;          // 16-bit, 8.8 : Decimal range of 0.0~255.99609375
typedef const f8p8x2        vf8p8x2;        // 2x 16-bit, 8.8 : Decimal range of 0.0~255.99609375
typedef const fp16n_1_1     vfp16n_1_1;     // Normalised 16-bit : Decimal range of -1.0~1.0
typedef const fp16n0_1      vfp16n0_1;      // Normalised 16-bit : Decimal range of 0.0~1.0
typedef const fp16n0_1x2    vfp16n0_1x2;    // 2x Normalised 16-bit : Decimal range of 0.0~1.0
typedef const fp16n0_1x4    vfp16n0_1x4;    // 4x normalised 16-bit : Decimal ranges of 0.0~1.0
typedef const fp16n0_2      vfp16n0_2;      // Normalised 16-bit : Decimal range of 0.0~2.0
typedef const fp16n0_3      vfp16n0_3;      // Normalised 16-bit : Decimal range of 0.0~3.0
typedef const fp16n0_3x16   vfp16n0_3x16;   // 16x Normalised 16-bit : Decimal range of 0.0~3.0
typedef const fp16n0_128    vfp16n0_128;    // Normalised 16-bit : Decimal range of 0.0~128.0
typedef const fp16n_128_128 vfp16n_128_128; // Normalised 16-bit : Decimal range of -128.0~128.0

#ifdef _24BIT_INTEGERS_

typedef const f0p24     cf0p24;     // 24-bit, 0.24 : Decimal range of 0.0~0.999999940395355224609375
typedef const f8p16     cf8p16;     // 24-bit, 8.16 : Decimal range of 0.0~255.9999847412109375
typedef const f12p12    cf12p12;    // 24-bit, 12.12 : Decimal range of 0.0~4095.999755859375
typedef const f16p8     cf16p8;     // 24-bit, 16.8 : Decimal range of 0.0~65535.99609375
typedef const fp24n0_1  cfp24n0_1;  // Normalised 24-bit : Decimal range of 0.0~1.0
typedef const fp24n_1_1 cfp24n_1_1; // Normalised 24-bit : Decimal range of 0.0~1.0

typedef const f0p24     vf0p24;     // 24-bit, 0.24 : Decimal range of 0.0~0.999999940395355224609375
typedef const f8p16     vf8p16;     // 24-bit, 8.16 : Decimal range of 0.0~255.9999847412109375
typedef const f12p12    vf12p12;    // 24-bit, 12.12 : Decimal range of 0.0~4095.999755859375
typedef const f16p8     vf16p8;     // 24-bit, 16.8 : Decimal range of 0.0~65535.99609375
typedef const fp24n0_1  vfp24n0_1;  // Normalised 24-bit : Decimal range of 0.0~1.0
typedef const fp24n_1_1 vfp24n_1_1; // Normalised 24-bit : Decimal range of 0.0~1.0

#endif

typedef const f0p32     cf0p32;      // 32-bit, 0.32 : Decimal range of 0.0~0.99999999976716935634613037109375
typedef const f16p16    cf16p16;     // 32-bit, 16.16 : Decimal range of 0.0~65535.9999847412109375
typedef const fp32n0_1  cfp32n0_1;   // Normalised 32-bit : Decimal range of 0.0~1.0
typedef const fp32n_1_1 cfp32n_1_1; // Normalised 32-bit : Decimal range of -1.0~1.0

typedef const f0p32     vf0p32;      // 32-bit, 0.32 : Decimal range of 0.0~0.99999999976716935634613037109375
typedef const f16p16    vf16p16;     // 32-bit, 16.16 : Decimal range of 0.0~65535.9999847412109375
typedef const fp32n0_1  vfp32n0_1;   // Normalised 32-bit : Decimal range of 0.0~1.0
typedef const fp32n_1_1 vfp32n_1_1; // Normalised 32-bit : Decimal range of -1.0~1.0
