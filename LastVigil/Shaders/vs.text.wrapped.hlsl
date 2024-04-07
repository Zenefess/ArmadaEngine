/************************************************************
 * File: vs.text.wrapped.hlsl           Created: 2023/01/22 *
 *                                Last modified: 2023/01/25 *
 *                                                          *
 * Desc: 128 characters per instance.                       *
 *                                                          *
 *  Copyright (c) David William Bull. All rights reserved.  *
 ************************************************************/

struct CHAR_IVS {   // 16 bytes
   uint2 tc;      // Texture coordinates : 4x(1p15)
   float width;   // Relative X dimension
   float xos;      // Relative X offset
};

struct VIn {   // 40 bytes
   float4 textCol   : TEXTCOLOUR;   // RGBA
   float2 origin    : ORIGIN;         // Absolute top-left coordinate
   float2 charScale : SCALE;
   float  rotAngle  : ROTATION;
   uint   taos_bf   : OFFSET;         // 0-15==Offset (>> 4) into structured buffer, 16==Origin set to top of viewport, 17-31==???
};

struct VOut {   // 80 bytes
   uint4 offset[4]   : OFFSETS;   // X coord offsets : 16x2(float16)
   uint  origin      : COORDS;   // 0-15==X offset, 16-31==Y offset : float16
   uint    size        : SIZE;      // 0-15==X scalar, 16-31==Y scalar : float16
   uint  colour      : COLOUR;   // RGBA : 0p8/9/7/8n
   uint  textArrayOS : TEXTOS;   // 0-30==Offset (>> 4) into structured buffer, 31==Y origin set to top of viewport
};

const StructuredBuffer<CHAR_IVS> alphabet;
      StructuredBuffer<uint4>    char16;   // Text data array

VOut main(in const VIn input, in const uint segment : SV_VertexID) {
   const uint bitField = input.taos_bf >> 16;
   const uint tAOS     = input.taos_bf & 0x0FFFF;
   const uint textOS   = tAOS + (segment << 3);

   const float2 coMod[2] = { input.origin * 2.0f - 1.0f, input.origin * -2.0f + 1.0f };
   const uint   coords   = f32tof16(input.origin.x * 2.0f - 1.0f) | (f32tof16(bitField & 0x01 ? coMod[1].y : coMod[0].y) << 16);

   const float4 modMultis   = float4(255.0f, 511.0f, 127.0f, 255.0f);
   const uint4  modMasks    = uint4(0x0FF, 0x01FF, 0x07F, 0x0FF);
   const uint4  modShifts   = uint4(0, 8, 17, 24);
   const uint4  textColours = (uint4(input.textCol * modMultis) & modMasks) << modShifts;
   const uint   colour      = textColours.x | textColours.y | textColours.z | textColours.w;

   const uint size = f32tof16(input.charScale.x) | (f32tof16(input.charScale.y) << 16);

   uint4 offsets[4];
   float charOS = 0.0f;

   [unroll]   // Calculate screen-space offset for every 4th character
   for(uint i = 0; i < 8; i++) {
      const uint  vec      = i >> 1;
      const uint  comp     = (i & 0x01) << 1;
      const uint  index    = textOS + i;
      const uint4 char4[4] = { (char16[index].xxxx >> uint4(0, 8, 16, 24)) & 0x0FF, (char16[index].yyyy >> uint4(0, 8, 16, 24)) & 0x0FF,
                               (char16[index].zzzz >> uint4(0, 8, 16, 24)) & 0x0FF, (char16[index].wwww >> uint4(0, 8, 16, 24)) & 0x0FF };

      if(!char4[0].x) break;

      charOS += alphabet[char4[0].x].xos;
      offsets[vec][comp] = f32tof16(charOS);
      charOS += alphabet[char4[0].x].width + alphabet[char4[0].y].width + alphabet[char4[0].z].width + alphabet[char4[0].w].width;
      charOS += alphabet[char4[0].y].xos + alphabet[char4[0].z].xos + alphabet[char4[0].w].xos + alphabet[char4[1].x].xos;
      offsets[vec][comp] |= f32tof16(charOS) << 16;
      charOS += alphabet[char4[1].x].width + alphabet[char4[1].y].width + alphabet[char4[1].z].width + alphabet[char4[1].w].width;
      charOS += alphabet[char4[1].y].xos + alphabet[char4[1].z].xos + alphabet[char4[1].w].xos + alphabet[char4[2].x].xos;
      offsets[vec][comp + 1] = f32tof16(charOS);
      charOS += alphabet[char4[2].x].width + alphabet[char4[2].y].width + alphabet[char4[2].z].width + alphabet[char4[2].w].width;
      charOS += alphabet[char4[2].y].xos + alphabet[char4[2].z].xos + alphabet[char4[2].w].xos + alphabet[char4[3].x].xos;
      offsets[vec][comp + 1] |= f32tof16(charOS) << 16;
      charOS += alphabet[char4[3].x].width + alphabet[char4[3].y].width + alphabet[char4[3].z].width + alphabet[char4[3].w].width;
      charOS += alphabet[char4[3].y].xos + alphabet[char4[3].z].xos + alphabet[char4[3].w].xos;
   }

   const VOut output = { offsets, coords, size, colour, tAOS + ((bitField & 0x01) << 31) };
   return output;
}
