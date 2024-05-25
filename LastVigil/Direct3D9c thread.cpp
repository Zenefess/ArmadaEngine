/************************************************************
 * File: Direct3D9c thread.cpp          Created: 2022/10/09 *
 *                               Code last mod.: 2022/10/09 *
 *                                                          *
 * Desc: Video rendering via Direct3D 9.0c API.             *
 *                                                          *
 *  Copyright (c) David William Bull. All rights reserved.  *
 ************************************************************/

#include "pch.h"

#define MAX_DISPLAY_MODES 256

#define MAX_TEXTURES 128
#define MAX_FRAMES   1024
#define MAX_SPRITES  256

#include "include\Direct3D9c thread.h"

ui32 rtW = 2560, rtH = 1440;	// Early develepment only...
float rtA = float(rtW) / float(rtH);
ui16 texEntry = 0, sprEntry = 0;

al8 HWND                hRndrWnd = NULL;
LPDIRECT3D9             pD3D9 = NULL;
LPDIRECT3DDEVICE9       pD3DDevice = NULL;
LPDIRECT3DVERTEXBUFFER9 pVB[8];

// Forward declaration of functions
LPDIRECT3DTEXTURE9 LoadTexture(wchptr, D3DFORMAT, LPDIRECT3DTEXTURE9*);
void               UnloadTexture(LPDIRECT3DTEXTURE9&);	// To do...
void               CreateSprite(SPRITE*, float);	// To do...

void Direct3D9cThread(ptr argList) {
	al16  const int D3DDM_SIZE = sizeof(D3DDISPLAYMODE);
	ui32  adapterModeCount30, adapterModeCount32, adapterMode = 0;
	ui8   c;
	al16  reg ui64 threadLife;
	ui32  iTotalFrames = 0, iCurrentFrames = 0, iTotalTicks = 0, iCurrentTicks = 0, iFPS = 0;
	al16  D3DDISPLAYMODE d3ddm[MAX_DISPLAY_MODES];	// d3ddm[DESKTOP_MODE] -> Desktop properties
	D3DPRESENT_PARAMETERS d3dpp;
	WNDCLASSEX wcex;

	// Prevent thread from shutting down (after engine reset)
	THREAD_LIFE &= VIDEO_THREAD_DIED ^ -1;

	// Update debug status
	stThrdStat[0][1] = wchptr(stInit);
	stThrdStat[1][1] = wchptr(stD3D9c);
	InvalidateRect(hWnd, NULL, TRUE);

	// Initialise Direct3D 9.0c
	for (c = 8; c && FAILED(pD3D9 = Direct3DCreate9(D3D_SDK_VERSION)); c--)
		Sleep(100);
	if(!c) {
		// Update debug status
		stThrdStat[1][1] = wchptr(L"InitD3D error"); // stError);
		InvalidateRect(hWnd, NULL, TRUE);
		THREAD_LIFE |= VIDEO_THREAD_ERROR;
		_endthread();
	}

	// Update debug status
	stThrdStat[0][1] = wchptr(stCreate);
	stThrdStat[1][1] = NULL;
	InvalidateRect(hWnd, NULL, TRUE);

	// Create render window
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_NOCLOSE;
	wcex.lpfnWndProc = DefWindowProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = GetModuleHandle(NULL);
	wcex.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_LASTVIGIL));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = HBRUSH(COLOR_BACKGROUND + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = renderWndClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_LASTVIGIL));
	RegisterClassEx(&wcex);
	hRndrWnd = CreateWindow(renderWndClass, L"Armada Infinium Direct3D 9.0c render window",
		WS_OVERLAPPED, CW_USEDEFAULT, CW_USEDEFAULT,
		rtW + 6, rtH + 26, NULL, NULL, hInst, NULL);
	if (!hRndrWnd) {
		pD3D9->Release();
		// Update debug status
		stThrdStat[1][1] = wchptr(L"RndrWnd error"); // stError);
		InvalidateRect(hWnd, NULL, TRUE);
		THREAD_LIFE |= VIDEO_THREAD_ERROR;
		_endthread();
	}

