/**************************************************************
 * File: vs.entities.sprites.x32.hlsl     Created: 2023/06/10 *
 *                                      Last mod.: 2023/06/10 *
 *                                                            *
 * Desc:                                                      *
 *                                                            *
 *   Copyright (c) David William Bull. All rights reserved.   *
 **************************************************************/

#include "common.hlsli"

// index: uint4x8==32 bone indices
uint4x8 main(in cuint4x8 index : INDEX) : INDEX { return index; }
