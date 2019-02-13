/*
 * QEMU RISC-V Board Compatible with Andes AE250 platform
 *
 * Copyright (c) 2018 Andes Tech. Corp.
 *
 * Provides a  board compatible with the Andes AE250 platform:
 *
 * 0) UART
 * 1) PLMT (Platform Machine Timer)
 * 2) PLIC (Platform Level Interrupt Controller)
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
#include "qemu/log.h"
#include "qapi/error.h"
#include "exec/address-spaces.h"
#include "exec/address-spaces.h"
#include "sysemu/arch_init.h"
#include "sysemu/device_tree.h"
#include "target/riscv/cpu.h"
#include "chardev/char.h"
#include "hw/hw.h"
#include "hw/boards.h"
#include "hw/loader.h"
#include "hw/sysbus.h"
#include "hw/char/serial.h"
#include "hw/andes/atcuart100.h"
#include "hw/andes/atcpit100.h"
#include "hw/andes/atfsdc010.h"
#include "hw/andes/atfmac100.h"
#include "hw/riscv/andes_plmt.h"
#include "hw/riscv/andes_plic.h"
#include "hw/riscv/riscv_hart.h"
#include "hw/riscv/andes_ae250.h"
#include "elf.h"
#include "libfdt.h"

#define AE250_DEFAULT_RSTVEC (DEFAULT_RSTVEC|(1u<<31))

static const struct MemmapEntry {
    hwaddr base;
    hwaddr size;
} andes_ae250_memmap[] = {
    [ANDES_AE250_DRAM]      = { 0x00000000u, 0x80000000u },
    [ANDES_AE250_SPI]       = { 0x80000000u, 0x00100000u },
    [ANDES_AE250_ILM]       = { 0xa0000000u, 0x00200000u },
    [ANDES_AE250_DLM]       = { 0xa0200000u, 0x00200000u },
    [ANDES_AE250_BMC]       = { 0xe0000000u, 0x00100000u },
    [ANDES_AE250_MAC]       = { 0xe0100000u, 0x00100000u },
    [ANDES_AE250_DMAC]      = { 0xe0e00000u, 0x00100000u },
    [ANDES_AE250_PLIC]      = { 0xe4000000u, 0x02000000u },
    [ANDES_AE250_PLMT]      = { 0xe6000000u, 0x00100000u },
    [ANDES_AE250_SWINT]     = { 0xe6400000u, 0x00400000u },
    [ANDES_AE250_PLDM]      = { 0xe6800000u, 0x00100000u },
    [ANDES_AE250_APBBRG]    = { 0xf0000000u, 0x00100000u },
    [ANDES_AE250_SUM]       = { 0xf0100000u, 0x00100000u },
    [ANDES_AE250_UART1]     = { 0xf0200000u, 0x00100000u },
    [ANDES_AE250_UART2]     = { 0xf0300000u, 0x00100000u },
    [ANDES_AE250_PIT]       = { 0xf0400000u, 0x00100000u },
    [ANDES_AE250_WDT]       = { 0xf0500000u, 0x00100000u },
    [ANDES_AE250_RTC]       = { 0xf0600000u, 0x00100000u },
    [ANDES_AE250_GPIO]      = { 0xf0700000u, 0x00100000u },
    [ANDES_AE250_I2C]       = { 0xf0a00000u, 0x00100000u },
    [ANDES_AE250_SPI1]      = { 0xf0b00000u, 0x00100000u },
    [ANDES_AE250_SDC]       = { 0xf0e00000u, 0x00100000u },
    [ANDES_AE250_SPI2]      = { 0xf0f00000u, 0x00100000u },
    [ANDES_AE250_PIT2]      = { 0xf1000000u, 0x00100000u },
    [ANDES_AE250_PIT3]      = { 0xf1100000u, 0x00100000u },
    [ANDES_AE250_PIT4]      = { 0xf1200000u, 0x00100000u },
    [ANDES_AE250_PIT5]      = { 0xf1300000u, 0x00100000u },
    [ANDES_AE250_SPI3]      = { 0xf1900000u, 0x00100000u },
    [ANDES_AE250_SPI4]      = { 0xf1a00000u, 0x00100000u },
    [ANDES_AE250_I2C2]      = { 0xf1b00000u, 0x00100000u },
    [ANDES_AE250_DEBUG]     = { 0xffff0000u, 0x00000100u },
};

typedef struct ResetData {
    RISCVCPU *cpu;
    target_ulong vector;
} ResetData;

static void main_cpu_reset(void *opaque)
{
    ResetData *s = (ResetData *)opaque;
    CPURISCVState *env = &s->cpu->env;

    cpu_reset(CPU(s->cpu));
    env->pc = s->vector;
}

static uint64_t load_kernel(const char *kernel_filename)
{
    uint64_t kernel_entry, kernel_high;

    if (load_elf(kernel_filename, NULL, NULL,
                 &kernel_entry, NULL, &kernel_high,
                 0, EM_RISCV, 1, 0) < 0) {
        error_report("qemu: could not load kernel '%s'", kernel_filename);
        exit(1);
    }
    return kernel_entry;
}

static void mock_mmio_emulate(MemoryRegion *parent, const char *name,
                         uintptr_t offset, uintptr_t length)
{
    MemoryRegion *mock_mmio = g_new(MemoryRegion, 1);
    memory_region_init_ram(mock_mmio, NULL, name, length, &error_fatal);
    memory_region_add_subregion(parent, offset, mock_mmio);
}

static void riscv_andes_ae250_init(MachineState *machine)
{
    const struct MemmapEntry *memmap = andes_ae250_memmap;

    AndesAe250State *s = g_new0(AndesAe250State, 1);
    MemoryRegion *sys_mem = get_system_memory();
    MemoryRegion *main_mem = g_new(MemoryRegion, 1);
    ResetData *reset_info;
    int i;

    /* Initialize SoC */
    object_initialize_child(OBJECT(machine), "soc", &s->soc,
                            sizeof(s->soc), TYPE_ANDES_AE250_SOC,
                            &error_abort, NULL);
    object_property_set_bool(OBJECT(&s->soc), true, "realized",
                            &error_abort);

    reset_info = g_malloc0(sizeof(ResetData));
    reset_info->cpu = s->soc.cpus.harts;
    qemu_register_reset(main_cpu_reset, reset_info);

    /* Data Tightly Integrated Memory */
    memory_region_init_ram(main_mem, NULL, "riscv.andes.ae250.ram",
        memmap[ANDES_AE250_DRAM].size, &error_fatal);
    memory_region_add_subregion(sys_mem,
        memmap[ANDES_AE250_DRAM].base, main_mem);

    /* Mask ROM reset vector */
    uint32_t reset_vec[2] = {
        0x204002b7,        /* 0x1000: lui     t0,0x20400 */
        0x00028067,        /* 0x1004: jr      t0 */
    };

    /* copy in the reset vector in little_endian byte order */
    for (i = 0; i < sizeof(reset_vec) >> 2; i++) {
        reset_vec[i] = cpu_to_le32(reset_vec[i]);
    }
    rom_add_blob_fixed_as("mrom.reset", reset_vec, sizeof(reset_vec),
                          memmap[ANDES_AE250_SPI].base, &address_space_memory);

    if (machine->kernel_filename) {
        reset_info->vector = load_kernel(machine->kernel_filename);
    }
}

