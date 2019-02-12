/*
 * QEMU RISC-V CPU
 *
 * Copyright (c) 2017-2018 Andes Tech. Corp.
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

#include "cpu_andes.h"
#include "exec/cpu_ldst.h"

#define RISCV_DEBUG_INTERRUPT 1

#if RISCV_DEBUG_INTERRUPT
const char * const andes_riscv_intr_names[] = {
    "M-IMECCI",
    "M-BWEI",
    "M-PMOVI",
    "?",
    "S-IMECCI",
    "S-BWEI",
    "S-PMOVI",
    "?",
    "???"
};
#endif

#ifndef CONFIG_USER_ONLY
/*
 * Return RISC-V IRQ number if an interrupt should be taken, else -1.
 * Used in cpu-exec.c
 *
 * Adapted from Spike's processor_t::take_interrupt()
 */
static int andes_riscv_cpu_hw_interrupts_pending(CPURISCVState *env)
{
    CPURVAndesExt* ext = env->ext;
    target_ulong pending_interrupts = atomic_read(&ext->slip) & ext->slie;

    target_ulong mie = get_field(env->mstatus, MSTATUS_MIE);
    target_ulong m_enabled = env->priv < PRV_M || (env->priv == PRV_M && mie);
    target_ulong enabled_interrupts = pending_interrupts &
                                      ~ext->mslideleg & -m_enabled;

    target_ulong sie = get_field(env->mstatus, MSTATUS_SIE);
    target_ulong s_enabled = env->priv < PRV_S || (env->priv == PRV_S && sie);
    enabled_interrupts |= pending_interrupts & ext->mslideleg &
                          -s_enabled;

    if (enabled_interrupts) {
        return ctz64(enabled_interrupts) + ANDES_SLI_BIAS; /* since non-zero */
    } else {
        return riscv_cpu_local_irq_pending(env);
    }
}
#endif

bool andes_riscv_cpu_exec_interrupt(CPUState *cs, int interrupt_request)
{
#if !defined(CONFIG_USER_ONLY)
    if (interrupt_request & CPU_INTERRUPT_HARD) {
        RISCVCPU *cpu = RISCV_CPU(cs);
        CPURISCVState *env = &cpu->env;
        int interruptno = andes_riscv_cpu_hw_interrupts_pending(env);
        if (interruptno >= 0) {
            CPUClass *cc = CPU_GET_CLASS(cs);
            cs->exception_index = RISCV_EXCP_INT_FLAG | interruptno;
            cc->do_interrupt(cs);
            return true;
        }
    }
#endif

    return false;
}

/*
 * Handle Traps
 *
 * Adapted from Spike's processor_t::take_trap.
 *
 */
