/*
 * Andes PLIC (Platform Level Interrupt Controller)
 *
 * Copyright (c) 2018 Andes Tech. Corp.
 *
 * This provides a parameterizable interrupt controller based on Andes' PLIC.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "qemu/osdep.h"
#include "qemu/error-report.h"
#include "qemu/log.h"
#include "qapi/error.h"
#include "target/riscv/cpu.h"
#include "hw/sysbus.h"
#include "hw/riscv/andes_plic.h"

/* #define DEBUG_ANDES_PLIC */
#define LOGGE(x...) qemu_log_mask(LOG_GUEST_ERROR,x)
#define xLOG(x...)
#define yLOG(x...) qemu_log(x)
#ifdef DEBUG_ANDES_PLIC
  #define LOG(x...) yLOG(x)
#else
  #define LOG(x...) xLOG(x)
#endif

enum register_names {
    REG_FEATURE_ENABLE = 0x0000,
    REG_TRIGGER_TYPE_BASE = 0x1080,
    REG_NUM_IRQ_TARGET = 0x1100,
    REG_VER_MAX_PRIORITY = 0x1104,
};

static uint64_t
andes_plic_read(void *opaque, hwaddr addr, unsigned size)
{
    AndesPLICState *s = ANDES_PLIC(opaque);
    SiFivePLICState *ss = SIFIVE_PLIC(s);
    uint64_t value;

    if ((addr & 0x3)) {
        error_report("%s: invalid register write: %08x", __func__, (uint32_t)addr);
    }

    switch (addr)
    {
    case REG_FEATURE_ENABLE:
        value = 0x3; /* PREEMPT|VECTORED */
        break;
    case REG_NUM_IRQ_TARGET:
        /* TODO: NO hardcode target number */
        /* value = (ss->num_targets << 16) | ss->num_sources; */
        value = (2 << 16) | ss->num_sources; /* MS */
        break;
    case REG_VER_MAX_PRIORITY:
        value = (ss->num_priorities << 16) | 0x0007; /* PLIC ver 0.7 */
        break;
    default:
        if (addr >= REG_TRIGGER_TYPE_BASE &&
            addr < REG_TRIGGER_TYPE_BASE + (ss->bitfield_words << 2)) {
            value = s->trigger_type[addr >> 2];
            break;
        }

        memory_region_dispatch_read(&s->parent_mmio, addr, &value, size,
                                    MEMTXATTRS_UNSPECIFIED);
    }

    LOG("%s:  addr %08x, size %08x, value %08x\n", __func__, (int)addr, size, (int)value);
    return value;
}

static void
andes_plic_write(void *opaque, hwaddr addr, uint64_t value, unsigned size)
{
    AndesPLICState *s = ANDES_PLIC(opaque);
    SiFivePLICState *ss = SIFIVE_PLIC(s);
    LOG("%s: addr %08x, size %08x, value %08x\n", __func__, (int)addr, size, (int)value);

    if ((addr & 0x3)) {
        error_report("%s: invalid register write: %08x", __func__, (uint32_t)addr);
    }

    switch (addr)
    {
    case REG_FEATURE_ENABLE:
        s->feature_enable = value & 0x3;
        /* TODO: side effects */
        break;
    case REG_NUM_IRQ_TARGET:   /* RO */
    case REG_VER_MAX_PRIORITY: /* RO */
        break;
    default:
        /* R/W1S */
        if (addr >= REG_TRIGGER_TYPE_BASE &&
            addr < REG_TRIGGER_TYPE_BASE + (ss->bitfield_words << 2)) {
            s->trigger_type[addr >> 2] |= value;
            break;
        }

        memory_region_dispatch_write(&s->parent_mmio, addr, value, size,
                                     MEMTXATTRS_UNSPECIFIED);
    }
}

static const MemoryRegionOps andes_plic_ops = {
    .read = andes_plic_read,
    .write = andes_plic_write,
    .endianness = DEVICE_LITTLE_ENDIAN,
    .valid = {
        .min_access_size = 4,
        .max_access_size = 4
    }
};

static void
andes_plic_reset(DeviceState * dev)
{
    LOG("%s:\n", __func__);
    AndesPLICState *s = ANDES_PLIC(dev);
    AndesPLICClass *k = ANDES_PLIC_GET_CLASS(s);
    SiFivePLICState *ss = SIFIVE_PLIC(s);

    memset(ss->target_priority, 0, sizeof(uint32_t) * ss->num_addrs);

    if (k->parent_reset)
        k->parent_reset(dev);

    s->feature_enable = 0;
}

static void
andes_plic_realize(DeviceState *dev, Error **errp)
{
    LOG("%s:\n", __func__);
    AndesPLICState *s = ANDES_PLIC(dev);
    SiFivePLICState *ss = SIFIVE_PLIC(dev);
    AndesPLICClass *k = ANDES_PLIC_GET_CLASS(s);
    Error *local_err = NULL;

    k->parent_realize(dev, &local_err);
    if (local_err) {
        error_propagate(errp, local_err);
        return;
    }

    s->trigger_type = g_new0(uint32_t, ss->bitfield_words);

    /* override MemoryRegionOps */
    s->parent_mmio = ss->mmio;
    memory_region_init_io(&ss->mmio, OBJECT(dev), &andes_plic_ops, s,
                          TYPE_ANDES_PLIC, ss->aperture_size);
}

