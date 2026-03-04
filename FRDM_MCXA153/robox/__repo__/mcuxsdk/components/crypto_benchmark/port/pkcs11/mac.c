/*
 * Copyright 2025 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "cb_osal.h"
#include "cb_mac.h"

#include "include/common_utils.h"

typedef struct _mac_ctx_pkcs11
{
    session_ctx_pkcs11_t session_ctx;
    CK_MECHANISM key_mech;
    CK_OBJECT_CLASS secret_key_class;
    CK_BBOOL ck_true;
    CK_MECHANISM_TYPE key_allowed_mech[1];
    CK_ULONG key_length;
    CK_KEY_TYPE secret_key_type;
    CK_ATTRIBUTE key_attrs[7];
    CK_MECHANISM sign_verify_mech;
    CK_OBJECT_HANDLE hsecretkey;
} mac_ctx_pkcs11_t;

static cb_status_t set_hmac_key_params(cb_hash_t hash_type, CK_MECHANISM_TYPE_PTR key_allowed_mech, CK_KEY_TYPE *key_type)
{
    cb_status_t status = CB_STATUS_SUCCESS;

    switch (hash_type)
    {
        case CB_HASH_TYPE_SHA1:
            *key_allowed_mech = CKM_SHA_1_HMAC;
            *key_type = CKK_SHA_1_HMAC;
            break;
        case CB_HASH_TYPE_SHA224:
            *key_allowed_mech = CKM_SHA224_HMAC;
            *key_type = CKK_SHA224_HMAC;
            break;
        case CB_HASH_TYPE_SHA256:
            *key_allowed_mech = CKM_SHA256_HMAC;
            *key_type = CKK_SHA256_HMAC;
            break;
        case CB_HASH_TYPE_SHA384:
            *key_allowed_mech = CKM_SHA384_HMAC;
            *key_type = CKK_SHA384_HMAC;
            break;
        case CB_HASH_TYPE_SHA512:
            *key_allowed_mech = CKM_SHA512_HMAC;
            *key_type = CKK_SHA512_HMAC;
            break;
        case CB_HASH_TYPE_SHA3_224:
            *key_allowed_mech = CKM_SHA3_224_HMAC;
            *key_type = CKK_SHA3_224_HMAC;
            break;
        case CB_HASH_TYPE_SHA3_256:
            *key_allowed_mech = CKM_SHA3_256_HMAC;
            *key_type = CKK_SHA3_256_HMAC;
            break;
        case CB_HASH_TYPE_SHA3_384:
            *key_allowed_mech = CKM_SHA3_384_HMAC;
            *key_type = CKK_SHA3_384_HMAC;
            break;
        case CB_HASH_TYPE_SHA3_512:
            *key_allowed_mech = CKM_SHA3_512_HMAC;
            *key_type = CKK_SHA3_512_HMAC;
            break;
        default:
            status = CB_STATUS_FAIL;
            break;
    }

    return status;
}

static cb_status_t set_key_params(mac_ctx_pkcs11_t *ctx, cb_mac_algorithm_t algorithm, cb_hash_t hash_type, size_t key_size)
{
    cb_status_t status = CB_STATUS_SUCCESS;

    if (NULL == ctx)
    {
        return CB_STATUS_FAIL;
    }

    ctx->hsecretkey = 0;
    ctx->secret_key_class = CKO_SECRET_KEY;
    ctx->ck_true = CK_TRUE;
    ctx->key_length = (CK_ULONG)key_size >> 3;

    switch (algorithm)
    {
        case CB_MAC_ALGORITHM_HMAC:
            ctx->key_mech.mechanism = CKM_GENERIC_SECRET_KEY_GEN;
            status = set_hmac_key_params(hash_type, &ctx->key_allowed_mech[0], &ctx->secret_key_type);
            break;
        case CB_MAC_ALGORITHM_CMAC:
            ctx->key_mech.mechanism = CKM_AES_KEY_GEN;
            ctx->key_allowed_mech[0] = CKM_AES_CMAC;
            ctx->secret_key_type = CKK_AES;
            break;
        default:
            status = CB_STATUS_FAIL;
            break;
    }

    if (CB_STATUS_SUCCESS != status)
    {
        return status;
    }

    ctx->sign_verify_mech.mechanism = ctx->key_allowed_mech[0];
    ctx->sign_verify_mech.pParameter = NULL_PTR;
    ctx->sign_verify_mech.ulParameterLen = 0;

    SET_ATTRIBUTE(ctx->key_attrs[0], CKA_CLASS, ctx->secret_key_class);
    SET_ATTRIBUTE(ctx->key_attrs[1], CKA_SIGN, ctx->ck_true);
    SET_ATTRIBUTE(ctx->key_attrs[2], CKA_VERIFY, ctx->ck_true);
    SET_ATTRIBUTE(ctx->key_attrs[3], CKA_ALLOWED_MECHANISMS, ctx->key_allowed_mech);
    SET_ATTRIBUTE(ctx->key_attrs[4], CKA_KEY_TYPE, ctx->secret_key_type);
    SET_ATTRIBUTE(ctx->key_attrs[5], CKA_VALUE_LEN, ctx->key_length);

    return CB_STATUS_SUCCESS;
}

/*******************************************************************************
 *                         TRANSPARENT WRAPPERS                                *
*******************************************************************************/

