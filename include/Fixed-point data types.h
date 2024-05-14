/**************************************************************
 * File: Fixed-point data types.h         Created: 2024/05/11 *
 *                                  Last modified: 2024/05/14 *
 *                                                            *
 * Note: Define FPDT_NO_CUSTOM before including this file to  *
 *       exclude declarations of variables & functions for    *
 *       types with user-definable ranges.                    *
 *                                                            *
 *                           Copyright (c) David William Bull *
 **************************************************************/
#pragma once

#include "typedefs.h"

#define _FIXED_POINT_DATA_TYPES_

static cfl32x4 _fpdt_65535fx4    = _mm_set_ps1(65535.0f);
static cfl32x4 _fpdt_255fx4      = _mm_set_ps1(255.0f);
static cfl32x4 _fpdt_rcp255fx4   = _mm_set_ps1(1.0f / 255.0f);
static cfl32x4 _fpdt_rcp65535fx4 = _mm_set_ps1(1.0f / 65535.0f);
static cui128  _fpdt_shuffle8s   = _mm_cvtsi32_si128(0x00C080400);
static cui128  _fpdt_shuffle16s  = _mm_cvtsi64_si128(0x00D0C090805040100);

static cfl32 _fpdt_65535div2f   = 32767.5f;
static cfl32 _fpdt_65535div128f = 511.9921875f;
static cfl32 _fpdt_65535div256f = 255.99609375f;
static cfl32 _fpdt_rcp16f       = 0.0625f;
static cfl32 _fpdt_rcp128f      = 0.0078125;
static cfl32 _fpdt_rcp255f      = 1.0f / 255.0f;
static cfl32 _fpdt_rcp256f      = 0.00390625f;
static cfl32 _fpdt_rcp512f      = 0.001953125f;
static cfl32 _fpdt_rcp65535f    = 1.0f / 65535.0f;
static cfl32 _fpdt_rcp65536f    = 1.0f / 65536.0f;
static cfl32 _fpdt_rcp2p24_1f   = 1.0f / 16777215.0f;
static cfl64 _fpdt_rcp2p32_1    = 1.0 / 4294967295.0;
static cfl32 _fpdt_2rcp65535f   = 2.0f / 65535.0f;
static cfl32 _fpdt_2rcp65536f   = 1.0f / 32768.0f;
static cfl32 _fpdt_128rcp65535f = 128.0f / 65535.0f;
static cfl32 _fpdt_256rcp65535f = 256.0f / 65535.0f;

#ifndef FPDT_NO_CUSTOM

// Data for types with a user-defineable range
static fl32 origin8       = -1.0f;
static fl32 range8        = 1.0f;
static fl32 maxDivRange8  = 255.0f;
static fl32 rangeDivMax8  = _fpdt_rcp255f;
static fl32 origin16      = -1.0f;
static fl32 range16       = 1.0f;
static fl32 maxDivRange16 = 65535.0f;
static fl32 rangeDivMax16 = _fpdt_rcp65535f;
static fl32 origin24      = -1.0f;
static fl32 range24       = 1.0f;
static fl32 maxDivRange24 = 16777215.0f;
static fl32 rangeDivMax24 = _fpdt_rcp2p24_1f;
static fl64 origin32      = -1.0;
static fl64 range32       = 1.0;
static fl64 maxDivRange32 = 4294967295.0;
static fl64 rangeDivMax32 = _fpdt_rcp2p32_1;

// Normalised 8-bit : User-defineable decimal range
struct fp8n {
   typedef const fp8n cfp8n;

   ui8 data;

   inline cfp8n toFixed(cfl32 &value) const { return (cfp8n &)ui8((value - origin8) * maxDivRange8); }
   inline cfl32 toFloat(void) const { return fl32(data) * rangeDivMax8 + origin8; };
   inline cfl32 toFloat(cfp8n &value) const { return fl32(value.data) * rangeDivMax8 + origin8; };

   fp8n(void) = default;
   fp8n(cfl32 floor, cfl32 ceiling) {
      origin8      = floor;
      range8       = ceiling - floor;
      maxDivRange8 = 255.0f / range8;
      rangeDivMax8 = range8 / 255.0f;
   };
   fp8n(cfl32 value, cfl32 floor, cfl32 ceiling) {
      origin8      = floor;
      range8       = ceiling - floor;
      maxDivRange8 = 255.0f / range8;
      rangeDivMax8 = range8 / 255.0f;

      data = ui8((value - floor) * maxDivRange8);
   };
   fp8n(cui8 value) { data = value; }
   fp8n(cui16 value) { data = (ui8 &)value; }
   fp8n(cui32 value) { data = (ui8 &)value; }
   fp8n(csi32 value) { data = (ui8 &)value; }
   fp8n(cfl32 value) { data = toFixed(value); }
   fp8n(cfl64 value) { data = ui8((value - fl64(origin8)) * (255.0 / fl64(range8))); }

   operator ui8(void) const { return data; }
   operator ui16(void) const { return (ui16 &)data & 0x0FF; }
   operator cui32(void) const { return (ui32 &)data & 0x0FF; }
   operator cui64(void) const { return (ui64 &)data & 0x0FF; }
   operator cfl32(void) const { return toFloat(); }
   operator cfl64(void) const { return fl64(data) * (fl64(range8) / 255.0) + fl64(origin8); }

   inline cfp8n &operator&(void) const { return *this; }
   inline cfp8n operator~(void) const { return (cfp8n &)~data; }
   inline cfp8n operator-(void) const { return (cfp8n &)~data; }
   inline cbool operator!(void) const { return data ? false : true; }

   inline cfp8n operator<<(si32 &value) const { return (cfp8n &)(data << value); }
   inline cfp8n operator>>(si32 &value) const { return (cfp8n &)(data >> value); }
   inline cfp8n operator<<=(si32 &value) { return (cfp8n &)(data <<= value); }
   inline cfp8n operator>>=(si32 &value) { return (cfp8n &)(data >>= value); }

   inline cbool operator==(cfp8n &value) const { return data == value.data; }
   inline cbool operator!=(cfp8n &value) const { return data != value.data; }
   inline cbool operator>=(cfp8n &value) const { return data >= value.data; }
   inline cbool operator<=(cfp8n &value) const { return data <= value.data; }
   inline cbool operator>(cfp8n &value) const { return data > value.data; }
   inline cbool operator<(cfp8n &value) const { return data < value.data; }
   inline cbool operator||(cfp8n &value) const { return data || value.data; }
   inline cbool operator&&(cfp8n &value) const { return data && value.data; }

   inline cfp8n operator+(cfp8n &value) const { return (cfp8n &)(data + value.data); }
   inline cfp8n operator-(cfp8n &value) const { return (cfp8n &)(data - value.data); }
   inline cfp8n operator*(cfp8n &value) const { return toFixed(toFloat() * toFloat(value)); }
   inline cfp8n operator/(cfp8n &value) const { return toFixed(toFloat() / toFloat(value)); }
   inline cfp8n operator%(cfp8n &value) const { return (cfp8n &)(data % value.data); }
   inline cfp8n operator&(cfp8n &value) const { return (cfp8n &)(data & value.data); }
   inline cfp8n operator|(cfp8n &value) const { return (cfp8n &)(data | value.data); }
   inline cfp8n operator^(cfp8n &value) const { return (cfp8n &)(data ^ value.data); }
   inline cfp8n operator+=(cfp8n &value) { return (cfp8n &)(data += value.data); }
   inline cfp8n operator-=(cfp8n &value) { return (cfp8n &)(data -= value.data); }
   inline cfp8n operator*=(cfp8n &value) { return *this = toFixed(toFloat() * toFloat(value)); }
   inline cfp8n operator/=(cfp8n &value) { return *this = toFixed(toFloat() / toFloat(value)); }
   inline cfp8n operator%=(cfp8n &value) { return (cfp8n &)(data %= value.data); }
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

