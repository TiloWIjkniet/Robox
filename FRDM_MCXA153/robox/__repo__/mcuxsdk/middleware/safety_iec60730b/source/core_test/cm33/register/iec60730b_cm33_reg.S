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

#define __ASM__
    #include "iec60730b_core.h"
    #include "asm_mac_common.h"
#undef  __ASM__

ASM_COMP_SPECIFIC_DIRECTIVES
 ASM_CODE_SECTION(.text)

/*******************************************************************************
 * Functions
 ******************************************************************************/
 ASM_PUBLIC(FS_CM33_CPU_Register_NDSP)
 ASM_PUBLIC(FS_CM33_CPU_NonStackedRegister)
 ASM_PUBLIC(FS_CM33_CPU_Primask_S)
 ASM_PUBLIC(FS_CM33_CPU_SPmain_S)
 ASM_PUBLIC(FS_CM33_CPU_SPmain_Limit_S)
 ASM_PUBLIC(FS_CM33_CPU_SPprocess_S)
 ASM_PUBLIC(FS_CM33_CPU_SPprocess_Limit_S)
 ASM_PUBLIC(FS_CM33_CPU_Control)
 ASM_PUBLIC(FS_CM33_CPU_Control_S)
 ASM_PUBLIC(FS_CM33_CPU_Control_NFPU)
 ASM_PUBLIC(FS_CM33_CPU_Special8PriorityLevels_S)

#if TZ_SUPPORT
 ASM_PUBLIC(FS_CM33_CPU_Primask_NS)
 ASM_PUBLIC(FS_CM33_CPU_SPmain_NS)
 ASM_PUBLIC(FS_CM33_CPU_SPmain_Limit_NS)
 ASM_PUBLIC(FS_CM33_CPU_SPprocess_NS)
 ASM_PUBLIC(FS_CM33_CPU_SPprocess_Limit_NS)
 ASM_PUBLIC(FS_CM33_CPU_Control_NS)
 ASM_PUBLIC(FS_CM33_CPU_Special8PriorityLevels_NS)
#endif

/*******************************************************************************
 * Implementation
 ******************************************************************************/
/*******************************************************************************
 ******************************************************************************/
ASM_PUBLIC_BEGIN(FS_CM33_CPU_Register_NDSP)
ASM_PUBLIC_FUNC(FS_CM33_CPU_Register_NDSP)
ASM_LABEL(FS_CM33_CPU_Register_NDSP)

    PUSH {R4-R7, LR}

    /* R0, R1 */
    LDR     R0, =0x55555555    /* load pattern into R0 */
    LDR     R1, =0x55555555    /* load pattern into R1 */
    CMP     R0, R1
    BNE     Endless_loop   /* if not the same, jump to endless loop */

    LDR     R0, =0xAAAAAAAA    /* load pattern into R0 */
    LDR     R1, =0xAAAAAAAA    /* load pattern into R1 */
    CMP     R0, R1 // validation_mark FS_CM33_CPU_Register_NDSP r0 Endless_loop
    BNE     Endless_loop   /* if not the same, jump to endless loop */
    /* APSR */
    LDR     R0, =0x50000000    /* N=0, Z=1, C=0, V=1, Q=0*/
    MSR     APSR_nzcvq, R0       /* move pattern into APSR */
    BMI     Endless_loop          /* if N corrupted */
    BNE     Endless_loop          /* if Z corrupted */
    BCS     Endless_loop          /* if C corrupted */
    BVC     Endless_loop          /* if V corrupted */

    MRS     R1,APSR        /* because of Q */
    CMP     R0, R1
    BNE     Error_CPU

    LDR     R0, =0xA8000000       /* N=1, Z=0, C=1, V=0, Q=1*/
    MSR     APSR_nzcvq, R0       /* move pattern into APSR */
    BPL     Endless_loop          /* if N corrupted */
    BEQ     Endless_loop          /* if Z corrupted */
    BCC     Endless_loop          /* if C corrupted */
    BVS     Endless_loop          /* if V corrupted */

    MRS     R1,APSR               /* because of Q */
    CMP     R0, R1
    BNE     Error_CPU
    /*  R14 (LR) */
    LDR     R1, =0x55555555    /* load pattern into R1 */
    LDR     R0, =0x55555555    /* load pattern into R0 */
    MOV     LR, R0             /* move pattern into LR*/
    CMP     LR, R1 // validation_mark FS_CM33_CPU_Register_NDSP r1 Endless_loop
    BNE     Endless_loop   /* if not the same, jump to endless loop */

    LDR     R0, =0xAAAAAAAA    /* load pattern into R0 */
    MOV     LR, R0             /* move pattern into LR */
    CMP     LR, R0
    BNE     Endless_loop  /* if not the same, jump to endless loop */

    B       Continue_cpu     /* if everything ok, jump over the endless loop */

ASM_LABEL(Endless_loop)   /* endless loop for case R0, R1 or LR is corrupted*/
    CPSID   i                      /* disable interrupts */
    B       Endless_loop

