/*
 * Copyright 2024-2025 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/**
 * @file  cb_hash.h
 * @brief Contains declarations for HASH benchmarking.
 */

#ifndef CB_HASH_H
#define CB_HASH_H

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "crypto_benchmark.h"
#include "cb_types.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* SHA-1 */

#define SHA1_BLOCK         64u        /* SHA1 block is 64 Bytes (512bit) */
#define SHA1_PAYLOAD       SHA1_BLOCK /* Single-block payload == single block (we do not care about padding) */
#define SHA1_PAYLOAD_MULTI (SHA1_BLOCK * (MULTIPLE_BLOCK - 1) + SHA1_PAYLOAD) /* Multi-block */

/* SHA-2 */

#define SHA224_BLOCK         64u                                              /* SHA224 block is 64 Bytes (512bit) */
#define SHA224_PAYLOAD       SHA224_BLOCK /* Single-block payload == single block (we do not care about padding) */
#define SHA224_PAYLOAD_MULTI (SHA224_BLOCK * (MULTIPLE_BLOCK - 1) + SHA224_PAYLOAD) /* Multi-block */

#define SHA256_BLOCK         64u          /* SHA256 block is 64 Bytes (512bit) */
#define SHA256_PAYLOAD       SHA256_BLOCK /* Single-block payload == single block (we do not care about padding) */
#define SHA256_PAYLOAD_MULTI (SHA256_BLOCK * (MULTIPLE_BLOCK - 1) + SHA256_PAYLOAD) /* Multi-block */

#define SHA384_BLOCK         128u         /* SHA384 block is 128 Bytes (1024bit) */
#define SHA384_PAYLOAD       SHA384_BLOCK /* Single-block payload == single block (we do not care about padding) */
#define SHA384_PAYLOAD_MULTI (SHA384_BLOCK * (MULTIPLE_BLOCK - 1) + SHA384_PAYLOAD) /* Multi-block */

#define SHA512_BLOCK         128u         /* SHA512 block is 128 Bytes (1024bit) */
#define SHA512_PAYLOAD       SHA512_BLOCK /* Single-block payload == single block (we do not care about padding) */
#define SHA512_PAYLOAD_MULTI (SHA512_BLOCK * (MULTIPLE_BLOCK - 1) + SHA512_PAYLOAD) /* Multi-block */

/* SHA-3 */

#define SHA3_224_BLOCK         144u           /* SHA-3_224 block is 144 Bytes (1152bit) */
#define SHA3_224_PAYLOAD       SHA3_224_BLOCK /* Single-block payload == single block (we do not care about padding) */
#define SHA3_224_PAYLOAD_MULTI (SHA3_224_BLOCK * (MULTIPLE_BLOCK - 1) + SHA3_224_PAYLOAD) /* Multi-block */

#define SHA3_256_BLOCK         136u           /* SHA-3_256 block is 136 Bytes (1088bit) */
#define SHA3_256_PAYLOAD       SHA3_256_BLOCK /* Single-block payload == single block (we do not care about padding) */
#define SHA3_256_PAYLOAD_MULTI (SHA3_256_BLOCK * (MULTIPLE_BLOCK - 1) + SHA3_256_PAYLOAD) /* Multi-block */

#define SHA3_384_BLOCK         104u           /* SHA-3_384 block is 104 Bytes (832bit) */
#define SHA3_384_PAYLOAD       SHA3_384_BLOCK /* Single-block payload == single block (we do not care about padding) */
#define SHA3_384_PAYLOAD_MULTI (SHA3_384_BLOCK * (MULTIPLE_BLOCK - 1) + SHA3_384_PAYLOAD) /* Multi-block */

#define SHA3_512_BLOCK         72u            /* SHA-3_512 block is 72 Bytes (576bit) */
#define SHA3_512_PAYLOAD       SHA3_512_BLOCK /* Single-block payload == single block (we do not care about padding) */
#define SHA3_512_PAYLOAD_MULTI (SHA3_512_BLOCK * (MULTIPLE_BLOCK - 1) + SHA3_512_PAYLOAD) /* Multi-block */

/** Same principle as PSA_HASH_LENGTH */
#define CB_HASH_LENGTH(hash_alg)                  \
    (                                             \
        hash_alg == CB_HASH_TYPE_SHA1     ? 20u : \
        hash_alg == CB_HASH_TYPE_SHA224   ? 28u : \
        hash_alg == CB_HASH_TYPE_SHA256   ? 32u : \
        hash_alg == CB_HASH_TYPE_SHA384   ? 48u : \
        hash_alg == CB_HASH_TYPE_SHA512   ? 64u : \
        hash_alg == CB_HASH_TYPE_SHA3_224 ? 28u : \
        hash_alg == CB_HASH_TYPE_SHA3_256 ? 32u : \
        hash_alg == CB_HASH_TYPE_SHA3_384 ? 48u : \
        hash_alg == CB_HASH_TYPE_SHA3_512 ? 64u : \
        hash_alg == CB_HASH_TYPE_SHAKE256 ? 64u : \
        0u)

