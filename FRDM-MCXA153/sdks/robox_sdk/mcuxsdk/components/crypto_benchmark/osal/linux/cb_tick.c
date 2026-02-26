/*
 * Copyright 2025 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdint.h>
#include <unistd.h>

#include <time.h>

int cb_osal_get_core_sys_clk_freq(void) {

    return sysconf(_SC_CLK_TCK);
}

uint64_t cb_osal_timing_hardclock(void)
{
    uint64_t cycles = 0;
    int freq = cb_osal_get_core_sys_clk_freq();
    struct timespec time = { 0 };

    clock_gettime(CLOCK_REALTIME, &time);

    cycles = time.tv_nsec * freq;
    cycles /= 1000000000;
    cycles += time.tv_sec * freq;

    return cycles;
}
