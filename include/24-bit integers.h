/****************************************************************
 * File: 24-bit integers.h                  Created: 2024/05/13 *
 *                                    Last modified: 2024/05/17 *
 *                                                              *
 * Desc: Both signed & unsigned data types.                     *
 *                                                              *
 *                             Copyright (c) David William Bull *
 ****************************************************************/
#pragma once

#define _24BIT_INTEGERS_

// 24-bit unsigned integer
struct ui24 {
#ifndef _COMMON_TYPES_
   typedef const bool cbool;
   typedef __int8  si8;  typedef const si8  csi8;
   typedef __int16 si16; typedef const si16 csi16;
   typedef __int32 si32; typedef const si32 csi32;
   typedef __int64 si64; typedef const si64 csi64;
   typedef float   fl32; typedef const fl32 cfl32;
   typedef double  fl64; typedef const fl64 cfl64;
   typedef unsigned __int8  ui8;  typedef const ui8  cui8;
   typedef unsigned __int16 ui16; typedef const ui16 cui16;
   typedef unsigned __int32 ui32; typedef const ui32 cui32;
   typedef unsigned __int64 ui64; typedef const ui64 cui64;
#endif
   typedef const ui24 cui24; typedef ui8(&ui8_3)[3]; typedef const ui8_3 cui8_3;

   ui8 data[3];

   ui24(void) = default;
   ui24(cui8 value) { data[0] = value; (ui16 &)data[1] = 0; }
   ui24(csi8 value) { data[0] = value; (ui16 &)data[1] = (value & 0x080) ? 0x0FFFF : 0; }
   ui24(cui16 value) { (ui16 &)data = (ui16 &)value; data[2] = 0; }
   ui24(csi16 value) { (ui16 &)data = (ui16 &)value; data[2] = (value & 0x08000) ? 0x0FF : 0; }
   ui24(cui8_3 value) { (ui16 &)data = (ui16 &)value[0]; data[2] = value[2]; }
   ui24(cui32 value) { (ui16 &)data = (ui16 &)value; data[2] = ((ui8_3)value)[2]; }
   ui24(csi32 value) { (ui16 &)data = (ui16 &)value; data[2] = ((ui8_3)value)[2]; }
   ui24(cfl32 value) { cui32 temp = (ui32)value; (ui16 &)data = (ui16 &)temp; data[2] = ((ui8_3)temp)[2]; }
   ui24(cfl64 value) { cui32 temp = (ui32)value; (ui16 &)data = (cui16 &)temp; data[2] = ((ui8_3)temp)[2]; }

   operator cui8(void) const { return (ui8 &)data; }
   operator csi8(void) const { return (si8 &)data; }
   operator cui16(void) const { return (ui16 &)data; }
   operator csi16(void) const { return (si16 &)data; }
   operator cui8_3(void) const { return (ui8_3 &)data; }
   operator cui32(void) const { return (ui32 &)data & 0x0FFFFFF; }
   operator csi32(void) const { return (si32 &)data & 0x0FFFFFF; }
   operator cui64(void) const { return (ui64 &)data & 0x0FFFFFF; }
   operator csi64(void) const { return (si64 &)data & 0x0FFFFFF; }
   operator cfl32(void) const { return fl32((si32 &)data & 0x0FFFFFF); }
   operator cfl64(void) const { return fl64((si32 &)data & 0x0FFFFFF); }

   inline cui24 &operator&(void) const { return *this; }
   inline cui24 &operator()(void) const { return *this; }
   inline cui24 operator~(void) const { return (ui32 &)data ^ 0x0FFFFFF; }
   inline cui24 operator-(void) const { return (ui32 &)data ^ 0x0FFFFFF; }

   inline cui24 operator++(void) { ((ui16 &)data)++; data[2] += ((ui16 &)data == 0); return *this; }
   inline cui24 operator++(int) { cui24 temp = data; ((ui16 &)data)++; data[2] += ((ui16 &)data == 0); return temp; }
   inline cui24 operator--(void) { ((ui16 &)data)--; data[2] -= ((ui16 &)data == 0x0FFFF); return *this; }
   inline cui24 operator--(int) { cui24 temp = data; ((ui16 &)data)--; data[2] -= ((ui16 &)data == 0x0FFFF); return temp; }

