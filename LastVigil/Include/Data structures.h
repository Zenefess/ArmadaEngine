/************************************************************
 * File: Data structures.h              Created: 2022/10/20 *
 *                                Last modified: 2024/03/29 *
 *                                                          *
 * Desc:                                                    *
 *                                                          *
 *  Copyright (c) David William Bull. All rights reserved.  *
 ************************************************************/
#pragma once

#ifdef DATA_TRACKING
#include "data tracking.h"
extern SYSTEM_DATA sysData;
#endif

#include "typedefs.h"
#include "Vector structures.h"

al4 const union ID32 {
   struct {
      ui16 index;
      ui16 group;
   };
   ui32 id;
};

al8 const union ID64 {
   struct {
      ui32 index;
      ui32 group;
   };
   ui64 id;
};

al8 union UNIPTR {
   ptr     p;
   chptr   pCH;
   wchptr  pWC;
   ui8ptr  p8;
   ui16ptr pu16;
   ui32ptr pu32;
   ui64ptr pu64;
   fl32ptr pf32;
   fl64ptr pf64;
};

al8 struct BUFFER16 {
   union {
      ptr     p;
      chptr   pCH;
      wchptr  pWC;
      ui8ptr  p8;
      ui16ptr pu16;
      ui32ptr pu32;
      ui64ptr pu64;
      fl32ptr pf32;
      fl64ptr pf64;
      si256  *pi256;
   };
   ui16 byteOffset;
   ui16 byteCount;
   ui16 byteMax;
   ui16 bitField;
};

al16 struct BUFFER32 {
   union {
      ptr     p;
      chptr   pCH;
      wchptr  pWC;
      ui8ptr  p8;
      ui16ptr pu16;
      ui32ptr pu32;
      ui64ptr pu64;
      fl32ptr pf32;
      fl64ptr pf64;
   };
   ui32 byteCount;
   ui32 byteOffset;
};

al32 struct BUFFER64 {
   union {
      ptr     p;
      chptr   pCH;
      wchptr  pWC;
      ui8ptr  p8;
      ui16ptr pu16;
      ui32ptr pu32;
      ui64ptr pu64;
      fl32ptr pf32;
      fl64ptr pf64;
   };
   ui64 byteCount;
   ui64 byteOffset;
   ui64 bitField;
};

al32 struct TEXTBUFFER {
   BUFFER16 source; // Source text array
   // bitField: 0==Active, 1==Confirm, 2==Cancel, 3==Clear temporary array after copy
   BUFFER16 temp;  // Text array for temporary copy of source
   // bitField: 0==???

   // maxCharCount == Maximum number of characters in array
   // wideChars    == Allocate 2 bytes per character
   TEXTBUFFER(cui16 maxCharCount, cbool wideChars) {
      cui32 byteCount = temp.byteMax = maxCharCount << (wideChars ? 1 : 0);
      ptrc  pointer   = _aligned_malloc(byteCount, 16);

      if(pointer) {
#ifdef DATA_TRACKING
         sysData.memory.byteCount[sysData.memory.allocations]  = byteCount;
         sysData.memory.location[sysData.memory.allocations++] = pointer;
         sysData.memory.allocated += byteCount;
#endif
         temp.p = pointer;
      }
   }

   ~TEXTBUFFER(void) {
#ifdef DATA_TRACKING
      cui32 allocations = sysData.memory.allocations;
      aptrc location    = sysData.memory.location;
      ui32  index       = 0;

      // Find entry
      while(temp.p != location[index] && allocations >= index)
         index++;

      // Is the pointer valid?
      if(index < allocations) {
         sysData.memory.allocations--;
         sysData.memory.allocated -= sysData.memory.byteCount[index];
         sysData.memory.location[index] = NULL;
         sysData.memory.byteCount[index] = 0;
      }
#endif
      _aligned_free(temp.p);
   }
};

// Global control variables
al32 struct GLOBALCTRLVARS {   // 128 bytes
   union {   // Position of cursor relative to client window
      si32     curCoord[2];
      VEC2Ds32 curCoords;
   };
   union {
      ui64 bits;   // Bitfield: 60==Numerical input, 61==Text input, 62==No button activity, 63==Update state changes
      ui8  misc[8];
   };
   union {
      union {
         float  axis[12];
         VEC3Df vec3D[4];
         VEC2Df vec2D[6];
         struct {
            VEC4Df xy[2];
            VEC2Df z[2];
         };
      };
      union {
         si32     axis[12];
         VEC3Ds32 vec3D[4];
         VEC2Ds32 vec2D[6];
         struct {
            VEC4Ds32 xy[2];
            VEC2Ds32 z[2];
         };
      } s32;
   };
   union {
      ui256 button, key;
      struct { ui64 buttons0, buttons1, buttons2, buttons3; };
      struct { ui64 keys0, keys1, keys2, keys3; };
      ui64 buttons[4], keys[4];
      ui8  b[32], k[32];
   } imm;
   union {
      ui256 button, key;
      struct { ui64 buttons0, buttons1, buttons2, buttons3; };
      struct { ui64 keys0, keys1, keys2, keys3; };
      ui64 buttons[4], keys[4];
      ui8  b[32], k[32];
   } rel;
};

// Global coordinates
al16 struct GLOBALCOORDS {   // 48 bytes
   union {
      float co[12];
      struct {
         union {   // Position
            VEC3Df pos;
            float  p[3];
         };
         union {   // Velocity
            VEC3Df vel;
            float  v[3];
         };
         union {   // Orientation
            VEC6Df ori;
            float  o[6];
         };
      };
   };
};

struct RESDATA {   // 36 bytes
   union {
      float  dim[2];
      VEC2Df dims;
      struct {
         float w;   // Width
         float h;   // Height
      };
   };
   float aspect;   // Aspect ratio (height / width)
   float aspectI;   // Inverted aspect ratio (width / height)
   float gamma;   // Gamma ramp scalar
   UINT  fmtBB;   // Back buffer format
   UINT  fmtDB;   // Depth buffer format
   si16  msaa;      // (Multi)sample count
   si16  msaaQ;   // Multisample quality
   si16  buffers;   // Number of back buffers in swap chain
   si16  RES;
};

al16 struct RESOLUTION {   // 112 bytes
   union {
      RESDATA dims[3];
      struct {
         RESDATA window;
         RESDATA borderless;
         RESDATA full;
      };
   };
   VEC2Du16 windowOS;
   VEC2Du16 borderlessOS;
   si8      state;   // 0 == Windowed | 1 == Borderless | 2 = Fullscreen
   ui8      RES[7];
};
