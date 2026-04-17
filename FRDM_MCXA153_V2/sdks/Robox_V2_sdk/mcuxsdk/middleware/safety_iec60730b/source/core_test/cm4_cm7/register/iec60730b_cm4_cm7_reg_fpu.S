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
 * @brief CPU registers (FPU) test routines for Cortex-M4/M7 core - IEC60730 Class B.
 *
 */

#define __ASM__
    #include "iec60730b_core.h"
    #include "asm_mac_common.h"
#undef  __ASM__

ASM_COMP_SPECIFIC_DIRECTIVES
 ASM_CODE_SECTION(.text)

/*******************************************************************************
 * Functions
 ******************************************************************************/
 ASM_PUBLIC(FS_CM4_CM7_CPU_ControlFpu)
 ASM_PUBLIC(FS_CM4_CM7_CPU_Float1)
 ASM_PUBLIC(FS_CM4_CM7_CPU_Float2)

/*******************************************************************************
 * Implementation
 ******************************************************************************/
/*******************************************************************************
 ******************************************************************************/
ASM_PUBLIC_BEGIN(FS_CM4_CM7_CPU_ControlFpu)
ASM_PUBLIC_FUNC(FS_CM4_CM7_CPU_ControlFpu)
ASM_LABEL(FS_CM4_CM7_CPU_ControlFpu)

    MOV     R3, SP             /* store value of SP */
    MRS     R2, CONTROL        /* Backup of CONTROL in R0 */

    MOVS    R0, #0x00000002    /* pattern for SP_process */
    MSR     CONTROL, R0        /* move pattern into CONTROL */
    MRS     R1, CONTROL        /* move pattern from CONTROL */
    CMP     R1, #0x00000002
    BNE     Error_Control_fpu      /* if not the same, jump to ErrorCONTROL label */

    MOVS    R0, #0x00000000    /* pattern for SP_main */
    MSR     CONTROL, R0        /* move pattern into CONTROL */
    MRS     R1, CONTROL        /* move pattern from CONTROL */
    CMP     R1, #0x00000000
    BNE     Error_Control_fpu      /* if not the same, jump to ErrorCONTROL label */

    MOVS    R0, #0x00000004    /* pattern for FP extension active */
    MSR     CONTROL, R0        /* move pattern into CONTROL */
    MRS     R1, CONTROL        /* move pattern from CONTROL */
    CMP     R1, #0x00000004
    BNE     Error_Control_fpu      /* if not the same, jump to ErrorCONTROL label */

    MSR     CONTROL, R2        /* restore CONTROL */
    MOVS    R0, #FS_PASS          /* move PASS return into R0 */
    B       Continue_Control_fpu   /* jump over the ErrorCONTROL label */

ASM_LABEL(Error_Control_fpu)       /* ErrorCONTROL label */
    LDR     R0, =FS_FAIL_CPU_CONTROL  /* move the FAIL return into R0 */
    MOV     SP, R3             /* in case of error state, with CONTROL changed, to use original Stack */

ASM_LABEL(Continue_Control_fpu)
    BX      LR

 ASM_PUBLIC_END(FS_CM4_CM7_CPU_ControlFpu)

/*******************************************************************************
 ******************************************************************************/