   inline cfp8n operator+(cfl32 &value) const { return toFixed(toFloat() + value); }
   inline cfp8n operator-(cfl32 &value) const { return toFixed(toFloat() - value); }
   inline cfp8n operator*(cfl32 &value) const { return toFixed(toFloat() * value); }
   inline cfp8n operator/(cfl32 &value) const { return toFixed(toFloat() / value); }
   inline cfp8n operator%(cfl32 &value) const { return (cfp8n &)(data % toFixed(value).data); }
   inline cfp8n operator&(cfl32 &value) const { return (cfp8n &)(data & toFixed(value).data); }
   inline cfp8n operator|(cfl32 &value) const { return (cfp8n &)(data | toFixed(value).data); }
   inline cfp8n operator^(cfl32 &value) const { return (cfp8n &)(data ^ toFixed(value).data); }
   inline cfp8n operator+=(cfl32 &value) { return *this = toFixed(toFloat() + value); }
   inline cfp8n operator-=(cfl32 &value) { return *this = toFixed(toFloat() - value); }
   inline cfp8n operator*=(cfl32 &value) { return *this = toFixed(toFloat() * value); }
   inline cfp8n operator/=(cfl32 &value) { return *this = toFixed(toFloat() / value); }
   inline cfp8n operator%=(cfl32 &value) { return (cfp8n &)(data %= toFixed(value).data); }
   inline cfp8n operator&=(cfl32 &value) { return (cfp8n &)(data &= toFixed(value).data); }
   inline cfp8n operator|=(cfl32 &value) { return (cfp8n &)(data |= toFixed(value).data); }
   inline cfp8n operator^=(cfl32 &value) { return (cfp8n &)(data ^= toFixed(value).data); }
};

// 4x normalised 8-bit : User-defineable decimal range
struct fp8nx4 {
   typedef const fp8nx4 cfp8nx4;

   union {
      ui32    data32;
      fp8n    data[4];
      VEC4Du8 vec4D;
      ui8     data8[4];
   };

   inline cfp8nx4 toFixed4(cfl32x4 &value) const {
      return (cfp8nx4 &)_mm_shuffle_epi8( _mm_cvttps_epu32( _mm_mul_ps(
             _mm_sub_ps(value, _mm_set_ps1(origin8)), _mm_set_ps1(maxDivRange8))), _fpdt_shuffle8s);
   }

   inline cfl32x4 toFloat4(void) const {
      cui128 temp = _mm_cvtsi32_si128(data32);
      return _mm_add_ps(_mm_mul_ps(_mm_cvtepu32_ps(_mm_cvtepu8_epi32(temp)), _mm_set_ps1(rangeDivMax8)), _mm_set_ps1(origin8));
   };

   inline cfl32x4 toFloat4(cfp8nx4 &value) const {
      cui128 temp = _mm_cvtsi32_si128(value.data32);
      return _mm_add_ps(_mm_mul_ps(_mm_cvtepu32_ps(_mm_cvtepu8_epi32(temp)), _mm_set_ps1(rangeDivMax8)), _mm_set_ps1(origin8));
   };

   fp8nx4(void) = default;
   fp8nx4(cfl32 floor, cfl32 ceiling) {
      origin8      = floor;
      range8       = ceiling - floor;
      maxDivRange8 = 255.0f / range8;
      rangeDivMax8 = range8 / 255.0f;
   };
   fp8nx4(fp8n value, cui8 index) { data[index] = value.data; }
   fp8nx4(cui8 value, cui8 index) { data8[index] = value; }
   fp8nx4(cfl32 value, cui8 index) { data8[index] = ui8((value - origin8) * maxDivRange8); }
   fp8nx4(fp8n (&value)[4]) { data32 = (ui32 &)value[0]; }
   fp8nx4(cui8 (&value)[4]) { data32 = (ui32 &)value[0]; }
   fp8nx4(cui32 value) { data32 = value; }
   fp8nx4(cfl32x4 value) { *this = toFixed4(value); }
   fp8nx4(cui8 value0, cui8 value1, cui8 value2, cui8 value3) {
      data32 = ui32(value0) + (ui32(value1) << 8) + (ui32(value2) << 16) + (ui32(value3) << 24);
   }
   fp8nx4(csi32 value0, csi32 value1, csi32 value2, csi32 value3) {
      data32 = ui32(value0 + (value1 << 8) + (value2 << 16)) + ui32(value3 << 24);
   }
   fp8nx4(cfl32 value, cui8 index, cfl32 floor, cfl32 ceiling) {
      origin8      = floor;
      range8       = ceiling - floor;
      maxDivRange8 = 255.0f / range8;
      rangeDivMax8 = range8 / 255.0f;
      data8[index] = ui8((value - floor) * maxDivRange8);
   };
   fp8nx4(cfl32x4 value, cui8 index, cfl32 floor, cfl32 ceiling) {
      origin8      = floor;
      range8       = ceiling - floor;
      maxDivRange8 = 255.0f / range8;
      rangeDivMax8 = range8 / 255.0f;
      data32       = toFixed4(value);
   }

   operator cui32(void) const { return data32; }
   operator cui64(void) const { return (ui64 &)data32 & 0x0FFFFFFFF; }
   operator cfl32x4(void) const { return toFloat4(); }

   inline cfp8nx4 &operator&(void) const { return *this; }
   inline cfp8nx4 operator~(void) const { return (cfp8nx4 &)~data32; }
   inline cfp8nx4 operator-(void) const { return (cfp8nx4 &)~data32; }
   inline cbool operator!(void) const { return data32 ? false : true; }

   inline cfp8nx4 operator<<(si32 &value) const { return fp8nx4{ data8[0] << value, data8[1] << value, data8[2] << value, data8[3] << value }; }
   inline cfp8nx4 operator>>(si32 &value) const { return fp8nx4{ data8[0] >> value, data8[1] >> value, data8[2] >> value, data8[3] >> value }; }
   inline cfp8nx4 operator<<=(si32 &value) { return fp8nx4{ data8[0] <<= value, data8[1] <<= value, data8[2] <<= value, data8[3] <<= value }; }
   inline cfp8nx4 operator>>=(si32 &value) { return fp8nx4{ data8[0] >>= value, data8[1] >>= value, data8[2] >>= value, data8[3] >>= value }; }

   inline cui8 operator==(cfp8nx4 &value) const { return (data8[0] == value.data8[0]) | ((data8[1] == value.data8[1]) << 1) | ((data8[2] == value.data8[2]) << 2) | ((data8[3] == value.data8[3]) << 3); }
   inline cui8 operator!=(cfp8nx4 &value) const { return (data8[0] != value.data8[0]) | ((data8[1] != value.data8[1]) << 1) | ((data8[2] != value.data8[2]) << 2) | ((data8[3] != value.data8[3]) << 3); }
   inline cui8 operator>=(cfp8nx4 &value) const { return (data8[0] >= value.data8[0]) | ((data8[1] >= value.data8[1]) << 1) | ((data8[2] >= value.data8[2]) << 2) | ((data8[3] >= value.data8[3]) << 3); }
   inline cui8 operator<=(cfp8nx4 &value) const { return (data8[0] <= value.data8[0]) | ((data8[1] <= value.data8[1]) << 1) | ((data8[2] <= value.data8[2]) << 2) | ((data8[3] <= value.data8[3]) << 3); }
   inline cui8 operator>(cfp8nx4 &value) const { return (data8[0] > value.data8[0]) | ((data8[1] > value.data8[1]) << 1) | ((data8[2] > value.data8[2]) << 2) | ((data8[3] > value.data8[3]) << 3); }
   inline cui8 operator<(cfp8nx4 &value) const { return (data8[0] < value.data8[0]) | ((data8[1] < value.data8[1]) << 1) | ((data8[2] < value.data8[2]) << 2) | ((data8[3] < value.data8[3]) << 3); }
   inline cui8 operator||(cfp8nx4 &value) const { return (data8[0] || value.data8[0]) | ((data8[1] || value.data8[1]) << 1) | ((data8[2] || value.data8[2]) << 2) | ((data8[3] || value.data8[3]) << 3); }
   inline cui8 operator&&(cfp8nx4 &value) const { return (data8[0] && value.data8[0]) | ((data8[1] && value.data8[1]) << 1) | ((data8[2] && value.data8[2]) << 2) | ((data8[3] && value.data8[3]) << 3); }

