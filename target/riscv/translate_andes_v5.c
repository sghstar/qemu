/*
 * Andes RISC-V V5 extension translation routines.
 *
 * Copyright (c) 2018 Andes Tech. Corp.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2 or later, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

enum andes_v5_inst_id
{
    /* Code Dense Extension */
    EXEC_IT = (0x8000),

    /* V5 Performance Extension */
    /* custom 0 */
    ADDIGP = 0x01,
    LBGP = 0x00,
    LBUGP = 0x02,
    SBGP = 0x03,

    /* custom 1 */
    LHGP = 0x01,
    LHUGP = 0x05,
    LWGP = 0x02,
    LWUGP = 0x06,
    LDGP = 0x03,
    SHGP = 0x00,
    SWGP = 0x04,
    SDGP = 0x07,

    /* custom 2 */
    BBX = 0x07,
    BBX_BBC = 0x00,
    BBX_BBS = 0x01,
    BEQC = 0x05,
    BNEC = 0x06,
    BFOS = 0x03,
    BFOZ = 0x02,
    LEAF = 0x00,
    LEA_H = 0x05,
    LEA_W = 0x06,
    LEA_D = 0x07,
    LEA_B_ZE = 0x08,
    LEA_H_ZE = 0x09,
    LEA_W_ZE = 0x0a,
    LEA_D_ZE = 0x0b,
    F_FFB = 0x10,
    F_FFZMISM = 0x11,
    F_FFMISM = 0x12,
    F_FLMISM = 0x13,

    /* custom 3 */
    /* empty by now */
};

#define ANDES_V5_CODE_DENSE_MASK (0xe083)
#define ANDES_V5_CODE_DENSE_ID(x) ((x)&ANDES_V5_CODE_DENSE_MASK)
#define ANDES_V5_CODE_DENSE_IMM11(x) (     \
    (extract32(x, 4, 1) << 2) |   \
    (extract32(x, 10, 2) << 3) |  \
    (extract32(x, 2, 1) << 5) |   \
    (extract32(x, 5, 2) << 6) |   \
    (extract32(x, 9, 1) << 8) |   \
    (extract32(x, 3, 1) << 9) |   \
    (extract32(x, 12, 1) << 10) | \
    (extract64(x, 8, 1) << 11) | \
    0)

#define ANDES_V5_GET_JAL_UIMM(inst) ((extract32(inst, 21, 10) << 1) \
                           | (extract32(inst, 20, 1) << 11) \
                           | (extract32(inst, 12, 8) << 12) \
                           | (extract32(inst, 31, 1) << 20))

#define ANDES_V5_PERF_RD(x) extract32(x, 7, 5)
#define ANDES_V5_PERF_RS1(x) extract32(x, 15, 5)
#define ANDES_V5_PERF_RS2(x) extract32(x, 20, 5)

#define ANDES_V5_PERF_C0_ID(x) extract32(x, 12, 2)
#define ANDES_V5_PERF_C0_IMM18_LD(x) (  \
    (extract32(x, 14, 1) << 0) |   \
    (extract32(x, 21, 10) << 1) |  \
    (extract32(x, 20, 1) << 11) |  \
    (extract32(x, 17, 3) << 12) |  \
    (extract32(x, 15, 2) << 15) |  \
    (sextract64(x, 31, 1) << 17) | \
    0)
#define ANDES_V5_PERF_C0_IMM18_ST(x) (  \
    (extract32(x, 14, 1) << 0) |   \
    (extract32(x, 8, 4) << 1) |    \
    (extract32(x, 25, 6) << 5) |   \
    (extract32(x, 7, 1) << 11) |   \
    (extract32(x, 17, 3) << 12) |  \
    (extract32(x, 15, 2) << 15) |  \
    (sextract64(x, 31, 1) << 17) | \
    0)

