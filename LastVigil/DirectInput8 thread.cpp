/************************************************************
 * File: DirectInput8 thread.cpp        Created: 2022/11/01 *
 *                                Last modified: 2024/04/23 *
 *                                                          *
 * Desc:                                                    *
 *                                                          *
 *  Copyright (c) David William Bull. All rights reserved.  *
 ************************************************************/

#include "pch.h"
#include <algorithm>
#include <intrin.h>
#include "thread flags.h"
#include "DirectInput8 thread.h"
#include "Armada Intelligence/Input functions.h"

extern vui64 THREAD_LIFE; // 'Thread active' flags

TEXTBUFFER textBufferInfo(1024, false); // Buffer information for character input

inline static HRESULT PollKeyboard() {
   hr = di8Key->GetDeviceState(sizeof(keyState[0]), (ptr)keyState[0]);
   if(FAILED(hr))
      if((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED))
         di8Key->Acquire();
      else Try(stGetKeyDevState, -2);
   return hr;
}

inline static HRESULT PollMouse() {
   hr = di8Mse->Poll();
   if(FAILED(hr))
      do hr = di8Mse->Acquire();
      while(hr == DIERR_INPUTLOST);
   Try(stPollMouse, di8Mse->GetDeviceState(sizeof(DIMOUSESTATE38), &mseState[0]));
   return hr;
}

inline static cui8 DikToInteger(csi8 dikValue) {
   if(dikValue >= DIK_1 && dikValue <= DIK_9) return dikValue - 0x01;
   else if(dikValue >= DIK_NUMPAD7 && dikValue <= DIK_NUMPAD9) return dikValue - 0x040;
   else if(dikValue >= DIK_NUMPAD4 && dikValue <= DIK_NUMPAD6) return dikValue - 0x044;
   else if(dikValue >= DIK_NUMPAD1 && dikValue <= DIK_NUMPAD3) return dikValue - 0x04E;
   else return 0;
}

inline static cchar DikToAsciiN(csi8 dikValue) {
   if(dikValue >= DIK_1 && dikValue <= DIK_9) return dikValue + 0x02F;
   else if(dikValue >= DIK_NUMPAD7 && dikValue <= DIK_NUMPAD9) return dikValue - 0x010;
   else if(dikValue >= DIK_NUMPAD4 && dikValue <= DIK_NUMPAD6) return dikValue - 0x014;
   else if(dikValue >= DIK_NUMPAD1 && dikValue <= DIK_NUMPAD3) return dikValue - 0x01E;
   switch(dikValue) {
      case DIK_0: case DIK_NUMPAD0: return '0';
      case DIK_PERIOD: case DIK_NUMPADPERIOD: return '.';
      case DIK_MINUS: case DIK_NUMPADMINUS: return '-';
   }
   return 0;
}

