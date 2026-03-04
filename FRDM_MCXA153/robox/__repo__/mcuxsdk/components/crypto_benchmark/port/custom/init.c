/*
 * Copyright 2023 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "cb_osal.h"

/**
 *
 * CUSTOM-PORT-LAYER-TODO
 *  Required crypto headers
 *
 */

/******************************************************************************/
/******************** CRYPTO_InitHardware *************************************/
/******************************************************************************/

uint32_t g_isCryptoHWInitialized = false;

/*!
 * @brief Application init for Crypto blocks.
 *
 * This function is provided to be called by MCUXpresso SDK applications.
 * It calls basic init for Crypto Hw acceleration and Hw entropy modules.
 */
//status_t CRYPTO_InitHardware(void)
//{
//    status_t result = kStatus_Fail;
//
//    if (g_isCryptoHWInitialized == true)
//    {
//        return 0;
//    }
//
//    do
//    {
//        /**
//         *
//         * CUSTOM-PORT-LAYER-TODO
//         *  Add crypto initialization steps
//         *
//         */
//
//        g_isCryptoHWInitialized = true;
//
//    } while (0);
//
//    return result;
//}

/*!
 * @brief Application Deinit for Crypto blocks.
 *
 * This function is provided to be called by MCUXpresso SDK applications.
 * It calls basic deinit for Crypto Hw acceleration and Hw entropy modules.
 */
//status_t CRYPTO_DeinitHardware(void)
//{
//    status_t result = kStatus_Fail;
//
//    if (g_isCryptoHWInitialized == false)
//    {
//        return 0;
//    }
//
//    do
//    {
//        /**
//         *
//         * CUSTOM-PORT-LAYER-TODO
//         *  Add crypto deinitialization steps
//         *
//         */
//
//        g_isCryptoHWInitialized  = false;
//
//    } while (0);
//
//    return result;
//}
