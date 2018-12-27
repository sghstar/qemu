

#define sext32(x) ((int64_t)(int32_t)(x))

#if defined(TARGET_RISCV32)
#define sext_xlen(x) ((int64_t)(int32_t)(x))
#define UINT uint32_t
#else
#define sext_xlen(x) ((int64_t)(int64_t)(x))
#define UINT uint64_t
#endif
#if defined(TARGET_RISCV32)
#define zext_xlen(x) ((uint64_t)(int32_t)(x))
#else
#define zext_xlen(x) ((uint64_t)(int64_t)(x))
#endif

unsigned char ReverseBits7ops32bit(unsigned char v);
unsigned char ReverseBits7ops32bit(unsigned char v)
{
  return ((v * 0x0802LU & 0x22110LU) |
	  (v * 0x8020LU & 0x88440LU)) * 0x10101LU >> 16;
}
uint64_t inst_helper_bitrev(uint64_t x);
uint64_t inst_helper_bitrev(uint64_t x) {

#if defined(TARGET_RISCV32)
    x = (((x & 0xaaaaaaaa) >> 1) | ((x & 0x55555555) << 1));
    x = (((x & 0xcccccccc) >> 2) | ((x & 0x33333333) << 2));
    x = (((x & 0xf0f0f0f0) >> 4) | ((x & 0x0f0f0f0f) << 4));
    x = (((x & 0xff00ff00) >> 8) | ((x & 0x00ff00ff) << 8));
    x = ((x >> 16) | (x << 16));
    return x;
#else
    uint64_t result = 0;
    int i;
    for (i = 1; i < 64; i *= 8)
      {
	result |= ReverseBits7ops32bit((uint8_t)x >> i * 8) << (64- (i + 1) * 8);
      }
    return result;
#endif
}
//Signed 32-bit union type for SIMD data.
typedef union
{
  int32_t w0;

  struct
    {
      int16_t h0;
      int16_t h1;
    } b16;

  struct
    {
      int8_t b0;
      int8_t b1;
      int8_t b2;
      int8_t b3;
    } b8;

} union32_t;

//Unsigned 32-bit union type for SIMD data.
typedef union
{
  uint32_t w0;

  struct
    {
      uint16_t h0;
      uint16_t h1;
    } b16;
  struct
    {
      uint8_t b0;
      uint8_t b1;
      uint8_t b2;
      uint8_t b3;
    } b8;

} union32u_t;

//Signed 64-bit union type for SIMD data.
typedef union
{
  int64_t d0;
  struct
    {
      int32_t w0;
      int32_t w1;
    } b32;
  struct
    {
      int16_t h0;
      int16_t h1;
      int16_t h2;
      int16_t h3;
    } b16;
  struct
    {
      int8_t b0;
      int8_t b1;
      int8_t b2;
      int8_t b3;
      int8_t b4;
      int8_t b5;
      int8_t b6;
      int8_t b7;
    } b8;
} union64_t;

