/*
 * Copyright 2025 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/**
 * @file  common_utils.c
 * @brief Definitions for common utility functions for the PSA layer.
 */

#include "include/common_utils.h"

cb_status_t convert_cb_aead_type_to_psa_alg(cb_aead_type_t aead_type, psa_algorithm_t *alg)
{
    cb_status_t status = CB_STATUS_SUCCESS;

    switch (aead_type)
    {
        case CB_AEAD_TYPE_CCM:
            *alg = PSA_ALG_CCM;
            break;
        case CB_AEAD_TYPE_GCM:
            *alg = PSA_ALG_GCM;
            break;
        case CB_AEAD_TYPE_CHACHA20_POLY1305:
            *alg = PSA_ALG_CHACHA20_POLY1305;
            break;
        default:
            status = CB_STATUS_FAIL;
            break;
    }

    return status;
}

cb_status_t convert_cb_cipher_type_to_psa_alg(cb_cipher_type_t cipher_type, psa_algorithm_t *alg)
{
    cb_status_t status = CB_STATUS_SUCCESS;

    switch (cipher_type)
    {
        case CB_CIPHER_TYPE_STREAM:
            *alg = PSA_ALG_STREAM_CIPHER;
            break;
        case CB_CIPHER_TYPE_ECB_NO_PADDING:
            *alg = PSA_ALG_ECB_NO_PADDING;
            break;
        case CB_CIPHER_TYPE_CBC_NO_PADDING:
            *alg = PSA_ALG_CBC_NO_PADDING;
            break;
        case CB_CIPHER_TYPE_CBC_PKCS7_PADDING:
            *alg = PSA_ALG_CBC_PKCS7;
            break;
        case CB_CIPHER_TYPE_CTR:
            *alg = PSA_ALG_CTR;
            break;
        case CB_CIPHER_TYPE_OFB:
            *alg = PSA_ALG_OFB;
            break;
        case CB_CIPHER_TYPE_CFB:
            *alg = PSA_ALG_CFB;
            break;
        case CB_CIPHER_TYPE_XTS:
            *alg = PSA_ALG_XTS;
            break;
        default:
            status = CB_STATUS_FAIL;
            break;
    }

    return status;
}

cb_status_t convert_cb_hash_type_to_psa_alg(cb_hash_t hash_type, psa_algorithm_t *alg)
{
    cb_status_t status = CB_STATUS_SUCCESS;

    switch(hash_type)
    {
        case CB_HASH_TYPE_SHA1:
            *alg = PSA_ALG_SHA_1;
            break;
        case CB_HASH_TYPE_SHA224:
            *alg = PSA_ALG_SHA_224;
            break;
        case CB_HASH_TYPE_SHA256:
            *alg = PSA_ALG_SHA_256;
            break;
        case CB_HASH_TYPE_SHA384:
            *alg = PSA_ALG_SHA_384;
            break;
        case CB_HASH_TYPE_SHA512:
            *alg = PSA_ALG_SHA_512;
            break;
        case CB_HASH_TYPE_SHA3_224:
            *alg = PSA_ALG_SHA3_224;
            break;
        case CB_HASH_TYPE_SHA3_256:
            *alg = PSA_ALG_SHA3_256;
            break;
        case CB_HASH_TYPE_SHA3_384:
            *alg = PSA_ALG_SHA3_384;
            break;
        case CB_HASH_TYPE_SHA3_512:
            *alg = PSA_ALG_SHA3_512;
            break;
        case CB_HASH_TYPE_SHAKE256:
            *alg = PSA_ALG_SHAKE256_512;
            break;
        case CB_HASH_TYPE_ANY_HASH:
            *alg = PSA_ALG_ANY_HASH;
            break;
        case CB_HASH_TYPE_NO_HASH:
            *alg = PSA_ALG_NONE;
            break;
        default:
            status = CB_STATUS_FAIL;
            break;
    }

    return status;
}

cb_status_t convert_cb_ecc_family_to_psa_ecc_family(cb_ecc_family_t cb_ecc_family, psa_ecc_family_t *psa_ecc_family)
{
    cb_status_t status = CB_STATUS_SUCCESS;

    switch(cb_ecc_family)
    {
        case CB_ECC_FAMILY_SECP_K1:
            *psa_ecc_family = PSA_ECC_FAMILY_SECP_K1;
            break;
        case CB_ECC_FAMILY_SECP_R1:
            *psa_ecc_family = PSA_ECC_FAMILY_SECP_R1;
            break;
        case CB_ECC_FAMILY_SECP_R2:
            *psa_ecc_family = PSA_ECC_FAMILY_SECP_R2;
            break;
        case CB_ECC_FAMILY_SECT_K1:
            *psa_ecc_family = PSA_ECC_FAMILY_SECT_K1;
            break;
        case CB_ECC_FAMILY_SECT_R1:
            *psa_ecc_family = PSA_ECC_FAMILY_SECT_R1;
            break;
        case CB_ECC_FAMILY_SECT_R2:
            *psa_ecc_family = PSA_ECC_FAMILY_SECT_R2;
            break;
        case CB_ECC_FAMILY_BRAINPOOL_P_R1:
            *psa_ecc_family = PSA_ECC_FAMILY_BRAINPOOL_P_R1;
            break;
#if defined(PSA_ECC_FAMILY_FRP)
        case CB_ECC_FAMILY_FRP:
            *psa_ecc_family = PSA_ECC_FAMILY_FRP;
            break;
#endif /* PSA_ECC_FAMILY_FRP */
        case CB_ECC_FAMILY_MONTGOMERY:
            *psa_ecc_family = PSA_ECC_FAMILY_MONTGOMERY;
            break;
        case CB_ECC_FAMILY_TWISTED_EDWARDS:
            *psa_ecc_family = PSA_ECC_FAMILY_TWISTED_EDWARDS;
            break;
        default:
            status = CB_STATUS_FAIL;
            break;
    }

    return status;
}

// TODO need to think about this one... Need the hash algo here,
// so decide if I should pass the CB hash type or an already converted
// PSA hash type from a previous call to convert_cb_hash_type_to_psa_alg().
//cb_status_t convert_cb_ecdsa_type_to_psa_alg(cb_ecdsa_type_t ecdsa_type,  psa_algorithm_t *alg)
//{
//    cb_status_t status = CB_STATUS_SUCCESS;
//
//    switch (ecdsa_type)
//    {
//        case CB_ECDSA_RANDOMIZED:
//            *alg = PSA_ALG_ECDSA(context->hash_alg);
//            break;
//        case CB_ECDSA_DETERMINISTIC:
//            *alg = PSA_ALG_DETERMINISTIC_ECDSA(context->hash_alg);
//            break;
//        default:
//            status = CB_STATUS_FAIL;
//            break;
//    }
//
//    return status;
//}
