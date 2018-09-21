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

#ifndef ANDES_V5_RISCV_CPU_H
#define ANDES_V5_RISCV_CPU_H

void andes_v5_riscv_csrif_init(CPURISCVState *env);
void andes_v5_riscv_isaif_init(CPURISCVState *env);

#endif /* ANDES_V5_RISCV_CPU_H */