   inline cfp8nx4 operator+(cfp8nx4 &value) const { return fp8nx4{ data8[0] + value.data8[0], data8[1] + value.data8[1], data8[2] + value.data8[2], data8[3] + value.data8[3] }; }
   inline cfp8nx4 operator-(cfp8nx4 &value) const { return fp8nx4{ data8[0] - value.data8[0], data8[1] - value.data8[1], data8[2] - value.data8[2], data8[3] - value.data8[3] }; }
   inline cfp8nx4 operator*(cfp8nx4 &value) const { return toFixed4(_mm_mul_ps(toFloat4(), toFloat4(value))); }
   inline cfp8nx4 operator/(cfp8nx4 &value) const { return toFixed4(_mm_div_ps(toFloat4(), toFloat4(value))); }
   inline cfp8nx4 operator%(cfp8nx4 &value) const { return fp8nx4{ data8[0] % value.data8[0], data8[1] % value.data8[1], data8[2] % value.data8[2], data8[3] % value.data8[3] }; }
   inline cfp8nx4 operator&(cfp8nx4 &value) const { return (cfp8nx4 &)(data32 & value.data32); }
   inline cfp8nx4 operator|(cfp8nx4 &value) const { return (cfp8nx4 &)(data32 | value.data32); }
   inline cfp8nx4 operator^(cfp8nx4 &value) const { return (cfp8nx4 &)(data32 ^ value.data32); }
   inline cfp8nx4 operator+=(cfp8nx4 &value) { return fp8nx4{ data8[0] += value.data8[0], data8[1] += value.data8[1], data8[2] += value.data8[2], data8[3] += value.data8[3] }; }
   inline cfp8nx4 operator-=(cfp8nx4 &value) { return fp8nx4{ data8[0] -= value.data8[0], data8[1] -= value.data8[1], data8[2] -= value.data8[2], data8[3] -= value.data8[3] }; }
   inline cfp8nx4 operator*=(cfp8nx4 &value) { return *this = toFixed4(_mm_mul_ps(toFloat4(), toFloat4(value))); }
   inline cfp8nx4 operator/=(cfp8nx4 &value) { return *this = toFixed4(_mm_div_ps(toFloat4(), toFloat4(value))); }
   inline cfp8nx4 operator%(cfp8nx4 &value) { return *this = fp8nx4{ data8[0] % value.data8[0], data8[1] % value.data8[1], data8[2] % value.data8[2], data8[3] % value.data8[3] }; }
   inline cfp8nx4 operator&=(cfp8nx4 &value) { return (cfp8nx4 &)(data32 &= value.data32); }
   inline cfp8nx4 operator|=(cfp8nx4 &value) { return (cfp8nx4 &)(data32 |= value.data32); }
   inline cfp8nx4 operator^=(cfp8nx4 &value) { return (cfp8nx4 &)(data32 ^= value.data32); }

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

   inline cfp16n toFixed(cfl32 &value) const { return (cfp16n &)ui16((value - origin16) * maxDivRange16); }
   inline cfl32 toFloat(void) const { return fl32(data) * rangeDivMax16 + origin16; };
   inline cfl32 toFloat(cfp16n &value) const { return fl32(value.data) * rangeDivMax16 + origin16; };

   fp16n(void) = default;
   fp16n(cfl32 floor, cfl32 ceiling) {
      origin16      = floor;
      range16       = ceiling - floor;
      maxDivRange16 = 65535.0f / range16;
      rangeDivMax16 = range16 / 65535.0f;
   };
   fp16n(cfl32 value, cfl32 floor, cfl32 ceiling) {
      origin16      = floor;
      range16       = ceiling - floor;
      maxDivRange16 = 65535.0f / range16;
      rangeDivMax16 = range16 / 65535.0f;

      data = ui16((value - floor) * maxDivRange16);
   };
   fp16n(cui16 value) { data = (ui16 &)value; }
   fp16n(cui32 value) { data = (ui16 &)value; }
   fp16n(csi32 value) { data = (ui16 &)value; }
   fp16n(cfl32 value) { data = toFixed(value); }
   fp16n(cfl64 value) { data = ui8((value - fl64(origin16)) * (65535.0 / fl64(range16))); }

   operator ui16(void) const { return data; }
   operator cui32(void) const { return (ui32 &)data & 0x0FFFF; }
   operator cui64(void) const { return (ui64 &)data & 0x0FFFF; }
   operator cfl32(void) const { return toFloat(); }
   operator cfl64(void) const { return fl64(data) * (fl64(range16) / 65535.0) + fl64(origin16); }

   inline cfp16n &operator&(void) const { return *this; }
   inline cfp16n operator~(void) const { return (cfp16n &)~data; }
   inline cfp16n operator-(void) const { return (cfp16n &)~data; }
   inline cbool operator!(void) const { return data ? false : true; }

   inline cfp16n operator<<(si32 &value) const { return (cfp16n &)(data << value); }
   inline cfp16n operator>>(si32 &value) const { return (cfp16n &)(data >> value); }
   inline cfp16n operator<<=(si32 &value) { return (cfp16n &)(data <<= value); }
   inline cfp16n operator>>=(si32 &value) { return (cfp16n &)(data >>= value); }

   inline cbool operator==(cfp16n &value) const { return data == value.data; }
   inline cbool operator!=(cfp16n &value) const { return data != value.data; }
   inline cbool operator>=(cfp16n &value) const { return data >= value.data; }
   inline cbool operator<=(cfp16n &value) const { return data <= value.data; }
   inline cbool operator>(cfp16n &value) const { return data > value.data; }
   inline cbool operator<(cfp16n &value) const { return data < value.data; }
   inline cbool operator||(cfp16n &value) const { return data || value.data; }
   inline cbool operator&&(cfp16n &value) const { return data && value.data; }

   inline cfp16n operator+(cfp16n &value) const { return (cfp16n &)(data + value.data); }
   inline cfp16n operator-(cfp16n &value) const { return (cfp16n &)(data - value.data); }
   inline cfp16n operator*(cfp16n &value) const { return toFixed(toFloat() * toFloat(value)); }
   inline cfp16n operator/(cfp16n &value) const { return toFixed(toFloat() / toFloat(value)); }
   inline cfp16n operator%(cfp16n &value) const { return (cfp16n &)(data % value.data); }
   inline cfp16n operator&(cfp16n &value) const { return (cfp16n &)(data & value.data); }
   inline cfp16n operator|(cfp16n &value) const { return (cfp16n &)(data | value.data); }
   inline cfp16n operator^(cfp16n &value) const { return (cfp16n &)(data ^ value.data); }
   inline cfp16n operator+=(cfp16n &value) { return (cfp16n &)(data += value.data); }
   inline cfp16n operator-=(cfp16n &value) { return (cfp16n &)(data -= value.data); }
   inline cfp16n operator*=(cfp16n &value) { return *this = toFixed(toFloat() * toFloat(value)); }
   inline cfp16n operator/=(cfp16n &value) { return *this = toFixed(toFloat() / toFloat(value)); }
   inline cfp16n operator%=(cfp16n &value) { return (cfp16n &)(data %= value.data); }
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

