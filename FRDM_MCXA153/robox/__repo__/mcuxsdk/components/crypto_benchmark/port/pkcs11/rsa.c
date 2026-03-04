/*
 * Copyright 2025 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "cb_osal.h"
#include "cb_rsa.h"

#include "include/common_utils.h"

typedef struct _rsa_ctx_pkcs11
{
    session_ctx_pkcs11_t session_ctx;
    CK_MECHANISM key_mech;
    CK_BBOOL ck_true;
    CK_MECHANISM_TYPE key_allowed_mech[1];
    CK_ULONG modulus_bits;
    CK_ULONG private_exponent_size;
    CK_ULONG public_key_buffer_size;
    CK_ATTRIBUTE privkey_attrs[2];
    CK_ATTRIBUTE pubkey_attrs[2];
    CK_OBJECT_HANDLE hpubkey;
    CK_OBJECT_HANDLE hprivkey;
    CK_MECHANISM sign_verify_mech;
    cb_asym_sigver_input_type_t input_type;
    CK_ULONG signature_size;
} rsa_ctx_pkcs11_t;

static cb_status_t set_rsa_pkcs_mech(cb_hash_t hash_type, CK_MECHANISM_TYPE_PTR mechanism)
{
    cb_status_t status = CB_STATUS_SUCCESS;

    switch(hash_type)
    {
        case CB_HASH_TYPE_SHA1:
            *mechanism = CKM_SHA1_RSA_PKCS;
            break;
        case CB_HASH_TYPE_SHA224:
            *mechanism = CKM_SHA224_RSA_PKCS;
            break;
        case CB_HASH_TYPE_SHA256:
            *mechanism = CKM_SHA256_RSA_PKCS;
            break;
        case CB_HASH_TYPE_SHA384:
            *mechanism = CKM_SHA384_RSA_PKCS;
            break;
        case CB_HASH_TYPE_SHA512:
            *mechanism = CKM_SHA512_RSA_PKCS;
            break;
        case CB_HASH_TYPE_SHA3_224:
            *mechanism = CKM_SHA3_224_RSA_PKCS;
            break;
        case CB_HASH_TYPE_SHA3_256:
            *mechanism = CKM_SHA3_256_RSA_PKCS;
            break;
        case CB_HASH_TYPE_SHA3_384:
            *mechanism = CKM_SHA3_384_RSA_PKCS;
            break;
        case CB_HASH_TYPE_SHA3_512:
            *mechanism = CKM_SHA3_512_RSA_PKCS;
            break;
        default:
            status = CB_STATUS_FAIL;
            break;
    }

    return status;
}

static cb_status_t set_rsa_pss_mech(cb_hash_t hash_type, CK_MECHANISM_TYPE_PTR mechanism)
{
    cb_status_t status = CB_STATUS_SUCCESS;

    switch(hash_type)
    {
        case CB_HASH_TYPE_SHA1:
            *mechanism = CKM_SHA1_RSA_PKCS_PSS;
            break;
        case CB_HASH_TYPE_SHA224:
            *mechanism = CKM_SHA224_RSA_PKCS_PSS;
            break;
        case CB_HASH_TYPE_SHA256:
            *mechanism = CKM_SHA256_RSA_PKCS_PSS;
            break;
        case CB_HASH_TYPE_SHA384:
            *mechanism = CKM_SHA384_RSA_PKCS_PSS;
            break;
        case CB_HASH_TYPE_SHA512:
            *mechanism = CKM_SHA512_RSA_PKCS_PSS;
            break;
        case CB_HASH_TYPE_SHA3_224:
            *mechanism = CKM_SHA3_224_RSA_PKCS_PSS;
            break;
        case CB_HASH_TYPE_SHA3_256:
            *mechanism = CKM_SHA3_256_RSA_PKCS_PSS;
            break;
        case CB_HASH_TYPE_SHA3_384:
            *mechanism = CKM_SHA3_384_RSA_PKCS_PSS;
            break;
        case CB_HASH_TYPE_SHA3_512:
            *mechanism = CKM_SHA3_512_RSA_PKCS_PSS;
            break;
        default:
            status = CB_STATUS_FAIL;
            break;
    }

    return status;
}

static cb_status_t set_rsa_mech(cb_rsa_type_t rsa_type, cb_hash_t hash_type, CK_MECHANISM_TYPE_PTR mechanism)
{
    cb_status_t status = CB_STATUS_SUCCESS;

    switch(rsa_type)
    {
        case CB_RSA_PKCS1V15_SIGN:
            status = set_rsa_pkcs_mech(hash_type, mechanism);
            break;
        case CB_RSA_PSS:
            status = set_rsa_pss_mech(hash_type, mechanism);
            break;
        default:
            status = CB_STATUS_FAIL;
            break;
    }

    return status;
}

static cb_status_t set_context(rsa_ctx_pkcs11_t *ctx, cb_rsa_type_t rsa_type, cb_hash_t hash_type, size_t key_size, size_t public_key_buffer_size)
{
    cb_status_t status = CB_STATUS_FAIL;

    if (NULL == ctx)
    {
        return CB_STATUS_FAIL;
    }

    ctx->hprivkey = 0;
    ctx->hpubkey = 0;
    ctx->key_mech.mechanism = CKM_RSA_PKCS_KEY_PAIR_GEN;
    ctx->ck_true = CK_TRUE;
    ctx->modulus_bits = key_size;
    ctx->public_key_buffer_size = public_key_buffer_size;

    status = set_rsa_mech(rsa_type, hash_type, &ctx->key_allowed_mech[0]);
    if (CB_STATUS_SUCCESS != status)
    {
        return status;
    }

    ctx->sign_verify_mech.mechanism = ctx->key_allowed_mech[0];
    ctx->sign_verify_mech.pParameter = NULL_PTR;
    ctx->sign_verify_mech.ulParameterLen = 0;

    SET_ATTRIBUTE(ctx->privkey_attrs[0], CKA_SIGN, ctx->ck_true);
    SET_ATTRIBUTE(ctx->privkey_attrs[1], CKA_ALLOWED_MECHANISMS, ctx->key_allowed_mech);

    SET_ATTRIBUTE(ctx->pubkey_attrs[0], CKA_VERIFY, ctx->ck_true);
    SET_ATTRIBUTE(ctx->pubkey_attrs[1], CKA_MODULUS_BITS, ctx->modulus_bits);

    return CB_STATUS_SUCCESS;
}

static cb_status_t wrapper_rsa_init_common(void **ctx_internal,
                                           cb_rsa_type_t rsa_type,
                                           cb_hash_t hash_type,
                                           size_t key_size,
                                           size_t public_key_buffer_size,
                                           cb_asym_sigver_input_type_t input_type)
{
    cb_status_t status = CB_STATUS_FAIL;

    rsa_ctx_pkcs11_t *ctx = NULL;
    session_ctx_pkcs11_t session_ctx = { 0 };

    if (NULL == ctx_internal)
    {
        return CB_STATUS_FAIL;
    }

    /* PKCS11 does not handled hashed input. */
    if (input_type == CB_SIGVER_HASH)
    {
        return CB_STATUS_FAIL;
    }

    *ctx_internal = NULL;

    /* Allocate the internal PKCS11 context */
    ctx = (rsa_ctx_pkcs11_t *)malloc(sizeof(rsa_ctx_pkcs11_t));
    if (NULL == ctx)
    {
        return CB_STATUS_FAIL;
    }

    status = set_context(ctx, rsa_type, hash_type, key_size, public_key_buffer_size);
    if (CB_STATUS_SUCCESS != status)
    {
        goto end;
    }

    status = open_pkcs11_session(&session_ctx);
    if (CB_STATUS_SUCCESS != status)
    {
        goto end;
    }

    ctx->session_ctx = session_ctx;

    /* Save the context */
    *ctx_internal = (void *)ctx;

