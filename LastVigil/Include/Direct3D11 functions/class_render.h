/************************************************************
* File: class_render.h                 Created: 2023/01/16 *
*                                Last modified: 2023/01/16 *
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
#include "Direct3D11 functions\class_gui.h"

al16 struct CLASS_RENDER {
   ID3D11Device         *dev;
   ID3D11DeviceContext **devcon;
   CLASS_CONFIG         *cfg;
   CLASS_BUFFERS        *buf;
   CLASS_TEXTURES       *tex;
   CLASS_GUI            *gui;

   al16 struct { VEC3Ds32 pos; VEC4Du8  col; } vCube {};
   si16 bufVertex = 0;

   inline void DrawVoxel(VEC3Ds32 &location, const VEC4Du8 colour) {
      devcon[0]->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
      cfg->SetShaderGroup(0, 5);
      vCube = { location, colour };
      cfg->SetVertexFormat(0, 3);
      buf->UpdateVertex(0, &vCube, bufVertex, 1);
      buf->SetVertex(0, bufVertex, 0);
      devcon[0]->DrawInstanced(18, 1, 0, 0);
   }

   inline void DrawGUIElements(cui8 context) const {
      //gui->UploadElementBuffer(0);
      gui->UploadTextBuffer(context);
      gui->SetResources(0, 0);
      gui->DrawElements();
   }
};