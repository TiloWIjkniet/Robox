/*
 * Copyright 2024-2025 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <psa/crypto.h>

#include "cb_osal.h"
#include "cb_ecdsa.h"

#include "include/common_utils.h"

typedef struct _ecdsa_ctx_psa
{
    psa_algorithm_t alg;
    psa_algorithm_t hash_alg;
    size_t sig_size;
    psa_key_id_t key_id;
    psa_key_type_t key_type;
    psa_key_usage_t usage;
    psa_ecc_family_t curve;
    cb_asym_sigver_input_type_t input_type;
} ecdsa_ctx_psa_t;

static cb_status_t wrapper_ecdsa_init_common(void **ctx_internal,
                                             cb_ecdsa_type_t ecdsa_type,
                                             cb_hash_t hash_type,
                                             cb_ecc_family_t family,
                                             size_t key_size,
                                             cb_asym_sigver_input_type_t input_type)
{
    cb_status_t ret          = CB_STATUS_FAIL;
    ecdsa_ctx_psa_t *context = NULL;
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
    context = (ecdsa_ctx_psa_t *)malloc(sizeof(ecdsa_ctx_psa_t));
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

    ret = convert_cb_ecc_family_to_psa_ecc_family(family, &(context->curve));
    if (CB_STATUS_SUCCESS != ret)
    {
        free(context);
        return ret;
    }

    context->key_type = PSA_KEY_TYPE_ECC_KEY_PAIR(context->curve);

    if (CB_ECC_FAMILY_TWISTED_EDWARDS == family)
    {
        if (CB_SIGVER_MESSAGE == input_type)
        {
            context->alg = PSA_ALG_PURE_EDDSA;
        }
        else
        {
            if (key_size == 255)
            {
                context->alg = PSA_ALG_ED25519PH;

            }
            else if (key_size == 448)
            {
                context->alg = PSA_ALG_ED448PH;
            }
            else
            {
                ret = CB_STATUS_FAIL;
            }
        }

        if (key_size == 448)
        {
            /* Pass the correct key size to PSA_SIGN_OUTPUT_SIZE() */
            key_size = 456;
        }
    }
    else
    {
        switch (ecdsa_type)
        {
            case CB_ECDSA_RANDOMIZED:
                context->alg = PSA_ALG_ECDSA(context->hash_alg);
                break;
            case CB_ECDSA_DETERMINISTIC:
                context->alg = PSA_ALG_DETERMINISTIC_ECDSA(context->hash_alg);
                break;
            default:
                ret = CB_STATUS_FAIL;
                break;
        }
    }
    if (CB_STATUS_SUCCESS != ret)
    {
        free(context);
        return ret;
    }

    context->sig_size = PSA_SIGN_OUTPUT_SIZE(context->key_type, key_size, context->alg);

    context->input_type = input_type;
    if (CB_SIGVER_MESSAGE == input_type)
    {
        context->usage = PSA_KEY_USAGE_SIGN_MESSAGE | PSA_KEY_USAGE_VERIFY_MESSAGE;
    }
    else
    {
        context->usage = PSA_KEY_USAGE_SIGN_HASH | PSA_KEY_USAGE_VERIFY_HASH;
    }

    /* Save the context */
    *ctx_internal = (void *)context;
    return ret;
}

/*******************************************************************************
 *                          TRANSPARENT WRAPPERS                               *
*******************************************************************************/

cb_status_t wrapper_ecdsa_init(void **ctx_internal,
                               cb_ecdsa_type_t ecdsa_type,
                               cb_hash_t hash_type,
                               cb_ecc_family_t family,
                               size_t key_size,
                               cb_asym_sigver_input_type_t input_type)
{
    psa_status_t status;
    cb_status_t ret;
    ecdsa_ctx_psa_t *context = NULL;

    ret = wrapper_ecdsa_init_common(ctx_internal, ecdsa_type, hash_type,
                                    family, key_size, input_type);
    if (CB_STATUS_SUCCESS != ret)
    {
        return ret;
    }

    context = (ecdsa_ctx_psa_t *)(*ctx_internal);

    if (NULL == context)
    {
        return CB_STATUS_FAIL;
    }

    /************* transparent ECDSA needs keygen **********/
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


cb_status_t wrapper_ecdsa_deinit(void *ctx_internal)
{
    ecdsa_ctx_psa_t *ctx = (ecdsa_ctx_psa_t *)ctx_internal;
    psa_status_t status;

    if (NULL == ctx_internal)
    {
        return CB_STATUS_FAIL;
    }

    /* Destroy the key */
    status = psa_destroy_key(ctx->key_id);

    /* NULL check already done */
    memset(ctx_internal, 0, sizeof(ecdsa_ctx_psa_t));
    free(ctx_internal);

    return (status == PSA_SUCCESS) ? CB_STATUS_SUCCESS : CB_STATUS_FAIL;
}


cb_status_t wrapper_ecdsa_sign_compute(void *ctx_internal,
                                       const uint8_t *private_key,
                                       uint8_t *message,
                                       size_t message_size,
                                       uint8_t *signature)
{
    psa_status_t status     = PSA_ERROR_CORRUPTION_DETECTED;
    ecdsa_ctx_psa_t *ctx    = (ecdsa_ctx_psa_t *)ctx_internal;
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
        /* message size has to match hash length. assuming hash length passed is appropriate to avoid checking */
        status = psa_sign_hash(ctx->key_id, ctx->alg,
                               message, PSA_HASH_LENGTH(ctx->hash_alg),
                               signature, ctx->sig_size,
                               &signature_length);
    }

    return (status == PSA_SUCCESS) ? CB_STATUS_SUCCESS : CB_STATUS_FAIL;
}