///--- !!! Modify: (1) Allow for arbitrary mappings. (2) Options to process without numpad, and numpad only. !!!
inline static cchar DikToAscii(cui8 dikValue, cbool shift) {
   al8 char value = (shift ? -32 : 0);

   if(dikValue >= DIK_NUMPAD7 && dikValue <= DIK_NUMPAD9) value = dikValue - 0x010;
   else if(dikValue >= DIK_NUMPAD4 && dikValue <= DIK_NUMPAD6) value = dikValue - 0x014;
   else if(dikValue >= DIK_NUMPAD1 && dikValue <= DIK_NUMPAD3) value = dikValue - 0x01E;
   switch(dikValue) {
      case DIK_NUMPAD0: value = '0'; break;
      case DIK_NUMPADPLUS: value = '+'; break;
      case DIK_NUMPADSTAR: value = '*'; break;
      case DIK_NUMPADMINUS: value = '-'; break;
      case DIK_NUMPADEQUALS: value = '='; break;
      case DIK_NUMPADCOMMA: value = ','; break;
      case DIK_NUMPADPERIOD: value = '.'; break;
      case DIK_NUMPADSLASH: value = '/'; break;
      case DIK_1: value = (shift ? '!' : '1'); break;
      case DIK_2: value = (shift ? '@' : '2'); break;
      case DIK_3: value = (shift ? '#' : '3'); break;
      case DIK_4: value = (shift ? '$' : '4'); break;
      case DIK_5: value = (shift ? '%' : '5'); break;
      case DIK_6: value = (shift ? '^' : '6'); break;
      case DIK_7: value = (shift ? '&' : '7'); break;
      case DIK_8: value = (shift ? '*' : '8'); break;
      case DIK_9: value = (shift ? '(' : '9'); break;
      case DIK_0: value = (shift ? ')' : '0'); break;
      case DIK_GRAVE: value = (shift ? '~' : '`'); break;
      case DIK_MINUS: value = (shift ? '_' : '-'); break;
      case DIK_EQUALS: value = (shift ? '+' : '='); break;
      case DIK_LBRACKET: value = (shift ? '{' : '['); break;
      case DIK_RBRACKET: value = (shift ? '}' : ']'); break;
      case DIK_BACKSLASH: value = (shift ? '|' : '\\'); break;
      case DIK_SEMICOLON: value = (shift ? ':' : ';'); break;
      case DIK_APOSTROPHE: value = (shift ? '"' : '\''); break;
      case DIK_COMMA: value = (shift ? '<' : ','); break;
      case DIK_PERIOD: value = (shift ? '>' : '.'); break;
      case DIK_SLASH: value = (shift ? '?' : '/'); break;
      case DIK_SPACE: value = ' '; break;
      case DIK_A: value += 'a'; break;
      case DIK_B: value += 'b'; break;
      case DIK_C: value += 'c'; break;
      case DIK_D: value += 'd'; break;
      case DIK_E: value += 'e'; break;
      case DIK_F: value += 'f'; break;
      case DIK_G: value += 'g'; break;
      case DIK_H: value += 'h'; break;
      case DIK_I: value += 'i'; break;
      case DIK_J: value += 'j'; break;
      case DIK_K: value += 'k'; break;
      case DIK_L: value += 'l'; break;
      case DIK_M: value += 'm'; break;
      case DIK_N: value += 'n'; break;
      case DIK_O: value += 'o'; break;
      case DIK_P: value += 'p'; break;
      case DIK_Q: value += 'q'; break;
      case DIK_R: value += 'r'; break;
      case DIK_S: value += 's'; break;
      case DIK_T: value += 't'; break;
      case DIK_U: value += 'u'; break;
      case DIK_V: value += 'v'; break;
      case DIK_W: value += 'w'; break;
      case DIK_X: value += 'x'; break;
      case DIK_Y: value += 'y'; break;
      case DIK_Z: value += 'z'; break;
   }

   return (value == -32 ? 0 : value);
}

