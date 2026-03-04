/*
 * Copyright 2024-2025 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <psa/crypto.h>

#include "cb_osal.h"
#include "cb_mac.h"

#include "include/common_utils.h"

typedef struct _mac_ctx_psa
{
    psa_algorithm_t alg;
    psa_algorithm_t hash_alg;
    size_t mac_len;
    psa_key_id_t key_id;
    psa_key_type_t key_type;
} mac_ctx_psa_t;

/*******************************************************************************
 *                         TRANSPARENT WRAPPERS                                *
*******************************************************************************/

static cb_status_t wrapper_mac_init_common(void **ctx_internal,
                                           cb_mac_algorithm_t algorithm,
                                           cb_hash_t hash_type,
                                           size_t key_size)
{
    cb_status_t ret        = CB_STATUS_FAIL;
    mac_ctx_psa_t *context = NULL;
    psa_status_t status;

    if (NULL == ctx_internal)
    {
        return CB_STATUS_FAIL;
    }

    *ctx_internal = NULL;

    /* Initialize PSA Crypto */
    status = psa_crypto_init();
    if (status != PSA_SUCCESS)
    {
        return CB_STATUS_FAIL;
    }

    /* Allocate the internal PSA context */
    context = (mac_ctx_psa_t *)malloc(sizeof(mac_ctx_psa_t));
    if (NULL == context)
    {
        return CB_STATUS_FAIL;
    }

    ret = convert_cb_hash_type_to_psa_alg(hash_type, &(context->hash_alg));
    if (CB_STATUS_SUCCESS != ret)
    {
        free(context);
        return ret;
    }

    switch (algorithm)
    {
        case CB_MAC_ALGORITHM_HMAC:
            context->alg      = PSA_ALG_HMAC(context->hash_alg);
            context->key_type = PSA_KEY_TYPE_HMAC;
            break;
        case CB_MAC_ALGORITHM_CBC_MAC:
            context->alg      = PSA_ALG_CBC_MAC;
            context->key_type = PSA_KEY_TYPE_AES;
            break;
        case CB_MAC_ALGORITHM_CMAC:
            context->alg      = PSA_ALG_CMAC;
            context->key_type = PSA_KEY_TYPE_AES;
            break;
        default:
            ret = CB_STATUS_FAIL;
            break;
    }
    if (CB_STATUS_SUCCESS != ret)
    {
        free(context);
        return ret;
    }

    context->mac_len = PSA_MAC_LENGTH(context->key_type, key_size, context->alg);

    /* Save the context */
    *ctx_internal = (void *)context;
    return ret;
}


cb_status_t wrapper_mac_init(
    void **ctx_internal, cb_mac_algorithm_t algorithm, cb_hash_t hash_type, const uint8_t *key, size_t key_size)
{
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    mac_ctx_psa_t *context          = NULL;
    cb_status_t ret;
    psa_status_t status;

    ret = wrapper_mac_init_common(ctx_internal, algorithm, hash_type, key_size);
    if (CB_STATUS_SUCCESS != ret)
    {
        return ret;
    }

    context = (mac_ctx_psa_t *)*ctx_internal;

    /* Import a key */
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_SIGN_MESSAGE | PSA_KEY_USAGE_VERIFY_MESSAGE);
    psa_set_key_algorithm(&attributes, context->alg);
    psa_set_key_type(&attributes, context->key_type);
    psa_set_key_bits(&attributes, key_size);

    status = psa_import_key(&attributes, key, (key_size / 8), &(context->key_id));
    if (PSA_SUCCESS != status)
    {
        free(context);
        *ctx_internal = NULL;
        return CB_STATUS_FAIL;
    }

    return CB_STATUS_SUCCESS;
}

