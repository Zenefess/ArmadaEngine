/************************************************************
 * File: memory management.h            Created: 2008/12/08 *
 *                                Last modified: 2025/02/19 *
 *                                                          *
 * Notes: 2024/05/02: Added support for data tracking.      *
 *                                                          *
 *                         Copyright (c) David William Bull *
 ************************************************************/
#pragma once

#pragma intrinsic(_InterlockedExchange64)

#include <windows.h>
#include <corecrt_malloc.h>
#include "typedefs.h"
#include "common functions.h"

#ifdef DATA_TRACKING
#include "data tracking.h"
#endif

#define _MEMORY_MANAGER_

#define malloc1(byteCount)  malloc(byteCount, 1u)
#define malloc2(byteCount)  malloc(byteCount, 2u)
#define malloc4(byteCount)  malloc(byteCount, 4u)
#define malloc8(byteCount)  malloc(byteCount, 8u)
#define malloc16(byteCount) malloc(byteCount, 16u)
#define malloc32(byteCount) malloc(byteCount, 32u)
#define malloc64(byteCount) malloc(byteCount, 64u)

// Declare 1-dimensional array at 16-byte boundary
#define declare1d16(dataType, variableName, dim) \
   dataType *const variableName = (dataType *)malloc(RoundUpToNearest16(sizeof(dataType) * (dim)), 16u)

// Declare 1-dimensional array at 32-byte boundary
#define declare1d32(dataType, variableName, dim) \
   dataType *const variableName = (dataType *)malloc(RoundUpToNearest32(sizeof(dataType) * (dim)), 32u)

// Declare 1-dimensional array at 64-byte boundary
#define declare1d64(dataType, variableName, dim) \
   dataType *const variableName = (dataType *)malloc(RoundUpToNearest64(sizeof(dataType) * (dim)), 64u)

// Declare 2-dimensional array at 16-byte boundary
#define declare2d16(dataType, variableName, dim1, dim2) \
   dataType (*const variableName)[dim2] = (dataType (*)[dim2])malloc(RoundUpToNearest16(sizeof(dataType) * ((dim1) * (dim2))), 16u)

// Declare 2-dimensional array at 32-byte boundary
#define declare2d32(dataType, variableName, dim1, dim2) \
   dataType (*const variableName)[dim2] = (dataType (*)[dim2])malloc(RoundUpToNearest32(sizeof(dataType) * ((dim1) * (dim2))), 32u)

// Declare 2-dimensional array at 64-byte boundary
#define declare2d64(dataType, variableName, dim1, dim2) \
   dataType (*const variableName)[dim2] = (dataType (*)[dim2])malloc(RoundUpToNearest64(sizeof(dataType) * ((dim1) * (dim2))), 64u)

// Declare 1-dimensional array at 16-byte boundary, then zero contents
#define declare1d16z(dataType, variableName, dim) \
   dataType *const variableName = (dataType *)salloc(RoundUpToNearest16(sizeof(dataType) * (dim)), 16u, null128)

// Declare 1-dimensional array at 32-byte boundary, then zero contents
#define declare1d32z(dataType, variableName, dim) \
   dataType *const variableName = (dataType *)salloc(RoundUpToNearest32(sizeof(dataType) * (dim)), 32u, null128)

// Declare 1-dimensional array at 64-byte boundary, then zero contents
#define declare1d64z(dataType, variableName, dim) \
   dataType *const variableName = (dataType *)salloc(RoundUpToNearest64(sizeof(dataType) * (dim)), 64u, null128)

// Declare 2-dimensional array at 16-byte boundary, then zero contents
#define declare2d16z(dataType, variableName, dim1, dim2) \
   dataType (*const variableName)[dim2] = (dataType (*)[dim2])salloc(RoundUpToNearest16(sizeof(dataType) * ((dim1) * (dim2))), 16u, null128)

// Declare 2-dimensional array at 32-byte boundary, then zero contents
#define declare2d32z(dataType, variableName, dim1, dim2) \
   dataType (*const variableName)[dim2] = (dataType (*)[dim2])salloc(RoundUpToNearest32(sizeof(dataType) * ((dim1) * (dim2))), 32u, null128)

// Declare 2-dimensional array at 64-byte boundary, then zero contents
#define declare2d64z(dataType, variableName, dim1, dim2) \
   dataType (*const variableName)[dim2] = (dataType (*)[dim2])salloc(RoundUpToNearest64(sizeof(dataType) * ((dim1) * (dim2))), 64u, null128)

// Declare 1-dimensional array at 16-byte boundary, then set the entire array to a repeating pattern of 8~512 bits
#define declare1d16s(dataType, variableName, dim, bitPattern) \
   dataType *const variableName = (dataType *)salloc(RoundUpToNearest16(sizeof(dataType) * (dim)), 16u, bitPattern)

// Declare 1-dimensional array at 32-byte boundary, then set the entire array to a repeating pattern of 8~512 bits
#define declare1d32s(dataType, variableName, dim, bitPattern) \
   dataType *const variableName = (dataType *)salloc(RoundUpToNearest32(sizeof(dataType) * (dim)), 32u, bitPattern)

