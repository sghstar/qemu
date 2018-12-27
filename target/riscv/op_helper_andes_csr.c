/*
 * Andes RISC-V CSR Extensions Emulation Helpers for QEMU.
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

/* #define DEBUG_ANDES_CSR */
#define OK      (0)
#define NG      (1)
#define NO      (0)
#define YES     (1)
#define EXCP    (2)

enum andes_csr_name {
    /* V5 machine mode CSRs */
    CSR_MICM_CFG    = 0Xfc0,
    CSR_MDCM_CFG    = 0xfc1,
    CSR_MMSC_CFG    = 0xfc2,
    CSR_MILMB       = 0x7c0,
    CSR_MDLMB       = 0x7c1,
    CSR_MECC_CODE   = 0x7c2,
    CSR_MNVEC       = 0x7c3,
    CSR_MCACHE_CTL  = 0x7ca,
    CSR_MHSP_CTL    = 0x7c6,
    CSR_MSP_COUND   = 0x7c7,
    CSR_MSP_BASE    = 0x7c8,
    CSR_MXSTATUS    = 0x7c4,
    CSR_MDCAUSE     = 0x7c9,
    CSR_MPFT_CTL    = 0x7c5,
    CSR_MMISC_CTL   = 0x7d0,
    CSR_MSLIDELEG   = 0x7d5,

    /* trigger CSRs */
    // CSR_TSELECT  = 0x7a0,
    // CSR_TDATA1   = 0x7a1,
    // CSR_TDATA2   = 0x7a2,
    // CSR_TDATA3   = 0x7a3,
    CSR_TINFO    = 0x7a4,
    CSR_TCONTROL = 0x7a5,
    CSR_MCONTEXT = 0x7a8,
    CSR_SCONTEXT = 0x7aa,
    CSR_MCONTROL = CSR_TDATA1,
    CSR_ICOUNT   = CSR_TDATA1,
    CSR_ITRIGGER = CSR_TDATA1,
    CSR_ETRIGGER = CSR_TDATA1,
    CSR_TEXTRA32 = CSR_TDATA3,
    CSR_TEXTRA64 = CSR_TDATA3,

    CSR_MCCTLBEGINADDR = 0x7cb,
    CSR_MCCTLCOMMAND   = 0x7cc,
    CSR_MCCTLDATA      = 0x7cd,

    CSR_MCOUNTERWEN     = 0X7ce,
    CSR_MCOUNTERMASK_M  = 0X7d1,
    CSR_MCOUNTERMASK_S  = 0X7d2,
    CSR_MCOUNTERMASK_U  = 0X7d3,
    CSR_MCOUNTERINTEN   = 0X7cf,
    CSR_MCOUNTEROVF     = 0X7d4,

    /* internal machine mode CSRs */
    CSR_MRANDESQ    = 0x7fc,
    CSR_MRANDSEQH   = 0x7fd,
    CSR_MRANDSTATE  = 0x7fe,
    CSR_MRANDSTATEH = 0x7ff,

    /* debug mode CSRs */
    CSR_DEXC2DBG    = 0x7e0,
    CSR_DDCAUSE     = 0x7e1,

    /* supervisor mode CSRs */
    CSR_SLIE            = 0x9c4,
    CSR_SLIP            = 0x9c5,
    CSR_SCCTLDATA       = 0x9cd,
    CSR_SCOUNTERINTEN   = 0X9cf,
    CSR_SCOUNTERMASK_S  = 0X9d2,
    CSR_SCOUNTERMASK_U  = 0X9d3,
    CSR_SCOUNTEROVF     = 0X9d4,

    /* user mode CSRs */
    CSR_UITB            = 0x800,
    CSR_UCODE           = 0x801,
    CSR_LOOPB           = 0x802,
    CSR_LOOPE           = 0x803,
    CSR_LOOPC           = 0x804,
    CSR_UCCTLBEGINADDR  = 0x80b,
    CSR_UCCTLCOMMAND    = 0x80c,

    /* misc */
    CSR_MHPMEVENT0      = 0x320,
    CSR_MHPMCOUNTER0    = 0xb00,
    CSR_MHPMCOUNTER0H   = 0xb80,
    CSR_MTIME           = 0xb01,
    CSR_HPMCOUNTER0     = 0xc00,
    CSR_HPMCOUNTER0H    = 0xc80,
};

