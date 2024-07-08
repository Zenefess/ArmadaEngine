/**************************************************************
 * File: ps.full register access.hlsli    Created: 2023/04/14 *
 *                                      Last mod.: 2023/06/26 *
 *                                                            *
 * Desc: Functions for sampling across all 128 registers.     *
 *                                                            *
 * Copyright (c) David William Bull.     All rights reserved. *
 **************************************************************/

//#define FULL_LIBRARY

const Texture2D    Texture[128] : register(t0); // Atlas library: t0~t15==Miscellaneous textures, t16-t79==Terrain & sprite, t80~t127==GUI atlas'
const SamplerState Sampler[3]   : register(s0); // s0==Point filering (wrapped), s1==Point magnification (clamped), s2==Anisotropic filtering (wrapped)

// index.x==(Relative) texture register, index.y==Sampler register
const float4 SampleMiscellaneous(in const int2 index, in const float2 texCoord) {
   [forcecase] switch(index.x) {
   default:
      return Texture[0].Sample(Sampler[index.y], texCoord);
   case 1:
      return Texture[1].Sample(Sampler[index.y], texCoord);
   case 2:
      return Texture[2].Sample(Sampler[index.y], texCoord);
   case 3:
      return Texture[3].Sample(Sampler[index.y], texCoord);
#ifdef FULL_LIBRARY
   case 4:
      return Texture[4].Sample(Sampler[index.y], texCoord);
   case 5:
      return Texture[5].Sample(Sampler[index.y], texCoord);
   case 6:
      return Texture[6].Sample(Sampler[index.y], texCoord);
   case 7:
      return Texture[7].Sample(Sampler[index.y], texCoord);
   case 8:
      return Texture[8].Sample(Sampler[index.y], texCoord);
   case 9:
      return Texture[9].Sample(Sampler[index.y], texCoord);
   case 10:
      return Texture[10].Sample(Sampler[index.y], texCoord);
   case 11:
      return Texture[11].Sample(Sampler[index.y], texCoord);
   case 12:
      return Texture[12].Sample(Sampler[index.y], texCoord);
   case 13:
      return Texture[13].Sample(Sampler[index.y], texCoord);
   case 14:
      return Texture[14].Sample(Sampler[index.y], texCoord);
   case 15:
      return Texture[15].Sample(Sampler[index.y], texCoord);
#endif
   }
}

