/*
 * Copyright 2024-2025 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <psa/crypto.h>

#include "cb_osal.h"
#include "cb_rsa.h"

#include "include/common_utils.h"

typedef struct _rsa_ctx_psa
{
    psa_algorithm_t alg;
    psa_algorithm_t hash_alg;
    size_t sig_size;
    psa_key_id_t key_id;
    psa_key_type_t key_type;
    psa_key_usage_t usage;
    cb_asym_sigver_input_type_t input_type;
    size_t ciphertext_length;
} rsa_ctx_psa_t;

/*******************************************************************************
 *                         TRANSPARENT WRAPPERS                                *
*******************************************************************************/

cb_status_t wrapper_rsa_init_common(
    void **ctx_internal, cb_rsa_type_t rsa_type, cb_hash_t hash_type, size_t key_size, cb_asym_sigver_input_type_t input_type)
{
    cb_status_t ret        = CB_STATUS_FAIL;
    rsa_ctx_psa_t *context = NULL;
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
    context = (rsa_ctx_psa_t *)malloc(sizeof(rsa_ctx_psa_t));
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

    switch (rsa_type)
    {
        case CB_RSA_PKCS1V15_SIGN:
            context->alg = PSA_ALG_RSA_PKCS1V15_SIGN(context->hash_alg);
            break;
        case CB_RSA_PSS:
            context->alg = PSA_ALG_RSA_PSS(context->hash_alg);
            break;
        case CB_RSA_PKCS1V15_CRYPT:
            context->alg = PSA_ALG_RSA_PKCS1V15_CRYPT;
            break;
        case CB_RSA_OAEP:
            context->alg = PSA_ALG_RSA_OAEP(context->hash_alg);
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

    if (true == PSA_ALG_IS_ASYMMETRIC_ENCRYPTION(context->alg))
    {
        context->usage = PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT;
    }
    else
    {
        /* If not encrypting, we need to set usage based on input type */
        context->usage = CB_SIGVER_MESSAGE == input_type
                            ? (PSA_KEY_USAGE_SIGN_MESSAGE | PSA_KEY_USAGE_VERIFY_MESSAGE)
                            : (PSA_KEY_USAGE_SIGN_HASH    | PSA_KEY_USAGE_VERIFY_HASH);
    }

    context->key_type   = PSA_KEY_TYPE_RSA_KEY_PAIR;
    context->input_type = input_type;
    context->sig_size   = PSA_SIGN_OUTPUT_SIZE(context->key_type, key_size, context->alg);

    /* Save the context */
    *ctx_internal = (void *)context;
    return ret;
}

cb_status_t wrapper_rsa_init(
    void **ctx_internal, cb_rsa_type_t rsa_type, cb_hash_t hash_type, size_t key_size, size_t public_key_buffer_size, cb_asym_sigver_input_type_t input_type)
{
    psa_status_t status;
    cb_status_t ret;
    rsa_ctx_psa_t *context = NULL;

    ret = wrapper_rsa_init_common(ctx_internal, rsa_type, hash_type,
                                  key_size, input_type);
    if (CB_STATUS_SUCCESS != ret)
    {
        return ret;
    }

    context = (rsa_ctx_psa_t *)(*ctx_internal);

    if (NULL == context)
    {
        return CB_STATUS_FAIL;
    }

    /************* Doing keygen for now, same as ECDSA **********/
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_lifetime_t lifetime     =
        PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION( \
            PSA_KEY_LIFETIME_VOLATILE, 0);

    /* Generate a key */
    psa_set_key_usage_flags(&attributes, context->usage);
    psa_set_key_algorithm(&attributes, context->alg);
    psa_set_key_type(&attributes, context->key_type);
    psa_set_key_bits(&attributes, key_size);
    psa_set_key_lifetime(&attributes, lifetime);

    status = psa_generate_key(&attributes, &(context->key_id));
    if (status != PSA_SUCCESS)
    {
        free(context);
        *ctx_internal = NULL;
        return CB_STATUS_FAIL;
    }

    return CB_STATUS_SUCCESS;
}

cb_status_t wrapper_rsa_deinit(void *ctx_internal)
{
    rsa_ctx_psa_t *ctx = (rsa_ctx_psa_t *)ctx_internal;
    psa_status_t status;

    if (NULL == ctx_internal)
    {
        return CB_STATUS_FAIL;
    }

    /* Destroy the key */
    status = psa_destroy_key(ctx->key_id);

    /* NULL check already done */
    memset(ctx_internal, 0, sizeof(rsa_ctx_psa_t));
    free(ctx_internal);

    return (status == PSA_SUCCESS) ? CB_STATUS_SUCCESS : CB_STATUS_FAIL;
}

cb_status_t rsa_keygen_compute_common(size_t key_size,
                                      psa_key_lifetime_t lifetime)
{
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_id_t key_id;
    psa_status_t status;

    /* Initialize PSA Crypto */
    status = psa_crypto_init();
    if (status != PSA_SUCCESS)
    {
        return CB_STATUS_FAIL;
    }

    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT );
    psa_set_key_algorithm(&attributes, PSA_ALG_RSA_PKCS1V15_CRYPT);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_RSA_KEY_PAIR);
    psa_set_key_bits(&attributes, (key_size));
    psa_set_key_lifetime(&attributes, lifetime);

    status = psa_generate_key(&attributes, &key_id);

    return (status == PSA_SUCCESS) ? CB_STATUS_SUCCESS : CB_STATUS_FAIL;
}

