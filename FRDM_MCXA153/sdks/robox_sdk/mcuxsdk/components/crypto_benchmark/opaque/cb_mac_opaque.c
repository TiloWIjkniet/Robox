/*
 * Copyright 2024 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/**
 * @file  cb_mac_opaque.c
 * @brief Contains the opaque MAC benchmark.
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

/*******************************************************************************
 * Variables
 ******************************************************************************/
__weak const mac_benchmark_cases_t mac_benchmark_cases[0];
__weak const size_t mac_benchmark_cases_nb = ARRAY_SIZE(mac_benchmark_cases);

/*******************************************************************************
 * Code
 ******************************************************************************/
__weak cb_status_t wrapper_mac_opaque_init(void **ctx_internal,
                                    cb_mac_algorithm_t algorithm,
                                    cb_hash_t hash_type,
                                    size_t key_size)
{
    (void)ctx_internal;
    (void)algorithm;
    (void)hash_type;
    (void)key_size;

    return CB_STATUS_FAIL;
}

__weak cb_status_t wrapper_mac_opaque_deinit(void *ctx_internal)
{
    (void)ctx_internal;

    return CB_STATUS_FAIL;
}

__weak cb_status_t wrapper_mac_opaque_compute(
    void *ctx_internal, const uint8_t *message, size_t message_size, uint8_t *mac, uint32_t key_id)
{
    (void)ctx_internal;
    (void)message;
    (void)message_size;
    (void)mac;
    (void)key_id;

    return CB_STATUS_FAIL;
}

__weak cb_status_t wrapper_mac_opaque_setup(void *ctx_internal)
{
    (void)ctx_internal;

    return CB_STATUS_FAIL;
}

__weak cb_status_t wrapper_mac_opaque_cleanup(void *ctx_internal)
{
    (void)ctx_internal;

    return CB_STATUS_FAIL;
}

__weak cb_status_t wrapper_mac_opaque_key_generate(void *ctx_internal, size_t key_size, uint32_t *key_id)
{
    (void)ctx_internal;
    (void)key_size;
    (void)key_id;

    return CB_STATUS_FAIL;
}

__weak cb_status_t wrapper_mac_opaque_key_delete(void *ctx_internal, uint32_t key_id)
{
    (void)ctx_internal;
    (void)key_id;

    return CB_STATUS_FAIL;
}

cb_status_t mac_benchmark_opaque(void)
{
    cb_status_t status = CB_STATUS_FAIL;
    (void)status;
    double cb        = 0;
    uint32_t key_id  = 0u;
    uint8_t *digest  = g_CommonOutput;
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
    PRINTF("MAC generation opaque:\r\n");
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
        if (wrapper_mac_opaque_init(&ctx_internal, algorithm, hash,
                                    key_size) != CB_STATUS_SUCCESS)
        {
            print_failure("wrapper_mac_opaque_init()");
            continue;
        }

        /* Set up opaque services */
        status = wrapper_mac_opaque_setup(ctx_internal);
        if (CB_STATUS_SUCCESS != status)
        {
            print_failure("wrapper_mac_opaque_setup()");
            goto setup_fail;
        }

        /* Generate a key */
        if (wrapper_mac_opaque_key_generate(ctx_internal, key_size, &key_id) != CB_STATUS_SUCCESS)
        {
            print_failure("wrapper_mac_opaque_key_generate()");
            goto key_gen_fail;
        }

        /* == == Begin individual benchmarks == == */

        cb = CYCLES_BYTE(status == CB_STATUS_SUCCESS,
                         status = wrapper_mac_opaque_compute(ctx_internal, message, message_size, digest, key_id),
                         message_size);

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
            CB_MODE_GENERATE_STR, CB_KEY_OPACITY_OPAQUE_STR, key_size, CB_FALSE_STR,
            perf_value, perf_unit, CB_MAC_PERF_DATA_CNT);

        cb = CYCLES_BYTE(status == CB_STATUS_SUCCESS,
                         status = wrapper_mac_opaque_compute(ctx_internal, message, message_size_multi, digest, key_id),
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
            CB_MODE_GENERATE_STR, CB_KEY_OPACITY_OPAQUE_STR, key_size, CB_FALSE_STR,
            perf_value, perf_unit, CB_MAC_PERF_DATA_CNT);

        /* == == == == == == == == == == == == == == == == == == == == == == */

fail:
        /* Delete a key */
        if (wrapper_mac_opaque_key_delete(ctx_internal, key_id) != CB_STATUS_SUCCESS)
        {
            print_failure("wrapper_mac_opaque_key_delete()");
        }

key_gen_fail:
        /* Clean up opaque services */
        status = wrapper_mac_opaque_cleanup(ctx_internal);
        if (CB_STATUS_SUCCESS != status)
        {
            print_failure("wrapper_mac_opaque_cleanup()");
        }

setup_fail:
        /* Clean up the port context before the next iteration */
        if (wrapper_mac_opaque_deinit(ctx_internal) != CB_STATUS_SUCCESS)
        {
            print_failure("wrapper_mac_opaque_deinit()");
        }
    }

    return status;
}