enum andes_cctl_command {
    L1D_VA_INVAL = 0,
    L1D_VA_WB,
    L1D_VA_WBINVAL,
    L1D_VA_LOCK,
    L1D_VA_UNLOCK = 4,
    L1D_WBINVAL_ALL = 6,
    L1D_WB_ALL,
    L1I_VA_INVAL = 8,
    L1I_VA_LOCK = 11,
    L1I_VA_UNLOCK = 12,
    L1D_IX_INVAL = 16,
    L1D_IX_WB,
    L1D_IX_WBINVAL,
    L1D_IX_RTAG,
    L1D_IX_RDATA = 20,
    L1D_IX_WTAG,
    L1D_IX_WDATA,
    L1D_INVAL_ALL,
    L1I_IX_INVAL = 24,
    L1I_IX_RTAG = 27,
    L1I_IX_RDATA = 28,
    L1I_IX_WTAG,
    L1I_IX_WDATA,
};

void riscv_set_local_interrupt_smode(RISCVCPU *cpu, target_ulong mask, int value);

/* iothread_mutex must be held */
void riscv_set_local_interrupt_smode(RISCVCPU *cpu, target_ulong mask, int value)
{
    CPURVAndesExt *ext = cpu->env.ext;
    target_ulong old_slip = ext->slip;
    ext->slip = (old_slip & ~mask) | (value ? mask : 0);

    if (ext->slip && !old_slip) {
        cpu_interrupt(CPU(cpu), CPU_INTERRUPT_HARD);
    } else if (!ext->slip && old_slip) {
        cpu_reset_interrupt(CPU(cpu), CPU_INTERRUPT_HARD);
    }
}

static int do_cctl_command(CPURISCVState *env, target_ulong cmd, int mode)
{
#ifndef CONFIG_USER_ONLY
    CPUState *cs = CPU(riscv_env_get_cpu(env));
#if 0
    CPURVAndesExt *ext = env->ext;
    target_ulong va;
    hwaddr pa;
#endif

    if (mode > env->priv) {
        do_raise_exception_err(env, RISCV_EXCP_ILLEGAL_INST, GETPC());
        return NG;
    }

    switch(cmd) {
    case L1D_VA_INVAL:
        break;
    case L1D_VA_WB:
        break;
    case L1D_VA_WBINVAL:
        break;
    case L1D_VA_LOCK:
        break;
    case L1D_VA_UNLOCK:
        break;
    case L1D_WBINVAL_ALL:
        break;
    case L1D_WB_ALL:
        break;
    case L1I_VA_INVAL:
        /* invalidate jit code cache */
#if 1
        /* TODO: flush TBs within VA range instead of all */
        tb_flush(cs);
# else
        if (mode == PRV_M) {
            va = ext->mcctlbeginaddr;
        } else if (mode == PRV_U) {
            va = ext->ucctlbeginaddr;
        } else {
            do_raise_exception_err(env, RISCV_EXCP_ILLEGAL_INST, GETPC());
            return NG;
        }
        pa = riscv_cpu_get_phys_page_debug(cs, va);
        tb_invalidate_phys_range(pa, pa + 1);
        tb_invalidate_phys_addr(NULL, pa, NULL);
#endif
        break;
    case L1I_VA_LOCK:
        break;
    case L1I_VA_UNLOCK:
        break;
    case L1D_IX_INVAL:
        break;
    case L1D_IX_WB:
        break;
    case L1D_IX_WBINVAL:
        break;
    case L1D_IX_RTAG:
        break;
    case L1D_IX_RDATA:
        break;
    case L1D_IX_WTAG:
        break;
    case L1D_IX_WDATA:
        break;
    case L1D_INVAL_ALL:
        break;
    case L1I_IX_INVAL:
        /* invalidate jit code cache */
        /* TODO: flush TBs within INDEX range instead of all */
        tb_flush(cs);
        break;
    case L1I_IX_RTAG:
        break;
    case L1I_IX_RDATA:
        break;
    case L1I_IX_WTAG:
        break;
    case L1I_IX_WDATA:
        /* TODO */
        assert(0);
        break;
    default:
        qemu_log("== %s: Unknow command %08lx ==\n", __func__, (long)cmd);
    }
#endif
    return 0;
}

#if 0
static inline target_ulong read_pmnds_csr(CPURISCVState *env, target_ulong csrno)
{
    int counteridx = csrno - CSR_HPMCOUNTER0;
    CPURVAndesExt *ext = env->ext;
#if defined(TARGET_RISCV32)
    uint64_t counter = ext->mhpmcounter[counteridx] | ((uint64_t)ext->mhpmcounterh[counteridx] << 32);
#else
    uint64_t counter = ext->mhpmcounter[counteridx];
#endif
    return counter;
}