// Declare 1-dimensional array at 64-byte boundary, then set the entire array to a repeating pattern of 8~512 bits
#define declare1d64s(dataType, variableName, dim, bitPattern) \
   dataType *const variableName = (dataType *)salloc(RoundUpToNearest64(sizeof(dataType) * (dim)), 64u, bitPattern)

// Allocates RAM at 16-byte boundary, then sets the entire array to a repeating pattern of 8~512 bits
#define salloc16(byteCount, bitPattern) salloc(byteCount, 16u, bitPattern)

// Allocates RAM at 16-byte boundary, then sets the entire array to a repeating pattern of 8~512 bits
#define salloc1d16(dataType, dim, bitPattern) \
   (dataType *)salloc(RoundUpToNearest16(sizeof(dataType) * (dim)), 16u, bitPattern)

// Allocates RAM at 16-byte boundary, then sets the entire array to a repeating pattern of 8~512 bits
#define salloc2d16(dataType, dim1, dim2, bitPattern) \
   (dataType (*)[dim2])salloc(RoundUpToNearest16(sizeof(dataType) * ((dim1) * (dim2))), 16u, bitPattern)

// Allocates RAM at 32-byte boundary, then sets the entire array to a repeating pattern of 8~512 bits
#define salloc32(byteCount, bitPattern) salloc(byteCount, 32u, bitPattern)

// Allocates RAM at 32-byte boundary, then sets the entire array to a repeating pattern of 8~512 bits
#define salloc1d32(dataType, dim, bitPattern) \
   (dataType *)salloc(RoundUpToNearest32(sizeof(dataType) * (dim)), 32u, bitPattern)

// Allocates RAM at 32-byte boundary, then sets the entire array to a repeating pattern of 8~512 bits
#define salloc2d32(dataType, dim1, dim2, bitPattern) \
   (dataType (*)[dim2])salloc(RoundUpToNearest32(sizeof(dataType) * ((dim1) * (dim2))), 32u, bitPattern)

// Allocates RAM at 64-byte boundary, then sets the entire array to a repeating pattern of 8~512 bits
#define salloc64(byteCount, bitPattern) salloc(byteCount, 64u, bitPattern)

// Allocates RAM at 64-byte boundary, then sets the entire array to a repeating pattern of 8~512 bits
#define salloc1d64(dataType, dim, bitPattern) \
   (dataType *)salloc(RoundUpToNearest64(sizeof(dataType) * (dim)), 64u, bitPattern)

// Allocates RAM at 64-byte boundary, then sets the entire array to a repeating pattern of 8~512 bits
#define salloc2d64(dataType, dim1, dim2, bitPattern) \
   (dataType (*)[dim2])salloc(RoundUpToNearest64(sizeof(dataType) * ((dim1) * (dim2))), 64u, bitPattern)

// Allocates RAM at 16-byte boundary, then sets the entire array to zero
#define zalloc16(byteCount) salloc(byteCount, 16u, null128)

// Allocates RAM at 16-byte boundary, then sets the entire array to zero
#define zalloc1d16(dataType, dim) \
   (dataType *)salloc(RoundUpToNearest16(sizeof(dataType) * (dim)), 16u, null128)

// Allocates RAM at 16-byte boundary, then sets the entire array to zero
#define zalloc2d16(dataType, dim1, dim2) \
   (dataType (*)[dim2])salloc(RoundUpToNearest16(sizeof(dataType) * ((dim1) * (dim2))), 16u, null128)

#ifdef __AVX__
// Allocates RAM at 32-byte boundary, then sets the entire array to zero
#define zalloc32(byteCount) salloc(byteCount, 32u, null256)

// Allocates RAM at 32-byte boundary, then sets the entire array to zero
#define zalloc1d32(dataType, dim) \
   (dataType *)salloc(RoundUpToNearest32(sizeof(dataType) * (dim)), 32u, null256)

// Allocates RAM at 32-byte boundary, then sets the entire array to zero
#define zalloc2d32(dataType, dim1, dim2) \
   (dataType (*)[dim2])salloc(RoundUpToNearest32(sizeof(dataType) * ((dim1) * (dim2))), 32u, null256)
#else
// Allocates RAM at 32-byte boundary, then sets the entire array to zero
#define zalloc32(byteCount) salloc(byteCount, 32u, null128)

// Allocates RAM at 32-byte boundary, then sets the entire array to zero
#define zalloc1d32(dataType, dim) \
   (dataType *)salloc(RoundUpToNearest32(sizeof(dataType) * (dim)), 32u, null128)

// Allocates RAM at 32-byte boundary, then sets the entire array to zero
#define zalloc2d32(dataType, dim1, dim2) \
   (dataType (*)[dim2])salloc(RoundUpToNearest32(sizeof(dataType) * ((dim1) * (dim2))), 32u, null128)
#endif

#ifdef __AVX512__
// Allocates RAM at 64-byte boundary, then sets the entire array to zero
#define zalloc64(byteCount) salloc(byteCount, 64u, null512)