#define ANDES_V5_PERF_C1_ID(x) extract32(x, 12, 3)
#define ANDES_V5_PERF_C1_IMM18_LD(x) (  \
    (extract32(x, 21, 10) << 1) |  \
    (extract32(x, 20, 1) << 11) |  \
    (extract32(x, 17, 3) << 12) |  \
    (extract32(x, 15, 2) << 15) |  \
    (sextract64(x, 31, 1) << 17) | \
    0)
#define ANDES_V5_PERF_C1_IMM19_LD(x) (  \
    (extract32(x, 22, 9) << 2) |   \
    (extract32(x, 20, 1) << 11) |  \
    (extract32(x, 17, 3) << 12) |  \
    (extract32(x, 15, 2) << 15) |  \
    (extract32(x, 21, 1) << 17) |  \
    (sextract64(x, 31, 1) << 18) | \
    0)
#define ANDES_V5_PERF_C1_IMM20_LD(x) (  \
    (extract32(x, 23, 8) << 3) |   \
    (extract32(x, 20, 1) << 11) |  \
    (extract32(x, 17, 3) << 12) |  \
    (extract32(x, 15, 2) << 15) |  \
    (extract32(x, 21, 2) << 17) |  \
    (sextract64(x, 31, 1) << 19) | \
    0)
#define ANDES_V5_PERF_C1_IMM18_ST(x) (  \
    (extract32(x, 8, 4) << 1) |    \
    (extract32(x, 25, 6) << 5) |   \
    (extract32(x, 7, 1) << 11) |   \
    (extract32(x, 17, 3) << 12) |  \
    (extract32(x, 15, 2) << 15) |  \
    (sextract64(x, 31, 1) << 17) | \
    0)
#define ANDES_V5_PERF_C1_IMM19_ST(x) (  \
    (extract32(x, 9, 3) << 2) |    \
    (extract32(x, 25, 6) << 5) |   \
    (extract32(x, 7, 1) << 11) |   \
    (extract32(x, 17, 3) << 12) |  \
    (extract32(x, 15, 2) << 15) |  \
    (extract32(x, 8, 1) << 17) |   \
    (sextract64(x, 31, 1) << 18) | \
    0)
#define ANDES_V5_PERF_C1_IMM20_ST(x) (  \
    (extract32(x, 10, 2) << 3) |   \
    (extract32(x, 25, 6) << 5) |   \
    (extract32(x, 7, 1) << 11) |   \
    (extract32(x, 17, 3) << 12) |  \
    (extract32(x, 15, 2) << 15) |  \
    (extract32(x, 8, 2) << 17) |   \
    (sextract64(x, 31, 1) << 19) | \
    0)

#define ANDES_V5_PERF_C2_ID(x) extract32(x, 12, 3)
#define ANDES_V5_PERF_C2_BTBX_ID(x) extract32(x, 30, 1)
#define ANDES_V5_PERF_C2_LEAF_ID(x) extract32(x, 25, 7)
#define ANDES_V5_PERF_C2_CIMM6(x) (   \
    (extract32(x, 20, 5) << 0) | \
    (extract32(x, 7, 1) << 5) |  \
    0)
#define ANDES_V5_PERF_C2_IMM11(x) (     \
    (extract32(x, 8, 4) << 1) |    \
    (extract32(x, 25, 5) << 5) |   \
    (sextract64(x, 31, 1) << 10) | \
    0)
#define ANDES_V5_PERF_C2_CIMM7(x) (   \
    (extract32(x, 20, 5) << 0) | \
    (extract32(x, 7, 1) << 5) |  \
    (extract32(x, 30, 1) << 6) | \
    0)

