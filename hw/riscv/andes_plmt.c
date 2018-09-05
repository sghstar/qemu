/*
 * Andes PLMT (Platform Level Machine Timer)
 *
 * Copyright (c) 2018 Andes Tech. Corp.
 *
 * This provides real-time clock, timer and interprocessor interrupts.
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
#include "hw/sysbus.h"
#include "hw/riscv/sifive_clint.h"
#include "hw/riscv/andes_plmt.h"

/*
 * Create PLMT device.
 */
DeviceState *
andes_plmt_create(hwaddr addr, hwaddr size, uint32_t num_harts,
                  uint32_t time_base, uint32_t timecmp_base)
{
    DeviceState *dev;

    dev = sifive_clint_create(addr, size, num_harts,  ANDES_SIP_BASE, timecmp_base, time_base);

    return dev;
}
