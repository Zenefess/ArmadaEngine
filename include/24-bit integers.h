/****************************************************************
 * File: 24-bit integers.h                  Created: 2024/05/13 *
 *                                    Last modified: 2024/05/13 *
 *                                                              *
 * Desc: Both signed & unsigned data types.                     *
 *                                                              *
 *                             Copyright (c) David William Bull *
 ****************************************************************/
#pragma once

#include "typedefs.h"

#define _24BIT_INTEGERS_

// 24-bit unsigned integer
struct ui24 {
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
   ui24(cfl32 value) {
      cui32 integer = (ui32)value;
      (ui16 &)data = (ui16 &)integer;
      data[2] = ((ui8_3)integer)[2];
   }
   ui24(cfl64 value) {
      cui32 integer = (ui32)value;
      (ui16 &)data = (cui16 &)integer;
      data[2] = ((ui8_3)integer)[2];
   }

   operator cui8(void) const { return (ui8 &)data; }
   operator csi8(void) const { return (si8 &)data; }
   operator cui16(void) const { return (ui16 &)data; }
   operator csi16(void) const { return (si16 &)data; }
   operator cui8_3(void) const { return (ui8_3)data; }
   operator cui32(void) const { return (ui32 &)data & 0x0FFFFFF; }
   operator csi32(void) const { return (si32 &)data & 0x0FFFFFF; }
   operator cui64(void) const { return (ui64 &)data & 0x0FFFFFF; }
   operator csi64(void) const { return (si64 &)data & 0x0FFFFFF; }
   operator cfl32(void) const { return fl32((si32 &)data & 0x0FFFFFF); }
   operator cfl64(void) const { return fl64((si32 &)data & 0x0FFFFFF); }

   inline ui24 &operator&(void) const { return (ui24 &)data; }

   inline cbool operator==(cui24 &value) const {
      return ((ui32 &)data & 0x0FFFFFF) == ((ui32 &)value & 0x0FFFFFF);
   }

   inline cbool operator==(csi32 &value) const {
      return ((ui32 &)data & 0x0FFFFFF) == ((ui32 &)value & 0x0FFFFFF);
   }

   inline cbool operator!=(cui24 &value) const {
      return ((ui32 &)data & 0x0FFFFFF) != ((ui32 &)value & 0x0FFFFFF);
   }

   inline cbool operator!=(csi32 &value) const {
      return ((ui32 &)data & 0x0FFFFFF) != ((ui32 &)value & 0x0FFFFFF);
   }

   inline cui24 operator+(cui24 &value) const {
      cui32 temp = ((ui32 &)data + (ui32 &)value) & 0x0FFFFFF;
      return (ui24 &)temp;
   };

   inline cui24 operator+(csi32 &value) const {
      cui32 temp = ((ui32 &)data + (ui32 &)value) & 0x0FFFFFF;
      return (ui24 &)temp;
   };

   inline cui24 operator-(cui24 &value) const {
      cui32 temp = ((ui32 &)data - (ui32 &)value) & 0x0FFFFFF;
      return (ui24 &)temp;
   };

   inline cui24 operator-(csi32 &value) const {
      cui32 temp = ((ui32 &)data - (ui32 &)value) & 0x0FFFFFF;
      return (ui24 &)temp;
   };

   inline cui24 operator<<(csi32 &value) const {
      cui32 temp = ((ui32 &)data & 0x0FFFFFF) << ((ui32 &)value & 0x0FFFFFF);
      return (ui24 &)temp;
   };

   inline cui24 operator<<(cui24 &value) const {
      cui32 temp = ((ui32 &)data & 0x0FFFFFF) << ((ui32 &)value & 0x0FFFFFF);
      return (ui24 &)temp;
   };

   inline cui24 operator>>(csi32 &value) const {
      cui32 temp = ((ui32 &)data & 0x0FFFFFF) >> ((ui32 &)value & 0x0FFFFFF);
      return (ui24 &)temp;
   };

   inline cui24 operator>>(cui24 &value) const {
      cui32 temp = ((ui32 &)data & 0x0FFFFFF) >> ((ui32 &)value & 0x0FFFFFF);
      return (ui24 &)temp;
   };

   inline cui24 operator+=(cui24 &value) {
      (ui16 &)data += (ui16 &)value;
      data[2] += value.data[2] + (data[1] < value.data[1]);
      return (ui24 &)data;
   };

   inline cui24 operator-=(cui24 &value) {
      (ui16 &)data -= (ui16 &)value;
      data[2] -= value.data[2] + (data[1] > value.data[1]);
      return (ui24 &)data;
   };