ASM_LABEL(Continue_cpu)
    LDR     R0, =0x55555555    /* load pattern into R0 */
    LDR     R1, =0xAAAAAAAA    /* load pattern into R1 */
    /* R2 */
    LDR     R2, =0x55555555    /* load pattern into R2 */
    CMP     R2, R0 // validation_mark FS_CM33_CPU_Register_NDSP r2 None
    BNE     Error_CPU          /* if not the same, jump to Error label */
    LDR     R2, =0xAAAAAAAA    /* load pattern into R2 */
    CMP     R2, R1
    BNE     Error_CPU          /* if not the same, jump to Error label */
    /* R3 */
    LDR     R3, =0x55555555    /* load pattern into R3 */
    CMP     R3, R0
    BNE     Error_CPU          /* if not the same, jump to Error label */
    LDR     R3, =0xAAAAAAAA    /* load pattern into R3 */
    CMP     R3, R1 // validation_mark FS_CM33_CPU_Register_NDSP r3 None
    BNE     Error_CPU          /* if not the same, jump to Error label */
    /* R4 */
    LDR     R4, =0x55555555    /* load pattern into R4 */
    CMP     R4, R0
    BNE     Error_CPU          /* if not the same, jump to Error label */
    LDR     R4, =0xAAAAAAAA    /* load pattern into R4 */
    CMP     R4, R1 // validation_mark FS_CM33_CPU_Register_NDSP r4 None
    BNE     Error_CPU          /* if not the same, jump to Error label */
    /* R5 */
    LDR     R5, =0x55555555    /* load pattern into R5 */
    CMP     R5, R0
    BNE     Error_CPU          /* if not the same, jump to Error label */
    LDR     R5, =0xAAAAAAAA    /* load pattern into R5 */
    CMP     R5, R1 // validation_mark FS_CM33_CPU_Register_NDSP r5 None
    BNE     Error_CPU          /* if not the same, jump to Error label */
    /* R6 */
    LDR     R6, =0x55555555    /* load pattern into R6 */
    CMP     R6, R0
    BNE     Error_CPU          /* if not the same, jump to Error label */
    LDR     R6, =0xAAAAAAAA    /* load pattern into R6 */
    CMP     R6, R1 // validation_mark FS_CM33_CPU_Register_NDSP r6 None
    BNE     Error_CPU          /* if not the same, jump to Error label */
    /* R7 */
    LDR     R7, =0x55555555    /* load pattern into R7 */
    CMP     R7, R0
    BNE     Error_CPU          /* if not the same, jump to Error label */
    LDR     R7, =0xAAAAAAAA    /* load pattern into R7 */
    CMP     R7, R1 // validation_mark FS_CM33_CPU_Register_NDSP r7 None
    BNE     Error_CPU          /* if not the same, jump to Error label */
    /* R12 */
    LDR     R2, =0x55555555    /* load pattern into R2 */
    MOV     R12, R2            /* move pattern into R12 */
    CMP     R12, R0 // validation_mark FS_CM33_CPU_Register_NDSP r12 None
    BNE     Error_CPU          /* if not the same, jump to Error label */
    MOV     R12, R1            /* move pattern into R12 */
    CMP     R12, R1
    BNE     Error_CPU          /* if not the same, jump to Error label */

    MOVS    R0, #FS_PASS          /* move PASS return into R0 */
    B       Continue           /* jump over the Error label to the end of the function */

ASM_LABEL(Error_CPU)           /* Error label */
    LDR    R0, =FS_FAIL_CPU_REGISTER  /* move FAIL return into R0 */

ASM_LABEL(Continue)
    POP   {R4-R7, PC}
    ASM_LITERAL

ASM_PUBLIC_END(FS_CM33_CPU_Register_NDSP)

/*******************************************************************************
 ******************************************************************************/
ASM_PUBLIC_BEGIN(FS_CM33_CPU_NonStackedRegister)
ASM_PUBLIC_FUNC(FS_CM33_CPU_NonStackedRegister)
ASM_LABEL(FS_CM33_CPU_NonStackedRegister)

    PUSH {R8-R11, LR}

    /* saving registers content on the stack */
    LDR     R0, =0x55555555   /* load pattern into R0 */
    LDR     R1, =0xAAAAAAAA   /* load pattern into R1 */
    /* R8 */
    MOV     R8, R0
    CMP     R8, R0 // validation_mark FS_CM33_CPU_NonStackedRegister r8 None
    BNE     ErrorNonStacked   /* if not the same, jump to ErrorNonStacked label */
    MOV     R8, R1            /* move pattern into R8 */
    CMP     R8, R1 // validation_mark FS_CM33_CPU_NonStackedRegister r8 None
    BNE     ErrorNonStacked   /* if not the same, jump to ErrorNonStacked label */
    /* R9 */
    MOV     R9, R0            /* move pattern into R9 */
    CMP     R9, R0 // validation_mark FS_CM33_CPU_NonStackedRegister r9 None
    BNE     ErrorNonStacked   /* if not the same, jump to ErrorNonStacked label */
    MOV     R9, R1            /* move pattern into R9 */
    CMP     R9, R1 // validation_mark FS_CM33_CPU_NonStackedRegister r9 None
    BNE     ErrorNonStacked   /* if not the same, jump to ErrorNonStacked label */
    /* R10 */
    MOV     R10, R0           /* move pattern into R10 */
    CMP     R10, R0
    BNE     ErrorNonStacked   /* if not the same, jump to ErrorNonStacked label */
    MOV     R10, R1           /* move pattern into R10 */
    CMP     R10, R1 // validation_mark FS_CM33_CPU_NonStackedRegister r10 None
    BNE     ErrorNonStacked   /* if not the same, jump to ErrorNonStacked label */
    /* R11 */
    MOV     R11, R0           /* move pattern into R11 */
    CMP     R11, R0 // validation_mark FS_CM33_CPU_NonStackedRegister r11 None
    BNE     ErrorNonStacked   /* if not the same, jump to ErrorNonStacked label */
    MOV     R11, R1           /* move pattern into R11 */
    CMP     R11, R1
    BNE     ErrorNonStacked   /* if not the same, jump to ErrorNonStacked label */

    MOVS    R0, #FS_PASS  /* move PASS return into R0 */
    B       ContinueNonStacked          /* jump over the ErrorNonStacked */

