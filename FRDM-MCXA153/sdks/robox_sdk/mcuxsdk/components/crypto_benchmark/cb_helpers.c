/*
 * Copyright 2024-2025 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/**
 * @file  cb_helpers.c
 * @brief Helper functions for benchmarking.
 */

#include "cb_osal.h"
#include "cb_mac.h"
#include "cb_hash.h"

cb_status_t convert_hash_type_to_block_size(cb_hash_t hash, size_t *block_size, size_t *payload_size)
{
    cb_status_t status = CB_STATUS_SUCCESS;

    switch (hash)
    {
        case CB_HASH_TYPE_SHA1:
            *block_size   = SHA1_BLOCK;
            *payload_size = SHA1_PAYLOAD;
            break;
        case CB_HASH_TYPE_SHA224:
            *block_size   = SHA224_BLOCK;
            *payload_size = SHA224_PAYLOAD;
            break;
        case CB_HASH_TYPE_SHA256:
            *block_size   = SHA256_BLOCK;
            *payload_size = SHA256_PAYLOAD;
            break;
        case CB_HASH_TYPE_SHA384:
            *block_size   = SHA384_BLOCK;
            *payload_size = SHA384_PAYLOAD;
            break;
        case CB_HASH_TYPE_SHA512:
            *block_size   = SHA512_BLOCK;
            *payload_size = SHA512_PAYLOAD;
            break;
        case CB_HASH_TYPE_SHA3_224:
            *block_size   = SHA3_224_BLOCK;
            *payload_size = SHA3_224_PAYLOAD;
            break;
        case CB_HASH_TYPE_SHA3_256:
            *block_size   = SHA3_256_BLOCK;
            *payload_size = SHA3_256_PAYLOAD;
            break;
        case CB_HASH_TYPE_SHA3_384:
            *block_size   = SHA3_384_BLOCK;
            *payload_size = SHA3_384_PAYLOAD;
            break;
        case CB_HASH_TYPE_SHA3_512:
            *block_size   = SHA3_512_BLOCK;
            *payload_size = SHA3_512_PAYLOAD;
            break;
        default:
            *block_size   = 0u;
            *payload_size = 0u;
            status        = CB_STATUS_FAIL;
            break;
    }

    return status;
}

cb_status_t convert_mac_type_to_block_size(cb_mac_algorithm_t algorithm,
                                           cb_hash_t hash,
                                           size_t *block_size,
                                           size_t *payload_size)
{
    cb_status_t status = CB_STATUS_SUCCESS;

    if (CB_MAC_ALGORITHM_CMAC == algorithm || CB_MAC_ALGORITHM_CBC_MAC == algorithm)
    {
        /* Currently only AES supported, so use AES block size */
        *block_size   = CMAC_AES_BLOCK;
        *payload_size = CMAC_AES_PAYLOAD;
    }
    else
    {
        /* Else HMAC and we need to use the hash block size */
        status = convert_hash_type_to_block_size(hash, block_size, payload_size);
    }

    return status;
}

char *rsa_type_to_string(cb_rsa_type_t rsa_type)
{
    char *str = NULL;
    switch (rsa_type)
    {
        case CB_RSA_PKCS1V15_SIGN:
        case CB_RSA_PKCS1V15_CRYPT:
            str = "PKCSv1.5";
            break;
        case CB_RSA_PSS:
            str = "PSS";
            break;
        case CB_RSA_OAEP:
            str = "OAEP";
            break;
        default:
            str = "";
            break;
    }
    return str;
}

char *rsa_type_with_hash_to_string(cb_rsa_type_t rsa_type,
                                   cb_hash_t hash,
                                   char *rsa_specifier,
                                   size_t rsa_specifier_size)
{
    const size_t rsa_specifier_size_max = rsa_specifier_size - 1u;
    memset(rsa_specifier, 0, rsa_specifier_size);

    strncat(rsa_specifier, rsa_type_to_string(rsa_type), rsa_specifier_size_max);

    if (CB_HASH_TYPE_NO_HASH != hash)
    {
        strncat(rsa_specifier, "-", rsa_specifier_size_max - strlen(rsa_specifier));

        strncat(rsa_specifier, hash_to_string(hash), rsa_specifier_size_max - strlen(rsa_specifier));
    }

    return rsa_specifier;
}

char *hash_to_string(cb_hash_t hash)
{
    char *str = NULL;
    switch (hash)
    {
        case CB_HASH_TYPE_SHA1:
            str = "SHA1";
            break;
        case CB_HASH_TYPE_SHA224:
            str = "SHA224";
            break;
        case CB_HASH_TYPE_SHA256:
            str = "SHA256";
            break;
        case CB_HASH_TYPE_SHA384:
            str = "SHA384";
            break;
        case CB_HASH_TYPE_SHA512:
            str = "SHA512";
            break;
        case CB_HASH_TYPE_SHA3_224:
            str = "SHA3_224";
            break;
        case CB_HASH_TYPE_SHA3_256:
            str = "SHA3_256";
            break;
        case CB_HASH_TYPE_SHA3_384:
            str = "SHA3_384";
            break;
        case CB_HASH_TYPE_SHA3_512:
            str = "SHA3_512";
            break;
        case CB_HASH_TYPE_ANY_HASH:
            str = "ANY_HASH";
            break;
        default:
            str = "";
            break;
    }
    return str;
}