   inline cbool operator==(cui24 &value) const { return ((ui32 &)data & 0x0FFFFFF) == ((ui32 &)value & 0x0FFFFFF); }
   inline cbool operator!=(cui24 &value) const { return ((ui32 &)data & 0x0FFFFFF) != ((ui32 &)value & 0x0FFFFFF); }
   inline cbool operator<(cui24 &value) const { return ((ui32 &)data & 0x0FFFFFF) < ((ui32 &)value & 0x0FFFFFF); }
   inline cbool operator>(cui24 &value) const { return ((ui32 &)data & 0x0FFFFFF) > ((ui32 &)value & 0x0FFFFFF); }
   inline cbool operator<=(cui24 &value) const { return ((ui32 &)data & 0x0FFFFFF) <= ((ui32 &)value & 0x0FFFFFF); }
   inline cbool operator>=(cui24 &value) const { return ((ui32 &)data & 0x0FFFFFF) >= ((ui32 &)value & 0x0FFFFFF); }
   inline cbool operator||(cui24 &value) const { return ((ui32 &)data & 0x0FFFFFF) || ((ui32 &)value & 0x0FFFFFF); }
   inline cbool operator&&(cui24 &value) const { return ((ui32 &)data & 0x0FFFFFF) && ((ui32 &)value & 0x0FFFFFF); }
   inline cui24 operator+(cui24 &value) const { cui32 temp = ((ui32 &)data + (ui32 &)value) & 0x0FFFFFF; return (ui24 &)temp; };
   inline cui24 operator-(cui24 &value) const { cui32 temp = ((ui32 &)data - (ui32 &)value) & 0x0FFFFFF; return (ui24 &)temp; };
   inline cui24 operator*(cui24 &value) const { cui32 temp = ((ui32 &)data & 0x0FFFFFF) * ((cui32 &)value & 0x0FFFFFF); return (ui24 &)temp; };
   inline cui24 operator/(cui24 &value) const { cui32 temp = ((ui32 &)data & 0x0FFFFFF) / ((cui32 &)value & 0x0FFFFFF); return (ui24 &)temp; };
   inline cui24 operator%(cui24 &value) const { cui32 temp = ((ui32 &)data & 0x0FFFFFF) % ((cui32 &)value & 0x0FFFFFF); return (ui24 &)temp; };
   inline cui24 operator&(cui24 &value) const { cui32 temp = ((ui32 &)data & (ui32 &)value) & 0x0FFFFFF; return (ui24 &)temp; };
   inline cui24 operator|(cui24 &value) const { cui32 temp = ((ui32 &)data | (ui32 &)value) & 0x0FFFFFF; return (ui24 &)temp; };
   inline cui24 operator^(cui24 &value) const { cui32 temp = ((ui32 &)data ^ (ui32 &)value) & 0x0FFFFFF; return (ui24 &)temp; };
   inline cui24 operator<<(cui24 &value) const { cui32 temp = ((ui32 &)data & 0x0FFFFFF) << ((ui32 &)value & 0x0FFFFFF); return (ui24 &)temp; };
   inline cui24 operator>>(cui24 &value) const { cui32 temp = ((ui32 &)data & 0x0FFFFFF) >> ((ui32 &)value & 0x0FFFFFF); return (ui24 &)temp; };
   inline cui24 operator+=(cui24 &value) { (ui16 &)data += (ui16 &)value; data[2] += value.data[2] + (data[1] < value.data[1]); return (ui24 &)data; };
   inline cui24 operator-=(cui24 &value) { (ui16 &)data -= (ui16 &)value; data[2] -= value.data[2] + (data[1] > value.data[1]); return (ui24 &)data; };
   inline cui24 operator*=(cui24 &value) { cui32 temp = ((cui32 &)data & 0x0FFFFFF) * ((cui32 &)value & 0x0FFFFFF); (ui16 &)data = (ui16 &)temp; data[2] = ((ui8_3 &)temp)[2]; return (ui24 &)data; };
   inline cui24 operator/=(cui24 &value) { cui32 temp = ((cui32 &)data & 0x0FFFFFF) / ((cui32 &)value & 0x0FFFFFF); (ui16 &)data = (cui16 &)temp; data[2] = ((ui8_3 &)temp)[2]; return (ui24 &)data; };
   inline cui24 operator%=(cui24 &value) { cui32 temp = ((cui32 &)data & 0x0FFFFFF) % ((cui32 &)value & 0x0FFFFFF); (ui16 &)data = (cui16 &)temp; data[2] = ((ui8_3 &)temp)[2]; return (ui24 &)data; };
   inline cui24 operator&=(cui24 &value) { cui32 temp = ((ui32 &)data & (ui32 &)value) & 0x0FFFFFF; (ui16 &)data = (ui16 &)temp; data[2] = ((ui8_3)temp)[2]; return (ui24 &)data; };
   inline cui24 operator|=(cui24 &value) { cui32 temp = ((ui32 &)data | (ui32 &)value) & 0x0FFFFFF; (ui16 &)data = (ui16 &)temp; data[2] = ((ui8_3)temp)[2]; return (ui24 &)data; };
   inline cui24 operator^=(cui24 &value) { cui32 temp = ((ui32 &)data ^ (ui32 &)value) & 0x0FFFFFF; (ui16 &)data = (ui16 &)temp; data[2] = ((ui8_3)temp)[2]; return (ui24 &)data; };
   inline cui24 operator<<=(cui24 &value) { cui32 temp = ((ui32 &)data & 0x0FFFFFF) << ((ui32 &)value & 0x0FFFFFF); (ui16 &)data = (ui16 &)temp; data[2] = ((ui8_3 &)temp)[2]; return (ui24 &)data; };
   inline cui24 operator>>=(cui24 &value) { cui32 temp = ((ui32 &)data & 0x0FFFFFF) >> ((ui32 &)value & 0x0FFFFFF); (ui16 &)data = (ui16 &)temp; data[2] = ((ui8_3 &)temp)[2]; return (ui24 &)data; };

