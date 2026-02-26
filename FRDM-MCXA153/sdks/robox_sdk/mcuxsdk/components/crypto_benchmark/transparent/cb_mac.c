/*
 * Copyright 2024-2025 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/**
 * @file  cb_mac.c
 * @brief Contains the transparent MAC benchmark.
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "crypto_benchmark.h"
#include "cb_mac.h"
#include "cb_types.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define CB_MAC_PERF_DATA_CNT 2u

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void mac_oneshot_benchmark(void);
static void mac_multipart_benchmark(void);

/**
 * @brief An ELE specific benchmark for the transparent key FastMAC API.
 */
static void mac_fast_mac_benchmark(void);

/*******************************************************************************
 * Variables
 ******************************************************************************/
__weak const mac_benchmark_cases_t mac_benchmark_cases[0];
__weak const size_t mac_benchmark_cases_nb = ARRAY_SIZE(mac_benchmark_cases);

/*******************************************************************************
 * Code
 ******************************************************************************/
__weak cb_status_t wrapper_mac_init(void **ctx_internal,
                                    cb_mac_algorithm_t algorithm,
                                    cb_hash_t hash_type,
                                    const uint8_t *key,
                                    size_t key_size)
{
    (void)ctx_internal;
    (void)algorithm;
    (void)hash_type;
    (void)key;
    (void)key_size;

    return CB_STATUS_FAIL;
}

__weak cb_status_t wrapper_mac_deinit(void *ctx_internal)
{
    (void)ctx_internal;

    return CB_STATUS_FAIL;
}

__weak cb_status_t wrapper_mac_compute(
    void *ctx_internal, const uint8_t *message, size_t message_size, uint8_t *mac, const uint8_t *key)
{
    (void)ctx_internal;
    (void)message;
    (void)message_size;
    (void)mac;
    (void)key;

    return CB_STATUS_FAIL;
}

cb_status_t mac_benchmark(void)
{
    mac_oneshot_benchmark();

    mac_multipart_benchmark();

    mac_fast_mac_benchmark();

    return CB_STATUS_SUCCESS;
}