static inline target_ulong read_pmnds_csr_mode(CPURISCVState *env, target_ulong csrno)
{
    int counteridx = csrno - CSR_MHPMCOUNTER0;
    CPURVAndesExt *ext = env->ext;
#if defined(TARGET_RISCV32)
    uint64_t counter = ext->mhpmcounter[counteridx] | ((uint64_t)ext->mhpmcounterh[counteridx] << 32);
#else
    uint64_t counter = ext->mhpmcounter[counteridx];
#endif
    return counter;
}
#endif

static void andes_riscv_csr_validate_helper(CPURISCVState *env, uint64_t which,
                                            uint64_t write, uintptr_t ra, int *next)
{
#ifndef CONFIG_USER_ONLY
    CPURVAndesExt *ext = env->ext;
    int is_handled = 0;
    unsigned csr_priv = get_field((which), 0x300);
    unsigned csr_read_only = get_field((which), 0xC00) == 3;
    if (write && csr_read_only && ext->mcounterwen && (env->priv >= csr_priv)) {
        if ((which >= CSR_CYCLE) && (which <= CSR_HPMCOUNTER31) && (env->priv >= (riscv_has_ext(env, RVS) ? PRV_S : PRV_U))) {
            is_handled = (ext->mcounterwen >> (which & 0x1f)) & 1u;
        }
    }
    *next = is_handled ? 0 : 1;
#endif
}

