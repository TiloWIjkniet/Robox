/*
 * Copyright 2015 Freescale Semiconductor, Inc.
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

#ifndef IEC60730B_CM33_REG_H_
#define IEC60730B_CM33_REG_H_

/*
 * List of devices and supported functions can be found in library user guide,
 * section Core self test library - source code version.
 */

/******************************************************************************
 * Macros
 ******************************************************************************/
/* Return values */
#define FS_FAIL_CPU_REGISTER            (FS_CPU_CODE + 0x01)
#define FS_FAIL_CPU_NONSTACKED_REGISTER (FS_CPU_CODE + 0x02)
#define FS_FAIL_CPU_PRIMASK             (FS_CPU_CODE + 0x03)
#define FS_FAIL_CPU_CONTROL             (FS_CPU_CODE + 0x04)
#define FS_FAIL_CPU_SPECIAL             (FS_CPU_CODE + 0x05)


#define CPACR_CP10_CP11_MASK (0xF << 20)

/* Assembler skips following section */
#ifndef __ASM__

/******************************************************************************
 * Functions
 ******************************************************************************/
/*******************************************************************************
 *
 * @brief    This function tests the following CPU registers
 *           in a sequence: R0-R7, R12, LR, APSR.
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
FS_RESULT FS_CM33_CPU_Register_NDSP(void);

/*******************************************************************************
 *
 * @brief    This function tests the following CPU register: R8, R9, R10, R11.
 *           Can be executed in secure or non-secure mode.
 *
 * @param    void
 *
 * @return   FS_PASS
 *           FS_FAIL_CPU_NONSTACKED_REGISTER
 *
 ******************************************************************************/
FS_RESULT FS_CM33_CPU_NonStackedRegister(void);

/*******************************************************************************
 *
 * @brief    This function tests the PRIMASK_S (secure) register.
 *           The core must be in the secure state.
 *
 * @param    void
 *
 * @return   FS_PASS
 *           FS_FAIL_CPU_PRIMASK
 *
 ******************************************************************************/
FS_RESULT FS_CM33_CPU_Primask_S(void);

/*******************************************************************************
 *
 * @brief    This function tests the PRIMASK_NS (non-secure) register.
 *           The core must be in the secure state.
 *
 * @param    void
 *
 * @return   FS_PASS
 *           FS_FAIL_CPU_PRIMASK
 *
 ******************************************************************************/
FS_RESULT FS_CM33_CPU_Primask_NS(void);

/*******************************************************************************
 *
 * @brief    This function tests the MSP (secure) register.
 *           The core must be in the secure state.
 *
 * @param    void
 *
 * @return   FS_PASS
 *           Function will not return in case that MSP is corrupted.
 *           Then it will stay in endless loop.
 *
 ******************************************************************************/
FS_RESULT FS_CM33_CPU_SPmain_S(void);

/*******************************************************************************
 *
 * @brief    This function tests the MSP_NS (non-secure) register.
 *           The core must be in the secure state.
 *
 * @param    void
 *
 * @return   FS_PASS
 *           Function will not return in case that MSP_NS is corrupted.
 *           Then it will stay in endless loop.
 *
 ******************************************************************************/
FS_RESULT FS_CM33_CPU_SPmain_NS(void);

/*******************************************************************************
 *
 * @brief    This function tests the MSPLIM (secure) register.
 *           The core must be in the secure state.
 *
 * @param    void
 *
 * @return   FS_PASS
 *           Function will not return in case that MSPLIM is corrupted.
 *           Then it will stay in endless loop.
 *
 ******************************************************************************/
FS_RESULT FS_CM33_CPU_SPmain_Limit_S(void);

/*******************************************************************************
 *
 * @brief    This function tests the MSPLIM_NS (non-secure) register.
 *           The core must be in the secure state.
 *
 * @param    void
 *
 * @return   FS_PASS
 *           Function will not return in case that MSPLIM_NS is corrupted.
 *           Then it will stay in endless loop.
 *
 ******************************************************************************/
FS_RESULT FS_CM33_CPU_SPmain_Limit_NS(void);

