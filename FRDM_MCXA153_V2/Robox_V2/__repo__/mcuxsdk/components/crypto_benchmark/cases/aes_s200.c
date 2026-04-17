/*
 * Copyright 2025 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "cb_helpers.h"

#define AES_CASE(_type, _key_size)                                             \
  { .type = CB_CIPHER_TYPE_##_type, .key_size = _key_size##u }

const aes_benchmark_cases_t aes_benchmark_cases[] = {
    AES_CASE(CTR, 128),
    AES_CASE(CTR, 192),
    AES_CASE(CTR, 256),
    AES_CASE(CBC_NO_PADDING, 128),
    AES_CASE(CBC_NO_PADDING, 192),
    AES_CASE(CBC_NO_PADDING, 256),
    AES_CASE(ECB_NO_PADDING, 128),
    AES_CASE(ECB_NO_PADDING, 192),
    AES_CASE(ECB_NO_PADDING, 256),
};

const size_t aes_benchmark_cases_nb = ARRAY_SIZE(aes_benchmark_cases);