ASM_LABEL(ErrorNonStacked)          /* ErrorNonStacked label*/
    LDR     R0, =FS_FAIL_CPU_NONSTACKED_REGISTER     /* move FAIL return into R0 */

ASM_LABEL(ContinueNonStacked)
    /* loading registers content from the stack */
    POP    {R8-R11, PC}
    ASM_LITERAL

ASM_PUBLIC_END(FS_CM33_CPU_NonStackedRegister)

/*******************************************************************************
 ******************************************************************************/
ASM_PUBLIC_BEGIN(FS_CM33_CPU_Primask_S)
ASM_PUBLIC_FUNC(FS_CM33_CPU_Primask_S)
ASM_LABEL(FS_CM33_CPU_Primask_S)

    MRS     R1, PRIMASK              /* backup of PRIMASK */
    LDR     R2, =0x00000001          /* load pattern into R2 */
    MSR     PRIMASK, R2              /* move pattern into PRIMASK */
    MRS     R3, PRIMASK              /* move pattern from PRIMASK to R3 */
    CMP     R3, #0x00000001 // validation_mark FS_CM33_CPU_Primask_S r3 None
    BNE     ErrorPRIMASK_S           /* if not the same, jump to ErrorPRIMASK label */

    LDR     R3, =0x00000000          /* load pattern into R3 */
    MSR     PRIMASK, R3              /* move pattern into PRIMASK */
    MRS     R2, PRIMASK              /* move pattern from PRIMASK to R2 */
    MSR     PRIMASK, R1              /* restore PRIMASK */
    CMP     R3, R2
    BNE     ErrorPRIMASK_S           /* if not the same, jump to ErrorPRIMASK label */

    MOVS    R0, #FS_PASS         /* move PASS return into R0 */
    B       ContinuePRIMASK_S        /* jump over the Error label */

ASM_LABEL(ErrorPRIMASK_S)            /* Error label */
    LDR    R0, =FS_FAIL_CPU_PRIMASK  /* move FAIL return into R0 */

ASM_LABEL(ContinuePRIMASK_S)
    BX     LR

ASM_PUBLIC_END(FS_CM33_CPU_Primask_S)

/*******************************************************************************
 ******************************************************************************/
#if TZ_SUPPORT
ASM_PUBLIC_BEGIN(FS_CM33_CPU_Primask_NS)
ASM_PUBLIC_FUNC(FS_CM33_CPU_Primask_NS)
ASM_LABEL(FS_CM33_CPU_Primask_NS)

    MRS     R1, PRIMASK_NS           /* backup of PRIMASK */
    LDR     R2, =0x00000001          /* load pattern into R2 */
    MSR     PRIMASK_NS, R2           /* move pattern into PRIMASK */
    MRS     R3, PRIMASK_NS           /* move pattern from PRIMASK to R3 */
    CMP     R3, #0x00000001
    BNE     ErrorPRIMASK_NS          /* if not the same, jump to ErrorPRIMASK label */

    LDR     R3, =0x00000000          /* load pattern into R3 */
    MSR     PRIMASK_NS, R3           /* move pattern into PRIMASK */
    MRS     R2, PRIMASK_NS           /* move pattern from PRIMASK to R2 */
    MSR     PRIMASK_NS, R1           /* restore PRIMASK */
    CMP     R3, R2 // validation_mark FS_CM33_CPU_Primask_NS r2 None
    BNE     ErrorPRIMASK_NS          /* if not the same, jump to ErrorPRIMASK label */

    MOVS    R0, #FS_PASS         /* move PASS return into R0 */
    B       ContinuePRIMASK_NS       /* jump over the Error label */

ASM_LABEL(ErrorPRIMASK_NS)           /* Error label */
    LDR    R0, =FS_FAIL_CPU_PRIMASK  /* move FAIL return into R0 */

ASM_LABEL(ContinuePRIMASK_NS)
    BX     LR

ASM_PUBLIC_END(FS_CM33_CPU_Primask_NS)
#endif

/*******************************************************************************
 ******************************************************************************/
ASM_PUBLIC_BEGIN(FS_CM33_CPU_SPmain_S)
ASM_PUBLIC_FUNC(FS_CM33_CPU_SPmain_S)
ASM_LABEL(FS_CM33_CPU_SPmain_S)

    MRS     R3, MSP             /* backup of SP */

    LDR     R1, =0x55555554     /* load pattern into R1 */
    MSR     MSP, R1             /* move pattern into SP */
    MRS     R2, MSP             /* move SP into R2 */
    CMP     R1, R2              /* compare R1 with R2 */
    BNE     Error_SP_main_S     /* if not the same, jump to ErrorSP_main label */

    LDR     R1, =0xAAAAAAA8     /* load pattern into R1 */
    MSR     MSP, R1             /* move pattern into SP */
    MRS     R2, MSP             /* move SP into R2 */
    CMP     R1, R2              /* compare R1 with R2 */ // validation_mark FS_CM33_CPU_SPmain_S r1 Error_SP_main_S
    BNE     Error_SP_main_S     /* if not the same, jump to ErrorSP_main label */

    B       Continue_SP_main_S  /* jump over the ErrorSP_main label */

