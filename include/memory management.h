/************************************************************
 * File: memory management.h            Created: 2008/12/08 *
 *                                Last modified: 2024/05/26 *
 *                                                          *
 * Notes: 2024/05/02: Added support for data tracking.      *
 *                                                          *
 *                         Copyright (c) David William Bull *
 ************************************************************/
#pragma once

#pragma intrinsic(_InterlockedExchange64)

#include <corecrt_malloc.h>
#include "typedefs.h"

#ifdef DATA_TRACKING
#include "data tracking.h"
extern SYSTEM_DATA sysData;
#endif

#define malloc16(byteCount) malloc(byteCount, 16)
#define malloc32(byteCount) malloc(byteCount, 32)

inline ptrc malloc(csize_t byteCount, csize_t alignment) {
   ptrc pointer = _aligned_malloc(byteCount, alignment);
#ifdef DATA_TRACKING
   if(pointer) {
      sysData.mem.byteCount[sysData.mem.allocations]  = byteCount;
      sysData.mem.location[sysData.mem.allocations++] = pointer;
      sysData.mem.allocated += byteCount;
   }
#endif
   return pointer;
}

#define bitPatternx8 bitPattern, bitPattern, bitPattern, bitPattern, bitPattern, bitPattern, bitPattern, bitPattern

// Allocates RAM on 16-byte boundary, then sets the entire array to a repeating 128-bit pattern
inline ptrc salloc16(csize_t byteCount, cui128 bitPattern);

// Allocates RAM on 16-byte boundary, then sets the entire array to a repeating 8-bit pattern
inline ptrc salloc16(csize_t byteCount, cui8 bitPattern) {
   return salloc16(byteCount, cui128{ .m128i_u8 = { bitPatternx8, bitPatternx8 } });
}

// Allocates RAM on 16-byte boundary, then sets the entire array to a repeating 16-bit pattern
inline ptrc salloc16(csize_t byteCount, cui16 bitPattern) {
   return salloc16(byteCount, cui128{ .m128i_u16 = { bitPatternx8 } });
}

// Allocates RAM on 16-byte boundary, then sets the entire array to a repeating 32-bit pattern
inline ptrc salloc16(csize_t byteCount, cui32 bitPattern) {
   return salloc16(byteCount, cui128{ .m128i_u32 = { bitPattern, bitPattern, bitPattern, bitPattern } });
}

// Allocates RAM on 16-byte boundary, then sets the entire array to a repeating 64-bit pattern
inline ptrc salloc16(csize_t byteCount, cui64 bitPattern) {
   return salloc16(byteCount, cui128{ .m128i_u64 = { bitPattern, bitPattern } });
}

// Allocates RAM on 32-byte boundary, then sets the entire array to a repeating 256-bit pattern
inline ptrc salloc32(csize_t byteCount, cui256 bitPattern);

// Allocates RAM on 32-byte boundary, then sets the entire array to a repeating 8-bit pattern
inline ptrc salloc32(csize_t byteCount, cui8 bitPattern) {
   return salloc32(byteCount, cui256{ .m256i_u8 = { bitPatternx8, bitPatternx8, bitPatternx8, bitPatternx8 } });
}

// Allocates RAM on 32-byte boundary, then sets the entire array to a repeating 16-bit pattern
inline ptrc salloc32(csize_t byteCount, cui16 bitPattern) {
   return salloc32(byteCount, cui256{ .m256i_u16 = { bitPatternx8, bitPatternx8 } });
}

// Allocates RAM on 32-byte boundary, then sets the entire array to a repeating 32-bit pattern
inline ptrc salloc32(csize_t byteCount, cui32 bitPattern) {
   return salloc32(byteCount, cui256{ .m256i_u32 = { bitPatternx8 } });
}

// Allocates RAM on 32-byte boundary, then sets the entire array to a repeating 64-bit pattern
inline ptrc salloc32(csize_t byteCount, cui64 bitPattern) {
   return salloc32(byteCount, cui256{ .m256i_u64 = { bitPattern, bitPattern, bitPattern, bitPattern } });
}