// Allocates RAM at 64-byte boundary, then sets the entire array to zero
#define zalloc1d64(dataType, dim) \
   (dataType *)salloc(RoundUpToNearest64(sizeof(dataType) * (dim)), 64u, null512)

// Allocates RAM at 64-byte boundary, then sets the entire array to zero
#define zalloc2d64(dataType, dim1, dim2) \
   (dataType (*)[dim2])salloc(RoundUpToNearest64(sizeof(dataType) * ((dim1) * (dim2))), 64u, null512)
#else
#ifdef __AVX__
// Allocates RAM at 64-byte boundary, then sets the entire array to zero
#define zalloc64(byteCount) salloc(byteCount, 64u, null256)

// Allocates RAM at 64-byte boundary, then sets the entire array to zero
#define zalloc1d64(dataType, dim) \
   (dataType *)salloc(RoundUpToNearest64(sizeof(dataType) * (dim)), 64u, null256)

// Allocates RAM at 64-byte boundary, then sets the entire array to zero
#define zalloc2d64(dataType, dim1, dim2) \
   (dataType (*)[dim2])salloc(RoundUpToNearest64(sizeof(dataType) * ((dim1) * (dim2))), 64u, null256)
#else
// Allocates RAM at 64-byte boundary, then sets the entire array to zero
#define zalloc64(byteCount) salloc(byteCount, 64, null128)

// Allocates RAM at 64-byte boundary, then sets the entire array to zero
#define zalloc1d64(dataType, dim) \
   (dataType *)salloc(RoundUpToNearest64(sizeof(dataType) * (dim)), 64u, null128)

// Allocates RAM at 64-byte boundary, then sets the entire array to zero
#define zalloc2d64(dataType, dim1, dim2) \
   (dataType (*)[dim2])salloc(RoundUpToNearest64(sizeof(dataType) * ((dim1) * (dim2))), 64u, null128)
#endif
#endif

// Allocate RAM at aligned boundary
inline ptrc malloc(csize_t numBytes, csize_t alignment) {
   ptrc pointer = _aligned_malloc(numBytes, alignment);
#ifdef DATA_TRACKING
   if(pointer) {
      sysData.mem.byteCount[sysData.mem.allocations] = numBytes;
      sysData.mem.location[sysData.mem.allocations]  = pointer;
      sysData.mem.allocated += numBytes;
      sysData.mem.allocations++;
   }
#endif
   return pointer;
}

// Allocates RAM at aligned boundary, then sets the entire array to a repeating 8-bit pattern
inline ptrc salloc(csize_t numBytes, csize_t alignment, cui8 bitPattern) {
   ptrc pointer = _aligned_malloc(numBytes, alignment);
   if(pointer) {
#ifdef DATA_TRACKING
      sysData.mem.byteCount[sysData.mem.allocations] = numBytes;
      sysData.mem.location[sysData.mem.allocations]  = pointer;
      sysData.mem.allocated += numBytes;
      sysData.mem.allocations++;
#endif
      cui64 bitPat8  = ui64(bitPattern);
      cui64 bitPat16 = bitPat8  | (bitPat8 << 8u);
      cui64 bitPat32 = bitPat16 | (bitPat16 << 16u);
      cui64 bitPat64 = bitPat32 | (bitPat32 << 32u);
      cui64 limit    = numBytes >> 3;
      ui64  os;

      for(os = 0; os < limit; os++) ((ui64ptr)pointer)[os] = bitPat64;
      for(os <<= 3; os < numBytes; os++) ((ui8ptr)pointer)[os] = ((ui8 (&)[8])bitPat64)[os & 0x07];
}
   return pointer;
}

// Allocates RAM at aligned boundary, then sets the entire array to a repeating 16-bit pattern
inline ptrc salloc(csize_t numBytes, csize_t alignment, cui16 bitPattern) {
   ptrc pointer = _aligned_malloc(numBytes, alignment);
   if(pointer) {
#ifdef DATA_TRACKING
      sysData.mem.byteCount[sysData.mem.allocations] = numBytes;
      sysData.mem.location[sysData.mem.allocations]  = pointer;
      sysData.mem.allocated += numBytes;
      sysData.mem.allocations++;
#endif
      cui64 bitPat16 = ui64(bitPattern);
      cui64 bitPat32 = bitPat16 | (bitPat16 << 16u);
      cui64 bitPat64 = bitPat32 | (bitPat32 << 32u);
      cui64 limit    = numBytes >> 3;
      ui64  os;

      for(os = 0; os < limit; os++) ((ui64ptr)pointer)[os] = bitPat64;
      for(os <<= 3; os < numBytes; os++) ((ui8ptr)pointer)[os] = ((ui8 (&)[8])bitPat64)[os & 0x07];
   }
   return pointer;
}

