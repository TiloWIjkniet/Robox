/*
 * Copyright 2024 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/**
 * @file  cb_aes.h
 * @brief Contains declarations for AES benchmarking.
 */

#ifndef CB_AES_H
#define CB_AES_H

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "crypto_benchmark.h"
#include "cb_types.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define AES_BLOCK 16u /* AES block is 16 Bytes (128bit) */
#define AES_PAYLOAD                                                                            \
    16u               /* To be generic and support any kind of padding (including no padding), \
                       * the payload is set to the block size. */
#define AES_PAYLOAD_MULTI (AES_PAYLOAD * MULTIPLE_BLOCK) /* Multi-block */

/*******************************************************************************
 * Variables
 ******************************************************************************/

/* Array initializer list of AES benchmarking cases of type
 * aes_benchmark_cases_t.
 */
extern const aes_benchmark_cases_t aes_benchmark_cases[];
extern const size_t aes_benchmark_cases_nb;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
cb_status_t aes_benchmark(void);
cb_status_t aes_benchmark_opaque(void);

void print_aes_case_name(cb_cipher_type_t cipher_type, size_t key_size, size_t block_count);

/**
 * @brief Initialize the port layer internal context for AES.
 *
 *        This is called before every benchmarked call (the *_compute()
 *        functions). Used to preemptively set up the port layer's context
 *        in order to reduce the overhead of the *_compute() functions.
 *
 * @note  The internal context is expected to be heap allocated.
 *
 * @param ctx_internal This will hold the port layer's heap-allocated context.
 * @param cipher_type  This will hold the cipher type - for AES this
 *                     is the mode of operation.
 * @param key          The encryption key for the next benchmark.
 * @param key_size     The size of the key for the next benchmark, in bits.
 *
 * @return CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_aes_init(void **ctx_internal, cb_cipher_type_t cipher_type, const uint8_t *key, size_t key_size);

/**
 * @brief Deinitialize the port layer internal context for AES.
 *
 *        Free the heap-allocated internal context and carry out other
 *        port-specific deinitialization steps.
 *
 * @param ctx_internal The internal context to be free()'d.
 *
 * @retval CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_aes_deinit(void *ctx_internal);

/**
 * @brief Benchmarked function for AES encryption.
 *
 * @param ctx_internal The port layer internal context.
 * @param message      The message to encrypt.
 * @param message_size The length of the message.
 * @param iv           The 16-Byte initialization vector.
 * @param key          The encryption key.
 * @param ciphertext   The output ciphertext.
 *
 * @retval CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_aes_compute(void *ctx_internal,
                                const uint8_t *message,
                                size_t message_size,
                                const uint8_t *iv,
                                const uint8_t *key,
                                uint8_t *ciphertext);

/*******************************************************************************
 * Opaque wrappers
 ******************************************************************************/
/**
 * @brief Initialize the port layer internal context for opaque AES.
 *
 *        This is called before every benchmarked call (the *_compute()
 *        functions). Used to preemptively set up the port layer's context
 *        in order to reduce the overhead of the *_compute() functions.
 *
 * @note  The internal context is expected to be heap allocated.
 *
 * @param ctx_internal This will hold the port layer's heap-allocated context.
 * @param cipher_type  This will hold the cipher type - for AES this
 *                     is the mode of operation.
 * @param key_size     The size of the key for the next benchmark, in bits.
 *
 * @return CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_aes_opaque_init(void **ctx_internal, cb_cipher_type_t cipher_type, size_t key_size);

/**
 * @brief Deinitialize the port layer internal context for opaque AES.
 *
 *        Free the heap-allocated internal context and carry out other
 *        port-specific deinitialization steps.
 *
 * @param ctx_internal The internal context to be free()'d.
 *
 * @retval CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_aes_opaque_deinit(void *ctx_internal);

/**
 * @brief The benchmarked function call for AES encryption.
 *
 * @param ctx_internal The port layer internal context.
 * @param message      The input message to encrypt.
 * @param message_size The size of the message.
 * @param iv           The initialization vector.
 * @param ciphertext   The output buffer for the ciphertext.
 * @param key_id       The key ID of the opaque key to be used.
 *
 * @retval CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_aes_opaque_compute(void *ctx_internal,
                                       const uint8_t *message,
                                       size_t message_size,
                                       const uint8_t *iv,
                                       uint8_t *ciphertext,
                                       uint32_t key_id);

/**
 * @brief Setup called before each round of benchmarked AES encryption.
 *
 * @param ctx_internal The port layer internal context.
 *
 * @retval CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_aes_opaque_setup(void *ctx_internal);

/**
 * @brief Cleanup called after each round of benchmarked AES encryption.
 *
 * @param ctx_internal The port layer internal context.
 *
 * @retval CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_aes_opaque_cleanup(void *ctx_internal);

/**
 * @brief A call to this function is made before each each round of benchmarked
 *        AES encryption to generate a new valid opaque key.
 *
 * @param ctx_internal The port layer internal context.
 * @param key_size     The size of the generated key in bits.
 * @param key_id       Returns the key ID of the generated opaque key.
 *
 * @retval CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_aes_opaque_key_generate(void *ctx_internal, size_t key_size, uint32_t *key_id);

/**
 * @brief Delete an opaque key used for benchmarked AES encryption
 *        at the end of the benchmarking round.
 *
 * @param ctx_internal The port layer internal context.
 * @param key_id       The key ID of the opaque key.
 *
 * @retval CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_aes_opaque_key_delete(void *ctx_internal, uint32_t key_id);

#endif /* CB_AES_H */