#if 0
static Property sifive_plic_properties[] = {
    DEFINE_PROP_STRING("hart-config", SiFivePLICState, hart_config),
    DEFINE_PROP_UINT32("num-sources", SiFivePLICState, num_sources, 0),
    DEFINE_PROP_UINT32("num-priorities", SiFivePLICState, num_priorities, 0),
    DEFINE_PROP_UINT32("priority-base", SiFivePLICState, priority_base, 0),
    DEFINE_PROP_UINT32("pending-base", SiFivePLICState, pending_base, 0),
    DEFINE_PROP_UINT32("enable-base", SiFivePLICState, enable_base, 0),
    DEFINE_PROP_UINT32("enable-stride", SiFivePLICState, enable_stride, 0),
    DEFINE_PROP_UINT32("context-base", SiFivePLICState, context_base, 0),
    DEFINE_PROP_UINT32("context-stride", SiFivePLICState, context_stride, 0),
    DEFINE_PROP_UINT32("aperture-size", SiFivePLICState, aperture_size, 0),
    DEFINE_PROP_END_OF_LIST(),
};
#endif

static void andes_plic_class_init(ObjectClass *klass, void *data)
{
    LOG("%s:\n", __func__);
    DeviceClass *dc = DEVICE_CLASS(klass);
    /* SiFivePLICClass *sdc = SIFIVE_PLIC_CLASS(klass); */
    AndesPLICClass *adc = ANDES_PLIC_CLASS(klass);

    /* TODO: add own properties */
    /* dc->props = andes_plic_properties; */
    device_class_set_parent_reset(dc, andes_plic_reset, &adc->parent_reset);
    device_class_set_parent_realize(dc, andes_plic_realize, &adc->parent_realize);
}

static const TypeInfo andes_plic_info = {
    .name          = TYPE_ANDES_PLIC,
    .parent        = TYPE_SIFIVE_PLIC,
    .instance_size = sizeof(AndesPLICState),
    .class_init    = andes_plic_class_init,
    .class_size    = sizeof(AndesPLICClass),
};

static void andes_plic_register_types(void)
{
    LOG("%s:\n", __func__);
    type_register_static(&andes_plic_info);
}

type_init(andes_plic_register_types)

/*
 * Create PLIC device.
 */
DeviceState *andes_plic_create(hwaddr addr, char *hart_config,
    uint32_t num_sources, uint32_t num_priorities,
    uint32_t priority_base, uint32_t pending_base,
    uint32_t enable_base, uint32_t enable_stride,
    uint32_t context_base, uint32_t context_stride,
    uint32_t aperture_size)
{
    LOG("%s:\n", __func__);
    DeviceState *dev = qdev_create(NULL, TYPE_ANDES_PLIC);
    /* TODO: better inheritance, copy from sifive_plic_create() */
    assert(enable_stride == (enable_stride & -enable_stride));
    assert(context_stride == (context_stride & -context_stride));
    qdev_prop_set_string(dev, "hart-config", hart_config);
    qdev_prop_set_uint32(dev, "num-sources", num_sources);
    qdev_prop_set_uint32(dev, "num-priorities", num_priorities);
    qdev_prop_set_uint32(dev, "priority-base", priority_base);
    qdev_prop_set_uint32(dev, "pending-base", pending_base);
    qdev_prop_set_uint32(dev, "enable-base", enable_base);
    qdev_prop_set_uint32(dev, "enable-stride", enable_stride);
    qdev_prop_set_uint32(dev, "context-base", context_base);
    qdev_prop_set_uint32(dev, "context-stride", context_stride);
    qdev_prop_set_uint32(dev, "aperture-size", aperture_size);
    qdev_init_nofail(dev);
    sysbus_mmio_map(SYS_BUS_DEVICE(dev), 0, addr); /* n = 1 !! */
    return dev;
}

/*
 * Create PLIC SWINT device.
 */
DeviceState *
andes_plic_swint_create(hwaddr addr, char *hart_config, uint32_t num_sources,
                        uint32_t num_priorities, uint32_t priority_base,
                        uint32_t pending_base, uint32_t enable_base,
                        uint32_t enable_stride, uint32_t context_base,
                        uint32_t context_stride, uint32_t aperture_size,
                        uint32_t m_mode_mip_mask, uint32_t s_mode_mip_mask)
{
    LOG("%s:\n", __func__);
    DeviceState *dev = qdev_create(NULL, TYPE_ANDES_PLIC);
    /* TODO: better inheritance, copy from sifive_plic_create() */
    assert(enable_stride == (enable_stride & -enable_stride));
    assert(context_stride == (context_stride & -context_stride));
    qdev_prop_set_string(dev, "hart-config", hart_config);
    qdev_prop_set_uint32(dev, "num-sources", num_sources);
    qdev_prop_set_uint32(dev, "num-priorities", num_priorities);
    qdev_prop_set_uint32(dev, "priority-base", priority_base);
    qdev_prop_set_uint32(dev, "pending-base", pending_base);
    qdev_prop_set_uint32(dev, "enable-base", enable_base);
    qdev_prop_set_uint32(dev, "enable-stride", enable_stride);
    qdev_prop_set_uint32(dev, "context-base", context_base);
    qdev_prop_set_uint32(dev, "context-stride", context_stride);
    qdev_prop_set_uint32(dev, "aperture-size", aperture_size);
    qdev_prop_set_uint32(dev, "m-mode-mip-mask", m_mode_mip_mask);
    qdev_prop_set_uint32(dev, "s-mode-mip-mask", s_mode_mip_mask);
    qdev_init_nofail(dev);
    sysbus_mmio_map(SYS_BUS_DEVICE(dev), 0, addr); /* n = 1 !! */
    return dev;
}
