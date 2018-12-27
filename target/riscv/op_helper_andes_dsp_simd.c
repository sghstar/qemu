/*
 * RISC-V Andes DSP Extension Emulation Helpers
 *
 * Copyright (c) 2018 Andes Tech. Corp.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>.
 */

#include "exec/cpu_ldst.h"
#include <stdio.h>
#define u8 uint8_t
#define s8 int8_t
#define u16p uint16_t*
#define s16p int16_t*
#define u16 uint16_t
#define s16 int16_t

#define s64 int64_t
#define u64 uint64_t

#define s32 int32_t
#define u32 uint32_t
static
s64 U_helper(s64 val, s32 range)
{
  s64 max = (1 << range) - 1;

  if (val > max)
    val = max;
  else if (val < 0)
    val = 0;
  return val;

}
static
s64 Q_helper(s64 val, s64 range)
{
  s64 max = (1 << range) - 1;
  s64 min = -(1 << range);

  if (val > max)
    val = max;

  else if (val < min)
    val = min;

  return val;

}
#if defined(TARGET_RISCV32)
const uint32_t LC_8BIT= 4;
#else
const uint32_t LC_8BIT = 8;
#endif

#define u8p uint8_t*
#define s8p int8_t*
#define u8 uint8_t
#define s8 int8_t
target_ulong helper_andes_dsp_sub8(target_ulong rs1, target_ulong rs2)
{
  u8p rs1_p = (u8p)&rs1;
  u8p rs2_p = (u8p)&rs2;
  target_ulong rd;
  u8p rd_p = (u8p)&rd;
  unsigned i = 0;

  for (; i < LC_8BIT; i++)
      rd_p[i] = rs1_p[i] - rs2_p[i];
  return rd;
}
target_ulong helper_andes_dsp_add8(target_ulong rs1, target_ulong rs2)
{
  u8p rs1_p = (u8p)&rs1;
  u8p rs2_p = (u8p)&rs2;
  target_ulong rd;
  u8p rd_p = (u8p)&rd;
  unsigned i = 0;

  for (; i < LC_8BIT; i++)
      rd_p[i] = rs1_p[i] + rs2_p[i];
  return rd;
}
target_ulong helper_andes_dsp_radd8(target_ulong rs1, target_ulong rs2)
{
  s8p rs1_p = (s8p)&rs1;
  s8p rs2_p = (s8p)&rs2;
  target_ulong rd;
  s8p rd_p = (s8p)&rd;
  unsigned i = 0;

  for (; i < LC_8BIT; i++)
      rd_p[i] = (rs1_p[i] + rs2_p[i]) >> 1;
  return rd;
}
target_ulong helper_andes_dsp_rsub8(target_ulong rs1, target_ulong rs2)
{
  s8p rs1_p = (s8p)&rs1;
  s8p rs2_p = (s8p)&rs2;
  target_ulong rd;
  s8p rd_p = (s8p)&rd;
  unsigned i = 0;

  for (; i < LC_8BIT; i++)
      rd_p[i] = (rs1_p[i] - rs2_p[i]) >> 1;
  return rd;
}
target_ulong helper_andes_dsp_ursub8(target_ulong rs1, target_ulong rs2)
{
  u8p rs1_p = (u8p)&rs1;
  u8p rs2_p = (u8p)&rs2;
  target_ulong rd;
  u8p rd_p = (u8p)&rd;
  unsigned i = 0;

  for (; i < LC_8BIT; i++)
      rd_p[i] = (rs1_p[i] - rs2_p[i]) >> 1;
  return rd;
}
target_ulong helper_andes_dsp_uradd8(target_ulong rs1, target_ulong rs2)
{
  u8p rs1_p = (u8p)&rs1;
  u8p rs2_p = (u8p)&rs2;
  target_ulong rd;
  u8p rd_p = (u8p)&rd;
  unsigned i = 0;

  for (; i < LC_8BIT; i++)
      rd_p[i] = (rs1_p[i] + rs2_p[i]) >> 1;
  return rd;
}
target_ulong helper_andes_dsp_ksub8(target_ulong rs1, target_ulong rs2);
target_ulong helper_andes_dsp_ksub8(target_ulong rs1, target_ulong rs2)
{
  s8p rs1_p = (s8p)&rs1;
  s8p rs2_p = (s8p)&rs2;
  target_ulong rd;
  s8p rd_p = (s8p)&rd;
  unsigned i = 0;

  for (; i < LC_8BIT; i++) {
     s64 tmp = (rs1_p[i] - rs2_p[i]);
     rd_p[i] =  Q_helper(tmp, 7);
  }
  return rd;
}
target_ulong helper_andes_dsp_kadd8(target_ulong rs1, target_ulong rs2);
target_ulong helper_andes_dsp_kadd8(target_ulong rs1, target_ulong rs2)
{
  s8p rs1_p = (s8p)&rs1;
  s8p rs2_p = (s8p)&rs2;
  target_ulong rd;
  s8p rd_p = (s8p)&rd;
  unsigned i = 0;

  for (; i < LC_8BIT; i++) {
     s64 tmp = (rs1_p[i] + rs2_p[i]);
     rd_p[i] =  Q_helper(tmp, 7);
  }
  return rd;
}
target_ulong helper_andes_dsp_uksub8(target_ulong rs1, target_ulong rs2);
target_ulong helper_andes_dsp_uksub8(target_ulong rs1, target_ulong rs2)
{
  u8p rs1_p = (u8p)&rs1;
  u8p rs2_p = (u8p)&rs2;
  target_ulong rd;
  u8p rd_p = (u8p)&rd;
  unsigned i = 0;

  for (; i < LC_8BIT; i++) {
     u64 tmp = (rs1_p[i] - rs2_p[i]);
     rd_p[i] = U_helper(tmp, 8);
  }
  return rd;
}
target_ulong helper_andes_dsp_ukadd8(target_ulong rs1, target_ulong rs2);
target_ulong helper_andes_dsp_ukadd8(target_ulong rs1, target_ulong rs2)
{
  u8p rs1_p = (u8p)&rs1;
  u8p rs2_p = (u8p)&rs2;
  target_ulong rd;
  u8p rd_p = (u8p)&rd;
  unsigned i = 0;

  for (; i < LC_8BIT; i++) {
     s64 tmp = (rs1_p[i] + rs2_p[i]);
     rd_p[i] = U_helper(tmp, 8);
  }
  return rd;
}

#if defined(TARGET_RISCV32)
const uint32_t LC_16BIT= 2;
#else
const uint32_t LC_16BIT = 4;
#endif

