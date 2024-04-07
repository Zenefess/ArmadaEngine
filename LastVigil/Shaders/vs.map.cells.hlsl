/************************************************************
 * File: vs.map.cells.hlsl              Created: 2022/12/17 *
 *                                Last modified: 2023/01/29 *
 *                                                          *
 * Desc:                                                    *
 *                                                          *
 *  Copyright (c) David William Bull. All rights reserved.  *
 ************************************************************/

cbuffer MAPDIMS_ICB : register(b2) {
   const int3  iMapDim;      // X, Y, Z cell counts
   const uint  mapDimsE;   // Exponenets of .iMapDim, 24-31==Low byte of .zDim spawning offset
   const int3  iChunkDim;   // X, Y, Z cell counts per chunk
   const uint  chunkDimsE;   // Exponenets of .iChunkDim, 24-31==High byte of .zDim spawning offset
   const uint  chunkCount;   //   X, Y, Z chunk counts, 24-31==bitFlags
   const uint2 totalCells;   // Cells per chunk, Cells per map
   const uint  RES;
}

// Output: 4x4 indices
uint4x4 main(in const uint chunk : INDEX, const uint cell : SV_VertexID) : CELLS {
   const int3  iMapDimE    = (mapDimsE >> int3(0, 8, 16)) & 0x0FF;
   const int3  iChunkDimE  = (chunkDimsE >> int3(0, 8, 16)) & 0x0FF;
   const uint3 uiChunks    = (chunkCount >> int3(0, 8, 16)) & 0x0FF;
   const uint  uiChunkXY   = iChunkDimE.x + iChunkDimE.y;
   const uint3 uiChunksA   = uiChunks - 1;
   const uint3 uiCellsA    = iChunkDim - 1;
   const int3  iChunkOS    = int3(chunk & uiChunksA.x, (chunk / uiChunks.x) & uiChunksA.y, (chunk / (uiChunks.x * uiChunks.y)) & uiChunksA.z);
   const int3  iCellOS     = int3(cell & uiCellsA.x, (cell >> iChunkDimE.x) & uiCellsA.y, (cell >> uiChunkXY) & uiCellsA.z);
   const int   iCurCell    = (chunk << (uiChunkXY + iChunkDimE.z)) + cell;
   const uint2 uiChunkStep = uint2(totalCells.x * uiChunks.x, iChunkDim.x * uiCellsA.y);

   int4    columns = int4(2, 1, 0, -1);
   uint4x4 index   = iCurCell;

   if(iChunkOS.x == 0) {
      switch(iCellOS.x) {
      case 0:
         columns.xy = int2(0, 0);
         break;
      case 1:
         columns.x = 1;
         break;
      default:
         if(iCellOS.x >= uiCellsA.x) columns.w = -1 - int(uiChunkStep.y);
         break;
      }
   } else {
      if(iChunkOS.x >= uiChunksA.x) {
         switch(iCellOS.x) {
         case 0:
            columns.xy = int2(uiChunkStep.y + 2, uiChunkStep.y + 1);
            break;
         case 1:
            columns.x = uiChunkStep.y + 2;
            break;
         default:
            if(iCellOS.x >= uiCellsA.x) columns.w = 0;
            break;
         }
      } else {
         switch(iCellOS.x) {
         case 0:
            columns.xy = int2(uiChunkStep.y + 2, uiChunkStep.y + 1);
            break;
         case 1:
            columns.x = uiChunkStep.y + 2;
            break;
         default:
            if(iCellOS.x >= uiCellsA.x) columns.w = -1 - int(uiChunkStep.y);
            break;
         }
      }
   }
   index -= int4x4(columns, columns, columns, columns);
   if(iChunkOS.y == 0) {
      switch(iCellOS.y) {
      case 0:
         index[3] += iChunkDim.x;
         return transpose(index);
      case 1:
         index[0] -= iChunkDim.x;
         index[1] -= iChunkDim.x;
         index[3] += iChunkDim.x;
         return transpose(index);
      default:
         index[0] -= iChunkDim.x << 1;
         index[1] -= iChunkDim.x;
         if(iCellOS.y < uiCellsA.y)
            index[3] += iChunkDim.x;
         else
            index[3] += uiChunkStep.x - uiChunkStep.y;
         return transpose(index);
      }
   } else if(iChunkOS.y >= uiChunksA.y) {
      switch(iCellOS.y) {
      case 0:
         index[0] -= uiChunkStep.x - uiChunkStep.y + iChunkDim.x;
         index[1] -= uiChunkStep.x - uiChunkStep.y;
         index[3] += iChunkDim.x;
         return transpose(index);
      case 1:
         index[0] -= uiChunkStep.x - uiChunkStep.y + iChunkDim.x;
         index[1] -= iChunkDim.x;
         index[3] += iChunkDim.x;
         return transpose(index);
      default:
         index[0] -= iChunkDim.x << 1;
         index[1] -= iChunkDim.x;
         if(iCellOS.y < uiCellsA.y)
            index[3] += iChunkDim.x;
         return transpose(index);
      }
   } else {
      switch(iCellOS.y) {
      case 0:
         index[0] -= uiChunkStep.x - uiChunkStep.y + iChunkDim.x;
         index[1] -= uiChunkStep.x - uiChunkStep.y;
         index[3] += iChunkDim.x;
         return transpose(index);
      case 1:
         index[0] -= uiChunkStep.x - uiChunkStep.y + iChunkDim.x;
         index[1] -= iChunkDim.x;
         index[3] += iChunkDim.x;
         return transpose(index);
      default:
            index[0] -= iChunkDim.x << 1;
            index[1] -= iChunkDim.x;
         if(iCellOS.y < uiCellsA.y)
            index[3] += iChunkDim.x;
         else
            index[3] += uiChunkStep.x - uiChunkStep.y;
         return transpose(index);
      }
   }
   return transpose(index);
}