   inline cfp16n operator+(cfl32 &value) const { return toFixed(toFloat() + value); }
   inline cfp16n operator-(cfl32 &value) const { return toFixed(toFloat() - value); }
   inline cfp16n operator*(cfl32 &value) const { return toFixed(toFloat() * value); }
   inline cfp16n operator/(cfl32 &value) const { return toFixed(toFloat() / value); }
   inline cfp16n operator%(cfl32 &value) const { return (cfp16n &)(data % toFixed(value).data); }
   inline cfp16n operator&(cfl32 &value) const { return (cfp16n &)(data & toFixed(value).data); }
   inline cfp16n operator|(cfl32 &value) const { return (cfp16n &)(data | toFixed(value).data); }
   inline cfp16n operator^(cfl32 &value) const { return (cfp16n &)(data ^ toFixed(value).data); }
   inline cfp16n operator+=(cfl32 &value) { return *this = toFixed(toFloat() + value); }
   inline cfp16n operator-=(cfl32 &value) { return *this = toFixed(toFloat() - value); }
   inline cfp16n operator*=(cfl32 &value) { return *this = toFixed(toFloat() * value); }
   inline cfp16n operator/=(cfl32 &value) { return *this = toFixed(toFloat() / value); }
   inline cfp16n operator%=(cfl32 &value) { return (cfp16n &)(data %= toFixed(value).data); }
   inline cfp16n operator&=(cfl32 &value) { return (cfp16n &)(data &= toFixed(value).data); }
   inline cfp16n operator|=(cfl32 &value) { return (cfp16n &)(data |= toFixed(value).data); }
   inline cfp16n operator^=(cfl32 &value) { return (cfp16n &)(data ^= toFixed(value).data); }
};

// 4x normalised 16-bit : User-defineable decimal range
struct fp16nx4 {
   typedef const fp16nx4 cfp16nx4;

   union {
      ui64     data64;
      fp16n    data[4];
      VEC4Du16 vec4D;
      ui16     data16[4];
   };

   inline cfp16nx4 toFixed4(cfl32x4 &value) const {
      return (cfp16nx4 &)_mm_shuffle_epi8( _mm_cvttps_epu32( _mm_mul_ps(
         _mm_sub_ps(value, _mm_set_ps1(origin16)), _mm_set_ps1(maxDivRange16))), _fpdt_shuffle16s);
   }

   inline cfl32x4 toFloat4(void) const {
      cui128 temp = _mm_cvtsi64_si128(data64);
      return _mm_add_ps(_mm_mul_ps(_mm_cvtepu32_ps(_mm_cvtepu16_epi32(temp)), _mm_set_ps1(rangeDivMax16)), _mm_set_ps1(origin16));
   };

   inline cfl32x4 toFloat4(cfp16nx4 &value) const {
      cui128 temp = _mm_cvtsi64_si128(value.data64);
      return _mm_add_ps(_mm_mul_ps(_mm_cvtepu32_ps(_mm_cvtepu16_epi32(temp)), _mm_set_ps1(rangeDivMax16)), _mm_set_ps1(origin16));
   };

   fp16nx4(void) = default;
   fp16nx4(cfl32 floor, cfl32 ceiling) {
      origin16      = floor;
      range16       = ceiling - floor;
      maxDivRange16 = 65535.0f / range16;
      rangeDivMax16 = range16 / 65535.0f;
   };
   fp16nx4(fp16n value, cui8 index) { data[index] = value.data; }
   fp16nx4(cui16 value, cui8 index) { data16[index] = value; }
   fp16nx4(cfl32 value, cui8 index) { data16[index] = ui16((value - origin16) * maxDivRange16); }
   fp16nx4(fp16n (&value)[4]) { data64 = (ui64 &)value[0]; }
   fp16nx4(cui16 (&value)[4]) { data64 = (ui64 &)value[0]; }
   fp16nx4(cui64 value) { data64 = value; }
   fp16nx4(cfl32x4 value) { *this = toFixed4(value); }
   fp16nx4(cui16 value0, cui16 value1, cui16 value2, cui16 value3) { data64 = ui64(value0) + (ui64(value1) << 8) + (ui64(value2) << 16) + (ui64(value3) << 24); }
   fp16nx4(csi32 value0, csi32 value1, csi32 value2, csi32 value3) { data64 = ui64(value0 + (value1 << 8) + (value2 << 16)) + ui64(value3 << 24); }
   fp16nx4(cfl32 value, cui8 index, cfl32 floor, cfl32 ceiling) {
      origin16      = floor;
      range16       = ceiling - floor;
      maxDivRange16 = 65535.0f / range16;
      rangeDivMax16 = range16 / 65535.0f;
      data16[index] = ui16((value - floor) * maxDivRange16);
   };
   fp16nx4(cfl32x4 value, cui8 index, cfl32 floor, cfl32 ceiling) {
      origin16      = floor;
      range16       = ceiling - floor;
      maxDivRange16 = 65535.0f / range16;
      rangeDivMax16 = range16 / 65535.0f;
      data64        = toFixed4(value);
   }

   operator cui64(void) const { return data64; }
   operator cfl32x4(void) const { return toFloat4(); }

   inline cfp16nx4 &operator&(void) const { return *this; }
   inline cfp16nx4 operator~(void) const { return (cfp16nx4 &)~data64; }
   inline cfp16nx4 operator-(void) const { return (cfp16nx4 &)~data64; }
   inline cbool operator!(void) const { return data64 ? false : true; }

   inline cfp16nx4 operator<<(si32 &value) const { return fp16nx4{ data16[0] << value, data16[1] << value, data16[2] << value, data16[3] << value }; }
   inline cfp16nx4 operator>>(si32 &value) const { return fp16nx4{ data16[0] >> value, data16[1] >> value, data16[2] >> value, data16[3] >> value }; }
   inline cfp16nx4 operator<<=(si32 &value) { return fp16nx4{ data16[0] <<= value, data16[1] <<= value, data16[2] <<= value, data16[3] <<= value }; }
   inline cfp16nx4 operator>>=(si32 &value) { return fp16nx4{ data16[0] >>= value, data16[1] >>= value, data16[2] >>= value, data16[3] >>= value }; }

   inline cui8 operator==(cfp16nx4 &value) const { return (data16[0] == value.data16[0]) | ((data16[1] == value.data16[1]) << 1) | ((data16[2] == value.data16[2]) << 2) | ((data16[3] == value.data16[3]) << 3); }
   inline cui8 operator!=(cfp16nx4 &value) const { return (data16[0] != value.data16[0]) | ((data16[1] != value.data16[1]) << 1) | ((data16[2] != value.data16[2]) << 2) | ((data16[3] != value.data16[3]) << 3); }
   inline cui8 operator>=(cfp16nx4 &value) const { return (data16[0] >= value.data16[0]) | ((data16[1] >= value.data16[1]) << 1) | ((data16[2] >= value.data16[2]) << 2) | ((data16[3] >= value.data16[3]) << 3); }
   inline cui8 operator<=(cfp16nx4 &value) const { return (data16[0] <= value.data16[0]) | ((data16[1] <= value.data16[1]) << 1) | ((data16[2] <= value.data16[2]) << 2) | ((data16[3] <= value.data16[3]) << 3); }
   inline cui8 operator>(cfp16nx4 &value) const { return (data16[0] > value.data16[0]) | ((data16[1] > value.data16[1]) << 1) | ((data16[2] > value.data16[2]) << 2) | ((data16[3] > value.data16[3]) << 3); }
   inline cui8 operator<(cfp16nx4 &value) const { return (data16[0] < value.data16[0]) | ((data16[1] < value.data16[1]) << 1) | ((data16[2] < value.data16[2]) << 2) | ((data16[3] < value.data16[3]) << 3); }
   inline cui8 operator||(cfp16nx4 &value) const { return (data16[0] || value.data16[0]) | ((data16[1] || value.data16[1]) << 1) | ((data16[2] || value.data16[2]) << 2) | ((data16[3] || value.data16[3]) << 3); }
   inline cui8 operator&&(cfp16nx4 &value) const { return (data16[0] && value.data16[0]) | ((data16[1] && value.data16[1]) << 1) | ((data16[2] && value.data16[2]) << 2) | ((data16[3] && value.data16[3]) << 3); }

