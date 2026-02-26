/*
 * Copyright 2024-2025 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/**
 * @file  cb_hash.c
 * @brief Contains definitions for HASH benchmarking.
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "crypto_benchmark.h"
#include "cb_hash.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define CB_HASH_PERF_DATA_CNT 2u

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void hash_oneshot_benchmark(void);
static void hash_multipart_benchmark(void);

/*******************************************************************************
 * Variables
 ******************************************************************************/
__weak const hash_benchmark_cases_t hash_benchmark_cases[0];
__weak const size_t hash_benchmark_cases_nb = ARRAY_SIZE(hash_benchmark_cases);

/*******************************************************************************
 * Code
 ******************************************************************************/
__weak cb_status_t wrapper_hash_init(void **ctx_internal, cb_hash_t hash_type)
{
    (void)ctx_internal;
    (void)hash_type;

    return CB_STATUS_FAIL;
}

__weak cb_status_t wrapper_hash_deinit(void *ctx_internal)
{
    (void)ctx_internal;

    return CB_STATUS_FAIL;
}

__weak cb_status_t wrapper_hash_compute(void *ctx_internal, const uint8_t *message, size_t message_size, uint8_t *hash)
{
    (void)ctx_internal;
    (void)message;
    (void)message_size;
    (void)hash;

    return CB_STATUS_FAIL;
}

__weak cb_status_t wrapper_hash_init_multipart(void **ctx_internal, cb_hash_t hash_type)
{
    (void)ctx_internal;
    (void)hash_type;

    return CB_STATUS_FAIL;
}

__weak cb_status_t wrapper_hash_deinit_multipart(void *ctx_internal)
{
    (void)ctx_internal;

    return CB_STATUS_FAIL;
}

__weak cb_status_t wrapper_hash_start_multipart_compute(void *ctx_internal)
{
    (void)ctx_internal;

    return CB_STATUS_FAIL;
}

__weak cb_status_t wrapper_hash_update_multipart_compute(void *ctx_internal,
                                                         const uint8_t *message,
                                                         size_t message_size,
                                                         uint8_t *hash)
{
    (void)ctx_internal;
    (void)message;
    (void)message_size;
    (void)hash;

    return CB_STATUS_FAIL;
}

__weak cb_status_t wrapper_hash_finish_multipart_compute(void *ctx_internal, uint8_t *hash)
{
    (void)ctx_internal;
    (void)hash;

    return CB_STATUS_FAIL;
}

cb_status_t hash_benchmark(void)
{
    hash_oneshot_benchmark();

    hash_multipart_benchmark();

    return CB_STATUS_SUCCESS;
}

static void hash_oneshot_benchmark(void)
{
    cb_status_t status  = CB_STATUS_FAIL;
    double cb        = 0;
    uint8_t *digest  = g_CommonOutput;
    uint8_t *message = g_CommonPayload;

    void *ctx_internal                       = NULL;
    cb_hash_t hash                           = CB_HASH_TYPE_NO_HASH;
    size_t block_size                        = 0u;
    size_t payload_size                      = 0u;
    size_t message_size                      = 0u;
    size_t message_size_multi                = 0u;
    size_t message_size_perfcurve            = 0u;
    double perf_value[CB_HASH_PERF_DATA_CNT] = {0};
    char *perf_unit[CB_HASH_PERF_DATA_CNT]   = {CB_PERF_UNIT_CPB, CB_PERF_UNIT_KIBPS};

    size_t i = 0u;
    size_t j = 0u;

#if !defined(CB_PRINT_CSV)
    PRINTF("HASH generation:\r\n");
#endif
    for (; i < hash_benchmark_cases_nb; i++)
    {
        hash = hash_benchmark_cases[i].hash;

        /* We need to get the underlying algorithm block size */
        if (convert_hash_type_to_block_size(hash, &block_size, &payload_size) !=
            CB_STATUS_SUCCESS)
        {
            print_failure("test case not supported");
            continue;
        }
        message_size       = payload_size;
        message_size_multi = (block_size * (MULTIPLE_BLOCK - 1) + payload_size);

        /* Initialize the port context */
        if (wrapper_hash_init(&ctx_internal, hash) != CB_STATUS_SUCCESS)
        {
            print_failure("wrapper_hash_init()");
            continue;
        }

        /* == == Begin individual benchmarks == == */

        cb = CYCLES_BYTE(status == CB_STATUS_SUCCESS,
                         status = wrapper_hash_compute(ctx_internal, message, message_size, digest), message_size);

        if (status != CB_STATUS_SUCCESS)
        {
            print_failure(NULL);
            goto fail;
        }

        perf_value[0] = cb;
        perf_value[1] = CYCLES_BYTE_TO_KB_S(cb, message_size);

        print_benchmark_case(message_size, "", hash_to_string(hash), "", CB_MODE_GENERATE_STR, "", 0u,
                             CB_FALSE_STR, perf_value, perf_unit, CB_HASH_PERF_DATA_CNT);

        cb = CYCLES_BYTE(status == CB_STATUS_SUCCESS,
                         status = wrapper_hash_compute(ctx_internal, message, message_size_multi, digest),
                         message_size_multi);

        if (status != CB_STATUS_SUCCESS)
        {
            print_failure(NULL);
            goto fail;
        }

        perf_value[0] = cb;
        perf_value[1] = CYCLES_BYTE_TO_KB_S(cb, message_size_multi);

        print_benchmark_case(message_size_multi, "", hash_to_string(hash), "", CB_MODE_GENERATE_STR, "", 0u,
                             CB_FALSE_STR, perf_value, perf_unit, CB_HASH_PERF_DATA_CNT);

#if defined(CRYPTO_BENCHMARK_PERFCURVE) && (CRYPTO_BENCHMARK_PERFCURVE == 1u)
        for (j = 0u; j < (sizeof(g_cb_perfcurve_block_counts) / sizeof(size_t)); j++)
        {
            message_size_perfcurve = (block_size * (g_cb_perfcurve_block_counts[j] - 1) + payload_size);

            cb = CYCLES_BYTE(status == CB_STATUS_SUCCESS,
                             status = wrapper_hash_compute(ctx_internal, message, message_size_perfcurve, digest),
                             message_size_perfcurve);
            if (status != CB_STATUS_SUCCESS)
            {
                print_failure(NULL);
                continue;
            }

            perf_value[0] = cb;
            perf_value[1] = CYCLES_BYTE_TO_KB_S(cb, message_size_perfcurve);

            print_benchmark_case(message_size_perfcurve, "", hash_to_string(hash), "", CB_MODE_GENERATE_STR, "",
                                 0u, CB_FALSE_STR, perf_value, perf_unit, CB_HASH_PERF_DATA_CNT);
        }
#endif /* CRYPTO_BENCHMARK_PERFCURVE */

        /* == == == == == == == == == == == == == == == == == == == == == == */

fail:
        /* Clean up the port context before the next iteration */
        if (wrapper_hash_deinit(ctx_internal) != CB_STATUS_SUCCESS)
        {
            print_failure("wrapper_hash_deinit()");
        }
    }
}