   inline cbool operator==(csi32 &value) const { return ((ui32 &)data & 0x0FFFFFF) == ((ui32 &)value & 0x0FFFFFF); }
   inline cbool operator!=(csi32 &value) const { return ((ui32 &)data & 0x0FFFFFF) != ((ui32 &)value & 0x0FFFFFF); }
   inline cbool operator<(csi32 &value) const { return ((ui32 &)data & 0x0FFFFFF) < ((ui32 &)value & 0x0FFFFFF); }
   inline cbool operator>(csi32 &value) const { return ((ui32 &)data & 0x0FFFFFF) > ((ui32 &)value & 0x0FFFFFF); }
   inline cbool operator<=(csi32 &value) const { return ((ui32 &)data & 0x0FFFFFF) <= ((ui32 &)value & 0x0FFFFFF); }
   inline cbool operator>=(csi32 &value) const { return ((ui32 &)data & 0x0FFFFFF) >= ((ui32 &)value & 0x0FFFFFF); }
   inline cbool operator||(csi32 &value) const { return ((ui32 &)data & 0x0FFFFFF) || ((ui32 &)value & 0x0FFFFFF); }
   inline cbool operator&&(csi32 &value) const { return ((ui32 &)data & 0x0FFFFFF) && ((ui32 &)value & 0x0FFFFFF); }
   inline cui24 operator+(csi32 &value) const { cui32 temp = ((ui32 &)data + (ui32 &)value) & 0x0FFFFFF; return (ui24 &)temp; };
   inline cui24 operator-(csi32 &value) const { cui32 temp = ((ui32 &)data - (ui32 &)value) & 0x0FFFFFF; return (ui24 &)temp; };
   inline cui24 operator*(csi32 &value) const { cui32 temp = ((ui32 &)data & 0x0FFFFFF) * ((cui32 &)value & 0x0FFFFFF); return (ui24 &)temp; };
   inline cui24 operator/(csi32 &value) const { cui32 temp = ((ui32 &)data & 0x0FFFFFF) / ((cui32 &)value & 0x0FFFFFF); return (ui24 &)temp; };
   inline cui24 operator%(csi32 &value) const { cui32 temp = ((ui32 &)data & 0x0FFFFFF) % ((cui32 &)value & 0x0FFFFFF); return (ui24 &)temp; };
   inline cui24 operator&(csi32 &value) const { cui32 temp = ((ui32 &)data & (ui32 &)value) & 0x0FFFFFF; return (ui24 &)temp; };
   inline cui24 operator|(csi32 &value) const { cui32 temp = ((ui32 &)data | (ui32 &)value) & 0x0FFFFFF; return (ui24 &)temp; };
   inline cui24 operator^(csi32 &value) const { cui32 temp = ((ui32 &)data ^ (ui32 &)value) & 0x0FFFFFF; return (ui24 &)temp; };
   inline cui24 operator<<(csi32 &value) const { cui32 temp = ((ui32 &)data & 0x0FFFFFF) << ((ui32 &)value & 0x0FFFFFF); return (ui24 &)temp; };
   inline cui24 operator>>(csi32 &value) const { cui32 temp = ((ui32 &)data & 0x0FFFFFF) >> ((ui32 &)value & 0x0FFFFFF); return (ui24 &)temp; };
   inline cui24 operator+=(csi32 &value) { (ui16 &)data += (ui16 &)value; data[2] += ((ui8_3)value)[2] + (data[1] < ((ui8_3 &)value)[1]); return (ui24 &)data; };
   inline cui24 operator-=(csi32 &value) { (ui16 &)data -= (ui16 &)value; data[2] -= ((ui8_3)value)[2] + (data[1] < ((ui8_3 &)value)[1]); return (ui24 &)data; };
   inline cui24 operator*=(csi32 &value) { cui32 temp = ((cui32 &)data & 0x0FFFFFF) * ((cui32 &)value & 0x0FFFFFF); (ui16 &)data = (ui16 &)temp; data[2] = ((ui8_3 &)temp)[2]; return (ui24 &)data; };
   inline cui24 operator/=(csi32 &value) { cui32 temp = ((cui32 &)data & 0x0FFFFFF) / ((cui32 &)value & 0x0FFFFFF); (ui16 &)data = (cui16 &)temp; data[2] = ((ui8_3 &)temp)[2]; return (ui24 &)data; };
   inline cui24 operator%=(csi32 &value) { cui32 temp = ((cui32 &)data & 0x0FFFFFF) % ((cui32 &)value & 0x0FFFFFF); (ui16 &)data = (cui16 &)temp; data[2] = ((ui8_3 &)temp)[2]; return (ui24 &)data; };
   inline cui24 operator&=(csi32 &value) { cui32 temp = ((ui32 &)data & (ui32 &)value) & 0x0FFFFFF; (ui16 &)data = (ui16 &)temp; data[2] = ((ui8_3)temp)[2]; return (ui24 &)data; };
   inline cui24 operator|=(csi32 &value) { cui32 temp = ((ui32 &)data | (ui32 &)value) & 0x0FFFFFF; (ui16 &)data = (ui16 &)temp; data[2] = ((ui8_3)temp)[2]; return (ui24 &)data; };
   inline cui24 operator^=(csi32 &value) { cui32 temp = ((ui32 &)data ^ (ui32 &)value) & 0x0FFFFFF; (ui16 &)data = (ui16 &)temp; data[2] = ((ui8_3)temp)[2]; return (ui24 &)data; };
   inline cui24 operator<<=(csi32 &value) { cui32 temp = ((ui32 &)data & 0x0FFFFFF) << ((ui32 &)value & 0x0FFFFFF); (ui16 &)data = (ui16 &)temp; data[2] = ((ui8_3 &)temp)[2]; return (ui24 &)data; };
   inline cui24 operator>>=(csi32 &value) { cui32 temp = ((ui32 &)data & 0x0FFFFFF) >> ((ui32 &)value & 0x0FFFFFF); (ui16 &)data = (ui16 &)temp; data[2] = ((ui8_3 &)temp)[2]; return (ui24 &)data; };
};