target_ulong helper_andes_dsp_sub16(target_ulong rs1, target_ulong rs2)
{
  u16p rs1_p = (u16p)&rs1;
  u16p rs2_p = (u16p)&rs2;
  target_ulong rd;
  u16p rd_p = (u16p)&rd;
  unsigned i = 0;

  for (; i < LC_16BIT; i++)
      rd_p[i] = rs1_p[i] - rs2_p[i];
  return rd;
}
target_ulong helper_andes_dsp_crsa16(target_ulong rs1, target_ulong rs2)
{
  u16p rs1_p = (u16p)&rs1;
  u16p rs2_p = (u16p)&rs2;
  target_ulong rd;
  u16p rd_p = (u16p)&rd;
  unsigned i = 1;
#if defined(TARGET_RISCV32)
const uint32_t LC= 1;
#else
const uint32_t LC= 3;
#endif

  for (; i <= LC; i+=2) {
      rd_p[i] = rs1_p[i] - rs2_p[i-1];
      rd_p[i-1] = rs1_p[i-1] + rs2_p[i];
  }
  return rd;
}
target_ulong helper_andes_dsp_cras16(target_ulong rs1, target_ulong rs2)
{
  u16p rs1_p = (u16p)&rs1;
  u16p rs2_p = (u16p)&rs2;
  target_ulong rd;
  u16p rd_p = (u16p)&rd;
  unsigned i = 1;
#if defined(TARGET_RISCV32)
const uint32_t LC= 1;
#else
const uint32_t LC= 3;
#endif

  for (; i <= LC; i+=2) {
      rd_p[i] = rs1_p[i] + rs2_p[i-1];
      rd_p[i-1] = rs1_p[i-1] - rs2_p[i];
  }
  return rd;
}
target_ulong helper_andes_dsp_srli8_u(target_ulong rs1, target_ulong rs2);
target_ulong helper_andes_dsp_srli8_u(target_ulong rs1, target_ulong rs2)
{
  u8p rs1_p = (u8p)&rs1;
  target_ulong rd;
  u8p rd_p = (u8p)&rd;
  unsigned i = 0;
  unsigned shift_amount = rs2;
  target_ulong rnd_mask = (1UL << (shift_amount - 1));
  int8_t rnd_val;

  if  (shift_amount == 0)
      return rs1;

  for (; i < LC_8BIT; i++) {
      rnd_val = (rs1_p[i] & rnd_mask) ? 1 : 0;
      rd_p[i] = (rs1_p[i] >> (shift_amount)) + rnd_val;
  }
  return rd;
}
target_ulong helper_andes_dsp_srli16_u(target_ulong rs1, target_ulong rs2)
{
  u16p rs1_p = (u16p)&rs1;
  target_ulong rd;
  u16p rd_p = (u16p)&rd;
  unsigned i = 0;
  unsigned shift_amount = rs2;
  target_ulong rnd_mask = (1UL << (shift_amount - 1));
  int16_t rnd_val;

  if  (shift_amount == 0)
      return rs1;

  for (; i < LC_16BIT; i++) {
      rnd_val = (rs1_p[i] & rnd_mask) ? 1 : 0;
      rd_p[i] = (rs1_p[i] >> (shift_amount)) + rnd_val;
  }
  return rd;
}
target_ulong helper_andes_dsp_srai8_u(target_ulong rs1, target_ulong rs2);
target_ulong helper_andes_dsp_srai8_u(target_ulong rs1, target_ulong rs2)
{
  s8p rs1_p = (s8p)&rs1;
  target_ulong rd;
  s8p rd_p = (s8p)&rd;
  unsigned i = 0;
  unsigned shift_amount = rs2;
  target_ulong rnd_mask = (1UL << (rs2 - 1));
  int8_t rnd_val;

  if  (shift_amount == 0)
      return rs1;

  for (; i < LC_8BIT; i++) {
      rnd_val = (rs1_p[i] & rnd_mask) ? 1 : 0;
      rd_p[i] = (rs1_p[i] >> (shift_amount)) + rnd_val;
  }
  return rd;
}
target_ulong helper_andes_dsp_srai16_u(target_ulong rs1, target_ulong rs2)
{
  s16p rs1_p = (s16p)&rs1;
  target_ulong rd;
  s16p rd_p = (s16p)&rd;
  unsigned i = 0;
  unsigned shift_amount = rs2;
  target_ulong rnd_mask = (1UL << (shift_amount - 1));
  int16_t rnd_val;

  if  (shift_amount == 0)
      return rs1;

  for (; i < LC_16BIT; i++) {
      rnd_val = (rs1_p[i] & rnd_mask) ? 1 : 0;
      rd_p[i] = (rs1_p[i] >> (shift_amount)) + rnd_val;
  }
  return rd;
}
target_ulong helper_andes_dsp_srli8(target_ulong rs1, target_ulong rs2)
{
  u8p rs1_p = (u8p)&rs1;
  target_ulong rd;
  u8p rd_p = (u8p)&rd;
  unsigned i = 0;
  unsigned shift_amount = rs2;

  for (; i < LC_8BIT; i++)
      rd_p[i] = rs1_p[i] >> (shift_amount);
  return rd;
}
target_ulong helper_andes_dsp_srli16(target_ulong rs1, target_ulong rs2)
{
  u16p rs1_p = (u16p)&rs1;
  target_ulong rd;
  u16p rd_p = (u16p)&rd;
  unsigned i = 0;
  unsigned shift_amount = rs2;

  for (; i < LC_16BIT; i++)
      rd_p[i] = rs1_p[i] >> (shift_amount);
  return rd;
}
target_ulong helper_andes_dsp_srai8(target_ulong rs1, target_ulong rs2)
{
  s8p rs1_p = (s8p)&rs1;
  target_ulong rd;
  s8p rd_p = (s8p)&rd;
  unsigned i = 0;
  unsigned shift_amount = rs2;

  for (; i < LC_8BIT; i++)
      rd_p[i] = rs1_p[i] >> (shift_amount);
  return rd;
}
target_ulong helper_andes_dsp_srai16(target_ulong rs1, target_ulong rs2)
{
  s16p rs1_p = (s16p)&rs1;
  target_ulong rd;
  s16p rd_p = (s16p)&rd;
  unsigned i = 0;
  unsigned shift_amount = rs2;

  for (; i < LC_16BIT; i++)
      rd_p[i] = rs1_p[i] >> (shift_amount);
  return rd;
}
target_ulong helper_andes_dsp_srl8_u(target_ulong rs1, target_ulong rs2)
{
  u8p rs1_p = (u8p)&rs1;
  target_ulong rd;
  u8p rd_p = (u8p)&rd;
  unsigned i = 0;
  unsigned shift_amount = (rs2 & 0x7);
  target_ulong rnd_mask = (1ULL << (shift_amount - 1));
  int16_t rnd_val;

  if  (shift_amount == 0)
      return rs1;

  for (; i < LC_8BIT; i++) {
      rnd_val = (rs1_p[i] & rnd_mask) ? 1 : 0;
      rd_p[i] = (rs1_p[i] >> (shift_amount)) + rnd_val;
  }
  return rd;
}
target_ulong helper_andes_dsp_srl16_u(target_ulong rs1, target_ulong rs2)
{
  u16p rs1_p = (u16p)&rs1;
  target_ulong rd;
  u16p rd_p = (u16p)&rd;
  unsigned i = 0;
  unsigned shift_amount = (rs2 & 0xf);
  target_ulong rnd_mask = (1UL << (shift_amount - 1));
  int16_t rnd_val;

  if  (shift_amount == 0)
      return rs1;

  for (; i < LC_16BIT; i++) {
      rnd_val = (rs1_p[i] & rnd_mask) ? 1 : 0;
      rd_p[i] = (rs1_p[i] >> (shift_amount)) + rnd_val;
  }
  return rd;
}
target_ulong helper_andes_dsp_sra8_u(target_ulong rs1, target_ulong rs2)
{
  s8p rs1_p = (s8p)&rs1;
  target_ulong rd;
  s8p rd_p = (s8p)&rd;
  unsigned i = 0;
  unsigned shift_amount = (rs2 & 0x7);
  target_ulong rnd_mask = (1UL << (shift_amount- 1));
  int8_t rnd_val;

  if  (shift_amount == 0)
      return rs1;

  for (; i < LC_8BIT; i++) {
      rnd_val = (rs1_p[i] & rnd_mask) ? 1 : 0;
      rd_p[i] = (rs1_p[i] >> (shift_amount)) + rnd_val;
  }
  return rd;
}
target_ulong helper_andes_dsp_sra16_u(target_ulong rs1, target_ulong rs2)
{
  s16p rs1_p = (s16p)&rs1;
  target_ulong rd;
  s16p rd_p = (s16p)&rd;
  unsigned i = 0;
  unsigned shift_amount = (rs2 & 0xf);
  target_ulong rnd_mask = (1UL << (shift_amount- 1));
  int16_t rnd_val;

  if  (shift_amount== 0)
      return rs1;

  for (; i < LC_16BIT; i++) {
      rnd_val = (rs1_p[i] & rnd_mask) ? 1 : 0;
      rd_p[i] = (rs1_p[i] >> (shift_amount)) + rnd_val;
  }
  return rd;
}
target_ulong helper_andes_dsp_kslli8(target_ulong rs1, target_ulong rs2);
target_ulong helper_andes_dsp_kslli8(target_ulong rs1, target_ulong rs2)
{
  s8p rs1_p = (s8p)&rs1;
  target_ulong rd;
  s8p rd_p = (s8p)&rd;
  unsigned i = 0;
  unsigned shift_amount = rs2;
  if (shift_amount == 0)
      return rs1;

  for (; i < LC_8BIT; i++) {
     s32 tmp1  = rs1_p[i] << (shift_amount);
     rd_p[i] =  Q_helper(tmp1, 7);
  }
  return rd;
}
target_ulong helper_andes_dsp_kslli16(target_ulong rs1, target_ulong rs2);
target_ulong helper_andes_dsp_kslli16(target_ulong rs1, target_ulong rs2)
{
  s16p rs1_p = (s16p)&rs1;
  target_ulong rd;
  s16p rd_p = (s16p)&rd;
  unsigned i = 0;
  unsigned shift_amount = rs2;
  if (shift_amount == 0)
      return rs1;

  for (; i < LC_16BIT; i++) {
     s32 tmp1  = rs1_p[i] << (shift_amount);
     rd_p[i] =  Q_helper(tmp1, 15);
  }
  return rd;
}
target_ulong helper_andes_dsp_kslra8(target_ulong rs1, target_ulong rs2);
target_ulong helper_andes_dsp_kslra8(target_ulong rs1, target_ulong rs2)
{
  s8p rs1_p = (s8p)&rs1;
  target_ulong rd;
  s8p rd_p = (s8p)&rd;
  unsigned i = 0;
  int rs2_4_0 = rs2 & 0x1f;

  if (rs2_4_0 < 0)
    {
      for (i = 0; i < LC_16BIT; i++)
	  rd_p[i]  = rs1_p[i] >> (-rs2_4_0);
    }
  else
    {
      if (rs2_4_0 != 0)
	{
	  for (i = 0; i < LC_16BIT; i++)
	    {
	      s64 tmp = ((u16) rs1_p[i]) << -rs2_4_0;
              rd_p[i] =  Q_helper(tmp, 15);
	    }
	}
      else
	rd = rs1;
    }
  return rd;
}
target_ulong helper_andes_dsp_kslra16(target_ulong rs1, target_ulong rs2);
target_ulong helper_andes_dsp_kslra16(target_ulong rs1, target_ulong rs2)
{
  s16p rs1_p = (s16p)&rs1;
  target_ulong rd;
  s16p rd_p = (s16p)&rd;
  unsigned i = 0;
  int rs2_4_0 = rs2 & 0x1f;

  if (rs2_4_0 < 0)
    {
      for (i = 0; i < LC_16BIT; i++)
	  rd_p[i]  = rs1_p[i] >> (-rs2_4_0);
    }
  else
    {
      if (rs2_4_0 != 0)
	{
	  for (i = 0; i < LC_16BIT; i++)
	    {
	      s64 tmp = ((u16) rs1_p[i]) << -rs2_4_0;
              rd_p[i] =  Q_helper(tmp, 15);
	    }
	}
      else
	rd = rs1;
    }
  return rd;
}
target_ulong helper_andes_dsp_ksll8(target_ulong rs1, target_ulong rs2);
target_ulong helper_andes_dsp_ksll8(target_ulong rs1, target_ulong rs2)
{
  s8p rs1_p = (s8p)&rs1;
  fprintf(stderr, "xxx");
  target_ulong rd;
  s8p rd_p = (s8p)&rd;
  unsigned i = 0;
  unsigned shift_amount = rs2 & 0x7;
  if (rs2 == 0)
      return rs1;

  for (; i < LC_8BIT; i++) {
     s64 tmp1  = rs1_p[i] << (shift_amount);
     rd_p[i] =  Q_helper(tmp1, 7);
  }
  return rd;
}
target_ulong helper_andes_dsp_scmple16(target_ulong rs1, target_ulong rs2)
{
  s16p rs1_p = (s16p)&rs1;
  s16p rs2_p = (s16p)&rs2;
  target_ulong rd;
  s16p rd_p = (s16p)&rd;
  unsigned i = 0;

  for (; i < LC_16BIT; i++) {
     rd_p[i]  = (rs1_p[i] <= rs2_p[i]) ? 0xffff : 0;
  }
  return rd;
}
target_ulong helper_andes_dsp_ucmple16(target_ulong rs1, target_ulong rs2)
{
  u16p rs1_p = (u16p)&rs1;
  u16p rs2_p = (u16p)&rs2;
  target_ulong rd;
  u16p rd_p = (u16p)&rd;
  unsigned i = 0;

  for (; i < LC_16BIT; i++) {
     rd_p[i]  = (rs1_p[i] <= rs2_p[i]) ? 0xffff : 0;
  }
  return rd;
}
target_ulong helper_andes_dsp_ucmplt16(target_ulong rs1, target_ulong rs2)
{
  u16p rs1_p = (u16p)&rs1;
  u16p rs2_p = (u16p)&rs2;
  target_ulong rd;
  u16p rd_p = (u16p)&rd;
  unsigned i = 0;

  for (; i < LC_16BIT; i++) {
     rd_p[i]  = (rs1_p[i] < rs2_p[i]) ? 0xffff : 0;
  }
  return rd;
}
target_ulong helper_andes_dsp_ucmplt8(target_ulong rs1, target_ulong rs2)
{
  u8p rs1_p = (u8p)&rs1;
  u8p rs2_p = (u8p)&rs2;
  target_ulong rd;
  u8p rd_p = (u8p)&rd;
  unsigned i = 0;

  for (; i < LC_8BIT; i++) {
     rd_p[i]  = (rs1_p[i] < rs2_p[i]) ? 0xff : 0;
  }
  return rd;
}
target_ulong helper_andes_dsp_ucmple8(target_ulong rs1, target_ulong rs2)
{
  u8p rs1_p = (u8p)&rs1;
  u8p rs2_p = (u8p)&rs2;
  target_ulong rd;
  u8p rd_p = (u8p)&rd;
  unsigned i = 0;

  for (; i < LC_8BIT; i++) {
     rd_p[i]  = (rs1_p[i] <= rs2_p[i]) ? 0xff : 0;
  }
  return rd;
}
target_ulong helper_andes_dsp_scmple8(target_ulong rs1, target_ulong rs2)
{
  s8p rs1_p = (s8p)&rs1;
  s8p rs2_p = (s8p)&rs2;
  target_ulong rd;
  s8p rd_p = (s8p)&rd;
  unsigned i = 0;

  for (; i < LC_8BIT; i++) {
     rd_p[i]  = (rs1_p[i] <= rs2_p[i]) ? 0xff : 0;
  }
  return rd;
}
target_ulong helper_andes_dsp_scmplt8(target_ulong rs1, target_ulong rs2)
{
  s8p rs1_p = (s8p)&rs1;
  s8p rs2_p = (s8p)&rs2;
  target_ulong rd;
  s8p rd_p = (s8p)&rd;
  unsigned i = 0;

  for (; i < LC_8BIT; i++) {
     rd_p[i]  = (rs1_p[i] < rs2_p[i]) ? 0xff : 0;
  }
  return rd;
}
target_ulong helper_andes_dsp_scmplt16(target_ulong rs1, target_ulong rs2)
{
  s16p rs1_p = (s16p)&rs1;
  s16p rs2_p = (s16p)&rs2;
  target_ulong rd;
  s16p rd_p = (s16p)&rd;
  unsigned i = 0;

  for (; i < LC_16BIT; i++) {
     rd_p[i]  = (rs1_p[i] < rs2_p[i]) ? 0xffff : 0;
  }
  return rd;
}
target_ulong helper_andes_dsp_cmpeq8(target_ulong rs1, target_ulong rs2)
{
  u8p rs1_p = (u8p)&rs1;
  u8p rs2_p = (u8p)&rs2;
  target_ulong rd;
  u8p rd_p = (u8p)&rd;
  unsigned i = 0;

  for (; i < LC_8BIT; i++) {
     rd_p[i]  = (rs1_p[i] == rs2_p[i]) ? 0xff : 0;
  }
  return rd;
}
target_ulong helper_andes_dsp_umin8(target_ulong rs1, target_ulong rs2)
{
  u8p rs1_p = (u8p)&rs1;
  u8p rs2_p = (u8p)&rs2;
  target_ulong rd;
  u8p rd_p = (u8p)&rd;
  unsigned i = 0;

  for (; i < LC_8BIT; i++) {
     rd_p[i]  = (rs1_p[i] < rs2_p[i]) ? rs1_p[i]: rs2_p[i];
  }
  return rd;
}
target_ulong helper_andes_dsp_umax8(target_ulong rs1, target_ulong rs2)
{
  u8p rs1_p = (u8p)&rs1;
  u8p rs2_p = (u8p)&rs2;
  target_ulong rd;
  u8p rd_p = (u8p)&rd;
  unsigned i = 0;

  for (; i < LC_8BIT; i++) {
     rd_p[i]  = (rs1_p[i] > rs2_p[i]) ? rs1_p[i]: rs2_p[i];
  }
  return rd;
}
target_ulong helper_andes_dsp_smin8(target_ulong rs1, target_ulong rs2)
{
  s8p rs1_p = (s8p)&rs1;
  s8p rs2_p = (s8p)&rs2;
  target_ulong rd;
  s8p rd_p = (s8p)&rd;
  unsigned i = 0;

  for (; i < LC_8BIT; i++) {
     rd_p[i]  = (rs1_p[i] < rs2_p[i]) ? rs1_p[i]: rs2_p[i];
  }
  return rd;
}
target_ulong helper_andes_dsp_smax8(target_ulong rs1, target_ulong rs2)
{
  s8p rs1_p = (s8p)&rs1;
  s8p rs2_p = (s8p)&rs2;
  target_ulong rd;
  s8p rd_p = (s8p)&rd;
  unsigned i = 0;

  for (; i < LC_8BIT; i++) {
     rd_p[i]  = (rs1_p[i] > rs2_p[i]) ? rs1_p[i]: rs2_p[i];
  }
  return rd;
}
target_ulong helper_andes_dsp_smax16(target_ulong rs1, target_ulong rs2)
{
  s16p rs1_p = (s16p)&rs1;
  s16p rs2_p = (s16p)&rs2;
  target_ulong rd;
  s16p rd_p = (s16p)&rd;
  unsigned i = 0;

  for (; i < LC_16BIT; i++) {
     rd_p[i]  = (rs1_p[i] > rs2_p[i]) ? rs1_p[i]: rs2_p[i];
  }
  return rd;
}
target_ulong helper_andes_dsp_uclip8(target_ulong rs1, target_ulong rs2)
{
  u8p rs1_p = (u8p)&rs1;
  target_ulong rd;
  u8p rd_p = (u8p)&rd;
  unsigned i = 0;
  unsigned n = rs2;

  for (; i < LC_8BIT; i++) {
     s8 tmp1  = rs1_p[i];
     rd_p[i] =  U_helper(tmp1, n);
  }
  return rd;
}
target_ulong helper_andes_dsp_uclip16(target_ulong rs1, target_ulong rs2)
{
  u16p rs1_p = (u16p)&rs1;
  target_ulong rd;
  u16p rd_p = (u16p)&rd;
  unsigned i = 0;
  unsigned n = rs2;

  for (; i < LC_16BIT; i++) {
     s16 tmp1  = rs1_p[i];
     rd_p[i] =  U_helper(tmp1, n);
  }
  return rd;
}
target_ulong helper_andes_dsp_sclip8(target_ulong rs1, target_ulong rs2)
{
  s8p rs1_p = (s8p)&rs1;
  target_ulong rd;
  s8p rd_p = (s8p)&rd;
  unsigned i = 0;
  unsigned n = rs2;

  for (; i < LC_8BIT; i++) {
     s64 tmp1  = rs1_p[i];
     rd_p[i] =  Q_helper(tmp1, n);
  }
  return rd;
}
target_ulong helper_andes_dsp_sclip16(target_ulong rs1, target_ulong rs2)
{
  s16p rs1_p = (s16p)&rs1;
  target_ulong rd;
  s16p rd_p = (s16p)&rd;
  unsigned i = 0;
  unsigned n = rs2;

  for (; i < LC_16BIT; i++) {
     s64 tmp1  = rs1_p[i];
     rd_p[i] =  Q_helper(tmp1, n);
  }
  return rd;
}
target_ulong helper_andes_dsp_smin16(target_ulong rs1, target_ulong rs2)
{
  s16p rs1_p = (s16p)&rs1;
  s16p rs2_p = (s16p)&rs2;
  target_ulong rd;
  s16p rd_p = (s16p)&rd;
  unsigned i = 0;

  for (; i < LC_16BIT; i++) {
     rd_p[i]  = (rs1_p[i] < rs2_p[i]) ? rs1_p[i]: rs2_p[i];
  }
  return rd;
}
target_ulong helper_andes_dsp_umax16(target_ulong rs1, target_ulong rs2)
{
  u16p rs1_p = (u16p)&rs1;
  u16p rs2_p = (u16p)&rs2;
  target_ulong rd;
  u16p rd_p = (u16p)&rd;
  unsigned i = 0;

  for (; i < LC_16BIT; i++) {
     rd_p[i]  = (rs1_p[i] > rs2_p[i]) ? rs1_p[i]: rs2_p[i];
  }
  return rd;
}
target_ulong helper_andes_dsp_umin16(target_ulong rs1, target_ulong rs2)
{
  u16p rs1_p = (u16p)&rs1;
  u16p rs2_p = (u16p)&rs2;
  target_ulong rd;
  u16p rd_p = (u16p)&rd;
  unsigned i = 0;

  for (; i < LC_16BIT; i++) {
     rd_p[i]  = (rs1_p[i] < rs2_p[i]) ? rs1_p[i]: rs2_p[i];
  }
  return rd;
}
target_ulong helper_andes_dsp_cmpeq16(target_ulong rs1, target_ulong rs2)
{
  u16p rs1_p = (u16p)&rs1;
  u16p rs2_p = (u16p)&rs2;
  target_ulong rd;
  u16p rd_p = (u16p)&rd;
  unsigned i = 0;

  for (; i < LC_16BIT; i++) {
     rd_p[i]  = (rs1_p[i] == rs2_p[i]) ? 0xffff : 0;
  }
  return rd;
}
target_ulong helper_andes_dsp_ksll16(target_ulong rs1, target_ulong rs2);
target_ulong helper_andes_dsp_ksll16(target_ulong rs1, target_ulong rs2)
{
  s16p rs1_p = (s16p)&rs1;
  target_ulong rd;
  s16p rd_p = (s16p)&rd;
  unsigned i = 0;
  unsigned shift_amount = rs2 & 0xf;
  if (shift_amount == 0)
      return rs1;

  for (; i < LC_16BIT; i++) {
     s64 tmp1  = rs1_p[i] << (shift_amount);
     rd_p[i] =  Q_helper(tmp1, 15);
  }
  return rd;
}
target_ulong helper_andes_dsp_slli8(target_ulong rs1, target_ulong rs2)
{
  u8p rs1_p = (u8p)&rs1;
  target_ulong rd;
  u8p rd_p = (u8p)&rd;
  unsigned i = 0;
  unsigned shift_amount = rs2;
  if (rs2 == 0)
      return rs1;

  for (; i < LC_8BIT; i++)
      rd_p[i] = rs1_p[i] << (shift_amount);
  return rd;
}
target_ulong helper_andes_dsp_slli16(target_ulong rs1, target_ulong rs2)
{
  u16p rs1_p = (u16p)&rs1;
  target_ulong rd;
  u16p rd_p = (u16p)&rd;
  unsigned i = 0;
  unsigned shift_amount = rs2;
  if (rs2 == 0)
      return rs1;

  for (; i < LC_16BIT; i++)
      rd_p[i] = rs1_p[i] << (shift_amount);
  return rd;
}
target_ulong helper_andes_dsp_sll8(target_ulong rs1, target_ulong rs2)
{
  u8p rs1_p = (u8p)&rs1;
  target_ulong rd;
  u8p rd_p = (u8p)&rd;
  unsigned i = 0;
  unsigned shift_amount = rs2 & 0x7;
  if (rs2 == 0)
      return rs1;

  for (; i < LC_8BIT; i++)
      rd_p[i] = rs1_p[i] << (shift_amount);
  return rd;
}
target_ulong helper_andes_dsp_sll16(target_ulong rs1, target_ulong rs2)
{
  u16p rs1_p = (u16p)&rs1;
  target_ulong rd;
  u16p rd_p = (u16p)&rd;
  unsigned i = 0;
  unsigned shift_amount = rs2 & 0xf;
  if (rs2 == 0)
      return rs1;

  for (; i < LC_16BIT; i++)
      rd_p[i] = rs1_p[i] << (shift_amount);
  return rd;
}
target_ulong helper_andes_dsp_srl8(target_ulong rs1, target_ulong rs2)
{
  u8p rs1_p = (u8p)&rs1;
  target_ulong rd;
  u8p rd_p = (u8p)&rd;
  unsigned i = 0;
  unsigned shift_amount = rs2 & 0x7;
  if (rs2 == 0)
      return rs1;

  for (; i < LC_8BIT; i++)
      rd_p[i] = rs1_p[i] >> (shift_amount);
  return rd;
}
target_ulong helper_andes_dsp_srl16(target_ulong rs1, target_ulong rs2)
{
  u16p rs1_p = (u16p)&rs1;
  target_ulong rd;
  u16p rd_p = (u16p)&rd;
  unsigned i = 0;
  unsigned shift_amount = rs2 & 0xf;
  if (rs2 == 0)
      return rs1;

  for (; i < LC_16BIT; i++)
      rd_p[i] = rs1_p[i] >> (shift_amount);
  return rd;
}
target_ulong helper_andes_dsp_sra8(target_ulong rs1, target_ulong rs2)
{
  s8p rs1_p = (s8p)&rs1;
  target_ulong rd;
  s8p rd_p = (s8p)&rd;
  unsigned i = 0;
  unsigned shift_amount = rs2 & 0x7;
  if (rs2 == 0)
      return rs1;

  for (; i < LC_8BIT; i++)
      rd_p[i] = rs1_p[i] >> (shift_amount);
  return rd;
}
target_ulong helper_andes_dsp_sra16(target_ulong rs1, target_ulong rs2)
{
  s16p rs1_p = (s16p)&rs1;
  target_ulong rd;
  s16p rd_p = (s16p)&rd;
  unsigned i = 0;
  unsigned shift_amount = rs2 & 0xf;
  if (rs2 == 0)
      return rs1;

  for (; i < LC_16BIT; i++)
      rd_p[i] = rs1_p[i] >> (shift_amount);
  return rd;
}
target_ulong helper_andes_dsp_add16(target_ulong rs1, target_ulong rs2)
{
  u16p rs1_p = (u16p)&rs1;
  u16p rs2_p = (u16p)&rs2;
  target_ulong rd;
  u16p rd_p = (u16p)&rd;
  unsigned i = 0;

  for (; i < LC_16BIT; i++)
      rd_p[i] = rs1_p[i] + rs2_p[i];
  return rd;
}
target_ulong helper_andes_dsp_rsub16(target_ulong rs1, target_ulong rs2)
{
  s16p rs1_p = (s16p)&rs1;
  s16p rs2_p = (s16p)&rs2;
  target_ulong rd;
  s16p rd_p = (s16p)&rd;
  unsigned i = 0;

  for (; i < LC_16BIT; i++)
      rd_p[i] = (rs1_p[i] - rs2_p[i]) >> 1;
  return rd;
}
target_ulong helper_andes_dsp_radd16(target_ulong rs1, target_ulong rs2)
{
  s16p rs1_p = (s16p)&rs1;
  s16p rs2_p = (s16p)&rs2;
  target_ulong rd;
  s16p rd_p = (s16p)&rd;
  unsigned i = 0;

  for (; i < LC_16BIT; i++)
      rd_p[i] = (rs1_p[i] + rs2_p[i]) >> 1;
  return rd;
}
target_ulong helper_andes_dsp_rcrsa16(target_ulong rs1, target_ulong rs2)
{
  s16p rs1_p = (s16p)&rs1;
  s16p rs2_p = (s16p)&rs2;
  target_ulong rd;
  s16p rd_p = (s16p)&rd;
  unsigned i = 1;
#if defined(TARGET_RISCV32)
const uint32_t LC= 1;
#else
const uint32_t LC= 3;
#endif

  for (; i <= LC; i+=2) {
      rd_p[i] = (rs1_p[i] - rs2_p[i-1]) >> 1;
      rd_p[i-1] = (rs1_p[i-1] + rs2_p[i]) >> 1;
  }
  return rd;
}
target_ulong helper_andes_dsp_rcras16(target_ulong rs1, target_ulong rs2)
{
  s16p rs1_p = (s16p)&rs1;
  s16p rs2_p = (s16p)&rs2;
  target_ulong rd;
  s16p rd_p = (s16p)&rd;
  unsigned i = 1;
#if defined(TARGET_RISCV32)
const uint32_t LC= 1;
#else
const uint32_t LC= 3;
#endif

  for (; i <= LC; i+=2) {
      rd_p[i] = (rs1_p[i] + rs2_p[i-1]) >> 1;
      rd_p[i-1] = (rs1_p[i-1] - rs2_p[i]) >> 1;
  }
  return rd;
}
target_ulong helper_andes_dsp_urcrsa16(target_ulong rs1, target_ulong rs2)
{
  u16p rs1_p = (u16p)&rs1;
  u16p rs2_p = (u16p)&rs2;
  target_ulong rd;
  u16p rd_p = (u16p)&rd;
  unsigned i = 1;
#if defined(TARGET_RISCV32)
const uint32_t LC= 1;
#else
const uint32_t LC= 3;
#endif

  for (; i <= LC; i+=2) {
      rd_p[i] = (rs1_p[i] - rs2_p[i-1]) >> 1;
      rd_p[i-1] = (rs1_p[i-1] + rs2_p[i]) >> 1;
  }
  return rd;
}