static void andes_v5_gen_branch_tcgv(CPURISCVState *env, DisasContext *ctx, uint32_t opc,
                            TCGv source1, TCGv source2, target_long imm)
{
    TCGLabel *label = gen_new_label();

    switch (opc) {
    case OPC_RISC_BEQ:
        tcg_gen_brcond_tl(TCG_COND_EQ, source1, source2, label);
        break;
    case OPC_RISC_BNE:
        tcg_gen_brcond_tl(TCG_COND_NE, source1, source2, label);
        break;
    case OPC_RISC_BLT:
        tcg_gen_brcond_tl(TCG_COND_LT, source1, source2, label);
        break;
    case OPC_RISC_BGE:
        tcg_gen_brcond_tl(TCG_COND_GE, source1, source2, label);
        break;
    case OPC_RISC_BLTU:
        tcg_gen_brcond_tl(TCG_COND_LTU, source1, source2, label);
        break;
    case OPC_RISC_BGEU:
        tcg_gen_brcond_tl(TCG_COND_GEU, source1, source2, label);
        break;
    default:
        gen_exception_illegal(ctx);
        return;
    }

    gen_goto_tb(ctx, 1, ctx->pc_succ_insn);
    gen_set_label(label); /* branch taken */
    if (!riscv_has_ext(env, RVC) && ((ctx->base.pc_next + imm) & 0x3)) {
        /* misaligned */
        gen_exception_inst_addr_mis(ctx);
    } else {
        gen_goto_tb(ctx, 0, ctx->base.pc_next + imm);
    }
    ctx->base.is_jmp = DISAS_NORETURN;
}

/* exec.it #imm11 */
static int andes_v5_gen_codense_exec_it(CPURISCVState *env, DisasContext *ctx)
{
    CPURVAndesExt *ext = env->ext;
    uint32_t imm11;
    uint32_t insn;
    int next = 0;

    if (!extract32(ext->mmsc_cfg, 3, 1)) { /* mmsc_cfg.ECD */
        qemu_log_mask(LOG_GUEST_ERROR, "exec.it: is supported only when MMSC_CFG.ECD == 1!\n");
        gen_exception_illegal(ctx);
        return next;
    }

    imm11 = ANDES_V5_CODE_DENSE_IMM11(ctx->opcode);

    if (extract32(ext->uitb, 0, 1)) { /* UTIB.HW == 1 */
        qemu_log_mask(LOG_GUEST_ERROR, "exec.it: UITB.HW == 1 is not supported by now!\n");
        gen_exception_illegal(ctx);
        /*
        uint32_t instruction_table[0];
        insn = instruction_table[imm11 >> 2];
        */
        return next;
    } else { /* UTIB.HW == 0 */
        uint32_t vaddr = (ext->uitb & ~0x3) + imm11;
        insn = cpu_ldl_code(env, vaddr);
    }

    /* Execute(insn) */
    /*   do as the replaced instruction, even exceptions,
     *   except ctx->pc_succ_insn value (2).
     */
    ctx->opcode = insn;
    if ((insn & 3) != 3) { /* 32-bit instruction */
        gen_exception_illegal(ctx);
    } else {
        uint32_t op = MASK_OP_MAJOR(insn);
        if (op == OPC_RISC_JAL) {
            /* implement this by hack imm */
            int rd = GET_RD(ctx->opcode);
            target_long imm = ANDES_V5_GET_JAL_UIMM(ctx->opcode);
            target_ulong next_pc = (ctx->base.pc_next >> 21 << 21) | imm;
            imm = next_pc - ctx->base.pc_next;
            gen_jal(env, ctx, rd, imm);
        } else {
            /* JARL done as SPEC already */
            
            /* presume ctx->pc_succ_insn not changed in any ISA extension */
            next = env->isaif.decode_RV32_64G(env, ctx);
        }
    }

    return next;
}

