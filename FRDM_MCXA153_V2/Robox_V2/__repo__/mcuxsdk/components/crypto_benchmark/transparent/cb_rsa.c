/*
 * Copyright 2024-2025 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "crypto_benchmark.h"
#include "cb_rsa.h"
#include "cb_types.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define CB_RSA_PERF_DATA_CNT      (1u)
#define CB_RSA_ALGORITHM_STR      ("RSA")
#define CB_RSA_SPECIFIER_MAX_SIZE (31u)

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void rsa_keygen_benchmark(void);
static void rsa_sign_verify_benchmark(void);
static void rsa_encrypt_decrypt_benchmark(void);

/*******************************************************************************
 * Variables
 ******************************************************************************/
/* Public can stay constant - using 65,537 */
static uint8_t public_exponent[]       = {0x1, 0x0, 0x1};
static size_t public_exponent_buf_size = sizeof(public_exponent) / sizeof(public_exponent[0]);

__weak const rsa_benchmark_cases_t rsa_cipher_benchmark_cases[0];
__weak const size_t rsa_cipher_benchmark_cases_nb = ARRAY_SIZE(rsa_cipher_benchmark_cases);

__weak const rsa_benchmark_cases_t rsa_signature_benchmark_cases[0];
__weak const size_t rsa_signature_benchmark_cases_nb = ARRAY_SIZE(rsa_signature_benchmark_cases);

__weak const rsa_keygen_benchmark_cases_t rsa_keygen_benchmark_cases[0];
__weak const size_t rsa_keygen_benchmark_cases_nb = ARRAY_SIZE(rsa_keygen_benchmark_cases);

/*******************************************************************************
 * Code
 ******************************************************************************/
__weak cb_status_t wrapper_rsa_keygen_compute(uint8_t *modulus,
                                              size_t modulus_buf_size,
                                              uint8_t *private_exponent,
                                              size_t private_exponent_buf_size,
                                              const uint8_t *public_exponent,
                                              size_t public_exponent_buf_size,
                                              size_t key_size)
{
    (void)modulus;
    (void)modulus_buf_size;
    (void)private_exponent;
    (void)private_exponent_buf_size;
    (void)public_exponent;
    (void)public_exponent_buf_size;
    (void)key_size;

    return CB_STATUS_FAIL;
}

__weak cb_status_t wrapper_rsa_init(
    void **ctx_internal, cb_rsa_type_t rsa_type, cb_hash_t hash_type, size_t key_size, size_t public_key_buffer_size, cb_asym_sigver_input_type_t input_type)
{
    (void)ctx_internal;
    (void)rsa_type;
    (void)hash_type;
    (void)key_size;
    (void)public_key_buffer_size;
    (void)input_type;

    return CB_STATUS_FAIL;
}

__weak cb_status_t wrapper_rsa_deinit(void *ctx_internal)
{
    (void)ctx_internal;

    return CB_STATUS_FAIL;
}

__weak cb_status_t wrapper_rsa_sign_compute(void *ctx_internal,
                                            const uint8_t *modulus,
                                            const uint8_t *private_exponent,
                                            uint8_t *message,
                                            size_t message_size,
                                            uint8_t *signature)
{
    (void)ctx_internal;
    (void)modulus;
    (void)private_exponent;
    (void)message;
    (void)message_size;
    (void)signature;

    return CB_STATUS_FAIL;
}

__weak cb_status_t wrapper_rsa_verify_compute(void *ctx_internal,
                                              const uint8_t *modulus,
                                              const uint8_t *public_exponent,
                                              uint8_t *message,
                                              size_t message_size,
                                              uint8_t *signature)
{
    (void)ctx_internal;
    (void)modulus;
    (void)public_exponent;
    (void)message;
    (void)message_size;
    (void)signature;

    return CB_STATUS_FAIL;
}

__weak cb_status_t wrapper_rsa_encrypt_compute(void *ctx_internal,
                                               const uint8_t *modulus,
                                               const uint8_t *public_exponent,
                                               uint8_t *message,
                                               size_t message_size,
                                               uint8_t *ciphertext)
{
    (void)ctx_internal;
    (void)modulus;
    (void)public_exponent;
    (void)message;
    (void)message_size;
    (void)ciphertext;

    return CB_STATUS_FAIL;
}