target_ulong helper_andes_dsp_urcras16(target_ulong rs1, target_ulong rs2)
{
  u16p rs1_p = (u16p)&rs1;
  u16p rs2_p = (u16p)&rs2;
  target_ulong rd;
  u16p rd_p = (u16p)&rd;
  unsigned i = 1;
#if defined(TARGET_RISCV32)
const uint32_t LC= 1;
#else
const uint32_t LC= 3;
#endif

  for (; i <= LC; i+=2) {
      rd_p[i] = (rs1_p[i] + rs2_p[i-1]) >> 1;
      rd_p[i-1] = (rs1_p[i-1] - rs2_p[i]) >> 1;
  }
  return rd;
}
target_ulong helper_andes_dsp_ursub16(target_ulong rs1, target_ulong rs2)
{
  u16p rs1_p = (u16p)&rs1;
  u16p rs2_p = (u16p)&rs2;
  target_ulong rd;
  u16p rd_p = (u16p)&rd;
  unsigned i = 0;

  for (; i < LC_16BIT; i++)
      rd_p[i] = (rs1_p[i] - rs2_p[i]) >> 1;
  return rd;
}
target_ulong helper_andes_dsp_uradd16(target_ulong rs1, target_ulong rs2)
{
  u16p rs1_p = (u16p)&rs1;
  u16p rs2_p = (u16p)&rs2;
  target_ulong rd;
  u16p rd_p = (u16p)&rd;
  unsigned i = 0;

  for (; i < LC_16BIT; i++)
      rd_p[i] = (rs1_p[i] + rs2_p[i]) >> 1;
  return rd;
}



