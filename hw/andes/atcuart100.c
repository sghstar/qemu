/*
 * Andes ATCUART100 (programming sequence compatible with the 16C550D UART)
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

#include "qemu/osdep.h"
#include "qemu/log.h"
#include "chardev/char-fe.h"
#include "qapi/error.h"
#include "hw/qdev.h"
#include "hw/sysbus.h"
#include "hw/char/serial.h"
#include "hw/andes/atcuart100.h"

static uint64_t
atcuart_read(void *opaque, hwaddr addr, unsigned size)
{
  Atcuart100State *as = opaque;
  SerialState *s = &as->state;
  uint64_t rz = 0;

  switch (addr)
    {
    case ATCUART_IDREV: /* RO */
      rz = as->idrev;
      break;
    case ATCUART_CFG: /* RO */
      rz = as->cfg;
      break;
    case ATCUART_OSCR:
      rz = as->oscr;
      break;
    case ATCUART_RBR: /* or ATCUART_DLL */
    case ATCUART_IER: /* or ATCUART_DLM */
    case ATCUART_IIR:
    case ATCUART_LCR:
    case ATCUART_MCR:
    case ATCUART_LSR:
    case ATCUART_MSR:
    case ATCUART_SCR:
      rz = serial_io_ops.read(s, (addr - ATCUART_RBR) >> 2, 1);
      break;
    default:
      qemu_log_mask(LOG_GUEST_ERROR, "atcuart_read: Bad addr %x\n",
		    (int )addr);
      break;
    }

  return rz;
}

static void
atcuart_write(void *opaque, hwaddr addr,
              uint64_t value, unsigned size)
{
  Atcuart100State *as = opaque;
  SerialState *s = &as->state;

  switch (addr)
    {
    case ATCUART_IDREV: /* RO */
      break;
    case ATCUART_CFG: /* RO */
      break;
    case ATCUART_OSCR:
      as->oscr = value;
      break;
    case ATCUART_THR: /* or ATCUART_DLL */
    case ATCUART_IER: /* or ATCUART_DLM */
    case ATCUART_IIR:
    case ATCUART_LCR:
    case ATCUART_MCR:
    case ATCUART_LSR:
    case ATCUART_MSR:
    case ATCUART_SCR:
      serial_io_ops.write(s, (addr - ATCUART_THR) >> 2, value, 1);
      break;
    default:
      qemu_log_mask(LOG_GUEST_ERROR, "atcuart_write: Bad addr %x\n",
		    (int )addr);
    }
}

#if 0
/* TODO: refer to serial.c */
const VMStateDescription vmstate_atcuart100 = {
    .name = "atcuart",
    .version_id = 2,
    .minimum_version_id = 2,
    .fields = (VMStateField[]) {
        VMSTATE_UINT32(readbuff, ATCUARTState),
        VMSTATE_UINT32(flags, ATCUARTState),
        VMSTATE_UINT32(lcr, ATCUARTState),
        VMSTATE_UINT32(rsr, ATCUARTState),
        VMSTATE_UINT32(cr, ATCUARTState),
        VMSTATE_UINT32(dmacr, ATCUARTState),
        VMSTATE_UINT32(int_enabled, ATCUARTState),
        VMSTATE_UINT32(int_level, ATCUARTState),
        VMSTATE_UINT32_ARRAY(read_fifo, ATCUARTState, 16),
        VMSTATE_UINT32(ilpr, ATCUARTState),
        VMSTATE_UINT32(ibrd, ATCUARTState),
        VMSTATE_UINT32(fbrd, ATCUARTState),
        VMSTATE_UINT32(ifl, ATCUARTState),
        VMSTATE_INT32(read_pos, ATCUARTState),
        VMSTATE_INT32(read_count, ATCUARTState),
        VMSTATE_INT32(read_trigger, ATCUARTState),
        VMSTATE_STRUCT(state, ATCUARTState, 0, vmstate_serial, SerialState),
        VMSTATE_END_OF_LIST()
    }
};
#endif

static
const MemoryRegionOps atcuart_ops = {
    .read = atcuart_read,
    .write = atcuart_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
    .valid = {
        .min_access_size = 4,
        .max_access_size = 4,
    },
};

/*
 * Create ATCUART100 device.
 */
Atcuart100State *atcuart100_create(
    MemoryRegion *address_space, hwaddr base,
    qemu_irq irq, int baudbase, Chardev *chr)
{
    Atcuart100State *as = g_malloc0(sizeof(Atcuart100State));
    SerialState *s = &as->state;

    s->irq = irq;
    s->baudbase = baudbase;
    qemu_chr_fe_init(&s->chr, chr, &error_abort);
    serial_realize_core(s, &error_fatal);

    vmstate_register(NULL, base, &vmstate_serial, s);

    memory_region_init_io(&s->io, NULL, &atcuart_ops, as,
                          TYPE_ATCUART100, ATCUART_MAX);
    memory_region_add_subregion(address_space, base, &s->io);

    return as;
}
