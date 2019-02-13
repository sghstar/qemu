/*
 * QEMU RISC-V Board Compatible with Andes AE350 platform
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
#include "qemu/error-report.h"
#include "qapi/error.h"
#include "sysemu/arch_init.h"
#include "sysemu/device_tree.h"
#include "exec/address-spaces.h"
#include "target/riscv/cpu.h"
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
#include "hw/riscv/andes_ae350.h"
#include "elf.h"

#include <libfdt.h>

static const struct MemmapEntry {
    hwaddr base;
    hwaddr size;
} andes_ae350_memmap[] = {
    [ANDES_AE350_DEBUG]     = { 0x00000000u, 0x00000100u },
    [ANDES_AE350_DRAM]      = { 0x00000000u, 0x80000000u },
    [ANDES_AE350_MROM]      = { 0xb0000000u, 0x00010000u },
    [ANDES_AE350_MAC]       = { 0xe0100000u, 0x00100000u },
    [ANDES_AE350_GEM]       = { 0xe0200000u, 0x00100000u },
    [ANDES_AE350_PLIC]      = { 0xe4000000u, 0x00400000u },
    [ANDES_AE350_PLMT]      = { 0xe6000000u, 0x00100000u },
    [ANDES_AE350_SWINT]     = { 0xe6400000u, 0x00400000u },
    [ANDES_AE350_UART1]     = { 0xf0200000u, 0x00100000u },
    [ANDES_AE350_UART2]     = { 0xf0300000u, 0x00100000u },
    [ANDES_AE350_PIT]       = { 0xf0400000u, 0x00100000u },
    [ANDES_AE350_SDC]       = { 0xf0e00000u, 0x00100000u },
    [ANDES_AE350_VIRTIO]    = { 0xfe000000u, 0x00001000u },
};

#define ATFMAC_REVISION     0x10070109
#define GEM_REVISION        0x10070109

static char *ae350_get_plic(Object *obj, Error **errp)
{
    AndesAe350MachineState *s = (AndesAe350MachineState*)obj;
    return g_strdup(s->plic_targets);
}

static void ae350_set_plic(Object *obj, const char *value, Error **errp)
{
    char *p;
    AndesAe350MachineState *s = (AndesAe350MachineState*)obj;

    g_free(s->plic_targets);
    s->plic_targets = g_strdup(value);

    for (p = s->plic_targets; *p; ++p) {
        if (*p == ';') {
            *p = ',';
        }
    }
}

static char *ae350_get_plicsw(Object *obj, Error **errp)
{
    AndesAe350MachineState *s = (AndesAe350MachineState*)obj;
    return g_strdup(s->plicsw_targets);
}

static void ae350_set_plicsw(Object *obj, const char *value, Error **errp)
{
    char *p;
    AndesAe350MachineState *s = (AndesAe350MachineState*)obj;

    g_free(s->plicsw_targets);
    s->plicsw_targets = g_strdup(value);

    for (p = s->plicsw_targets; *p; ++p) {
        if (*p == ';') {
            *p = ',';
        }
    }
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

static void create_fdt(AndesAe350BoardState *s, const struct MemmapEntry *memmap,
    uint64_t mem_size, const char *cmdline)
{
    void *fdt;
    int cpu;
    uint32_t *cells;
    char *nodename;
    uint32_t plic_phandle;

    fdt = s->fdt = create_device_tree(&s->fdt_size);
    if (!fdt) {
        error_report("create_device_tree() failed");
        exit(1);
    }

    qemu_fdt_setprop_string(fdt, "/", "model", "ucbbar,spike-bare,qemu");
    qemu_fdt_setprop_string(fdt, "/", "compatible", "ucbbar,spike-bare-dev");
    qemu_fdt_setprop_cell(fdt, "/", "#size-cells", 0x2);
    qemu_fdt_setprop_cell(fdt, "/", "#address-cells", 0x2);

    qemu_fdt_add_subnode(fdt, "/soc");
    qemu_fdt_setprop(fdt, "/soc", "ranges", NULL, 0);
    qemu_fdt_setprop_string(fdt, "/soc", "compatible", "simple-bus");
    qemu_fdt_setprop_cell(fdt, "/soc", "#size-cells", 0x2);
    qemu_fdt_setprop_cell(fdt, "/soc", "#address-cells", 0x2);

    nodename = g_strdup_printf("/memory@%lx",
        (long)memmap[ANDES_AE350_DRAM].base);
    qemu_fdt_add_subnode(fdt, nodename);
    qemu_fdt_setprop_cells(fdt, nodename, "reg",
        memmap[ANDES_AE350_DRAM].base >> 32, memmap[ANDES_AE350_DRAM].base,
        mem_size >> 32, mem_size);
    qemu_fdt_setprop_string(fdt, nodename, "device_type", "memory");
    g_free(nodename);

    qemu_fdt_add_subnode(fdt, "/cpus");
    qemu_fdt_setprop_cell(fdt, "/cpus", "timebase-frequency",
        ANDES_PLMT_TIMEBASE_FREQ);
    qemu_fdt_setprop_cell(fdt, "/cpus", "#size-cells", 0x0);
    qemu_fdt_setprop_cell(fdt, "/cpus", "#address-cells", 0x1);

    for (cpu = s->soc.cpus.num_harts - 1; cpu >= 0; cpu--) {
        nodename = g_strdup_printf("/cpus/cpu@%d", cpu);
        char *intc = g_strdup_printf("/cpus/cpu@%d/interrupt-controller", cpu);
        char *isa = riscv_isa_string(&s->soc.cpus.harts[cpu]);
        qemu_fdt_add_subnode(fdt, nodename);
        qemu_fdt_setprop_cell(fdt, nodename, "clock-frequency",
                              ANDES_AE350_CLOCK_FREQ);
        qemu_fdt_setprop_string(fdt, nodename, "mmu-type", "riscv,sv48");
        qemu_fdt_setprop_string(fdt, nodename, "riscv,isa", isa);
        qemu_fdt_setprop_string(fdt, nodename, "compatible", "riscv");
        qemu_fdt_setprop_string(fdt, nodename, "status", "okay");
        qemu_fdt_setprop_cell(fdt, nodename, "reg", cpu);
        qemu_fdt_setprop_string(fdt, nodename, "device_type", "cpu");
        qemu_fdt_add_subnode(fdt, intc);
        qemu_fdt_setprop_cell(fdt, intc, "phandle", 1);
        qemu_fdt_setprop_cell(fdt, intc, "linux,phandle", 1);
        qemu_fdt_setprop_string(fdt, intc, "compatible", "riscv,cpu-intc");
        qemu_fdt_setprop(fdt, intc, "interrupt-controller", NULL, 0);
        qemu_fdt_setprop_cell(fdt, intc, "#interrupt-cells", 1);
        g_free(isa);
        g_free(intc);
        g_free(nodename);
    }

    cells =  g_new0(uint32_t, s->soc.cpus.num_harts * 4);
    for (cpu = 0; cpu < s->soc.cpus.num_harts; cpu++) {
        nodename =
            g_strdup_printf("/cpus/cpu@%d/interrupt-controller", cpu);
        uint32_t intc_phandle = qemu_fdt_get_phandle(fdt, nodename);
        cells[cpu * 4 + 0] = cpu_to_be32(intc_phandle);
        cells[cpu * 4 + 1] = cpu_to_be32(IRQ_M_SOFT);
        cells[cpu * 4 + 2] = cpu_to_be32(intc_phandle);
        cells[cpu * 4 + 3] = cpu_to_be32(IRQ_M_TIMER);
        g_free(nodename);
    }
/*    
    nodename = g_strdup_printf("/soc/clint@%lx",
        (long)memmap[ANDES_AE350_CLINT].base);
    qemu_fdt_add_subnode(fdt, nodename);
    qemu_fdt_setprop_string(fdt, nodename, "compatible", "riscv,clint0");
    qemu_fdt_setprop_cells(fdt, nodename, "reg",
        0x0, memmap[ANDES_AE350_CLINT].base,
        0x0, memmap[ANDES_AE350_CLINT].size);
    qemu_fdt_setprop(fdt, nodename, "interrupts-extended",
        cells, s->soc.cpus.num_harts * sizeof(uint32_t) * 4);
    g_free(cells);
    g_free(nodename);
*/
    cells =  g_new0(uint32_t, s->soc.cpus.num_harts * 4);
    for (cpu = 0; cpu < s->soc.cpus.num_harts; cpu++) {
        nodename =
            g_strdup_printf("/cpus/cpu@%d/interrupt-controller", cpu);
        uint32_t intc_phandle = qemu_fdt_get_phandle(fdt, nodename);
        cells[cpu * 4 + 0] = cpu_to_be32(intc_phandle);
        cells[cpu * 4 + 1] = cpu_to_be32(IRQ_M_EXT);
        cells[cpu * 4 + 2] = cpu_to_be32(intc_phandle);
        cells[cpu * 4 + 3] = cpu_to_be32(IRQ_S_EXT);
        g_free(nodename);
    }
    nodename = g_strdup_printf("/soc/interrupt-controller@%lx",
        (long)memmap[ANDES_AE350_PLIC].base);
    qemu_fdt_add_subnode(fdt, nodename);
    qemu_fdt_setprop_cell(fdt, nodename, "#interrupt-cells", 1);
    qemu_fdt_setprop_string(fdt, nodename, "compatible", "riscv,plic0");
    qemu_fdt_setprop(fdt, nodename, "interrupt-controller", NULL, 0);
    qemu_fdt_setprop(fdt, nodename, "interrupts-extended",
        cells, s->soc.cpus.num_harts * sizeof(uint32_t) * 4);
    qemu_fdt_setprop_cells(fdt, nodename, "reg",
        0x0, memmap[ANDES_AE350_PLIC].base,
        0x0, memmap[ANDES_AE350_PLIC].size);
    qemu_fdt_setprop_string(fdt, nodename, "reg-names", "control");
    qemu_fdt_setprop_cell(fdt, nodename, "riscv,max-priority", 7);
    qemu_fdt_setprop_cell(fdt, nodename, "riscv,ndev", 0x35);
    qemu_fdt_setprop_cells(fdt, nodename, "phandle", 2);
    qemu_fdt_setprop_cells(fdt, nodename, "linux,phandle", 2);
    plic_phandle = qemu_fdt_get_phandle(fdt, nodename);
    g_free(cells);
    g_free(nodename);

    nodename = g_strdup_printf("/soc/ethernet@%lx",
        (long)memmap[ANDES_AE350_GEM].base);
    qemu_fdt_add_subnode(fdt, nodename);
    qemu_fdt_setprop_string(fdt, nodename, "compatible", "cdns,macb");
    qemu_fdt_setprop_cells(fdt, nodename, "reg",
        0x0, memmap[ANDES_AE350_GEM].base,
        0x0, memmap[ANDES_AE350_GEM].size);
    qemu_fdt_setprop_string(fdt, nodename, "reg-names", "control");
    qemu_fdt_setprop_string(fdt, nodename, "phy-mode", "gmii");
    qemu_fdt_setprop_cells(fdt, nodename, "interrupt-parent", plic_phandle);
    qemu_fdt_setprop_cells(fdt, nodename, "interrupts", ANDES_AE350_GEM_IRQ);
    qemu_fdt_setprop_cells(fdt, nodename, "#address-cells", 1);
    qemu_fdt_setprop_cells(fdt, nodename, "#size-cells", 0);
    g_free(nodename);

    nodename = g_strdup_printf("/soc/ethernet@%lx/ethernet-phy@0",
        (long)memmap[ANDES_AE350_GEM].base);
    qemu_fdt_add_subnode(fdt, nodename);
    qemu_fdt_setprop_cells(fdt, nodename, "reg", 0x0);
    g_free(nodename);

    nodename = g_strdup_printf("/soc/uart@%lx",
        (long)memmap[ANDES_AE350_UART2].base);
    qemu_fdt_add_subnode(fdt, nodename);
    qemu_fdt_setprop_string(fdt, nodename, "compatible", "sifive,uart0");
    qemu_fdt_setprop_cells(fdt, nodename, "reg",
        0x0, memmap[ANDES_AE350_UART2].base,
        0x0, memmap[ANDES_AE350_UART2].size);
    qemu_fdt_setprop_cells(fdt, nodename, "interrupt-parent", plic_phandle);
    qemu_fdt_setprop_cells(fdt, nodename, "interrupts", 1);

    qemu_fdt_add_subnode(fdt, "/chosen");
    qemu_fdt_setprop_string(fdt, "/chosen", "stdout-path", nodename);
    qemu_fdt_setprop_string(fdt, "/chosen", "bootargs", cmdline);
    g_free(nodename);
}

