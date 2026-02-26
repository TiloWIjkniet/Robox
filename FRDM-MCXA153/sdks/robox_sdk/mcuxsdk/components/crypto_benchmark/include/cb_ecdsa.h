/*
 * Copyright 2024-2025 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/**
 * @file  cb_ecdsa.h
 * @brief Contains definitions for ECDSA benchmarking.
 */

#ifndef CB_ECDSA_H
#define CB_ECDSA_H

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "crypto_benchmark.h"
#include "cb_types.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/**
 * @brief How many runs should be averaged when benchmarking key generation.
 *
 *        Both opaque and transparent key generation is timed this many times
 *        before averaging the benchmarked timings for a single result value.
 */
#define CB_ECDSA_BENCHMARK_KEYGEN_ITERS (10u)

/**
 * @brief The message size to be signed / verified.
 */
#define CRYPTO_BENCHMARK_ECDSA_MSG_SIZE (32u)

/*******************************************************************************
 * Variables
 ******************************************************************************/

/* Array initializer list of AES benchmarking cases of type
 * ecdsa_benchmark_cases_t.
 */
extern const ecdsa_benchmark_cases_t ecdsa_benchmark_cases[];
extern const size_t ecdsa_benchmark_cases_nb;

/* Array initializer list of ECDSA key genration benchmarking cases of type
 * ecdsa_keygen_benchmark_cases_t.
 */
extern const ecdsa_keygen_benchmark_cases_t ecdsa_keygen_benchmark_cases[];
extern const size_t ecdsa_keygen_benchmark_cases_nb;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
cb_status_t ecdsa_benchmark(void);
cb_status_t ecdsa_benchmark_opaque(void);

void print_ecdsa_case_name(cb_ecc_family_t ecc_family, cb_ecdsa_type_t ecdsa_type, size_t key_size);

/*******************************************************************************
 * Generic wrappers
 ******************************************************************************/
/**
 * @brief Initialize the port layer internal context for transparent ECDSA.
 *
 *        This is called before every benchmarked call (the *_compute()
 *        functions). Used to preemptively set up the port layer's context
 *        in order to reduce the overhead of the *_compute() functions.
 *
 * @note  The internal context is expected to be heap allocated.
 *
 * @param ctx_internal This will hold the port layer's heap-allocated context.
 * @param ecdsa_type   Type of ECDSA to be used.
 * @param hash_type    Type of hash to be used.
 * @param family       ECC family to be used.
 * @param key_size     Size of the key in bits.
 * @param input_type   Type of the input - message or hash.
 *
 * @return CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_ecdsa_init(void **ctx_internal,
                               cb_ecdsa_type_t ecdsa_type,
                               cb_hash_t hash_type,
                               cb_ecc_family_t family,
                               size_t key_size,
                               cb_asym_sigver_input_type_t input_type);

/**
 * @brief Deinitialize the port layer internal context for transparent ECDSA.
 *
 *        Free the heap-allocated internal context and carry out other
 *        port-specific deinitialization steps.
 *
 * @param ctx_internal The internal context to be free()'d.
 *
 * @retval CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_ecdsa_deinit(void *ctx_internal);

/**
 * @brief Benchmarked function for transparent ECDSA key generation.
 *
 * @param ctx_internal         The port layer internal context.
 * @param private_key          Output buffer for the private key.
 * @param private_key_buf_size Buffer size of the private key buffer.
 * @param public_key           Output buffer for the public key.
 * @param public_key_buf_size  Buffer size of the public key buffer.
 * @param key_size             Size of the key to be generated.
 *
 * @retval CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_ecdsa_keygen_compute(void *ctx_internal,
                                         uint8_t *private_key,
                                         size_t private_key_buf_size,
                                         uint8_t *public_key,
                                         size_t public_key_buf_size,
                                         size_t key_size);

/**
 * @brief The benchmarked function call for ECDSA signature generation.
 *
 * @param ctx_internal The port layer internal context.
 * @param private_key  Private key to be signed with.
 * @param message      The input message to be signed.
 * @param message_size The size of the message to be signed.
 * @param signature    The output buffer for the signature.
 *
 * @retval CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_ecdsa_sign_compute(void *ctx_internal,
                                       const uint8_t *private_key,
                                       uint8_t *message,
                                       size_t message_size,
                                       uint8_t *signature);

/**
 * @brief The benchmarked function call for ECDSA signature verification.
 *
 * @param ctx_internal    The port layer internal context.
 * @param public_key      The public key for the verification operation.
 * @param message         The input message for the signature to be verified against.
 * @param message_size    The size of the message to be verified against.
 * @param signature       The signature to be verified.
 *
 * @retval CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_ecdsa_verify_compute(void *ctx_internal,
                                         const uint8_t *public_key,
                                         uint8_t *message,
                                         size_t message_size,
                                         uint8_t *signature);

/*******************************************************************************
 * Opaque wrappers
 ******************************************************************************/
