/************************************************************
 * File: Input functions.h              Created: 2024/04/24 *
 *                                Last modified: 2024/05/30 *
 *                                                          *
 * Desc:                                                    *
 *                                                          *
 *  Copyright (c) David William Bull. All rights reserved.  *
 ************************************************************/
#pragma once

#include "typedefs.h"

al32 struct INPUT_PROC_DATA { // 32 bytes
   ui256   *inputMask; // Array of input bitmasks for testing
   VEC2Du8 *funcCount; // Array of function counts per on & off input tests
   funcptr *function;  // Array of functions for execution
   union {
      ui16 inputs[3];
      struct {
         ui8 global; // Input bitmask count for global actions
         ui8 ui;     // Input bitmask count for GUI space
         ui8 world;  // Input bitmask count for world space
      } input;
   };
   ui16 RES;
};

extern void ProcessInputs(GLOBALCTRLVARS &);
extern void ProcessInputs(INPUT_PROC_DATA &, GLOBALCTRLVARS &);
