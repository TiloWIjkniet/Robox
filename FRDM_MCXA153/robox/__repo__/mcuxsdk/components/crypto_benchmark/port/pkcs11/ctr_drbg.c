/*
 * Copyright 2025 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "cb_osal.h"
#include "cb_ctr_drbg.h"

#include "include/common_utils.h"

typedef session_ctx_pkcs11_t ctr_drbg_ctx_pkcs11_t;

cb_status_t wrapper_ctr_drbg_init(void **ctx_internal)
{
    cb_status_t status = CB_STATUS_FAIL;

    ctr_drbg_ctx_pkcs11_t *ctx = NULL;
    session_ctx_pkcs11_t session_ctx = { 0 };

    if (NULL == ctx_internal)
    {
        goto end;
    }

    *ctx_internal = NULL;

    /* Allocate the internal PKCS11 hash context */
    ctx = (ctr_drbg_ctx_pkcs11_t *)malloc(sizeof(ctr_drbg_ctx_pkcs11_t));
    if (NULL == ctx)
    {
        goto end;
    }

    status = open_pkcs11_session(&session_ctx);
    if (CB_STATUS_SUCCESS != status)
    {
        goto end;
    }

    *ctx = session_ctx;

    /* Save the context */
    *ctx_internal = (void *)ctx;

end:
    if (CB_STATUS_SUCCESS != status && NULL != ctx)
    {
        free(ctx);
    }

    return status;
}
cb_status_t wrapper_ctr_drbg_deinit(void *ctx_internal)
{
    ctr_drbg_ctx_pkcs11_t *ctx = (ctr_drbg_ctx_pkcs11_t *)ctx_internal;

    if (NULL != ctx_internal)
    {
        close_pkcs11_session(ctx);

        free(ctx_internal);
    }

    return CB_STATUS_SUCCESS;
}

cb_status_t wrapper_ctr_drbg_compute(void *ctx_internal, uint8_t *output, size_t output_size)
{
    ctr_drbg_ctx_pkcs11_t *ctx = (ctr_drbg_ctx_pkcs11_t *)ctx_internal;
    CK_RV ret = CKR_OK;

    if (NULL == ctx)
    {
        return CB_STATUS_FAIL;
    }

    ret = ctx->pfunc->C_GenerateRandom(ctx->sess, (CK_BYTE_PTR)output, (CK_ULONG)output_size);

    return (ret == CKR_OK) ? CB_STATUS_SUCCESS : CB_STATUS_FAIL;
}
