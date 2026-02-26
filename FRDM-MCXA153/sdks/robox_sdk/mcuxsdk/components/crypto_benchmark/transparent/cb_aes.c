/*
 * Copyright 2024-2025 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/**
 * @file  cb_aes.c
 * @brief Contains definitions for AES benchmarking.
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "crypto_benchmark.h"
#include "cb_aes.h"
#include "cb_types.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define CB_AES_ALGORITHM_STR "AES"
#define CB_AES_PERF_DATA_CNT 2u

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void aes_oneshot_benchmark(void);
static void aes_multipart_benchmark(void);

/*******************************************************************************
 * Variables
 ******************************************************************************/
__weak const aes_benchmark_cases_t aes_benchmark_cases[0];
__weak const size_t aes_benchmark_cases_nb = ARRAY_SIZE(aes_benchmark_cases);

/*******************************************************************************
 * Code
 ******************************************************************************/
__weak cb_status_t wrapper_aes_init(void **ctx_internal, cb_cipher_type_t cipher_type, const uint8_t *key, size_t key_size)
{
    (void)ctx_internal;
    (void)cipher_type;
    (void)key;
    (void)key_size;

    return CB_STATUS_FAIL;
}

__weak cb_status_t wrapper_aes_deinit(void *ctx_internal)
{
    (void)ctx_internal;

    return CB_STATUS_FAIL;
}

__weak cb_status_t wrapper_aes_compute(void *ctx_internal,
                                       const uint8_t *message,
                                       size_t message_size,
                                       const uint8_t *iv,
                                       const uint8_t *key,
                                       uint8_t *ciphertext)
{
    (void)ctx_internal;
    (void)message;
    (void)message_size;
    (void)iv;
    (void)key;
    (void)ciphertext;

    return CB_STATUS_FAIL;
}

cb_status_t aes_benchmark(void)
{
    aes_oneshot_benchmark();

    aes_multipart_benchmark();

    return CB_STATUS_SUCCESS;
}

static void aes_oneshot_benchmark(void)
{
    cb_status_t status  = CB_STATUS_FAIL;
    double cb           = 0;
    uint8_t *ciphertext = g_CommonOutput;
    uint8_t *key        = g_CommonPayload;
    uint8_t *iv         = key + 32u; // AES max key size is 32B
    uint8_t *message    = iv + 16u;  // AES IV is 16B

    void *ctx_internal                      = NULL;
    cb_cipher_type_t cipher_type            = 0;
    size_t key_size                         = 0u;
    size_t block_size                       = AES_BLOCK;
    size_t payload_size                     = AES_PAYLOAD;
    size_t message_size                     = payload_size;
    size_t message_size_multi               = (block_size * (MULTIPLE_BLOCK - 1) + payload_size);
    size_t message_size_perfcurve           = 0u;
    double perf_value[CB_AES_PERF_DATA_CNT] = {0};
    char *perf_unit[CB_AES_PERF_DATA_CNT]   = {CB_PERF_UNIT_CPB, CB_PERF_UNIT_KIBPS};

    size_t i = 0u;
    size_t k = 0u;

#if !defined(CB_PRINT_CSV)
    PRINTF("AES encryption:\r\n");
#endif
    for (; i < aes_benchmark_cases_nb; i++)
    {
        key_size = aes_benchmark_cases[i].key_size;
        cipher_type = aes_benchmark_cases[i].type;

        /* Initialize the port context */
        if (wrapper_aes_init(&ctx_internal, cipher_type, key, key_size) != CB_STATUS_SUCCESS)
        {
            print_failure("wrapper_aes_init()");
            continue;
        }

        /* == == Begin individual benchmarks == == */

        cb = CYCLES_BYTE(status == CB_STATUS_SUCCESS,
                         status = wrapper_aes_compute(ctx_internal, message, message_size, iv, key, ciphertext),
                         message_size);

        if (status != CB_STATUS_SUCCESS)
        {
            print_failure(NULL);
            goto fail;
        }

        perf_value[0] = cb;
        perf_value[1] = CYCLES_BYTE_TO_KB_S(cb, message_size);

        print_benchmark_case(message_size, CB_TRUE_STR, CB_AES_ALGORITHM_STR,
                             cipher_to_string(cipher_type), CB_MODE_ENCRYPT_STR,
                             CB_KEY_OPACITY_TRANSPARENT_STR, key_size, CB_FALSE_STR, perf_value, perf_unit,
                             CB_AES_PERF_DATA_CNT);

        cb = CYCLES_BYTE(
            status == CB_STATUS_SUCCESS,
            status = wrapper_aes_compute(ctx_internal, message, message_size_multi, iv, key, ciphertext),
            message_size_multi);

        if (status != CB_STATUS_SUCCESS)
        {
            print_failure(NULL);
            goto fail;
        }

        perf_value[0] = cb;
        perf_value[1] = CYCLES_BYTE_TO_KB_S(cb, message_size_multi);

        print_benchmark_case(message_size_multi, CB_TRUE_STR, CB_AES_ALGORITHM_STR,
                             cipher_to_string(cipher_type), CB_MODE_ENCRYPT_STR,
                             CB_KEY_OPACITY_TRANSPARENT_STR, key_size, CB_FALSE_STR, perf_value, perf_unit,
                             CB_AES_PERF_DATA_CNT);

#if defined(CRYPTO_BENCHMARK_PERFCURVE) && (CRYPTO_BENCHMARK_PERFCURVE == 1u)
        for (k = 0u; k < (sizeof(g_cb_perfcurve_block_counts) / sizeof(size_t)); k++)
        {
            message_size_perfcurve = (block_size * (g_cb_perfcurve_block_counts[k] - 1) + payload_size);

            cb = CYCLES_BYTE(
                status == CB_STATUS_SUCCESS,
                status = wrapper_aes_compute(ctx_internal, message, message_size_perfcurve, iv, key, ciphertext),
                                             message_size_perfcurve);

            if (status != CB_STATUS_SUCCESS)
            {
                print_failure(NULL);
                continue;
            }

            perf_value[0] = cb;
            perf_value[1] = CYCLES_BYTE_TO_KB_S(cb, message_size_perfcurve);

            print_benchmark_case(message_size_perfcurve, CB_TRUE_STR, CB_AES_ALGORITHM_STR,
                                 cipher_to_string(cipher_type), CB_MODE_ENCRYPT_STR,
                                 CB_KEY_OPACITY_TRANSPARENT_STR, key_size, CB_FALSE_STR, perf_value,
                                 perf_unit, CB_AES_PERF_DATA_CNT);
        }
#endif /* CRYPTO_BENCHMARK_PERFCURVE */

        /* == == == == == == == == == == == == == == == == == == == == == */

fail:
        /* Clean up the port context before the next iteration */
        if (wrapper_aes_deinit(ctx_internal) != CB_STATUS_SUCCESS)
        {
            print_failure("wrapper_aes_deinit()");
        }
    }
}

static void aes_multipart_benchmark(void)
{
    // Placeholder
}
