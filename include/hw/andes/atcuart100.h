/*
 * Andes UART interface
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

#ifndef HW_ATCUART_H
#define HW_ATCUART_H

#define TYPE_ATCUART100 "atcuart100"

#define ATCUART100(obj) OBJECT_CHECK(Atcuart100State, (obj), TYPE_ATCUART100)

enum {
    ATCUART_IDREV = 0x00,
    ATCUART_CFG   = 0x10,
    ATCUART_OSCR  = 0x14,
    ATCUART_RBR   = 0x20,
    ATCUART_THR   = 0x20,
    ATCUART_DLL   = 0x20,
    ATCUART_IER   = 0x24,
    ATCUART_DLM   = 0x24,
    ATCUART_IIR   = 0x28,
    ATCUART_FCR   = 0x28,
    ATCUART_LCR   = 0x2c,
    ATCUART_MCR   = 0x30,
    ATCUART_LSR   = 0x34,
    ATCUART_MSR   = 0x38,
    ATCUART_SCR   = 0x3c,
    ATCUART_MAX   = 0x40,
};

typedef struct Atcuart100State {
    SerialState state;

    /* Andes extensions */
    uint32_t idrev;
    uint32_t cfg;
    uint32_t oscr;
} Atcuart100State;

Atcuart100State *atcuart100_create(
    MemoryRegion *address_space, hwaddr base,
    qemu_irq irq, int baudbase, Chardev *chr);

#endif