// Allocates RAM at aligned boundary, then sets the entire array to a repeating 32-bit pattern
inline ptrc salloc(csize_t numBytes, csize_t alignment, cui32 bitPattern) {
   ptrc pointer = _aligned_malloc(numBytes, alignment);
   if(pointer) {
#ifdef DATA_TRACKING
      sysData.mem.byteCount[sysData.mem.allocations] = numBytes;
      sysData.mem.location[sysData.mem.allocations]  = pointer;
      sysData.mem.allocated += numBytes;
      sysData.mem.allocations++;
#endif
      cui64 bitPat32 = ui64(bitPattern);
      cui64 bitPat64 = bitPat32 | (bitPat32 << 32u);
      cui64 limit    = numBytes >> 3;
      ui64  os;

      for(os = 0; os < limit; os++) ((ui64ptr)pointer)[os] = bitPat64;
      for(os <<= 3; os < numBytes; os++) ((ui8ptr)pointer)[os] = ((ui8 (&)[8])bitPat64)[os & 0x07];
   }
   return pointer;
}

// Allocates RAM at aligned boundary, then sets the entire array to a repeating 64-bit pattern
inline ptrc salloc(csize_t numBytes, csize_t alignment, cui64 bitPattern) {
   ptrc pointer = _aligned_malloc(numBytes, alignment);
   if(pointer) {
#ifdef DATA_TRACKING
      sysData.mem.byteCount[sysData.mem.allocations] = numBytes;
      sysData.mem.location[sysData.mem.allocations]  = pointer;
      sysData.mem.allocated += numBytes;
      sysData.mem.allocations++;
#endif
      cui64 limit = numBytes >> 3;
      ui64  os;

      for(os = 0; os < limit; os++) ((ui64ptr)pointer)[os] = bitPattern;
      for(os <<= 3; os < numBytes; os++) ((ui8ptr)pointer)[os] = ((ui8 (&)[8])bitPattern)[os & 0x07];
   }
   return pointer;
}

// Allocates RAM at aligned (to multiple of 16 byte) boundary, then sets the entire array to a repeating 128-bit pattern
inline ptrc salloc(csize_t numBytes, csize_t alignment, cui128 bitPattern) {
   ptrc pointer = _aligned_malloc(numBytes, alignment);
   if(pointer) {
#ifdef DATA_TRACKING
      sysData.mem.byteCount[sysData.mem.allocations] = numBytes;
      sysData.mem.location[sysData.mem.allocations]  = pointer;
      sysData.mem.allocated += numBytes;
      sysData.mem.allocations++;
#endif
      cui64 limit = numBytes >> 4;
      ui64  os;

      for(os = 0; os < limit; os++) ((ui128ptr)pointer)[os] = bitPattern;
      for(os <<= 4; os < numBytes; os++) ((ui8ptr)pointer)[os] = ((ui8 (&)[16])bitPattern)[os & 0x0F];
   }
   return pointer;
}

// Allocates RAM at aligned (to multiple of 32 byte) boundary, then sets the entire array to a repeating 256-bit pattern
inline ptrc salloc(csize_t numBytes, csize_t alignment, cui256 bitPattern) {
   ptrc pointer = _aligned_malloc(numBytes, alignment);
   if(pointer) {
#ifdef DATA_TRACKING
      sysData.mem.byteCount[sysData.mem.allocations] = numBytes;
      sysData.mem.location[sysData.mem.allocations]  = pointer;
      sysData.mem.allocated += numBytes;
      sysData.mem.allocations++;
#endif
      cui64 limit = numBytes >> 5;
      ui64  os;

      for(os = 0; os < limit; os++) ((ui256ptr)pointer)[os] = bitPattern;
      for(os <<= 5; os < numBytes; os++) ((ui8ptr)pointer)[os] = ((ui8 (&)[32])bitPattern)[os & 0x01F];
   }
   return pointer;
}

// Allocates RAM at aligned (to multiple of 64 byte) boundary, then sets the entire array to a repeating 512-bit pattern
inline ptrc salloc(csize_t numBytes, csize_t alignment, cui512 bitPattern) {
   ptrc pointer = _aligned_malloc(numBytes, alignment);
   if(pointer) {
#ifdef DATA_TRACKING
      sysData.mem.byteCount[sysData.mem.allocations] = numBytes;
      sysData.mem.location[sysData.mem.allocations]  = pointer;
      sysData.mem.allocated += numBytes;
      sysData.mem.allocations++;
#endif
      cui64 limit = numBytes >> 6;
      ui64  os;

      for(os = 0; os < limit; os++) ((ui512ptr)pointer)[os] = bitPattern;
      for(os <<= 3; os < (numBytes >> 3); os++) ((ui32ptr)pointer)[os] = ((ui8 (&)[16])bitPattern)[os & 0x0F];
      for(os <<= 6; os < numBytes; os++) ((ui8ptr)pointer)[os] = ((ui8 (&)[64])bitPattern)[os & 0x03F];
   }
   return pointer;
}

inline cui64 mdealloc(ptrc pointer);

// Frees a pointer and returns true if successful.
inline constexpr cui64 mfree1(ptr pointer) {
    if(pointer) {
        mdealloc(pointer);
#if defined(DATA_TRACKING)
        int x = 0;
        for(; sysData.mem.location[x] != pointer && x << sysData.mem.allocations; x++)
        if(sysData.mem.location[x] != pointer) sysData.mem.location[x] = 0;
#endif
        return true;
    }
    return false;
}
//#define mfree1(pointer) mdealloc(pointer)

