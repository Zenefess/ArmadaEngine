/************************************************************
 * File: Entity structures.h            Created: 2022/12/05 *
 *                                Last modified: 2023/06/30 *
 *                                                          *
 * Desc:                                                    *
 *                                                          *
 *  Copyright (c) David William Bull. All rights reserved.  *
 ************************************************************/
#pragma once

#include "..\master header.h"

// Header elements of immutable structured buffer for predefined objects
al4 struct OBJECT_IGS {
   ui32 ppi; // Parent PART_IGS index
   union {
      ui32 qc_bits;
      struct {
         ui8  qc;   // Quad count - 1
         ui24 bits; // Bitfield
      };
   };
};

// Elements of immutable structured buffer for predefined objects
al16 struct PART_IGS {
   VEC3Df pos; // Position (relative)
   union {
      ui32 ai_bits;
      struct {
         ui8  ai;   // Runtime index of atlas texture
         ui24 bits; // 0==Shape (quad/tri), 1==Billboard, 2~23==???
      };
   };
   VEC3Df  rot;  // Rotation (relative)
   ui32    oppi; // Owner's parent part index (this==No owner)
   VEC2Df  size; // Relative to parent
   f1p15x4 tc;   // tc[2].x == Paint map     | tc[2].y == Emission map (+0.5f)
                 // tc[3].x == Highlight map | tc[3].y == Occlusion map
   union {
      struct SLIDEROTATION {
         VEC3Df slide; // Maximum slide vector transformation
         fl32   RES;
         VEC3Df rot;   // Maximum rotation vector transformation
         fl32   RES2;
      } trans = { .RES = 1.0f, .RES2 = 1.0f };
      SSE4Df32 sliderot[2];
   };
};

// Elements of dynamic structured buffer for entity states
al16 struct BONE_DGS {
   union {
      struct {
         VEC3Df pos;  // Current position (relative to owner or world)
         fl32   lerp; // pos->mt.s & rot->mt.r (current recoil state)
      };
      SSE4Df32 pos_lerp;
   };
   union {
      struct {
         VEC3Df rot; // Current rotation (relative to owner or world)
         fl32   aft; // Animation frame time
      };
      SSE4Df32 rot_aft;
   };
   union {
      struct {
         VEC3Df size; // Scale (.x==0 ? Not drawn)
         union {
            struct {
               ui8  afc; // Animation frame count : value - 1
               ui8  afo; // Animation frame offset
               ui16 oai; // Object array index
            };
            ui32 afc_afo_oai;
         };
      };
      SSE4Df32 size_afco_oai;
   };
   ui32 opi; // Object part index
   ui32 sai; // SPRITE array index
   ui32 pbi; // Parent's bone index (this==No parent)
   ui32 obi; // Owner's bone index (this==No owner)
};

// Elements of dynamic structured buffer for sprite modifiers
al16 struct SPRITE_DPS {
   fp8n0_1x4  pmc; // Paint map colour [RGBA] : p8n0.0~1.0
   fp8n0_1x4  gtc; // Global tint colour [RGBA] : p8n0.0~1.0
   fs7p8      gev; // Global emission value : s7p8
   fp16n0_128 ems; // Emission map scalar : p16n0.0~128
   fp8n0_1    nms; // Normal map scalar : p8n0.0~1.0(1.818359375)
   fp8n0_1    rms; // Roughness map scalar : p8n0.0~1.0
   fp8n0_1    pms; // Paint map scalar : p8n0.0~1.0
   ui8        RES;
};

al16 struct BONE { // 80 bytes
   VEC3Df vel;     // Velocity
   fl32   mass;    // Kilograms
   fl32   tension; // Rate of return to origin (units/second)
   fl32   strInt;  // Structural integrity (health analogue)
   fl32   curTemp; // Current temperature (kelvin)
   fl32   lsTemp;  // Lower boundary of safe temperature range
   fl32   usTemp;  // Upper boundary of safe temperature range
   fl32   sac;     // Upper boundary of safe acid concentration
   VEC3Df cbd;     // Collision bounding shape dimension(s) (% of BONE_DGS.size)
   ui8    cbt;     // Collision bounding shape type (0==None, 1==Sphere, 2==Cylinder, 3==Box)
   ui8    fct[3];  // Fuel consumption types
   ui32   fl[3];   // Fuel levels
   ui16   fcr[3];  // Fuel consumption rates
   ui16   bits;    // Miscellaneous flags
};

struct BRAIN { // 8 bytes
   ui8   tl;  // Thought latency
   ui8   tps; // Thoughts per second
   ui8   mst; // Maximum simultaneous thoughts
   ui8   tdc; // Thought disruption chance
   ui8   ddc; // Decision disruption chance
   ui8   dc;  // Decision confidence
   si16  tb;  // Index into Think() bank
};

al16 struct ENTITY { // 336 bytes
   ENTITY     *part[32];    // Array of attached body parts. Global max of 32 (0==root)
   wchptr      name;
   wchptr      text;
   BONE       *bone;        // Pointer to entity's parent bone
   BONE_DGS   *geometry;    // Pointer into array for GPU's geometry shader
   SPRITE_DPS *sprite;      // Pointer into array for GPU's pixel shader
   BRAIN       brain;       // A.I. variables
   VEC3Df      vbd;         // Visual bounding shape dimension(s); absolute
   ui8         vbt;         // Collision bounding shape type (0==None, 1==Sphere, 2==Cylinder, 3==Box)
   ui8         numParts;    // Number of entity entries in array - 1
   si16        soundIndex;  // Array index of sound bank
   ui32        objectIndex; // Array index of object template
   ui32        boneIndex;   // Array index of parent bone
   ui64        bits;
/*   union {
      SSE4Df32 oldPosLerp;
      SSE4Ds32 oldCell;
      struct {
         VEC3Df oldPos;
         fl32   oldLerp;
      };
      struct {
         VEC3Ds32 oldCellCoord;
         si32     oldCellIndex;
      };
   };*/
};