// Allocates RAM on 32-byte boundary, then sets the entire array to a repeating 128-bit pattern
inline ptrc salloc32(csize_t byteCount, cui128 bitPattern) {
   return salloc32(byteCount, cui256{ .m256i_u64 =
      { bitPattern.m128i_u64[0], bitPattern.m128i_u64[1], bitPattern.m128i_u64[0], bitPattern.m128i_u64[1]} });
}

// Allocates RAM on 64-byte boundary, then sets the entire array to a repeating 512-bit pattern
inline ptrc salloc64(csize_t byteCount, cui512 bitPattern);

// Allocates RAM on 64-byte boundary, then sets the entire array to a repeating 8-bit pattern
inline ptrc salloc64(csize_t byteCount, cui8 bitPattern) {
   return salloc64(byteCount, cui512{ .m512i_u8 = { bitPatternx8, bitPatternx8, bitPatternx8, bitPatternx8,
                                                    bitPatternx8, bitPatternx8, bitPatternx8, bitPatternx8 } });
}

// Allocates RAM on 64-byte boundary, then sets the entire array to a repeating 16-bit pattern
inline ptrc salloc64(csize_t byteCount, cui16 bitPattern) {
   return salloc64(byteCount, cui512{ .m512i_u16 = { bitPatternx8, bitPatternx8, bitPatternx8, bitPatternx8 } });
}

// Allocates RAM on 64-byte boundary, then sets the entire array to a repeating 32-bit pattern
inline ptrc salloc64(csize_t byteCount, cui32 bitPattern) {
   return salloc64(byteCount, cui512{ .m512i_u32 = { bitPatternx8, bitPatternx8 } });
}

// Allocates RAM on 64-byte boundary, then sets the entire array to a repeating 64-bit pattern
inline ptrc salloc64(csize_t byteCount, cui64 bitPattern) {
   return salloc64(byteCount, cui512{ .m512i_u64 = { bitPattern, bitPattern, bitPattern, bitPattern,
                                                     bitPattern, bitPattern, bitPattern, bitPattern } });
}

// Allocates RAM on 64-byte boundary, then sets the entire array to a repeating 128-bit pattern
inline ptrc salloc64(csize_t byteCount, cui128 bitPattern) {
   return salloc64(byteCount, cui512{ .m512i_u64 =
      { bitPattern.m128i_u64[0], bitPattern.m128i_u64[1], bitPattern.m128i_u64[0], bitPattern.m128i_u64[1],
        bitPattern.m128i_u64[0], bitPattern.m128i_u64[1], bitPattern.m128i_u64[0], bitPattern.m128i_u64[1]} });
}

// Allocates RAM on 64-byte boundary, then sets the entire array to a repeating 256-bit pattern
inline ptrc salloc64(csize_t byteCount, cui256 bitPattern) {
   return salloc64(byteCount, cui512{ .m512i_u64 =
      { bitPattern.m256i_u64[0], bitPattern.m256i_u64[1], bitPattern.m256i_u64[2], bitPattern.m256i_u64[3],
        bitPattern.m256i_u64[0], bitPattern.m256i_u64[1], bitPattern.m256i_u64[2], bitPattern.m256i_u64[3]} });
}

// Allocates RAM on 16-byte boundary, then sets the entire array to zero
inline ptrc zalloc16(csize_t byteCount) {
   return salloc16(byteCount, cui128{ .m128i_u64 = { 0x0, 0x0 } });
}

// Allocates RAM on 32-byte boundary, then sets the entire array to zero
inline ptrc zalloc32(csize_t byteCount) {
   return salloc32(byteCount, cui256{ .m256i_u64 = { 0x0, 0x0, 0x0, 0x0 } });
}

// Allocates RAM on 64-byte boundary, then sets the entire array to zero
inline ptrc zalloc64(csize_t byteCount) {
   return salloc64(byteCount, cui512{ .m512i_u64 = { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 } });
}

