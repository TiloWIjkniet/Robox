/*
 * Copyright 2024 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "cb_osal.h"
#include "cb_hash.h"

/**
 *
 * CUSTOM-PORT-LAYER-TODO
 *  Crypto headers
 *
 */

/*******************************************************************************
 * HASH
 ******************************************************************************/

typedef struct _hash_ctx_custom_t
{
    /**
     *
     * CUSTOM-PORT-LAYER-TODO
     *  Add custom wrapper context
     *
     */
} hash_ctx_custom_t;

cb_status_t wrapper_hash_init(void **ctx_internal, cb_hash_t hash_type)
{
    cb_status_t status = CB_STATUS_FAIL;
    /**
     * CUSTOM-PORT-LAYER-TODO
     *  Add wrapper implementation
     */
    return status;
}

cb_status_t wrapper_hash_deinit(void *ctx_internal)
{
    cb_status_t status = CB_STATUS_FAIL;
    /**
     * CUSTOM-PORT-LAYER-TODO
     *  Add wrapper implementation
     */
    return status;
}

cb_status_t wrapper_hash_compute(void *ctx_internal, const uint8_t *message, size_t message_size, uint8_t *hash)
{
    cb_status_t status = CB_STATUS_FAIL;
    /**
     * CUSTOM-PORT-LAYER-TODO
     *  Add wrapper implementation
     */
    return status;
}

cb_status_t wrapper_hash_init_multipart(void **ctx_internal, cb_hash_t hash_type)
{
    cb_status_t status = CB_STATUS_FAIL;
    /**
     * CUSTOM-PORT-LAYER-TODO
     *  Add wrapper implementation
     */
    return status;
}

cb_status_t wrapper_hash_deinit_multipart(void *ctx_internal)
{
    cb_status_t status = CB_STATUS_FAIL;
    /**
     * CUSTOM-PORT-LAYER-TODO
     *  Add wrapper implementation
     */
    return status;
}

cb_status_t wrapper_hash_start_multipart_compute(void *ctx_internal)
{
    cb_status_t status = CB_STATUS_FAIL;
    /**
     * CUSTOM-PORT-LAYER-TODO
     *  Add wrapper implementation
     */
    return status;
}

cb_status_t wrapper_hash_update_multipart_compute(void *ctx_internal, const uint8_t *message, size_t message_size, uint8_t *hash)
{
    cb_status_t status = CB_STATUS_FAIL;
    /**
     * CUSTOM-PORT-LAYER-TODO
     *  Add wrapper implementation
     */
    return status;
}

cb_status_t wrapper_hash_finish_multipart_compute(void *ctx_internal, uint8_t *hash)
{
    cb_status_t status = CB_STATUS_FAIL;
    /**
     * CUSTOM-PORT-LAYER-TODO
     *  Add wrapper implementation
     */
    return status;
}

/**
 * CUSTOM-PORT-LAYER-TODO
 *  Other wrappers either here or in separate files.
 *  If creating other files, please update the
 *  Crypto Benchmark CMakeLists.txt and regerate the project.
 */