__weak cb_status_t wrapper_rsa_decrypt_compute(void *ctx_internal,
                                               const uint8_t *modulus,
                                               const uint8_t *private_exponent,
                                               uint8_t *message,
                                               size_t message_size,
                                               uint8_t *ciphertext)
{
    (void)ctx_internal;
    (void)modulus;
    (void)private_exponent;
    (void)message;
    (void)message_size;
    (void)ciphertext;

    return CB_STATUS_FAIL;
}

cb_status_t rsa_benchmark(void)
{
    rsa_keygen_benchmark();

    rsa_sign_verify_benchmark();

    rsa_encrypt_decrypt_benchmark();

    return CB_STATUS_SUCCESS;
}

static void rsa_encrypt_decrypt_benchmark(void)
{
    cb_status_t status = CB_STATUS_FAIL;

    /* Set up the common payload buffer such that there is no aliasing */
    uint8_t *modulus          = g_CommonPayload + (1024u / 8u); // Place key after the largest expected input
    uint8_t *private_exponent = modulus + (8192u / 8u);         // Expecting at most an 8k key
    void *ctx_internal        = NULL;
    size_t message_size       = 0u;
    size_t key_size           = 0u;
    size_t key_buf_size       = 0u;

    cb_rsa_type_t type                     = 0;
    cb_hash_t hash                         = 0;
    cb_asym_sigver_input_type_t input_type = 0;

    size_t i = 0u;

    double perf_value[CB_RSA_PERF_DATA_CNT]       = {0};
    char *perf_unit[CB_RSA_PERF_DATA_CNT]         = {0};
    char rsa_specifier[CB_RSA_SPECIFIER_MAX_SIZE] = {0};

#if !defined(CB_PRINT_CSV)
    PRINTF("RSA Generic Encrypt / Decrypt:\r\n");
#endif

    for (; i < rsa_cipher_benchmark_cases_nb; i++)
    {
        type = rsa_cipher_benchmark_cases[i].type;
        hash = rsa_cipher_benchmark_cases[i].hash;
        input_type = rsa_cipher_benchmark_cases[i].input_type;
        key_size = rsa_cipher_benchmark_cases[i].key_size;

        /* In order to give the porting layer the correct key size */
        key_buf_size = key_size / 8;

        /* Copy keys to the common buffer, so that we have control over algorithm input placement in memory */
        memcpy(modulus, rsa_cipher_benchmark_cases[i].modulus, key_buf_size);
        memcpy(private_exponent, rsa_cipher_benchmark_cases[i].private_exponent, key_buf_size);

        /* Initialize the port context */
        if (wrapper_rsa_init(&ctx_internal, type, hash,
                             key_size, public_exponent_buf_size, input_type) != CB_STATUS_SUCCESS)
        {
            print_failure("wrapper_rsa_init()");
            continue;
        }

        /* == == Begin individual benchmarks == == */

        perf_value[0] = TIME_TOTAL_INTERVAL(
            status == CB_STATUS_SUCCESS,
            status = wrapper_rsa_encrypt_compute(ctx_internal, modulus, public_exponent, g_CommonPayload,
                                                 CRYPTO_BENCHMARK_RSA_MSG_SIZE, g_CommonOutput));
        perf_unit[0] = CB_PERF_UNIT_ENCPS;

        if (status != CB_STATUS_SUCCESS)
        {
            print_failure(NULL);
            goto fail;
        }

        print_benchmark_case(CRYPTO_BENCHMARK_RSA_MSG_SIZE, CB_FALSE_STR, CB_RSA_ALGORITHM_STR,
                             rsa_type_with_hash_to_string(type,
                                                          hash, rsa_specifier,
                                                          CB_RSA_SPECIFIER_MAX_SIZE),
                             CB_MODE_ENCRYPT_STR, CB_KEY_OPACITY_TRANSPARENT_STR, key_size, "",
                             perf_value, perf_unit, CB_RSA_PERF_DATA_CNT);

        perf_value[0] = TIME_TOTAL_INTERVAL(
            status == CB_STATUS_SUCCESS,
            status = wrapper_rsa_decrypt_compute(ctx_internal, modulus, private_exponent, g_CommonPayload,
                                                 CRYPTO_BENCHMARK_RSA_MSG_SIZE, g_CommonOutput));
        perf_unit[0] = CB_PERF_UNIT_DECPS;

        if (status != CB_STATUS_SUCCESS)
        {
            print_failure(NULL);
            goto fail;
        }

        print_benchmark_case(CRYPTO_BENCHMARK_RSA_MSG_SIZE, CB_FALSE_STR, CB_RSA_ALGORITHM_STR,
                             rsa_type_with_hash_to_string(type,
                                                          hash, rsa_specifier,
                                                          CB_RSA_SPECIFIER_MAX_SIZE),
                             CB_MODE_DECRYPT_STR, CB_KEY_OPACITY_TRANSPARENT_STR, key_size, "",
                             perf_value, perf_unit, CB_RSA_PERF_DATA_CNT);

        /* == == == == == == == == == == == == == == == == == == == == == == */

fail:
        /* Clean up the port context before the next iteration */
        if (wrapper_rsa_deinit(ctx_internal) != CB_STATUS_SUCCESS)
        {
            print_failure("wrapper_rsa_deinit()");
        }
    }
}

