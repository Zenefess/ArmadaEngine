/************************************************************
 * File: vs.instanced.squads.hlsl       Created: 2022/11/21 *
 *                                Last modified: 2022/11/21 *
 *                                                          *
 * Desc:                                                    *
 *                                                          *
 *  Copyright (c) David William Bull. All rights reserved.  *
 ************************************************************/

// index: 0-11==Object index (~4K unique), 12-31==Parent bone index (~1,024K unique)
uint main(in const uint index : INDEX) : INDEX { return index; }
