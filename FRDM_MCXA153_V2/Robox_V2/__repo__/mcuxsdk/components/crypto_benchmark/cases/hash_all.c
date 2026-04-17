/*
 * Copyright 2025 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "cb_helpers.h"

#define HASH_CASE(_hash)                                                       \
  { .hash = CB_HASH_TYPE_##_hash }

const hash_benchmark_cases_t hash_benchmark_cases[] = {
    HASH_CASE(SHA1),
    HASH_CASE(SHA224),
    HASH_CASE(SHA256),
    HASH_CASE(SHA384),
    HASH_CASE(SHA512),
    HASH_CASE(SHA3_224),
    HASH_CASE(SHA3_256),
    HASH_CASE(SHA3_384),
    HASH_CASE(SHA3_512),
};

const size_t hash_benchmark_cases_nb = ARRAY_SIZE(hash_benchmark_cases);
