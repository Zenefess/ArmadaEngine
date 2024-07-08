/************************************************************
 * File: Data structures.h              Created: 2022/10/20 *
 *                                Last modified: 2024/07/08 *
 *                                                          *
 * Desc:                                                    *
 *                                                          *
 * Copyright (c) David William Bull.   All rights reserved. *
 ************************************************************/
#pragma once

#ifdef DATA_TRACKING
#include "data tracking.h"
#endif

#include "typedefs.h"
#include "Vector structures.h"

enum AE_SUBSYSTEM : ui8 {
   ss_video,
   ss_audio,
   ss_input,
   ss_gui
};

enum AE_BUFFER_USAGE : ui8 {
   ae_buf_default,
   ae_buf_immutable,
   ae_buf_dynamic,
   ae_buf_staging
};

enum AE_GPU_STAGE : ui8 {
   ae_stage_null,
   ae_stage_vertex,
   ae_stage_geometry,
   ae_stages_vertex_geometry,
   ae_stage_pixel,
   ae_stages_vertex_pixel,
   ae_stages_geometry_pixel,
   ae_stages_vertex_geometry_pixel
};

enum AE_PRIMITIVES : ui32 {
   ae_undefined,
   ae_pointlist,
   ae_linelist,
   ae_linestrip,
   ae_trianglelist,
   ae_trianglestrip
};

enum AE_WINDOW_STATE : ui8 {
   ae_windowed,
   ae_borderless,
   ae_fullscreen
};

al4 union ID32 {
   struct {
      ui16 index;
      ui16 group;
   };
   ui32 id;
};

al8 union ID64 {
   struct {
      ui32 index;
      ui32 group;
   };
   ui64 id;
};

al4 union ID32c {
   struct {
      cui16 index;
      cui16 group;
   };
   cui32 id;
};

al8 union ID64c {
   struct {
      cui32 index;
      cui32 group;
   };
   cui64 id;
};

al8 union UNIPTR {
   ptr        p;
   chptr      pCH;
   wchptr     pWC;
   ui8ptr     pu8;
   si8ptr     ps8;
   ui16ptr    pu16;
   si16ptr    ps16;
#ifdef _24BIT_INTEGERS_
   ui24ptr    pu24;
   si24ptr    ps24;
#endif
   ui32ptr    pu32;
   si32ptr    ps32;
   ui64ptr    pu64;
   si64ptr    ps64;
   ui128ptr   pu128;
   si128ptr   pi128;
   ui256ptr   pu256;
   si256ptr   pi256;
   ui512ptr   pu512;
   si512ptr   pi512;
   fl32ptr    pf32;
   fl64ptr    pf64;
   fl32x4ptr  pf128;
   fl32x8ptr  pf256;
   fl32x16ptr pf512;
};

al8 struct BUFFER16 {
   union {
      ptr        p;
      chptr      pCH;
      wchptr     pWC;
      ui8ptr     pu8;
      si8ptr     ps8;
      ui16ptr    pu16;
      si16ptr    ps16;
#ifdef _24BIT_INTEGERS_
      ui24ptr    pu24;
      si24ptr    ps24;
#endif
      ui32ptr    pu32;
      si32ptr    ps32;
      ui64ptr    pu64;
      si64ptr    ps64;
      ui128ptr   pu128;
      si128ptr   pi128;
      ui256ptr   pu256;
      si256ptr   pi256;
      ui512ptr   pu512;
      si512ptr   pi512;
      fl32ptr    pf32;
      fl64ptr    pf64;
      fl32x4ptr  pf128;
      fl32x8ptr  pf256;
      fl32x16ptr pf512;
   };
   ui16 byteOffset;
   ui16 byteCount;
   ui16 byteMax;
   ui16 bitField;
};

al16 struct BUFFER32 {
   union {
      ptr        p;
      chptr      pCH;
      wchptr     pWC;
      ui8ptr     pu8;
      si8ptr     ps8;
      ui16ptr    pu16;
      si16ptr    ps16;
#ifdef _24BIT_INTEGERS_
      ui24ptr    pu24;
      si24ptr    ps24;
#endif
      ui32ptr    pu32;
      si32ptr    ps32;
      ui64ptr    pu64;
      si64ptr    ps64;
      ui128ptr   pu128;
      si128ptr   pi128;
      ui256ptr   pu256;
      si256ptr   pi256;
      ui512ptr   pu512;
      si512ptr   pi512;
      fl32ptr    pf32;
      fl64ptr    pf64;
      fl32x4ptr  pf128;
      fl32x8ptr  pf256;
      fl32x16ptr pf512;
   };
   ui32 byteCount;
   ui32 byteOffset;
};

