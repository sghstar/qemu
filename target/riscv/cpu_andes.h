/*
 * QEMU ANDES RISC-V CPU
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

#ifndef ANDES_RISCV_CPU_H
#define ANDES_RISCV_CPU_H

#define ANDES_A25_DEFAULT_RSTVEC  (0xb0000000u)
#define ANDES_AX25_DEFAULT_RSTVEC (0xb0000000u)
#define ANDES_N25_DEFAULT_RSTVEC  (0x00000000u)
#define ANDES_NX25_DEFAULT_RSTVEC (0x00000000u)

#define ANDES_SLI       (16)
#define ANDES_SLI_BIAS  (256)

typedef struct CPURVAndesExt CPURVAndesExt;

struct CPURVAndesExt {
    /* extended CSRs */
    target_ulong micm_cfg;
    target_ulong mdcm_cfg;
    target_ulong mmsc_cfg;
    target_ulong milmb;
    target_ulong mdlmb;
    target_ulong mecc_code;
    target_ulong mnvec;
    target_ulong mcache_ctl;
    target_ulong mhsp_ctl;
    target_ulong msp_bound;
    target_ulong msp_base;
    target_ulong mxstatus;
    target_ulong mdcause;
    target_ulong mpft_ctl;
    target_ulong mmisc_ctl;
    target_ulong mrandseq;
    target_ulong mrandseqh;
    target_ulong mrandstate;
    target_ulong mrandstateh;
    target_ulong dexc2dbg;
    target_ulong ddcause;
    target_ulong uitb;
    target_ulong mcctlbeginaddr;
    target_ulong mcctlcommand;
    target_ulong mcctldata;
    target_ulong scctldata;
    target_ulong ucctlbeginaddr;
    target_ulong ucctlcommand;

    /* local interrupt CSRs */
    target_ulong mslideleg;
    target_ulong slie;
    target_ulong slip;

    /* counter related CSRs */
    target_ulong mcounterwen;
    target_ulong mcountermask_m;
    target_ulong mcountermask_s;
    target_ulong mcountermask_u;
    target_ulong mcounterinten;
    target_ulong mcounterovf;

    target_ulong mhpmcounter[32];
    target_ulong mhpmcounterh[32];
    target_ulong mhpmevent[32];

    /* trigger CSRs */
    target_ulong tselect;
    target_ulong tdata1;
    target_ulong tdata2;
    target_ulong tdata3;
    target_ulong tinfo;
    target_ulong tcontrol;
    target_ulong mcontext;
    target_ulong scontext;

    /* counter internals */
    uint64_t hpmcounter_mark[32];

    /* vectored interrupt internals */
    uint32_t vectored_irq_m;
    uint32_t vectored_irq_s;

    /* DSP extension internals */
    target_ulong ucode;
    target_ulong loopb;
    target_ulong loope;
    target_ulong loopc;
};

void andes_riscv_isaif_init(CPURISCVState *env);
bool andes_riscv_cpu_exec_interrupt(CPUState *cs, int interrupt_request);
void andes_riscv_cpu_do_interrupt(CPUState *cs);

#endif /* ANDES_RISCV_CPU_H */