end:
    if (CB_STATUS_SUCCESS != status)
    {
        close_pkcs11_session(&session_ctx);

        if (NULL != ctx)
        {
            free(ctx);
        }
    }

    return status;
}

/*******************************************************************************
 *                         TRANSPARENT WRAPPERS                                *
*******************************************************************************/

cb_status_t wrapper_rsa_init(
    void **ctx_internal, cb_rsa_type_t rsa_type, cb_hash_t hash_type, size_t key_size, size_t public_key_buffer_size, cb_asym_sigver_input_type_t input_type)
{
    cb_status_t status = CB_STATUS_FAIL;
    rsa_ctx_pkcs11_t *ctx = NULL;
    CK_RV ret = CKR_OK;

    status = wrapper_rsa_init_common(ctx_internal, rsa_type, hash_type,
                                     key_size, public_key_buffer_size, input_type);
    if (CB_STATUS_SUCCESS != status)
    {
        return status;
    }

    ctx = (rsa_ctx_pkcs11_t *)*ctx_internal;
    if (NULL == ctx)
    {
        return CB_STATUS_FAIL;
    }

    ret = ctx->session_ctx.pfunc->C_GenerateKeyPair(ctx->session_ctx.sess,
                                                    &ctx->key_mech,
                                                    ctx->pubkey_attrs,
                                                    ARRAY_SIZE(ctx->pubkey_attrs),
                                                    ctx->privkey_attrs,
                                                    ARRAY_SIZE(ctx->privkey_attrs),
                                                    &ctx->hpubkey,
                                                    &ctx->hprivkey);

    if (CKR_OK != ret)
    {
        (void)wrapper_rsa_deinit(ctx);
    }

    return (ret == CKR_OK) ? CB_STATUS_SUCCESS : CB_STATUS_FAIL;
}