cb_status_t wrapper_rsa_keygen_compute(uint8_t *modulus,
                                       size_t modulus_buf_size,
                                       uint8_t *private_exponent,
                                       size_t private_exponent_buf_size,
                                       const uint8_t *public_exponent,
                                       size_t public_exponent_buf_size,
                                       size_t key_size)
{
    return rsa_keygen_compute_common(key_size, 0);
}

/*******************************************************************************
 * SIGN / VERIFY wrappers
 ******************************************************************************/
cb_status_t wrapper_rsa_sign_compute(void *ctx_internal,
                                     const uint8_t *modulus,
                                     const uint8_t *private_exponent,
                                     uint8_t *message,
                                     size_t message_size,
                                     uint8_t *signature)
{
    psa_status_t status     = PSA_ERROR_CORRUPTION_DETECTED;
    rsa_ctx_psa_t *ctx      = (rsa_ctx_psa_t *)ctx_internal;
    size_t signature_length = 0u;

    if (NULL == ctx_internal)
    {
        return CB_STATUS_FAIL;
    }

    if (CB_SIGVER_MESSAGE == ctx->input_type)
    {
        status = psa_sign_message(ctx->key_id, ctx->alg,
                                  message, message_size,
                                  signature, ctx->sig_size,
                                  &signature_length);
    }
    else /* CB_SIGVER_HASH */
    {
        status = psa_sign_hash(ctx->key_id, ctx->alg,
                              message, PSA_HASH_LENGTH(ctx->hash_alg),
                              signature, ctx->sig_size,
                              &signature_length);
    }

    return (status == PSA_SUCCESS) ? CB_STATUS_SUCCESS : CB_STATUS_FAIL;
}

cb_status_t wrapper_rsa_verify_compute(
    void *ctx_internal, const uint8_t *modulus, const uint8_t *public_exponent, uint8_t *message, size_t message_size, uint8_t *signature)
{
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    rsa_ctx_psa_t *ctx  = (rsa_ctx_psa_t *)ctx_internal;

    if (NULL == ctx_internal)
    {
        return CB_STATUS_FAIL;
    }

    if (CB_SIGVER_MESSAGE == ctx->input_type)
    {
        status = psa_verify_message(ctx->key_id, ctx->alg,
                                    message, message_size,
                                    signature, ctx->sig_size);
    }
    else /* CB_SIGVER_HASH */
    {
        status = psa_verify_hash(ctx->key_id, ctx->alg,
                                 message, PSA_HASH_LENGTH(ctx->hash_alg),
                                 signature, ctx->sig_size);
    }

    return (status == PSA_SUCCESS) ? CB_STATUS_SUCCESS : CB_STATUS_FAIL;
}