al32 struct BUFFER64 {
   union {
      ptr        p;
      chptr      pCH;
      wchptr     pWC;
      ui8ptr     pu8;
      si8ptr     ps8;
      ui16ptr    pu16;
      si16ptr    ps16;
#ifdef _24BIT_INTEGERS_
      ui24ptr    pu24;
      si24ptr    ps24;
#endif
      ui32ptr    pu32;
      si32ptr    ps32;
      ui64ptr    pu64;
      si64ptr    ps64;
      ui128ptr   pu128;
      si128ptr   pi128;
      ui256ptr   pu256;
      si256ptr   pi256;
      ui512ptr   pu512;
      si512ptr   pi512;
      fl32ptr    pf32;
      fl64ptr    pf64;
      fl32x4ptr  pf128;
      fl32x8ptr  pf256;
      fl32x16ptr pf512;
   };
   ui64 byteCount;
   ui64 byteOffset;
   ui64 bitField;
};

al32 struct TEXTBUFFER {
   BUFFER16 source {}; // bitField: 0==Active, 1==Confirm, 2==Cancel, 3==Clear temporary array after copy, 4==Focused, 8==Defocused, 9~15==???
   BUFFER16 temp {}; // bitField: 0==In use, 1~15==???

   // maxCharCount == Maximum number of characters in array
   // wideChars == Allocate 2 bytes per character
   TEXTBUFFER(cui16 maxCharCount, cbool wideChars) {
      cui32 byteCount = temp.byteMax = maxCharCount << (wideChars ? 1 : 0);
      ptrc  pointer   = malloc16(byteCount);

      if(pointer) temp.p = pointer;
   }

   ~TEXTBUFFER(void) {
#ifdef DATA_TRACKING
      cui32     allocations = sysData.mem.allocations;
      vptrcptrc location    = sysData.mem.location;

      ui32 index = 0;

      // Find entry
      while(temp.p != location[index] && allocations >= index)
         index++;

      // Is the pointer valid?
      if(index < allocations) {
         sysData.mem.allocations--;
         sysData.mem.allocated -= sysData.mem.byteCount[index];
         sysData.mem.location[index] = NULL;
         sysData.mem.byteCount[index] = 0;
      }
#endif
      _aligned_free(temp.p);
   }
};

al16 struct FUNCTION {
   union {
      ptr      ptr;    // Raw pointer to function
      func     func;   // Function with no arguments
      funcptr  func1a; // Function with 1 argument
      funcptr2 func2a; // Function with 2 arguments
      funcptr3 func3a; // Function with 3 arguments
      funcptr4 func4a; // Function with 4 arguments
   };
   union {
      cchptr cLabel; // Display name of function (constant string)
      chptr  label;  // Display name of function
   };
};

struct SOUND_BANK {
   union {
      cchptr cLabel; // Display name of sound bank (constant string)
      chptr  label;  // Display name of sound bank
   };

   /// To do...
};

// Global control variables
al64 union GLOBALCTRLVARS {
   struct {
      union {
         struct {
            struct {
               VEC4Df s; // Analog sticks
               VEC4Df t; // Triggers
            } joy[8];
            struct {
               fl32 x;
               fl32 y;
               si32 z; // Wheel
               si32 w; // Wheel; horizontal
            } mouse;
            struct {
               fl32 x;
               fl32 y;
            } pointer[2];
         };
#ifdef __AVX__
         fl32x8 faxis32[9];
         ui256  iaxis32[9];
#endif
         fl32x4 faxis16[18];
         ui128  iaxis16[18];
         fl32  faxis[72];
         si32   iaxis[72];
      };
      union { // Position of cursor relative to client window
         si32     curCoord[2];
         VEC2Ds32 curCoords;
      };
      union { // Bitfield: 59==Mouse cursor visible, 60==Numerical input, 61==Text input, 62==No button activity, 63==Update state changes
         ui64 bits;
         ui8  misc[8];
      };
      ui64 RES[2];
      union { // Immediate key states
#ifdef __AVX__
         struct { ui256 key, button; };
#else
         struct { ui128 key[2], button[2]; };
#endif
         struct { ui64 k64[4], b64[4]; };
         struct { ui32 k32[8], b32[8]; };
         struct { ui8  k[32], b[32]; };
      } imm;
      union { // Relative key states
#ifdef __AVX__
         struct { ui256 key, button; };
#else
         struct { ui128 key[2], button[2]; };
#endif
         struct { ui64 k64[4], b64[4]; };
         struct { ui32 k32[8], b32[8]; };
         struct { ui8  k[32], b[32]; };
      } rel;
   };
   fl32x16 zmm[7];
   fl32x8  ymm[14];
   fl32x4  xmm[28];
};