cb_status_t wrapper_rsa_deinit(void *ctx_internal)
{
    rsa_ctx_pkcs11_t *ctx = (rsa_ctx_pkcs11_t *)ctx_internal;

    if (NULL != ctx_internal)
    {
        if (ctx->session_ctx.pfunc)
        {
            if (ctx->hpubkey)
            {
                (void)ctx->session_ctx.pfunc->C_DestroyObject(ctx->session_ctx.sess, ctx->hpubkey);
            }

            if (ctx->hprivkey)
            {
                (void)ctx->session_ctx.pfunc->C_DestroyObject(ctx->session_ctx.sess, ctx->hprivkey);
            }
        }

        close_pkcs11_session(&ctx->session_ctx);

        free(ctx_internal);
    }

    return CB_STATUS_SUCCESS;
}

cb_status_t wrapper_rsa_keygen_compute(uint8_t *modulus,
                                       size_t modulus_buf_size,
                                       uint8_t *private_exponent,
                                       size_t private_exponent_buf_size,
                                       const uint8_t *public_exponent,
                                       size_t public_exponent_buf_size,
                                       size_t key_size)
{
    rsa_ctx_pkcs11_t *ctx = NULL;
    CK_OBJECT_CLASS private_key_class = CKO_PRIVATE_KEY;
    CK_OBJECT_CLASS public_key_class = CKO_PUBLIC_KEY;
    CK_KEY_TYPE key_type = CKK_RSA;
    cb_status_t status = CB_STATUS_FAIL;
    CK_RV ret = CKR_OK;

    status = wrapper_rsa_init_common((void **)&ctx, CB_RSA_PKCS1V15_SIGN, CB_HASH_TYPE_SHA512, modulus_buf_size, public_exponent_buf_size, CB_SIGVER_MESSAGE);
    if (CB_STATUS_SUCCESS != status)
    {
        return status;
    }

    ret = ctx->session_ctx.pfunc->C_GenerateKeyPair(ctx->session_ctx.sess,
                                                    &ctx->key_mech,
                                                    ctx->pubkey_attrs,
                                                    ARRAY_SIZE(ctx->pubkey_attrs),
                                                    ctx->privkey_attrs,
                                                    ARRAY_SIZE(ctx->privkey_attrs),
                                                    &ctx->hpubkey,
                                                    &ctx->hprivkey);

end:
    (void)wrapper_rsa_deinit(ctx);

    return (ret == CKR_OK) ? CB_STATUS_SUCCESS : CB_STATUS_FAIL;
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
    rsa_ctx_pkcs11_t *ctx = (rsa_ctx_pkcs11_t *)ctx_internal;
    CK_OBJECT_CLASS private_key_class = CKO_PRIVATE_KEY;
    CK_KEY_TYPE key_type = CKK_RSA;
    CK_ULONG signature_size = 0;
    CK_RV ret = CKR_OK;

    (void)modulus;
    (void)private_exponent;

    if (NULL == ctx)
    {
        return CB_STATUS_FAIL;
    }

    ret = ctx->session_ctx.pfunc->C_SignInit(ctx->session_ctx.sess, &ctx->sign_verify_mech, ctx->hprivkey);
    if (CKR_OK != ret)
    {
        goto end;
    }

    ret = ctx->session_ctx.pfunc->C_Sign(ctx->session_ctx.sess, NULL_PTR, 0, NULL_PTR, &ctx->signature_size);
    if (CKR_OK != ret)
    {
        goto end;
    }

    ret = ctx->session_ctx.pfunc->C_Sign(ctx->session_ctx.sess, (CK_BYTE_PTR)message, (CK_ULONG)message_size, (CK_BYTE_PTR)signature, &ctx->signature_size);

end:
    return (ret == CKR_OK) ? CB_STATUS_SUCCESS : CB_STATUS_FAIL;
}

