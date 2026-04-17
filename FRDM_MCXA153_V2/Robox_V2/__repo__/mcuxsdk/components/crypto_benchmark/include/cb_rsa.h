/*
 * Copyright 2024-2025 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/**
 * @file  cb_rsa.h
 * @brief Contains definitions for RSA benchmarking.
 */

#ifndef CB_RSA_H
#define CB_RSA_H

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
#define CB_RSA_BENCHMARK_KEYGEN_ITERS (10u)

/**
 * @brief Input message size for the individual benchmarks.
 */
#define CRYPTO_BENCHMARK_RSA_MSG_SIZE (32u)

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
cb_status_t rsa_benchmark(void);
cb_status_t rsa_benchmark_opaque(void);

void print_rsa_case_name(cb_rsa_type_t rsa_type, cb_hash_t hash, size_t key_size);

/*******************************************************************************
 * Variables
 ******************************************************************************/

/* Array initializer list of RSA cipher benchmarking cases of type
 * rsa_benchmark_cases_t.
 */
extern const rsa_benchmark_cases_t rsa_cipher_benchmark_cases[];
extern const size_t rsa_cipher_benchmark_cases_nb;

/* Array initializer list of RSA signature benchmarking cases of type
 * rsa_benchmark_cases_t.
 */
extern const rsa_benchmark_cases_t rsa_signature_benchmark_cases[];
extern const size_t rsa_signature_benchmark_cases_nb;

/* Array initializer list of RSA key genration benchmarking cases of type
 * rsa_keygen_benchmark_cases_t.
 */
extern const rsa_keygen_benchmark_cases_t rsa_keygen_benchmark_cases[];
extern const size_t rsa_keygen_benchmark_cases_nb;

/*******************************************************************************
 * Generic wrappers
 ******************************************************************************/

