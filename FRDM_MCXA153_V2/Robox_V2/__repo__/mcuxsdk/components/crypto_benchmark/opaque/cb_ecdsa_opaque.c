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
#include "cb_ecdsa.h"
#include "cb_types.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define CB_ECDSA_ALGORITHM_STR "ECDSA"
#define CB_ECDSA_PERF_DATA_CNT 1u

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void ecdsa_keygen_benchmark_opaque(void);
static void ecdsa_sign_verify_benchmark_opaque(void);

/*******************************************************************************
 * Variables
 ******************************************************************************/
__weak const ecdsa_benchmark_cases_t ecdsa_benchmark_cases[0];
__weak const size_t ecdsa_benchmark_cases_nb = ARRAY_SIZE(ecdsa_benchmark_cases);

__weak const ecdsa_keygen_benchmark_cases_t ecdsa_keygen_benchmark_cases[0];
__weak const size_t ecdsa_keygen_benchmark_cases_nb = ARRAY_SIZE(ecdsa_keygen_benchmark_cases);

/*******************************************************************************
 * Code
 ******************************************************************************/
__weak cb_status_t wrapper_ecdsa_opaque_init(void **ctx_internal,
                                             cb_ecdsa_type_t ecdsa_type,
                                             cb_hash_t hash_type,
                                             cb_ecc_family_t family,
                                             size_t key_size,
                                             cb_asym_sigver_input_type_t input_type)
{
    (void)ctx_internal;
    (void)ecdsa_type;
    (void)hash_type;
    (void)family;
    (void)key_size;
    (void)input_type;

    return CB_STATUS_FAIL;
}

__weak cb_status_t wrapper_ecdsa_opaque_deinit(void *ctx_internal)
{
    (void)ctx_internal;

    return CB_STATUS_FAIL;
}

__weak cb_status_t wrapper_ecdsa_keygen_opaque_compute(void *ctx_internal,
                                                       size_t key_size,
                                                       uint32_t *key_id,
                                                       uint8_t *public_key_addr,
                                                       size_t public_key_buf_size)
{
    (void)ctx_internal;
    (void)key_size;
    (void)key_id;
    (void)public_key_addr;
    (void)public_key_buf_size;

    return CB_STATUS_FAIL;
}

__weak cb_status_t wrapper_ecdsa_keygen_opaque_setup(void)
{
    return CB_STATUS_FAIL;
}

__weak cb_status_t wrapper_ecdsa_keygen_opaque_cleanup(void)
{
    return CB_STATUS_FAIL;
}

__weak cb_status_t wrapper_ecdsa_keygen_opaque_key_delete(uint32_t key_id)
{
    (void)key_id;

    return CB_STATUS_FAIL;
}

__weak cb_status_t wrapper_ecdsa_sign_opaque_setup(void *ctx_internal)
{
    (void)ctx_internal;

    return CB_STATUS_FAIL;
}

__weak cb_status_t wrapper_ecdsa_sign_opaque_cleanup(void *ctx_internal)
{
    (void)ctx_internal;

    return CB_STATUS_FAIL;
}

__weak cb_status_t wrapper_ecdsa_sign_opaque_key_generate(void *ctx_internal,
                                                          uint32_t *key_id,
                                                          size_t key_size,
                                                          uint8_t *public_key_addr,
                                                          size_t *public_key_size,
                                                          size_t public_key_buf_size)
{
    (void)ctx_internal;
    (void)key_id;
    (void)key_size;
    (void)public_key_addr;
    (void)public_key_size;
    (void)public_key_buf_size;

    return CB_STATUS_FAIL;
}

__weak cb_status_t wrapper_ecdsa_sign_opaque_key_delete(void *ctx_internal, uint32_t key_id)
{
    (void)ctx_internal;
    (void)key_id;

    return CB_STATUS_FAIL;
}

__weak cb_status_t wrapper_ecdsa_sign_opaque_compute(void *ctx_internal, const uint8_t *message, size_t message_size, uint8_t *signature, uint32_t key_id)
{
    (void)ctx_internal;
    (void)message;
    (void)message_size;
    (void)signature;
    (void)key_id;

    return CB_STATUS_FAIL;
}

