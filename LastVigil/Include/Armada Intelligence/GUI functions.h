/************************************************************
 * File: GUI functions.h                Created: 2023/06/13 *
 *                                Last modified: 2024/06/24 *
 *                                                          *
 * Desc: Forward declarations for user interface functions  *
 *       for in-game menus and developer environment.       *
 *                                                          *
 *  Copyright (c) David William Bull. All rights reserved.  *
 ************************************************************/
#pragma once

#include "typedefs.h"
#include "GUI structures.h"

cchar stOnHoverDefault[]          = "Generic: Cursor over element";
cchar stOffHoverDefault[]         = "Generic: Cursor not over element";
cchar stActivate00DefaultButton[] = "Button: Input 0 depressed";
cchar stActivate01DefaultButton[] = "Button: Input 0 released";
cchar stActivate00DefaultToggle[] = "Toggle: Input 0 depressed";
cchar stActivate01DefaultToggle[] = "Toggle: Input 0 released";
cchar stActivate00DefaultScalar[] = "Scalar: Input 0 depressed";
cchar stActivate01DefaultScalar[] = "Scalar: Input 0 released";
cchar stActivate10DefaultScalar[] = "Scalar: Input 1 depressed";
cchar stActivate11DefaultScalar[] = "Scalar: Input 1 released";
cchar stPassiveDefaultInput[]     = "Input panel: Automatically process input";
cchar stActivate00DefaultInput[]  = "Input panel: Toggles text editing";
cchar stActivate01DefaultInput[]  = "Input panel: Input 0 released";
cchar stActivate10DefaultInput[]  = "Input panel: Cancels text editing";
cchar stPassiveDefaultArray[]     = "Option panel: Highlight option under pointer";
cchar stActivate00DefaultArray[]  = "Option panel: Select option under pointer";
cchar stActivate01DefaultArray[]  = "Option panel: Input 0 depressed";
cchar stActivate10DefaultArray[]  = "Option panel: Scroll list";

void __OnHover_Default(cptrc);
void __OffHover_Default(cptrc);
void __Activate0_0_Default_Button(cptrc);
void __Activate0_1_Default_Button(cptrc);
void __Activate0_0_Default_Toggle(cptrc);
void __Activate0_1_Default_Toggle(cptrc);
void __Activate0_0_Default_Scalar(cptrc);
void __Activate0_1_Default_Scalar(cptrc);
void __Activate1_0_Default_Scalar(cptrc);
void __Activate1_1_Default_Scalar(cptrc);
void __Passive_Default_Input(cptrc);
void __Activate0_0_Default_Input(cptrc);
void __Activate0_1_Default_Input(cptrc);
void __Activate1_0_Default_Input(cptrc);
void __Passive_Default_Array(cptrc);
void __Activate0_0_Default_Array(cptrc);
void __Activate0_1_Default_Array(cptrc);
void __Activate1_0_Default_Array(cptrc);

enum AE_DEF_FUNC_ENUM : ui16 {
   Null,
   OnHoverDefault, OffHoverDefault,
   Activate00DefaultButton, Activate01DefaultButton,
   Activate00DefaultToggle, Activate01DefaultToggle,
   Activate00DefaultScalar, Activate01DefaultScalar, Activate10DefaultScalar, Activate11DefaultScalar,
   PassiveDefaultInput, Activate00DefaultInput, Activate01DefaultInput, Activate10DefaultInput,
   PassiveDefaultArray, Activate00DefaultArray, Activate01DefaultArray, Activate10DefaultArray
};

typedef const AE_DEF_FUNC_ENUM cAE_DEF_FUNC_ENUM;
