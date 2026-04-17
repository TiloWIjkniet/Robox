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

#define __ASM__
    #include "iec60730b_core.h"
    #include "asm_mac_common.h"
#undef  __ASM__

ASM_COMP_SPECIFIC_DIRECTIVES
 ASM_CODE_SECTION(.text)

/*******************************************************************************
 * Functions
 ******************************************************************************/
 ASM_PUBLIC(FS_CM33_CPU_Register)


/*******************************************************************************
 * Implementation
 ******************************************************************************/
/*******************************************************************************
 ******************************************************************************/
ASM_PUBLIC_BEGIN(FS_CM33_CPU_Register)
ASM_PUBLIC_FUNC(FS_CM33_CPU_Register)
ASM_LABEL(FS_CM33_CPU_Register)
    PUSH {R4-R7, LR}

    /* R0, R1 */
    LDR     R0, =0x55555555    /* load pattern into R0 */
    LDR     R1, =0x55555555    /* load pattern into R1 */
    CMP     R0, R1
    BNE     Endless_loop   /* if not the same, jump to endless loop */

    LDR     R0, =0xAAAAAAAA    /* load pattern into R0 */
    LDR     R1, =0xAAAAAAAA    /* load pattern into R1 */
    CMP     R0, R1 // validation_mark FS_CM33_CPU_Register r0 Endless_loop
    BNE     Endless_loop   /* if not the same, jump to endless loop */
    /* APSR */
    LDR     R0, =0x50050000    /* N=0, Z=1, C=0, V=1, Q=0, GE = 0x5 */
    MSR     APSR_nzcvqg, R0       /* move pattern into APSR */
    BMI     Endless_loop          /* if N corrupted */
    BNE     Endless_loop          /* if Z corrupted */
    BCS     Endless_loop          /* if C corrupted */
    BVC     Endless_loop          /* if V corrupted */

    MRS     R1,APSR        /* because of Q, GE fields */
    CMP     R0, R1
    BNE     Error_CPU

    LDR     R0, =0xA80A0000       /* N=1, Z=0, C=1, V=0, Q=1, GE = 0xA */
    MSR     APSR_nzcvqg, R0       /* move pattern into APSR */
    BPL     Endless_loop          /* if N corrupted */
    BEQ     Endless_loop          /* if Z corrupted */
    BCC     Endless_loop          /* if C corrupted */
    BVS     Endless_loop          /* if V corrupted */

    MRS     R1,APSR               /* because of Q, GE fields */
    CMP     R0, R1
    BNE     Error_CPU
    /*  R14 (LR) */
    LDR     R1, =0x55555555    /* load pattern into R1 */
    LDR     R0, =0x55555555    /* load pattern into R0 */
    MOV     LR, R0             /* move pattern into LR*/
    CMP     LR, R1 // validation_mark FS_CM33_CPU_Register r1 Endless_loop
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
    CMP     R2, R0 // validation_mark FS_CM33_CPU_Register r2 None
    BNE     Error_CPU          /* if not the same, jump to Error label */
    LDR     R2, =0xAAAAAAAA    /* load pattern into R2 */
    CMP     R2, R1
    BNE     Error_CPU          /* if not the same, jump to Error label */
    /* R3 */
    LDR     R3, =0x55555555    /* load pattern into R3 */
    CMP     R3, R0
    BNE     Error_CPU          /* if not the same, jump to Error label */
    LDR     R3, =0xAAAAAAAA    /* load pattern into R3 */
    CMP     R3, R1 // validation_mark FS_CM33_CPU_Register r3 None
    BNE     Error_CPU          /* if not the same, jump to Error label */
    /* R4 */
    LDR     R4, =0x55555555    /* load pattern into R4 */
    CMP     R4, R0
    BNE     Error_CPU          /* if not the same, jump to Error label */
    LDR     R4, =0xAAAAAAAA    /* load pattern into R4 */
    CMP     R4, R1 // validation_mark FS_CM33_CPU_Register r4 None
    BNE     Error_CPU          /* if not the same, jump to Error label */
    /* R5 */
    LDR     R5, =0x55555555    /* load pattern into R5 */
    CMP     R5, R0
    BNE     Error_CPU          /* if not the same, jump to Error label */
    LDR     R5, =0xAAAAAAAA    /* load pattern into R5 */
    CMP     R5, R1 // validation_mark FS_CM33_CPU_Register r5 None
    BNE     Error_CPU          /* if not the same, jump to Error label */
    /* R6 */
    LDR     R6, =0x55555555    /* load pattern into R6 */
    CMP     R6, R0
    BNE     Error_CPU          /* if not the same, jump to Error label */
    LDR     R6, =0xAAAAAAAA    /* load pattern into R6 */
    CMP     R6, R1 // validation_mark FS_CM33_CPU_Register r6 None
    BNE     Error_CPU          /* if not the same, jump to Error label */
    /* R7 */
    LDR     R7, =0x55555555    /* load pattern into R7 */
    CMP     R7, R0
    BNE     Error_CPU          /* if not the same, jump to Error label */
    LDR     R7, =0xAAAAAAAA    /* load pattern into R7 */
    CMP     R7, R1 // validation_mark FS_CM33_CPU_Register r7 None
    BNE     Error_CPU          /* if not the same, jump to Error label */
    /* R12 */
    LDR     R2, =0x55555555    /* load pattern into R2 */
    MOV     R12, R2            /* move pattern into R12 */
    CMP     R12, R0 // validation_mark FS_CM33_CPU_Register r12 None
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

ASM_PUBLIC_END(FS_CM33_CPU_Register)

 ASM_ALIGN(4)
 ASM_END