static cb_status_t wrapper_mac_init_common(void **ctx_internal,
                                           cb_mac_algorithm_t algorithm,
                                           cb_hash_t hash_type,
                                           size_t key_size)
{
    cb_status_t status = CB_STATUS_FAIL;

    mac_ctx_pkcs11_t *ctx = NULL;
    session_ctx_pkcs11_t session_ctx = { 0 };

    if (NULL == ctx_internal)
    {
        return CB_STATUS_FAIL;
    }

    *ctx_internal = NULL;

    /* Allocate the internal PKCS11 context */
    ctx = (mac_ctx_pkcs11_t *)malloc(sizeof(mac_ctx_pkcs11_t));
    if (NULL == ctx)
    {
        return CB_STATUS_FAIL;
    }

    status = set_key_params(ctx, algorithm, hash_type, key_size);
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

cb_status_t wrapper_mac_init(
    void **ctx_internal, cb_mac_algorithm_t algorithm, cb_hash_t hash_type, const uint8_t *key, size_t key_size)
{
    cb_status_t status = CB_STATUS_FAIL;
    mac_ctx_pkcs11_t *ctx = NULL;
    CK_RV ret = CKR_OK;

    (void)key;

    status = wrapper_mac_init_common(ctx_internal, algorithm, hash_type, key_size);
    if (CB_STATUS_SUCCESS != status)
    {
        return status;
    }

    ctx = (mac_ctx_pkcs11_t *)*ctx_internal;
    if (NULL == ctx)
    {
        return CB_STATUS_FAIL;
    }

    ctx->key_attrs[5].type = CKA_VALUE;
    ctx->key_attrs[5].pValue = &key;
    ctx->key_attrs[5].ulValueLen = (CK_ULONG)key_size >> 3;

    SET_ATTRIBUTE(ctx->key_attrs[6], CKA_TOKEN, ctx->ck_true);

    /* Import a key */
    ret = ctx->session_ctx.pfunc->C_CreateObject(ctx->session_ctx.sess,
                                                 ctx->key_attrs,
                                                 ARRAY_SIZE(ctx->key_attrs),
                                                 &ctx->hsecretkey);
    if (CKR_OK != ret)
    {
        (void)wrapper_mac_deinit(ctx);
    }

    return (ret == CKR_OK) ? CB_STATUS_SUCCESS : CB_STATUS_FAIL;
}

cb_status_t wrapper_mac_deinit(void *ctx_internal)
{
    mac_ctx_pkcs11_t *ctx = (mac_ctx_pkcs11_t *)ctx_internal;

    if (NULL != ctx_internal)
    {
        if (ctx->hsecretkey && ctx->session_ctx.pfunc)
        {
            (void)ctx->session_ctx.pfunc->C_DestroyObject(ctx->session_ctx.sess, ctx->hsecretkey);
        }

        close_pkcs11_session(&ctx->session_ctx);

        free(ctx_internal);
    }

    return CB_STATUS_SUCCESS;
}

cb_status_t wrapper_mac_compute(void *ctx_internal,
                                const uint8_t *message,
                                size_t message_size,
                                uint8_t *mac,
                                const uint8_t *key)
{
    mac_ctx_pkcs11_t *ctx = (mac_ctx_pkcs11_t *)ctx_internal;
    CK_ULONG signature_size = 0;
    CK_RV ret = CKR_OK;

    (void)mac;
    (void)key;

    if (NULL == ctx)
    {
        return CB_STATUS_FAIL;
    }

    ret = ctx->session_ctx.pfunc->C_SignInit(ctx->session_ctx.sess, &ctx->sign_verify_mech, ctx->hsecretkey);
    if (CKR_OK != ret)
    {
        goto end;
    }

    ret = ctx->session_ctx.pfunc->C_Sign(ctx->session_ctx.sess, NULL_PTR, 0, NULL_PTR, &signature_size);
    if (CKR_OK != ret)
    {
        goto end;
    }

    ret = ctx->session_ctx.pfunc->C_Sign(ctx->session_ctx.sess, (CK_BYTE_PTR)message, (CK_ULONG)message_size, (CK_BYTE_PTR)mac, &signature_size);

end:
    return (ret == CKR_OK) ? CB_STATUS_SUCCESS : CB_STATUS_FAIL;
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

cb_status_t wrapper_mac_opaque_compute(void *ctx_internal,
                                       const uint8_t *message,
                                       size_t message_size,
                                       uint8_t *mac,
                                       uint32_t key_id)
{
    (void)key_id;

    return wrapper_mac_compute(ctx_internal, message, message_size, mac, NULL);
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
    mac_ctx_pkcs11_t *ctx = (mac_ctx_pkcs11_t *)ctx_internal;
    CK_RV ret = CKR_OK;

    (void)key_size;
    (void)key_id;

    if (NULL == ctx)
    {
        return CB_STATUS_FAIL;
    }

    ret = ctx->session_ctx.pfunc->C_GenerateKey(ctx->session_ctx.sess,
                                                &ctx->key_mech,
                                                ctx->key_attrs,
                                                ARRAY_SIZE(ctx->key_attrs) - 1,
                                                &ctx->hsecretkey);

    return (ret == CKR_OK) ? CB_STATUS_SUCCESS : CB_STATUS_FAIL;
}

cb_status_t wrapper_mac_opaque_key_delete(void *ctx_internal, uint32_t key_id)
{
    /* PKCS11 destroys keys the same way for both transparent and opaque.
     * Can defer this to wrapper_mac_opaque_deinit(). */
    (void)ctx_internal;
    (void)key_id;

    return CB_STATUS_SUCCESS;
}
