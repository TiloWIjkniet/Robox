/*
 * Copyright 2015 Freescale Semiconductor, Inc.
 * Copyright 2015-2021, 2025 NXP
 *
 * This software is owned or controlled by NXP and may only be used strictly
 * in accordance with the license terms
 * defined in <distribution-root>/IEC60730-LICENSE.txt file.
 * By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you
 * have read, and that you agree to comply with and are bound by,
 * such license terms.  If you do not agree to be bound by the applicable
 * license terms, then you may not retain, install, activate or otherwise
 * use the software.
 *
 * version 5.0
 *
 * @brief Core header file for Cortex-M33 core - IEC60730 Class B.
 *
 */

#ifndef IEC60730B_CORE_H_
#define IEC60730B_CORE_H_

/******************************************************************************
 * Types
 ******************************************************************************/
#include "iec60730b_types.h"

/******************************************************************************
 * Includes
 ******************************************************************************/
#include "iec60730b_cm33_flash.h"
#include "iec60730b_cm33_pc.h"
#include "iec60730b_cm33_ram.h"
#include "iec60730b_cm33_reg.h"
#include "iec60730b_cm33_stack.h"

#if FPU_SUPPORT
  #include "iec60730b_cm33_reg_dsp_fpu.h"
#endif


#endif /* IEC60730B_CORE_H_ */