ASM_PUBLIC_BEGIN(FS_CM4_CM7_CPU_Float1)
ASM_PUBLIC_FUNC(FS_CM4_CM7_CPU_Float1)
ASM_LABEL(FS_CM4_CM7_CPU_Float1)

    LDR.W   R2, =0xE000ED88    /* address of CPACR register */
    LDR     R3, [R2]           /* content of CPACR register....R3 */
    MOV     R1, #CPACR_CP10_CP11_MASK /* mask for enabling FPU */
    ORRS    R0, R3, R1         /* ORR of the mask and current state of CPACR register */
    STR     R0, [R2]           /* enable coprocessor if it isn't enabled */
    
    NOP                      /* delay to be sure the coprocessor is enabled before start of the test */
    NOP
    NOP
    NOP
    NOP 
    
    VMRS   R2, FPSCR               /* backup value from FPSCR */
    LDR    R0, =0x55400015   /* test pattern for FPSCR register */
    VMSR   FPSCR, R0         /* Move from general register to Special Register */
    VMRS   R1, FPSCR         /* Move from Special Register to general register */
    CMP    R0, R1
    BNE    ErrorFPU0

    LDR    R0, =0xA280008A   /* test pattern for FPSCR register */
    VMSR   FPSCR, R0         /* Move from general register to Special Register */
    VMRS   R1, FPSCR         /* Move from Special Register to general register */
    CMP    R0, R1
    BNE    ErrorFPU0
    VMSR   FPSCR, R2               /* put backup value of FPSCR back to this register */

    /* testing of FPU registers S0 - S15 for Stuck at faults */
    MOV    R0, #0x55555555    /* testing pattern  0x55555555 */
    MOV    R2, #0xAAAAAAAA    /* testing pattern  0xAAAAAAAA */

    VMOV   S0, R0     /*...S0...*/
    VMOV   R1, S0
    CMP    R1, R0
    BNE    ErrorFPU0
    VMOV   S0, R2
    VMOV   R1, S0
    CMP    R1, R2
    BNE    ErrorFPU0
    VMOV   S1, R0     /*...S1...*/
    VMOV   R1, S1
    CMP    R1, R0
    BNE    ErrorFPU0
    VMOV   S1, R2
    VMOV   R1, S1
    CMP    R1, R2
    BNE    ErrorFPU0
    VMOV   S2, R0     /*...S2...*/
    VMOV   R1, S2
    CMP    R1, R0
    BNE    ErrorFPU0
    VMOV   S2, R2
    VMOV   R1, S2
    CMP    R1, R2
    BNE    ErrorFPU0
    VMOV   S3, R0     /*...S3...*/
    VMOV   R1, S3
    CMP    R1, R0
    BNE    ErrorFPU0
    VMOV   S3, R2
    VMOV   R1, S3
    CMP    R1, R2
    BNE    ErrorFPU0
    VMOV   S4, R0     /*...S4...*/
    VMOV   R1, S4
    CMP    R1, R0
    BNE    ErrorFPU0
    VMOV   S4, R2
    VMOV   R1, S4
    CMP    R1, R2
    BNE    ErrorFPU0
    VMOV   S5, R0     /*...S5...*/
    VMOV   R1, S5
    CMP    R1, R0
    BNE    ErrorFPU0
    VMOV   S5, R2
    VMOV   R1, S5
    CMP    R1, R2
    BNE    ErrorFPU0
    VMOV   S6, R0     /*...S6...*/
    VMOV   R1, S6
    CMP    R1, R0
    BNE    ErrorFPU0
    VMOV   S6, R2
    VMOV   R1, S6
    CMP    R1, R2
    BNE    ErrorFPU0
    VMOV   S7, R0     /*...S7...*/
    VMOV   R1, S7
    CMP    R1, R0
    BNE    ErrorFPU0
    VMOV   S7, R2
    VMOV   R1, S7
    CMP    R1, R2
    BNE    ErrorFPU0
    VMOV   S8, R0      /*...S8...*/
    VMOV   R1, S8
    CMP    R1, R0
    BNE    ErrorFPU0
    VMOV   S8, R2
    VMOV   R1, S8
    CMP    R1, R2 // validation_mark FS_CPU_Float1 r1 None
    BNE    ErrorFPU0

    B     ContinueFPU0

ASM_LABEL(ErrorFPU0)   /* auxiliary label because of limited range of conditional branch instruction */
    B       ErrorFPU