cb_status_t wrapper_rsa_verify_compute(
    void *ctx_internal, const uint8_t *modulus, const uint8_t *public_exponent, uint8_t *message, size_t message_size, uint8_t *signature)
{
    rsa_ctx_pkcs11_t *ctx = (rsa_ctx_pkcs11_t *)ctx_internal;
    CK_OBJECT_CLASS public_key_class = CKO_PUBLIC_KEY;
    CK_KEY_TYPE key_type = CKK_RSA;
    CK_RV ret = CKR_OK;

    if (NULL == ctx)
    {
        return CB_STATUS_FAIL;
    }

    ret = ctx->session_ctx.pfunc->C_VerifyInit(ctx->session_ctx.sess, &ctx->sign_verify_mech, ctx->hpubkey);
    if (CKR_OK != ret)
    {
        goto end;
    }

    ret = ctx->session_ctx.pfunc->C_Verify(ctx->session_ctx.sess, message, message_size, signature, ctx->signature_size);

end:
    return (ret == CKR_OK) ? CB_STATUS_SUCCESS : CB_STATUS_FAIL;
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
    return wrapper_rsa_init_common(ctx_internal, rsa_type, hash_type, key_size, 3, input_type);
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
                                           size_t public_key_buffer_size)
{
    rsa_ctx_pkcs11_t *ctx = NULL;
    cb_status_t status = CB_STATUS_FAIL;
    CK_RV ret = CKR_OK;

    (void)key_id;
    (void)public_key_addr;
    (void)public_key_buffer_size;

    status = wrapper_rsa_init_common((void **)&ctx, CB_RSA_PKCS1V15_SIGN, CB_HASH_TYPE_SHA512, key_size, 3, CB_SIGVER_MESSAGE);
    if (CB_STATUS_SUCCESS != status)
    {
        return status;
    }

    ret = ctx->session_ctx.pfunc->C_GenerateKeyPair(ctx->session_ctx.sess,
                                                    &ctx->key_mech,
                                                    ctx->pubkey_attrs,
                                                    ARRAY_SIZE(ctx->pubkey_attrs),
                                                    ctx->privkey_attrs,
                                                    ARRAY_SIZE(ctx->privkey_attrs),
                                                    &ctx->hpubkey,
                                                    &ctx->hprivkey);

    (void)wrapper_rsa_opaque_deinit(ctx);

    return (ret == CKR_OK) ? CB_STATUS_SUCCESS : CB_STATUS_FAIL;
}

cb_status_t wrapper_rsa_keygen_opaque_key_delete(uint32_t key_id)
{
    /* PKCS11 destroys keys the same way for both transparent and opaque.
     * Can defer this to wrapper_rsa_opaque_deinit(). */
    (void)key_id;

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
    rsa_ctx_pkcs11_t *ctx = (rsa_ctx_pkcs11_t *)ctx_internal;
    CK_RV ret = CKR_OK;

    (void)key_id;
    (void)key_size;
    (void)public_key_addr;
    (void)public_key_size;
    (void)public_key_buf_size;

    if (NULL == ctx)
    {
        return CB_STATUS_FAIL;
    }

    ret = ctx->session_ctx.pfunc->C_GenerateKeyPair(ctx->session_ctx.sess,
                                                    &ctx->key_mech,
                                                    ctx->pubkey_attrs,
                                                    ARRAY_SIZE(ctx->pubkey_attrs),
                                                    ctx->privkey_attrs,
                                                    ARRAY_SIZE(ctx->privkey_attrs),
                                                    &ctx->hpubkey,
                                                    &ctx->hprivkey);

    return (ret == CKR_OK) ? CB_STATUS_SUCCESS : CB_STATUS_FAIL;
}

cb_status_t wrapper_rsa_sign_opaque_key_delete(void *ctx_internal, uint32_t key_id)
{
    /* PKCS11 destroys keys the same way for both transparent and opaque.
     * Can defer this to wrapper_rsa_opaque_deinit(). */
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
    (void)key_id;

    return wrapper_rsa_sign_compute(ctx_internal, NULL, NULL, (uint8_t *)message, message_size, signature);
}

cb_status_t wrapper_rsa_verify_opaque_compute(void *ctx_internal,
                                              const uint8_t *message,
                                              size_t message_size,
                                              uint8_t *signature,
                                              uint8_t *public_key,
                                              size_t public_key_size)
{
    (void)public_key;
    (void)public_key_size;

    return wrapper_rsa_verify_compute(ctx_internal, NULL, NULL, (uint8_t *)message, message_size, signature);
}
