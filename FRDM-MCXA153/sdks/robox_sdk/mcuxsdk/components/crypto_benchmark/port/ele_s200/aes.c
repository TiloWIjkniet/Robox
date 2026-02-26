/*
 * Copyright 2024 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "cb_osal.h"
#include "cb_aes.h"
#include "ele.h"
#include "ele_init.h"

typedef struct _aes_ctx_ele_t
{
    //generic_cipher_algo_t algo;
    sss_algorithm_t algo;
    sss_sscp_object_t sssKey;
    sss_sscp_symmetric_t ctx;
    uint16_t key_size;
    uint16_t key_size_bytes;
    //key_permitted_alg_t permitted_alg; /*!< Used in opaque only */
} aes_ctx_ele_t;

cb_status_t wrapper_aes_init(void **ctx_internal, cb_cipher_type_t cipher_type, const uint8_t *key, size_t key_size)
{
    cb_status_t status         = CB_STATUS_FAIL;
    aes_ctx_ele_t *ele_context = NULL;

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
            ele_context->algo = kAlgorithm_SSS_AES_ECB;
            break;
        case CB_CIPHER_TYPE_CBC_NO_PADDING:
            ele_context->algo = kAlgorithm_SSS_AES_CBC;
            break;
        case CB_CIPHER_TYPE_CTR:
            ele_context->algo = kAlgorithm_SSS_AES_CTR;
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
    
    if ((sss_sscp_key_object_init(&ele_context->sssKey, &g_ele_ctx.keyStore)) != kStatus_SSS_Success)
    {
        free(ele_context);
        return CB_STATUS_FAIL;
    }

    if ((sss_sscp_key_object_allocate_handle(&ele_context->sssKey, 1u, /* key id */
                                             kSSS_KeyPart_Default, kSSS_CipherType_AES,  ele_context->key_size_bytes,
                                             kSSS_KeyProp_CryptoAlgo_AES)) != kStatus_SSS_Success)
    {
        (void)sss_sscp_key_object_free(&ele_context->sssKey, kSSS_keyObjFree_KeysStoreDefragment);
        free(ele_context);
        return CB_STATUS_FAIL;
    }

    if ((sss_sscp_key_store_set_key(&g_ele_ctx.keyStore, &ele_context->sssKey, key,  ele_context->key_size_bytes,
                                    key_size, kSSS_KeyPart_Default)) != kStatus_SSS_Success)
    {
        (void)sss_sscp_key_object_free(&ele_context->sssKey, kSSS_keyObjFree_KeysStoreDefragment);
        free(ele_context);
        return CB_STATUS_FAIL;
    }

    if ((sss_sscp_symmetric_context_init(&ele_context->ctx, &g_ele_ctx.sssSession, &ele_context->sssKey,  ele_context->algo, kMode_SSS_Encrypt)) !=
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

cb_status_t wrapper_aes_deinit(void *ctx_internal)
{
    if (NULL != ctx_internal)
    {
        aes_ctx_ele_t *ctx        = (aes_ctx_ele_t *)ctx_internal;
        
        (void)sss_sscp_symmetric_context_free(&ctx->ctx);
        (void)sss_sscp_key_object_free(&ctx->sssKey, kSSS_keyObjFree_KeysStoreDefragment);
        memset(ctx_internal, 0, sizeof(aes_ctx_ele_t));
        free(ctx_internal);
    }

    return CB_STATUS_SUCCESS;
}

cb_status_t wrapper_aes_compute(void *ctx_internal, const uint8_t *message, size_t message_size, const uint8_t *iv, const uint8_t *key, uint8_t *ciphertext)
{
    aes_ctx_ele_t *ctx        = (aes_ctx_ele_t *)ctx_internal;
    
    //size_t iv                   = kAlgorithm_SSS_AES_ECB == ctx->algo ? 0u : (uint32_t)iv; // Workaround for ELE accepting IV with ECB
    size_t iv_size              = kAlgorithm_SSS_AES_ECB == ctx->algo ? 0u : 16u;
    /* RUN AES */
    if ((sss_sscp_cipher_one_go(&ctx->ctx, (uint8_t *)iv, iv_size, message, ciphertext, message_size)) != kStatus_SSS_Success)
    {
        (void)sss_sscp_symmetric_context_free(&ctx->ctx);
        (void)sss_sscp_key_object_free(&ctx->sssKey, kSSS_keyObjFree_KeysStoreDefragment);
        return CB_STATUS_FAIL;
    };

    return CB_STATUS_SUCCESS;
}
