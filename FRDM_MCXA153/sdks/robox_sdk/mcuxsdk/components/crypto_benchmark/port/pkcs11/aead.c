/*
 * Copyright 2025 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "cb_osal.h"
#include "cb_aead.h"

#include "include/common_utils.h"

typedef struct _aead_ctx_pkcs11
{
    session_ctx_pkcs11_t session_ctx;
    CK_MECHANISM key_mech;
    CK_OBJECT_CLASS secret_key_class;
    CK_BBOOL ck_true;
    CK_ULONG key_length;
    CK_MECHANISM_TYPE key_allowed_mech[1];
    CK_ATTRIBUTE key_attrs[7];
    CK_MECHANISM aead_mech;
    CK_OBJECT_HANDLE hsecretkey;
} aead_ctx_pkcs11_t;

static cb_status_t set_key_params(aead_ctx_pkcs11_t *ctx, cb_aead_type_t aead_type, size_t key_size)
{
    cb_status_t status = CB_STATUS_SUCCESS;

    if (NULL == ctx)
    {
        return CB_STATUS_FAIL;
    }

    ctx->hsecretkey = 0;
    ctx->key_mech.mechanism = CKM_AES_KEY_GEN;
    ctx->secret_key_class = CKO_SECRET_KEY;
    ctx->ck_true = CK_TRUE;
    ctx->key_length = (CK_ULONG)key_size >> 3;

    switch (aead_type)
    {
        case CB_AEAD_TYPE_CCM:
            ctx->key_allowed_mech[0] = CKM_AES_CCM;
            break;
        case CB_AEAD_TYPE_GCM:
            ctx->key_allowed_mech[0] = CKM_AES_GCM;
            break;
        case CB_AEAD_TYPE_CHACHA20_POLY1305:
            ctx->key_allowed_mech[0] = CKM_CHACHA20_POLY1305;
            break;
        default:
            status = CB_STATUS_FAIL;
            break;
    }

    SET_ATTRIBUTE(ctx->key_attrs[0], CKA_CLASS, ctx->secret_key_class);
    SET_ATTRIBUTE(ctx->key_attrs[1], CKA_ENCRYPT, ctx->ck_true);
    SET_ATTRIBUTE(ctx->key_attrs[2], CKA_DECRYPT, ctx->ck_true);
    SET_ATTRIBUTE(ctx->key_attrs[3], CKA_ALLOWED_MECHANISMS, ctx->key_allowed_mech);
    SET_ATTRIBUTE(ctx->key_attrs[4], CKA_VALUE_LEN, ctx->key_length);

    return status;
}

static cb_status_t set_aead_mech(aead_ctx_pkcs11_t *ctx, cb_aead_type_t aead_type)
{
    cb_status_t status = CB_STATUS_SUCCESS;

    if (NULL == ctx)
    {
        return CB_STATUS_FAIL;
    }

    switch (aead_type)
    {
        case CB_AEAD_TYPE_CCM:
            ctx->aead_mech.mechanism = CKM_AES_CCM;
            ctx->aead_mech.ulParameterLen = sizeof(CK_CCM_PARAMS);
            break;
        case CB_AEAD_TYPE_GCM:
            ctx->aead_mech.mechanism = CKM_AES_GCM;
            ctx->aead_mech.ulParameterLen = sizeof(CK_GCM_PARAMS);
            break;
        case CB_AEAD_TYPE_CHACHA20_POLY1305:
            ctx->aead_mech.mechanism = CKM_CHACHA20_POLY1305;
            ctx->aead_mech.ulParameterLen = sizeof(CK_SALSA20_CHACHA20_POLY1305_PARAMS);
            break;
        default:
            status = CB_STATUS_FAIL;
            break;
    }

    if (CB_STATUS_SUCCESS == status)
    {
        ctx->aead_mech.pParameter = (CK_VOID_PTR *)malloc(ctx->aead_mech.ulParameterLen);
        if (NULL == ctx->aead_mech.pParameter)
        {
            status = CB_STATUS_FAIL;
        }
    }

    return status;
}

static inline cb_status_t update_aead_mech(aead_ctx_pkcs11_t *ctx,
                                           size_t message_size,
                                           const uint8_t *iv,
                                           size_t iv_size,
                                           const uint8_t *aad,
                                           size_t aad_size,
                                           size_t tag_size)
{
    cb_status_t status = CB_STATUS_SUCCESS;

    if (NULL == ctx || NULL == ctx->aead_mech.pParameter)
    {
        return CB_STATUS_FAIL;
    }

    switch (ctx->aead_mech.mechanism)
    {
        case CKM_AES_CCM:
            ((CK_CCM_PARAMS_PTR)ctx->aead_mech.pParameter)->pAAD = (CK_BYTE_PTR)aad;
            ((CK_CCM_PARAMS_PTR)ctx->aead_mech.pParameter)->ulAADLen = (CK_ULONG)aad_size;
            ((CK_CCM_PARAMS_PTR)ctx->aead_mech.pParameter)->pNonce = (CK_BYTE_PTR)iv;
            ((CK_CCM_PARAMS_PTR)ctx->aead_mech.pParameter)->ulNonceLen = (CK_ULONG)iv_size;
            ((CK_CCM_PARAMS_PTR)ctx->aead_mech.pParameter)->ulDataLen = (CK_ULONG)message_size;
            ((CK_CCM_PARAMS_PTR)ctx->aead_mech.pParameter)->ulMACLen = (CK_ULONG)AEAD_BLOCK;
            break;
        case CKM_AES_GCM:
            ((CK_GCM_PARAMS_PTR)ctx->aead_mech.pParameter)->pAAD = (CK_BYTE_PTR)aad;
            ((CK_GCM_PARAMS_PTR)ctx->aead_mech.pParameter)->ulAADLen = (CK_ULONG)aad_size;
            ((CK_GCM_PARAMS_PTR)ctx->aead_mech.pParameter)->pIv = (CK_BYTE_PTR)iv;
            ((CK_GCM_PARAMS_PTR)ctx->aead_mech.pParameter)->ulIvLen = (CK_ULONG)iv_size;
            ((CK_GCM_PARAMS_PTR)ctx->aead_mech.pParameter)->ulTagBits = (CK_ULONG)(tag_size << 3);
            break;
        case CKM_CHACHA20_POLY1305:
            ((CK_SALSA20_CHACHA20_POLY1305_PARAMS_PTR)ctx->aead_mech.pParameter)->pAAD = (CK_BYTE_PTR)aad;
            ((CK_SALSA20_CHACHA20_POLY1305_PARAMS_PTR)ctx->aead_mech.pParameter)->ulAADLen = (CK_ULONG)aad_size;
            ((CK_SALSA20_CHACHA20_POLY1305_PARAMS_PTR)ctx->aead_mech.pParameter)->pNonce = (CK_BYTE_PTR)iv;
            ((CK_SALSA20_CHACHA20_POLY1305_PARAMS_PTR)ctx->aead_mech.pParameter)->ulNonceLen = (CK_ULONG)iv_size;
            break;
        default:
            status = CB_STATUS_FAIL;
            break;
    }

    return status;
}

static cb_status_t wrapper_aead_init_common(void **ctx_internal, cb_aead_type_t aead_type, size_t key_size)
{
    cb_status_t status = CB_STATUS_FAIL;

    aead_ctx_pkcs11_t *ctx = NULL;
    session_ctx_pkcs11_t session_ctx = { 0 };

    if (NULL == ctx_internal)
    {
        return CB_STATUS_FAIL;
    }

    *ctx_internal = NULL;

    /* Allocate the internal PKCS11 context */
    ctx = (aead_ctx_pkcs11_t *)malloc(sizeof(aead_ctx_pkcs11_t));
    if (NULL == ctx)
    {
        return CB_STATUS_FAIL;
    }

    status = set_key_params(ctx, aead_type, key_size);
    if (CB_STATUS_SUCCESS != status)
    {
        goto end;
    }

    status = set_aead_mech(ctx, aead_type);
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
            if (NULL != ctx->aead_mech.pParameter)
            {
                free(ctx->aead_mech.pParameter);
            }

            free(ctx);
        }
    }

    return status;
}

