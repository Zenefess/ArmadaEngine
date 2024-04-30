/************************************************************
* File: Map structures.h               Created: 2022/12/11 *
*                                Last modified: 2024/04/30 *
*                                                          *
* Desc:                                                    *
*                                                          *
*  Copyright (c) David William Bull. All rights reserved.  *
************************************************************/
#pragma once

#include "pch.h"
#include "typedefs.h"
#include "Vector structures.h"

#define MM_VIS_BUSY  0x01
#define MM_MOD_BUSY  0x02
#define MM_VIS_START 0x0FFFFFFFC0000000E
#define MM_MOD_START 0x000000003FFFFFFFD

al16 struct ELEM_IGS { // 16 bytes
   VEC4Du16 tc; // Texture coordinates
   union {
      struct {
         ui24 aft; // Animation frame time : 16p8
         ui8  et;  // Base transparency : p8n0.0~1.0
      };
      ui32 aft_et;
   };
   union {
      struct {
         ui8 afo; // Animation frame offset
         ui8 afc; // Animation frame count : value - 1
         ui8 tcs; // Texture coordinate scalar : 4p4-1
         ui8 ai;  // 0~6==Runtime index of atlas texture, 7==???
      };
      ui32 afo_afc_tcs;
   };
};

al16 struct ELEM_TYPE { // 48 bytes //56 bytes
   wchptr    stName; // Name of element
//   ELEM_IGS   *geometry; // Pointer to element's array entry
   float     tmp;    // Temperature; melting point
   float     tbp;    // Temperature; boiling point
   float     tip;    // Temperature; ignition point
   float     tp;     // Temperature propagation (% per second)
   float     eft;    // Fusion temperature
   float     edr;    // Decay rate (radioactivity)
   VEC2Du16  er;     // Electrical resistance at 0K & .tbp : 1p15
   VEC2Du16  ad;     // Atomic density below .tmp & above .tbp : 6p10
   ui16      acid;   // Acidity: 6p10
   ui8       ede;    // Decay result
   ui8       efe;    // Fusion result
   // ???
};

al16 struct ELEM_TABLE { // 32 bytes
   wchptr     stName;
   ELEM_TYPE *element;     // Pointer to array of elements
   ELEM_IGS  *pIGS;        // Pointer to array for GPU's geometry shader
   si32       numElements;
   ui32       RES;
};

al16 struct CELL_DGS { // 16 bytes
   VEC4Du8 et;   // Element type for each layer
   VEC4Du8 er;   // Element ratios
   VEC4Du8 end;  // Roughness (lerp noise deviation) for each layer
   ui16    dens; // Density/viscosity (dictates mesh array median height) : p16n0.0~(1.0+1.0/65534.0)
   VEC2Du8 warp; // Texture warp scalars [XY] : 0p8
};

al16 struct CELL_DPS { // 16 bytes
   VEC4Du8 pmc; // Paint map colour [RGBA] : p8n0.0~1.0
   VEC4Du8 gtc; // Global tint colour [RGBA] : p8n0.0~1.0
   ui16    gev; // Global emission value : p16n-127.0~128.0
   ui16    ems; // Emission map scalar : p16n0.0~127.5
   ui8     nms; // Normal map scalar : p8n0.0~1.818359375
   ui8     rms; // Roughness map scalar : p8n0.0~1.0
   ui8     pms; // Paint map scalar : p8n0.0~1.0
   ui8     ai;  // Runtime index of atlas texture
};

al8 struct CELL { // 40 bytes
   CELL_DGS *geometry; // Pointer into array for GPU's geometry shader
   CELL_DPS *pixel;    // Pointer into array for GPU's pixel shader
   VEC2Df    vel;      // Radial direction and origin of element propogation
   float     temp;     // Current temperature (kelvin)
   float     rad;      // Current radiation decay
   float     elec;     // Current electron density
   ui32      RES;
};

al16 struct MAPDIMS_ICB { // 48 bytes
   VEC3Ds32 mapDims;     // X, Y, Z cell counts
   VEC3Du8  mapDimsE;    // Exponenets of .iMapDim
   ui8      zsoL;        // Low byte of Z spawning offset
   VEC3Ds32 chunkDims;   // X, Y, Z cell counts
   VEC3Du8   chunkDimsE; // Exponenets of .iChunkDim
   ui8      zsoH;        // High byte of Z spawning offset
   VEC3Du8   chunkCount; // X, Y, Z chunk counts - 1
   ui8      bitFlags;
   VEC2Du32 totalCells;  // Cells/chunk, Cells/map
   ui32     totalChunks; // Chunks/map
};

