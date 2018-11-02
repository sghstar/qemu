/*
 * Andes PLMT (Platform Level Machine Timer)
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
#include "qemu/error-report.h"
#include "qemu/timer.h"
#include "target/riscv/cpu.h"
#include "hw/sysbus.h"
#include "hw/riscv/andes_plmt.h"

static uint64_t cpu_riscv_read_rtc(void)
{
    return muldiv64(qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL),
        ANDES_PLMT_TIMEBASE_FREQ, NANOSECONDS_PER_SECOND);
}

/*
 * Called when timecmp is written to update the QEMU timer or immediately
 * trigger timer interrupt if mtimecmp <= current timer value.
 */
static void andes_plmt_write_timecmp(RISCVCPU* cpu, uint64_t value)
{
    uint64_t next;
    uint64_t diff;

    uint64_t rtc_r = cpu_riscv_read_rtc();

    cpu->env.timecmp = value;
    if (cpu->env.timecmp <= rtc_r) {
        /* if we're setting an MTIMECMP value in the "past",
           immediately raise the timer interrupt */
        riscv_set_local_interrupt(cpu, MIP_MTIP, 1);
        return;
    }

    /* otherwise, set up the future timer interrupt */
    riscv_set_local_interrupt(cpu, MIP_MTIP, 0);
    diff = cpu->env.timecmp - rtc_r;
    /* back to ns (note args switched in muldiv64) */
    next = qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL) + muldiv64(diff, NANOSECONDS_PER_SECOND, ANDES_PLMT_TIMEBASE_FREQ);
    timer_mod(cpu->env.timer, next);
}

/*
 * Callback used when the timer set using timer_mod expires.
 * Should raise the timer interrupt line
 */
static void andes_plmt_timer_cb(void* opaque)
{
    RISCVCPU* cpu = opaque;
    riscv_set_local_interrupt(cpu, MIP_MTIP, 1);
}

static uint64_t andes_plmt_read(void* opaque, hwaddr addr, unsigned size)
{
    AndesPLMTState* plmt = opaque;
    uint64_t rz;
    switch (addr) {
    case 0: /* mtime */
        rz = cpu_riscv_read_rtc() & 0xFFFFFFFFu;
        break;
    case 4: /* mtimeh */
        rz = (cpu_riscv_read_rtc() >> 32) & 0xFFFFFFFFu;
        break;
    case 8: /* mtimecmp */
    case 12: /* mtimecmph */
    {
        size_t hartid = plmt->target;
        CPUState* cpu = qemu_get_cpu(hartid);
        CPURISCVState* env = cpu ? cpu->env_ptr : NULL;
        if (!env) {
            error_report("plmt: invalid timecmp hartid: %zu", hartid);
        } else if (addr == 8) {
            rz = env->timecmp & 0xFFFFFFFFu;
        } else if (addr == 12) {
            rz = (env->timecmp >> 32) & 0xFFFFFFFFu;
        } else {
            error_report("plmt: invalid read: %08x", (uint32_t)addr);
            return 0;
        }
        break;
    }
    default:
        rz = 0;
        error_report("plmt: invalid read: %08x", (uint32_t)addr);
    }

    return rz;
}

static void andes_plmt_write(void* opaque, hwaddr addr, uint64_t value, unsigned size)
{
    AndesPLMTState* plmt = opaque;

    switch (addr) {
    case 0: /* mtime */
        error_report("plmt: mtime write not implemented");
        break;
    case 4: /* mtimeh */
        error_report("plmt: mtimeh write not implemented");
        break;
    case 8: /* mtimecmp */
    case 12: /* mtimecmph */
    {
        size_t hartid = plmt->target;
        CPUState* cpu = qemu_get_cpu(hartid);
        CPURISCVState* env = cpu ? cpu->env_ptr : NULL;
        if (!env) {
            error_report("plmt: invalid timecmp hartid: %zu", hartid);
        } else if (addr == 8) {
            uint64_t timecmp_hi = env->timecmp >> 32;
            andes_plmt_write_timecmp(RISCV_CPU(cpu), (timecmp_hi << 32) | (value & 0xFFFFFFFFu));
        } else if (addr == 12) {
            uint64_t timecmp_lo = env->timecmp;
            andes_plmt_write_timecmp(RISCV_CPU(cpu), (value << 32) | (timecmp_lo & 0xFFFFFFFFu));
        } else {
            error_report("plmt: invalid timecmp write: %08x", (uint32_t)addr);
        }
        break;
    }
    default:
        error_report("plmt: invalid write: %08x", (uint32_t)addr);
    }
}

static const MemoryRegionOps andes_plmt_ops = {
    .read = andes_plmt_read,
    .write = andes_plmt_write,
    .endianness = DEVICE_LITTLE_ENDIAN,
    .valid = {
        .min_access_size = 4,
        .max_access_size = 4 }
};

static Property andes_plmt_properties[] = {
    DEFINE_PROP_UINT32("target", AndesPLMTState, target, 0),
    DEFINE_PROP_END_OF_LIST(),
};

static void andes_plmt_realize(DeviceState* dev, Error** errp)
{
    AndesPLMTState* s = ANDES_PLMT(dev);
    memory_region_init_io(&s->mmio, OBJECT(dev), &andes_plmt_ops, s,
        TYPE_ANDES_PLMT, ANDES_PLMT_MMIO_SIZE);
    sysbus_init_mmio(SYS_BUS_DEVICE(dev), &s->mmio);
}

static void andes_plmt_class_init(ObjectClass* klass, void* data)
{
    DeviceClass* dc = DEVICE_CLASS(klass);
    dc->realize = andes_plmt_realize;
    dc->props = andes_plmt_properties;
}

static const TypeInfo andes_plmt_info = {
    .name = TYPE_ANDES_PLMT,
    .parent = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(AndesPLMTState),
    .class_init = andes_plmt_class_init,
};

static void andes_plmt_register_types(void)
{
    type_register_static(&andes_plmt_info);
}

type_init(andes_plmt_register_types)

/*
 * Create PLMT device.
 */
DeviceState* andes_plmt_create(hwaddr addr, hwaddr size, uint32_t target)
{
    CPUState* cpu = qemu_get_cpu(target);
    CPURISCVState* env = cpu ? cpu->env_ptr : NULL;
    if (env) {
        env->timer = timer_new_ns(QEMU_CLOCK_VIRTUAL, &andes_plmt_timer_cb, cpu);
        env->timecmp = 0;
    }

    DeviceState* dev = qdev_create(NULL, TYPE_ANDES_PLMT);
    qdev_prop_set_uint32(dev, "target", target);
    qdev_init_nofail(dev);
    sysbus_mmio_map(SYS_BUS_DEVICE(dev), 0, addr);
    return dev;
}