// 24-bit signed integer
struct si24 {
#ifndef _COMMON_TYPES_
   typedef const bool cbool;
   typedef __int8  si8;  typedef const si8 csi8;
   typedef __int16 si16; typedef const si16 csi16;
   typedef __int32 si32; typedef const si32 csi32;
   typedef __int64 si64; typedef const si64 csi64;
   typedef   float fl32; typedef const fl32 cfl32;
   typedef  double fl64; typedef const fl64 cfl64;
   typedef unsigned __int8  ui8;  typedef const ui8 cui8;
   typedef unsigned __int16 ui16; typedef const ui16 cui16;
   typedef unsigned __int32 ui32; typedef const ui32 cui32;
   typedef unsigned __int64 ui64; typedef const ui64 cui64;
#endif
   typedef const si24 csi24; typedef si8(&si8_3)[3]; typedef const si8_3 csi8_3;

   si8 data[3];

   si24(void) = default;
   si24(cui8 value) { data[0] = value; (si16 &)data[1] = 0; }
   si24(csi8 value) { data[0] = value; (si16 &)data[1] = (value & 0x080) ? 0x0FFFF : 0; }
   si24(cui16 value) { (si16 &)data = (si16 &)value; data[2] = 0; }
   si24(csi16 value) { (si16 &)data = (si16 &)value; data[2] = (value & 0x08000) ? 0x0FF : 0; }
   si24(csi8_3 value) { (si16 &)data = (si16 &)value; data[2] = value[2]; }
   si24(cui32 value) { (si16 &)data = (si16 &)value; data[2] = ((si8_3)value)[2]; }
   si24(csi32 value) { (si16 &)data = (si16 &)value; data[2] = ((si8_3)value)[2]; }
   si24(cfl32 value) { csi32 temp = (si32)value; (si16 &)data = (si16 &)temp; data[2] = ((si8_3)temp)[2]; }
   si24(cfl64 value) { csi32 temp = (si32)value; (si16 &)data = (csi16 &)temp; data[2] = ((si8_3)temp)[2]; }

