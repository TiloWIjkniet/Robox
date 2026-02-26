/*
 * Copyright 2024-2025 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/**
 * @file  cb_aead_opaque.c
 * @brief Contains definitions for opaque AEAD benchmarking.
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "crypto_benchmark.h"
#include "cb_aead.h"
#include "cb_types.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define CB_AEAD_ALGORITHM_STR "AEAD"
#define CB_AEAD_PERF_DATA_CNT 2u

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void aead_oneshot_benchmark_opaque(void);
static void aead_multipart_benchmark_opaque(void);

/*******************************************************************************
 * Variables
 ******************************************************************************/
__weak const aead_benchmark_cases_t aead_benchmark_cases[0];
__weak const size_t aead_benchmark_cases_nb = ARRAY_SIZE(aead_benchmark_cases);

/*******************************************************************************
 * Code
 ******************************************************************************/
__weak cb_status_t wrapper_aead_opaque_init(void **ctx_internal, cb_aead_type_t aead_type, size_t key_size)
{
    (void)ctx_internal;
    (void)aead_type;
    (void)key_size;

    return CB_STATUS_FAIL;
}

__weak cb_status_t wrapper_aead_opaque_deinit(void *ctx_internal)
{
    (void)ctx_internal;

    return CB_STATUS_FAIL;
}

__weak cb_status_t wrapper_aead_opaque_compute(void *ctx_internal,
                                        const uint8_t *message,
                                        size_t message_size,
                                        const uint8_t *iv,
                                        size_t iv_size,
                                        const uint8_t *aad,
                                        size_t aad_size,
                                        uint8_t *ciphertext,
                                        uint8_t *tag,
                                        size_t tag_size,
                                        uint32_t key_id)
{
    (void)ctx_internal;
    (void)message;
    (void)message_size;
    (void)iv;
    (void)iv_size;
    (void)aad;
    (void)aad_size;
    (void)ciphertext;
    (void)tag;
    (void)tag_size;
    (void)key_id;

    return CB_STATUS_FAIL;
}

__weak cb_status_t wrapper_aead_opaque_setup(void *ctx_internal)
{
    (void)ctx_internal;

    return CB_STATUS_FAIL;
}

__weak cb_status_t wrapper_aead_opaque_cleanup(void *ctx_internal)
{
    (void)ctx_internal;

    return CB_STATUS_FAIL;
}

__weak cb_status_t wrapper_aead_opaque_key_generate(void *ctx_internal, size_t key_size, uint32_t *key_id)
{
    (void)ctx_internal;
    (void)key_size;
    (void)ctx_internal;

    return CB_STATUS_FAIL;
}

__weak cb_status_t wrapper_aead_opaque_key_delete(void *ctx_internal, uint32_t key_id)
{
    (void)ctx_internal;

    return CB_STATUS_FAIL;
}

cb_status_t aead_benchmark_opaque(void)
{
    aead_oneshot_benchmark_opaque();

    aead_multipart_benchmark_opaque();

    return CB_STATUS_SUCCESS;
}