static void mac_oneshot_benchmark(void)
{
    cb_status_t status  = CB_STATUS_FAIL;
    double cb        = 0;
    uint8_t *digest  = g_CommonOutput;
    uint8_t *key     = digest + 1024u; // For the key, use the output buffer, as we may need the entire payload
    uint8_t *message = g_CommonPayload;

    void *ctx_internal                      = NULL;
    size_t key_size                         = 0u;
    cb_mac_algorithm_t algorithm            = 0;
    cb_hash_t hash                          = 0;
    size_t block_size                       = 0u;
    size_t payload_size                     = 0u;
    size_t message_size                     = 0u;
    size_t message_size_multi               = 0u;
    double perf_value[CB_MAC_PERF_DATA_CNT] = {0};
    char *perf_unit[CB_MAC_PERF_DATA_CNT]   = {CB_PERF_UNIT_CPB, CB_PERF_UNIT_KIBPS};

    size_t i = 0u;

#if !defined(CB_PRINT_CSV)
    PRINTF("MAC generation:\r\n");
#endif
    for (; i < mac_benchmark_cases_nb; i++)
    {
        key_size = mac_benchmark_cases[i].key_size;
        algorithm = mac_benchmark_cases[i].algorithm;
        hash = mac_benchmark_cases[i].hash;

        /* We need to get the underlying algorithm block size */
        if (convert_mac_type_to_block_size(algorithm, hash, &block_size,
                                           &payload_size) != CB_STATUS_SUCCESS)
        {
            print_failure("test case not supported");
            continue;
        }
        message_size       = payload_size;
        message_size_multi = (block_size * (MULTIPLE_BLOCK - 1) + payload_size);

        /* Initialize the port context */
        if (wrapper_mac_init(&ctx_internal, algorithm, hash,
                             key, key_size) != CB_STATUS_SUCCESS)
        {
            print_failure("wrapper_mac_init()");
            continue;
        }

        /* == == Begin individual benchmarks == == */

        cb = CYCLES_BYTE(status == CB_STATUS_SUCCESS,
                         status = wrapper_mac_compute(ctx_internal, message, message_size, digest, key), message_size);

        if (status != CB_STATUS_SUCCESS)
        {
            print_failure(NULL);
            goto fail;
        }

        perf_value[0] = cb;
        perf_value[1] = CYCLES_BYTE_TO_KB_S(cb, message_size);

        print_benchmark_case(
            message_size, "", mac_to_string(algorithm),
            mac_algo_specifier_to_string(algorithm, hash),
            CB_MODE_GENERATE_STR, CB_KEY_OPACITY_TRANSPARENT_STR, key_size, CB_FALSE_STR,
            perf_value, perf_unit, CB_MAC_PERF_DATA_CNT);

        cb = CYCLES_BYTE(status == CB_STATUS_SUCCESS,
                         status = wrapper_mac_compute(ctx_internal, message, message_size_multi, digest, key),
                         message_size_multi);

        if (status != CB_STATUS_SUCCESS)
        {
            print_failure(NULL);
            goto fail;
        }

        perf_value[0] = cb;
        perf_value[1] = CYCLES_BYTE_TO_KB_S(cb, message_size_multi);

        print_benchmark_case(
            message_size_multi, "", mac_to_string(algorithm),
            mac_algo_specifier_to_string(algorithm, hash),
            CB_MODE_GENERATE_STR, CB_KEY_OPACITY_TRANSPARENT_STR, key_size, CB_FALSE_STR,
            perf_value, perf_unit, CB_MAC_PERF_DATA_CNT);

        /* == == == == == == == == == == == == == == == == == == == == == == */

fail:
        /* Clean up the port context before the next iteration */
        if (wrapper_mac_deinit(ctx_internal) != CB_STATUS_SUCCESS)
        {
            print_failure("wrapper_mac_deinit()");
        }
    }
}

static void mac_multipart_benchmark(void)
{
    // Placeholder
}

__weak cb_status_t wrapper_fast_mac_start(const uint8_t *key)
{
    (void)key;

    return CB_STATUS_FAIL;
}

__weak cb_status_t wrapper_fast_mac_proceed(const uint8_t *message, size_t message_size, uint8_t *mac)
{
    (void)message;
    (void)message_size;
    (void)mac;

    return CB_STATUS_FAIL;
}

__weak cb_status_t wrapper_fast_mac_end(void)
{
    return CB_STATUS_FAIL;
}

