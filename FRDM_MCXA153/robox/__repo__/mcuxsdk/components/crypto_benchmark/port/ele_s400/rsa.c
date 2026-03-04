/*
 * Copyright 2024-2025 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "cb_osal.h"
#include "cb_rsa.h"
#include "ele_crypto.h"
#include "ele.h"

typedef struct _rsa_ctx_ele_t
{
    size_t key_size;               /*!< RSA key size in bits */
    size_t key_size_bytes;         /*!< RSA key size in Bytes */
    size_t public_key_buffer_size; /*!< The buffer size of the public key array */
    generic_rsa_algo_t algorithm;  /*!< Algorithm to use */
    uint16_t ciphertext_size;      /*!< Size of signature / ciphertext */
    cb_asym_sigver_input_type_t input_type; /*!< Signing a message or hash */
} rsa_ctx_ele_t;

cb_status_t wrapper_rsa_init(
    void **ctx_internal, cb_rsa_type_t rsa_type, cb_hash_t hash_type, size_t key_size, size_t public_key_buffer_size, cb_asym_sigver_input_type_t input_type)
{
    cb_status_t status         = CB_STATUS_FAIL;
    rsa_ctx_ele_t *ele_context = NULL;

    /* Allocate the internal ELE context */
    ele_context = (rsa_ctx_ele_t *)malloc(sizeof(rsa_ctx_ele_t));
    if (NULL == ele_context)
    {
        return CB_STATUS_FAIL;
    }
    status = CB_STATUS_SUCCESS;

    /* Set the context */
    ele_context->algorithm              = (generic_rsa_algo_t)((uint32_t)rsa_type | (uint32_t)hash_type);
    ele_context->key_size               = key_size;
    ele_context->key_size_bytes         = key_size / 8u;
    ele_context->ciphertext_size        = key_size / 8u;
    ele_context->public_key_buffer_size = public_key_buffer_size;
    ele_context->input_type             = input_type;

    /* Save the context */
    *ctx_internal = (void *)ele_context;
    return CB_STATUS_SUCCESS;
}

cb_status_t wrapper_rsa_deinit(void *ctx_internal)
{
    memset(ctx_internal, 0, sizeof(rsa_ctx_ele_t));
    free(ctx_internal);

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
    status_t status = kStatus_Fail;

    ele_generic_rsa_t conf  = {0};
    conf.modulus            = (uint32_t)modulus;
    conf.modulus_size       = (uint32_t)modulus_buf_size;
    conf.priv_exponent      = (uint32_t)private_exponent;
    conf.priv_exponent_size = (uint32_t)private_exponent_buf_size;
    conf.pub_exponent       = (uint32_t)public_exponent;
    conf.pub_exponent_size  = (uint32_t)public_exponent_buf_size;
    conf.key_size           = key_size;

    status = ELE_GenericRsaKeygen(S3MU, &conf);

    return (status == kStatus_Success) ? CB_STATUS_SUCCESS : CB_STATUS_FAIL;
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
    status_t status    = kStatus_Fail;
    rsa_ctx_ele_t *ctx = (rsa_ctx_ele_t *)ctx_internal;

    ele_generic_rsa_t conf  = {0};
    conf.mode               = kSignGen;
    conf.algo               = ctx->algorithm;
    conf.key_size           = ctx->key_size;
    conf.priv_exponent      = (uint32_t)private_exponent;
    conf.priv_exponent_size = (uint32_t)ctx->key_size_bytes;
    conf.modulus            = (uint32_t)modulus;
    conf.modulus_size       = (uint32_t)ctx->key_size_bytes;
    conf.digest             = (uint32_t)message;
    conf.digest_size        = message_size;
    conf.signature          = (uint32_t)signature;
    conf.signature_size     = ctx->ciphertext_size;
    conf.flags              = CB_SIGVER_MESSAGE == ctx->input_type ? kFlagActualMsg : kFlagDigest;

    status = ELE_GenericRsa(S3MU, &conf);

    return (status == kStatus_Success) ? CB_STATUS_SUCCESS : CB_STATUS_FAIL;
}