Reinitialise_:
	// Update debug status
	stThrdStat[0][1] = wchptr(stInit);
	stThrdStat[1][1] = wchptr(stD3D9c);
	InvalidateRect(hWnd, NULL, TRUE);

	THREAD_LIFE &= VIDEO_THREAD_RESET ^ -1;
	if (FAILED(pD3D9->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm[DESKTOP_MODE]))) {
		pD3D9->Release();
		// Update debug status
		stThrdStat[1][1] = wchptr(L"GDM error"); // stError);
		InvalidateRect(hWnd, NULL, TRUE);
		THREAD_LIFE |= VIDEO_THREAD_ERROR;
		_endthread();
	}
	ZeroMemory(&d3ddm, D3DDM_SIZE * DESKTOP_MODE);
	adapterModeCount32 = pD3D9->GetAdapterModeCount(D3DADAPTER_DEFAULT, D3DFMT_X8R8G8B8);
	for (reg ui32 xx = 0; xx < adapterModeCount32 && adapterMode <= MAX_DISPLAY_MODES; xx++)
		if (HRESULT_CODE(pD3D9->EnumAdapterModes(D3DADAPTER_DEFAULT, D3DFMT_X8R8G8B8, xx, &d3ddm[adapterMode])) == D3D_OK)
			adapterMode++;
	adapterModeCount30 = pD3D9->GetAdapterModeCount(D3DADAPTER_DEFAULT, D3DFMT_A2R10G10B10);
	for (reg ui32 xx = 0; xx < adapterModeCount30 && adapterMode <= MAX_DISPLAY_MODES; xx++)
		if (HRESULT_CODE(pD3D9->EnumAdapterModes(D3DADAPTER_DEFAULT, D3DFMT_A2R10G10B10, xx, &d3ddm[adapterMode])) == D3D_OK)
			adapterMode++;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.BackBufferWidth = rtW;
	d3dpp.BackBufferHeight = rtH;
	d3dpp.BackBufferFormat = d3ddm[0].Format;
	d3dpp.BackBufferCount = 1;
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE; // D3DMULTISAMPLE_4_SAMPLES;
	d3dpp.hDeviceWindow = hRndrWnd;
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24X8;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE; //ONE;
	// Create the Direct3D device
	for (c = 8; c && FAILED(pD3D9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hRndrWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &pD3DDevice)); c--)
		Sleep(100);
	if(!c) {
			pD3D9->Release();
			DestroyWindow(hRndrWnd);
			// Update debug status
			stThrdStat[1][1] = wchptr(L"CreateDev error"); // stError);
			InvalidateRect(hWnd, NULL, TRUE);
			THREAD_LIFE |= VIDEO_THREAD_ERROR;
			_endthread();
	}
	// Display the window
	ShowWindow(hRndrWnd, SW_SHOWNORMAL);

	pD3DDevice->SetSamplerState(0, D3DSAMP_MAXANISOTROPY, 32);
	pD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	pD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
	pD3DDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_ANISOTROPIC);
	pD3DDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD); //D3DSHADE_FLAT);
	pD3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pD3DDevice->SetRenderState(D3DRS_ALPHAREF, (DWORD)0x0000004d);
	pD3DDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_LESS);
	pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_SRCALPHA);
	pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE); //D3DCULL_CW);
	pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
	pD3DDevice->Present(NULL, NULL, NULL, NULL);

	// Update debug status
	stThrdStat[0][1] = wchptr(stD3D9c);
	stThrdStat[1][1] = wchptr(stBusy);
	InvalidateRect(hWnd, NULL, TRUE);

	// Primary rendering loop
	do {
		al16 MSG msg;

		threadLife = THREAD_LIFE;
		if (threadLife & VIDEO_THREAD_RESET) {
			// Shutdown Direct3D9c HAL then goto initialisation.
			pD3DDevice->Release();
			goto Reinitialise_;
		}

		// Process commands
		if (threadLife & (GEN_THREAD_DONE | MAIN_THREAD_OVER)) {
			for (al16 reg ui8 cmdCnt = 0, curCmd = vidQueue[cmdCnt]; curCmd && cmdCnt < 128; cmdCnt++, curCmd = vidQueue[cmdCnt]) {
				switch (curCmd) {
				case CMD_LOAD:
					if (texEntry >= MAX_TEXTURES) break;
					LoadTexture(texFiles[texEntry], D3DFMT_UNKNOWN, &texList[texEntry]);
					texEntry++;
					break;
				case CMD_UNLOAD:
					if (sprEntry--) {
						_aligned_free(spriteList[sprEntry]->anim[0]);
						_aligned_free(spriteList[sprEntry]->anim);
						_aligned_free(spriteList[sprEntry]);
					}
					if (texEntry--) texList[texEntry]->Release();
					break;
				case CMD_SETUP:
					if (sprEntry >= MAX_SPRITES) break;
					if (sprEntry == 0) {
						spriteList[sprEntry] = (SPRITE*)_aligned_malloc(sizeof(SPRITE), 16);
						spriteList[sprEntry]->anim = (FRAME**)_aligned_malloc(8 * PTR_SIZE, 16);
						spriteList[sprEntry]->anim[0] = (FRAME*)_aligned_malloc(PTR_SIZE, 16);
						spriteList[sprEntry]->anim[0]->tex = texList[0];
						spriteList[sprEntry]->anim[0]->coords.u1 = 0.0f;
						spriteList[sprEntry]->anim[0]->coords.v1 = 1.0f;
						spriteList[sprEntry]->anim[0]->coords.u2 = 1.0f;
						spriteList[sprEntry]->anim[0]->coords.v2 = 0.0f;
						spriteList[sprEntry]->width = 64.0f;
						spriteList[sprEntry]->height = 64.0f;
						spriteList[sprEntry]->tes = 2;
						CreateSprite(spriteList[sprEntry], -16.0f);
					}
					if (sprEntry == 1) {
						spriteList[sprEntry] = (SPRITE*)_aligned_malloc(sizeof(SPRITE), 16);
						spriteList[sprEntry]->anim = (FRAME**)_aligned_malloc(8 * PTR_SIZE, 16);
						spriteList[sprEntry]->anim[0] = (FRAME*)_aligned_malloc(PTR_SIZE, 16);
						spriteList[sprEntry]->anim[0]->tex = texList[1];
						spriteList[sprEntry]->anim[0]->coords.u1 = 3.0f / 7.0f;
						spriteList[sprEntry]->anim[0]->coords.v1 = 1.0f;
						spriteList[sprEntry]->anim[0]->coords.u2 = 4.0f / 7.0f;
						spriteList[sprEntry]->anim[0]->coords.v2 = 0.0f;
						spriteList[sprEntry]->width = 1.0f;
						spriteList[sprEntry]->height = 1.0f;
						spriteList[sprEntry]->tes = 2;
						CreateSprite(spriteList[sprEntry], 0.0f);
					}
					sprEntry++;
					break;
				case CMD_CLEAR:
					pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
					break;
				case CMD_PRESENT:
					pD3DDevice->Present(NULL, NULL, NULL, NULL);
					break;
				}
			}
			THREAD_LIFE &= (GEN_THREAD_DONE | MAIN_THREAD_OVER) ^ -1;
		}

		pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x003f3f3f, 1.0f, 0);
		//		pD3DDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0x007f7f7f, 1.0f, 0);

		pD3DDevice->BeginScene();

		D3DXMATRIXA16 matWorld;
		D3DXMatrixIdentity(&matWorld);
		D3DXMatrixRotationZ(&matWorld, sinf(timeGetTime() * 0.002f) * 0.5f);
		pD3DDevice->SetTransform(D3DTS_WORLD, &matWorld);

		D3DXVECTOR3 vEyePt(0.0f, 12.0f, 4.0f);	// Parallel
		D3DXVECTOR3 vLookatPt(0.0f, 0.0f, 4.0f);
		//		D3DXVECTOR3 vEyePt(0.0f, 12.0f, 0.0f);	// Tilted
		//		D3DXVECTOR3 vLookatPt(0.0f, 0.0f, 4.0f);
		D3DXVECTOR3 vUpVec(0.0f, 0.0f, 1.0f);
		D3DXMATRIXA16 matView;
		D3DXMatrixLookAtLH(&matView, &vEyePt, &vLookatPt, &vUpVec);
		pD3DDevice->SetTransform(D3DTS_VIEW, &matView);

		D3DXMATRIXA16 matProj;
		D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4, rtA, 1.0f, 100.0f);
		pD3DDevice->SetTransform(D3DTS_PROJECTION, &matProj);

		pD3DDevice->SetTexture(0, texList[0]);
		pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