cb_status_t wrapper_mac_deinit(void *ctx_internal)
{
    mac_ctx_psa_t *ctx = (mac_ctx_psa_t *)ctx_internal;
    psa_status_t status;

    if (NULL == ctx_internal)
    {
        return CB_STATUS_FAIL;
    }

    /* Destroy the key */
    status = psa_destroy_key(ctx->key_id);

    /* NULL check already done */
    memset(ctx_internal, 0, sizeof(mac_ctx_psa_t));
    free(ctx_internal);

    return (status == PSA_SUCCESS) ? CB_STATUS_SUCCESS : CB_STATUS_FAIL;
}

cb_status_t wrapper_mac_compute(void *ctx_internal, const uint8_t *message, size_t message_size, uint8_t *mac, const uint8_t *key)
{
    psa_status_t status;
    mac_ctx_psa_t *ctx = (mac_ctx_psa_t *)ctx_internal;
    size_t mac_size    = 0u;

    if (NULL == ctx_internal)
    {
        return CB_STATUS_FAIL;
    }

    status = psa_mac_compute(ctx->key_id, ctx->alg, message, message_size, mac, ctx->mac_len, &mac_size);

    return (status == PSA_SUCCESS) ? CB_STATUS_SUCCESS : CB_STATUS_FAIL;
}


/*******************************************************************************
 *                            OPAQUE WRAPPERS                                  *
*******************************************************************************/

cb_status_t wrapper_mac_opaque_init(
    void **ctx_internal, cb_mac_algorithm_t algorithm, cb_hash_t hash_type, size_t key_size)
{
    return wrapper_mac_init_common(ctx_internal, algorithm, hash_type, key_size);
}

cb_status_t wrapper_mac_opaque_deinit(void *ctx_internal)
{
    return wrapper_mac_deinit(ctx_internal);
}

cb_status_t wrapper_mac_opaque_compute(void *ctx_internal, const uint8_t *message, size_t message_size, uint8_t *mac, uint32_t key_id)
{
    psa_status_t status;
    mac_ctx_psa_t *ctx = (mac_ctx_psa_t *)ctx_internal;
    size_t mac_size    = 0u;

    if (NULL == ctx_internal)
    {
        return CB_STATUS_FAIL;
    }

    status = psa_mac_compute(ctx->key_id, ctx->alg, message, message_size, mac, ctx->mac_len, &mac_size);

    return (status == PSA_SUCCESS) ? CB_STATUS_SUCCESS : CB_STATUS_FAIL;
}

cb_status_t wrapper_mac_opaque_setup(void *ctx_internal)
{
    (void)ctx_internal;

    return CB_STATUS_SUCCESS;
}

cb_status_t wrapper_mac_opaque_cleanup(void *ctx_internal)
{
    (void)ctx_internal;

    return CB_STATUS_SUCCESS;
}

cb_status_t wrapper_mac_opaque_key_generate(void *ctx_internal, size_t key_size, uint32_t *key_id)
{
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_lifetime_t lifetime     = PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION( \
                                        PSA_KEY_LIFETIME_VOLATILE, OPAQUE_KEY_LOCATION);
    mac_ctx_psa_t *context          = (mac_ctx_psa_t *)ctx_internal;
    psa_status_t status;

    if (NULL == ctx_internal)
    {
        return CB_STATUS_FAIL;
    }

    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_SIGN_MESSAGE | PSA_KEY_USAGE_VERIFY_MESSAGE);
    psa_set_key_algorithm(&attributes, context->alg);
    psa_set_key_type(&attributes, context->key_type);
    psa_set_key_bits(&attributes, key_size);
    psa_set_key_lifetime(&attributes, lifetime);

    status = psa_generate_key(&attributes, key_id);
    if (PSA_SUCCESS != status)
    {
        return CB_STATUS_FAIL;
    }

    context->key_id = *key_id;

    return CB_STATUS_SUCCESS;
}

cb_status_t wrapper_mac_opaque_key_delete(void *ctx_internal, uint32_t key_id)
{
    /* PSA destroys keys the same way for both transparent and opaque.
     * Can defer this to wrapper_aead_opaque_deinit(). */
    (void)ctx_internal;
    (void)key_id;

    return CB_STATUS_SUCCESS;
}
