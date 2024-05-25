/************************************************************
 * File: common.hlsli                   Created: 2023/01/18 *
 *                                    Last mod.: 2023/05/14 *
 *                                                          *
 * Desc: Common shader presets.                             *
 *                                                          *
 * Copyright (c) David William Bull.   All rights reserved. *
 ************************************************************/

#ifndef COMMON
#define COMMON

// Emualted data types
struct ui64 { uint low, high; };               // 64bit unsigned integer
struct si64 { int low, high; };                // 64bit signed integer
struct TIME { uint seconds; float fraction; }; // High-resolution time variable

// Type defines
typedef int2   int2x18[18];
typedef float3 float3x18[18];
typedef uint4  uint4x8[8];
typedef int4   int4x11[11];

typedef const bool      cbool;
typedef const uint      cuint;
typedef const uint2     cuint2;
typedef const uint3     cuint3;
typedef const uint4     cuint4;
typedef const ui64      cui64;
typedef const uint2x2   cuint2x2;
typedef const uint3x2   cuint3x2;
typedef const uint4x2   cuint4x2;
typedef const uint2x3   cuint2x3;
typedef const uint3x3   cuint3x3;
typedef const uint4x3   cuint4x3;
typedef const uint2x4   cuint2x4;
typedef const uint3x4   cuint3x4;
typedef const uint4x4   cuint4x4;
typedef const uint4x8   cuint4x8;
typedef const int       cint;
typedef const int2      cint2;
typedef const int3      cint3;
typedef const int4      cint4;
typedef const si64      csi64;
typedef const int2x2    cint2x2;
typedef const int3x2    cint3x2;
typedef const int4x2    cint4x2;
typedef const int2x3    cint2x3;
typedef const int3x3    cint3x3;
typedef const int4x3    cint4x3;
typedef const int2x4    cint2x4;
typedef const int3x4    cint3x4;
typedef const int4x4    cint4x4;
typedef const int4x11   cint4x11;
typedef const float     cfloat;
typedef const float2    cfloat2;
typedef const float3    cfloat3;
typedef const float4    cfloat4;
typedef const float2x2  cfloat2x2;
typedef const float3x2  cfloat3x2;
typedef const float4x2  cfloat4x2;
typedef const float2x3  cfloat2x3;
typedef const float3x3  cfloat3x3;
typedef const float4x3  cfloat4x3;
typedef const float2x4  cfloat2x4;
typedef const float3x4  cfloat3x4;
typedef const float4x4  cfloat4x4;
typedef const float3x18 cfloat3x18;
typedef const vector    cvector;
typedef const matrix    cmatrix;
typedef const TIME      cTIME;

// Static expressions
static const float4 rcp8978 = float4(0.0039215686274509803921568627451f, 0.00195694716242661448140900195695f, 0.00787401574803149606299212598425f, 0.0039215686274509803921568627451f);

static const uint4 shift8888 = uint4(0, 8, 16, 24);
static const uint4 shift8978 = uint4(0, 8, 17, 24);

static const uint4 mask8978  = uint4(0x0FF, 0x01FF, 0x07F, 0x0FF);

static const float rcp16    = 0.0625f;
static const float rcp255   = 0.0039215686274509803921568627451f;
static const float rcp256   = 0.00390625f;
static const float rcp511   = 0.00195694716242661448140900195695f;
static const float rcp512   = 0.001953125f;
static const float rcp16384 = 0.00006103515625f;
static const float rcp21845 = 4.5777065690089265278095674067292e-5f;
static const float rcp32768 = 0.000030517578125f;
static const float rcp65534 = 0.000015259254737998596148564104129154f;
static const float rcp65535 = 0.000015259021896696421759365224689097f;

static const float  PIf     = 3.1415926535897932384626433832795f;
static const double PId     = 3.1415926535897932384626433832795;
static const float  sqrt05f = 0.70710678118654752440084436210485f;
static const double sqrt05d = 0.70710678118654752440084436210485;
static const float  sqrt20f = 1.4142135623730950488016887242097f;
static const double sqrt20d = 1.4142135623730950488016887242097;

// ALU functions for emulated data type: ui64
inline void add64(inout ui64 a, in cui64 b) { const uint c = a.low; a.low += b.low; a.high += b.high + (a.low < c); }

// PRNG
inline int2 rand_ui15(in const uint2 indices) {
   const int2 i = (indices << 13u) ^ indices;
   return (i * (i * i * 15731u + 789221u) + 1376312589u) & 0x07FFF;
}

inline uint rand_ui31(in const uint index) {
   const uint i = (index << 13u) ^ index;
   return (i * (i * i * 15731u + 789221u) + 1376312589u) & 0x07FFFFFFF;
}

inline uint2 rand_ui31(in const uint2 indices) {
   const uint2 i = (indices << 13u) ^ indices;
   return (i * (i * i * 15731u + 789221u) + 1376312589u) & 0x07FFFFFFF;
}