target_ulong helper_andes_dsp_ukcrsa16(target_ulong rs1, target_ulong rs2);
target_ulong helper_andes_dsp_ukcrsa16(target_ulong rs1, target_ulong rs2)
{
  u16p rs1_p = (u16p)&rs1;
  u16p rs2_p = (u16p)&rs2;
  target_ulong rd;
  u16p rd_p = (u16p)&rd;
  unsigned i = 1;
#if defined(TARGET_RISCV32)
const uint32_t LC= 1;
#else
const uint32_t LC= 3;
#endif

  for (; i <= LC; i+=2) {
     s64 tmp1 = (rs1_p[i] - rs2_p[i-1]);
     rd_p[i] = U_helper(tmp1, 16);
     s64 tmp2 = (rs1_p[i-1] + rs2_p[i]);
     rd_p[i-1] = U_helper(tmp2, 16);
  }
  return rd;
}
target_ulong helper_andes_dsp_ukcras16(target_ulong rs1, target_ulong rs2);
target_ulong helper_andes_dsp_ukcras16(target_ulong rs1, target_ulong rs2)
{
  u16p rs1_p = (u16p)&rs1;
  u16p rs2_p = (u16p)&rs2;
  target_ulong rd;
  u16p rd_p = (u16p)&rd;
  unsigned i = 1;
#if defined(TARGET_RISCV32)
const uint32_t LC= 1;
#else
const uint32_t LC= 3;
#endif

  for (; i <= LC; i+=2) {
     s64 tmp1 = (rs1_p[i] + rs2_p[i-1]);
     rd_p[i] = U_helper(tmp1, 16);
     s64 tmp2 = (rs1_p[i-1] - rs2_p[i]);
     rd_p[i-1] = U_helper(tmp2, 16);
  }
  return rd;
}
target_ulong helper_andes_dsp_uksub16(target_ulong rs1, target_ulong rs2);
target_ulong helper_andes_dsp_uksub16(target_ulong rs1, target_ulong rs2)
{
  u16p rs1_p = (u16p)&rs1;
  u16p rs2_p = (u16p)&rs2;
  target_ulong rd;
  u16p rd_p = (u16p)&rd;
  unsigned i = 0;

  for (; i < LC_16BIT; i++) {
     u64 tmp = (rs1_p[i] - rs2_p[i]);
     rd_p[i] = U_helper(tmp, 16);
  }
  return rd;
}
target_ulong helper_andes_dsp_ukadd16(target_ulong rs1, target_ulong rs2);
target_ulong helper_andes_dsp_ukadd16(target_ulong rs1, target_ulong rs2)
{
  u16p rs1_p = (u16p)&rs1;
  u16p rs2_p = (u16p)&rs2;
  target_ulong rd;
  u16p rd_p = (u16p)&rd;
  unsigned i = 0;

  for (; i < LC_16BIT; i++) {
     s64 tmp = (rs1_p[i] + rs2_p[i]);
     rd_p[i] = U_helper(tmp, 16);
  }
  return rd;
}
target_ulong helper_andes_dsp_kcrsa16(target_ulong rs1, target_ulong rs2);
target_ulong helper_andes_dsp_kcrsa16(target_ulong rs1, target_ulong rs2)
{
  s16p rs1_p = (s16p)&rs1;
  s16p rs2_p = (s16p)&rs2;
  target_ulong rd;
  s16p rd_p = (s16p)&rd;
  unsigned i = 1;
#if defined(TARGET_RISCV32)
const uint32_t LC= 1;
#else
const uint32_t LC= 3;
#endif

  for (; i <= LC; i+=2) {
     s64 tmp1 = (rs1_p[i] - rs2_p[i-1]);
     rd_p[i] =  Q_helper(tmp1, 15);
     s64 tmp2 = (rs1_p[i-1] + rs2_p[i]);
     rd_p[i-1] =  Q_helper(tmp2, 15);
  }
  return rd;
}
target_ulong helper_andes_dsp_kcras16(target_ulong rs1, target_ulong rs2);
target_ulong helper_andes_dsp_kcras16(target_ulong rs1, target_ulong rs2)
{
  s16p rs1_p = (s16p)&rs1;
  s16p rs2_p = (s16p)&rs2;
  target_ulong rd;
  s16p rd_p = (s16p)&rd;
  unsigned i = 1;
#if defined(TARGET_RISCV32)
const uint32_t LC= 1;
#else
const uint32_t LC= 3;
#endif

  for (; i <= LC; i+=2) {
     s64 tmp1 = (rs1_p[i] + rs2_p[i-1]);
     rd_p[i] =  Q_helper(tmp1, 15);
     s64 tmp2 = (rs1_p[i-1] - rs2_p[i]);
     rd_p[i-1] =  Q_helper(tmp2, 15);
  }
  return rd;
}
target_ulong helper_andes_dsp_ksub16(target_ulong rs1, target_ulong rs2);
target_ulong helper_andes_dsp_ksub16(target_ulong rs1, target_ulong rs2)
{
  s16p rs1_p = (s16p)&rs1;
  s16p rs2_p = (s16p)&rs2;
  target_ulong rd;
  s16p rd_p = (s16p)&rd;
  unsigned i = 0;

  for (; i < LC_16BIT; i++) {
     s64 tmp = (rs1_p[i] - rs2_p[i]);
     rd_p[i] =  Q_helper(tmp, 15);
  }
  return rd;
}
target_ulong helper_andes_dsp_kadd16(target_ulong rs1, target_ulong rs2);
target_ulong helper_andes_dsp_kadd16(target_ulong rs1, target_ulong rs2)
{
  s16p rs1_p = (s16p)&rs1;
  s16p rs2_p = (s16p)&rs2;
  target_ulong rd;
  s16p rd_p = (s16p)&rd;
  unsigned i = 0;

  for (; i < LC_16BIT; i++) {
     s64 tmp = (rs1_p[i] + rs2_p[i]);
     rd_p[i] =  Q_helper(tmp, 15);
  }
  return rd;
}
target_ulong helper_andes_dsp_sunpkd832(target_ulong rs1)
{
  s8p rs1_p = (s8p)&rs1;
  target_ulong rd;
  s16p rd_p = (s16p)&rd;

  rd_p[0] = (s16)rs1_p[2];
  rd_p[1] = (s16)rs1_p[3];
#if defined(TARGET_RISCV64)
  rd_p[2] = (s16)rs1_p[6];
  rd_p[3] = (s16)rs1_p[7];
#endif
  return rd;
}
target_ulong helper_andes_dsp_zunpkd832(target_ulong rs1)
{
  u8p rs1_p = (u8p)&rs1;
  target_ulong rd;
  u16p rd_p = (u16p)&rd;

  rd_p[0] = (u16)rs1_p[2];
  rd_p[1] = (u16)rs1_p[3];
#if defined(TARGET_RISCV64)
  rd_p[2] = (u16)rs1_p[6];
  rd_p[3] = (u16)rs1_p[7];
#endif
  return rd;
}
target_ulong helper_andes_dsp_sunpkd831(target_ulong rs1)
{
  s8p rs1_p = (s8p)&rs1;
  target_ulong rd;
  s16p rd_p = (s16p)&rd;

  rd_p[0] = (s16)rs1_p[1];
  rd_p[1] = (s16)rs1_p[3];
#if defined(TARGET_RISCV64)
  rd_p[2] = (s16)rs1_p[5];
  rd_p[3] = (s16)rs1_p[7];
#endif
  return rd;
}
target_ulong helper_andes_dsp_zunpkd831(target_ulong rs1)
{
  u8p rs1_p = (u8p)&rs1;
  target_ulong rd;
  u16p rd_p = (u16p)&rd;

  rd_p[0] = (u16)rs1_p[1];
  rd_p[1] = (u16)rs1_p[3];
#if defined(TARGET_RISCV64)
  rd_p[2] = (u16)rs1_p[5];
  rd_p[3] = (u16)rs1_p[7];
#endif
  return rd;
}
target_ulong helper_andes_dsp_sunpkd830(target_ulong rs1)
{
  s8p rs1_p = (s8p)&rs1;
  target_ulong rd;
  s16p rd_p = (s16p)&rd;

  rd_p[0] = (s16)rs1_p[0];
  rd_p[1] = (s16)rs1_p[3];
#if defined(TARGET_RISCV64)
  rd_p[2] = (s16)rs1_p[4];
  rd_p[3] = (s16)rs1_p[7];
#endif
  return rd;
}
target_ulong helper_andes_dsp_zunpkd830(target_ulong rs1)
{
  u8p rs1_p = (u8p)&rs1;
  target_ulong rd;
  u16p rd_p = (u16p)&rd;

  rd_p[0] = (u16)rs1_p[0];
  rd_p[1] = (u16)rs1_p[3];
#if defined(TARGET_RISCV64)
  rd_p[2] = (u16)rs1_p[4];
  rd_p[3] = (u16)rs1_p[7];
#endif
  return rd;
}
target_ulong helper_andes_dsp_sunpkd820(target_ulong rs1)
{
  s8p rs1_p = (s8p)&rs1;
  target_ulong rd;
  s16p rd_p = (s16p)&rd;

  rd_p[0] = (s16)rs1_p[0];
  rd_p[1] = (s16)rs1_p[2];
#if defined(TARGET_RISCV64)
  rd_p[2] = (s16)rs1_p[4];
  rd_p[3] = (s16)rs1_p[6];
#endif
  return rd;
}
target_ulong helper_andes_dsp_zunpkd820(target_ulong rs1)
{
  u8p rs1_p = (u8p)&rs1;
  target_ulong rd;
  u16p rd_p = (u16p)&rd;

  rd_p[0] = (u16)rs1_p[0];
  rd_p[1] = (u16)rs1_p[2];
#if defined(TARGET_RISCV64)
  rd_p[2] = (u16)rs1_p[4];
  rd_p[3] = (u16)rs1_p[6];
#endif
  return rd;
}
target_ulong helper_andes_dsp_sunpkd810(target_ulong rs1)
{
  s8p rs1_p = (s8p)&rs1;
  target_ulong rd;
  s16p rd_p = (s16p)&rd;

  rd_p[0] = (s16)rs1_p[0];
  rd_p[1] = (s16)rs1_p[1];

#if defined(TARGET_RISCV64)
  rd_p[2] = (s16)rs1_p[3];
  rd_p[3] = (s16)rs1_p[5];
#endif
  return rd;
}
target_ulong helper_andes_dsp_zunpkd810(target_ulong rs1)
{
  u8p rs1_p = (u8p)&rs1;
  target_ulong rd;
  u16p rd_p = (u16p)&rd;

  rd_p[0] = (u16)rs1_p[0];
  rd_p[1] = (u16)rs1_p[1];

#if defined(TARGET_RISCV64)
  rd_p[2] = (u16)rs1_p[3];
  rd_p[3] = (u16)rs1_p[5];
#endif
  return rd;
}

