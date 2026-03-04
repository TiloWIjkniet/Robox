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
 * @brief Stack overflow test routines for Cortex-M4/M7 core - IEC60730 Class B.
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
 ASM_PUBLIC(FS_CM4_CM7_STACK_Init)
 ASM_PUBLIC(FS_CM4_CM7_STACK_Test)

/*******************************************************************************
 * Implementation
 ******************************************************************************/
/*******************************************************************************
 ******************************************************************************/
ASM_PUBLIC_BEGIN(FS_CM4_CM7_STACK_Init)
ASM_PUBLIC_FUNC(FS_CM4_CM7_STACK_Init)
ASM_LABEL(FS_CM4_CM7_STACK_Init)

    /* R0............. stack_test_pattern   */
    /* R1..............first_address */
    /* R2..............second_address */
    /* R3..............block_size */

    PUSH {R4, LR}

    SUBS    R4, R1, R3  /* address of block */

ASM_LABEL(SAVE_IN_FRONT_OF)
    STR    R0, [R1]     /* store pattern */
    SUBS   R1, R1, #4   /* decrement address */
    CMP    R1, R4
    BHI    SAVE_IN_FRONT_OF  /* loop to fill area with pattern */

    ADDS   R4, R2, R3   /* address of block */

ASM_LABEL(SAVE_BEHIND)
    STR    R0, [R2]     /* store pattern */
    ADDS   R2, R2, #4   /* increment address */
    CMP    R2, R4
    BLT    SAVE_BEHIND       /* loop to fill area with pattern */

    LDR    R0, =0       /* move 0 to R0 as return value*/
    POP {R4, PC}

ASM_PUBLIC_END(FS_CM4_CM7_STACK_Init)

/*******************************************************************************
 ******************************************************************************/
ASM_PUBLIC_BEGIN(FS_CM4_CM7_STACK_Test)
ASM_PUBLIC_FUNC(FS_CM4_CM7_STACK_Test)
ASM_LABEL(FS_CM4_CM7_STACK_Test)

    /* R0............. stack_test_pattern */
    /* R1..............first_address */
    /* R2..............second_address */
    /* R3..............block_size */

    PUSH {R4, R5, LR}

    SUBS   R4, R1, R3  /* address of block */

ASM_LABEL(LOAD_IN_FRONT_OF)
    LDR    R5, [R1]    /* load value from address in front of the stack area */
    CMP    R0, R5      /* compare that value with known pattern */
    BNE    STACK_ERROR /* if not the same, jump to STACK_ERROR*/
    SUBS   R1, R1, #4  /* decrement address */
    CMP    R1, R4
    BHI    LOAD_IN_FRONT_OF  /* loop to load pattern from the area */

    ADDS   R4, R2, R3  /* address of block */

ASM_LABEL(LOAD_BEHIND)
    LDR    R5, [R2]    /* load value from address behind the stack area */
    CMP    R0, R5      /* compare that value with known pattern */
    BNE    STACK_ERROR /* if not the same, jump to STACK_ERROR*/
    ADDS   R2, R2, #4  /* increment address */
    CMP    R2, R4
    BLT    LOAD_BEHIND       /* loop to load pattern from the area */

    LDR    R0, =FS_PASS      /* if test is OK, save 0 as return value */
    POP {R4, R5, PC}

ASM_LABEL(STACK_ERROR)       /* STACK_ERROR label */
    LDR   R0, =FS_FAIL_STACK       /* save ...FS_FAIL_STACK as return value */
    POP {R4, R5, PC}

ASM_PUBLIC_END(FS_CM4_CM7_STACK_Test)


 ASM_ALIGN(4)
 ASM_END