cb_status_t wrapper_rsa_verify_compute(
    void *ctx_internal, const uint8_t *modulus, const uint8_t *public_exponent, uint8_t *message, size_t message_size, uint8_t *signature)
{
    status_t status    = kStatus_Fail;
    rsa_ctx_ele_t *ctx = (rsa_ctx_ele_t *)ctx_internal;

    ele_generic_rsa_t conf = {0};
    conf.mode              = kVerification;
    conf.algo              = ctx->algorithm;
    conf.key_size          = ctx->key_size;
    conf.pub_exponent      = (uint32_t)public_exponent;
    conf.pub_exponent_size = (uint32_t)ctx->public_key_buffer_size;
    conf.modulus           = (uint32_t)modulus;
    conf.modulus_size      = (uint32_t)ctx->key_size_bytes;
    conf.digest            = (uint32_t)message;
    conf.digest_size       = message_size;
    conf.signature         = (uint32_t)signature;
    conf.signature_size    = ctx->ciphertext_size;
    conf.flags             = CB_SIGVER_MESSAGE == ctx->input_type ? kFlagActualMsg : kFlagDigest;

    status = ELE_GenericRsa(S3MU, &conf);

    return (status == kStatus_Success) ? CB_STATUS_SUCCESS : CB_STATUS_FAIL;
}

/*******************************************************************************
 * ENCRYPT / DECRYPT wrappers
 ******************************************************************************/
cb_status_t wrapper_rsa_encrypt_compute(void *ctx_internal,
                                        const uint8_t *modulus,
                                        const uint8_t *public_exponent,
                                        uint8_t *message,
                                        size_t message_size,
                                        uint8_t *ciphertext)
{
    status_t status    = kStatus_Fail;
    rsa_ctx_ele_t *ctx = (rsa_ctx_ele_t *)ctx_internal;

    ele_generic_rsa_t conf = {0};
    conf.mode              = kEncryption;
    conf.algo              = ctx->algorithm;
    conf.key_size          = ctx->key_size;
    conf.pub_exponent      = (uint32_t)public_exponent;
    conf.pub_exponent_size = (uint32_t)ctx->public_key_buffer_size;
    conf.modulus           = (uint32_t)modulus;
    conf.modulus_size      = (uint32_t)ctx->key_size_bytes;
    conf.plaintext         = (uint32_t)message;
    conf.plaintext_size    = message_size;
    conf.ciphertext        = (uint32_t)ciphertext;
    conf.ciphertext_size   = ctx->ciphertext_size;

    status = ELE_GenericRsa(S3MU, &conf);

    return (status == kStatus_Success) ? CB_STATUS_SUCCESS : CB_STATUS_FAIL;
}

cb_status_t wrapper_rsa_decrypt_compute(void *ctx_internal,
                                        const uint8_t *modulus,
                                        const uint8_t *private_exponent,
                                        uint8_t *message,
                                        size_t message_size,
                                        uint8_t *ciphertext)
{
    status_t status    = kStatus_Fail;
    rsa_ctx_ele_t *ctx = (rsa_ctx_ele_t *)ctx_internal;

    ele_generic_rsa_t conf  = {0};
    conf.mode               = kDecryption;
    conf.algo               = ctx->algorithm;
    conf.key_size           = ctx->key_size;
    conf.priv_exponent      = (uint32_t)private_exponent;
    conf.priv_exponent_size = (uint32_t)ctx->key_size_bytes;
    conf.modulus            = (uint32_t)modulus;
    conf.modulus_size       = (uint32_t)ctx->key_size_bytes;
    conf.plaintext          = (uint32_t)message;
    conf.plaintext_size     = message_size;
    conf.ciphertext         = (uint32_t)ciphertext;
    conf.ciphertext_size    = ctx->ciphertext_size;

    status = ELE_GenericRsa(S3MU, &conf);

    return (status == kStatus_Success) ? CB_STATUS_SUCCESS : CB_STATUS_FAIL;
}

/*******************************************************************************
 * OPAQUE
 ******************************************************************************/
typedef struct _rsa_opaque_ctx_ele_t
{
    uint16_t signature_size;
    key_size_t key_size;
    size_t key_size_bytes;
    key_sig_scheme_t signature_scheme;
    cb_asym_sigver_input_type_t input_type;
} rsa_opaque_ctx_ele_t;