u16 clz(u16 v, int n);
u16 clz(u16 v, int n) {

    u16 count = 0;
    int i;
    for (i = n-1; i >= 0; i--) {
	if (((v >> i) & 0x1) == 0x0)
	  count++;
	else
	  return count;
    }
    return count;
}
u16 clo(u16 v, int n);
u16 clo(u16 v, int n) {

    u16 count = 0;
    int i;
    for (i = n-1; i >= 0; i--) {
	if (((v >> i) & 0x1) == 0x1)
	  count++;
	else
	  return count;
    }
    return count;
}
u16 clrs(u16 v, int n);
u16 clrs(u16 v, int n) {

    u16 count = 0;
    int sign = ((v >> (n-1))) & 0x1;
    int i;
    for (i = n-2; i >= 0; i--) {
	if (((v >> i) & 0x1) == sign)
	  count++;
	else
	  return count;
    }
    return count;
}
target_ulong helper_andes_dsp_clz8(target_ulong rs1)
{
  u8p rs1_p = (u8p)&rs1;
  target_ulong rd;
  u8p rd_p = (u8p)&rd;
  int i = 0;

  for (; i < LC_8BIT; i++)
      rd_p[i] = clz(rs1_p[i], 8);

  return rd;
}
target_ulong helper_andes_dsp_clz16(target_ulong rs1)
{
  u16p rs1_p = (u16p)&rs1;
  target_ulong rd;
  u16p rd_p = (u16p)&rd;
  int i = 0;

  for (; i < LC_16BIT; i++)
      rd_p[i] = clz(rs1_p[i], 16);

  return rd;
}
target_ulong helper_andes_dsp_clo8(target_ulong rs1);
target_ulong helper_andes_dsp_clo8(target_ulong rs1)
{
  u8p rs1_p = (u8p)&rs1;
  target_ulong rd;
  u8p rd_p = (u8p)&rd;
  int i = 0;

  for (; i < LC_8BIT; i++)
      rd_p[i] = clo(rs1_p[i], 8);

  return rd;
}
target_ulong helper_andes_dsp_clo16(target_ulong rs1);
target_ulong helper_andes_dsp_clo16(target_ulong rs1)
{
  u16p rs1_p = (u16p)&rs1;
  target_ulong rd;
  u16p rd_p = (u16p)&rd;
  int i = 0;

  for (; i < LC_16BIT; i++)
      rd_p[i] = clo(rs1_p[i], 16);

  return rd;
}
target_ulong helper_andes_dsp_clrs16(target_ulong rs1)
{
  u16p rs1_p = (u16p)&rs1;
  target_ulong rd;
  u16p rd_p = (u16p)&rd;
  int i = 0;

  for (; i < LC_16BIT; i++)
      rd_p[i] = clrs(rs1_p[i], 16);

  return rd;
}
target_ulong helper_andes_dsp_clrs8(target_ulong rs1)
{
  u8p rs1_p = (u8p)&rs1;
  target_ulong rd;
  u8p rd_p = (u8p)&rd;
  int i = 0;

  for (; i < LC_8BIT; i++)
      rd_p[i] = clrs(rs1_p[i], 8);

  return rd;
}
target_ulong helper_andes_dsp_kabs16(target_ulong rs1);
target_ulong helper_andes_dsp_kabs16(target_ulong rs1)
{
  u16p rs1_p = (u16p)&rs1;
  target_ulong rd;
  u16p rd_p = (u16p)&rd;
  int i = 0;

  for (; i < LC_16BIT; i++) {
    if (rs1_p[i] == 0x8000)
      rd_p[i] = 0x7fff;
    else if (((rs1_p[i] >> 15) & 0x1) == 1)
      rd_p[i] = -(s16)(rs1_p[i]);
    else
      rd_p[i] = rs1_p[i];
  }

  return rd;
}
target_ulong helper_andes_dsp_kabs8(target_ulong rs1);
target_ulong helper_andes_dsp_kabs8(target_ulong rs1)
{
  u8p rs1_p = (u8p)&rs1;
  target_ulong rd;
  u8p rd_p = (u8p)&rd;
  int i = 0;

  for (; i < LC_8BIT; i++) {
    if (rs1_p[i] == 0x80)
      rd_p[i] = 0x7f;
    else if (((rs1_p[i] >> 7) & 0x1) == 1)
      rd_p[i] = -(s8)(rs1_p[i]);
    else
      rd_p[i] = rs1_p[i];
  }

  return rd;
}
target_ulong helper_andes_dsp_maxw(target_ulong rs1, target_ulong rs2)
{
  s32 ra = rs1;
  s32 rb = rs2;
  return (ra > rb) ? ra : rb;
}
target_ulong helper_andes_dsp_minw(target_ulong rs1, target_ulong rs2)
{
  s32 ra = rs1;
  s32 rb = rs2;
  return (ra < rb) ? ra : rb;
}

