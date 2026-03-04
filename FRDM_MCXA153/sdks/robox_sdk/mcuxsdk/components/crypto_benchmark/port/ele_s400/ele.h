/*
 * Copyright 2022 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef ELE_H
#define ELE_H

#include "cb_osal.h"

typedef struct
{
    uint32_t session_handle;
    bool is_fw_loaded;
    /* Below attributes are used in case opaque keys are enabled  */
    uint32_t keystore_handle;
    uint32_t key_management_handle;
    uint32_t cipher_handle;
    uint32_t mac_handle;
    uint32_t sign_handle;
    uint32_t verify_handle;
} ele_ctx_t;

extern ele_ctx_t g_ele_ctx;

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @brief Application init for Crypto blocks.
 *
 * This function is provided to be called by MCUXpresso SDK applications.
 * It calls basic init for Crypto Hw acceleration and Hw entropy modules.
 */
status_t CRYPTO_InitHardware(void);

/*!
 * @brief Application deinit for Crypto blocks.
 *
 * This function is provided to be called by MCUXpresso SDK applications.
 * It calls basic deinit for Crypto Hw acceleration and Hw entropy modules.
 */
status_t CRYPTO_DeinitHardware(void);

#ifdef __cplusplus
}
#endif

#endif /* ELE_H */