/**
 * @brief Initialize the port layer internal context for opaque ECDSA.
 *
 *        This is called before every benchmarked call (the *_opaque_compute()
 *        functions). Used to preemptively set up the port layer's context
 *        in order to reduce the overhead of the *_opaque_compute() functions.
 *
 * @note  The internal context is expected to be heap allocated.
 *
 * @param ctx_internal This will hold the port layer's heap-allocated context.
 * @param ecdsa_type   Type of ECDSA to be used.
 * @param hash_type    Type of hash to be used.
 * @param family       ECC family to be used.
 * @param key_size     Size of the key in bits.
 * @param input_type   Type of the input - message or hash.
 *
 * @return CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_ecdsa_opaque_init(void **ctx_internal,
                                   cb_ecdsa_type_t ecdsa_type,
                                   cb_hash_t hash_type,
                                   cb_ecc_family_t family,
                                   size_t key_size,
                                   cb_asym_sigver_input_type_t input_type);

/**
 * @brief Deinitialize the port layer internal context for opaque ECDSA.
 *
 *        Free the heap-allocated internal context and carry out other
 *        port-specific deinitialization steps.
 *
 * @param ctx_internal The internal context to be free()'d.
 *
 * @retval CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_ecdsa_opaque_deinit(void *ctx_internal);

/**
 * @brief Benchmarked function for opaque ECDSA key generation.
 *
 * @param ctx_internal        The port layer internal context.
 * @param key_size            The key size to be used.
 * @param key_id              Returns the key ID of the generated opaque key.
 * @param public_key_addr     Pointer to the public key output address.
 * @param public_key_buf_size The buffer size of the public key buffer.
 *
 * @retval CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_ecdsa_keygen_opaque_compute(void *ctx_internal,
                                             size_t key_size,
                                             uint32_t *key_id,
                                             uint8_t *public_key_addr,
                                             size_t public_key_buf_size);

/**
 * @brief Setup called before each benchmarked key generation function.
 *        Can be used by port layer to prepare opaque services.
 *
 * @retval CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_ecdsa_keygen_opaque_setup(void);

/**
 * @brief Cleanup called after each benchmarked key generation function.
 *        Can be used by port layer to clean up opaque services.
 *
 * @retval CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_ecdsa_keygen_opaque_cleanup(void);

/**
 * @brief Opaque key deletion called after each benchmarked key generation
 *        function. Can be used by port layer to keep key stores empty.
 *
 * @param key_id The key ID of the opaque key.
 *
 * @retval CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_ecdsa_keygen_opaque_key_delete(uint32_t key_id);

/**
 * @brief Setup called before each benchmarked signing and verification.
 *
 * @note  The wrapper_ecdsa_verify_opaque_compute() function also depends on
 *        this function to set up opaque verification services.
 *
 * @retval CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_ecdsa_sign_opaque_setup(void *ctx_internal);

/**
 * @brief Cleanup called after each benchmarked signing and verification.
 *
 * @param ctx_internal The port layer internal context.
 *
 * @retval CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_ecdsa_sign_opaque_cleanup(void *ctx_internal);

/**
 * @brief A call to this function is made before each signing / verification
 *        benchmarking pair to generate a new valid opaque key.
 *
 * @param ctx_internal        The port layer internal context.
 * @param key_id              Returns the key ID of the generated opaque key.
 * @param key_size            The size of the generated key.
 * @param public_key_addr     Output address for the public key buffer.
 * @param public_key_size     Output value for the size of the generated public key.
 * @param public_key_buf_size The buffer size of the public key buffer.
 *
 * @retval CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_ecdsa_sign_opaque_key_generate(void *ctx_internal,
                                                uint32_t *key_id,
                                                size_t key_size,
                                                uint8_t *public_key_addr,
                                                size_t *public_key_size,
                                                size_t public_key_buf_size);

/**
 * @brief Delete an opaque key used for signing / verification after benchmarking.
 *
 * @param ctx_internal The port layer internal context.
 * @param key_id       The key ID of the opaque key.
 *
 * @retval CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_ecdsa_sign_opaque_key_delete(void *ctx_internal, uint32_t key_id);

/**
 * @brief The benchmarked function call for ECDSA signature generation.
 *
 * @param ctx_internal The port layer internal context.
 * @param message      The input message to be signed.
 * @param message_size The size of the message to be signed.
 * @param signature    The output buffer for the signature.
 * @param key_id       The key ID of the opaque key to be used.
 *
 * @retval CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_ecdsa_sign_opaque_compute(void *ctx_internal, const uint8_t *message, size_t message_size, uint8_t *signature, uint32_t key_id);

/**
 * @brief The benchmarked function call for ECDSA signature verification.
 *
 * @param ctx_internal    The port layer internal context.
 * @param message         The input message for the signature to be verified against.
 * @param message_size    The size of the message to be verified against.
 * @param signature       The signature to be verified.
 * @param public_key      The public key for the verification operation.
 * @param public_key_size The size of the public key.
 *
 * @retval CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_ecdsa_verify_opaque_compute(void *ctx_internal, const uint8_t *message, size_t message_size, uint8_t *signature, uint8_t *public_key, size_t public_key_size);

#endif /* CB_ECDSA_H */
