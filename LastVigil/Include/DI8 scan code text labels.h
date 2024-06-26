/****************************************************************
 * File: DI8 scan code text labels.h        Created: 2023/07/17 *
 *                                    Last modified: 2024/06/05 *
 *                                                              *
 * Desc:                                                        *
 *                                                              *
 *    Copyright (c) David William Bull. All rights reserved.    *
 ****************************************************************/
#pragma once

#include "typedefs.h"

// Inserts mouse & gamepad POV data into indices unused by DirectInput 8
// Mouse inputs @ 0x080~0x08B (12 inputs)
// Gamepads' 1st POV inputs & 0x0E0~0x0FF (32 inputs)
// Gamepads' 2nd POV inputs & 0x0A6~0x0AD, 0x0B9~0x0C4, 0x0CA, 0x0CC, 0x0CE, 0x0D4~0x0DA, 0x0EE~0x0EF (32 inputs)
al32 cchar inputCodeText[256][16] = {
   "<Not bound>",    // 0x000
   "Escape",
   "1",
   "2",
   "3",
   "4",
   "5",
   "6",
   "7",
   "8",
   "9",
   "0",
   "-",
   "=",
   "Backspace",
   "Tab",
   "Q",              // 0x010
   "W",
   "E",
   "R",
   "T",
   "Y",
   "U",
   "I",
   "O",
   "P",
   "[",
   "]",
   "Return",
   "Left Ctrl",
   "A",
   "S",
   "D",              // 0x020
   "F",
   "G",
   "H",
   "J",
   "K",
   "L",
   ";",
   "'",
   "`",
   "Left Shift",
   "\\",
   "Z",
   "X",
   "C",
   "V",
   "B",              // 0x030
   "N",
   "M",
   ",",
   ".",
   "/",
   "Right Shift",
   "*",
   "Left Alt",
   "Spacebar",
   "Capital",
   "F1",
   "F2",
   "F3",
   "F4",
   "F5",
   "F6",             // 0x040
   "F7",
   "F8",
   "F9",
   "F10",
   "Num Lock",
   "Scroll Lock",
   "Numpad 7",
   "Numpad 8",
   "Numpad 9",
   "Numpad -",
   "Numpad 4",
   "Numpad 5",
   "Numpad 6",
   "Numpad +",
   "Numpad 1",
   "Numpad 2",       // 0x050
   "Numpad 3",
   "Numpad 0",
   "Numpad .",
   "0x054",
   "0x055",
   "<> or \\|",
   "F11",
   "F12",
   "0x059",
   "0x05A",
   "0x05B",
   "0x05C",
   "0x05D",
   "0x05E",
   "0x05F",
   "0x060",          // 0x060
   "0x061",
   "0x062",
   "0x063",
   "F13",
   "F14",
   "F15",
   "F16",
   "F17",
   "F18",
   "F19",
   "F20",
   "F21",
   "F22",
   "F23",
   "F24",
   "Kana",           // 0x070
   "0x071",
   "0x072",
   "/",
   "0x074",
   "0x075",
   "0x076",
   "0x077",
   "0x078",
   "Convert",
   "0x07A",
   "No Convert",
   "0x07C",
   "Yen",
   "Numpad .",
   "0x07F",
   "Mouse button 1", // 0x080
   "Mouse button 2",
   "Mouse button 3",
   "Mouse button 4",
   "Mouse button 5",
   "Mouse button 6",
   "Mouse button 7",
   "Mouse button 8",
   "Mouse wheel \24",
   "Mouse wheel \25",
   "Mouse wheel \26",
   "Mouse wheel \27",
   "0x08C",
   "Numpad =",
   "0x08E",
   "0x08F",
   "Previous Track", // 0x090
   "@",
   ":",
   "_",
   "Kanji",
   "Stop",
   "Ax",
   "<No name>",
   "0x098",
   "Next Track",
   "0x09A",
   "0x09B",
   "Numpad Enter",
   "Right Ctrl",
   "0x09E",
   "0x09F",
   "Mute",            // 0x0A0
   "Calculator",
   "Play/Pause",
   "0x0A3",
   "Stop",
   "0x0A5",
   "Pad 0 POV 1 \24",
   "Pad 0 POV 1 \25",
   "Pad 0 POV 1 \26",
   "Pad 0 POV 1 \27",
   "Pad 1 POV 1 \24",
   "Pad 1 POV 1 \25",
   "Pad 1 POV 1 \26",
   "Pad 1 POV 1 \27",
   "Volume Down",
   "0x0AF",
   "Volume Up",       // 0x0B0
   "0x0B1",
   "Web Home",
   "Numpad ,",
   "0x0B4",
   "Numpad /",
   "0x0B6",
   "Print Screen",
   "Right Alt",
   "Pad 2 POV 1 \24",
   "Pad 2 POV 1 \25",
   "Pad 2 POV 1 \26",
   "Pad 2 POV 1 \27",
   "Pad 3 POV 1 \24",
   "Pad 3 POV 1 \25",
   "Pad 3 POV 1 \26",
   "Pad 3 POV 1 \27", // 0x0C0
   "Pad 4 POV 1 \24",
   "Pad 4 POV 1 \25",
   "Pad 4 POV 1 \26",
   "Pad 4 POV 1 \26",
   "Pause",
   "0x0C6",
   "Home",
   "\24",
   "Page Up",
   "Pad 5 POV 1 \24",
   "\27",
   "Pad 5 POV 1 \25",
   "\26",
   "Pad 5 POV 1 \26",
   "End",
   "\25",             // 0x0D0
   "Page Down",
   "Insert",
   "Delete",
   "Pad 5 POV 1 \27",
   "Pad 6 POV 1 \24",
   "Pad 6 POV 1 \25",
   "Pad 6 POV 1 \26",
   "Pad 6 POV 1 \27",
   "Pad 7 POV 1 \24",
   "Pad 7 POV 1 \25",
   "Left Windows",
   "Right Windows",
   "App Menu",
   "Pad 7 POV 1 \26",
   "Pad 7 POV 1 \27",
   "Pad 0 POV 0 \24", // 0x0E0
   "Pad 0 POV 0 \25",
   "Pad 0 POV 0 \26",
   "Pad 0 POV 0 \27",
   "Pad 1 POV 0 \24",
   "Pad 1 POV 0 \25",
   "Pad 1 POV 0 \26",
   "Pad 1 POV 0 \27",
   "Pad 2 POV 0 \24",
   "Pad 2 POV 0 \25",
   "Pad 2 POV 0 \26",
   "Pad 2 POV 0 \27",
   "Pad 3 POV 0 \24",
   "Pad 3 POV 0 \25",
   "Pad 3 POV 0 \26",
   "Pad 3 POV 0 \27", // 0x0F0
   "Pad 4 POV 0 \24",
   "Pad 4 POV 0 \25",
   "Pad 4 POV 0 \26",
   "Pad 4 POV 0 \26",
   "Pad 5 POV 0 \24",
   "Pad 5 POV 0 \25",
   "Pad 5 POV 0 \26",
   "Pad 5 POV 0 \27",
   "Pad 6 POV 0 \24",
   "Pad 6 POV 0 \25",
   "Pad 6 POV 0 \26",
   "Pad 6 POV 0 \27",
   "Pad 7 POV 0 \24",
   "Pad 7 POV 0 \25",
   "Pad 7 POV 0 \26",
   "Pad 7 POV 0 \27"
};
