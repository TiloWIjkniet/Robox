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

#ifndef CRYPTO_BENCHMARK_H
#define CRYPTO_BENCHMARK_H

#include <stdint.h>

#include "cb_osal.h"
#include "cb_common_config.h"
#include "cb_helpers.h"

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
void benchmark_set_alarm(int seconds);
void benchmark_poll_alarm(void);

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define SINGLE_BLOCK   1u
#define MULTIPLE_BLOCK 384u

#if defined(CRYPTO_BENCHMARK_PERFCURVE) && (CRYPTO_BENCHMARK_PERFCURVE == 1u)
static const size_t g_cb_perfcurve_block_counts[] = {2u, 4u, 16u, 64u, 256u};
#endif /* CRYPTO_BENCHMARK_PERFCURVE */

#define BYTES_TO_BITS(bytes) (bytes * 8u)

#define LARGEST_INPUT_BLOCK_SIZE  (144u)         /* SHA-3_224 */
#define LARGEST_OUTPUT_BLOCK_SIZE (64u)          /* ChaCha20-Poly1305 */
#define COMMON_PAYLOAD_SIZE \
    ((LARGEST_INPUT_BLOCK_SIZE * MULTIPLE_BLOCK) + (LARGEST_INPUT_BLOCK_SIZE * 3)) /* A large enough size for the common input buffer + 3 input block sizes for wiggle room */
#define COMMON_OUTPUT_SIZE \
    ((LARGEST_OUTPUT_BLOCK_SIZE * MULTIPLE_BLOCK) + (LARGEST_OUTPUT_BLOCK_SIZE * 3))  /* A large enough size for the common output buffer + 3 output block sizes for wiggle room */

/* Keystore and opaque cipher related defines */
#define KEYSTORE_ID    0x12345678u
#define KEYSTORE_NONCE 0xabcdef12u

/** @brief Common input payload buffer for various algorithms */
DATA_AT_SECTION(extern uint8_t g_CommonPayload[COMMON_PAYLOAD_SIZE], DATA_LOCATION);

/** @brief Common output buffer for various algorithms */
DATA_AT_SECTION(extern uint8_t g_CommonOutput[COMMON_OUTPUT_SIZE], DATA_LOCATION);

/** Convert cycles per byte to KiB/s.
 *  (CB * (1024u * INPUT_SIZE)) calculates the number of cycles taken during the CYCLES_BYTE measurement macro. */
#define CYCLES_BYTE_TO_KB_S(CB, INPUT_SIZE) ((INPUT_SIZE) / ((CB * (1024u * INPUT_SIZE)) / cb_osal_get_core_sys_clk_freq()))

#if defined(CRYPTO_BENCHMARK_STOP_CONDITION) && (CRYPTO_BENCHMARK_STOP_CONDITION == 1u)
#define STOP_COND(COND) \
    if (!(COND))        \
        break;
#else
#define STOP_COND(COND)
#endif

/** Macro function adapted from existing mbedtls benchmark */
#define CYCLES_BYTE(COND, CODE, INPUT_SIZE)                                            \
    ({                                                                                 \
        uint32_t jj;                                                                   \
        uint64_t tsc1;                                                                 \
        tsc1 = cb_osal_timing_hardclock();                                             \
        for (jj = 0U; jj < 1024U; ++jj)                                                \
        {                                                                              \
            CODE;                                                                      \
            STOP_COND(COND);                                                           \
        }                                                                              \
        (double)((float)(cb_osal_timing_hardclock() - tsc1) / (1024U * INPUT_SIZE));   \
    })

/** Macro function adapted from existing mbedtls benchmark */
#define KB_S(COND, CODE, INPUT_SIZE)                                                               \
    ({                                                                                             \
        uint64_t ii;                                                                               \
        uint64_t tsc1;                                                                             \
        uint64_t tsc2;                                                                             \
        benchmark_set_alarm(0x1U); /* Measure only 1 second  (KB/s) */                             \
        tsc1 = cb_osal_timing_hardclock();                                                         \
        for (ii = 1U; !benchmark_timing_alarmed; ++ii)                                             \
        {                                                                                          \
            CODE;                                                                                  \
            STOP_COND(COND);                                                                       \
            benchmark_poll_alarm();                                                                \
        }                                                                                          \
        tsc2 = cb_osal_timing_hardclock();                                                         \
        (double)((ii * INPUT_SIZE / 1024) / (((float)(tsc2 - tsc1)) / cb_osal_get_core_sys_clk_freq())); \
    })

/* Provide absolut cycle count of executed CODE */
#define CYCLES_COUNT(CODE)                   \
    ({                                       \
        uint64_t tsc1, tsc2;                 \
        tsc1 = cb_osal_timing_hardclock();   \
        CODE;                                \
        tsc2 = cb_osal_timing_hardclock();   \
                                             \
        (double)(tsc2 - tsc1);               \
    })

/* Provide cycles count for single CODE execution during given time */
#define CYCLES_COUNT_TIME(COND, CODE, SEC)                               \
    do                                                                   \
    {                                                                    \
        uint32_t ii;                                                     \
        uint64_t tsc1, tsc2;                                             \
        benchmark_set_alarm(SEC);                                        \
                                                                         \
        tsc1 = cb_osal_timing_hardclock();                               \
                                                                         \
        for (ii = 1; !benchmark_timing_alarmed ii++)                     \
        {                                                                \
            CODE;                                                        \
            STOP_COND(COND);                                             \
            benchmark_poll_alarm();                                      \
        }                                                                \
                                                                         \
        tsc2 = cb_osal_timing_hardclock();                               \
                                                                         \
        PRINTF("%6.2f CYCLES", (double)(((tsc2 - tsc1) / ((float)ii)))); \
        PRINTF("\r\n");                                                  \
                                                                         \
    } while (0)

