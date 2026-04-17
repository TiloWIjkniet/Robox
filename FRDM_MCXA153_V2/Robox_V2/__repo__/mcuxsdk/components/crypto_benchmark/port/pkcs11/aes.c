/*
 * Copyright 2025 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "cb_osal.h"
#include "cb_aes.h"

#include "include/common_utils.h"

typedef struct _cipher_ctx_pkcs11
{
    session_ctx_pkcs11_t session_ctx;
    CK_MECHANISM key_mech;
    CK_OBJECT_CLASS secret_key_class;
    CK_BBOOL ck_true;
    CK_BBOOL ck_false;
    CK_ULONG key_length;
    CK_MECHANISM_TYPE key_allowed_mech[1];
    CK_ATTRIBUTE key_attrs[9];
    CK_MECHANISM cipher_mech;
    CK_AES_CTR_PARAMS ctr_params;
    CK_OBJECT_HANDLE hsecretkey;
} cipher_ctx_pkcs11_t;

/* CK_AES_CTR_PARAMS */
static const CK_BYTE counter_block[] = { 0x00, 0x01, 0x02, 0x03,
                                         0x04, 0x05, 0x06, 0x07,
                                         0x00, 0x00, 0x00, 0x00,
                                         0x00, 0x00, 0x00, 0x00 };

CK_BYTE iv[] = { 0x01, 0x02,  0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                 0x09, 0x010, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F };

static cb_status_t set_key_params(cipher_ctx_pkcs11_t *ctx, cb_cipher_type_t cipher_type, size_t key_size)
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
    ctx->ck_false = CK_FALSE;
    ctx->key_length = (CK_ULONG)key_size >> 3;

    switch (cipher_type)
    {
        case CB_CIPHER_TYPE_CTR:
            ctx->key_allowed_mech[0] = CKM_AES_CTR;
            break;
        case CB_CIPHER_TYPE_XTS:
            ctx->key_allowed_mech[0] = CKM_AES_XTS;
            break;
        case CB_CIPHER_TYPE_CBC_NO_PADDING:
            ctx->key_allowed_mech[0] = CKM_AES_CBC;
            break;
        case CB_CIPHER_TYPE_ECB_NO_PADDING:
            ctx->key_allowed_mech[0] = CKM_AES_ECB;
            break;
        case CB_CIPHER_TYPE_STREAM:
        case CB_CIPHER_TYPE_CFB:
        case CB_CIPHER_TYPE_OFB:
        case CB_CIPHER_TYPE_CBC_PKCS7_PADDING:
        default:
            status = CB_STATUS_FAIL;
            break;
    }

    SET_ATTRIBUTE(ctx->key_attrs[0], CKA_CLASS, ctx->secret_key_class);
    SET_ATTRIBUTE(ctx->key_attrs[1], CKA_ENCRYPT, ctx->ck_true);
    SET_ATTRIBUTE(ctx->key_attrs[2], CKA_DECRYPT, ctx->ck_true);
    SET_ATTRIBUTE(ctx->key_attrs[3], CKA_ALLOWED_MECHANISMS, ctx->key_allowed_mech);
    SET_ATTRIBUTE(ctx->key_attrs[4], CKA_TOKEN, ctx->ck_false);
    SET_ATTRIBUTE(ctx->key_attrs[5], CKA_SENSITIVE, ctx->ck_false);
    if (CB_CIPHER_TYPE_XTS == cipher_type)
    {
        SET_ATTRIBUTE(ctx->key_attrs[6], CKA_EXTRACTABLE, ctx->ck_true);
    }
    else
    {
        SET_ATTRIBUTE(ctx->key_attrs[6], CKA_EXTRACTABLE, ctx->ck_false);
    }
    SET_ATTRIBUTE(ctx->key_attrs[7], CKA_VALUE_LEN, ctx->key_length);

    return status;
}

static cb_status_t set_cipher_mech(cipher_ctx_pkcs11_t *ctx, cb_cipher_type_t cipher_type)
{
    cb_status_t status = CB_STATUS_SUCCESS;

    if (NULL == ctx)
    {
        return CB_STATUS_FAIL;
    }

    switch (cipher_type)
    {
        case CB_CIPHER_TYPE_CTR:
            ctx->cipher_mech.mechanism = CKM_AES_CTR;
            memcpy(ctx->ctr_params.cb, counter_block,
                   sizeof(counter_block));
            ctx->ctr_params.ulCounterBits = 64;
            ctx->cipher_mech.pParameter = &ctx->ctr_params;
            ctx->cipher_mech.ulParameterLen = sizeof(ctx->ctr_params);
            break;
        case CB_CIPHER_TYPE_XTS:
            ctx->cipher_mech.mechanism = CKM_AES_XTS;
            ctx->cipher_mech.pParameter = iv;
            ctx->cipher_mech.ulParameterLen = sizeof(iv);
            break;
        case CB_CIPHER_TYPE_CBC_NO_PADDING:
            ctx->cipher_mech.mechanism = CKM_AES_CBC;
            ctx->cipher_mech.pParameter = iv;
            ctx->cipher_mech.ulParameterLen = sizeof(iv);
            break;
        case CB_CIPHER_TYPE_ECB_NO_PADDING:
            ctx->cipher_mech.mechanism = CKM_AES_ECB;
            ctx->cipher_mech.pParameter = NULL_PTR;
            ctx->cipher_mech.ulParameterLen = 0;
            break;
        case CB_CIPHER_TYPE_STREAM:
        case CB_CIPHER_TYPE_CFB:
        case CB_CIPHER_TYPE_OFB:
        case CB_CIPHER_TYPE_CBC_PKCS7_PADDING:
        default:
            status = CB_STATUS_FAIL;
            break;
    }

    return status;
}

