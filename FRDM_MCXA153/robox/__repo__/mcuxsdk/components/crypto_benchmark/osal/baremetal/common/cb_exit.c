/*
 * Copyright 2025 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "cb_types.h"
#include "cb_osal.h"

void cb_osal_exit(void)
{
    while (1)
    {
        char ch = GETCHAR();
        PUTCHAR(ch);
    }
}