#if !defined(CONFIG_USER_ONLY)
static void andes_riscv_cpu_do_interrupt_essence(CPUState *cs)
{
    RISCVCPU *cpu = RISCV_CPU(cs);
    CPURISCVState *env = &cpu->env;
    CPURVAndesExt* ext = env->ext;

    int log_cause = cs->exception_index & RISCV_EXCP_INT_MASK;
    if (RISCV_DEBUG_INTERRUPT) {
        int idx = (log_cause > ANDES_SLI_BIAS) ? (ANDES_SLI_BIAS - 4) : 0;
        idx = log_cause - ANDES_SLI - idx;
        qemu_log("core " TARGET_FMT_ld ": intr %s, epc 0x" TARGET_FMT_lx "\n",
            env->mhartid, andes_riscv_intr_names[idx], env->pc);
    }

    target_ulong fixed_cause;
    /* hacky for now. the MSB (bit 63) indicates interrupt but cs->exception
        index is only 32 bits wide */
    fixed_cause = cs->exception_index & RISCV_EXCP_INT_MASK;
    fixed_cause |= ((target_ulong)1) << (TARGET_LONG_BITS - 1);

    target_ulong backup_epc = env->pc;

    target_ulong bit = fixed_cause;
    target_ulong deleg = ext->mslideleg;

    bit &= ~((target_ulong)1 << (TARGET_LONG_BITS - 1));


    int hasbadaddr = 0;
    int is_smode = 0;
    if (bit >= ANDES_SLI_BIAS) {
        is_smode = 1;
        bit -= ANDES_SLI_BIAS;
    }

    if (env->priv <= PRV_S && is_smode && ((deleg >> bit) & 1)) {
        /* handle the trap in S-mode */
        /* No need to check STVEC for misaligned - lower 2 bits cannot be set */
        env->pc = env->stvec;
        env->scause = fixed_cause;
        env->sepc = backup_epc;

        if (hasbadaddr) {
            if (RISCV_DEBUG_INTERRUPT) {
                qemu_log("core " TARGET_FMT_ld
                    ": badaddr 0x" TARGET_FMT_lx "\n", env->mhartid, env->badaddr);
            }
            env->sbadaddr = env->badaddr;
        } else {
            /* otherwise we must clear sbadaddr/stval
             * todo: support populating stval on illegal instructions */
            env->sbadaddr = 0;
        }

        target_ulong s = env->mstatus;
        s = set_field(s, MSTATUS_SPIE, env->priv_ver >= PRIV_VERSION_1_10_0 ?
            get_field(s, MSTATUS_SIE) : get_field(s, MSTATUS_UIE << env->priv));
        s = set_field(s, MSTATUS_SPP, env->priv);
        s = set_field(s, MSTATUS_SIE, 0);
        csr_write_helper(env, s, CSR_MSTATUS);
        riscv_set_mode(env, PRV_S);
    } else {
        /* No need to check MTVEC for misaligned - lower 2 bits cannot be set */
        env->pc = env->mtvec;
        env->mepc = backup_epc;
        env->mcause = fixed_cause;

        if (hasbadaddr) {
            if (RISCV_DEBUG_INTERRUPT) {
                qemu_log("core " TARGET_FMT_ld
                    ": badaddr 0x" TARGET_FMT_lx "\n", env->mhartid, env->badaddr);
            }
            env->mbadaddr = env->badaddr;
        } else {
            /* otherwise we must clear mbadaddr/mtval
             * todo: support populating mtval on illegal instructions */
            env->mbadaddr = 0;
        }

        target_ulong s = env->mstatus;
        s = set_field(s, MSTATUS_MPIE, env->priv_ver >= PRIV_VERSION_1_10_0 ?
            get_field(s, MSTATUS_MIE) : get_field(s, MSTATUS_UIE << env->priv));
        s = set_field(s, MSTATUS_MPP, env->priv);
        s = set_field(s, MSTATUS_MIE, 0);
        csr_write_helper(env, s, CSR_MSTATUS);
        riscv_set_mode(env, PRV_M);
    }
    /* TODO yield load reservation  */
    cs->exception_index = EXCP_NONE; /* mark handled to qemu */
}
#endif

/*
 * Handle Traps
 *
 * Adapted from Spike's processor_t::take_trap.
 *
 */
void andes_riscv_cpu_do_interrupt(CPUState *cs)
{
#if !defined(CONFIG_USER_ONLY)

    RISCVCPU *cpu = RISCV_CPU(cs);
    CPURISCVState *env = &cpu->env;
    CPURVAndesExt* ext = env->ext;
    target_ulong *cause, ip_mask, base;
    uint32_t irq_id = 0;

    int log_cause = cs->exception_index & RISCV_EXCP_INT_MASK;
    if ((cs->exception_index & RISCV_EXCP_INT_FLAG) && (log_cause >= ANDES_SLI)) {
        andes_riscv_cpu_do_interrupt_essence(cs);
    } else {
        riscv_cpu_do_interrupt(cs);
    }

    /* vectored extension */
    if ((ext->mmisc_ctl >> MMISC_CTL_VEC_PLIC) & 1) {
        if (env->priv == PRV_S) {
            base = env->stvec;
            cause = &env->scause;
            ip_mask = SIP_SEIP;
            irq_id = ext->vectored_irq_s;
        } else if (env->priv == PRV_M) {
            base = env->mtvec;
            cause = &env->mcause;
            ip_mask = MIP_MEIP;
            irq_id = ext->vectored_irq_m;
        } else {
            g_assert_not_reached();
        }
        if ((target_long)*cause < 0) {
            uint code = *cause & 0xffffu;
            if ((code >= 8) && (code < 12)) {
                assert(irq_id);
                *cause = irq_id;
                riscv_cpu_update_mip(cpu, ip_mask, BOOL_TO_MASK(0));
            } else {
                irq_id = 0;
            }
        }
        env->pc = ((uint64_t)env->pc >> 32) << 32;
        env->pc |= cpu_ldl_data(env, base + (irq_id << 2));
    }
#endif
}

