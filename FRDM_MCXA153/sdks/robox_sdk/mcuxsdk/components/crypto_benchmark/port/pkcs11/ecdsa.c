/*
 * Copyright 2025 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "cb_osal.h"
#include "cb_ecdsa.h"

#include "include/common_utils.h"

typedef struct _ecdsa_ctx_pkcs11
{
    session_ctx_pkcs11_t session_ctx;
    CK_MECHANISM key_mech;
    CK_BBOOL ck_true;
    CK_MECHANISM_TYPE key_allowed_mech[1];
    CK_BYTE_PTR ec_params;
    CK_ULONG ec_params_length;
    CK_EDDSA_PARAMS ed_params;
    CK_ATTRIBUTE privkey_attrs[2];
    CK_ATTRIBUTE pubkey_attrs[2];
    CK_OBJECT_HANDLE hpubkey;
    CK_OBJECT_HANDLE hprivkey;
    CK_MECHANISM sign_verify_mech;
    CK_ULONG signature_size;
} ecdsa_ctx_pkcs11_t;

#define ASN1_PRINTABLE_STRING  19

static cb_status_t set_ec_params(ecdsa_ctx_pkcs11_t *ctx, cb_ecc_family_t family, size_t key_size)
{
    cb_status_t status = CB_STATUS_SUCCESS;

    CK_BYTE_PTR ec_name = NULL_PTR;

    if (NULL == ctx)
    {
        return CB_STATUS_FAIL;
    }

    switch(family)
    {
        case CB_ECC_FAMILY_SECP_R1:
            switch(key_size)
            {
                case 192:
                    ec_name = "prime192v1";
                    break;
                case 224:
                    ec_name = "secp224r1";
                    break;
                case 256:
                    ec_name = "prime256v1";
                    break;
                case 384:
                    ec_name = "secp384r1";
                    break;
                case 521:
                    ec_name = "secp521r1";
                    break;
                default:
                    status = CB_STATUS_FAIL;
                    break;
            }
            break;
        case CB_ECC_FAMILY_BRAINPOOL_P_R1:
            switch(key_size)
            {
                case 160:
                    ec_name = "brainpoolP160r1";
                    break;
                case 192:
                    ec_name = "brainpoolP192r1";
                    break;
                case 224:
                    ec_name = "brainpoolP224r1";
                    break;
                case 256:
                    ec_name = "brainpoolP256r1";
                    break;
                case 320:
                    ec_name = "brainpoolP320r1";
                    break;
                case 384:
                    ec_name = "brainpoolP384r1";
                    break;
                case 512:
                    ec_name = "brainpoolP512r1";
                    break;
                default:
                    status = CB_STATUS_FAIL;
                    break;
            }
            break;
        case CB_ECC_FAMILY_TWISTED_EDWARDS:
            switch(key_size)
            {
                case 255:
                    ec_name = "edwards25519";
                    break;
                case 448:
                    ec_name = "edwards448";
                    break;
                default:
                    status = CB_STATUS_FAIL;
                    break;
            }
            break;
        default:
            status = CB_STATUS_FAIL;
            break;
    }

    ctx->ec_params_length = strlen(ec_name) + 2;
    ctx->ec_params = malloc(ctx->ec_params_length);
    if (NULL == ctx->ec_params)
    {
        return CB_STATUS_FAIL;
    }

    ctx->ec_params[0] = ASN1_PRINTABLE_STRING;
    ctx->ec_params[1] = strlen(ec_name);

    memcpy(&ctx->ec_params[2], ec_name, ctx->ec_params_length - 2);

    return status;
}

static cb_status_t set_ecdsa_mech(cb_hash_t hash_type, CK_MECHANISM_TYPE_PTR mechanism)
{
    cb_status_t status = CB_STATUS_SUCCESS;

    switch(hash_type)
    {
        case CB_HASH_TYPE_SHA1:
            *mechanism = CKM_ECDSA_SHA1;
            break;
        case CB_HASH_TYPE_SHA224:
            *mechanism = CKM_ECDSA_SHA224;
            break;
        case CB_HASH_TYPE_SHA256:
            *mechanism = CKM_ECDSA_SHA256;
            break;
        case CB_HASH_TYPE_SHA384:
            *mechanism = CKM_ECDSA_SHA384;
            break;
        case CB_HASH_TYPE_SHA512:
            *mechanism = CKM_ECDSA_SHA512;
            break;
        case CB_HASH_TYPE_SHA3_224:
            *mechanism = CKM_ECDSA_SHA3_224;
            break;
        case CB_HASH_TYPE_SHA3_256:
            *mechanism = CKM_ECDSA_SHA3_256;
            break;
        case CB_HASH_TYPE_SHA3_384:
            *mechanism = CKM_ECDSA_SHA3_384;
            break;
        case CB_HASH_TYPE_SHA3_512:
            *mechanism = CKM_ECDSA_SHA3_512;
            break;
        default:
            status = CB_STATUS_FAIL;
            break;
    }

    return status;
}

static cb_status_t set_context(ecdsa_ctx_pkcs11_t *ctx, cb_hash_t hash_type, cb_ecc_family_t family, size_t key_size, cb_asym_sigver_input_type_t input_type)
{
    cb_status_t status = CB_STATUS_FAIL;

    if (NULL == ctx)
    {
        return CB_STATUS_FAIL;
    }

    ctx->hprivkey = 0;
    ctx->hpubkey = 0;
    ctx->key_mech.mechanism = CKM_EC_KEY_PAIR_GEN;
    ctx->ck_true = CK_TRUE;

    status = set_ec_params(ctx, family, key_size);
    if (CB_STATUS_SUCCESS != status)
    {
        return status;
    }

    if (CB_ECC_FAMILY_TWISTED_EDWARDS == family)
    {
        ctx->key_allowed_mech[0] = CKM_EDDSA;
        ctx->sign_verify_mech.mechanism = CKM_EDDSA;

        ctx->ed_params.ulContextDataLen = 0;
        ctx->ed_params.pContextData = NULL_PTR;
        ctx->ed_params.phFlag = CK_FALSE;

        if (CB_SIGVER_HASH == input_type)
        {
            ctx->ed_params.phFlag = CK_TRUE;
        }

        ctx->sign_verify_mech.pParameter = (CK_VOID_PTR)&ctx->ed_params;
        ctx->sign_verify_mech.ulParameterLen = sizeof(ctx->ed_params);

        /* For edwards448, security size is 448 bits and key size is 456 bits */
        if (key_size == 448)
        {
            key_size = 456;
        }
    }
    else
    {
        status = set_ecdsa_mech(hash_type, &ctx->key_allowed_mech[0]);
        if (CB_STATUS_SUCCESS != status)
        {
            return status;
        }

        if (CB_SIGVER_HASH == input_type)
        {
            ctx->sign_verify_mech.mechanism = CKM_ECDSA;
        }
        else
        {
            ctx->sign_verify_mech.mechanism = ctx->key_allowed_mech[0];
        }

        ctx->sign_verify_mech.pParameter = NULL_PTR;
        ctx->sign_verify_mech.ulParameterLen = 0;
    }

    SET_ATTRIBUTE(ctx->privkey_attrs[0], CKA_SIGN, ctx->ck_true);
    SET_ATTRIBUTE(ctx->privkey_attrs[1], CKA_ALLOWED_MECHANISMS, ctx->key_allowed_mech);

    SET_ATTRIBUTE(ctx->pubkey_attrs[0], CKA_VERIFY, ctx->ck_true);
    ctx->pubkey_attrs[1].type = CKA_EC_PARAMS;
    ctx->pubkey_attrs[1].pValue = ctx->ec_params;
    ctx->pubkey_attrs[1].ulValueLen = ctx->ec_params_length;

    ctx->signature_size = (key_size + 7) >> 2;

    return CB_STATUS_SUCCESS;
}

