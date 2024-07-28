/************************************************************
 * File: project definitions.h          Created: 2024/06/15 *
 *                                Last modified: 2024/07/13 *
 *                                                          *
 * Desc:                                                    *
 *                                                          *
 *  Copyright (c) David William Bull. All rights reserved.  *
 ************************************************************/
#pragma once

// Enable tracking of operational data
#define DATA_TRACKING

// Disable custom fixed-point data types
#define FPDT_NO_CUSTOM

constexpr auto CFG_MAX_SHADERS = 128u; // Maximum number of shaders
constexpr auto CFG_MAX_STATES  = 16u;  // Maximum number of sampler states

constexpr auto MAX_TEXTURES = 1024u; // Maximum number of textures
constexpr auto MAX_SRVs     = 1024u; // Maximum number of shader resource views

constexpr auto TEX_REG_OS_MISC   = 0u;  // First register of miscellaneous textures
constexpr auto TEX_REG_OS_WORLD  = 16u; // First register of world textures
constexpr auto TEX_REG_OS_GUI    = 80u; // First register of GUI atlas textures
constexpr auto TEX_REG_MAX_MISC  = 15u; // Maximum offset of miscellaneous textures
constexpr auto TEX_REG_MAX_WORLD = 63u; // Maximum offset of world textures
constexpr auto TEX_REG_MAX_GUI   = 47u; // Maximum offset of GUI atlas textures

constexpr auto MAX_LIGHTS = 1024u; // Maximum definable light sources

constexpr auto MAX_SOUNDS        = 4096u; // Maximum number of unique sound samples
constexpr auto MAX_SOUND_BANKS   = 1024u; // Maximum number of sound banks
constexpr auto DEFAULT_SOUND_SET = 8;     // Number of sounds in the default set

// GUI maximums
constexpr auto MAX_ATLAS            = 48u;
constexpr auto MAX_ALPHABETS        = 48u;
constexpr auto MAX_CHARACTERS       = 1024u;
constexpr auto MAX_NAME_SIZE        = 64u;
constexpr auto MAX_TEXT_BUFFER      = 131072u;
constexpr auto MAX_GUI_SPRITE_LIBS  = 16u;
constexpr auto MAX_GUI_LIB_SPRITES  = 256u;
constexpr auto MAX_GUI_ELEMENTS     = 1024u;
constexpr auto MAX_GUI_FUNCTIONS    = 255u;
constexpr auto MAX_GUI_VERTICES     = 2048u;
constexpr auto MAX_INTERFACES       = 32u;
constexpr auto MAX_INTERFACE_VERTS  = 256u;
constexpr auto MAX_INTERFACE_INPUTS = 256u;

// World manager maximums
constexpr auto MAX_CELL_X = 1024u;
constexpr auto MAX_CELL_Y = 1024u;
constexpr auto MAX_CELL_Z = 1024u;
constexpr auto MAX_CELLS  = MAX_CELL_X * MAX_CELL_Y * MAX_CELL_Z;

constexpr auto MAX_MAPS   = 2097152u;
constexpr auto MAX_WORLDS = 512u;

constexpr auto MAX_TABLES   = 64u;
constexpr auto MAX_ELEMENTS = MAX_TABLES * 256u;

// Maximum levels of detail
constexpr auto MAX_MAP_LOD = 3u;
constexpr auto MAX_ENT_LOD = 3u;

// Entity manager maximums
constexpr auto MAX_OBJECT_GROUPS    = 64u;
constexpr auto MAX_ENTITY_GROUPS    = 64u;
constexpr auto MAX_OBJECTS          = 4096u;
constexpr auto MAX_PARTS            = 32768u;
constexpr auto MAX_ENTITIES         = 1048576u;
constexpr auto MAX_BONES_PER_ENTITY = 32u;
constexpr auto MAX_BONES            = 1048576u;
constexpr auto MAX_OPAQUE_SPRITES   = 1048576u;
constexpr auto MAX_TRANS_SPRITES    = 1048576u;

// Generic string library
const char stNone[] = "(none)";

const wchar_t wstNone[] = L"(none)";
