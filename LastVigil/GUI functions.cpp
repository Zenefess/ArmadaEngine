/************************************************************
 * File: GUI functions.cpp              Created: 2023/06/13 *
 *                                Last modified: 2023/07/18 *
 *                                                          *
 * Desc: User interface functions for in-game menus and     *
 *       developer environment.                             *
 *                                                          *
 *  Copyright (c) David William Bull. All rights reserved.  *
 ************************************************************/
#pragma once

#include "pch.h"
#include "resource.h"
#include "typedefs.h"
#include "Vector structures.h"
#include "Data structures.h"
#include "thread flags.h"
#include "main thread.h"
#include "Common functions.h"
#include "class_timers.h"
#include "Direct3D11 thread.h"
#include "Direct3D11 functions.h"

BUFFER textBufferInfo {};	// Buffer information for character input

void __Activate0_0_Default_Button(cptrc indices) {
	const ID64 &data = (ID64 &)indices;

	GUI_ELEMENT &element    = (*(const CLASS_GUI *)ptrLib[4]).element[data.group];
	GUI_EL_DGS (&vertex)[2] = (GUI_EL_DGS (&)[2])(*(const CLASS_GUI *)ptrLib[4]).element_dgs[data.index];

	if(!(element.stateBits & 0x04)) {
		element.bitField ^= 0x0480;
		vertex[0].orient ^= 0x08;
		vertex[1].orient ^= 0x08;
	}
}

void __Activate0_1_Default_Button(cptrc indices) {
	const ID64 &data = (ID64 &)indices;

	GUI_ELEMENT &element    = (*(const CLASS_GUI *)ptrLib[4]).element[data.group];
	GUI_EL_DGS (&vertex)[2] = (GUI_EL_DGS (&)[2])(*(const CLASS_GUI *)ptrLib[4]).element_dgs[data.index];

	if(element.stateBits & 0x04) {
		element.bitField ^= 0x0480;
		vertex[0].orient ^= 0x08;
		vertex[1].orient ^= 0x08;
	}
}

void __Activate0_0_Default_Toggle(cptrc indices) {
	const ID64 &data = (ID64 &)indices;

	GUI_ELEMENT &element    = (*(const CLASS_GUI *)ptrLib[4]).element[data.group];
	GUI_EL_DGS (&vertex)[2] = (GUI_EL_DGS (&)[2])(*(const CLASS_GUI *)ptrLib[4]).element_dgs[data.index];

	if(!(element.stateBits & 0x04)) {
		element.bitField ^= 0x0400;
		vertex[0].orient ^= 0x08;
		vertex[1].orient ^= 0x08;
	}
}

void __Activate0_1_Default_Toggle(cptrc indices) {
	const ID64 &data = (ID64 &)indices;

	GUI_ELEMENT &element = (*(const CLASS_GUI *)ptrLib[4]).element[data.group];

	if(element.stateBits & 0x04)
		element.bitField ^= 0x0480;
}

void __Activate0_0_Default_Scalar(cptrc indices) {
	const ID64 &data = (ID64 &)indices;

	GUI_ELEMENT &element    = (*(const CLASS_GUI *)ptrLib[4]).element[data.group];
	GUI_EL_DGS (&vertex)[2] = (GUI_EL_DGS (&)[2])(*(const CLASS_GUI *)ptrLib[4]).element_dgs[data.index];

	if(!(element.stateBits & 0x04)) {
		element.stateBits |= 0x04;
		element.si32Var[0] = gcv.curCoords.x;
	} else {
		csi32 curCoord   = gcv.curCoords.x;
		cfl32 difference = fl32(curCoord - element.si32Var[0]) / ScrRes.dims[ScrRes.state].dim[0];

		element.si32Var[0] = curCoord;

		vertex[1].viewPos.x += difference * element.viewScale.x * ScrRes.dims[ScrRes.state].aspectI;

		if(vertex[1].viewPos.x < -1.0f)
			vertex[1].viewPos.x = -1.0f;
		else if(vertex[1].viewPos.x > 1.0f)
			vertex[1].viewPos.x = 1.0f;
	}
}

void __Activate0_1_Default_Scalar(cptrc indices) {
	const ID64 &data = (ID64 &)indices;

	GUI_ELEMENT &element = (*(const CLASS_GUI *)ptrLib[4]).element[data.group];

	if(element.stateBits & 0x04)
		element.stateBits &= 0x0FB;
}