static void rsa_sign_verify_benchmark(void)
{
    cb_status_t status = CB_STATUS_FAIL;

    /* Set up the common payload buffer such that there is no aliasing */
    uint8_t *input_digest     = g_CommonPayload;
    uint8_t *modulus          = input_digest + (512u / 8u); // Place key after the largest digest
    uint8_t *private_exponent = modulus + (8192u / 8u);     // Expecting at most an 8k key
    size_t message_size       = 0u;
    size_t key_size           = 0u;
    size_t key_buf_size       = 0u;
    char *sign_mode           = NULL;
    char *verify_mode         = NULL;

    cb_rsa_type_t type                     = 0;
    cb_hash_t hash                         = 0;
    cb_asym_sigver_input_type_t input_type = 0;

    size_t i = 0u;

    /* The internal port layer context populated by wrapper_*_init() */
    void *ctx_internal = NULL;

    double perf_value[CB_RSA_PERF_DATA_CNT]       = {0};
    char *perf_unit[CB_RSA_PERF_DATA_CNT]         = {0};
    char rsa_specifier[CB_RSA_SPECIFIER_MAX_SIZE] = {0};

#if !defined(CB_PRINT_CSV)
    PRINTF("RSA Generic Sign / Verify:\r\n");
#endif
    for (; i < rsa_signature_benchmark_cases_nb; i++)
    {
        key_size = rsa_signature_benchmark_cases[i].key_size;

        /* In order to give the porting layer the correct key size */
        key_buf_size = key_size / 8;

        /* Copy keys to the common buffer, so that we have control over algorithm input placement in memory */
        memcpy(modulus, rsa_signature_benchmark_cases[i].modulus, key_buf_size);
        memcpy(private_exponent, rsa_signature_benchmark_cases[i].private_exponent, key_buf_size);

        type = rsa_signature_benchmark_cases[i].type;
        hash = rsa_signature_benchmark_cases[i].hash;
        input_type = rsa_signature_benchmark_cases[i].input_type;

        if (CB_SIGVER_MESSAGE == input_type)
        {
            message_size = CRYPTO_BENCHMARK_RSA_MSG_SIZE;
            sign_mode = CB_MODE_SIGN_MSG_STR;
            verify_mode = CB_MODE_VERIFY_MSG_STR;
        }
        else
        {
            message_size = CB_HASH_LENGTH(hash);
            sign_mode = CB_MODE_SIGN_HASH_STR;
            verify_mode = CB_MODE_VERIFY_HASH_STR;
        }

        /* Initialize the port context */
        if (wrapper_rsa_init(&ctx_internal, type,
                             hash, key_size,
                             public_exponent_buf_size, input_type) != CB_STATUS_SUCCESS)
        {
            print_failure("wrapper_rsa_init()");
            continue;
        }

        /* == == Begin individual benchmarks == == */

        perf_value[0] = TIME_TOTAL_INTERVAL(
            status == CB_STATUS_SUCCESS,
            status = wrapper_rsa_sign_compute(ctx_internal, modulus, private_exponent, input_digest,
                                              message_size, g_CommonOutput));
        perf_unit[0] = CB_PERF_UNIT_SIGPS;

        if (status != CB_STATUS_SUCCESS)
        {
            print_failure(NULL);
            goto fail;
        }

        print_benchmark_case(message_size, CB_FALSE_STR, CB_RSA_ALGORITHM_STR,
                             rsa_type_with_hash_to_string(type,
                                                          hash, rsa_specifier,
                                                          CB_RSA_SPECIFIER_MAX_SIZE),
                             sign_mode, CB_KEY_OPACITY_TRANSPARENT_STR, key_size, "", perf_value,
                             perf_unit, CB_RSA_PERF_DATA_CNT);

        perf_value[0] = TIME_TOTAL_INTERVAL(
            status == CB_STATUS_SUCCESS,
            status = wrapper_rsa_verify_compute(ctx_internal, modulus, public_exponent, input_digest,
                                                message_size, g_CommonOutput));
        perf_unit[0] = CB_PERF_UNIT_VERIFPS;

        if (status != CB_STATUS_SUCCESS)
        {
            print_failure(NULL);
            goto fail;
        }

        print_benchmark_case(message_size, CB_FALSE_STR, CB_RSA_ALGORITHM_STR,
                             rsa_type_with_hash_to_string(type,
                                                          hash, rsa_specifier,
                                                          CB_RSA_SPECIFIER_MAX_SIZE),
                             verify_mode, CB_KEY_OPACITY_TRANSPARENT_STR, key_size, "",
                             perf_value, perf_unit, CB_RSA_PERF_DATA_CNT);

        /* == == == == == == == == == == == == == == == == == == == == == == */

fail:
        /* Clean up the port context before the next iteration */
        if (wrapper_rsa_deinit(ctx_internal) != CB_STATUS_SUCCESS)
        {
            print_failure("wrapper_rsa_deinit()");
        }
    }
}

