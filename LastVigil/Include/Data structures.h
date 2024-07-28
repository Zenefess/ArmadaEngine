/************************************************************
 * File: Data structures.h              Created: 2022/10/20 *
 *                                Last modified: 2024/07/23 *
 *                                                          *
 * Desc:                                                    *
 *                                                          *
 * Copyright (c) David William Bull.   All rights reserved. *
 ************************************************************/
#pragma once

enum AE_SUBSYSTEM : ui8 {
   ss_main,
   ss_input,
   ss_video,
   ss_audio,
   ss_gui,
   ss_worldgen
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

enum AE_SOUND_TYPE : ui16 {
   snd_null,
   snd_
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

   UNIPTR(void) {}
   UNIPTR(ptrc address) : p(address) {}
};

al8 union UNIPTRc {
   cptr        p;
   cchptr      pCH;
   cwchptr     pWC;
   cui8ptr     pu8;
   csi8ptr     ps8;
   cui16ptr    pu16;
   csi16ptr    ps16;
#ifdef _24BIT_INTEGERS_
   cui24ptr    pu24;
   csi24ptr    ps24;
#endif
   cui32ptr    pu32;
   csi32ptr    ps32;
   cui64ptr    pu64;
   csi64ptr    ps64;
   cui128ptr   pu128;
   csi128ptr   pi128;
   cui256ptr   pu256;
   csi256ptr   pi256;
   cui512ptr   pu512;
   csi512ptr   pi512;
   cfl32ptr    pf32;
   cfl64ptr    pf64;
   cfl32x4ptr  pf128;
   cfl32x8ptr  pf256;
   cfl32x16ptr pf512;

   UNIPTRc(void) {}
   UNIPTRc(cptrc address) : p(address) {}
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

struct SOUND_DESC {
   union {
      cchptr cLabel; // Display name of sound (constant string)
      chptr  label;  // Display name of sound
   };
   ui16 apiIndex;  // "Hardware" index via the current sound api
   si16 loopCount; // Repeat count; 0=No loop, -1=Infinite loop
   fl32 amplitude; // Volume modifier
};

struct SOUND_BANK {
   union {
      cchptr cLabel; // Display name of sound bank (constant string)
      chptr  label;  // Display name of sound bank
   };
   union { SOUND_DESC *const sound; SOUND_DESC *_sound; }; // Array of sounds
   union { ui32ptrc mainSounds; ui32ptr _mainSounds; }; // Indicies for default sound set
   ui32 maxSounds;
   ui32 soundCount;

   SOUND_BANK(void) {}
   SOUND_BANK(cui32 maxSoundCount) : sound(zalloc1d64(SOUND_DESC, maxSoundCount)),
                                     mainSounds(zalloc1d16(ui32, DEFAULT_SOUND_SET)) {}

   ~SOUND_BANK() { mfree1(sound); }
};

struct SOUND_LIB {
   union {
      cchptr cLabel; // Display name of sound library (constant string)
      chptr  label;  // Display name of sound library
   };
   union { SOUND_BANK *const bank; SOUND_BANK *_bank; }; // Array of sound banks
   ui32 maxBanks;
   ui32 maxSounds = 0u;
   ui32 bankCount = 0u;
   ui32 soundCount = 0u;

   SOUND_LIB(cui32 maxSoundBanks) : bank(zalloc1d64(SOUND_BANK, maxSoundBanks)) {
      maxBanks = maxSoundBanks;
      CreateBank(0u);
      bank[0].cLabel = stNone;
      bank[0]._sound[0].cLabel = stNone;
      for(ui8 i = 0; i < DEFAULT_SOUND_SET; ++i)
         bank[0]._mainSounds[i] = 0;
   }
   SOUND_LIB(cui32 maxSoundBanks, cui32 firstBankSize) : bank(zalloc1d64(SOUND_BANK, maxSoundBanks)) {
      maxBanks = maxSoundBanks;
      CreateBank(firstBankSize);
   }

   ~SOUND_LIB() { mfree1(bank); }

   cui32 CreateBank(cui32 maximumSounds) {
      bank[bankCount]._sound      = zalloc1d64(SOUND_DESC, maximumSounds);
      bank[bankCount]._mainSounds = zalloc1d16(ui32, DEFAULT_SOUND_SET);

      maxSounds += maximumSounds;
      for(ui8 i = 0; i < DEFAULT_SOUND_SET; ++i)
         bank[bankCount]._mainSounds[i] = i;
      bank[bankCount].maxSounds  = maximumSounds;
      bank[bankCount].soundCount = 0;
      return bankCount++;
   }

   SOUND_DESC &Sound(cui32 bankIndex, cui32 soundIndex) const { return bank[bankIndex].sound[soundIndex]; }
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
         fl32x16 faxis64[4];
         fl32x8  faxis32[9];
         ui256   iaxis32[9];
         fl32x4  faxis16[18];
         ui128   iaxis16[18];
         fl32    faxis[72];
         si32    iaxis[72];
      };
      union { // Position of cursor relative to client window
         si32     curCoord[2];
         VEC2Ds32 curCoords;
      };
      union { // Bitfield: 59==Mouse cursor visible, 60==Numerical input, 61==Text input, 62==No button activity, 63==Update state changes
         ui64 bits;
         ui8  misc[8];
      };
      VEC2Df scrCoord; // Cursor coordinate in screen space : 0.0~1.0
      union _GCV_MASK { // Immediate key states
         struct { ui512 k512,    b512; };
         struct { ui256 k256[2], b256[2]; };
         struct { ui64  k64[8],  b64[8]; };
         struct { ui32  k32[16], b32[16]; };
         struct { ui8   k[64],   b[64]; };
      } imm;
      _GCV_MASK rel; // Relative key states
   };
   fl32x16 zmm[9];
   fl32x8  ymm[18];
   fl32x4  xmm[36];
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

al32 union THREAD_PROPS {
private:
   struct {
      ptrptr  _handle;
      ui32ptr _idealProcessor;
      si32ptr _priority;
      ui8ptr  _sleepTime;
   };
public:
   struct {
      ptrptrc  handle;
      ui32ptrc idealProcessor;
      si32ptrc priority;
      ui8ptrc  sleepTime;
   };

   THREAD_PROPS(cui8 maxThreads) {
      _handle         = zalloc1d16(ptr, (maxThreads << 1u) + ((maxThreads + 7u) >> 3u));
      _idealProcessor = ui32ptr(_handle + maxThreads);
      _priority       = si32ptr(_idealProcessor + maxThreads);
      _sleepTime      = ui8ptr(_priority + maxThreads);
   };

   ~THREAD_PROPS(void) { mfree1(_handle); };
};

typedef const ID32               cID32;
typedef       ID32       *       ID32ptr;
typedef const ID32       *       cID32ptr;
typedef       ID32       * const ID32ptrc;
typedef const ID32       * const cID32ptrc;
typedef const ID64               cID64;
typedef       ID64       *       ID64ptr;
typedef const ID64       *       cID64ptr;
typedef       ID64       * const ID64ptrc;
typedef const ID64       * const cID64ptrc;
typedef const UNIPTR             cUNIPTR;
typedef       UNIPTR     *       UNIPTRptr;
typedef const UNIPTR     *       cUNIPTRptr;
typedef       UNIPTR     * const UNIPTRptrc;
typedef const UNIPTR     * const cUNIPTRptrc;
typedef const UNIPTRc            cUNIPTRc;
typedef       UNIPTRc    *       UNIPTRcptr;
typedef const UNIPTRc    *       cUNIPTRcptr;
typedef       UNIPTRc    * const UNIPTRcptrc;
typedef const UNIPTRc    * const cUNIPTRcptrc;
typedef const FUNCTION           cFUNCTION;
typedef       FUNCTION   *       FUNCTIONptr;
typedef const FUNCTION   *       cFUNCTIONptr;
typedef       FUNCTION   * const FUNCTIONptrc;
typedef const FUNCTION   * const cFUNCTIONptrc;
typedef const SOUND_LIB          cSOUND_LIB;
typedef       SOUND_LIB  *       SOUND_LIBptr;
typedef const SOUND_LIB  *       cSOUND_LIBptr;
typedef       SOUND_LIB  * const SOUND_LIBptrc;
typedef const SOUND_LIB  * const cSOUND_LIBptrc;

typedef const AE_SUBSYSTEM    cAE_SUBSYSTEM;
typedef const AE_WINDOW_STATE cAE_WINDOW_STATE;
typedef const RESDATA         cRESDATA;
typedef const RESOLUTION      cRESOLUTION;

typedef vol GLOBALCOORDS   vGLOBALCOORDS;
typedef vol GLOBALCTRLVARS vGLOBALCTRLVARS;
