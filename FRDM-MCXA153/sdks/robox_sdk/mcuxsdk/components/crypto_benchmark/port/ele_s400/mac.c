/*
 * Copyright 2024-2025 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "cb_osal.h"
#include "cb_mac.h"
#include "ele_crypto.h"
#include "ele.h"

typedef struct _mac_ctx_ele_t
{
    mac_algo_t algorithm;
    uint16_t key_size;
    uint16_t key_size_bytes;
    uint16_t mac_size;   /*!< Only needed in opaque implementation */
    key_type_t key_type; /*!< Only needed in opaque implementation */
} mac_ctx_ele_t;

cb_status_t wrapper_mac_init(void **ctx_internal, cb_mac_algorithm_t algorithm, cb_hash_t hash_type, const uint8_t *key, size_t key_size)
{
    cb_status_t status         = CB_STATUS_FAIL;
    mac_ctx_ele_t *ele_context = NULL;
    uint32_t algorithm_tmp     = 0u;

    /* Not using key in the init function */
    (void)key;

    /* Allocate the internal ELE context */
    ele_context = (mac_ctx_ele_t *)malloc(sizeof(mac_ctx_ele_t));
    if (NULL == ele_context)
    {
        return CB_STATUS_FAIL;
    }
    status = CB_STATUS_SUCCESS;

    ele_context->key_size       = (uint16_t)key_size;
    ele_context->key_size_bytes = (uint16_t)(key_size / 8u);

    /* Since ELE works with PSA values, we can do this */
    algorithm_tmp = (uint32_t)algorithm;
    if (CB_MAC_ALGORITHM_HMAC == algorithm)
    {
        algorithm_tmp |= hash_type;
    }
    ele_context->algorithm = (mac_algo_t)algorithm_tmp;

    /* Save the context */
    *ctx_internal = (void *)ele_context;
    return status;
}

cb_status_t wrapper_mac_deinit(void *ctx_internal)
{
    if (NULL != ctx_internal)
    {
        memset(ctx_internal, 0, sizeof(mac_ctx_ele_t));
        free(ctx_internal);
    }

    return CB_STATUS_SUCCESS;
}

cb_status_t wrapper_mac_compute(
    void *ctx_internal, const uint8_t *message, size_t message_size, uint8_t *mac, const uint8_t *key)
{
    status_t status    = kStatus_Fail;
    mac_ctx_ele_t *ctx = (mac_ctx_ele_t *)ctx_internal;
    uint32_t mac_size  = 0u;

    status = ELE_GenericHmac(S3MU, message, message_size, mac, &mac_size, key, ctx->key_size_bytes, ctx->algorithm);

    return (status == kStatus_Success) ? CB_STATUS_SUCCESS : CB_STATUS_FAIL;
}

/*******************************************************************************
 * Opaque
 ******************************************************************************/
cb_status_t wrapper_mac_opaque_init(void **ctx_internal,
                                    cb_mac_algorithm_t algorithm,
                                    cb_hash_t hash_type,
                                    size_t key_size)
{
    cb_status_t status = CB_STATUS_FAIL;
    mac_ctx_ele_t *ctx = NULL;

    /* We can reuse the generic init - key can be NULL; it's not used in init */
    status = wrapper_mac_init(ctx_internal, algorithm, hash_type, NULL, key_size);
    if (CB_STATUS_SUCCESS != status)
    {
        return status;
    }
    ctx = (mac_ctx_ele_t *)(*ctx_internal);

    /* Now we need the MAC size and key type */
    if (CB_MAC_ALGORITHM_CMAC == algorithm)
    {
        /* ELE CMAC supports AES only */
        ctx->mac_size = 16u;
        ctx->key_type = kKeyType_AES;
    }
    else
    {
        /* HMAC size depends on hash type */
        switch (hash_type)
        {
            case CB_HASH_TYPE_SHA256:
                ctx->mac_size = 32u;
                break;
            case CB_HASH_TYPE_SHA384:
                ctx->mac_size = 48u;
                break;
            default:
                ctx->mac_size = 0u;
                status        = CB_STATUS_FAIL;
                break;
        }
        ctx->key_type = kKeyType_HMAC;
    }

    if (CB_STATUS_SUCCESS != status)
    {
        wrapper_mac_opaque_deinit(*ctx_internal);
    }

    return status;
}

cb_status_t wrapper_mac_opaque_deinit(void *ctx_internal)
{
    return wrapper_mac_deinit(ctx_internal);
}

