/************************************************************
* File: Map structures.h               Created: 2022/12/11 *
*                                Last modified: 2024/06/30 *
*                                                          *
* Desc:                                                    *
*                                                          *
*  Copyright (c) David William Bull. All rights reserved.  *
************************************************************/
#pragma once

#include "..\master header.h"

#define MM_VIS_BUSY  0x01
#define MM_MOD_BUSY  0x02
#define MM_VIS_START 0x0FFFFFFFC0000000E
#define MM_MOD_START 0x000000003FFFFFFFD

al16 struct ELEM_IGS { // 16 bytes
   f1p15x4 tc; // Texture coordinates : 1p15
   union {
      struct {
         f16p8   aft; // Animation frame time : 16p8
         fp8n0_1 et;  // Base transparency : p8n0.0~1.0
      };
      ui32 aft_et;
   };
   union {
      struct {
         ui8  afo; // Animation frame offset
         ui8  afc; // Animation frame count : value - 1
         f4p4 tcs; // Texture coordinate scalar : 4p4-1
         ui8  ai;  // 0~6==Runtime index of atlas texture, 7==???
      };
      ui32 afo_afc_tcs;
   };
};

al16 struct ELEM_TYPE { // 48 bytes //56 bytes
   wchptr  stName; // Name of element
//   ELEM_IGS   *geometry; // Pointer to element's array entry
   float   tmp;    // Temperature; melting point
   float   tbp;    // Temperature; boiling point
   float   tip;    // Temperature; ignition point
   float   tp;     // Temperature propagation (% per second)
   float   eft;    // Fusion temperature
   float   edr;    // Decay rate (radioactivity)
   f1p15x2 er;     // Electrical resistance at 0K & .tbp : 1p15
   f6p10x2 ad;     // Atomic density below .tmp & above .tbp : 6p10
   f6p10   acid;   // Acidity: 6p10
   ui8     ede;    // Decay result
   ui8     efe;    // Fusion result
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
   VEC4Du8 er;   // Element ratios                                      <<< Change both ???
   VEC4Du8 end;  // Roughness (lerp noise deviation) for each layer     <<< to p8n0_1x4 ???
   fl32    dens; // Density/viscosity (dictates mesh array median height)
/// Unpacking .dens is noticeably slower
//   fp16n   dens; // Density/viscosity (dictates mesh array median height) : p16n0.0~(1.0+1.0/65534.0)
//   f0p8x2  warp; // Texture warp scalars [XY] : 0p8
};