static void riscv_andes_ae250_soc_init(Object *obj)
{
    AndesAe250SocState *s = ANDES_AE250_SOC(obj);

    object_initialize_child(obj, "cpus", &s->cpus,
                            sizeof(s->cpus), TYPE_RISCV_HART_ARRAY,
                            &error_abort, NULL);
    object_property_set_str(OBJECT(&s->cpus), ANDES_AE250_CPU, "cpu-type",
                            &error_abort);
    object_property_set_int(OBJECT(&s->cpus), smp_cpus, "num-harts",
                            &error_abort);
}

static void riscv_andes_ae250_soc_realize(DeviceState *dev, Error **errp)
{
    const struct MemmapEntry *memmap = andes_ae250_memmap;

    AndesAe250SocState *s = ANDES_AE250_SOC(dev);
    MemoryRegion *sys_mem = get_system_memory();

    object_property_set_bool(OBJECT(&s->cpus), true, "realized",
                            &error_abort);

    /* MMIO */
    s->plic = andes_plic_create(
        memmap[ANDES_AE250_PLIC].base, (char *)ANDES_AE250_PLIC_HART_CONFIG,
        ANDES_AE250_PLIC_NUM_SOURCES, ANDES_AE250_PLIC_NUM_PRIORITIES,
        ANDES_AE250_PLIC_PRIORITY_BASE, ANDES_AE250_PLIC_PENDING_BASE,
        ANDES_AE250_PLIC_ENABLE_BASE, ANDES_AE250_PLIC_ENABLE_STRIDE,
        ANDES_AE250_PLIC_CONTEXT_BASE, ANDES_AE250_PLIC_CONTEXT_STRIDE,
        memmap[ANDES_AE250_PLIC].size);

    andes_plmt_create(memmap[ANDES_AE250_PLMT].base, memmap[ANDES_AE250_PLMT].size,
                      smp_cpus, ANDES_PLMT_TIME_BASE, ANDES_PLMT_TIMECMP_BASE);
    atcuart100_create(sys_mem, memmap[ANDES_AE250_UART1].base,
                      qdev_get_gpio_in(DEVICE(s->plic), ANDES_AE250_UART1_IRQ),
                      115200, serial_hd(1));
    atcuart100_create(sys_mem, memmap[ANDES_AE250_UART2].base,
                      qdev_get_gpio_in(DEVICE(s->plic), ANDES_AE250_UART2_IRQ),
                      115200, serial_hd(0));
    atcpit100_create(memmap[ANDES_AE250_PIT].base,
                     qdev_get_gpio_in(DEVICE(s->plic), ANDES_AE250_PIT_IRQ));
    atfsdc010_create(memmap[ANDES_AE250_SDC].base,
                     qdev_get_gpio_in(DEVICE(s->plic), ANDES_AE250_SDC_IRQ));
    atfmac100_create(OBJECT(s), "atfmac100", &nd_table[0],
                     memmap[ANDES_AE250_MAC].base,
                     qdev_get_gpio_in(DEVICE(s->plic), ANDES_AE250_MAC_IRQ));

    /* Mock Devices */
    mock_mmio_emulate(sys_mem, "riscv.andes.ae250.bmc",
                      memmap[ANDES_AE250_BMC].base,
                      memmap[ANDES_AE250_BMC].size);
    mock_mmio_emulate(sys_mem, "riscv.andes.ae250.sum",
                      memmap[ANDES_AE250_SUM].base,
                      memmap[ANDES_AE250_SUM].size);
}

static void riscv_andes_ae250_machine_init(MachineClass *mc)
{
    mc->desc = "RISC-V Board compatible with Andes AE250 Platform";
    mc->init = riscv_andes_ae250_init;
    mc->max_cpus = 1;
}

DEFINE_MACHINE("andes_ae250", riscv_andes_ae250_machine_init)

static void riscv_andes_ae250_soc_class_init(ObjectClass *oc, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(oc);

    dc->realize = riscv_andes_ae250_soc_realize;
    /* Reason: Uses serial_hds in realize function, thus can't be used twice */
    dc->user_creatable = false;
}

static const TypeInfo riscv_andes_ae250_soc_type_info = {
    .name          = TYPE_ANDES_AE250_SOC,
    .parent        = TYPE_DEVICE,
    .instance_size = sizeof(AndesAe250SocState),
    .instance_init = riscv_andes_ae250_soc_init,
    .class_init    = riscv_andes_ae250_soc_class_init,
};

static void riscv_andes_ae250_soc_register_types(void)
{
    type_register_static(&riscv_andes_ae250_soc_type_info);
}

type_init(riscv_andes_ae250_soc_register_types);