cb_status_t wrapper_rsa_opaque_init(void **ctx_internal,
                                    cb_rsa_type_t rsa_type,
                                    cb_hash_t hash_type,
                                    size_t key_size,
                                    cb_asym_sigver_input_type_t input_type)
{
    cb_status_t status                = CB_STATUS_FAIL;
    rsa_opaque_ctx_ele_t *ele_context = NULL;

    /* Allocate the internal ELE context */
    ele_context = (rsa_opaque_ctx_ele_t *)malloc(sizeof(rsa_opaque_ctx_ele_t));
    if (NULL == ele_context)
    {
        return CB_STATUS_FAIL;
    }
    status = CB_STATUS_SUCCESS;

    /* Set the context */
    ele_context->key_size         = (key_size_t)key_size;
    ele_context->key_size_bytes   = key_size / 8u;
    ele_context->signature_size   = key_size / 8u;
    ele_context->signature_scheme = (key_sig_scheme_t)((uint32_t)rsa_type | (uint32_t)hash_type);
    ele_context->input_type       = input_type;

    /* Save the context */
    *ctx_internal = (void *)ele_context;

    return CB_STATUS_SUCCESS;
}

cb_status_t wrapper_rsa_opaque_deinit(void *ctx_internal)
{
    memset(ctx_internal, 0, sizeof(rsa_opaque_ctx_ele_t));
    free(ctx_internal);

    return kStatus_Success;
}

cb_status_t wrapper_rsa_keygen_opaque_setup(void)
{
    status_t status = kStatus_Fail;

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

    return CB_STATUS_SUCCESS;
}