inline ptrc salloc16(csize_t numBytes, cui128 bitPattern) {
   ptrc pointer = _aligned_malloc(numBytes, 16);
   if(pointer) {
#ifdef DATA_TRACKING
      sysData.mem.byteCount[sysData.mem.allocations]  = numBytes;
      sysData.mem.location[sysData.mem.allocations++] = pointer;
      sysData.mem.allocated += numBytes;
#endif
      cui64 limit = numBytes >> 4;
      ui64  os;

      for(os = 0; os < limit; os++) ((ui128ptr)pointer)[os] = bitPattern;
      for(os <<= 4; os < numBytes; os++) ((ui8ptr)pointer)[os] = bitPattern.m128i_u8[os & 0x0F];
   }
   return pointer;
}

inline ptrc salloc32(csize_t numBytes, cui256 bitPattern) {
   ptrc pointer = _aligned_malloc(numBytes, 32);
   if(pointer) {
#ifdef DATA_TRACKING
      sysData.mem.byteCount[sysData.mem.allocations]  = numBytes;
      sysData.mem.location[sysData.mem.allocations++] = pointer;
      sysData.mem.allocated += numBytes;
#endif
      cui64 limit = numBytes >> 5;
      ui64  os;

      for(os = 0; os < limit; os++) ((ui256ptr)pointer)[os] = bitPattern;
      for(os <<= 5; os < numBytes; os++) ((ui8ptr)pointer)[os] = bitPattern.m256i_u8[os & 0x01F];
   }
   return pointer;
}

inline ptrc salloc64(csize_t numBytes, cui512 bitPattern) {
   ptrc pointer = _aligned_malloc(numBytes, 64);
   if(pointer) {
#ifdef DATA_TRACKING
      sysData.mem.byteCount[sysData.mem.allocations]  = numBytes;
      sysData.mem.location[sysData.mem.allocations++] = pointer;
      sysData.mem.allocated += numBytes;
#endif
      cui64 limit = numBytes >> 6;
      ui64  os;

      for(os = 0; os < limit; os++) ((ui512ptr)pointer)[os] = bitPattern;
      for(os <<= 3; os < (numBytes >> 3); os++) ((ui32ptr)pointer)[os] = bitPattern.m512i_u32[os & 0x0F];
      for(os <<= 6; os < numBytes; os++) ((ui8ptr)pointer)[os] = bitPattern.m512i_u8[os & 0x03F];
   }
   return pointer;
}

// Frees a pointer and returns true if successful.
#define mfree1(pointer) mdealloc(pointer)

// Frees a variable number of pointers. Each bit represents each pointer (in argument order), and will be true if the relevant pointer is freed.
#define mfree(pointer, ...) mdealloc_(pointer, __VA_ARGS__, -1)

// Frees a pointer and returns true if successful.
inline cui64 mdealloc(ptrc pointer) {
   if(pointer) {
#ifdef DATA_TRACKING
      cui32     allocations = sysData.mem.allocations;
      vptrcptrc location    = sysData.mem.location;

      ui32 index = 0;

      // Find entry
      while(pointer != location[index] && allocations >= index) index++;

      // Is the pointer invalid?
      if(index >= allocations) return false;

      sysData.mem.allocations--;
      sysData.mem.allocated -= sysData.mem.byteCount[index];
      sysData.mem.location[index] = NULL;
      sysData.mem.byteCount[index] = 0;
#endif
      _aligned_free(pointer);
      return true;
   }
   return false;
}

// Frees a variable number of pointers. Each bit represents each pointer (in argument order), and will be true if the relevant pointer is freed.
inline cui64 mdealloc_(ptrc pointer, ...) {
   va_list val;
   ui64    retVal = 0, ptrBit = 0x01;

   va_start(val, pointer);

   for(ptr arg = va_arg(val, ptrc); (ui64 &)arg != -1; arg = va_arg(val, ptrc), ptrBit <<= 1) {
      if(pointer) {
#ifdef DATA_TRACKING
         cui32     allocations = sysData.mem.allocations;
         vptrcptrc location    = sysData.mem.location;

         ui32 index = 0;

         // Find entry
         while(pointer != location[index] && allocations >= index) index++;

         // Is the pointer invalid?
         if(index >= allocations) continue;

         sysData.mem.allocations--;
         sysData.mem.allocated -= sysData.mem.byteCount[index];
         sysData.mem.location[index] = NULL;
         sysData.mem.byteCount[index] = 0;
#endif
         _aligned_free(pointer);
         retVal |= ptrBit;
      }
   }
   va_end(val);
   return retVal;
}

