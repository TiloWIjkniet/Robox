/*
 * Copyright 2025 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "cb_helpers.h"

#define AES_CASE(_type, _key_size)                                             \
  { .type = CB_CIPHER_TYPE_##_type, .key_size = _key_size }

const aes_benchmark_cases_t aes_benchmark_cases[] = {
    AES_CASE(STREAM, 128),
    AES_CASE(STREAM, 192),
    AES_CASE(STREAM, 256),
    AES_CASE(CTR, 128),
    AES_CASE(CTR, 192),
    AES_CASE(CTR, 256),
    AES_CASE(CFB, 128),
    AES_CASE(CFB, 192),
    AES_CASE(CFB, 256),
    AES_CASE(OFB, 128),
    AES_CASE(OFB, 192),
    AES_CASE(OFB, 256),
    AES_CASE(XTS, 256),
    AES_CASE(XTS, 384),
    AES_CASE(XTS, 512),
    AES_CASE(CBC_NO_PADDING, 128),
    AES_CASE(CBC_NO_PADDING, 192),
    AES_CASE(CBC_NO_PADDING, 256),
    AES_CASE(CBC_PKCS7_PADDING, 128),
    AES_CASE(CBC_PKCS7_PADDING, 192),
    AES_CASE(CBC_PKCS7_PADDING, 256),
    AES_CASE(ECB_NO_PADDING, 128),
    AES_CASE(ECB_NO_PADDING, 192),
    AES_CASE(ECB_NO_PADDING, 256),
};

const size_t aes_benchmark_cases_nb = ARRAY_SIZE(aes_benchmark_cases);