cb_status_t wrapper_rsa_keygen_opaque_cleanup(void)
{
    status_t status = kStatus_Fail;

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

cb_status_t wrapper_rsa_keygen_opaque_compute(size_t key_size,
                                              uint32_t *key_id,
                                              uint8_t *public_key_addr,
                                              size_t public_key_buf_size)
{
    status_t status       = kStatus_Fail;
    uint16_t key_size_out = 0u;

    ele_gen_key_t conf = {0};
    conf.key_type      = kKeyType_RSA_KEY_PAIR;
    conf.key_lifetime  = kKey_Volatile;
    conf.key_lifecycle = kKeylifecycle_Open;
    conf.key_usage     = kKeyUsage_SignMessage;
    conf.key_size      = (key_size_t)key_size;
    conf.permitted_alg = kPermitted_RSA_PKCS1_V1_5_SHA256;
    conf.pub_key_addr  = (uint32_t)public_key_addr;
    conf.pub_key_size  = (uint32_t)public_key_buf_size;
    conf.key_group     = 1u;
    conf.key_id        = 0u;

    status = ELE_GenerateKey(S3MU, g_ele_ctx.key_management_handle, &conf, key_id, &key_size_out, false, false);

    return (status == kStatus_Success) ? CB_STATUS_SUCCESS : CB_STATUS_FAIL;
}

cb_status_t wrapper_rsa_keygen_opaque_key_delete(uint32_t key_id)
{
    status_t status = kStatus_Fail;

    status = ELE_DeleteKey(S3MU, g_ele_ctx.key_management_handle, key_id, false, false);

    return (status == kStatus_Success) ? CB_STATUS_SUCCESS : CB_STATUS_FAIL;
}

static cb_status_t wrapper_rsa_common_opaque_setup(void)
{
    status_t status = kStatus_Fail;

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

    return CB_STATUS_SUCCESS;
}

cb_status_t wrapper_rsa_sign_opaque_setup(void *ctx_internal)
{
    status_t status = kStatus_Fail;
    (void)ctx_internal;

    status = wrapper_rsa_common_opaque_setup();

    /* Open a signing service */
    status = ELE_OpenSignService(S3MU, g_ele_ctx.keystore_handle, &g_ele_ctx.sign_handle);
    if (status != kStatus_Success)
    {
        ELE_CloseKeyService(S3MU, g_ele_ctx.key_management_handle);
        ELE_CloseKeystore(S3MU, g_ele_ctx.keystore_handle);
        return CB_STATUS_FAIL;
    }

    /* Open a verification service */
    status = ELE_OpenVerifyService(S3MU, g_ele_ctx.session_handle, &g_ele_ctx.verify_handle);
    if (status != kStatus_Success)
    {
        ELE_CloseSignService(S3MU, g_ele_ctx.sign_handle);
        ELE_CloseKeyService(S3MU, g_ele_ctx.key_management_handle);
        ELE_CloseKeystore(S3MU, g_ele_ctx.keystore_handle);
        return CB_STATUS_FAIL;
    }

    return CB_STATUS_SUCCESS;
}

cb_status_t wrapper_rsa_sign_opaque_cleanup(void *ctx_internal)
{
    status_t status = kStatus_Fail;

    (void)ctx_internal;

    status = ELE_CloseSignService(S3MU, g_ele_ctx.sign_handle);
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

cb_status_t wrapper_rsa_sign_opaque_key_generate(void *ctx_internal,
                                                 uint32_t *key_id,
                                                 size_t key_size,
                                                 uint8_t *public_key_addr,
                                                 size_t *public_key_size,
                                                 size_t public_key_buf_size)
{
    status_t status           = kStatus_Fail;
    uint16_t key_size_out     = 0u;
    rsa_opaque_ctx_ele_t *ctx = (rsa_opaque_ctx_ele_t *)ctx_internal;

    ele_gen_key_t conf = {0};
    conf.key_type      = kKeyType_RSA_KEY_PAIR;
    conf.key_lifetime  = kKey_Volatile;
    conf.key_lifecycle = kKeylifecycle_Open;
    conf.key_usage     = kKeyUsage_SignMessage;
    conf.key_size      = (key_size_t)ctx->key_size; // Using key size from *_opaque_init()
    conf.permitted_alg = (key_permitted_alg_t)ctx->signature_scheme;
    conf.pub_key_addr  = (uint32_t)public_key_addr;
    conf.pub_key_size  = (uint32_t)public_key_buf_size;
    conf.key_group     = 1u;
    conf.key_id        = 0u;

    status = ELE_GenerateKey(S3MU, g_ele_ctx.key_management_handle, &conf, key_id, &key_size_out, false, false);
    *public_key_size = (size_t)key_size_out;

    return (status == kStatus_Success) ? CB_STATUS_SUCCESS : CB_STATUS_FAIL;
}

cb_status_t wrapper_rsa_sign_opaque_key_delete(void *ctx_internal, uint32_t key_id)
{
    status_t status = kStatus_Fail;
    (void)ctx_internal;

    status = ELE_DeleteKey(S3MU, g_ele_ctx.key_management_handle, key_id, false, false);

    return (status == kStatus_Success) ? CB_STATUS_SUCCESS : CB_STATUS_FAIL;
}

cb_status_t wrapper_rsa_sign_opaque_compute(void *ctx_internal,
                                            const uint8_t *message,
                                            size_t message_size,
                                            uint8_t *signature,
                                            uint32_t key_id)
{
    status_t status             = kStatus_Fail;
    ele_sign_t conf             = {0};
    rsa_opaque_ctx_ele_t *ctx   = (rsa_opaque_ctx_ele_t *)ctx_internal;
    uint32_t signature_size_out = 0u;

    conf.key_id     = key_id;
    conf.msg        = (const uint8_t *)message;
    conf.msg_size   = message_size;
    conf.signature  = signature;
    conf.sig_size   = ctx->signature_size;
    conf.scheme     = ctx->signature_scheme;
    conf.input_flag = CB_SIGVER_MESSAGE == ctx->input_type ? true : false;/* TODO BUG setting this to false does not seem to work on the S400? Returns failure */

    status = ELE_Sign(S3MU, g_ele_ctx.sign_handle, &conf, &signature_size_out);

    return (status == kStatus_Success) ? CB_STATUS_SUCCESS : CB_STATUS_FAIL;
}

cb_status_t wrapper_rsa_verify_opaque_compute(void *ctx_internal,
                                             const uint8_t *message,
                                             size_t message_size,
                                             uint8_t *signature,
                                             uint8_t *public_key,
                                             size_t public_key_size)
{
    status_t status           = kStatus_Fail;
    ele_verify_t conf         = {0};
    rsa_opaque_ctx_ele_t *ctx = (rsa_opaque_ctx_ele_t *)ctx_internal;
    bool result               = false;

    conf.pub_key           = (const uint8_t *)public_key;
    conf.key_size          = (uint16_t)ctx->key_size_bytes;
    conf.msg               = (const uint8_t *)message;
    conf.msg_size          = message_size;
    conf.signature         = signature;
    conf.sig_size          = ctx->signature_size;
    conf.keypair_type      = kKeyType_RSA_PUB_KEY;
    conf.scheme            = ctx->signature_scheme;
    conf.input_flag        = CB_SIGVER_MESSAGE == ctx->input_type ? true : false;
    conf.key_security_size = ctx->key_size;
    conf.internal          = false;

    status = ELE_Verify(S3MU, g_ele_ctx.verify_handle, &conf, &result);

    return (status == kStatus_Success) ? CB_STATUS_SUCCESS : CB_STATUS_FAIL;
}