target_ulong andes_riscv_csr_read_helper(CPURISCVState *env, target_ulong csrno, int *next)
{
    int cont = NO;
    CPURVAndesExt *ext = env->ext;
    target_ulong csr;

    if (next) {
        *next = OK; /* presume */
    }

    /* counter CSRs */
#ifndef CONFIG_USER_ONLY
    target_ulong ctr_en = env->priv == PRV_U ? env->scounteren :
                          env->priv == PRV_S ? env->mcounteren : -1U;
#else
    target_ulong ctr_en = -1;
#endif
    target_ulong ctr_ok = (ctr_en >> (csrno & 31)) & 1;

    if ((csrno >= CSR_HPMCOUNTER3 && csrno <= CSR_HPMCOUNTER31) && (ctr_ok)) {
        return (csrno > CSR_HPMCOUNTER6) ? 0 : ext->mhpmcounter[csrno - CSR_HPMCOUNTER0];
    }
#if defined(TARGET_RISCV32)
    if ((csrno >= CSR_HPMCOUNTER3H && csrno <= CSR_HPMCOUNTER31H) && (ctr_ok)) {
        return (csrno > CSR_HPMCOUNTER6H) ? 0 : ext->mhpmcounterh[csrno - CSR_HPMCOUNTER0H];
    }
#endif

    if (csrno >= CSR_MHPMCOUNTER3 && csrno <= CSR_MHPMCOUNTER31) {
        return (csrno > CSR_MHPMCOUNTER6) ? 0 : ext->mhpmcounter[csrno - CSR_MHPMCOUNTER0];
    }
#if defined(TARGET_RISCV32)
    if (csrno >= CSR_MHPMCOUNTER3H && csrno <= CSR_MHPMCOUNTER31H) {
        return (csrno > CSR_MHPMCOUNTER6H) ? 0 : ext->mhpmcounterh[csrno - CSR_MHPMCOUNTER0H];
    }
#endif

    if (csrno >= CSR_MHPMEVENT3 && csrno <= CSR_MHPMEVENT31) {
        return ext->mhpmevent[csrno - CSR_MHPMEVENT0];
    }

    /* other CSRs */
    switch (csrno) {
#if 0
    case CSR_CYCLE:
    case CSR_INSTRET:
        cont = YES;
        break;
#endif
    case CSR_MICM_CFG: /* MRO */
        csr = ext->micm_cfg;
        break;
    case CSR_MDCM_CFG: /* MRO */
        csr = ext->mdcm_cfg;
        break;
    case CSR_MMSC_CFG: /* MRO */
        csr = ext->mmsc_cfg;
        break;
    case CSR_MILMB:
        csr = ext->milmb;
        break;
    case CSR_MDLMB:
        csr = ext->mdlmb;
        break;
    case CSR_MECC_CODE:
        csr = ext->mecc_code;
        break;
    case CSR_MNVEC:
        csr = ext->mnvec;
        break;
    case CSR_MCACHE_CTL:
        csr = ext->mcache_ctl;
        break;
    case CSR_MHSP_CTL:
        csr = ext->mhsp_ctl;
        break;
    case CSR_MSP_COUND:
        csr = ext->msp_bound;
        break;
    case CSR_MSP_BASE:
        csr = ext->msp_base;
        break;
    case CSR_MXSTATUS:
        csr = ext->mxstatus;
        break;
    case CSR_MDCAUSE:
        csr = ext->mdcause;
        break;
    case CSR_MPFT_CTL:
        csr = ext->mpft_ctl;
        break;
    case CSR_MMISC_CTL:
        csr = ext->mmisc_ctl;
        break;
    case CSR_MRANDESQ:
        csr = ext->mrandseq;
        break;
    case CSR_MRANDSEQH:
        csr = ext->mrandseqh;
        break;
    case CSR_MRANDSTATE:
        csr = ext->mrandstate;
        break;
    case CSR_MRANDSTATEH:
        csr = ext->mrandstateh;
        break;
    case CSR_DEXC2DBG:
        csr = ext->dexc2dbg;
        break;
    case CSR_DDCAUSE:
        csr = ext->ddcause;
        break;
    case CSR_UITB:
        csr = ext->uitb;
        break;
    case CSR_UCODE:
        csr = ext->ucode;
        break;
    case CSR_LOOPB:
        csr = ext->loopb;
        break;
    case CSR_LOOPE:
        csr = ext->loope;
        break;
    case CSR_LOOPC:
        csr = ext->loopc;
        break;
    case CSR_MCCTLBEGINADDR:
        if (ext->mmsc_cfg & (1u << MMSC_CFG_CCTLCSR)) {
            csr = ext->mcctlbeginaddr;
        } else {
            cont = YES;
        }
        break;
    case CSR_MCCTLCOMMAND:
        if (ext->mmsc_cfg & (1u << MMSC_CFG_CCTLCSR)) {
            csr = ext->mcctlcommand;
        } else {
            cont = YES;
        }
        break;
    case CSR_MCCTLDATA:
        if (ext->mmsc_cfg & (1u << MMSC_CFG_CCTLCSR)) {
            csr = ext->mcctldata;
        } else {
            cont = YES;
        }
        break;
    case CSR_SCCTLDATA:
        if (ext->mmsc_cfg & (1u << MMSC_CFG_CCTLCSR) && riscv_has_ext(env, RVS)) {
            csr = ext->scctldata;
        } else {
            cont = YES;
        }
        break;
    case CSR_UCCTLBEGINADDR:
        if (ext->mmsc_cfg & (1u << MMSC_CFG_CCTLCSR) && riscv_has_ext(env, RVU)) {
            csr = ext->ucctlbeginaddr;
        } else {
            cont = YES;
        }
        break;
    case CSR_UCCTLCOMMAND:
        if (ext->mmsc_cfg & (1u << MMSC_CFG_CCTLCSR) && riscv_has_ext(env, RVU)) {
            csr = ext->ucctlcommand;
        } else {
            cont = YES;
        }
        break;
    case CSR_MCOUNTERWEN:
        if (ext->mmsc_cfg & (1u << MMSC_CFG_PMNDS)) {
            csr = ext->mcounterwen;
        } else {
            cont = YES;
        }
        break;
    case CSR_MCOUNTERMASK_M:
        if (ext->mmsc_cfg & (1u << MMSC_CFG_PMNDS)) {
            csr = ext->mcountermask_m;
        } else {
            cont = YES;
        }
        break;
    case CSR_MCOUNTERMASK_S:
    case CSR_SCOUNTERMASK_S:
        if (ext->mmsc_cfg & (1u << MMSC_CFG_PMNDS)) {
            csr = ext->mcountermask_s;
        } else {
            cont = YES;
        }
        break;
    case CSR_MCOUNTERMASK_U:
    case CSR_SCOUNTERMASK_U:
        if (ext->mmsc_cfg & (1u << MMSC_CFG_PMNDS)) {
            csr = ext->mcountermask_u;
        } else {
            cont = YES;
        }
        break;
    case CSR_MCOUNTERINTEN:
    case CSR_SCOUNTERINTEN:
        if (ext->mmsc_cfg & (1u << MMSC_CFG_PMNDS)) {
            csr = ext->mcounterinten;
        } else {
            cont = YES;
        }
        break;
    case CSR_MCOUNTEROVF:
    case CSR_SCOUNTEROVF:
        if (ext->mmsc_cfg & (1u << MMSC_CFG_PMNDS)) {
            csr = ext->mcounterovf;
        } else {
            cont = YES;
        }
        break;
    case CSR_TSELECT:
        csr = ext->tselect;
        break;
    case CSR_TDATA1: /* CSR_MCONTROL, CSR_ICOUNT, CSR_ITRIGGER, or CSR_ETRIGGER */
        csr = ext->tdata1;
        break;
    case CSR_TDATA2:
        csr = ext->tdata2;
        break;
    case CSR_TDATA3: /* CSR_TEXTRA32 (RV32), or CSR_TEXTRA64 (RV64) */
        csr = ext->tdata3;
        break;
    case CSR_TINFO:
        // csr = ext->tinfo;
        csr = 1; /* does not exist */
        break;
    case CSR_TCONTROL:
        csr = ext->tcontrol;
        break;
    case CSR_MCONTEXT:
        csr = ext->mcontext;
        break;
    case CSR_SCONTEXT:
        csr = ext->scontext;
        break;
    case CSR_MSLIDELEG:
        if (riscv_has_ext(env, RVS)) {
            const target_ulong mask = (7u << 16);
            csr = ext->mslideleg & mask;
        } else {
            cont = YES;
        }
        break;
    case CSR_SLIE:
        if (riscv_has_ext(env, RVS)) {
#ifndef CONFIG_USER_ONLY
            target_ulong mask = (env->priv == PRV_M) ? (7u << 16) : ext->mslideleg;
            csr = ext->slie & mask;
#endif
        } else {
            cont = YES;
        }
        break;
    case CSR_SLIP:
        if (riscv_has_ext(env, RVS)) {
#ifndef CONFIG_USER_ONLY
            target_ulong mask = (env->priv == PRV_M) ? (7u << 16) : ext->mslideleg;
            csr = ext->slip & mask;
#endif
        } else {
            cont = YES;
        }
        break;
    default:
        cont = YES;
    }

    if (next) {
        *next = cont ? NG : OK;
    }

#ifdef DEBUG_ANDES_CSR
    if (cont) {
        csr = 0xdeadbeef; /* debug only! */
    } else {
        qemu_log("== %s: CSR %03lx V %08lx ==\n", __func__, (long)csrno, (long)csr);
    }
#endif

    return csr;
}

