/*
 * RISC-V Andes V5 Extension Emulation Helpers
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

target_ulong helper_andes_v5_bfo_x(target_ulong rd, target_ulong rs1,
                                   target_ulong insn)
{
    int msb, lsb, is_se;
    int lsbp1, msbm1, lsbm1, lenm1;
    uint64_t se;
    uint64_t nxrd = rd; /* for safety sake */

    msb = extract32(insn, 26, 6);
    lsb = extract32(insn, 20, 6);
    is_se = 0x3 == (0x7 & (insn >> 12)); /* BFOS */
    lsbp1 = lsb + 1;
    msbm1 = msb - 1;
    lsbm1 = lsb - 1;

    if (msb == 0)
    {
        nxrd = deposit64(nxrd, lsb, 1, 1 & rs1);
        if (lsb < 63)
        {
            se = (is_se && (1 & rs1)) ? -1LL : 0;
            nxrd = deposit64(nxrd, lsbp1, 64 - lsbp1, se);
        }
        if (lsb > 0)
        {
            nxrd = deposit64(nxrd, 0, lsbm1 + 1, 0);
        }
    }
    else if (msb < lsb)
    {
        lenm1 = lsb - msb;
        nxrd = deposit64(nxrd, msb, lenm1 + 1, rs1 >> 0);
        if (lsb < 63)
        {
            se = (is_se && (1 & (rs1 >> lenm1))) ? -1LL : 0;
            nxrd = deposit64(nxrd, lsbp1, 64 - lsbp1, se);
        }
        nxrd = deposit64(nxrd, 0, msbm1 + 1, 0);
    }
    else
    { /* msb >= lsb */
        lenm1 = msb - lsb;
        nxrd = deposit64(nxrd, 0, lenm1 + 1, rs1 >> lsb);
        se = (is_se && (1 & (rs1 >> msb))) ? -1LL : 0;
        nxrd = deposit64(nxrd, lenm1 + 1, 63 - lenm1, se);
    }

    return (target_long)nxrd;
}

typedef int (*test_function)(uint8_t a, uint8_t b);

/* TODO: is_little_endian shall be is_increase */
static int andes_v5_fb_x_internal(uint8_t *bytes1, uint8_t *bytes2, int size,
                             int is_little_endian, test_function test)
{
    int i, found;

    found = 0;

    if (is_little_endian)
    {
        for (i = 0; i < size; ++i)
        {
            if (test(bytes1[i], bytes2[i]))
            {
                found = i - size;
                break;
            }
        }
    }
    else
    { /* is_big_endian */
        for (i = size - 1; i >= 0; --i)
        {
            if (test(bytes1[i], bytes2[i]))
            {
                found = i - size;
                break;
            }
        }
    }

    return found;
}

static int andes_v5_test_match(uint8_t a, uint8_t b)
{
    return (a == b);
}

static int andes_v5_test_mismatch(uint8_t a, uint8_t b)
{
    return (a != b);
}

static int andes_v5_test_zero_mismatch(uint8_t a, uint8_t b)
{
    return (a == 0) || (a != b);
}

target_ulong helper_andes_v5_fb_x(target_ulong rs1, target_ulong rs2,
                                  target_ulong insn)
{
    target_ulong rd;
    uint8_t *pa, *pb;
    uint op, size;

    op = extract32(insn, 25, 7);
    size = sizeof(target_ulong);
    pa = (uint8_t *)&rs1;
    pb = (uint8_t *)&rs2;
    rd = 0;

    switch (op)
    {
    case 0x10: /* FFB */
        /* Each byte in Rs1 is matched with the value in Rs2[7:0].  */
        memset(pb, *pb, size);
        rd = andes_v5_fb_x_internal(pa, pb, size, 1, andes_v5_test_match);
        break;
    case 0x11: /* FFZMISM */
        rd = andes_v5_fb_x_internal(pa, pb, size, 1, andes_v5_test_zero_mismatch);
        break;
    case 0x12: /* FFMISM */
        rd = andes_v5_fb_x_internal(pa, pb, size, 1, andes_v5_test_mismatch);
        break;
    case 0x13: /* FLMISM */
        /* tricky!
         *   # reverse endian to find last
         *   # patch result
         */
        rd = andes_v5_fb_x_internal(pa, pb, size, 0, andes_v5_test_mismatch);
        break;
    default:
        /* helper_raise_exception(env, RISCV_EXCP_ILLEGAL_INST); */
        break;
    }

    return rd;
}

void helper_andes_v5_flhw(CPURISCVState *env, uint32_t insn)
{
    int imm12 = sextract32(insn, 20, 12);
    int rs1 = extract32(insn, 15, 5);
    int frd = extract32(insn, 7, 5);
    int ieee = 1;
    target_long va = env->gpr[rs1] + imm12;
    float16 hfp = cpu_lduw_data(env, va);
    float32 sfp = float16_to_float32(hfp, ieee, &env->fp_status);
    if (float16_is_signaling_nan(hfp, &env->fp_status)) {
        /* env->fcsr |= 0x10; *//* FCSR.NV */ 
        cpu_riscv_set_fflags(env, FPEXC_NV);
        sfp = 0x7fc00000u;
    }
    env->fpr[frd] = (env->fpr[frd] >> 32 << 32) | sfp;
}

void helper_andes_v5_fshw(CPURISCVState *env, uint32_t insn)
{
    int imm12 = (sextract32(insn, 25, 7) << 5) | extract32(insn, 7, 5);
    int rs1 = extract32(insn, 15, 5);
    int frs2 = extract32(insn, 20, 5);
    int ieee = 1;
    target_long va = env->gpr[rs1] + imm12;
    float32 sfp = env->fpr[frs2];
    float16 hfp = float32_to_float16(sfp, ieee, &env->fp_status);
    /* TODO:
     *   + rounding mode "round towards zero"
     *   + saturated overflow handling
     *   + underflow handling
     */
    if (float32_is_signaling_nan(sfp, &env->fp_status)) {
        /* env->fcsr |= 0x10; *//* FCSR.NV */ 
        cpu_riscv_set_fflags(env, FPEXC_NV);
        hfp = 0x7e00u;
    }
    cpu_stw_data(env, va, hfp);
}
