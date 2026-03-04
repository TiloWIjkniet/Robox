/*
 * Copyright 2015-2022, 2025 NXP
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
 * @brief CPU registers test routines for Cortex-M33 core - IEC60730 Class B.
 *
 */

#ifndef IEC60730B_CM33_REG_DSP_FPU_H_
#define IEC60730B_CM33_REG_DSP_FPU_H_

/*
 * List of devices and supported functions can be found in library user guide,
 * section Core self test library - source code version.
 */

/******************************************************************************
 * Macros
 ******************************************************************************/
/* Return values */
#define FS_FAIL_CPU_FLOAT_1             (FS_CPU_CODE + 0x06)
#define FS_FAIL_CPU_FLOAT_2             (FS_CPU_CODE + 0x07)



/* Assembler skips following section */
#ifndef __ASM__

/******************************************************************************
 * Functions
 ******************************************************************************/
/*******************************************************************************
 *
 * @brief    This function tests the following CPU registers
 *           in a sequence: R0-R7, R12, LR, APSR with G and GE bit.
 *           Can be executed in secure or non-secure mode.
 *
 * @param    void
 *
 * @return   FS_PASS
 *           FS_FAIL_CPU_REGISTER
 *           Function will not return in case that at least one of
 *           R0, R1, APSR, LR is corrupted. Then it will stay in endless loop.
 *
 ******************************************************************************/
FS_RESULT FS_CM33_CPU_Register(void);

/*******************************************************************************
 *
 * @brief    This function tests the FPU register (FPSCR, S0-S15).
 *           Only for devices with Floating Point Unit.
 *           The core must be in the secure state.
 *
 * @param    void
 *
 * @return   FS_PASS
 *           FS_FAIL_CPU_FLOAT_1
 *
 ******************************************************************************/
FS_RESULT FS_CM33_CPU_Float1(void);

/*******************************************************************************
 *
 * @brief    This function tests the FPU register (S16-S31).
 *           Only for devices with Floating Point Unit.
 *           The core must be in the secure state.
 *
 * @param    void
 *
 * @return   FS_PASS
 *           FS_FAIL_CPU_FLOAT_1
 *
 ******************************************************************************/
FS_RESULT FS_CM33_CPU_Float2(void);

#endif /* __ASM__ */
#endif /* IEC60730B_CM33_REG_DSP_FPU_H_ */