   inline cfp16nx4 operator+(cfp16nx4 &value) const { return fp16nx4{ data16[0] + value.data16[0], data16[1] + value.data16[1], data16[2] + value.data16[2], data16[3] + value.data16[3] }; }
   inline cfp16nx4 operator-(cfp16nx4 &value) const { return fp16nx4{ data16[0] - value.data16[0], data16[1] - value.data16[1], data16[2] - value.data16[2], data16[3] - value.data16[3] }; }
   inline cfp16nx4 operator*(cfp16nx4 &value) const { return toFixed4(_mm_mul_ps(toFloat4(), toFloat4(value))); }
   inline cfp16nx4 operator/(cfp16nx4 &value) const { return toFixed4(_mm_div_ps(toFloat4(), toFloat4(value))); }
   inline cfp16nx4 operator%(cfp16nx4 &value) const { return fp16nx4{ data16[0] % value.data16[0], data16[1] % value.data16[1], data16[2] % value.data16[2], data16[3] % value.data16[3] }; }
   inline cfp16nx4 operator&(cfp16nx4 &value) const { return (cfp16nx4 &)(data64 & value.data64); }
   inline cfp16nx4 operator|(cfp16nx4 &value) const { return (cfp16nx4 &)(data64 | value.data64); }
   inline cfp16nx4 operator^(cfp16nx4 &value) const { return (cfp16nx4 &)(data64 ^ value.data64); }
   inline cfp16nx4 operator+=(cfp16nx4 &value) { return fp16nx4{ data16[0] += value.data16[0], data16[1] += value.data16[1], data16[2] += value.data16[2], data16[3] += value.data16[3] }; }
   inline cfp16nx4 operator-=(cfp16nx4 &value) { return fp16nx4{ data16[0] -= value.data16[0], data16[1] -= value.data16[1], data16[2] -= value.data16[2], data16[3] -= value.data16[3] }; }
   inline cfp16nx4 operator*=(cfp16nx4 &value) { return *this = toFixed4(_mm_mul_ps(toFloat4(), toFloat4(value))); }
   inline cfp16nx4 operator/=(cfp16nx4 &value) { return *this = toFixed4(_mm_div_ps(toFloat4(), toFloat4(value))); }
   inline cfp16nx4 operator%(cfp16nx4 &value) { return *this = fp16nx4{ data16[0] % value.data16[0], data16[1] % value.data16[1], data16[2] % value.data16[2], data16[3] % value.data16[3] }; }
   inline cfp16nx4 operator&=(cfp16nx4 &value) { return (cfp16nx4 &)(data64 &= value.data64); }
   inline cfp16nx4 operator|=(cfp16nx4 &value) { return (cfp16nx4 &)(data64 |= value.data64); }
   inline cfp16nx4 operator^=(cfp16nx4 &value) { return (cfp16nx4 &)(data64 ^= value.data64); }

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
      cfp16nx4 temp = toFixed4(value);
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
      cfp16nx4 temp = toFixed4(value);
      return *this = fp16nx4{ data16[0] % temp.data16[0], data16[1] % temp.data16[1], data16[2] % temp.data16[2], data16[3] % temp.data16[3] };
   }
   inline cfp16nx4 operator&=(cfl32x4 &value) { (cfp16nx4 &)(data64 &= toFixed4(value).data64); }
   inline cfp16nx4 operator|=(cfl32x4 &value) { (cfp16nx4 &)(data64 |= toFixed4(value).data64); }
   inline cfp16nx4 operator^=(cfl32x4 &value) { (cfp16nx4 &)(data64 ^= toFixed4(value).data64); }
};

// User-defineable ranges
typedef const fp8n    cfp8n;    // Normalised 8-bit : User-defineable decimal range
typedef const fp8nx4  cfp8nx4;  // 4x normalised 8-bit : User-defineable decimal range
typedef const fp16n   cfp16n;   // Normalised 16-bit : User-defineable decimal range
typedef const fp16nx4 cfp16nx4; // 4x normalised 16-bit : User-defineable decimal range

#endif // #ifndef FPDT_NO_CUSTOM

// 8-bit, 0.8 : Decimal range of 0.0~0.99609375
struct f0p8 {
   typedef const f0p8 cf0p8;

   ui8 data;

   f0p8(void) = default;
   f0p8(cui8 value) { data = value; }
   f0p8(cui32 value) { data = (ui8 &)value; }
   f0p8(csi32 value) { data = (ui8 &)value; }
   f0p8(cfl32 value) { data = ui8(value * 256.0f); }

   operator ui8(void) const { return data; }
   operator cfl32(void) const { return cfl32(data) * _fpdt_rcp256f; }

   inline cf0p8 operator+(cf0p8 &value) const { return data + value.data; }
   inline cf0p8 operator-(cf0p8 &value) const { return data - value.data; }
   inline cf0p8 operator+(cfl32 &value) const { return data + ui8(value * 256.0f); }
   inline cf0p8 operator-(cfl32 &value) const { return data - ui8(value * 256.0f); }

   inline cf0p8 operator+=(cf0p8 &value) { return (data += value.data); }
   inline cf0p8 operator-=(cf0p8 &value) { return (data -= value.data); }
   inline cf0p8 operator+=(cfl32 &value) { return (data += ui8(value * 256.0f)); }
   inline cf0p8 operator-=(cfl32 &value) { return (data -= ui8(value * 256.0f)); }
};

// 8-bit, 1.7 : Decimal range of 0.0~1.9921875
struct f1p7 {
   typedef const f1p7 cf1p7;

   ui8 data;

   f1p7(void) = default;
   f1p7(cui8 value) { data = value; }
   f1p7(cui32 value) { data = (ui8 &)value; }
   f1p7(csi32 value) { data = (ui8 &)value; }
   f1p7(cfl32 value) { data = ui8(value * 128.0f); }

   operator ui8(void) const { return data; }
   operator cfl32(void) const { return cfl32(data) * _fpdt_rcp128f; }

   inline cf1p7 operator+(cf1p7 &value) const { return data + value.data; }
   inline cf1p7 operator-(cf1p7 &value) const { return data - value.data; }
   inline cf1p7 operator+(cfl32 &value) const { return data + ui8(value * 128.0f); }
   inline cf1p7 operator-(cfl32 &value) const { return data - ui8(value * 128.0f); }

   inline cf1p7 operator+=(cf1p7 &value) { return (data += value.data); }
   inline cf1p7 operator-=(cf1p7 &value) { return (data -= value.data); }
   inline cf1p7 operator+=(cfl32 &value) { return (data += ui8(value * 128.0f)); }
   inline cf1p7 operator-=(cfl32 &value) { return (data -= ui8(value * 128.0f)); }
};

// 8-bit, 4.4 : Decimal range of 0.0~15.9375
struct f4p4 {
   typedef const f4p4 cf4p4;

   ui8 data;

   f4p4(void) = default;
   f4p4(cui8 value) { data = value; }
   f4p4(cui32 value) { data = (ui8 &)value; }
   f4p4(csi32 value) { data = (ui8 &)value; }
   f4p4(cfl32 value) { data = ui8(value * 16.0f); }

   operator ui8(void) const { return data; }
   operator cfl32(void) const { return cfl32(data) * _fpdt_rcp16f; }

   inline cf4p4 operator+(cf4p4 &value) const { return data + value.data; }
   inline cf4p4 operator-(cf4p4 &value) const { return data - value.data; }
   inline cf4p4 operator+(cfl32 &value) const { return data + ui8(value * 16.0f); }
   inline cf4p4 operator-(cfl32 &value) const { return data - ui8(value * 16.0f); }

   inline cf4p4 operator+=(cf4p4 &value) { return (data += value.data); }
   inline cf4p4 operator-=(cf4p4 &value) { return (data -= value.data); }
   inline cf4p4 operator+=(cfl32 &value) { return (data += ui8(value * 16.0f)); }
   inline cf4p4 operator-=(cfl32 &value) { return (data -= ui8(value * 16.0f)); }
};

// Normalised 8-bit : Decimal range of 0.0~1.0
struct fp8n0_1 {
   typedef const fp8n0_1 cfp8n0_1;

   ui8 data;