// Global coordinates
al64 union GLOBALCOORDS {
   struct {
      union { // Position
         SSE4Df32 pos;
         fl32     p[4];
      };
      union { // Velocity
         SSE4Df32 vel;
         fl32     v[4];
      };
      union { // Orientation
         struct {
            SSE4Df32 dir;
            SSE4Df32 up;
         };
         AVX8Df32 ori;
         fl32     o[8];
      };
   };
   fl32x16 zmm;
   fl32x8  ymm[2];
   fl32x4  xmm[4];
   fl32    _fl[16];
};

al16 struct RESDATA { // 32 bytes
   union {
      fl32  dim[2];
      VEC2Df dims;
      struct { fl32 w, h; };
      struct {
         fl32 width;
         fl32 height;
      };
   };
   fl32 aspect;  // Aspect ratio (height / width)
   fl32 aspectI; // Inverted aspect ratio (width / height)
   fl32 gamma;   // Gamma ramp scalar
   ui32 fmtBB;   // Back buffer format
   ui32 fmtDB;   // Depth buffer format
   ui16 buffers; // Number of back buffers in swap chain
   ui8  msaa;    // (Multi)sample count
   ui8  msaaQ;   // Multisample quality
};

al16 struct RESOLUTION { // 112 bytes
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
   rn3216   refreshRate;
   si8      state;        // 0 == Windowed | 1 == Borderless | 2 = Fullscreen
   //   ui8      RES;
};

typedef const ID32               cID32;
typedef const ID64               cID64;
typedef const ID32c              cID32c;
typedef const ID64c              cID64c;
typedef       ID32       *       ID32ptr;
typedef       ID64       *       ID64ptr;
typedef       ID32c      *       ID32cptr;
typedef       ID64c      *       ID64cptr;
typedef const ID32       *       cID32ptr;
typedef const ID64       *       cID64ptr;
typedef const ID32c      *       cID32cptr;
typedef const ID64c      *       cID64cptr;
typedef       ID32       * const ID32ptrc;
typedef       ID64       * const ID64ptrc;
typedef       ID32c      * const ID32cptrc;
typedef       ID64c      * const ID64cptrc;
typedef const ID32       * const cID32ptrc;
typedef const ID64       * const cID64ptrc;
typedef const ID32c      * const cID32cptrc;
typedef const ID64c      * const cID64cptrc;
typedef const FUNCTION           cFUNCTION;
typedef       FUNCTION   *       FUNCTIONptr;
typedef const FUNCTION   *       cFUNCTIONptr;
typedef       FUNCTION   * const FUNCTIONptrc;
typedef const FUNCTION   * const cFUNCTIONptrc;
typedef const SOUND_BANK         cSOUND_BANK;
typedef       SOUND_BANK *       SOUND_BANKptr;
typedef const SOUND_BANK *       cSOUND_BANKptr;
typedef       SOUND_BANK * const SOUND_BANKptrc;
typedef const SOUND_BANK * const cSOUND_BANKptrc;

typedef const AE_SUBSYSTEM    cAE_SUBSYSTEM;
typedef const AE_WINDOW_STATE cAE_WINDOW_STATE;
typedef const RESDATA         cRESDATA;
typedef const RESOLUTION      cRESOLUTION;

typedef vol GLOBALCOORDS   vGLOBALCOORDS;
typedef vol GLOBALCTRLVARS vGLOBALCTRLVARS;