static void aead_oneshot_benchmark_opaque(void)
{
    cb_status_t status  = CB_STATUS_FAIL;
    double cb           = 0;
    uint32_t key_id     = 0u;
    uint8_t *tag        = g_CommonOutput;
    uint8_t *ciphertext = tag + AEAD_TAG_SIZE;
    uint8_t *iv         = g_CommonPayload;
    uint8_t *aad        = iv + AEAD_IV_SIZE;
    uint8_t *message    = aad + AEAD_AAD_SIZE;

    void *ctx_internal                       = NULL;
    cb_aead_type_t aead_type                 = 0;
    size_t key_size                          = 0u;
    size_t block_size                        = AEAD_BLOCK;
    size_t payload_size                      = AEAD_PAYLOAD;
    size_t message_size                      = payload_size;
    size_t message_size_multi                = (block_size * (MULTIPLE_BLOCK - 1) + payload_size);
    size_t message_size_perfcurve            = 0u;
    double perf_value[CB_AEAD_PERF_DATA_CNT] = {0};
    char *perf_unit[CB_AEAD_PERF_DATA_CNT]   = {CB_PERF_UNIT_CPB, CB_PERF_UNIT_KIBPS};

    size_t i = 0u;
    size_t k = 0u;

#if !defined(CB_PRINT_CSV)
    PRINTF("AEAD encryption opaque:\r\n");
#endif
    for (; i < aead_benchmark_cases_nb; i++)
    {
        key_size = aead_benchmark_cases[i].key_size;
        aead_type = aead_benchmark_cases[i].type;

        /* Initialize the port context */
        if (wrapper_aead_opaque_init(&ctx_internal, aead_type, key_size) != CB_STATUS_SUCCESS)
        {
            print_failure("wrapper_aead_opaque_init()");
            continue;
        }

        /* Set up opaque services */
        if (wrapper_aead_opaque_setup(ctx_internal) != CB_STATUS_SUCCESS)
        {
            print_failure("wrapper_aead_opaque_setup()");
            goto setup_fail;
        }

        /* Generate a key */
        if (wrapper_aead_opaque_key_generate(ctx_internal, key_size, &key_id) != CB_STATUS_SUCCESS)
        {
            print_failure("wrapper_aead_opaque_key_generate()");
            goto key_gen_fail;
        }

        /* == == Begin individual benchmarks == == */

        cb = CYCLES_BYTE(
            status == CB_STATUS_SUCCESS,
            status = wrapper_aead_opaque_compute(ctx_internal, message, message_size, iv, AEAD_IV_SIZE, aad,
                                                 AEAD_AAD_SIZE, ciphertext, tag, AEAD_TAG_SIZE, key_id),
            message_size);
        if (status != CB_STATUS_SUCCESS)
        {
            print_failure(NULL);
            goto fail;
        }

        perf_value[0] = cb;
        perf_value[1] = CYCLES_BYTE_TO_KB_S(cb, message_size);

        print_benchmark_case(message_size, CB_TRUE_STR, CB_AEAD_ALGORITHM_STR,
                             aead_to_string(aead_type), CB_MODE_ENCRYPT_STR,
                             CB_KEY_OPACITY_OPAQUE_STR, key_size, CB_FALSE_STR, perf_value, perf_unit,
                             CB_AEAD_PERF_DATA_CNT);

        cb = CYCLES_BYTE(
            status == CB_STATUS_SUCCESS,
            status = wrapper_aead_opaque_compute(ctx_internal, message, message_size_multi, iv, AEAD_IV_SIZE, aad,
                                                 AEAD_AAD_SIZE, ciphertext, tag, AEAD_TAG_SIZE, key_id),
            message_size_multi);

        if (status != CB_STATUS_SUCCESS)
        {
            print_failure(NULL);
            goto fail;
        }

        perf_value[0] = cb;
        perf_value[1] = CYCLES_BYTE_TO_KB_S(cb, message_size_multi);

        print_benchmark_case(message_size_multi, CB_TRUE_STR, CB_AEAD_ALGORITHM_STR,
                             aead_to_string(aead_type), CB_MODE_ENCRYPT_STR,
                             CB_KEY_OPACITY_OPAQUE_STR, key_size, CB_FALSE_STR, perf_value, perf_unit,
                             CB_AEAD_PERF_DATA_CNT);

#if defined(CRYPTO_BENCHMARK_PERFCURVE) && (CRYPTO_BENCHMARK_PERFCURVE == 1u)
        for (k = 0u; k < (sizeof(g_cb_perfcurve_block_counts) / sizeof(size_t)); k++)
        {
            message_size_perfcurve = (block_size * (g_cb_perfcurve_block_counts[k] - 1) + payload_size);

            cb = CYCLES_BYTE(
                status == CB_STATUS_SUCCESS,
                status = wrapper_aead_opaque_compute(ctx_internal, message, message_size_perfcurve, iv, AEAD_IV_SIZE,
                                                     aad, AEAD_AAD_SIZE, ciphertext, tag, AEAD_TAG_SIZE, key_id),
                message_size_perfcurve);
            perf_value[0] = cb;
            perf_value[1] = CYCLES_BYTE_TO_KB_S(cb, message_size_perfcurve);

            if (status != CB_STATUS_SUCCESS)
            {
                print_failure(NULL);
                continue;
            }

            print_benchmark_case(message_size_perfcurve, CB_TRUE_STR, CB_AEAD_ALGORITHM_STR,
                                 aead_to_string(aead_type), CB_MODE_ENCRYPT_STR,
                                 CB_KEY_OPACITY_OPAQUE_STR, key_size, CB_FALSE_STR, perf_value, perf_unit,
                                 CB_AEAD_PERF_DATA_CNT);
        }
#endif /* CRYPTO_BENCHMARK_PERFCURVE */

        /* == == == == == == == == == == == == == == == == == == == == == */

fail:
        /* Delete a key */
        if (wrapper_aead_opaque_key_delete(ctx_internal, key_id) != CB_STATUS_SUCCESS)
        {
            print_failure("wrapper_aead_opaque_key_delete()");
        }

key_gen_fail:
        /* Clean up opaque services */
        if (wrapper_aead_opaque_cleanup(ctx_internal) != CB_STATUS_SUCCESS)
        {
            print_failure("wrapper_aead_opaque_cleanup()");
        }

setup_fail:
        /* Clean up the port context before the next iteration */
        if (wrapper_aead_opaque_deinit(ctx_internal) != CB_STATUS_SUCCESS)
        {
            print_failure("wrapper_aead_opaque_deinit()");
        }
    }
}

static void aead_multipart_benchmark_opaque(void)
{
    // Placeholder
}
