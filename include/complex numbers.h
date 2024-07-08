/************************************************************
* File: complex numbers.h              Created: 2024/07/01 *
*                                Last modified: 2024/07/01 *
*                                                          *
* Notes:                                                   *
*                                                          *
* MIT license.            Copyright (c) David William Bull *
************************************************************/
#pragma once

#include "typedefs.h"

#define _COMPLEX_NUMBERS_

#ifndef _VECTOR_STRUCTURES_
typedef struct {
   ui16 _ui16[2];
   struct {
      union { ui16 u, x; };
      union { ui16 v, y; };
   };
} VEC2Du16;

typedef struct {
   ui32 _ui32[2];
   struct {
      union { ui32 u, x; };
      union { ui32 v, y; };
   };
} VEC2Du32;

typedef const VEC2Du16 cVEC2Du16;
typedef const VEC2Du32 cVEC2Du32;
#endif

// Rational number; 2x 16-bit unsigned integer
al4 union rn16 {
   struct { ui16 n, d; };
   struct { ui16 num, denom; };
   ui16 _ui16[2];

   rn16(void) = default;
   rn16(cui16 numerator, cui16 denominator) { n = numerator; d = denominator; }
   rn16(VEC2Du16 value) { n = value.x; d = value.y; }
   rn16(fl64 value) {
      if(value == 0.0) { n = 0; d = 1; return; }
      ui64 temp = ui64(value * 400.0);
      if(!(temp & 15u)) { n = ui16(temp) >> 4u; d = 25u; return; }
      else if(!(temp & 7u)) { n = ui16(temp) >> 3u; d = 50u; return; }
      else if(!(temp & 3u)) { n = ui16(temp) >> 2u; d = 100u; return; }
      else if(!(temp & 1u)) { n = ui16(temp) >> 1u; d = 200u; return; }
      n = ui16(temp); d = 400u;
   };

   operator ptrc(void) const { return *this; }
   operator cbool(void) const { return n ? true : false; }
   operator cui16(void) const { return n / d; }
   operator fl32(void) const { return fl32(n) / fl32(d); }
   operator fl64(void) const { return fl64(n) / fl64(d); }
   operator cVEC2Du16(void) const { return { n, d }; }
   operator cVEC2Du32(void) const { return { ui32(n), ui32(d) }; }
};

// Rational number; 32-bit & 16-bit unsigned integers
al2 union rn3216 {
   #define n32 *(ui32ptr)n

   struct { ui16 n[2], d; };
   struct { ui16 num[2], denom; };
   ui16 _ui16[3];

   rn3216(void) = default;
   rn3216(cui32 numerator, cui16 denominator) { n32 = numerator; d = denominator; }
   rn3216(VEC2Du16 value) { n32 = (ui32)value.x; d = value.y; }
   rn3216(VEC2Du32 value) { n32 = value.x; d = (ui16 &)value.y; }
   rn3216(fl64 value) {
      if(value == 0.0) { n32 = 0; d = 1; return; }
      ui64 temp = ui64(value * 40000.0);
      if(!(temp & 63u)) { n32 = ui32(temp) >> 6u; d = 625u; return; }
      else if(!(temp & 31u)) { n32 = ui32(temp) >> 5u; d = 1250u; return; }
      else if(!(temp & 15u)) { n32 = ui32(temp) >> 4u; d = 2500u; return; }
      else if(!(temp & 7u)) { n32 = ui32(temp) >> 3u; d = 5000u; return; }
      else if(!(temp & 3u)) { n32 = ui32(temp) >> 2u; d = 10000u; return; }
      else if(!(temp & 1u)) { n32 = ui32(temp) >> 1u; d = 20000u; return; }
      n32 = ui32(temp); d = 40000u;
   };

   operator ptrc(void) const { return *this; }
   operator cbool(void) const { return n32 ? true : false; }
   operator cui32(void) const { return n32 / ui32(d); }
   operator fl32(void) const { return fl32(n32) / fl32(d); }
   operator fl64(void) const { return fl64(n32) / fl64(d); }
   operator cVEC2Du16(void) const { return { n[0], d}; }
   operator cVEC2Du32(void) const { return { n32, ui32(d) }; }

   #undef n32
};
