/************************************************************
 * File: class_camera.h                 Created: 2022/10/20 *
 *                                Last modified: 2025/09/27 *
 *                                                          *
 * Desc:                                                    *
 *                                                          *
 *  Copyright (c) David William Bull. All rights reserved.  *
 ************************************************************/
#pragma once

#include <geometry_math_avx2.h>
#include "Direct3D11 thread.h"
#include "Direct3D11 functions\class_buffers.h"

// Maximum of 8 cameras
al32 struct CLASS_CAM {
   CLASS_BUFFERS        &buf;
   ID3D11Device         *dev;
   ID3D11DeviceContext **devcon;

   CAMERA_DATAf32 *const data32 = (CAMERA_DATAf32*)malloc64(sizeof(CAMERA_DATAf32[8]));
   CAMERA_DATAf64 *const data64 = (CAMERA_DATAf64*)malloc64(sizeof(CAMERA_DATAf64[8]));

   CB_VIEW *const cbProj = (CB_VIEW*)malloc64(sizeof(CB_VIEW[8]));
   CB_MAIN *const cbMain = (CB_MAIN*)malloc64(sizeof(CB_MAIN[8]));

   matrix  *const mCamera        = (matrix*)malloc64(sizeof(matrix[64]));
   matrix  *const mInverseCamera = &mCamera[8];
   matrix  *const mOrthoCamera   = &mCamera[16];
   matrix  *const mProjCamera    = &mCamera[24];
   matrix (*const mProj)[4]      = (matrix(*)[4])&mCamera[32]; // Top half of mCamera

   si32ptrc bufferSlot = (si32ptr)malloc32(sizeof(si32[24]));

   si32 (*const bufferIndex)[2] = (si32(*)[2])&bufferSlot[8];

   cfl32x4 vOrigin  = { 0.0f, 0.0f, 0.0f, 0.0f };
   cfl32x4 vUp      = { 0.0f, 1.0f, 0.0f, 0.0f };
   cfl32x4 vForward = { 0.0f, 0.0f, 1.0f, 0.0f };
   union { AVX8Df32 vCamOri {}; struct { fl32x4 vCamDir, vCamUp; }; };

   VEC2Du8 currentCamProj = {};

   CLASS_CAM(CLASS_BUFFERS &bufClass) : buf(bufClass) { ptrLib[5] = this; }

   inline void SetDimsf(cVEC3Du16 dims, cui8 cam) { data32[cam].fDims[0].vector = { fl32(dims.x), fl32(dims.y), fl32(dims.z), 1.0f }; }

   inline void SetDimsf(cVEC4Du16 dims, cui8 cam) { data32[cam].fDims[0].vector = { fl32(dims.x), fl32(dims.y), fl32(dims.z), fl32(dims.w) }; }

   inline void SetDimsf(cVEC3Du16 chunkDims, cVEC3Du16 chunkCounts, cui8 cam) {
      data32[cam].fDims[0].vector = { fl32(chunkDims.x), fl32(chunkDims.y), fl32(chunkDims.z), 1.0f };
      data32[cam].fDims[1].vector = { fl32(chunkCounts.x), fl32(chunkCounts.y), fl32(chunkCounts.z), 1.0f };
   }

   inline void SetDimsf(cVEC4Du16 chunkDims, cVEC4Du16 chunkCounts, cui8 cam) {
      data32[cam].fDims[0].vector = { fl32(chunkDims.x), fl32(chunkDims.y), fl32(chunkDims.z), fl32(chunkDims.w) };
      data32[cam].fDims[1].vector = { fl32(chunkCounts.x), fl32(chunkCounts.y), fl32(chunkCounts.z), fl32(chunkDims.w) };
   }

   inline void SetCamera(cui8 cam) { currentCamProj.x = cam; }

   inline void SetProjection(cui8 proj) { currentCamProj.y = proj; }

   inline void SetCameraAndProjection(cui8 cam, cui8 proj) { currentCamProj = { cam, proj }; }

   void CreateCamera(cui8 cam, cui8 projectionStages, cui8 cameraStages, cui8 slot, cbool setAsCurrent) {
      bufferIndex[cam][0] = buf.CreateConstant(0, &cbProj[cam], sizeof(CB_VIEW), 1, slot, projectionStages, false);
      bufferIndex[cam][1] = buf.CreateConstant(0, &cbMain[cam], sizeof(CB_MAIN), 1, slot + 1, cameraStages, false);

      bufferSlot[cam] = slot;

      data32[cam].fZoom   = 1.0f;
      data32[cam].fSize   = 256.0f;
      data32[cam].fWidth  = 1.0f;
      data32[cam].fHeight = 1.0f;
      data32[cam].fNearZ  = 1.0f;
      data32[cam].fFarZ   = 16384.0f;

      if(setAsCurrent) currentCamProj.x = cam;
   }

   inline void LoadProjection(cmatrix mView, cui8 cam, cui8 proj) { mProj[cam][proj] = mView; }

   inline void LoadCamera(cmatrix mView, cui8 cam) { mCamera[cam] = mView; }

   inline void SetCameraPos(cfl32 x, cfl32 y, cfl32 z, cui8 cam) { data32[cam].fXpos = x; data32[cam].fYpos = y; data32[cam].fZpos = z; }
   inline void SetCameraPos(cfl64 x, cfl64 y, cfl64 z, cui8 cam) { data64[cam].dXpos = x; data64[cam].dYpos = y; data64[cam].dZpos = z; }

   inline void SetCameraRot(cfl32 x, cfl32 y, cfl32 z, cui8 cam) { data32[cam].fXrot = x; data32[cam].fYrot = y; data32[cam].fZrot = z; }
   inline void SetCameraRot(cfl64 x, cfl64 y, cfl64 z, cui8 cam) { data64[cam].dXrot = x; data64[cam].dYrot = y; data64[cam].dZrot = z; }

   inline void MoveCameraUpY(cfl32 distance, cui8 cam) { data32[cam].fYpos += distance; }
   inline void MoveCameraUpY(cfl64 distance, cui8 cam) { data64[cam].dYpos += distance; }

   inline void MoveCameraRightXZ(cfl32 distance, cui8 cam) {
      data32[cam].fXpos += cosf(data32[cam].fYrot) * distance;
      data32[cam].fZpos -= sinf(data32[cam].fYrot) * distance;
   }
   inline void MoveCameraRightXZ(cfl64 distance, cui8 cam) {
      data64[cam].dXpos += cos(data64[cam].dYrot) * distance;
      data64[cam].dZpos -= sin(data64[cam].dYrot) * distance;
   }

   inline void MoveCameraForwardXZ(cfl32 distance, cui8 cam) {
      data32[cam].fXpos += sinf(data32[cam].fYrot) * distance;
      data32[cam].fZpos += cosf(data32[cam].fYrot) * distance;
   }
   inline void MoveCameraForwardXZ(cfl64 distance, cui8 cam) {
      data64[cam].dXpos += sin(data64[cam].dYrot) * distance;
      data64[cam].dZpos += cos(data64[cam].dYrot) * distance;
   }

   inline void MoveCamera2DVectorXZ(cVEC2Df distance, cui8 cam) {
      //cfl32 fXZsq = distance.x * distance.x + distance.y * distance.y;
      if(!(distance.x + distance.y)) return;
      cfl32 fSinY = sinf(data32[cam].fYrot);
      cfl32 fCosY = cosf(data32[cam].fYrot);
      data32[cam].fXpos += fSinY * distance.y + fCosY * distance.x;
      data32[cam].fZpos += fCosY * distance.y - fSinY * distance.x;
   }
   inline void MoveCamera2DVectorXZ(cVEC2Dd distance, cui8 cam) {
      //cfl64 dXZsq = distance.x * distance.x + distance.y * distance.y;
      if(!(distance.x + distance.y)) return;
      cfl64 dSinY = sin(data64[cam].dYrot);
      cfl64 dCosY = cos(data64[cam].dYrot);
      data64[cam].dXpos += dCosY * distance.x + dSinY * distance.y;
      data64[cam].dZpos += dCosY * distance.y - dSinY * distance.x;
   }
   // Projections 2 & 3 use square aspect ratio
   inline void PrecalculateProjections(cui8 cam) {
      mProj[cam][0] = (aemtrx)DX::XMMatrixPerspectiveLH(data32[cam].fZoom * data32[cam].fWidth, data32[cam].fZoom * data32[cam].fHeight, data32[cam].fNearZ, data32[cam].fFarZ);
      mProj[cam][1] = (aemtrx)DX::XMMatrixOrthographicLH(data32[cam].fSize * ScrRes.dims[ScrRes.state].aspectI, data32[cam].fSize, data32[cam].fNearZ, data32[cam].fFarZ);
      mProj[cam][2] = (aemtrx)DX::XMMatrixPerspectiveLH(data32[cam].fZoom, data32[cam].fZoom, data32[cam].fNearZ, data32[cam].fFarZ);
      mProj[cam][3] = (aemtrx)DX::XMMatrixOrthographicLH(data32[cam].fSize, data32[cam].fSize, data32[cam].fNearZ, data32[cam].fFarZ);
   }

#if !defined(USE_OLD_CODE)
   inline void TransformCamera(cui8 cam, cbool squareAspect) {
      cfl32x4 vCamPos    = _mm_setr_ps(data32[cam].fXpos, data32[cam].fYpos, data32[cam].fZpos, 0.0f);
      cmatrix mCamRot    = MatrixRotationRollPitchYaw(data32[cam].fXrot, data32[cam].fYrot, -data32[cam].fZrot);
      cfl32x4 vCamRotPos = Transform3D(vCamPos, mCamRot);

      vCamUp  = TransformNormal3D(vUp, mCamRot);
      vCamDir = TransformNormal3D(vForward, mCamRot);

      cmatrix     mLookAtFromOrigin    = MatrixLookAtLH(vOrigin, vCamDir, vCamUp);
      cfl32x4     vCamRotPosFromOrigin = Transform3D(vCamRotPos, mLookAtFromOrigin);
      cAVXmatrix2 viewFrame            = BuildViewFrame(vCamRotPosFromOrigin, vCamDir, vCamUp);

      mCamera[cam]        = viewFrame.view;
      mInverseCamera[cam] = viewFrame.inverse;

      cmatrix &projPerspective = mProj[cam][(squareAspect ? 2 : 0)];
      cmatrix &projOrtho       = mProj[cam][(squareAspect ? 3 : 1)];

      mProjCamera[cam]  = MatrixMultiply(viewFrame.view, projPerspective);
      mOrthoCamera[cam] = MatrixMultiply(viewFrame.view, projOrtho);

      cmatrix &projCam = mProjCamera[cam];
      fl32x4   col0    = projCam.xmm[0], col1 = projCam.xmm[1], col2 = projCam.xmm[2], col3 = projCam.xmm[3];

      _MM_TRANSPOSE4_PS(col0, col1, col2, col3);

      cfl32x8 col01  = _mm256_insertf128_ps(_mm256_castps128_ps256(col0), col1, 1);
      cfl32x8 col3x2 = _mm256_insertf128_ps(_mm256_castps128_ps256(col3), col3, 1);
      cfl32x8 ones   = _mm256_set1_ps(1.0f);

      data32[cam].frustum.ymm[0] = _mm256_fmadd_ps(col01, ones, col3x2);
      data32[cam].frustum.ymm[1] = _mm256_fnmadd_ps(col01, ones, col3x2);

      cfl32x4 ones128 = _mm_set1_ps(1.0f);

      data32[cam].frustum.xmm[4] = _mm_fmadd_ps(col2, ones128, col3);
      data32[cam].frustum.xmm[5] = _mm_fnmadd_ps(col2, ones128, col3);

      for(ui8 i = 0; i < 6; i++) {
         cfl32 mag = sqrtf(_mm_dp_ps(data32[cam].frustum.xmm[i], data32[cam].frustum.xmm[i], 0x071).m128_f32[0]);
         data32[cam].frustum.xmm[i] = _mm_div_ps(data32[cam].frustum.xmm[i], _mm_set_ps1(mag));
      }
   }
#else
   inline void TransformCamera(cui8 cam, cbool squareAspect) {
      cfl32x4 vCamPos    = { data32[cam].fXpos, data32[cam].fYpos, data32[cam].fZpos, 0.0f };
      cmatrix mCamRot    = MatrixRotationRollPitchYaw(data32[cam].fXrot, data32[cam].fYrot, -data32[cam].fZrot);
      cfl32x4 vCamRotPos = Transform3D(vCamPos, mCamRot);

      vCamUp  = TransformNormal3D(vUp, mCamRot);
      vCamDir = TransformNormal3D(vForward, mCamRot);

      cmatrix mLookAtFromOrigin    = MatrixLookAtLH(vOrigin, vCamDir, vCamUp);
      cfl32x4 vCamRotPosFromOrigin = Transform3D(vCamRotPos, mLookAtFromOrigin);

      mCamera[cam]        = (aemtrx)DX::XMMatrixLookToLH(vCamRotPosFromOrigin, vCamDir, vCamUp);
      mInverseCamera[cam] = (aemtrx)DX::XMMatrixInverse(NULL, (dxmtrx)mCamera[cam]);
      mProjCamera[cam]    = MatrixMultiply(mCamera[cam], mProj[cam][(squareAspect ? 2 : 0)]);
      mOrthoCamera[cam]   = MatrixMultiply(mCamera[cam], mProj[cam][(squareAspect ? 3 : 1)]);

      cAVX8Df32 (&mFrustum)[2] = (cAVX8Df32(&)[2])mProjCamera[cam];

      cAVX8Df32 m[2] = { { ._fl = { mFrustum[0].vector.x.x, mFrustum[0].vector.y.x, mFrustum[1].vector.x.x, mFrustum[1].vector.y.x,
                                    mFrustum[0].vector.x.y, mFrustum[0].vector.y.y, mFrustum[1].vector.x.y, mFrustum[1].vector.y.y } },
                         { ._fl = { mFrustum[0].vector.x.z, mFrustum[0].vector.y.z, mFrustum[1].vector.x.z, mFrustum[1].vector.y.z,
                                    mFrustum[0].vector.x.w, mFrustum[0].vector.y.w, mFrustum[1].vector.x.w, mFrustum[1].vector.y.w } } };

      cfl32x8 &mw = (cfl32x8 &)_mm256_broadcastsi128_si256((cui128 &)m[1].xmm[1]);

 #ifdef __AVX__
      data32[cam].frustum.ymm[0] = _mm256_add_ps(mw, m[0].ymm);
      data32[cam].frustum.ymm[1] = _mm256_sub_ps(mw, m[0].ymm);
 #else
      data32[cam].frustum.xmm[0] = _mm_add_ps(m[1].xmm1, m[0].xmm0);
      data32[cam].frustum.xmm[1] = _mm_add_ps(m[1].xmm1, m[0].xmm1);
      data32[cam].frustum.xmm[2] = _mm_sub_ps(m[1].xmm1, m[0].xmm0);
      data32[cam].frustum.xmm[3] = _mm_sub_ps(m[1].xmm1, m[0].xmm1);
 #endif
      data32[cam].frustum.xmm[4] = _mm_add_ps(m[1].xmm1, m[1].xmm0);
      data32[cam].frustum.xmm[5] = _mm_sub_ps(m[1].xmm1, m[1].xmm0);

      for(ui8 i = 0; i < 6; i++) Normalize3D(data32[cam].frustum.xmm[i]);
   }
#endif

   inline void UploadProjections(cui64 bits, cui8 cam) {
      cbProj[cam] = { (aemtrx)DX::XMMatrixTranspose((dxmtrx)mProj[cam][0]), (aemtrx)DX::XMMatrixTranspose((dxmtrx)mProj[cam][1]), { ScrRes.dims[ScrRes.state].aspect, 1.0f }, bits };

      buf.UpdateConstant(0, &cbProj[cam], bufferIndex[cam][0], 1);
   }

   inline void UploadCamera(cui32 frameCount, cfl32 frameTime, cfl32 secsDelta, cui32 secsTotal, cui8 bits, cui8 cam) {
      cbMain[cam] = { (aemtrx)DX::XMMatrixTranspose((dxmtrx)mCamera[cam]), frameCount, frameTime, secsDelta, (secsTotal & 0x0FFFFFF) | (bits << 24)};

      buf.UpdateConstant(0, &cbMain[cam], bufferIndex[cam][1], 1);
   }

   inline void UploadCamera(cfl64 totalTime, cfl32 frameTime, cui32 frameCount, cui8 bits, cui8 cam) {
      cfl64 dTotalSecs  = trunc(totalTime);
      cfl32 fDeltaSecs  = cfl32(totalTime - dTotalSecs);
      cui32 uiTotalSecs = ui32(dTotalSecs) & 0x0FFFFFF;

      cbMain[cam] = { (aemtrx)DX::XMMatrixTranspose((dxmtrx)mCamera[cam]), frameCount, frameTime, fDeltaSecs, uiTotalSecs | (bits << 24) };

      buf.UpdateConstant(0, &cbMain[cam], bufferIndex[cam][1], 1);
   }

   inline cfl32 DistanceFromCamera(csi128 target, cui8 cam) {
      cfl32x4 diff   = _mm_sub_ps(_mm_cvtepi32_ps(target), data32[cam].pos.xmm);
      cfl32x4 square = _mm_mul_ps(diff, diff);

      return sqrtf(square.m128_f32[0] + square.m128_f32[1] + square.m128_f32[2]);
   }

   inline cfl32 DistanceFromCamera(cfl32x4 target, cui8 cam) {
      cfl32x4 diff   = _mm_sub_ps(target, data32[cam].pos.xmm);
      cfl32x4 square = _mm_mul_ps(diff, diff);

      return sqrtf(square.m128_f32[0] + square.m128_f32[1] + square.m128_f32[2]);
   }

   inline cfl32 CursorSphereIntersect(cfl32x4 &sphere, cVEC2Ds32 curPos, cVEC2Du8 camProj) const {
      cmatrix mInvertedCam = mInverseCamera[camProj.x];
      cVEC2Df vScreen      = { curPos.x * ScrRes.rcpDims.w - 1.0f, 1.0f - (curPos.y * ScrRes.rcpDims.h) };
      cVEC2Df vProjected   = { vScreen.x / mProj[camProj.x][camProj.y].fl[0], vScreen.y / mProj[camProj.x][camProj.y].fl[5]};

      cAVX8Df32 projCOs = { .xmm = { _mm_set_ps1(vProjected.x), _mm_set_ps1(vProjected.y) } };
      cAVX8Df32 rayDir_ = { .ymm = _mm256_mul_ps(projCOs.ymm, mInvertedCam.ymm[0])};
      cSSE4Df32 rayDir  = { .xmm = _mm_add_ps(_mm_add_ps(rayDir_.xmm0, rayDir_.xmm1), mInvertedCam.xmm[2])};

      cfl32x4 vRayOri = _mm_sub_ps(data32[camProj.x].pos.xmm, sphere);
      cfl32x4 vRayDir = DX::XMVector3Normalize(rayDir.xmm);

      // Calculate the a, b, and c coefficients
      cfl32 a = _mm_dp_ps(vRayDir, vRayDir, 0x071).m128_f32[0];
      cfl32 b = _mm_dp_ps(vRayDir, vRayOri, 0x071).m128_f32[0];
      cfl32 c = _mm_dp_ps(vRayOri, vRayOri, 0x071).m128_f32[0] - (sphere.m128_f32[3] * sphere.m128_f32[3]);
      // Returns discriminant; if result is negative the picking ray missed the sphere
      return b - (a * c);
   }

   // Return values are rayMissed if target is beyond horizon
   inline cVEC2Ds32 __vectorcall CursorLayerIntersect(csi32 layer, cfl32x4 camOffset, cVEC2Ds32 curPos, cui8 cam, cui8 proj) const noexcept {
      cmatrix mInvertedCam = mInverseCamera[cam];
      cVEC2Df vScreen      = { curPos.x * ScrRes.rcpDims.w - 1.0f, 1.0f - (curPos.y * ScrRes.rcpDims.h) };
      cVEC2Df vProjected   = { vScreen.x / mProj[cam][proj].fl[0], vScreen.y / mProj[cam][proj].fl[5] };

      cAVX8Df32 projCOs = { .xmm = { _mm_set_ps1(vProjected.x), _mm_set_ps1(vProjected.y) } };
      cAVX8Df32 rayDir_ = { .ymm = _mm256_mul_ps(projCOs.ymm, mInvertedCam.ymm[0]) };
      cSSE4Df32 rayDir  = { .xmm = _mm_add_ps(_mm_add_ps(rayDir_.xmm0, rayDir_.xmm1), mInvertedCam.xmm[2]) };

      cfl32x4 vRayOri = _mm_add_ps(data32[cam].pos.xmm, camOffset);
      cfl32x4 vRayDir = DX::XMVector3Normalize(rayDir.xmm);

      cfl32x4 planeDir = { 0.0f, 0.0f, 1.0f };
      cfl32x4 planeOri = { 0.0f, 0.0f, fl32(layer)};

      cfl32 denom = _mm_dp_ps(planeDir, vRayDir, 0x071).m128_f32[0];

      if(denom != 0.0f) {
         cfl32x4 vDiff = _mm_sub_ps(planeOri, vRayOri);
         cfl32x4 vT    = DX::XMVector3Dot(vDiff, planeDir);
         cfl32   fT    = vT.m128_f32[0] / denom;

         if(fT > 0.0f) {
            // Calculate 2D coordinate
            cVEC2Df   fStep = { vRayDir.m128_f32[0] * fT, vRayDir.m128_f32[1] * fT };
            cVEC2Ds32 vCell = { si32(floorf(vRayOri.m128_f32[0] + fStep.x)), si32(floorf(vRayOri.m128_f32[1] + fStep.y)) };

            return vCell;
         }
      }
      return { rayMissed, rayMissed };
   }

   // First return value is (quiet) NaN if target is beyond horizon
   inline cVEC3Df CursorLayerIntersect(csi32 layer, cVEC2Ds32 curPos, cVEC2Du8 camProj) const {
      cmatrix &mInvertedCam = mInverseCamera[camProj.x];
      cVEC2Df  vScreen      = { curPos.x * ScrRes.rcpDims.w - 1.0f, 1.0f - (curPos.y * ScrRes.rcpDims.h) };
      cVEC2Df  vProjected   = { vScreen.x / mProj[camProj.x][camProj.y].fl[0], vScreen.y / mProj[camProj.x][camProj.y].fl[5] };

      cAVX8Df32 projCOs = { .xmm = { _mm_set_ps1(vProjected.x), _mm_set_ps1(vProjected.y) } };
      cAVX8Df32 rayDir_ = { .ymm = _mm256_mul_ps(projCOs.ymm, mInvertedCam.ymm[0]) };
      cSSE4Df32 rayDir  = { .xmm = _mm_add_ps(_mm_add_ps(rayDir_.xmm0, rayDir_.xmm1), mInvertedCam.xmm[2]) };

//      cfl32x4 vRayOri = _mm_add_ps(data[camProj.x].pos32.xmm, camOffset);
      cfl32x4 vRayOri = data32[camProj.x].pos.xmm;
      cfl32x4 vRayDir = DX::XMVector3Normalize(rayDir.xmm);

      cfl32   coordZ   = fl32(layer);
      cfl32x4 planeDir = { 0.0f, 0.0f, 1.0f };
      cfl32x4 planeOri = { 0.0f, 0.0f, coordZ};

      cfl32 denom = _mm_dp_ps(planeDir, vRayDir, 0x071).m128_f32[0];

      if(denom != 0.0f) {
         cfl32x4 vDiff = _mm_sub_ps(planeOri, vRayOri);
         cfl32x4 vT    = DX::XMVector3Dot(vDiff, planeDir);
         cfl32   fT    = vT.m128_f32[0] / denom;

         if(fT > 0.0f) {
            // Calculate 2D coordinate
            cVEC2Df fStep = { vRayDir.m128_f32[0] * fT, vRayDir.m128_f32[1] * fT };

            return { vRayOri.m128_f32[0] + fStep.x, vRayOri.m128_f32[1] + fStep.y, coordZ };
         }
      }

      static cui32 uiNaN = NAN; // From <math.h>

      return { (cfl32 &)uiNaN };
   }

   // Return value is false if target is beyond horizon
   inline cbool CursorLayerIntersect(SSE4Df32 &location, cVEC2Ds32 curPos, cVEC2Du8 camProj) const {
      cmatrix mInvertedCam = mInverseCamera[camProj.x];
      cVEC2Df vScreen      = { curPos.x * ScrRes.rcpDims.w - 1.0f, 1.0f - (curPos.y * ScrRes.rcpDims.h) };
      cVEC2Df vProjected   = { vScreen.x / mProj[camProj.x][camProj.y].fl[0], vScreen.y / mProj[camProj.x][camProj.y].fl[5] };

      cAVX8Df32 projCOs = { .xmm = { _mm_set_ps1(vProjected.x), _mm_set_ps1(vProjected.y) } };
      cAVX8Df32 rayDir_ = { .ymm = _mm256_mul_ps(projCOs.ymm, mInvertedCam.ymm[0]) };
      cSSE4Df32 rayDir  = { .xmm = _mm_add_ps(_mm_add_ps(rayDir_.xmm0, rayDir_.xmm1), mInvertedCam.xmm[2]) };

//      cfl32x4 vRayOri = _mm_add_ps(data[camProj.x].pos32.xmm, camOffset);
      cfl32x4 vRayOri = data32[camProj.x].pos.xmm;
      cfl32x4 vRayDir = DX::XMVector3Normalize(rayDir.xmm);

      cfl32x4 planeDir = { 0.0f, 0.0f, 1.0f };
      cfl32x4 planeOri = { 0.0f, 0.0f, location.vector.z};

      cfl32 denom = _mm_dp_ps(planeDir, vRayDir, 0x071).m128_f32[0];

      if(denom != 0.0f) {
         cfl32x4 vDiff = _mm_sub_ps(planeOri, vRayOri);
         cfl32x4 vT    = DX::XMVector3Dot(vDiff, planeDir);
         cfl32   fT    = vT.m128_f32[0] / denom;

         if(fT > 0.0f) {
            // Calculate 2D coordinate
            cVEC2Df fStep = { vRayDir.m128_f32[0] * fT, vRayDir.m128_f32[1] * fT };

            location.vector.x = vRayOri.m128_f32[0] + fStep.x;
            location.vector.y = vRayOri.m128_f32[1] + fStep.y;

            return true;
         }
      }

      return false;
   }

   // Each true bit in the return value == sphere visible
   inline cui8 SphereFrustumIntersect2(cAVX8Df32 spheres, cui8 cam) {
      ui8 i, success = 0x03;

      for (i = 0; i < 6; i++)
         if(_mm_dp_ps(spheres.xmm0, data32[cam].frustum.xmm[i], 0x0F1).m128_f32[0] < 0) { success ^= 0x01; break; }
      for (i = 0; i < 6; i++)
         if(_mm_dp_ps(spheres.xmm1, data32[cam].frustum.xmm[i], 0x0F1).m128_f32[0] < 0) { return success ^ 0x02; }

      return success;
   }

   // Each true bit in the return value == sphere visible
   inline cui8 SphereFrustumIntersect8(cSSE4Df32 (&spheres)[8], cui8 cam) {
      PLANEfl32 (&plane)[6] = data32[cam].frustum.fPlane;
      ui8 i, success = 0xFF;

      for (i = 0; i < 6; i++)
         if(_mm_dp_ps(spheres[0].xmm, plane[i].xmm, 0x071).m128_f32[0] + plane[i].xmm.m128_f32[3] < -spheres[0].vector.w) { success ^= 0x01; break; }
      for (i = 0; i < 6; i++)
         if(_mm_dp_ps(spheres[1].xmm, plane[i].xmm, 0x071).m128_f32[0] + plane[i].xmm.m128_f32[3] < -spheres[1].vector.w) { success ^= 0x02; break; }
      for (i = 0; i < 6; i++)
         if(_mm_dp_ps(spheres[2].xmm, plane[i].xmm, 0x071).m128_f32[0] + plane[i].xmm.m128_f32[3] < -spheres[2].vector.w) { success ^= 0x04; break; }
      for (i = 0; i < 6; i++)
         if(_mm_dp_ps(spheres[3].xmm, plane[i].xmm, 0x071).m128_f32[0] + plane[i].xmm.m128_f32[3] < -spheres[3].vector.w) { success ^= 0x08; break; }
      for (i = 0; i < 6; i++)
         if(_mm_dp_ps(spheres[4].xmm, plane[i].xmm, 0x071).m128_f32[0] + plane[i].xmm.m128_f32[3] < -spheres[4].vector.w) { success ^= 0x10; break; }
      for (i = 0; i < 6; i++)
         if(_mm_dp_ps(spheres[5].xmm, plane[i].xmm, 0x071).m128_f32[0] + plane[i].xmm.m128_f32[3] < -spheres[5].vector.w) { success ^= 0x20; break; }
      for (i = 0; i < 6; i++)
         if(_mm_dp_ps(spheres[6].xmm, plane[i].xmm, 0x071).m128_f32[0] + plane[i].xmm.m128_f32[3] < -spheres[6].vector.w) { success ^= 0x40; break; }
      for (i = 0; i < 6; i++)
         if(_mm_dp_ps(spheres[7].xmm, plane[i].xmm, 0x071).m128_f32[0] + plane[i].xmm.m128_f32[3] < -spheres[7].vector.w) { return success ^ 0x80; }

      return success;
   }

   // Each true bit in the return value == chunk visible
   inline cui8 ChunkFrustumIntersect2_(cAVX8Ds32 chunks, cui8 cam, cui8 proj) {
      cmatrix mViewSpace = (aemtrx)DX::XMMatrixMultiply((dxmtrx)mCamera[cam], (dxmtrx)mProj[cam][proj]);

      cfl32 depthOS      = data32[cam].fDims[1].vector.z * 0.5f;
      cfl32 boundingSize = max(max(data32[cam].fDims[0].vector.x, data32[cam].fDims[0].vector.y), data32[cam].fDims[0].vector.z) * 0.86602540378443864676372317075294f;

      cAVX8Df32 chunkOffsets = { ._fl = { 0.5f, 0.5f, depthOS + 0.5f, 1.0f, 0.5f, 0.5f, depthOS + 0.5f, 1.0f } };
      cAVX8Df32 chunkScale   = { .xmm = { data32[cam].fDims[0].xmm, data32[cam].fDims[0].xmm } };
      cAVX8Df32 chunkOrigins = { .ymm = _mm256_mul_ps(_mm256_add_ps(_mm256_cvtepi32_ps(chunks.ymm), chunkOffsets.ymm), chunkScale.ymm) };

      cAVX8Df32 rad = { ._fl = { boundingSize, boundingSize, chunkOrigins.vector.x.z, chunkOrigins.vector.x.w,
                                 boundingSize, boundingSize, chunkOrigins.vector.y.z, chunkOrigins.vector.y.w } };

      cAVX8Df32 rayToCam = { .xmm = { DX::XMVector3Transform(chunkOrigins.xmm0, (dxmtrx)mViewSpace), DX::XMVector3Transform(chunkOrigins.xmm1, (dxmtrx)mViewSpace) } };
      cAVX8Df32 radProj  = { .xmm = { DX::XMVector3Transform(rad.xmm0, (dxmtrx)mViewSpace), DX::XMVector3Transform(rad.xmm1, (dxmtrx)mViewSpace) } };

      cVEC2Df   rcpDepth   = { 1.0f / rayToCam.vector.x.w, 1.0f / rayToCam.vector.y.w };
      cSSE4Df32 rcpDepths  = { .xmm = (__m128 &)_mm_set1_epi64x((cui64 &)rcpDepth) };
      cSSE4Df32 rayXYs     = { .vector = { rayToCam.vector.x.x, rayToCam.vector.x.y, rayToCam.vector.y.x, rayToCam.vector.y.y } };
      cSSE4Df32 rayOrigins = { .xmm = _mm_mul_ps(rayXYs.xmm, rcpDepths.xmm) };

      cSSE4Df32 radPrXYs = { .vector = { radProj.vector.x.x, radProj.vector.x.y, radProj.vector.y.x, radProj.vector.y.y } };
      cSSE4Df32 radius   = { .xmm = _mm_mul_ps(radPrXYs.xmm, rcpDepths.xmm) };

      cVEC4Df antiSize = { data32[cam].fZoom - radius.vector.x, data32[cam].fZoom - radius.vector.y,
                           data32[cam].fZoom - radius.vector.z, data32[cam].fZoom - radius.vector.w };

      cui8 result = (fabsf(rayOrigins.vector.x) > antiSize.x || fabsf(rayOrigins.vector.y) > antiSize.y ? 0 : 0x01) |
                    (fabsf(rayOrigins.vector.z) > antiSize.z || fabsf(rayOrigins.vector.w) > antiSize.w ? 0 : 0x02);

      return result;
   }

   // Each true bit in the return value == chunk visible
   inline cui8 ChunkFrustumIntersect2(cAVX8Ds32 chunks, cui8 cam) {
      cAVX8Df32 chunkOffsets = { ._fl = { 0.5f, 0.5f, 0.5f, 1.0f, 0.5f, 0.5f, 0.5f, 1.0f } };
      cAVX8Df32 chunkScale   = { .xmm = { data32[cam].fDims[0].xmm, data32[cam].fDims[0].xmm } };
      cAVX8Df32 chunkOrigins = { .ymm = _mm256_mul_ps(_mm256_add_ps(_mm256_cvtepi32_ps(chunks.ymm), chunkOffsets.ymm), chunkScale.ymm) };

      cfl32 boundingSize = Max3(chunkScale.vector.x) * -0.86602540378443864676372317075294f;

      ui8 i, success = 0x03;

      for (i = 0; i < 6; i++) {
         cfl32x4 plData = data32[cam].frustum.xmm[i];
         cfl32   side   = _mm_dp_ps(chunkOrigins.xmm0, plData, 0x0F1).m128_f32[0];
         if (side < boundingSize) {
            success &= 0x0FE;
            break;
         }
      }
      for (i = 0; i < 6; i++) {
         cfl32x4 plData = data32[cam].frustum.xmm[i];
         cfl32   side   = _mm_dp_ps(chunkOrigins.xmm1, plData, 0x0F1).m128_f32[0];
         if (side < boundingSize) {
            success &= 0x0FD;
            break;
         }
      }

      return success;
   }

#if defined(USE_ORACLE_CODE)
   // Each true bit in the return value == chunk visible
   inline cui8 ChunkFrustumIntersect8(SSE4Ds32 chunk, cui8 cam) {
      cAVX8Df32 chunkOffsets = { ._fl = { 0.5f, 0.5f, 0.5f, 1.0f, 0.5f, 0.5f, 0.5f, 1.0f } };
      cAVX8Df32 chunkScale   = { .xmm = { data32[cam].fDims[0].xmm, data32[cam].fDims[0].xmm } };

      cfl32 boundingSize = Max3(chunkScale.vector.x) * -0.86602540378443864676372317075294f;

      ui8 i, j, success = 0xFF;

      for(i = 0; i < 8; i += 2, chunk.vector.x++) {
         cSSE4Ds32 chunk1       = { .vector = { chunk.vector.x++, chunk.vector.y, chunk.vector.z } };
         cAVX8Ds32 chunks       = { .xmm = { chunk1.xmm, chunk.xmm } };
         cAVX8Df32 chunkOrigins = { .ymm = _mm256_mul_ps(_mm256_add_ps(_mm256_cvtepi32_ps(chunks.ymm), chunkOffsets.ymm), chunkScale.ymm) };

         for (j = 0; j < 6; j++) {
            cfl32x4 plData = data32[cam].frustum.fPlane[j].xmm;
            cfl32   side   = _mm_dp_ps(chunkOrigins.xmm0, plData, 0x0F1).m128_f32[0];
            if (side < boundingSize) {
               success ^= 0x01 << i;
               break;
            }
         }
         for (j = 0; j < 6; j++) {
            cfl32x4 plData = data32[cam].frustum.fPlane[j].xmm;
            cfl32   side   = _mm_dp_ps(chunkOrigins.xmm1, plData, 0x0F1).m128_f32[0];
            if (side < boundingSize) {
               success ^= 0x02 << i;
               break;
            }
         }
      }

      return success;
   }
#else
   // Each true bit in the return value == chunk visible
   inline cui8 ChunkFrustumIntersect8(SSE4Ds32 chunk, cui8 cam) {
      cVEC4Df &chunkScale = data32[cam].fDims[0].vector;

      cfl32 boundingSize = Max3(chunkScale) * -0.86602540378443864676372317075294f;

      csi32 baseX = chunk.vector.x;
      cfl32 baseY = fl32(chunk.vector.y);
      cfl32 baseZ = fl32(chunk.vector.z);

      cfl32 halfScaleX = chunkScale.x * 0.5f;
      cfl32 halfScaleY = chunkScale.y * 0.5f;
      cfl32 halfScaleZ = chunkScale.z * 0.5f;

      cfl32x8 baseIndices = _mm256_cvtepi32_ps(_mm256_add_epi32(_mm256_set1_epi32(baseX), _mm256_setr_epi32(0, 1, 2, 3, 4, 5, 6, 7)));
      cfl32x8 chunkX      = _mm256_fmadd_ps(baseIndices, _mm256_set1_ps(chunkScale.x), _mm256_set1_ps(halfScaleX));
      cfl32x8 chunkY      = _mm256_fmadd_ps(_mm256_set1_ps(baseY), _mm256_set1_ps(chunkScale.y), _mm256_set1_ps(halfScaleY));
      cfl32x8 chunkZ      = _mm256_fmadd_ps(_mm256_set1_ps(baseZ), _mm256_set1_ps(chunkScale.z), _mm256_set1_ps(halfScaleZ));
      cfl32x8 chunkW      = _mm256_set1_ps(chunkScale.w);
      cfl32x8 threshold   = _mm256_set1_ps(boundingSize);

      ui8 success = 0xFF;

      for(ui8 planeIndex = 0; planeIndex < 6 && success; planeIndex++) {
         const PLANEfl32 &plane = data32[cam].frustum.fPlane[planeIndex];

         cfl32x8 planeX = _mm256_broadcast_ss(&plane.xmm.m128_f32[0]);
         cfl32x8 planeY = _mm256_broadcast_ss(&plane.xmm.m128_f32[1]);
         cfl32x8 planeZ = _mm256_broadcast_ss(&plane.xmm.m128_f32[2]);
         cfl32x8 planeW = _mm256_broadcast_ss(&plane.xmm.m128_f32[3]);

         fl32x8 distance = _mm256_fmadd_ps(chunkX, planeX, _mm256_setzero_ps());
                distance = _mm256_fmadd_ps(chunkY, planeY, distance);
                distance = _mm256_fmadd_ps(chunkZ, planeZ, distance);
                distance = _mm256_fmadd_ps(chunkW, planeW, distance);

         cui8 mask = cui8(_mm256_movemask_ps(_mm256_cmp_ps(distance, threshold, _CMP_LT_OQ)));
         if(mask) success &= cui8(~mask);
      }

      return success;
   }
#endif
};