cb_status_t wrapper_ecdsa_verify_compute(void *ctx_internal,
                                         const uint8_t *public_key,
                                         uint8_t *message,
                                         size_t message_size,
                                         uint8_t *signature)
{
    psa_status_t status  = PSA_ERROR_CORRUPTION_DETECTED;
    ecdsa_ctx_psa_t *ctx = (ecdsa_ctx_psa_t *)ctx_internal;

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
        /* message size has to match hash length. assuming hash length passed is appropriate to avoid checking */
        status = psa_verify_hash(ctx->key_id, ctx->alg,
                                 message, PSA_HASH_LENGTH(ctx->hash_alg),
                                 signature, ctx->sig_size);
    }

    return (status == PSA_SUCCESS) ? CB_STATUS_SUCCESS : CB_STATUS_FAIL;
}


/*******************************************************************************
 *                            OPAQUE WRAPPERS                                  *
*******************************************************************************/

cb_status_t wrapper_ecdsa_opaque_init(
    void **ctx_internal, cb_ecdsa_type_t ecdsa_type, cb_hash_t hash_type, cb_ecc_family_t family, size_t key_size,
    cb_asym_sigver_input_type_t input_type)
{
    return wrapper_ecdsa_init_common(ctx_internal, ecdsa_type, hash_type,
                                     family, key_size, input_type);
}

cb_status_t wrapper_ecdsa_opaque_deinit(void *ctx_internal)
{
    return wrapper_ecdsa_deinit(ctx_internal);
}

cb_status_t wrapper_ecdsa_sign_opaque_setup(void *ctx_internal)
{
    (void)ctx_internal;

    return CB_STATUS_SUCCESS;
}

cb_status_t wrapper_ecdsa_sign_opaque_cleanup(void *ctx_internal)
{
    (void)ctx_internal;

    return CB_STATUS_SUCCESS;
}

cb_status_t wrapper_ecdsa_sign_opaque_key_generate(void *ctx_internal,
                                                   uint32_t *key_id,
                                                   size_t key_size,
                                                   uint8_t *public_key_addr,
                                                   size_t *public_key_size,
                                                   size_t public_key_buf_size)
{
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    ecdsa_ctx_psa_t *context        = (ecdsa_ctx_psa_t *)ctx_internal;
    psa_key_lifetime_t lifetime     =
        PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION( \
            PSA_KEY_LIFETIME_VOLATILE, OPAQUE_KEY_LOCATION);
    psa_status_t status;

    if (NULL == ctx_internal)
    {
        return CB_STATUS_FAIL;
    }

    /* Generate a key */
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

cb_status_t wrapper_ecdsa_sign_opaque_key_delete(void *ctx_internal, uint32_t key_id)
{
    (void)ctx_internal;
    (void)key_id;

    return CB_STATUS_SUCCESS;
}

cb_status_t wrapper_ecdsa_sign_opaque_compute(void *ctx_internal, const uint8_t *message, size_t message_size, uint8_t *signature, uint32_t key_id)
{
    psa_status_t status     = PSA_ERROR_CORRUPTION_DETECTED;
    ecdsa_ctx_psa_t *ctx    = (ecdsa_ctx_psa_t *)ctx_internal;
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
        /* message size has to match hash length. assuming hash length passed is appropriate to avoid checking */
        status = psa_sign_hash(ctx->key_id, ctx->alg,
                               message, PSA_HASH_LENGTH(ctx->hash_alg),
                               signature, ctx->sig_size,
                               &signature_length);
    }

    return (status == PSA_SUCCESS) ? CB_STATUS_SUCCESS : CB_STATUS_FAIL;
}

cb_status_t wrapper_ecdsa_verify_opaque_compute(void *ctx_internal, const uint8_t *message, size_t message_size, uint8_t *signature, uint8_t *public_key, size_t public_key_size)
{
    psa_status_t status  = PSA_ERROR_CORRUPTION_DETECTED;
    ecdsa_ctx_psa_t *ctx = (ecdsa_ctx_psa_t *)ctx_internal;

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
        /* message size has to match hash length. assuming hash length passed is appropriate to avoid checking */
        status = psa_verify_hash(ctx->key_id, ctx->alg,
                                 message, PSA_HASH_LENGTH(ctx->hash_alg),
                                 signature, ctx->sig_size);
    }

    return (status == PSA_SUCCESS) ? CB_STATUS_SUCCESS : CB_STATUS_FAIL;
}
