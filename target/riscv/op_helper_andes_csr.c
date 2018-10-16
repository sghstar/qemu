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

#define OK (0)
#define NG (1)

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
    CSR_MCCTLBEGINADDR = 0x7cb,
    CSR_MCCTLCOMMAND   = 0x7cc,
    CSR_MCCTLDATA      = 0x7cd,

    /* internal machine mode CSRs */
    CSR_MRANDESQ    = 0x7fc,
    CSR_MRANDSEQH   = 0x7fd,
    CSR_MRANDSTATE  = 0x7fe,
    CSR_MRANDSTATEH = 0x7ff,

    /* debug mode CSRs */
    CSR_DEXC2DBG    = 0x7e0,
    CSR_DDCAUSE     = 0x7e1,

    /* supervisor mode CSRs */
    CSR_SCCTLDATA   = 0x9cd,

    /* user mode CSRs */
    CSR_UITB        = 0x800,
    CSR_UCCTLBEGINADDR = 0x80b,
    CSR_UCCTLCOMMAND   = 0x80c,
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

target_ulong andes_riscv_csr_read_helper(CPURISCVState *env, target_ulong csrno, int *next)
{
    CPURVAndesExt *ext = env->ext;
    target_ulong csr;

    if (next) {
        *next = OK; /* presume */
    }
    switch (csrno) {
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
    case CSR_MCCTLBEGINADDR:
        csr = ext->mcctlbeginaddr;
        break;
    case CSR_MCCTLCOMMAND:
        csr = ext->mcctlcommand;
        break;
    case CSR_MCCTLDATA:
        csr = ext->mcctldata;
        break;
    case CSR_SCCTLDATA:
        csr = ext->scctldata;
        break;
    case CSR_UCCTLBEGINADDR:
        csr = ext->ucctlbeginaddr;
        break;
    case CSR_UCCTLCOMMAND:
        csr = ext->ucctlcommand;
        break;
    default:
        csr = 0xdeadbeef;
        if (next) {
            *next = NG;
        }
    }

    if (next && !*next) qemu_log("%s: CSR %03lx V %08lx\n", __func__, (long)csrno, (long)csr);
    return csr;
}

void andes_riscv_csr_write_helper(CPURISCVState *env, target_ulong value, target_ulong csrno, int *next)
{
    CPURVAndesExt *ext = env->ext;

    if (next) {
        *next = OK; /* presume */
    }
    switch (csrno) {
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
    case CSR_MCCTLBEGINADDR:
        ext->mcctlbeginaddr = value;
        break;
    case CSR_MCCTLCOMMAND:
        do_cctl_command(env, value, PRV_M);
        ext->mcctlcommand = value;
        break;
    case CSR_MCCTLDATA:
        ext->mcctldata = value;
        break;
    case CSR_SCCTLDATA:
        ext->scctldata = value;
        break;
    case CSR_UCCTLBEGINADDR:
        ext->ucctlbeginaddr = value;
        break;
    case CSR_UCCTLCOMMAND:
        do_cctl_command(env, value, PRV_U);
        ext->ucctlcommand = value;
        break;
    default:
        if (next) {
            *next = NG;
        }
    }

    if (next && !*next) qemu_log("%s: CSR %03lx V %08lx\n", __func__, (long)csrno, (long)value);
}

void andes_riscv_csrif_init(CPURISCVState *env)
{
    env->csrif.csr_read_helper = andes_riscv_csr_read_helper;
    env->csrif.csr_write_helper = andes_riscv_csr_write_helper;
}
