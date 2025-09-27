/************************************************************
 * File: GUI functions.h                Created: 2023/06/13 *
 *                                Last modified: 2024/07/25 *
 *                                                          *
 * Desc: Forward declarations for user interface functions  *
 *       for in-game menus and developer environment.       *
 *                                                          *
 *  Copyright (c) David William Bull. All rights reserved.  *
 ************************************************************/
#pragma once

#include "typedefs.h"
#include "GUI structures.h"

enum AE_DEF_FUNC_ENUM : ui16 {
   Null,                    OnHoverDefault,          OffHoverDefault,
   Activate00DefaultButton, Activate01DefaultButton, Activate00DefaultToggle, Activate01DefaultToggle,
   Activate00DefaultScalar, Activate01DefaultScalar, Activate10DefaultScalar, Activate11Default,
   PassiveDefaultInput,     Activate00DefaultInput,  Activate01DefaultInput,  Activate10DefaultInput,
   OnHoverDefaultArray,     OffHoverDefaultArray,    Activate00DefaultArray,  Activate01DefaultArray,  Activate10DefaultArray,
   Activate01NewButton,     Activate01SaveButton,    Activate01LoadButton,    Activate01EditorsButton, Activate01OptionsButton,
   Activate01ExitButton,    Activate01UIEditorButton
};
typedef const AE_DEF_FUNC_ENUM cAE_DEF_FUNC_ENUM;

cchar stOnHoverDefault[]          = "Generic: Cursor over element";
cchar stOffHoverDefault[]         = "Generic: Cursor not over element";
cchar stActivate00DefaultButton[] = "Button: Input 0 depressed";
cchar stActivate01DefaultButton[] = "Button: Input 0 released";
cchar stActivate00DefaultToggle[] = "Toggle: Input 0 depressed";
cchar stActivate01DefaultToggle[] = "Toggle: Input 0 released";
cchar stActivate00DefaultScalar[] = "Scalar: Input 0 depressed";
cchar stActivate01DefaultScalar[] = "Scalar: Input 0 released";
cchar stActivate10DefaultScalar[] = "Scalar: Input 1 depressed";
cchar stActivate11Default[]       = "Input 1 released";
cchar stPassiveDefaultInput[]     = "Input panel: Automatically process input";
cchar stActivate00DefaultInput[]  = "Input panel: Toggles text editing";
cchar stActivate01DefaultInput[]  = "Input panel: Input 0 released";
cchar stActivate10DefaultInput[]  = "Input panel: Cancels text editing";
cchar stOnHoverDefaultArray[]     = "Option panel: Highlight entry under pointer";
cchar stOffHoverDefaultArray[]    = "Option panel: Remove highlight";
cchar stActivate00DefaultArray[]  = "Option panel: Select entry under pointer";
cchar stActivate01DefaultArray[]  = "Option panel: Input 0 released";
cchar stActivate10DefaultArray[]  = "Option panel: Scroll list";

cchar stActivate01NewButton[]     = "Activate \"new game\" menu";
cchar stActivate01SaveButton[]    = "Activate \"save\" menu";
cchar stActivate01LoadButton[]    = "Activate \"load\" menu";
cchar stActivate01OptionsButton[] = "Activate \"options\" menu";
cchar stActivate01EditorsButton[] = "Activate \"editors\" menu";
cchar stActivate01ExitButton[]    = "Activate \"exit\" menu";

cchar stActivate01UIEditorButton[] = "Activate \"User interface editor\" menu";

void __OnHover_Default(cptrc);
void __OffHover_Default(cptrc);
void __Activate0_0_Default_Button(cptrc);
void __Activate0_1_Default_Button(cptrc);
void __Activate0_0_Default_Toggle(cptrc);
void __Activate0_1_Default_Toggle(cptrc);
void __Activate0_0_Default_Scalar(cptrc);
void __Activate0_1_Default_Scalar(cptrc);
void __Activate1_0_Default_Scalar(cptrc);
void __Activate1_1_Default(cptrc);
void __Passive_Default_Input(cptrc);
void __Activate0_0_Default_Input(cptrc);
void __Activate0_1_Default_Input(cptrc);
void __Activate1_0_Default_Input(cptrc);
void __OnHover_Default_Array(cptrc);
void __OffHover_Default_Array(cptrc);
void __Activate0_0_Default_Array(cptrc);
void __Activate0_1_Default_Array(cptrc);
void __Activate1_0_Default_Array(cptrc);
void __Activate1_1_Default_Array(cptrc);

void __Activate0_1_New_Button(cptrc);
void __Activate0_1_Save_Button(cptrc);
void __Activate0_1_Load_Button(cptrc);
void __Activate0_1_Options_Button(cptrc);
void __Activate0_1_Editors_Button(cptrc);
void __Activate0_1_Exit_Button(cptrc);

void __Activate0_1_UI_Editor_Button(cptrc);

// Remove after menus are complete
// argList == VEC4Du16(soundBank, alphabet, spriteLib, panelSprite)
void TempMenuGen(cptrc argList);
