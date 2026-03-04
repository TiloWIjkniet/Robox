/*--------------------------------------------------------------------------*/
/* Copyright 2022-2024 NXP                                                  */
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
#include <mcuxClCore_Platform.h>
#include <mcuxCsslAnalysis.h>
#include <mcuxClOscca_Memory.h>
#include <mcuxClOsccaSm3_Constants.h>
#include <internal/mcuxClOsccaSm3_Internal.h>
#include <internal/mcuxClHash_Internal.h>

#if defined(MCUXCL_FEATURE_HASH_HW_SM3)

MCUX_CSSL_ANALYSIS_START_PATTERN_OBJ_SIZES()
/* Hash Cpu Workarea size generation */
volatile uint8_t mcuxClOsccaSm3_oneShot_WaCpuSm3 [MCUXCLOSCCASM3_BLOCK_SIZE_SM3 + 2u * MCUXCLOSCCASM3_STATE_SIZE_SM3 + 2u * MCUXCLOSCCASM3_OUTPUT_SIZE_SM3]; // two additional outputSize for compare
volatile uint8_t mcuxClOsccaSm3_oneShot_WaCpuMax [MCUXCLOSCCASM3_BLOCK_SIZE_SM3 + 2u * MCUXCLOSCCASM3_STATE_SIZE_SM3 + 2u * MCUXCLOSCCASM3_OUTPUT_SIZE_SM3]; // two additional outputSize for compare


volatile uint8_t mcuxClOsccaSm3_process_WaCpuSm3 [MCUXCLOSCCASM3_STATE_SIZE_SM3];
volatile uint8_t mcuxClOsccaSm3_process_WaCpuMax [MCUXCLOSCCASM3_STATE_SIZE_SM3];

volatile uint8_t mcuxClOsccaSm3_finish_WaCpuSm3 [2u * MCUXCLOSCCASM3_STATE_SIZE_SM3]; // one additional state for compare
volatile uint8_t mcuxClOsccaSm3_finish_WaCpuMax [2u * MCUXCLOSCCASM3_STATE_SIZE_SM3];

/* Hash multi-part context size generation */
volatile uint8_t mcuxClOsccaSm3_Ctx_size[mcuxClOscca_alignSize(sizeof(mcuxClHash_ContextDescriptor_t) + MCUXCLHASH_CONTEXT_MAX_ALIGNMENT_OFFSET + MCUXCLOSCCASM3_BLOCK_SIZE_SM3 + MCUXCLOSCCASM3_STATE_SIZE_SM3)];

volatile uint8_t mcuxClOsccaSm3_export_import_WaCpu[MCUXCLOSCCASM3_COUNTER_SIZE_SM3 + MCUXCLOSCCASM3_STATE_SIZE_SM3];

MCUX_CSSL_ANALYSIS_STOP_PATTERN_OBJ_SIZES()
#endif /* MCUXCL_FEATURE_HASH_SW_SM3 || MCUXCL_FEATURE_HASH_HW_SM3 */