/*******************************************************************************
 * ENCRYPT / DECRYPT wrappers
 ******************************************************************************/
cb_status_t wrapper_rsa_encrypt_compute(void *ctx_internal,
                                        const uint8_t *modulus,
                                        const uint8_t *public_exponent,
                                        uint8_t *message,
                                        size_t message_size,
                                        uint8_t *ciphertext)
{
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    rsa_ctx_psa_t *ctx  = (rsa_ctx_psa_t *)ctx_internal;

    if (NULL == ctx_internal)
    {
        return CB_STATUS_FAIL;
    }

    status = psa_asymmetric_encrypt(ctx->key_id, ctx->alg,
                                    message, message_size,
                                    NULL, 0,
                                    ciphertext, 1024 /* Output buffer will be large enough */,
                                    &(ctx->ciphertext_length));

    return (status == PSA_SUCCESS) ? CB_STATUS_SUCCESS : CB_STATUS_FAIL;
}

cb_status_t wrapper_rsa_decrypt_compute(void *ctx_internal,
                                        const uint8_t *modulus,
                                        const uint8_t *private_exponent,
                                        uint8_t *message,
                                        size_t message_size,
                                        uint8_t *ciphertext)
{
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    rsa_ctx_psa_t *ctx  = (rsa_ctx_psa_t *)ctx_internal;
    size_t plaintext_length;

    if (NULL == ctx_internal)
    {
        return CB_STATUS_FAIL;
    }

    status = psa_asymmetric_decrypt(ctx->key_id, ctx->alg,
                                    ciphertext, ctx->ciphertext_length,
                                    NULL, 0,
                                    message, 1024 /* Output buffer will be large enough */,
                                    &plaintext_length);

    return (status == PSA_SUCCESS) ? CB_STATUS_SUCCESS : CB_STATUS_FAIL;
}

/*******************************************************************************
 *                            OPAQUE WRAPPERS                                  *
*******************************************************************************/

cb_status_t wrapper_rsa_opaque_init(void **ctx_internal,
                                    cb_rsa_type_t rsa_type,
                                    cb_hash_t hash_type,
                                    size_t key_size,
                                    cb_asym_sigver_input_type_t input_type)
{
    return wrapper_rsa_init_common(ctx_internal, rsa_type, hash_type, key_size, input_type);
}

cb_status_t wrapper_rsa_opaque_deinit(void *ctx_internal)
{
    return wrapper_rsa_deinit(ctx_internal);
}

cb_status_t wrapper_rsa_keygen_opaque_setup(void)
{
    return CB_STATUS_SUCCESS;
}

cb_status_t wrapper_rsa_keygen_opaque_cleanup(void)
{
    return CB_STATUS_SUCCESS;
}

cb_status_t wrapper_rsa_keygen_opaque_compute(size_t key_size,
                                           uint32_t *key_id,
                                           uint8_t *public_key_addr,
                                           size_t public_key_buf_size)
{
    psa_key_lifetime_t lifetime =
        PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION( \
            PSA_KEY_LIFETIME_VOLATILE, OPAQUE_KEY_LOCATION);

    return rsa_keygen_compute_common(key_size, lifetime);
}

cb_status_t wrapper_rsa_keygen_opaque_key_delete(uint32_t key_id)
{
    return CB_STATUS_SUCCESS;
}

cb_status_t wrapper_rsa_sign_opaque_setup(void *ctx_internal)
{
    return CB_STATUS_SUCCESS;
}

