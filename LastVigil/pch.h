/************************************************************
 * File: pch.h                          Created: 2022/10/09 *
 *                                Last modified: 2024/03/30 *
 *                                                          *
 * Desc:                                                    *
 *                                                          *
 *  Copyright (c) David William Bull. All rights reserved.  *
 ************************************************************/
#pragma warning(once:4244)
#pragma warning(once:4267)
#pragma warning(once:4838)

#define DATA_TRACKING

#ifndef PCH_H
   #define PCH_H

   #include "targetver.h"

   #define WIN32_LEAN_AND_MEAN

   #include <windows.h>
   #include <process.h>
   #include <memory management.h>
   #include <stdlib.h>
   #include <tchar.h>
#endif

#include "resource.h"
