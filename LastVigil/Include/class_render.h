/************************************************************
* File: class_render.h                 Created: 2023/01/16 *
*                                Last modified: 2024/06/25 *
*                                                          *
* Desc:                                                    *
*                                                          *
*  Copyright (c) David William Bull. All rights reserved.  *
************************************************************/
#pragma once

#include "Direct3D11 thread.h"
#include "Direct3D11 functions\class_config.h"
#include "Direct3D11 functions\class_buffers.h"
#include "Direct3D11 functions\class_textures.h"

al64 struct CLASS_RENDER {
   CLASS_CONFIG         &cfg;
   CLASS_BUFFERS        &buf;
   CLASS_TEXTURES       &tex;
   ID3D11Device         *dev;
   ID3D11DeviceContext **devcon;

   si16 bufVertex = 0;

   al16 struct { VEC3Ds32 pos; fp8n0_1x4 col; } vCube {};

   CLASS_RENDER(CLASS_CONFIG &cfgClass, CLASS_BUFFERS &bufClass, CLASS_TEXTURES &texClass) : cfg(cfgClass), buf(bufClass), tex(texClass) {}

   inline void Draw(cui8 context, cui32 vertexCount) const {
      devcon[context]->Draw(vertexCount, 0);
      sysData.gpu.total.drawCalls++;
   }

   inline void DrawInstanced(cui8 context, cui32 verticesPerInstance, cui32 instances) const {
      devcon[context]->DrawInstanced(verticesPerInstance, instances, 0, 0);
      sysData.gpu.total.drawCalls++;
   }

   inline void DrawVoxel(cui8 context, cVEC3Ds32 &location, cfp8n0_1x4 colour) {
      vCube = { location, colour };
      cfg.SetShaderGroup(context, 5);
      cfg.SetVertexFormat(context, 3);
      buf.UpdateVertex(context, &vCube, bufVertex, 1);
      buf.SetVertex(context, bufVertex, 0, ae_trianglestrip);
      devcon[context]->DrawInstanced(18, 1, 0, 0);
   }
};
