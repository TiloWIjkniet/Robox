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
 * @brief Program counter test routines for Cortex-M0+ core - IEC60730 Class B.
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
 ASM_PUBLIC(FS_CM0_PC_Test) 

/*******************************************************************************
 * Implementation
 ******************************************************************************/
/*******************************************************************************
 ******************************************************************************/
ASM_PUBLIC_BEGIN(FS_CM0_PC_Test)
ASM_PUBLIC_FUNC(FS_CM0_PC_Test)
ASM_LABEL(FS_CM0_PC_Test) 

    /* test pattern......R0 */
    /* object function...R1 */
    /* flag address......R2 */

    PUSH  {R4, R5, R6, R7, LR}

    MOV    R4, R0         /* test pattern .......R4 */
    LDRH   R3, [R0]       /* load lower half of data from tested address into R3 */ 
    LDRH   R7, [R0,#2]    /* load upper half of data from tested address into R7 */   
    LSLS   R7, #16        /* shift half-word in R7 to the left */
    ORRS   R3, R7         /* ORR R7 with R3 to backup the data in R3 */
  
    LDR    R5, = 0x467F   /* instruction: MOV  R7, PC */
    STRH   R5, [R0]       /* store in half-address in R0 */
    LDR    R5, = 0x46B7   /* instruction: MOV PC, R6 */
    STRH   R5, [R0,#2]    /* store in half-address in R0 + 2 */
       
    LDR    R0, =FS_FAIL_PC  /* pre-set the result to false */
    MOV    R6, R1           /* address of object function */
    MOVS   R1, #1           /* move 1 into R1 */
    STR    R1, [R2]         /* set the PC_test_flag */
    
    MOV    R5, PC           /* Program counter...R5 */
    ADDS   R5, #0x5         /* offset for LR */
    MOV    LR, R5           /* fill LR with address behind the following routine */
    MOV    PC, R4           /* jump to pattern with command code : MOV  R7, PC    MOV  PC, R6  */
    
    SUBS   R4, #4           /* test pattern.....R4 */
        
    STRH   R3, [R4]         /* restore lower half-word of original data */
    LSRS   R3, #16          /* shift the upper half-word */
    STRH   R3, [R4,#2]      /* restore upper half-word of original data */

    POP   {R4, R5, R6, R7, PC}

ASM_PUBLIC_END(FS_CM0_PC_Test)

 ASM_ALIGN(4)
 ASM_END
