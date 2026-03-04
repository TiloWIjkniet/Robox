/*
 *  Benchmark demonstration program
 *
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0
 *  Copyright 2017, 2021-2023 NXP. Not a Contribution
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  This file is part of mbed TLS (https://tls.mbed.org)
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "cb_osal.h"
#include "cb_helpers.h"
#include "crypto_benchmark.h"

#include "cb_hash.h"
#include "cb_mac.h"
#include "cb_aes.h"
#include "cb_aead.h"
#include "cb_rsa.h"
#include "cb_ecdsa.h"
#include "cb_ctr_drbg.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
DATA_AT_SECTION(uint8_t g_CommonPayload[COMMON_PAYLOAD_SIZE], DATA_LOCATION);
DATA_AT_SECTION(uint8_t g_CommonOutput[COMMON_OUTPUT_SIZE], DATA_LOCATION);

volatile int benchmark_timing_alarmed;
uint64_t s_Timeout;

__weak cb_status_t hash_benchmark        () { return CB_STATUS_FAIL; }
__weak cb_status_t aes_benchmark         () { return CB_STATUS_FAIL; }
__weak cb_status_t aes_benchmark_opaque  () { return CB_STATUS_FAIL; }
__weak cb_status_t aead_benchmark        () { return CB_STATUS_FAIL; }
__weak cb_status_t aead_benchmark_opaque () { return CB_STATUS_FAIL; }
__weak cb_status_t mac_benchmark         () { return CB_STATUS_FAIL; }
__weak cb_status_t mac_benchmark_opaque  () { return CB_STATUS_FAIL; }
__weak cb_status_t rsa_benchmark         () { return CB_STATUS_FAIL; }
__weak cb_status_t rsa_benchmark_opaque  () { return CB_STATUS_FAIL; }
__weak cb_status_t ecdsa_benchmark       () { return CB_STATUS_FAIL; }
__weak cb_status_t ecdsa_benchmark_opaque() { return CB_STATUS_FAIL; }
__weak cb_status_t ctr_drbg_benchmark    () { return CB_STATUS_FAIL; }
__weak cb_status_t hmac_drbg_benchmark   () { return CB_STATUS_FAIL; }
__weak cb_status_t ecp_benchmark         () { return CB_STATUS_FAIL; }
__weak cb_status_t entropy_benchmark     () { return CB_STATUS_FAIL; }
__weak cb_status_t ecdh_benchmark        () { return CB_STATUS_FAIL; }

const testcases_t testcases[] = {
    {"hash", hash_benchmark},
    {"aes", aes_benchmark},
    {"aes_opaque", aes_benchmark_opaque},
    {"aead", aead_benchmark},
    {"aead_opaque", aead_benchmark_opaque},
    {"mac", mac_benchmark},
    {"mac_opaque", mac_benchmark_opaque},
    {"rsa", rsa_benchmark},
    {"rsa_opaque", rsa_benchmark_opaque},
    {"ecdsa", ecdsa_benchmark},
    {"ecdsa_opaque", ecdsa_benchmark_opaque},
    {"ctr_drbg", ctr_drbg_benchmark},
    {"hmac_drbg", hmac_drbg_benchmark},
    {"ecc", ecp_benchmark},
    {"entropy", entropy_benchmark},
    {"ecdh", ecdh_benchmark},
    {NULL, NULL}};

/*******************************************************************************
 * Code
 ******************************************************************************/

void benchmark_set_alarm(int seconds)
{
    benchmark_timing_alarmed = 0;
    s_Timeout                = cb_osal_timing_hardclock() + (seconds * cb_osal_get_core_sys_clk_freq());
}

void benchmark_poll_alarm(void)
{
    if (cb_osal_timing_hardclock() > s_Timeout)
    {
        benchmark_timing_alarmed = 1;
    }
}

int main(int argc, char *argv[])
{
    const testcases_t *test;

    cb_status_t status = CB_STATUS_FAIL;

    status = cb_osal_init_platform();
    if (status != CB_STATUS_SUCCESS)
    {
        PRINTF("Platform init failure");
        exit(1);
    }

    status = cb_osal_init_crypto();
    if (status != CB_STATUS_SUCCESS)
    {
        PRINTF("Crypto module init failure");
        exit(1);
    }

    status = cb_osal_init_tick();
    if (status != CB_STATUS_SUCCESS)
    {
        PRINTF("Tick init failure");
        exit(1);
    }

#if defined(CB_PRINT_CSV) && (CB_PRINT_CSV == 1u)
    printf_csv_header();
#endif

    /* Run all the tests */
    for (test = testcases; test->name != NULL; test++)
    {
        test->function();
    }

    PRINTF("\n");

    cb_osal_exit();

    exit(0);
}