/**
 * @brief A simple wrapper for RSA key generation. Does not take the port context as an argument.
 *
 * @param modulus                   Output buffer for the modulus part of the key.
 * @param modulus_buf_size          The buffer size for the modulus.
 * @param private_exponent          Output buffer for the private exponent part of the key.
 * @param private_exponent_buf_size The buffer size for the private exponent.
 * @param public_exponent           The chosen public exponent as input for the generaion process. By default 65,537.
 * @param public_exponent_buf_size  The size of the public exponent input buffer. For 65,537 it is 3 Bytes.
 * @param key_size                  The key size in bits.
 *
 * @return CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_rsa_keygen_compute(uint8_t *modulus,
                                       size_t modulus_buf_size,
                                       uint8_t *private_exponent,
                                       size_t private_exponent_buf_size,
                                       const uint8_t *public_exponent,
                                       size_t public_exponent_buf_size,
                                       size_t key_size);

/**
 * @brief Initialize the port layer internal context for RSA.
 *
 *        This is called before every benchmarked call (the *_compute()
 *        functions). Used to preemptively set up the port layer's context
 *        in order to reduce the overhead of the *_compute() functions.
 *
 * @note  The internal context is expected to be heap allocated.
 *
 * @param ctx_internal           This will hold the port layer's heap-allocated context.
 * @param rsa_type               Type of the RSA scheme.
 * @param hash_type              Type of hash function to be used.
 * @param key_size               Size of the key in bits.
 * @param public_key_buffer_size The maximum buffer size that is provided for the public key.
 * @param input_type             Type of the input - message or hash. Used only for signature RSA schemes.
 *
 * @return CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_rsa_init(
    void **ctx_internal, cb_rsa_type_t rsa_type, cb_hash_t hash_type, size_t key_size, size_t public_key_buffer_size, cb_asym_sigver_input_type_t input_type);

/**
 * @brief Deinitialize the port layer internal context for RSA.
 *
 *        Free the heap-allocated internal context and carry out other
 *        port-specific deinitialization steps.
 *
 * @param ctx_internal The internal context to be free()'d.
 *
 * @retval CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_rsa_deinit(void *ctx_internal);

/**
 * @brief Benchmarked function for RSA signature generation.
 *
 * @param ctx_internal     The port layer internal context.
 * @param modulus          The modulus part of the key.
 * @param private_exponent The private exponent part of the key.
 * @param message          The message to be signed.
 * @param message_size     The size of the message to be signed. By default always \c CRYPTO_BENCHMARK_RSA_MSG_SIZE.
 * @param signature        The output buffer for the generated signature.
 *
 * @retval CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_rsa_sign_compute(void *ctx_internal,
                                     const uint8_t *modulus,
                                     const uint8_t *private_exponent,
                                     uint8_t *message,
                                     size_t message_size,
                                     uint8_t *signature);

/**
 * @brief Benchmarked function for RSA signature verification.
 *
 * @param ctx_internal    The port layer internal context.
 * @param modulus         The modulus part of the key.
 * @param public_exponent The public exponent part of the key.
 * @param message         The message from which the signature was generated.
 * @param message_size    The size of the message to be verified. By default always \c CRYPTO_BENCHMARK_RSA_MSG_SIZE.
 * @param signature       The signature generated from \c message . The size can be inferred from key size.
 *
 * @retval CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_rsa_verify_compute(void *ctx_internal,
                                       const uint8_t *modulus,
                                       const uint8_t *public_exponent,
                                       uint8_t *message,
                                       size_t message_size,
                                       uint8_t *signature);

/**
 * @brief Benchmarked function for RSA ecryption.
 *
 * @param ctx_internal    The port layer internal context.
 * @param modulus         The modulus part of the key.
 * @param public_exponent The public exponent part of the key.
 * @param message         The message to encrypt.
 * @param message_size    The size of the message to be encrypted. By default always \c CRYPTO_BENCHMARK_RSA_MSG_SIZE.
 * @param ciphertext      The output buffer for the generated ciphertext.
 *
 * @retval CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_rsa_encrypt_compute(void *ctx_internal,
                                        const uint8_t *modulus,
                                        const uint8_t *public_exponent,
                                        uint8_t *message,
                                        size_t message_size,
                                        uint8_t *ciphertext);

/**
 * @brief Benchmarked function for RSA decryption.
 *
 * @param ctx_internal     The port layer internal context.
 * @param modulus          The modulus part of the key.
 * @param private_exponent The private exponent part of the key.
 * @param message          The output buffer for the decrypted message.
 * @param message_size     The size of the message after decryption. By default always \c CRYPTO_BENCHMARK_RSA_MSG_SIZE.
 * @param ciphertext       The ciphertext to be decrypted.
 *
 * @retval CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_rsa_decrypt_compute(void *ctx_internal,
                                        const uint8_t *modulus,
                                        const uint8_t *private_exponent,
                                        uint8_t *message,
                                        size_t message_size,
                                        uint8_t *ciphertext);

/*******************************************************************************
 * Opaque wrappers
 ******************************************************************************/

