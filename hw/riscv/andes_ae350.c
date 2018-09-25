/*
 * QEMU RISC-V Board Compatible with Andes AE350 platform
 *
 * Copyright (c) 2018 Andes Tech. Corp.
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

// clang-format off
#include "qemu/osdep.h"
#include "qemu/error-report.h"
#include "qemu/log.h"
#include "qemu/option.h"
#include "qapi/error.h"
#include "elf.h"
#include "exec/address-spaces.h"
#include "sysemu/arch_init.h"
#include "sysemu/device_tree.h"
#include "target/riscv/cpu.h"
#include "chardev/char.h"
#include "hw/hw.h"
#include "hw/sysbus.h"
#include "hw/loader.h"
#include "hw/boards.h"
#include "hw/char/serial.h"
#include "hw/char/atcuart100.h"
#include "hw/sd/atfsdc010.h"
#include "hw/timer/atcpit100.h"
#include "hw/riscv/andes_plic.h"
#include "hw/riscv/andes_plmt.h"
#include "hw/riscv/riscv_hart.h"
#include "hw/riscv/andes_ae350.h"
// clang-format on

typedef struct {
    MachineClass parent;
} Ae350MachineClass;

typedef struct {
    MachineState parent;
    char *plic_targets;
    char *Ae350MachineState;
} Ae350MachineState;

#define TYPE_AE350_MACHINE MACHINE_TYPE_NAME("andes_ae350")
#define AE350_MACHINE(obj)                                                      \
    OBJECT_CHECK(Ae350MachineState, (obj), TYPE_AE350_MACHINE)
#define AE350_MACHINE_GET_CLASS(obj)                                            \
    OBJECT_GET_CLASS(Ae350MachineClass, obj, TYPE_AE350_MACHINE)
#define AE350_MACHINE_CLASS(klass)                                              \
    OBJECT_CLASS_CHECK(Ae350MachineClass, klass, TYPE_AE350_MACHINE)

static const struct MemmapEntry {
    hwaddr base;
    hwaddr size;
} ae350_machine_memmap[] = {
    // clang-format off
    [AE350_MM_DRAM]       = {0x00000000u, 0x80000000u},
    [AE350_MM_BOOTROM]    = {0x80000000u, 0x00010000u},
    [AE350_MM_MROM]       = {0x80001000u, 0x00002000u},
    [AE350_MM_MAC]        = {0xe0100000u, 0x00100000u},
    [AE350_MM_PLIC]       = {0xe4000000u, 0x00400000u},
    [AE350_MM_PLMT]       = {0xe6000000u, 0x00100000u},
    [AE350_MM_SWINT]      = {0xe6400000u, 0x00400000u},
    [AE350_MM_UART1]      = {0xf0200000u, 0x00100000u},
    [AE350_MM_UART2]      = {0xf0300000u, 0x00100000u},
    [AE350_MM_PIT]        = {0xf0400000u, 0x00100000u},
    [AE350_MM_SDC]        = {0xf0e00000u, 0x00100000u},
    [AE350_MM_VIRTIO]     = {0xfe000000u, 0x00001000u},
    [AE350_MM_DEBUG]      = {0xffff0000u, 0x00000100u},
    // clang-format on
};

static uint64_t
identity_translate(void *opaque, uint64_t addr)
{
    return addr;
}

static uint64_t
load_kernel(const char *kernel_filename)
{
    uint64_t kernel_entry, kernel_high;

    if (load_elf(kernel_filename, identity_translate, NULL, &kernel_entry, NULL,
                 &kernel_high,
                 /* little_endian = */ 0, ELF_MACHINE, 1, 0) < 0) {
        error_report("qemu: could not load kernel '%s'", kernel_filename);
        exit(1);
    }
    return kernel_entry;
}

