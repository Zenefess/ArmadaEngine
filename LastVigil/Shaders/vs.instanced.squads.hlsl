/************************************************************
 * File: vs.instanced.squads.hlsl       Created: 2020/10/27 *
 *                                Last modified: 2020/10/31 *
 *                                                          *
 * Desc:                                                    *
 *                                                          *
 *  Copyright (c) David William Bull. All rights reserved.  *
 ************************************************************/

// index: 0-11==Object index (~4K unique), 12-31==Parent bone index (~1,024K unique)
uint main(in uint index : BLENDINDICES) : BLENDINDICES { return index; }