static void andes_ae350_machine_init(MachineState *machine)
{
    const struct MemmapEntry *memmap = andes_ae350_memmap;

    AndesAe350BoardState *s = g_new0(AndesAe350BoardState, 1);
    s->machine = (AndesAe350MachineState*)machine;
    MemoryRegion *system_memory = get_system_memory();
    MemoryRegion *main_mem = g_new(MemoryRegion, 1);
    target_ulong entry = memmap[ANDES_AE350_DRAM].base;
    int i;

    /* Initialize SoC */
    object_initialize_child(OBJECT(machine), "soc", &s->soc,
                            sizeof(s->soc), TYPE_ANDES_AE350_SOC,
                            &error_abort, NULL);
    object_property_set_bool(OBJECT(&s->soc), true, "realized",
                            &error_abort);

    /* register RAM */
    memory_region_init_ram(main_mem, NULL, "riscv.andes.ae350.ram",
                           machine->ram_size, &error_fatal);
    memory_region_add_subregion(system_memory, memmap[ANDES_AE350_DRAM].base,
                                main_mem);

    /* create device tree */
    if (machine->dtb) {
        s->fdt = load_device_tree(machine->dtb, &s->fdt_size);
    }
    if (!s->fdt) {
        create_fdt(s, memmap, machine->ram_size, machine->kernel_cmdline);
    }

    if (machine->kernel_filename) {
        entry = load_kernel(machine->kernel_filename);
    }

    /* reset vector */
    uint32_t reset_vec[8] = {
        0x00000297,                    /* 1:  auipc  t0, %pcrel_hi(dtb) */
        0x02028593,                    /*     addi   a1, t0, %pcrel_lo(1b) */
        0xf1402573,                    /*     csrr   a0, mhartid  */
#if defined(TARGET_RISCV32)
        0x0182a283,                    /*     lw     t0, 24(t0) */
#elif defined(TARGET_RISCV64)
        0x0182b283,                    /*     ld     t0, 24(t0) */
#else
#error "Unsupported target!"
#endif
        0x00028067,                    /*     jr     t0 */
        0x00000000,
        entry,                          /* start: .dword DRAM_BASE */
#if defined(TARGET_RISCV32)
        0x00000000,
#else
        entry >> 32,
#endif
                                       /* dtb: */
    };

    /* copy in the reset vector in little_endian byte order */
    for (i = 0; i < sizeof(reset_vec) >> 2; i++) {
        reset_vec[i] = cpu_to_le32(reset_vec[i]);
    }
    rom_add_blob_fixed_as("mrom.reset", reset_vec, sizeof(reset_vec),
                          memmap[ANDES_AE350_MROM].base, &address_space_memory);

    /* copy in the device tree */
    if (fdt_pack(s->fdt) || fdt_totalsize(s->fdt) >
            memmap[ANDES_AE350_MROM].size - sizeof(reset_vec)) {
        error_report("not enough space to store device-tree");
        exit(1);
    }
    qemu_fdt_dumpdtb(s->fdt, fdt_totalsize(s->fdt));
    rom_add_blob_fixed_as("mrom.fdt", s->fdt, fdt_totalsize(s->fdt),
                          memmap[ANDES_AE350_MROM].base + sizeof(reset_vec),
                          &address_space_memory);
}

