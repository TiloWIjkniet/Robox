/*
 * Copyright 2024 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "cb_osal.h"
#include "cb_ctr_drbg.h"
#include "ele_crypto.h"
#include "ele.h"

/*******************************************************************************
 * GENERIC
 ******************************************************************************/
cb_status_t wrapper_ctr_drbg_init(void **ctx_internal)
{
    *ctx_internal = NULL;

    return CB_STATUS_SUCCESS;
}

cb_status_t wrapper_ctr_drbg_deinit(void *ctx_internal)
{
    return CB_STATUS_SUCCESS;
}

cb_status_t wrapper_ctr_drbg_compute(void *ctx_internal, uint8_t *output, size_t output_size)
{
    (void)ctx_internal;

    return ELE_RngGetRandom(S3MU, (uint32_t*)output, output_size, kNoReseed);
}
