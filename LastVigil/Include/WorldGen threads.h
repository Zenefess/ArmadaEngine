/**********************************************************
 * File: WorldGen threads.h             Created: 08/10/28 *
 *                                Last modified: 08/10/28 *
 *                                                        *
 * Desc:                                                  *
 *                                                        *
 * Copyright (c) David William Bull. All rights reserved. *
 **********************************************************/

#include "typedefs.h"
#include "Data structures.h"
#include "Command queue.h"
//#include "Armada Intelligence\class_entitymanager.h"

 // Thread status strings
extern cwchptr stInit;
extern cwchptr stBusy;
extern cwchptr stError;
       cwchptr stWGen = L"TankEngine";

extern al16 vui64  THREAD_LIFE; // 'Thread active' flags
extern al16 wchptr stThrdStat;  // Text strings for thread status
extern al8  HWND   hWnd;        // Main window's handle