static void andes_ae350_soc_instance_init(Object *obj)
{
    AndesAe350SocState *s = ANDES_AE350_SOC(obj);

    object_initialize_child(obj, "cpus", &s->cpus, sizeof(s->cpus),
                            TYPE_RISCV_HART_ARRAY, &error_abort, NULL);
    object_property_set_str(OBJECT(&s->cpus), ANDES_AE350_CPU, "cpu-type",
                            &error_abort);
    object_property_set_int(OBJECT(&s->cpus), smp_cpus, "num-harts",
                            &error_abort);
#ifdef USE_NIC_ATFMAC100
    sysbus_init_child_obj(obj, "atfmac", &s->atfmac, sizeof(s->atfmac),
                          TYPE_ATFMAC100);
#endif
#ifdef USE_NIC_CADENCE_GEM
    sysbus_init_child_obj(obj, "gem", &s->gem, sizeof(s->gem),
                          TYPE_CADENCE_GEM);
#endif
}

static void andes_ae350_soc_realize(DeviceState *dev, Error **errp)
{
    AndesAe350SocState *s = ANDES_AE350_SOC(dev);
    AndesAe350BoardState *bs = container_of(s, AndesAe350BoardState, soc);
    AndesAe350MachineState *ms = bs->machine;
    const struct MemmapEntry *memmap = andes_ae350_memmap;
    MemoryRegion *system_memory = get_system_memory();
    MemoryRegion *mask_rom = g_new(MemoryRegion, 1);
    qemu_irq plic_gpios[ANDES_AE350_PLIC_NUM_SOURCES];
    int i;
    Error *err = NULL;
    NICInfo *nd = &nd_table[0];

    DeviceState *ds;
    SiFivePLICState *splic;
    SiFivePLICState *splicsw;
    int quit = 0;

    object_property_set_bool(OBJECT(&s->cpus), true, "realized",
                             &error_abort);

    /* boot rom */
    memory_region_init_rom(mask_rom, NULL, "riscv.andes.ae350.mrom",
                           memmap[ANDES_AE350_MROM].size, &error_fatal);
    memory_region_add_subregion(system_memory, memmap[ANDES_AE350_MROM].base,
                                mask_rom);

    /* MMIO */
    /* PLIC */
    /* printf("%s: plic_targets '%s'\n", __func__, ms->plic_targets); */
    s->plic = andes_plic_create(
        memmap[ANDES_AE350_PLIC].base, ms->plic_targets,
        ANDES_AE350_PLIC_NUM_SOURCES, ANDES_AE350_PLIC_NUM_PRIORITIES,
        ANDES_AE350_PLIC_PRIORITY_BASE, ANDES_AE350_PLIC_PENDING_BASE,
        ANDES_AE350_PLIC_ENABLE_BASE, ANDES_AE350_PLIC_ENABLE_STRIDE,
        ANDES_AE350_PLIC_CONTEXT_BASE, ANDES_AE350_PLIC_CONTEXT_STRIDE,
        memmap[ANDES_AE350_PLIC].size);
    splic = SIFIVE_PLIC(s->plic);

    for (i = 0; i < ANDES_AE350_PLIC_NUM_SOURCES; i++) {
        plic_gpios[i] = qdev_get_gpio_in(DEVICE(s->plic), i);
    }

    /* SWINT */
    /* printf("%s: plicsw_targets '%s'\n", __func__, ms->plicsw_targets); */
    ds = andes_plic_swint_create(
        memmap[ANDES_AE350_SWINT].base, ms->plicsw_targets,
        ANDES_AE350_PLICSW_NUM_SOURCES, ANDES_AE350_PLICSW_NUM_PRIORITIES,
        ANDES_AE350_PLICSW_PRIORITY_BASE, ANDES_AE350_PLICSW_PENDING_BASE,
        ANDES_AE350_PLICSW_ENABLE_BASE, ANDES_AE350_PLICSW_ENABLE_STRIDE,
        ANDES_AE350_PLICSW_CONTEXT_BASE, ANDES_AE350_PLICSW_CONTEXT_STRIDE,
        memmap[ANDES_AE350_SWINT].size, MIP_MSIP, MIP_SSIP);
    splicsw = SIFIVE_PLIC(ds);

    andes_plmt_create(memmap[ANDES_AE350_PLMT].base, memmap[ANDES_AE350_PLMT].size,
                      smp_cpus, ANDES_PLMT_TIME_BASE, ANDES_PLMT_TIMECMP_BASE);
    atcuart100_create(system_memory, memmap[ANDES_AE350_UART1].base,
                        plic_gpios[ANDES_AE350_UART1_IRQ], 115200,
                        serial_hd(1));
    atcuart100_create(system_memory, memmap[ANDES_AE350_UART2].base,
                        plic_gpios[ANDES_AE350_UART2_IRQ], 115200,
                        serial_hd(0));

    atcpit100_create(memmap[ANDES_AE350_PIT].base,
                     plic_gpios[ANDES_AE350_PIT_IRQ]);

    atfsdc010_create(memmap[ANDES_AE350_SDC].base,
                     plic_gpios[ANDES_AE350_SDC_IRQ]);

    /* NIC */
#ifdef USE_NIC_ATFMAC100
    // atfmac100_create(OBJECT(ms), "atfmac100",
    //                  nd, memmap[ANDES_AE350_MAC].base,
    //                  plic_gpios[ANDES_AE350_MAC_IRQ]);
    /* NIC */
    if (nd->used) {
        qemu_check_nic_model(nd, TYPE_ATFMAC100);
        qdev_set_nic_properties(DEVICE(&s->atfmac), nd);
    }
    object_property_set_int(OBJECT(&s->atfmac), ATFMAC_REVISION, "revision",
                            &error_abort);
    object_property_set_bool(OBJECT(&s->atfmac), true, "realized", &err);
    if (err) {
        error_propagate(errp, err);
        return;
    }
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->atfmac), 0, memmap[ANDES_AE350_MAC].base);
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->atfmac), 0,
                       plic_gpios[ANDES_AE350_MAC]);
