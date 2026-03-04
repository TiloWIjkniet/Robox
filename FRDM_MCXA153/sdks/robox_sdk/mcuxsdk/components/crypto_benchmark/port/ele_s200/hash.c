/*
 * Copyright 2024 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "cb_osal.h"
#include "cb_hash.h"
//#include "ele_crypto.h"
#include "ele.h"

typedef struct _hash_ctx_ele_t
{
    //hash_algo_t algorithm;
    //ele_hash_ctx_t multipart_ctx; /*!< Used in multipart API */
  uint32_t dummy; /* Dummy variable, delete after filling context*/
} hash_ctx_ele_t;

cb_status_t wrapper_hash_init(void **ctx_internal, cb_hash_t hash_type)
{
//    hash_ctx_ele_t *ele_context = NULL;
//    uint32_t hash_algorithm_tmp = 0u;
//
//    /* Allocate the internal ELE context */
//    ele_context = (hash_ctx_ele_t *)malloc(sizeof(hash_ctx_ele_t));
//    if (NULL == ele_context)
//    {
//        return CB_STATUS_FAIL;
//    }
//
//    hash_algorithm_tmp = 0x02000000u | (uint32_t)hash_type;
//
//    ele_context->algorithm = (hash_algo_t)hash_algorithm_tmp;
//
//    /* Save the context */
//    *ctx_internal = (void *)ele_context;
    return CB_STATUS_SUCCESS;
}

cb_status_t wrapper_hash_deinit(void *ctx_internal)
{
    if (NULL != ctx_internal)
    {
        memset(ctx_internal, 0, sizeof(hash_ctx_ele_t));
        free(ctx_internal);
    }

    return CB_STATUS_SUCCESS;
}

cb_status_t wrapper_hash_compute(void *ctx_internal, const uint8_t *message, size_t message_size, uint8_t *hash)
{
//    status_t status     = kStatus_Fail;
//    hash_ctx_ele_t *ctx = (hash_ctx_ele_t *)ctx_internal;
//    uint32_t out_length = 0u;
//
//    status = ELE_Hash(S3MU, message, message_size, hash, (1024u / 8u), &out_length, ctx->algorithm);
//
//    return (status == kStatus_Success) ? CB_STATUS_SUCCESS : CB_STATUS_FAIL;
      return CB_STATUS_SUCCESS;
}

cb_status_t wrapper_hash_init_multipart(void **ctx_internal, cb_hash_t hash_type)
{
    /* Reuse regular init */
    return wrapper_hash_init(ctx_internal, hash_type);
}

cb_status_t wrapper_hash_deinit_multipart(void *ctx_internal)
{
    /* Reuse regular deinit */
    return wrapper_hash_deinit(ctx_internal);
}

cb_status_t wrapper_hash_start_multipart_compute(void *ctx_internal)
{
//    status_t status     = kStatus_Fail;
//    hash_ctx_ele_t *ctx = (hash_ctx_ele_t *)ctx_internal;

//    status = ELE_Hash_Init(S3MU, &(ctx->multipart_ctx), ctx->algorithm);
//
//    return (status == kStatus_Success) ? CB_STATUS_SUCCESS : CB_STATUS_FAIL;
      return CB_STATUS_SUCCESS;
}

cb_status_t wrapper_hash_update_multipart_compute(void *ctx_internal, const uint8_t *message, size_t message_size, uint8_t *hash)
{
//    status_t status     = kStatus_Fail;
//    hash_ctx_ele_t *ctx = (hash_ctx_ele_t *)ctx_internal;

//    status = ELE_Hash_Update(S3MU, &(ctx->multipart_ctx), ctx->algorithm, message, message_size);
//
//    return (status == kStatus_Success) ? CB_STATUS_SUCCESS : CB_STATUS_FAIL;
      return CB_STATUS_SUCCESS;
}

cb_status_t wrapper_hash_finish_multipart_compute(void *ctx_internal, uint8_t *hash)
{
//    status_t status     = kStatus_Fail;
//    hash_ctx_ele_t *ctx = (hash_ctx_ele_t *)ctx_internal;
//    uint32_t out_length = 0u;
//
//    status =  ELE_Hash_Finish(S3MU, &(ctx->multipart_ctx), ctx->algorithm, hash, (1024u / 8u), &out_length, NULL, 0u);

//    return (status == kStatus_Success) ? CB_STATUS_SUCCESS : CB_STATUS_FAIL;
      return CB_STATUS_SUCCESS;
}
