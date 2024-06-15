/************************************************************
* File: class_render.h                 Created: 2023/01/16 *
*                                Last modified: 2024/06/15 *
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
   CLASS_CONFIG         &cfg;
   CLASS_BUFFERS        &buf;
   CLASS_TEXTURES       &tex;
   CLASS_GUI            &gui;
   ID3D11Device         *dev;
   ID3D11DeviceContext **devcon;

   al16 struct { VEC3Ds32 pos; fp8n0_1x4 col; } vCube {};
   si16 bufVertex = 0;

   CLASS_RENDER(CLASS_CONFIG &cfgClass, CLASS_BUFFERS &bufClass, CLASS_TEXTURES &texClass, CLASS_GUI &guiClass) : cfg(cfgClass), buf(bufClass), tex(texClass), gui(guiClass) {}

   inline void DrawVoxel(VEC3Ds32 &location, const fp8n0_1x4 colour) {
      devcon[0]->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
      cfg.SetShaderGroup(0, 5);
      vCube = { location, colour };
      cfg.SetVertexFormat(0, 3);
      buf.UpdateVertex(0, &vCube, bufVertex, 1);
      buf.SetVertex(0, bufVertex, 0);
      devcon[0]->DrawInstanced(18, 1, 0, 0);
   }

   inline void DrawGUIElements(cui8 context, cui32 interfaceIndex) const {
      gui.SetResources(context, 0);
      gui.UploadTextBuffer(context);
      gui.UploadEntryBuffer(context);
      gui.DrawInterface(context);
//      gui->DrawElements();
   }
};