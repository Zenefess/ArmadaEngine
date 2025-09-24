/****************************************************************
 * File: Reduced-bit integers.h             Created: 2024/05/27 *
 *                                    Last modified: 2024/05/27 *
 *                                                              *
 * Desc: Both signed & unsigned data types.                     *
 *                                                              *
 * MIT license                 Copyright (c) David William Bull *
 ****************************************************************/
#pragma once

#define _REDBIT_INTEGERS_

 // 3x 6-bit unsigned integer
struct ui6x3 {
#ifndef _COMMON_TYPES_
   typedef const bool cbool;
   typedef void *ptr;
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
   typedef const ui6x3 cui6x3; typedef ui8 (& ui8_3)[3]; typedef const ui8_3 cui8_3;

   ui32 data;

   ui6x3(void) = default;
   ui6x3(cui32 value) { data = value; }
   ui6x3(cui8 value, cui8 index) { data = value << (index * 6u); }
   ui6x3(cui8 value0, cui8 value1, cui8 value2) { data = (value0 & 0x03Fu) | ((value1 & 0x03Fu) << 6u) | ((value2 & 0x03Fu) << 12u); }
   ui6x3(csi32 value0, csi32 value1, csi32 value2) { data = ((ui32 &)value0 & 0x03Fu) | (((ui32 &)value1 & 0x03Fu) << 6u) | (((ui32 &)value2 & 0x03Fu) << 12u); }
   ui6x3(cui8_3 value) { data = (value[0] & 0x03Fu) | ((value[1] & 0x03Fu) << 6u) | ((value[2] & 0x03Fu) << 12u); }

   operator ptr(void) const { return *this; }
   operator cbool(void) const { return ((ui32 &)data & 0x03FFFFu) != 0; }
   operator cui32(void) const { return data & 0x03FFFFu; }
   operator csi32(void) const { return (si32 &)data & 0x03FFFF; }
   operator cui64(void) const { return (ui64 &)data & 0x03FFFFu; }
   operator csi64(void) const { return (si64 &)data & 0x03FFFF; }

   inline cui6x3 &operator&(void) const { return *this; }
   inline cui6x3 &operator()(void) const { return *this; }
   inline cui6x3 operator~(void) const { cui6x3 temp = data ^ 0x03FFFFu; return temp; }
   inline cui6x3 operator-(void) const { cui6x3 temp = data ^ 0x03FFFFu; return temp; }

   inline cui6x3 operator++(void) { data = ((data + 1u) & 0x03Fu) | ((data + 64u) & 0x0FC0) | ((data + 4096u) & 0x03F000u); return *this; }
   inline cui6x3 operator++(int) { cui32 temp = data; data = ((data + 1u) & 0x03Fu) | ((data + 64u) & 0x0FC0u) | ((data + 4096u) & 0x03F000u); return *(ui6x3 *)&temp; }
   inline cui6x3 operator--(void) { data = ((data - 1u) & 0x03Fu) | ((data - 64u) & 0x0FC0) | ((data - 4096u) & 0x03F000u); return *this; }
   inline cui6x3 operator--(int) { cui32 temp = data; data = ((data - 1u) & 0x03Fu) | ((data - 64u) & 0x0FC0u) | ((data - 4096u) & 0x03F000u); return *(ui6x3 *)&temp; }

   inline cbool operator==(cui6x3 &value) const { return (data & 0x03FFFFu) == (value.data & 0x03FFFFu); }
   inline cbool operator!=(cui6x3 &value) const { return (data & 0x03FFFFu) != (value.data & 0x03FFFFu); }
   inline cbool operator||(cui6x3 &value) const { return (data & 0x03FFFFu) || (value.data & 0x03FFFFu); }
   inline cbool operator&&(cui6x3 &value) const { return (data & 0x03FFFFu) && (value.data & 0x03FFFFu); }
   inline cui6x3 operator+(cui6x3 &value) const { cui32 temp = ((data + value.data) & 0x03Fu) | ((data + (value.data << 6)) & 0x0FC0u) | ((data + (value.data << 12)) & 0x03F000u); return *(ui6x3 *)&temp; };
   inline cui6x3 operator-(cui6x3 &value) const { cui32 temp = ((data - value.data) & 0x03Fu) | ((data - (value.data << 6)) & 0x0FC0u) | ((data - (value.data << 12)) & 0x03F000u); return *(ui6x3 *)&temp; };
   inline cui6x3 operator&(cui6x3 &value) const { cui32 temp = (data & value.data) & 0x03FFFFu; return *(ui6x3 *)&temp; };
   inline cui6x3 operator|(cui6x3 &value) const { cui32 temp = (data | value.data) & 0x03FFFFu; return *(ui6x3 *)&temp; };
   inline cui6x3 operator+=(cui6x3 &value) { data = ((data + value.data) & 0x03Fu) | ((data + (value.data << 6)) & 0x0FFC0u) | ((data + (value.data << 12)) & 0x03F000u); return *this; };
   inline cui6x3 operator-=(cui6x3 &value) { data = ((data - value.data) & 0x03Fu) | ((data - (value.data << 6)) & 0x0FFC0u) | ((data - (value.data << 12)) & 0x03F000u); return *this; };
   inline cui6x3 operator&=(cui6x3 &value) { data = (data & value.data) & 0x03FFFFu; return *this; }
   inline cui6x3 operator|=(cui6x3 &value) { data = (data | value.data) & 0x03FFFFu; return *this; }
};