// Returns number of characters [buffer] has been incremented by
inline static csi8 ModifyCharBuffer(TEXTBUFFER &textBuffer) {
   ui64  (&keysI)[4] = (ui64 (&)[4])gcvLocalPtr->imm;
   ui64  (&keysR)[4] = (ui64 (&)[4])gcvLocalPtr->rel;
   cui16 byteOffset  = textBuffer.source.byteOffset;

   // Copy contents of source buffer to temporary buffer
   cui16 byteLimit = (byteOffset + 15) >> 4;
   for(ui16 i = 0; i < byteLimit; i++)
      textBuffer.temp.pi256[i] = _mm256_load_si256(&textBuffer.source.pi256[i]);
   textBuffer.temp.byteOffset = byteOffset;
   textBuffer.temp.byteCount = textBuffer.source.byteCount;

   // Is the 'cancel' flag set?
   if(textBuffer.source.bitField & 0x04) {
      // Restore contents of source buffer
      for(ui16 i = 0; i < byteLimit; i++)
         textBuffer.source.pi256[i] = _mm256_load_si256(&textBuffer.temp.pi256[i]);
      textBuffer.temp.byteOffset = byteOffset;
      return 0;
   }

   // Has backspace been pressed?
   if(keysI[0] & 0x04000 && keysR[0] & 0x04000) {
      // Remove it's activity status
      keysI[0] &= 0x0FFFFFFFFFFFFBFFF;
      keysR[0] &= 0x0FFFFFFFFFFFFBFFF;
      // Is there a character to clear?
      if(byteOffset) {
         textBuffer.source.pCH[--textBuffer.source.byteOffset] = 0;
         return -1;
      } else
         return 0;
   }

   // Has enter or numpad enter been pressed?
   if((keysI[0] & 0x010000000 && keysR[0] & 0x010000000) || (keysI[2] & 0x010000000 && keysR[2] & 0x010000000)) {
      // Remove their activity status
      keysI[0] &= 0x0FFFFFFFF4FFFFFFF;
      keysR[0] &= 0x0FFFFFFFF4FFFFFFF;
      keysI[2] &= 0x0FFFFFFFFEFFFFFFF;
      keysR[2] &= 0x0FFFFFFFFEFFFFFFF;
      // Is the 'clear' flag set?
      if(textBuffer.source.bitField & 0x08) {
         for(ui16 i = 0; i < byteLimit; i++)
            textBuffer.temp.pi256[i] = _mm256_setzero_si256();
         textBuffer.temp.byteOffset = 0;
         textBuffer.temp.byteCount  = 0;
      }
      textBuffer.source.bitField   = 0x03;
      textBuffer.source.byteOffset = textBuffer.temp.byteOffset;

      return si8(textBuffer.source.byteOffset - byteOffset);
   }

   // Is either shift being depressed?
   cbool shift = (keysI[0] & 0x040000000000 || keysI[0] & 0x040000000000000 ? true : false);

   // Process pressed keys
   for(ui8 i = 0, j; i < 4; i++)
      for(j = 0; j < 64; j++) {
         cui64 curBit = ui64(0x01) << j;

         if(keysI[i] & curBit && keysR[i] & curBit) {
            cchar result = DikToAscii((i << 6) + j, shift);
            if(result) {
               // Remove key's activity status
               keysI[i] &= ~curBit;
               keysR[i] &= ~curBit;

               // Add character to buffer if space available
               if(textBuffer.source.byteOffset < textBuffer.source.byteCount)
                  textBuffer.source.pCH[textBuffer.source.byteOffset++] = result;
            }
         }
      }

   // Add null character to buffer if space available
   if(textBuffer.source.byteOffset + 1 < textBuffer.source.byteCount)
      textBuffer.source.pCH[textBuffer.source.byteOffset + 1] = 0;

   return si8(textBuffer.source.byteOffset - byteOffset);
}

void DirectInput8Thread(ptr ArgList) {
   GLOBALCTRLVARS gcvLocal {};
   ui64           threadLife, j;
   fl32           fMouseScale = 0.0003025f;
   ui16           i;
   ui8            k, keyCode;
   DIPROPDWORD    dpw {};

   gcvLocalPtr = &gcvLocal;

   // Prevent thread from shutting down (after engine reset)
   THREAD_LIFE &= ~INPUT_THREAD_DIED;

Reinitialise_:

   THREAD_LIFE &= ~INPUT_THREAD_RESET;

   Sleep(1000);

   Try(stDI8Create, DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&di8, NULL));
   Try(stCreateDev, di8->CreateDevice(GUID_SysKeyboard, &di8Key, NULL));
   Try(stCreateDev, di8->CreateDevice(GUID_SysMouse, &di8Mse, NULL));
   Try(stSetKeyFormat, di8Key->SetDataFormat(&c_dfDIKeyboard));
   Try(stSetMseFormat, di8Mse->SetDataFormat(&dfMouse38));
   dpw.diph.dwHeaderSize = sizeof(dpw.diph);
   dpw.diph.dwHow = DIPH_DEVICE;
   dpw.diph.dwObj = NULL;
   dpw.diph.dwSize = sizeof(dpw);
   dpw.dwData = DIPROPAXISMODE_REL;
   Try(stSetMseProperty, di8Mse->SetProperty(DIPROP_AXISMODE,&dpw.diph));