static inline void
update_pmnds_interrupt(CPURISCVState *env)
{
#ifndef CONFIG_USER_ONLY
    RISCVCPU *cpu = riscv_env_get_cpu(env);
    CPURVAndesExt *ext = env->ext;
    target_ulong irq = ext->mcounterovf & ext->mcounterinten;
    target_ulong irq_m = irq & ~ext->mcountermask_m;
    target_ulong irq_s = irq & ext->mcountermask_m;
    riscv_cpu_update_mip(cpu, MIP_PMOVI, BOOL_TO_MASK(!!irq_m));
    riscv_set_local_interrupt_smode(cpu, MIP_PMOVI, !!irq_s);
#endif
}

#ifndef CONFIG_USER_ONLY
static QEMUTimer *pmnds_timer = NULL; /* Internal timer PMNDS */
static int activated_event_count = 0;
#endif

static inline uint64_t get_pmnds_counter(CPURVAndesExt* ext, int index)
{
#if defined(TARGET_RISCV32)
    uint64_t counter = ext->mhpmcounter[index];
    counter |= ((uint64_t)ext->mhpmcounterh[index] << 32);
#else
    uint64_t counter = ext->mhpmcounter[index];
#endif
    return counter;
}

static inline void set_pmnds_counter(CPURVAndesExt* ext, int index, uint64_t counter)
{
#if defined(TARGET_RISCV32)
    ext->mhpmcounter[index] = (target_ulong)counter;
    ext->mhpmcounterh[index] = counter >> 32;
#else
    ext->mhpmcounter[index] = counter;
#endif
}

#ifndef CONFIG_USER_ONLY
static void pmnds_timer_cb(void *opaque)
{
    // printf("%s:\n", __func__);
    CPURISCVState* env = opaque;
    CPURVAndesExt* ext = env->ext;
    int i, is_ov = 0;
    uint64_t klock = qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL);
    /* schedule next tick */
    if (activated_event_count > 0) {
        uint64_t next = klock + NANOSECONDS_PER_SECOND / 10; /* 10Hz */
        timer_mod(pmnds_timer, next);
    }
    /* update counters */
    for (i = 0; i < 7; ++i) {
        if (i == 1) { continue; } /* skip MTIME */
        if ((i < 3) || (ext->mhpmevent[i])) {
            uint64_t delta, counter, next;
            uint32_t ratio, mask;
            if (klock > ext->hpmcounter_mark[i]) {
                delta = klock - ext->hpmcounter_mark[i];
            } else {
                /* wrap */
                delta = klock + ~ext->hpmcounter_mark[i] + 1;
            }

            /* counter mask simulation */
            ratio = 100;
            mask = (1u << i);
            if (ext->mcountermask_m & mask) { ratio -= 1; }
            if (ext->mcountermask_s & mask) { ratio -= 19; }
            if (ext->mcountermask_u & mask) { ratio -= 80; }
            delta = delta * ratio / 100;

            counter = get_pmnds_counter(ext, i);
            next = counter + delta;
            if (next > counter) {
                counter = next;
            } else {
                /* overflow */
                counter = next + ~counter + 1;
                ext->mcounterovf |= (1u << i);
                ++is_ov;
            }
            ext->hpmcounter_mark[i] = klock;

            set_pmnds_counter(ext, i, counter);
        }
    }

    if (is_ov) {
        update_pmnds_interrupt(env);
    }
}
#endif