ASM_LABEL(Error_SP_main_S)      /* Endless loop */
    CPSID   i                   /* Disable interrupts */
    B       Error_SP_main_S

ASM_LABEL(Continue_SP_main_S)
    MSR     MSP, R3              /* restore SP */
    MOVS    R0, #FS_PASS    /* move PASS return into R0 */
    BX   LR
    ASM_LITERAL

ASM_PUBLIC_END(FS_CM33_CPU_SPmain_S)

/*******************************************************************************
 ******************************************************************************/
#if TZ_SUPPORT
ASM_PUBLIC_BEGIN(FS_CM33_CPU_SPmain_NS)
ASM_PUBLIC_FUNC(FS_CM33_CPU_SPmain_NS)
ASM_LABEL(FS_CM33_CPU_SPmain_NS)

    MRS     R3, MSP_NS           /* backup of SP */

    LDR     R1, =0x55555554      /* load pattern into R1 */
    MSR     MSP_NS, R1           /* move pattern into SP */
    MRS     R2, MSP_NS           /* move SP into R2 */
    CMP     R1, R2               /* compare R1 with R2 */
    BNE     Error_SP_main_NS     /* if not the same, jump to ErrorSP_main label */

    LDR     R1, =0xAAAAAAA8      /* load pattern into R1 */
    MSR     MSP_NS, R1           /* move pattern into SP */
    MRS     R2, MSP_NS           /* move SP into R2 */
    CMP     R1, R2               /* compare R1 with R2 */ // validation_mark FS_CM33_CPU_SPmain_NS r1 Error_SP_main_NS
    BNE     Error_SP_main_NS     /* if not the same, jump to ErrorSP_main label */

    B       Continue_SP_main_NS  /* jump over the ErrorSP_main label */

ASM_LABEL(Error_SP_main_NS)      /* Endless loop */
    CPSID   i                    /* Disable interrupts */
    B       Error_SP_main_NS

ASM_LABEL(Continue_SP_main_NS)
    MSR     MSP_NS, R3           /* restore SP */
    MOVS    R0, #FS_PASS     /* move PASS return into R0 */
    BX   LR
    ASM_LITERAL

ASM_PUBLIC_END(FS_CM33_CPU_SPmain_NS)
#endif

/*******************************************************************************
 ******************************************************************************/
ASM_PUBLIC_BEGIN(FS_CM33_CPU_SPmain_Limit_S)
ASM_PUBLIC_FUNC(FS_CM33_CPU_SPmain_Limit_S)
ASM_LABEL(FS_CM33_CPU_SPmain_Limit_S)

    MRS     R3, MSPLIM                /* backup of SP lim */

    LDR     R1, =0x55555550           /* load pattern into R1 */
    MSR     MSPLIM, R1                /* move pattern into SP lim */
    MRS     R2, MSPLIM                /* move SP lim into R2 */
    CMP     R1, R2                    /* compare R1 with R2 */
    BNE     Error_SP_main_Limit_S     /* if not the same, jump to ErrorSP_main label */

    LDR     R1, =0xAAAAAAA8           /* load pattern into R1 */
    MSR     MSPLIM, R1                /* move pattern into SP lim */
    MRS     R2, MSPLIM                /* move SP lim into R2 */
    CMP     R1, R2                    /* compare R1 with R2 */ // validation_mark FS_CM33_CPU_SPmain_Limit_S r1 Error_SP_main_Limit_S
    BNE     Error_SP_main_Limit_S     /* if not the same, jump to ErrorSP_main label */

    B       Continue_SP_main_Limit_S  /* jump over the ErrorSP_main label */

ASM_LABEL(Error_SP_main_Limit_S)      /* Endless loop */
    CPSID   i                         /* Disable interrupts */
    B       Error_SP_main_Limit_S

ASM_LABEL(Continue_SP_main_Limit_S)
    MSR     MSPLIM, R3                /* restore SP lim */
    MOVS    R0, #FS_PASS          /* move PASS return into R0 */
    BX   LR
    ASM_LITERAL

ASM_PUBLIC_END(FS_CM33_CPU_SPmain_Limit_S)

/*******************************************************************************
 ******************************************************************************/
