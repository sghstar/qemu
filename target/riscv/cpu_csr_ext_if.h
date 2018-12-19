/*
 * QEMU RISC-V CPU CSR Extension Interface
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

#ifndef RISCV_CPU_CSR_EXT_IF_H
#define RISCV_CPU_CSR_EXT_IF_H

typedef struct CPURVCsrExtIf {
    void (*csr_validate_helper)(CPURISCVState *env, uint64_t which, uint64_t write, uintptr_t ra, int *next);
    target_ulong (*csr_read_helper)(CPURISCVState *env, target_ulong csrno, int *next);
    void (*csr_write_helper)(CPURISCVState *env, target_ulong value, target_ulong csrno, int *next);
};

#endif /* RISCV_CPU_CSR_EXT_IF_H */