// 3x 10-bit unsigned integer
struct ui10x3 {
#ifndef _COMMON_TYPES_
   typedef const bool cbool;
   typedef void *ptr;
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
   typedef const ui10x3 cui10x3; typedef ui16 (& ui16_3)[3]; typedef const ui16_3 cui16_3;

   ui32 data;

   ui10x3(void) = default;
   ui10x3(cui32 value) { data = value; }
   ui10x3(cui16 value, cui8 index) { data = value << (index * 10u); }
   ui10x3(cui16 value0, cui16 value1, cui16 value2) { data = (value0 & 0x03FFu) | ((value1 & 0x03FFu) << 10u) | ((value2 & 0x03FFu) << 20u); }
   ui10x3(csi32 value0, csi32 value1, csi32 value2) { data = ((ui32 &)value0 & 0x03FFu) | (((ui32 &)value1 & 0x03FFu) << 10u) | (((ui32 &)value2 & 0x03FFu) << 20u); }
   ui10x3(cui16_3 value) { data = (value[0] & 0x03FFu) | ((value[1] & 0x03FFu) << 10u) | ((value[2] & 0x03FFu) << 20u); }

   operator ptr(void) const { return *this; }
   operator cbool(void) const { return ((ui32 &)data & 0x03FFFFFFFu) != 0; }
   operator cui32(void) const { return data & 0x03FFFFFFFu; }
   operator csi32(void) const { return (si32 &)data & 0x03FFFFFFF; }
   operator cui64(void) const { return (ui64 &)data & 0x03FFFFFFFu; }
   operator csi64(void) const { return (si64 &)data & 0x03FFFFFFF; }

   inline cui10x3 &operator&(void) const { return *this; }
   inline cui10x3 &operator()(void) const { return *this; }
   inline cui10x3 operator~(void) const { cui10x3 temp = data ^ 0x03FFFFFFFu; return temp; }
   inline cui10x3 operator-(void) const { cui10x3 temp = data ^ 0x03FFFFFFFu; return temp; }

   inline cui10x3 operator++(void) { data = ((data + 1u) & 0x03FFu) | ((data + 1024u) & 0x0FFC00) | ((data + 0x010000u) & 0x03FF00000u); return *this; }
   inline cui10x3 operator++(int) { cui32 temp = data; data = ((data + 1u) & 0x03FFu) | ((data + 1024u) & 0x0FFC00u) | ((data + 0x010000u) & 0x03FF00000u); return *(ui10x3 *)&temp; }
   inline cui10x3 operator--(void) { data = ((data - 1u) & 0x03FFu) | ((data - 1024u) & 0x0FFC00) | ((data - 0x010000u) & 0x03FF00000u); return *this; }
   inline cui10x3 operator--(int) { cui32 temp = data; data = ((data - 1u) & 0x03FFu) | ((data - 1024u) & 0x0FFC00u) | ((data - 0x010000u) & 0x03FF00000u); return *(ui10x3 *)&temp; }

   inline cbool operator==(cui10x3 &value) const { return (data & 0x03FFFFFFFu) == (value.data & 0x03FFFFFFFu); }
   inline cbool operator!=(cui10x3 &value) const { return (data & 0x03FFFFFFFu) != (value.data & 0x03FFFFFFFu); }
   inline cbool operator||(cui10x3 &value) const { return (data & 0x03FFFFFFFu) || (value.data & 0x03FFFFFFFu); }
   inline cbool operator&&(cui10x3 &value) const { return (data & 0x03FFFFFFFu) && (value.data & 0x03FFFFFFFu); }
   inline cui10x3 operator+(cui10x3 &value) const { cui32 temp = ((data + value.data) & 0x03FFu) | ((data + (value.data << 10)) & 0x0FFC00) | ((data + (value.data << 20)) & 0x03FF00000u); return *(ui10x3 *)&temp; };
   inline cui10x3 operator-(cui10x3 &value) const { cui32 temp = ((data - value.data) & 0x03FFu) | ((data - (value.data << 10)) & 0x0FFC00) | ((data - (value.data << 20)) & 0x03FF00000u); return *(ui10x3 *)&temp; };
   inline cui10x3 operator&(cui10x3 &value) const { cui32 temp = (data & value.data) & 0x03FFFFFFFu; return *(ui10x3 *)&temp; };
   inline cui10x3 operator|(cui10x3 &value) const { cui32 temp = (data | value.data) & 0x03FFFFFFFu; return *(ui10x3 *)&temp; };
   inline cui10x3 operator+=(cui10x3 &value) { data = ((data + value.data) & 0x03FFu) | ((data + (value.data << 10)) & 0x0FFC00) | ((data + (value.data << 20)) & 0x03FF00000u); return *this; };
   inline cui10x3 operator-=(cui10x3 &value) { data = ((data - value.data) & 0x03FFu) | ((data - (value.data << 10)) & 0x0FFC00) | ((data - (value.data << 20)) & 0x03FF00000u); return *this; };
   inline cui10x3 operator&=(cui10x3 &value) { data = (data & value.data) & 0x03FFFFFFFu; return *this; }
   inline cui10x3 operator|=(cui10x3 &value) { data = (data | value.data) & 0x03FFFFFFFu; return *this; }
};

typedef const ui6x3  cui6x3;
typedef const ui10x3 cui10x3;
