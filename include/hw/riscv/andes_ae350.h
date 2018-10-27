/*
 * Andes AE350 platform machine interface
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

#ifndef HW_ANDES_AE350_H
#define HW_ANDES_AE350_H

#define USE_NIC_ATFMAC100
#undef  USE_NIC_CADENCE_GEM

#include "hw/andes/atfmac100.h"
#include "hw/net/cadence_gem.h"

#define TYPE_ANDES_AE350_SOC "riscv.andes.ae350.soc"
#define ANDES_AE350_SOC(obj) \
    OBJECT_CHECK(AndesAe350SocState, (obj), TYPE_ANDES_AE350_SOC)

typedef struct AndesAe350SocState {
    /*< private >*/
    SysBusDevice parent_obj;

    /*< public >*/
    RISCVHartArrayState cpus;
    DeviceState *plic;
#ifdef USE_NIC_ATFMAC100
    ATFMAC100State atfmac;
#endif
#ifdef USE_NIC_CADENCE_GEM
    CadenceGEMState gem;
#endif
} AndesAe350SocState;

typedef struct AndesAe350MachineState {
    MachineState parent;
    char *plic_targets;
    char *plicsw_targets;
} AndesAe350MachineState;

typedef struct AndesAe350BoardState {
    /*< private >*/
    SysBusDevice parent_obj;

    /*< public >*/
    AndesAe350SocState soc;
    void *fdt;
    int fdt_size;
    AndesAe350MachineState *machine;
} AndesAe350BoardState;

enum {
    ANDES_AE350_DEBUG,
    ANDES_AE350_MROM,
    ANDES_AE350_PLMT,
    ANDES_AE350_SWINT,
    ANDES_AE350_PLIC,
    ANDES_AE350_UART1,
    ANDES_AE350_UART2,
    ANDES_AE350_DRAM,
    ANDES_AE350_GEM,
    ANDES_AE350_PIT,
    ANDES_AE350_SDC,
    ANDES_AE350_MAC,
    ANDES_AE350_VIRTIO,
};

enum {
    ANDES_AE350_PIT_IRQ = 3,
    ANDES_AE350_UART1_IRQ = 8,
    ANDES_AE350_UART2_IRQ = 9,
    ANDES_AE350_SDC_IRQ = 18,
    ANDES_AE350_MAC_IRQ = 19,
    ANDES_AE350_GEM_IRQ = 0x35,
    ANDES_AE350_VIRTIO_COUNT = 8,
    ANDES_AE350_VIRTIO_IRQ = 16, /* 16 to 23 */
};

enum {
    ANDES_AE350_CLOCK_FREQ = 1000000000
};

#define ANDES_AE350_PLIC_HART_CONFIG      "MS,MS"
#define ANDES_AE350_PLIC_NUM_SOURCES      128
#define ANDES_AE350_PLIC_NUM_PRIORITIES   32
#define ANDES_AE350_PLIC_PRIORITY_BASE    0x0
#define ANDES_AE350_PLIC_PENDING_BASE     0x1000
#define ANDES_AE350_PLIC_ENABLE_BASE      0x2000
#define ANDES_AE350_PLIC_ENABLE_STRIDE    0x80
#define ANDES_AE350_PLIC_CONTEXT_BASE     0x200000
#define ANDES_AE350_PLIC_CONTEXT_STRIDE   0x1000

#define ANDES_AE350_PLICSW_HART_CONFIG    "M,M"
#define ANDES_AE350_PLICSW_NUM_SOURCES    64
#define ANDES_AE350_PLICSW_NUM_PRIORITIES 8
#define ANDES_AE350_PLICSW_PRIORITY_BASE  0x0
#define ANDES_AE350_PLICSW_PENDING_BASE   0x1000
#define ANDES_AE350_PLICSW_ENABLE_BASE    0x2000
#define ANDES_AE350_PLICSW_ENABLE_STRIDE  0x80
#define ANDES_AE350_PLICSW_CONTEXT_BASE   0x200000
#define ANDES_AE350_PLICSW_CONTEXT_STRIDE 0x1000


#if defined(TARGET_RISCV32)
#define ANDES_AE350_CPU TYPE_RISCV_CPU_ANDES_A25
#elif defined(TARGET_RISCV64)
#define ANDES_AE350_CPU TYPE_RISCV_CPU_ANDES_AX25
#endif

#define DEFINE_MACHINE_EXT(machine) \
    static const TypeInfo machine##_machine_type_info = { \
        .name       = MACHINE_TYPE_NAME(#machine), \
        .parent     = TYPE_MACHINE, \
        .instance_init = machine##_machine_instance_init, \
        .class_init = machine##_machine_class_init, \
    }; \
    static void machine##_machine_register_types(void) \
    { \
        type_register_static(&machine##_machine_type_info); \
    } \
    type_init(machine##_machine_register_types)

#define DEFINE_MACHINE_EX(namestr, machine_initfn) \
    static void machine_initfn##_class_init(ObjectClass *oc, void *data) \
    { \
        MachineClass *mc = MACHINE_CLASS(oc); \
        machine_initfn(mc); \
    } \
    static const TypeInfo machine_initfn##_typeinfo = { \
        .name       = MACHINE_TYPE_NAME(namestr), \
        .parent     = TYPE_MACHINE, \
        .instance_size = sizeof(AndesAe350MachineState), \
        .instance_init = machine_initfn##_instance_init, \
        .class_init = machine_initfn##_class_init, \
    }; \
    static void machine_initfn##_register_types(void) \
    { \
        type_register_static(&machine_initfn##_typeinfo); \
    } \
    type_init(machine_initfn##_register_types)

#endif
