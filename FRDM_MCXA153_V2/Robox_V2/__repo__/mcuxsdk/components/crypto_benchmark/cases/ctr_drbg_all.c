/*
 * Copyright 2025 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "cb_helpers.h"

#define CTR_DRBG_CASE(_output_size)                                            \
  { .output_size = _output_size }

const ctr_drbg_benchmark_cases_t ctr_drbg_benchmark_cases[] = {
    CTR_DRBG_CASE(8),
    CTR_DRBG_CASE(16),
    CTR_DRBG_CASE(32),
    CTR_DRBG_CASE(256),
    CTR_DRBG_CASE(1024),
};

const size_t ctr_drbg_benchmark_cases_nb = ARRAY_SIZE(ctr_drbg_benchmark_cases);