ASM_LABEL(ContinueFPU0)
    VMOV   S9, R0      /*...S9...*/
    VMOV   R1, S9
    CMP    R1, R0
    BNE    ErrorFPU0
    VMOV   S9, R2
    VMOV   R1, S9
    CMP    R1, R2
    BNE    ErrorFPU0
    VMOV   S10, R0     /*...S10..*/
    VMOV   R1, S10
    CMP    R1, R0
    BNE    ErrorFPU0
    VMOV   S10, R2
    VMOV   R1, S10
    CMP    R1, R2
    BNE    ErrorFPU0
    VMOV   S11, R0     /*...S11..*/
    VMOV   R1, S11
    CMP    R1, R0
    BNE    ErrorFPU0
    VMOV   S11, R2
    VMOV   R1, S11
    CMP    R1, R2
    BNE    ErrorFPU0
    VMOV   S12, R0     /*...S12..*/
    VMOV   R1, S12
    CMP    R1, R0
    BNE    ErrorFPU0
    VMOV   S12, R2
    VMOV   R1, S12
    CMP    R1, R2
    BNE    ErrorFPU0
    VMOV   S13, R0     /*...S13..*/
    VMOV   R1, S13
    CMP    R1, R0
    BNE    ErrorFPU0
    VMOV   S13, R2
    VMOV   R1, S13
    CMP    R1, R2
    BNE    ErrorFPU0
    VMOV   S14, R0     /*...S14..*/
    VMOV   R1, S14
    CMP    R1, R0
    BNE    ErrorFPU0
    VMOV   S14, R2
    VMOV   R1, S14
    CMP    R1, R2
    BNE    ErrorFPU0
    VMOV   S15, R0     /*...S15..*/
    VMOV   R1, S15
    CMP    R1, R0
    BNE    ErrorFPU0
    VMOV   S15, R2
    VMOV   R1, S15
    CMP    R1, R2
    BNE    ErrorFPU0

    MOVS   R0, #FS_PASS          /* move PASS return into R0 */
    B      ContinueFPU

ASM_LABEL(ErrorFPU)
    LDR    R0, =FS_FAIL_CPU_FLOAT_1  /* move FAIL return into R0 */

ASM_LABEL(ContinueFPU)
    LDR.W   R2, =0xE000ED88    /* address of CPACR register */
    STR    R3, [R2]           /* original content of CPACR register */
    BX     LR

ASM_PUBLIC_END(FS_CM4_CM7_CPU_Float1)

/*******************************************************************************
 ******************************************************************************/
ASM_PUBLIC_BEGIN(FS_CM4_CM7_CPU_Float2)
ASM_PUBLIC_FUNC(FS_CM4_CM7_CPU_Float2)
ASM_LABEL(FS_CM4_CM7_CPU_Float2)

    LDR.W   R2, =0xE000ED88    /* address of CPACR register */
    LDR     R3, [R2]           /* content of CPACR register....R3 */
    MOV     R1, #CPACR_CP10_CP11_MASK  /* mask for enabling FPU */
    ORRS    R0, R3, R1         /* ORR of the mask and current state of CPACR register */
    STR     R0, [R2]           /* enable coprocessor if it isn't enabled */
    
    NOP                      /* delay to be sure the coprocessor is enabled before start of the test */
    NOP
    NOP
    NOP
    NOP   

    VPUSH {S16-S31}   /* push of FPU preserved registers on the stack */

    /* testing of FPU registers S6 - S31 for Stuck at faults */
    MOV    R0, #0x55555555    /* testing pattern  0x55555555 */
    MOV    R2, #0xAAAAAAAA    /* testing pattern  0xAAAAAAAA */

    VMOV   S16, R0     /*...S16..*/
    VMOV   R1, S16
    CMP    R1, R0
    BNE    ErrorFPU02
    VMOV   S16, R2
    VMOV   R1, S16
    CMP    R1, R2
    BNE    ErrorFPU02
    VMOV   S17, R0     /*...S17..*/
    VMOV   R1, S17
    CMP    R1, R0
    BNE    ErrorFPU02
    VMOV   S17, R2
    VMOV   R1, S17
    CMP    R1, R2
    BNE    ErrorFPU02
    VMOV   S18, R0     /*...S18..*/
    VMOV   R1, S18
    CMP    R1, R0
    BNE    ErrorFPU02
    VMOV   S18, R2
    VMOV   R1, S18
    CMP    R1, R2
    BNE    ErrorFPU02
    VMOV   S19, R0     /*...S19..*/
    VMOV   R1, S19
    CMP    R1, R0
    BNE    ErrorFPU02
    VMOV   S19, R2
    VMOV   R1, S19
    CMP    R1, R2
    BNE    ErrorFPU02
    VMOV   S20, R0     /*...S20..*/
    VMOV   R1, S20
    CMP    R1, R0
    BNE    ErrorFPU02
    VMOV   S20, R2
    VMOV   R1, S20
    CMP    R1, R2
    BNE    ErrorFPU02
    VMOV   S21, R0     /*...S21..*/
    VMOV   R1, S21
    CMP    R1, R0
    BNE    ErrorFPU02
    VMOV   S21, R2
    VMOV   R1, S21
    CMP    R1, R2
    BNE    ErrorFPU02
    VMOV   S22, R0     /*...S22..*/
    VMOV   R1, S22
    CMP    R1, R0
    BNE    ErrorFPU02
    VMOV   S22, R2
    VMOV   R1, S22
    CMP    R1, R2
    BNE    ErrorFPU02
    VMOV   S23, R0     /*...S23..*/
    VMOV   R1, S23
    CMP    R1, R0
    BNE    ErrorFPU02
    VMOV   S23, R2
    VMOV   R1, S23
    CMP    R1, R2
    BNE    ErrorFPU02
    VMOV   S24, R0      /*...S24..*/
    VMOV   R1, S24
    CMP    R1, R0
    BNE    ErrorFPU02
    VMOV   S24, R2
    VMOV   R1, S24
    CMP    R1, R2 // validation_mark FS_CPU_Float2 r1 None
    BNE    ErrorFPU02

    B     ContinueFPU02