#endif
#ifdef USE_NIC_CADENCE_GEM
    if (nd->used) {
        qemu_check_nic_model(nd, TYPE_CADENCE_GEM);
        qdev_set_nic_properties(DEVICE(&s->gem), nd);
    }
    object_property_set_int(OBJECT(&s->gem), GEM_REVISION, "revision",
                            &error_abort);
    object_property_set_bool(OBJECT(&s->gem), true, "realized", &err);
    if (err) {
        error_propagate(errp, err);
        return;
    }
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->gem), 0, memmap[ANDES_AE350_GEM].base);
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->gem), 0,
                       plic_gpios[ANDES_AE350_GEM_IRQ]);
#endif

    /* VIRTIO */
    for (i = 0; i < ANDES_AE350_VIRTIO_COUNT; i++) {
        sysbus_create_simple("virtio-mmio",
            memmap[ANDES_AE350_VIRTIO].base + i * memmap[ANDES_AE350_VIRTIO].size,
            plic_gpios[ANDES_AE350_VIRTIO_IRQ + i]);
    }

    /* sanity-check smp_cpus and plicX max target hart_id */
    if (smp_cpus > (1 + splic->addr_config[splic->num_addrs - 1].hartid)) {
        error_report("Invalid PLIC targets '%s' < %d-SMP-CPUS",
            ms->plic_targets, smp_cpus);
        quit = 1;
    }
    if (smp_cpus > (1 + splicsw->addr_config[splicsw->num_addrs - 1].hartid)) {
        error_report("Invalid PLICSW targets '%s' < %d-SMP-CPUS",
            ms->plicsw_targets, smp_cpus);
        quit = 1;
    }
    if (quit) {
        exit(1);
    }
}