#if TZ_SUPPORT
ASM_PUBLIC_BEGIN(FS_CM33_CPU_SPmain_Limit_NS)
ASM_PUBLIC_FUNC(FS_CM33_CPU_SPmain_Limit_NS)
ASM_LABEL(FS_CM33_CPU_SPmain_Limit_NS)

    MRS     R3, MSPLIM_NS              /* backup of SP lim */

    LDR     R1, =0x55555550            /* load pattern into R1 */
    MSR     MSPLIM_NS, R1              /* move pattern into SP lim */
    MRS     R2, MSPLIM_NS              /* move SP lim into R2 */
    CMP     R1, R2                     /* compare R1 with R2 */
    BNE     Error_SP_main_Limit_NS     /* if not the same, jump to ErrorSP_main label */

    LDR     R1, =0xAAAAAAA8            /* load pattern into R1 */
    MSR     MSPLIM_NS, R1              /* move pattern into SP lim */
    MRS     R2, MSPLIM_NS              /* move SP lim into R2 */
    CMP     R1, R2                     /* compare R1 with R2 */ // validation_mark FS_CM33_CPU_SPmain_Limit_NS r1 Error_SP_main_Limit_NS
    BNE     Error_SP_main_Limit_NS     /* if not the same, jump to ErrorSP_main label */

    B       Continue_SP_main_Limit_NS  /* jump over the ErrorSP_main label */

ASM_LABEL(Error_SP_main_Limit_NS)      /* Endless loop */
    CPSID   i                          /* Disable interrupts */
    B       Error_SP_main_Limit_NS

ASM_LABEL(Continue_SP_main_Limit_NS)
    MSR     MSPLIM_NS, R3              /* restore SP lim */
    MOVS    R0, #FS_PASS           /* move PASS return into R0 */
    BX   LR
    ASM_LITERAL

ASM_PUBLIC_END(FS_CM33_CPU_SPmain_Limit_NS)
#endif

/*******************************************************************************
 ******************************************************************************/
ASM_PUBLIC_BEGIN(FS_CM33_CPU_SPprocess_S)
ASM_PUBLIC_FUNC(FS_CM33_CPU_SPprocess_S)
ASM_LABEL(FS_CM33_CPU_SPprocess_S)

    MRS     R3, PSP                /* backup of SP */

    LDR     R1, =0x55555554        /* load pattern into R1 */
    MSR     PSP, R1                /* move pattern into SP */
    MRS     R2, PSP                /* move SP into R2 */
    CMP     R1, R2                 /* compare R1 with R2 */
    BNE     Error_SP_process_S     /* if not the same, jump to ErrorSP_process label */

    LDR     R1, =0xAAAAAAA8        /* load pattern into R1 */
    MSR     PSP, R1                /* move pattern into SP */
    MRS     R2, PSP                /* move SP into R2 */
    CMP     R1, R2                 /* compare R1 with R2 */ // validation_mark FS_CM33_CPU_SPprocess_S r1 Error_SP_process_S
    BNE     Error_SP_process_S     /* if not the same, jump to ErrorSP_process label */

    B       Continue_SP_process_S  /* jump over the ErrorSP_process label */

ASM_LABEL(Error_SP_process_S)      /* Endless loop */
    CPSID   i                      /* Disable interrupts */
    B       Error_SP_process_S

ASM_LABEL(Continue_SP_process_S)
    MSR     PSP, R3                /* restore SP */
    MOVS    R0, #FS_PASS       /* move PASS return into R0 */
    BX   LR
    ASM_LITERAL

ASM_PUBLIC_END(FS_CM33_CPU_SPprocess_S)

/*******************************************************************************
 ******************************************************************************/
#if TZ_SUPPORT
ASM_PUBLIC_BEGIN(FS_CM33_CPU_SPprocess_NS)
ASM_PUBLIC_FUNC(FS_CM33_CPU_SPprocess_NS)
ASM_LABEL(FS_CM33_CPU_SPprocess_NS)

    MRS     R3, PSP_NS              /* backup of SP */

    LDR     R1, =0x55555554         /* load pattern into R1 */
    MSR     PSP_NS, R1              /* move pattern into SP */
    MRS     R2, PSP_NS              /* move SP into R2 */
    CMP     R1, R2                  /* compare R1 with R2 */
    BNE     Error_SP_process_NS     /* if not the same, jump to ErrorSP_process label */

    LDR     R1, =0xAAAAAAA8         /* load pattern into R1 */
    MSR     PSP_NS, R1              /* move pattern into SP */
    MRS     R2, PSP_NS              /* move SP into R2 */
    CMP     R1, R2                  /* compare R1 with R2 */ // validation_mark FS_CM33_CPU_SPprocess_NS r1 Error_SP_process_NS
    BNE     Error_SP_process_NS     /* if not the same, jump to ErrorSP_process label */

    B       Continue_SP_process_NS  /* jump over the ErrorSP_process label */

ASM_LABEL(Error_SP_process_NS)      /* Endless loop */
    CPSID   i                       /* Disable interrupts */
    B       Error_SP_process_NS

ASM_LABEL(Continue_SP_process_NS)
    MSR     PSP_NS, R3              /* restore SP */
    MOVS    R0, #FS_PASS        /* move PASS return into R0 */
    BX   LR
    ASM_LITERAL

ASM_PUBLIC_END(FS_CM33_CPU_SPprocess_NS)
#endif

/*******************************************************************************
 ******************************************************************************/
