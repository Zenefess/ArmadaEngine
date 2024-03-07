/************************************************************
 * File: GUI functions.h                Created: 2023/06/13 *
 *                                Last modified: 2023/06/28 *
 *                                                          *
 * Desc: Forward declarations for user interface functions  *
 *       for in-game menus and developer environment.       *
 *                                                          *
 *  Copyright (c) David William Bull. All rights reserved.  *
 ************************************************************/
#pragma once

#include "typedefs.h"
#include "GUI structures.h"

struct UI_PTRS { GUI_ELEMENT &element; GUI_EL_DGS *vertex; };

union GUI_INDICES { cptrc data; struct { si32 vertex; si32 element; }; };

void onHover(cptrc);
void offHover(cptrc);

void __Activate0_0_Default_Button(cptrc);
void __Activate0_1_Default_Button(cptrc);

void __Activate0_0_Default_Toggle(cptrc);
void __Activate0_1_Default_Toggle(cptrc);

void __Activate0_0_Default_Scalar(cptrc);
void __Activate0_1_Default_Scalar(cptrc);
void __Activate1_0_Default_Scalar(cptrc);
void __Activate1_1_Default_Scalar(cptrc);

void __Activate0_0_Default_Input(cptrc);
