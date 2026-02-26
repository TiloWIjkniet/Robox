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
#include <mcuxClOscca_Memory.h>
#include <internal/mcuxClOsccaRandomModes_Private_RNG.h>

MCUX_CSSL_ANALYSIS_START_PATTERN_OBJ_SIZES()
/* *********************** */
/* *** Work area sizes *** */
/* *********************** */
#ifdef MCUXCL_FEATURE_RANDOMMODES_OSCCA_TRNG
volatile uint8_t mcuxClOsccaRandomModes_Context_RNG[sizeof(mcuxClOsccaRandomModes_Context_RNG_t)];
volatile uint8_t mcuxClOsccaRandomModes_RNG_WaCpuMax[mcuxClOscca_alignSize(sizeof(mcuxClOsccaRandomModes_Context_RNG_t))];
#endif /* MCUXCL_FEATURE_RANDOMMODES_OSCCA_TRNG */
MCUX_CSSL_ANALYSIS_STOP_PATTERN_OBJ_SIZES()