ASM_PUBLIC_BEGIN(FS_CM33_CPU_SPprocess_Limit_S)
ASM_PUBLIC_FUNC(FS_CM33_CPU_SPprocess_Limit_S)
ASM_LABEL(FS_CM33_CPU_SPprocess_Limit_S)

    MRS     R3, PSPLIM                /* backup of SP lim */

    LDR     R1, =0x55555550           /* load pattern into R1 */
    MSR     PSPLIM, R1                /* move pattern into SP lim */
    MRS     R2, PSPLIM                /* move SP lim into R2 */
    CMP     R1, R2                    /* compare R1 with R2 */
    BNE     Error_SP_process_Limit_S     /* if not the same, jump to ErrorSP_process label */

    LDR     R1, =0xAAAAAAA8           /* load pattern into R1 */
    MSR     PSPLIM, R1                /* move pattern into SP lim */
    MRS     R2, PSPLIM                /* move SP lim into R2 */
    CMP     R1, R2                    /* compare R1 with R2 */ // validation_mark FS_CM33_CPU_SPprocess_Limit_S r1 Error_SP_process_Limit_S
    BNE     Error_SP_process_Limit_S     /* if not the same, jump to ErrorSP_process label */

    B       Continue_SP_process_Limit_S  /* jump over the ErrorSP_process label */

ASM_LABEL(Error_SP_process_Limit_S)      /* Endless loop */
    CPSID   i                         /* Disable interrupts */
    B       Error_SP_process_Limit_S

ASM_LABEL(Continue_SP_process_Limit_S)
    MSR     PSPLIM, R3                /* restore SP lim */
    MOVS    R0, #FS_PASS          /* move PASS return into R0 */
    BX   LR
    ASM_LITERAL

ASM_PUBLIC_END(FS_CM33_CPU_SPprocess_Limit_S)

/*******************************************************************************
 ******************************************************************************/
#if TZ_SUPPORT
ASM_PUBLIC_BEGIN(FS_CM33_CPU_SPprocess_Limit_NS)
ASM_PUBLIC_FUNC(FS_CM33_CPU_SPprocess_Limit_NS)
ASM_LABEL(FS_CM33_CPU_SPprocess_Limit_NS)

    MRS     R3, PSPLIM_NS              /* backup of SP lim */

    LDR     R1, =0x55555550            /* load pattern into R1 */
    MSR     PSPLIM_NS, R1              /* move pattern into SP lim */
    MRS     R2, PSPLIM_NS              /* move SP lim into R2 */
    CMP     R1, R2                     /* compare R1 with R2 */
    BNE     Error_SP_process_Limit_NS     /* if not the same, jump to ErrorSP_process label */

    LDR     R1, =0xAAAAAAA8            /* load pattern into R1 */
    MSR     PSPLIM_NS, R1              /* move pattern into SP lim */
    MRS     R2, PSPLIM_NS              /* move SP lim into R2 */
    CMP     R1, R2                     /* compare R1 with R2 */ // validation_mark FS_CM33_CPU_SPprocess_Limit_NS r1 Error_SP_process_Limit_NS
    BNE     Error_SP_process_Limit_NS     /* if not the same, jump to ErrorSP_process label */

    B       Continue_SP_process_Limit_NS  /* jump over the ErrorSP_process label */

ASM_LABEL(Error_SP_process_Limit_NS)      /* Endless loop */
    CPSID   i                          /* Disable interrupts */
    B       Error_SP_process_Limit_NS

ASM_LABEL(Continue_SP_process_Limit_NS)
    MSR     PSPLIM_NS, R3              /* restore SP lim */
    MOVS    R0, #FS_PASS           /* move PASS return into R0 */
    BX   LR
    ASM_LITERAL

ASM_PUBLIC_END(FS_CM33_CPU_SPprocess_Limit_NS)
#endif

/*******************************************************************************
 ******************************************************************************/
ASM_PUBLIC_BEGIN(FS_CM33_CPU_Control)
ASM_PUBLIC_FUNC(FS_CM33_CPU_Control)
ASM_LABEL(FS_CM33_CPU_Control)

    MOV     R3, SP                    /* store value of SP */
    MRS     R2, CONTROL               /* Backup of CONTROL in R2 */

    MOVS    R0, #0x00000002           /* pattern for SP_process and SFPA */
    MSR     CONTROL, R0               /* move pattern into CONTROL */
    MRS     R1, CONTROL               /* move pattern from CONTROL */
    CMP     R1, #0x00000002 // validation_mark FS_CM33_CPU_Control r1 None
    BNE     Error_Control             /* if not the same, jump to Error_Control label */

    MOVS    R0, #0x00000004           /* pattern for SP_main and FPCA */
    MSR     CONTROL, R0               /* move pattern into CONTROL */
    MRS     R1, CONTROL               /* move pattern from CONTROL */
    CMP     R1, #0x00000004 // validation_mark FS_CM33_CPU_Control r1 None
    BNE     Error_Control             /* if not the same, jump to Error_Control label */

    MSR     CONTROL, R2               /* restore CONTROL */
    MOVS    R0, #FS_PASS          /* move PASS return into R0 */
    B       Continue_Control          /* jump over the Error_Control label */

ASM_LABEL(Error_Control)              /* Error_Control label */
    MOV     R0, #FS_FAIL_CPU_CONTROL  /* move the FAIL return into R0 */
    MOV     SP, R3                    /* in case of error state, with CONTROL changed, to use original Stack */

ASM_LABEL(Continue_Control)
    BX      LR

ASM_PUBLIC_END(FS_CM33_CPU_Control)


/*******************************************************************************
 ******************************************************************************/
