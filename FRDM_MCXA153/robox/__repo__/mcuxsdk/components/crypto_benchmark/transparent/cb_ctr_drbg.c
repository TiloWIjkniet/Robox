/*
 * Copyright 2024 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "crypto_benchmark.h"
#include "cb_ctr_drbg.h"
#include "cb_types.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define CB_CTR_DRBG_ALGORITHM_STR "CTR-DRBG"
#define CB_CTR_DRBG_PERF_DATA_CNT 2u

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
__weak const ctr_drbg_benchmark_cases_t ctr_drbg_benchmark_cases[0];
__weak const size_t ctr_drbg_benchmark_cases_nb = ARRAY_SIZE(ctr_drbg_benchmark_cases);

/*******************************************************************************
 * Code
 ******************************************************************************/
__weak cb_status_t wrapper_ctr_drbg_init(void **ctx_internal)
{
    (void)ctx_internal;

    return CB_STATUS_FAIL;
}
__weak cb_status_t wrapper_ctr_drbg_deinit(void *ctx_internal)
{
    (void)ctx_internal;

    return CB_STATUS_FAIL;
}
__weak cb_status_t wrapper_ctr_drbg_compute(void *ctx_internal, uint8_t *output, size_t output_size)
{
    (void)ctx_internal;
    (void)output;
    (void)output_size;

    return CB_STATUS_FAIL;
}

cb_status_t ctr_drbg_benchmark(void)
{
    cb_status_t status = CB_STATUS_FAIL;
    double cb                                    = 0;
    size_t output_size                           = 0u;
    double perf_value[CB_CTR_DRBG_PERF_DATA_CNT] = {0};
    char *perf_unit[CB_CTR_DRBG_PERF_DATA_CNT]   = {CB_PERF_UNIT_CPB, CB_PERF_UNIT_KIBPS};

    void *ctx_internal = NULL;

    size_t i = 0u;

#if !defined(CB_PRINT_CSV)
    PRINTF("CTR DRBG:\r\n");
#endif
    for (; i < ctr_drbg_benchmark_cases_nb; i++)
    {
        output_size = ctr_drbg_benchmark_cases[i].output_size;

        /* Initialize the port context */
        if (wrapper_ctr_drbg_init(&ctx_internal) != CB_STATUS_SUCCESS)
        {
            print_failure("wrapper_ctr_drbg_init()");
            continue;
        }

        /* == == Begin individual benchmarks == == */

        cb = CYCLES_BYTE(status == CB_STATUS_SUCCESS,
                         status = wrapper_ctr_drbg_compute(ctx_internal, g_CommonOutput, output_size), output_size);

        perf_value[0] = cb;
        perf_value[1] = CYCLES_BYTE_TO_KB_S(cb, output_size);

        if (status != CB_STATUS_SUCCESS)
        {
            print_failure(NULL);
            goto fail;
        }

        print_benchmark_case(output_size, "", CB_CTR_DRBG_ALGORITHM_STR, "TRNG seeded", CB_MODE_GENERATE_STR, "", 0u, "", perf_value,
                             perf_unit, CB_CTR_DRBG_PERF_DATA_CNT);

        /* == == == == == == == == == == == == == == == == == == == == == == */

fail:
        /* Clean up the port context before the next iteration */
        if (wrapper_ctr_drbg_deinit(ctx_internal) != CB_STATUS_SUCCESS)
        {
            print_failure("wrapper_ctr_drbg_deinit()");
        }
    }

    return CB_STATUS_SUCCESS;
}