static inline void
update_pmnds_event(CPURISCVState *env, target_ulong value, target_ulong csrno)
{
#ifndef CONFIG_USER_ONLY
    int hpmidx = csrno - CSR_MHPMEVENT0;
    // printf("%s: hpmidx %d = 0x%08lx\n", __func__, hpmidx, (long)value);
    CPURVAndesExt* ext = env->ext;
    target_ulong previous = ext->mhpmevent[hpmidx];
    ext->mhpmevent[hpmidx] = value;
    int inc = 0;
    /* the simulation is simpplified as following:
     *   # the count will update on next 'tick' ignoring the imprecise
     */
    if (previous) {
        --inc;
        /* finish the previous event */
    }
    if (value) {
        ++inc;
        /* FEEDME: initialize timer here, any better place? */
        if (!pmnds_timer) {
            pmnds_timer = timer_new_ns(QEMU_CLOCK_VIRTUAL, &pmnds_timer_cb, env);
        }
        /* initialize the new start event */
    }
    activated_event_count += inc;
    assert(activated_event_count >= 0 && "activated_event_count >= 0");
    if (inc == 1) {
        uint64_t klock = qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL);
        ext->hpmcounter_mark[hpmidx] = klock;
        if (activated_event_count == 1) {
            uint64_t next = klock + NANOSECONDS_PER_SECOND / 10; /* 10Hz */
            timer_mod(pmnds_timer, next);
        }
    }
#endif
}

static inline int
write_pmnds_csr(CPURISCVState *env, target_ulong value, target_ulong csrno)
{
    int cont = NO;
    CPURVAndesExt *ext = env->ext;

#ifndef CONFIG_USER_ONLY
    target_ulong ctr_en = env->priv == PRV_U ? env->scounteren :
                          env->priv == PRV_S ? env->mcounteren : -1U;
#else
    target_ulong ctr_en = -1;
#endif

    target_ulong ctr_ok = (ctr_en >> (csrno & 31)) & 1;
    ctr_ok &= (ext->mcounterwen >> (csrno & 31)) & 1;

    do {
        if ((csrno == CSR_TIME) || (csrno == CSR_MTIME)) {
            cont = YES;
            break;
        }

        if ((csrno >= CSR_CYCLE) && (csrno <= CSR_HPMCOUNTER31)) {
            if (ctr_ok) {
                ext->mhpmcounter[csrno - CSR_CYCLE] = value;
            } else {
                cont = EXCP;
            }
            break;
        }
#if defined(TARGET_RISCV32)
        if ((csrno >= CSR_CYCLEH) && (csrno <= CSR_HPMCOUNTER31H)) {
            if (ctr_ok) {
                ext->mhpmcounterh[csrno - CSR_CYCLEH] = value;
            } else {
                cont = EXCP;
            }
            break;
        }
#endif

        if ((csrno >= CSR_MCYCLE) && (csrno <= CSR_MHPMCOUNTER31)) {
            ext->mhpmcounter[csrno - CSR_MCYCLE] = value;
            break;
        }
#if defined(TARGET_RISCV32)
        if ((csrno >= CSR_MCYCLEH) && (csrno <= CSR_MHPMCOUNTER31H)) {
            ext->mhpmcounterh[csrno - CSR_MCYCLEH] = value;
            break;
        }
#endif

        if ((csrno >= CSR_MHPMEVENT3) && (csrno <= CSR_MHPMEVENT31)) {
            if (csrno <= CSR_MHPMEVENT6) {
                update_pmnds_event(env, value, csrno);
            }
            break;
        }

        cont = YES;
    } while (0);

    if (cont == EXCP) {
        cont = NO;
        do_raise_exception_err(env, RISCV_EXCP_ILLEGAL_INST, GETPC());
    }

    return cont;
}

