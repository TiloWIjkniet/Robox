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
    MAC_CASE(HMAC, SHA512, 224),
    MAC_CASE(HMAC, SHA512, 256),
    MAC_CASE(HMAC, SHA512, 384),
    MAC_CASE(HMAC, SHA512, 512),
    MAC_CASE(HMAC, SHA3_256, 224),
    MAC_CASE(HMAC, SHA3_256, 256),
    MAC_CASE(HMAC, SHA3_256, 384),
    MAC_CASE(HMAC, SHA3_256, 512),
    MAC_CASE(HMAC, SHA3_384, 224),
    MAC_CASE(HMAC, SHA3_384, 256),
    MAC_CASE(HMAC, SHA3_384, 384),
    MAC_CASE(HMAC, SHA3_384, 512),
    MAC_CASE(HMAC, SHA3_512, 224),
    MAC_CASE(HMAC, SHA3_512, 256),
    MAC_CASE(HMAC, SHA3_512, 384),
    MAC_CASE(HMAC, SHA3_512, 512),
    MAC_CASE(CMAC, NO_HASH, 128),
    MAC_CASE(CMAC, NO_HASH, 192),
    MAC_CASE(CMAC, NO_HASH, 256),
    MAC_CASE(CBC_MAC, NO_HASH, 128),
    MAC_CASE(CBC_MAC, NO_HASH, 192),
    MAC_CASE(CBC_MAC, NO_HASH, 256),
};

const size_t mac_benchmark_cases_nb = ARRAY_SIZE(mac_benchmark_cases);