   operator cui8(void) const { return (ui8 &)data; }
   operator csi8(void) const { return (si8 &)data; }
   operator cui16(void) const { return (ui16 &)data; }
   operator csi16(void) const { return (si16 &)data; }
   operator csi8_3(void) const { return (si8_3)data; }
   operator cui32(void) const { return (ui32 &)data & 0x0FFFFFF; }
   operator csi32(void) const { return (si32 &)data & 0x0FFFFFF; }
   operator cui64(void) const { return (ui64 &)data & 0x0FFFFFF; }
   operator csi64(void) const { return (si64 &)data & 0x0FFFFFF; }
   operator cfl32(void) const { return fl32((si32 &)data & 0x0FFFFFF); }
   operator cfl64(void) const { return fl64((si32 &)data & 0x0FFFFFF); }

   inline csi24 &operator&(void) const { return *this; }
   inline csi24 &operator()(void) const { return *this; }
   inline csi24 operator~(void) const { return (ui32 &)data ^ 0x0FFFFFF; }
   inline csi24 operator-(void) const { return (ui32 &)data ^ 0x0FFFFFF; }

   inline csi24 operator++(void) { ((ui16 &)data)++; data[2] += ((ui16 &)data == 0); return *this; }
   inline csi24 operator++(int) { csi24 temp = data; ((ui16 &)data)++; data[2] += ((ui16 &)data == 0); return temp; }
   inline csi24 operator--(void) { ((ui16 &)data)--; data[2] -= ((ui16 &)data == 0x0FFFF); return *this; }
   inline csi24 operator--(int) { csi24 temp = data; ((ui16 &)data)--; data[2] -= ((ui16 &)data == 0x0FFFF); return temp; }