inline uint4 rand_ui31(in const uint4 indices) {
   const uint4 i = (indices << 13u) ^ indices;
   return (i * (i * i * 15731u + 789221u) + 1376312589u) & 0x07FFFFFFF;
}

// Exchange the values of 2 variables
inline void Swap(inout uint a, inout uint b) { a ^= b; b ^= a; a ^= b; };

inline void Swap(inout uint2 v) { v.x ^= v.y; v.y ^= v.x; v.x ^= v.y; };

// Sort 4 indices based on ratios
inline void Sort(in const uint4 ratios, inout uint4 indices) {
   if(ratios.x < ratios.y) { indices.x ^= indices.y; indices.y ^= indices.x; indices.x ^= indices.y; };
   if(ratios.y < ratios.z) { indices.y ^= indices.z; indices.z ^= indices.y; indices.y ^= indices.z; };
   if(ratios.z < ratios.w) { indices.z ^= indices.w; indices.w ^= indices.z; indices.z ^= indices.w; };
   if(ratios.x < ratios.y) { indices.x ^= indices.y; indices.y ^= indices.x; indices.x ^= indices.y; };
   if(ratios.y < ratios.z) { indices.y ^= indices.z; indices.z ^= indices.y; indices.y ^= indices.z; };
   if(ratios.x < ratios.y) { indices.x ^= indices.y; indices.y ^= indices.x; indices.x ^= indices.y; };
}

// Vector rotation
inline float2 RotateVector(in float2 v, in const float2 dir) { return float2(dot(v, float2(dir.x, -dir.y)), dot(v, dir.yx)); }

// Calculate sin & cos values for orientionation of triangle, then pack into uints
inline uint3 PackSinCosFromVerts(in const float3 vert0, in const float3 vert1, in const float3 vert2) {
   const float2x3 fUV     = float2x3(vert1 - vert0, vert2 - vert0);
   const float3   fNormal = float3((fUV[0].y * fUV[1].z) - (fUV[0].z * fUV[1].y),
                                   (fUV[0].z * fUV[1].x) - (fUV[0].x * fUV[1].z),
                                   (fUV[0].x * fUV[1].y) - (fUV[0].y * fUV[1].x));
   const float2   fPerp   = float2(0.0f, -1.0f);

   const float3x2 fSinCos = { (length(fNormal.yz) == 0.0f ? fPerp : normalize(fNormal.yz)),
                              (length(fNormal.xz) == 0.0f ? fPerp : normalize(fNormal.xz)),
                              (length(fNormal.xy) == 0.0f ? fPerp : normalize(fNormal.xy)) };
   
   return f32tof16(-float3(fSinCos[0].y, fSinCos[1].y, fSinCos[2].y)) | (f32tof16(float3(fSinCos[0].x, fSinCos[1].x, fSinCos[2].x)) << 16);
}

inline uint2 PackXYSinCosFromVerts(in const float3 vert0, in const float3 vert1, in const float3 vert2) {
   const float2x3 fUV     = float2x3(vert1 - vert0, vert2 - vert0);
   const float3   fNormal = float3((fUV[0].y * fUV[1].z) - (fUV[0].z * fUV[1].y),
                                   (fUV[0].z * fUV[1].x) - (fUV[0].x * fUV[1].z),
                                   (fUV[0].x * fUV[1].y) - (fUV[0].y * fUV[1].x));
   const float2   fPerp   = float2(0.0f, -1.0f);

   const float2x2 fSinCos = { (length(fNormal.yz) == 0.0f ? fPerp : normalize(fNormal.yz)),
                              (length(fNormal.xz) == 0.0f ? fPerp : normalize(fNormal.xz)) };
   
   return f32tof16(-float2(fSinCos[0].y, fSinCos[1].y)) | (f32tof16(float2(fSinCos[0].x, fSinCos[1].x)) << 16);
}

// Denormalise: 2x packed uint16 -> 2x float
inline float2 Float16x2(in const uint input, in const float floor, in const float ceiling) {
   const uint2  prefloat  = uint2(input & 0x0FFFF, input >> 16);
   const float2 postfloat = float2(prefloat) * 0.000015259021896696421759365224689097f;

   return postfloat * (ceiling - floor) + floor;
}

// Denormalise: 4x packed uint16 -> 4x float
inline float4 Float16x4(in const uint2 input, in const float floor, in const float ceiling) {
   const uint4  prefloat  = uint4(input & 0x0FFFF, input >> 16);
   const float4 postfloat = float4(prefloat.xzyw) * 0.000015259021896696421759365224689097f;

   return postfloat * (ceiling - floor) + floor;
}

// Convert & add integer seconds and floating-point reaminder to double
inline double Seconds(in const uint seconds, in const float fraction) {
   return double(seconds) + double(fraction);
}

inline double Seconds(in cTIME time ) {
   return double(time.seconds) + double(time.fraction);
}

#endif