   fp8n0_1(void) = default;
   fp8n0_1(cui8 value) { data = value; }
   fp8n0_1(cui32 value) { data = (ui8 &)value; }
   fp8n0_1(csi32 value) { data = (ui8 &)value; }
   fp8n0_1(cfl32 value) { data = ui8(value * 255.0f); }

   operator ui8(void) const { return data; }
   operator cfl32(void) const { return cfl32(data) * _fpdt_rcp255f; }

   inline cfp8n0_1 operator+(cfp8n0_1 &value) const { return data + value.data; }
   inline cfp8n0_1 operator-(cfp8n0_1 &value) const { return data - value.data; }
   inline cfp8n0_1 operator+(cfl32 &value) const { return data + ui8(value * 255.0f); }
   inline cfp8n0_1 operator-(cfl32 &value) const { return data - ui8(value * 255.0f); }

   inline cfp8n0_1 operator+=(cfp8n0_1 &value) { return (data += value.data); }
   inline cfp8n0_1 operator-=(cfp8n0_1 &value) { return (data -= value.data); }
   inline cfp8n0_1 operator+=(cfl32 &value) { return (data += ui8(value * 255.0f)); }
   inline cfp8n0_1 operator-=(cfl32 &value) { return (data -= ui8(value * 255.0f)); }
};

// 4x normalised 8-bit : Decimal ranges of 0.0~1.0
struct fp8n0_1x4 {
   typedef const fp8n0_1x4 cfp8n0_1x4;

   union {
      ui32    data32;
      fp8n0_1 data[4];
      VEC4Du8 vec4D;
      ui8     data8[4];
   };

   fp8n0_1x4(void) = default;
   fp8n0_1x4(fp8n0_1 value, cui8 index) { data[index] = value; }
   fp8n0_1x4(cui8 value, cui8 index) { data8[index] = value; }
   fp8n0_1x4(cfl32 value, cui8 index) { data8[index] = ui8(value * 255.0f); }
   fp8n0_1x4(fp8n0_1 (&value)[4]) { data32 = (ui32 &)value[0]; }
   fp8n0_1x4(cui8 (&value)[4]) { data32 = (ui32 &)value[0]; }
   fp8n0_1x4(cui32 value) { data32 = value; }
   fp8n0_1x4(cfl32x4 value) {
      data32 = (ui32 &)_mm_shuffle_epi8( _mm_cvttps_epu32(
               _mm_mul_ps(value, _fpdt_255fx4)), _fpdt_shuffle8s);
   }

   operator cui32(void) const { return data32; }
   operator cfl32x4(void) const {
      cui128 temp = _mm_cvtsi32_si128(data32);
      return _mm_mul_ps(_mm_cvtepu32_ps(_mm_cvtepu8_epi32(temp)), _fpdt_rcp255fx4);
   }

   inline cfp8n0_1x4 operator+(cfp8n0_1x4 &value) const { return data32 + value.data32; }
   inline cfp8n0_1x4 operator-(cfp8n0_1x4 &value) const { return data32 - value.data32; }
   inline cfp8n0_1x4 operator+(cfl32x4 &value) const {
      return data32 + (ui32 &)_mm_shuffle_epi8( _mm_cvttps_epu32(
                      _mm_mul_ps(value, _fpdt_255fx4)), _fpdt_shuffle8s);
   }
   inline cfp8n0_1x4 operator-(cfl32x4 &value) const {
      return data32 - (ui32 &)_mm_shuffle_epi8( _mm_cvttps_epu32(
                      _mm_mul_ps(value, _fpdt_255fx4)), _fpdt_shuffle8s);
   }

   inline cfp8n0_1x4 operator+=(cfp8n0_1x4 &value) { return (data32 += value.data32); }
   inline cfp8n0_1x4 operator-=(cfp8n0_1x4 &value) { return (data32 -= value.data32); }
   inline cfp8n0_1x4 operator+=(cfl32x4 &value) {
      return (data32 += (ui32 &)_mm_shuffle_epi8( _mm_cvttps_epu32(
                        _mm_mul_ps(value, _fpdt_255fx4)), _fpdt_shuffle8s));
   }
   inline cfp8n0_1x4 operator-=(cfl32x4 &value) {
      return (data32 -= (ui32 &)_mm_shuffle_epi8( _mm_cvttps_epu32(
                        _mm_mul_ps(value, _fpdt_255fx4)), _fpdt_shuffle8s));
   }
};

// 16-bit, 0.16 : Decimal range of 0.0~0.9999847412109375
struct f0p16 {
   typedef const f0p16 cf0p16;

   ui16 data;

   f0p16(void) = default;
   f0p16(cui16 value) { data = value; }
   f0p16(cui32 value) { data = (ui16 &)value; }
   f0p16(csi32 value) { data = (ui16 &)value; }
   f0p16(cfl32 value) { data = ui16(value * 65536.0f); }

   operator ui16(void) const { return data; }
   operator cfl32(void) const { return cfl32(data) * _fpdt_rcp65536f; }

   inline cf0p16 operator+(cf0p16 &value) const { return data + value.data; }
   inline cf0p16 operator-(cf0p16 &value) const { return data - value.data; }
   inline cf0p16 operator+(cfl32 &value) const { return data + ui16(value * 65536.0f); }
   inline cf0p16 operator-(cfl32 &value) const { return data - ui16(value * 65536.0f); }

   inline cf0p16 operator+=(cf0p16 &value) { return (data += value.data); }
   inline cf0p16 operator-=(cf0p16 &value) { return (data -= value.data); }
   inline cf0p16 operator+=(cfl32 &value)  { return (data += ui16(value * 65536.0f)); }
   inline cf0p16 operator-=(cfl32 &value)  { return (data -= ui16(value * 65536.0f)); }
};

// 16-bit, 1.15 : Decimal range of 0.0~1.999969482421875
struct f1p15 {
   typedef const f1p15 cf1p15;

   ui16 data;

   f1p15(void) = default;
   f1p15(cui16 value) { data = value; }
   f1p15(cui32 value) { data = (ui16 &)value; }
   f1p15(csi32 value) { data = (ui16 &)value; }
   f1p15(cfl32 value) { data = ui16(value * 32768.0f); }

   operator ui16(void) const { return data; }
   operator cfl32(void) const { return cfl32(data) * _fpdt_2rcp65536f; }

   inline cf1p15 operator+(cf1p15 &value) const { return data + value.data; }
   inline cf1p15 operator-(cf1p15 &value) const { return data - value.data; }
   inline cf1p15 operator+(cfl32 &value) const { return data + ui16(value * 32768.0f); }
   inline cf1p15 operator-(cfl32 &value) const { return data - ui16(value * 32768.0f); }

   inline cf1p15 operator+=(cf1p15 &value) { return (data += value.data); }
   inline cf1p15 operator-=(cf1p15 &value) { return (data -= value.data); }
   inline cf1p15 operator+=(cfl32 &value)  { return (data += ui16(value * 32768.0f)); }
   inline cf1p15 operator-=(cfl32 &value)  { return (data -= ui16(value * 32768.0f)); }
};

// 16-bit, signed 7.8 : Decimal range of -128.0~127.99609375
struct fs7p8 {
   typedef const fs7p8 cfs7p8;

   ui16 data;

   fs7p8(void) = default;
   fs7p8(cui16 value) { data = value; }
   fs7p8(cui32 value) { data = (ui16 &)value; }
   fs7p8(csi32 value) { data = (ui16 &)value; }
   fs7p8(cfl32 value) { data = ui16((value + 128.0f) * 256.0f); }

   operator ui16(void) const { return data; }
   operator cfl32(void) const { return cfl32(data) * _fpdt_rcp256f - 128.0f; }

   inline cfs7p8 operator+(cfs7p8 &value) const { return data + value.data; }
   inline cfs7p8 operator-(cfs7p8 &value) const { return data - value.data; }
   inline cfs7p8 operator+(cfl32 &value) const { return data + ui16((value + 128.0f) * 256.0f); }
   inline cfs7p8 operator-(cfl32 &value) const { return data - ui16((value + 128.0f) * 256.0f); }

