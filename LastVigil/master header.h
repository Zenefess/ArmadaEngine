/************************************************************
 * File: master header.h                Created: 2022/10/09 *
 *                                Last modified: 2024/03/30 *
 *                                                          *
 * Desc:                                                    *
 *                                                          *
 *  Copyright (c) David William Bull. All rights reserved.  *
 ************************************************************/
#pragma warning(once:4244)
#pragma warning(once:4267)
#pragma warning(once:4838)

#ifndef PCH_H
   #define PCH_H

   #include "project definitions.h"

   #include "targetver.h"

   #define WIN32_LEAN_AND_MEAN

   #include <windows.h>
   #include <process.h>
   #include <data tracking.h>
   #include <typedefs.h>
   #include <24-bit integers.h>
   #include <vector structures.h>
   #include <complex numbers.h>
   #include <Fixed-point data types.h>
   #include <memory management.h>
   #include <Common functions.h>
   #include <stdlib.h>
   #include <tchar.h>
   #include <thread flags.h>
   #include <Data structures.h>
   #include <File operations.h>
   #include <class_timers.h>
   #include <main thread.h>
   #include <GUI structures.h>
   #include <Map structures.h>
   #include <Entity structures.h>
   #include <Common functions.h>
#endif

#include "resource.h"