ASM_PUBLIC_BEGIN(FS_CM33_CPU_Control_NFPU)
ASM_PUBLIC_FUNC(FS_CM33_CPU_Control_NFPU)
ASM_LABEL(FS_CM33_CPU_Control_NFPU)

    MOV     R3, SP                    /* store value of SP */
    MRS     R2, CONTROL               /* Backup of CONTROL in R2 */

    MOVS    R0, #0x00000002           /* pattern for SP_process and SFPA */
    MSR     CONTROL, R0               /* move pattern into CONTROL */
    MRS     R1, CONTROL               /* move pattern from CONTROL */
    CMP     R1, #0x00000002  // validation_mark FS_CM33_CPU_Control_NFPU r1 None
    BNE     Error_Control_NFPU        /* if not the same, jump to Error_Control label */

    MOVS    R0, #0x00000000           /* pattern for SP_main and FPCA */
    MSR     CONTROL, R0               /* move pattern into CONTROL */
    MRS     R1, CONTROL               /* move pattern from CONTROL */
    CMP     R1, #0x00000000  // validation_mark FS_CM33_CPU_Control_NFPU r1 None
    BNE     Error_Control_NFPU        /* if not the same, jump to Error_Control label */

    MSR     CONTROL, R2               /* restore CONTROL */
    MOVS    R0, #FS_PASS              /* move PASS return into R0 */
    B       Continue_Control_NFPU     /* jump over the Error_Control label */

ASM_LABEL(Error_Control_NFPU)              /* Error_Control label */
    MOV     R0, #FS_FAIL_CPU_CONTROL  /* move the FAIL return into R0 */
    MOV     SP, R3                    /* in case of error state, with CONTROL changed, to use original Stack */

ASM_LABEL(Continue_Control_NFPU)
    BX      LR

ASM_PUBLIC_END(FS_CM33_CPU_Control_NFPU)

/*******************************************************************************
 ******************************************************************************/
ASM_PUBLIC_BEGIN(FS_CM33_CPU_Control_S)
ASM_PUBLIC_FUNC(FS_CM33_CPU_Control_S)
ASM_LABEL(FS_CM33_CPU_Control_S)

    MOV     R3, SP                    /* store value of SP */
    MRS     R2, CONTROL               /* Backup of CONTROL in R2 */

    MOVS    R0, #0x0000000A           /* pattern for SP_process and SFPA */
    MSR     CONTROL, R0               /* move pattern into CONTROL */
    MRS     R1, CONTROL               /* move pattern from CONTROL */
    CMP     R1, #0x0000000A // validation_mark FS_CM33_CPU_Control_S r1 None
    BNE     Error_Control_S           /* if not the same, jump to Error_Control_S label */

    MOVS    R0, #0x00000004           /* pattern for SP_main and FPCA */
    MSR     CONTROL, R0               /* move pattern into CONTROL */
    MRS     R1, CONTROL               /* move pattern from CONTROL */
    CMP     R1, #0x00000004 // validation_mark FS_CM33_CPU_Control_S r1 None
    BNE     Error_Control_S           /* if not the same, jump to Error_Control_S label */

    MSR     CONTROL, R2               /* restore CONTROL */
    MOVS    R0, #FS_PASS          /* move PASS return into R0 */
    B       Continue_Control_S        /* jump over the Error_Control_S label */

ASM_LABEL(Error_Control_S)            /* Error_Control_S label */
    MOV     R0, #FS_FAIL_CPU_CONTROL  /* move the FAIL return into R0 */
    MOV     SP, R3                    /* in case of error state, with CONTROL changed, to use original Stack */

ASM_LABEL(Continue_Control_S)
    BX      LR

ASM_PUBLIC_END(FS_CM33_CPU_Control_S)

/*******************************************************************************
 ******************************************************************************/
#if TZ_SUPPORT
ASM_PUBLIC_BEGIN(FS_CM33_CPU_Control_NS)
ASM_PUBLIC_FUNC(FS_CM33_CPU_Control_NS)
ASM_LABEL(FS_CM33_CPU_Control_NS)

    MRS     R3, SP_NS                 /* store value of SP */
    MRS     R2, CONTROL_NS            /* Backup of CONTROL in R2 */

    MOVS    R0, #0x00000002           /* pattern for SP_process */
    MSR     CONTROL_NS, R0            /* move pattern into CONTROL */
    MRS     R1, CONTROL_NS            /* move pattern from CONTROL */
    CMP     R1, #0x00000002
    BNE     Error_Control_NS          /* if not the same, jump to Error_Control_NS label */

    MOVS    R0, #0x00000004           /* pattern for SP_main and FPCA */
    MSR     CONTROL_NS, R0            /* move pattern into CONTROL */
    MRS     R1, CONTROL_NS            /* move pattern from CONTROL */
    CMP     R1, #0x00000004 // validation_mark FS_CM33_CPU_Control_NS r1 None
    BNE     Error_Control_NS          /* if not the same, jump to Error_Control_NS label */

    MSR     CONTROL_NS, R2            /* restore CONTROL */
    MOVS    R0, #FS_PASS          /* move PASS return into R0 */
    B       Continue_Control_NS       /* jump over the Error_Control_NS label */

ASM_LABEL(Error_Control_NS)           /* Error_Control_NS label */
    LDR     R0, =FS_FAIL_CPU_CONTROL  /* move the FAIL return into R0 */
    MSR     SP_NS, R3                 /* in case of error state, with CONTROL changed, to use original Stack */

ASM_LABEL(Continue_Control_NS)
    BX      LR