static cb_status_t wrapper_ecdsa_init_common(void **ctx_internal,
                                             cb_ecdsa_type_t ecdsa_type,
                                             cb_hash_t hash_type,
                                             cb_ecc_family_t family,
                                             size_t key_size,
                                             cb_asym_sigver_input_type_t input_type)
{
    cb_status_t status = CB_STATUS_FAIL;

    ecdsa_ctx_pkcs11_t *ctx = NULL;
    session_ctx_pkcs11_t session_ctx = { 0 };

    (void)ecdsa_type;

    if (NULL == ctx_internal)
    {
        return status;
    }

    *ctx_internal = NULL;

    /* Allocate the internal PKCS11 context */
    ctx = (ecdsa_ctx_pkcs11_t *)malloc(sizeof(ecdsa_ctx_pkcs11_t));
    if (NULL == ctx)
    {
        return CB_STATUS_FAIL;
    }

    status = set_context(ctx, hash_type, family, key_size, input_type);
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

cb_status_t wrapper_ecdsa_init(void **ctx_internal,
                               cb_ecdsa_type_t ecdsa_type,
                               cb_hash_t hash_type,
                               cb_ecc_family_t family,
                               size_t key_size,
                               cb_asym_sigver_input_type_t input_type)
{
    cb_status_t status = CB_STATUS_FAIL;
    ecdsa_ctx_pkcs11_t *ctx = NULL;
    CK_RV ret = CKR_OK;

    status = wrapper_ecdsa_init_common(ctx_internal, ecdsa_type, hash_type,
                                       family, key_size, input_type);
    if (CB_STATUS_SUCCESS != status)
    {
        return status;
    }

    ctx = (ecdsa_ctx_pkcs11_t *)*ctx_internal;
    if (NULL == ctx)
    {
        return CB_STATUS_FAIL;
    }

    /* Key pair must be generated since the transparent layer does not pass valid private and public keys. */
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
        (void)wrapper_ecdsa_deinit(ctx);
    }

    return (ret == CKR_OK) ? CB_STATUS_SUCCESS : CB_STATUS_FAIL;
}


