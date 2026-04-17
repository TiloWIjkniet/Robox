/*
 * Copyright 2024 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/**
 * @file  cb_ctr_drbg.h
 * @brief Contains definitions for CTR DRBG benchmarking.
 */

#ifndef CB_CTR_DRBG_H
#define CB_CTR_DRBG_H

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "crypto_benchmark.h"
#include "cb_types.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/* Array initializer list of CTR DBG benchmarking cases of type
 * size_t.
 */
extern const ctr_drbg_benchmark_cases_t ctr_drbg_benchmark_cases[];
extern const size_t ctr_drbg_benchmark_cases_nb;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
cb_status_t ctr_drbg_benchmark(void);
cb_status_t ctr_drbg_benchmark_opaque(void);

/*******************************************************************************
 * Generic wrappers
 ******************************************************************************/
/**
 * @brief Initialize the port layer internal context for CTR DRBG.
 *
 *        This is called before every benchmarked call (the *_compute()
 *        functions). Used to preemptively set up the port layer's context
 *        in order to reduce the overhead of the *_compute() functions.
 *
 * @note  The internal context is expected to be heap allocated.
 *
 * @param ctx_internal This will hold the port layer's heap-allocated context.
 *
 * @return CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_ctr_drbg_init(void **ctx_internal);

/**
 * @brief Deinitialize the port layer internal context for CTR DRBG.
 *
 *        Free the heap-allocated internal context and carry out other
 *        port-specific deinitialization steps.
 *
 * @param ctx_internal The internal context to be free()'d.
 *
 * @retval CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_ctr_drbg_deinit(void *ctx_internal);

/**
 * @brief Benchmarked function for CTR DRBG.
 *
 * @param ctx_internal The port layer internal context.
 * @param output       Output buffer for the random generated data.
 * @param output_size  Size of the generated random data.
 *
 * @retval CB_STATUS_SUCCESS on success, different otherwise.
 */
cb_status_t wrapper_ctr_drbg_compute(void *ctx_internal, uint8_t *output, size_t output_size);

#endif /* CB_CTR_DRBG_H */
