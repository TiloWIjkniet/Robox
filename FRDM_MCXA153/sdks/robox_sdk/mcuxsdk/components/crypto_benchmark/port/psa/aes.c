/*
 * Copyright 2024-2025 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <psa/crypto.h>

#include "cb_osal.h"
#include "cb_aes.h"

#include "include/common_utils.h"

typedef struct _cipher_ctx_psa
{
    psa_algorithm_t alg;
    psa_key_id_t key_id;
    psa_key_type_t key_type;
} cipher_ctx_psa_t;

/*******************************************************************************
 *                         TRANSPARENT WRAPPERS                                *
*******************************************************************************/

cb_status_t wrapper_aes_init(void **ctx_internal, cb_cipher_type_t cipher_type, const uint8_t *key, size_t key_size)
{
    cb_status_t ret           = CB_STATUS_FAIL;
    cipher_ctx_psa_t *context = NULL;
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
    context = (cipher_ctx_psa_t *)malloc(sizeof(cipher_ctx_psa_t));
    if (NULL == context)
    {
        return CB_STATUS_FAIL;
    }

    key_type = PSA_KEY_TYPE_AES;
    ret      = convert_cb_cipher_type_to_psa_alg(cipher_type, &(context->alg));
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

cb_status_t wrapper_aes_deinit(void *ctx_internal)
{
    cipher_ctx_psa_t *ctx = (cipher_ctx_psa_t *)ctx_internal;
    psa_status_t status;

    if (NULL == ctx_internal)
    {
        return CB_STATUS_FAIL;
    }

    /* Destroy the key */
    status = psa_destroy_key(ctx->key_id);

    /* NULL check already done */
    memset(ctx_internal, 0, sizeof(cipher_ctx_psa_t));
    free(ctx_internal);

    return (status == PSA_SUCCESS) ? CB_STATUS_SUCCESS : CB_STATUS_FAIL;
}

cb_status_t wrapper_aes_compute(void *ctx_internal, const uint8_t *message, size_t message_size, const uint8_t *iv, const uint8_t *key, uint8_t *ciphertext)
{
    cipher_ctx_psa_t *ctx = (cipher_ctx_psa_t *)ctx_internal;
    size_t out_len;

    /* For single part cipher, iv is calculated randomly in PSA API and placed at start of output buffer.
     * Hence assuming that framework passes big enough output buffer and ignoring IV passed.
     * Also note that ciphers with a PKCS7 padding scheme require AT LEAST one more Byte of space
     * in the output buffer. We will give one block's worth of space to be safe - buffer should be large enough.
     */
    if (NULL == ctx_internal)
    {
        return CB_STATUS_FAIL;
    }

    psa_status_t status = psa_cipher_encrypt(ctx->key_id, ctx->alg, message, message_size, ciphertext, message_size + 16 + 16, &out_len);

    return (status == PSA_SUCCESS) ? CB_STATUS_SUCCESS : CB_STATUS_FAIL;
}

/*******************************************************************************
 *                            OPAQUE WRAPPERS                                  *
*******************************************************************************/

cb_status_t wrapper_aes_opaque_init(void **ctx_internal, cb_cipher_type_t cipher_type, size_t key_size)
{
    cb_status_t ret           = CB_STATUS_FAIL;
    cipher_ctx_psa_t *context = NULL;
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
    context = (cipher_ctx_psa_t *)malloc(sizeof(cipher_ctx_psa_t));
    if (NULL == context)
    {
        return CB_STATUS_FAIL;
    }

    context->key_type = PSA_KEY_TYPE_AES;
    ret               = convert_cb_cipher_type_to_psa_alg(cipher_type, &(context->alg));
    if (CB_STATUS_SUCCESS != ret)
    {
        free(context);
        return ret;
    }

    /* Save the context */
    *ctx_internal = (void *)context;
    return ret;
}

cb_status_t wrapper_aes_opaque_deinit(void *ctx_internal)
{
    return wrapper_aes_deinit(ctx_internal);
}

cb_status_t wrapper_aes_opaque_compute(void *ctx_internal, const uint8_t *message, size_t message_size, const uint8_t *iv, uint8_t *ciphertext, uint32_t key_id)
{
    (void)key_id;
    return wrapper_aes_compute(ctx_internal, message, message_size, iv, NULL, ciphertext);
}

cb_status_t wrapper_aes_opaque_setup(void *ctx_internal)
{
    (void)ctx_internal;

    return CB_STATUS_SUCCESS;
}

cb_status_t wrapper_aes_opaque_cleanup(void *ctx_internal)
{
    (void)ctx_internal;

    return CB_STATUS_SUCCESS;
}

cb_status_t wrapper_aes_opaque_key_generate(void *ctx_internal, size_t key_size, uint32_t *key_id)
{
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_lifetime_t lifetime     = PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION( \
                                        PSA_KEY_LIFETIME_VOLATILE, OPAQUE_KEY_LOCATION);
    cipher_ctx_psa_t *context       = (cipher_ctx_psa_t *)ctx_internal;
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

cb_status_t wrapper_aes_opaque_key_delete(void *ctx_internal, uint32_t key_id)
{
    /* PSA destroys keys the same way for both transparent and opaque.
     * Can defer this to wrapper_aes_opaque_deinit(). */
    (void)ctx_internal;
    (void)key_id;

    return CB_STATUS_SUCCESS;
}