cb_status_t wrapper_rsa_sign_opaque_cleanup(void *ctx_internal)
{
    return CB_STATUS_SUCCESS;
}

cb_status_t wrapper_rsa_sign_opaque_key_generate(void *ctx_internal,
                                                 uint32_t *key_id,
                                                 size_t key_size,
                                                 uint8_t *public_key_addr,
                                                 size_t *public_key_size,
                                                 size_t public_key_buf_size)
{
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_lifetime_t lifetime     = PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION( \
                                        PSA_KEY_LIFETIME_VOLATILE, OPAQUE_KEY_LOCATION);
    rsa_ctx_psa_t *context          = (rsa_ctx_psa_t *)ctx_internal;
    psa_status_t status;

    if (NULL == ctx_internal)
    {
        return CB_STATUS_FAIL;
    }

    psa_set_key_usage_flags(&attributes, context->usage);
    psa_set_key_algorithm(&attributes, context->alg);
    psa_set_key_type(&attributes, context->key_type);
    psa_set_key_bits(&attributes, key_size);
    psa_set_key_lifetime(&attributes, lifetime);

    status = psa_generate_key(&attributes, key_id);
    if (status != PSA_SUCCESS)
    {
        return CB_STATUS_FAIL;
    }

    context->key_id = *key_id;

    return CB_STATUS_SUCCESS;
}

cb_status_t wrapper_rsa_sign_opaque_key_delete(void *ctx_internal, uint32_t key_id)
{
    /* PSA destroys keys the same way for both transparent and opaque.
     * Can defer this to wrapper_aead_opaque_deinit(). */
    (void)ctx_internal;
    (void)key_id;

    return CB_STATUS_SUCCESS;
}

cb_status_t wrapper_rsa_sign_opaque_compute(void *ctx_internal,
                                            const uint8_t *message,
                                            size_t message_size,
                                            uint8_t *signature,
                                            uint32_t key_id)
{
    psa_status_t status     = PSA_ERROR_CORRUPTION_DETECTED;
    rsa_ctx_psa_t *ctx      = (rsa_ctx_psa_t *)ctx_internal;
    size_t signature_length = 0u;

    if (NULL == ctx_internal)
    {
        return CB_STATUS_FAIL;
    }

    if (CB_SIGVER_MESSAGE == ctx->input_type)
    {
        status = psa_sign_message(ctx->key_id, ctx->alg,
                                  message, message_size,
                                  signature, ctx->sig_size,
                                  &signature_length);
    }
    else /* CB_SIGVER_HASH */
    {
        status = psa_sign_hash(ctx->key_id, ctx->alg,
                               message, PSA_HASH_LENGTH(ctx->hash_alg),
                               signature, ctx->sig_size,
                               &signature_length);
    }

    return (status == PSA_SUCCESS) ? CB_STATUS_SUCCESS : CB_STATUS_FAIL;
}

cb_status_t wrapper_rsa_verify_opaque_compute(void *ctx_internal,
                                              const uint8_t *message,
                                              size_t message_size,
                                              uint8_t *signature,
                                              uint8_t *public_key,
                                              size_t public_key_size)
{
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    rsa_ctx_psa_t *ctx  = (rsa_ctx_psa_t *)ctx_internal;

    if (NULL == ctx_internal)
    {
        return CB_STATUS_FAIL;
    }

    if (CB_SIGVER_MESSAGE == ctx->input_type)
    {
        status = psa_verify_message(ctx->key_id, ctx->alg,
                                    message, message_size,
                                    signature, ctx->sig_size);
    }
    else /* CB_SIGVER_HASH */
    {
        status = psa_verify_hash(ctx->key_id, ctx->alg,
                                 message, PSA_HASH_LENGTH(ctx->hash_alg),
                                 signature, ctx->sig_size);
    }

    return (status == PSA_SUCCESS) ? CB_STATUS_SUCCESS : CB_STATUS_FAIL;
}
