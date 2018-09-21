/*
 * ANDES RISC-V CPU CSR Extension
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

#ifndef ANDES_CSR_RISCV_CPU_H
#define ANDES_CSR_RISCV_CPU_H

void andes_riscv_csr_write_helper(CPURISCVState *env, target_ulong val_to_write, target_ulong csrno, int *next);
target_ulong andes_riscv_csr_read_helper(CPURISCVState *env, target_ulong csrno, int *next);

void andes_riscv_csrif_init(CPURISCVState *env);

#endif /* ANDES_CSR_RISCV_CPU_H */