cb_status_t wrapper_mac_opaque_compute(
    void *ctx_internal, const uint8_t *message, size_t message_size, uint8_t *mac, uint32_t key_id)
{
    cb_status_t status = kStatus_Fail;
    uint16_t mac_size  = 0u;
    mac_ctx_ele_t *ctx = (mac_ctx_ele_t *)ctx_internal;
    ele_mac_t conf     = {0};
    conf.mac_handle_id = g_ele_ctx.mac_handle;
    conf.key_id        = key_id;
    conf.payload       = (uint32_t)message;
    conf.payload_size  = message_size;
    conf.mac           = (uint32_t)mac;
    conf.mac_size      = ctx->mac_size;
    conf.alg           = (key_permitted_alg_t)ctx->algorithm;
    conf.mode          = kMAC_Generate;

    status = ELE_Mac(S3MU, &conf, NULL, &mac_size);

    return (status == kStatus_Success) ? CB_STATUS_SUCCESS : CB_STATUS_FAIL;
}

cb_status_t wrapper_mac_opaque_setup(void *ctx_internal)
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

    /* Open a MAC service */
    status = ELE_OpenMacService(S3MU, g_ele_ctx.keystore_handle, &g_ele_ctx.mac_handle);
    if (status != kStatus_Success)
    {
        ELE_CloseKeyService(S3MU, g_ele_ctx.key_management_handle);
        ELE_CloseKeystore(S3MU, g_ele_ctx.keystore_handle);
        return CB_STATUS_FAIL;
    }

    return CB_STATUS_SUCCESS;
}

cb_status_t wrapper_mac_opaque_cleanup(void *ctx_internal)
{
    status_t status = kStatus_Fail;
    (void)ctx_internal;

    status = ELE_CloseMacService(S3MU, g_ele_ctx.mac_handle);
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

cb_status_t wrapper_mac_opaque_key_generate(void *ctx_internal, size_t key_size, uint32_t *key_id)
{
    status_t status       = kStatus_Fail;
    uint16_t key_size_out = 0u;
    ele_gen_key_t cfg     = {0};
    mac_ctx_ele_t *ctx    = (mac_ctx_ele_t *)ctx_internal;

    /* Generate a key */
    cfg.key_type      = ctx->key_type;
    cfg.key_lifetime  = kKey_Persistent;
    cfg.key_usage     = kKeyUsage_SignMessage | kKeyUsage_VerifyMessage;
    cfg.key_size      = ctx->key_size;
    cfg.permitted_alg = ctx->algorithm;
    cfg.pub_key_addr  = 0u;
    cfg.pub_key_size  = 0u;
    cfg.key_group     = 1u;

    status = ELE_GenerateKey(S3MU, g_ele_ctx.key_management_handle, &cfg, key_id, &key_size_out, false, false);

    return (status == kStatus_Success) ? CB_STATUS_SUCCESS : CB_STATUS_FAIL;
}

cb_status_t wrapper_mac_opaque_key_delete(void *ctx_internal, uint32_t key_id)
{
    status_t status = kStatus_Fail;
    (void)ctx_internal;
    status = ELE_DeleteKey(S3MU, g_ele_ctx.key_management_handle, key_id, false, false);

    return (status == kStatus_Success) ? CB_STATUS_SUCCESS : CB_STATUS_FAIL;
}

cb_status_t wrapper_fast_mac_start(const uint8_t *key)
{
    status_t status = kStatus_Fail;

    status = ELE_FastMacStart(S3MU, key);

    return (status == kStatus_Success) ? CB_STATUS_SUCCESS : CB_STATUS_FAIL;
}

cb_status_t wrapper_fast_mac_proceed(const uint8_t *message, size_t message_size, uint8_t *mac)
{
    status_t status = kStatus_Fail;

    status = ELE_FastMacProceed(S3MU, message, mac, message_size, FAST_MAC_ONE_SHOT | FAST_MAC_USE_KEY_0, NULL);

    return (status == kStatus_Success) ? CB_STATUS_SUCCESS : CB_STATUS_FAIL;
}

cb_status_t wrapper_fast_mac_end(void)
{
    status_t status = kStatus_Fail;

    status = ELE_FastMacEnd(S3MU);

    return (status == kStatus_Success) ? CB_STATUS_SUCCESS : CB_STATUS_FAIL;
}
