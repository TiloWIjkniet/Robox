/*
 * Copyright 2025 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdint.h>

#include "cb_osal.h"

#include "app.h"

volatile uint32_t s_MsCount = 0U;

/*!
 * @brief Milliseconds counter since last POR/reset.
 */
void SysTick_Handler(void)
{
    s_MsCount++;
}

int cb_osal_get_core_sys_clk_freq(void) {

	return CLOCK_GetCoreSysClkFreq();
}

uint64_t cb_osal_timing_hardclock(void)
{
    uint32_t currMsCount;
    uint32_t currTick;
    uint32_t loadTick;

    do
    {
        currMsCount = s_MsCount;
        currTick    = SysTick->VAL;
    } while (currMsCount != s_MsCount);

    loadTick = cb_osal_get_core_sys_clk_freq() / 1000U;
    return (((uint64_t)currMsCount) * loadTick) + loadTick - currTick;
}