// Set a region of memory to a repeating pattern
#define setmem(addr, numBytes, bitPattern) mset(addr, numBytes, bitPattern)

// Set a region of memory to a repeating 128-bit pattern
inline void mset(ptrc addr, cui64 numBytes, cui128 bitPattern) {
   cui64 limit = numBytes >> 4;
   ui64  os;

   for(os = 0; os < limit; os++) ((ui128ptr)addr)[os] = bitPattern;
   for(os <<= 4; os < numBytes; os++) ((ui8ptr)addr)[os] = bitPattern.m128i_u8[os & 0x0F];
}

// Set a region of memory to a repeating 256-bit pattern
inline void mset(ptrc addr, cui64 numBytes, cui256 bitPattern) {
   cui64 limit = numBytes >> 5;
   ui64  os;

   for(os = 0; os < limit; os++) ((ui256ptr)addr)[os] = bitPattern;
   for(os <<= 5; os < numBytes; os++) ((ui8ptr)addr)[os] = bitPattern.m256i_u8[os & 0x01F];
}

// Set a region of memory to a repeating 512-bit pattern
inline void mset(ptrc addr, cui64 numBytes, cui512 bitPattern) {
   cui64 limit = numBytes >> 6;
   ui64  os;

   for(os = 0; os < limit; os++) ((ui512ptr)addr)[os] = bitPattern;
   for(os <<= 3; os < (numBytes >> 3); os++) ((ui8ptr)addr)[os] = bitPattern.m512i_u32[os & 0x0F];
   for(os <<= 6; os < numBytes; os++) ((ui8ptr)addr)[os] = bitPattern.m512i_u8[os & 0x03F];
}

// Set a region of memory to a repeating 8-bit pattern
inline void mset(ptrc addr, cui64 numBytes, cui8 bitPattern) {
#ifdef __AVX512__
   cui512 bits = { .m512i_u8 = { bitPatternx8, bitPatternx8, bitPatternx8, bitPatternx8,
                                 bitPatternx8, bitPatternx8, bitPatternx8, bitPatternx8 } };
#else
#ifdef __AVX__
   cui256 bits = { .m256i_u8 = { bitPatternx8, bitPatternx8, bitPatternx8, bitPatternx8 } };
#else
   cui128 bits = { .m128i_u8 = { bitPatternx8, bitPatternx8 } };
#endif
#endif
   mset(addr, numBytes, bits);
}

// Set a region of memory to a repeating 16-bit pattern
inline void mset(ptrc addr, cui64 numBytes, cui16 bitPattern) {
#ifdef __AVX512__
   cui512 bits = { .m512i_u16 = { bitPatternx8, bitPatternx8, bitPatternx8, bitPatternx8 } };
#else
#ifdef __AVX__
   cui256 bits = { .m256i_u16 = { bitPatternx8, bitPatternx8 } };
#else
   cui128 bits = { .m128i_u16 = { bitPatternx8 } };
#endif
#endif
   mset(addr, numBytes, bits);
}

// Set a region of memory to a repeating 32-bit pattern
inline void mset(ptrc addr, cui64 numBytes, cui32 bitPattern) {
#ifdef __AVX512__
   cui512 bits = { .m512i_u32 = { bitPatternx8, bitPatternx8 } };
#else
#ifdef __AVX__
   cui256 bits = { .m256i_u32 = { bitPatternx8 } };
#else
   cui128 bits = { .m128i_u32 = { bitPattern, bitPattern, bitPattern, bitPattern } };
#endif
#endif
   mset(addr, numBytes, bits);
}

// Set a region of memory to a repeating 64-bit pattern
inline void mset(ptrc addr, cui64 numBytes, cui64 bitPattern) {
#ifdef __AVX512__
   cui512 bits = { .m512i_u64 = { bitPatternx8 } };
#else
#ifdef __AVX__
   cui256 bits = { .m256i_u64 = { bitPattern, bitPattern, bitPattern, bitPattern } };
#else
   cui128 bits = { .m128i_u64 = { bitPattern, bitPattern } };
#endif
#endif
   mset(addr, numBytes, bits);
}

