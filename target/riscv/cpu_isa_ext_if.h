/*
 * QEMU RISC-V CPU ISA Extension Interface
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

#ifndef RISCV_CPU_ISA_EXT_IF_H
#define RISCV_CPU_ISA_EXT_IF_H

typedef struct DisasContext DisasContext; /* forward reference */

typedef struct CPURVIsaExtIf {
    int (*decode_RV32_64C)(CPURISCVState *env, DisasContext *ctx);
    int (*decode_RV32_64G)(CPURISCVState *env, DisasContext *ctx);
} CPURVIsaExtIf;

#endif /* RISCV_CPU_ISA_EXT_IF_H */
