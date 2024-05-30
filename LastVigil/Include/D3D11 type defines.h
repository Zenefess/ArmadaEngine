/**********************************************************
 * File: D3D11 type defines.h         Created: 2023/04/16 *
 *                              Last modified: 2023/04/28 *
 *                                                        *
 * Desc:                                                  *
 *                                                        *
 * Copyright (c) David William Bull. All rights reserved. *
 **********************************************************/
#pragma once

#define DX     DirectX
#define dxmtrx DirectX::XMMATRIX &
#define aemtrx cmatrix &

#include "typedefs.h"
#include "vector structures.h"

typedef al16       __m128    vector;
typedef al16 const vector    cvector;
typedef al32       AVXmatrix matrix;
typedef al32 const matrix    cmatrix;

// Constant buffers
al32 struct CB_VIEW {
   matrix projection;   // Projection transformation
   matrix orthographic; // Projection transformation
   VEC2Df guiScale;     // Final scaling factors for GUI
   ui64   bitField;
};

al32 struct CB_MAIN {
   matrix camera;     // Camera transformation
   ui32   frameCount; // Total number of frames presented
   float  frameTime;  // Duration of last frame
   float  secsDelta;  // == (Elpased time - SecsTotal)
   union {
      ui32 st_bf;
      struct {
         ui8 secsTotal[3]; // Elapsed seconds
         ui8 bitField;     // 0==Draw transparent sprites, 1-7==???
      };
   };
};

al16 struct CB_LIGHT { // 32 bytes
   VEC3Df  pos;
   float   range;
   fs7p8x3 col;
   struct {
      union { f0p8 s, size; };
      union { f4p4 i, intensity; };
   } hl; // Highlight: Size & intensity : 0p8-1, 4p4-1
   //--- Add radius% cos, and direction
   ui8 RES[8];
};

struct PLANEfl32 { // 16 bytes
   union {
      fl32x4 xmm;
      struct {
         VEC3Df direction;
         fl32   length;
      };
   };
};

struct PLANEfl64 { // 32 bytes
   union {
      fl64x4 ymm;
      struct {
         VEC3Dd direction;
         fl64   length;
      };
   };
};

struct FRUSTUM_DATA { // 192 bytes
   union {
      union {
         fl64x4 ymm[6];
         fl64x2 xmm[12];
         struct {
            fl64x4 ymm0;
            fl64x4 ymm1;
            fl64x4 ymm2;
            fl64x4 ymm3;
            fl64x4 ymm4;
            fl64x4 ymm5;
         };
         struct {
            fl64x2 xmm0;
            fl64x2 xmm1;
            fl64x2 xmm2;
            fl64x2 xmm3;
            fl64x2 xmm4;
            fl64x2 xmm5;
            fl64x2 xmm6;
            fl64x2 xmm7;
            fl64x2 xmm8;
            fl64x2 xmm9;
            fl64x2 xmm10;
            fl64x2 xmm11;
         };
      } f64;
      union {
         fl32x8 ymm[3];
         fl32x4 xmm[6];
         struct {
            fl32x8 ymm0;
            fl32x8 ymm1;
            fl32x8 ymm2;
         };
         struct {
            fl32x4 xmm0;
            fl32x4 xmm1;
            fl32x4 xmm2;
            fl32x4 xmm3;
            fl32x4 xmm4;
            fl32x4 xmm5;
         };
      } f32;
      PLANEfl64 dPlane[6];
      struct {
         PLANEfl64 dLeft;
         PLANEfl64 dBottom;
         PLANEfl64 dRight;
         PLANEfl64 dTop;
         PLANEfl64 dNear;
         PLANEfl64 dFar;
      };
      PLANEfl32 fPlane[6];
      struct {
         PLANEfl32 fLeft;
         PLANEfl32 fBottom;
         PLANEfl32 fRight;
         PLANEfl32 fTop;
         PLANEfl32 fNear;
         PLANEfl32 fFar;
      };
   };
};

al32 struct CAMERA_DATA {
   union {
      struct {
         AVX4Df64 pos64;
         AVX4Df64 rot64;
      };
      AVX8Df32 pos_rot32;
      struct {
         SSE4Df32 pos32;
         SSE4Df32 rot32;
      };
      struct {
         double dXpos;
         double dYpos;
         double dZpos;
         double dSize;
         double dXrot;
         double dYrot;
         double dZrot;
         double dZoom;
      };
      struct {
         float fXpos;
         float fYpos;
         float fZpos;
         float fSize;
         float fXrot;
         float fYrot;
         float fZrot;
         float fZoom;
      };
   };
   FRUSTUM_DATA frustum;
   union {
      AVX4Df64 dView;
      struct {
         fl64 dWidth;
         fl64 dHeight;
         fl64 dNearZ;
         fl64 dFarZ;
      };
      SSE4Df32 fView;
      struct {
         fl32 fWidth;
         fl32 fHeight;
         fl32 fNearZ;
         fl32 fFarZ;
      };
   };
   union {
      AVX4Df64 dDims[2];
      struct {
         SSE4Df32 fDims[2];
         SSE4Ds32 iDims[2];
      };
   };
};