   inline cbool operator==(csi24 &value) const { return ((si32 &)data & 0x0FFFFFF) == ((si32 &)value & 0x0FFFFFF); }
   inline cbool operator!=(csi24 &value) const { return ((si32 &)data & 0x0FFFFFF) != ((si32 &)value & 0x0FFFFFF); }
   inline cbool operator<(csi24 &value) const { return ((si32 &)data & 0x0FFFFFF) < ((si32 &)value & 0x0FFFFFF); }
   inline cbool operator>(csi24 &value) const { return ((si32 &)data & 0x0FFFFFF) > ((si32 &)value & 0x0FFFFFF); }
   inline cbool operator<=(csi24 &value) const { return ((si32 &)data & 0x0FFFFFF) <= ((si32 &)value & 0x0FFFFFF); }
   inline cbool operator>=(csi24 &value) const { return ((si32 &)data & 0x0FFFFFF) >= ((si32 &)value & 0x0FFFFFF); }
   inline cbool operator||(csi24 &value) const { return ((si32 &)data & 0x0FFFFFF) || ((si32 &)value & 0x0FFFFFF); }
   inline cbool operator&&(csi24 &value) const { return ((si32 &)data & 0x0FFFFFF) && ((si32 &)value & 0x0FFFFFF); }
   inline csi24 operator+(csi24 &value) const { csi32 temp = ((si32 &)data + (si32 &)value) & 0x0FFFFFF; return (si24 &)temp; };
   inline csi24 operator-(csi24 &value) const { csi32 temp = ((si32 &)data - (si32 &)value) & 0x0FFFFFF; return (si24 &)temp; };
   inline csi24 operator*(csi24 &value) const { csi32 temp = ((si32 &)data & 0x0FFFFFF) * ((csi32 &)value & 0x0FFFFFF); return (si24 &)temp; };
   inline csi24 operator/(csi24 &value) const { csi32 temp = ((si32 &)data & 0x0FFFFFF) / ((csi32 &)value & 0x0FFFFFF); return (si24 &)temp; };
   inline csi24 operator%(csi24 &value) const { csi32 temp = ((si32 &)data & 0x0FFFFFF) % ((csi32 &)value & 0x0FFFFFF); return (si24 &)temp; };
   inline csi24 operator&(csi24 &value) const { csi32 temp = ((si32 &)data & (si32 &)value) & 0x0FFFFFF; return (si24 &)temp; };
   inline csi24 operator|(csi24 &value) const { csi32 temp = ((si32 &)data | (si32 &)value) & 0x0FFFFFF; return (si24 &)temp; };
   inline csi24 operator^(csi24 &value) const { csi32 temp = ((si32 &)data ^ (si32 &)value) & 0x0FFFFFF; return (si24 &)temp; };
   inline csi24 operator<<(csi24 &value) const { csi32 temp = ((si32 &)data & 0x0FFFFFF) << ((si32 &)value & 0x0FFFFFF); return (si24 &)temp; };
   inline csi24 operator>>(csi24 &value) const { csi32 temp = ((si32 &)data & 0x0FFFFFF) >> ((si32 &)value & 0x0FFFFFF); return (si24 &)temp; };
   inline csi24 operator+=(csi24 &value) { (ui16 &)data += (ui16 &)value; data[2] += value.data[2] + (data[1] < value.data[1]); return (si24 &)data; };
   inline csi24 operator-=(csi24 &value) { (ui16 &)data -= (ui16 &)value; data[2] -= value.data[2] + (data[1] > value.data[1]); return (si24 &)data; };
   inline csi24 operator*=(csi24 &value) { csi32 temp = ((csi32 &)data & 0x0FFFFFF) * ((csi32 &)value & 0x0FFFFFF); (ui16 &)data = (ui16 &)temp; data[2] = ((si8_3 &)temp)[2]; return (si24 &)data; };
   inline csi24 operator/=(csi24 &value) { csi32 temp = ((csi32 &)data & 0x0FFFFFF) / ((csi32 &)value & 0x0FFFFFF); (ui16 &)data = (cui16 &)temp; data[2] = ((si8_3 &)temp)[2]; return (si24 &)data; };
   inline csi24 operator%=(csi24 &value) { csi32 temp = ((csi32 &)data & 0x0FFFFFF) % ((csi32 &)value & 0x0FFFFFF); (ui16 &)data = (cui16 &)temp; data[2] = ((si8_3 &)temp)[2]; return (si24 &)data; };
   inline csi24 operator&=(csi24 &value) { csi32 temp = ((si32 &)data & (si32 &)value) & 0x0FFFFFF; (ui16 &)data = (ui16 &)temp; data[2] = ((si8_3)temp)[2]; return (si24 &)data; };
   inline csi24 operator|=(csi24 &value) { csi32 temp = ((si32 &)data | (si32 &)value) & 0x0FFFFFF; (ui16 &)data = (ui16 &)temp; data[2] = ((si8_3)temp)[2]; return (si24 &)data; };
   inline csi24 operator^=(csi24 &value) { csi32 temp = ((si32 &)data ^ (si32 &)value) & 0x0FFFFFF; (ui16 &)data = (ui16 &)temp; data[2] = ((si8_3)temp)[2]; return (si24 &)data; };
   inline csi24 operator<<=(csi24 &value) { csi32 temp = ((si32 &)data & 0x0FFFFFF) << ((si32 &)value & 0x0FFFFFF); (ui16 &)data = (ui16 &)temp; data[2] = ((si8_3 &)temp)[2]; return (si24 &)data; };
   inline csi24 operator>>=(csi24 &value) { csi32 temp = ((si32 &)data & 0x0FFFFFF) >> ((si32 &)value & 0x0FFFFFF); (ui16 &)data = (ui16 &)temp; data[2] = ((si8_3 &)temp)[2]; return (si24 &)data; };