//Unsigned 64-bit union type for SIMD data.
typedef union
{
  uint64_t d0;
  struct
    {
      uint32_t w0;
      uint32_t w1;
    } b32;
  struct
    {
      uint16_t h0;
      uint16_t h1;
      uint16_t h2;
      uint16_t h3;
    } b16;
  struct
    {
      uint8_t b0;
      uint8_t b1;
      uint8_t b2;
      uint8_t b3;
      uint8_t b4;
      uint8_t b5;
      uint8_t b6;
      uint8_t b7;
    } b8;
} union64u_t;
#include "andes_dsp_opcode.h"
u32 Clrs(u32 v, int n);
u32 Clrs(u32 v, int n) {

    u32 count = 0;
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
u32 Clz(u32 v, int n);
u32 Clz(u32 v, int n) {

    u32 count = 0;
    int i;
    for (i = n-1; i >= 0; i--) {
	if (((v >> i) & 0x1) == 0x0)
	  count++;
	else
	  return count;
    }
    return count;
}
u16 Clo(u32 v, int n);
u16 Clo(u32 v, int n) {

    u32 count = 0;
    int i;
    for (i = n-1; i >= 0; i--) {
	if (((v >> i) & 0x1) == 0x1)
	  count++;
	else
	  return count;
    }
    return count;
}

#if defined(TARGET_RISCV32)
#define WITH_TARGET_WORD_BITSIZE 32
#else
#define WITH_TARGET_WORD_BITSIZE 64
#endif

#if defined(TARGET_RISCV64)
typedef union {
    uint64_t u;
    int64_t s;

    struct
      {
	int32_t i0;
	int32_t i1;
      } b32;
    struct
      {
	int16_t h0;
	int16_t h1;
	int16_t h2;
	int16_t h3;
      } b16;
    struct
      {
	int8_t b0;
	int8_t b1;
	int8_t b2;
	int8_t b3;
	int8_t b4;
	int8_t b5;
	int8_t b6;
	int8_t b7;
      } b8;
    struct
      {
	uint32_t i0;
	uint32_t i1;
      } ub32;

    struct
      {
	uint16_t h0;
	uint16_t h1;
	uint16_t h2;
	uint16_t h3;
      } ub16;
    struct
      {
	uint8_t b0;
	uint8_t b1;
	uint8_t b2;
	uint8_t b3;
	uint8_t b4;
	uint8_t b5;
	uint8_t b6;
	uint8_t b7;
      } ub8;
} reg_t;
#else
typedef union {
    uint32_t u;
    int32_t s;

    struct
      {
	int32_t i0;
      } b32;
    struct
      {
	int16_t h0;
	int16_t h1;
      } b16;
    struct
      {
	int8_t b0;
	int8_t b1;
	int8_t b2;
	int8_t b3;
      } b8;
    struct
      {
	uint16_t h0;
	uint16_t h1;
      } ub16;
    struct
      {
	uint8_t b0;
	uint8_t b1;
	uint8_t b2;
	uint8_t b3;
      } ub8;
} reg_t;
#endif


void SET_OV(void);
void SET_OV(void)
{
  /*
  // FIXME: pass cpu_env ???
  target_ulong ucode = andes_riscv_csr_read_helper(cpu_env, CSR_UCODE, 0);
  andes_riscv_csr_read_helper(cpu_env, CSR_UCODE, (ucode & 0xfffffffe) | 0x1);
  */
}
static int16_t
insn_sat_khm_helper (int16_t aop, int16_t bop)
{
  int16_t res;
  if (((int16_t) 0x8000 != aop) || ((int16_t) 0x8000 != bop))
    res = (int16_t) (((int32_t) aop * bop) >> 15);
  else
    {
      res = 0x7fff;
      //CCPU_SR_SET (PSW, PSW_OV);
    }
  return res;
}

static int64_t
insn_sat_helper (int64_t res, const short range)
{
  const int64_t max = (1LL << range) - 1;
  const int64_t min = -(1LL << range);

  if (res > max)
    {
      res = max;
      SET_OV();
    }
  else if (res < min)
    {
      res = min;
      SET_OV();
    }

  return res;
}
target_ulong psimd_rrrr(uint32_t op, target_ulong rd, target_ulong rs1, target_ulong rs2);
target_ulong psimd_rrrr(uint32_t op, target_ulong rd, target_ulong rs1, target_ulong rs2) {


    reg_t result;
    result.u = rd;
    reg_t reg_rs1;
    reg_rs1.u = rs1;
    reg_t reg_rs2;
    reg_rs2.u = rs2;
    int32_t i;
#if defined(TARGET_RISCV64)
    int32_t vec32_num = 2;
#else
    int32_t vec32_num = 1;
#endif
    int32_t *ptr32, *ptr_a32, *ptr_b32, *ptr_d32;
    ptr32 = (int32_t *) & result.b32;
    ptr_a32 = (int32_t *) & rs1;
    ptr_b32 = (int32_t *) & rs2;
    ptr_d32 = (int32_t *) & rd;
    int16_t *ptr_a16, *ptr_b16;
    ptr_a16 = (int16_t *) & rs1;
    ptr_b16 = (int16_t *) & rs2;
    switch (op) {
      case kdmatt: {

		       union32_t ra, rb, rt;

		       ra.w0 = rs1;
		       rb.w0 = rs2;
		       rt.w0 = rd;

		       int addop;
		       if ((int16_t)0x8000 == ra.b16.h1 && (int16_t)0x8000 == rb.b16.h1)
			 {
			   addop = 0x7fffffff;
			   SET_OV();
			 }
		       else
			 {
			   addop = ((int32_t)(ra.b16.h1) * rb.b16.h1) << 1;
			 }
		       int64_t res;
		       res = (int64_t)(rt.w0) + addop;
		       res = insn_sat_helper(res, 31);
		       rt.w0 = res;
		   }break;
      case kdmabt: {

		       union32_t ra, rb, rt;

		       ra.w0 = rs1;
		       rb.w0 = rs2;
		       rt.w0 = rd;

		       int addop;
		       if ((int16_t)0x8000 == ra.b16.h0 && (int16_t)0x8000 == rb.b16.h1)
			 {
			   addop = 0x7fffffff;
			   SET_OV();
			 }
		       else
			 {
			   addop = ((int32_t)(ra.b16.h0) * rb.b16.h1) << 1;
			 }
		       int64_t res;
		       res = (int64_t)(rt.w0) + addop;
		       res = insn_sat_helper(res, 31);
		       rt.w0 = res;

		       result.s = sext_xlen(rt.w0);
		   }break;
      case kdmabb: {

		       union32_t ra, rb, rt;

		       ra.w0 = rs1;
		       rb.w0 = rs2;
		       rt.w0 = rd;

		       int addop;
		       if ((int16_t)0x8000 == ra.b16.h0 && (int16_t)0x8000 == rb.b16.h0)
			 {
			   addop = 0x7fffffff;
			   SET_OV();
			 }
		       else
			 {
			   addop = ((int32_t)(ra.b16.h0) * rb.b16.h0) << 1;
			 }
		       int64_t res;
		       res = (int64_t)(rt.w0) + addop;
		       res = insn_sat_helper(res, 31);
		       rt.w0 = res;

		       result.s = sext_xlen(rt.w0);

		   }break;
      case maddr32:
		 result.u += (reg_rs1.u * reg_rs2.u);
		 break;
      case pbsada:
		   {
		     int a, b, c, d;
		     /* The four unsigned 8-bit elements of Ra are subtracted from the four
			unsigned 8-bit elements of Rb.  */
		     a = (reg_rs1.u & 0xff) - (reg_rs2.u & 0xff);
		     b = ((reg_rs1.u >> 8) & 0xff) - ((reg_rs2.u >> 8) & 0xff);
		     c = ((reg_rs1.u >> 16) & 0xff) - ((reg_rs2.u >> 16) & 0xff);
		     d = ((reg_rs1.u >> 24) & 0xff) - ((reg_rs2.u >> 24) & 0xff);

		     /* Absolute difference of four unsigned 8-bit data elements.  */
		     a = (a >= 0) ? a : -a;
		     b = (b >= 0) ? b : -b;
		     c = (c >= 0) ? c : -c;
		     d = (d >= 0) ? d : -d;

		     /* pbsada */
		     result.u = result.u + a + b + c + d;

		     break;
		   }
      case kmsxda:
		   {
		     int64_t res;
		     for (i = 0; i <= vec32_num; i+=2)
		       {
			 res = (int64_t) *(ptr_d32 + (i / 2))
			   - ((int32_t) *(ptr_a16 + (i + 1)) * *(ptr_b16 + i))
			   - ((int32_t) *(ptr_a16 + i) * *(ptr_b16 + (i + 1)));
			 *(ptr32 + (i / 2)) = insn_sat_helper (res, 31);
		       }
		     result.s = result.s;
		   }
		 break;
      case kmsda:
		   {
		     int64_t res;
		     for (i = 0; i <= vec32_num; i+=2)
		       {
			 res = (int64_t) *(ptr_d32 + (i / 2))
			   - ((int32_t) *(ptr_a16 + (i + 1)) * *(ptr_b16 + (i + 1)))
			   - ((int32_t) *(ptr_a16 + i) * *(ptr_b16 + i));
			 *(ptr32 + (i / 2)) = insn_sat_helper (res, 31);
		       }
		     result.s = result.s;
		   }
		 break;
      case kmaxds:
		   {
		     int64_t res;
		     for (i = 0; i <= vec32_num; i+=2)
		       {
			 res = (int64_t) *(ptr_d32 + (i / 2))
			   + ((int32_t) *(ptr_a16 + (i + 1)) * *(ptr_b16 + i))
			   - ((int32_t) *(ptr_a16 + i) * *(ptr_b16 + (i + 1)));
			 *(ptr32 + (i / 2)) = insn_sat_helper (res, 31);
		       }
		     result.s = result.s;
		   }
		 break;
      case kmadrs:
		   {
		     int64_t res;
		     for (i = 0; i <= vec32_num; i+=2)
		       {
			 res = (int64_t) *(ptr_d32 + (i / 2))
			   - ((int32_t) *(ptr_a16 + (i + 1)) * *(ptr_b16 + (i + 1)))
			   + ((int32_t) *(ptr_a16 + i) * *(ptr_b16 + i));
			 *(ptr32 + (i / 2)) = insn_sat_helper (res, 31);
		       }
		     result.s = result.s;
		   }
		 break;
      case kmads:
		   {
		     int64_t res;
		     for (i = 0; i <= vec32_num; i+=2)
		       {
			 res = (int64_t) *(ptr_d32 + (i / 2))
			   + ((int32_t) *(ptr_a16 + (i + 1)) * *(ptr_b16 + (i + 1)))
			   - ((int32_t) *(ptr_a16 + i) * *(ptr_b16 + i));
			 *(ptr32 + (i / 2)) = insn_sat_helper (res, 31);
		       }
		     result.s = result.s;
		   }
		 break;
      case kmaxda:
		   {
		     int64_t res;
		     for (i = 0; i <= vec32_num; i+=2)
		       {
			 res = (int64_t) *(ptr_d32 + (i / 2))
			   + ((int32_t) *(ptr_a16 + (i + 1)) * *(ptr_b16 + i))
			   + ((int32_t) *(ptr_a16 + i) * *(ptr_b16 + (i + 1)));
			 *(ptr32 + (i / 2)) = insn_sat_helper (res, 31);
		       }
		     result.s = result.s;
		   }
		 break;
      case kmada:
		   {
		     int64_t res;
		     for (i = 0; i <= vec32_num; i+=2)
		       {
			 res = (int64_t) *(ptr_d32 + (i / 2))
			   + ((int32_t) *(ptr_a16 + (i + 1)) * *(ptr_b16 + (i + 1)))
			   + ((int32_t) *(ptr_a16 + i) * *(ptr_b16 + i));
			 *(ptr32 + (i / 2)) = insn_sat_helper (res, 31);
		       }
		     result.s = result.s;
		   }
		 break;
      case kmatt:
		   {
		     int32_t mul;
		     int64_t res;
		     for (i = 0; i <= vec32_num; i+=2)
		       {
			 mul = (int32_t) *(ptr_a16 + (i + 1)) * *(ptr_b16 + (i + 1));
			 res = (int64_t) *(ptr_d32 + (i / 2)) + mul;
			 *(ptr32 + (i / 2)) = insn_sat_helper (res, 31);
		       }
		     result.s = result.s;
		   }
		 break;
      case kmabt:
		   {
		     int32_t mul;
		     int64_t res;
		     for (i = 0; i <= vec32_num; i+=2)
		       {
			 mul = (int32_t) *(ptr_a16 + i) * *(ptr_b16 + (i + 1));
			 res = (int64_t) *(ptr_d32 + (i / 2)) + mul;
			 *(ptr32 + (i / 2)) = insn_sat_helper (res, 31);
		       }
		     result.s = result.s;
		   }
		 break;
      case kmabb:
		   {
		     int32_t mul;
		     int64_t res;
		     for (i = 0; i <= vec32_num; i+=2)
		       {
			 mul = (int32_t) *(ptr_a16 + i) * *(ptr_b16 + i);
			 res = (int64_t) *(ptr_d32 + (i / 2)) + mul;
			 *(ptr32 + (i / 2)) = insn_sat_helper (res, 31);
		       }
		     result.s = result.s;
		   }
		 break;
      case kmmawt2_u:
		   {
		     union64_t temp;
		     int64_t res;
		     int32_t rnd_val;
		     int b16_offset = 1;
		     int addop;
		     for (i = 0; i < vec32_num; i++)
		       {
			 if ((0x80000000 == *(ptr_a32 + i)) &&
			     ((int16_t)0x8000 == *(ptr_b16 + b16_offset))) {
			     addop = 0x7fffffff;
			 } else {
			     temp.d0 = (int64_t) * (ptr_a32 + i) * *(ptr_b16 + b16_offset);
			     rnd_val = (temp.b32.w0 & (1UL << 14)) ? (1L << 15) : 0;
			     temp.d0 += rnd_val;
			     addop = (temp.d0 >> 15);
			 }
			 res = (int64_t) *(ptr_d32 + i) + addop;
			 *(ptr32 + i) = insn_sat_helper (res, 31);
			 b16_offset += 2;
		       }
		   }
		 break;
      case kmmawt2:
		   {
		     union64_t temp;
		     int64_t res;
		     int b16_offset = 1;
		     for (i = 0; i < vec32_num; i++)
		       {
			 temp.d0 = (int64_t) *(ptr_a32 + i) * *(ptr_b16 + b16_offset);
			 res = (int64_t) *(ptr_d32 + i) + (int32_t) (temp.d0 >> 15);
			 *(ptr32 + i) = insn_sat_helper (res, 31);
			 b16_offset += 2;
		       }
		     result.s = result.s;
		   }
		 break;
      case kmmawb2_u:
		   {
		     union64_t temp;
		     int64_t res;
		     int32_t rnd_val;
		     int b16_offset = 0;
		     for (i = 0; i < vec32_num; i++)
		       {
			 temp.d0 = (int64_t) *(ptr_a32 + i) * *(ptr_b16 + b16_offset);
			 rnd_val = (temp.b32.w0 & (1UL << 14)) ? (1L << 15) : 0;
			 temp.d0 += rnd_val;
			 res = (int64_t) *(ptr_d32 + i) + (int32_t) (temp.d0 >> 15);
			 *(ptr32 + i) = insn_sat_helper (res, 31);
		       }
		     result.s = result.s;
		   }
		 break;
      case kmmawb2:
		   {
		     union64_t temp;
		     int64_t res;
		     int b16_offset = 0;
		     for (i = 0; i < vec32_num; i++)
		       {
			 temp.d0 = (int64_t) *(ptr_a32 + i) * *(ptr_b16 + b16_offset);
			 res = (int64_t) *(ptr_d32 + i) + (int32_t) (temp.d0 >> 15);
			 *(ptr32 + i) = insn_sat_helper (res, 31);
			 b16_offset += 2;
		       }
		     result.s = result.s;
		   }
		 break;
      case kmmawt_u:
		   {
		     union64_t temp;
		     int64_t res;
		     int32_t rnd_val;
		     int b16_offset = 1;
		     for (i = 0; i < vec32_num; i++)
		       {
			 temp.d0 = (int64_t) *(ptr_a32 + i) * *(ptr_b16 + b16_offset);
			 rnd_val = (temp.b32.w0 & (1UL << 15)) ? (1L << 16) : 0;
			 temp.d0 += rnd_val;
			 res = (int64_t) *(ptr_d32 + i) + (int32_t) (temp.d0 >> 16);
			 *(ptr32 + i) = insn_sat_helper (res, 31);
		       }
		     result.s = result.s;
		   }
		 break;
      case kmmawt:
		   {
		     union64_t temp;
		     int64_t res;
		     int b16_offset = 1;
		     for (i = 0; i < vec32_num; i++)
		       {
			 temp.d0 = (int64_t) *(ptr_a32 + i) * *(ptr_b16 + b16_offset);
			 res = (int64_t) *(ptr_d32 + i) + (int32_t) (temp.d0 >> 16);
			 *(ptr32 + i) = insn_sat_helper (res, 31);
			 b16_offset += 2;
		       }
		     result.s = result.s;
		   }
		 break;
      case kmmawb_u:
		   {
		     union64_t temp;
		     int64_t res;
		     int32_t rnd_val;
		     int b16_offset = 0;
		     for (i = 0; i < vec32_num; i++)
		       {
			 temp.d0 = (int64_t) *(ptr_a32 + i) * *(ptr_b16 + b16_offset);
			 rnd_val = (temp.b32.w0 & (1UL << 15)) ? (1L << 16) : 0;
			 temp.d0 += rnd_val;
			 res = (int64_t) *(ptr_d32 + i) + (int32_t) (temp.d0 >> 16);
			 *(ptr32 + i) = insn_sat_helper (res, 31);
		       }
		     result.s = result.s;
		   }
		 break;
      case kmmawb:
		   {
		     union64_t temp;
		     int64_t res;
		     int b16_offset = 0;
		     for (i = 0; i < vec32_num; i++)
		       {
			 temp.d0 = (int64_t) *(ptr_a32 + i) * *(ptr_b16 + b16_offset);
			 res = (int64_t) *(ptr_d32 + i) + (int32_t) (temp.d0 >> 16);
			 *(ptr32 + i) = insn_sat_helper (res, 31);
			 b16_offset += 2;
		       }
		     result.s = result.s;
		   }
		 break;
      case kmmac_u:
		   {
		     union64_t temp, res;

		     for (i = 0; i < vec32_num; i++)
		       {
			 temp.d0 = (int64_t) *(ptr_a32 + i) * *(ptr_b32 + i);
			 if ((temp.b32.w0 >> 31) != 0)
			   temp.b32.w1 += 1;

			 res.d0 = (int64_t) result.s + temp.b32.w1;
			 res.d0 = insn_sat_helper (res.d0, 31);
			 *(ptr32 + i) = res.d0;
		       }
		     result.s = result.s;
		   }
		 break;
      case kmmac:
		   {
		     union64_t temp, res;

		     for (i = 0; i < vec32_num; i++)
		       {
			 temp.d0 = (int64_t) *(ptr_a32 + i) * *(ptr_b32 + i);
			 res.d0 = (int64_t) result.s + temp.b32.w1;
			 res.d0 = insn_sat_helper (res.d0, 31);
			 *(ptr32 + i) = res.d0;
		       }
		     result.s = result.s;
		   }
		 break;
      case kmmsb:
		   {
		     union64_t temp, res;

		     for (i = 0; i < vec32_num; i++)
		       {
			 temp.d0 = (int64_t) *(ptr_a32 + i) * *(ptr_b32 + i);
			 res.d0 = (int64_t) result.s - temp.b32.w1;
			 res.d0 = insn_sat_helper (res.d0, 31);
			 *(ptr32 + i) = res.d0;
		       }
		     result.s = result.s;
		   }
		 break;
      case kmmsb_u:
		   {
		     union64_t temp, res;

		     for (i = 0; i < vec32_num; i++)
		       {
			 temp.d0 = (int64_t) *(ptr_a32 + i) * *(ptr_b32 + i);
			 if ((temp.b32.w0 >> 31) != 0)
			   temp.b32.w1 += 1;

			 res.d0 = (int64_t) result.s - temp.b32.w1;
			 res.d0 = insn_sat_helper (res.d0, 31);
			 *(ptr32 + i) = res.d0;
		       }
		     result.s = result.s;
		   }
		 break;

      default: {
		   exit(1);
		   break;
	       }
    }
    return result.u;

}
target_ulong psimd_rr(uint32_t op, target_ulong rs1);
target_ulong psimd_rr(uint32_t op, target_ulong rs1) {


    reg_t result;
    uint32_t *uptr_a32;
    uptr_a32 = (uint32_t *) & rs1;
    switch (op) {
      case clrs32: {
#if defined(TARGET_RISCV32)
		       result.u = Clrs(*uptr_a32, 32);
#else
		       result.ub32.i0 = Clrs(*uptr_a32, 32);
		       result.ub32.i1 = Clrs(*(uptr_a32+1), 32);
#endif
		   }
		 break;
      case Dclo32: {
#if defined(TARGET_RISCV32)
		       result.u = Clo(*uptr_a32, 32);
#else
		       result.ub32.i0 = Clo(*uptr_a32, 32);
		       result.ub32.i1 = Clo(*(uptr_a32+1), 32);
#endif
		   }
		 break;
      case Dclz32: {
#if defined(TARGET_RISCV32)
		       result.u = Clz(*uptr_a32, 32);
#else
		       result.ub32.i0 = Clz(*uptr_a32, 32);
		       result.ub32.i1 = Clz(*(uptr_a32+1), 32);
#endif
		   }
		 break;


      default:
		 exit(1);
		 break;
    }
    return result.u;
}
target_ulong psimd_rr_uimm(uint32_t op, target_ulong rs1, target_ulong uimm);
target_ulong psimd_rr_uimm(uint32_t op, target_ulong rs1, target_ulong uimm) {


    reg_t result;
    reg_t reg_rs1;
    reg_rs1.u = rs1;
    int32_t res, i;
#if defined(TARGET_RISCV64)
    int32_t vec32_num = 2;
#else
    int32_t vec32_num = 1;
#endif
    int32_t *ptr32, *ptr_a32;
    ptr32 = (int32_t *) & result.b32;
    ptr_a32 = (int32_t *) & rs1;
    uint32_t *uptr32, *uptr_a32;
    uptr32 = (uint32_t *) & result.b32;
    uptr_a32 = (uint32_t *) & rs1;
#define imm5u uimm
#define imm6u uimm

    switch (op) {
      case bitrevi:
	  {
#if defined(TARGET_RISCV32)
	    uint32_t bits = reg_rs1.u;
	    bits = (((bits & 0xaaaaaaaa) >> 1) | ((bits & 0x55555555) << 1));
	    bits = (((bits & 0xcccccccc) >> 2) | ((bits & 0x33333333) << 2));
	    bits = (((bits & 0xf0f0f0f0) >> 4) | ((bits & 0x0f0f0f0f) << 4));
	    bits = (((bits & 0xff00ff00) >> 8) | ((bits & 0x00ff00ff) << 8));
	    bits = ((bits >> 16) | (bits << 16));
	    result.u = bits >> (32 - (imm5u + 1));
#else
	    uint64_t bits = reg_rs1.u;
	    uint64_t r = 0;
	    int i;
	    for (i = 63; i >= 0; i--)
	      r |= ((bits >> i) & 0x1L) << (63 - i);

	    result.u = bits >> (64 - (imm6u + 1));
#endif
	  }
	break;
      case slli32:
	  {
	    for (i = 0; i < vec32_num; i++)
	      {
		res = *(ptr_a32 + i) << imm5u;
		*(ptr32 + i) = res;
	      }
	    result.s = result.s;
	  }
	break;
      case srli32:
	  {
	    for (i = 0; i < vec32_num; i++)
	      {
		res = *(uptr_a32 + i) >> imm5u;
		*(uptr32 + i) = res;
	      }
	    result.u = result.u;
	  }
	break;
      case srai32:
	  {
	    for (i = 0; i < vec32_num; i++)
	      {
		res = *(ptr_a32 + i) >> imm5u;
		*(ptr32 + i) = res;
	      }
	    result.s = result.s;
	  }
	break;
      case kslliw:
	  {

	    union32u_t ra, rt;

	    ra.w0 = rs1;

	    uint32_t sh = imm5u;
	    int64_t res;

	    res = (int64_t)((int32_t)(ra.w0)) << sh;
	    res = insn_sat_helper(res, 31);
	    rt.w0 = res;
	    result.s = sext_xlen(rt.w0);


	  }break;
      case uclip32:
	  {
	    int32_t val;
	    for (i = 0; i < vec32_num; i++)
	      {
		val = *(ptr_a32 + i);
		if (val > ((1 << imm5u) - 1))
		  {
		    *(ptr32 + i) = ((1 << imm5u) - 1);
		    SET_OV();
		  }
		else if (val < 0)
		  {
		    *(ptr32 + i) = 0;
		    SET_OV();
		  }
		else
		  *(ptr32 + i) = val;
	      }
	    result.s = result.s;
	  }
	break;
      case sclip32:
	  {
	    int32_t val;
	    for (i = 0; i < vec32_num; i++)
	      {
		val = *(ptr_a32 + i);
		if (val > ((1 << uimm) - 1))
		  {
		    *(ptr32 + i) = ((1 << uimm) - 1);
		    SET_OV();
		  }

		else if (val < -(1 << uimm))
		  {
		    *(ptr32 + i) = -(1 << uimm);
		    SET_OV();
		  }
		else
		  *(ptr32 + i) = val;
	      }
	    result.s = result.s;
	  }
	break;
      default:
	exit(1);
	break;
    }
    return result.u;
}
uint64_t psimd_rrr64(uint32_t op, uint64_t rs1, uint64_t rs2);
uint64_t psimd_rrr64(uint32_t op, uint64_t rs1, uint64_t rs2) {

    switch(op) {
      case smal:
	  {

	    union64_t t, a;
	    union64_t b;

	    a.d0 = rs1;
	    b.d0 = rs2;

	    t.d0 = a.d0 + (int64_t)(b.b16.h1) * b.b16.h0;
#if (WITH_TARGET_WORD_BITSIZE == 64)
	    t.d0 += (int64_t)(b.b16.h3) * b.b16.h2;
#endif
	    return t.d0;

	  }

	break;
      case sub64: {
		      union64_t a, b, t;

		      a.d0 = rs1;
		      b.d0 = rs2;


		      t.d0 = a.d0 - b.d0;
		      return t.d0;
		  }
		break;
      case add64: {
		      union64_t a, b, t;

		      a.d0 = rs1;
		      b.d0 = rs2;


		      t.d0 = a.d0 + b.d0;
		      return t.d0;

		  }
		break;
      case radd64:
		  {
		    union64_t a, b, t;
		    int32_t lsb_eq_1 = 1;

		    a.d0 = rs1;
		    b.d0 = rs2;

		    lsb_eq_1 = lsb_eq_1 & a.d0 & b.d0;
		    t.d0 = (a.d0 >> 1) + (b.d0 >> 1) + lsb_eq_1;
		    return t.d0;
		  }
		break;
      case rsub64:
		  {
		    union64_t a, b, t;

		    a.d0 = rs1;
		    b.d0 = rs2;

		    int32_t high_a = a.d0 < 0 ? 0xffffffff : 0x0;
		    int32_t high_b = b.d0 < 0 ? 0xffffffff : 0x0;
		    int64_t high = high_a - high_b;
		    uint64_t low = ((uint64_t)(a.d0) - b.d0);
		    if (low > (uint64_t)(a.d0))
		      high --;
		    t.d0 = (low >> 1) | (high & 0x1 ? 0x8000000000000000 : 0x0);
		    return t.d0;
		  }
		break;
      case ursub64: {

			union64u_t a, b, t;


			a.d0 = rs1;
			b.d0 = rs2;

			uint32_t high = 0;
			uint64_t low = (a.d0 - b.d0);
			if (low > a.d0)
			  high --;
			t.d0 = (low >> 1) | (high & 0x1 ? 0x8000000000000000 : 0x0);
			return t.d0;

		    }
		  break;
      case ksub64: {

		       union64_t a, b, t;
		       a.d0 = rs1;
		       b.d0 = rs2;

		       t.d0 = a.d0 - b.d0;
		       if ((a.d0 > 0) && (b.d0 < 0))
			 {
			   if (t.d0 <= 0)
			     {
			       SET_OV();
			       t.d0 = 0x7FFFFFFFFFFFFFFFLL;
			     }
			 }
		       else if ((a.d0 < 0) && (b.d0 > 0))
			 {
			   if (t.d0 >= 0)
			     {
			       SET_OV();
			       t.d0 = 0x8000000000000000LL;
			     }
			 }
		       return t.d0;
		   }
		 break;
      case uksub64: {

			union64u_t a, b, t;


			a.d0 = rs1;
			b.d0 = rs2;

			t.d0 = a.d0 - b.d0;
			if (a.d0 < b.d0)
			  {
			    SET_OV();
			    t.d0 = 0LL;
			  }
			return t.d0;
		    }
		  break;
      case uradd64:
		    {
		      union64u_t a, b, t;
		      int32_t lsb_eq_1 = 1;

		      a.d0 = rs1;
		      b.d0 = rs2;

		      lsb_eq_1 = lsb_eq_1 & a.d0 & b.d0;
		      t.d0 = (a.d0 >> 1) + (b.d0 >> 1) + lsb_eq_1;
		      return t.d0;
		    }
		  break;
      case kadd64:
		    {
		      union64_t a, b, t;

		      a.d0 = rs1;
		      b.d0 = rs2;

		      t.d0 = a.d0 + b.d0;
		      if ((a.d0 > 0) && (b.d0 > 0))
			{
			  if (t.d0 <= 0)
			    {
			      SET_OV();
			      t.d0 = 0x7FFFFFFFFFFFFFFFLL;
			    }
			}
		      else if ((a.d0 < 0) && (b.d0 < 0))
			{
			  if (t.d0 >= 0)
			    {
			      SET_OV();
			      t.d0 = 0x8000000000000000LL;
			    }
			}
		      return t.d0;
		    }
      case ukadd64:
		    {

		      union64u_t a, b, t;


		      a.d0 = rs1;
		      b.d0 = rs2;

		      t.d0 = a.d0 + b.d0;
		      if (t.d0 < a.d0)
			{
			  SET_OV();
			  t.d0 = 0xFFFFFFFFFFFFFFFFLL ;
			}
		      return t.d0;
		    }
		  break;
      default:
		  exit(1);
		  break;
    }
    exit(1);
    return 0;
}
target_ulong psimd_rrr(uint32_t op, target_ulong rs1, target_ulong rs2);
target_ulong psimd_rrr(uint32_t op, target_ulong rs1, target_ulong rs2) {


    reg_t result;
    reg_t reg_rs1;
    reg_rs1.u = rs1;
    reg_t reg_rs2;
    reg_rs2.u = rs2;
    int32_t i;
#if defined(TARGET_RISCV64)
    int32_t res;
#endif
#if defined(TARGET_RISCV64)
    int32_t vec32_num = 2;
#else
    int32_t vec32_num = 1;
#endif
    int32_t *ptr32, *ptr_a32, *ptr_b32;
    ptr32 = (int32_t *) & result.b32;
    ptr_a32 = (int32_t *) & rs1;
    ptr_b32 = (int32_t *) & rs2;


#if defined(TARGET_RISCV64)
    uint32_t *uptr32, *uptr_a32, *uptr_b32;
    uptr32 = (uint32_t *) & result.b32;
    uptr_a32 = (uint32_t *) & rs1;
    uptr_b32 = (uint32_t *) & rs2;
#endif
    int16_t *ptr_a16, *ptr_b16;
    ptr_a16 = (int16_t *) & rs1;
    ptr_b16 = (int16_t *) & rs2;
    switch (op) {

#if (WITH_TARGET_WORD_BITSIZE == 64)
      case add32:
	  {
	    for (i = 0; i < vec32_num; i++)
	      {
		res = *(ptr_a32 + i) + *(ptr_b32 + i);
		*(ptr32 + i) = res;
	      }
	    result.s = result.s;
	  }
	break;
      case radd32:
	  {
	    for (i = 0; i < vec32_num; i++)
	      {
		res = (int32_t) (((int64_t) *(ptr_a32 + i) + *(ptr_b32 + i)) >> 1);
		*(ptr32 + i) = res;
	      }
	    result.s = result.s;
	  }
	break;
      case uradd32:
	  {
	    for (i = 0; i < vec32_num; i++)
	      {
		res =
		  (uint32_t) (((uint64_t) *(uptr_a32 + i) + *(uptr_b32 + i)) >> 1);
		*(uptr32 + i) = res;
	      }
	    result.u = result.u;
	  }
	break;
      case sub32:
	  {
	    for (i = 0; i < vec32_num; i++)
	      {
		res = *(ptr_a32 + i) - *(ptr_b32 + i);
		*(ptr32 + i) = res;
	      }
	    result.s = result.s;
	  }
	break;
      case rsub32:
	  {
	    for (i = 0; i < vec32_num; i++)
	      {
		res = (int32_t) (((int64_t) *(ptr_a32 + i) - *(ptr_b32 + i)) >> 1);
		*(ptr32 + i) = res;
	      }
	    result.s = result.s;
	  }
	break;
      case ursub32:
	  {
	    for (i = 0; i < vec32_num; i++)
	      {
		res =
		  (uint32_t) (((uint64_t) *(uptr_a32 + i) - *(uptr_b32 + i)) >> 1);
		*(uptr32 + i) = res;
	      }
	    result.u = result.u;
	  }
	break;
      case urcras32:
	  {
	    result.ub32.i0 = (uint32_t) (((uint64_t) reg_rs1.ub32.i1
					  + reg_rs2.ub32.i0) >> 1);
	    result.ub32.i1 = (uint32_t) (((uint64_t) reg_rs1.ub32.i0
					  - reg_rs2.ub32.i1) >> 1);
	    result.u = result.u;
	  }
	break;
      case urstas32:
	  {
	    result.ub32.i0 = (uint32_t) (((uint64_t) reg_rs1.ub32.i0
					  + reg_rs2.ub32.i0) >> 1);
	    result.ub32.i1 = (uint32_t) (((uint64_t) reg_rs1.ub32.i1
					  - reg_rs2.ub32.i1) >> 1);
	    result.u = result.u;
	  }
	break;
      case urcrsa32:
	  {
	    result.ub32.i0 = (uint32_t) (((uint64_t) reg_rs1.ub32.i0
					  - reg_rs2.ub32.i0) >> 1);
	    result.ub32.i1 = (uint32_t) (((uint64_t) reg_rs1.ub32.i0
					  + reg_rs2.ub32.i1) >> 1);
	    result.u = result.u;
	  }
	break;
      case urstsa32:
	  {
	    result.ub32.i0 = (uint32_t) (((uint64_t) reg_rs1.ub32.i0
					  - reg_rs2.ub32.i0) >> 1);
	    result.ub32.i1 = (uint32_t) (((uint64_t) reg_rs1.ub32.i1
					  + reg_rs2.ub32.i1) >> 1);
	    result.u = result.u;
	  }
	break;
      case rcras32:
	  {
	    result.b32.i0 = (int32_t) (((int64_t) reg_rs1.b32.i1
					+ reg_rs2.b32.i0) >> 1);
	    result.b32.i1 = (int32_t) (((int64_t) reg_rs1.b32.i0
					- reg_rs2.b32.i1) >> 1);
	    result.s = result.s;
	  }
	break;
      case rstas32:
	  {
	    result.b32.i0 = (int32_t) (((int64_t) reg_rs1.b32.i0
					+ reg_rs2.b32.i0) >> 1);
	    result.b32.i1 = (int32_t) (((int64_t) reg_rs1.b32.i1
					- reg_rs2.b32.i1) >> 1);
	    result.s = result.s;
	  }
	break;
      case rcrsa32:
	  {
	    result.b32.i0 = (int32_t) (((int64_t) reg_rs1.b32.i1
					- reg_rs2.b32.i0) >> 1);
	    result.b32.i1 = (int32_t) (((int64_t) reg_rs1.b32.i0
					+ reg_rs2.b32.i1) >> 1);
	    result.s = result.s;
	  }
	break;
      case rstsa32:
	  {
	    result.b32.i0 = (int32_t) (((int64_t) reg_rs1.b32.i0
					- reg_rs2.b32.i0) >> 1);
	    result.b32.i1 = (int32_t) (((int64_t) reg_rs1.b32.i1
					+ reg_rs2.b32.i1) >> 1);
	    result.s = result.s;
	  }
	break;
      case cras32:
	  {
	    result.b32.i0 = reg_rs1.b32.i1 + reg_rs2.b32.i0;
	    result.b32.i1 = reg_rs1.b32.i0 - reg_rs2.b32.i1;
	    result.s = result.s;
	  }
	break;
      case stas32:
	  {
	    result.b32.i0 = reg_rs1.b32.i0 + reg_rs2.b32.i0;
	    result.b32.i1 = reg_rs1.b32.i1 - reg_rs2.b32.i1;
	    result.s = result.s;
	  }
	break;
      case crsa32:
	  {
	    result.b32.i0 = reg_rs1.b32.i1 - reg_rs2.b32.i0;
	    result.b32.i1 = reg_rs1.b32.i0 + reg_rs2.b32.i1;
	    result.s = result.s;
	  }
	break;
      case stsa32:
	  {
	    result.b32.i0 = reg_rs1.b32.i0 - reg_rs2.b32.i0;
	    result.b32.i1 = reg_rs1.b32.i1 + reg_rs2.b32.i1;
	    result.s = result.s;
	  }
	break;
      case sll32:
	  {
	    for (i = 0; i < vec32_num; i++)
	      {
		res = *(ptr_a32 + i) << (reg_rs2.u & 0x1f);
		*(ptr32 + i) = res;
	      }
	    result.s = result.s;
	  }
	break;
      case srl32:
	  {
	    for (i = 0; i < vec32_num; i++)
	      {
		res = *(uptr_a32 + i) >> (reg_rs2.u & 0x1f);
		*(uptr32 + i) = res;
	      }
	    result.u = result.u;
	  }
	break;
      case sra32:
	  {
	    for (i = 0; i < vec32_num; i++)
	      {
		res = *(ptr_a32 + i) >> (reg_rs2.u & 0x1f);
		*(ptr32 + i) = res;
	      }
	    result.s = result.s;
	  }
	break;
      case umax32:
	  {
	    for (i = 0; i < vec32_num; i++)
	      {
		if (*(uptr_a32 + i) > *(uptr_b32 + i))
		  *(uptr32 + i) = *(uptr_a32 + i);
		else
		  *(uptr32 + i) = *(uptr_b32 + i);
	      }
	    result.u = result.u;
	  }
	break;
      case smax32:
	  {
	    for (i = 0; i < vec32_num; i++)
	      {
		if (*(ptr_a32 + i) > *(ptr_b32 + i))
		  *(ptr32 + i) = *(ptr_a32 + i);
		else
		  *(ptr32 + i) = *(ptr_b32 + i);
	      }
	    result.s = result.s;
	  }
	break;
      case umin32:
	  {
	    for (i = 0; i < vec32_num; i++)
	      {
		if (*(uptr_a32 + i) < *(uptr_b32 + i))
		  *(uptr32 + i) = *(uptr_a32 + i);
		else
		  *(uptr32 + i) = *(uptr_b32 + i);
	      }
	    result.u = result.u;
	  }
	break;
      case smin32:
	  {
	    for (i = 0; i < vec32_num; i++)
	      {
		if (*(ptr_a32 + i) < *(ptr_b32 + i))
		  *(ptr32 + i) = *(ptr_a32 + i);
		else
		  *(ptr32 + i) = *(ptr_b32 + i);
	      }
	    result.s = result.s;
	  }
	break;
      case smtt32:
	  {
	    result.s = reg_rs1.b32.i1 * reg_rs2.b32.i1;
	  }
	break;
      case smbt32:
	  {
	    result.s = reg_rs1.b32.i0 * reg_rs2.b32.i1;
	  }
	break;
      case smbb32:
	  {
	    result.s = reg_rs1.b32.i0 * reg_rs2.b32.i0;
	  }
	break;
      case smds32:
	  {
	    result.s
	      = ((int64_t) reg_rs1.b32.i1 * reg_rs2.b32.i1)
	      - ((int64_t) reg_rs1.b32.i0 * reg_rs2.b32.i0);
	  }
	break;
      case smdrs32:
	  {
	    result.s
	      = ((int64_t) reg_rs1.b32.i0 * reg_rs2.b32.i0)
	      - ((int64_t) reg_rs1.b32.i1 * reg_rs2.b32.i1);
	  }
	break;
      case smxds32:
	  {
	    result.s
	      = ((int64_t) reg_rs1.b32.i1 * reg_rs2.b32.i0)
	      - ((int64_t) reg_rs1.b32.i0 * reg_rs2.b32.i1);
	  }
	break;
#endif
      case bitrev:
	  {
	    union64u_t ra, rb, rt;

	    ra.d0 = rs1;
	    rb.d0 = rs2;

#if (WITH_TARGET_WORD_BITSIZE == 32)
	    uint32_t width = rb.d0 & 0x1f;
#else
	    uint32_t width = rb.d0 & 0x3f;
#endif

	    ra.d0 = inst_helper_bitrev(ra.d0);
#if (WITH_TARGET_WORD_BITSIZE == 32)
	    rt.d0 = ra.d0 >> (32- 1 - width);
#else
	    rt.d0 = ra.d0 >> (64- 1 - width);
#endif
	    return  zext_xlen(rt.d0);
	  }
	break;
      case ave:
	  {
	    int64_t r = ((int64_t) reg_rs1.s)
	      + ((int64_t) reg_rs2.s) + 1;
	    result.u = (r >> 1);
	  }
	break;
      case kslraw: {

		       union32_t ra, rb, rt;

		       ra.w0 = rs1;
		       rb.w0 = rs2;

		       uint32_t sh = rb.w0 & 0x3f;

		       if ((rb.w0 & 0x10) == 0x10)
			 {
			   int new_sh = -(int8_t)(sh);

			   //if (current_cpu->OT.conf_isa_dsp.get())
			   new_sh = new_sh > 31 ? 31 : new_sh;

			   int temp = (int)(ra.w0) >> new_sh;

			   //if the c shift count >= 32 is a undefined behavior
			   //ia-32 processor do mask the shift count to 5 bits
			   //so we do the special handling here
			   if (new_sh >= 32)
			     {
			       if (ra.w0 >= 0)
				 temp = 0;
			       else
				 temp = -1;
			     }

			   rt.w0 = temp;
			   result.s = sext_xlen(rt.w0);
			 }
		       else
			 {
			   int64_t res = 0;
			   int new_sh = sh;

			   //if (current_cpu->OT.conf_isa_dsp.get())
			   new_sh = sh > 31 ? 31 : sh;

			   if (new_sh != 0)
			     {
			       if (new_sh <= 32)
				 {
				   res = (int64_t)((int32_t)(ra.w0)) << new_sh;
				   res = insn_sat_helper(res, 31);
				 }
			       else if (new_sh > 32)
				 {
				   //if the shift count > 32 the sign bit will be shifted out.
				   //so we do the special handling here
				   if (ra.w0 < 0)
				     {
				       res = INT_MIN;
				       SET_OV();
				     }
				   else if (ra.w0 > 0)
				     {
				       res = INT_MAX;
				       SET_OV();
				     }
				 }

			       rt.w0 = res;
			       result.s = sext_xlen(rt.w0);
			     }
			   else
			     result.s = sext_xlen(ra.w0);
			 }
		   }break;
      case ksllw: {

		      union32u_t ra, rb, rt;

		      ra.w0 = rs1;
		      rb.w0 = rs2;

		      uint32_t sh = rb.w0 & 0x1f;
		      int64_t res;

		      res = (int64_t)((int32_t)(ra.w0)) << sh;
		      res = insn_sat_helper(res, 31);
		      rt.w0 = res;
		      result.s = sext_xlen(rt.w0);
		  }break;
      case raddw:
		  {
		    int32_t val1 = reg_rs1.s;
		    int32_t val2 = reg_rs2.s;
		    result.s = (int32_t) (((int64_t) val1 + (int64_t) val2) >> 1);
		  }
		break;
      case uraddw:
		  {
		    uint32_t val1 = reg_rs1.u;
		    uint32_t val2 = reg_rs2.u;
		    result.u = (uint32_t) (((uint64_t) val1 + val2) >> 1);
		    return result.u;
		  }
		break;
      case rsubw:
		  {
		    int32_t val1 = reg_rs1.s;
		    int32_t val2 = reg_rs2.s;
		    result.s = (int32_t) (((int64_t) val1 - (int64_t) val2) >> 1);
		  }
		break;
      case ursubw:
		  {
		    uint32_t val1 = reg_rs1.u;
		    uint32_t val2 = reg_rs2.u;
		    result.u = (uint32_t) (((uint64_t) val1 - val2) >> 1);
		    return result.u;
		  }
		break;
      case kaddw:
		  {

		    long long sra, srb, tmp;

		    sra = sext_xlen(rs1);
		    srb = sext_xlen(rs2);

		    tmp = sra + srb;

		    if ((long long) 0x000000007fffffffLL < tmp)
		      {
#if defined(TARGET_RISCV32)
			result.s = (int32_t)0x7fffffff;
#else
			result.s = (int64_t)0x7fffffff;
#endif
			SET_OV();
		      }
		    else if ((long long) 0xFFFFFFFF80000000LL > tmp)
		      {
#if defined(TARGET_RISCV32)
			result.s = (int32_t)0x80000000;
#else
			result.s = (int64_t)0x80000000;
#endif
			SET_OV();
		      }
		    else
#if defined(TARGET_RISCV32)
		      result.s = (int32_t)(int32_t)tmp;
#else
		    result.s = (int64_t)(int32_t)tmp;
#endif

		  }
		break;
      case ksubw:
		  {

		    int64_t sra, srb, tmp;
		    sra = sext_xlen(rs1);
		    srb = sext_xlen(rs2);

		    tmp = sra - srb;

		    if ((long long) 0x000000007FFFFFFFLL < tmp)
		      {
			result.s = sext_xlen(0x7FFFFFFF);
			SET_OV();
		      }
		    else if ((long long) 0xFFFFFFFF80000000LL > tmp)
		      {
			result.s = sext_xlen(0x80000000);
			SET_OV();
		      }
		    else
		      result.s = sext_xlen(tmp);
		  }break;
      case kdmbb:
		  {
		    long long sra, srb, tmp;
		    UINT ra = rs1;
		    UINT rb = rs2;

		    sra = (long long) ((short int) ((int)ra));
		    srb = (long long) ((short int) ((int)rb));

		    if ((long long)0xffffffffffff8000LL != sra || (long long)0xffffffffffff8000LL != srb)
		      {
			tmp = (sra * srb) << 1;
#if defined(TARGET_RISCV32)
			result.s = (int32_t)tmp;
#else
			result.s = (int64_t)tmp;
#endif
		      }
		    else
		      {
#if defined(TARGET_RISCV32)
			result.s = (int32_t)(int32_t)0x7FFFFFFF;
#else
			result.s = (int64_t)(int32_t)0x7FFFFFFF;
#endif
			SET_OV();
		      }

		  }break;
      case kdmbt:
		  {

		    long long sra, srb, tmp;
		    UINT ra = rs1;
		    UINT rb = rs2;

		    sra = (long long) ((short int) ((int)ra));
		    srb = (long long) ((short int) (((int)rb) >> 16));

		    if ((long long)0xffffffffffff8000LL != sra || (long long)0xffffffffffff8000LL != srb)
		      {
			tmp = (sra * srb) << 1;
#if defined(TARGET_RISCV32)
			result.s = (int32_t)tmp;
#else
			result.s = (int64_t)tmp;
#endif
		      }
		    else
		      {
#if defined(TARGET_RISCV32)
			result.s = (int32_t)(int32_t)0x7FFFFFFF;
#else
			result.s = (int64_t)(int32_t)0x7FFFFFFF;
#endif
			SET_OV();
		      }
		  }break;
      case kdmtt: {

		      long long sra, srb, tmp;
		      UINT ra = rs1;
		      UINT rb =  rs2;

		      sra = (long long) ((short int) (((int)ra) >> 16));
		      srb = (long long) ((short int) (((int)rb) >> 16));

		      if ((long long)0xffffffffffff8000LL != sra || (long long)0xffffffffffff8000LL != srb)
			{
			  tmp = (sra * srb) << 1;
#if defined(TARGET_RISCV32)
			  result.s = (int32_t)tmp;
#else
			  result.s = (int64_t)tmp;
#endif
			}
		      else
			{
			  result.s = 0x7FFFFFFF;
			  SET_OV();
			}
		  }break;
      case kaddh:
		  {
		    int64_t tmp = (int64_t)reg_rs1.s + (int64_t)reg_rs2.s;
		    result.s = insn_sat_helper (tmp, 15);
		  }
		break;
      case ksubh:
		  {
		    int64_t tmp = (int64_t)reg_rs1.s - (int64_t)reg_rs2.s;
		    result.s = insn_sat_helper (tmp, 15);
		  }
		break;
      case khmbb:
		  {
		    int16_t aop = reg_rs1.b16.h0;
		    int16_t bop = reg_rs2.b16.h0;
		    result.s = insn_sat_khm_helper (aop, bop);
		  }
		break;
      case khmbt:
		  {
		    int16_t aop = reg_rs1.b16.h0;
		    int16_t bop = reg_rs2.b16.h1;
		    result.s = insn_sat_khm_helper (aop, bop);
		  }
		break;
      case khmtt:
		  {
		    int16_t aop = reg_rs1.b16.h1;
		    int16_t bop = reg_rs2.b16.h1;
		    result.s = insn_sat_khm_helper (aop, bop);
		  }
		break;
      case kwmmul:
		  {
		    union64_t temp;
		    for (i = 0; i < vec32_num; i++)
		      {
			if ((*(ptr_a32 + i) != 0x80000000)
			    || (*(ptr_b32 + i) != 0x80000000))
			  {
			    temp.d0 = ((int64_t) *(ptr_a32 + i) * *(ptr_b32 + i)) << 1;
			    *(ptr32 + i) = temp.b32.w1;
			  }
			else
			  {
			    *(ptr32 + i) = 0x7fffffff;
			    SET_OV();
			  }
		      }
		    result.s = result.s;
		  }
		break;
      case kwmmul_u:
		  {
		    union64_t temp;
		    for (i = 0; i < vec32_num; i++)
		      {
			if ((*(ptr_a32 + i) != 0x80000000)
			    || (*(ptr_b32 + i) != 0x80000000))
			  {
			    temp.d0 = ((int64_t) *(ptr_a32 + i) * *(ptr_b32 + i));
			    /* Let 30bit add 1 and left sh1ft 1.  */
			    temp.d0 = (temp.d0 + (int32_t) (1 << 30)) << 1;
			    *(ptr32 + i) = temp.b32.w1;
			  }
			else
			  {
			    *(ptr32 + i) = 0x7fffffff;
			    SET_OV();
			  }
		      }
		    result.s = result.s;
		  }
		break;
      case pbsad:
		  {
		    int a, b, c, d;
		    /* The four unsigned 8-bit elements of Ra are subtracted from the four
		       unsigned 8-bit elements of Rb.  */
		    a = (reg_rs1.u & 0xff) - (reg_rs2.u & 0xff);
		    b = ((reg_rs1.u >> 8) & 0xff) - ((reg_rs2.u >> 8) & 0xff);
		    c = ((reg_rs1.u >> 16) & 0xff) - ((reg_rs2.u >> 16) & 0xff);
		    d = ((reg_rs1.u >> 24) & 0xff) - ((reg_rs2.u >> 24) & 0xff);

		    /* Absolute difference of four unsigned 8-bit data elements.  */
		    a = (a >= 0) ? a : -a;
		    b = (b >= 0) ? b : -b;
		    c = (c >= 0) ? c : -c;
		    d = (d >= 0) ? d : -d;

		    /* pbsad */
		    result.u = a + b + c + d;

		  }
		break;
      case smxds:
		  {
		    /* Rt = (Ra[31:16] * Rb[15:0]) - (Ra[15:0] * Rb[31:16]) */
		    for (i = 0; i <= vec32_num; i+=2)
		      {
			*(ptr32 + (i / 2)) =
			  ((int32_t) *(ptr_a16 + (i + 1)) * *(ptr_b16 + i))
			  - ((int32_t) *(ptr_a16 + i) * *(ptr_b16 + (i + 1)));
		      }
		    result.s = result.s;
		  }
		break;
      case smdrs:
		  {
		    /* Rt = (Ra[15:0] * Rb[15:0]) - (Ra[31:16] * Rb[31:16]) */
		    for (i = 0; i <= vec32_num; i+=2)
		      {
			*(ptr32 + (i / 2)) =
			  ((int32_t) *(ptr_a16 + i) * *(ptr_b16 + i))
			  - ((int32_t) *(ptr_a16 + (i + 1)) * *(ptr_b16 + (i + 1)));
		      }
		    result.s = result.s;
		  }
		break;
      case smds:
		  {
		    /* Rt = (Ra[31:16] * Rb[31:16]) - (Ra[15:0] * Rb[15:0]) */
		    for (i = 0; i <= vec32_num; i+=2)
		      {
			*(ptr32 + (i / 2)) =
			  ((int32_t) *(ptr_a16 + (i + 1)) * *(ptr_b16 + (i + 1)))
			  - ((int32_t) *(ptr_a16 + i) * *(ptr_b16 + i));
		      }
		    result.s = result.s;
		  }
		break;
      case kmxda:
		  {
		    for (i = 0; i < vec32_num; i++)
		      {
			if ((*(ptr_a32 + i) != 0x80008000)
			    || (*(ptr_b32 + i) != 0x80008000))
			  {
			    *(ptr32 + i) =
			      ((int32_t) *(ptr_a16 + (i + 1)) * *(ptr_b16 + i))
			      + ((int32_t) *(ptr_a16 + i) * *(ptr_b16 + (i + 1)));
			  }
			else
			  {
			    *(ptr32 + i) = 0x7fffffff;
			    SET_OV();
			  }
		      }
		    result.s = result.s;
		  }
		break;
      case kmda:
		  {
		    for (i = 0; i < vec32_num; i++)
		      {
			if ((*(ptr_a32 + i) != 0x80008000)
			    || (*(ptr_b32 + i) != 0x80008000))
			  {
			    *(ptr32 + i) =
			      ((int32_t) *(ptr_a16 + (i + 1)) * *(ptr_b16 + (i + 1)))
			      + ((int32_t) *(ptr_a16 + i) * *(ptr_b16 + i));
			  }
			else
			  {
			    *(ptr32 + i) = 0x7fffffff;
			    SET_OV();
			  }
		      }
		    result.s = result.s;
		  }
		break;
      case smtt16:
		  {
		    /* Rt = Ra[31:16] * Rb[31:16] */
#if (WITH_TARGET_WORD_BITSIZE == 32)
		    result.s = reg_rs1.b16.h1 * reg_rs2.b16.h1;
#else
		    result.b32.i0 = reg_rs1.b16.h1 * reg_rs2.b16.h1;
		    result.b32.i1 = reg_rs1.b16.h3 * reg_rs2.b16.h3;
#endif
		  }
		break;
      case smbt16:
		  {
		    /* Rt = Ra[15:0] * Rb[31:16] */
#if (WITH_TARGET_WORD_BITSIZE == 32)
		    result.s = reg_rs1.b16.h0 * reg_rs2.b16.h1;
#else
		    result.b32.i0 = reg_rs1.b16.h0 * reg_rs2.b16.h1;
		    result.b32.i1 = reg_rs1.b16.h2 * reg_rs2.b16.h3;
#endif
		  }
		break;
      case smbb16:
		  {
		    /* Rt = Ra[15:0] * Rb[15:0] */
#if (WITH_TARGET_WORD_BITSIZE == 32)
		    result.s = reg_rs1.b16.h0 * reg_rs2.b16.h0;
#else
		    result.b32.i0 = reg_rs1.b16.h0 * reg_rs2.b16.h0;
		    result.b32.i1 = reg_rs1.b16.h2 * reg_rs2.b16.h2;
#endif
		  }
		break;
#if 0
      case kmmwt2_u:
		  {
		    int64_t res;
		    int b16_offset = 1;
		    for (i = 0; i < vec32_num; i++)
		      {
			if ((*(ptr_a32 + i) == 0x80000000)
			    && ((int32_t)*(ptr_b16 + b16_offset) == 0x8000))
			  {
			    *(ptr32 + i) = 0x7fffffff;
			    SET_OV();
			  }
			else
			  {
			    res = ((int64_t) *(ptr_a32 + i)
				   * (int64_t) *(ptr_b16 + b16_offset));
			    int32_t rnd_val = (res & (1UL << 14)) ? (1 << 15) : 0;
			    res += rnd_val;
			    res = res >> 15;
			    *(ptr32 + i) = insn_sat_helper(res, 31);
			  }
			b16_offset += 2;
		      }
		    result.s = result.s;
		  }
		break;
#endif
#if 0
      case kmmwt2:
		  {
		    int b16_offset = 1;
		    for (i = 0; i < vec32_num; i++)
		      {
			if ((*(ptr_a32 + i) == 0x80000000)
			    && ((int32_t)*(ptr_b16 + b16_offset) == 0x8000))
			  {
			    *(ptr32 + i) = 0x7fffffff;
			    SET_OV();
			  }
			else
			  {
			    *(ptr32 + i) = ((int64_t) *(ptr_a32 + i)
					    * (int64_t) *(ptr_b16 + b16_offset)) >> 15;
			  }
			b16_offset += 2;
		      }
		    result.s = result.s;
		  }
		break;
      case kmmwb2_u:
		  {
		    int64_t res;
		    int round_up, b16_offset = 0;
		    for (i = 0; i < vec32_num; i++)
		      {
			if ((*(ptr_a32 + i) == 0x80000000)
			    && ((int32_t)*(ptr_b16 + b16_offset) == 0x8000))
			  {
			    *(ptr32 + i) = 0x7fffffff;
			    SET_OV();
			  }
			else
			  {
			    res = ((int64_t) *(ptr_a32 + i)
				   * (int64_t) *(ptr_b16 + b16_offset));
			    round_up = (res >> 14) & 0x1;
			    if (round_up != 0)
			      *(ptr32 + i) = (res >> 15) + 1;
			    else
			      *(ptr32 + i) = res >> 15;
			  }
			b16_offset += 2;
		      }
		    result.s = result.s;
		  }
		break;
#endif
#if 0
      case kmmwb2:
		  {
		    int b16_offset = 0;
		    for (i = 0; i < vec32_num; i++)
		      {
			if ((*(ptr_a32 + i) == 0x80000000)
			    && ((*(ptr_b16 + b16_offset)) == 0x8000))
			  {
			    *(ptr32 + i) = 0x7fffffff;
			    SET_OV();
			  }
			else
			  {
			    *(ptr32 + i) = ((int64_t) *(ptr_a32 + i)
					    * (int64_t) *(ptr_b16 + b16_offset)) >> 15;
			  }
			b16_offset += 2;
		      }
		    result.s = result.s;
		  }
		break;
#endif
      case smmwt_u:
		  {
		    int64_t res;
		    int round_up, b16_offset = 1;
#if defined(TARGET_RISCV64)
		    int vec = 1;
#else
		    int vec = 0;
#endif

		    for (i = 0; i <= vec; i++)
		      {
			res = ((int64_t) *(ptr_a32 + i)
			       * (int64_t) *(ptr_b16 + b16_offset));
			round_up = (res >> 15) & 0x1;
			if (round_up != 0)
			  *(ptr32 + i) = (res >> 16) + 1;
			else
			  *(ptr32 + i) = res >> 16;
			b16_offset += 2;
		      }
		    result.s = result.s;
		  }
		break;
      case smmwt:
		  {
		    int b16_offset = 1;
#if defined(TARGET_RISCV64)
		    int vec = 1;
#else
		    int vec = 0;
#endif

		    for (i = 0; i <= vec; i++)
		      {
			*(ptr32 + i) = ((int64_t) *(ptr_a32 + i)
					* (int64_t) *(ptr_b16 + b16_offset)) >> 16;
			b16_offset += 2;
		      }
		    result.s = result.s;
		  }
		break;
      case smmwb_u:
		  {
		    int64_t res;
		    int round_up, b16_offset = 0;
#if defined(TARGET_RISCV64)
		    int vec = 1;
#else
		    int vec = 0;
#endif

		    for (i = 0; i <= vec; i++)
		      {
			res = ((int64_t) *(ptr_a32 + i)
			       * (int64_t) *(ptr_b16 + b16_offset));
			round_up = (res >> 15) & 0x1;
			if (round_up != 0)
			  *(ptr32 + i) = (res >> 16) + 1;
			else
			  *(ptr32 + i) = res >> 16;
			b16_offset += 2;
		      }
		    result.s = result.s;
		  }
		break;
      case smmwb:
		  {
		    int b16_offset = 0;
#if defined(TARGET_RISCV64)
		    int vec = 1;
#else
		    int vec = 0;
#endif

		    for (i = 0; i <= vec; i++)
		      {
			*(ptr32 + i) = ((int64_t) *(ptr_a32 + i)
					* (int64_t) *(ptr_b16 + b16_offset)) >> 16;
			b16_offset += 2;
		      }
		    result.s = result.s;
		  }
		break;
      case pkbb16:
		  {
		    /* Rt[31:0] = CONCAT(Ra[15:0], Rb[15:0]) */
#if (WITH_TARGET_WORD_BITSIZE == 32)
		    result.s = (reg_rs1.b16.h0 << 16)
		      | reg_rs2.ub16.h0;
#else
		    result.b32.i0 = (reg_rs1.b16.h0 << 16)
		      | reg_rs1.ub16.h0;
		    result.b32.i1 = (reg_rs2.b16.h2 << 16)
		      | reg_rs2.ub16.h2;
#endif
		  }
		break;
      case pkbt16:
		  {
		    /* Rt[31:0] = CONCAT(Ra[15:0], Rb[31:16]) */
#if (WITH_TARGET_WORD_BITSIZE == 32)
		    result.s = (reg_rs1.b16.h0 << 16)
		      | reg_rs2.ub16.h1;
#else
		    result.b32.i0 = (reg_rs1.b16.h0 << 16)
		      | reg_rs2.ub16.h1;
		    result.b32.i1 = (reg_rs1.b16.h2 << 16)
		      | reg_rs2.ub16.h3;
#endif
		  }
		break;
      case pktb16:
		  {
		    /* Rt[31:0] = CONCAT(Ra[31:16], Rb[15:0]) */
#if (WITH_TARGET_WORD_BITSIZE == 32)
		    result.s = (reg_rs1.b16.h1 << 16)
		      | reg_rs2.ub16.h0;
#else
		    result.b32.i0 = (reg_rs1.b16.h1 << 16)
		      | reg_rs2.ub16.h0;
		    result.b32.i1 = (reg_rs1.b16.h3 << 16)
		      | reg_rs2.ub16.h2;
#endif
		  }
		break;
      case pktt16:
		  {
		    /* Rt[31:0] = CONCAT(Ra[31:16], Rb[31:16]) */
#if (WITH_TARGET_WORD_BITSIZE == 32)
		    result.s = (reg_rs1.b16.h1 << 16)
		      | reg_rs2.ub16.h1;
#else
		    result.b32.i0 = (reg_rs1.b16.h1 << 16)
		      | reg_rs2.ub16.h1;
		    result.b32.i1 = (reg_rs1.b16.h3 << 16)
		      | reg_rs2.ub16.h3;
#endif
		  }
		break;
      case smmul:
		  {
#if (WITH_TARGET_WORD_BITSIZE == 32)
		    result.s =
		      ((int64_t) reg_rs1.s * (int64_t) reg_rs2.s) >> 32;
#else
		    result.b32.i0 = ((int64_t) reg_rs1.b32.i0
				     * (int64_t) reg_rs2.b32.i0) >> 32;
		    result.b32.i1 = ((int64_t) reg_rs1.b32.i1
				     * (int64_t) reg_rs2.b32.i1) >> 32;
#endif
		  }
		break;
      case smmul_u:
		  {
		    int32_t round_up;
		    int64_t res;
#if defined(TARGET_RISCV32)
		    int vec = 0;
#else
		    int vec = 1;
#endif

		    for (i = 0; i <= vec; i++)
		      {
			res = (int64_t) *(ptr_a32 + i) * (int64_t) *(ptr_b32 + i);
			round_up = (res >> 31) & 0x1;
			if (round_up != 0)
			  *(ptr32 + i) = (res >> 32) + 1;
			else
			  *(ptr32 + i) = res >> 32;
		      }
		    result.s = result.s;
		  }
		break;
      default:
		break;
    }
    return result.u;

}
#define CASE(NAME) \
  target_ulong helper_andes_dsp_##NAME(target_ulong rs1) \
{ \
  return psimd_rr(NAME, rs1); \
}

#include "andes_dsp_rr_psimd.def"
#undef CASE
#undef CASE_ALIAS
#define CASE_ALIAS(NAME, ALIAS) CASE(NAME)
#define CASE(NAME) \
  target_ulong helper_andes_dsp_##NAME(target_ulong rs1, target_ulong rs2) \
{ \
  return psimd_rr_uimm(NAME, rs1, rs2); \
}

#include "andes_dsp_rr_uimm5.def"
#undef CASE
#undef CASE_ALIAS
uint64_t helper_andes_dsp_umsr64(uint64_t rd, target_ulong rs1,  target_ulong rs2)
{
  union64u_t t, a, b;
  a.d0 = rs1;
  b.d0 = rs2;
  t.d0 = rd;


  t.d0 = t.d0 - (uint64_t)a.b32.w0 * b.b32.w0;
#if defined(TARGET_RISCV64)
  t.d0 = t.d0 - (uint64_t)a.b32.w1 * b.b32.w1;
#endif
  return t.d0;
}
uint64_t helper_andes_dsp_umar64(uint64_t rd, target_ulong rs1,  target_ulong rs2)
{

  union64u_t t, a, b;
  a.d0 = rs1;
  b.d0 = rs2;
  t.d0 = rd;


  t.d0 = t.d0 + (uint64_t)a.b32.w0 * b.b32.w0;
#if defined(TARGET_RISCV64)
  t.d0 = t.d0 + (uint64_t)a.b32.w1 * b.b32.w1;
#endif
  return t.d0;
}
uint64_t helper_andes_dsp_smsr64(uint64_t rd, target_ulong rs1,  target_ulong rs2)
{

  union64_t t, a, b;

  a.d0 = rs1;
  b.d0 = rs2;
  t.d0 = rd;

  t.d0 = t.d0 - (int64_t)(int32_t)a.b32.w0 * (int32_t)b.b32.w0;
#if defined(TARGET_RISCV64)
  t.d0 = t.d0 - (int64_t)(int32_t)a.b32.w1 * (int32_t)b.b32.w1;
#endif
  return t.d0;
}
uint64_t helper_andes_dsp_smslxda(uint64_t rd, target_ulong rs1,  target_ulong rs2)
{

  union64_t t;
  union64_t a, b;

  a.d0 = rs1;
  b.d0 = rs2;
  t.d0 = rd;

  t.d0 = t.d0 - (int32_t)a.b16.h1 * b.b16.h0;
  t.d0 = t.d0 - (int32_t)a.b16.h0 * b.b16.h1;
#if defined(TARGET_RISCV64)
  t.d0 = t.d0 - (int32_t)a.b16.h3 * b.b16.h2;
  t.d0 = t.d0 - (int32_t)a.b16.h2 * b.b16.h3;
#endif
  return t.d0;
}
uint64_t helper_andes_dsp_smslda(uint64_t rd, target_ulong rs1,  target_ulong rs2)
{
  union64_t t;
  union64_t a, b;

  a.d0 = rs1;
  b.d0 = rs2;
  t.d0 = rd;

  t.d0 = t.d0 - (int32_t)a.b16.h1 * b.b16.h1;
  t.d0 = t.d0 - (int32_t)a.b16.h0 * b.b16.h0;
#if defined(TARGET_RISCV64)
  t.d0 = t.d0 - (int32_t)a.b16.h3 * b.b16.h3;
  t.d0 = t.d0 - (int32_t)a.b16.h2 * b.b16.h2;
#endif
  return t.d0;

}
uint64_t helper_andes_dsp_smalxds(uint64_t rd, target_ulong rs1,  target_ulong rs2)
{

  union64_t t;
  union64_t a, b;
  a.d0 = rs1;
  b.d0 = rs2;
  t.d0 = rd;
  t.d0 = t.d0 + (int32_t)a.b16.h1 * b.b16.h0;
  t.d0 = t.d0 - (int32_t)a.b16.h0 * b.b16.h1;
#if defined(TARGET_RISCV64)
  t.d0 = t.d0 + (int32_t)a.b16.h3 * b.b16.h2;
  t.d0 = t.d0 - (int32_t)a.b16.h2 * b.b16.h3;
#endif
  return t.d0;
}
uint64_t helper_andes_dsp_smaldrs(uint64_t rd, target_ulong rs1,  target_ulong rs2)
{
  union64_t t;
  union64_t a, b;
  a.d0 = rs1;
  b.d0 = rs2;
  t.d0 = rd;

  t.d0 = t.d0 + (int32_t)a.b16.h0 * b.b16.h0;
  t.d0 = t.d0 - (int32_t)a.b16.h1 * b.b16.h1;
#if defined(TARGET_RISCV64)
  t.d0 = t.d0 + (int32_t)a.b16.h2 * b.b16.h2;
  t.d0 = t.d0 - (int32_t)a.b16.h3 * b.b16.h3;
#endif
  return t.d0;

}
uint64_t helper_andes_dsp_smalds(uint64_t rd, target_ulong rs1,  target_ulong rs2)
{

  union64_t t;
  union64_t a, b;

  a.d0 = rs1;
  b.d0 = rs2;
  t.d0 = rd;

  t.d0 = t.d0 + (int32_t)a.b16.h1 * b.b16.h1;
  t.d0 = t.d0 - (int32_t)a.b16.h0 * b.b16.h0;
#if defined(TARGET_RISCV64)
  t.d0 = t.d0 + (int32_t)a.b16.h3 * b.b16.h3;
  t.d0 = t.d0 - (int32_t)a.b16.h2 * b.b16.h2;
#endif
  return t.d0;
}
uint64_t helper_andes_dsp_smalxda(uint64_t rd, target_ulong rs1,  target_ulong rs2)
{

  union64_t t;
  union64_t a, b;

  a.d0 = rs1;
  b.d0 = rs2;
  t.d0 = rd;

  t.d0 = t.d0 + (int32_t)a.b16.h1 * b.b16.h0;
  t.d0 = t.d0 + (int32_t)a.b16.h0 * b.b16.h1;
#if defined(TARGET_RISCV64)
  t.d0 = t.d0 + (int32_t)a.b16.h3 * b.b16.h2;
  t.d0 = t.d0 + (int32_t)a.b16.h2 * b.b16.h3;
#endif
  return t.d0;
}
uint64_t helper_andes_dsp_smalda(uint64_t rd, target_ulong rs1,  target_ulong rs2)
{
  union64_t t;
  union64_t a, b;

  a.d0 = rs1;
  b.d0 = rs2;
  t.d0 = rd;

  t.d0 = t.d0 + (int32_t)a.b16.h1 * b.b16.h1;
  t.d0 = t.d0 + (int32_t)a.b16.h0 * b.b16.h0;
#if defined(TARGET_RISCV64)
  t.d0 = t.d0 + (int32_t)a.b16.h3 * b.b16.h3;
  t.d0 = t.d0 + (int32_t)a.b16.h2 * b.b16.h2;
#endif
  return t.d0;

}
uint64_t helper_andes_dsp_smar64(uint64_t rd, target_ulong rs1,  target_ulong rs2)
{

  union64_t t, a, b;

  a.d0 = rs1;
  b.d0 = rs2;
  t.d0 = rd;

  t.d0 = t.d0 + (int64_t)(int32_t)a.b32.w0 * (int32_t)b.b32.w0;
#if defined(TARGET_RISCV64)
  t.d0 += (int64_t)(int32_t)a.b32.w1 * (int32_t)b.b32.w1;
#endif
  return t.d0;
}
target_ulong helper_andes_dsp_bitrevi(target_ulong rs1, target_ulong rs2)
{
  return psimd_rr_uimm(bitrevi, rs1, rs2);
}
target_ulong helper_andes_dsp_wexti(uint64_t rs1, target_ulong imm)
{

  union64u_t a;
  int32_t t;
  a.d0 = rs1;

  t = (int32_t)(a.d0 >> imm);
  return sext32(t);
}
target_ulong helper_andes_dsp_bpick(target_ulong rs1, target_ulong rs2, target_ulong rc)
{

  reg_t result;
  reg_t reg_rs1;
  reg_rs1.u = rs1;
  reg_t reg_rs2;
  reg_rs2.u = rs2;
  reg_t reg_rc;
  reg_rc.u = rc;
  result.u = (reg_rs1.u & reg_rc.u ) | (reg_rs2.u & ~reg_rc.u);
  return result.u;
}
target_ulong helper_andes_dsp_insb(target_ulong rd, target_ulong rs1, target_ulong imm)
{

  union64_t ra, rt;

  ra.d0 = rs1;
  rt.d0 = rd;

  switch (imm)
    {
    case 0:
      rt.b8.b0 = ra.b8.b0;
      break;
    case 1:
      rt.b8.b1 = ra.b8.b0;
      break;
    case 2:
      rt.b8.b2 = ra.b8.b0;
      break;
    case 3:
      rt.b8.b3 = ra.b8.b0;
      break;
    case 4:
      rt.b8.b4 = ra.b8.b0;
      break;
    case 5:
      rt.b8.b5 = ra.b8.b0;
      break;
    case 6:
      rt.b8.b6 = ra.b8.b0;
      break;
    case 7:
      rt.b8.b7 = ra.b8.b0;
      break;
    default:
      break;
    }

  return rt.d0;

}
target_ulong helper_andes_dsp_wext(uint64_t rs1, target_ulong rs2)
{

  union64u_t a;
  int32_t t;
  a.d0 = rs1;

  t = (int32_t)(a.d0 >> (rs2 & 0x1f));
  return sext32(t);

}

#define CASE_ALIAS(NAME, ALIAS) CASE(NAME)
#define CASE(NAME) \
  uint64_t helper_andes_dsp_##NAME(uint64_t rs1, uint64_t rs2) \
{ \
  return psimd_rrr64(NAME, rs1, rs2); \
}

#include "andes_dsp_rrr64.def"
#undef CASE
#undef CASE_ALIAS
#define CASE(NAME) \
  target_ulong helper_andes_dsp_##NAME(target_ulong rs1, target_ulong rs2) \
{ \
  return psimd_rrr(NAME, rs1, rs2); \
}

#include "andes_dsp_rrr_psimd.def"
#undef CASE
#define CASE(NAME) \
  target_ulong helper_andes_dsp_##NAME(target_ulong rd, target_ulong rs1, target_ulong rs2) \
{ \
  return psimd_rrrr(NAME, rd, rs1, rs2); \
}
#include "andes_dsp_rrrr_psimd.def"
#undef CASE