al16 struct OBJECT_GROUP { // 48 bytes
   chptr       name;
   chptr       text;
   OBJECT_IGS *object;
   PART_IGS   *part;
   si32        totalObjects;
   si32        totalParts;
   si32        maxObjects;
   si32        maxParts;
};

al16 struct ENTITY_GROUP { // 112 bytes
   chptr     name;
   chptr     text;
   ENTITY   *entity;
   BONE     *bone;
   BONE_DGS *bone_dgs;
   union {
      struct {
         SPRITE_DPS *spriteO;
         SPRITE_DPS *spriteT;
      };
      SPRITE_DPS *sprite_dps[2];
   };
   ui64ptr entityVis; // 1-bit entity visiblity array
   ui64ptr entityMod; // 1-bit entity activity array
   si32    totalEntities;
   si32    totalBones;
   si32    totalBone_DGS;
   si32    totalSpritesO;
   si32    totalSpritesT;
   si32    maxEntities;
   si32    maxBones;
};

// Paramters for queued rendering
struct ENT_PARAMS {
   csi32    (&gpuBuf)[4];
   csi32    (&vertBuf)[MAX_ENT_LOD];
   ui32ptrc (&visBuf)[MAX_ENT_LOD];
   ui32     (&vertCounts)[];
   cui8       levelsOfDetail;
   cui8       context;
};

// Declarations for threaded culling functionality
al16 struct ENTMAN_THREAD_DATA {
   ENTITY_GROUP *group;
   union {
      ptr        p;
      ui32ptrptr entityVis;
      ui32ptr    entityMod;
   };
};

// Declarations for threaded culling functionality
al16 struct ENTMAN_THREAD_DATAc {
   ENTITY_GROUP * const group;
   union {
      ptrc        p;
      ui32ptrptrc entityVis;
      ui32ptrc    entityMod;
   };
};

// Pointers to vertex buffers for input assembler
al32 union ENT_PTRS {
   ptr p[8];
   struct { ptr p0, p1, p2, p3, p4, p5, p6, p7; };
   struct { ui32ptr mod; ui32ptr vis[7]; };
   struct {
      OBJECT_IGS *object;
      PART_IGS   *part;
      BONE_DGS   *bone;
      union {
         SPRITE_DPS *sprite[2];
         struct { SPRITE_DPS *spriteO; SPRITE_DPS *spriteT; };
      };
   };
};

// Pointers to vertex buffers for input assembler
al32 union ENT_PTRSc {
   ptrc p[8];
   struct { ptrc p0, p1, p2, p3, p4, p5, p6, p7; };
   struct { ui32ptrc mod; ui32ptrc vis[7]; };
   struct {
      OBJECT_IGS * const object;
      PART_IGS   * const part;
      BONE_DGS   * const bone;
      union {
         SPRITE_DPS * const sprite[2];
         struct { SPRITE_DPS * const spriteO; SPRITE_DPS * const spriteT; };
      };
   };
};

typedef       BONE_DGS            *       BONE_DGSptr;
typedef       BONE_DGS            * const BONE_DGSptrc;
typedef const ENT_PARAMS                  cENT_PARAMS;
typedef       ENT_PARAMS          *       ENT_PARAMSptr;
typedef const ENT_PARAMS          *       cENT_PARAMSptr;
typedef       ENT_PARAMS          * const ENT_PARAMSptrc;
typedef const ENT_PARAMS          * const cENT_PARAMSptrc;
typedef const ENT_PTRS                    cENT_PTRS;
typedef const ENT_PTRSc                   cENT_PTRSc;
typedef       ENTMAN_THREAD_DATA  *       EMTDptr;
typedef       ENTMAN_THREAD_DATA  * const EMTDptrc;
typedef const ENTMAN_THREAD_DATA  *       cEMTDptr;
typedef const ENTMAN_THREAD_DATA  * const cEMTDptrc;
typedef       ENTMAN_THREAD_DATAc *       EMTDcptr;
typedef       ENTMAN_THREAD_DATAc * const EMTDcptrc;
typedef const ENTMAN_THREAD_DATAc *       cEMTDcptr;
typedef const ENTMAN_THREAD_DATAc * const cEMTDcptrc;
typedef const ENTITY                      cENTITY;
typedef const ENTITY_GROUP                cENTITY_GROUP;
typedef       ENTITY_GROUP        *       ENTITY_GROUPptr;
typedef const ENTITY_GROUP        *       cENTITY_GROUPptr;
typedef       ENTITY_GROUP        * const ENTITY_GROUPptrc;
typedef const ENTITY_GROUP        * const cENTITY_GROUPptrc;
typedef const OBJECT_GROUP                cOBJECT_GROUP;
typedef       OBJECT_GROUP        *       OBJECT_GROUPptr;
typedef const OBJECT_GROUP        *       cOBJECT_GROUPptr;
typedef       OBJECT_GROUP        * const OBJECT_GROUPptrc;
typedef const OBJECT_GROUP        * const cOBJECT_GROUPptrc;
typedef       SPRITE_DPS          *       SPRITE_DPSptr;
typedef       SPRITE_DPS          * const SPRITE_DPSptrc;