/**
 * @brief Benchmarked function for opaque RSA key generation.
 *
 * @param key_size            The key size to be used, in bits.
 * @param key_id              Returns the key ID of the generated opaque key.
 * @param public_key_addr     Output buffer for the public key. This buffer is
 *                            large enough for a full public key,
 *                            where the modulus and public exponent are placed
 *                            next to each other in memory.
 * @param public_key_buf_size The buffer size of the public key buffer.
 *
 * @retval CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_rsa_keygen_opaque_compute(size_t key_size,
                                              uint32_t *key_id,
                                              uint8_t *public_key_addr,
                                              size_t public_key_buf_size);

/**
 * @brief Setup called before each benchmarked key generation function.
 *        Can be used by port layer to prepare opaque services.
 *
 * @retval CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_rsa_keygen_opaque_setup(void);

/**
 * @brief Cleanup called after each benchmarked key generation function.
 *        Can be used by port layer to clean up opaque services.
 *
 * @retval CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_rsa_keygen_opaque_cleanup(void);

/**
 * @brief Opaque key deletion called after each benchmarked key generation
 *        function. Can be used by port layer to keep key stores empty.
 *
 * @param key_id The key ID of the opaque key.
 *
 * @retval CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_rsa_keygen_opaque_key_delete(uint32_t key_id);

/**
 * @brief Initialize the port layer internal context for opaque RSA.
 *
 *        This is called before every benchmarked call (the *_compute()
 *        functions). Used to preemptively set up the port layer's context
 *        in order to reduce the overhead of the *_compute() functions.
 *
 * @note  The internal context is expected to be heap allocated.
 *
 * @param ctx_internal This will hold the port layer's heap-allocated context.
 * @param rsa_type     Type of the RSA scheme.
 * @param hash_type    Type of hash function to be used.
 * @param key_size     Size of the key in bits.
 * @param input_type   Type of the input - message or hash. Used only for signature RSA schemes.
 *
 * @return CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_rsa_opaque_init(void **ctx_internal, cb_rsa_type_t rsa_type, cb_hash_t hash_type, size_t key_size, cb_asym_sigver_input_type_t input_type);

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
cb_status_t wrapper_rsa_opaque_deinit(void *ctx_internal);

/**
 * @brief Setup called before each round of benchmarked RSA signature generation.
 *
 * @param ctx_internal The port layer internal context.
 *
 * @retval CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_rsa_sign_opaque_setup(void *ctx_internal);

/**
 * @brief Cleanup called after each round of benchmarked RSA signature generation.
 *
 * @param ctx_internal The port layer internal context.
 *
 * @retval CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_rsa_sign_opaque_cleanup(void *ctx_internal);

/**
 * @brief A call to this function is made before each each round of benchmarked
 *        RSA signature generation to generate a new valid opaque key.
 *
 * @param ctx_internal        The port layer internal context.
 * @param key_id              Returns the key ID of the generated opaque key.
 * @param key_size            The size of the generated key in bits.
 * @param public_key_addr     Output buffer for the public key. This buffer is
 *                            large enough for a full public key,
 *                            where the modulus and public exponent are placed
 *                            next to each other in memory.
 * @param public_key_size     Returns the size of the public key in bits. Used by \c wrapper_rsa_verify_opaque_compute()
 * .
 * @param public_key_buf_size The buffer size of \c public_key_addr in Bytes. By default 2,048 Bytes.
 *
 * @retval CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_rsa_sign_opaque_key_generate(void *ctx_internal,
                                                 uint32_t *key_id,
                                                 size_t key_size,
                                                 uint8_t *public_key_addr,
                                                 size_t *public_key_size,
                                                 size_t public_key_buf_size);

/**
 * @brief Delete an opaque key used for benchmarked RSA signature generation
 *        at the end of the benchmarking round.
 *
 * @param ctx_internal The port layer internal context.
 * @param key_id       The key ID of the opaque key.
 *
 * @retval CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_rsa_sign_opaque_key_delete(void *ctx_internal, uint32_t key_id);

/**
 * @brief The benchmarked function call for RSA signature generation.
 *
 * @param ctx_internal The port layer internal context.
 * @param message      The message to be signed.
 * @param message_size The size of the message to be signed. By default always \c CRYPTO_BENCHMARK_RSA_MSG_SIZE.
 * @param signature    The output buffer for the generated signature.
 * @param key_id       The key ID of the opaque key.
 *
 * @retval CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_rsa_sign_opaque_compute(
    void *ctx_internal, const uint8_t *message, size_t message_size, uint8_t *signature, uint32_t key_id);

/**
 * @brief The benchmarked function call for RSA signature verification.
 *
 * @param ctx_internal    The port layer internal context.
 * @param message         The message from which the signature was generated.
 * @param message_size    The size of the message to be verified. By default always \c CRYPTO_BENCHMARK_RSA_MSG_SIZE.
 * @param signature       The signature generated from \c message . The size can be inferred from key size.
 * @param public_key      Buffer containing the public key generated via \c wrapper_rsa_sign_opaque_key_generate() .
 * @param public_key_size The public key size as returned from \c wrapper_rsa_sign_opaque_key_generate() .
 *
 * @retval CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_rsa_verify_opaque_compute(void *ctx_internal,
                                              const uint8_t *message,
                                              size_t message_size,
                                              uint8_t *signature,
                                              uint8_t *public_key,
                                              size_t public_key_size);

/**
 * @brief Setup called before each round of benchmarked RSA encryption.
 *
 * @param ctx_internal The port layer internal context.
 *
 * @retval CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_rsa_encrypt_opaque_setup(void);

/**
 * @brief Cleanup called after each round of benchmarked RSA encryption.
 *
 * @param ctx_internal The port layer internal context.
 *
 * @retval CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_rsa_encrypt_opaque_cleanup(void *ctx_internal);

/**
 * @brief A call to this function is made before each each round of benchmarked
 *        RSA encryption to generate a new valid opaque key.
 *
 * @param ctx_internal        The port layer internal context.
 * @param key_id              Returns the key ID of the generated opaque key.
 * @param key_size            The size of the generated key in bits.
 * @param public_key_addr     Output buffer for the public key. This buffer is
 *                            large enough for a full public key,
 *                            where the modulus and public exponent are placed
 *                            next to each other in memory.
 * @param public_key_size     Returns the size of the public key in bits. Used by \c
 * wrapper_rsa_encrypt_opaque_compute() .
 * @param public_key_buf_size The buffer size of \c public_key_addr in Bytes. By default 2,048 Bytes.
 *
 * @retval CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_rsa_encrypt_opaque_key_generate(void *ctx_internal,
                                                    uint32_t *key_id,
                                                    size_t key_size,
                                                    uint8_t *public_key_addr,
                                                    size_t *public_key_size,
                                                    size_t public_key_buf_size);

/**
 * @brief Delete an opaque key used for benchmarked RSA encryption
 *        at the end of the benchmarking round.
 *
 * @param ctx_internal The port layer internal context.
 * @param key_id       The key ID of the opaque key.
 *
 * @retval CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_rsa_encrypt_opaque_key_delete(void *ctx_internal, uint32_t key_id);

/**
 * @brief The benchmarked function call for RSA encryption.
 *
 * @param ctx_internal    The port layer internal context.
 * @param message         The message to be encrypted.
 * @param message_size    The size of the message to be encrypted. By default always \c CRYPTO_BENCHMARK_RSA_MSG_SIZE.
 * @param ciphertext      The output buffer for the generated ciphertext.
 * @param public_key      Buffer containing the public key generated via \c wrapper_rsa_encrypt_opaque_key_generate() .
 * @param public_key_size The public key size as returned from \c wrapper_rsa_encrypt_opaque_key_generate() .
 *
 * @retval CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_rsa_encrypt_opaque_compute(void *ctx_internal,
                                               const uint8_t *message,
                                               size_t message_size,
                                               uint8_t *ciphertext,
                                               uint8_t *public_key,
                                               size_t public_key_size);

/**
 * @brief The benchmarked function call for RSA decryption.
 *
 * @param ctx_internal    The port layer internal context.
 * @param message         The output buffer for the decrypted message.
 * @param message_size    The size of the message after decryption. By default always \c CRYPTO_BENCHMARK_RSA_MSG_SIZE.
 * @param ciphertext      The ciphertext to be decrypted.
 * @param key_id          The key ID of the opaque key.
 *
 * @retval CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_rsa_decrypt_opaque_compute(
    void *ctx_internal, const uint8_t *message, size_t message_size, uint8_t *ciphertext, uint32_t key_id);

#endif /* CB_RSA_H */
