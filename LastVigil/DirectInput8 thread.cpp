/************************************************************
 * File: DirectInput8 thread.cpp        Created: 2022/11/01 *
 *                                Last modified: 2024/06/06 *
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

BOOL CALLBACK GamepadEnumeration(const DIDEVICEINSTANCE *inst, ptrc con) {
   csi32 result = di8->CreateDevice(inst->guidInstance, &di8Pad[padCount], NULL);

   if FAILED(result) return DIENUM_CONTINUE;
   else {
      Try(stSetPadFormat, di8Pad[padCount]->SetDataFormat(&dfGamepadAE));
      Try(stSetPadCoop, di8Pad[padCount]->SetCooperativeLevel(NULL, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE));
      //      Try(stSetPadCoop, di8Pad[gamepadCount]->SetCooperativeLevel(NULL, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE));
      padCount++;
   }

   if(padCount >= 8) return DIENUM_STOP;

   return DIENUM_CONTINUE;
}

inline static void ReorderGamepad(cui8 hardwareIndex, cui8 relativeIndex) {
   if(padCount <= hardwareIndex) return;

   ui64 &gpo = (ui64 &)padOrder;

   gpo = (gpo & (0x0FFFFFFFFFFFFFFFFu >> (64u - (hardwareIndex << 3u)))) | (gpo << ((hardwareIndex << 3u) + 8u)) | (ui64(relativeIndex) << (hardwareIndex << 3u));
}

inline static void ReorderGamepads(cVEC8Du8 padIndices) { (ui64 &)padOrder = (ui64 &)padIndices; }

inline static void ReorderGamepads(cui8ptr padIndices) { *(ui64ptr)padOrder = *(ui64ptr)padIndices; }

inline static void ReorderGamepads(cui64 padIndices) { (ui64 &)padOrder = padIndices; }

inline static void ReorderGamepads(cui8 pad0, cui8 pad1, cui8 pad2, cui8 pad3, cui8 pad4, cui8 pad5, cui8 pad6, cui8 pad7) {
   padOrder[0] = pad0; padOrder[1] = pad1; padOrder[2] = pad2; padOrder[3] = pad3;
   padOrder[4] = pad4; padOrder[5] = pad5; padOrder[6] = pad6; padOrder[7] = pad7;
}

inline static void SetGamepadShaping(cVEC8Df padShapes) { Copy(&padShapes, &padShaping, 32u); }

inline static void SetGamepadShaping(cfl32ptr padShapes) { Copy(padShapes, &padShaping, 32u); }

inline static void SetGamepadShaping(cAVX8Df32 padShapes) { padShaping = padShapes; }

inline static void SetGamepadShaping(cfl32 padShape0, cfl32 padShape1, cfl32 padShape2, cfl32 padShape3, cfl32 padShape4, cfl32 padShape5, cfl32 padShape6, cfl32 padShape7, cfl32 padShape8) {
   padShaping._fl[0] = padShape0; padShaping._fl[1] = padShape1; padShaping._fl[2] = padShape2; padShaping._fl[3] = padShape3;
   padShaping._fl[4] = padShape4; padShaping._fl[5] = padShape5; padShaping._fl[6] = padShape6; padShaping._fl[7] = padShape7;
}

inline static HRESULT PollKeyboard(void) {
   hr = di8Key->GetDeviceState(sizeof(keyState[0]), (ptr)keyState[0]);

   if(FAILED(hr))
      if((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED))
         di8Key->Acquire();
      else
         Try(stGetKeyDevState, -2);

   return hr;
}

inline static HRESULT PollMouse(void) {
   hr = di8Mse->Poll();

   if(FAILED(hr)) do hr = di8Mse->Acquire(); while(hr == DIERR_INPUTLOST);

   Try(stPollMouse, di8Mse->GetDeviceState(sizeof(DIMOUSESTATE38), &mseState[0]));

   // Process axes
   GetCursorPos((LPPOINT)gcvLocal.curCoord);
   ScreenToClient(hWnd, (LPPOINT)gcvLocal.curCoord);

   gcvLocal.mouse.x = float(mseState[0].x) * mouseScale;
   gcvLocal.mouse.y = float(mseState[0].y) * mouseScale;
   gcvLocal.mouse.z += mseState[0].z / 120;
   gcvLocal.mouse.w += mseState[0].w / 120;

   return hr;
}

inline static HRESULT PollGamepads(void) {
#ifdef __AVX__
   for(ui32 os = 0, index = padOrder[0]; index < padCount; index = padOrder[++os]) {
      hr = di8Pad[index]->Poll();

      if(FAILED(hr)) do hr = di8Pad[index]->Acquire(); while(hr == DIERR_INPUTLOST);

      Try(stPollGamepad, di8Pad[index]->GetDeviceState(sizeof(DIJOYSTATEAE), &padState[0][index]));

      csi256 axes    = _mm256_sub_epi32(padState[0][index].ymm[0], padOS[index]);
      csi256 zone[2] = { _mm256_set1_epi32(padDeadZone.vec2D[index].x), _mm256_set1_epi32(padDeadZone.vec2D[index].y) };

      // Center & clip according to dead zone limits
      gcvLocal.iaxis32[index] = _mm256_andnot_epi32(_mm256_cmpeq_epi32(_mm256_cmpgt_epi32(axes, zone[0]), _mm256_cmpgt_epi32(zone[1], axes)), axes);

      // Reduce axes by deadzone values if non-zero
      gcvLocal.iaxis32[index] = _mm256_sub_epi32(gcvLocal.iaxis32[index], _mm256_and_epi32(_mm256_cmpgt_epi32(gcvLocal.iaxis32[index], null256), zone[1]));
      gcvLocal.iaxis32[index] = _mm256_sub_epi32(gcvLocal.iaxis32[index], _mm256_and_epi32(_mm256_cmpgt_epi32(null256, gcvLocal.iaxis32[index]), zone[0]));

      // Convert axes to floats
      gcvLocal.faxis32[index] = _mm256_mul_ps(_mm256_cvtepi32_ps(gcvLocal.iaxis32[index]), padScale[index]);

      // Reshape axes
      gcvLocal.faxis32[index] = _mm256_add_ps(_mm256_mul_ps(gcvLocal.faxis32[index], _mm256_sub_ps(ones32x8f, padShaping.ymm)),
                                              _mm256_mul_ps(_mm256_mul_ps(gcvLocal.faxis32[index], gcvLocal.faxis32[index]), padShaping.ymm));
   }
#else
   for(ui32 os = 0, index = gamepadOrder[0], index_1 = gamepadOrder[1]; index < (gamepadCount << 1u); index = gamepadOrder[i += 2], index_1 = gamepadOrder[i + 1]) {
      hr = di8Pad[index]->Poll();

      if(FAILED(hr)) do hr = di8Pad[index]->Acquire(); while(hr == DIERR_INPUTLOST);

      Try(stPollGamepad, di8Pad[index]->GetDeviceState(sizeof(DIJOYSTATEAE), &padState[0][index]));

      cui32  i       = index >> 1u;
      csi128 axes[2] = _mm_sub_epi32(padState[0][i].xmm[0], padOS[i]);
      csi128 zone[2] = { _mm_set1_epi32(padDeadZone.vec2D[i].x), _mm_set1_epi32(padDeadZone.vec2D[i].y) };

      // Center & clip according to dead zone limits
      gcvLocal.iaxis16[index]   = _mm_andnot_epi32(_mm_cmpeq_epi32(_mm_cmpgt_epi32(axes[0], zone[0]), _mm_cmpgt_epi32(zone[1], axes[0])), axes[0]);
      gcvLocal.iaxis16[index_1] = _mm_andnot_epi32(_mm_cmpeq_epi32(_mm_cmpgt_epi32(axes[1], zone[0]), _mm_cmpgt_epi32(zone[1], axes[1])), axes[1]);

      // Reduce axes by deadzone values if non-zero
      gcvLocal.iaxis16[index]   = _mm_sub_epi32(gcvLocal.iaxis16[index], _mm_and_epi32(_mm_cmpgt_epi32(gcvLocal.iaxis16[index], null128), zone[1]));
      gcvLocal.iaxis16[index]   = _mm_sub_epi32(gcvLocal.iaxis16[index], _mm_and_epi32(_mm_cmpgt_epi32(null128, gcvLocal.iaxis16[index]), zone[0]));
      gcvLocal.iaxis16[index_1] = _mm_sub_epi32(gcvLocal.iaxis16[index_1], _mm_and_epi32(_mm_cmpgt_epi32(gcvLocal.iaxis16[index_1], null128), zone[1]));
      gcvLocal.iaxis16[index_1] = _mm_sub_epi32(gcvLocal.iaxis16[index_1], _mm_and_epi32(_mm_cmpgt_epi32(null128, gcvLocal.iaxis16[index_1]), zone[0]));

      // Convert axes to floats
      gcvLocal.faxis16[index]   = _mm_mul_ps(_mm_cvtepi32_ps(gcvLocal.iaxis16[index]), padScale[i][0]);
      gcvLocal.faxis16[index_1] = _mm_mul_ps(_mm_cvtepi32_ps(gcvLocal.iaxis16[index_1]), padScale[i][1]);

      // Reshape axes
      gcvLocal.faxis16[index]   = _mm_add_ps(_mm_mul_ps(gcvLocal.faxis16[index], _mm_sub_ps(ones32x4f, padShaping.xmm[0])),
                                             _mm_mul_ps(_mm_mul_ps(gcvLocal.faxis16[index], gcvLocal.faxis16[index]), padShaping.xmm[0]));
      gcvLocal.faxis16[index_1] = _mm_add_ps(_mm_mul_ps(gcvLocal.faxis16[index_1], _mm_sub_ps(ones32x4f, padShaping.xmm[1])),
                                             _mm_mul_ps(_mm_mul_ps(gcvLocal.faxis16[index_1], gcvLocal.faxis16[index_1]), padShaping.xmm[1]));
   }
#endif

   return hr;
}

inline static HRESULT PollGamepad(cui32 gamepadIndex) {
#ifdef __AVX__
   hr = di8Pad[gamepadIndex]->Poll();

   if(FAILED(hr))
      do hr = di8Pad[gamepadIndex]->Acquire(); while(hr == DIERR_INPUTLOST);

   Try(stPollGamepad, di8Pad[gamepadIndex]->GetDeviceState(sizeof(DIJOYSTATEAE), &padState[0][gamepadIndex]));

   csi256 axes    = _mm256_sub_epi32(padState[0][gamepadIndex].ymm[0], padOS[gamepadIndex]);
   csi256 zone[2] = { _mm256_set1_epi32(padDeadZone.vec2D[gamepadIndex].x), _mm256_set1_epi32(padDeadZone.vec2D[gamepadIndex].y) };

   // Center & clip according to dead zone limits
   gcvLocal.iaxis32[gamepadIndex] = _mm256_andnot_epi32(_mm256_cmpeq_epi32(_mm256_cmpgt_epi32(axes, zone[0]), _mm256_cmpgt_epi32(zone[1], axes)), axes);

   // Reduce axes by deadzone values if non-zero
   gcvLocal.iaxis32[gamepadIndex] = _mm256_sub_epi32(gcvLocal.iaxis32[gamepadIndex], _mm256_and_epi32(_mm256_cmpgt_epi32(gcvLocal.iaxis32[gamepadIndex], null256), zone[1]));
   gcvLocal.iaxis32[gamepadIndex] = _mm256_sub_epi32(gcvLocal.iaxis32[gamepadIndex], _mm256_and_epi32(_mm256_cmpgt_epi32(null256, gcvLocal.iaxis32[gamepadIndex]), zone[0]));

   // Convert axes to floats
   gcvLocal.faxis32[gamepadIndex] = _mm256_mul_ps(_mm256_cvtepi32_ps(gcvLocal.iaxis32[gamepadIndex]), padScale[gamepadIndex]);
#else
   hr = di8Pad[index]->Poll();

   if(FAILED(hr))
      do hr = di8Pad[gamepadIndex]->Acquire(); while(hr == DIERR_INPUTLOST);

   Try(stPollGamepad, di8Pad[gamepadIndex]->GetDeviceState(sizeof(DIJOYSTATEAE), &padState[0][gamepadIndex]));

   cui32  index   = gamepadIndex << 1u
   cui32  index_1 = index + 1;
   csi128 axes[2] = _mm_sub_epi32(padState[0][gamepadIndex].xmm[0], padOS[gamepadIndex]);
   csi128 zone[2] = { _mm_set1_epi32(padDeadZone.vec2D[gamepadIndex].x), _mm_set1_epi32(padDeadZone.vec2D[gamepadIndex].y) };

   // Center & clip according to dead zone limits
   gcvLocal.iaxis16[index]   = _mm_andnot_epi32(_mm_cmpeq_epi32(_mm_cmpgt_epi32(axes[0], zone[0]), _mm_cmpgt_epi32(zone[1], axes[0])), axes[0]);
   gcvLocal.iaxis16[index_1] = _mm_andnot_epi32(_mm_cmpeq_epi32(_mm_cmpgt_epi32(axes[1], zone[0]), _mm_cmpgt_epi32(zone[1], axes[1])), axes[1]);

   // Reduce axes by deadzone values if non-zero
   gcvLocal.iaxis16[index]   = _mm_sub_epi32(gcvLocal.iaxis16[index], _mm_and_epi32(_mm_cmpgt_epi32(gcvLocal.iaxis16[index], null128), zone[1]));
   gcvLocal.iaxis16[index]   = _mm_sub_epi32(gcvLocal.iaxis16[index], _mm_and_epi32(_mm_cmpgt_epi32(null128, gcvLocal.iaxis16[index]), zone[0]));
   gcvLocal.iaxis16[index_1] = _mm_sub_epi32(gcvLocal.iaxis16[index_1], _mm_and_epi32(_mm_cmpgt_epi32(gcvLocal.iaxis16[index_1], null128), zone[1]));
   gcvLocal.iaxis16[index_1] = _mm_sub_epi32(gcvLocal.iaxis16[index_1], _mm_and_epi32(_mm_cmpgt_epi32(null128, gcvLocal.iaxis16[index_1]), zone[0]));

   // Convert axes to floats
   gcvLocal.faxis16[index]   = _mm_mul_ps(_mm_cvtepi32_ps(gcvLocal.iaxis16[index]), padScale[gamepadIndex]);
   gcvLocal.faxis16[index_1] = _mm_mul_ps(_mm_cvtepi32_ps(gcvLocal.iaxis16[index_1]), padScale[gamepadIndex]);
#endif

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
   DIPROPDWORD dpw {};

   ui64 threadLife, i, j, k;
   ui16 keyCode;
   bool povState;

   gcvLocalPtr = &gcvLocal;

   // Prevent thread from shutting down (after engine reset)
   THREAD_LIFE &= ~INPUT_THREAD_DIED;

Reinitialise_:

   THREAD_LIFE &= ~INPUT_THREAD_RESET;

   Sleep(1000);

   Try(stDI8Create, DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (ptrptr)&di8, NULL));
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
      Copy32(padState[0], padState[1], 768u);

#ifdef __AVX__
      // Clear button states
      gcvLocal.imm.key    = _mm256_setzero_si256();
      gcvLocal.imm.button = _mm256_setzero_si256();
      gcvLocal.rel.key    = _mm256_setzero_si256();
      gcvLocal.rel.button = _mm256_setzero_si256();
#else
      // Clear button states
      gcvLocal.imm[0].key = _mm_setzero_si128();
      gcvLocal.imm[1].key    = _mm_setzero_si128();
      gcvLocal.imm[0].button = _mm_setzero_si128();
      gcvLocal.imm[1].button = _mm_setzero_si128();
      gcvLocal.rel[0].key    = _mm_setzero_si128();
      gcvLocal.rel[1].key    = _mm_setzero_si128();
      gcvLocal.rel[0].button = _mm_setzero_si128();
      gcvLocal.rel[1].button = _mm_setzero_si128();
#endif

      PollKeyboard();
      PollMouse();
      PollGamepads();

      // Populate input arrays with keyboard data
      for(i = 0; i < 0x080; i++) {
         if(keyState[0][i] & 0x080)
            gcvLocal.imm.k64[i >> 6] |= ui64(0x01) << (i & 0x03F);
         if(keyState[0][i] != keyState[1][i])
            gcvLocal.rel.k64[i >> 6] |= ui64(0x01) << (i & 0x03F);
      }
      if(keyState[0][0x08C] & 0x080)
         gcvLocal.imm.k64[2] |= 0x01000;
      if(keyState[0][0x08C] != keyState[1][i])
         gcvLocal.rel.k64[2] |= 0x01000;
      if(keyState[0][0x08D] & 0x080)
         gcvLocal.imm.k64[2] |= 0x02000;
      if(keyState[0][0x08D] != keyState[1][i])
         gcvLocal.rel.k64[2] |= 0x02000;
      for(i = 0x090; i < 0x0B9; i++) {
         if(keyState[0][i] & 0x080)
            gcvLocal.imm.k64[i >> 6] |= ui64(0x01) << (i & 0x03F);
         if(keyState[0][i] != keyState[1][i])
            gcvLocal.rel.k64[i >> 6] |= ui64(0x01) << (i & 0x03F);
      }
      for(i = 0x0C5; i < 0x0DD; i++) {
         if(keyState[0][i] & 0x080)
            gcvLocal.imm.k64[i >> 6] |= ui64(0x01) << (i & 0x03F);
         if(keyState[0][i] != keyState[1][i])
            gcvLocal.rel.k64[i >> 6] |= ui64(0x01) << (i & 0x03F);
      }
      // Populate input arrays with mouse data
      for(i = 0; i < 8; i++) {
         gcvLocal.imm.k[16] |= ui8(mseState[0].b[i] & 0x080u) >> (7 - i);
         gcvLocal.rel.k[16] |= ui8(mseState[0].b[i] != mseState[1].b[i]) << i;
      }
      // Populate input arrays with mouse wheel data
      gcvLocal.imm.k[21] |= ui8(mseState[0].z < 0);
      gcvLocal.rel.k[21] |= ui8(mseState[0].z != mseState[1].z);
      gcvLocal.imm.k[21] |= ui8(mseState[0].z > 0) << 1;
      gcvLocal.rel.k[21] |= ui8(mseState[0].z != mseState[1].z) << 1;
      gcvLocal.imm.k[21] |= ui8(mseState[0].w < 0) << 2;
      gcvLocal.rel.k[21] |= ui8(mseState[0].w != mseState[1].w) << 2;
      gcvLocal.imm.k[21] |= ui8(mseState[0].w > 0) << 3;
      gcvLocal.rel.k[21] |= ui8(mseState[0].w != mseState[1].w) << 3;
      // Populate input arrays with joypad data
      for(k = 28, j = 0; j < padCount; ++j, ++k) {
         // Even gamepads: POV 0; up
         povState = ((padState[0][j].pov[0] >= 31500u) && padState[0][j].pov[0] != 0x0FFFFFFFF) || (padState[0][j].pov[0] <= 4500u);
         gcvLocal.imm.k[k] |= (povState & 0x01u);
         gcvLocal.rel.k[k] |= (povState != (((padState[1][j].pov[0] >= 31500u) && padState[1][j].pov[0] != 0x0FFFFFFFF) || (padState[1][j].pov[0] <= 4500u)));
         // Even gamepads: POV 0; down
         povState = (padState[0][j].pov[0] >= 13500u) && (padState[0][j].pov[0] <= 22500u);
         gcvLocal.imm.k[k] |= (povState & 0x01u) << 1u;
         gcvLocal.rel.k[k] |= (povState != ((padState[1][j].pov[0] >= 13500u) && (padState[1][j].pov[0] <= 22500u))) << 1u;
         // Even gamepads: POV 0; right
         povState = (padState[0][j].pov[0] >= 4500u) && (padState[0][j].pov[0] <= 13500u);
         gcvLocal.imm.k[k] |= (povState & 0x01u) << 2u;
         gcvLocal.rel.k[k] |= (povState != ((padState[1][j].pov[0] >= 4500u) && (padState[1][j].pov[0] <= 13500u))) << 2u;
         // Even gamepads: POV 0; left
         povState = (padState[0][j].pov[0] >= 22500u) && (padState[0][j].pov[0] <= 31500u);
         gcvLocal.imm.k[k] |= (povState & 0x01u) << 3u;
         gcvLocal.rel.k[k] |= (povState != ((padState[1][j].pov[0] >= 22500u) && (padState[1][j].pov[0] <= 31500u))) << 3u;
         if(++j >= padCount) break;
         // Odd gamepads: POV 0; up
         povState = ((padState[0][j].pov[0] >= 31500u) && padState[0][j].pov[0] != 0x0FFFFFFFF) || (padState[0][j].pov[0] <= 4500u);
         gcvLocal.imm.k[k] |= (povState & 0x01u) << 4u;
         gcvLocal.rel.k[k] |= (povState != (((padState[1][j].pov[0] >= 31500u) && padState[1][j].pov[0] != 0x0FFFFFFFF) || (padState[1][j].pov[0] <= 4500u))) << 4u;
         // Odd gamepads: POV 0; down
         povState = (padState[0][j].pov[0] >= 13500u) && (padState[0][j].pov[0] <= 22500u);
         gcvLocal.imm.k[k] |= (povState & 0x01u) << 5u;
         gcvLocal.rel.k[k] |= (povState != ((padState[1][j].pov[0] >= 13500u) && (padState[1][j].pov[0] <= 22500u))) << 5u;
         // Odd gamepads: POV 0; right
         povState = (padState[0][j].pov[0] >= 4500u) && (padState[0][j].pov[0] <= 13500u);
         gcvLocal.imm.k[k] |= (povState & 0x01u) << 6u;
         gcvLocal.rel.k[k] |= (povState != ((padState[1][j].pov[0] >= 4500u) && (padState[1][j].pov[0] <= 13500u))) << 6u;
         // Odd gamepads: POV 0; left
         povState = (padState[0][j].pov[0] >= 22500u) && (padState[0][j].pov[0] <= 31500u);
         gcvLocal.imm.k[k] |= (povState & 0x01u) << 7u;
         gcvLocal.rel.k[k] |= (povState != ((padState[1][j].pov[0] >= 22500u) && (padState[1][j].pov[0] <= 31500u))) << 7u;
      }
      for(j = 0; j < padCount; ++j) // Buttons
         for(k = 0; k < 4; ++k)
            for(i = 0; i < 8; ++i) {
               cui64 index = k * 8u + i;
               gcvLocal.imm.b[j * 4u + k] |= (padState[0][j].button[index] & 0x080u) >> (7 - i);
               gcvLocal.rel.b[j * 4u + k] |= (padState[0][j].button[index] != padState[1][j].button[index]) << i;
            }

      // Send standard characters to text input buffer
      if(textBufferInfo.source.bitField & 0x01) ModifyCharBuffer(textBufferInfo);

      // Populate global input registers
      (ui64 &)inputsImmediate = 0x0;
      // Test first 256 entires
      for(i = 0, k = 0, keyCode = 0; i < 4 && k < 4; i++)
         for(j = 0x01u; j && k < 4; j <<= 1, keyCode++)
            if(gcvLocal.imm.k64[i] & j)
               inputsImmediate._ui16[k++] = keyCode;
      // Test last 256 entries
      for(i = 0; i < 4 && k < 4; i++)
         for(j = 0x01u; j && k < 4; j <<= 1, keyCode++)
            if(gcvLocal.imm.b64[i] & j)
               inputsImmediate._ui16[k++] = keyCode;

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
               gcvLocal.joy[2].s.y2 = -1.0f;
               break;
            case DIK_NUMPAD5:
               gcvLocal.joy[2].s.y2 = 1.0f;
               break;
            case DIK_NUMPAD4:
               gcvLocal.joy[2].s.x2 = -1.0f;
               break;
            case DIK_NUMPAD6:
               gcvLocal.joy[2].s.x2 = 1.0f;
               break;
            case DIK_NUMPAD7:
               gcvLocal.joy[2].t.x = 1.0f;
               break;
            case DIK_NUMPAD0:
               gcvLocal.joy[2].t.x = -1.0f;
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
