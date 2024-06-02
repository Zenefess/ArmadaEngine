/************************************************************
 * File: DirectInput8 thread.cpp        Created: 2022/11/01 *
 *                                Last modified: 2024/06/01 *
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
#include "Xinput.h"
#include "Armada Intelligence/Input functions.h"

extern vui64 THREAD_LIFE; // 'Thread active' flags

TEXTBUFFER textBufferInfo(1024, false); // Buffer information for character input

inline static HRESULT PollKeyboard(void) {
   hr = di8Key->GetDeviceState(sizeof(keyState[0]), (ptr)keyState[0]);
   if(FAILED(hr))
      if((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED))
         di8Key->Acquire();
      else Try(stGetKeyDevState, -2);
   return hr;
}

inline static HRESULT PollMouse(void) {
   hr = di8Mse->Poll();
   if(FAILED(hr))
      do hr = di8Mse->Acquire();
      while(hr == DIERR_INPUTLOST);
   Try(stPollMouse, di8Mse->GetDeviceState(sizeof(DIMOUSESTATE38), &mseState[0]));
   return hr;
}

inline static HRESULT PollGamepads(void) {
   for(ui32 index = 0; index < gamepadCount; index++) {
      hr = di8Pad[index]->Poll();
      if(FAILED(hr))
         do hr = di8Pad[index]->Acquire();
      while(hr == DIERR_INPUTLOST);
      Try(stPollGamepad, di8Pad[index]->GetDeviceState(sizeof(DIJOYSTATE), &padState[0][index]));
   }
   return hr;
}

inline static HRESULT PollGamepad(cui32 gamepadIndex) {
   hr = di8Pad[gamepadIndex]->Poll();
   if(FAILED(hr))
      do hr = di8Pad[gamepadIndex]->Acquire();
   while(hr == DIERR_INPUTLOST);
   Try(stPollGamepad, di8Pad[gamepadIndex]->GetDeviceState(sizeof(DIJOYSTATE), &padState[0][gamepadIndex]));
   return hr;
}

BOOL CALLBACK GamepadEnumeration(const DIDEVICEINSTANCE *inst, ptrc con) {
   csi32 result = di8->CreateDevice(inst->guidInstance, &di8Pad[gamepadCount], NULL);
   if FAILED(result) return DIENUM_CONTINUE;
   else gamepadCount++;
   if(gamepadCount >= 8) return DIENUM_STOP;
   return DIENUM_CONTINUE;
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
      case DIK_NUMPAD0: return '0';
      case DIK_NUMPADPLUS: return '+';
      case DIK_NUMPADSTAR: return '*';
      case DIK_NUMPADMINUS: return '-';
      case DIK_NUMPADEQUALS: return '=';
      case DIK_NUMPADCOMMA: return ',';
      case DIK_NUMPADPERIOD: return '.';
      case DIK_NUMPADSLASH: return '/';
      case DIK_1: return (shift ? '!' : '1');
      case DIK_2: return (shift ? '@' : '2');
      case DIK_3: return (shift ? '#' : '3');
      case DIK_4: return (shift ? '$' : '4');
      case DIK_5: return (shift ? '%' : '5');
      case DIK_6: return (shift ? '^' : '6');
      case DIK_7: return (shift ? '&' : '7');
      case DIK_8: return (shift ? '*' : '8');
      case DIK_9: return (shift ? '(' : '9');
      case DIK_0: return (shift ? ')' : '0');
      case DIK_GRAVE: return (shift ? '~' : '`');
      case DIK_MINUS: return (shift ? '_' : '-');
      case DIK_EQUALS: return (shift ? '+' : '=');
      case DIK_LBRACKET: return (shift ? '{' : '[');
      case DIK_RBRACKET: return (shift ? '}' : ']');
      case DIK_BACKSLASH: return (shift ? '|' : '\\');
      case DIK_SEMICOLON: return (shift ? ':' : ';');
      case DIK_APOSTROPHE: return (shift ? '"' : '\'');
      case DIK_COMMA: return (shift ? '<' : ',');
      case DIK_PERIOD: return (shift ? '>' : '.');
      case DIK_SLASH: return (shift ? '?' : '/');
      case DIK_SPACE: return ' ';
      case DIK_A: return value + 'a';
      case DIK_B: return value + 'b';
      case DIK_C: return value + 'c';
      case DIK_D: return value + 'd';
      case DIK_E: return value + 'e';
      case DIK_F: return value + 'f';
      case DIK_G: return value + 'g';
      case DIK_H: return value + 'h';
      case DIK_I: return value + 'i';
      case DIK_J: return value + 'j';
      case DIK_K: return value + 'k';
      case DIK_L: return value + 'l';
      case DIK_M: return value + 'm';
      case DIK_N: return value + 'n';
      case DIK_O: return value + 'o';
      case DIK_P: return value + 'p';
      case DIK_Q: return value + 'q';
      case DIK_R: return value + 'r';
      case DIK_S: return value + 's';
      case DIK_T: return value + 't';
      case DIK_U: return value + 'u';
      case DIK_V: return value + 'v';
      case DIK_W: return value + 'w';
      case DIK_X: return value + 'x';
      case DIK_Y: return value + 'y';
      case DIK_Z: return value + 'z';
   }

   return 0;
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
   // Obtain keyboard
   Try(stCreateDev, di8->CreateDevice(GUID_SysKeyboard, &di8Key, NULL));
   Try(stSetKeyFormat, di8Key->SetDataFormat(&c_dfDIKeyboard));
   //   Try(stSetKeyCoop, di8Key->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE));
   Try(stSetKeyCoop, di8Key->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE));
   Try(stKeyAcquire, di8Key->Acquire());
   // Obtain mouse
   Try(stCreateDev, di8->CreateDevice(GUID_SysMouse, &di8Mse, NULL));
   Try(stSetMseFormat, di8Mse->SetDataFormat(&dfMouse38));
   dpw.diph.dwHeaderSize = sizeof(dpw.diph);
   dpw.diph.dwHow = DIPH_DEVICE;
   dpw.diph.dwObj = NULL;
   dpw.diph.dwSize = sizeof(dpw);
   dpw.dwData = DIPROPAXISMODE_REL;
   Try(stSetMseProperty, di8Mse->SetProperty(DIPROP_AXISMODE,&dpw.diph));
//   Try(stSetMseCoop, di8Mse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE));
   Try(stSetMseCoop, di8Mse->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE));
   PollMouse();
   // Obtain gamepads
   Try(stEnumGamepads, di8->EnumDevices(DI8DEVCLASS_GAMECTRL, GamepadEnumeration, NULL, DIEDFL_ATTACHEDONLY));
   for(i = 0; i < gamepadCount; i++) {
      Try(stSetPadFormat, di8Pad[i]->SetDataFormat(&c_dfDIJoystick));
      Try(stSetPadCoop, di8Pad[i]->SetCooperativeLevel(NULL, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE));
   }
   do {
      threadLife = THREAD_LIFE;
      if (threadLife & INPUT_THREAD_RESET) goto Reinitialise_;

      // Stall if no state change requested
      if(!(gcv.misc[7] & 0x080)) {
         Sleep(1);
         _mm_pause(); // To flush branch prediction. Sleep(0 || user defineable)???
         continue;
      }
      gcv.misc[7] &= 0x07F;

      // Backup state changes
      Copy32(keyState[0], keyState[1], 256u);
      Copy8(&mseState[0], &mseState[1], 24u);
      Copy16(padState[0], padState[1], 640u);

#ifdef __AVX__
      // Clear button states
      gcvLocal.imm.button = _mm256_setzero_si256();
      gcvLocal.rel.button = _mm256_setzero_si256();
      // Clear joypad states
      for(i = 0; i < 6; i++) gcvLocal.faxis32[i] = _mm256_setzero_ps();
#else
      // Clear button states
      gcvLocal.imm[0].button = _mm_setzero_si128();
      gcvLocal.imm[1].button = _mm_setzero_si128();
      gcvLocal.rel[0].button = _mm_setzero_si128();
      gcvLocal.rel[1].button = _mm_setzero_si128();
      // Clear joypad states
      for(i = 0; i < 12; i++) gcvLocal.faxis16[i] = _mm_setzero_ps();
#endif

      PollKeyboard();
      PollMouse();
      PollGamepads();

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
