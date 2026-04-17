/*
 * Copyright 2025 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "cb_types.h"
#include "cb_osal.h"
#include "app.h"

cb_status_t cb_osal_init_platform()
{
    /* HW init */
    BOARD_InitHardware();

    return CB_STATUS_SUCCESS;
}

cb_status_t cb_osal_init_crypto()
{
    CRYPTO_InitHardware();

    return CB_STATUS_SUCCESS;
}

cb_status_t cb_osal_init_tick()
{
    /* Init SysTick module */
    /* call CMSIS SysTick function. It enables the SysTick interrupt at low priority */
    SysTick_Config(CLOCK_GetCoreSysClkFreq() / 1000U); /* 1 ms period */

    return CB_STATUS_SUCCESS;
}

