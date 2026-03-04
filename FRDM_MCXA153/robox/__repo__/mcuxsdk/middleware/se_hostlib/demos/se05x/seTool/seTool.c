/* Copyright 2019, 2023 NXP
 * SPDX-License-Identifier: Apache-2.0
 */

#include "seTool.h"

#include <ex_sss_boot.h>
#include <fsl_sss_se05x_apis.h>
#include <nxEnsure.h>
#include <nxLog_App.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/bn.h>
#include <openssl/ossl_typ.h>
#include <openssl/rsa.h>
#if (OPENSSL_VERSION_NUMBER >= 0x30000000)
#include <openssl/core_names.h>
#endif
#include <se05x_APDU.h>
#include <se05x_const.h>
#include <se05x_ecc_curves.h>
#include <se05x_ecc_curves_values.h>
#include <se05x_tlv.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

static ex_sss_boot_ctx_t gex_sss_se_tool;

#define EX_SSS_BOOT_PCONTEXT (&gex_sss_se_tool)
#define EX_SSS_BOOT_DO_ERASE 0
#define EX_SSS_BOOT_EXPOSE_ARGC_ARGV 1

/* ************************************************************************** */
/* Include "main()" with the platform specific startup code for Plug & Trust  */
/* MW examples which will call ex_sss_entry()                                 */
/* ************************************************************************** */
#include <ex_sss_main_inc.h>
static sss_status_t getSE05xPublicRsaKey(
    ex_sss_boot_ctx_t *pCtx, uint32_t keyId, uint8_t *rsaKeyData, size_t *rsaKeyDataSize, size_t *rsaKeyBitLen);

#if (OPENSSL_VERSION_NUMBER >= 0x30000000)
static EVP_PKEY *set_rsa_ref_key(BIGNUM *n, BIGNUM *e, BIGNUM *d, BIGNUM *p, BIGNUM *q, BIGNUM *dmp1, BIGNUM *dmq1, BIGNUM *iqmp);
static unsigned char *bn_to_bin(BIGNUM *bn, int *len);
#endif

static void usage()
{
    LOG_W("Usage:\r\n");
    LOG_W("EC NIST P256:\r\n");
    LOG_W("\tseTool genECC <keyId> <portname>\r\n");
    LOG_W("\tseTool setECC <keyId> <filename> <portname>\r\n");
    LOG_W("\tseTool getECCPublic <keyId> <filename> <portname>\r\n");
    LOG_W("\tseTool getECCRef <keyId> <filename> <portname>\r\n");
    LOG_W("\r\n");
    LOG_W("RSA 1024/2048/3072/4096:\r\n");
    LOG_W("\tseTool genRSA <rsaKeySize> <keyId> <portname>\r\n");
    LOG_W("\tseTool setRSA <rsaKeySize> <keyId> <filename> <portname>\r\n");
    LOG_W("\tseTool getRSAPublic <keyId> <filename> <portname>\r\n");
    LOG_W("\tseTool getRSARef <keyId> <filename> <portname>\r\n");
    LOG_W("\r\n");
    LOG_W("rsaKeySize = 1024, 2048, 3072 or 4096 (supported by the seTool)\r\n");
    LOG_W("Note: Please check the SE05x documentation for supported SE05x \r\n");
    LOG_W("      device RSA key sizes.\r\n");
    LOG_W("\r\n");
    LOG_W("portname = Subsystem specific connection parameters. Example: COM6,\r\n");
    LOG_W("127.0.0.1:8050. Use \"None\" where not applicable. e.g. SCI2C/T1oI2C.\r\n");
    LOG_W("Default i2c port (i2c-1) will be used for port name = \"none\"\r\n");
    LOG_W("\r\n");
    LOG_W("Note: The Privacy-Enhanced Mail (PEM) file format is used for\r\n");
    LOG_W("storing and importing keys.\r\n");
}