// Copy byteCount bytes of unaligned data
inline void Copy(ptrc source, ptrc dest, cui64 byteCount) {
   cui64 k = byteCount >> 2;
   ui64  i = 0;
#ifdef __AVX512__
   cui64 j = byteCount >> 6;
   for(; i < j; i++) ((ui512ptr)dest)[i] = _mm512_loadu_epi64(&((ui512ptr)source)[i]);
   i <<= 4;
#else
#ifdef __AVX__
   cui64 j = byteCount >> 5;
   for(; i < j; i++) ((ui256ptr)dest)[i] = _mm256_lddqu_si256(&((ui256ptr)source)[i]);
   i <<= 3;
#else
   cui64 j = byteCount >> 4;
   for(; i < j; i++) ((ui128ptr)dest)[i] = _mm_lddqu_si128(&((ui128ptr)source)[i]);
   i <<= 2;
#endif
#endif
   for(; i < k; i++) ((ui32ptr)dest)[i] = ((ui32ptr)source)[i];
   for(i = k << 2; i < byteCount; i++) ((ui8ptr)dest)[i] = ((ui8ptr)source)[i];
}

// Copy byteCount (rounded-down to the nearest 8) bytes of data
inline void Copy8(ptrc source, ptrc dest, cui64 byteCount) {
   cui64 j = byteCount >> 3;
   for(ui64 i = 0; i < j; i++) ((ui64ptr)dest)[i] = ((ui64ptr)source)[i];
}

// Copy byteCount (rounded-down to the nearest 8) bytes of data
inline void Copy8(vptrc source, vptrc dest, cui64 byteCount) {
   cui64 j = byteCount >> 3;
   for(ui64 i = 0; i < j; i++) ((vui64ptr)dest)[i] = ((vui64ptr)source)[i];
}

// Copy byteCount (rounded-down to the nearest 16) bytes of 128-bit-aligned data via SIMD instruction
inline void Copy16(ptrc source, ptrc dest, cui64 byteCount) {
   cui64 j = byteCount >> 4;
   for(ui64 i = 0; i < j; i++) ((ui128ptr)dest)[i] = _mm_load_si128(&((ui128ptr)source)[i]);
}

// Copy byteCount (rounded-down to the nearest 16) bytes of 128-bit-aligned data via SIMD instruction
inline void Copy16(vptrc source, vptrc dest, cui64 byteCount) {
   cui64 j = byteCount >> 4;
   for(ui64 i = 0; i < j; i++) ((ui128ptr)dest)[i] = _mm_load_si128(&((ui128ptr)source)[i]);
}

// Copy byteCount (rounded-down to the nearest 32) bytes of 256-bit-aligned data via SIMD instruction
inline void Copy32(ptrc source, ptrc dest, cui64 byteCount) {
#ifdef __AVX__
   cui64 j = byteCount >> 5;
   for(ui64 i = 0; i < j; i++) ((ui256ptr)dest)[i] = _mm256_load_si256(&((ui256ptr)source)[i]);
#else
   cui64 j = byteCount >> 4;
   for(ui64 i = 0; i < j; i++) ((ui128ptr)dest)[i] = _mm_load_si128(&((ui128ptr)source)[i]);
#endif
}

// Copy byteCount (rounded-up to the nearest 32) bytes of 256-bit-aligned data via SIMD instruction
inline void Copy32(vptrc source, vptrc dest, cui64 byteCount) {
#ifdef __AVX__
   cui64 j = byteCount >> 5;
   for(ui64 i = 0; i < j; i++) ((ui256ptr)dest)[i] = _mm256_load_si256(&((ui256ptr)source)[i]);
#else
   cui64 j = byteCount >> 4;
   for(ui64 i = 0; i < j; i++) ((ui128ptr)dest)[i] = _mm_load_si128(&((ui128ptr)source)[i]);
#endif
}

