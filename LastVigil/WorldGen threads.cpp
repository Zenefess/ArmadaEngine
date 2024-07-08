/************************************************************
 * File: WorldGen threads.cpp           Created: 2022/10/09 *
 *                           Code last modified: 2022/10/09 *
 *                                                          *
 * Desc: Entity creation & processing.                      *
 *                                                          *
 *  Copyright (c) David William Bull. All rights reserved.  *
 ************************************************************/

#include "master header.h"
#include "thread flags.h"
#include "WorldGen threads.h"

extern COMMAND_MANAGER cmd;

void WorldGenThread(ptr argList) {
   al16 ui64 threadLife;
//        si32 siGroup[1], siSprite[1];

//   CLASS_ENTITYMANAGER entities;
//   siGroup[0] = entities.AddGroup(OPAQUE_SPRITES, 1);
//   siSprite[0] = entities.CreateSimpleSpriteO(siGroup[0]);
//   cmd.queue[1].ptr = entities.spriteO[siGroup[0]];
//   cmd.queue[1].p_si16[0] = sizeof(VERTEX1);
//   cmd.queue[1].p_si16[1] = 
//   cmd.queue[1].cmd[2] = PAR_VID_BUF;
//   cmd.queue[1].cmd[1] = PAR_VID_VER;
//   cmd.queue[1].cmd[0] = CMD_UPDATE;
//   cmd.queue[0].ptr = entities.spriteO[siGroup[0]];
//   cmd.queue[0].p_si16[0] = sizeof(VERTEX1);
//   cmd.queue[0].p_si16[1] = 
//   cmd.queue[0].cmd[2] = PAR_VID_BUF;
//   cmd.queue[0].cmd[1] = PAR_VID_VER;
//   cmd.queue[0].cmd[0] = CMD_CREATE;

   // Primary processing loop
   do {
      threadLife = THREAD_LIFE & GEN_THREADS;

      Sleep(1);
   } while (threadLife & GEN_THREAD_ALIVE);

   THREAD_LIFE |= GEN_THREAD_DIED;
   //_endthread();
}
