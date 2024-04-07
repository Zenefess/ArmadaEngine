/************************************************************
 * File: DI8 error testing.h            Created: 2023/02/07 *
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

cchar stDI8Create[]      = "DirectInput8Create";
cchar stCreateDev[]      = "di8->CreteDevice";
cchar stSetKeyFormat[]   = "di8Key->SetDataFormat";
cchar stSetMseFormat[]   = "di8Mse->SetDataFormat";
cchar stSetMseProperty[] = "di8Mse->SetProperty";
cchar stSetKeyCoop[]     = "di8Key->SetCooperationLevel";
cchar stSetMseCoop[]     = "di8Mse->SetCooperationLevel";
cchar stKeyAcquire[]     = "di8Key->Acquire";
cchar stGetKeyDevState[] = "di8Key->GetDeviceState";
cchar stPollMouse[]      = "di8Mse->Poll";

inline static void Try(cchptr stEvent, ui32 uiResult) {
   static char stDescription[64];

   if(uiResult & 0x080000000) {
      sprintf(stDescription, "INPUT:%04X : %s", uiResult & 0x03FFFFFFF, stEvent);
      if(uiResult & 0x040000000)
         WriteError(stDescription, true);
      else
         WriteError(stDescription, false);
   }
}
