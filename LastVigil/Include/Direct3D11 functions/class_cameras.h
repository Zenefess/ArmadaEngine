/************************************************************
 * File: class_camera.h                 Created: 2022/10/20 *
 *                                Last modified: 2024/04/25 *
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

   fl32x4 vCamDir {}, vCamUp {};

   CAMERA_DATA data[8] {};

   CB_VIEW cbProj[8] {};
   CB_MAIN cbMain[8] {};
   matrix  mCamera[8] {}, mInverseCamera[8] {}, mPerspCamera[8] {}, mProjCamera[8] {}, mProj[4] {};
   cfl32x4 vOrigin  = DX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
   cfl32x4 vUp      = DX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
   cfl32x4 vForward = DX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
   si32    bufferIndex[8][2] {}, bufferSlot[8] = {};

   CLASS_CAM(void) { ptrLib[5] = this; }

   void SetDims(cVEC3Ds16 dims, ui8 cam) { data[cam].iDims[0].vector = { dims.x, dims.y, dims.z, 1 }; }

   void SetDims(cVEC4Ds16 dims, ui8 cam) { data[cam].iDims[0].vector = { dims.x, dims.y, dims.z, dims.w }; }

   void SetDimsf(cVEC3Ds16 dims, ui8 cam) { data[cam].fDims[0].vector = { float(dims.x), float(dims.y), float(dims.z), 1.0f }; }

   void SetDimsf(cVEC4Ds16 dims, ui8 cam) { data[cam].fDims[0].vector = { float(dims.x), float(dims.y), float(dims.z), float(dims.w) }; }

   void SetDimsf(cVEC3Ds16 chunkDims, cVEC3Ds16 chunkCounts, ui8 cam) {
      data[cam].fDims[0].vector = { float(chunkDims.x), float(chunkDims.y), float(chunkDims.z), 1.0f };
      data[cam].fDims[1].vector = { float(chunkCounts.x), float(chunkCounts.y), float(chunkCounts.z), 1.0f };
   }

   void SetDimsf(cVEC4Ds16 chunkDims, cVEC4Ds16 chunkCounts, ui8 cam) {
      data[cam].fDims[0].vector = { float(chunkDims.x), float(chunkDims.y), float(chunkDims.z), float(chunkDims.w) };
      data[cam].fDims[1].vector = { float(chunkCounts.x), float(chunkCounts.y), float(chunkCounts.z), float(chunkDims.w) };
   }

   void CreateCamera(ui8 cam, ui8 stage, ui8 slot) {
      bufferIndex[cam][0] = buf->CreateConstant(0, &cbProj[cam], sizeof(CB_VIEW), 1, slot, stage, false);
      bufferIndex[cam][1] = buf->CreateConstant(0, &cbMain[cam], sizeof(CB_MAIN), 1, slot + 1, stage, false);
      bufferSlot[cam] = slot;
      data[cam].fZoom   = 1.0f;
      data[cam].fSize   = 256.0f;
      data[cam].fWidth  = 1.0f;
      data[cam].fHeight = 1.0f;
      data[cam].fNearZ  = 1.0f;
      data[cam].fFarZ   = 16384.0f;
   }

   inline void LoadProjection(matrix mView, ui8 proj) { mProj[proj] = mView; }

   inline void LoadCamera(matrix mView, ui8 cam) { mCamera[cam] = mView; }

   inline void SetCameraPos(float x, float y, float z, ui8 cam) { data[cam].fXpos = x; data[cam].fYpos = y; data[cam].fZpos = z; }
   inline void SetCameraPos(double x, double y, double z, ui8 cam) { data[cam].dXpos = x; data[cam].dYpos = y; data[cam].dZpos = z; }

   inline void SetCameraRot(float x, float y, float z, ui8 cam) { data[cam].fXrot = x; data[cam].fYrot = y; data[cam].fZrot = z; }
   inline void SetCameraRot(double x, double y, double z, ui8 cam) { data[cam].dXrot = x; data[cam].dYrot = y; data[cam].dZrot = z; }

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

   inline void MoveCamera2DVectorXZ(VEC2Df distance, cui8 cam) {
      //cfl32 fXZsq = distance.x * distance.x + distance.y * distance.y;
      if(!(distance.x + distance.y)) return;
      cfl32 fSinY = sinf(data[cam].fYrot);
      cfl32 fCosY = cosf(data[cam].fYrot);
      data[cam].fXpos += fSinY * distance.y + fCosY * distance.x;
      data[cam].fZpos += fCosY * distance.y - fSinY * distance.x;
   }
   inline void MoveCamera2DVectorXZ(VEC2Dd distance, cui8 cam) {
      //cfl64 dXZsq = distance.x * distance.x + distance.y * distance.y;
      if(!(distance.x + distance.y)) return;
      cfl64 dSinY = sin(data[cam].dYrot);
      cfl64 dCosY = cos(data[cam].dYrot);
      data[cam].dXpos += dCosY * distance.x + dSinY * distance.y;
      data[cam].dZpos += dCosY * distance.y - dSinY * distance.x;
   }

   // Projections 2 & 3 use square aspect ratio
   inline void TransformProjections(cui8 cam) {
      mProj[0] = DX::XMMatrixPerspectiveLH(data[cam].fZoom * data[cam].fWidth, data[cam].fZoom * data[cam].fHeight, data[cam].fNearZ, data[cam].fFarZ);
      mProj[1] = DX::XMMatrixOrthographicLH(data[cam].fSize * ScrRes.dims[ScrRes.state].aspectI, data[cam].fSize, data[cam].fNearZ, data[cam].fFarZ);
      mProj[2] = DX::XMMatrixPerspectiveLH(data[cam].fZoom, data[cam].fZoom, data[cam].fNearZ, data[cam].fFarZ);
      mProj[3] = DX::XMMatrixOrthographicLH(data[cam].fSize, data[cam].fSize, data[cam].fNearZ, data[cam].fFarZ);
   }

   inline void TransformCamera(cui8 cam, cui8 proj) {
      cfl32x4 vCamPos    = DX::XMVectorSet(data[cam].fXpos, data[cam].fYpos, data[cam].fZpos, 0.0f);
      cmatrix mCamRot    = DX::XMMatrixRotationRollPitchYaw(data[cam].fXrot, data[cam].fYrot, -data[cam].fZrot);
      cfl32x4 vCamRotPos = DX::XMVector3Transform(vCamPos, mCamRot);

      vCamUp  = DX::XMVector3TransformNormal(vUp, mCamRot);
      vCamDir = DX::XMVector3TransformNormal(vForward, mCamRot);

      cmatrix mLookAtFromOrigin    = DX::XMMatrixLookAtLH(vOrigin, vCamDir, vCamUp);
      cfl32x4 vCamRotPosFromOrigin = DX::XMVector3Transform(vCamRotPos, mLookAtFromOrigin);

      mCamera[cam]        = DX::XMMatrixLookToLH(vCamRotPosFromOrigin, vCamDir, vCamUp);
      mInverseCamera[cam] = DX::XMMatrixInverse(NULL, mCamera[cam]);
      mProjCamera[cam]    = DX::XMMatrixMultiply(mCamera[cam], mProj[proj]);

      cmatrix mCamProj = mProjCamera[cam];

      cAVX8Df32 *mFrustum = (cAVX8Df32 *)&mCamProj;

      cAVX8Df32 m[2] = { { .vector = { { mFrustum[0].vector0.x, mFrustum[0].vector1.x, mFrustum[1].vector0.x, mFrustum[1].vector1.x },
                                       { mFrustum[0].vector0.y, mFrustum[0].vector1.y, mFrustum[1].vector0.y, mFrustum[1].vector1.y } } },
                         { .vector = { { mFrustum[0].vector0.z, mFrustum[0].vector1.z, mFrustum[1].vector0.z, mFrustum[1].vector1.z },
                                       { mFrustum[0].vector0.w, mFrustum[0].vector1.w, mFrustum[1].vector0.w, mFrustum[1].vector1.w } } } };

      data[cam].frustum.fLeft   = { .xmm = _mm_add_ps(m[1].xmm1, m[0].xmm0) };
      data[cam].frustum.fBottom = { .xmm = _mm_add_ps(m[1].xmm1, m[0].xmm1) };
      data[cam].frustum.fRight  = { .xmm = _mm_sub_ps(m[1].xmm1, m[0].xmm0) };
      data[cam].frustum.fTop    = { .xmm = _mm_sub_ps(m[1].xmm1, m[0].xmm1) };
      data[cam].frustum.fNear   = { .xmm = _mm_add_ps(m[1].xmm1, m[1].xmm0) };
      data[cam].frustum.fFar    = { .xmm = _mm_sub_ps(m[1].xmm1, m[1].xmm0) };

      for(ui8 i = 0; i < 6; i++) {
         cfl32 magRCP = 1.0f / V3Magnitute(data[cam].frustum.fPlane[i].xmm);
         V3NormaliseR(data[cam].frustum.fPlane[i].xmm, magRCP);
         data[cam].frustum.fPlane[i].length *= magRCP;
      }
   }

   inline void UploadProjections(cui64 bits, cui8 cam) {
      cbProj[cam] = { DX::XMMatrixTranspose(mProj[0]), DX::XMMatrixTranspose(mProj[1]), { ScrRes.dims[ScrRes.state].aspect, 1.0f }, bits };

      buf->UpdateConstant(0, &cbProj[cam], bufferIndex[cam][0], 1);
   }

   inline void UploadCamera(cui32 frameCount, cfl32 frameTime, cfl32 secsDelta, cui32 secsTotal, cui8 bits, cui8 cam) {
      cbMain[cam] = { DX::XMMatrixTranspose(mCamera[cam]), frameCount, frameTime, secsDelta, (secsTotal & 0x0FFFFFF) | (bits << 24) };

      buf->UpdateConstant(0, &cbMain[cam], bufferIndex[cam][1], 1);
   }

   inline void UploadCamera(cfl64 totalTime, cfl32 frameTime, cui32 frameCount, cui8 bits, cui8 cam) {
      cfl64 dTotalSecs  = trunc(totalTime);
      cfl32  fDeltaSecs  = cfl32(totalTime - dTotalSecs);
      cui32   uiTotalSecs = ui32(dTotalSecs) & 0x0FFFFFF;

      cbMain[cam] = { DX::XMMatrixTranspose(mCamera[cam]), frameCount, frameTime, fDeltaSecs, uiTotalSecs | (bits << 24) };

      buf->UpdateConstant(0, &cbMain[cam], bufferIndex[cam][1], 1);
   }

   inline cfl32 DistanceFromCamera(csi128 target, cui8 cam) {
      cfl32x4 fTarget = _mm_cvtepi32_ps(target);

      cfl32x4 diff = { fTarget.m128_f32[0] - data[cam].fXpos, fTarget.m128_f32[0] - data[cam].fYpos, fTarget.m128_f32[0] - data[cam].fZpos };

      return sqrtf(diff.m128_f32[0] * diff.m128_f32[0] + diff.m128_f32[1] * diff.m128_f32[1] + diff.m128_f32[1] * diff.m128_f32[1]);
   }

   inline cfl32 DistanceFromCamera(cfl32x4 target, cui8 cam) {
      cfl32x4 diff = { target.m128_f32[0] - data[cam].fXpos, target.m128_f32[0] - data[cam].fYpos, target.m128_f32[0] - data[cam].fZpos };

      return sqrtf(diff.m128_f32[0] * diff.m128_f32[0] + diff.m128_f32[1] * diff.m128_f32[1] + diff.m128_f32[1] * diff.m128_f32[1]);
   }

   inline cfl32 RaySphereIntersect(vector rayOri, vector rayDir, float radius) {
      // Calculate the a, b, and c coefficients
      cfl32 a = _mm_dp_ps(rayDir, rayDir, 0x071).m128_f32[0];
      cfl32 b = _mm_dp_ps(rayDir, rayOri, 0x071).m128_f32[0];
      cfl32 c = _mm_dp_ps(rayOri, rayOri, 0x071).m128_f32[0] - (radius * radius);
      // If result is negative the picking ray missed the sphere, otherwise return discriminant
      return b - (a * c);
   }

   inline float RayPlaneIntersect(cfl32x4 rayOri, cfl32x4 rayDir, cfl32x4 planeOri, cfl32x4 planeDir) {
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
      cfl32x4 planeOri = { 0.0f, 0.0f, float(layer)};
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

   // Return values are 0x080000001 if target is beyond horizon
   inline cVEC2Ds32 CursorLayerIntersect(csi32 layer, cfl32x4 camOffset, cVEC2Ds32 curPos, cui8 cam, cui8 proj) const {
      cmatrix mInvertedCam = mInverseCamera[cam];
      cVEC2Df vScreen      = { (curPos.x / ScrRes.dims[ScrRes.state].w) * 2.0f - 1.0f, 1.0f - (curPos.y / ScrRes.dims[ScrRes.state].h) * 2.0f };
      cVEC2Df vProjected   = { vScreen.x / mProj[proj].r[0].m128_f32[0], vScreen.y / mProj[proj].r[1].m128_f32[1] };

      cAVX8Df32 projCOs = { .xmm = { _mm_set_ps1(vProjected.x), _mm_set_ps1(vProjected.y) } };
      cAVX8Df32 rayDir_ = { .ymm = _mm256_mul_ps(projCOs.ymm, (__m256 &)mInvertedCam.r[0]) };
      cSSE4Df32 rayDir  = { .xmm = _mm_add_ps(_mm_add_ps(rayDir_.xmm0, rayDir_.xmm1), mInvertedCam.r[2]) };

      cfl32x4 vRayOri = _mm_add_ps(data[cam].pos32.xmm, camOffset);
      cfl32x4 vRayDir = DX::XMVector3Normalize(rayDir.xmm);

      cfl32x4 planeDir = { 0.0f, 0.0f, 1.0f };
      cfl32x4 planeOri = { 0.0f, 0.0f, float(layer)};

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
      return { (si32)0x080000001, (si32)0x080000001 };
   }

   // First return value is (quiet) NaN if target is beyond horizon
//   inline void CursorLayerIntersect(VEC3Ds32 &cell, cfl32x4 camOffset, cVEC2Ds32 curPos, cVEC2Du8 camProj) const {
   inline cVEC3Df CursorLayerIntersect(csi32 layer, cVEC2Ds32 curPos, cVEC2Du8 camProj) const {
      cmatrix mInvertedCam = mInverseCamera[camProj.x];
      cVEC2Df vScreen      = { (curPos.x / ScrRes.dims[ScrRes.state].w) * 2.0f - 1.0f, 1.0f - (curPos.y / ScrRes.dims[ScrRes.state].h) * 2.0f };
      cVEC2Df vProjected   = { vScreen.x / mProj[camProj.y].r[0].m128_f32[0], vScreen.y / mProj[camProj.y].r[1].m128_f32[1] };

      cAVX8Df32 projCOs = { .xmm = { _mm_set_ps1(vProjected.x), _mm_set_ps1(vProjected.y) } };
      cAVX8Df32 rayDir_ = { .ymm = _mm256_mul_ps(projCOs.ymm, (__m256 &)mInvertedCam.r[0]) };
      cSSE4Df32 rayDir  = { .xmm = _mm_add_ps(_mm_add_ps(rayDir_.xmm0, rayDir_.xmm1), mInvertedCam.r[2]) };

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

      cui32 uiNaN = 0x0EFAAAAAA;

      return { (cfl32 &)uiNaN };
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
      cmatrix mViewSpace = DX::XMMatrixMultiply(mCamera[cam], mProj[proj]);

      cfl32 depthOS      = data[cam].fDims[1].vector.z * 0.5f;
      cfl32 boundingSize = max(max(data[cam].fDims[0].vector.x, data[cam].fDims[0].vector.y), data[cam].fDims[0].vector.z) * 0.86602540378443864676372317075294f;

      cAVX8Df32 chunkOffsets = { .vector = { { 0.5f, 0.5f, depthOS + 0.5f, 1.0f }, { 0.5f, 0.5f, depthOS + 0.5f, 1.0f } } };
      cAVX8Df32 chunkScale   = { .xmm = { data[cam].fDims[0].xmm, data[cam].fDims[0].xmm } };
      cAVX8Df32 chunkOrigins = { .ymm = _mm256_mul_ps(_mm256_add_ps(_mm256_cvtepi32_ps(chunks.ymm), chunkOffsets.ymm), chunkScale.ymm) };

      cAVX8Df32 rad = { .vector = { { boundingSize, boundingSize, chunkOrigins.vector0.z, chunkOrigins.vector0.w },
                                    { boundingSize, boundingSize, chunkOrigins.vector1.z, chunkOrigins.vector1.w } } };

      cAVX8Df32 rayToCam = { .xmm = { DX::XMVector3Transform(chunkOrigins.xmm0, mViewSpace), DX::XMVector3Transform(chunkOrigins.xmm1, mViewSpace) } };
      cAVX8Df32 radProj  = { .xmm = { DX::XMVector3Transform(rad.xmm0, mViewSpace), DX::XMVector3Transform(rad.xmm1, mViewSpace) } };

      cVEC2Df   rcpDepth   = { 1.0f / rayToCam.vector0.w, 1.0f / rayToCam.vector1.w };
      cSSE4Df32 rcpDepths  = { .xmm = (__m128 &)_mm_set1_epi64x((cui64 &)rcpDepth) };
      cSSE4Df32 rayXYs     = { .vector = { rayToCam.vector0.x, rayToCam.vector0.y, rayToCam.vector1.x, rayToCam.vector1.y } };
      cSSE4Df32 rayOrigins = { .xmm = _mm_mul_ps(rayXYs.xmm, rcpDepths.xmm) };

      cSSE4Df32 radPrXYs = { .vector = { radProj.vector0.x, radProj.vector0.y, radProj.vector1.x, radProj.vector1.y } };
      cSSE4Df32 radius   = { .xmm = _mm_mul_ps(radPrXYs.xmm, rcpDepths.xmm) };

      cVEC4Df antiSize = { data[cam].fZoom - radius.vector.x, data[cam].fZoom - radius.vector.y,
                           data[cam].fZoom - radius.vector.z, data[cam].fZoom - radius.vector.w };

      cui8 result = (fabsf(rayOrigins.vector.x) > antiSize.x || fabsf(rayOrigins.vector.y) > antiSize.y ? 0 : 0x01) |
                    (fabsf(rayOrigins.vector.z) > antiSize.z || fabsf(rayOrigins.vector.w) > antiSize.w ? 0 : 0x02);
//      cui8 result = (rayOrigins.vector.x > antiSize.x || rayOrigins.vector.y > antiSize.y ? 0 : 0x01) |
//                    (rayOrigins.vector.z > antiSize.z || rayOrigins.vector.w > antiSize.w ? 0 : 0x02);
//      cui8 result = (fabsf(rayOrigins.vector.x) > data[cam].fWidth || fabsf(rayOrigins.vector.y) > data[cam].fHeight ? 0 : 0x01) |
//                    (fabsf(rayOrigins.vector.z) > data[cam].fWidth || fabsf(rayOrigins.vector.w) > data[cam].fHeight ? 0 : 0x02);
//      cui8 result = (rayOrigins.vector.x > data[cam].fWidth || rayOrigins.vector.y > data[cam].fHeight ? 0 : 0x01) |
//                    (rayOrigins.vector.z > data[cam].fWidth || rayOrigins.vector.w > data[cam].fHeight ? 0 : 0x02);

      return result;
   }

   // Each true bit in the return value == chunk visible
   inline cui8 ChunkFrustumIntersect2(cAVX8Ds32 chunks, cui8 cam) {
      cAVX8Df32 chunkOffsets = { .vector = { { 0.5f, 0.5f, 0.5f, 1.0f }, { 0.5f, 0.5f, 0.5f, 1.0f } } };
      cAVX8Df32 chunkScale   = { .xmm = { data[cam].fDims[0].xmm, data[cam].fDims[0].xmm } };
      cAVX8Df32 chunkOrigins = { .ymm = _mm256_mul_ps(_mm256_add_ps(_mm256_cvtepi32_ps(chunks.ymm), chunkOffsets.ymm), chunkScale.ymm) };

      cfl32 boundingSize = Max3(chunkScale.vector0) * -0.86602540378443864676372317075294f;

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
      cAVX8Df32 chunkOffsets = { .vector = { { 0.5f, 0.5f, 0.5f, 1.0f }, { 0.5f, 0.5f, 0.5f, 1.0f } } };
      cAVX8Df32 chunkScale   = { .xmm = { data[cam].fDims[0].xmm, data[cam].fDims[0].xmm } };

      cfl32 boundingSize = Max3(chunkScale.vector0) * -0.86602540378443864676372317075294f;

      ui8 i, j, success = 0xFF;

      for(i = 0; i < 8; i += 2, chunk.vector.x++) {
         cSSE4Ds32 chunk1       = { .vector = { chunk.vector.x++, chunk.vector.y, chunk.vector.z } };
         cAVX8Ds32 chunks       = { { .xmm = { chunk1.xmm, chunk.xmm } } };
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
/*
   // Each true bit in the return value == chunk visible
   inline cui8 Test8EntityVisibility(cui32 entityIndex, cui8 cam) {
      cfl32 boundingSize = Max(data[cam].fDims[0].vector) * -0.86602540378443864676372317075294f;

      ui8 i, j, success = 0xFF;

      for(i = 0; i < 8; i += 2, chunk.vector.x++) {
         cSSE4Ds32 chunk1       = { .vector = { chunk.vector.x++, chunk.vector.y, chunk.vector.z } };
         cAVX8Ds32 chunks       = { { .xmm = { chunk1.xmm, chunk.xmm } } };
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
   }*/
};
