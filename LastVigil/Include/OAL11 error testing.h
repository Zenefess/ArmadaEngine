/************************************************************
 * File: OAL11 error testing.h          Created: 2023/02/07 *
 *                                Last modified: 2023/02/07 *
 *                                                          *
 * Desc:                                                    *
 *                                                          *
 *  Copyright (c) David William Bull. All rights reserved.  *
 ************************************************************/
#pragma once

#include "pch.h"
#include "typedefs.h"
#include <stdio.h>

extern void WriteError(cchptr const, cbool);

cchar stOpenDev[]   = "alcOpenDevice";
cchar stCreateCon[] = "alcCreateContext";

inline static void Try(cchptr stEvent, ui32 uiResult) {
   static char stDescription[64];

   if(uiResult & 0x080000000) {
      sprintf(stDescription, "AUDIO:%04X : %s", uiResult & 0x03FFFFFFF, stEvent);
      if(uiResult & 0x040000000)
         WriteError(stDescription, true);
      else
         WriteError(stDescription, false);
   }
}