   inline cbool operator==(csi32 &value) const { return ((si32 &)data & 0x0FFFFFF) == ((si32 &)value & 0x0FFFFFF); }
   inline cbool operator!=(csi32 &value) const { return ((si32 &)data & 0x0FFFFFF) != ((si32 &)value & 0x0FFFFFF); }
   inline cbool operator<(csi32 &value) const { return ((si32 &)data & 0x0FFFFFF) < ((si32 &)value & 0x0FFFFFF); }
   inline cbool operator>(csi32 &value) const { return ((si32 &)data & 0x0FFFFFF) > ((si32 &)value & 0x0FFFFFF); }
   inline cbool operator<=(csi32 &value) const { return ((si32 &)data & 0x0FFFFFF) <= ((si32 &)value & 0x0FFFFFF); }
   inline cbool operator>=(csi32 &value) const { return ((si32 &)data & 0x0FFFFFF) >= ((si32 &)value & 0x0FFFFFF); }
   inline cbool operator||(csi32 &value) const { return ((si32 &)data & 0x0FFFFFF) || ((si32 &)value & 0x0FFFFFF); }
   inline cbool operator&&(csi32 &value) const { return ((si32 &)data & 0x0FFFFFF) && ((si32 &)value & 0x0FFFFFF); }
   inline csi24 operator+(csi32 &value) const { csi32 temp = ((si32 &)data + (si32 &)value) & 0x0FFFFFF; return (si24 &)temp; };
   inline csi24 operator-(csi32 &value) const { csi32 temp = ((si32 &)data - (si32 &)value) & 0x0FFFFFF; return (si24 &)temp; };
   inline csi24 operator*(csi32 &value) const { csi32 temp = ((si32 &)data & 0x0FFFFFF) * ((csi32 &)value & 0x0FFFFFF); return (si24 &)temp; };
   inline csi24 operator/(csi32 &value) const { csi32 temp = ((si32 &)data & 0x0FFFFFF) / ((csi32 &)value & 0x0FFFFFF); return (si24 &)temp; };
   inline csi24 operator%(csi32 &value) const { csi32 temp = ((si32 &)data & 0x0FFFFFF) % ((csi32 &)value & 0x0FFFFFF); return (si24 &)temp; };
   inline csi24 operator&(csi32 &value) const { csi32 temp = ((si32 &)data & (si32 &)value) & 0x0FFFFFF; return (si24 &)temp; };
   inline csi24 operator|(csi32 &value) const { csi32 temp = ((si32 &)data | (si32 &)value) & 0x0FFFFFF; return (si24 &)temp; };
   inline csi24 operator^(csi32 &value) const { csi32 temp = ((si32 &)data ^ (si32 &)value) & 0x0FFFFFF; return (si24 &)temp; };
   inline csi24 operator<<(csi32 &value) const { csi32 temp = ((si32 &)data & 0x0FFFFFF) << ((si32 &)value & 0x0FFFFFF); return (si24 &)temp; };
   inline csi24 operator>>(csi32 &value) const { csi32 temp = ((si32 &)data & 0x0FFFFFF) >> ((si32 &)value & 0x0FFFFFF); return (si24 &)temp; };
   inline csi24 operator+=(csi32 &value) { (ui16 &)data += (ui16 &)value; data[2] += ((si8_3)value)[2] + (data[1] < ((si8_3 &)value)[1]); return (si24 &)data; };
   inline csi24 operator-=(csi32 &value) { (ui16 &)data -= (ui16 &)value; data[2] -= ((si8_3)value)[2] + (data[1] < ((si8_3 &)value)[1]); return (si24 &)data; };
   inline csi24 operator*=(csi32 &value) { csi32 temp = ((csi32 &)data & 0x0FFFFFF) * ((csi32 &)value & 0x0FFFFFF); (ui16 &)data = (ui16 &)temp; data[2] = ((si8_3 &)temp)[2]; return (si24 &)data; };
   inline csi24 operator/=(csi32 &value) { csi32 temp = ((csi32 &)data & 0x0FFFFFF) / ((csi32 &)value & 0x0FFFFFF); (ui16 &)data = (cui16 &)temp; data[2] = ((si8_3 &)temp)[2]; return (si24 &)data; };
   inline csi24 operator%=(csi32 &value) { csi32 temp = ((csi32 &)data & 0x0FFFFFF) % ((csi32 &)value & 0x0FFFFFF); (ui16 &)data = (cui16 &)temp; data[2] = ((si8_3 &)temp)[2]; return (si24 &)data; };
   inline csi24 operator&=(csi32 &value) { csi32 temp = ((si32 &)data & (si32 &)value) & 0x0FFFFFF; (ui16 &)data = (ui16 &)temp; data[2] = ((si8_3)temp)[2]; return (si24 &)data; };
   inline csi24 operator|=(csi32 &value) { csi32 temp = ((si32 &)data | (si32 &)value) & 0x0FFFFFF; (ui16 &)data = (ui16 &)temp; data[2] = ((si8_3)temp)[2]; return (si24 &)data; };
   inline csi24 operator^=(csi32 &value) { csi32 temp = ((si32 &)data ^ (si32 &)value) & 0x0FFFFFF; (ui16 &)data = (ui16 &)temp; data[2] = ((si8_3)temp)[2]; return (si24 &)data; };
   inline csi24 operator<<=(csi32 &value) { csi32 temp = ((si32 &)data & 0x0FFFFFF) << ((si32 &)value & 0x0FFFFFF); (ui16 &)data = (ui16 &)temp; data[2] = ((si8_3 &)temp)[2]; return (si24 &)data; };
   inline csi24 operator>>=(csi32 &value) { csi32 temp = ((si32 &)data & 0x0FFFFFF) >> ((si32 &)value & 0x0FFFFFF); (ui16 &)data = (ui16 &)temp; data[2] = ((si8_3 &)temp)[2]; return (si24 &)data; };
};

typedef const    ui24            cui24;
typedef const    si24            csi24;
typedef volatile ui24            vui24;
typedef volatile si24            vsi24;
typedef          ui24 *          ui24ptr;
typedef          si24 *          si24ptr;
typedef const    ui24 *          cui24ptr;
typedef const    si24 *          csi24ptr;
typedef volatile ui24 *          vui24ptr;
typedef volatile si24 *          vsi24ptr;
typedef          ui24 * const    ui24ptrc;
typedef          si24 * const    si24ptrc;
typedef const    ui24 * const    cui24ptrc;
typedef const    si24 * const    csi24ptrc;
typedef volatile ui24 * const    vui24ptrc;
typedef volatile si24 * const    vsi24ptrc;
typedef          ui24 * volatile ui24ptrv;
typedef          si24 * volatile si24ptrv;
typedef const    ui24 * volatile cui24ptrv;
typedef const    si24 * volatile csi24ptrv;
typedef volatile ui24 * volatile vui24ptrv;
typedef volatile si24 * volatile vsi24ptrv;
