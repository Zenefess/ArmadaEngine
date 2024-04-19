/************************************************************
* File: Data tracking.h                Created: 2024/03/30 *
*                                Last modified: 2024/03/30 *
*                                                          *
* Desc:                                                    *
*                                                          *
*                         Copyright (c) David William Bull *
************************************************************/
#pragma once

#include "typedefs.h"
#include "Shlobj.h"

al32 struct SYSTEM_DATA {   // 1,192 bytes
   ///--- APU read-outs?
   ///--- Disk read-outs
   ///--- Network read-outs
   ///--- CPU read-outs
   al8 struct { // Hardware information
   } cpu;
   ///--- GPU read-outs
   al8 struct {
      struct {
         fl64 rate          = 0.0;
         fl64 time          = 0.0;
         ui32 curDrawCalls  = 0;
         ui32 prevDrawCalls = 0;
      } frame;
      struct {
         ui64 frames    = 0;
         ui64 drawCalls = 0;
      } total;
   } gpu;
   ///--- Misc. read-outs
   al8 struct { // Culling information
      struct {
         fl64 time   = 0.0;
         ui32 mod    = 0;
         ui32 vis[7] = {};
      } map;
      struct {
         fl64 time   = 0.0;
         ui32 mod    = 0;
         ui32 vis[7] = {};
      } entity;
      // More?
   } culling;
   ///--- RAM read-outs
   al8 struct {
      aptr    location       = NULL;
      ui64ptr byteCount      = NULL;
      ui64    allocated      = 0;
      ui32    allocations    = 0;
      ui32    maxAllocations = 0;
   } memory;

   wchar folderProgramData[256];
   wchar folderAppData[256];

   ui64 processorMask;
   ui32 processorCount; // Number of virtual CPU cores

   SYSTEM_DATA(cui32 maxMemAllocations) {
      SYSTEM_INFO sysInfo;
      wchptr      stPath;

      memory.location  = (aptr)_aligned_malloc(maxMemAllocations << 3, 32);
      memory.byteCount = (ui64ptr)_aligned_malloc(maxMemAllocations << 3, 32);
      for(ui32 i = 0; i < maxMemAllocations; i++) {
         memory.location[i]  = 0;
         memory.byteCount[i] = 0;
      }
      memory.maxAllocations = maxMemAllocations;

      SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, 0, &stPath);
      wcscpy(folderAppData, stPath);
      CoTaskMemFree(stPath);
      SHGetKnownFolderPath(FOLDERID_ProgramFiles, 0, 0, &stPath);
      wcscpy(folderProgramData, stPath);
      CoTaskMemFree(stPath);

      GetSystemInfo(&sysInfo);
      processorMask  = sysInfo.dwActiveProcessorMask;
      processorCount = sysInfo.dwNumberOfProcessors;
   }

   ~SYSTEM_DATA(void) {
      processorCount = 0;
      processorMask  = 0;

      // Clear file path strings
      for(ui32 i = 0; i < 256; i++)
         folderProgramData[i] = folderAppData[i] = 0;

      // Free all memory still allocated
      for(ui32 i = 0; i < memory.allocations; i++)
         if(memory.location[i])
            _aligned_free(memory.location[i]);

      _aligned_free(memory.byteCount);
      _aligned_free(memory.location);
      memory.maxAllocations = 0;
   }
};