ASM_LABEL(ErrorFPU02)   /* auxiliary label because of limited range of conditional branch instruction */
    B       ErrorFPU2

ASM_LABEL(ContinueFPU02)
    VMOV   S25, R0      /*...S25..*/
    VMOV   R1, S25
    CMP    R1, R0
    BNE    ErrorFPU02
    VMOV   S25, R2
    VMOV   R1, S25
    CMP    R1, R2
    BNE    ErrorFPU02
    VMOV   S26, R0     /*...S26..*/
    VMOV   R1, S26
    CMP    R1, R0
    BNE    ErrorFPU02
    VMOV   S26, R2
    VMOV   R1, S26
    CMP    R1, R2
    BNE    ErrorFPU02
    VMOV   S27, R0     /*...S27..*/
    VMOV   R1, S27
    CMP    R1, R0
    BNE    ErrorFPU02
    VMOV   S27, R2
    VMOV   R1, S27
    CMP    R1, R2
    BNE    ErrorFPU02
    VMOV   S28, R0     /*...S28..*/
    VMOV   R1, S28
    CMP    R1, R0
    BNE    ErrorFPU02
    VMOV   S28, R2
    VMOV   R1, S28
    CMP    R1, R2
    BNE    ErrorFPU02
    VMOV   S29, R0     /*...S29..*/
    VMOV   R1, S29
    CMP    R1, R0
    BNE    ErrorFPU02
    VMOV   S29, R2
    VMOV   R1, S29
    CMP    R1, R2
    BNE    ErrorFPU02
    VMOV   S30, R0     /*...S30..*/
    VMOV   R1, S30
    CMP    R1, R0
    BNE    ErrorFPU02
    VMOV   S30, R2
    VMOV   R1, S30
    CMP    R1, R2
    BNE    ErrorFPU02
    VMOV   S31, R0     /*...S31..*/
    VMOV   R1, S31
    CMP    R1, R0
    BNE    ErrorFPU02
    VMOV   S31, R2
    VMOV   R1, S31
    CMP    R1, R2
    BNE    ErrorFPU02

    MOVS   R0, #FS_PASS          /* move PASS return into R0 */
    B      ContinueFPU2

ASM_LABEL(ErrorFPU2)
    LDR    R0, =FS_FAIL_CPU_FLOAT_2  /* move FAIL return into R0 */

ASM_LABEL(ContinueFPU2)
    VPOP  {S16-S31}            /* pop of FPU preserved registers from the stack */
    LDR.W   R2, =0xE000ED88    /* address of CPACR register */
    STR    R3, [R2]            /* original content of CPACR register */
    BX     LR

ASM_PUBLIC_END(FS_CM4_CM7_CPU_Float2)


 ASM_ALIGN(4)
 ASM_END