/*******************************************************************************
 *
 * @brief    This function tests the PSP (secure) register.
 *           The core must be in the secure state.
 *
 * @param    void
 *
 * @return   FS_PASS
 *           Function will not return in case that PSP is corrupted.
 *           Then it will stay in endless loop.
 *
 ******************************************************************************/
FS_RESULT FS_CM33_CPU_SPprocess_S(void);

/*******************************************************************************
 *
 * @brief    This function tests the PSP_NS (non-secure) register.
 *           The core must be in the secure state.
 *
 * @param    void
 *
 * @return   FS_PASS
 *           Function will not return in case that PSP_NS is corrupted.
 *           Then it will stay in endless loop.
 *
 ******************************************************************************/
FS_RESULT FS_CM33_CPU_SPprocess_NS(void);

/*******************************************************************************
 *
 * @brief    This function tests the PSPLIM (secure) register.
 *           The core must be in the secure state.
 *
 * @param    void
 *
 * @return   FS_PASS
 *           Function will not return in case that PSPLIM is corrupted.
 *           Then it will stay in endless loop.
 *
 ******************************************************************************/
FS_RESULT FS_CM33_CPU_SPprocess_Limit_S(void);

/*******************************************************************************
 *
 * @brief    This function tests the PSPLIM_NS (non-secure) register.
 *           The core must be in the secure state.
 *
 * @param    void
 *
 * @return   FS_PASS
 *           Function will not return in case that PSPLIM_NS is corrupted.
 *           Then it will stay in endless loop.
 *
 ******************************************************************************/
FS_RESULT FS_CM33_CPU_SPprocess_Limit_NS(void);

/*******************************************************************************
 *
 * @brief    This function tests the CONTROL register.
 *           This test must be executed only on devices without TrustZone support.
 *
 * @param    void
 *
 * @return   FS_PASS
 *           FS_FAIL_CPU_CONTROL
 *
 ******************************************************************************/
FS_RESULT FS_CM33_CPU_Control(void);

/*******************************************************************************
 *
 * @brief    This function tests the CONTROL (secure) register.
 *           The core must be in the secure state.
 *
 * @param    void
 *
 * @return   FS_PASS
 *           FS_FAIL_CPU_CONTROL
 *
 ******************************************************************************/
FS_RESULT FS_CM33_CPU_Control_S(void);

/*******************************************************************************
 *
 * @brief    This function tests the CONTROL_NS (non-secure) register.
 *           The core must be in the secure state.
 *
 * @param    void
 *
 * @return   FS_PASS
 *           FS_FAIL_CPU_CONTROL
 *
 ******************************************************************************/
FS_RESULT FS_CM33_CPU_Control_NS(void);

/*******************************************************************************
 *
 * @brief    This function tests the CONTROL register.
 *           This test must be executed only on devices without TrustZone extension
 *           and without FPU extension
 *
 * @param    void
 *
 * @return   FS_PASS
 *           FS_FAIL_CPU_CONTROL
 *
 ******************************************************************************/
FS_RESULT FS_CM33_CPU_Control_NFPU(void);

/*******************************************************************************
 *
 * @brief    This function tests the BASEPRI and FAULTMASK (secure) registers.
 *           For devices with 8 priority levels for interrupts.
 *           The core must be in the secure state.
 *
 * @param    void
 *
 * @return   FS_PASS
 *           FS_FAIL_CPU_SPECIAL
 *
 ******************************************************************************/
FS_RESULT FS_CM33_CPU_Special8PriorityLevels_S(void);

/*******************************************************************************
 *
 * @brief    This function tests the BASEPRI_NS and FAULTMASK_NS
 *           (non-secure) registers.
 *           For devices with 8 priority levels for interrupts.
 *           The core must be in the secure state.
 *
 * @param    void
 *
 * @return   FS_PASS
 *           FS_FAIL_CPU_SPECIAL
 *
 ******************************************************************************/
FS_RESULT FS_CM33_CPU_Special8PriorityLevels_NS(void);

#endif /* __ASM__ */
#endif /* IEC60730B_CM33_REG_H_ */
