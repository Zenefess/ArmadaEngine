/************************************************************
* File: class_render.h                 Created: 2023/01/16 *
*                                Last modified: 2024/07/02 *
*                                                          *
* Desc:                                                    *
*                                                          *
*  Copyright (c) David William Bull. All rights reserved.  *
************************************************************/
#pragma once

#include "..\master header.h"
#include "Direct3D11 thread.h"
#include "Direct3D11 functions\class_config.h"
#include "Direct3D11 functions\class_buffers.h"
#include "Direct3D11 functions\class_textures.h"

al32 struct CLASS_RENDER {
   CLASS_CONFIG   &cfg;
   CLASS_BUFFERS  &buf;
   CLASS_TEXTURES &tex;

   ID3D11Device         *dev;
   ID3D11DeviceContext **devcon;

   ID3D11Texture2D        *pBackBuffer[2];
   ID3D11Texture2D        *pDepthBuffer[2];
   ID3D11RenderTargetView *rtvBackBuffer;
   ID3D11DepthStencilView *pDSV;

   IDXGISwapChain2 *swapchain;

   si16 bufVertex = 0;
   ui8  msaaLevel = 0;

   // 5 byte spare

   vui16 jobCount    = 0; // Current number of jobs waiting to complete
   vui16 activeJob   = 0; // Current job being processed
   vui8  threadsBusy = 0; // Flag array of remaining status for current worker threads
   vui8  threadsMax  = 0; // Flag array of active status for current worker threads

   // 2 bytes spare

   al16 struct { VEC3Ds32 pos; fp8n0_1x4 col; } vCube {};

   CLASS_RENDER(CLASS_CONFIG &cfgClass, CLASS_BUFFERS &bufClass, CLASS_TEXTURES &texClass) : cfg(cfgClass), buf(bufClass), tex(texClass) {}

   inline void Draw(cui8 context, cui32 vertexCount) const {
      devcon[context]->Draw(vertexCount, 0);
      ++sysData.gpu.total.drawCalls;
   }

   inline void InstanceDraw(cui8 context, cui32 verticesPerInstance, cui32 instances) const {
      devcon[context]->DrawInstanced(verticesPerInstance, instances, 0, 0);
      ++sysData.gpu.total.drawCalls;
   }

   inline void DrawVoxel(cui8 context, cVEC3Ds32 &location, cfp8n0_1x4 colour) {
      vCube = { location, colour };
      cfg.SetShaderGroup(context, 5);
      cfg.SetVertexFormat(context, 3);
      buf.UpdateVertex(context, &vCube, bufVertex, 1);
      buf.SetVertexPrimitive(context, bufVertex, 0, ae_trianglestrip);
      devcon[context]->DrawInstanced(18, 1, 0, 0);
      ++sysData.gpu.total.drawCalls;
   }

   inline void ClearAndFlipOutputImage(cfl32x4 colour) const {
      devcon[0]->OMSetRenderTargets(1, &rtvBackBuffer, pDSV);
      devcon[0]->ClearRenderTargetView(rtvBackBuffer, colour.m128_f32);
      devcon[0]->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
   }

   inline void PresentOutputImage(cui8 vsyncs) const {   // Add test for vsync enabled
      Try(stSCPresent, (ScrRes.state != 2) && (msaaLevel <= 1) ? swapchain->Present(0, DXGI_PRESENT_ALLOW_TEARING) : swapchain->Present(vsyncs, 0), ss_video);
//      static const DXGI_PRESENT_PARAMETERS PP = { 0, NULL, NULL, NULL };
//      Try(stSCPresent, (ScrRes.state != 2) && (msaaLevel <= 1) ? swapchain->Present1(0, DXGI_PRESENT_ALLOW_TEARING, &PP) : swapchain->Present1(vsyncs, 0, &PP), ss_video);
      sysData.gpu.total.frames++;
      sysData.gpu.frame.prevDrawCalls = sysData.gpu.frame.curDrawCalls;
      sysData.gpu.frame.curDrawCalls  = 0;
   }

   // Add to queue and wait
   inline constexpr void AddJob(void) {
      // Wait until PresentRenderTarget completes
      while(activeJob >= 32768) _mm_pause();
      cui16 localJC = ++jobCount;
      // Wait until 
      while(activeJob && (activeJob != localJC)) _mm_pause();
      activeJob = localJC; }

   // Remove from queue
   inline constexpr void RemoveJob(void) { if(jobCount > activeJob) ++activeJob; else activeJob = 0; }

   inline void QueueDrawMap(cMAP_PARAMS &params) {
      cui8 context = params.context;
      AddJob();
      cfg.SetBlendState(context, 0);
      cfg.SetDepthStencilState(context, 3, 0);
      cfg.SetVertexFormat(context, 0);
      buf.SetGSR(context, params.gpuBuf[1], 0, 3u);
      buf.SetPSR(context, params.gpuBuf[4], 0, 1u);
      for(ui8 i = 0; i <= params.levelsOfDetail; ++i) {
         cfg.SetShaderGroup(context, i);
         buf.UpdateVertex(context, params.visBuf[i], params.vertBuf[i], RoundUpToNearest4(params.vertCounts[i]));
         buf.SetVertexPrimitive(context, params.vertBuf[i], 0, ae_pointlist);
         devcon[context]->DrawInstanced(params.chunkCells, params.vertCounts[i], 0, 0); // For use with ?s.map.cells.hlsl
//         gpu.devcon[0]->DrawInstanced(map.desc.chunkCells >> 3, mapManThreadData._ui32[i], 0, 0); // For use with ?s.map.cells.x8.hlsl
         sysData.gpu.total.drawCalls++;
      }
      RemoveJob();
   }

   inline void QueueDrawEntities(cENT_PARAMS &params) {
      cui8 context = params.context;
      AddJob();
      cfg.SetBlendState(context, 0);
      cfg.SetDepthStencilState(context, 3, 0);
      cfg.SetVertexFormat(context, 1u);
      buf.SetGSR(context, params.gpuBuf[0], 0, 3u);
      buf.SetPSR(context, params.gpuBuf[3], 0, 1u);
      for(ui8 i = 0; i <= params.levelsOfDetail; i++) {
         cui32 vertCount = params.vertCounts[i] >> 5u;
         cfg.SetShaderGroup(context, i + 3u);
         buf.UpdateVertex(context, params.visBuf[i], params.vertBuf[i], RoundUpToNearest4(vertCount));
         buf.SetVertexPrimitive(context, params.vertBuf[i], 0, ae_pointlist);
         devcon[context]->Draw(vertCount, 0);
         sysData.gpu.total.drawCalls++;
      }
      RemoveJob();
   }

   inline void QueueDrawGUI(cui8 context, cui16 vertexBuffer, cui16 UAVBuffer, cui32 vertexCount) {
      AddJob();
      cfg.SetShaderGroup(context, 4u);
      cfg.SetVertexFormat(context, 2u);
      buf.SetVertexPrimitive(context, vertexBuffer, 0, ae_pointlist);
      buf.SetUAV(context, UAVBuffer, 1u, 1u);
      devcon[context]->Draw(vertexCount, 0);
      ++sysData.gpu.total.drawCalls;
      RemoveJob();
   }

   inline void QueueDrawVoxel(cui8 context, cVEC3Ds32 location, cfp8n0_1x4 colour) {
      AddJob();
      DrawVoxel(context, location, colour);
      RemoveJob();
   }

   inline void QueueClearAndFlipOutputImage(cfl32x4 colour) {
      AddJob();
      ClearAndFlipOutputImage(colour);
      RemoveJob();
   }

   inline void QueuePresentOutputImage(cui8 vsyncs) {
      cui16 localJC = (jobCount += 32768u);
      while(activeJob) _mm_pause();
      PresentOutputImage(vsyncs);
      jobCount = 0;
   }

   inline void RequestPresentOutputImage(cui8 vsyncs) { jobCount += 32768; }

   // Unmanaged presentation.
   // To be called by the thread that manages the message pump
   inline void WaitForRequestToPresentOutputImage(cui8 vsyncs) {
      while(jobCount < 32768u) _mm_pause();
      while(activeJob) _mm_pause();
      PresentOutputImage(vsyncs);
      jobCount = 0;
   }

   // Managed presentation.
   // To be called, by the thread that manages the message pump, prior to final drawing and presentation
   inline void WaitForThreadsToComplete(void) { while(threadsBusy) _mm_pause(); while(activeJob) _mm_pause(); threadsBusy = threadsMax; }
};
