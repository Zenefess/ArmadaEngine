/************************************************************
 * File: Input functions.h              Created: 2024/04/24 *
 *                                Last modified: 2024/07/23 *
 *                                                          *
 * Desc:                                                    *
 *                                                          *
 *  Copyright (c) David William Bull. All rights reserved.  *
 ************************************************************/
#pragma once

#include "typedefs.h"

al32 struct INPUT_PROC_DATA { // 32 bytes
   union {
      ptr      inputMask;    // Array of input bitmasks for testing
      ui64ptr  inputMask64;  // Array of 64-input bitmasks for testing
      ui128ptr inputMask128; // Array of 128-input bitmasks for testing
      ui256ptr inputMask256; // Array of 256-input bitmasks for testing
      ui512ptr inputMask512; // Array of 512-input bitmasks for testing
   };
   VEC2Du8 *funcCount; // Array of function counts per on & off input tests
   funcptr *function;  // Array of functions for execution
   union {
      ui8 inputs[3];
      struct {
         ui8 global; // Input bitmask count for global actions
         ui8 ui;     // Input bitmask count for GUI space
         ui8 world;  // Input bitmask count for world space
      } input;
   };
};

void ProcessInputs(GLOBALCTRLVARS &ctrlVars);
void ProcessInputs(INPUT_PROC_DATA &ipd, GLOBALCTRLVARS &ctrlVars);