// Frees a variable number of pointers. Each bit represents each pointer (in argument order), and will be true if the relevant pointer is freed.
#define mfree(pointer, ...) mdealloc_(pointer, __VA_ARGS__, -1ll)

// Frees a pointer and returns true if successful.
inline cui64 mdealloc(ptrc pointer) {
   if(pointer) {
#ifdef DATA_TRACKING
      cui32    allocations = sysData.mem.allocations;
      vptrptrc location    = sysData.mem.location;

      ui32 index = 0;

      // Find entry
      while(allocations < index && pointer != location[index]) index++;

      // Is the pointer invalid?
      if(index >= allocations) return false;

      --sysData.mem.allocations;
      sysData.mem.allocated -= sysData.mem.byteCount[index];
      sysData.mem.location[index]  = 0;
      sysData.mem.byteCount[index] = 0;
      location[index] = 0;
#endif
      _aligned_free(pointer);
      return true;
   }
   return false;
}

// Frees a variable number of pointers. Each bit represents each pointer (in argument order), and will be true if the relevant pointer is freed.
inline cui64 mdealloc_(ptr pointer, ...) {
   va_list val; va_start(val, pointer);
   ui64    retVal = 0, ptrBit = 0x01u;

   for(; (ui64 &)pointer != -1; pointer = va_arg(val, ptrc)) {
      if(pointer) {
#ifdef DATA_TRACKING
         cui32    allocations = sysData.mem.allocations;
         vptrptrc location    = sysData.mem.location;
         ui32     index       = 0;

         // Find entry
         while(allocations < index && pointer != location[index]) index++;
         // Is the pointer invalid?
         if(index >= allocations) continue;

         --sysData.mem.allocations;
         sysData.mem.allocated -= sysData.mem.byteCount[index];
         sysData.mem.location[index]  = 0;
         sysData.mem.byteCount[index] = 0;
         location[index] = 0;
#endif
         _aligned_free(pointer);
         retVal |= ptrBit;
         ptrBit <<= 1;
      }
   }
   va_end(val);
   return retVal;
}

// Set a region of memory to zero
inline void mzero(ptrc addr, cui64 numBytes) {
   ui64 i;
#ifdef __AVX512__
   if(numBytes & 0x02Fu) {
#endif
#ifdef __AVX__
      if(numBytes & 0x01Fu) {
#endif
         if(numBytes & 0x0F) {
            cui64 count = numBytes >> 3;
            for(i = 0; i < count; ++i) ((ui64ptr)addr)[i] = 0ull;
            for(i <<= 3; i < numBytes; ++i) ((ui8ptr)addr)[i] = 0u;
            return;
         }
         cui64 count = numBytes >> 4;
         for(i = 0; i < count; ++i) ((ui128ptr)addr)[i] = null128;
         for(i <<= 2; i < (numBytes >> 2); ++i) ((ui32ptr)addr)[i] = 0u;
         for(i <<= 2; i < numBytes; ++i) ((ui8ptr)addr)[i] = 0u;
         return;
#ifdef __AVX__
      }
      cui64 count = numBytes >> 5;
      for(i = 0; i < count; ++i) ((ui256ptr)addr)[i] = null256;
      for(i <<= 3; i < (numBytes >> 2); ++i) ((ui32ptr)addr)[i] = 0u;
      for(i <<= 2; i < numBytes; ++i) ((ui8ptr)addr)[i] = 0u;
      return;
#endif
#ifdef __AVX512__
   }
   cui64 count = numBytes >> 6;
   for(i = 0; i < count; ++i) ((ui512ptr)addr)[i] = null512;
   for(i <<= 3; i < (numBytes >> 3); ++i) ((ui64ptr)addr)[i] = 0u;
   for(i <<= 3; i < numBytes; ++i) ((ui8ptr)addr)[i] = 0u;
#endif
}

// Set a region of memory to zero
inline void mzero(vptrc addr, cui64 numBytes) {
   ui64 i;
#ifdef __AVX512__
   if(numBytes & 0x02F) {
#endif
#ifdef __AVX__
      if(numBytes & 0x01F) {
#endif
         if(numBytes & 0x0F) {
            cui64 count = numBytes >> 3;
            for(i = 0; i < count; ++i) ((ui64ptr)addr)[i] = 0ull;
            for(i <<= 3; i < numBytes; ++i) ((ui8ptr)addr)[i] = 0u;
            return;
         }
         cui64 count = numBytes >> 4;
         for(i = 0; i < count; ++i) ((ui128ptr)addr)[i] = null128;
         for(i <<= 2; i < (numBytes >> 2); ++i) ((ui32ptr)addr)[i] = 0u;
         for(i <<= 2; i < numBytes; ++i) ((ui8ptr)addr)[i] = 0u;
         return;
#ifdef __AVX__
      }
      cui64 count = numBytes >> 5;
      for(i = 0; i < count; ++i) ((ui256ptr)addr)[i] = null256;
      for(i <<= 3; i < (numBytes >> 2); ++i) ((ui32ptr)addr)[i] = 0u;
      for(i <<= 2; i < numBytes; ++i) ((ui8ptr)addr)[i] = 0u;
      return;
#endif
#ifdef __AVX512__
   }
   cui64 count = numBytes >> 6;
   for(i = 0; i < count; ++i) ((ui512ptr)addr)[i] = null512;
   for(i <<= 3; i < (numBytes >> 3); ++i) ((ui64ptr)addr)[i] = 0u;
   for(i <<= 3; i < numBytes; ++i) ((ui8ptr)addr)[i] = 0u;
#endif
}