static cb_status_t wrapper_aes_init_common(void **ctx_internal, cb_cipher_type_t cipher_type, size_t key_size)
{
    cb_status_t status = CB_STATUS_FAIL;

    cipher_ctx_pkcs11_t *ctx = NULL;
    session_ctx_pkcs11_t session_ctx = { 0 };

    if (NULL == ctx_internal)
    {
        return CB_STATUS_FAIL;
    }

    *ctx_internal = NULL;

    /* Allocate the internal PKCS11 context */
    ctx = (cipher_ctx_pkcs11_t *)malloc(sizeof(cipher_ctx_pkcs11_t));
    if (NULL == ctx)
    {
        return CB_STATUS_FAIL;
    }

    status = set_key_params(ctx, cipher_type, key_size);
    if (CB_STATUS_SUCCESS != status)
    {
        goto end;
    }

    status = set_cipher_mech(ctx, cipher_type);
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

    /* Save the ctx */
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

cb_status_t wrapper_aes_init(void **ctx_internal, cb_cipher_type_t cipher_type, const uint8_t *key, size_t key_size)
{
    cb_status_t status = CB_STATUS_FAIL;
    cipher_ctx_pkcs11_t *ctx = NULL;
    CK_KEY_TYPE keyType = CKK_AES;
    CK_RV ret = CKR_OK;

    status = wrapper_aes_init_common(ctx_internal, cipher_type, key_size);
    if (CB_STATUS_SUCCESS != status)
    {
        return status;
    }

    ctx = (cipher_ctx_pkcs11_t *)*ctx_internal;
    if (NULL == ctx)
    {
        return CB_STATUS_FAIL;
    }

    SET_ATTRIBUTE(ctx->key_attrs[7], CKA_KEY_TYPE, keyType);

    ctx->key_attrs[8].type = CKA_VALUE;
    ctx->key_attrs[8].pValue = &key;
    ctx->key_attrs[8].ulValueLen = (CK_ULONG)key_size >> 3;

    if (CB_CIPHER_TYPE_XTS != cipher_type)
    {
        SET_ATTRIBUTE(ctx->key_attrs[4], CKA_TOKEN, ctx->ck_true);
    }

    /* Import a key */
    ret = ctx->session_ctx.pfunc->C_CreateObject(ctx->session_ctx.sess,
                                                 ctx->key_attrs,
                                                 ARRAY_SIZE(ctx->key_attrs),
                                                 &ctx->hsecretkey);

    if (CKR_OK != ret)
    {
        (void)wrapper_aes_deinit(ctx);
    }

    return (ret == CKR_OK) ? CB_STATUS_SUCCESS : CB_STATUS_FAIL;
}

cb_status_t wrapper_aes_deinit(void *ctx_internal)
{
    cipher_ctx_pkcs11_t *ctx = (cipher_ctx_pkcs11_t *)ctx_internal;

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

cb_status_t wrapper_aes_compute(void *ctx_internal, const uint8_t *message, size_t message_size, const uint8_t *iv, const uint8_t *key, uint8_t *ciphertext)
{
    cipher_ctx_pkcs11_t *ctx = (cipher_ctx_pkcs11_t *)ctx_internal;
    CK_ULONG ciphertext_size = 0;
    CK_RV ret = CKR_OK;

    (void)key;
    (void)iv;

    if (NULL == ctx)
    {
        return CB_STATUS_FAIL;
    }

    ret = ctx->session_ctx.pfunc->C_EncryptInit(ctx->session_ctx.sess, &ctx->cipher_mech, ctx->hsecretkey);
    if (CKR_OK != ret)
    {
        goto end;
    }

    ret = ctx->session_ctx.pfunc->C_Encrypt(ctx->session_ctx.sess, (CK_BYTE_PTR)message, (CK_ULONG)message_size, NULL_PTR, &ciphertext_size);
    if (CKR_OK != ret)
    {
        goto end;
    }

    ret = ctx->session_ctx.pfunc->C_Encrypt(ctx->session_ctx.sess, (CK_BYTE_PTR)message, (CK_ULONG)message_size, (CK_BYTE_PTR)ciphertext, &ciphertext_size);

end:
    return (ret == CKR_OK) ? CB_STATUS_SUCCESS : CB_STATUS_FAIL;
}

/*******************************************************************************
 *                            OPAQUE WRAPPERS                                  *
*******************************************************************************/

cb_status_t wrapper_aes_opaque_init(void **ctx_internal, cb_cipher_type_t cipher_type, size_t key_size)
{
    return wrapper_aes_init_common(ctx_internal, cipher_type, key_size);
}

cb_status_t wrapper_aes_opaque_deinit(void *ctx_internal)
{
    return wrapper_aes_deinit(ctx_internal);
}

cb_status_t wrapper_aes_opaque_compute(void *ctx_internal, const uint8_t *message, size_t message_size, const uint8_t *iv, uint8_t *ciphertext, uint32_t key_id)
{
    (void)iv;
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
    cipher_ctx_pkcs11_t *ctx = (cipher_ctx_pkcs11_t *)ctx_internal;
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

cb_status_t wrapper_aes_opaque_key_delete(void *ctx_internal, uint32_t key_id)
{
    /* PKCS11 destroys keys the same way for both transparent and opaque.
     * Can defer this to wrapper_aes_opaque_deinit(). */
    (void)ctx_internal;
    (void)key_id;

    return CB_STATUS_SUCCESS;
}
