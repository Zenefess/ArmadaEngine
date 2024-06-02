/************************************************************
 * File: vs.map.cells.x8.hlsl           Created: 2023/04/29 *
 * Type: Vertex shader            Last modified: 2023/05/30 *
 *                                                          *
 * Desc:                                                    *
 *                                                          *
 *  Copyright (c) David William Bull. All rights reserved.  *
 ************************************************************/

#include "common.hlsli"

cbuffer MAPDIMS_ICB : register(b2) {
   // Map dimensions: X, Y, Z cell counts - 1     --- 30 bits -- [..][..][..][30]
   // Chunk dimensions: X, Y, Z cell counts - 1   --- 18 bits -- [..][..][16][32]
   // Map Cells: Total cells per map - 1          --- 30 bits -- [..][14][32][..]
   // Chunk Cells: Total cells per chunk - 1      --- 18 bits -- [..][32][..][..]
   // Map chunks: X, Y, Z chunk counts - 1        --- 21 bits -- [21][..][..][..]
   // flag                                        ---  1 bits -- [22][..][..][..]
   // zso: Z spawning offset - 1                  --- 10 bits -- [32][..][..][..]
   const uint4 dimData;
}

int4x11 main(in const uint chunk : INDEX, const uint _cell : SV_VertexID) : CELLS {
   const uint3 uiCellsA    = { (dimData.x >> 30u) | ((dimData.y << 2u) & 0x03Cu), (dimData.y >> 4u) & 0x03Fu, (dimData.y >> 10u) & 0x03Fu };
   const uint  cell        = _cell << 3;
   const int3  iChunkDim   = uiCellsA + 1;
   const uint  uiChunkXY   = iChunkDim.x * iChunkDim.y;
   const uint3 uiChunks    = ((dimData.w >> uint3(0, 7u, 14u)) & 0x07Fu) + 1u;
   const int   i1stEntry   = (chunk * (uiChunkXY * iChunkDim.z)) + cell - 2;
   const uint2 totalCells  = { (dimData.z >> 14u) + 1u, ((dimData.y >> 16u) | ((dimData.z & 0x03FFu) << 16u)) + 1u };
   const uint2 uiChunkStep = { totalCells.x * uiChunks.x, iChunkDim.x * uiCellsA.y };
   const int3  iChunkOS    = { chunk % uiChunks.x, (chunk / uiChunks.x) % uiChunks.y, (chunk / (uiChunks.x * uiChunks.y)) % uiChunks.z };
   const int3  iCellOS     = { cell & uiCellsA.x, (cell / iChunkDim.x) & uiCellsA.y, (cell / uiChunkXY) & uiCellsA.z };

   int4    rows = int4(iChunkDim.x << 1, iChunkDim.x, 0, -iChunkDim.x);
   int4x11 index;

   if(iChunkOS.y == 0) {
      switch(iCellOS.y) {
      case 0:
         rows.xy = 0;
         break;
      case 1:
         rows.x = iChunkDim.x;
         break;
      default:
         if(iCellOS.y >= uiCellsA.y)
            rows.w = uiChunkStep.y - uiChunkStep.x;
         break;
      }
   } else if(iChunkOS.y > uiChunks.y) {
      switch(iCellOS.y) {
      case 0:
         rows.xy += uiChunkStep.x - totalCells.x;
         break;
      case 1:
         rows.x += uiChunkStep.x - totalCells.x;
         break;
      default:
         if(iCellOS.y >= uiCellsA.y)
            rows.w = 0;
         break;
      }
   } else {
      switch(iCellOS.y) {
      case 0:
         rows.xy += uiChunkStep.x - totalCells.x;
         break;
      case 1:
         rows.x += uiChunkStep.x - totalCells.x;
         break;
      default:
         if(iCellOS.y >= uiCellsA.y)
            rows.w = uiChunkStep.y - uiChunkStep.x;
         break;
      }
   }

   [unroll]
   for(int i = 0; i < 11; i++)
      index[i] = i1stEntry.xxxx + i.xxxx - rows;

   if(iChunkOS.x == 0) {
      switch(iCellOS.x) {
      case 0:
         index[0] += 2;
         index[1] += 1;
         break;
      case 1:
         index[0] += 1;
         break;
      default:
         if(iCellOS.x > uiCellsA.x - 8)
            index[10] += int(uiChunkStep.y);
         break;
      }
   } else {
      if(iChunkOS.x > uiChunks.x) {
         switch(iCellOS.x) {
         case 0:
            index[0] -= uiChunkStep.y;
            index[1] -= uiChunkStep.y;
            break;
         case 1:
            index[0] -= uiChunkStep.y;
            break;
         default:
            if(iCellOS.x > uiCellsA.x - 8)
               index[10] -= 1;
            break;
         }
      } else {
         switch(iCellOS.x) {
         case 0:
            index[0] -= uiChunkStep.y;
            index[1] -= uiChunkStep.y;
            break;
         case 1:
            index[0] -= uiChunkStep.y;
            break;
         default:
            if(iCellOS.x > uiCellsA.x - 8)
               index[10] += int(uiChunkStep.y);
            break;
         }
      }
   }

   return index;
}
