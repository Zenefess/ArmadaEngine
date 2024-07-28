/************************************************************
* File: Data tracking.h                Created: 2024/03/30 *
*                                Last modified: 2024/07/04 *
*                                                          *
* Desc:                                                    *
*                                                          *
*                         Copyright (c) David William Bull *
************************************************************/
#pragma once

#include "typedefs.h"
#include "Shlobj.h"

// Input: Maxmimum memory allocations
al16 struct SYSTEM_DATA {
   ///--- APU read-outs?
   ///--- Network read-outs
   ///--- CPU read-outs
   struct { // Hardware information
      ui64 processorMask;
      ui32 processorCount; // Number of virtual CPU cores
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
   ///--- Storage read-outs
   struct {
      vui64 bytesRead    = 0;
      vui64 bytesWritten = 0;
      vui32 filesOpened  = 0;
      vui32 filesClosed  = 0;
   } storage;

   ui64 _PADDING_;

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
      // More?
   } culling;

   wchptrc folderProgramData = (wchptr)_aligned_malloc(sizeof(wchar) * 1024u, 64u);
   wchptrc folderAppData     = folderProgramData + 512u;
   bool    freeAllAllocations;

   SYSTEM_DATA(cui64 maxMemAllocations, cbool freeAllMemoryOnDeletion) {
      SYSTEM_INFO sysInfo;
      wchptr      stPath;

      mem.location  = (vptrptr)_aligned_malloc(maxMemAllocations << 3u, 32u);
      mem.byteCount = (vui64ptr)_aligned_malloc(maxMemAllocations << 3u, 32u);
      for(ui64 i = 0; i < maxMemAllocations; ++i) {
         mem.location[i]  = 0;
         mem.byteCount[i] = 0;
      }
      mem.maxAllocations = (ui32)maxMemAllocations;

      SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, 0, &stPath);
      wcscpy(folderAppData, stPath);
      CoTaskMemFree(stPath);
      SHGetKnownFolderPath(FOLDERID_ProgramFiles, 0, 0, &stPath);
      wcscpy(folderProgramData, stPath);
      CoTaskMemFree(stPath);

      GetSystemInfo(&sysInfo);
      cpu.processorMask  = sysInfo.dwActiveProcessorMask;
      cpu.processorCount = sysInfo.dwNumberOfProcessors;

      freeAllAllocations = freeAllMemoryOnDeletion;
   }

   ~SYSTEM_DATA(void) {
      // Free all memory still allocated?
      if(freeAllAllocations)
         for(ui32 i = 0; i < mem.allocations; i++)
            if(mem.location[i])
               _aligned_free((ptr)mem.location[i]);
      _aligned_free(folderProgramData);
      _aligned_free((ptr)mem.byteCount);
      _aligned_free(mem.location);
      mem.maxAllocations = 0;
   }
};

extern SYSTEM_DATA sysData;
