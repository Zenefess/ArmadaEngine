/************************************************************
 * File: memory management.h            Created: 2008/12/08 *
 *                                Last modified: 2024/05/02 *
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
   return salloc32(byteCount, cui256{ .m256i_u64 = { bitPattern.m128i_u64[0], bitPattern.m128i_u64[1], bitPattern.m128i_u64[0], bitPattern.m128i_u64[1]}});
}

// Allocates RAM on 16-byte boundary, then sets the entire array to zero
inline ptrc zalloc16(csize_t byteCount) {
   return salloc16(byteCount, cui128{ .m128i_u64 = { 0x0, 0x0 } });
}

// Allocates RAM on 32-byte boundary, then sets the entire array to zero
inline ptrc zalloc32(csize_t byteCount) {
   return salloc32(byteCount, cui256{ .m256i_u64 = { 0x0, 0x0, 0x0, 0x0 } });
}

inline ptrc salloc16(csize_t byteCount, cui128 bitPattern) {
   ptrc pointer = _aligned_malloc(byteCount, 16);
   if(pointer) {
#ifdef DATA_TRACKING
      sysData.mem.byteCount[sysData.mem.allocations]  = byteCount;
      sysData.mem.location[sysData.mem.allocations++] = pointer;
      sysData.mem.allocated += byteCount;
#endif
      cui64 limit = (byteCount >> 4) - 1;

      ui64 os = 0;
      while(os < limit) ((ui128 *)pointer)[os++] = bitPattern;
      os <<= 4;
      while(os < byteCount) ((ui8 *)pointer)[os++] = bitPattern.m128i_u8[os & 0x0F];
   }
   return pointer;
}

inline ptrc salloc32(csize_t byteCount, cui256 bitPattern) {
   ptrc pointer = _aligned_malloc(byteCount, 32);
   if(pointer) {
#ifdef DATA_TRACKING
      sysData.mem.byteCount[sysData.mem.allocations]  = byteCount;
      sysData.mem.location[sysData.mem.allocations++] = pointer;
      sysData.mem.allocated += byteCount;
#endif
      cui64 limit = byteCount >> 5;

      ui64 os = 0;
      while(os < limit) ((ui256 *)pointer)[os++] = bitPattern;
      os <<= 5;
      while(os < byteCount) ((ui8 *)pointer)[os++] = bitPattern.m256i_u8[os & 0x0F];
   }
   return pointer;
}

#undef bitPatternx8

// Frees a pointer and returns true if successful.
#define mfree1(pointer) mdealloc(pointer)
// Frees a variable number of pointers. Each bit represents ~64 pointers in argument order, and will be true if the relevant pointer is freed.
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

// Frees a variable number of pointers. Each bit represents ~64 pointers in argument order, and will be true if the relevant pointer is freed.
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

#define setmem(addr, numBytes, dwVal) mset(addr, numBytes, dwVal)

inline void mset(ptrc addr, cui32 numBytes, cui32 dwVal) {
   cui32 numDWs = numBytes >> 2;

   ui32 os       = 0;
   ui32 remBytes = numBytes & 3;

	while(os < numDWs) (ui32ptr(addr))[os++] = dwVal;

	for(os = numDWs << 2; os < numBytes; os++, remBytes--)
		(ui8ptr(addr))[os] = ui8(dwVal >> (remBytes << 2));
}

inline void mset(ptrc addr, cui64 numBytes, cui64 qwVal) {
   cui64 numQWs = numBytes >> 3;

   ui64 os       = 0;
   ui64 remBytes = numBytes & 7;

	while(os < numQWs) (ui64ptr(addr))[os++] = qwVal;

	for(os = numQWs << 3; os < numBytes; os++, remBytes--)
		(ui8ptr(addr))[os] = ui8(qwVal >> (remBytes << 3));
}

// Interlocked copy byteCount (rounded-up to nearest 8) bytes
inline void LockedMove(si64ptrc source, vsi64ptrc dest, csi32 byteCount) {
   csi32 j = (byteCount + 7) >> 3;
   for(si32 i = 0; i < j; i++)
      _InterlockedExchange64(&dest[i], source[i]);
}

// Interlocked move byteCount (rounded-up to nearest 8) bytes and set source to zero
inline void LockedMoveAndClear(si64ptrc source, vsi64ptrc dest, csi32 byteCount) {
   csi32 j = (byteCount + 7) >> 3;
   for(si32 i = 0; i < j; i++)
      dest[i] = _InterlockedExchange64(&source[i], 0);
}
