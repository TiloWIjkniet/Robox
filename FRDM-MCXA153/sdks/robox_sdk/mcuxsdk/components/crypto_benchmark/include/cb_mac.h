/*
 * Copyright 2024-2025 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/**
 * @file  cb_mac.h
 * @brief Contains definitions and declarations for MAC benchmarking.
 */
#ifndef CB_MAC_H
#define CB_MAC_H

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "crypto_benchmark.h"
#include "cb_types.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define CMAC_AES_BLOCK         16u
#define CMAC_AES_PAYLOAD       16u
#define CMAC_AES_PAYLOAD_MULTI (CMAC_BLOCK * (MULTIPLE_BLOCK - 1) + CMAC_PAYLOAD)

/*******************************************************************************
 * Variables
 ******************************************************************************/

/* Array initializer list of opaque MAC benchmarking cases of type
 * mac_benchmark_cases_t.
 */
extern const mac_benchmark_cases_t mac_benchmark_cases[];
extern const size_t mac_benchmark_cases_nb;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
cb_status_t mac_benchmark(void);
cb_status_t mac_benchmark_opaque(void);

void print_mac_case_name(cb_mac_algorithm_t mac, cb_hash_t hash, size_t key_size, size_t block_count);

/**
 * @brief Helper function for getting the correct block size
 *        for a specific MAC type.
 *
 * @note  Currently for CMAC and CBC-MAC only AES is supported.
 *        For HMAC, only SHA1 and SHA2 is supported.
 *
 * @param algorithm    The MAC algorithm that is going to be benchmarked.
 * @param hash         The hash type in case \c algorithm is HMAC.
 * @param block_size   Output parameter for the resulting block size.
 * @param payload_size Output parameter for the resulting payload size,
 *                     which is the \c block_size minus the padding.
 *
 * @return CB_STATUS_SUCCESS on success, CB_STATUS_FAIL otherwise.
 */
cb_status_t convert_mac_type_to_block_size(cb_mac_algorithm_t algorithm,
                                           cb_hash_t hash,
                                           size_t *block_size,
                                           size_t *payload_size);

/*******************************************************************************
 * Generic wrappers
 ******************************************************************************/
/**
 * @brief Initialize the port layer internal context for transparent MAC.
 *
 *        This is called before every benchmarked call (the *_compute()
 *        functions). Used to preemptively set up the port layer's context
 *        in order to reduce the overhead of the *_compute() functions.
 *
 * @note  The internal context is expected to be heap allocated.
 *
 * @param ctx_internal This will hold the port layer's heap-allocated context.
 * @param algorithm    The MAC algorithm to be used.
 * @param hash_type    The hash type in case the \c algorithm is HMAC.
 * @param key          The encryption key for the next benchmark.
 * @param key_size     The size of the key in bits.
 *
 * @return CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_mac_init(void **ctx_internal,
                             cb_mac_algorithm_t algorithm,
                             cb_hash_t hash_type,
                             const uint8_t *key,
                             size_t key_size);

/**
 * @brief Deinitialize the port layer internal context for transparent MAC.
 *
 *        Free the heap-allocated internal context and carry out other
 *        port-specific deinitialization steps.
 *
 * @param ctx_internal The internal context to be free()'d.
 *
 * @retval CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_mac_deinit(void *ctx_internal);

/**
 * @brief Benchmarked function for transparent MAC generation.
 *
 * @param ctx_internal The port layer internal context.
 * @param message      The message to authenticate.
 * @param message_size The length of the message.
 * @param mac          The output MAC.
 * @param key          The input key with which to authenticate.
 *
 * @retval CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_mac_compute(
    void *ctx_internal, const uint8_t *message, size_t message_size, uint8_t *mac, const uint8_t *key);

/*******************************************************************************
 * Opaque wrappers
 ******************************************************************************/
/**
 * @brief Initialize the port layer internal context for opaque MAC.
 *
 *        This is called before every benchmarked call (the *_compute()
 *        functions). Used to preemptively set up the port layer's context
 *        in order to reduce the overhead of the *_compute() functions.
 *
 * @note  The internal context is expected to be heap allocated.
 *
 * @param ctx_internal This will hold the port layer's heap-allocated context.
 * @param algorithm    The MAC algorithm to be used.
 * @param hash_type    The hash type in case the \c algorithm is HMAC.
 * @param key_size     The size of the key in bits.
 *
 * @return CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_mac_opaque_init(void **ctx_internal,
                                    cb_mac_algorithm_t algorithm,
                                    cb_hash_t hash_type,
                                    size_t key_size);

/**
 * @brief Deinitialize the port layer internal context for opaque MAC.
 *
 *        Free the heap-allocated internal context and carry out other
 *        port-specific deinitialization steps.
 *
 * @param ctx_internal The internal context to be free()'d.
 *
 * @retval CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_mac_opaque_deinit(void *ctx_internal);

/**
 * @brief The benchmarked function call for MAC generation.
 *
 * @param ctx_internal The port layer internal context.
 * @param message      The input message to generate a MAC from.
 * @param message_size The size of the message.
 * @param mac          The output buffer for the MAC.
 * @param key_id       The key ID of the opaque key to be used.
 *
 * @retval CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_mac_opaque_compute(
    void *ctx_internal, const uint8_t *message, size_t message_size, uint8_t *mac, uint32_t key_id);

/**
 * @brief Setup called before each round of benchmarked MAC generation.
 *
 * @param ctx_internal The port layer internal context.
 *
 * @retval CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_mac_opaque_setup(void *ctx_internal);

/**
 * @brief Cleanup called after each round of benchmarked MAC generation.
 *
 * @param ctx_internal The port layer internal context.
 *
 * @retval CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_mac_opaque_cleanup(void *ctx_internal);

/**
 * @brief A call to this function is made before each each round of benchmarked
 *        MAC generation to generate a new valid opaque key.
 *
 * @param ctx_internal The port layer internal context.
 * @param key_size     The size of the generated key in bits.
 * @param key_id       Returns the key ID of the generated opaque key.
 *
 * @retval CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_mac_opaque_key_generate(void *ctx_internal, size_t key_size, uint32_t *key_id);

/**
 * @brief Delete an opaque key used for benchmarked MAC generation
 *        at the end of the benchmarking round.
 *
 * @param ctx_internal The port layer internal context.
 * @param key_id       The key ID of the opaque key.
 *
 * @retval CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_mac_opaque_key_delete(void *ctx_internal, uint32_t key_id);

/* These functions are specific to devices with the ELE FastMAC API */
#define CB_FAST_MAC_ADDITIONAL_PAYLOAD_SIZES \
    {                                        \
        32, 80, 1500                         \
    }

/**
 * @brief Start the FastMAC sevice.
 *
 * @param key The input key, which is always 256bit.
 *
 * @retval CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_fast_mac_start(const uint8_t *key);

/**
 * @brief Start the FastMAC sevice.
 *
 * @param message The input message.
 * @param message_size The input message size in bytes.
 * @param mac The output buffer for the generated MAC.
 *
 * @retval CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_fast_mac_proceed(const uint8_t *message, size_t message_size, uint8_t *mac);

/**
 * @brief Close the FastMAC sevice.
 *
 * @retval CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_fast_mac_end(void);

#endif /* CB_MAC_H */
