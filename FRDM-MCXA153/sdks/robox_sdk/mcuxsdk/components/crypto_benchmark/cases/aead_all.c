/*
 * Copyright 2025 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "cb_helpers.h"

#define AEAD_CASE(_type, _key_size)                                            \
  { .type = CB_AEAD_TYPE_##_type, .key_size = _key_size##u }

const aead_benchmark_cases_t aead_benchmark_cases[] = {
    AEAD_CASE(CCM, 128),
    AEAD_CASE(CCM, 192),
    AEAD_CASE(CCM, 256),
    AEAD_CASE(GCM, 128),
    AEAD_CASE(GCM, 192),
    AEAD_CASE(GCM, 256),
    AEAD_CASE(CHACHA20_POLY1305, 128),
    AEAD_CASE(CHACHA20_POLY1305, 192),
    AEAD_CASE(CHACHA20_POLY1305, 256),
};

const size_t aead_benchmark_cases_nb = ARRAY_SIZE(aead_benchmark_cases);
