/************************************************************
 * File: Command queue.h                Created: 2022/11/20 *
 *                                Last modified: 2022/11/20 *
 *                                                          *
 * Desc:                                                    *
 *                                                          *
 *  Copyright (c) David William Bull. All rights reserved.  *
 ************************************************************/
#pragma once

#include "typedefs.h"
#include "Data structures.h"

//-- CMDENTRY.cmd combinable commands
// cmd[0] entries
#define CMD_NULL    0x000L
#define CMD_SETUP   0x001L
#define CMD_CREATE  0x002L
#define CMD_LOAD    0x004L
#define CMD_UNLOAD  0x008L
#define CMD_FILL    0x010L
#define CMD_CLEAR   0x020L
#define CMD_UPDATE  0x040L
#define CMD_PRESENT 0x080L
// cmd[1] entries
#define PAR_VID_VER 0x001L // Vertex stage
#define PAR_VID_GEO 0x002L // Geometry stage
#define PAR_VID_PIX 0x004L // Pixel stage
// cmd[2] entries
#define PAR_VID_BUF 0x001L // Buffer
#define PAR_VID_SHA 0x002L // Shader
// cmd[3] entries

//-- CMDENTRY.cmd combinable commands

// Inter-thread command queue
al16 struct COMMAND_ENTRY {   // 24 bytes
   ui8 cmd[4];
   union {
      ui32 p_ui32;
      si32 p_si32;
      ui16 p_ui16[2];
      si16 p_si16[2];
      ui8  p_ui8[4];
      si8  p_si8[4];
   };
   union {
      ptr  p;
      ui64 p_ui64;
      si64 p_si64;
      ui32 p2_ui32[2];
      si32 p2_si32[2];
      ui16 p2_ui16[4];
      si16 p2_si16[4];
      ui8  p2_ui8[8];
      si8  p2_si8[8];
   };
   union {
      ptr  p2;
      ui64 p2_ui64;
      si64 p2_si64;
      ui32 p3_ui32[2];
      si32 p3_si32[2];
      ui16 p3_ui16[4];
      si16 p3_si16[4];
      ui8  p3_ui8[8];
      si8  p3_si8[8];
   };
};

extern inline si32 LocalProcessing(COMMAND_ENTRY *command);

// Inter-thread command manager
al32 struct COMMAND_MANAGER {
   COMMAND_ENTRY queue[64] {};   // 1,024 bytes
   vsi8          nextAvailable = 0, nextToProcess = 0;

   // Add commands in reverse order for bulk processing
   inline si8 AddCommands(COMMAND_ENTRY *commands, ui8 count) {
      si8 siTemp = (nextAvailable + 1) & 0x03F;
      for(si8 i = (count << 1) - 1; i > -1; i -= 2, siTemp++) {
         if(siTemp == nextToProcess) return -1; // Check for sufficient slots
         ((ui64 *)queue)[siTemp + i] = ((ui64 *)commands)[i];
         ((ui64 *)queue)[siTemp + i - 1] = ((ui64 *)commands)[i - 1];
      }
      return (nextAvailable = siTemp);
   }

   inline si8 ProcessCommands() {
      si8 siTemp;
      for(siTemp = nextToProcess; queue[siTemp].cmd[0]; siTemp = (siTemp + 1) & 0x03F) LocalProcessing(&queue[siTemp]);

      return (nextToProcess = siTemp);
   }
};