// index.x==(Relative) texture register, index.y==Sampler register
const float3x4 Sample3of4Terrain(in const int2 index, in const float2 texCoord) {
   float3x4 sample;

   [forcecase] switch(index.x) {
   default:
      sample[0] = Texture[16].Sample(Sampler[index.y], texCoord);
      sample[1] = Texture[17].Sample(Sampler[index.y], texCoord);
      sample[2] = Texture[18].Sample(Sampler[index.y], texCoord);
      return sample;
   case 20:
      sample[0] = Texture[20].Sample(Sampler[index.y], texCoord);
      sample[1] = Texture[21].Sample(Sampler[index.y], texCoord);
      sample[2] = Texture[22].Sample(Sampler[index.y], texCoord);
      return sample;
   case 24:
      sample[0] = Texture[24].Sample(Sampler[index.y], texCoord);
      sample[1] = Texture[25].Sample(Sampler[index.y], texCoord);
      sample[2] = Texture[26].Sample(Sampler[index.y], texCoord);
      return sample;
   case 28:
      sample[0] = Texture[28].Sample(Sampler[index.y], texCoord);
      sample[1] = Texture[29].Sample(Sampler[index.y], texCoord);
      sample[2] = Texture[30].Sample(Sampler[index.y], texCoord);
      return sample;
#ifdef FULL_LIBRARY
   case 32:
      sample[0] = Texture[32].Sample(Sampler[index.y], texCoord);
      sample[1] = Texture[33].Sample(Sampler[index.y], texCoord);
      sample[2] = Texture[34].Sample(Sampler[index.y], texCoord);
      return sample;
   case 36:
      sample[0] = Texture[36].Sample(Sampler[index.y], texCoord);
      sample[1] = Texture[37].Sample(Sampler[index.y], texCoord);
      sample[2] = Texture[38].Sample(Sampler[index.y], texCoord);
      return sample;
   case 40:
      sample[0] = Texture[40].Sample(Sampler[index.y], texCoord);
      sample[1] = Texture[41].Sample(Sampler[index.y], texCoord);
      sample[2] = Texture[42].Sample(Sampler[index.y], texCoord);
      return sample;
   case 44:
      sample[0] = Texture[44].Sample(Sampler[index.y], texCoord);
      sample[1] = Texture[45].Sample(Sampler[index.y], texCoord);
      sample[2] = Texture[46].Sample(Sampler[index.y], texCoord);
      return sample;
   case 48:
      sample[0] = Texture[48].Sample(Sampler[index.y], texCoord);
      sample[1] = Texture[49].Sample(Sampler[index.y], texCoord);
      sample[2] = Texture[50].Sample(Sampler[index.y], texCoord);
      return sample;
   case 52:
      sample[0] = Texture[52].Sample(Sampler[index.y], texCoord);
      sample[1] = Texture[53].Sample(Sampler[index.y], texCoord);
      sample[2] = Texture[54].Sample(Sampler[index.y], texCoord);
      return sample;
   case 56:
      sample[0] = Texture[56].Sample(Sampler[index.y], texCoord);
      sample[1] = Texture[57].Sample(Sampler[index.y], texCoord);
      sample[2] = Texture[58].Sample(Sampler[index.y], texCoord);
      return sample;
   case 60:
      sample[0] = Texture[60].Sample(Sampler[index.y], texCoord);
      sample[1] = Texture[61].Sample(Sampler[index.y], texCoord);
      sample[2] = Texture[62].Sample(Sampler[index.y], texCoord);
      return sample;
   case 64:
      sample[0] = Texture[64].Sample(Sampler[index.y], texCoord);
      sample[1] = Texture[65].Sample(Sampler[index.y], texCoord);
      sample[2] = Texture[66].Sample(Sampler[index.y], texCoord);
      return sample;
   case 68:
      sample[0] = Texture[68].Sample(Sampler[index.y], texCoord);
      sample[1] = Texture[69].Sample(Sampler[index.y], texCoord);
      sample[2] = Texture[70].Sample(Sampler[index.y], texCoord);
      return sample;
   case 72:
      sample[0] = Texture[72].Sample(Sampler[index.y], texCoord);
      sample[1] = Texture[73].Sample(Sampler[index.y], texCoord);
      sample[2] = Texture[74].Sample(Sampler[index.y], texCoord);
      return sample;
   case 76:
      sample[0] = Texture[76].Sample(Sampler[index.y], texCoord);
      sample[1] = Texture[77].Sample(Sampler[index.y], texCoord);
      sample[2] = Texture[78].Sample(Sampler[index.y], texCoord);
      return sample;
#endif
   }
}

// index.x==(Relative) texture register, index.y==Sampler register
const float3x4 Sample3of4Sprite(in const int2 index, in const float2 texCoord) {
   float3x4 sample;

   [forcecase] switch(index.x) {
   default:
      sample[0] = Texture[16].Sample(Sampler[index.y], texCoord);
      sample[1] = Texture[17].Sample(Sampler[index.y], texCoord);
      sample[2] = Texture[18].Sample(Sampler[index.y], texCoord);
      return sample;
   case 20:
      sample[0] = Texture[20].Sample(Sampler[index.y], texCoord);
      sample[1] = Texture[21].Sample(Sampler[index.y], texCoord);
      sample[2] = Texture[22].Sample(Sampler[index.y], texCoord);
      return sample;
   case 24:
      sample[0] = Texture[24].Sample(Sampler[index.y], texCoord);
      sample[1] = Texture[25].Sample(Sampler[index.y], texCoord);
      sample[2] = Texture[26].Sample(Sampler[index.y], texCoord);
      return sample;
   case 28:
      sample[0] = Texture[28].Sample(Sampler[index.y], texCoord);
      sample[1] = Texture[29].Sample(Sampler[index.y], texCoord);
      sample[2] = Texture[30].Sample(Sampler[index.y], texCoord);
      return sample;
#ifdef FULL_LIBRARY
   case 32:
      sample[0] = Texture[32].Sample(Sampler[index.y], texCoord);
      sample[1] = Texture[33].Sample(Sampler[index.y], texCoord);
      sample[2] = Texture[34].Sample(Sampler[index.y], texCoord);
      return sample;
   case 36:
      sample[0] = Texture[36].Sample(Sampler[index.y], texCoord);
      sample[1] = Texture[37].Sample(Sampler[index.y], texCoord);
      sample[2] = Texture[38].Sample(Sampler[index.y], texCoord);
      return sample;
   case 40:
      sample[0] = Texture[40].Sample(Sampler[index.y], texCoord);
      sample[1] = Texture[41].Sample(Sampler[index.y], texCoord);
      sample[2] = Texture[42].Sample(Sampler[index.y], texCoord);
      return sample;
   case 44:
      sample[0] = Texture[44].Sample(Sampler[index.y], texCoord);
      sample[1] = Texture[45].Sample(Sampler[index.y], texCoord);
      sample[2] = Texture[46].Sample(Sampler[index.y], texCoord);
      return sample;
   case 48:
      sample[0] = Texture[48].Sample(Sampler[index.y], texCoord);
      sample[1] = Texture[49].Sample(Sampler[index.y], texCoord);
      sample[2] = Texture[50].Sample(Sampler[index.y], texCoord);
      return sample;
   case 52:
      sample[0] = Texture[52].Sample(Sampler[index.y], texCoord);
      sample[1] = Texture[53].Sample(Sampler[index.y], texCoord);
      sample[2] = Texture[54].Sample(Sampler[index.y], texCoord);
      return sample;
   case 56:
      sample[0] = Texture[56].Sample(Sampler[index.y], texCoord);
      sample[1] = Texture[57].Sample(Sampler[index.y], texCoord);
      sample[2] = Texture[58].Sample(Sampler[index.y], texCoord);
      return sample;
   case 60:
      sample[0] = Texture[60].Sample(Sampler[index.y], texCoord);
      sample[1] = Texture[61].Sample(Sampler[index.y], texCoord);
      sample[2] = Texture[62].Sample(Sampler[index.y], texCoord);
      return sample;
   case 64:
      sample[0] = Texture[64].Sample(Sampler[index.y], texCoord);
      sample[1] = Texture[65].Sample(Sampler[index.y], texCoord);
      sample[2] = Texture[66].Sample(Sampler[index.y], texCoord);
      return sample;
   case 68:
      sample[0] = Texture[68].Sample(Sampler[index.y], texCoord);
      sample[1] = Texture[69].Sample(Sampler[index.y], texCoord);
      sample[2] = Texture[70].Sample(Sampler[index.y], texCoord);
      return sample;
   case 72:
      sample[0] = Texture[72].Sample(Sampler[index.y], texCoord);
      sample[1] = Texture[73].Sample(Sampler[index.y], texCoord);
      sample[2] = Texture[74].Sample(Sampler[index.y], texCoord);
      return sample;
   case 76:
      sample[0] = Texture[76].Sample(Sampler[index.y], texCoord);
      sample[1] = Texture[77].Sample(Sampler[index.y], texCoord);
      sample[2] = Texture[78].Sample(Sampler[index.y], texCoord);
      return sample;
#endif
   }
}