static void
create_fdt(Ae350BoardState *s, const struct MemmapEntry *memmap,
           uint64_t mem_size, const char *cmdline)
{
    void *fdt;
    int cpu, i;
    uint32_t *cells;
    char *nodename;
    uint32_t plic_phandle;

    fdt = s->fdt = create_device_tree(&s->fdt_size);
    if (!fdt) {
        error_report("create_device_tree() failed");
        exit(1);
    }

    qemu_fdt_setprop_string(fdt, "/", "model", "andestech,ae350");
    qemu_fdt_setprop_string(fdt, "/", "compatible", "andestech,ae350");
    qemu_fdt_setprop_cell(fdt, "/", "#size-cells", 0x2);
    qemu_fdt_setprop_cell(fdt, "/", "#address-cells", 0x2);

    qemu_fdt_add_subnode(fdt, "/soc");
    qemu_fdt_setprop(fdt, "/soc", "ranges", NULL, 0);
    qemu_fdt_setprop_string(fdt, "/soc", "compatible", "ucbbar,spike-bare-soc");
    qemu_fdt_appendprop_string(fdt, "/soc", "compatible", "simple-bus");
    qemu_fdt_setprop_cell(fdt, "/soc", "#size-cells", 0x2);
    qemu_fdt_setprop_cell(fdt, "/soc", "#address-cells", 0x2);

    nodename =
        g_strdup_printf("/memory@%lx", (long)memmap[AE350_MM_DRAM].base);
    qemu_fdt_add_subnode(fdt, nodename);
    qemu_fdt_setprop_cells(
        fdt, nodename, "reg", memmap[AE350_MM_DRAM].base >> 32,
        memmap[AE350_MM_DRAM].base, mem_size >> 32, mem_size);
    qemu_fdt_setprop_string(fdt, nodename, "device_type", "memory");
    g_free(nodename);

    qemu_fdt_add_subnode(fdt, "/cpus");
    qemu_fdt_setprop_cell(fdt, "/cpus", "timebase-frequency", 10000000);
    qemu_fdt_setprop_cell(fdt, "/cpus", "#size-cells", 0x0);
    qemu_fdt_setprop_cell(fdt, "/cpus", "#address-cells", 0x1);

    for (cpu = s->soc.num_harts - 1; cpu >= 0; cpu--) {
        nodename = g_strdup_printf("/cpus/cpu@%d", cpu);
        char *intc = g_strdup_printf("/cpus/cpu@%d/interrupt-controller", cpu);
        char *isa = riscv_isa_string(&s->soc.harts[cpu]);
        qemu_fdt_add_subnode(fdt, nodename);
        qemu_fdt_setprop_cell(fdt, nodename, "clock-frequency", 60000000);
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

    cells = g_new0(uint32_t, s->soc.num_harts * 2);
    for (cpu = 0; cpu < s->soc.num_harts; cpu++) {
        nodename = g_strdup_printf("/cpus/cpu@%d/interrupt-controller", cpu);
        uint32_t intc_phandle = qemu_fdt_get_phandle(fdt, nodename);
        cells[cpu * 2 + 0] = cpu_to_be32(intc_phandle);
        cells[cpu * 2 + 1] = cpu_to_be32(IRQ_M_TIMER);
        g_free(nodename);
    }
    nodename =
        g_strdup_printf("/soc/plmt0@%lx", (long)memmap[AE350_MM_PLMT].base);
    qemu_fdt_add_subnode(fdt, nodename);
    qemu_fdt_setprop_string(fdt, nodename, "compatible", "riscv,plmt0");
    qemu_fdt_setprop_cells(fdt, nodename, "reg", 0x0,
                           memmap[AE350_MM_PLMT].base, 0x0,
                           memmap[AE350_MM_PLMT].size);
    qemu_fdt_setprop(fdt, nodename, "interrupts-extended", cells,
                     s->soc.num_harts * sizeof(uint32_t) * 2);
    g_free(cells);
    g_free(nodename);

    cells = g_new0(uint32_t, s->soc.num_harts * 2);
    for (cpu = 0; cpu < s->soc.num_harts; cpu++) {
        nodename = g_strdup_printf("/cpus/cpu@%d/interrupt-controller", cpu);
        uint32_t intc_phandle = qemu_fdt_get_phandle(fdt, nodename);
        cells[cpu * 2 + 0] = cpu_to_be32(intc_phandle);
        cells[cpu * 2 + 1] = cpu_to_be32(IRQ_M_SOFT);
        g_free(nodename);
    }
    nodename =
        g_strdup_printf("/soc/swint0@%lx", (long)memmap[AE350_MM_SWINT].base);
    qemu_fdt_add_subnode(fdt, nodename);
    qemu_fdt_setprop_string(fdt, nodename, "compatible", "riscv,swint0");
    qemu_fdt_setprop_cells(fdt, nodename, "reg", 0x0,
                           memmap[AE350_MM_SWINT].base, 0x0,
                           memmap[AE350_MM_SWINT].size);
    qemu_fdt_setprop(fdt, nodename, "interrupts-extended", cells,
                     s->soc.num_harts * sizeof(uint32_t) * 2);
    g_free(cells);
    g_free(nodename);

    cells = g_new0(uint32_t, s->soc.num_harts * 4);
    for (cpu = 0; cpu < s->soc.num_harts; cpu++) {
        nodename = g_strdup_printf("/cpus/cpu@%d/interrupt-controller", cpu);
        uint32_t intc_phandle = qemu_fdt_get_phandle(fdt, nodename);
        cells[cpu * 4 + 0] = cpu_to_be32(intc_phandle);
        cells[cpu * 4 + 1] = cpu_to_be32(IRQ_M_EXT);
        cells[cpu * 4 + 2] = cpu_to_be32(intc_phandle);
        cells[cpu * 4 + 3] = cpu_to_be32(IRQ_S_EXT);
        g_free(nodename);
    }
    nodename = g_strdup_printf("/soc/interrupt-controller@%lx",
                               (long)memmap[AE350_MM_PLIC].base);
    qemu_fdt_add_subnode(fdt, nodename);
    qemu_fdt_setprop_cell(fdt, nodename, "#interrupt-cells", 2);
    qemu_fdt_setprop_string(fdt, nodename, "compatible", "riscv,plic0");
    qemu_fdt_setprop(fdt, nodename, "interrupt-controller", NULL, 0);
    qemu_fdt_setprop(fdt, nodename, "interrupts-extended", cells,
                     s->soc.num_harts * sizeof(uint32_t) * 4);
    qemu_fdt_setprop_cells(fdt, nodename, "reg", 0x0,
                           memmap[AE350_MM_PLIC].base, 0x0,
                           memmap[AE350_MM_PLIC].size);
    qemu_fdt_setprop_string(fdt, nodename, "reg-names", "control");
    qemu_fdt_setprop_cell(fdt, nodename, "riscv,max-priority", 7);
    qemu_fdt_setprop_cell(fdt, nodename, "riscv,ndev", 31);
    qemu_fdt_setprop_cell(fdt, nodename, "phandle", 2);
    qemu_fdt_setprop_cell(fdt, nodename, "linux,phandle", 2);
    plic_phandle = qemu_fdt_get_phandle(fdt, nodename);
    g_free(cells);
    g_free(nodename);

    nodename =
        g_strdup_printf("/soc/serial@%lx", (long)memmap[AE350_MM_UART2].base);
    qemu_fdt_add_subnode(fdt, nodename);
    qemu_fdt_setprop_string(fdt, nodename, "compatible", "andestech,uart16550");
    qemu_fdt_appendprop_string(fdt, nodename, "compatible", "ns16550a");
    qemu_fdt_setprop_cells(fdt, nodename, "reg", 0x0,
                           memmap[AE350_MM_UART2].base, 0x0,
                           memmap[AE350_MM_UART2].size);
    qemu_fdt_setprop_cells(fdt, nodename, "interrupt-parent", plic_phandle);
    qemu_fdt_setprop_cells(fdt, nodename, "interrupts", 9, 4);
    qemu_fdt_setprop_cells(fdt, nodename, "reg-shift", 2);
    qemu_fdt_setprop_cells(fdt, nodename, "reg-offset", 32);
    qemu_fdt_setprop_cells(fdt, nodename, "no-loopback-test", 1);
    qemu_fdt_setprop_cells(fdt, nodename, "clock-frequency", 19660800);
    g_free(nodename);

    for (i = 0; i < AE350_VIRTIO_COUNT; ++i) {
        nodename = g_strdup_printf("/virtio_mmio@%lx",
                                   (long)(memmap[AE350_MM_VIRTIO].base +
                                          i * memmap[AE350_MM_VIRTIO].size));
        qemu_fdt_add_subnode(fdt, nodename);
        qemu_fdt_setprop_string(fdt, nodename, "compatible", "virtio,mmio");
        qemu_fdt_setprop_cells(fdt, nodename, "reg", 0x0,
                               memmap[AE350_MM_VIRTIO].base +
                                   i * memmap[AE350_MM_VIRTIO].size,
                               0x0, memmap[AE350_MM_VIRTIO].size);
        qemu_fdt_setprop_cells(fdt, nodename, "interrupt-parent", plic_phandle);
        qemu_fdt_setprop_cells(fdt, nodename, "interrupts",
                               AE350_VIRTIO_IRQ + i, 4);
        g_free(nodename);
    }

    qemu_fdt_add_subnode(fdt, "/chosen");
    qemu_fdt_setprop_string(fdt, "/chosen", "stdout-path", "uart0:38400n8");
    qemu_fdt_setprop_string(fdt, "/chosen", "bootargs", cmdline);
}

static void
ae350_machine_init(MachineState *machine)
{
    const struct MemmapEntry *memmap = ae350_machine_memmap;

    Ae350MachineState *nms = AE350_MACHINE(machine);
    Ae350BoardState *s = g_new0(Ae350BoardState, 1);
    MemoryRegion *sys_memory = get_system_memory();
    MemoryRegion *main_mem = g_new(MemoryRegion, 1);
    MemoryRegion *boot_rom = g_new(MemoryRegion, 1);
    DeviceState *ds;
    SiFivePLICState *splic;
    SiFivePLICState *splicsw;
    /* start: .dword DRAM_BASE */;
    target_ulong entry = memmap[AE350_MM_DRAM].base;
    int i, quit = 0;

    /* Initialize SOC */
    printf("%s: hart_array '%s' x %d\n", __func__, TYPE_RISCV_CPU_ANDES, smp_cpus);
    object_initialize(&s->soc, sizeof(s->soc), TYPE_RISCV_HART_ARRAY);
    object_property_add_child(OBJECT(machine), "soc", OBJECT(&s->soc),
                              &error_abort);
    object_property_set_str(OBJECT(&s->soc), TYPE_RISCV_CPU_ANDES, "cpu-type",
                            &error_abort);
    object_property_set_int(OBJECT(&s->soc), smp_cpus, "num-harts",
                            &error_abort);
    object_property_set_bool(OBJECT(&s->soc), true, "realized", &error_abort);

    /* register RAM */
    memory_region_init_ram(main_mem, NULL, "riscv.andes.ae350.ram",
                           machine->ram_size, &error_fatal);
    /* for phys mem size check in page table walk */
    memory_region_add_subregion(sys_memory, memmap[AE350_MM_DRAM].base,
                                main_mem);

    /* create device tree */
    if (machine->dtb) {
        s->fdt = load_device_tree(machine->dtb, &s->fdt_size);
    }
    if (!s->fdt) {
        create_fdt(s, memmap, machine->ram_size, machine->kernel_cmdline);
    }

    /* boot rom */
    /* fdt_size = FDT_MAX_SIZE, defined in device_tree.c only!
     * boot_rom size must >= fdt_size + 0x1020
     */
    memory_region_init_ram(boot_rom, NULL, "riscv.andes.ae350.bootrom",
                           s->fdt_size + 0x2000, &error_fatal);
    /* memory_region_set_readonly(boot_rom, true); */
    memory_region_add_subregion(sys_memory, memmap[AE350_MM_BOOTROM].base,
                                boot_rom);

    if (machine->kernel_filename) {
        entry = load_kernel(machine->kernel_filename);
    }

    /* reset vector */
    uint32_t reset_vec[8] = {
        0x00000297, /* 1:  auipc  t0, %pcrel_hi(dtb) */
        0x02028593, /*     addi   a1, t0, %pcrel_lo(1b) */
        0xf1402573, /*     csrr   a0, mhartid  */
#if defined(TARGET_RISCV32)
        0x0182a283, /*     lw     t0, 24(t0) */
#elif defined(TARGET_RISCV64)
        0x0182b283, /*     ld     t0, 24(t0) */
#else
#error "Unsupported target!"
#endif
        0x00028067, /*     jr     t0 */
        0x00000000,
        entry, /* start: .dword DRAM_BASE */
#if defined(TARGET_RISCV32)
        0x00000000,
#else
        entry >> 32,
#endif
        /* dtb: */
    };

    /* copy in the reset vector */
    cpu_physical_memory_write(memmap[AE350_MM_MROM].base, reset_vec,
                              sizeof(reset_vec));

    /* copy in the device tree */
    qemu_fdt_dumpdtb(s->fdt, s->fdt_size);
    cpu_physical_memory_write(memmap[AE350_MM_MROM].base + sizeof(reset_vec),
                              s->fdt, s->fdt_size);

    /* MMIO */
    /* PLIC */
    printf("%s: plic_targets '%s'\n", __func__, nms->plic_targets);
    s->plic = andes_plic_create(
        memmap[AE350_MM_PLIC].base, nms->plic_targets,
        AE350_PLIC_NUM_SOURCES, AE350_PLIC_NUM_PRIORITIES,
        AE350_PLIC_PRIORITY_BASE, AE350_PLIC_PENDING_BASE,
        AE350_PLIC_ENABLE_BASE, AE350_PLIC_ENABLE_STRIDE,
        AE350_PLIC_CONTEXT_BASE, AE350_PLIC_CONTEXT_STRIDE,
        memmap[AE350_MM_PLIC].size);
    splic = SIFIVE_PLIC(s->plic);

    /* SWINT */
    printf("%s: Ae350MachineState '%s'\n", __func__, nms->Ae350MachineState);
    ds = andes_plic_swint_create(
        memmap[AE350_MM_SWINT].base, nms->Ae350MachineState,
        AE350_PLIC_NUM_SOURCES, AE350_PLIC_NUM_PRIORITIES,
        AE350_PLIC_PRIORITY_BASE, AE350_PLIC_PENDING_BASE,
        AE350_PLIC_ENABLE_BASE, AE350_PLIC_ENABLE_STRIDE,
        AE350_PLIC_CONTEXT_BASE, AE350_PLIC_CONTEXT_STRIDE,
        memmap[AE350_MM_SWINT].size, MIP_MSIP, MIP_SSIP);
    splicsw = SIFIVE_PLIC(ds);

    if (serial_hds[1]) {
        atcuart100_create(sys_memory, memmap[AE350_MM_UART1].base,
                          splic->irqs[AE350_UART1_IRQ], 115200,
                          serial_hds[1]);
    }
    if (serial_hds[0]) {
        atcuart100_create(sys_memory, memmap[AE350_MM_UART2].base,
                          splic->irqs[AE350_UART2_IRQ], 115200,
                          serial_hds[0]);
    }

    andes_plmt_create(memmap[AE350_MM_PLMT].base,
                      memmap[AE350_MM_PLMT].size, smp_cpus, ANDES_TIME_BASE,
                      ANDES_TIMECMP_BASE);

    atcpit100_create(memmap[AE350_MM_PIT].base,
                     splic->irqs[AE350_PIT_IRQ]);

    atfsdc010_create(memmap[AE350_MM_SDC].base,
                     splic->irqs[AE350_SDC_IRQ]);

    for (i = 0; i < AE350_VIRTIO_COUNT; i++) {
        sysbus_create_simple("virtio-mmio",
                             memmap[AE350_MM_VIRTIO].base +
                                 i * memmap[AE350_MM_VIRTIO].size,
                             splic->irqs[AE350_VIRTIO_IRQ + i]);
    }

    /* sanity-check smp_cpus and plicX max target hart_id */
    if (smp_cpus > (1 + splic->addr_config[splic->num_addrs - 1].hartid)) {
        error_report("Invalid PLIC targets '%s' < %d-SMP-CPUS",
                     nms->plic_targets, smp_cpus);
        quit = 1;
    }
    if (smp_cpus > (1 + splicsw->addr_config[splicsw->num_addrs - 1].hartid)) {
        error_report("Invalid PLICSW targets '%s' < %d-SMP-CPUS",
                     nms->Ae350MachineState, smp_cpus);
        quit = 1;
    }
    if (quit) {
        exit(1);
    }
}

static char *
ae350_get_plicsw(Object *obj, Error **errp)
{
    Ae350MachineState *nms = AE350_MACHINE(obj);

    return g_strdup(nms->Ae350MachineState);
}

static void
ae350_set_plicsw(Object *obj, const char *value, Error **errp)
{
    Ae350MachineState *nms = AE350_MACHINE(obj);
    char *p;

    g_free(nms->Ae350MachineState);
    nms->Ae350MachineState = g_strdup(value);

    for (p = nms->Ae350MachineState; *p; ++p) {
        if (*p == ';') {
            *p = ',';
        }
    }
}

static char *
ae350_get_plic(Object *obj, Error **errp)
{
    Ae350MachineState *nms = AE350_MACHINE(obj);

    return g_strdup(nms->plic_targets);
}

static void
ae350_set_plic(Object *obj, const char *value, Error **errp)
{
    Ae350MachineState *nms = AE350_MACHINE(obj);
    char *p;

    g_free(nms->plic_targets);
    nms->plic_targets = g_strdup(value);

    for (p = nms->plic_targets; *p; ++p) {
        if (*p == ';') {
            *p = ',';
        }
    }
}

static void
ae350_machine_instance_init(Object *obj)
{
    Ae350MachineState *ams = AE350_MACHINE(obj);

    object_property_add_str(obj, "plic-targets", ae350_get_plic, ae350_set_plic, NULL);
    object_property_set_description(obj, "plic", "PLIC configuration string",
                                    NULL);
    object_property_add_str(obj, "plicsw-targets", ae350_get_plicsw, ae350_set_plicsw,
                            NULL);
    object_property_set_description(obj, "plicsw",
                                    "PLICSW configuration string", NULL);

    ams->plic_targets = g_strdup(AE350_PLIC_HART_CONFIG);
    ams->Ae350MachineState = g_strdup(AE350_PLICSW_HART_CONFIG);
}

static void
ae350_machine_class_init(ObjectClass *oc, void *data)
{
    MachineClass *mc = MACHINE_CLASS(oc);

    mc->desc = "Andes RISC-V AE350 Platform";
    mc->init = ae350_machine_init;
    mc->max_cpus = 4;
    mc->min_cpus = 1;
    mc->default_cpus = 2;
    // mc->ignore_memory_transaction_failures = true;
}

static const TypeInfo ae350_machine_info = {
    .name = TYPE_AE350_MACHINE,
    .parent = TYPE_MACHINE,
    .class_size = sizeof(Ae350MachineClass),
    .class_init = ae350_machine_class_init,
    .instance_size = sizeof(Ae350MachineState),
    .instance_init = ae350_machine_instance_init,
};

static void
ae350_machine_register_types(void)
{
    type_register_static(&ae350_machine_info);
}

type_init(ae350_machine_register_types);

/* AE350 board */
static int
ae350_board_init(SysBusDevice *sysbusdev)
{
    return 0;
}

static void
ae350_board_class_init(ObjectClass *klass, void *data)
{
    SysBusDeviceClass *k = SYS_BUS_DEVICE_CLASS(klass);
    k->init = ae350_board_init;
}

static const TypeInfo ae350_board_info = {
    .name = TYPE_AE350_BOARD,
    .parent = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(Ae350BoardState),
    .class_init = ae350_board_class_init,
};

static void
ae350_board_register_types(void)
{
    type_register_static(&ae350_board_info);
}

type_init(ae350_board_register_types);
