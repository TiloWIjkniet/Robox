/*
 * Copyright 2024 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "cb_osal.h"
#include "cb_aes.h"
#include "ele_crypto.h"
#include "ele.h"

typedef struct _aes_ctx_ele_t
{
    generic_cipher_algo_t algo;
    uint16_t key_size;
    uint16_t key_size_bytes;
    key_permitted_alg_t permitted_alg; /*!< Used in opaque only */
} aes_ctx_ele_t;

cb_status_t wrapper_aes_init(void **ctx_internal, cb_cipher_type_t cipher_type, const uint8_t *key, size_t key_size)
{
    cb_status_t status         = CB_STATUS_FAIL;
    aes_ctx_ele_t *ele_context = NULL;

    (void)key; /* Unused in this case */

    /* Allocate the internal ELE context */
    ele_context = (aes_ctx_ele_t *)malloc(sizeof(aes_ctx_ele_t));
    if (NULL == ele_context)
    {
        return CB_STATUS_FAIL;
    }
    status = CB_STATUS_SUCCESS;

    ele_context->key_size       = key_size;
    ele_context->key_size_bytes = key_size / 8u;

    /* Port the PSA cipher type value to ELE value */
    switch (cipher_type)
    {
        case CB_CIPHER_TYPE_ECB_NO_PADDING:
            ele_context->algo = kAES_ECB;
            break;
        case CB_CIPHER_TYPE_CBC_NO_PADDING:
            ele_context->algo = kAES_CBC;
            break;
        case CB_CIPHER_TYPE_CTR:
            ele_context->algo = kAES_CTR;
            break;
        case CB_CIPHER_TYPE_OFB:
            ele_context->algo = kAES_OFB;
            break;
        case CB_CIPHER_TYPE_CFB:
            ele_context->algo = kAES_CFB;
            break;
        default:
            status = CB_STATUS_FAIL;
            break;
    }

    if (CB_STATUS_SUCCESS != status)
    {
        free(ele_context);
        return status;
    }

    /* Save the context */
    *ctx_internal = (void *)ele_context;
    return status;
}

cb_status_t wrapper_aes_deinit(void *ctx_internal)
{
    if (NULL != ctx_internal)
    {
        memset(ctx_internal, 0, sizeof(aes_ctx_ele_t));
        free(ctx_internal);
        ctx_internal = NULL;
    }

    return CB_STATUS_SUCCESS;
}

cb_status_t wrapper_aes_compute(void *ctx_internal,
                                const uint8_t *message,
                                size_t message_size,
                                const uint8_t *iv,
                                const uint8_t *key,
                                uint8_t *ciphertext)
{
    status_t status           = kStatus_Fail;
    aes_ctx_ele_t *ctx        = (aes_ctx_ele_t *)ctx_internal;
    ele_generic_cipher_t conf = {0};
    conf.algo                 = ctx->algo;
    conf.data                 = (uint32_t)message;
    conf.size                 = (uint32_t)message_size;
    conf.iv                   = kAES_ECB == ctx->algo ? 0u : (uint32_t)iv; // Workaround for ELE accepting IV with ECB
    conf.iv_size              = kAES_ECB == ctx->algo ? 0u : 16u;
    conf.key                  = (uint32_t)key;
    conf.key_size             = ctx->key_size_bytes;
    conf.mode                 = kEncrypt;
    conf.output               = (uint32_t)ciphertext;

    status = ELE_GenericCipher(S3MU, &conf);

    return (status == kStatus_Success) ? CB_STATUS_SUCCESS : CB_STATUS_FAIL;
   
}

cb_status_t wrapper_aes_opaque_init(void **ctx_internal, cb_cipher_type_t cipher_type, size_t key_size)
{
    cb_status_t status         = CB_STATUS_FAIL;
    aes_ctx_ele_t *ele_context = NULL;

    status = wrapper_aes_init(ctx_internal, cipher_type, NULL, key_size);
    if (CB_STATUS_SUCCESS != status)
    {
        wrapper_aes_deinit(*ctx_internal);
        return status;
    }
    ele_context = (aes_ctx_ele_t *)(*ctx_internal);

    switch (cipher_type)
    {
        case CB_CIPHER_TYPE_ECB_NO_PADDING:
            ele_context->permitted_alg = kPermitted_ECB;
            break;
        case CB_CIPHER_TYPE_CBC_NO_PADDING:
            ele_context->permitted_alg = kPermitted_CBC;
            break;
        case CB_CIPHER_TYPE_CTR:
            ele_context->permitted_alg = kPermitted_CTR;
            break;
        case CB_CIPHER_TYPE_OFB:
            ele_context->permitted_alg = kPermitted_OFB;
            break;
        case CB_CIPHER_TYPE_CFB:
            ele_context->permitted_alg = kPermitted_CFB;
            break;
        default:
            status = CB_STATUS_FAIL;
            break;
    }

    if (CB_STATUS_SUCCESS != status)
    {
        wrapper_aes_opaque_deinit(*ctx_internal);
    }

    return status;
}

