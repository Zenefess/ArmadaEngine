/************************************************************
 * File: Main thread.cpp                Created: 2022/10/09 *
 *                           Code last modified: 2022/10/09 *
 *                                                          *
 * Desc: Root thread; primary game code, and sub-thread     *
 *       management.                                        *
 *                                                          *
 *  Copyright (c) David William Bull. All rights reserved.  *
 ************************************************************/

#include "pch.h"

#include "include\Main thread.h"

void MainThread() {
	al16 ui64 threadLife;

	// Prevent thread from shutting down (after engine reset)
	THREAD_LIFE &= ~MAIN_THREAD_DIED;

	THREAD_LIFE |= MAIN_THREAD_ALIVE;

	// Begin world generation thread
	THREAD_LIFE |= GEN_THREAD_ALIVE;
	_beginthread(WorldGenThread, 0, NULL);
	// Begin video rendering thread
	THREAD_LIFE |= VIDEO_THREAD_ALIVE;
	_beginthread(Direct3D11Thread, 0, NULL);
	// Begin audio rendering thread
	THREAD_LIFE |= AUDIO_THREAD_ALIVE;
	_beginthread(OpenAL1_1Thread, 0, NULL);
	// Begin inupt processing thread
	THREAD_LIFE |= INPUT_THREAD_ALIVE;
	_beginthread(DirectInput8Thread, 0, NULL);

	// Primary application loop
	do {
		threadLife = THREAD_LIFE;

		Sleep(1);
	} while (threadLife & MAIN_THREAD_ALIVE);

	// Request input thread shutdown
	THREAD_LIFE &= ~INPUT_THREAD_ALIVE;
	do {
		threadLife = THREAD_LIFE & INPUT_THREAD;
		Sleep(8);
	} while (!(threadLife & INPUT_THREAD_DIED));

	// Request audio thread shutdown
	THREAD_LIFE &= ~AUDIO_THREAD_ALIVE;
	do {
		threadLife = THREAD_LIFE & AUDIO_THREAD;
		Sleep(8);
	} while (!(threadLife & AUDIO_THREAD_DIED));

	// Request video thread shutdown
	THREAD_LIFE &= ~VIDEO_THREAD_ALIVE;
	do {
		threadLife = THREAD_LIFE & VIDEO_THREAD;
		Sleep(8);
	} while (!(threadLife & VIDEO_THREAD_DIED));

	// Request world generation thread shutdown
	THREAD_LIFE &= ~GEN_THREAD_ALIVE;
	do {
		threadLife = THREAD_LIFE & GEN_THREADS;
		Sleep(8);
	} while (!(threadLife & GEN_THREAD_DIED));

//	_endthread();
}