static int andes_v5_gen_custom_0(CPURISCVState *env, DisasContext *ctx)
{
    int next = 0;
    int rd, rs1, rs2;
    uint32_t op;
    target_long imm;

    op = ANDES_V5_PERF_C0_ID(ctx->opcode);
    rd = ANDES_V5_PERF_RD(ctx->opcode);
    rs1 = 3; /* implid GP (x3) */
    switch (op)
    {
    case ADDIGP: /* addi rd, x3, imm18 */
        imm = ANDES_V5_PERF_C0_IMM18_LD(ctx->opcode);
        gen_arith_imm(ctx, OPC_RISC_ADDI, rd, rs1, imm);
        break;
    case LBGP: /* lb rd, [x3 + imm18] */
        imm = ANDES_V5_PERF_C0_IMM18_LD(ctx->opcode);
        gen_load(ctx, OPC_RISC_LB, rd, rs1, imm);
        break;
    case LBUGP: /* lbu rd, [x3 + imm18] */
        imm = ANDES_V5_PERF_C0_IMM18_LD(ctx->opcode);
        gen_load(ctx, OPC_RISC_LBU, rd, rs1, imm);
        break;
    case SBGP: /* sb rs2, [x3 + imm18] */
        rs2 = ANDES_V5_PERF_RS2(ctx->opcode);
        imm = ANDES_V5_PERF_C0_IMM18_ST(ctx->opcode);
        gen_store(ctx, OPC_RISC_SB, rs1, rs2, imm);
        break;
    }

    return next;
}

static int andes_v5_gen_custom_1(CPURISCVState *env, DisasContext *ctx)
{
    int next = 0;
    int rd, rs1, rs2;
    uint32_t op;
    target_long imm;

    op = ANDES_V5_PERF_C1_ID(ctx->opcode);
    rd = ANDES_V5_PERF_RD(ctx->opcode);
    rs1 = 3; /* implid GP (x3) */
    rs2 = ANDES_V5_PERF_RS2(ctx->opcode);
    switch (op)
    {
    case LHGP: /* lh rd, [x3, imm18] */
        imm = ANDES_V5_PERF_C1_IMM18_LD(ctx->opcode);
        gen_load(ctx, OPC_RISC_LH, rd, rs1, imm);
        break;
    case LHUGP: /* lhu rd, [x3 + imm18] */
        imm = ANDES_V5_PERF_C1_IMM18_LD(ctx->opcode);
        gen_load(ctx, OPC_RISC_LHU, rd, rs1, imm);
        break;
    case LWGP: /* lw rd, [x3 + imm19] */
        imm = ANDES_V5_PERF_C1_IMM19_LD(ctx->opcode);
        gen_load(ctx, OPC_RISC_LW, rd, rs1, imm);
        break;
#if defined(TARGET_RISCV64)
    case LWUGP: /* lwu rd, [x3 + imm19] */
        imm = ANDES_V5_PERF_C1_IMM19_LD(ctx->opcode);
        gen_load(ctx, OPC_RISC_LWU, rd, rs1, imm);
        break;
    case LDGP: /* ld rd, [x3, imm20] */
        imm = ANDES_V5_PERF_C1_IMM20_LD(ctx->opcode);
        gen_load(ctx, OPC_RISC_LD, rd, rs1, imm);
        break;
#endif
    case SHGP: /* sh rs2, [x3 + imm18] */
        imm = ANDES_V5_PERF_C1_IMM18_ST(ctx->opcode);
        gen_store(ctx, OPC_RISC_SH, rs1, rs2, imm);
        break;
    case SWGP: /* sw rs2, [x3 + imm19] */
        imm = ANDES_V5_PERF_C1_IMM19_ST(ctx->opcode);
        gen_store(ctx, OPC_RISC_SW, rs1, rs2, imm);
        break;
#if defined(TARGET_RISCV64)
    case SDGP: /* sd rs2, [x3 + imm20] */
        imm = ANDES_V5_PERF_C1_IMM20_ST(ctx->opcode);
        gen_store(ctx, OPC_RISC_SD, rs1, rs2, imm);
        break;
#endif
    default:
        next = 1;
    }

    return next;
}

