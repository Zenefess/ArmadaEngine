/************************************************************
 * File: Fixed-point math.h             Created: 2023/06/25 *
 *                                    Last mod.: 2023/07/06 *
 *                                                          *
 * Desc:                                                    *
 *                                                          *
 * MIT license.            Copyright (c) David William Bull *
 ************************************************************/
#pragma once

#include "typedefs.h"
#include "Vector structures.h"

static cfl32 _fpm_65535f    = 65535.0f;
static cfl32 _fpm_rcp65535f = 1.0f / 65535.0f;

/*********************************************
 *  Floating-point to fixed-point functions  *
 *********************************************/

/*
 *  Fixed range conversion functions
 */

// Convert 32-bit float (range 0.0~1.0) to 8-bit fixed
inline cui8 Fix8(cfl32 scalar) { return ui8(scalar * 255.0f); }

#ifdef _24BIT_INTEGERS_
// Convert 32-bit float (range 0.0~65535.99609375) to 16.8 fixed
inline cui24 Fix16p8(cfl32 scalar) { return ui24(scalar * 256.0f); }
#endif

// Convert 32-bit float (range 0.0~65535.99609375) to 16.8 fixed-point
inline void Fix16p8(ui8 (&dest)[3], cfl32 scalar) {
   const union { cui32 u32; cui16 u16; cui8 u8s[3]; } result = { ui32(scalar * 256.0f) };

   ((ui16 *)dest)[0] = result.u16;
   dest[2] = result.u8s[2];
}

// Convert 4 32-bit floats (range 0.0~1.0) to 8-bit fixed
inline cVEC4Du8 Fix8x4(cVEC4Df scalars) {
   return { ui8(scalars.x1 * 255.0f), ui8(scalars.y1 * 255.0f),
            ui8(scalars.x2 * 255.0f), ui8(scalars.y2 * 255.0f) };
}

// Convert 4 32-bit floats (range 0.0~1.0) to packed fixed-point colour values: R8G9B7A8
inline cui32 Fix8978(cVEC4Df scalars) {
   return { (ui32(scalars.r * 255.0f) & 0x0FF) | ((ui32(scalars.g * 511.0f) & 0x01FF) << 8) |
            ((ui32(scalars.b * 127.0f) & 0x07F) << 17) | ((ui32(scalars.a * 255.0f) & 0x0FF) << 24) };
}

// Convert 4 32-bit floats (range 0.0~1.0) to 16-bit fixed
inline cVEC4Du16 Fix16x4(cVEC4Df scalars) {
   return { ui16(scalars.r * 65535.0f), ui16(scalars.g * 65535.0f),
            ui16(scalars.b * 65535.0f), ui16(scalars.a * 65535.0f) };
}

// Convert 4 32-bit floats (range 0.0~1.0) to 16-bit fixed
inline cVEC4Du16 Fix16x4(cSSE4Df32 scalars) {
   cSSE4Df32 result = { .xmm = _mm_mul_ps(scalars.xmm, _mm_broadcast_ss(&_fpm_65535f)) };

   return { ui16(result.xmm.m128_f32[0]), ui16(result.xmm.m128_f32[1]),
            ui16(result.xmm.m128_f32[2]), ui16(result.xmm.m128_f32[3]) };
}

// Convert 4 32-bit floats (range 0.0~1.0) to 16-bit fixed, then return 4 copies of the result
inline cAVX16Du16 Fix16x4x4(cSSE4Df32 scalars) {
   cSSE4Df32 interm = { .xmm = _mm_mul_ps(scalars.xmm, _mm_broadcast_ss(&_fpm_65535f)) };
   cVEC4Du16 result = { ui16(interm.xmm.m128_f32[0]), ui16(interm.xmm.m128_f32[1]),
                        ui16(interm.xmm.m128_f32[2]), ui16(interm.xmm.m128_f32[3]) };

   return { .ymm = _mm256_broadcastq_epi64((__m128i &)result) };
}

/*
 *  Scalable range conversion functions
 */

// Convert 32-bit float to 8-bit fixed
inline cui8 Fix8(cfl32 input, cfl32 scale) { return ui32(input * scale) & 0x0FF; }

// Convert 32-bit float to 16-bit fixed
inline cui16 Fix16(cfl32 input, cfl32 scale) { return ui32(input * scale) & 0x0FFFF; }

