/*
 * Copyright 2024-2025 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <psa/crypto.h>

#include "cb_osal.h"
#include "cb_aead.h"

#include "include/common_utils.h"

typedef struct _aead_ctx_psa
{
    psa_algorithm_t alg;
    psa_key_id_t key_id;
    psa_key_type_t key_type;
} aead_ctx_psa_t;

/*******************************************************************************
 *                         TRANSPARENT WRAPPERS                                *
*******************************************************************************/

cb_status_t wrapper_aead_init(void **ctx_internal, cb_aead_type_t aead_type, const uint8_t *key, size_t key_size)
{
    cb_status_t ret                 = CB_STATUS_FAIL;
    aead_ctx_psa_t *context         = NULL;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_type_t key_type;
    psa_status_t status;

    if (NULL == ctx_internal)
    {
        return CB_STATUS_FAIL;
    }

    *ctx_internal = NULL;

    /* Initialize PSA Crypto */
    status = psa_crypto_init();
    if (PSA_SUCCESS != status)
    {
        return CB_STATUS_FAIL;
    }

    /* Allocate the internal PSA context */
    context = (aead_ctx_psa_t *)malloc(sizeof(aead_ctx_psa_t));
    if (NULL == context)
    {
        return CB_STATUS_FAIL;
    }

    /* Only AES supported */
    key_type = PSA_KEY_TYPE_AES;

    /* Get the AEAD variant */
    ret = convert_cb_aead_type_to_psa_alg(aead_type, &(context->alg));
    if (CB_STATUS_SUCCESS != ret)
    {
        free(context);
        return ret;
    }

    /* Import a key */
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
    psa_set_key_algorithm(&attributes, context->alg);
    psa_set_key_type(&attributes, key_type);
    psa_set_key_bits(&attributes, key_size);

    status = psa_import_key(&attributes, key, (key_size / 8), &(context->key_id));
    if (PSA_SUCCESS != status)
    {
        free(context);
        return CB_STATUS_FAIL;
    }

    /* Save the context */
    *ctx_internal = (void *)context;
    return ret;
}

cb_status_t wrapper_aead_deinit(void *ctx_internal)
{
    aead_ctx_psa_t *ctx = (aead_ctx_psa_t *)ctx_internal;
    psa_status_t status;

    if (NULL == ctx_internal)
    {
        return CB_STATUS_FAIL;
    }

    /* Destroy the key */
    status = psa_destroy_key(ctx->key_id);

    /* NULL check already done */
    memset(ctx_internal, 0, sizeof(aead_ctx_psa_t));
    free(ctx_internal);

    return (status == PSA_SUCCESS) ? CB_STATUS_SUCCESS : CB_STATUS_FAIL;
}

cb_status_t wrapper_aead_compute(void *ctx_internal,
                                 const uint8_t *message,
                                 size_t message_size,
                                 const uint8_t *iv,
                                 size_t iv_size,
                                 const uint8_t *aad,
                                 size_t aad_size,
                                 uint8_t *ciphertext,
                                 uint8_t *tag,
                                 size_t tag_size,
                                 const uint8_t *key,
                                 size_t key_size)
{
    aead_ctx_psa_t *ctx = (aead_ctx_psa_t *)ctx_internal;
    size_t out_len;

    /* The `tag` and `ciphertext` function arguments will always be in a single
     * global output buffer in the common layer. These two memory chunks will
     * also always be right next to each other in that buffer.
     * PSA dictates that "the authentication tag is appended to the encrypted
     * data", hence we just check which of those two parameters is first
     * in memory and swap them if needed. */
    uint8_t *output = (uintptr_t)ciphertext < (uintptr_t)tag ? ciphertext : tag;

    if (NULL == ctx_internal)
    {
        return CB_STATUS_FAIL;
    }

    psa_status_t status = psa_aead_encrypt(ctx->key_id, ctx->alg, iv, iv_size,
                                           aad, aad_size, message, message_size,
                                           output, (message_size + tag_size),
                                           &out_len);

    return (status == PSA_SUCCESS) ? CB_STATUS_SUCCESS : CB_STATUS_FAIL;
}

/*******************************************************************************
 *                            OPAQUE WRAPPERS                                  *
*******************************************************************************/

cb_status_t wrapper_aead_opaque_init(void **ctx_internal, cb_aead_type_t aead_type, size_t key_size)
{
    cb_status_t ret         = CB_STATUS_FAIL;
    aead_ctx_psa_t *context = NULL;
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
    context = (aead_ctx_psa_t *)malloc(sizeof(aead_ctx_psa_t));
    if (NULL == context)
    {
        return CB_STATUS_FAIL;
    }

    /* Only AES supported */
    context->key_type = PSA_KEY_TYPE_AES;

    /* Get the AEAD variant */
    ret = convert_cb_aead_type_to_psa_alg(aead_type, &(context->alg));
    if (CB_STATUS_SUCCESS != ret)
    {
        free(context);
        return ret;
    }

    /* Save the context */
    *ctx_internal = (void *)context;
    return ret;
}


cb_status_t wrapper_aead_opaque_deinit(void *ctx_internal)
{
    return wrapper_aead_deinit(ctx_internal);
}

cb_status_t wrapper_aead_opaque_compute(void *ctx_internal,
                                        const uint8_t *message,
                                        size_t message_size,
                                        const uint8_t *iv,
                                        size_t iv_size,
                                        const uint8_t *aad,
                                        size_t aad_size,
                                        uint8_t *ciphertext,
                                        uint8_t *tag,
                                        size_t tag_size,
                                        uint32_t key_id)
{
    return wrapper_aead_compute(ctx_internal, message, message_size, iv, iv_size, aad, aad_size, ciphertext, tag, tag_size, NULL, 0);
}

cb_status_t wrapper_aead_opaque_setup(void *ctx_internal)
{
    (void)ctx_internal;

    return CB_STATUS_SUCCESS;
}

cb_status_t wrapper_aead_opaque_cleanup(void *ctx_internal)
{
    (void)ctx_internal;

    return CB_STATUS_SUCCESS;
}

cb_status_t wrapper_aead_opaque_key_generate(void *ctx_internal, size_t key_size, uint32_t *key_id)
{
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_lifetime_t lifetime     = PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION( \
                                        PSA_KEY_LIFETIME_VOLATILE, OPAQUE_KEY_LOCATION);
    aead_ctx_psa_t *context         = (aead_ctx_psa_t *)ctx_internal;
    psa_status_t status;

    if (NULL == ctx_internal)
    {
        return CB_STATUS_FAIL;
    }

    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
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

cb_status_t wrapper_aead_opaque_key_delete(void *ctx_internal, uint32_t key_id)
{
    /* PSA destroys keys the same way for both transparent and opaque.
     * Can defer this to wrapper_aead_opaque_deinit(). */
    (void)ctx_internal;
    (void)key_id;

    return CB_STATUS_SUCCESS;
}
