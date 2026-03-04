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
#include <internal/mcuxClOsccaSm4_Internal_Constants.h>
#include <internal/mcuxClOsccaAeadModes_Internal_Types.h>

MCUX_CSSL_ANALYSIS_START_PATTERN_OBJ_SIZES()
/* *********************** */
/* *** Work area sizes *** */
/* *********************** */

volatile uint8_t mcuxClOsccaAeadModes_WaCpuMax[MCUXCLOSCCASM4_BLOCK_SIZE * 2u + MCUXCLOSCCASM4_RK_WORDS * sizeof(uint32_t)];
volatile uint8_t mcuxClOsccaAeadModes_Context_size[sizeof(mcuxClOsccaAeadModes_Context_t)];

MCUX_CSSL_ANALYSIS_STOP_PATTERN_OBJ_SIZES()
