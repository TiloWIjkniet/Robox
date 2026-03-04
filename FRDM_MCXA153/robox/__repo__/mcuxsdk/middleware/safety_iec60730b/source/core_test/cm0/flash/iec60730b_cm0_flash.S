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
 * @brief Flash test routines for Cortex-M0+ core - IEC60730 Class B.
 *
 */

#define __ASM__
    #include "iec60730b_core.h"
    #include "asm_mac_common.h"
#undef  __ASM__

ASM_COMP_SPECIFIC_DIRECTIVES
 ASM_CODE_SECTION(.text)

/*
 * Value of the golden CRC must be placed in 
 * another memory section than the calculated CRC!
 */

/*******************************************************************************
 * Functions
 ******************************************************************************/
 ASM_PUBLIC(FS_CM0_FLASH_HW16)
 ASM_PUBLIC(FS_CM0_FLASH_HW16_LPC)
 ASM_PUBLIC(FS_CM0_FLASH_SW16)

/*******************************************************************************
 * Implementation
 ******************************************************************************/
/*******************************************************************************
 ******************************************************************************/
ASM_PUBLIC_BEGIN(FS_CM0_FLASH_HW16) 
ASM_PUBLIC_FUNC(FS_CM0_FLASH_HW16)  
ASM_LABEL(FS_CM0_FLASH_HW16)

    /* start address ............... R0 */
    /* size of the test memory ..... R1 */
    /* crc module address .......... R2 */
    /* start condition seed ........ R3 */

    PUSH  {R4, R5, LR}

    CMP     R1, #0                   /* test if size is no zero */
    BEQ     SIZE_ERROR_HW

    MOV     R4, R2                   /* address of data register ....... R4 */
    ADDS    R2, #8                   /* address of control register .... R2 */

    MOVS    R5, #0
    STR     R5, [R2]                 /* init CRC peripheral */
        
    LDR     R5, =FS_FLASH_CRC_CTRL_WAS_MASK
    STR     R5, [R2]                 /* set WAS bit and reset CRC */
    STR     R3, [R4]                 /* store seed value into data register */

    MOVS    R5, #0
    STR     R5, [R2]                 /* clear WAS bit and begin to calculate CRC */

ASM_LABEL(NEXT_CRC)
    LDRB    R2, [R0]   /* load byte from address in R0 */
    STRB    R2, [R4]   /* store byte into CRC data register */

    ADDS    R0, #1     /* increment address */
    SUBS    R1, #1     /* decrement R1 for numbers of iterations */
    CMP     R1, #0
    BNE     NEXT_CRC

    LDRH    R0, [R4]       /* return value from result CRC register */
    POP {R4,R5, PC}

ASM_LABEL(SIZE_ERROR_HW)   /* Error label */
    MOVS    R0, #0 
    POP {R4,R5, PC}

ASM_PUBLIC_END(FS_CM0_FLASH_HW16)

/*******************************************************************************
 ******************************************************************************/
ASM_PUBLIC_BEGIN(FS_CM0_FLASH_HW16_LPC)    
ASM_PUBLIC_FUNC(FS_CM0_FLASH_HW16_LPC) 
ASM_LABEL(FS_CM0_FLASH_HW16_LPC)

    /* start address ............... R0 */
    /* size of the test memory ..... R1 */
    /* crc module address .......... R2 */  
    /* start condition seed ........ R3 */ 

    PUSH  {R4, R5, LR}  
        
    CMP     R1, #0                   /* test if size is no zero */  
    BEQ     SIZE_ERROR_HW_LPC
    
    MOV     R4, R2                   /* address of mode register ....... R2  BASE*/
    ADDS    R4, #8                   /* address of DATA register .... R4 */    
       
    MOVS    R5, #0x00000000          /* Polynomial = x16 + x12 + x5 + 1
                                        Bit order reverse for data input: NO
                                        1's complement for data input: NO
                                        Bit order reverse for CRC sum: NO
                                        1's complement for CRC sum: NO
                                        CRC_MODE = 0x0000 0000*/       
    STR     R5, [R2]                 /* init CRC peripheral */
    
    MOV     R5, R2                   /*R5 CRC BASE */
    ADDS    R5, #0x4                 /*R5 offset to SEED register (0x4) */
    STR     R3, [R5]                 /* store seed value into seed register */ 
  
ASM_LABEL(NEXT_CRC_LPC)
    LDRB    R2, [R0]   /* load byte from address in R0 */
    STRB    R2, [R4]   /* store byte into CRC data register */

    ADDS    R0, #1     /* increment address */
    SUBS    R1, #1     /* decrement R1 for numbers of iterations */     
    CMP     R1, #0
    BNE     NEXT_CRC_LPC
        
    LDRH    R0, [R4]       /* return value from result CRC register */            
    POP {R4,R5, PC} 

ASM_LABEL(SIZE_ERROR_HW_LPC)   /* Error label */    
    MOVS    R0, #0 
    POP {R4,R5, PC} 
                
ASM_PUBLIC_END(FS_CM0_FLASH_HW16_LPC)

/*******************************************************************************
 ******************************************************************************/
ASM_PUBLIC_BEGIN(FS_CM0_FLASH_SW16)    
ASM_PUBLIC_FUNC(FS_CM0_FLASH_SW16) 
ASM_LABEL(FS_CM0_FLASH_SW16)

    /* start address ............... R0 */
    /* size of the test memory ..... R1 */
    /* empty parameter ............. R2 */  
    /* start condition seed ........ R3 */

    PUSH {R4-R6, LR} 

    ADD    R1, R0    /* calculate end address of tested block */
    MOV    R2, R3    /* move condition seed into R2 */    
        
ASM_LABEL(NEXT_BYTE)     
    LDRB    R3, [R0]      /* load byte from memory */
    LSLS    R3, R3, #8    /* shift to left 8-bit */
    UXTH    R3, R3
    EORS    R2, R3  
    MOVS    R5, #8        /* 8-bit */
    
ASM_LABEL(NEXT_BIT)   
    LDR    R6, =0x8000   /* mask 16-bit of the value */
    ANDS    R6, R2
    CMP    R6, #0
    BEQ    ZERO_BIT   
    
    LSLS    R2, R2, #1   /* if 16-bit is logical 1 go there */      
    LDR    R4, =0x1021   /* load polynom 0x1021 */
    EORS    R4, R2
    UXTH    R2, R4  

    B    NON_ZERO_BIT
    
ASM_LABEL(ZERO_BIT)  /* if 16-bit is logical 0 go there */  
    LSLS    R6, R2, #1
    UXTH    R2, R6

ASM_LABEL(NON_ZERO_BIT)     
    SUBS    R5, #1   /* check bit counter and go to next bit if is not last one */
    CMP     R5, #0
    BNE     NEXT_BIT
    
    ADDS    R0, #1   /* check next byte counter if exceed go to end */
    CMP     R0, R1
    BLT     NEXT_BYTE

    MOV     R0, R2
    POP {R4-R6, PC}

ASM_PUBLIC_END(FS_CM0_FLASH_SW16)


 ASM_ALIGN(4)
 ASM_END
