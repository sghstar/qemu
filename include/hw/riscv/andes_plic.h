/*
 * Andes PLIC (Platform Level Interrupt Controller) interface
 *
 * Copyright (c) 2018 Andes Tech. Corp.
 *
 * This provides a RISC-V PLIC device with Andes' extensions.
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

#ifndef HW_ANDES_PLIC_H
#define HW_ANDES_PLIC_H

#include "hw/riscv/sifive_plic.h"

#define TYPE_ANDES_PLIC "riscv.andes.plic"

#define ANDES_PLIC_GET_CLASS(obj) \
    OBJECT_GET_CLASS(AndesPLICClass, (obj), TYPE_ANDES_PLIC)
#define ANDES_PLIC_CLASS(klass) \
    OBJECT_CLASS_CHECK(AndesPLICClass, (klass), TYPE_ANDES_PLIC)
#define ANDES_PLIC(obj) \
    OBJECT_CHECK(AndesPLICState, (obj), TYPE_ANDES_PLIC)

/* sync with sifive_plic.h */
typedef SysBusDeviceClass SiFivePLICClass;
#define SIFIVE_PLIC_CLASS(klass) \
    OBJECT_CLASS_CHECK(SiFivePLICClass, (klass), TYPE_SIFIVE_PLIC)

typedef struct AndesPLICClass {
    SiFivePLICClass parent_class;

    DeviceRealize parent_realize;
    DeviceReset parent_reset;
} AndesPLICClass;

 typedef struct AndesPLICState {
    SiFivePLICState parent_obj;

    MemoryRegion parent_mmio;

    /* registers */
    uint32_t *trigger_type;
    uint32_t *preempted_priority;
    uint32_t feature_enable;

    /* internal status */
    uint32_t *preempted_id;
    uint32_t priority_words;
    uint32_t claim_count;
    uint32_t last_claimed_id;
    uint32_t last_claimed_priority;
} AndesPLICState;

DeviceState *
andes_plic_create(hwaddr addr, char *hart_config,
    uint32_t num_sources, uint32_t num_priorities,
    uint32_t priority_base, uint32_t pending_base,
    uint32_t enable_base, uint32_t enable_stride,
    uint32_t context_base, uint32_t context_stride,
    uint32_t aperture_size);

DeviceState *
andes_plic_swint_create(hwaddr addr, char *hart_config,
    uint32_t num_sources, uint32_t num_priorities,
    uint32_t priority_base, uint32_t pending_base,
    uint32_t enable_base, uint32_t enable_stride,
    uint32_t context_base, uint32_t context_stride,
    uint32_t aperture_size, uint32_t m_mode_mip_mask,
    uint32_t s_mode_mip_mask);

#endif