   inline cfs7p8 operator+=(cfs7p8 &value) { return (data += value.data); }
   inline cfs7p8 operator-=(cfs7p8 &value) { return (data -= value.data); }
   inline cfs7p8 operator+=(cfl32 &value)  { return (data += ui16((value + 128.0f) * 256.0f)); }
   inline cfs7p8 operator-=(cfl32 &value)  { return (data -= ui16((value + 128.0f) * 256.0f)); }
};

// 16-bit, 7.9 : Decimal range of 0.0~127.998046875
struct f7p9 {
   typedef const f7p9 cf7p9;

   ui16 data;

   f7p9(void) = default;
   f7p9(cui16 value) { data = value; }
   f7p9(cui32 value) { data = (ui16 &)value; }
   f7p9(csi32 value) { data = (ui16 &)value; }
   f7p9(cfl32 value) { data = ui16(value * 512.0f); }

   operator ui16(void) const { return data; }
   operator cfl32(void) const { return cfl32(data) * _fpdt_rcp512f; }

   inline cf7p9 operator+(cf7p9 &value) const { return data + value.data; }
   inline cf7p9 operator-(cf7p9 &value) const { return data - value.data; }
   inline cf7p9 operator+(cfl32 &value) const { return data + ui16(value * 512.0f); }
   inline cf7p9 operator-(cfl32 &value) const { return data - ui16(value * 512.0f); }

   inline cf7p9 operator+=(cf7p9 &value) { return (data += value.data); }
   inline cf7p9 operator-=(cf7p9 &value) { return (data -= value.data); }
   inline cf7p9 operator+=(cfl32 &value)  { return (data += ui16(value * 512.0f)); }
   inline cf7p9 operator-=(cfl32 &value)  { return (data -= ui16(value * 512.0f)); }
};

// 16-bit, 8.8 : Decimal range of 0.0~255.99609375
struct f8p8 {
   typedef const f8p8 cf8p8;

   ui16 data;

   f8p8(void) = default;
   f8p8(cui16 value) { data = value; }
   f8p8(cui32 value) { data = (ui16 &)value; }
   f8p8(csi32 value) { data = (ui16 &)value; }
   f8p8(cfl32 value) { data = ui16(value * 256.0f); }

   operator ui16(void) const { return data; }
   operator cfl32(void) const { return cfl32(data) * _fpdt_rcp256f; }

   inline cf8p8 operator+(cf8p8 &value) const { return data + value.data; }
   inline cf8p8 operator-(cf8p8 &value) const { return data - value.data; }
   inline cf8p8 operator+(cfl32 &value) const { return data + ui16(value * 256.0f); }
   inline cf8p8 operator-(cfl32 &value) const { return data - ui16(value * 256.0f); }

   inline cf8p8 operator+=(cf8p8 &value) { return (data += value.data); }
   inline cf8p8 operator-=(cf8p8 &value) { return (data -= value.data); }
   inline cf8p8 operator+=(cfl32 &value) { return (data += ui16(value * 256.0f)); }
   inline cf8p8 operator-=(cfl32 &value) { return (data -= ui16(value * 256.0f)); }
};

// Normalised 16-bit : Decimal range of 0.0~1.0
struct fp16n0_1 {
   typedef const fp16n0_1 cfp16n0_1;

   ui16 data;

   fp16n0_1(void) = default;
   fp16n0_1(cui16 value) { data = value; }
   fp16n0_1(cui32 value) { data = (ui16 &)value; }
   fp16n0_1(csi32 value) { data = (ui16 &)value; }
   fp16n0_1(cfl32 value) { data = ui16(value * 65535.0f); }

   operator ui16(void) const { return data; }
   operator cfl32(void) const { return cfl32(data) * _fpdt_rcp65535f; }

   inline cfp16n0_1 operator+(cfp16n0_1 &value) const { return data + value.data; }
   inline cfp16n0_1 operator-(cfp16n0_1 &value) const { return data - value.data; }
   inline cfp16n0_1 operator+(cfl32 &value) const { return data + ui16(value * 65535.0f); }
   inline cfp16n0_1 operator-(cfl32 &value) const { return data - ui16(value * 65535.0f); }

   inline cfp16n0_1 operator+=(cfp16n0_1 &value) { return (data += value.data); }
   inline cfp16n0_1 operator-=(cfp16n0_1 &value) { return (data -= value.data); }
   inline cfp16n0_1 operator+=(cfl32 &value) { return (data += ui16(value * 65535.0f)); }
   inline cfp16n0_1 operator-=(cfl32 &value) { return (data -= ui16(value * 65535.0f)); }
};

// Normalised 16-bit : Decimal range of 0.0~2.0
struct fp16n0_2 {
   typedef const fp16n0_2 cfp16n0_2;

   ui16 data;

   fp16n0_2(void) = default;
   fp16n0_2(cui16 value) { data = value; }
   fp16n0_2(cui32 value) { data = (ui16 &)value; }
   fp16n0_2(csi32 value) { data = (ui16 &)value; }
   fp16n0_2(cfl32 value) { data = ui16(value * 32767.5f); }

   operator ui16(void) const { return data; }
   operator cfl32(void) const { return cfl32(data) * _fpdt_2rcp65535f; }

   inline cfp16n0_2 operator+(cfp16n0_2 &value) const { return data + value.data; }
   inline cfp16n0_2 operator-(cfp16n0_2 &value) const { return data - value.data; }
   inline cfp16n0_2 operator+(cfl32 &value) const { return data + ui16(value * 32767.5f); }
   inline cfp16n0_2 operator-(cfl32 &value) const { return data - ui16(value * 32767.5f); }

   inline cfp16n0_2 operator+=(cfp16n0_2 &value) { return (data += value.data); }
   inline cfp16n0_2 operator-=(cfp16n0_2 &value) { return (data -= value.data); }
   inline cfp16n0_2 operator+=(cfl32 &value) { return (data += ui16(value * 32767.5f)); }
   inline cfp16n0_2 operator-=(cfl32 &value) { return (data -= ui16(value * 32767.5f)); }
};

// Normalised 16-bit : Decimal range of 0.0~128.0
struct fp16n0_128 {
   typedef const fp16n0_128 cfp16n0_128;

   ui16 data;

   fp16n0_128(void) = default;
   fp16n0_128(cui16 value) { data = value; }
   fp16n0_128(cui32 value) { data = (ui16 &)value; }
   fp16n0_128(csi32 value) { data = (ui16 &)value; }
   fp16n0_128(cfl32 value) { data = ui16(value * _fpdt_65535div128f); }

   operator ui16(void) const { return data; }
   operator cfl32(void) const { return fl32(data) * _fpdt_128rcp65535f; }

   inline cfp16n0_128 operator+(cfp16n0_128 &value) const { return data + value.data; }
   inline cfp16n0_128 operator-(cfp16n0_128 &value) const { return data - value.data; }
   inline cfp16n0_128 operator+(cfl32 &value) const { return data + ui16(value * _fpdt_65535div128f); }
   inline cfp16n0_128 operator-(cfl32 &value) const { return data - ui16(value * _fpdt_65535div128f); }

   inline cfp16n0_128 operator+=(cfp16n0_128 &value) { return (data += value.data); }
   inline cfp16n0_128 operator-=(cfp16n0_128 &value) { return (data -= value.data); }
   inline cfp16n0_128 operator+=(cfl32 &value) { return (data += ui16(value * _fpdt_65535div128f)); }
   inline cfp16n0_128 operator-=(cfl32 &value) { return (data -= ui16(value * _fpdt_65535div128f)); }
};

// Normalised 16-bit : Decimal range of -1.0~1.0
struct fp16n_1_1 {
   typedef const fp16n_1_1 cfp16n_1_1;

   ui16 data;

   fp16n_1_1(void) = default;
   fp16n_1_1(cui16 value) { data = value; }
   fp16n_1_1(cui32 value) { data = (ui16 &)value; }
   fp16n_1_1(csi32 value) { data = (ui16 &)value; }
   fp16n_1_1(cfl32 value) { data = ui16((value + 1.0f) * _fpdt_65535div2f); }