// Convert 32-bit float to 32-bit fixed
inline cui32 Fix32(cfl32 input, cfl32 scale) { return ui32(input * scale); }

// Convert 2 32-bit floats to 16-bit fixeds
inline cVEC2Du16 Fix16x2(cVEC2Df floats, cfl32 scale) {
   return { ui16(floats.x * scale), ui16(floats.y * scale) };
}

// Convert 4 32-bit floats to 8-bit fixeds
inline cVEC4Du8 Fix8x4(cVEC4Df floats, cfl32 scale) {
   return { ui8(floats.x1 * scale), ui8(floats.y1 * scale),
            ui8(floats.x2 * scale), ui8(floats.y2 * scale) };
}

// Convert 4 32-bit floats to 16-bit fixeds
inline cVEC4Du16 Fix16x4(cVEC4Df floats, cfl32 scale) {
   return { ui16(floats.x1 * scale), ui16(floats.y1 * scale),
            ui16(floats.x2 * scale), ui16(floats.y2 * scale) };
}

/*
 *  Scalable floor & range conversion functions
 */

// Convert 32-bit float to 8-bit fixed
inline cui8 Fix8(cfl32 input, cfl32 floor, cfl32 range) { return ui32((input - floor) / range * 255.0f) & 0x0FF; }

// Convert 32-bit float to 16-bit fixed
inline cui16 Fix16(cfl32 input, cfl32 floor, cfl32 range) { return ui32((input - floor) / range * 65535.0f) & 0x0FFFF; }

// Convert 2 32-bit floats to 16-bit fixeds
inline cVEC2Du16 Fix16x2(cVEC2Df floats, cfl32 floor, cfl32 range) {
   cfl32 rangeExp = 65535.0f / range;

   return { ui16((floats.x - floor) * rangeExp), ui16((floats.y - floor) * rangeExp) };
}

// Convert 4 32-bit floats to 8-bit fixeds
inline cVEC4Du8 Fix8x4(cVEC4Df floats, cfl32 floor, cfl32 range) {
   cfl32 rangeExp = 255.0f / range;

   return { ui8((floats.x1 - floor) * rangeExp), ui8((floats.y1 - floor) * rangeExp),
            ui8((floats.x2 - floor) * rangeExp), ui8((floats.y2 - floor) * rangeExp) };
}

// Convert 4 32-bit floats to 16-bit fixeds
inline cVEC4Du16 Fix16x4(cVEC4Df floats, cfl32 floor, cfl32 range) {
   cfl32 rangeExp = _fpm_65535f / range;

   return { ui16((floats.x1 - floor) * rangeExp), ui16((floats.y1 - floor) * rangeExp),
            ui16((floats.x2 - floor) * rangeExp), ui16((floats.y2 - floor) * rangeExp) };
}

/*********************************************
 *  Fixed-point to floating-point functions  *
 *********************************************/

/*
 *  Fixed range conversion functions
 */



/*
 *  Scalable range conversion functions
 */

// Convert 16-bit fixed to 32-bit float
inline cfl32 Float16(cui16 input, cfl32 range) { return fl32(input) * range * _fpm_rcp65535f; }

// Convert 2 16-bit fixeds to 32-bit floats
inline cVEC2Df Float16x2(cVEC2Du16 ui16s, cfl32 range) {
   cfl32 rangeRCP = range * _fpm_rcp65535f;

   return { fl32(ui16s.x) * rangeRCP, fl32(ui16s.y) * rangeRCP };
}

// Convert 4 16-bit fixeds to 32-bit floats
inline cVEC4Df Float16x4(cVEC4Du16 ui16s, cfl32 range) {
   cfl32 rangeRCP = range * _fpm_rcp65535f;

   return { fl32(ui16s.x1) * rangeRCP, fl32(ui16s.y1) * rangeRCP,
            fl32(ui16s.x2) * rangeRCP, fl32(ui16s.y2) * rangeRCP };
}

/*
 *  Scalable floor & range conversion functions
 */

// Convert 4 16-bit fixeds to 32-bit floats
inline cVEC4Df Float16x4(cVEC4Du16 ui16s, cfl32 floor, cfl32 range) {
   cfl32 rangeRCP = range * _fpm_rcp65535f;

   return { fl32(ui16s.x1) * rangeRCP + floor, fl32(ui16s.y1) * rangeRCP + floor,
            fl32(ui16s.x2) * rangeRCP + floor, fl32(ui16s.y2) * rangeRCP + floor };
}
