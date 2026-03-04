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
#include <internal/mcuxClOsccaMacModes_Types.h>
#include <internal/mcuxClOsccaMacModes_Ctx.h>
#include <internal/mcuxClOsccaSm4_Internal_Constants.h>

MCUX_CSSL_ANALYSIS_START_PATTERN_OBJ_SIZES()
/* *********************** */
/* *** Work area sizes *** */
/* *********************** */
volatile uint8_t mcuxClOsccaMacModes_Context_size[sizeof(mcuxClOsccaMacModes_Context_t)];

volatile uint8_t mcuxClOsccaMacModes_Compute_CpuWA_MaxSize[MCUXCLOSCCASM4_BLOCK_SIZE * 4u + MCUXCLOSCCASM4_RK_WORDS * sizeof(uint32_t)];


volatile uint8_t mcuxClOsccaMacModes_Init_CpuWA_Size[sizeof(uint32_t)];
volatile uint8_t mcuxClOsccaMacModes_Update_CpuWA_Size[MCUXCLOSCCASM4_RK_WORDS * sizeof(uint32_t)];
volatile uint8_t mcuxClOsccaMacModes_Final_CpuWA_MaxSize[MCUXCLOSCCASM4_BLOCK_SIZE * 2u + MCUXCLOSCCASM4_RK_WORDS * sizeof(uint32_t)];

volatile uint8_t mcuxClOsccaMacModes_WaCpuMax [MCUXCLOSCCASM4_BLOCK_SIZE * 4u + MCUXCLOSCCASM4_RK_WORDS * sizeof(uint32_t) + 2u * MCUXCLOSCCAMACMODES_MAX_OUTPUT_SIZE];

MCUX_CSSL_ANALYSIS_STOP_PATTERN_OBJ_SIZES()
