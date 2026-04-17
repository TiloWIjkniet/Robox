/*
 * Copyright 2024-2025 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/**
 * @file  cb_helpers.h
 * @brief Declarations of helper functions and defines for benchmarking.
 */
#include <stddef.h>
#include <stdio.h>
#include "cb_types.h"
#include "cb_mac.h"
#include "cb_hash.h"

#define __weak __attribute__((weak))

#if !defined ARRAY_SIZE
#define ARRAY_SIZE(_array) sizeof(_array)/sizeof(_array[0])
#endif /* ARRAY_SIZE */

/* MODE */
#define CB_MODE_ENCRYPT_STR      "encrypt"
#define CB_MODE_DECRYPT_STR      "decrypt"
#define CB_MODE_KEYGEN_STR       "keygen"
#define CB_MODE_GENERATE_STR     "generate"
#define CB_MODE_VERIFY_STR       "verify"
#define CB_MODE_SIGN_STR         "sign"
#define CB_MODE_VERIFY_HASH_STR  "verify_hash"
#define CB_MODE_SIGN_HASH_STR    "sign_hash"
#define CB_MODE_VERIFY_MSG_STR   "verify_message"
#define CB_MODE_SIGN_MSG_STR     "sign_message"
/* KEY OPACITY */
#define CB_KEY_OPACITY_TRANSPARENT_STR "transparent"
#define CB_KEY_OPACITY_OPAQUE_STR      "opaque"
/* BOOLEAN */
#define CB_TRUE_STR  "TRUE"
#define CB_FALSE_STR "FALSE"
/* PERFORMANCE UNIT */
#define CB_PERF_UNIT_KIBPS   "KiB/s"
#define CB_PERF_UNIT_CPB     "cycle/byte"
#define CB_PERF_UNIT_SIGPS   "sig/s"
#define CB_PERF_UNIT_VERIFPS "ver/s"
#define CB_PERF_UNIT_ENCPS   "enc/s"
#define CB_PERF_UNIT_DECPS   "dec/s"
#define CB_PERF_UNIT_SEC     "s"
#define CB_PERF_UNIT_MSEC    "ms"
#define CB_PERF_UNIT_USEC    "us"
#define CB_PERF_UNIT_CYCLES  "cycles"

void printf_csv_header();

void print_benchmark_case(size_t payload_size,
                          char *is_symmetric,
                          char *algorithm,
                          char *algorithm_specifier,
                          char *mode,
                          char *key_opacity,
                          size_t key_size,
                          char *multipart,
                          double perf_value[],
                          char *perf_unit[],
                          size_t perf_data_cnt);
cb_status_t convert_hash_type_to_block_size(cb_hash_t hash, size_t *block_size, size_t *payload_size);

cb_status_t convert_mac_type_to_block_size(cb_mac_algorithm_t algorithm,
                                           cb_hash_t hash,
                                           size_t *block_size,
                                           size_t *payload_size);

char *rsa_type_to_string(cb_rsa_type_t rsa_type);

char *rsa_type_with_hash_to_string(cb_rsa_type_t rsa_type,
                                   cb_hash_t hash,
                                   char *rsa_specifier,
                                   size_t rsa_specifier_size);

char *hash_to_string(cb_hash_t hash);

char *aead_to_string(cb_aead_type_t aead_type);

char *cipher_to_string(cb_cipher_type_t cipher_type);

char *ecdsa_type_to_string(cb_ecdsa_type_t ecdsa_type);

char *ecc_family_to_string(cb_ecc_family_t ecc_family);

char *mac_to_string(cb_mac_algorithm_t mac);

char *mac_algo_specifier_to_string(cb_mac_algorithm_t mac, cb_hash_t hash);

void print_rsa_case_name(cb_rsa_type_t rsa_type, cb_hash_t hash, size_t key_size);

void print_aead_case_name(cb_aead_type_t aead_type, size_t key_size, size_t block_count);

void print_aes_case_name(cb_cipher_type_t cipher_type, size_t key_size, size_t block_count);

void print_ecdsa_case_name(cb_ecc_family_t ecc_family, cb_ecdsa_type_t ecdsa_type, size_t key_size);

void print_mac_case_name(cb_mac_algorithm_t mac, cb_hash_t hash, size_t key_size, size_t block_count);

void print_hash_case_name(cb_hash_t hash, size_t block_count);

void print_failure(const char *additional_message);
