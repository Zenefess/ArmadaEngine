/************************************************************
* File: project definitions.h          Created: 2024/06/15 *
*                                Last modified: 2024/06/18 *
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
constexpr auto CFG_MAX_SHADERS = 128;
constexpr auto CFG_MAX_STATES  = 16;

constexpr auto MAX_TEXTURES = 1024;
constexpr auto MAX_SRVs     = 1024;

// Maximum definable light sources
constexpr auto MAX_LIGHTS = 1024;

// Maximum possible unique sound samples
constexpr auto MAX_SOUNDS = 4096;

// GUI maximums
constexpr auto MAX_ATLAS            = 48;
constexpr auto MAX_ALPHABETS        = 48;
constexpr auto MAX_CHARACTERS       = 1024;
constexpr auto MAX_NAME_SIZE        = 64;
constexpr auto MAX_TEXT_BUFFER      = 128 * 1024;
constexpr auto MAX_GUI_SPRITE_LIBS  = 16;
constexpr auto MAX_GUI_LIB_SPRITES  = 256;
constexpr auto MAX_GUI_ENTRIES      = 2048;
constexpr auto MAX_GUI_ELEMENTS     = 1024;
constexpr auto MAX_GUI_FUNCTIONS    = 255;
constexpr auto MAX_INTERFACES       = 32;
constexpr auto MAX_INTERFACE_VERTS  = 256;
constexpr auto MAX_INTERFACE_INPUTS = 256;

// World manager maximums
constexpr auto MAX_CELL_X = 1024;
constexpr auto MAX_CELL_Y = 1024;
constexpr auto MAX_CELL_Z = 1024;
constexpr auto MAX_CELLS  = (MAX_CELL_X * MAX_CELL_Y * MAX_CELL_Z);

constexpr auto MAX_MAPS   = 2097152;
constexpr auto MAX_WORLDS = 512;

constexpr auto MAX_TABLES   = 64;
constexpr auto MAX_ELEMENTS = (MAX_TABLES * 256);

// Entity manager maximums
constexpr auto MAX_OBJECT_GROUPS    = 64;
constexpr auto MAX_ENTITY_GROUPS    = 64;
constexpr auto MAX_OBJECTS          = 4096;
constexpr auto MAX_PARTS            = 32768;
constexpr auto MAX_ENTITIES         = 1048576;
constexpr auto MAX_BONES_PER_ENTITY = 32;
constexpr auto MAX_BONES            = 1048576;
constexpr auto MAX_OPAQUE_SPRITES   = 1048576;
constexpr auto MAX_TRANS_SPRITES    = 1048576;