#if defined(TARGET_RISCV32)

static void rv32gcsux_andes_a25_priv1_10_0_cpu_init(Object *obj)
{
    CPURISCVState *env = &RISCV_CPU(obj)->env;
    set_misa(env, RV32 | RVI | RVM | RVA | RVF | RVD | RVC | RVS | RVU | RVX);
    set_versions(env, USER_VERSION_2_02_0, PRIV_VERSION_1_10_0);
    set_resetvec(env, ANDES_A25_DEFAULT_RSTVEC);
    set_feature(env, RISCV_FEATURE_MMU);

    CPURVAndesExt *ext = g_new0(CPURVAndesExt, 1);
    env->ext = ext;
#ifndef CONFIG_USER_ONLY
    env->mstatus = (0x3 << 11); /* MPP=M */
#endif
    ext->micm_cfg = \
        (0u << MMSC_CFG_ISET) |         /* 64 sets */
        (3u << MMSC_CFG_IWAY) |         /*  4 ways */
        (3u << MMSC_CFG_ISZ) |          /* 32 bytes per line */
        (0u << MMSC_CFG_ILCK) |         /* no locking support */
        0;
    ext->mdcm_cfg = \
        (0u << MMSC_CFG_DSET) |         /* 64 sets */
        (3u << MMSC_CFG_DWAY) |         /*  4 ways */
        (3u << MMSC_CFG_DSZ) |          /* 32 bytes per line */
        (0u << MMSC_CFG_DLCK) |         /* no locking support */
        0;
    ext->mmsc_cfg = \
        (1u << MMSC_CFG_ECD) |
        (1u << MMSC_CFG_VPLIC) |
        (1u << MMSC_CFG_EV5PE) |
        (1u << MMSC_CFG_PMNDS) |
        (1u << MMSC_CFG_CCTLCSR) |
        (1u << MMSC_CFG_EFHW) |
        0;

    andes_riscv_csrif_init(env);
    andes_riscv_isaif_init(env);
}

static void rv32gcsux_andes_priv1_10_0_cpu_init(Object *obj)
{
    CPURISCVState *env = &RISCV_CPU(obj)->env;
    set_misa(env, RV32 | RVI | RVM | RVA | RVF | RVD | RVC | RVS | RVU | RVX);
    set_versions(env, USER_VERSION_2_02_0, PRIV_VERSION_1_10_0);
    set_resetvec(env, ANDES_N25_DEFAULT_RSTVEC);

    CPURVAndesExt *ext = g_new0(CPURVAndesExt, 1);
    env->ext = ext;
#ifndef CONFIG_USER_ONLY
    env->mstatus = (0x3 << 11); /* MPP=M */
#endif
    ext->micm_cfg = \
        (0u << MMSC_CFG_ISET) |         /* 64 sets */
        (3u << MMSC_CFG_IWAY) |         /*  4 ways */
        (3u << MMSC_CFG_ISZ) |          /* 32 bytes per line */
        (0u << MMSC_CFG_ILCK) |         /* no locking support */
        0;
    ext->mdcm_cfg = \
        (0u << MMSC_CFG_DSET) |         /* 64 sets */
        (3u << MMSC_CFG_DWAY) |         /*  4 ways */
        (3u << MMSC_CFG_DSZ) |          /* 32 bytes per line */
        (0u << MMSC_CFG_DLCK) |         /* no locking support */
        0;
    ext->mmsc_cfg = \
        (1u << MMSC_CFG_ECD) |
        (1u << MMSC_CFG_VPLIC) |
        (1u << MMSC_CFG_EV5PE) |
        (1u << MMSC_CFG_PMNDS) |
        (1u << MMSC_CFG_CCTLCSR) |
        (1u << MMSC_CFG_EFHW) |
        0;

    andes_riscv_csrif_init(env);
    andes_riscv_isaif_init(env);
}