//		pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
//		pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
		pD3DDevice->SetStreamSource(0, pVB[0], 0, sizeof(CUSTOMVERTEX));
		pD3DDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
		pD3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, spriteList[0]->tes);

		pD3DDevice->SetTexture(0, texList[1]);
		pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
		pD3DDevice->SetStreamSource(0, pVB[1], 0, sizeof(CUSTOMVERTEX));
		pD3DDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
		pD3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, spriteList[1]->tes);

		pD3DDevice->EndScene();

		pD3DDevice->Present(NULL, NULL, NULL, NULL);
		iCurrentFrames++;
		if ((iCurrentTicks = GetTickCount() - iTotalTicks) >= 1000) {
			iFPS = ui32(float(iCurrentFrames) / float(iCurrentTicks) * 1000.0f);
			if (iFPS > 9999) { iFPS = 9999; stFPS[4] = '*'; }
			else stFPS[4] = 32;
			while (iFPS >= 10000) iFPS -= 10000;
			if (iFPS >= 1000) stFPS[0] = iFPS / 1000 + 48;
			else stFPS[0] = 32;
			while (iFPS >= 1000) iFPS -= 1000;
			if (iFPS >= 100) stFPS[1] = iFPS / 100 + 48;
			else stFPS[1] = 32;
			while (iFPS >= 100) iFPS -= 100;
			if (iFPS >= 10) stFPS[2] = iFPS / 10 + 48;
			else stFPS[2] = 32;
			while (iFPS >= 10) iFPS -= 10;
			stFPS[3] = iFPS + 48;
			iTotalTicks += iCurrentTicks;
			iTotalFrames += iCurrentFrames;
			iCurrentFrames = 0;
			stThrdStat[1][1] = stFPS;
			InvalidateRect(hWnd, NULL, TRUE);
		}

		// Process (render) window's messages
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) DispatchMessage(&msg);
		//		Sleep(0);
	} while (threadLife & VIDEO_THREAD_ALIVE);

	// Free memory allocated to sprites & textures (CMD_UNLOAD)
	for (; sprEntry--; sprEntry) {
//		_aligned_free(spriteList[sprEntry]->anim[0]);
		_aligned_free(spriteList[sprEntry]->anim);
		_aligned_free(spriteList[sprEntry]);
	}
	if (texEntry) texList[--texEntry]->Release();

	// Release Direct3D device and close render window
	pD3DDevice->Release();
	pD3D9->Release();
	DestroyWindow(hRndrWnd);

	// Update debug status
	stThrdStat[0][1] = NULL;
	stThrdStat[1][1] = NULL;
	InvalidateRect(hWnd, NULL, TRUE);
	THREAD_LIFE |= VIDEO_THREAD_DIED;
	_endthread();
}

