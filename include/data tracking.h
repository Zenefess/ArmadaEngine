/*
 * File: data tracking.h
 * Version: v1.0.1
 * Owner: David William Bull
 * Created: 2024-03-30
 * Last Modified: 2026-08-12
 * Description: System data aggregation: CPU topology, memory-allocation tracking, and run-time performance read-outs.
 * To Do: 1) Add support for processor groups (>64 virtual cores) via GetLogicalProcessorInformationEx.
 *        2) Add network (and APU?) read-out sections.
 * Dependencies: typedefs.h, Shlobj.h
 * ISA: SSE4.2
 * Thread-safety: Reentrant
 * Reviewers: Unassigned
 * License: LicenseRef-Proprietary  Copyright: David William Bull
 */
#pragma once

#include "typedefs.h"
#include "Shlobj.h"

// Input: Maxmimum memory allocations
al64 struct SYSTEM_DATA {
   ///--- APU read-outs?
   ///--- Network read-outs
   ///--- CPU read-outs
   struct CPU_INFO_BLOCK { // Hardware information
      ui64 virtCoreMap[2] = {}; // Bitmaps of available virtual cores; 0==Non-SMT, 1==SMT
      ui32 cacheL1[2][2]  = {}; // Sizes of L1 code and data caches per core; [0==Non-SMT, 1==SMT][0==Code, 1==Data]
      ui32 cacheL2[2]     = {}; // Size of L2 cache per core; 0==Non-SMT, 1==SMT
      ui32 cacheL3        = 0;  // Size of L3 cache per core complex
      ui16 coreCount[2]   = {}; // Number of physical cores; 0==Non-SMT, 1==SMT
      ui16 virtCoreCount  = 0;  // Total number of virtual CPU cores
      ui8  SMTCount       = 0;  // Number of virtual cores per physical SMT core
      ui8  instructions   = 0;  // Bit flags: 0x01==SSE2, 0x02==SSE3, 0x04==SSSE3, 0x08==SSE4.1, 0x10==SSE4.2, 0x20==AVX, 0x40==AVX2, 0x80==AVX-512F
      // 4 bytes padding
   } cpu;
   ///--- RAM read-outs
   struct {
      vptrptr  location;
      vui64ptr byteCount;
      vui64    allocated      = 0;
      vui32    allocations    = 0;
      vui32    maxAllocations = 0;
   } mem;
   ///--- Storage read-outs
   struct {
      vui64 bytesRead    = 0;
      vui64 bytesWritten = 0;
      vui32 filesOpened  = 0;
      vui32 filesClosed  = 0;
   } storage;

   wchptrc folderProgramFiles = (wchptr)_aligned_malloc(sizeof(wchar) * 1024u, 64u);
   wchptrc folderAppData      = folderProgramFiles ? folderProgramFiles + 512u : 0;

   ///--- GPU read-outs
   struct {
      struct {
         vfl64 rate          = 0.0;
         vfl64 time          = 0.0;
         vui32 curDrawCalls  = 0;
         vui32 prevDrawCalls = 0;
      } frame;
      struct {
         vui64 frames    = 0;
         vui64 drawCalls = 0;
      } total;
   } gpu;
   ///--- Input read-outs
   struct {
      vfl64 ticRate  = 0.0;
      vfl64 ticTime  = 0.0;
      vui64 ticTotal = 0;
   } input;
   ///--- Misc. read-outs
   struct { // Culling information
      struct {
         vfl64 time   = 0.0;
         vui32 mod    = 0;
         vui32 vis[7] = {};
      } map;
      struct {
         vfl64 time   = 0.0;
         vui32 mod    = 0;
         vui32 vis[7] = {};
      } entity;
      ///--- More?
   } culling;
private:
   bool freeAllAllocations; // 1 byte overflow beyond 272 byte alignment
public:
   SYSTEM_DATA(cui64 maxMemAllocations, cbool freeAllMemoryOnDeletion) {
      typedef SYSTEM_LOGICAL_PROCESSOR_INFORMATION SLPI, * SLPIptr, *const SLPIptrc;
      constexpr cui64 SLPI_SIZE = sizeof(SLPI);

      SLPIptrc sysLPI = (SLPIptr)_aligned_malloc(SLPI_SIZE * 4096u, 64u); // 128KB
      wchptr   stPath = 0;
      DWORD    bytesProc;

      mem.location  = (vptrptr)_aligned_malloc(maxMemAllocations << 3u, 32u);
      mem.byteCount = (vui64ptr)_aligned_malloc(maxMemAllocations << 3u, 32u);
      if(mem.location && mem.byteCount) {
         for(ui64 i = 0; i < maxMemAllocations; ++i) {
            mem.location[i]  = 0;
            mem.byteCount[i] = 0;
         }
         mem.maxAllocations = (ui32)maxMemAllocations;
      } else { // Partial failure: release the survivor; tracking stays disabled (maxAllocations remains 0 per its default)
         _aligned_free((ptr)mem.byteCount);
         _aligned_free(mem.location);
         mem.location  = 0;
         mem.byteCount = 0;
      }

      if(folderProgramFiles) {
         if(SUCCEEDED(SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, 0, &stPath))) wcsncpy_s(folderAppData, 512u, stPath, _TRUNCATE);
         else folderAppData[0] = 0;
         CoTaskMemFree(stPath);
         if(SUCCEEDED(SHGetKnownFolderPath(FOLDERID_ProgramFiles, 0, 0, &stPath))) wcsncpy_s(folderProgramFiles, 512u, stPath, _TRUNCATE);
         else folderProgramFiles[0] = 0;
         CoTaskMemFree(stPath);
      }

      if(!sysLPI || !GetLogicalProcessorInformation(sysLPI, &(bytesProc = SLPI_SIZE * 4096u))) bytesProc = 0; // Alloc/query failure: skip scan
      for(si32 i = 0; (si32&)bytesProc > 0; ++i, bytesProc -= SLPI_SIZE) { ///--- Modify to account for >64 virtual cores !!!
         cui8 coreType = (PopulationCount64(sysLPI[i].ProcessorMask) > 1 ? 1 : 0);

         switch(sysLPI[i].Relationship) {
         case 0: // Processor core
            if(!(cpu.virtCoreMap[coreType] & sysLPI[i].ProcessorMask)) ++cpu.coreCount[coreType];
            cpu.virtCoreMap[coreType] |= sysLPI[i].ProcessorMask; ///--- Replace [coreType] with [coreType][processorGroup] !!!
            if(sysLPI[i].ProcessorCore.Flags) {
               cui8 SMT = (ui8)PopulationCount64(sysLPI[i].ProcessorMask);
               if(!cpu.SMTCount || cpu.SMTCount < SMT) cpu.SMTCount = SMT; // Set (maximum) SMT count per physical core
            }
            break;
         case 1: // Numa node
            break;
         case 2: // Cache
            switch(sysLPI[i].Cache.Level) {
            case 1:
               if(sysLPI[i].Cache.Type == CacheInstruction) // Set (smallest) L1 code size
                  if(!cpu.cacheL1[coreType][0] || cpu.cacheL1[coreType][0] > sysLPI[i].Cache.Size)
                     cpu.cacheL1[coreType][0] = sysLPI[i].Cache.Size;
               if(sysLPI[i].Cache.Type == CacheData) // Set (smallest) L1 code size
                  if(!cpu.cacheL1[coreType][1] || cpu.cacheL1[coreType][1] > sysLPI[i].Cache.Size)
                     cpu.cacheL1[coreType][1] = sysLPI[i].Cache.Size;
               break;
            case 2:
               if(!cpu.cacheL2[coreType] || cpu.cacheL2[coreType] > sysLPI[i].Cache.Size) // Set (smallest) L2 size
                  cpu.cacheL2[coreType] = sysLPI[i].Cache.Size;
               break;
            case 3:
               if(!cpu.cacheL3 || cpu.cacheL3 > sysLPI[i].Cache.Size) // Set (smallest) L3 size
                  cpu.cacheL3 = sysLPI[i].Cache.Size;
               break;
            }
            break;
         case 3: // Processor package
            break;
         }
      }
      _aligned_free(sysLPI);
      cpu.virtCoreCount = PopulationCount64(cpu.virtCoreMap[0] | cpu.virtCoreMap[1]);
      cpu.instructions  =  IsProcessorFeaturePresent(PF_XMMI64_INSTRUCTIONS_AVAILABLE) & 1;
      cpu.instructions |= (IsProcessorFeaturePresent(PF_SSE3_INSTRUCTIONS_AVAILABLE) & 1) << 1;
      cpu.instructions |= (IsProcessorFeaturePresent(PF_SSSE3_INSTRUCTIONS_AVAILABLE) & 1) << 2;
      cpu.instructions |= (IsProcessorFeaturePresent(PF_SSE4_1_INSTRUCTIONS_AVAILABLE) & 1) << 3;
      cpu.instructions |= (IsProcessorFeaturePresent(PF_SSE4_2_INSTRUCTIONS_AVAILABLE) & 1) << 4;
      cpu.instructions |= (IsProcessorFeaturePresent(PF_AVX_INSTRUCTIONS_AVAILABLE) & 1) << 5;
      cpu.instructions |= (IsProcessorFeaturePresent(PF_AVX2_INSTRUCTIONS_AVAILABLE) & 1) << 6;
      cpu.instructions |= (IsProcessorFeaturePresent(PF_AVX512F_INSTRUCTIONS_AVAILABLE) & 1) << 7;

      freeAllAllocations = freeAllMemoryOnDeletion;
   }

   ~SYSTEM_DATA(void) {
      if(freeAllAllocations && mem.location)
         for(ui32 i = 0; i < mem.maxAllocations; ++i)
            if(mem.location[i]) _aligned_free((ptr)mem.location[i]);
      _aligned_free(folderProgramFiles);
      _aligned_free((ptr)mem.byteCount);
      _aligned_free(mem.location);
      mem.maxAllocations = 0;
   }
};

extern SYSTEM_DATA sysData;