   operator ui16(void) const { return data; }
   operator cfl32(void) const { return fl32(data) * _fpdt_2rcp65535f; }

   inline cfp16n_1_1 operator+(cfp16n_1_1 &value) const { return data + value.data; }
   inline cfp16n_1_1 operator-(cfp16n_1_1 &value) const { return data - value.data; }
   inline cfp16n_1_1 operator+(cfl32 &value) const { return data + ui16((value + 1.0f) * _fpdt_65535div2f); }
   inline cfp16n_1_1 operator-(cfl32 &value) const { return data - ui16((value + 1.0f) * _fpdt_65535div2f); }

   inline cfp16n_1_1 operator+=(cfp16n_1_1 &value) { return (data += value.data); }
   inline cfp16n_1_1 operator-=(cfp16n_1_1 &value) { return (data -= value.data); }
   inline cfp16n_1_1 operator+=(cfl32 &value) { return (data += ui16((value + 1.0f) * _fpdt_65535div2f)); }
   inline cfp16n_1_1 operator-=(cfl32 &value) { return (data -= ui16((value + 1.0f) * _fpdt_65535div2f)); }
};

// Normalised 16-bit : Decimal range of -128.0~128.0
struct fp16n_128_128 {
   typedef const fp16n_128_128 cfp16n_128_128;

   ui16 data;

   fp16n_128_128(void) = default;
   fp16n_128_128(cui16 value) { data = value; }
   fp16n_128_128(cui32 value) { data = (ui16 &)value; }
   fp16n_128_128(csi32 value) { data = (ui16 &)value; }
   fp16n_128_128(cfl32 value) { data = ui16((value + 128.0f) * _fpdt_65535div256f); }

   operator ui16(void) const { return data; }
   operator cfl32(void) const { return fl32(data) * _fpdt_256rcp65535f - 128.0f; }

   inline cfp16n_128_128 operator+(cfp16n_128_128 &value) const { return data + value.data; }
   inline cfp16n_128_128 operator-(cfp16n_128_128 &value) const { return data - value.data; }
   inline cfp16n_128_128 operator+(cfl32 &value) const { return data + ui16((value + 128.0f) * _fpdt_65535div256f); }
   inline cfp16n_128_128 operator-(cfl32 &value) const { return data - ui16((value + 128.0f) * _fpdt_65535div256f); }

   inline cfp16n_128_128 operator+=(cfp16n_128_128 &value) { return (data += value.data); }
   inline cfp16n_128_128 operator-=(cfp16n_128_128 &value) { return (data -= value.data); }
   inline cfp16n_128_128 operator+=(cfl32 &value) { return (data += ui16((value + 128.0f) * _fpdt_65535div256f)); }
   inline cfp16n_128_128 operator-=(cfl32 &value) { return (data -= ui16((value + 128.0f) * _fpdt_65535div256f)); }
};

// 4x normalised 16-bit : Decimal ranges of 0.0~1.0
struct fp16n0_1x4 {
   typedef const fp16n0_1x4 cfp16n0_1x4;

   union {
      ui64     data64;
      fp16n0_1 data[4];
      VEC4Du16 vec4D;
      ui16     data16[4];
   };

   fp16n0_1x4(void) = default;
   fp16n0_1x4(fp16n0_1 value, cui16 index) { data[index] = value; }
   fp16n0_1x4(cui8 value, cui16 index) { data16[index] = value; }
   fp16n0_1x4(cfl32 value, cui16 index) { data16[index] = ui16(value * 65535.0f); }
   fp16n0_1x4(fp16n0_1 (&value)[4]) { data64 = (ui64 &)value; }
   fp16n0_1x4(cui16 (&value)[4]) { data64 = (ui64 &)value; }
   fp16n0_1x4(cui64 value) { data64 = value; }
   fp16n0_1x4(cfl32x4 value) {
      data64 = (ui64 &)_mm_shuffle_epi8( _mm_cvttps_epu32(
         _mm_mul_ps(value, _fpdt_65535fx4)), _fpdt_shuffle16s);
   }

   operator cui64(void) const { return data64; }
   operator cfl32x4(void) const {
      cui128 temp = _mm_cvtsi64_si128(data64);
      return _mm_mul_ps(_mm_cvtepu32_ps(_mm_cvtepu8_epi32(temp)), _fpdt_rcp255fx4);
   }

   inline cfp16n0_1x4 operator+(cfp16n0_1x4 &value) const { return data64 + value.data64; }
   inline cfp16n0_1x4 operator-(cfp16n0_1x4 &value) const { return data64 - value.data64; }
   inline cfp16n0_1x4 operator+(cfl32x4 &value) const {
      return data64 + (ui64 &)_mm_shuffle_epi8( _mm_cvttps_epu32(
         _mm_mul_ps(value, _fpdt_65535fx4)), _fpdt_shuffle16s);
   }
   inline cfp16n0_1x4 operator-(cfl32x4 &value) const {
      return data64 - (ui64 &)_mm_shuffle_epi8( _mm_cvttps_epu32(
         _mm_mul_ps(value, _fpdt_65535fx4)), _fpdt_shuffle16s);
   }

   inline cfp16n0_1x4 operator+=(cfp16n0_1x4 &value) { return (data64 += value.data64); }
   inline cfp16n0_1x4 operator-=(cfp16n0_1x4 &value) { return (data64 -= value.data64); }
   inline cfp16n0_1x4 operator+=(cfl32x4 &value) {
      return (data64 += (ui64 &)_mm_shuffle_epi8( _mm_cvttps_epu32(
         _mm_mul_ps(value, _fpdt_65535fx4)), _fpdt_shuffle16s));
   }
   inline cfp16n0_1x4 operator-=(cfl32x4 &value) {
      return (data64 -= (ui64 &)_mm_shuffle_epi8( _mm_cvttps_epu32(
         _mm_mul_ps(value, _fpdt_65535fx4)), _fpdt_shuffle16s));
   }
};

// Fixed range within 8 bits
typedef const f0p8          cf0p8;      // 8-bit, 0.8 : Decimal range of 0.0~0.99609375
typedef const f1p7          cf1p7;      // 8-bit, 1.7 : Decimal range of 0.0~1.9921875
typedef const f4p4          cf4p4;      // 8-bit, 4.4 : Decimal range of 0.0~15.9375
typedef const fp8n0_1       cfp8n0_1;   // Normalised 8-bit : Decimal range of 0.0~1.0
typedef const fp8n0_1x4     cfp8n0_1x4; // 4x normalised 8-bit : Decimal ranges of 0.0~1.0

// Fixed range within 16 bits
typedef const f0p16         cf0p16;         // 16-bit, 0.16 : Decimal range of 0.0~0.9999847412109375
typedef const f1p15         cf1p15;         // 16-bit, 1.15 : Decimal range of 0.0~1.999969482421875
typedef const fs7p8         cfs7p8;         // 16-bit, signed 7.8 : Decimal range of -128.0~127.99609375
typedef const f7p9          cf7p9;          // 16-bit, 7.9 : Decimal range of 0.0~127.998046875
typedef const f8p8          cf8p8;          // 16-bit, 8.8 : Decimal range of 0.0~255.99609375
typedef const fp16n0_1      cfp16n0_1;      // Normalised 16-bit : Decimal range of 0.0~1.0
typedef const fp16n0_2      cfp16n0_2;      // Normalised 16-bit : Decimal range of 0.0~2.0
typedef const fp16n0_128    cfp16n0_128;    // Normalised 16-bit : Decimal range of 0.0~128.0
typedef const fp16n_1_1     cfp16n_1_1;     // Normalised 16-bit : Decimal range of -1.0~1.0
typedef const fp16n_128_128 cfp16n_128_128; // Normalised 16-bit : Decimal range of -128.0~128.0
typedef const fp16n0_1x4    cfp16n0_1x4;    // 4x normalised 16-bit : Decimal ranges of 0.0~1.0

