/*
 * Copyright 2024 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "cb_osal.h"
#include "cb_aead.h"
#include "ele_init.h"

typedef struct _aead_ctx_ele_t
{
    sss_algorithm_t algo;
    sss_sscp_object_t sssKey;
    sss_sscp_aead_t ctx;
    uint16_t key_size;
    uint16_t key_size_bytes;
    //key_permitted_alg_t opaque_algo; /*!< Used in opaque only */
} aead_ctx_ele_t;

/**
 *  Set AEAD key which is placed in keybuffer into keyslot inside ELE
 */
static cb_status_t aes_aead_setkey(sss_sscp_object_t *sssKey, const uint8_t *key_buffer, size_t key_bits)
{
    size_t key_bytes = key_bits >> 3u;
    
    if ((sss_sscp_key_object_init(sssKey, &g_ele_ctx.keyStore)) != kStatus_SSS_Success)
    {
        return CB_STATUS_FAIL;
    }

    if ((sss_sscp_key_object_allocate_handle(sssKey, 1u, /* key id */
                                             kSSS_KeyPart_Default, kSSS_CipherType_AES, key_bytes,
                                             kSSS_KeyProp_CryptoAlgo_AEAD)) != kStatus_SSS_Success)
    {
        (void)sss_sscp_key_object_free(sssKey, kSSS_keyObjFree_KeysStoreDefragment);
        return CB_STATUS_FAIL;
    }

    if ((sss_sscp_key_store_set_key(&g_ele_ctx.keyStore, sssKey, key_buffer, key_bytes, key_bits,
                                    kSSS_KeyPart_Default)) != kStatus_SSS_Success)
    {
        (void)sss_sscp_key_object_free(sssKey, kSSS_keyObjFree_KeysStoreDefragment);
        return CB_STATUS_FAIL;
    }

    return CB_STATUS_SUCCESS;
}

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
            ele_context->algo = kAlgorithm_SSS_AES_CCM;
            break;
        case CB_AEAD_TYPE_GCM:
            ele_context->algo = kAlgorithm_SSS_AES_GCM;
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
    
    if ((aes_aead_setkey(&ele_context->sssKey, key, key_size)) != CB_STATUS_SUCCESS)
    {
        free(ele_context);
        return CB_STATUS_FAIL;
    }

    if ((sss_sscp_aead_context_init(&ele_context->ctx, &g_ele_ctx.sssSession, &ele_context->sssKey, ele_context->algo, kMode_SSS_Encrypt)) !=
        kStatus_SSS_Success)
    {
        (void)sss_sscp_key_object_free(&ele_context->sssKey, kSSS_keyObjFree_KeysStoreDefragment);
        free(ele_context);
        return CB_STATUS_FAIL;
    }

    /* Save the context */
    *ctx_internal = (void *)ele_context;
    return status;
}

cb_status_t wrapper_aead_deinit(void *ctx_internal)
{
    if (NULL != ctx_internal)
    {
        aead_ctx_ele_t *ctx     = (aead_ctx_ele_t *)ctx_internal;
        
        (void)sss_sscp_aead_context_free(&ctx->ctx);
        (void)sss_sscp_key_object_free(&ctx->sssKey, kSSS_keyObjFree_KeysStoreDefragment);
        memset(ctx_internal, 0, sizeof(aead_ctx_ele_t));
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
    aead_ctx_ele_t *ctx = (aead_ctx_ele_t *)ctx_internal;
    size_t tlen         = tag_size;

    /* RUN AEAD */
    if ((sss_sscp_aead_one_go(&ctx->ctx, message, ciphertext, message_size, (uint8_t *)iv, iv_size,
                              aad, aad_size, tag, &tlen)) !=
        kStatus_SSS_Success)
    {
        (void)sss_sscp_aead_context_free(&ctx->ctx);
        (void)sss_sscp_key_object_free(&ctx->sssKey, kSSS_keyObjFree_KeysStoreDefragment);
        return CB_STATUS_FAIL;
    };
    
    return CB_STATUS_SUCCESS;

}