static void rsa_keygen_benchmark(void)
{
    cb_status_t status      = CB_STATUS_FAIL;
    uint8_t *modulus        = g_CommonOutput;
    size_t modulus_buf_size = (8192u / 8u);                 // Expecting at most an 8k key

    uint8_t *private_exponent = modulus + modulus_buf_size; // Some ptr arithmetic, so that there is no buffer aliasing
    size_t private_exponent_buf_size = (8192u / 8u);

    size_t key_size = 0u;

    double perf_value[CB_RSA_PERF_DATA_CNT] = {0};
    char *perf_unit[CB_RSA_PERF_DATA_CNT]   = {CB_PERF_UNIT_SEC};

    size_t i = 0u;

#if !defined(CB_PRINT_CSV)
    PRINTF("RSA Generic Keygen:\r\n");
#endif
    for (; i < rsa_keygen_benchmark_cases_nb; i++)
    {
        key_size = rsa_keygen_benchmark_cases[i].key_size;

        perf_value[0] = TIME_AVG(
            status == CB_STATUS_SUCCESS,
            status = wrapper_rsa_keygen_compute(modulus, modulus_buf_size, private_exponent, private_exponent_buf_size,
                                                public_exponent, public_exponent_buf_size, key_size),
            CB_RSA_BENCHMARK_KEYGEN_ITERS, (void)0, (void)0);

        if (status != CB_STATUS_SUCCESS)
        {
            print_failure(NULL);
            continue;
        }

        print_benchmark_case(0u, CB_FALSE_STR, CB_RSA_ALGORITHM_STR, "", CB_MODE_KEYGEN_STR,
                             CB_KEY_OPACITY_TRANSPARENT_STR, key_size, "", perf_value, perf_unit,
                             CB_RSA_PERF_DATA_CNT);
    }
}