const float4 Sample1of2GUI(in const uint2 index, in const float2 texCoord) {
   [forcecase] switch(index.x) {
   default:
      return Texture[80].Sample(Sampler[index.y], texCoord);
   case 82:
      return Texture[82].Sample(Sampler[index.y], texCoord);
   case 84:
      return Texture[84].Sample(Sampler[index.y], texCoord);
   case 86:
      return Texture[86].Sample(Sampler[index.y], texCoord);
#ifdef FULL_LIBRARY
   case 88:
      return Texture[88].Sample(Sampler[index.y], texCoord);
   case 90:
      return Texture[90].Sample(Sampler[index.y], texCoord);
   case 92:
      return Texture[92].Sample(Sampler[index.y], texCoord);
   case 94:
      return Texture[94].Sample(Sampler[index.y], texCoord);
   case 96:
      return Texture[96].Sample(Sampler[index.y], texCoord);
   case 98:
      return Texture[98].Sample(Sampler[index.y], texCoord);
   case 100:
      return Texture[100].Sample(Sampler[index.y], texCoord);
   case 102:
      return Texture[102].Sample(Sampler[index.y], texCoord);
   case 104:
      return Texture[104].Sample(Sampler[index.y], texCoord);
   case 106:
      return Texture[106].Sample(Sampler[index.y], texCoord);
   case 108:
      return Texture[108].Sample(Sampler[index.y], texCoord);
   case 110:
      return Texture[110].Sample(Sampler[index.y], texCoord);
   case 112:
      return Texture[112].Sample(Sampler[index.y], texCoord);
   case 114:
      return Texture[114].Sample(Sampler[index.y], texCoord);
   case 116:
      return Texture[116].Sample(Sampler[index.y], texCoord);
   case 118:
      return Texture[118].Sample(Sampler[index.y], texCoord);
   case 120:
      return Texture[120].Sample(Sampler[index.y], texCoord);
   case 122:
      return Texture[122].Sample(Sampler[index.y], texCoord);
   case 124:
      return Texture[124].Sample(Sampler[index.y], texCoord);
   case 126:
      return Texture[126].Sample(Sampler[index.y], texCoord);
#endif
   }
}

