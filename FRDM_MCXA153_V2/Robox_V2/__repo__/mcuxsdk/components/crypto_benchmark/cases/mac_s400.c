/*
 * Copyright 2025 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "cb_helpers.h"

#define MAC_CASE(_algorithm, _hash, _key_size)                                 \
  {                                                                            \
    .algorithm = CB_MAC_ALGORITHM_##_algorithm, .hash = CB_HASH_TYPE_##_hash,  \
    .key_size = _key_size##u                                                   \
  }

const mac_benchmark_cases_t mac_benchmark_cases[] = {
    MAC_CASE(HMAC, SHA256, 224),
    MAC_CASE(HMAC, SHA256, 256),
    MAC_CASE(HMAC, SHA256, 384),
    MAC_CASE(HMAC, SHA256, 512),
    MAC_CASE(HMAC, SHA384, 224),
    MAC_CASE(HMAC, SHA384, 256),
    MAC_CASE(HMAC, SHA384, 384),
    MAC_CASE(HMAC, SHA384, 512),
    MAC_CASE(CMAC, NO_HASH, 128),
    MAC_CASE(CMAC, NO_HASH, 192),
    MAC_CASE(CMAC, NO_HASH, 256),
};

const size_t mac_benchmark_cases_nb = ARRAY_SIZE(mac_benchmark_cases);
