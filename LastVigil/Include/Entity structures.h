/************************************************************
 * File: Entity structures.h            Created: 2022/12/05 *
 *                                Last modified: 2023/06/11 *
 *                                                          *
 * Desc:                                                    *
 *                                                          *
 *  Copyright (c) David William Bull. All rights reserved.  *
 ************************************************************/
#pragma once

#include "typedefs.h"
#include "Vector structures.h"

// Header elements of immutable structured buffer for predefined objects
al4 struct OBJECT_IGS {   // 8 bytes
   ui32 ppi;   // Parent PART_IGS index
   union {
      ui32 qc_bits;
      struct {
         ui8  qc;      // Quad count - 1
         ui24 bits;   // Bitfield
      };
   };
};

// Elements of immutable structured buffer for predefined objects
al16 struct PART_IGS {   // 68 bytes + 12 res
   VEC3Df    pos;         // Position (relative)
   union {
      ui32 ai_bits;
      struct {
         ui8  ai;      //   Runtime index of atlas texture
         ui24 bits;   // 0==Shape (quad/tri), 1==Billboard, 2~23==???
      };
   };
   VEC3Df    rot;         // Rotation   (relative)
   ui32      RES1;
   VEC2Df    size;      //   Relative to parent
   VEC4Du16  tc;         // tc[2].x == Paint map     | tc[2].y == Emission map (+0.5f)
                        // tc[3].x == Highlight map | tc[3].y == Occlusion map
   struct SLIDEROTATION {
      VEC3Df slide;      // Maximum slide vector transformation
      ui32   RES;
      VEC3Df rot;         // Maximum rotation vector transformation
   } trans;
   ui32      RES2;   
};

// Elements of dynamic structured buffer for entity states
al16 struct BONE_DGS {   // 64 bytes
   VEC3Df pos;      // Current position (relative to owner or world)
   fl32   lerp;   // pos->mt.s & rot->mt.r (current recoil state)
   VEC3Df rot;      // Current rotation (relative to owner or world)
   ui32   sai;      // SPRITE array index
   VEC3Df size;   // Scale (.x==0 ? Not drawn)
   ui32   opi;      // Object part index
   fl32   aft;      // Animation frame time
   union {
      struct {
         ui8  afc;   // Animation frame count : value - 1
         ui8  afo;   // Animation frame offset
         ui16 oai;   // Object array index
      };
      ui32 afc_afo_oai;
   };
   ui32   pbi;      // Parent's bone index (this==No parent)
   ui32   obi;      // Owner's bone index (this==No owner)
};

// Elements of dynamic structured buffer for sprite modifiers
al16 struct SPRITE_DPS {   // 16 bytes
   VEC4Du8 pmc;   // Paint map colour [RGBA] : p8n0.0~1.0
   VEC4Du8 gtc;   // Global tint colour [RGBA] : p8n0.0~1.0
   ui16    gev;   // Global emission value : p16n-127.0~128.0
   ui16    ems;   // Emission map scalar : p16n0.0~127.5
   ui8     nms;   // Normal map scalar : p8n0.0~1.818359375
   ui8     rms;   // Roughness map scalar : p8n0.0~1.0
   ui8     pms;   // Paint map scalar : p8n0.0~1.0
   ui8     RES;
};

al16 struct BONE {   // 80 bytes
   VEC3Df   vel;      // Velocity
   float      mass;      // Kilograms
   float      tension;   // Rate of return to origin (units/second)
   float      strInt;   // Structural integrity (health analogue)
   float      curTemp;   // Current temperature (kelvin)
   float      lsTemp;   // Lower boundary of safe temperature range
   float      usTemp;   // Upper boundary of safe temperature range
   float      sac;      // Upper boundary of safe acid concentration
   VEC3Df   cbd;      //   Collision bounding shape dimension(s) (% of BONE_DGS.size)
   ui8      cbt;      // Collision bounding shape type (0==None, 1==Sphere, 2==Cylinder, 3==Box)
   ui8      fct[3];   // Fuel consumption types
   ui32      fl[3];   // Fuel levels
   ui16      fcr[3];   // Fuel consumption rates
   ui16      bits;      // Miscellaneous flags
};

struct BRAIN {   // 8 bytes
   ui8   tl;   // Thought latency
   ui8   tps;   // Thoughts per second
   ui8   mst;   // Maximum simultaneous thoughts
   ui8   tdc;   // Thought disruption chance
   ui8   ddc;   // Decision disruption chance
   ui8   dc;   // Decision confidence
   si16   tb;   // Index into Think() bank
};

al32 struct ENTITY {  // 352 bytes
   ENTITY      *part[32];      // Array of attached body parts. Global max of 32 (0==root)
   wchptr       name;
   wchptr       text;
   BONE         *bone;         // Pointer to entity's parent bone
   BONE_DGS      *geometry;      // Pointer into array for GPU's geometry shader
   SPRITE_DPS   *sprite;         // Pointer into array for GPU's pixel shader
   BRAIN          brain;         // A.I. variables
   VEC3Df       vbd;            //   Visual bounding shape dimension(s); absolute
   ui8          vbt;            // Collision bounding shape type (0==None, 1==Sphere, 2==Cylinder, 3==Box)
   ui8          numParts;      // Number of entity entries in array - 1
   si16          soundIndex;   // Array index of sound bank
   ui32         objectIndex;   // Array index of object template
   ui32         boneIndex;      // Array index of parent bone
   ui64          bits;
};

al16 struct OBJECT_GROUP {   // 48 bytes
   wchptr      name;
   wchptr      text;
   OBJECT_IGS *object;
   PART_IGS   *part;
   si32        totalObjects;
   si32        totalParts;
   si32        maxObjects;
   si32        maxParts;
};

al16 struct ENTITY_GROUP {   // 108 bytes
   wchptr      name;
   wchptr      text;
   ENTITY     *entity;
   BONE       *bone;
   BONE_DGS   *bone_dgs;
   SPRITE_DPS *spriteO;
   SPRITE_DPS *spriteT;
   SPRITE_DPS *const sprite_dps[2] = { spriteO, spriteT };
   ui64ptr     entityVis; // 1-bit entity visiblity array
   ui64ptr     entityMod; // 1-bit entity activity array
   si32        totalEntities;
   si32        totalBones;
   si32        totalBone_DGS;
   si32        totalSpritesO;
   si32        totalSpritesT;
   si32        maxEntities;
   si32        maxBones;
};

// Declarations for threaded culling functionality
al32 struct ENTMAN_THREAD_DATA {
   ENTITY_GROUP *group;
   union {
      ptr      p;
      ui32aptr entityVis;
      ui32ptr  entityMod;
   };
};

al32 struct cENTMAN_THREAD_DATA {
   ENTITY_GROUP * const group;
   union {
      ptrc      p;
      ui32aptrc entityVis;
      ui32ptrc  entityMod;
   };
};

// Pointers to vertex buffers for input assembler
al32 union ENT_PTRS {   // 64 bytes
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

typedef const OBJECT_GROUP cOBJECT_GROUP;
typedef const ENTITY_GROUP cENTITY_GROUP;
typedef const ENTITY       cENTITY;
typedef const ENT_PTRS     cENT_PTRS;
typedef const ID32         cID32;
typedef const ID64         cID64;