al16 struct MAPDIMS_ICB_ { // 48 bytes
   VEC3Ds32 mapDims;     // X, Y, Z cell counts
   si32      zso;        // Z spawning offset
   VEC3Ds32 chunkDims;   // X, Y, Z cell counts
   VEC3Du8   chunkCount; // X, Y, Z chunk counts
   ui8      bitFlags;
   VEC2Du32 totalCells;  // Cells/chunk, Cells/map
   ui32     totalChunks; // Chunks/map
   ui32     RES;
};

// Cell-realted return values (for input processing)
al32 struct MAP_CELL_RV {
   AVX8Ds32 activeElements;
   union {
      AVX8Ds32 activeCells;
      struct {
         SSE4Ds32 cellIndex[2];
      };
      struct {
         VEC3Ds32 activeCell;
         si32     cell;
         si32     activeChunk;
         si32     RES1[3];
      };
      struct {
         VEC2Ds32 activePlane;
         si32     RES2[6];
      };
      AVX8Df32 activeLocations;
      struct {
         SSE4Df32 locationOS[2];
      };
      struct {
         VEC3Df activeLocation;
         fl32   location;
         fl32   activeMajorLoc;
         fl32   fRES1[3];
      };
      struct {
         VEC2Df fActivePlane;
         fl32   fRES2[6];
      };
   };
};

// List-related return values (for input processing)
al32 struct WORLD_LIST_RV {
   si32    world;       // World index
   si32    map;         // Map index
   si32    cellCount;   // Selected cell count
   si32    entityCount; // Selected entity count
   si32ptr cellIndex;   // Array of cell indices
   si32ptr entityIndex; // Array of entity indices
};

// Critical information for maps, including input return values
al32 struct MAP_DESC { // 160 bytes
   wchptr stName; // Text label
   wchptr stInfo; // Extended text
   union {
      VEC3Ds32 totals;
      struct {
         si32 chunkCells; // Chunk cell count
         si32 mapCells;   // Map cell count
         si32 mapChunks;  // Map chunk count
      };
   };
   si16 zso;     // .zDim spawning offset (default ground level)
   si16 ptIndex; // Index of periodic table used by map
   union {
      VEC3Ds16 dims[3];
      struct {
         VEC3Ds16 mapDim;     // Map dimensions; cells
         VEC3Ds16 chunkDim;   // Chunk dimensions; cells
         VEC3Ds16 chunkCount; // Map dimensions; chunks
      };
   };

   si16    maxListIndices; // Maximum entity associations per cell
   ui32    RES;
   si32ptr entityList;     // Entity indices associated with map cells

   MAP_CELL_RV   mcrv;
   WORLD_LIST_RV wlrv;
};

al32 struct MAP { // 256 bytes
   MAPDIMS_ICB *pCB;      // Pointer to GPU's constant buffer
   CELL_DGS    *pDGS;     // Pointer to array for GPU's geometry shader
   CELL_DPS    *pDPS;     // Pointer to array for GPU's pixel shader
   CELL        *cell;     // Pointer to cell data array
   ui64        *chunkVis; // 1-bit chunk visiblity array
   ui64        *chunkMod; // 1-bit chunk activity array
   CELL         oob;      // Properties for out-of-bounds area
   ui64         RES;
   MAP_DESC     desc;     // Map descriptors
};

///--- !!! Add WORLD struct; add world chunk data funcitonality !!!
al32 struct WORLD { // 64 bytes
   MAP       **map;       // Pointer to world's maps
   ELEM_TABLE *perTable;  // Pointer to world's periodic table
   wchptr      stName;    // Text label
   wchptr      stInfo;    // Extended text
   ui64ptr     mapVis;    // 1-bit map visiblity array
   ui64ptr     mapMod;    // 1-bit map activity array
   si32        curMap;    // Current active map
   si32        totalMaps; // Map count
   si32        maxMaps;   // Maximum map count

};

// Declarations for threaded culling functionality
al32 struct MAPMAN_THREAD_DATA {
   MAP *map;
   union {
      ptr      p;
      ui32aptr chunkVis;
      ui32ptr  chunkMod;
   };
};

al32 struct cMAPMAN_THREAD_DATA {
   MAP * const map;
   union {
      ptrc      p;
      ui32aptrc chunkVis;
      ui32ptrc  chunkMod;
   };
};

// Pointers to vertex buffers for input assembler
al32 struct MAP_PTRS {   // 64 bytes
   union {
      ptr p[8];
      struct { ptr p0, p1, p2, p3, p4, p5, p6, p7; };
      struct { ui32ptr mod; ui32ptr vis[7]; };
      struct {
         MAPDIMS_ICB *mapDims;
         ELEM_IGS    *elements;
         CELL_DGS    *cell_dgs;
         CELL_DPS    *cell_dps;
      };
   };
};

typedef const ELEM_TABLE cELEM_TABLE;
typedef const CELL       cCELL;
typedef const MAP_DESC   cMAP_DESC;
typedef const MAP        cMAP;
typedef const WORLD      cWORLD;