#elif defined(TARGET_RISCV64)

static void rv64gcsux_andes_ax25_priv1_10_0_cpu_init(Object *obj)
{
    CPUState* cs = CPU(obj);
    CPUClass* cc = CPU_GET_CLASS(cs);
    CPURISCVState *env = &RISCV_CPU(obj)->env;
    set_misa(env, RV64 | RVI | RVM | RVA | RVF | RVD | RVC | RVS | RVU | RVX);
    set_versions(env, USER_VERSION_2_02_0, PRIV_VERSION_1_10_0);
    set_resetvec(env, ANDES_AX25_DEFAULT_RSTVEC);
    set_feature(env, RISCV_FEATURE_MMU);

    CPURVAndesExt *ext = g_new0(CPURVAndesExt, 1);
    env->ext = ext;
#ifndef CONFIG_USER_ONLY
    env->mstatus = (0x3 << 11); /* MPP=M */
#endif
    ext->micm_cfg = \
        (0u << MMSC_CFG_ISET) |         /* 64 sets */
        (3u << MMSC_CFG_IWAY) |         /*  4 ways */
        (3u << MMSC_CFG_ISZ) |          /* 32 bytes per line */
        (0u << MMSC_CFG_ILCK) |         /* no locking support */
        0;
    ext->mdcm_cfg = \
        (0u << MMSC_CFG_DSET) |         /* 64 sets */
        (3u << MMSC_CFG_DWAY) |         /*  4 ways */
        (3u << MMSC_CFG_DSZ) |          /* 32 bytes per line */
        (0u << MMSC_CFG_DLCK) |         /* no locking support */
        0;
    ext->mmsc_cfg = \
        (1u << MMSC_CFG_ECD) |
        (1u << MMSC_CFG_VPLIC) |
        (1u << MMSC_CFG_EV5PE) |
        (1u << MMSC_CFG_PMNDS) |
        (1u << MMSC_CFG_CCTLCSR) |
        (1u << MMSC_CFG_EFHW) |
        0;

    andes_riscv_csrif_init(env);
    andes_riscv_isaif_init(env);
    /* intif */
    cc->cpu_exec_interrupt = andes_riscv_cpu_exec_interrupt;
    cc->do_interrupt = andes_riscv_cpu_do_interrupt;
}

static void rv64gcsux_andes_priv1_10_0_cpu_init(Object *obj)
{
    CPURISCVState *env = &RISCV_CPU(obj)->env;
    set_misa(env, RV64 | RVI | RVM | RVA | RVF | RVD | RVC | RVS | RVU | RVX);
    set_versions(env, USER_VERSION_2_02_0, PRIV_VERSION_1_10_0);
    set_resetvec(env, ANDES_NX25_DEFAULT_RSTVEC);

    CPURVAndesExt *ext = g_new0(CPURVAndesExt, 1);
    env->ext = ext;
#ifndef CONFIG_USER_ONLY
    env->mstatus = (0x3 << 11); /* MPP=M */
#endif
    ext->micm_cfg = \
        (0u << MMSC_CFG_ISET) |         /* 64 sets */
        (3u << MMSC_CFG_IWAY) |         /*  4 ways */
        (3u << MMSC_CFG_ISZ) |          /* 32 bytes per line */
        (0u << MMSC_CFG_ILCK) |         /* no locking support */
        0;
    ext->mdcm_cfg = \
        (0u << MMSC_CFG_DSET) |         /* 64 sets */
        (3u << MMSC_CFG_DWAY) |         /*  4 ways */
        (3u << MMSC_CFG_DSZ) |          /* 32 bytes per line */
        (0u << MMSC_CFG_DLCK) |         /* no locking support */
        0;
    ext->mmsc_cfg = \
        (1u << MMSC_CFG_ECD) |
        (1u << MMSC_CFG_VPLIC) |
        (1u << MMSC_CFG_EV5PE) |
        (1u << MMSC_CFG_PMNDS) |
        (1u << MMSC_CFG_CCTLCSR) |
        (1u << MMSC_CFG_EFHW) |
        0;

    andes_riscv_csrif_init(env);
    andes_riscv_isaif_init(env);
}

#endif