//   Try(stSetKeyCoop, di8Key->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE));
   Try(stSetKeyCoop, di8Key->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE));
//   Try(stSetMseCoop, di8Mse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE));
   Try(stSetMseCoop, di8Mse->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE));
   Try(stKeyAcquire, di8Key->Acquire());
   PollMouse();
   do {
      threadLife = THREAD_LIFE;
      if (threadLife & INPUT_THREAD_RESET) goto Reinitialise_;

      // Stall if no state change requested
      if(!(gcv.misc[7] & 0x080)) {
         Sleep(1);
         _mm_pause();
         continue;
      }
      gcv.misc[7] &= 0x07F;

      // Backup state changes
      memcpy(&mseState[1], &mseState[0], sizeof(mseState[0]));
      memcpy(keyState[1], keyState[0], sizeof(keyState[0]));

      // Clear button states
      gcvLocal.imm.button = _mm256_setzero_si256();
      gcvLocal.rel.button = _mm256_setzero_si256();

      // Clear joypad states
      gcvLocal.faxis32[0] = _mm256_setzero_ps();
      gcvLocal.faxis16[2] = _mm_setzero_ps();

      PollKeyboard();
      PollMouse();

      // Set mouse axes
      GetCursorPos((LPPOINT)gcvLocal.curCoord);
      ScreenToClient(hWnd, (LPPOINT)gcvLocal.curCoord);
      gcvLocal.mouse.x = float(mseState[0].x) * fMouseScale;
      gcvLocal.mouse.y = float(mseState[0].y) * fMouseScale;
      gcvLocal.mouse.z += mseState[0].z / 120;
      gcvLocal.mouse.w += mseState[0].w / 120;

      // Populate input arrays with keyboard data
      for(i = 0; i < 0x080; i++) {
         if(keyState[0][i] & 0x080)
            gcvLocal.imm.keys[i >> 6] |= ui64(0x01) << (i & 0x03F);
         if(keyState[0][i] != keyState[1][i])
            gcvLocal.rel.keys[i >> 6] |= ui64(0x01) << (i & 0x03F);
      }
      if(keyState[0][0x08C] & 0x080)
         gcvLocal.imm.keys[2] |= 0x01000;
      if(keyState[0][0x08C] != keyState[1][i])
         gcvLocal.rel.keys[2] |= 0x01000;
      if(keyState[0][0x08D] & 0x080)
         gcvLocal.imm.keys[2] |= 0x02000;
      if(keyState[0][0x08D] != keyState[1][i])
         gcvLocal.rel.keys[2] |= 0x02000;
      for(i = 0x090; i < 0x0B9; i++) {
         if(keyState[0][i] & 0x080)
            gcvLocal.imm.keys[i >> 6] |= ui64(0x01) << (i & 0x03F);
         if(keyState[0][i] != keyState[1][i])
            gcvLocal.rel.keys[i >> 6] |= ui64(0x01) << (i & 0x03F);
      }
      for(i = 0x0C5; i < 0x0DD; i++) {
         if(keyState[0][i] & 0x080)
            gcvLocal.imm.keys[i >> 6] |= ui64(0x01) << (i & 0x03F);
         if(keyState[0][i] != keyState[1][i])
            gcvLocal.rel.keys[i >> 6] |= ui64(0x01) << (i & 0x03F);
      }
      // Populate input arrays with mouse data
      for(i = 0; i < 8; i++) {
         gcvLocal.imm.b[16] |= ui8(mseState[0].b[i] & 0x080) >> (7 - i);
         gcvLocal.rel.b[16] |= ui8(mseState[0].b[i] != mseState[1].b[i]) << i;
      }
      // Populate input arrays with mouse wheel data
      gcvLocal.imm.b[21] |= ui8(mseState[0].z < 0);
      gcvLocal.rel.b[21] |= ui8(mseState[0].z != mseState[1].z);
      gcvLocal.imm.b[21] |= ui8(mseState[0].z > 0) << 1;
      gcvLocal.rel.b[21] |= ui8(mseState[0].z != mseState[1].z) << 1;
      gcvLocal.imm.b[21] |= ui8(mseState[0].w < 0) << 2;
      gcvLocal.rel.b[21] |= ui8(mseState[0].w != mseState[1].w) << 2;
      gcvLocal.imm.b[21] |= ui8(mseState[0].w > 0) << 3;
      gcvLocal.rel.b[21] |= ui8(mseState[0].w != mseState[1].w) << 3;
      // Populate input arrays with joypad data
///--- To do...

      // Send standard characters to text input buffer
      if(textBufferInfo.source.bitField & 0x01) ModifyCharBuffer(textBufferInfo);

      // Populate global input registers
      (ui32 &)inputsImmediate = 0x0;
      for(i = 0, k = 0, keyCode = 0; i < 4 && k < 4; i++)
         for(j = 0x01; j && k < 4; j <<= 1, keyCode++)
            if(gcvLocal.imm.keys[i] & j)
               inputsImmediate._ui8[k++] = keyCode;

      ///--- Old input system ---///
      for(i = 0; i < 256; i++) {
         // Key has changed state since last poll
         if(keyState[0][i] != keyState[1][i]) {
            if(keyState[0][i] & 0x080) {   // Key is down
               switch(i) {
               case DIK_1:
                  gcvLocal.misc[0] = 0x00;
                  break;
               case DIK_2:
                  gcvLocal.misc[0] = 0x01;
                  break;
               case DIK_3:
                  gcvLocal.misc[0] = 0x02;
                  break;
               case DIK_Q:
                  gcvLocal.misc[2] = 0x00;
                  break;
               case DIK_W:
                  gcvLocal.misc[2] = 0x01;
                  break;
               case DIK_E:
                  gcvLocal.misc[2] = 0x02;
                  break;
               case DIK_SPACE:
                  gcvLocal.misc[1] = (gcvLocal.misc[1] + 1) & 0x03;
                  break;
               default:
                  gcvLocal.misc[3] |= 0x040;
               }
            } else {   // Key is up
               0;
            }
            continue;
         }
         if(keyState[0][i] & 0x080) {// Key is down
            switch(i) {
            case DIK_NUMPAD8:
               gcvLocal.joy[0].ls.y = 1.0f;
               break;
            case DIK_NUMPAD5:
               gcvLocal.joy[0].ls.y = -1.0f;
               break;
            case DIK_NUMPAD4:
               gcvLocal.joy[0].ls.x = -1.0f;
               break;
            case DIK_NUMPAD6:
               gcvLocal.joy[0].ls.x = 1.0f;
               break;
            case DIK_NUMPAD7:
               gcvLocal.joy[0].lt = 1.0f;
               break;
            case DIK_NUMPAD0:
               gcvLocal.joy[0].lt = -1.0f;
               break;
            case DIK_ESCAPE:
               THREAD_LIFE &= ~MAIN_THREAD_ALIVE;
               break;
            default:
               gcvLocal.misc[3] |= 0x080;
            }
         } else {   // Key is up
            0;
         }
      }

      ProcessInputs(gcvLocal);

      // Forcing volatile copy to avoid glitches 
      LockedCopy(&gcvLocal, &gcv, sizeof(gcvLocal));

      Sleep(1);
   } while (threadLife & INPUT_THREAD_ALIVE);

//   Try(di8Key->Unacquire());
//   Try(di8Mse->Unacquire());
   THREAD_LIFE |= INPUT_THREAD_DIED;
   //_endthread();
}