char *aead_to_string(cb_aead_type_t aead_type)
{
    char *str = NULL;
    switch (aead_type)
    {
        case CB_AEAD_TYPE_CCM:
            str = "CCM";
            break;
        case CB_AEAD_TYPE_GCM:
            str = "GCM";
            break;
        case CB_AEAD_TYPE_CHACHA20_POLY1305:
            str = "CHACHA20_POLY1305";
            break;
        default:
            str = "";
            break;
    }
    return str;
}

char *cipher_to_string(cb_cipher_type_t cipher_type)
{
    char *str = NULL;
    switch (cipher_type)
    {
        case CB_CIPHER_TYPE_STREAM:
            str = "STREAM";
            break;
        case CB_CIPHER_TYPE_CTR:
            str = "CTR";
            break;
        case CB_CIPHER_TYPE_CFB:
            str = "CFB";
            break;
        case CB_CIPHER_TYPE_OFB:
            str = "OFB";
            break;
        case CB_CIPHER_TYPE_XTS:
            str = "XTS";
            break;
        case CB_CIPHER_TYPE_CBC_NO_PADDING:
            str = "CBC_NO_PADDING";
            break;
        case CB_CIPHER_TYPE_CBC_PKCS7_PADDING:
            str = "CBC_PKCS7_PADDING";
            break;
        case CB_CIPHER_TYPE_ECB_NO_PADDING:
            str = "ECB_NO_PADDING";
            break;
        default:
            str = "";
            break;
    }
    return str;
}

char *ecdsa_type_to_string(cb_ecdsa_type_t ecdsa_type)
{
    char *str = NULL;
    switch (ecdsa_type)
    {
        case CB_ECDSA_RANDOMIZED:
            str = "RAND";
            break;
        case CB_ECDSA_DETERMINISTIC:
            str = "DETERM";
            break;
        default:
            str = "";
            break;
    }
    return str;
}

char *ecc_family_to_string(cb_ecc_family_t ecc_family)
{
    char *str = NULL;
    switch (ecc_family)
    {
        case CB_ECC_FAMILY_SECP_K1:
            str = "SECP-K1";
            break;
        case CB_ECC_FAMILY_SECP_R1:
            str = "SECP-R1";
            break;
        case CB_ECC_FAMILY_SECP_R2:
            str = "SECP-R2";
            break;
        case CB_ECC_FAMILY_SECT_K1:
            str = "SECT-K1";
            break;
        case CB_ECC_FAMILY_SECT_R1:
            str = "SECT-R1";
            break;
        case CB_ECC_FAMILY_SECT_R2:
            str = "SECT-R2";
            break;
        case CB_ECC_FAMILY_BRAINPOOL_P_R1:
            str = "BRAINPOOL-R1";
            break;
        case CB_ECC_FAMILY_FRP:
            str = "FRP";
            break;
        case CB_ECC_FAMILY_MONTGOMERY:
            str = "MONTGOMERY";
            break;
        case CB_ECC_FAMILY_TWISTED_EDWARDS:
            str = "TWISTED_EDWARDS";
            break;
        default:
            str = "";
            break;
    }
    return str;
}

char *mac_to_string(cb_mac_algorithm_t mac)
{
    char *str = NULL;
    switch (mac)
    {
        case CB_MAC_ALGORITHM_HMAC:
            str = "HMAC";
            break;
        case CB_MAC_ALGORITHM_CBC_MAC:
            str = "CBC_MAC";
            break;
        case CB_MAC_ALGORITHM_CMAC:
            str = "CMAC";
            break;
        default:
            str = "";
            break;
    }
    return str;
}

char *mac_algo_specifier_to_string(cb_mac_algorithm_t mac, cb_hash_t hash)
{
    char *str = (mac == CB_MAC_ALGORITHM_HMAC ? hash_to_string(hash) : "AES");
    return str;
}

void printf_csv_header()
{
    PRINTF(
        "payload_size,is_symmetric,algorithm,algorithm_specifier,mode,key_opacity,key_size,multipart,perf_value,perf_"
        "unit\n");
}

