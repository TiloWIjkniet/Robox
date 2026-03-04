/*
 * Copyright 2025 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef CB_OSAL_H
#define CB_OSAL_H

#include <stdint.h>

#include "cb_types.h"

#include "fsl_common.h"
#include "fsl_debug_console.h"

#include "mcux_psa_defines.h"

#define DATA_AT_SECTION(var, section) var @"section"

cb_status_t cb_osal_init_platform(void);
cb_status_t cb_osal_init_crypto(void);
cb_status_t cb_osal_init_tick(void);

int cb_osal_get_core_sys_clk_freq(void);
uint64_t cb_osal_timing_hardclock(void);

void cb_osal_exit(void);

#endif /* CB_OSAL_H */
