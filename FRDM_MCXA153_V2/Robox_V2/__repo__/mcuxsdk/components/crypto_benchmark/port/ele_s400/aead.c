/*
 * Copyright 2024 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "cb_osal.h"
#include "cb_aead.h"
#include "ele_crypto.h"
#include "ele.h"

typedef struct _aead_ctx_ele_t
{
    generic_aead_algo_t algo;
    uint16_t key_size;
    uint16_t key_size_bytes;
    key_permitted_alg_t opaque_algo; /*!< Used in opaque only */
} aead_ctx_ele_t;

cb_status_t wrapper_aead_init(void **ctx_internal, cb_aead_type_t aead_type, const uint8_t *key, size_t key_size)
{
    cb_status_t status          = CB_STATUS_FAIL;
    aead_ctx_ele_t *ele_context = NULL;

    /* Allocate the internal ELE context */
    ele_context = (aead_ctx_ele_t *)malloc(sizeof(aead_ctx_ele_t));
    if (NULL == ele_context)
    {
        return CB_STATUS_FAIL;
    }
    status = CB_STATUS_SUCCESS;

    ele_context->key_size       = key_size;
    ele_context->key_size_bytes = key_size / 8u;

    switch (aead_type)
    {
        case CB_AEAD_TYPE_CCM:
            ele_context->algo = kAES_CCM;
            break;
        case CB_AEAD_TYPE_GCM:
            ele_context->algo = kAES_GCM;
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

cb_status_t wrapper_aead_deinit(void *ctx_internal)
{
    if (NULL != ctx_internal)
    {
        memset(ctx_internal, 0, sizeof(aead_ctx_ele_t));
        free(ctx_internal);
        ctx_internal = NULL;
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
    status_t status = kStatus_Fail;
    aead_ctx_ele_t *ctx     = (aead_ctx_ele_t *)ctx_internal;
    ele_generic_aead_t conf = {0};

    conf.data     = (uint32_t)message;
    conf.output   = (uint32_t)ciphertext;
    conf.size     = (uint32_t)message_size;
    conf.key      = (uint32_t)key;
    conf.key_size = (uint32_t)ctx->key_size_bytes;
    conf.iv       = (uint32_t)iv;
    conf.iv_size  = (uint32_t)iv_size;
    conf.aad      = (uint32_t)aad;
    conf.aad_size = (uint32_t)aad_size;
    conf.tag      = (uint32_t)tag;
    conf.tag_size = (uint32_t)tag_size;
    conf.mode     = kEncrypt;
    conf.algo     = ctx->algo;

    status = ELE_GenericAead(S3MU, &conf);

    return (status == kStatus_Success) ? CB_STATUS_SUCCESS : CB_STATUS_FAIL;
}

cb_status_t wrapper_aead_opaque_init(void **ctx_internal, cb_aead_type_t aead_type, size_t key_size)
{
    cb_status_t status          = CB_STATUS_FAIL;
    aead_ctx_ele_t *ele_context = NULL;

    /* Reuse generic init */
    status = wrapper_aead_init(ctx_internal, aead_type, NULL, key_size);
    if (CB_STATUS_SUCCESS != status)
    {
        wrapper_aead_opaque_deinit(*ctx_internal);
        return status;
    }

    ele_context = (aead_ctx_ele_t *)(*ctx_internal);

    switch (aead_type)
    {
        case CB_AEAD_TYPE_CCM:
            ele_context->opaque_algo = kPermitted_CCM;
            break;
        case CB_AEAD_TYPE_GCM:
            ele_context->opaque_algo = kPermitted_GCM;
            break;
        default:
            status = CB_STATUS_FAIL;
            break;
    }

    if (CB_STATUS_SUCCESS != status)
    {
        wrapper_aead_opaque_deinit(*ctx_internal);
    }

    return status;
}

cb_status_t wrapper_aead_opaque_deinit(void *ctx_internal)
{
    if (NULL != ctx_internal)
    {
        memset(ctx_internal, 0, sizeof(aead_ctx_ele_t));
        free(ctx_internal);
    }

    return CB_STATUS_SUCCESS;
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
    status_t status = kStatus_Fail;
    ele_hsm_cipher_t conf = {0};
    aead_ctx_ele_t *ctx   = (aead_ctx_ele_t *)ctx_internal;

    // ELE outputs the tag into the ciphertext buffer, so not used here
    (void)tag;
    (void)tag_size;

    conf.keyID       = key_id;
    conf.input       = (uint32_t)message;
    conf.input_size  = message_size;
    conf.output      = (uint32_t)ciphertext;
    conf.output_size = NULL;
    conf.iv          = (uint32_t)iv;
    conf.iv_size     = iv_size;
    conf.alg         = ctx->opaque_algo;
    conf.mode        = kHSM_Encrypt;

    status = ELE_Aead(S3MU, g_ele_ctx.cipher_handle, &conf, (uint32_t)aad, aad_size, kHSM_IV_User);

    return (status == kStatus_Success) ? CB_STATUS_SUCCESS : CB_STATUS_FAIL;
}

cb_status_t wrapper_aead_opaque_setup(void *ctx_internal)
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

cb_status_t wrapper_aead_opaque_cleanup(void *ctx_internal)
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

cb_status_t wrapper_aead_opaque_key_generate(void *ctx_internal, size_t key_size, uint32_t *key_id)
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
    cfg.permitted_alg = kPermitted_All_AEAD;
    cfg.pub_key_addr  = 0u;
    cfg.pub_key_size  = 0u;
    cfg.key_group     = 1u;

    status = ELE_GenerateKey(S3MU, g_ele_ctx.key_management_handle, &cfg, key_id, &key_size_out, false, false);

    return (status == kStatus_Success) ? CB_STATUS_SUCCESS : CB_STATUS_FAIL;
}

cb_status_t wrapper_aead_opaque_key_delete(void *ctx_internal, uint32_t key_id)
{
    status_t status = kStatus_Fail;

    (void)ctx_internal;
    status = ELE_DeleteKey(S3MU, g_ele_ctx.key_management_handle, key_id, false, false);

    return (status == kStatus_Success) ? CB_STATUS_SUCCESS : CB_STATUS_FAIL;
}
