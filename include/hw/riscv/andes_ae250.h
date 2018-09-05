/*
 * Andes AE250 platform interface
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

#ifndef HW_ANDES_AE250_H
#define HW_ANDES_AE250_H

#define TYPE_ANDES_AE250_SOC "riscv.andes.ae250.soc"
#define ANDES_AE250_SOC(obj) \
    OBJECT_CHECK(AndesAe250SocState, (obj), TYPE_ANDES_AE250_SOC)

typedef struct AndesAe250SocState {
    /*< private >*/
    SysBusDevice parent_obj;

    /*< public >*/
    RISCVHartArrayState cpus;
    DeviceState *plic;
} AndesAe250SocState;

typedef struct AndesAe250State {
    /*< private >*/
    SysBusDevice parent_obj;

    /*< public >*/
    AndesAe250SocState soc;
} AndesAe250State;

enum {
    ANDES_AE250_DRAM,
    ANDES_AE250_SPI,
    ANDES_AE250_ILM,
    ANDES_AE250_DLM,
    ANDES_AE250_BMC,
    ANDES_AE250_MAC,
    ANDES_AE250_DMAC,
    ANDES_AE250_PLIC,
    ANDES_AE250_PLMT,
    ANDES_AE250_SWINT,
    ANDES_AE250_PLDM,
    ANDES_AE250_APBBRG,
    ANDES_AE250_SUM,
    ANDES_AE250_UART1,
    ANDES_AE250_UART2,
    ANDES_AE250_PIT,
    ANDES_AE250_WDT,
    ANDES_AE250_RTC,
    ANDES_AE250_GPIO,
    ANDES_AE250_I2C,
    ANDES_AE250_SPI1,
    ANDES_AE250_SDC,
    ANDES_AE250_SPI2,
    ANDES_AE250_PIT2,
    ANDES_AE250_PIT3,
    ANDES_AE250_PIT4,
    ANDES_AE250_PIT5,
    ANDES_AE250_SPI3,
    ANDES_AE250_SPI4,
    ANDES_AE250_I2C2,
    ANDES_AE250_DEBUG,
};

enum {
    ANDES_AE250_PIT_IRQ = 3,
    ANDES_AE250_UART1_IRQ = 8,
    ANDES_AE250_UART2_IRQ = 9,
    ANDES_AE250_SDC_IRQ = 18,
    ANDES_AE250_MAC_IRQ = 19,
};

#define ANDES_AE250_PLIC_HART_CONFIG "MS"
#define ANDES_AE250_PLIC_NUM_SOURCES 1023
#define ANDES_AE250_PLIC_NUM_PRIORITIES 255
#define ANDES_AE250_PLIC_PRIORITY_BASE 0x0
#define ANDES_AE250_PLIC_PENDING_BASE 0x1000
#define ANDES_AE250_PLIC_ENABLE_BASE 0x2000
#define ANDES_AE250_PLIC_ENABLE_STRIDE 0x80
#define ANDES_AE250_PLIC_CONTEXT_BASE 0x200000
#define ANDES_AE250_PLIC_CONTEXT_STRIDE 0x1000

#if defined(TARGET_RISCV32)
#define ANDES_AE250_CPU TYPE_RISCV_CPU_ANDES_N25
#elif defined(TARGET_RISCV64)
#define ANDES_AE250_CPU TYPE_RISCV_CPU_ANDES_NX25
#endif

#endif