// Set a region of memory to zero
inline void mzero(ui128ptrc addr, cui64 numBytes) {
   ui64 i;
   cui64 count = numBytes >> 4;
   for(i = 0; i < count; ++i) ((ui128ptr)addr)[i] = null128;
   for(i <<= 2; i < (numBytes >> 2); ++i) ((ui32ptr)addr)[i] = 0u;
   for(i <<= 2; i < numBytes; ++i) ((ui8ptr)addr)[i] = 0u;
   return;
}

// Set a region of memory to zero
inline void mzero(ui256ptrc addr, cui64 numBytes) {
   ui64 i;
#ifdef __AVX__
   cui64 count = numBytes >> 5;
   for(i = 0; i < count; ++i) ((ui256ptr)addr)[i] = null256;
   for(i <<= 3; i < (numBytes >> 2); ++i) ((ui32ptr)addr)[i] = 0u;
   for(i <<= 2; i < numBytes; ++i) ((ui8ptr)addr)[i] = 0u;
#else
   cui64 count = numBytes >> 4;
   for(i = 0; i < count; ++i) ((ui128ptr)addr)[i] = null128;
   for(i <<= 2; i < (numBytes >> 2); ++i) ((ui32ptr)addr)[i] = 0u;
   for(i <<= 2; i < numBytes; ++i) ((ui8ptr)addr)[i] = 0u;
#endif
   return;
}

// Set a region of memory to zero
inline void mzero(ui512ptrc addr, cui64 numBytes) {
   ui64 i;
#ifdef __AVX512__
   cui64 count = numBytes >> 6;
   for(i = 0; i < count; ++i) ((ui512ptr)addr)[i] = null512;
   for(i <<= 3; i < (numBytes >> 3); ++i) ((ui64ptr)addr)[i] = 0u;
   for(i <<= 3; i < numBytes; ++i) ((ui8ptr)addr)[i] = 0u;
#else
#ifdef __AVX__
   cui64 count = numBytes >> 5;
   for(i = 0; i < count; ++i) ((ui256ptr)addr)[i] = null256;
   for(i <<= 3; i < (numBytes >> 2); ++i) ((ui32ptr)addr)[i] = 0u;
   for(i <<= 2; i < numBytes; ++i) ((ui8ptr)addr)[i] = 0u;
#else
   cui64 count = numBytes >> 4;
   for(i = 0; i < count; ++i) ((ui128ptr)addr)[i] = null128;
   for(i <<= 2; i < (numBytes >> 2); ++i) ((ui32ptr)addr)[i] = 0u;
   for(i <<= 2; i < numBytes; ++i) ((ui8ptr)addr)[i] = 0u;
#endif
#endif
   return;
}

// Set a region of memory to a repeating pattern
#define setmem(addr, numBytes, bitPattern) mset(addr, numBytes, bitPattern)

// Set a region of 16-byte-aligned memory to a repeating 128-bit pattern
inline void mset(ptrc addr, cui64 numBytes, cui128 bitPattern) {
   cui64 limit = numBytes >> 4;
   ui64  os;

   for(os = 0; os < limit; os++) ((ui128ptr)addr)[os] = bitPattern;
   for(os <<= 4; os < numBytes; os++) ((ui8ptr)addr)[os] = bitPattern.m128i_u8[os & 0x0F];
}

// Set a region of 32-byte-aligned memory to a repeating 256-bit pattern
inline void mset(ptrc addr, cui64 numBytes, cui256 bitPattern) {
   cui64 limit = numBytes >> 5;
   ui64  os;

   for(os = 0; os < limit; os++) ((ui256ptr)addr)[os] = bitPattern;
   for(os <<= 5; os < numBytes; os++) ((ui8ptr)addr)[os] = bitPattern.m256i_u8[os & 0x01F];
}

// Set a region of 64-byte-aligned memory to a repeating 512-bit pattern
inline void mset(ptrc addr, cui64 numBytes, cui512 bitPattern) {
   cui64 limit = numBytes >> 6;
   ui64  os;

   for(os = 0; os < limit; os++) ((ui512ptr)addr)[os] = bitPattern;
   for(os <<= 3; os < (numBytes >> 3); os++) ((ui64ptr)addr)[os] = bitPattern.m512i_u64[os & 0x0F];
   for(os <<= 3; os < numBytes; os++) ((ui8ptr)addr)[os] = bitPattern.m512i_u8[os & 0x03F];
}