cb_status_t wrapper_ecdsa_deinit(void *ctx_internal)
{
    ecdsa_ctx_pkcs11_t *ctx = (ecdsa_ctx_pkcs11_t *)ctx_internal;

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

        if (ctx->ec_params)
        {
            free(ctx->ec_params);
        }

        free(ctx_internal);
    }

    return CB_STATUS_SUCCESS;
}


cb_status_t wrapper_ecdsa_sign_compute(void *ctx_internal,
                                       const uint8_t *private_key,
                                       uint8_t *message,
                                       size_t message_size,
                                       uint8_t *signature)
{
    ecdsa_ctx_pkcs11_t *ctx = (ecdsa_ctx_pkcs11_t *)ctx_internal;
    CK_RV ret = CKR_OK;

    (void)private_key;

    if (NULL == ctx)
    {
        return CB_STATUS_FAIL;
    }

    ret = ctx->session_ctx.pfunc->C_SignInit(ctx->session_ctx.sess, &ctx->sign_verify_mech, ctx->hprivkey);
    if (CKR_OK != ret)
    {
        goto end;
    }

    ret = ctx->session_ctx.pfunc->C_Sign(ctx->session_ctx.sess, message, message_size, signature, &ctx->signature_size);

end:
    return (ret == CKR_OK) ? CB_STATUS_SUCCESS : CB_STATUS_FAIL;
}

cb_status_t wrapper_ecdsa_verify_compute(void *ctx_internal,
                                         const uint8_t *public_key,
                                         uint8_t *message,
                                         size_t message_size,
                                         uint8_t *signature)
{
    ecdsa_ctx_pkcs11_t *ctx = (ecdsa_ctx_pkcs11_t *)ctx_internal;
    CK_RV ret = CKR_OK;

    (void)public_key;

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

cb_status_t wrapper_ecdsa_opaque_init(
    void **ctx_internal, cb_ecdsa_type_t ecdsa_type, cb_hash_t hash_type, cb_ecc_family_t family, size_t key_size,
    cb_asym_sigver_input_type_t input_type)
{
    return wrapper_ecdsa_init_common(ctx_internal, ecdsa_type, hash_type,
                                     family, key_size, input_type);
}

cb_status_t wrapper_ecdsa_opaque_deinit(void *ctx_internal)
{
    return wrapper_ecdsa_deinit(ctx_internal);
}

cb_status_t wrapper_ecdsa_sign_opaque_setup(void *ctx_internal)
{
    (void)ctx_internal;

    return CB_STATUS_SUCCESS;
}

cb_status_t wrapper_ecdsa_sign_opaque_cleanup(void *ctx_internal)
{
    (void)ctx_internal;

    return CB_STATUS_SUCCESS;
}

cb_status_t wrapper_ecdsa_sign_opaque_key_generate(void *ctx_internal,
                                                   uint32_t *key_id,
                                                   size_t key_size,
                                                   uint8_t *public_key_addr,
                                                   size_t *public_key_size,
                                                   size_t public_key_buf_size)
{
    ecdsa_ctx_pkcs11_t *ctx = (ecdsa_ctx_pkcs11_t *)ctx_internal;
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

cb_status_t wrapper_ecdsa_sign_opaque_key_delete(void *ctx_internal, uint32_t key_id)
{
    /* PKCS11 destroys keys the same way for both transparent and opaque.
     * Can defer this to wrapper_ecdsa_opaque_deinit(). */
    (void)ctx_internal;
    (void)key_id;

    return CB_STATUS_SUCCESS;
}

cb_status_t wrapper_ecdsa_sign_opaque_compute(void *ctx_internal, const uint8_t *message, size_t message_size, uint8_t *signature, uint32_t key_id)
{
    return wrapper_ecdsa_sign_compute(ctx_internal, NULL, (uint8_t *)message, message_size, signature);
}

cb_status_t wrapper_ecdsa_verify_opaque_compute(void *ctx_internal, const uint8_t *message, size_t message_size, uint8_t *signature, uint8_t *public_key, size_t public_key_size)
{
    return wrapper_ecdsa_verify_compute(ctx_internal, NULL, (uint8_t *)message, message_size, signature);
}


cb_status_t wrapper_ecdsa_keygen_opaque_compute(void *ctx_internal,
                                                size_t key_size,
                                                uint32_t *key_id,
                                                uint8_t *public_key_addr,
                                                size_t public_key_buf_size)
{
    ecdsa_ctx_pkcs11_t *ctx = (ecdsa_ctx_pkcs11_t *)ctx_internal;
    CK_RV ret = CKR_OK;

    (void)ctx_internal;
    (void)key_size;
    (void)key_id;
    (void)public_key_addr;
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

cb_status_t wrapper_ecdsa_keygen_opaque_setup(void)
{
    return CB_STATUS_SUCCESS;
}

cb_status_t wrapper_ecdsa_keygen_opaque_key_delete(uint32_t key_id)
{
    /* PKCS11 destroys keys the same way for both transparent and opaque.
     * Can defer this to wrapper_ecdsa_opaque_deinit(). */
    (void)key_id;

    return CB_STATUS_SUCCESS;
}

cb_status_t wrapper_ecdsa_keygen_opaque_cleanup(void)
{
    return CB_STATUS_SUCCESS;
}
