/*
 * Copyright 2024 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/**
 * @file  cb_aead.h
 * @brief Contains declarations for AEAD benchmarking.
 */

#ifndef CB_AEAD_H
#define CB_AEAD_H

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "crypto_benchmark.h"
#include "cb_types.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define AEAD_IV_SIZE       (12u) /* 12 Byte IV for GCM/CCM or 12 Byte nonce for CHACHAPOLY */
#define AEAD_TAG_SIZE      (16u) /* GCM, CCM, and CHACHAPOLY support 128-bit tags */
#define AEAD_AAD_SIZE      (16u)
#define AEAD_BLOCK         (16u) /* GCM and CCM only work with 128-bit blocks, use same logic for CHACHAPOLY */
#define AEAD_PAYLOAD       (16u)
#define AEAD_PAYLOAD_MULTI (AEAD_PAYLOAD * MULTIPLE_BLOCK) /* Multi-block */

/*******************************************************************************
 * Variables
 ******************************************************************************/

/* Array initializer list of AEAD benchmarking cases of type
 * aead_benchmark_cases_t.
 */
extern const aead_benchmark_cases_t aead_benchmark_cases[];
extern const size_t aead_benchmark_cases_nb;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
cb_status_t aead_benchmark(void);
cb_status_t aead_benchmark_opaque(void);

void print_aead_case_name(cb_aead_type_t aead, size_t key_size, size_t block_count);

/**
 * @brief Initialize the port layer internal context for AEAD.
 *
 *        This is called before every benchmarked call (the *_compute()
 *        functions). Used to preemptively set up the port layer's context
 *        in order to reduce the overhead of the *_compute() functions.
 *
 * @note  The internal context is expected to be heap allocated.
 *
 * @param ctx_internal This will hold the port layer's heap-allocated context.
 * @param aead_type    This will hold the AEAD type.
 * @param key          The encryption key for the next benchmark.
 * @param key_size     The size of the key for the next benchmark, in bits.
 *
 * @return CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_aead_init(void **ctx_internal, cb_aead_type_t aead_type, const uint8_t *key, size_t key_size);

/**
 * @brief Deinitialize the port layer internal context for AEAD.
 *
 *        Free the heap-allocated internal context and carry out other
 *        port-specific deinitialization steps.
 *
 * @param ctx_internal The internal context to be free()'d.
 *
 * @retval CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_aead_deinit(void *ctx_internal);

/**
 * @brief Benchmarked function for AEAD authenticated encryption.
 *
 * @param ctx_internal The port layer internal context.
 * @param message      The input message.
 * @param message_size The input message size.
 * @param iv           The input initialization vector.
 * @param iv_size      The IV size in Bytes.
 * @param aad          The input additional data.
 * @param aad_size     The AAD size in Bytes.
 * @param ciphertext   The output buffer for the resulting ciphertext.
 * @param tag          The output buffer for the resulting authentication tag.
 * @param tag_size     The size of the tag.
 * @param key          The input key.
 * @param key_size     The key size in bits
 *
 * @retval CB_STATUS_SUCCESS on success, different otherwise.
 */
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
                                 size_t key_size);

/*******************************************************************************
 * Opaque wrappers
 ******************************************************************************/
/**
 * @brief Initialize the port layer internal context for opaque AEAD.
 *
 *        This is called before every benchmarked call (the *_compute()
 *        functions). Used to preemptively set up the port layer's context
 *        in order to reduce the overhead of the *_compute() functions.
 *
 * @note  The internal context is expected to be heap allocated.
 *
 * @param ctx_internal This will hold the port layer's heap-allocated context.
 * @param aead_type    This will hold the AEAD type.
 * @param key_size     The size of the key for the next benchmark, in bits.
 *
 * @return CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_aead_opaque_init(void **ctx_internal, cb_aead_type_t aead_type, size_t key_size);

/**
 * @brief Deinitialize the port layer internal context for opaque AEAD.
 *
 *        Free the heap-allocated internal context and carry out other
 *        port-specific deinitialization steps.
 *
 * @param ctx_internal The internal context to be free()'d.
 *
 * @retval CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_aead_opaque_deinit(void *ctx_internal);

/**
 * @brief The benchmarked function call for AEAD.
 *
 * @param ctx_internal The port layer internal context.
 * @param message      The input message.
 * @param message_size The input message size.
 * @param iv           The input initialization vector.
 * @param iv_size      The IV size in Bytes.
 * @param aad          The input additional data.
 * @param aad_size     The AAD size in Bytes.
 * @param ciphertext   The output buffer for the resulting ciphertext.
 * @param tag          The output buffer for the resulting authentication tag.
 * @param tag_size     The size of the tag.
 * @param key_id       The opaque key ID.
 *
 * @retval CB_STATUS_SUCCESS on success, different otherwise.
 */
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
                                        uint32_t key_id);

/**
 * @brief Setup called before each round of benchmarked AEAD.
 *
 * @param ctx_internal The port layer internal context.
 *
 * @retval CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_aead_opaque_setup(void *ctx_internal);

/**
 * @brief Cleanup called after each round of benchmarked AEAD.
 *
 * @param ctx_internal The port layer internal context.
 *
 * @retval CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_aead_opaque_cleanup(void *ctx_internal);

/**
 * @brief A call to this function is made before each each round of benchmarked
 *        AEAD to generate a new valid opaque key.
 *
 * @param ctx_internal The port layer internal context.
 * @param key_size     The size of the generated key in bits.
 * @param key_id       Returns the key ID of the generated opaque key.
 *
 * @retval CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_aead_opaque_key_generate(void *ctx_internal, size_t key_size, uint32_t *key_id);

/**
 * @brief Delete an opaque key used for benchmarked AEAD
 *        at the end of the benchmarking round.
 *
 * @param ctx_internal The port layer internal context.
 * @param key_id       The key ID of the opaque key.
 *
 * @retval CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_aead_opaque_key_delete(void *ctx_internal, uint32_t key_id);

#endif /* CB_AEAD_H */