ASM_PUBLIC_END(FS_CM33_CPU_Control_NS)
#endif

/*******************************************************************************
 ******************************************************************************/
ASM_PUBLIC_BEGIN(FS_CM33_CPU_Special8PriorityLevels_S)
ASM_PUBLIC_FUNC(FS_CM33_CPU_Special8PriorityLevels_S)
ASM_LABEL(FS_CM33_CPU_Special8PriorityLevels_S)

    /* basepri
     * Register priority value fields are eight bits wide, and non-implemented
     * low-order bits read as zero and ignore writes. */

    MRS     R2, BASEPRI              /* backup value from base priority Register to general register  */
    LDR     R1, =0x00
    MSR     BASEPRI, R1              /* cleared BASEPRI before start of the test */

    LDR     R1, =0xA0                /* load pattern into R1*/
    MSR     BASEPRI, R1
    MRS     R0, BASEPRI
    CMP     R0, R1 // validation_mark FS_CM33_CPU_Special8PriorityLevels_S r1 None
    BNE     Error_Special_8_S        /* if not the same, jump to Error label */

    LDR     R1, =0x40                /* load pattern into R1*/
    MSR     BASEPRI, R1
    MRS     R0, BASEPRI
    MSR     BASEPRI, R2              /* Move backup value back to base priority register from general Register */

    CMP     R0, R1
    BNE     Error_Special_8_S        /* if not the same, jump to Error label */

    /* fault mask */
    MRS     R2, FAULTMASK            /* backup value from fault Register to general register */
    LDR     R1, =0x1                 /* load pattern into R1*/
    MSR     FAULTMASK, R1
    MRS     R0, FAULTMASK
    CMP     R0, R1
    BNE     Error_Special_8_S        /* if not the same, jump to Error label */

    LDR     R1, =0x0                 /* load pattern into R1*/
    MSR     FAULTMASK, R1
    MRS     R0, FAULTMASK
    MSR     FAULTMASK, R2            /* Move backup value back to fault register from general Register */

    CMP     R0, R1
    BNE     Error_Special_8_S        /* if not the same, jump to error label  */

    MOV     R0, #0                   /* move 0 into R0 for PASS return */
    B       Continue_Special_8_S     /* if everything ok, jump over the error label */
ASM_LABEL(Error_Special_8_S)         /* Error label */
    LDR    R0, =FS_FAIL_CPU_SPECIAL  /* move FAIL return into R0 */

ASM_LABEL(Continue_Special_8_S)
    BX   LR
    ASM_LITERAL

ASM_PUBLIC_END(FS_CM33_CPU_Special8PriorityLevels_S)

/*******************************************************************************
 ******************************************************************************/
#if TZ_SUPPORT
ASM_PUBLIC_BEGIN(FS_CM33_CPU_Special8PriorityLevels_NS)
ASM_PUBLIC_FUNC(FS_CM33_CPU_Special8PriorityLevels_NS)
ASM_LABEL(FS_CM33_CPU_Special8PriorityLevels_NS)

    /* basepri
     * Register priority value fields are eight bits wide, and non-implemented
     * low-order bits read as zero and ignore writes. */

    MRS     R2, BASEPRI_NS           /* backup value from base priority Register to general register  */
    LDR     R1, =0x00
    MSR     BASEPRI_NS, R1           /* cleared BASEPRI before start of the test */

    LDR     R1, =0xA0                /* load pattern into R1*/
    MSR     BASEPRI_NS, R1
    MRS     R0, BASEPRI_NS
    CMP     R0, R1
    BNE     Error_Special_8_NS       /* if not the same, jump to Error label */

    LDR     R1, =0x40                /* load pattern into R1*/
    MSR     BASEPRI_NS, R1
    MRS     R0, BASEPRI_NS
    MSR     BASEPRI_NS, R2           /* Move backup value back to base priority register from general Register */

    CMP     R0, R1
    BNE     Error_Special_8_NS       /* if not the same, jump to Error label */

    /* fault mask */
    MRS     R2, FAULTMASK_NS         /* backup value from fault Register to general register */
    LDR     R1, =0x1                 /* load pattern into R1*/
    MSR     FAULTMASK_NS, R1
    MRS     R0, FAULTMASK_NS
    CMP     R0, R1 // validation_mark FS_CM33_CPU_Special8PriorityLevels_NS r1 None
    BNE     Error_Special_8_NS       /* if not the same, jump to Error label */

    LDR     R1, =0x0                 /* load pattern into R1*/
    MSR     FAULTMASK_NS, R1
    MRS     R0, FAULTMASK_NS
    MSR     FAULTMASK_NS, R2         /* Move backup value back to fault register from general Register */

    CMP     R0, R1
    BNE     Error_Special_8_NS       /* if not the same, jump to error label  */

    MOV     R0, #0                   /* move 0 into R0 for PASS return */
    B       Continue_Special_8_NS    /* if everything ok, jump over the error label */
ASM_LABEL(Error_Special_8_NS)        /* Error label */
    LDR    R0, =FS_FAIL_CPU_SPECIAL  /* move FAIL return into R0 */

ASM_LABEL(Continue_Special_8_NS)
    BX   LR
    ASM_LITERAL

ASM_PUBLIC_END(FS_CM33_CPU_Special8PriorityLevels_NS)
#endif


 ASM_ALIGN(4)
 ASM_END
