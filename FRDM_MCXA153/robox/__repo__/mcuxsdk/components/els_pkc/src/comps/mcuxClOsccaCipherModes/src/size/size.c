/*--------------------------------------------------------------------------*/
/* Copyright 2022-2023 NXP                                                  */
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
#include <mcuxClCore_Platform.h>
#include <mcuxClKey_Types.h>
#include <mcuxClOscca_Memory.h>
#include <internal/mcuxClKey_Types_Internal.h>
#include <mcuxClOscca_Memory.h>
#include <internal/mcuxClOsccaCipherModes_Internal_Types.h>

MCUX_CSSL_ANALYSIS_START_PATTERN_OBJ_SIZES()
/* *********************** */
/* *** Work area sizes *** */
/* *********************** */
#ifdef MCUXCL_FEATURE_CIPHERMODES_SM4
volatile uint8_t mcuxClOsccaCipherModes_WorkArea_Sm4_OneShot[mcuxClOscca_alignSize(sizeof(mcuxClOsccaCipherModes_Context_Sm4_t))];  // SM4 Oneshot Context needs to be in WA


volatile uint8_t mcuxClOsccaCipherModes_Ctx_Sm4[sizeof(mcuxClOsccaCipherModes_Context_Sm4_t)];
#endif /* MCUXCL_FEATURE_CIPHERMODES_SM4 */
MCUX_CSSL_ANALYSIS_STOP_PATTERN_OBJ_SIZES()