#ifdef __AVX512__
#define _mm_PB64_mm_ cui512 { .m512i_u64 = { bitPattern64, bitPattern64, bitPattern64, bitPattern64, \
                                             bitPattern64, bitPattern64, bitPattern64, bitPattern64 } }
#else
#ifdef __AVX__
#define _mm_PB64_mm_ cui256 { .m256i_u64 = { bitPattern64, bitPattern64, bitPattern64, bitPattern64 } }
#else
#define _mm_PB64_mm_ cui128 { .m128i_u64 = { bitPattern64, bitPattern64 } }
#endif
#endif

// Set a region of 64-byte-aligned memory to a repeating 8-bit pattern
inline void mset(ptrc addr, cui64 numBytes, cui8 bitPattern8) {
   cui64 bitPattern   = ui64(bitPattern8);
   cui64 bitPattern16 = bitPattern   | (bitPattern   << 8u);
   cui64 bitPattern32 = bitPattern16 | (bitPattern16 << 16u);
   cui64 bitPattern64 = bitPattern32 | (bitPattern32 << 32u);
   mset(addr, numBytes, _mm_PB64_mm_);
}

// Set a region of 64-byte-aligned memory to a repeating 16-bit pattern
inline void mset(ptrc addr, cui64 numBytes, cui16 bitPattern16) {
   cui64 bitPattern   = ui64(bitPattern16);
   cui64 bitPattern32 = bitPattern   | (bitPattern   << 16u);
   cui64 bitPattern64 = bitPattern32 | (bitPattern32 << 32u);
   mset(addr, numBytes, _mm_PB64_mm_);
}

// Set a region of 64-byte-aligned memory to a repeating 32-bit pattern
inline void mset(ptrc addr, cui64 numBytes, cui32 bitPattern32) {
   cui64 bitPattern   = ui64(bitPattern32);
   cui64 bitPattern64 = bitPattern | (bitPattern << 32u);
   mset(addr, numBytes, _mm_PB64_mm_);
}

// Set a region of 64-byte-aligned memory to a repeating 64-bit pattern
inline void mset(ptrc addr, cui64 numBytes, cui64 bitPattern64) {
   mset(addr, numBytes, _mm_PB64_mm_);
}

#undef _mm_PB64_mm_

