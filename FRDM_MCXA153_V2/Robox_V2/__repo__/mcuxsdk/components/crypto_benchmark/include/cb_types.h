/*
 * Copyright 2024-2025 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/**
 * @file  cb_types.h
 * @brief Type definitions for the Crypto Benchmark utility.
 *
 *        The types in this file aim to mimic PSA values while not necessarily
 *        defining the entire PSA value set.
 */

#ifndef CB_TYPES_H
#define CB_TYPES_H

#include <stddef.h>
#include <stdint.h>

typedef enum {
    CB_STATUS_SUCCESS = 0,
    CB_STATUS_FAIL = 1
} cb_status_t;

/*******************************************************************************
 * GENERIC TYPES
 ******************************************************************************/
/**
 * @brief Used in asymmetric testcase definitions to distinguish if we
 *        are using the benchmarked calls to sign/verify hashes or messages.
 */
typedef enum _cb_asym_sigver_input_type_t
{
    CB_SIGVER_HASH    = 0u,
    CB_SIGVER_MESSAGE = 1u,
} cb_asym_sigver_input_type_t;

/*******************************************************************************
 * CIPHER
 ******************************************************************************/
typedef enum _cb_cipher_type_t
{
    CB_CIPHER_TYPE_STREAM            = 0x01u,
    CB_CIPHER_TYPE_CTR               = 0x10u,
    CB_CIPHER_TYPE_CFB               = 0x11u,
    CB_CIPHER_TYPE_OFB               = 0x12u,
    CB_CIPHER_TYPE_XTS               = 0xFFu,
    CB_CIPHER_TYPE_CBC_NO_PADDING    = 0x40u,
    CB_CIPHER_TYPE_CBC_PKCS7_PADDING = 0x41u,
    CB_CIPHER_TYPE_ECB_NO_PADDING    = 0x44u,
} cb_cipher_type_t;

/**
 * @brief This enum defines values for various block ciphers based on the
 *        PSA key type macros.
 */
typedef enum _cb_block_cipher_t
{
    CB_BLOCK_CIPHER_AES = 0x2400u,
    CB_BLOCK_CIPHER_DES = 0x2301u,
    CB_BLOCK_CIPHER_SM4 = 0x2405u,
} cb_block_cipher_t;

typedef struct _aes_benchmark_cases_t
{
    cb_cipher_type_t type; /*!< Type of cipher (for AES; mode of operation). */
    size_t key_size;       /*!< Key size in bits. */
} aes_benchmark_cases_t;

/*******************************************************************************
 * AEAD
 ******************************************************************************/
typedef enum _cb_aead_type_t
{
    CB_AEAD_TYPE_CCM               = 0x01u,
    CB_AEAD_TYPE_GCM               = 0x02u,
    CB_AEAD_TYPE_CHACHA20_POLY1305 = 0x05u,
} cb_aead_type_t;

typedef struct _aead_benchmark_cases_t
{
    cb_aead_type_t type; /*!< The type of AEAD. */
    size_t key_size;     /*!< Key size in bits. */
} aead_benchmark_cases_t;

/*******************************************************************************
 * HASH
 ******************************************************************************/
typedef enum _cb_hash_t
{
    CB_HASH_TYPE_NO_HASH  = 0x00u,
    CB_HASH_TYPE_SHA1     = 0x05u,
    CB_HASH_TYPE_SHA224   = 0x08u,
    CB_HASH_TYPE_SHA256   = 0x09u,
    CB_HASH_TYPE_SHA384   = 0x0Au,
    CB_HASH_TYPE_SHA512   = 0x0Bu,
    CB_HASH_TYPE_SHA3_224 = 0x10u,
    CB_HASH_TYPE_SHA3_256 = 0x11u,
    CB_HASH_TYPE_SHA3_384 = 0x12u,
    CB_HASH_TYPE_SHA3_512 = 0x13u,
    CB_HASH_TYPE_SHAKE256 = 0x14u,
    CB_HASH_TYPE_ANY_HASH = 0xFFu,
} cb_hash_t;

typedef struct _hash_benchmark_cases_t
{
    cb_hash_t hash; /*!< Hash to be used. */
} hash_benchmark_cases_t;

/*******************************************************************************
 * RSA
 ******************************************************************************/