void CreateSprite(SPRITE* meta, float offset) {
	al16 CUSTOMVERTEX* pVertices;
	al16 reg ui32 rTes = meta->tes;
	al16 reg double xxStep, zzStep, uuStep, vvStep;
	al16 reg double rTesXDelta = double(meta->width) / double(rTes - 1);
	reg double rTesZDelta = double(meta->height) / double(rTes - 1);
	al16 reg double uuDelta = double(meta->anim[0]->coords.u2 - meta->anim[0]->coords.u1) / double(rTes - 1);
	reg double vvDelta = double(meta->anim[0]->coords.v2 - meta->anim[0]->coords.v1) / double(rTes - 1);

	pD3DDevice->CreateVertexBuffer(rTes * rTes * sizeof(CUSTOMVERTEX), 0, D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &pVB[sprEntry], NULL);
	pVB[sprEntry]->Lock(0, 0, (void**)&pVertices, 0);
	zzStep = -meta->height * 0.5;
	vvStep = meta->anim[0]->coords.v1;
	for (al16 DWORD zz = 0; zz < rTes; zz++, zzStep += rTesZDelta, vvStep += vvDelta) {
		xxStep = -meta->width * 0.5;
		uuStep = meta->anim[0]->coords.u1;
		for (al16 DWORD xx = 0; xx < rTes; xx++, xxStep += rTesXDelta, uuStep += uuDelta) {
			pVertices[xx + (zz << 1)].pos = D3DXVECTOR3(xxStep, offset, zzStep);
			pVertices[xx + (zz << 1)].tu = uuStep;
			pVertices[xx + (zz << 1)].tv = vvStep;
		}
	}
	pVB[sprEntry]->Unlock();
}

PDIRECT3DTEXTURE9 LoadTexture(wchar* filename, D3DFORMAT format, LPDIRECT3DTEXTURE9* pD3DTex) {
	if (FAILED(D3DXCreateTextureFromFileEx(pD3DDevice, filename, D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2,
		0, 0, format, D3DPOOL_DEFAULT, D3DX_FILTER_TRIANGLE, D3DX_FILTER_BOX, 0, NULL, NULL, pD3DTex)))
	{
		return NULL;
	}
	return *pD3DTex;
}