// index.x==(Relative) texture register, index.y==Sampler register
const float2x4 Sample2GUI(in const uint2 index, in const float2 texCoord) {
   float2x4 sample;

   [forcecase] switch(index.x) {
   default:
      sample[0] = Texture[80].Sample(Sampler[index.y], texCoord);
      sample[1] = Texture[81].Sample(Sampler[index.y], texCoord);
      return sample;
   case 82:
      sample[0] = Texture[82].Sample(Sampler[index.y], texCoord);
      sample[1] = Texture[83].Sample(Sampler[index.y], texCoord);
      return sample;
   case 84:
      sample[0] = Texture[84].Sample(Sampler[index.y], texCoord);
      sample[1] = Texture[85].Sample(Sampler[index.y], texCoord);
      return sample;
   case 86:
      sample[0] = Texture[86].Sample(Sampler[index.y], texCoord);
      sample[1] = Texture[87].Sample(Sampler[index.y], texCoord);
      return sample;
#ifdef FULL_LIBRARY
   case 88:
      sample[0] = Texture[88].Sample(Sampler[index.y], texCoord);
      sample[1] = Texture[89].Sample(Sampler[index.y], texCoord);
      return sample;
   case 90:
      sample[0] = Texture[90].Sample(Sampler[index.y], texCoord);
      sample[1] = Texture[91].Sample(Sampler[index.y], texCoord);
      return sample;
   case 92:
      sample[0] = Texture[92].Sample(Sampler[index.y], texCoord);
      sample[1] = Texture[93].Sample(Sampler[index.y], texCoord);
      return sample;
   case 94:
      sample[0] = Texture[94].Sample(Sampler[index.y], texCoord);
      sample[1] = Texture[95].Sample(Sampler[index.y], texCoord);
      return sample;
   case 96:
      sample[0] = Texture[96].Sample(Sampler[index.y], texCoord);
      sample[1] = Texture[97].Sample(Sampler[index.y], texCoord);
      return sample;
   case 98:
      sample[0] = Texture[98].Sample(Sampler[index.y], texCoord);
      sample[1] = Texture[99].Sample(Sampler[index.y], texCoord);
      return sample;
   case 100:
      sample[0] = Texture[100].Sample(Sampler[index.y], texCoord);
      sample[1] = Texture[101].Sample(Sampler[index.y], texCoord);
      return sample;
   case 102:
      sample[0] = Texture[102].Sample(Sampler[index.y], texCoord);
      sample[1] = Texture[103].Sample(Sampler[index.y], texCoord);
      return sample;
   case 104:
      sample[0] = Texture[104].Sample(Sampler[index.y], texCoord);
      sample[1] = Texture[105].Sample(Sampler[index.y], texCoord);
      return sample;
   case 106:
      sample[0] = Texture[106].Sample(Sampler[index.y], texCoord);
      sample[1] = Texture[107].Sample(Sampler[index.y], texCoord);
      return sample;
   case 108:
      sample[0] = Texture[108].Sample(Sampler[index.y], texCoord);
      sample[1] = Texture[109].Sample(Sampler[index.y], texCoord);
      return sample;
   case 110:
      sample[0] = Texture[110].Sample(Sampler[index.y], texCoord);
      sample[1] = Texture[111].Sample(Sampler[index.y], texCoord);
      return sample;
   case 112:
      sample[0] = Texture[112].Sample(Sampler[index.y], texCoord);
      sample[1] = Texture[113].Sample(Sampler[index.y], texCoord);
      return sample;
   case 114:
      sample[0] = Texture[114].Sample(Sampler[index.y], texCoord);
      sample[1] = Texture[115].Sample(Sampler[index.y], texCoord);
      return sample;
   case 116:
      sample[0] = Texture[116].Sample(Sampler[index.y], texCoord);
      sample[1] = Texture[117].Sample(Sampler[index.y], texCoord);
      return sample;
   case 118:
      sample[0] = Texture[118].Sample(Sampler[index.y], texCoord);
      sample[1] = Texture[119].Sample(Sampler[index.y], texCoord);
      return sample;
   case 120:
      sample[0] = Texture[120].Sample(Sampler[index.y], texCoord);
      sample[1] = Texture[121].Sample(Sampler[index.y], texCoord);
      return sample;
   case 122:
      sample[0] = Texture[122].Sample(Sampler[index.y], texCoord);
      sample[1] = Texture[123].Sample(Sampler[index.y], texCoord);
      return sample;
   case 124:
      sample[0] = Texture[124].Sample(Sampler[index.y], texCoord);
      sample[1] = Texture[125].Sample(Sampler[index.y], texCoord);
      return sample;
   case 126:
      sample[0] = Texture[126].Sample(Sampler[index.y], texCoord);
      sample[1] = Texture[127].Sample(Sampler[index.y], texCoord);
      return sample;
#endif
   }
}