sss_status_t ex_sss_entry(ex_sss_boot_ctx_t *pCtx)
{
    sss_status_t status = kStatus_SSS_Fail;
    int argc            = gex_sss_argc;
    const char **argv   = gex_sss_argv;
    int genECC          = 0;
    int getECCPublic    = 0;
    int getECCRef       = 0;
    int setECC          = 0;
    int genRSA          = 0;
    int setRSA          = 0;
    int getRSAPublic    = 0;
    int getRSARef       = 0;

    if (argc < 2) {
        usage();
        status = kStatus_SSS_Success;
        goto exit;
    }
    if (strncmp(argv[1], "genECC", sizeof("genECC")) == 0) {
        genECC = 1;
    }
    else if (strncmp(argv[1], "setECC", sizeof("setECC")) == 0) {
        setECC = 1;
    }
    else if (strncmp(argv[1], "getECCPublic", sizeof("getECCPublic")) == 0) {
        getECCPublic = 1;
    }
    else if (strncmp(argv[1], "getECCRef", sizeof("getECCRef")) == 0) {
        getECCRef = 1;
    }
    else if (strncmp(argv[1], "genRSA", sizeof("genRSA")) == 0) {
        genRSA = 1;
    }
    else if (strncmp(argv[1], "setRSA", sizeof("setRSA")) == 0) {
        setRSA = 1;
    }
    else if (strncmp(argv[1], "getRSAPublic", sizeof("getRSAPublic")) == 0) {
        getRSAPublic = 1;
    }
    else if (strncmp(argv[1], "getRSARef", sizeof("getRSARef")) == 0) {
        getRSARef = 1;
    }
    else {
        LOG_E("Invalid command line parameter");
        usage();
        goto exit;
    }

    if (genECC) {
        if (argc < 4) {
            usage();
            goto exit;
        }

        unsigned long int id = strtoul(argv[2], NULL, 0);
        if (id > UINT32_MAX) {
            LOG_E("id can not be greater than 4 bytes.");
            goto exit;
        }
        uint32_t keyId = (uint32_t)(id);
        /*We demonstrate for cipher type and key size corresponding to NISTP-256 keypair only*/
        sss_object_t obj             = {0};
        sss_key_part_t keyPart       = kSSS_KeyPart_Pair;
        sss_cipher_type_t cipherType = kSSS_CipherType_EC_NIST_P;
        size_t keyBitLen             = 256;

        status = sss_key_object_init(&obj, &pCtx->ks);
        ENSURE_OR_GO_EXIT(status == kStatus_SSS_Success);

        if (keyBitLen > (SIZE_MAX / 8)) {
            status = kStatus_SSS_Fail;
            goto exit;
        }
        status =
            sss_key_object_allocate_handle(&obj, keyId, keyPart, cipherType, keyBitLen * 8, kKeyObject_Mode_Persistent);
        ENSURE_OR_GO_EXIT(status == kStatus_SSS_Success);

        status = sss_key_store_generate_key(&pCtx->ks, &obj, keyBitLen, NULL);
        ENSURE_OR_GO_EXIT(status == kStatus_SSS_Success);

        sss_key_object_free(&obj);
    }
    else if (getECCPublic) {
        if (argc < 4) {
            usage();
            goto exit;
        }

        unsigned long int id = strtoul(argv[2], NULL, 0);
        if (id > UINT32_MAX) {
            LOG_E("id can not be greater than 4 bytes.");
            goto exit;
        }
        uint32_t keyId = (uint32_t)(id);
        /*We demonstrate for cipher type and key size corresponding to NISTP-256 keypair only*/
        sss_object_t obj  = {0};
        size_t keyBitLen  = 256;
        uint8_t key[256]  = {0};
        size_t keyByteLen = sizeof(key);

        status = sss_key_object_init(&obj, &pCtx->ks);
        ENSURE_OR_GO_EXIT(status == kStatus_SSS_Success);

        status = sss_key_object_get_handle(&obj, keyId);
        ENSURE_OR_GO_EXIT(status == kStatus_SSS_Success);

        status = sss_key_store_get_key(&pCtx->ks, &obj, key, &keyByteLen, &keyBitLen);
        ENSURE_OR_GO_EXIT(status == kStatus_SSS_Success);

        sss_key_object_free(&obj);

        BIO *bio = BIO_new_mem_buf(key, (int)sizeof(key));
        if (bio == NULL) {
            LOG_E("Unable to initialize BIO");
            status = kStatus_SSS_Fail;
            goto exit;
        }

        EVP_PKEY *pKey = d2i_PUBKEY_bio(bio, NULL);
        if (!pKey) {
            LOG_E("Failed to load public key");
            status = kStatus_SSS_Fail;
            goto exit;
        }

        char file_name[MAX_FILE_NAME_SIZE] = {0};
        FILE *fp                           = NULL;
        strncpy(file_name, argv[3], sizeof(file_name) - 1);
        if (strstr(file_name, "..") != NULL) {
            LOG_W("Potential directory traversal");
        }
        if (file_name[0] == '/') {
            LOG_W("Accessing file using absolute path");
        }
        fp = fopen(file_name, "wb+");
        if (fp == NULL) {
            LOG_E("Can not open the file");
            status = kStatus_SSS_Fail;
            goto exit;
        }
        else {
            PEM_write_PUBKEY(fp, pKey);
            if (fclose(fp) != 0) {
                LOG_E("Can not close the file");
                status = kStatus_SSS_Fail;
                goto exit;
            }
        }
    }
    else if (setECC) {
        if (argc < 4) {
            usage();
            goto exit;
        }

        unsigned long int id = strtoul(argv[2], NULL, 0);
        if (id > UINT32_MAX) {
            LOG_E("id can not be greater than 4 bytes.");
            goto exit;
        }
        uint32_t keyId = (uint32_t)(id);
        /*We demonstrate for cipher type and key size corresponding to NISTP-256 keypair only*/
        sss_object_t obj             = {0};
        sss_key_part_t keyPart       = kSSS_KeyPart_Pair;
        sss_cipher_type_t cipherType = kSSS_CipherType_EC_NIST_P;
        size_t keyBitLen             = 256;
        uint8_t key[256]             = {0};
        size_t keyByteLen            = sizeof(key);
        unsigned char *data          = &key[0];
        int len                      = 0;

        EVP_PKEY *pKey = NULL;

        char file_name[MAX_FILE_NAME_SIZE] = {0};
        FILE *fp                           = NULL;
        strncpy(file_name, argv[3], sizeof(file_name) - 1);
        if (strstr(file_name, "..") != NULL) {
            LOG_W("Potential directory traversal");
        }
        if (file_name[0] == '/') {
            LOG_W("Accessing file using absolute path");
        }
        fp = fopen(file_name, "rb");
        if (fp == NULL) {
            LOG_E("Can not open the file");
            goto exit;
        }
        else {
            PEM_read_PrivateKey(fp, &pKey, NULL, NULL);
            if (fclose(fp) != 0) {
                LOG_E("Can not close the file");
                status = kStatus_SSS_Fail;
                goto exit;
            }
            if (pKey == NULL) {
                LOG_E("Failed to read Keypair");
                status = kStatus_SSS_Fail;
                goto exit;
            }
        }

        len = i2d_PrivateKey(pKey, &data);
        if (len <= 0) {
            goto exit;
        }
        keyByteLen = len;

        status = sss_key_object_init(&obj, &pCtx->ks);
        ENSURE_OR_GO_EXIT(status == kStatus_SSS_Success);

        if (keyBitLen > (SIZE_MAX / 8)) {
            status = kStatus_SSS_Fail;
            goto exit;
        }
        status =
            sss_key_object_allocate_handle(&obj, keyId, keyPart, cipherType, keyBitLen * 8, kKeyObject_Mode_Persistent);
        ENSURE_OR_GO_EXIT(status == kStatus_SSS_Success);

        status = sss_key_store_set_key(&pCtx->ks, &obj, key, keyByteLen, keyBitLen, NULL, 0);
        ENSURE_OR_GO_EXIT(status == kStatus_SSS_Success);

        sss_key_object_free(&obj);
    }
    else if (getECCRef) {
        if (argc < 4) {
            usage();
            goto exit;
        }

        unsigned long int id = strtoul(argv[2], NULL, 0);
        if (id > UINT32_MAX) {
            LOG_E("id can not be greater than 4 bytes.");
            goto exit;
        }
        uint32_t keyId = (uint32_t)(id);
        /*We demonstrate for cipher type and key size corresponding to NISTP-256 keypair only*/
        sss_object_t obj      = {0};
        size_t keyBitLen      = 256;
        uint8_t key[256]      = {0};
        size_t keyByteLen     = sizeof(key);
        uint8_t priv_header[] = PRIV_PREFIX_NIST_P_256;
        uint8_t pub_header[]  = PUBLIC_PREFIX_NIST_P_256;
        uint8_t magic[]       = MAGIC_BYTES_SE05X_OPENSSL_ENGINE;
        uint8_t ref[256]      = {0};
        size_t ref_len        = 0;
        uint8_t id_array[4]   = {0};

        status = sss_key_object_init(&obj, &pCtx->ks);
        ENSURE_OR_GO_EXIT(status == kStatus_SSS_Success);

        status = sss_key_object_get_handle(&obj, keyId);
        ENSURE_OR_GO_EXIT(status == kStatus_SSS_Success);

        status = sss_key_store_get_key(&pCtx->ks, &obj, key, &keyByteLen, &keyBitLen);
        ENSURE_OR_GO_EXIT(status == kStatus_SSS_Success);

        sss_key_object_free(&obj);

        memcpy(&ref[ref_len], priv_header, sizeof(priv_header));
        ref_len += sizeof(priv_header);

        id_array[0] = (keyId & 0xFF000000) >> (8 * 3);
        id_array[1] = (keyId & 0x00FF0000) >> (8 * 2);
        id_array[2] = (keyId & 0x0000FF00) >> (8 * 1);
        id_array[3] = (keyId & 0x000000FF) >> (8 * 0);

        memcpy(&ref[ref_len], id_array, sizeof(id_array));
        ref_len += sizeof(id_array);

        memcpy(&ref[ref_len], magic, sizeof(magic));
        ref_len += sizeof(magic);

        ref[ref_len++] = 0x10; // Hardcoded for Keypair
        ref[ref_len++] = 0x00; // Hardcoded as 0x00

        memcpy(&ref[ref_len], pub_header, sizeof(pub_header));
        ref_len += sizeof(pub_header);

        if ((ref_len + (keyByteLen - 22)) > sizeof(ref)) {
            status = kStatus_SSS_Fail;
            goto exit;
        }

        if ((23 + (keyByteLen - 22)) > sizeof(key)) {
            status = kStatus_SSS_Fail;
            goto exit;
        }

        /* Copy only public key part */
        memcpy(&ref[ref_len], &key[23], keyByteLen - 22);
        ref_len += (keyByteLen - 22);

        BIO *bio = BIO_new_mem_buf(ref, (int)(ref_len));
        if (bio == NULL) {
            LOG_E("Unable to initialize BIO");
            status = kStatus_SSS_Fail;
            goto exit;
        }

        EVP_PKEY *pKey = d2i_PrivateKey_bio(bio, NULL);
        if (!pKey) {
            LOG_E("Failed to load public key");
            status = kStatus_SSS_Fail;
            goto exit;
        }

        char file_name[MAX_FILE_NAME_SIZE] = {0};
        FILE *fp                           = NULL;
        strncpy(file_name, argv[3], sizeof(file_name) - 1);
        if (strstr(file_name, "..") != NULL) {
            LOG_W("Potential directory traversal");
        }
        if (file_name[0] == '/') {
            LOG_W("Accessing file using absolute path");
        }
        fp = fopen(file_name, "wb+");
        if (fp == NULL) {
            LOG_E("Can not open the file");
            status = kStatus_SSS_Fail;
            goto exit;
        }
        else {
            PEM_write_PrivateKey(fp, pKey, NULL, NULL, 0, NULL, NULL);
            if (fclose(fp) != 0) {
                LOG_E("Can not close the file");
                status = kStatus_SSS_Fail;
                goto exit;
            }
        }
    }
    else if (genRSA) {
        if (argc < 5) {
            usage();
            goto exit;
        }

        unsigned long int rsaKeySize = strtoul(argv[2], NULL, 0);
        unsigned long int id         = strtoul(argv[3], NULL, 0);
        if (id > UINT32_MAX) {
            LOG_E("id can not be greater than 4 bytes.");
            goto exit;
        }
        uint32_t keyId               = (uint32_t)(id);
        sss_object_t obj             = {0};
        sss_key_part_t keyPart       = kSSS_KeyPart_Pair;
        sss_cipher_type_t cipherType = kSSS_CipherType_RSA_CRT;
        size_t keyBitLen             = rsaKeySize;

        status = sss_key_object_init(&obj, &pCtx->ks);
        ENSURE_OR_GO_EXIT(status == kStatus_SSS_Success);

        if (keyBitLen > (SIZE_MAX / 8)) {
            status = kStatus_SSS_Fail;
            goto exit;
        }
        status =
            sss_key_object_allocate_handle(&obj, keyId, keyPart, cipherType, keyBitLen * 8, kKeyObject_Mode_Persistent);
        ENSURE_OR_GO_EXIT(status == kStatus_SSS_Success);

        status = sss_key_store_generate_key(&pCtx->ks, &obj, keyBitLen, NULL);
        ENSURE_OR_GO_EXIT(status == kStatus_SSS_Success);

        sss_key_object_free(&obj);
    }
    else if (setRSA) {
        if (argc < 5) {
            usage();
            goto exit;
        }
        unsigned long int rsaKeySize = strtoul(argv[2], NULL, 0);
        unsigned long int id         = strtoul(argv[3], NULL, 0);
        if (id > UINT32_MAX) {
            LOG_E("id can not be greater than 4 bytes.");
            goto exit;
        }
        uint32_t keyId = (uint32_t)(id);

        sss_object_t obj             = {0};
        sss_key_part_t keyPart       = kSSS_KeyPart_Pair;
        sss_cipher_type_t cipherType = kSSS_CipherType_RSA_CRT;
        size_t keyBitLen             = rsaKeySize;
        uint8_t key[4096]            = {0};
        size_t keyByteLen            = sizeof(key);
        unsigned char *data          = &key[0];
        int len                      = 0;

        EVP_PKEY *pKey = NULL;

        char file_name[MAX_FILE_NAME_SIZE] = {0};
        FILE *fp                           = NULL;
        strncpy(file_name, argv[4], sizeof(file_name) - 1);
        if (strstr(file_name, "..") != NULL) {
            LOG_W("Potential directory traversal");
        }
        if (file_name[0] == '/') {
            LOG_W("Accessing file using absolute path");
        }
        fp = fopen(file_name, "rb");
        if (fp == NULL) {
            LOG_E("Can not open the file");
            status = kStatus_SSS_Fail;
            goto exit;
        }
        else {
            PEM_read_PrivateKey(fp, &pKey, NULL, NULL);
            if (fclose(fp) != 0) {
                LOG_E("Can not close the file");
                status = kStatus_SSS_Fail;
                goto exit;
            }
            if (pKey == NULL) {
                LOG_E("Failed to read Keypair");
                status = kStatus_SSS_Fail;
                goto exit;
            }
        }

        len = i2d_PrivateKey(pKey, &data);
        if (len <= 0) {
            goto exit;
        }
        keyByteLen = len;

        status = sss_key_object_init(&obj, &pCtx->ks);
        ENSURE_OR_GO_EXIT(status == kStatus_SSS_Success);

        if (keyBitLen > (SIZE_MAX / 8)) {
            status = kStatus_SSS_Fail;
            goto exit;
        }
        status =
            sss_key_object_allocate_handle(&obj, keyId, keyPart, cipherType, keyBitLen * 8, kKeyObject_Mode_Persistent);
        ENSURE_OR_GO_EXIT(status == kStatus_SSS_Success);

        status = sss_key_store_set_key(&pCtx->ks, &obj, key, keyByteLen, keyBitLen, NULL, 0);
        ENSURE_OR_GO_EXIT(status == kStatus_SSS_Success);

        sss_key_object_free(&obj);
    }
    else if (getRSAPublic) {
        if (argc < 4) {
            usage();
            goto exit;
        }

        unsigned long int id = strtoul(argv[2], NULL, 0);
        if (id > UINT32_MAX) {
            LOG_E("id can not be greater than 4 bytes.");
            goto exit;
        }
        uint32_t keyId = (uint32_t)(id);

        // sss_object_t obj;
        size_t rsaKeyBitLen = 0;

        uint8_t rsaKeyData[808] = {0}; //up to RSA 4096
        size_t rsaKeyByteLen    = sizeof(rsaKeyData);

        /* get SE05x public rsa key. */
        status = getSE05xPublicRsaKey(pCtx, keyId, rsaKeyData, &rsaKeyByteLen, &rsaKeyBitLen);
        ENSURE_OR_GO_EXIT(status == kStatus_SSS_Success);

        BIO *bio = BIO_new_mem_buf(rsaKeyData, rsaKeyByteLen);
        if (bio == NULL) {
            LOG_E("BIO_new_mem_buf() - failed");
            status = kStatus_SSS_Fail;
            goto exit;
        }

        EVP_PKEY *pKey = d2i_PUBKEY_bio(bio, NULL);
        if (!pKey) {
            LOG_E("d2i_PUBKEY_bio() - failed");
            status = kStatus_SSS_Fail;
            goto exit;
        }

        char file_name[MAX_FILE_NAME_SIZE] = {0};
        FILE *fp                           = NULL;
        strncpy(file_name, argv[3], sizeof(file_name) - 1);
        if (strstr(file_name, "..") != NULL) {
            LOG_W("Potential directory traversal");
        }
        if (file_name[0] == '/') {
            LOG_W("Accessing file using absolute path");
        }
        fp = fopen(file_name, "wb+");
        if (fp == NULL) {
            LOG_E("Can not open the file");
            status = kStatus_SSS_Fail;
            goto exit;
        }
        else {
            PEM_write_PUBKEY(fp, pKey);
            if (fclose(fp) != 0) {
                LOG_E("Can not close the file");
                status = kStatus_SSS_Fail;
                goto exit;
            }
        }
    }
    else if (getRSARef) {
        if (argc < 4) {
            usage();
            goto exit;
        }

        unsigned long int id = strtoul(argv[2], NULL, 0);
        if (id > UINT32_MAX) {
            LOG_E("id can not be greater than 4 bytes.");
            goto exit;
        }
        uint32_t keyId = (uint32_t)(id);

        uint8_t rsaKeyData[808] = {0}; //up to RSA 4096
        size_t rsaKeyByteLen    = sizeof(rsaKeyData);
        size_t rsaKeyBitLen     = 0;

        /* get SE05x public rsa key. */
        status = getSE05xPublicRsaKey(pCtx, keyId, rsaKeyData, &rsaKeyByteLen, &rsaKeyBitLen);
        ENSURE_OR_GO_EXIT(status == kStatus_SSS_Success);

        BIO *bio = BIO_new_mem_buf(rsaKeyData, rsaKeyByteLen);
        if (bio == NULL) {
            LOG_E("BIO_new_mem_buf() - failed");
            status = kStatus_SSS_Fail;
            goto exit;
        }

        EVP_PKEY *pKey = d2i_PUBKEY_bio(bio, NULL);
        if (!pKey) {
            LOG_E("d2i_PUBKEY_bio() -failed");
            status = kStatus_SSS_Fail;
            goto exit;
        }

        /* placeholder for p(aka 'prime1') */
        BIGNUM *pBigNum_d = BN_new();
        BN_set_word(pBigNum_d, 1L);

        /* placeholder for p(aka 'prime1') */
        BIGNUM *pBigNum_p = BN_new();
        BN_set_word(pBigNum_p, 1L);

        /*The value reserved for �q�(aka 'prime2') is used to store the 32 bit key identifier*/
        BIGNUM *pBigNum_q = BN_new();
        BN_set_word(pBigNum_q, (unsigned long)keyId);

        /* placeholder for exponent1 d mod(p - 1) */
        BIGNUM *pBigNum_dmp1 = BN_new();
        BN_set_word(pBigNum_dmp1, 1L);

        /* placeholder for exponent2 d mod(q - 1) */
        BIGNUM *pBigNum_dmq1 = BN_new();
        BN_set_word(pBigNum_dmq1, 1L);

#if (OPENSSL_VERSION_NUMBER >= 0x30000000)
        BIGNUM *pBigNum_n = NULL, *pBigNum_e = NULL;
        // Retrieve n and e
        if (EVP_PKEY_get_bn_param(pKey, OSSL_PKEY_PARAM_RSA_N, &pBigNum_n) <= 0) {
             status = kStatus_SSS_Fail;
             goto exit;
        }

        if (EVP_PKEY_get_bn_param(pKey, OSSL_PKEY_PARAM_RSA_E, &pBigNum_e) <= 0) {
             status = kStatus_SSS_Fail;
             goto exit;
        }

        /* The value reserved for �(inverse of q) mod p' */
        /* (aka �coefficient�) is used to store the magic number 0xA5A6B5B6 */
        /* to indicate this key is a referece key */
        BIGNUM *pBigNum_iqmp = BN_new();
        BN_set_word(pBigNum_iqmp, (unsigned long)0xB6B5A6A5);

        EVP_PKEY *pKey2 = NULL;
        pKey2 = set_rsa_ref_key(pBigNum_n, pBigNum_e, pBigNum_d, pBigNum_p, pBigNum_q, pBigNum_dmp1, pBigNum_dmq1, pBigNum_iqmp);
#else
        RSA *pRSAPublic         = NULL;
        pRSAPublic = EVP_PKEY_get1_RSA(pKey);
        if (!pRSAPublic) {
            LOG_E("EVP_PKEY_get1_RSA() - failed");
            status = kStatus_SSS_Fail;
            goto exit;
        }

        //Retrieve n and e
        BIGNUM *pBigNum_n = BN_new();
        pBigNum_n         = (BIGNUM *)RSA_get0_n(pRSAPublic);

        BIGNUM *pBigNum_e = BN_new();
        pBigNum_e         = (BIGNUM *)RSA_get0_e(pRSAPublic);

        /* The value reserved for �(inverse of q) mod p' */
        /* (aka �coefficient�) is used to store the magic number 0xA5A6B5B6 */
        /* to indicate this key is a referece key */
        BIGNUM *pBigNum_iqmp = BN_new();
        BN_set_word(pBigNum_iqmp, (unsigned long)0xA5A6B5B6);

        /* Generate a private reference key  */
        RSA *pRSAPrivate = RSA_new();

        /* Add n and e  */
        RSA_set0_key(pRSAPrivate, pBigNum_n, pBigNum_e, pBigNum_d);
        RSA_set0_factors(pRSAPrivate, pBigNum_p, pBigNum_q);
        RSA_set0_crt_params(pRSAPrivate, pBigNum_dmp1, pBigNum_dmq1, pBigNum_iqmp);

        EVP_PKEY *pKey2 = EVP_PKEY_new();
        EVP_PKEY_assign_RSA(pKey2, pRSAPrivate);
#endif

        char file_name[MAX_FILE_NAME_SIZE] = {0};
        FILE *fp                           = NULL;
        strncpy(file_name, argv[3], sizeof(file_name) - 1);
        if (strstr(file_name, "..") != NULL) {
            LOG_W("Potential directory traversal");
        }
        if (file_name[0] == '/') {
            LOG_W("Accessing file using absolute path");
        }
        fp = fopen(file_name, "wb+");
        if (fp == NULL) {
            LOG_E("Can not open the file");
            status = kStatus_SSS_Fail;
            goto exit;
        }
        else {
            PEM_write_PrivateKey(fp, pKey2, NULL, NULL, 0, NULL, NULL);
            if (fclose(fp) != 0) {
                LOG_E("Can not close the file");
                status = kStatus_SSS_Fail;
                goto exit;
            }
            status = kStatus_SSS_Success;
        }

        if (pBigNum_n != NULL)
            BN_free(pBigNum_n);
        if (pBigNum_e != NULL)
            BN_free(pBigNum_e);
        if (pBigNum_d != NULL)
            BN_free(pBigNum_d);
        if (pBigNum_p != NULL)
            BN_free(pBigNum_p);
        if (pBigNum_q != NULL)
            BN_free(pBigNum_q);
        if (pBigNum_dmp1 != NULL)
            BN_free(pBigNum_dmp1);
        if (pBigNum_dmq1 != NULL)
            BN_free(pBigNum_dmq1);
        if (pBigNum_iqmp != NULL)
            BN_free(pBigNum_iqmp);
    }
exit:
    return status;
}