// Copy byteCount (rounded-down to the nearest 64) bytes of 512-bit-aligned data via SIMD instruction
inline void Copy64(ptrc source, ptrc dest, cui64 byteCount) {
#ifdef __AVX512__
   cui64 j = byteCount >> 6;
   for(ui64 i = 0; i < j; i++) ((ui512ptr)dest)[i] = _mm512_load_epi32(&((ui512ptr)source)[i]);
#else
#ifdef __AVX__
   cui64 j = byteCount >> 5;
   for(ui64 i = 0; i < j; i++) ((ui256ptr)dest)[i] = _mm256_load_si256(&((ui256ptr)source)[i]);
#else
   cui64 j = byteCount >> 4;
   for(ui64 i = 0; i < j; i++) ((ui128ptr)dest)[i] = _mm_load_si128(&((ui128ptr)source)[i]);
#endif
#endif
}

// Copy byteCount (rounded-up to the nearest 64) bytes of 512-bit-aligned data via SIMD instruction
inline void Copy64(vptrc source, vptrc dest, cui64 byteCount) {
#ifdef __AVX512__
   cui64 j = ui64((byteCount + 63) >> 6);
   for(ui64 i = 0; i < j; i++) ((ui512ptr)dest)[i] = _mm512_load_epi32(&((ui512ptr)source)[i]);
#else
#ifdef __AVX__
   cui64 j = ui64((byteCount + 31) >> 5);
   for(ui64 i = 0; i < j; i++) ((ui256ptr)dest)[i] = _mm256_load_si256(&((ui256ptr)source)[i]);
#else
   cui64 j = ui64((byteCount + 15) >> 4);
   for(ui64 i = 0; i < j; i++) ((ui128ptr)dest)[i] = _mm_load_si128(&((ui128ptr)source)[i]);
#endif
#endif
}

// Interlock copy byteCount (rounded-up to nearest 8) bytes
inline void LockedCopy(ptrc source, vptrc dest, csi32 byteCount) {
   csi32 j = (byteCount + 7) >> 3;
   for(si32 i = 0; i < j; i++) _InterlockedExchange64(&((vsi64ptr)dest)[i], ((si64ptr)source)[i]);
}

// Interlock copy byteCount (rounded-up to nearest 8) bytes
inline void LockedCopy(vptrc source, vptrc dest, csi32 byteCount) {
   csi32 j = (byteCount + 7) >> 3;
   for(si32 i = 0; i < j; i++) _InterlockedExchange64(&((vsi64ptr)dest)[i], ((si64ptr)source)[i]);
}

// Interlock swap byteCount (rounded-up to nearest 8) bytes
inline void LockedSwap(ptrc source1, vptrc source2, csi32 byteCount) {
   csi32 j = (byteCount + 7) >> 3;
   for(si32 i = 0; i < j; i++) ((vsi64ptr)source1)[i] = _InterlockedExchange64(&((vsi64ptr)source2)[i], ((si64ptr)source1)[i]);
}

// Interlock swap byteCount (rounded-up to nearest 8) bytes
inline void LockedSwap(vptrc source1, vptrc source2, csi32 byteCount) {
   csi32 j = (byteCount + 7) >> 3;
   for(si32 i = 0; i < j; i++) ((vsi64ptr)source1)[i] = _InterlockedExchange64(&((vsi64ptr)source2)[i], ((si64ptr)source1)[i]);
}

// Interlock move byteCount (rounded-up to nearest 8) bytes and set source to zero
inline void LockedMoveAndClear(vptrc source, ptrc dest, csi32 byteCount) {
   csi32 j = (byteCount + 7) >> 3;
   for(si32 i = 0; i < j; i++) ((vsi64ptr)dest)[i] = _InterlockedExchange64(&((vsi64ptr)source)[i], 0);
}

// Interlock move byteCount (rounded-up to nearest 8) bytes and set source to zero
inline void LockedMoveAndClear(vptrc source, vptrc dest, csi32 byteCount) {
   csi32 j = (byteCount + 7) >> 3;
   for(si32 i = 0; i < j; i++)
      ((vsi64ptr)dest)[i] = _InterlockedExchange64(&((vsi64ptr)source)[i], 0);
}

#undef bitPatternx8
