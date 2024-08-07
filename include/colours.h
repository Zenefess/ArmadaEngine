/************************************************************
* File: colours.h                      Created: 2024/04/20 *
*                                Last modified: 2024/06/29 *
*                                                          *
* Desc:                                                    *
*                                                          *
*                        Copyright (c) David William Bull. *
************************************************************/
#pragma once

#include "Vector structures.h"

constexpr cVEC3Df c3_white = { 1.0f, 1.0f, 1.0f };
constexpr cVEC3Df c3_black = { 0.0f, 0.0f, 0.0f };

constexpr cfl32x4 c4_white        = { 1.0f, 1.0f, 1.0f, 1.0f };
constexpr cfl32x4 c4_white_trans  = { 1.0f, 1.0f, 1.0f, 0.0f };
constexpr cfl32x4 c4_black        = { 0.0f, 0.0f, 0.0f, 1.0f };
constexpr cfl32x4 c4_black_trans  = { 0.0f, 0.0f, 0.0f, 0.0f };
constexpr cfl32x4 c4_light_violet = { 0.875f, 0.75f, 1.0f, 1.0f };