void __Activate1_0_Default_Scalar(cptrc indices) {
	const ID64 &data = (ID64 &)indices;

	GUI_ELEMENT &element    = (*(const CLASS_GUI *)ptrLib[4]).element[data.group];
	GUI_EL_DGS (&vertex)[2] = (GUI_EL_DGS (&)[2])(*(const CLASS_GUI *)ptrLib[4]).element_dgs[data.index];

	if(!(element.stateBits & 0x08))
		element.stateBits |= 0x08;
	else {
		cfl32 curCoord = fl32(gcv.curCoords.x) / ScrRes.dims[ScrRes.state].dim[0];

		vertex[1].viewPos.x = (curCoord - element.activePos.x) * element.viewScale.x * ScrRes.dims[ScrRes.state].aspectI;

		if(vertex[1].viewPos.x < -1.0f)
			vertex[1].viewPos.x = -1.0f;
		else if(vertex[1].viewPos.x > 1.0f)
			vertex[1].viewPos.x = 1.0f;
	}
}

void __Activate1_1_Default_Scalar(cptrc indices) {
	const ID64 &data = (ID64 &)indices;

	GUI_ELEMENT &element = (*(const CLASS_GUI *)ptrLib[4]).element[data.group];

	if(element.stateBits & 0x08)
		element.stateBits &= 0x0F7;
}

void __Activate0_0_Default_Input(cptrc indices) {
	const ID64 &data = (ID64 &)indices;

	GUI_ELEMENT &element = (*(const CLASS_GUI *)ptrLib[4]).element[data.group];
	GUI_EL_DGS  &vertex  = (*(const CLASS_GUI *)ptrLib[4]).element_dgs[data.index + 1];

	if(!(element.stateBits & 0x04)) {
		element.stateBits |= 0x04;
		(ui64 &)textBufferInfo.offset = ui64(vertex.textArrayOS) | (ui64(element.charCount) << 32) | (ui64(element.vertexCount[1]) << 52);
	} else {
		element.stateBits &= 0x0FB;
		(ui64 &)textBufferInfo.offset = 0;
	}
}

void onHover(cptrc indices) {
	const ID64 &data = (ID64 &)indices;

	GUI_EL_DGS (&vertex)[2] = (GUI_EL_DGS (&)[2])(*(const CLASS_GUI *)ptrLib[4]).element_dgs[data.index];

	vertex[0].colour.vector[0] = Fix16x4(cVEC4Df{ 0.5f, 0.5f, 0.5f, 1.0f });
	vertex[0].colour.vector[3] = Fix16x4(cVEC4Df{ 0.0f, 0.0f, 0.0f, 1.0f });
}

void offHover(cptrc indices) {
	const ID64 &data = (ID64 &)indices;

	GUI_EL_DGS (&vertex)[2] = (GUI_EL_DGS (&)[2])(*(const CLASS_GUI *)ptrLib[4]).element_dgs[data.index];

	vertex[0].colour.vector[0] = Fix16x4(cVEC4Df{ 1.0f, 1.0f, 1.0f, 1.0f });
	vertex[0].colour.vector[3] = Fix16x4(cVEC4Df{ 1.0f, 1.0f, 1.0f, 1.0f });
}

/*
 *  Editor menus
 */

void GUI_Editor(cptrc argList) {
	const CLASS_GUI &gui = *(const CLASS_GUI *)ptrLib[4];

}

/*
 *  Game menus
 */

void PauseMenu(cptrc argList) {				// 5th
	const CLASS_GUI &gui = *(const CLASS_GUI *)ptrLib[4];
}

void AudioMenu(cptrc argList) {				// 4th
	const CLASS_GUI &gui = *(const CLASS_GUI *)ptrLib[4];
}

void VideoMenu(cptrc argList) {				// 2nd
	const CLASS_GUI &gui = *(const CLASS_GUI *)ptrLib[4];
}

void ControlsMenu(cptrc argList) {			// 1st
	const CLASS_GUI &gui = *(const CLASS_GUI *)ptrLib[4];


}

void GameplayMenu(cptrc argList) {			// 7th
	const CLASS_GUI &gui = *(const CLASS_GUI *)ptrLib[4];
}

void OptionsMenu(cptrc argList) {			// 6th
	const CLASS_GUI &gui = *(const CLASS_GUI *)ptrLib[4];
}

void MainMenu(cptrc argList) {				// 8th
	const CLASS_GUI &gui = *(const CLASS_GUI *)ptrLib[4];
}

void DeveloperInterface(cptrc argList) {	// 3rd
	const CLASS_GUI &gui = *(const CLASS_GUI *)ptrLib[4];
}