/*******************************************************************************
 *                         TRANSPARENT WRAPPERS                                *
*******************************************************************************/

cb_status_t wrapper_aead_init(void **ctx_internal, cb_aead_type_t aead_type, const uint8_t *key, size_t key_size)
{
    cb_status_t status = CB_STATUS_FAIL;
    aead_ctx_pkcs11_t *ctx = NULL;
    CK_KEY_TYPE keyType = CKK_AES;
    CK_RV ret = CKR_OK;

    status = wrapper_aead_init_common(ctx_internal, aead_type, key_size);
    if (CB_STATUS_SUCCESS != status)
    {
        return status;
    }

    ctx = (aead_ctx_pkcs11_t *)*ctx_internal;
    if (NULL == ctx)
    {
        return CB_STATUS_FAIL;
    }

    SET_ATTRIBUTE(ctx->key_attrs[4], CKA_KEY_TYPE, keyType);

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
        (void)wrapper_aead_deinit(ctx);
    }

    return (ret == CKR_OK) ? CB_STATUS_SUCCESS : CB_STATUS_FAIL;
}

cb_status_t wrapper_aead_deinit(void *ctx_internal)
{
    aead_ctx_pkcs11_t *ctx = (aead_ctx_pkcs11_t *)ctx_internal;

    if (NULL != ctx_internal)
    {
        if (ctx->hsecretkey && ctx->session_ctx.pfunc)
        {
            (void)ctx->session_ctx.pfunc->C_DestroyObject(ctx->session_ctx.sess, ctx->hsecretkey);
        }

        if (NULL != ctx->aead_mech.pParameter)
        {
            free(ctx->aead_mech.pParameter);
        }

        close_pkcs11_session(&ctx->session_ctx);

        free(ctx_internal);
    }

    return CB_STATUS_SUCCESS;
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
    cb_status_t status = CB_STATUS_FAIL;
    aead_ctx_pkcs11_t *ctx = (aead_ctx_pkcs11_t *)ctx_internal;
    CK_ULONG ciphertext_size = 0;
    CK_RV ret = CKR_OK;

    (void)key;
    (void)key_size;

    if (NULL == ctx)
    {
        return CB_STATUS_FAIL;
    }

    status = update_aead_mech(ctx, message_size, iv, iv_size, aad, aad_size, tag_size);
    if (CB_STATUS_SUCCESS != status)
    {
        return status;
    }

    ret = ctx->session_ctx.pfunc->C_EncryptInit(ctx->session_ctx.sess, &ctx->aead_mech, ctx->hsecretkey);
    if (CKR_OK != ret)
    {
        goto end;
    }

    ret = ctx->session_ctx.pfunc->C_Encrypt(ctx->session_ctx.sess, (CK_BYTE_PTR)message, (CK_ULONG)message_size, NULL_PTR, &ciphertext_size);
    if (CKR_OK != ret)
    {
        goto end;
    }

    if (COMMON_OUTPUT_SIZE < ciphertext_size + tag_size)
    {
        ret = CKR_HOST_MEMORY;
        goto end;
    }

    ret = ctx->session_ctx.pfunc->C_Encrypt(ctx->session_ctx.sess, (CK_BYTE_PTR)message, (CK_ULONG)message_size, (CK_BYTE_PTR)ciphertext, &ciphertext_size);

end:
    return (ret == CKR_OK) ? CB_STATUS_SUCCESS : CB_STATUS_FAIL;
}

/*******************************************************************************
 *                            OPAQUE WRAPPERS                                  *
*******************************************************************************/

cb_status_t wrapper_aead_opaque_init(void **ctx_internal, cb_aead_type_t aead_type, size_t key_size)
{
    return wrapper_aead_init_common(ctx_internal, aead_type, key_size);
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
    (void)key_id;

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
    aead_ctx_pkcs11_t *ctx = (aead_ctx_pkcs11_t *)ctx_internal;
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
                                                ARRAY_SIZE(ctx->key_attrs) - 2,
                                                &ctx->hsecretkey);

    return (ret == CKR_OK) ? CB_STATUS_SUCCESS : CB_STATUS_FAIL;
}

cb_status_t wrapper_aead_opaque_key_delete(void *ctx_internal, uint32_t key_id)
{
    /* PKCS11 destroys keys the same way for both transparent and opaque.
     * Can defer this to wrapper_aead_opaque_deinit(). */
    (void)ctx_internal;
    (void)key_id;

    return CB_STATUS_SUCCESS;
}