static void andes_ae350_machine_class_init_cb_instance_init(Object *obj)
{
    AndesAe350MachineState *s = (AndesAe350MachineState*)obj;
    assert(!s->plic_targets && !s->plicsw_targets);
    s->plic_targets = g_strdup(ANDES_AE350_PLIC_HART_CONFIG);
    s->plicsw_targets = g_strdup(ANDES_AE350_PLICSW_HART_CONFIG);

    object_property_add_str(obj, "plic-targets", ae350_get_plic, ae350_set_plic, NULL);
    object_property_set_description(obj, "plic", "PLIC configuration string", NULL);
    object_property_add_str(obj, "plicsw-targets", ae350_get_plicsw, ae350_set_plicsw, NULL);
    object_property_set_description(obj, "plicsw", "PLICSW configuration string", NULL);
}

static void andes_ae350_machine_class_init_cb(MachineClass *mc)
{
    mc->desc = "RISC-V Board compatible with Andes AE350 Platform";
    mc->init = andes_ae350_machine_init;
    mc->max_cpus = 4;
    mc->default_cpus = 2;
}

DEFINE_MACHINE_EX("andes_ae350", andes_ae350_machine_class_init_cb)

static void andes_ae350_soc_class_init(ObjectClass *oc, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(oc);

    dc->realize = andes_ae350_soc_realize;
    /* Reason: Uses serial_hds in realize function, thus can't be used twice */
    dc->user_creatable = false;
}

static const TypeInfo andes_ae350_soc_type_info = {
    .name = TYPE_ANDES_AE350_SOC,
    .parent = TYPE_DEVICE,
    .instance_size = sizeof(AndesAe350SocState),
    .instance_init = andes_ae350_soc_instance_init,
    .class_init = andes_ae350_soc_class_init,
};

static void andes_ae350_soc_register_types(void)
{
    type_register_static(&andes_ae350_soc_type_info);
}

type_init(andes_ae350_soc_register_types)
