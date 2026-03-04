/*
 * Copyright 2025 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/**
 * @file  common_utils.h
 * @brief Declarations for common utility functions for the PSA layer.
 */

#ifndef CB_PSA_PORT_COMMON_UTILS_H
#define CB_PSA_PORT_COMMON_UTILS_H

#include <psa/crypto.h>

#include "cb_types.h"

#if defined(PSA_CRYPTO_DRIVER_ELE_S2XX)
#include "ele_s2xx_crypto_primitives.h"
#elif defined(PSA_CRYPTO_DRIVER_ELS_PKC)
#include "els_pkc_driver.h"
#endif

/* The location to be used for generation of keys should come from prj.conf */
#if !defined(OPAQUE_KEY_LOCATION)

/* Definitions for ELE S4XX */
#if defined(PSA_CRYPTO_DRIVER_ELE_S4XX)

#if defined(PSA_CRYPTO_ELE_S4XX_LOCATION)
#define OPAQUE_KEY_LOCATION (PSA_CRYPTO_ELE_S4XX_LOCATION)
#endif /* PSA_CRYPTO_ELE_S4XX_LOCATION */

/* Definitions for ELS PKC */
#elif defined(PSA_CRYPTO_DRIVER_ELS_PKC)

#define OPAQUE_KEY_LOCATION (PSA_CRYPTO_ELS_PKC_LOCATION_S50_RFC3394_STORAGE)

#else /* No PSA_CRYPTO_DRIVER_* found - use default location */ // TODO add S2XX

#define OPAQUE_KEY_LOCATION (PSA_KEY_LOCATION_LOCAL_STORAGE)

#endif /* PSA_CRYPTO_DRIVER_* */

#endif /* !OPAQUE_KEY_LOCATION */

cb_status_t convert_cb_aead_type_to_psa_alg(cb_aead_type_t aead_type,
                                            psa_algorithm_t *alg);

cb_status_t convert_cb_cipher_type_to_psa_alg(cb_cipher_type_t cipher_type,
                                              psa_algorithm_t *alg);

cb_status_t convert_cb_hash_type_to_psa_alg(cb_hash_t hash_type,
                                            psa_algorithm_t *alg);

cb_status_t convert_cb_ecc_family_to_psa_ecc_family(cb_ecc_family_t cb_ecc_family,
                                                    psa_ecc_family_t *psa_ecc_family);

#endif /* CB_PSA_PORT_COMMON_UTILS_H */