/* Provide cycle count for single CODE execution with given ITERATIONS of execution */
#define CYCLES_COUNT_ITER(COND, CODE, ITERATIONS)                        \
    ({                                                                   \
        uint32_t ii;                                                     \
        uint64_t tsc1, tsc2;                                             \
        benchmark_set_alarm(3);                                          \
                                                                         \
        tsc1 = cb_osal_timing_hardclock();                               \
                                                                         \
        for (ii = 0; ii < ITERATIONS; ii++)                              \
        {                                                                \
            CODE;                                                        \
            STOP_COND(COND);                                             \
        }                                                                \
                                                                         \
        tsc2 = cb_osal_timing_hardclock();                               \
                                                                         \
        PRINTF("%6.2f CYCLES", (double)(((tsc2 - tsc1) / ((float)ii)))); \
        PRINTF("\r\n");                                                  \
    })

/* Provide time spent in single code execution in seconds */
#define TIME_TOTAL(CODE)                                                                            \
    ({                                                                                              \
        uint64_t tsc;                                                                               \
        tsc = cb_osal_timing_hardclock();                                                           \
        CODE;                                                                                       \
        (double)((double)(cb_osal_timing_hardclock() - tsc) / (double)cb_osal_get_core_sys_clk_freq()); \
    })

/* Provide average time spent in code execution in seconds over ITERATIONS
 * number of times. Similar to TIME_TOTAL_ITER but provides parameters for
 * untimed setup and cleanup code (before and after) the timed code.
 */
#define TIME_AVG(COND, CODE, ITERATIONS, UNTIMED_ITERATION_SETUP_CODE, UNTIMED_ITERATION_CLEANUP_CODE)         \
    ({                                                                                                         \
        uint32_t ii;                                                                                           \
        uint64_t tsc;                                                                                          \
        double sum = 0.0;                                                                                      \
        for (ii = 0; ii < ITERATIONS; ii++)                                                                    \
        {                                                                                                      \
            UNTIMED_ITERATION_SETUP_CODE;                                                                      \
            tsc = cb_osal_timing_hardclock();                                                                  \
            CODE;                                                                                              \
            sum += (double)((double)(cb_osal_timing_hardclock() - tsc) / (double)cb_osal_get_core_sys_clk_freq()); \
            UNTIMED_ITERATION_CLEANUP_CODE;                                                                    \
            STOP_COND(COND);                                                                                   \
        }                                                                                                      \
        (double)((double)sum / (double)ITERATIONS);                                                            \
    })

/* Provide time spent in single code execution in micro seconds */
#define TIME_TOTAL_USEC(CODE)                                                                                 \
    ({                                                                                                        \
        uint64_t tsc;                                                                                         \
        tsc = cb_osal_timing_hardclock();                                                                     \
        CODE;                                                                                                 \
        (double)((double)(cb_osal_timing_hardclock() - tsc) / (double)cb_osal_get_core_sys_clk_freq()) * 1000000; \
    })

/* Provide time spent in single code execution, measured during given iterations in sec */
#define TIME_TOTAL_ITER(COND, CODE, ITERATIONS)                                                   \
    ({                                                                                            \
        uint32_t ii;                                                                              \
        uint64_t tsc;                                                                             \
                                                                                                  \
        tsc = cb_osal_timing_hardclock();                                                         \
        for (ii = 0; ii < ITERATIONS; ii++)                                                       \
        {                                                                                         \
            CODE;                                                                                 \
            STOP_COND(COND);                                                                      \
        }                                                                                         \
                                                                                                  \
        (double)(((cb_osal_timing_hardclock() - tsc) / (float)ii) / cb_osal_get_core_sys_clk_freq()); \
    })

/* Provide time spent in single code execution, measured during given period of time (3s by default) */
#define TIME_TOTAL_INTERVAL(COND, CODE)                                                             \
    ({                                                                                              \
        uint32_t ii;                                                                                \
        uint64_t tsc;                                                                               \
        benchmark_set_alarm(3);                                                                     \
                                                                                                    \
        tsc = cb_osal_timing_hardclock();                                                           \
        for (ii = 1; !benchmark_timing_alarmed; ii++)                                               \
        {                                                                                           \
            CODE;                                                                                   \
            STOP_COND(COND);                                                                        \
            benchmark_poll_alarm();                                                                 \
        }                                                                                           \
                                                                                                    \
        (double)(((float)ii) / ((cb_osal_timing_hardclock() - tsc) / cb_osal_get_core_sys_clk_freq())); \
    })

typedef struct
{
    const char *name;
    cb_status_t (*function)(void);
} testcases_t;

/* A mask for the algorithm category encoding as defined by PSA */
#define CB_ALGORITHM_CATEGORY_MASK ((uint32_t)0x7F000000u)
#define CB_ALGORITHM_CATEGORY(x)   ((uint32_t)x & CB_ALGORITHM_CATEGORY_MASK)

/*******************************************************************************
 * Variables
 ******************************************************************************/
extern volatile int benchmark_timing_alarmed;
extern uint64_t s_Timeout;

#endif /* CRYPTO_BENCHMARK_H */
