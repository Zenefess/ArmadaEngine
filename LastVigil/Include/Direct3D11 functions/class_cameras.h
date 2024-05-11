/************************************************************
 * File: class_camera.h                 Created: 2022/10/20 *
 *                                Last modified: 2024/05/09 *
 *                                                          *
 * Desc:                                                    *
 *                                                          *
 *  Copyright (c) David William Bull. All rights reserved.  *
 ************************************************************/
#pragma once

#include "Direct3D11 thread.h"
#include "Direct3D11 functions\class_buffers.h"

al32 struct CLASS_CAM {
   ID3D11Device         *dev;
   ID3D11DeviceContext **devcon;
   CLASS_BUFFERS        *buf;

   cfl32x4 vOrigin  = DX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
   cfl32x4 vUp      = DX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
   cfl32x4 vForward = DX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

   CAMERA_DATA data[8] {};

   CB_VIEW cbProj[8] {};
   CB_MAIN cbMain[8] {};
   matrix  mCamera[8] {}, mInverseCamera[8] {}, mPerspCamera[8] {}, mProjCamera[8] {}, mProj[4] {};
   fl32x4  vCamDir {}, vCamUp {};
   si32    bufferIndex[8][2] {}, bufferSlot[8] = {};
   VEC2Du8 currentCamProj {};

   CLASS_CAM(void) { ptrLib[5] = this; }

   inline void SetDims(cVEC3Ds16 dims, cui8 cam) { data[cam].iDims[0].vector = { dims.x, dims.y, dims.z, 1 }; }

   inline void SetDims(cVEC4Ds16 dims, cui8 cam) { data[cam].iDims[0].vector = { dims.x, dims.y, dims.z, dims.w }; }

   inline void SetDimsf(cVEC3Ds16 dims, cui8 cam) { data[cam].fDims[0].vector = { fl32(dims.x), fl32(dims.y), fl32(dims.z), 1.0f }; }

   inline void SetDimsf(cVEC4Ds16 dims, cui8 cam) { data[cam].fDims[0].vector = { fl32(dims.x), fl32(dims.y), fl32(dims.z), fl32(dims.w) }; }

   inline void SetDimsf(cVEC3Ds16 chunkDims, cVEC3Ds16 chunkCounts, cui8 cam) {
      data[cam].fDims[0].vector = { fl32(chunkDims.x), fl32(chunkDims.y), fl32(chunkDims.z), 1.0f };
      data[cam].fDims[1].vector = { fl32(chunkCounts.x), fl32(chunkCounts.y), fl32(chunkCounts.z), 1.0f };
   }

   inline void SetDimsf(cVEC4Ds16 chunkDims, cVEC4Ds16 chunkCounts, cui8 cam) {
      data[cam].fDims[0].vector = { fl32(chunkDims.x), fl32(chunkDims.y), fl32(chunkDims.z), fl32(chunkDims.w) };
      data[cam].fDims[1].vector = { fl32(chunkCounts.x), fl32(chunkCounts.y), fl32(chunkCounts.z), fl32(chunkDims.w) };
   }

   inline void SetCamera(cui8 cam) { currentCamProj.x = cam; }

   inline void SetProjection(cui8 proj) { currentCamProj.y = proj; }

   inline void SetCameraAndProjection(cui8 cam, cui8 proj) { currentCamProj = { cam, proj }; }

   void CreateCamera(cui8 cam, cui8 stage, cui8 slot, cbool setAsCurrent) {
      bufferIndex[cam][0] = buf->CreateConstant(0, &cbProj[cam], sizeof(CB_VIEW), 1, slot, stage, false);
      bufferIndex[cam][1] = buf->CreateConstant(0, &cbMain[cam], sizeof(CB_MAIN), 1, slot + 1, stage, false);

      bufferSlot[cam] = slot;

      data[cam].fZoom   = 1.0f;
      data[cam].fSize   = 256.0f;
      data[cam].fWidth  = 1.0f;
      data[cam].fHeight = 1.0f;
      data[cam].fNearZ  = 1.0f;
      data[cam].fFarZ   = 16384.0f;

      if(setAsCurrent) currentCamProj.x = cam;
   }

   inline void LoadProjection(cmatrix mView, cui8 proj) { mProj[proj] = mView; }

   inline void LoadCamera(cmatrix mView, cui8 cam) { mCamera[cam] = mView; }

   inline void SetCameraPos(cfl32 x, cfl32 y, cfl32 z, cui8 cam) { data[cam].fXpos = x; data[cam].fYpos = y; data[cam].fZpos = z; }
   inline void SetCameraPos(cfl64 x, cfl64 y, cfl64 z, cui8 cam) { data[cam].dXpos = x; data[cam].dYpos = y; data[cam].dZpos = z; }

   inline void SetCameraRot(cfl32 x, cfl32 y, cfl32 z, cui8 cam) { data[cam].fXrot = x; data[cam].fYrot = y; data[cam].fZrot = z; }
   inline void SetCameraRot(cfl64 x, cfl64 y, cfl64 z, cui8 cam) { data[cam].dXrot = x; data[cam].dYrot = y; data[cam].dZrot = z; }

   inline void MoveCameraUpY(cfl32 distance, cui8 cam) { data[cam].fYpos += distance; }
   inline void MoveCameraUpY(cfl64 distance, cui8 cam) { data[cam].dYpos += distance; }

   inline void MoveCameraRightXZ(cfl32 distance, cui8 cam) {
      data[cam].fXpos += cosf(data[cam].fYrot) * distance;
      data[cam].fZpos -= sinf(data[cam].fYrot) * distance;
   }
   inline void MoveCameraRightXZ(cfl64 distance, cui8 cam) {
      data[cam].dXpos += cos(data[cam].dYrot) * distance;
      data[cam].dZpos -= sin(data[cam].dYrot) * distance;
   }

   inline void MoveCameraForwardXZ(cfl32 distance, cui8 cam) {
      data[cam].fXpos += sinf(data[cam].fYrot) * distance;
      data[cam].fZpos += cosf(data[cam].fYrot) * distance;
   }
   inline void MoveCameraForwardXZ(cfl64 distance, cui8 cam) {
      data[cam].dXpos += sin(data[cam].dYrot) * distance;
      data[cam].dZpos += cos(data[cam].dYrot) * distance;
   }

   inline void MoveCamera2DVectorXZ(cVEC2Df distance, cui8 cam) {
      //cfl32 fXZsq = distance.x * distance.x + distance.y * distance.y;
      if(!(distance.x + distance.y)) return;
      cfl32 fSinY = sinf(data[cam].fYrot);
      cfl32 fCosY = cosf(data[cam].fYrot);
      data[cam].fXpos += fSinY * distance.y + fCosY * distance.x;
      data[cam].fZpos += fCosY * distance.y - fSinY * distance.x;
   }
   inline void MoveCamera2DVectorXZ(cVEC2Dd distance, cui8 cam) {
      //cfl64 dXZsq = distance.x * distance.x + distance.y * distance.y;
      if(!(distance.x + distance.y)) return;
      cfl64 dSinY = sin(data[cam].dYrot);
      cfl64 dCosY = cos(data[cam].dYrot);
      data[cam].dXpos += dCosY * distance.x + dSinY * distance.y;
      data[cam].dZpos += dCosY * distance.y - dSinY * distance.x;
   }

   // Projections 2 & 3 use square aspect ratio
   inline void TransformProjections(cui8 cam) {
      mProj[0] = (aemtrx)DX::XMMatrixPerspectiveLH(data[cam].fZoom * data[cam].fWidth, data[cam].fZoom * data[cam].fHeight, data[cam].fNearZ, data[cam].fFarZ);
      mProj[1] = (aemtrx)DX::XMMatrixOrthographicLH(data[cam].fSize * ScrRes.dims[ScrRes.state].aspectI, data[cam].fSize, data[cam].fNearZ, data[cam].fFarZ);
      mProj[2] = (aemtrx)DX::XMMatrixPerspectiveLH(data[cam].fZoom, data[cam].fZoom, data[cam].fNearZ, data[cam].fFarZ);
      mProj[3] = (aemtrx)DX::XMMatrixOrthographicLH(data[cam].fSize, data[cam].fSize, data[cam].fNearZ, data[cam].fFarZ);
   }

   inline void TransformCamera(cui8 cam, cui8 proj) {
      cfl32x4 vCamPos    = DX::XMVectorSet(data[cam].fXpos, data[cam].fYpos, data[cam].fZpos, 0.0f);
      cmatrix mCamRot    = (aemtrx)DX::XMMatrixRotationRollPitchYaw(data[cam].fXrot, data[cam].fYrot, -data[cam].fZrot);
      cfl32x4 vCamRotPos = DX::XMVector3Transform(vCamPos, (dxmtrx)mCamRot);

      vCamUp  = DX::XMVector3TransformNormal(vUp, (dxmtrx)mCamRot);
      vCamDir = DX::XMVector3TransformNormal(vForward, (dxmtrx)mCamRot);

      cmatrix mLookAtFromOrigin    = (aemtrx)DX::XMMatrixLookAtLH(vOrigin, vCamDir, vCamUp);
      cfl32x4 vCamRotPosFromOrigin = DX::XMVector3Transform(vCamRotPos, (dxmtrx)mLookAtFromOrigin);

      mCamera[cam]        = (aemtrx)DX::XMMatrixLookToLH(vCamRotPosFromOrigin, vCamDir, vCamUp);
      mInverseCamera[cam] = (aemtrx)DX::XMMatrixInverse(NULL, (dxmtrx)mCamera[cam]);
      mProjCamera[cam]    = (aemtrx)DX::XMMatrixMultiply((dxmtrx)mCamera[cam], (dxmtrx)mProj[proj]);

      cmatrix mCamProj = mProjCamera[cam];

      cAVX8Df32 *mFrustum = (cAVX8Df32 *)&mCamProj;

      cAVX8Df32 m[2] = { { ._fl = { mFrustum[0].vector.x.x, mFrustum[0].vector.y.x, mFrustum[1].vector.x.x, mFrustum[1].vector.y.x,
                                    mFrustum[0].vector.x.y, mFrustum[0].vector.y.y, mFrustum[1].vector.x.y, mFrustum[1].vector.y.y } },
                         { ._fl = { mFrustum[0].vector.x.z, mFrustum[0].vector.y.z, mFrustum[1].vector.x.z, mFrustum[1].vector.y.z,
                                    mFrustum[0].vector.x.w, mFrustum[0].vector.y.w, mFrustum[1].vector.x.w, mFrustum[1].vector.y.w } } };

      data[cam].frustum.fLeft   = { .xmm = _mm_add_ps(m[1].xmm1, m[0].xmm0) };
      data[cam].frustum.fBottom = { .xmm = _mm_add_ps(m[1].xmm1, m[0].xmm1) };
      data[cam].frustum.fRight  = { .xmm = _mm_sub_ps(m[1].xmm1, m[0].xmm0) };
      data[cam].frustum.fTop    = { .xmm = _mm_sub_ps(m[1].xmm1, m[0].xmm1) };
      data[cam].frustum.fNear   = { .xmm = _mm_add_ps(m[1].xmm1, m[1].xmm0) };
      data[cam].frustum.fFar    = { .xmm = _mm_sub_ps(m[1].xmm1, m[1].xmm0) };

      for(ui8 i = 0; i < 6; i++) normaliseV4(data[cam].frustum.fPlane[i].xmm);
   }

   inline void UploadProjections(cui64 bits, cui8 cam) {
      cbProj[cam] = { (aemtrx)DX::XMMatrixTranspose((dxmtrx)mProj[0]), (aemtrx)DX::XMMatrixTranspose((dxmtrx)mProj[1]), { ScrRes.dims[ScrRes.state].aspect, 1.0f }, bits };

      buf->UpdateConstant(0, &cbProj[cam], bufferIndex[cam][0], 1);
   }

   inline void UploadCamera(cui32 frameCount, cfl32 frameTime, cfl32 secsDelta, cui32 secsTotal, cui8 bits, cui8 cam) {
      cbMain[cam] = { (aemtrx)DX::XMMatrixTranspose((dxmtrx)mCamera[cam]), frameCount, frameTime, secsDelta, (secsTotal & 0x0FFFFFF) | (bits << 24) };

      buf->UpdateConstant(0, &cbMain[cam], bufferIndex[cam][1], 1);
   }

   inline void UploadCamera(cfl64 totalTime, cfl32 frameTime, cui32 frameCount, cui8 bits, cui8 cam) {
      cfl64 dTotalSecs  = trunc(totalTime);
      cfl32 fDeltaSecs  = cfl32(totalTime - dTotalSecs);
      cui32 uiTotalSecs = ui32(dTotalSecs) & 0x0FFFFFF;

      cbMain[cam] = { (aemtrx)DX::XMMatrixTranspose((dxmtrx)mCamera[cam]), frameCount, frameTime, fDeltaSecs, uiTotalSecs | (bits << 24) };

      buf->UpdateConstant(0, &cbMain[cam], bufferIndex[cam][1], 1);
   }

   inline cfl32 DistanceFromCamera(csi128 target, cui8 cam) {
      cfl32x4 fTarget = _mm_cvtepi32_ps(target);

      //cfl32x4 diff = { fTarget.m128_f32[0] - data[cam].fXpos, fTarget.m128_f32[1] - data[cam].fYpos, fTarget.m128_f32[2] - data[cam].fZpos };
      cfl32x4 diff = _mm_sub_ps(fTarget, data[cam].pos32.xmm);

      //return sqrtf(diff.m128_f32[0] * diff.m128_f32[0] + diff.m128_f32[1] * diff.m128_f32[1] + diff.m128_f32[2] * diff.m128_f32[2]);
      cfl32x4 square = _mm_mul_ps(diff, diff);
      return sqrtf(square.m128_f32[0] + square.m128_f32[1] + square.m128_f32[2]);
   }

   inline cfl32 DistanceFromCamera(cfl32x4 target, cui8 cam) {
      //cfl32x4 diff = { target.m128_f32[0] - data[cam].fXpos, target.m128_f32[1] - data[cam].fYpos, target.m128_f32[2] - data[cam].fZpos };
      cfl32x4 diff = _mm_sub_ps(target, data[cam].pos32.xmm);

      //return sqrtf(diff.m128_f32[0] * diff.m128_f32[0] + diff.m128_f32[1] * diff.m128_f32[1] + diff.m128_f32[2] * diff.m128_f32[2]);
      cfl32x4 square = _mm_mul_ps(diff, diff);
      return sqrtf(square.m128_f32[0] + square.m128_f32[1] + square.m128_f32[2]);
   }

   inline cfl32 RaySphereIntersect(vector rayOri, vector rayDir, fl32 radius) {
      // Calculate the a, b, and c coefficients
      cfl32 a = _mm_dp_ps(rayDir, rayDir, 0x071).m128_f32[0];
      cfl32 b = _mm_dp_ps(rayDir, rayOri, 0x071).m128_f32[0];
      cfl32 c = _mm_dp_ps(rayOri, rayOri, 0x071).m128_f32[0] - (radius * radius);
      // Returns discriminant; if result is negative the picking ray missed the sphere
      return b - (a * c);
   }

   inline fl32 RayPlaneIntersect(cfl32x4 rayOri, cfl32x4 rayDir, cfl32x4 planeOri, cfl32x4 planeDir) {
      cfl32x4 vDenom = DX::XMVector3Dot(planeDir, rayDir);

      if(vDenom.m128_f32[0] != 0.0f) {
         cfl32x4 vDiff = DX::XMVectorSubtract(planeOri, rayOri);
         cfl32x4 vT    = DX::XMVector3Dot(vDiff, planeDir);
         cfl32   fT    = vT.m128_f32[0] / vDenom.m128_f32[0];

         if(fT > 0.0f) return fT;
      }
      return vDenom.m128_f32[0];
   }

   // Return values are 0x080000001 if target is beyond horizon
   inline VEC2Ds32 RayLayerIntersect(cfl32x4 rayOri, cfl32x4 rayDir, si32 layer) {
      cfl32x4 planeDir = { 0.0f, 0.0f, 1.0f };
      cfl32x4 planeOri = { 0.0f, 0.0f, fl32(layer)};
      cfl32x4 vDenom   = DX::XMVector3Dot(planeDir, rayDir);

      if(vDenom.m128_f32[0] != 0.0f) {
         cfl32x4 vDiff = DX::XMVectorSubtract(planeOri, rayOri);
         cfl32x4 vT    = DX::XMVector3Dot(vDiff, planeDir);
         cfl32   fT    = vT.m128_f32[0] / vDenom.m128_f32[0];

         if(fT > 0.0f) {
            //--- Calculate 2D coordinate
            cVEC2Df   fStep = { rayDir.m128_f32[0] * fT, rayDir.m128_f32[1] * fT };
            cVEC2Ds32 vCell = { si32(floorf(rayOri.m128_f32[0] + fStep.x)), si32(floorf(rayOri.m128_f32[1] + fStep.y)) };

            return vCell;
         }
      }
      return { (si32)0x080000001, (si32)0x080000001 };
   }

   inline cfl32 CursorSphereIntersect(cfl32x4 &sphere, cVEC2Ds32 curPos, cVEC2Du8 camProj) const {
      cmatrix mInvertedCam = mInverseCamera[camProj.x];
      cVEC2Df vScreen      = { (curPos.x / ScrRes.dims[ScrRes.state].w) * 2.0f - 1.0f, 1.0f - (curPos.y / ScrRes.dims[ScrRes.state].h) * 2.0f };
      cVEC2Df vProjected   = { vScreen.x / mProj[camProj.y].fl[0], vScreen.y / mProj[camProj.y].fl[5]};

      cAVX8Df32 projCOs = { .xmm = { _mm_set_ps1(vProjected.x), _mm_set_ps1(vProjected.y) } };
      cAVX8Df32 rayDir_ = { .ymm = _mm256_mul_ps(projCOs.ymm, mInvertedCam.ymm[0])};
      cSSE4Df32 rayDir  = { .xmm = _mm_add_ps(_mm_add_ps(rayDir_.xmm0, rayDir_.xmm1), mInvertedCam.xmm[2])};

      cfl32x4 vRayOri = _mm_sub_ps(data[camProj.x].pos32.xmm, sphere);
      cfl32x4 vRayDir = DX::XMVector3Normalize(rayDir.xmm);

      // Calculate the a, b, and c coefficients
      cfl32 a = _mm_dp_ps(vRayDir, vRayDir, 0x071).m128_f32[0];
      cfl32 b = _mm_dp_ps(vRayDir, vRayOri, 0x071).m128_f32[0];
      cfl32 c = _mm_dp_ps(vRayOri, vRayOri, 0x071).m128_f32[0] - (sphere.m128_f32[3] * sphere.m128_f32[3]);
      // Returns discriminant; if result is negative the picking ray missed the sphere
      return b - (a * c);
   }

   // First return values are 0x080000001 if target is beyond horizon
   inline cVEC2Ds32 CursorLayerIntersect(csi32 layer, cfl32x4 camOffset, cVEC2Ds32 curPos, cui8 cam, cui8 proj) const {
      cmatrix mInvertedCam = mInverseCamera[cam];
      cVEC2Df vScreen      = { (curPos.x / ScrRes.dims[ScrRes.state].w) * 2.0f - 1.0f, 1.0f - (curPos.y / ScrRes.dims[ScrRes.state].h) * 2.0f };
      cVEC2Df vProjected   = { vScreen.x / mProj[proj].fl[0], vScreen.y / mProj[proj].fl[5] };

      cAVX8Df32 projCOs = { .xmm = { _mm_set_ps1(vProjected.x), _mm_set_ps1(vProjected.y) } };
      cAVX8Df32 rayDir_ = { .ymm = _mm256_mul_ps(projCOs.ymm, mInvertedCam.ymm[0]) };
      cSSE4Df32 rayDir  = { .xmm = _mm_add_ps(_mm_add_ps(rayDir_.xmm0, rayDir_.xmm1), mInvertedCam.xmm[2]) };

      cfl32x4 vRayOri = _mm_add_ps(data[cam].pos32.xmm, camOffset);
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
      return { (si32)0x080000001 };
   }

   // First return value is (quiet) NaN if target is beyond horizon
   inline cVEC3Df CursorLayerIntersect(csi32 layer, cVEC2Ds32 curPos, cVEC2Du8 camProj) const {
      cmatrix mInvertedCam = mInverseCamera[camProj.x];
      cVEC2Df vScreen      = { (curPos.x / ScrRes.dims[ScrRes.state].w) * 2.0f - 1.0f, 1.0f - (curPos.y / ScrRes.dims[ScrRes.state].h) * 2.0f };
      cVEC2Df vProjected   = { vScreen.x / mProj[camProj.y].fl[0], vScreen.y / mProj[camProj.y].fl[5] };

      cAVX8Df32 projCOs = { .xmm = { _mm_set_ps1(vProjected.x), _mm_set_ps1(vProjected.y) } };
      cAVX8Df32 rayDir_ = { .ymm = _mm256_mul_ps(projCOs.ymm, mInvertedCam.ymm[0]) };
      cSSE4Df32 rayDir  = { .xmm = _mm_add_ps(_mm_add_ps(rayDir_.xmm0, rayDir_.xmm1), mInvertedCam.xmm[2]) };

//      cfl32x4 vRayOri = _mm_add_ps(data[camProj.x].pos32.xmm, camOffset);
      cfl32x4 vRayOri = data[camProj.x].pos32.xmm;
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

      static cui32 uiNaN = 0x0EFAAAAAA;

      return { (cfl32 &)uiNaN };
   }

   // Return value is false if target is beyond horizon
   inline cbool CursorLayerIntersect(SSE4Df32 &location, cVEC2Ds32 curPos, cVEC2Du8 camProj) const {
      cmatrix mInvertedCam = mInverseCamera[camProj.x];
      cVEC2Df vScreen      = { (curPos.x / ScrRes.dims[ScrRes.state].w) * 2.0f - 1.0f, 1.0f - (curPos.y / ScrRes.dims[ScrRes.state].h) * 2.0f };
      cVEC2Df vProjected   = { vScreen.x / mProj[camProj.y].fl[0], vScreen.y / mProj[camProj.y].fl[5] };

      cAVX8Df32 projCOs = { .xmm = { _mm_set_ps1(vProjected.x), _mm_set_ps1(vProjected.y) } };
      cAVX8Df32 rayDir_ = { .ymm = _mm256_mul_ps(projCOs.ymm, mInvertedCam.ymm[0]) };
      cSSE4Df32 rayDir  = { .xmm = _mm_add_ps(_mm_add_ps(rayDir_.xmm0, rayDir_.xmm1), mInvertedCam.xmm[2]) };

      //      cfl32x4 vRayOri = _mm_add_ps(data[camProj.x].pos32.xmm, camOffset);
      cfl32x4 vRayOri = data[camProj.x].pos32.xmm;
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
         if(_mm_dp_ps(spheres.xmm0, data[cam].frustum.fPlane[i].xmm, 0x0F1).m128_f32[0] < 0) { success ^= 0x01; break; }
      for (i = 0; i < 6; i++)
         if(_mm_dp_ps(spheres.xmm1, data[cam].frustum.fPlane[i].xmm, 0x0F1).m128_f32[0] < 0) { return success ^ 0x02; }

      return success;
   }

   // Each true bit in the return value == sphere visible
   inline cui8 SphereFrustumIntersect8(cSSE4Df32 (&spheres)[8], cui8 cam) {
      PLANEfl32 (&plane)[6] = data[cam].frustum.fPlane;
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
      cmatrix mViewSpace = (aemtrx)DX::XMMatrixMultiply((dxmtrx)mCamera[cam], (dxmtrx)mProj[proj]);

      cfl32 depthOS      = data[cam].fDims[1].vector.z * 0.5f;
      cfl32 boundingSize = max(max(data[cam].fDims[0].vector.x, data[cam].fDims[0].vector.y), data[cam].fDims[0].vector.z) * 0.86602540378443864676372317075294f;

      cAVX8Df32 chunkOffsets = { ._fl = { 0.5f, 0.5f, depthOS + 0.5f, 1.0f, 0.5f, 0.5f, depthOS + 0.5f, 1.0f } };
      cAVX8Df32 chunkScale   = { .xmm = { data[cam].fDims[0].xmm, data[cam].fDims[0].xmm } };
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

      cVEC4Df antiSize = { data[cam].fZoom - radius.vector.x, data[cam].fZoom - radius.vector.y,
                           data[cam].fZoom - radius.vector.z, data[cam].fZoom - radius.vector.w };

      cui8 result = (fabsf(rayOrigins.vector.x) > antiSize.x || fabsf(rayOrigins.vector.y) > antiSize.y ? 0 : 0x01) |
                    (fabsf(rayOrigins.vector.z) > antiSize.z || fabsf(rayOrigins.vector.w) > antiSize.w ? 0 : 0x02);

      return result;
   }

   // Each true bit in the return value == chunk visible
   inline cui8 ChunkFrustumIntersect2(cAVX8Ds32 chunks, cui8 cam) {
      cAVX8Df32 chunkOffsets = { ._fl = { 0.5f, 0.5f, 0.5f, 1.0f, 0.5f, 0.5f, 0.5f, 1.0f } };
      cAVX8Df32 chunkScale   = { .xmm = { data[cam].fDims[0].xmm, data[cam].fDims[0].xmm } };
      cAVX8Df32 chunkOrigins = { .ymm = _mm256_mul_ps(_mm256_add_ps(_mm256_cvtepi32_ps(chunks.ymm), chunkOffsets.ymm), chunkScale.ymm) };

      cfl32 boundingSize = Max3(chunkScale.vector.x) * -0.86602540378443864676372317075294f;

      ui8 i, success = 0x03;

      for (i = 0; i < 6; i++) {
         cfl32x4 plData = data[cam].frustum.fPlane[i].xmm;
         cfl32   side   = _mm_dp_ps(chunkOrigins.xmm0, plData, 0x0F1).m128_f32[0];
         if (side < boundingSize) {
            success &= 0x0FE;
            break;
         }
      }
      for (i = 0; i < 6; i++) {
         cfl32x4 plData = data[cam].frustum.fPlane[i].xmm;
         cfl32   side   = _mm_dp_ps(chunkOrigins.xmm1, plData, 0x0F1).m128_f32[0];
         if (side < boundingSize) {
            success &= 0x0FD;
            break;
         }
      }

      return success;
   }

   // Each true bit in the return value == chunk visible
   inline cui8 ChunkFrustumIntersect8(SSE4Ds32 chunk, cui8 cam) {
      cAVX8Df32 chunkOffsets = { ._fl = { 0.5f, 0.5f, 0.5f, 1.0f, 0.5f, 0.5f, 0.5f, 1.0f } };
      cAVX8Df32 chunkScale   = { .xmm = { data[cam].fDims[0].xmm, data[cam].fDims[0].xmm } };

      cfl32 boundingSize = Max3(chunkScale.vector.x) * -0.86602540378443864676372317075294f;

      ui8 i, j, success = 0xFF;

      for(i = 0; i < 8; i += 2, chunk.vector.x++) {
         cSSE4Ds32 chunk1       = { .vector = { chunk.vector.x++, chunk.vector.y, chunk.vector.z } };
         cAVX8Ds32 chunks       = { .xmm = { chunk1.xmm, chunk.xmm } };
         cAVX8Df32 chunkOrigins = { .ymm = _mm256_mul_ps(_mm256_add_ps(_mm256_cvtepi32_ps(chunks.ymm), chunkOffsets.ymm), chunkScale.ymm) };

         for (j = 0; j < 6; j++) {
            cfl32x4 plData = data[cam].frustum.fPlane[j].xmm;
            cfl32   side   = _mm_dp_ps(chunkOrigins.xmm0, plData, 0x0F1).m128_f32[0];
            if (side < boundingSize) {
               success ^= 0x01 << i;
               break;
            }
         }
         for (j = 0; j < 6; j++) {
            cfl32x4 plData = data[cam].frustum.fPlane[j].xmm;
            cfl32   side   = _mm_dp_ps(chunkOrigins.xmm1, plData, 0x0F1).m128_f32[0];
            if (side < boundingSize) {
               success ^= 0x02 << i;
               break;
            }
         }
      }

      return success;
   }
};
