/*
 * Andes AE350 platform interface
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

#ifndef HW_ANDES_AE350_H
#define HW_ANDES_AE350_H

#define TYPE_AE350_BOARD "andes.ae350.board"

#define AE30_BOARD(obj) OBJECT_CHECK(Ae350BoardState, (obj), TYPE_AE350_BOARD)

typedef struct {
    /*< private >*/
    SysBusDevice parent_obj;

    /*< public >*/
    RISCVHartArrayState soc;
    DeviceState *plic;
    void *fdt;
    int fdt_size;
} Ae350BoardState;

enum {
    AE350_MM_VIRTIO,
    AE350_MM_DEBUG,
    AE350_MM_DRAM,
    AE350_MM_BOOTROM,
    AE350_MM_MROM,
    AE350_MM_PLMT,
    AE350_MM_PLIC,
    AE350_MM_SWINT,
    AE350_MM_UART1,
    AE350_MM_UART2,
    AE350_MM_SDC,
    AE350_MM_MAC,
    AE350_MM_PIT,
};

enum {
    AE350_PIT_IRQ = 3,
    AE350_UART1_IRQ = 8,
    AE350_UART2_IRQ = 9,
    AE350_SDC_IRQ = 18,
    AE350_MAC_IRQ = 19,
    AE350_VIRTIO_COUNT = 8,
    AE350_VIRTIO_IRQ = 16, /* 16 to 23 */
};

#define AE350_PLIC_HART_CONFIG "MS,MS"
#define AE350_PLIC_NUM_SOURCES 1023
#define AE350_PLIC_NUM_PRIORITIES 255
#define AE350_PLIC_PRIORITY_BASE 0x0
#define AE350_PLIC_PENDING_BASE 0x1000
#define AE350_PLIC_ENABLE_BASE 0x2000
#define AE350_PLIC_ENABLE_STRIDE 0x80
#define AE350_PLIC_CONTEXT_BASE 0x200000
#define AE350_PLIC_CONTEXT_STRIDE 0x1000

#define AE350_PLICSW_HART_CONFIG "M,M"
#define AE350_PLICSW_NUM_SOURCES 16
#define AE350_PLICSW_NUM_PRIORITIES 255
#define AE350_PLICSW_PRIORITY_BASE 0x0
#define AE350_PLICSW_PENDING_BASE 0x1000
#define AE350_PLICSW_ENABLE_BASE 0x2000
#define AE350_PLICSW_ENABLE_STRIDE 0x80
#define AE350_PLICSW_CONTEXT_BASE 0x200000
#define AE350_PLICSW_CONTEXT_STRIDE 0x1000

#endif