#if defined(CB_PRINT_CSV) && (CB_PRINT_CSV == 1u)
#define CB_PRINT_FORMAT_STR_MAIN                     ("%lu,%s,%s,%s,%s,%s,%u,%s,")
#define CB_PRINT_FORMAT_STR_PERF_VALUE_LOOP_PROLOGUE ("\"[")
#define CB_PRINT_FORMAT_STR_PERF_VALUE_LOOP_BODY     ("%.3f,")
#define CB_PRINT_FORMAT_STR_PERF_VALUE_LOOP_EPILOGUE ("%.3f]\",")
#define CB_PRINT_FORMAT_STR_PERF_UNIT_LOOP_PROLOGUE  ("\"[")
#define CB_PRINT_FORMAT_STR_PERF_UNIT_LOOP_BODY      ("%s,")
#define CB_PRINT_FORMAT_STR_PERF_UNIT_TAIL_EPILOGUE  ("%s]\"")
#else
#define CB_PRINT_FORMAT_STR_MAIN                     ("  %lu\t%s  %s  %s\t%s\t%s\t%u  %s\t")
#define CB_PRINT_FORMAT_STR_PERF_VALUE_LOOP_PROLOGUE ("[")
#define CB_PRINT_FORMAT_STR_PERF_VALUE_LOOP_BODY     ("%.3f, ")
#define CB_PRINT_FORMAT_STR_PERF_VALUE_LOOP_EPILOGUE ("%.3f]  ")
#define CB_PRINT_FORMAT_STR_PERF_UNIT_LOOP_PROLOGUE  ("[")
#define CB_PRINT_FORMAT_STR_PERF_UNIT_LOOP_BODY      ("%s, ")
#define CB_PRINT_FORMAT_STR_PERF_UNIT_TAIL_EPILOGUE  ("%s]")

static char *empty_str_to_na(char *str)
{
    return strlen(str) != 0 ? str : "n/a";
}
#endif

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
                          size_t perf_data_cnt)
{
    size_t i = 0u;

    PRINTF(CB_PRINT_FORMAT_STR_MAIN,
#if defined(CB_PRINT_CSV) && (CB_PRINT_CSV == 1u)
           payload_size, is_symmetric, algorithm, algorithm_specifier, mode, key_opacity, key_size, multipart
#else
           payload_size, empty_str_to_na(is_symmetric), empty_str_to_na(algorithm),
           empty_str_to_na(algorithm_specifier), empty_str_to_na(mode), empty_str_to_na(key_opacity), key_size,
           empty_str_to_na(multipart)
#endif
    );

    PRINTF(CB_PRINT_FORMAT_STR_PERF_VALUE_LOOP_PROLOGUE);
    for (; i < perf_data_cnt - 1; i++)
    {
        PRINTF(CB_PRINT_FORMAT_STR_PERF_VALUE_LOOP_BODY, perf_value[i]);
    }
    PRINTF(CB_PRINT_FORMAT_STR_PERF_VALUE_LOOP_EPILOGUE, perf_value[perf_data_cnt - 1]);

    PRINTF(CB_PRINT_FORMAT_STR_PERF_UNIT_LOOP_PROLOGUE);
    for (i = 0; i < perf_data_cnt - 1; i++)
    {
        PRINTF(CB_PRINT_FORMAT_STR_PERF_UNIT_LOOP_BODY, perf_unit[i]);
    }
    PRINTF(CB_PRINT_FORMAT_STR_PERF_UNIT_TAIL_EPILOGUE, perf_unit[perf_data_cnt - 1]);
    PRINTF("\n");
}

void print_rsa_case_name(cb_rsa_type_t rsa_type, cb_hash_t hash, size_t key_size)
{
    PRINTF("\tRSA-%u-%s-%s\t", key_size, rsa_type_to_string(rsa_type), hash_to_string(hash));
}

void print_aead_case_name(cb_aead_type_t aead_type, size_t key_size, size_t block_count)
{
    PRINTF("\tAEAD-%s-%u %u-block\t", aead_to_string(aead_type), key_size, block_count);
}

void print_aes_case_name(cb_cipher_type_t cipher_type, size_t key_size, size_t block_count)
{
    PRINTF("\tAES-%s-%u %u-block\t", cipher_to_string(cipher_type), key_size, block_count);
}

void print_ecdsa_case_name(cb_ecc_family_t ecc_family, cb_ecdsa_type_t ecdsa_type, size_t key_size)
{
    PRINTF("\tECDSA-%u-%s-%s\t", key_size, ecc_family_to_string(ecc_family), ecdsa_type_to_string(ecdsa_type));
}

void print_mac_case_name(cb_mac_algorithm_t mac, cb_hash_t hash, size_t key_size, size_t block_count)
{
    // There will be two dashes if not HMAC, as hash_to_string() will return ""
    PRINTF("\tMAC-%s-%s-%u %u-block\t", mac_to_string(mac), hash_to_string(hash), key_size, block_count);
}

void print_hash_case_name(cb_hash_t hash, size_t block_count)
{
    PRINTF("\tHASH-%s %u-block\t", hash_to_string(hash), block_count);
}

void print_failure(const char *additional_message)
{
#if defined(CB_PRINT_VERBOSE) && (CB_PRINT_VERBOSE == 1u)
    PRINTF("BENCHMARK FAILURE %s\r\n", additional_message != NULL ? additional_message : "");
#endif
}
