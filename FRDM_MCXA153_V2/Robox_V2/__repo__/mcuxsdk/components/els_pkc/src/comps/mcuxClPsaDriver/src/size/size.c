/*--------------------------------------------------------------------------*/
/* Copyright 2022 NXP                                                       */
/*                                                                          */
/* NXP Proprietary. This software is owned or controlled by NXP and may     */
/* only be used strictly in accordance with the applicable license terms.   */
/* By expressly accepting such terms or by downloading, installing,         */
/* activating and/or otherwise using the software, you are agreeing that    */
/* you have read, and that you agree to comply with and are bound by, such  */
/* license terms.  If you do not agree to be bound by the applicable        */
/* license terms, then you may not retain, install, activate or otherwise   */
/* use the software.                                                        */
/*--------------------------------------------------------------------------*/

/**
 *
 * @file:   size.c
 * @brief:  This file contains objects which will be used to measure size of particular types.
 *
 */

#include <mcuxCsslAnalysis.h>
#include <psa/crypto_types.h>
#include <internal/mcuxClPsaDriver_Internal.h>
#include <internal/mcuxClHashModes_Internal_Memory.h>

/* ******************************** */
/* *** Internal structure sizes *** */
/* ******************************** */

MCUX_CSSL_ANALYSIS_START_PATTERN_OBJ_SIZES()
volatile mcuxClPsaDriver_ClnsData_Cipher_t mcuxClPsaDriver_ClnsData_Cipher;
volatile mcuxClPsaDriver_ClnsData_Aead_t mcuxClPsaDriver_ClnsData_Aead;
volatile mcuxClPsaDriver_ClnsData_Mac_t mcuxClPsaDriver_ClnsData_Mac;
/* mcuxClPsaDriver_ClnsData_Hash_t cannot be used, because additional context data is placed in memory behind the Hash context struct */
volatile uint8_t mcuxClPsaDriver_ClnsData_Hash[MCUXCLHASHMODES_CONTEXT_MAX_SIZE_INTERNAL];
MCUX_CSSL_ANALYSIS_STOP_PATTERN_OBJ_SIZES()
