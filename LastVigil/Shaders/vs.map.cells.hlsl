/************************************************************
 * File: vs.map.cells.hlsl              Created: 2024/06/08 *
 * Type: Vertex shader            Last modified: 2024/06/11 *
 *                                                          *
 * Desc:                                                    *
 *                                                          *
 *  Copyright (c) David William Bull. All rights reserved.  *
 ************************************************************/

#include "common.hlsli"

struct CELL_DYN { // 16 bytes (1 vector)
   uint  eTypes;    // Element type for each layer
   uint  eRatios;   // Element ratios for each layer
   uint  roughness; // Roughness (lerp noise deviation) for each layer
   float dens;      // Density/viscosity (dictates mesh array median height)
//   uint  dens_warp; // Density/viscosity (dictates mesh array median height)
};
/*
cbuffer MAPDIMS_ICB : register(b2) { // 32 bytes (2 vectors)
   // Map dimensions: X, Y, Z cell counts
   // Map chunks: X, Y, Z chunk counts - 1        --- 21 bits -- [21][..][..][..]
   // zso: Z spawning offset                      --- 11 bits -- [32][..][..][..]
   // Chunk dimensions: X, Y, Z cell counts
   // Out-of-bounds density
   const uint3 mapDim;
   const uint  mapChunk_zso;
   const uint3 chunkDim;
   const uint  oobDens;
}
*/
cbuffer MAPDIMS_ICB : register(b2) {
   // Map dimensions: X, Y, Z cell counts - 1     --- 30 bits -- [..][..][..][30]
   // Chunk dimensions: X, Y, Z cell counts - 1   --- 18 bits -- [..][..][16][32]
   // Map Cells: Total cells per map - 1          --- 30 bits -- [..][14][32][..] --- No longer used
   // Chunk Cells: Total cells per chunk - 1      --- 18 bits -- [..][32][..][..] --- No longer used
   // Map chunks: X, Y, Z chunk counts - 1        --- 21 bits -- [21][..][..][..]
   // flag                                        ---  1 bits -- [22][..][..][..] --- No longer used
   // zso: Z spawning offset - 1                  --- 10 bits -- [32][..][..][..]
   const uint4 dimData;
   CELL_DYN    oob; // Out-of-bounds cell data
}

// Output == Relative cell index
uint main(in const uint chunk : INDEX, const uint cell : SV_VertexID) : CELL {
   const uint3 chunkDim = (uint3((dimData.x >> 30u) | (dimData.y << 2u), dimData.y >> 4u, dimData.y >> 10u) & 0x03Fu) + 1u; // Chunk dimensions: X, Y, Z cell counts
   const uint2 mapChunk = ((dimData.ww >> uint2(0, 7u)) & 0x07Fu) + 1u; // Map chunks: X, Y chunk counts

   // Convert cell index from compound to relative
   return ((chunk % mapChunk.x) * chunkDim.x + (cell % chunkDim.x))
        + ((((chunk / mapChunk.x) % mapChunk.y) * chunkDim.y + (((cell / chunkDim.x) % chunkDim.y))) << 10u)
        + (((chunk / (mapChunk.x * mapChunk.y)) * chunkDim.z + (((cell / (chunkDim.x * chunkDim.y)) % chunkDim.z))) << 20u);
}