static int andes_v5_gen_custom_2(CPURISCVState *env, DisasContext *ctx)
{
    int next = 0;
    int rd, rs1, rs2;
    uint32_t op, fun, is, cimm;
    target_long imm;
    TCGv v0, v1, v2;

    op = ANDES_V5_PERF_C2_ID(ctx->opcode);

    switch (op)
    {
    case BBX:
        /* BBX_BBC: bbc rs1, #cimm6, #imm11 */
        /* BBX_BBS: bbs rs1, #cimm6, #imm11 */
        rs1 = ANDES_V5_PERF_RS1(ctx->opcode);
        cimm = ANDES_V5_PERF_C2_CIMM6(ctx->opcode);
        imm = ANDES_V5_PERF_C2_IMM11(ctx->opcode);
        is = BBX_BBC == ANDES_V5_PERF_C2_BTBX_ID(ctx->opcode);
        v0 = tcg_const_tl(0);
        v1 = tcg_temp_new();
        gen_get_gpr(v1, rs1);
        tcg_gen_andi_tl(v1, v1, (target_ulong)1u << cimm);
        andes_v5_gen_branch_tcgv(env, ctx, is ? OPC_RISC_BEQ : OPC_RISC_BNE, v0, v1, imm);
        tcg_temp_free(v0);
        tcg_temp_free(v1);
        break;
    case BEQC: /* beqc rs1, #cimm7, #imm11 */
    case BNEC: /* bnec rs1, #cimm7, #imm11 */
        rs1 = ANDES_V5_PERF_RS1(ctx->opcode);
        cimm = ANDES_V5_PERF_C2_CIMM7(ctx->opcode);
        imm = ANDES_V5_PERF_C2_IMM11(ctx->opcode);
        is = op == BEQC;
        v0 = tcg_const_tl(cimm);
        v1 = tcg_temp_new();
        gen_get_gpr(v1, rs1);
        andes_v5_gen_branch_tcgv(env, ctx, is ? OPC_RISC_BEQ : OPC_RISC_BNE, v0, v1, imm);
        tcg_temp_free(v0);
        tcg_temp_free(v1);
        break;
    case BFOS: /* bfos rd, rs1, #msb6, #lsb6 */
    case BFOZ: /* bfoz rd, rs1, #msb6, #lsb6 */
        rd = ANDES_V5_PERF_RD(ctx->opcode);
        rs1 = ANDES_V5_PERF_RS1(ctx->opcode);
        v0 = tcg_temp_new();
        v1 = tcg_temp_new();
        v2 = tcg_const_tl(ctx->opcode);
        gen_get_gpr(v0, rs1);
        gen_get_gpr(v1, rd);
        gen_helper_andes_v5_bfo_x(v0, v1, v0, v2);
        gen_set_gpr(rd, v0);
        tcg_temp_free(v0);
        tcg_temp_free(v1);
        tcg_temp_free(v2);
        break;
    case LEAF:
        rd = ANDES_V5_PERF_RD(ctx->opcode);
        rs1 = ANDES_V5_PERF_RS1(ctx->opcode);
        rs2 = ANDES_V5_PERF_RS2(ctx->opcode);
        fun = ANDES_V5_PERF_C2_LEAF_ID(ctx->opcode);
        v0 = tcg_temp_new();
        v1 = tcg_temp_new();
        gen_get_gpr(v0, rs1);
        gen_get_gpr(v1, rs2);
        switch (fun)
        {
        case LEA_H: /* lea.h rd, rs1, rs2 */
        case LEA_W: /* lea.w rd, rs1, rs2 */
        case LEA_D: /* lea.d rd, rs1, rs2 */
            tcg_gen_shli_tl(v1, v1, fun - LEA_H + 1);
            tcg_gen_add_tl(v0, v0, v1);
            break;
#if defined(TARGET_RISCV64)
        case LEA_B_ZE: /* lea.b.ze rd, rs1, rs2 */
        case LEA_H_ZE: /* lea.h.ze rd, rs1, rs2 */
        case LEA_W_ZE: /* lea.w.ze rd, rs1, rs2 */
        case LEA_D_ZE: /* lea.d.ze rd, rs1, rs2 */
            tcg_gen_ext32u_tl(v1, v1);
            tcg_gen_shli_tl(v1, v1, fun - LEA_B_ZE);
            tcg_gen_add_tl(v0, v0, v1);
            break;
#endif
        case F_FFB:
        case F_FFZMISM:
        case F_FFMISM:
        case F_FLMISM:
            v2 = tcg_const_tl(ctx->opcode);
            gen_helper_andes_v5_fb_x(v0, v0, v1, v2);
            tcg_temp_free(v2);
            break;
        default:
            next = 1;
        }
        gen_set_gpr(rd, v0);
        tcg_temp_free(v0);
        tcg_temp_free(v1);
        break;
    default:
        next = 1;
    }

    return next;
}