static void mac_fast_mac_benchmark(void)
{
    cb_status_t status                      = CB_STATUS_FAIL;
    double cb                               = 0;
    uint8_t *digest                         = g_CommonOutput;
    uint8_t *key                            = digest + 1024u;
    uint8_t *message                        = g_CommonPayload;
    size_t block_size                       = 0u;
    size_t payload_size                     = 0u;
    size_t message_size                     = 0u;
    size_t message_size_multi               = 0u;
    double perf_value[CB_MAC_PERF_DATA_CNT] = {0};
    char *perf_unit[CB_MAC_PERF_DATA_CNT]   = {CB_PERF_UNIT_CPB, CB_PERF_UNIT_KIBPS};
    char fast_mac_str[]                     = "FastMAC-SHA256";
    size_t fastmac_key_size                 = 256u;
    size_t additional_payload_sizes[]       = CB_FAST_MAC_ADDITIONAL_PAYLOAD_SIZES;
    double time_spent                       = 0;
    double finish_cycles                    = 0;

    size_t i = 0u;

#if !defined(CB_PRINT_CSV)
    /* FastMAC can only do SHA256 HMAC */
    PRINTF("FastMAC generation:\r\n");
#endif

    /* We need to get the underlying algorithm block size */
    if (convert_mac_type_to_block_size(CB_MAC_ALGORITHM_HMAC, CB_HASH_TYPE_SHA256, &block_size, &payload_size) !=
        CB_STATUS_SUCCESS)
    {
        print_failure("FastMAC-HMAC_SHA256-256: test case not supported");
        return;
    }
    message_size       = payload_size;
    message_size_multi = (block_size * (MULTIPLE_BLOCK - 1) + payload_size);

    /* Fast MAC Start */
    status = wrapper_fast_mac_start(key);
    if (status != CB_STATUS_SUCCESS)
    {
        print_failure("wrapper_fast_mac_start()");
        goto fail;
    }

    /* Benchmark including Start and Finish (= same as Generic + 1 call for finish) */
    // double update_cycles = CYCLES_COUNT(wrapper_fast_mac_proceed(g_CommonPayload, HMAC_SHA256_PAYLOAD, digest));
    // double finish_cycles = CYCLES_COUNT(wrapper_fast_mac_end());
    // PRINTF("%f Cycles/Byte\r\n", (setup_cycles + update_cycles + finish_cycles) / HMAC_SHA256_BLOCK);

    cb = CYCLES_BYTE(status == CB_STATUS_SUCCESS,
                     status = wrapper_fast_mac_proceed(message, message_size, digest), message_size);

    if (status != CB_STATUS_SUCCESS)
    {
        print_failure("wrapper_fast_mac_proceed()");
        goto fail;
    }

    perf_value[0] = cb;
    perf_value[1] = CYCLES_BYTE_TO_KB_S(cb, message_size);

    print_benchmark_case(message_size, "", mac_to_string(CB_MAC_ALGORITHM_HMAC), fast_mac_str, CB_MODE_GENERATE_STR,
                         CB_KEY_OPACITY_TRANSPARENT_STR, fastmac_key_size, CB_TRUE_STR, perf_value, perf_unit,
                         CB_MAC_PERF_DATA_CNT);

    cb = CYCLES_BYTE(status == CB_STATUS_SUCCESS,
                     status = wrapper_fast_mac_proceed(message, message_size_multi, digest), message_size_multi);

    if (status != CB_STATUS_SUCCESS)
    {
        print_failure("wrapper_fast_mac_proceed()");
        goto fail;
    }

    perf_value[0] = cb;
    perf_value[1] = CYCLES_BYTE_TO_KB_S(cb, message_size_multi);

    print_benchmark_case(message_size_multi, "", mac_to_string(CB_MAC_ALGORITHM_HMAC), fast_mac_str,
                         CB_MODE_GENERATE_STR, CB_KEY_OPACITY_TRANSPARENT_STR, fastmac_key_size, CB_TRUE_STR,
                         perf_value, perf_unit, CB_MAC_PERF_DATA_CNT);

    /* Some FastMAC-specific measurements */
    for (; i < sizeof(additional_payload_sizes) / sizeof(additional_payload_sizes[0]); i++)
    {
        message_size_multi = additional_payload_sizes[i];
        time_spent         = TIME_TOTAL_USEC(status = wrapper_fast_mac_proceed(message, message_size_multi, digest));

        if (status != CB_STATUS_SUCCESS)
        {
            print_failure("wrapper_fast_mac_proceed()");
            continue;
        }

        perf_value[0] = time_spent;
        perf_value[1] = 0;
        perf_unit[0]  = CB_PERF_UNIT_USEC;

        print_benchmark_case(message_size_multi, "", mac_to_string(CB_MAC_ALGORITHM_HMAC), fast_mac_str,
                             CB_MODE_GENERATE_STR, CB_KEY_OPACITY_TRANSPARENT_STR, fastmac_key_size, CB_TRUE_STR,
                             perf_value, perf_unit, 1u);
    }

fail:
    /* Fast MAC Finish */
    finish_cycles = CYCLES_COUNT(status = wrapper_fast_mac_end());
    if (status != CB_STATUS_SUCCESS)
    {
        print_failure("wrapper_fast_mac_end()");
    }
}