static sss_status_t getSE05xPublicRsaKey(
    ex_sss_boot_ctx_t *pCtx, uint32_t keyId, uint8_t *rsaKeyData, size_t *rsaKeyDataSize, size_t *rsaKeyBitLen)
{
    sss_status_t status = kStatus_SSS_Fail;
    sss_object_t obj    = {0};

    status = sss_key_object_init(&obj, &pCtx->ks);
    ENSURE_OR_GO_EXIT(status == kStatus_SSS_Success);

    status = sss_key_object_get_handle(&obj, keyId);
    ENSURE_OR_GO_EXIT(status == kStatus_SSS_Success);

    status = sss_key_store_get_key(&pCtx->ks, &obj, rsaKeyData, rsaKeyDataSize, rsaKeyBitLen);
    ENSURE_OR_GO_EXIT(status == kStatus_SSS_Success);
exit:
    sss_key_object_free(&obj);
    if (status != kStatus_SSS_Success) {
        LOG_E("getSE05xPublicKey failed!");
    }
    return status;
}

#if (OPENSSL_VERSION_NUMBER >= 0x30000000)
static EVP_PKEY *set_rsa_ref_key(BIGNUM *n, BIGNUM *e, BIGNUM *d, BIGNUM *p, BIGNUM *q, BIGNUM *dmp1, BIGNUM *dmq1, BIGNUM *iqmp) {
    EVP_PKEY *pkey = NULL;
    EVP_PKEY_CTX *ctx = NULL;
    OSSL_PARAM params[9];  // Array to hold parameters
    size_t params_count = 0;
    int len = 0;
    int ret = 0;
    unsigned char *buf = NULL;

    ctx = EVP_PKEY_CTX_new_from_name(NULL, "RSA", NULL);
    if (ctx == NULL) {
        return NULL;
    }
    if (EVP_PKEY_fromdata_init(ctx) <= 0) {
        goto cleanup;
    }

    // Set modulus (n)
    buf = bn_to_bin(n, &len);
    if (buf == NULL) {
        return NULL;
    }
    // reverse the moduluse
    {
        int a       = 0;
        int b       = len - 1;
        uint8_t tmp = 0;
        for (; a < b; a++, b--) {
            if ((b < 0) || (b >= 512)) {
                return 0;
            }
            tmp        = buf[a];
            buf[a] = buf[b];
            buf[b] = tmp;
        }
    }
    params[params_count++] = OSSL_PARAM_construct_BN(OSSL_PKEY_PARAM_RSA_N, buf, len);
    // Set public exponent (e)
    buf = bn_to_bin(e, &len);
    if (buf == NULL) {
        return NULL;
    }
    params[params_count++] = OSSL_PARAM_construct_BN(OSSL_PKEY_PARAM_RSA_E, buf, len);
    // Set private exponent (d)
    if (d) {
        buf = bn_to_bin(d, &len);
        if (buf == NULL) {
            return NULL;
        }
        params[params_count++] = OSSL_PARAM_construct_BN(OSSL_PKEY_PARAM_RSA_D, buf, len);
    }
    // Set factor p
    if (p) {
        buf = bn_to_bin(p, &len);
        if (buf == NULL) {
            return NULL;
        }
        params[params_count++] = OSSL_PARAM_construct_BN(OSSL_PKEY_PARAM_RSA_FACTOR1, buf, len);
    }
    // Set factor q
    if (q) {
        buf = bn_to_bin(q, &len);
        if (buf == NULL) {
            return NULL;
        }
        params[params_count++] = OSSL_PARAM_construct_BN(OSSL_PKEY_PARAM_RSA_FACTOR2, buf, len);
    }
    // Set exponent dmp1
    if (dmp1) {
        buf = bn_to_bin(dmp1, &len);
        if (buf == NULL) {
            return NULL;
        }
        params[params_count++] = OSSL_PARAM_construct_BN(OSSL_PKEY_PARAM_RSA_EXPONENT1, buf, len);
    }
    // Set exponent dmq1
    if (dmq1) {
        buf = bn_to_bin(dmq1, &len);
        if (buf == NULL) {
            return NULL;
        }
        params[params_count++] = OSSL_PARAM_construct_BN(OSSL_PKEY_PARAM_RSA_EXPONENT2, buf, len);
    }
    // Set coefficient iqmp
    if (iqmp) {
        buf = bn_to_bin(iqmp, &len);
        if (buf == NULL) {
            return NULL;
        }
        params[params_count++] = OSSL_PARAM_construct_BN(OSSL_PKEY_PARAM_RSA_COEFFICIENT1, buf, len);
    }
    // Null-terminate the parameters array
    params[params_count] = OSSL_PARAM_construct_end();

    if (EVP_PKEY_fromdata(ctx, &pkey, EVP_PKEY_KEYPAIR, params) <= 0) {
        goto cleanup;
    }

    ret = 1;
cleanup:
    if (buf) {
        OPENSSL_free(buf);
    }
    if (ret == 0) {
        return NULL;
    }
    return pkey;
}

static unsigned char *bn_to_bin(BIGNUM *bn, int *len) {
    *len = BN_num_bytes(bn);
    unsigned char *buf = OPENSSL_malloc(*len);
    if (buf == NULL) {
        return NULL;
    }

    BN_bn2bin(bn, buf);
    return buf;
}
#endif
