/*
 * Copyright 2024-2025 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <psa/crypto.h>

#include "cb_osal.h"
#include "cb_hash.h"

#include "include/common_utils.h"

typedef struct _hash_ctx_psa
{
    psa_algorithm_t alg;
    uint32_t out_length;
    psa_hash_operation_t obj;
} hash_ctx_psa_t;

cb_status_t wrapper_hash_init(void **ctx_internal, cb_hash_t hash_type)
{
    /* Initialize PSA Crypto */
    psa_status_t status      = PSA_ERROR_CORRUPTION_DETECTED;
    cb_status_t ret          = CB_STATUS_FAIL;
    hash_ctx_psa_t *context = NULL;

    if (NULL == ctx_internal)
    {
        return CB_STATUS_FAIL;
    }

    *ctx_internal = NULL;

    status = psa_crypto_init();
    if (status != PSA_SUCCESS)
    {
        return CB_STATUS_FAIL;
    }

    /* Allocate the internal PSA hash context */
    context = (hash_ctx_psa_t *)malloc(sizeof(hash_ctx_psa_t));
    if (NULL == context)
    {
        return CB_STATUS_FAIL;
    }

    ret = convert_cb_hash_type_to_psa_alg(hash_type, &(context->alg));
    if (CB_STATUS_SUCCESS != ret)
    {
        free(context);
        return ret;
    }

    context->out_length = PSA_HASH_LENGTH(context->alg);

    /* Save the context */
    *ctx_internal = (void *)context;
    return CB_STATUS_SUCCESS;
}

cb_status_t wrapper_hash_compute(void *ctx_internal, const uint8_t *message,
                                 size_t message_size, uint8_t *hash)
{
    psa_status_t status  = PSA_ERROR_CORRUPTION_DETECTED;
    hash_ctx_psa_t *ctx = (hash_ctx_psa_t *)ctx_internal;
    size_t hash_len;

    if (NULL == ctx_internal)
    {
        return CB_STATUS_FAIL;
    }

    status = psa_hash_compute(ctx->alg, message, message_size, hash, ctx->out_length, &hash_len);

    return (status == PSA_SUCCESS) ? CB_STATUS_SUCCESS : CB_STATUS_FAIL;
}

cb_status_t wrapper_hash_deinit(void *ctx_internal)
{
    if (NULL != ctx_internal)
    {
        free(ctx_internal);
    }

    return CB_STATUS_SUCCESS;
}

/*******************************************************************************
 *                              MULTIPART                                      *
 ******************************************************************************/

cb_status_t wrapper_hash_init_multipart(void **ctx_internal, cb_hash_t hash_type)
{
    /* Reuse regular init */
    cb_status_t status   = wrapper_hash_init(ctx_internal, hash_type);
    hash_ctx_psa_t *ctx = (hash_ctx_psa_t *)*ctx_internal;

    if (NULL == ctx_internal)
    {
        return CB_STATUS_FAIL;
    }

    if (status == CB_STATUS_SUCCESS)
    {
          ctx->obj = psa_hash_operation_init();
    }

    return status;
}

cb_status_t wrapper_hash_deinit_multipart(void *ctx_internal)
{
    /* Reuse regular deinit */
    return wrapper_hash_deinit(ctx_internal);
}

cb_status_t wrapper_hash_start_multipart_compute(void *ctx_internal)
{
    psa_status_t status  = PSA_ERROR_CORRUPTION_DETECTED;
    hash_ctx_psa_t *ctx = (hash_ctx_psa_t *)ctx_internal;

    if (NULL == ctx_internal)
    {
        return CB_STATUS_FAIL;
    }

    status = psa_hash_setup(&ctx->obj, ctx->alg);

    return (status == PSA_SUCCESS) ? CB_STATUS_SUCCESS : CB_STATUS_FAIL;
}

cb_status_t wrapper_hash_update_multipart_compute(void *ctx_internal, const uint8_t *message, size_t message_size, uint8_t *hash)
{
    psa_status_t status  = PSA_ERROR_CORRUPTION_DETECTED;
    hash_ctx_psa_t *ctx = (hash_ctx_psa_t *)ctx_internal;

    (void)hash;

    if (NULL == ctx_internal)
    {
        return CB_STATUS_FAIL;
    }

    status = psa_hash_update(&ctx->obj, message, message_size);

    return (status == PSA_SUCCESS) ? CB_STATUS_SUCCESS : CB_STATUS_FAIL;
}

cb_status_t wrapper_hash_finish_multipart_compute(void *ctx_internal, uint8_t *hash)
{
    psa_status_t status  = PSA_ERROR_CORRUPTION_DETECTED;
    hash_ctx_psa_t *ctx = (hash_ctx_psa_t *)ctx_internal;
    size_t out_length    = 0u;

    if (NULL == ctx_internal)
    {
        return CB_STATUS_FAIL;
    }

    status = psa_hash_finish(&ctx->obj, hash, ctx->out_length, &out_length);

    return (status == PSA_SUCCESS) ? CB_STATUS_SUCCESS : CB_STATUS_FAIL;
}