static inline int
write_other_csr(CPURISCVState *env, target_ulong value, target_ulong csrno)
{
    int cont = NO;
    CPURVAndesExt *ext = env->ext;

#ifndef CONFIG_USER_ONLY
    uint64_t delegable_ints = MIP_SSIP | MIP_STIP | MIP_SEIP;
    uint64_t all_ints = delegable_ints | MIP_MSIP | MIP_MTIP | MIP_MEIP | MIP_PMOVI;
#endif

    switch (csrno) {
#ifndef CONFIG_USER_ONLY
    case CSR_MIE: {
        env->mie = (env->mie & ~all_ints) | (value & all_ints);
        break;
    }
#endif
    case CSR_MICM_CFG: /* MRO */
    case CSR_MDCM_CFG: /* MRO */
    case CSR_MMSC_CFG: /* MRO */
        break;
    case CSR_MILMB:
        ext->milmb = value;
        break;
    case CSR_MDLMB:
        ext->mdlmb = value;
        break;
    case CSR_MECC_CODE:
        ext->mecc_code = value;
        break;
    case CSR_MNVEC:
        ext->mnvec = value;
        break;
    case CSR_MCACHE_CTL:
        ext->mcache_ctl = value;
        /* TODO: effects */
        break;
    case CSR_MHSP_CTL:
        ext->mhsp_ctl = value;
        /* TODO: effects */
        break;
    case CSR_MSP_COUND:
        ext->msp_bound = value;
        break;
    case CSR_MSP_BASE:
        ext->msp_base = value;
        break;
    case CSR_MXSTATUS:
        ext->mxstatus = value;
        break;
    case CSR_MDCAUSE:
        ext->mdcause = value;
        break;
    case CSR_MPFT_CTL:
        ext->mpft_ctl = value;
        /* TODO: effects */
        break;
    case CSR_MMISC_CTL:
        ext->mmisc_ctl = value;
        /* TODO: effects */
        break;
    case CSR_MRANDESQ:
        ext->mrandseq = value;
        break;
    case CSR_MRANDSEQH:
        ext->mrandseqh = value;
        break;
    case CSR_MRANDSTATE:
        ext->mrandstate = value;
        break;
    case CSR_MRANDSTATEH:
        ext->mrandstateh = value;
        break;
    case CSR_DEXC2DBG:
        ext->dexc2dbg = value;
        /* TODO: effects */
        break;
    case CSR_DDCAUSE:
        ext->ddcause = value;
        break;
    case CSR_UITB:
        ext->uitb = (ext->uitb & 0x1) | (value & ~0x3);
        break;
    case CSR_UCODE:
        ext->ucode = value;
        break;
    case CSR_LOOPB:
        ext->loopb = value;
        break;
    case CSR_LOOPE:
        ext->loope = value;
        break;
    case CSR_LOOPC:
        ext->loopc = value;
        break;
    case CSR_MCCTLBEGINADDR:
        if (ext->mmsc_cfg & (1u << MMSC_CFG_CCTLCSR)) {
            ext->mcctlbeginaddr = value;
        } else {
            cont = YES;
        }
        break;
    case CSR_MCCTLCOMMAND:
        if (ext->mmsc_cfg & (1u << MMSC_CFG_CCTLCSR)) {
            do_cctl_command(env, value, PRV_M);
            ext->mcctlcommand = value;
        } else {
            cont = YES;
        }
        break;
    case CSR_MCCTLDATA:
        if (ext->mmsc_cfg & (1u << MMSC_CFG_CCTLCSR)) {
            ext->mcctldata = value;
        } else {
            cont = YES;
        }
        break;
    case CSR_SCCTLDATA:
        if (ext->mmsc_cfg & (1u << MMSC_CFG_CCTLCSR) && riscv_has_ext(env, RVS)) {
            ext->scctldata = value;
        } else {
            cont = YES;
        }
        break;
    case CSR_UCCTLBEGINADDR:
        if (ext->mmsc_cfg & (1u << MMSC_CFG_CCTLCSR) && riscv_has_ext(env, RVU)) {
            ext->ucctlbeginaddr = value;
        } else {
            cont = YES;
        }
        break;
    case CSR_UCCTLCOMMAND:
        if (ext->mmsc_cfg & (1u << MMSC_CFG_CCTLCSR) && riscv_has_ext(env, RVU)) {
            do_cctl_command(env, value, PRV_U);
            ext->ucctlcommand = value;
        } else {
            cont = YES;
        }
        break;
    case CSR_MCOUNTERWEN:
        if (ext->mmsc_cfg & (1u << MMSC_CFG_PMNDS)) {
            ext->mcounterwen = value;
        } else {
            cont = YES;
        }
        break;
    case CSR_MCOUNTERMASK_M:
        if (ext->mmsc_cfg & (1u << MMSC_CFG_PMNDS)) {
            ext->mcountermask_m = value;
        } else {
            cont = YES;
        }
        break;
    case CSR_SCOUNTERMASK_S:
        value = (ext->mcountermask_s & ~ext->mcounterwen) | (value & ext->mcounterwen);
        /* fall through */
    case CSR_MCOUNTERMASK_S:
        if (ext->mmsc_cfg & (1u << MMSC_CFG_PMNDS)) {
            ext->mcountermask_s = value;
        } else {
            cont = YES;
        }
        break;
    case CSR_SCOUNTERMASK_U:
        value = (ext->mcountermask_u & ~ext->mcounterwen) | (value & ext->mcounterwen);
        /* fall through */
    case CSR_MCOUNTERMASK_U:
        if (ext->mmsc_cfg & (1u << MMSC_CFG_PMNDS)) {
            ext->mcountermask_u = value;
        } else {
            cont = YES;
        }
        break;
    case CSR_SCOUNTERINTEN:
        value = (ext->mcounterinten & ~ext->mcounterwen) | (value & ext->mcounterwen);
        /* fall through */
    case CSR_MCOUNTERINTEN:
        if (ext->mmsc_cfg & (1u << MMSC_CFG_PMNDS)) {
            ext->mcounterinten = value;
            update_pmnds_interrupt(env);
        } else {
            cont = YES;
        }
        break;
    case CSR_SCOUNTEROVF:
        value &= ext->mcounterwen;
        /* fall through */
    case CSR_MCOUNTEROVF: /* W1C */
        if (ext->mmsc_cfg & (1u << MMSC_CFG_PMNDS)) {
            ext->mcounterovf &= ~value;
            update_pmnds_interrupt(env);
        } else {
            cont = YES;
        }
        break;
    case CSR_TSELECT:
        // ext->tselect = value;
        break;
    case CSR_TDATA1: /* CSR_MCONTROL, CSR_ICOUNT, CSR_ITRIGGER, or CSR_ETRIGGER */
        ext->tdata1 = value;
        break;
    case CSR_TDATA2:
        ext->tdata2 = value;
        break;
    case CSR_TDATA3: /* CSR_TEXTRA32 (RV32), or CSR_TEXTRA64 (RV64) */
        ext->tdata3 = value;
        break;
    case CSR_TINFO:
        // ext->tinfo = value;
        break;
    case CSR_TCONTROL:
        ext->tcontrol = value;
        break;
    case CSR_MCONTEXT:
        ext->mcontext = value;
        break;
    case CSR_SCONTEXT:
        ext->scontext = value;
        break;
    case CSR_MSLIDELEG:
        if (riscv_has_ext(env, RVS)) {
            const target_ulong mask = (7u << 16);
            ext->mslideleg = (ext->mslideleg & ~mask) | (value & mask);
        } else {
            cont = YES;
        }
        break;
    case CSR_SLIE:
        if (riscv_has_ext(env, RVS)) {
#ifndef CONFIG_USER_ONLY
            target_ulong mask = (env->priv == PRV_M) ? (7u << 16) : ext->mslideleg;
            ext->slie = (ext->slie & ~mask) | (value & mask);
#endif
        } else {
            cont = YES;
        }
        break;
    case CSR_SLIP:
        if (riscv_has_ext(env, RVS)) {
#ifndef CONFIG_USER_ONLY
            target_ulong mask = (env->priv == PRV_M) ? (7u << 16) : ext->mslideleg;
            ext->slip = (ext->slip & ~mask) | (value & mask);
#endif
        } else {
            cont = YES;
        }
        break;
    default:
        cont = YES;
    }

    return cont;
}

void andes_riscv_csr_write_helper(CPURISCVState *env, target_ulong value, target_ulong csrno, int *next)
{
    int cont = NO;
    CPURVAndesExt *ext = env->ext;

    if (extract32(ext->mmsc_cfg, MMSC_CFG_PMNDS, 1)) {
        cont = write_pmnds_csr(env, value, csrno);
    }

    if (cont) {
        cont = write_other_csr(env, value, csrno);
    }

    if (next) {
        *next = cont ? NG : OK;
    }

#ifdef DEBUG_ANDES_CSR
    if (!cont) {
        qemu_log("== %s: CSR %03lx V %08lx ==\n", __func__, (long)csrno, (long)value);
    }
#endif
}

void andes_riscv_csrif_init(CPURISCVState *env)
{
    env->csrif.csr_validate_helper = andes_riscv_csr_validate_helper;
    env->csrif.csr_read_helper = andes_riscv_csr_read_helper;
    env->csrif.csr_write_helper = andes_riscv_csr_write_helper;
}