__weak cb_status_t wrapper_ecdsa_verify_opaque_compute(void *ctx_internal, const uint8_t *message, size_t message_size, uint8_t *signature, uint8_t *public_key, size_t public_key_size)
{
    (void)ctx_internal;
    (void)message;
    (void)message_size;
    (void)signature;
    (void)public_key;
    (void)public_key_size;

    return CB_STATUS_FAIL;
}

cb_status_t ecdsa_benchmark_opaque(void)
{
    ecdsa_keygen_benchmark_opaque();

    ecdsa_sign_verify_benchmark_opaque();

    return CB_STATUS_SUCCESS;
}

static void ecdsa_sign_verify_benchmark_opaque()
{
    cb_status_t status = CB_STATUS_FAIL;
    uint32_t key_id = 0u;

    /* Set up the buffers */
    const size_t public_key_max_size = 2 * (1024u / 8u); // Save space for uncompressed 1k-bit keys
    size_t public_key_size           = 0u;
    uint8_t *public_key              = g_CommonOutput;
    uint8_t *signature               = public_key + public_key_max_size;
    uint8_t *message                 = g_CommonPayload;
    size_t message_size              = 0u;
    char *sign_mode                  = NULL;
    char *verify_mode                = NULL;

    void *ctx_internal                        = NULL;
    size_t key_size                           = 0u;
    cb_ecc_family_t family                    = 0;
    cb_ecdsa_type_t type                      = 0;
    cb_hash_t hash                            = 0;
    cb_asym_sigver_input_type_t input_type    = 0;
    double perf_value[CB_ECDSA_PERF_DATA_CNT] = {0};
    char *perf_unit[CB_ECDSA_PERF_DATA_CNT]   = {0};

    size_t i = 0u;

#if !defined(CB_PRINT_CSV)
    PRINTF("ECDSA Opaque Sign / Verify:\r\n");
#endif
    for (; i < ecdsa_benchmark_cases_nb; i++)
    {
        key_size = ecdsa_benchmark_cases[i].key_size;
        family = ecdsa_benchmark_cases[i].family;
        type = ecdsa_benchmark_cases[i].type;
        hash = ecdsa_benchmark_cases[i].hash;
        input_type = ecdsa_benchmark_cases[i].input_type;

        if (CB_SIGVER_MESSAGE == input_type)
        {
            message_size = CRYPTO_BENCHMARK_ECDSA_MSG_SIZE;
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
        if (wrapper_ecdsa_opaque_init(&ctx_internal, type, hash,
                                      family,
                                      key_size, input_type) != CB_STATUS_SUCCESS)
        {
            print_failure("wrapper_ecdsa_opaque_init()");
            continue;
        }

        /* Set up opaque services */
        status = wrapper_ecdsa_sign_opaque_setup(ctx_internal);
        if (CB_STATUS_SUCCESS != status)
        {
            print_failure("wrapper_ecdsa_sign_opaque_setup()");
            goto setup_fail;
        }

        /* Generate a key */
        if (wrapper_ecdsa_sign_opaque_key_generate(ctx_internal, &key_id, key_size, public_key,
                                                   &public_key_size, public_key_max_size) != CB_STATUS_SUCCESS)
        {
            print_failure("wrapper_ecdsa_sign_opaque_key_generate()");
            goto key_gen_fail;
        }

        /* == == Begin individual benchmarks == == */

        perf_value[0] =
            TIME_TOTAL_INTERVAL(status == CB_STATUS_SUCCESS,
                                status = wrapper_ecdsa_sign_opaque_compute(
                                    ctx_internal, message, message_size, signature, key_id));
        perf_unit[0] = CB_PERF_UNIT_SIGPS;

        if (status != CB_STATUS_SUCCESS)
        {
            print_failure(NULL);
            goto fail;
        }

        print_benchmark_case(message_size, CB_FALSE_STR, CB_ECDSA_ALGORITHM_STR,
                             ecc_family_to_string(family), sign_mode,
                             CB_KEY_OPACITY_OPAQUE_STR, key_size, "", perf_value,
                             perf_unit, CB_ECDSA_PERF_DATA_CNT);

        perf_value[0] = TIME_TOTAL_INTERVAL(
            status == CB_STATUS_SUCCESS,
            status = wrapper_ecdsa_verify_opaque_compute(ctx_internal, message, message_size,
                                                         signature, public_key, public_key_size));
        perf_unit[0] = CB_PERF_UNIT_VERIFPS;

        if (status != CB_STATUS_SUCCESS)
        {
            print_failure(NULL);
            goto fail;
        }

        print_benchmark_case(message_size, CB_FALSE_STR, CB_ECDSA_ALGORITHM_STR,
                             ecc_family_to_string(family), verify_mode,
                             CB_KEY_OPACITY_OPAQUE_STR, key_size, "", perf_value,
                             perf_unit, CB_ECDSA_PERF_DATA_CNT);

        /* == == == == == == == == == == == == == == == == == == == == == == */

fail:
        /* Delete a key */
        if (wrapper_ecdsa_sign_opaque_key_delete(ctx_internal, key_id) != CB_STATUS_SUCCESS)
        {
            print_failure("wrapper_ecdsa_sign_opaque_key_delete()");
        }

key_gen_fail:
        /* Clean up opaque services */
        status = wrapper_ecdsa_sign_opaque_cleanup(ctx_internal);
        if (CB_STATUS_SUCCESS != status)
        {
            print_failure("wrapper_ecdsa_sign_opaque_cleanup()");
        }

setup_fail:
        /* Clean up the port context before the next iteration */
        if (wrapper_ecdsa_opaque_deinit(ctx_internal) != CB_STATUS_SUCCESS)
        {
            print_failure("wrapper_ecdsa_opaque_deinit()");
        }
    }
}

static void ecdsa_keygen_benchmark_opaque()
{
    cb_status_t status = CB_STATUS_FAIL;
    uint32_t key_id             = 0u;
    void *ctx_internal          = NULL;
    const int millisecond_scale = 1000;

    size_t key_size                           = 0u;
    cb_ecc_family_t family                    = 0;
    double perf_value[CB_ECDSA_PERF_DATA_CNT] = {0};
    char *perf_unit[CB_ECDSA_PERF_DATA_CNT]   = {0};

    size_t i = 0u;

#if !defined(CB_PRINT_CSV)
    PRINTF("ECDSA Opaque Keygen:\r\n");
#endif
    for (; i < ecdsa_keygen_benchmark_cases_nb; i++)
    {
        key_size = ecdsa_keygen_benchmark_cases[i].key_size;
        family = ecdsa_keygen_benchmark_cases[i].family;

        perf_value[0] =
            millisecond_scale *
            TIME_AVG(
                status == CB_STATUS_SUCCESS,
                status = wrapper_ecdsa_keygen_opaque_compute(ctx_internal, key_size, &key_id,
                                                             g_CommonOutput, sizeof(g_CommonOutput)),
                CB_ECDSA_BENCHMARK_KEYGEN_ITERS,
                (wrapper_ecdsa_opaque_init(&ctx_internal, CB_ECDSA_RANDOMIZED, CB_HASH_TYPE_NO_HASH,
                                           family, key_size, CB_SIGVER_HASH),
                 wrapper_ecdsa_keygen_opaque_setup()),
                (wrapper_ecdsa_keygen_opaque_key_delete(key_id), wrapper_ecdsa_keygen_opaque_cleanup(),
                 wrapper_ecdsa_opaque_deinit(ctx_internal)));
        perf_unit[0] = CB_PERF_UNIT_MSEC;

        if (status != CB_STATUS_SUCCESS)
        {
            print_failure(NULL);
            continue;
        }

        print_benchmark_case(0u, CB_FALSE_STR, CB_ECDSA_ALGORITHM_STR,
                             ecc_family_to_string(family), CB_MODE_KEYGEN_STR,
                             CB_KEY_OPACITY_OPAQUE_STR, key_size, "", perf_value,
                             perf_unit, CB_ECDSA_PERF_DATA_CNT);
    }
}