static void hash_multipart_benchmark(void)
{
    cb_status_t status_setup, status_update, status_finish;
    double cb            = 0;
    double start_cycles  = 0;
    double update_cycles = 0;
    double finish_cycles = 0;
    uint8_t *digest      = g_CommonOutput;
    uint8_t *message     = g_CommonPayload;

    void *ctx_internal                       = NULL;
    cb_hash_t hash                           = CB_HASH_TYPE_NO_HASH;
    size_t block_size                        = 0u;
    size_t payload_size                      = 0u;
    size_t message_size                      = 0u;
    size_t message_size_multi                = 0u;
    double perf_value[CB_HASH_PERF_DATA_CNT] = {0};
    char *perf_unit[CB_HASH_PERF_DATA_CNT]   = {CB_PERF_UNIT_CPB, CB_PERF_UNIT_KIBPS};

    size_t i = 0u;

#if !defined(CB_PRINT_CSV)
    PRINTF("HASH generation (Multi-Part):\r\n");
#endif
    for (; i < hash_benchmark_cases_nb; i++)
    {
        hash = hash_benchmark_cases[i].hash;

        /* We need to get the underlying algorithm block size */
        if (convert_hash_type_to_block_size(hash, &block_size, &payload_size) !=
            CB_STATUS_SUCCESS)
        {
            print_failure("test case not supported");
            continue;
        }
        message_size       = payload_size;
        message_size_multi = (block_size * (MULTIPLE_BLOCK - 1) + payload_size);

        /* Initialize the port context */
        if (wrapper_hash_init_multipart(&ctx_internal, hash) != CB_STATUS_SUCCESS)
        {
            print_failure("wrapper_hash_init_multipart()");
            continue;
        }

        /* == == Begin individual benchmarks == == */

        start_cycles  = CYCLES_COUNT(status_setup = wrapper_hash_start_multipart_compute(ctx_internal));
        update_cycles = CYCLES_COUNT(
            status_update = wrapper_hash_update_multipart_compute(ctx_internal, message, message_size, digest));
        finish_cycles =
            CYCLES_COUNT(status_finish = wrapper_hash_finish_multipart_compute(ctx_internal, digest));

        if (status_setup != CB_STATUS_SUCCESS || status_update != CB_STATUS_SUCCESS || status_finish != CB_STATUS_SUCCESS)
        {
            print_failure(NULL);
            goto fail;
        }

        cb            = (start_cycles + update_cycles + finish_cycles) / message_size;
        perf_value[0] = cb;
        perf_value[1] = CYCLES_BYTE_TO_KB_S(cb, message_size);

        print_benchmark_case(message_size, "", hash_to_string(hash), "", CB_MODE_GENERATE_STR, "", 0u,
                             CB_TRUE_STR, perf_value, perf_unit, CB_HASH_PERF_DATA_CNT);

        start_cycles  = CYCLES_COUNT(status_setup = wrapper_hash_start_multipart_compute(ctx_internal));
        update_cycles = CYCLES_COUNT(
            status_update = wrapper_hash_update_multipart_compute(ctx_internal, message, message_size_multi, digest));
        finish_cycles = CYCLES_COUNT(status_finish = wrapper_hash_finish_multipart_compute(ctx_internal, digest));

        if (status_setup != CB_STATUS_SUCCESS || status_update != CB_STATUS_SUCCESS || status_finish != CB_STATUS_SUCCESS)
        {
            print_failure(NULL);
            goto fail;
        }

        cb            = (start_cycles + update_cycles + finish_cycles) / message_size_multi;
        perf_value[0] = cb;
        perf_value[1] = CYCLES_BYTE_TO_KB_S(cb, message_size_multi);

        print_benchmark_case(message_size_multi, "", hash_to_string(hash), "", CB_MODE_GENERATE_STR, "", 0u,
                             CB_TRUE_STR, perf_value, perf_unit, CB_HASH_PERF_DATA_CNT);

        /* == == == == == == == == == == == == == == == == == == == == == == */

fail:
        /* Clean up the port context before the next iteration */
        if (wrapper_hash_deinit_multipart(ctx_internal) != CB_STATUS_SUCCESS)
        {
            print_failure("wrapper_hash_deinit_multipart()");
        }
    }
}