// Copy byteCount bytes of unaligned data
inline void Copy(cptrc source, ptrc dest, cui64 byteCount) {
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
inline void Copy8(cptrc source, ptrc dest, cui64 byteCount) {
   cui64 j = byteCount >> 3;
   for(ui64 i = 0; i < j; i++) ((ui64ptr)dest)[i] = ((ui64ptr)source)[i];
}

// Copy byteCount (rounded-down to the nearest 8) bytes of data
inline void Copy8(vptrc source, vptrc dest, cui64 byteCount) {
   cui64 j = byteCount >> 3;
   for(ui64 i = 0; i < j; i++) ((vui64ptr)dest)[i] = ((vui64ptr)source)[i];
}

// Copy byteCount (rounded-down to the nearest 16) bytes of 128-bit-aligned data via SIMD instruction
inline void Copy16(cptrc source, ptrc dest, cui64 byteCount) {
   cui64 j = byteCount >> 4;
   for(ui64 i = 0; i < j; i++) ((ui128ptr)dest)[i] = _mm_load_si128(&((ui128ptr)source)[i]);
}

// Copy byteCount (rounded-down to the nearest 16) bytes of 128-bit-aligned data via SIMD instruction
inline void Copy16(vptrc source, vptrc dest, cui64 byteCount) {
   cui64 j = byteCount >> 4;
   for(ui64 i = 0; i < j; i++) ((ui128ptr)dest)[i] = _mm_load_si128(&((ui128ptr)source)[i]);
}

// Copy byteCount (rounded-down to the nearest 32) bytes of 256-bit-aligned data via SIMD instruction
inline void Copy32(cptrc source, ptrc dest, cui64 byteCount) {
#ifdef __AVX__
   cui64 j = byteCount >> 5;
   for(ui64 i = 0; i < j; i++) ((ui256ptr)dest)[i] = _mm256_load_si256(&((ui256ptr)source)[i]);
#else
   cui64 j = byteCount >> 4;
   for(ui64 i = 0; i < j; i++) ((ui128ptr)dest)[i] = _mm_load_si128(&((ui128ptr)source)[i]);
#endif
}

// Copy byteCount (rounded-down to the nearest 32) bytes of 256-bit-aligned data via SIMD instruction
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
inline void Copy64(cptrc source, ptrc dest, cui64 byteCount) {
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

// Copy byteCount (rounded-down to the nearest 64) bytes of 512-bit-aligned data via SIMD instruction
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

// Non-temporally copy byteCount (rounded-down to the nearest 16) bytes of 128-bit-aligned data via SIMD instruction
inline void Stream16(cptrc source, ptrc dest, cui64 byteCount) {
#ifdef __AVX__
   cui64 j = byteCount >> 4;
   for(ui64 i = 0; i < j; i++) ((ui128ptr)dest)[i] = _mm_stream_load_si128(&((ui128ptr)source)[i]);
#else
   cui64 j = byteCount >> 4;
   for(ui64 i = 0; i < j; i++) _mm_stream_si128(&((ui128ptr)dest)[i], ((ui128ptr)source)[i]);
#endif
}

// Non-temporally copy byteCount (rounded-down to the nearest 32) bytes of 256-bit-aligned data via SIMD instruction
inline void Stream32(cptrc source, ptrc dest, cui64 byteCount) {
#ifdef __AVX__
   cui64 j = byteCount >> 5;
   for(ui64 i = 0; i < j; i++) ((ui256ptr)dest)[i] = _mm256_stream_load_si256(&((ui256ptr)source)[i]);
#else
   cui64 j = byteCount >> 4;
   for(ui64 i = 0; i < j; i++) ((ui128ptr)dest)[i] = _mm_stream_load_si128(&((ui128ptr)source)[i]);
#endif
}

// Non-temporally copy byteCount (rounded-down to the nearest 64) bytes of 512-bit-aligned data via SIMD instruction
inline void Stream64(cptrc source, ptrc dest, cui64 byteCount) {
#ifdef __AVX512__
   cui64 j = ui64((byteCount + 63) >> 6);
   for(ui64 i = 0; i < j; i++) ((ui512ptr)dest)[i] = _mm512_stream_load_epi32(&((ui512ptr)source)[i]);
#else
#ifdef __AVX__
   cui64 j = ui64((byteCount + 31) >> 5);
   for(ui64 i = 0; i < j; i++)
      ((ui256ptr)dest)[i] = _mm256_stream_load_si256(&((ui256ptr)source)[i]);
#else
   cui64 j = ui64((byteCount + 15) >> 4);
   for(ui64 i = 0; i < j; i++) ((ui128ptr)dest)[i] = _mm_stream_load_si128(&((ui128ptr)source)[i]);
#endif
#endif
}

// (Non-temporally) Copy byteCount (rounded-up to the nearest 16/32/64) bytes of 128/256/512-bit-aligned data via SIMD instruction.
// If either source or dest is unaligned, standard copy is used.
inline void Stream(cptrc source, ptrc dest, cui64 byteCount) {
        if(((ui64 &)source & 0x0Fu)  || ((ui64 &)dest & 0x0Fu))  Copy(source, dest, byteCount);
   else if(((ui64 &)source & 0x010u) || ((ui64 &)dest & 0x010u)) Stream16(source, dest, RoundUpToNearest16(byteCount));
   else if(((ui64 &)source & 0x020u) || ((ui64 &)dest & 0x020u)) Stream32(source, dest, RoundUpToNearest32(byteCount));
   else                                                          Stream64(source, dest, RoundUpToNearest64(byteCount));
}

// Interlock copy byteCount (rounded-down to the nearest 8) bytes of data
inline void LockedCopy(ptrc source, vptrc dest, csi32 byteCount) {
   csi32 j = (byteCount + 7) >> 3;
   for(si32 i = 0; i < j; i++) _InterlockedExchange64(&((vsi64ptr)dest)[i], ((si64ptr)source)[i]);
}

// Interlock copy byteCount (rounded-down to the nearest 8) bytes of data
inline void LockedCopy(vptrc source, vptrc dest, csi32 byteCount) {
   csi32 j = (byteCount + 7) >> 3;
   for(si32 i = 0; i < j; i++) _InterlockedExchange64(&((vsi64ptr)dest)[i], ((si64ptr)source)[i]);
}

// Interlock swap byteCount (rounded-down to the nearest 8) bytes of data
inline void LockedSwap(ptrc source1, vptrc source2, csi32 byteCount) {
   csi32 j = (byteCount + 7) >> 3;
   for(si32 i = 0; i < j; i++) ((vsi64ptr)source1)[i] = _InterlockedExchange64(&((vsi64ptr)source2)[i], ((si64ptr)source1)[i]);
}

// Interlock swap byteCount (rounded-down to the nearest 8) bytes of data
inline void LockedSwap(vptrc source1, vptrc source2, csi32 byteCount) {
   csi32 j = (byteCount + 7) >> 3;
   for(si32 i = 0; i < j; i++) ((vsi64ptr)source1)[i] = _InterlockedExchange64(&((vsi64ptr)source2)[i], ((si64ptr)source1)[i]);
}

// Interlock move byteCount (rounded-down to the nearest 8) bytes of data and zeroes source
inline void LockedMoveAndClear(vptrc source, ptrc dest, csi32 byteCount) {
   csi32 j = (byteCount + 7) >> 3;
   for(si32 i = 0; i < j; i++) ((vsi64ptr)dest)[i] = _InterlockedExchange64(&((vsi64ptr)source)[i], 0);
}

// Interlock move byteCount (rounded-down to the nearest 8) bytes of data and zeroes source
inline void LockedMoveAndClear(vptrc source, vptrc dest, csi32 byteCount) {
   csi32 j = (byteCount + 7) >> 3;
   for(si32 i = 0; i < j; i++)
      ((vsi64ptr)dest)[i] = _InterlockedExchange64(&((vsi64ptr)source)[i], 0);
}
