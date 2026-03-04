/*
 * Copyright 2024-2025 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef CB_COMMON_CONFIG_H
#define CB_COMMON_CONFIG_H

// #define CB_PRINT_VERBOSE (1u)
//#define CB_PRINT_CSV (1u) // Print output as CSV

#define CRYPTO_BENCHMARK_STOP_CONDITION (1u)

/**
 * @brief When enabled, the AES, HASH, and AEAD categories will run at multiple
 *        input buffer sizes in order to gather additional performance data.
 */
// #define CRYPTO_BENCHMARK_PERFCURVE (1u)

#endif /* CB_COMMON_CONFIG_H */