/*******************************************************************************
 * Variables
 ******************************************************************************/

/* Array initializer list of HASH benchmarking cases of type
 * hash_benchmark_cases_t.
 */
extern const hash_benchmark_cases_t hash_benchmark_cases[];
extern const size_t hash_benchmark_cases_nb;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
cb_status_t hash_benchmark(void);

void print_hash_case_name(cb_hash_t hash, size_t block_count);

/**
 * @brief Helper function for getting the correct block size
 *        for a specific HASH type.
 *
 * @param hash         The hash type in case \c algorithm is HMAC.
 * @param block_size   Output parameter for the resulting block size.
 * @param payload_size Output parameter for the resulting payload size,
 *                     which is the \c block_size minus the padding.
 *
 * @return CB_STATUS_SUCCESS on success, CB_STATUS_FAIL otherwise.
 */
cb_status_t convert_hash_type_to_block_size(cb_hash_t hash, size_t *block_size, size_t *payload_size);

/**
 * @brief Initialize the port layer internal context for HASH.
 *
 *        This is called before every benchmarked call (the *_compute()
 *        functions). Used to preemptively set up the port layer's context
 *        in order to reduce the overhead of the *_compute() functions.
 *
 * @note  The internal context is expected to be heap allocated.
 *
 * @param ctx_internal This will hold the port layer's heap-allocated context.
 * @param hash_type    The hash type.
 *
 * @return CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_hash_init(void **ctx_internal, cb_hash_t hash_type);

/**
 * @brief Deinitialize the port layer internal context for HASH.
 *
 *        Free the heap-allocated internal context and carry out other
 *        port-specific deinitialization steps.
 *
 * @param ctx_internal The internal context to be free()'d.
 *
 * @retval CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_hash_deinit(void *ctx_internal);

/**
 * @brief Benchmarked function for HASH digest generation.
 *
 * @param ctx_internal The port layer internal context.
 * @param message      The message to digest.
 * @param message_size The length of the message.
 * @param hash         The output digest.
 *
 * @retval CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_hash_compute(void *ctx_internal, const uint8_t *message, size_t message_size, uint8_t *hash);

/**
 * @brief Initialize the port layer internal context for
 *        multi-part (streaming) HASH.
 *
 *        This is called before every benchmarked call (the *_compute()
 *        functions). Used to preemptively set up the port layer's context
 *        in order to reduce the overhead of the *_compute() functions.
 *
 * @note  The multi-part API consists of three separate *_compute() functions.
 *        This initializer is NOT called before each of them, but only before
 *        all three of them.
 *
 * @note  The internal context is expected to be heap allocated.
 *
 * @param ctx_internal This will hold the port layer's heap-allocated context.
 * @param hash_type    The hash type.
 *
 * @return CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_hash_init_multipart(void **ctx_internal, cb_hash_t hash_type);

/**
 * @brief Deinitialize the port layer internal context for HASH.
 *
 *        Free the heap-allocated internal context and carry out other
 *        port-specific deinitialization steps.
 *
 * @note  The multi-part API consists of three separate *_compute() functions.
 *        This initializer is NOT called after each of them, but only after
 *        all three of them.
 *
 * @param ctx_internal The internal context to be free()'d.
 *
 * @retval CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_hash_deinit_multipart(void *ctx_internal);

/**
 * @brief Benchmarked function for starting a multi-part (streaming) hash.
 *        No inputs/outputs for the hash are provided, as no actual calculation
 *        is expected in this call.
 *
 * @param ctx_internal The port layer internal context.
 *
 * @retval CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_hash_start_multipart_compute(void *ctx_internal);

/**
 * @brief Benchmarked function for updating a multi-part (streaming) hash.
 *        For benchmarking the actual calculation step in the multi-part API.
 *
 * @param ctx_internal The port layer internal context.
 * @param message      The message to digest.
 * @param message_size The length of the message.
 * @param hash         The output digest in case the API requires it.
 *
 * @retval CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_hash_update_multipart_compute(void *ctx_internal,
                                                  const uint8_t *message,
                                                  size_t message_size,
                                                  uint8_t *hash);

/**
 * @brief Benchmarked function for finalizing a multi-part (streaming) hash.
 *        This function is not expected to append more data to the hash, so no
 *        hash inputs are provided.
 *
 * @param ctx_internal The port layer internal context.
 * @param hash         The output digest of the streamed hash.
 *
 * @retval CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_hash_finish_multipart_compute(void *ctx_internal, uint8_t *hash);

#endif /* CB_HASH_H */