static int andes_v5_decode_RV32_64C(CPURISCVState *env, DisasContext *ctx)
{
    int next = 0; /* done */

    if (ANDES_V5_CODE_DENSE_ID(ctx->opcode) == EXEC_IT) {
        andes_v5_gen_codense_exec_it(env, ctx);
    } else {
        next = 1; /* unhandled */
    }
    if (next) {
        next = decode_RV32_64C(env, ctx); /* standard */
    }

    return next;
}

enum {
    OPC_RISC_FLB   = OPC_RISC_FP_LOAD | (0x0 << 12),
    OPC_RISC_FLH   = OPC_RISC_FP_LOAD | (0x1 << 12),
};

enum {
    OPC_RISC_FSB   = OPC_RISC_FP_STORE | (0x0 << 12),
    OPC_RISC_FSH   = OPC_RISC_FP_STORE | (0x1 << 12),
};

static int andes_v5_decode_RV32_64G(CPURISCVState *env, DisasContext *ctx)
{
    int next = 0; /* done */
    TCGv_i32 v0;

    switch (MASK_OP_MAJOR(ctx->opcode)) {
    case OPC_RISC_CUSTOM_0:
        next = andes_v5_gen_custom_0(env, ctx);
        break;
    case OPC_RISC_CUSTOM_1:
        next = andes_v5_gen_custom_1(env, ctx);
        break;
    case OPC_RISC_CUSTOM_2:
        next = andes_v5_gen_custom_2(env, ctx);
        break;
    case OPC_RISC_CUSTOM_3:
        next = 1; /* unhandled */
        break;
    case OPC_RISC_FP_LOAD:
        if (MASK_OP_FP_LOAD(ctx->opcode) == OPC_RISC_FLB) {
            if (!(ctx->flags & TB_FLAGS_FP_ENABLE)) {
                gen_exception_illegal(ctx);
            } else {
                v0 = tcg_const_i32(ctx->opcode);
                gen_helper_andes_v5_flhw(cpu_env, v0);
                tcg_temp_free_i32(v0);
            }
        } else {
            next = 1;
        }
        break;
    case OPC_RISC_FP_STORE:
        if (MASK_OP_FP_STORE(ctx->opcode) == OPC_RISC_FSB) {
            if (!(ctx->flags & TB_FLAGS_FP_ENABLE)) {
                gen_exception_illegal(ctx);
            } else {
                v0 = tcg_const_i32(ctx->opcode);
                gen_helper_andes_v5_fshw(cpu_env, v0);
                tcg_temp_free_i32(v0);
            }
        } else {
            next = 1;
        }
        break;
    default:
        next = 1; /* unhandled */
        break;
    }
    if (next) {
        next = decode_RV32_64G(env, ctx); /* standard */
    }
    return next;
}

void andes_v5_riscv_isaif_init(CPURISCVState *env)
{
    env->isaif.decode_RV32_64C = andes_v5_decode_RV32_64C;
    env->isaif.decode_RV32_64G = andes_v5_decode_RV32_64G;
}