target_ulong helper_andes_dsp_stas16(target_ulong rs1, target_ulong rs2)
{

  u16p rs1_p = (u16p)&rs1;
  u16p rs2_p = (u16p)&rs2;
  target_ulong rd;
  u16p rd_p = (u16p)&rd;
  unsigned i = 1;
#if defined(TARGET_RISCV32)
const uint32_t LC= 1;
#else
const uint32_t LC= 3;
#endif

  for (; i <= LC; i+=2) {
      rd_p[i] = rs1_p[i] + rs2_p[i];
      rd_p[i-1] = rs1_p[i-1] - rs2_p[i-1];
  }
  return rd;
}
target_ulong helper_andes_dsp_stsa16(target_ulong rs1, target_ulong rs2)
{

  u16p rs1_p = (u16p)&rs1;
  u16p rs2_p = (u16p)&rs2;
  target_ulong rd;
  u16p rd_p = (u16p)&rd;
  unsigned i = 1;
#if defined(TARGET_RISCV32)
const uint32_t LC= 1;
#else
const uint32_t LC= 3;
#endif

  for (; i <= LC; i+=2) {
      rd_p[i] = rs1_p[i] - rs2_p[i];
      rd_p[i-1] = rs1_p[i-1] + rs2_p[i-1];
  }
  return rd;
}
target_ulong helper_andes_dsp_rstas16(target_ulong rs1, target_ulong rs2)
{

  s16p rs1_p = (s16p)&rs1;
  s16p rs2_p = (s16p)&rs2;
  target_ulong rd;
  s16p rd_p = (s16p)&rd;
  unsigned i = 1;
#if defined(TARGET_RISCV32)
const uint32_t LC= 1;
#else
const uint32_t LC= 3;
#endif

  for (; i <= LC; i+=2) {
      rd_p[i] = (rs1_p[i] + rs2_p[i]) >> 1;
      rd_p[i-1] = (rs1_p[i-1] - rs2_p[i-1]) >> 1;
  }
  return rd;
}
target_ulong helper_andes_dsp_rstsa16(target_ulong rs1, target_ulong rs2)
{

  s16p rs1_p = (s16p)&rs1;
  s16p rs2_p = (s16p)&rs2;
  target_ulong rd;
  s16p rd_p = (s16p)&rd;
  unsigned i = 1;
#if defined(TARGET_RISCV32)
const uint32_t LC= 1;
#else
const uint32_t LC= 3;
#endif

  for (; i <= LC; i+=2) {
      rd_p[i] = (rs1_p[i] - rs2_p[i]) >> 1;
      rd_p[i-1] = (rs1_p[i-1] + rs2_p[i-1]) >> 1;
  }
  return rd;
}
target_ulong helper_andes_dsp_urstas16(target_ulong rs1, target_ulong rs2)
{

  u16p rs1_p = (u16p)&rs1;
  u16p rs2_p = (u16p)&rs2;
  target_ulong rd;
  u16p rd_p = (u16p)&rd;
  unsigned i = 1;
#if defined(TARGET_RISCV32)
const uint32_t LC= 1;
#else
const uint32_t LC= 3;
#endif

  for (; i <= LC; i+=2) {
      rd_p[i] = (rs1_p[i] + rs2_p[i]) >> 1;
      rd_p[i-1] = (rs1_p[i-1] - rs2_p[i-1]) >> 1;
  }
  return rd;

}
target_ulong helper_andes_dsp_urstsa16(target_ulong rs1, target_ulong rs2)
{

  u16p rs1_p = (u16p)&rs1;
  u16p rs2_p = (u16p)&rs2;
  target_ulong rd;
  u16p rd_p = (u16p)&rd;
  unsigned i = 1;
#if defined(TARGET_RISCV32)
const uint32_t LC= 1;
#else
const uint32_t LC= 3;
#endif

  for (; i <= LC; i+=2) {
      rd_p[i] = (rs1_p[i] - rs2_p[i]) >> 1;
      rd_p[i-1] = (rs1_p[i-1] + rs2_p[i-1]) >> 1;
  }
  return rd;

}
uint64_t helper_andes_dsp_getdouble(uint32_t even, uint32_t odd)
{
    return ((uint64_t)odd << 32) | ((uint64_t)even & 0xFFFFFFFF);
}
uint32_t helper_andes_dsp_seteven(uint64_t Val)
{

      return  Val & 0xFFFFFFFF;
}
uint32_t helper_andes_dsp_setodd(uint64_t Val)
{
      return (Val >> 32) & 0xFFFFFFFF;

}