   inline cui24 operator*=(cui24 &value) {
      cui32 result = ((cui32 &)data & 0x0FFFFFF) * ((cui32 &)value & 0x0FFFFFF);
      (ui16 &)data = (ui16 &)result;
      data[2] = ((ui8_3)result)[2];
      return (ui24 &)data;
   };

   inline cui24 operator/=(cui24 &value) {
      cui32 result = ((cui32 &)data & 0x0FFFFFF) / ((cui32 &)value & 0x0FFFFFF);
      (ui16 &)data = (cui16 &)result;
      data[2] = ((ui8_3)result)[2];
      return (ui24 &)data;
   };
};

// 24-bit signed integer
struct si24 {
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
   si24(cfl32 value) {
      csi32 integer = (si32)value;
      (si16 &)data = (si16 &)integer;
      data[2] = ((si8_3)integer)[2];
   }
   si24(cfl64 value) {
      csi32 integer = (si32)value;
      (si16 &)data = (csi16 &)integer;
      data[2] = ((si8_3)integer)[2];
   }

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

   inline cbool operator==(csi24 &value) const {
      return ((si32 &)data & 0x0FFFFFF) == ((si32 &)value & 0x0FFFFFF);
   }

   inline cbool operator==(csi32 &value) const {
      return ((si32 &)data & 0x0FFFFFF) == ((si32 &)value & 0x0FFFFFF);
   }

   inline cbool operator!=(csi24 &value) const {
      return ((si32 &)data & 0x0FFFFFF) != ((si32 &)value & 0x0FFFFFF);
   }

   inline cbool operator!=(csi32 &value) const {
      return ((si32 &)data & 0x0FFFFFF) != ((si32 &)value & 0x0FFFFFF);
   }

   inline csi24 operator+(csi24 &value) const {
      csi32 temp = ((si32 &)data + (si32 &)value) & 0x0FFFFFF;
      return (si24 &)temp;
   };

   inline csi24 operator+(csi32 &value) const {
      csi32 temp = ((si32 &)data + (si32 &)value) & 0x0FFFFFF;
      return (si24 &)temp;
   };

   inline csi24 operator-(csi24 &value) const {
      csi32 temp = ((si32 &)data - (si32 &)value) & 0x0FFFFFF;
      return (si24 &)temp;
   };

   inline csi24 operator-(csi32 &value) const {
      csi32 temp = ((si32 &)data - (si32 &)value) & 0x0FFFFFF;
      return (si24 &)temp;
   };

   inline csi24 operator<<(csi32 &value) const {
      csi32 temp = ((si32 &)data & 0x0FFFFFF) << ((si32 &)value & 0x0FFFFFF);
      return (si24 &)temp;
   };

   inline csi24 operator<<(csi24 &value) const {
      csi32 temp = ((si32 &)data & 0x0FFFFFF) << ((si32 &)value & 0x0FFFFFF);
      return (si24 &)temp;
   };

   inline csi24 operator>>(csi32 &value) const {
      csi32 temp = ((si32 &)data & 0x0FFFFFF) >> ((si32 &)value & 0x0FFFFFF);
      return (si24 &)temp;
   };

   inline csi24 operator>>(csi24 &value) const {
      csi32 temp = ((si32 &)data & 0x0FFFFFF) >> ((si32 &)value & 0x0FFFFFF);
      return (si24 &)temp;
   };

   inline csi24 operator+=(csi24 &value) {
      (si16 &)data += (si16 &)value;
      data[2] += value.data[2] + (data[1] < value.data[1]);
      return (si24 &)data;
   };

   inline csi24 operator-=(csi24 &value) {
      (si16 &)data -= (si16 &)value;
      data[2] -= value.data[2] + (data[1] > value.data[1]);
      return (si24 &)data;
   };

   inline csi24 operator*=(csi24 &value) {
      csi32 result = ((csi32 &)data & 0x0FFFFFF) * ((csi32 &)value & 0x0FFFFFF);
      (si16 &)data = (si16 &)result;
      data[2] = ((si8_3)result)[2];
      return (si24 &)data;
   };

   inline csi24 operator/=(csi24 &value) {
      csi32 result = ((csi32 &)data & 0x0FFFFFF) / ((csi32 &)value & 0x0FFFFFF);
      (si16 &)data = (csi16 &)result;
      data[2] = ((si8_3)result)[2];
      return (si24 &)data;
   };
};

typedef const ui24         cui24;
typedef const si24         csi24;
typedef vol   ui24         vui24;
typedef vol   si24         vsi24;
typedef       ui24 *       ui24ptr;
typedef       si24 *       si24ptr;
typedef const ui24 *       cui24ptr;
typedef const si24 *       csi24ptr;
typedef vol   ui24 *       vui24ptr;
typedef vol   si24 *       vsi24ptr;
typedef       ui24 * const ui24ptrc;
typedef       si24 * const si24ptrc;
