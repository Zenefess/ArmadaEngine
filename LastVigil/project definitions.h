/************************************************************
* File: project definitions.h          Created: 2024/06/15 *
*                                Last modified: 2024/06/15 *
*                                                          *
* Desc:                                                    *
*                                                          *
*  Copyright (c) David William Bull. All rights reserved.  *
************************************************************/
#pragma once

// Enable tracking of operational data
#define DATA_TRACKING

// Disable custom fixed-point data types
//#define FPDT_NO_CUSTOM

// Maximum entry counts for GPU objects
#define CFG_MAX_SHADERS 128
#define CFG_MAX_STATES  16

#define MAX_TEXTURES 1024
#define MAX_SRVs     1024

// Maximum definable light sources
#define MAX_LIGHTS 1024

// Maximum possible unique sound samples
#define MAX_SOUNDS 4096

// GUI maximums
#define MAX_ATLAS            48
#define MAX_ALPHABETS        48
#define MAX_CHARACTERS       1024
#define MAX_NAME_SIZE        64
#define MAX_TEXT_BUFFER      32768 // 32KB VRAM
#define MAX_GUI_SPRITE_LIBS  16
#define MAX_GUI_LIB_SPRITES  256
#define MAX_GUI_ENTRIES      2048  // 128KB VRAM
#define MAX_GUI_ELEMENTS     1024
#define MAX_INTERFACES       32
#define MAX_INTERFACE_VERTS  256
#define MAX_INTERFACE_INPUTS 256

// World manager maximums
#define MAX_CELL_X 1024
#define MAX_CELL_Y 1024
#define MAX_CELL_Z 1024
#define MAX_CELLS  (MAX_CELL_X * MAX_CELL_Y * MAX_CELL_Z)

#define MAX_MAPS   2097152 // 2 * 1,024 * 1,024
#define MAX_WORLDS 512

#define MAX_TABLES   64
#define MAX_ELEMENTS (MAX_TABLES * 256)

// Entity manager maximums
#define MAX_OBJECT_GROUPS    64
#define MAX_ENTITY_GROUPS    64
#define MAX_OBJECTS          4096
#define MAX_PARTS            32768
#define MAX_ENTITIES         1048576
#define MAX_BONES_PER_ENTITY 32
#define MAX_BONES            1048576
#define MAX_OPAQUE_SPRITES   1048576
#define MAX_TRANS_SPRITES    1048576
