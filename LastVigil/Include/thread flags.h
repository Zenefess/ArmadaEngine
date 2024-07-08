/************************************************************
 * File: thread flags.h                 Created: 2008/10/20 *
 *                                Last modified: 2024/07/03 *
 *                                                          *
 * Desc: Defines                                            *
 *                                                          *
 *  Copyright (c) David William Bull. All rights reserved.  *
 ************************************************************/
#pragma once

 /*   'Thread active' flags

    bit 00-01: Main thread
        02-06: Video renderer thread
        07-11: Audio renderer thread
        12-16: Input thread
        17-19: World generation thread
        18-20: World generation sub-thread
        21-23: AI thread
        24-25: 1st AI sub-thread
        ...
        57-58: 17th AI sub-thread
        59-63: Developer thread
 */
constexpr cui64  MAIN_THREAD_ALIVE = 0x00000000000000001u;
constexpr cui64  MAIN_THREAD_DIED  = 0x00000000000000002u;
constexpr cui64  MAIN_THREAD       = 0x00000000000000003u;
constexpr cui64  MAIN_THREAD_OVER  = 0x08000000000000000u;
constexpr cui64 VIDEO_THREAD_ALIVE = 0x00000000000000004u;
constexpr cui64 VIDEO_THREAD_DIED  = 0x00000000000000008u;
constexpr cui64 VIDEO_THREAD_RESET = 0x00000000000000010u;
constexpr cui64 VIDEO_THREAD_DONE  = 0x00000000000000020u;
constexpr cui64 VIDEO_THREAD_ERROR = 0x00000000000000040u;
constexpr cui64 VIDEO_THREAD       = 0x0000000000000007Cu;
constexpr cui64 AUDIO_THREAD_ALIVE = 0x00000000000000080u;
constexpr cui64 AUDIO_THREAD_DIED  = 0x00000000000000100u;
constexpr cui64 AUDIO_THREAD_RESET = 0x00000000000000200u;
constexpr cui64 AUDIO_THREAD_DONE  = 0x00000000000000400u;
constexpr cui64 AUDIO_THREAD_ERROR = 0x00000000000000800u;
constexpr cui64 AUDIO_THREAD       = 0x00000000000000F80u;
constexpr cui64 INPUT_THREAD_ALIVE = 0x00000000000001000u;
constexpr cui64 INPUT_THREAD_DIED  = 0x00000000000002000u;
constexpr cui64 INPUT_THREAD_RESET = 0x00000000000004000u;
constexpr cui64 INPUT_THREAD_DONE  = 0x00000000000008000u;
constexpr cui64 INPUT_THREAD_ERROR = 0x00000000000010000u;
constexpr cui64 INPUT_THREAD       = 0x0000000000001F000u;
constexpr cui64   GEN_THREAD_ALIVE = 0x00000000000020000u;
constexpr cui64   GEN_THREAD_DIED  = 0x00000000000040000u;
constexpr cui64   GEN_THREAD_DONE  = 0x00000000000080000u;
constexpr cui64  GEN2_THREAD_ALIVE = 0x00000000000100000u;
constexpr cui64  GEN2_THREAD_DIED  = 0x00000000000200000u;
constexpr cui64   GEN_THREADS      = 0x000000000003E0000u;
constexpr cui64    AI_THREAD_ALIVE = 0x00000000000400000u;
constexpr cui64    AI_THREAD_DIED  = 0x00000000000800000u;
constexpr cui64    AI_THREAD_DONE  = 0x00000000001000000u;
constexpr cui64    AI_THREAD       = 0x00000000001C00000u;
constexpr cui64 AIS01_THREAD_ALIVE = 0x00000000002000000u;
constexpr cui64 AIS01_THREAD_DEAD  = 0x00000000004000000u;
constexpr cui64 AIS02_THREAD_ALIVE = 0x00000000008000000u;
constexpr cui64 AIS02_THREAD_DEAD  = 0x00000000010000000u;
constexpr cui64 AIS03_THREAD_ALIVE = 0x00000000020000000u;
constexpr cui64 AIS03_THREAD_DEAD  = 0x00000000040000000u;
constexpr cui64 AIS04_THREAD_ALIVE = 0x00000000080000000u;
constexpr cui64 AIS04_THREAD_DEAD  = 0x00000000100000000u;
constexpr cui64 AIS05_THREAD_ALIVE = 0x00000000200000000u;
constexpr cui64 AIS05_THREAD_DEAD  = 0x00000000400000000u;
constexpr cui64 AIS06_THREAD_ALIVE = 0x00000000800000000u;
constexpr cui64 AIS06_THREAD_DEAD  = 0x00000001000000000u;
constexpr cui64 AIS07_THREAD_ALIVE = 0x00000002000000000u;
constexpr cui64 AIS07_THREAD_DEAD  = 0x00000004000000000u;
constexpr cui64 AIS08_THREAD_ALIVE = 0x00000008000000000u;
constexpr cui64 AIS08_THREAD_DEAD  = 0x00000010000000000u;
constexpr cui64 AIS09_THREAD_ALIVE = 0x00000020000000000u;
constexpr cui64 AIS09_THREAD_DEAD  = 0x00000040000000000u;
constexpr cui64 AIS10_THREAD_ALIVE = 0x00000080000000000u;
constexpr cui64 AIS10_THREAD_DEAD  = 0x00000100000000000u;
constexpr cui64 AIS11_THREAD_ALIVE = 0x00000200000000000u;
constexpr cui64 AIS11_THREAD_DEAD  = 0x00000400000000000u;
constexpr cui64 AIS12_THREAD_ALIVE = 0x00000800000000000u;
constexpr cui64 AIS12_THREAD_DEAD  = 0x00001000000000000u;
constexpr cui64 AIS13_THREAD_ALIVE = 0x00002000000000000u;
constexpr cui64 AIS13_THREAD_DEAD  = 0x00004000000000000u;
constexpr cui64 AIS14_THREAD_ALIVE = 0x00008000000000000u;
constexpr cui64 AIS14_THREAD_DEAD  = 0x00010000000000000u;
constexpr cui64 AIS15_THREAD_ALIVE = 0x00020000000000000u;
constexpr cui64 AIS15_THREAD_DEAD  = 0x00040000000000000u;
constexpr cui64 AIS16_THREAD_ALIVE = 0x00080000000000000u;
constexpr cui64 AIS16_THREAD_DEAD  = 0x00100000000000000u;
constexpr cui64 AIS17_THREAD_ALIVE = 0x00200000000000000u;
constexpr cui64 AIS17_THREAD_DEAD  = 0x00400000000000000u;
constexpr cui64   AIS_THREADS      = 0x007FFFFFFFE000000u;
constexpr cui64   DEV_THREAD_ALIVE = 0x00800000000000000u;
constexpr cui64   DEV_THREAD_DIED  = 0x01000000000000000u;
constexpr cui64   DEV_THREAD_DONE  = 0x02000000000000000u;
constexpr cui64   DEV_THREAD_MISC  = 0x04000000000000000u;
constexpr cui64   DEV_THREAD       = 0x07800000000000000u;
constexpr cui64   VAI_THREADS_DONE = 0x00000000000008420u;
constexpr cui64   ALL_THREADS_DONE = 0x02000000001088420u;

constexpr cui64    VS_THREAD_ALIVE = 0x00000000000000001u;
constexpr cui64    VS_THREAD_DEAD  = 0x00000000000000002u;
constexpr cui64    VS_THREAD       = 0x00000000000000003u;
constexpr cui64  VS01_THREAD_RESET = 0x00000000000000004u;
constexpr cui64  VS01_THREAD_DONE  = 0x00000000000000008u;
constexpr cui64  VS02_THREAD_RESET = 0x00000000000000010u;
constexpr cui64  VS02_THREAD_DONE  = 0x00000000000000020u;
constexpr cui64    VS_THREAD_RESET = 0x00000000000000014u;
constexpr cui64    VS_THREAD_DONE  = 0x00000000000000028u;
constexpr cui64    VS_THREADS      = 0x0000000000000003Cu;
