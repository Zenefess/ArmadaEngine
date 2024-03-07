/************************************************************
 * File: Data structures.h              Created: 2022/10/20 *
 *                                Last modified: 2023/06/28 *
 *                                                          *
 * Desc:                                                    *
 *                                                          *
 *  Copyright (c) David William Bull. All rights reserved.  *
 ************************************************************/
#pragma once

#include "typedefs.h"
#include "Vector structures.h"
#include "Shlobj.h"

al4 const union ID32 {
	struct {
		ui16 index;
		ui16 group;
	};
	ui32 id;
};

al8 const union ID64 {
	struct {
		ui32 index;
		ui32 group;
	};
	ui64 id;
};

al16 struct BUFFER {
	union {
		ptr     p;
		chptr   pCH;
		wchptr  pWC;
		ui8ptr  p8;
		ui16ptr pu16;
		ui32ptr pu32;
		ui64ptr pu64;
		fl32ptr pf32;
		fl64ptr pf64;
	};
	ui32 offset;
	ui16 index;
	ui16 count;
};

// Global control variables
al32 struct GLOBALCTRLVARS {	// 128 bytes
	union {	// Position of cursor relative to client window
		si32     curCoord[2];
		VEC2Ds32 curCoords;
	};
	union {
		ui64 bits;	// Bitfield: 60==Numerical input, 61==Text input, 62==No button activity, 63==Update state changes
		ui8  misc[8];
	};
	union {
		union {
			float  axis[12];
			VEC3Df vec3D[4];
			VEC2Df vec2D[6];
			struct {
				VEC4Df xy[2];
				VEC2Df z[2];
			};
		};
		union {
			si32     axis[12];
			VEC3Ds32 vec3D[4];
			VEC2Ds32 vec2D[6];
			struct {
				VEC4Ds32 xy[2];
				VEC2Ds32 z[2];
			};
		} s32;
	};
	union {
		ui256 button, key;
		struct { ui64 buttons0, buttons1, buttons2, buttons3; };
		struct { ui64 keys0, keys1, keys2, keys3; };
		ui64 buttons[4], keys[4];
		ui8  b[32], k[32];
	} imm;
	union {
		ui256 button, key;
		struct { ui64 buttons0, buttons1, buttons2, buttons3; };
		struct { ui64 keys0, keys1, keys2, keys3; };
		ui64 buttons[4], keys[4];
		ui8  b[32], k[32];
	} rel;
};

// Global coordinates
al16 struct GLOBALCOORDS {	// 48 bytes
	union {
		float co[12];
		struct {
			union {	// Position
				VEC3Df pos;
				float	p[3];
			};
			union {	// Velocity
				VEC3Df vel;
				float	v[3];
			};
			union {	// Orientation
				VEC6Df ori;
				float	o[6];
			};
		};
	};
};

struct RESDATA {	// 36 bytes
	union {
		float  dim[2];
		VEC2Df dims;
		struct {
			float w;	// Width
			float h;	// Height
		};
	};
	float aspect;	// Aspect ratio (height / width)
	float aspectI;	// Inverted aspect ratio (width / height)
	float gamma;	// Gamma ramp scalar
	UINT  fmtBB;	// Back buffer format
	UINT  fmtDB;	// Depth buffer format
	si16  msaa;		// (Multi)sample count
	si16  msaaQ;	// Multisample quality
	si16  buffers;	// Number of back buffers in swap chain
	si16  RES;
};

al16 struct RESOLUTION {	// 112 bytes
	union {
		RESDATA dims[3];
		struct {
			RESDATA window;
			RESDATA borderless;
			RESDATA full;
		};
	};
	VEC2Du16 windowOS;
	VEC2Du16 borderlessOS;
	si8      state;	// 0 == Windowed | 1 == Borderless | 2 = Fullscreen
	ui8      RES[7];
};

al32 struct SYSTEM_DATA {	// ??? bytes
///--- CPU read-outs
///--- GPU read-outs
///--- APU read-outs?
///--- Disk read-outs
///--- Network read-outs
///--- Misc. read-outs
	al8 struct {
		struct {	// GPU frame information
			fl64 rate          = 0.0;
			fl64 time          = 0.0;
			ui32 curDrawCalls  = 0;
			ui32 prevDrawCalls = 0;
		} frame;
		struct {
			ui64 frames    = 0;
			ui64 drawCalls = 0;
		} total;
	} gpu;
	al8 struct {	// Culling information
		struct {
			fl64 time   = 0.0;
			ui32 mod    = 0;
			ui32 vis[7] = {};
		} map;
		struct {
			fl64 time   = 0.0;
			ui32 mod    = 0;
			ui32 vis[7] = {};
		} entity;
		// More?
	} culling;
	al8 struct {
		ui64 allocated   = 0;
		ui32 allocations = 0;
		ui32 RES = 0x0FFFFFFFF;
	} memory;

	wchar folderProgramData[256];
	wchar folderAppData[256];

	ui64 processorMask;
	ui8  processorCount;	// Number of virtual CPU cores

	SYSTEM_DATA(void) {
		wchptr      stPath;
		SYSTEM_INFO sysInfo;

		SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, 0, &stPath);
		wcscpy(folderAppData, stPath);
		CoTaskMemFree(stPath);
		SHGetKnownFolderPath(FOLDERID_ProgramFiles, 0, 0, &stPath);
		wcscpy(folderProgramData, stPath);
		CoTaskMemFree(stPath);

		GetSystemInfo(&sysInfo);
		processorMask  = sysInfo.dwActiveProcessorMask;
		processorCount = (ui8)sysInfo.dwNumberOfProcessors;
	}
};