cb_status_t wrapper_aes_opaque_deinit(void *ctx_internal)
{
    if (NULL != ctx_internal)
    {
        memset(ctx_internal, 0, sizeof(aes_ctx_ele_t));
        free(ctx_internal);
    }

    return CB_STATUS_SUCCESS;
}

cb_status_t wrapper_aes_opaque_compute(void *ctx_internal,
                                       const uint8_t *message,
                                       size_t message_size,
                                       const uint8_t *iv,
                                       uint8_t *ciphertext,
                                       uint32_t key_id)
{
    status_t status       = kStatus_Fail;
    ele_hsm_cipher_t conf = {0};
    aes_ctx_ele_t *ctx    = (aes_ctx_ele_t *)ctx_internal;

    conf.keyID       = key_id;
    conf.input       = (uint32_t)message;
    conf.input_size  = message_size;
    conf.output      = (uint32_t)ciphertext;
    conf.output_size = NULL;
    conf.iv          = kAES_ECB == ctx->algo ? 0u : (uint32_t)iv; // Workaround for ELE accepting IV with ECB
    conf.iv_size     = kAES_ECB == ctx->algo ? 0u : 16u;
    conf.alg         = ctx->permitted_alg;
    conf.mode        = kHSM_Encrypt;

    status = ELE_Cipher(S3MU, g_ele_ctx.cipher_handle, &conf);

    return (status == kStatus_Success) ? CB_STATUS_SUCCESS : CB_STATUS_FAIL;
}

cb_status_t wrapper_aes_opaque_setup(void *ctx_internal)
{
    status_t status = kStatus_Fail;
    (void)ctx_internal;

    /* Create a keystore */
    ele_keystore_t keystore_param;
    keystore_param.id            = KEYSTORE_ID;
    keystore_param.nonce         = KEYSTORE_NONCE;
    keystore_param.max_updates   = 0xff;
    keystore_param.min_mac_check = false;
    keystore_param.min_mac_len   = 0u;

    status = ELE_CreateKeystore(S3MU, g_ele_ctx.session_handle, &keystore_param, &g_ele_ctx.keystore_handle);
    if (status != kStatus_Success)
    {
        return CB_STATUS_FAIL;
    }

    /* Set up key management */
    status = ELE_OpenKeyService(S3MU, g_ele_ctx.keystore_handle, &g_ele_ctx.key_management_handle);
    if (status != kStatus_Success)
    {
        ELE_CloseKeystore(S3MU, g_ele_ctx.keystore_handle);
        return CB_STATUS_FAIL;
    }

    /* Open a cipher service */
    status = ELE_OpenCipherService(S3MU, g_ele_ctx.keystore_handle, &g_ele_ctx.cipher_handle);
    if (status != kStatus_Success)
    {
        ELE_CloseKeyService(S3MU, g_ele_ctx.key_management_handle);
        ELE_CloseKeystore(S3MU, g_ele_ctx.keystore_handle);
        return CB_STATUS_FAIL;
    }

    return CB_STATUS_SUCCESS;
}

cb_status_t wrapper_aes_opaque_cleanup(void *ctx_internal)
{
    status_t status = kStatus_Fail;
    (void)ctx_internal;

    status = ELE_CloseCipherService(S3MU, g_ele_ctx.cipher_handle);
    if (status != kStatus_Success)
    {
        return CB_STATUS_FAIL;
    }

    status = ELE_CloseKeyService(S3MU, g_ele_ctx.key_management_handle);
    if (status != kStatus_Success)
    {
        return CB_STATUS_FAIL;
    }

    status = ELE_CloseKeystore(S3MU, g_ele_ctx.keystore_handle);
    if (status != kStatus_Success)
    {
        return CB_STATUS_FAIL;
    }

    return CB_STATUS_SUCCESS;
}

cb_status_t wrapper_aes_opaque_key_generate(void *ctx_internal, size_t key_size, uint32_t *key_id)
{
    status_t status       = kStatus_Fail;
    uint16_t key_size_out = 0u;
    ele_gen_key_t cfg     = {0};
    (void)ctx_internal;

    /* Generate a key */
    cfg.key_type      = kKeyType_AES;
    cfg.key_lifetime  = kKey_Persistent;
    cfg.key_usage     = kKeyUsage_Encrypt | kKeyUsage_Decrypt;
    cfg.key_size      = key_size;
    cfg.permitted_alg = kPermitted_All_Cipher;
    cfg.pub_key_addr  = 0u;
    cfg.pub_key_size  = 0u;
    cfg.key_group     = 1u;

    status = ELE_GenerateKey(S3MU, g_ele_ctx.key_management_handle, &cfg, key_id, &key_size_out, false, false);

    return (status == kStatus_Success) ? CB_STATUS_SUCCESS : CB_STATUS_FAIL;
}

cb_status_t wrapper_aes_opaque_key_delete(void *ctx_internal, uint32_t key_id)
{
    status_t status = kStatus_Fail;

    (void)ctx_internal;
    status = ELE_DeleteKey(S3MU, g_ele_ctx.key_management_handle, key_id, false, false);

    return (status == kStatus_Success) ? CB_STATUS_SUCCESS : CB_STATUS_FAIL;
}
