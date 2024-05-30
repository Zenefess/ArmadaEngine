/**********************************************************
 * File: Direct3D11 thread.cpp          Created: 16/09/08 *
 *                                Last modified: 16/09/08 *
 *                                                        *
 * Desc:                                                  *
 *                                                        *
 * Copyright (c) David William Bull. All rights reserved. *
 **********************************************************/

#include <directx\d3d9.h>
#include <directx\d3dx9.h>

#define DESKTOP_MODE MAX_DISPLAY_MODES-1

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ | D3DFVF_TEX1)

enum CMDs : ui8 {
	CMD_NULL,
	CMD_LOAD,
	CMD_UNLOAD,
	CMD_SETUP,
	CMD_CLEAR,
	CMD_PRESENT
};

struct al16 CUSTOMVERTEX {
	D3DXVECTOR3 pos;
	//	D3DCOLOR color;
	FLOAT tu, tv;
};

struct al16 LOC2D { float x, y; };

struct al16 TEXCOORD { float u1, v1, u2, v2; };

struct al16 FRAME {
	TEXCOORD           coords;
	LPDIRECT3DTEXTURE9 tex;
};

struct al16 SPRITE {
	FRAME** anim;
	float   width;
	float   height;
	ui32    tes;	// Tesselation
};

// Thread status strings
al8 cwchptr stCreate = L"Creating window";
cwchptr stD3D9c = L"Direct3D 11";
wchar  stFPS[9] = { '?', '?', '?', '?', 32, 'f', 'p', 's', 0 };
extern cwchptr stBusy;
extern cwchptr stError;
extern cwchptr stInit;

extern al16 wchptr    stThrdStat[2][9];	// Text strings for thread status
extern al16 vui64     THREAD_LIFE;			// 'Thread active' flags
extern al8  HINSTANCE hInst;					// Current instance's handle
extern al8  HWND      hWnd;					// Main window's handle

extern al16 wchptr texFiles[128];
extern al16 CMDs   vidQueue[256];

cwchar renderWndClass[] = L"LV_D3D";	// Render window's class name

al16 IDirect3DTexture9* texList[MAX_TEXTURES];
al16 FRAME* frameList[MAX_FRAMES];
al16 SPRITE* spriteList[MAX_SPRITES];