typedef enum _cb_rsa_type_t
{
    CB_RSA_PKCS1V15_SIGN  = 0x06000200u,
    CB_RSA_PSS            = 0x06000300u,
    CB_RSA_PKCS1V15_CRYPT = 0x07000200u,
    CB_RSA_OAEP           = 0x07000300u,
    CB_RSA_NONE           = 0x0,
} cb_rsa_type_t;

/**
 @brief For defining RSA benchmarking cases.
 */
typedef struct _rsa_benchmark_cases_t
{
    cb_rsa_type_t type;                     /*!< Type of RSA to be used. */
    cb_hash_t hash;                         /*!< Hash to be used with the RSA type. */
    cb_asym_sigver_input_type_t input_type; /*!< Input is message or hash. */
    size_t key_size;                        /*!< Key size in bits. */
    uint8_t *modulus;                       /*!< Key modulus. */
    uint8_t *private_exponent;              /*!< Key private exponent. */
} rsa_benchmark_cases_t;

/**
 @brief For defining RSA key generation benchmarking cases.
 */
typedef struct _rsa_keygen_benchmark_cases_t
{
    size_t key_size; /*!< Key size in bits. */

} rsa_keygen_benchmark_cases_t;

/*******************************************************************************
 * ECDSA
 ******************************************************************************/
typedef enum _cb_ecdsa_type_t
{
    CB_ECDSA_RANDOMIZED    = 0x06000600u,
    CB_ECDSA_DETERMINISTIC = 0x06000700u,
} cb_ecdsa_type_t;

typedef enum _cb_ecc_family_t
{
    CB_ECC_FAMILY_SECP_K1         = 0x0Bu,
    CB_ECC_FAMILY_SECP_R1         = 0x09u,
    CB_ECC_FAMILY_SECP_R2         = 0x0Du,
    CB_ECC_FAMILY_SECT_K1         = 0x13u,
    CB_ECC_FAMILY_SECT_R1         = 0x11u,
    CB_ECC_FAMILY_SECT_R2         = 0x15u,
    CB_ECC_FAMILY_BRAINPOOL_P_R1  = 0x18u,
    CB_ECC_FAMILY_FRP             = 0x19u,
    CB_ECC_FAMILY_MONTGOMERY      = 0x20u,
    CB_ECC_FAMILY_TWISTED_EDWARDS = 0x21u,
} cb_ecc_family_t;

/**
 @brief For defining ECDSA benchmarking cases.
 */
typedef struct _ecdsa_benchmark_cases_t
{
    cb_ecc_family_t family;                 /*!< The Elliptic Curve family to use. */
    cb_ecdsa_type_t type;                   /*!< Type of ECDSA to be used. */
    cb_hash_t hash;                         /*!< Hash to be used with the ECDSA type. */
    cb_asym_sigver_input_type_t input_type; /*!< Input is message or hash. */
    size_t key_size;                        /*!< Key size in bits. */
} ecdsa_benchmark_cases_t;

/**
 @brief For defining ECDSA key generation benchmarking cases.
 */
typedef struct _ecdsa_keygen_benchmark_cases_t
{
    cb_ecc_family_t family; /*!< The Elliptic Curve family to use. */
    cb_ecdsa_type_t type;   /*!< Type of ECDSA to be used. */
    cb_hash_t hash;         /*!< Hash to be used with the ECDSA type. */
    size_t key_size;        /*!< Key size in bits. */
} ecdsa_keygen_benchmark_cases_t;

/*******************************************************************************
 * MAC
 ******************************************************************************/
typedef enum _cb_mac_algorithm_t
{
    CB_MAC_ALGORITHM_HMAC    = 0x03800000u,
    CB_MAC_ALGORITHM_CBC_MAC = 0x03c00100u, /*!< @note Currently supports only AES */
    CB_MAC_ALGORITHM_CMAC    = 0x03c00200u, /*!< @note Currently supports only AES */
} cb_mac_algorithm_t;

typedef struct _mac_benchmark_cases_t
{
    cb_mac_algorithm_t algorithm; /*!< MAC algorithm to be used. */
    cb_hash_t hash;               /*!< Hash to be used if algorithm is HMAC. */
    size_t key_size;              /*!< Key size in bits. */
} mac_benchmark_cases_t;

/*******************************************************************************
 * CTR DRBG
 ******************************************************************************/
typedef struct _ctr_drbg_benchmark_cases_t
{
    size_t output_size; /*!< Ouput size in bytes. */
} ctr_drbg_benchmark_cases_t;

#endif /* CB_TYPES_H */