al16 struct CELL_DPS { // 16 bytes
   fp8n0_1x4 pmc; // Paint map colour [RGBA] : p8n0.0~1.0
   fp8n0_1x4 gtc; // Global tint colour [RGBA] : p8n0.0~1.0
   fs7p8     gev; // Global emission value : s7p8
   f7p9      ems; // Emission map scalar : 7p9
   fp8n0_1   nms; // Normal map scalar : p8n0.0~1.0(1.818359375)
   fp8n0_1   rms; // Roughness map scalar : p8n0.0~1.0
   fp8n0_1   pms; // Paint map scalar : p8n0.0~1.0
   ui8       ai;  // Runtime index of atlas texture
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

al16 struct MAPDIMS_ICB { // 16 bytes   ---   Map Cells and Chunk Cells not needed?
   ui64     dimData[2];
   CELL_DGS oobCell;
   // Map dimensions: X, Y, Z cell counts - 1     --- 30 bits -- [..][..][..][30]
   // Chunk dimensions: X, Y, Z cell counts - 1   --- 18 bits -- [..][..][16][32]
   // Map Cells: Total cells per map - 1          --- 30 bits -- [..][14][32][..]
   // Chunk Cells: Total cells per chunk - 1      --- 18 bits -- [..][32][..][..]
   // Map chunks: X, Y, Z chunk counts - 1        --- 21 bits -- [21][..][..][..]
   // flag                                        ---  1 bits -- [22][..][..][..]
   // zso: Z spawning offset - 1                  --- 10 bits -- [32][..][..][..]
   void setMapDims(csi32 value0, csi32 value1, csi32 value2) { dimData[0] = ui64(value0 & 0x03FFu) | (ui64(value1 & 0x03FFu) << 10u) | (ui64(value2 & 0x03FFu) << 20u) | (dimData[0] & 0x0FFFFFFFFC0000000u); }
   void setChunkDims(csi32 value0, csi32 value1, csi32 value2) { dimData[0] = (ui64(value0 & 0x03Fu) << 30u) | (ui64(value1 & 0x03Fu) << 36u) | (ui64(value2 & 0x03Fu) << 42u) | (dimData[0] & 0x03FFFFFFFu); }
   void setMapCells(csi64 value) { dimData[0] = (ui64(value & 0x0FFFFu) << 48u) | (dimData[0] & 0x0FFFFFFFFFFFFu); dimData[1] = ui64((value >> 16) & 0x03FFFu) | (dimData[1] & 0x0FFFFFFFFFFFFC000u); }
   void setChunkCells(csi64 value) { dimData[1] = ui64(value << 14u) | (dimData[1] & 0x0FFFFFFFF00003FFFu); }
   void setMapChunks(csi32 value0, csi32 value1, csi32 value2) { dimData[1] = (ui64(value0 & 0x07Fu) << 32u) | (ui64(value1 & 0x07Fu) << 39u) | (ui64(value2 & 0x07Fu) << 46u) | (dimData[1] & 0x0FFE00000FFFFFFFFu); }
   void setFlag(cbool value) { dimData[1] = dimData[1] | (dimData[1] & 0x0FFDFFFFFFFFFFFFFu); }
   void setSpawnOffset(csi16 value) { dimData[1] = (ui64(value) << 54u) | (dimData[1] & 0x03FFFFFFFFFFFFFu); }
};

// Cell-realted return values (for input processing)
al32 struct MAP_CELL_RV {
   AVX8Ds32 activeElements;
   union {
      AVX8Ds32 activeCells;
      SSE4Ds32 cellIndex[2];
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
      SSE4Df32 locationOS[2];
      struct {
         VEC3Df activeLocation0;
         fl32   magnitude0;
         VEC3Df activeLocation1;
         fl32   magnitude1;
      };
      struct {
         VEC2Df fActivePlane;
         fl32   fRES2[6];
      };
   };
};

// List-related return values (for input processing)
al32 struct WORLD_LIST_RV {
   union {
      struct {
         si32 map;   // Map index
         si32 world; // World index
      };
      ID64 mapID;
   };
   si32    cellCount;   // Selected cell count
   si32    entityCount; // Selected entity count
   si32ptr cellIndex;   // Array of cell indices
   ID64ptr entityIndex; // Array of entity indices
};

// Critical information for maps, including input return values
al32 struct MAP_DESC {
   wchptr stName; // Text label
   wchptr stInfo; // Extended text
   union {
      VEC3Du32 totals;
      struct {
         ui32 chunkCells; // Chunk cell count
         ui32 mapCells;   // Map cell count
         ui32 mapChunks;  // Map chunk count
      };
   };
   si16 zso;     // .zDim spawning offset (default ground level)
   si16 ptIndex; // Index of periodic table used by map
   union {
      VEC3Du16 dims[3];
      struct {
         VEC3Du16 mapDim;     // Map dimensions; cells
         VEC3Du16 chunkDim;   // Chunk dimensions; cells
         VEC3Du16 chunkCount; // Map dimensions; chunks
      };
   };

   si16 entListDim; // Maximum entity associations per cell
   union {
      fl32 RESfl;
      ui32 RES32;
      ui16 RES16[2];
   };
   ID64ptr entityList; // Entity IDs associated with map cells

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

// Paramters for queued rendering
struct MAP_PARAMS {
   csi32    (&gpuBuf)[5];
   csi32    (&vertBuf)[MAX_MAP_LOD];
   ui32ptrc (&visBuf)[MAX_MAP_LOD];
   ui32     (&vertCounts)[];
   cui32      chunkCells;
   cui8       levelsOfDetail;
   cui8       context;
};

// Declarations for threaded culling functionality
al16 struct MAPMAN_THREAD_DATA {
   MAP *map;
   union {
      ptr        p;
      ui32ptrptr chunkVis;
      ui32ptr    chunkMod;
   };
};

al16 struct MAPMAN_THREAD_DATAc {
   MAP * const map;
   union {
      ptrc         p;
      ui32ptrcptrc chunkVis;
      ui32ptrc     chunkMod;
   };
};

// Pointers to vertex buffers for input assembler
al32 struct MAP_PTRS { // 64 bytes
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

// Pointers to vertex buffers for input assembler
al32 struct MAP_PTRSc { // 64 bytes
   union {
      ptrc p[8];
      struct { ptrc p0, p1, p2, p3, p4, p5, p6, p7; };
      struct { ui32ptrc mod; ui32ptrc vis[7]; };
      struct {
         MAPDIMS_ICB * const mapDims;
         ELEM_IGS    * const elements;
         CELL_DGS    * const cell_dgs;
         CELL_DPS    * const cell_dps;
      };
   };
};

typedef const ELEM_TABLE                  cELEM_TABLE;
typedef const CELL                        cCELL;
typedef       CELL                *       CELLptr;
typedef const CELL                *       cCELLptr;
typedef       CELL                * const CELLptrc;
typedef const CELL                * const cCELLptrc;
typedef       CELL_DGS            *       CELL_DGSptr;
typedef       CELL_DGS            * const CELL_DGSptrc;
typedef       CELL_DPS            *       CELL_DPSptr;
typedef       CELL_DPS            * const CELL_DPSptrc;
typedef const ELEM_IGS                    cELEM_IGS;
typedef       ELEM_IGS            *       ELEM_IGSptr;
typedef const ELEM_IGS            *       cELEM_IGSptr;
typedef       ELEM_IGS            * const ELEM_IGSptrc;
typedef const ELEM_IGS            * const cELEM_IGSptrc;
typedef const ELEM_TABLE                  cELEM_TABLE;
typedef       ELEM_TABLE          *       ELEM_TABLEptr;
typedef const ELEM_TABLE          *       cELEM_TABLEptr;
typedef       ELEM_TABLE          * const ELEM_TABLEptrc;
typedef const ELEM_TABLE          * const cELEM_TABLEptrc;
typedef const ELEM_TYPE                   cELEM_TYPE;
typedef       ELEM_TYPE           *       ELEM_TYPEptr;
typedef const ELEM_TYPE           *       cELEM_TYPEptr;
typedef       ELEM_TYPE           * const ELEM_TYPEptrc;
typedef const ELEM_TYPE           * const cELEM_TYPEptrc;
typedef const MAP_DESC                    cMAP_DESC;
typedef const MAP                         cMAP;
typedef       MAP                 *       MAPptr;
typedef const MAP                 *       cMAPptr;
typedef       MAP                 * const MAPptrc;
typedef const MAP                 * const cMAPptrc;
typedef const MAP_PARAMS                  cMAP_PARAMS;
typedef       MAP_PARAMS          *       MAP_PARAMSptr;
typedef const MAP_PARAMS          *       cMAP_PARAMSptr;
typedef       MAP_PARAMS          * const MAP_PARAMSptrc;
typedef const MAP_PARAMS          * const cMAP_PARAMSptrc;
typedef const MAP_PTRS                    cMAP_PTRS;
typedef const MAP_PTRSc                   cMAP_PTRSc;
typedef       MAPMAN_THREAD_DATA  *       MMTDptr;
typedef const MAPMAN_THREAD_DATA  *       cMMTDptr;
typedef       MAPMAN_THREAD_DATA  * const MMTDptrc;
typedef const MAPMAN_THREAD_DATA  * const cMMTDptrc;
typedef       MAPMAN_THREAD_DATAc *       MMTDcptr;
typedef const MAPMAN_THREAD_DATAc *       cMMTDcptr;
typedef       MAPMAN_THREAD_DATAc * const MMTDcptrc;
typedef const MAPMAN_THREAD_DATAc * const cMMTDcptrc;
typedef const WORLD                       cWORLD;
typedef       WORLD               *       WORLDptr;
typedef const WORLD               *       cWORLDptr;
typedef       WORLD               * const WORLDptrc;
typedef const WORLD               * const cWORLDptrc;
