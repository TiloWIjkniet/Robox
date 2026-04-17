/*
 * Copyright 2025 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "cb_osal.h"
#include "cb_hash.h"

#include "include/common_utils.h"

typedef struct _hash_ctx_pkcs11
{
    session_ctx_pkcs11_t session_ctx;
    CK_MECHANISM digest_mech;
    CK_ULONG digest_length;
    CK_BYTE_PTR digest;
} hash_ctx_pkcs11_t;

static cb_status_t convert_cb_hash_type_to_pkcs11_alg(cb_hash_t hash_type, CK_MECHANISM *digest_mech)
{
    cb_status_t status = CB_STATUS_SUCCESS;

    digest_mech->pParameter = NULL_PTR;
    digest_mech->ulParameterLen = 0;

    switch(hash_type)
    {
        case CB_HASH_TYPE_SHA1:
            digest_mech->mechanism = CKM_SHA_1;
            break;
        case CB_HASH_TYPE_SHA224:
            digest_mech->mechanism = CKM_SHA224;
            break;
        case CB_HASH_TYPE_SHA256:
            digest_mech->mechanism = CKM_SHA256;
            break;
        case CB_HASH_TYPE_SHA384:
            digest_mech->mechanism = CKM_SHA384;
            break;
        case CB_HASH_TYPE_SHA512:
            digest_mech->mechanism = CKM_SHA512;
            break;
        case CB_HASH_TYPE_SHA3_224:
            digest_mech->mechanism = CKM_SHA3_224;
            break;
        case CB_HASH_TYPE_SHA3_256:
            digest_mech->mechanism = CKM_SHA3_256;
            break;
        case CB_HASH_TYPE_SHA3_384:
            digest_mech->mechanism = CKM_SHA3_384;
            break;
        case CB_HASH_TYPE_SHA3_512:
            digest_mech->mechanism = CKM_SHA3_512;
            break;
        default:
            status = CB_STATUS_FAIL;
            break;
    }

    return status;
}

cb_status_t wrapper_hash_init(void **ctx_internal, cb_hash_t hash_type)
{
    cb_status_t status = CB_STATUS_FAIL;

    hash_ctx_pkcs11_t *ctx = NULL;
    session_ctx_pkcs11_t session_ctx = { 0 };
    CK_ULONG digest_length = 0;
    CK_BYTE_PTR digest = NULL_PTR;

    if (NULL == ctx_internal)
    {
        return CB_STATUS_FAIL;
    }

    *ctx_internal = NULL;

    /* Allocate the internal PKCS11 hash context */
    ctx = (hash_ctx_pkcs11_t *)malloc(sizeof(hash_ctx_pkcs11_t));
    if (NULL == ctx)
    {
        return CB_STATUS_FAIL;
    }

    digest_length = CB_HASH_LENGTH(hash_type);
    if (!digest_length)
    {
        goto end;
    }

    digest = malloc(digest_length);
    if (NULL == digest)
    {
        goto end;
    }

    status = convert_cb_hash_type_to_pkcs11_alg(hash_type, &ctx->digest_mech);  
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
    ctx->digest_length = digest_length;
    ctx->digest = digest;

    /* Save the context */
    *ctx_internal = (void *)ctx;

end:
    if (CB_STATUS_SUCCESS != status)
    {
        if (NULL != digest)
        {
            free(digest);
        }

        if (NULL != ctx)
        {
            free(ctx);
        }
    }

    return status;
}

cb_status_t wrapper_hash_compute(void *ctx_internal, const uint8_t *message,
                                 size_t message_size, uint8_t *hash)
{
    hash_ctx_pkcs11_t *ctx = (hash_ctx_pkcs11_t *)ctx_internal;
    CK_ULONG digest_length = 0;
    CK_RV ret = CKR_OK;

    if (NULL == ctx)
    {
        return CB_STATUS_FAIL;
    }

    ret = ctx->session_ctx.pfunc->C_DigestInit(ctx->session_ctx.sess, &ctx->digest_mech);
    if (CKR_OK != ret)
    {
        goto end;
    }

    digest_length = ctx->digest_length;

    ret = ctx->session_ctx.pfunc->C_Digest(ctx->session_ctx.sess, (CK_BYTE_PTR)message, (CK_ULONG)message_size, ctx->digest, &digest_length);

end:
    return (ret == CKR_OK) ? CB_STATUS_SUCCESS : CB_STATUS_FAIL;
}

cb_status_t wrapper_hash_deinit(void *ctx_internal)
{
    hash_ctx_pkcs11_t *ctx = (hash_ctx_pkcs11_t *)ctx_internal;

    if (NULL != ctx_internal)
    {
        close_pkcs11_session(&ctx->session_ctx);

        if (NULL != ctx->digest)
        {
            free(ctx->digest);
        }

        free(ctx_internal);
    }

    return CB_STATUS_SUCCESS;
}
