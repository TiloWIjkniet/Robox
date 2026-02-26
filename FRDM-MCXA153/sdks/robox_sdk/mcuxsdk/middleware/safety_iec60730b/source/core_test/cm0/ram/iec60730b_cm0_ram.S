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
 * @brief RAM test routines for Cortex-M0+ core - IEC60730 Class B.
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
 ASM_PUBLIC(FS_CM0_RAM_AfterReset)
 ASM_PUBLIC(FS_CM0_RAM_Runtime)
 ASM_PUBLIC(FS_CM0_RAM_CopyToBackup)
 ASM_PUBLIC(FS_CM0_RAM_CopyFromBackup)
 ASM_PUBLIC(FS_CM0_RAM_SegmentMarchC)
 ASM_PUBLIC(FS_CM0_RAM_SegmentMarchX)

/*******************************************************************************
 * Implementation
 ******************************************************************************/
/*******************************************************************************
 ******************************************************************************/
ASM_PUBLIC_BEGIN(FS_CM0_RAM_AfterReset)
ASM_PUBLIC_FUNC(FS_CM0_RAM_AfterReset)
ASM_LABEL(FS_CM0_RAM_AfterReset)

    /* R0............. startAddress   */
    /* R1............. endAddress     */
    /* R2............. blockSize      */
    /* R3............. backupAddress  */
    /* [SP, #,0x14]... address of March function (offset after PUSH) */
    
    PUSH {R4-R7, LR}
    
    /* saving registers content on the stack */
    SUB     SP, SP, #0x10
    MOV     R4, R11
    STR     R4, [SP, #0xC]
    MOV     R4, R10
    STR     R4, [SP, #0x8]
    MOV     R4, R9
    STR     R4, [SP, #0x4]
    MOV     R4, R8
    STR     R4, [SP, #0x0]

    MOV   R8, R0           /* startAddress.... R8  */
    MOV   R9, R1           /* endAddress...... R9  */
    MOV   R11, R2          /* blockSize....... R11 */
    MOV   R10, R3          /* backupAddress... R10 */
    LDR   R6, [SP, #0x24]  /* March function address........R6 */ 
       
    /***************************************************************************
    *****  March test of Backup area  ******************************************
    ***************************************************************************/
    MOV   R0, R10     /* backup_start......R0 (march of backup area) */
    MOV   R1, R11     /* blockSize........R1 */
    MOV   R2, PC      /* Program counter...R2 */
    ADDS  R2, #0x5    /* offset for LR */
    MOV   LR, R2      /* fill LR with address behind following march routine */

    MOV   PC, R6      /* jump to March  (startAddress....R0, blockSize.....R1) */
    LDR   R1, =FS_FAIL_RAM
    CMP   R0, R1      /* check if the result is FALSE*/
    BEQ   RAM_AFTER_RESET_END  /* when FALSE, jump to RAM_AFTER_RESET_END */    
    MOV   R7, R8       /* actualAddress.....R7 */
    
    /***************************************************************************
    ******** Testing loop: startAddress ----> endAddress *********************
    ***************************************************************************/
ASM_LABEL(RAM_AFTER_RESET_CYCLE)

    /***************************************************************************
    ******* Copy block to Backup area ******************************************
    ***************************************************************************/
    MOV   R0, R7      /* actualAddress.....R0 */
    MOV   R1, R11     /* blockSize.........R1 */
    MOV   R2, R10     /* backupAddress.....R2 */
    BL    FS_CM0_RAM_CopyToBackup         /* (actualAddress...R0, blockSize...R1, backupAddress....R2) */
    
    /***************************************************************************
    ******* March test of block ************************************************
    ***************************************************************************/    
    MOV   R0, R7      /* actualAddress.....R0 */
    MOV   R1, R11     /* blockSize.........R1 */
    
    MOV   R2, PC      /* Program counter...R2 */
    ADDS  R2, #0x5    /* offset for LR */
    MOV   LR, R2      /* fill LR with address behind following march routine */

    MOV   PC, R6      /* jump to March  (startAddress ......R0, blockSize....R1) */
    MOV   R12, R0     /* move return into R12 */
    
    /***************************************************************************
    ********* Copy from backup to block ****************************************
    ***************************************************************************/
    MOV   R0, R7      /* actualAddress.....R0 */
     MOV   R1, R11    /* blockSize.........R1 */
    MOV   R2, R10     /* backupAddress.....R2 */
    BL    FS_CM0_RAM_CopyFromBackup    /* (actualAddress...R0, blockSize...R1, backupAddress....R2) */
    MOV   R0, R12
    LDR   R1, =FS_FAIL_RAM
    CMP   R0, R1      /* check if the return from march is FALSE*/
    BEQ   RAM_AFTER_RESET_END   /* when FALSE, jump to RAM_AFTER_RESET_END */
    
    /***************************************************************************
    ********* Handling *********************************************************
    ***************************************************************************/ 
    MOV   R3, R11     /* moved because of ADD instruction */
    ADDS  R7, R7, R3  /* updated actualAddress */
    MOV   R4, R9      /* endAddress...................R4 */
    SUBS  R1, R4, R7  /* endAddress - actualAddress..R1 */
       
    CMP   R1, R11        /* compare result with blockSize */
    BHI   STANDARD_SIZE  /* if higher, go to STANDARD_SIZE */
    MOV   R11, R1        /* if block size less than normal, put new value into R6 */

ASM_LABEL(STANDARD_SIZE) /* normal size of block */
    CMP R7, R4                 /* compare actualAddress with end address */
    BNE RAM_AFTER_RESET_CYCLE  /* if not equal, jump to RAM_AFTER_RESET_CYCLE */

    /***************************************************************************
    ****** End *****************************************************************
    ***************************************************************************/
    MOV   R0, R12       /* move return value back to R0 */
    
ASM_LABEL(RAM_AFTER_RESET_END)
    LDR     R1, [SP, #0x0]
    MOV     R8, R1
    LDR     R1, [SP, #0x4]
    MOV     R9, R1
    LDR     R1, [SP, #0x8]
    MOV     R10, R1
    LDR     R1, [SP, #0xC]
    MOV     R11, R1
    ADD     SP, SP, #0x10
    POP {R4-R7, PC}

ASM_PUBLIC_END(FS_CM0_RAM_AfterReset)

/*******************************************************************************
 ******************************************************************************/
ASM_PUBLIC_BEGIN(FS_CM0_RAM_Runtime)
ASM_PUBLIC_FUNC(FS_CM0_RAM_Runtime)
ASM_LABEL(FS_CM0_RAM_Runtime)

    /* R0............. startAddress   */
    /* R1............. endAddress     */
    /* R2............. &actualAddress */
    /* R3............. blockSize      */
    /* [SP, #0x14].... backupAddress (offset after PUSH) */
    /* [SP, #,0x18]... address of March function (offset after PUSH) */

    PUSH {R4-R7, LR}

    /* saving registers content on the stack */
    SUB     SP, SP, #0x10
    MOV     R4, R11
    STR     R4, [SP, #0xC]
    MOV     R4, R10
    STR     R4, [SP, #0x8]
    MOV     R4, R9
    STR     R4, [SP, #0x4]
    MOV     R4, R8
    STR     R4, [SP, #0x0]
    
    MOV   R8, R0          /* startAddress.... R8  */
    MOV   R9, R1          /* endAddress...... R9  */
    MOV   R12, R2         /* &actualAddress.. R12 */
    MOV   R11, R3         /* blockSize...... R11  */
    LDR   R7, [R2]        /* actualAddress....R7  */
    LDR   R1, [SP, #0x24] /* &backupAddress....R1 */
    MOV   R10, R1         /* backupAddress.... R10 */
    LDR   R6, [SP, #0x28] /* March function address........R6 */ 
    
    CMP   R7, R8          /* if it is same as the first address, make March of backup area */
    BNE   RUNTIME_OTHER_BLOCKS    /* if not equal, jump to MY_OWN_OTHER_BLOCKS */

    /***************************************************************************
    *****  March test of Backup area  ******************************************
    ***************************************************************************/
    MOV   R3, R11      /* moved because of ADD instruction */
    ADD   R7, R3       /* increment value of actualAddress....R7  */
    MOV   R1, R12      /* &actualAddress....R1 */
    STR   R7, [R1]     /* store new actualAddress */

    MOV   R0, R10      /* backup_start......R0 (march of backup area) */
    MOV   R1, R11      /* blockSize.........R1 */
  
    MOV   R2, PC       /* Program counter...R2 */
    ADDS  R2, #0x5     /* offset for LR */
    MOV   LR, R2       /* fill LR with address behind following march routine */
    MOV   PC, R6       /* jump to March  (startAddress ......R0, blockSize....R1) */
    B     RUNTIME_END  /* jump to end of the function */
 
    /***************************************************************************
    ******** Testing loop: startAddress ----> endAddress *********************
    ***************************************************************************/

ASM_LABEL(RUNTIME_OTHER_BLOCKS)
    MOV   R3, R11      /* moved because of ADD instruction */
    SUBS   R7, R7, R3  /* actualAddress.....R7 */
    MOV   R4, R9       /* endAddress...................R4 */
    SUBS  R1, R4, R7   /* endAddress - actualAddress..R1 */
    
    CMP   R1, R3       /* compare result with blockSize */ 
    BHI   RUNTIME_STANDARD_SIZE /* if higher, go to STANDARD_SIZE */
    MOV   R11, R1      /* blockSize.........R6 */

ASM_LABEL(RUNTIME_STANDARD_SIZE) /* case of not the last itteration */

    /***************************************************************************
    ******* Copy block to Backup area ******************************************
    ***************************************************************************/
    MOV   R0, R7     /* actualAddress.....R0 */
    MOV   R1, R11    /* blockSize.........R1 */
    MOV   R2, R10    /* backupAddress.....R2 */
    BL    FS_CM0_RAM_CopyToBackup         /* (actualAddress...R0, blockSize...R1, backupAddress....R2) */

    /***************************************************************************
    ******* March test of block ************************************************
    ***************************************************************************/ 
    MOV   R0, R7     /* actualAddress.....R0 */
    MOV   R1, R11    /* blockSize.........R1 */
    
    MOV   R2, PC     /* Program counter...R2 */
    ADDS  R2, #0x5   /* offset for LR */
    MOV   LR, R2     /* fill LR with address behind following march routine */
    MOV   PC, R6     /* jump to March  (startAddress ......R0, blockSize....R1) */
    MOV   R5, R0     /* move return into R5 */
    
    /***************************************************************************
    ********* Copy from backup to block ****************************************
    ***************************************************************************/
    MOV   R0, R7     /* actualAddress.....R0 */
    MOV   R1, R11    /* blockSize.........R1 */
    MOV   R2, R10    /* backupAddress.....R2 */
    BL    FS_CM0_RAM_CopyFromBackup    /* (actualAddress...R0, blockSize...R1, backupAddress....R2) */
    
    /***************************************************************************
    ********* Handling *********************************************************
    ***************************************************************************/ 
    MOV   R1, R12    /* &actualAddress...R1 */
    LDR   R7, [R1]   /* actualAddress....R7 */
    MOV   R3, R11    /* default blocksize........R3 */
    ADDS  R3, R3, R7 /* new actualAddress......R3 */
    STR   R3, [R1]   /* save new actualAddress */
    CMP   R7, R9     /* compare old actual address with endAddress */
    
    BLT  RUNTIME_NOT_LAST_ITTERATION /* if less than, jump to NOT_LAST_ITTERATION */
    MOV  R7, R8      /* if the last itteration was performed, move startAddress into R7*/
    STR  R7, [R1]    /* save startAddress as actual address */

ASM_LABEL(RUNTIME_NOT_LAST_ITTERATION)

    /***************************************************************************
    ****** End *****************************************************************
    ***************************************************************************/
    MOV  R0, R5     /* move return from march into R0 */

ASM_LABEL(RUNTIME_END)
    LDR     R1, [SP, #0x0]
    MOV     R8, R1
    LDR     R1, [SP, #0x4]
    MOV     R9, R1
    LDR     R1, [SP, #0x8]
    MOV     R10, R1
    LDR     R1, [SP, #0xC]
    MOV     R11, R1
    ADD     SP, SP, #0x10
    POP {R4-R7, PC}  /* load registers from stack */

ASM_PUBLIC_END(FS_CM0_RAM_Runtime)

/*******************************************************************************
 ******************************************************************************/
ASM_PUBLIC_BEGIN(FS_CM0_RAM_CopyToBackup)
ASM_PUBLIC_FUNC(FS_CM0_RAM_CopyToBackup)
ASM_LABEL(FS_CM0_RAM_CopyToBackup)

    /* R0....actualAddress */
    /* R1....blockSize */
    /* R2....backupAddress */

    MOV     R3, R0
    ADDS    R3, R3, R1    /* end of block..........R3 */
    MOV     R1, R2        /* backupAddress........R1 */

    /* copy segment to backup_array */
ASM_LABEL(SEGMENT_COPY_TO)
    LDR     R2, [R0]    /* load value from memory */
    STR     R2, [R1]    /* store value to memory */
    ADDS    R0, #4      /* increment segment mem cnt */
    ADDS    R1, #4      /* increment backup mem cnt */
            
    CMP     R3, R0           /* check counters */
    BHI     SEGMENT_COPY_TO  /* copy next long */
                    
    BX      LR

ASM_PUBLIC_END(FS_CM0_RAM_CopyToBackup)

/*******************************************************************************
 ******************************************************************************/
ASM_PUBLIC_BEGIN(FS_CM0_RAM_CopyFromBackup)
ASM_PUBLIC_FUNC(FS_CM0_RAM_CopyFromBackup)
ASM_LABEL(FS_CM0_RAM_CopyFromBackup)

    /* R0....actualAddress */
    /* R1....blockSize */
    /* R2....backupAddress */

    MOV     R3, R0
    ADDS    R3, R3, R1     /* end of block..........R3 */
    MOV     R1, R2         /* backupAddress........R1 */

    /* copy segment to backup_array */
ASM_LABEL(SEGMENT_COPY_FROM)
    LDR     R2, [R1]       /* load value from memory */
    STR     R2, [R0]       /* store value to memory */
    ADDS    R0, #4         /* increment segment mem cnt */
    ADDS    R1, #4         /* increment backup mem cnt */

    CMP     R3, R0            /* check counters */
    BHI     SEGMENT_COPY_FROM /* copy next long */ 

    BX      LR

ASM_PUBLIC_END(FS_CM0_RAM_CopyFromBackup)

/*******************************************************************************
 ******************************************************************************/
ASM_PUBLIC_BEGIN(FS_CM0_RAM_SegmentMarchC)
ASM_PUBLIC_FUNC(FS_CM0_RAM_SegmentMarchC)
ASM_LABEL(FS_CM0_RAM_SegmentMarchC)

    /* startAddress............R0 */
    /* blockSize...............R1 */

    PUSH {R4,R5}

    MOVS    R3, #3
    MOVS    R4, #0

ASM_LABEL(BLOCK_ALLIGNMENT) 
    MOV     R2, R1
    ANDS    R2, R3            /* logical AND to find out if block size is byte alligned */  
    CMP     R2, R4            /* compare with zero value */
    BEQ     INITIALIZATION
    ADDS    R1, #1            /* increment block size if it is not alligned*/
    B       BLOCK_ALLIGNMENT

ASM_LABEL(INITIALIZATION)        
    ADDS    R3, R0, R1        /* end address.......R3 */
    LDR     R2, =0x55555555   /* pattern for March */
    LDR     R4, =0xAAAAAAAA   /* pattern for March */

    /***********************************************
    *  March C - step 1 - clear all block
    ***********************************************/
ASM_LABEL(LOOP_C)
    STR     R2, [R0,#0]  /* write 0x55555555 */
    ADDS    R0, #4       /* increment address */
    CMP     R3, R0
    BHI     LOOP_C

    /***********************************************
    *  March C - step 2 - read 0x55555555 and write 0xAAAAAAAA - rise direction
    ***********************************************/
    SUBS   R0, R3, R1  /* sub the segment value, R1 is the blockSize */

ASM_LABEL(LOOP_C_1)
    LDR    R5, [R0]         /* read 0x55555555 */
    CMP    R5, R2
    BNE    ERROR_C

    STR    R4, [R0,#0]     /* write 0xAAAAAAAA */
    ADDS   R0, #4          /* increment address */
    CMP    R3, R0
    BHI    LOOP_C_1

    /***********************************************
    *  March C - step 3 - read 0xAAAAAAAA and write 0x55555555 - rise direction
    ***********************************************/
    SUBS   R0, R3, R1    /* sub the segment value, R2 is the block size */

ASM_LABEL(LOOP_C_2)
    LDR    R5, [R0]      /* read 0xAAAAAAAA */
    CMP    R5, R4
    BNE    ERROR_C

    STR    R2, [R0,#0]     /* write 0x55555555 */
    ADDS   R0, #4          /* increment address */
    CMP    R3, R0
    BHI    LOOP_C_2

    /***********************************************
    *  March C - step 4 - read 0x55555555 and write 0xAAAAAAAA - descent direction
    ***********************************************/
    SUBS    R3, R0, R1
    SUBS    R3, #4      /* startAddress - 4 .........R3 */
    SUBS    R0, #4      /* endAddress - 4 ...........R0 */

ASM_LABEL(LOOP_C_3)
    LDR     R5, [R0]         /* read 0x55555555 */
    CMP     R5, R2
    BNE     ERROR_C

    STR     R4, [R0,#0]    /* write 0xAAAAAAAA */
    SUBS    R0, #4         /* decrement value */
    CMP     R0, R3
    BHI     LOOP_C_3
    /***********************************************
    *  March C - step 5 - read 0xAAAAAAAA and write 0x55555555 - descent direction
    ***********************************************/
    ADDS    R0, R3, R1     /* endAddress - 4 ...........R0 */

ASM_LABEL(LOOP_C_4)
    LDR     R5, [R0]        /* read 0xAAAAAAAA */
    CMP     R5, R4
    BNE     ERROR_C

    STR     R2, [R0,#0]     /* write 0x55555555 */
    SUBS    R0, #4
    CMP     R0, R3
    BHI     LOOP_C_4
    /***************************************************
    *  March C - step 6 - read 0x55555555 - descent direction
    ****************************************************/
    ADDS    R0, R3, R1    /* endAddress - 4 ...........R0 */

ASM_LABEL(LOOP_C_5)
    LDR     R5, [R0]        /* read 0x55555555 */
    CMP     R5, R2
    BNE     ERROR_C

    SUBS    R0, #4          /* decrement address */
    CMP     R0, R3
    BHI     LOOP_C_5
    MOVS    R0, #FS_PASS    /* SEGMENT OK */
    B       SEGMENT_C_OK

ASM_LABEL(ERROR_C)
    LDR     R0, =FS_FAIL_RAM    /* SEGMENT FAIL */

ASM_LABEL(SEGMENT_C_OK)
    POP {R4,R5}
    BX      LR

ASM_PUBLIC_END(FS_CM0_RAM_SegmentMarchC)

/*******************************************************************************
 ******************************************************************************/
ASM_PUBLIC_BEGIN(FS_CM0_RAM_SegmentMarchX)
ASM_PUBLIC_FUNC(FS_CM0_RAM_SegmentMarchX)
ASM_LABEL(FS_CM0_RAM_SegmentMarchX)

    /* startAddress............R0 */
    /* blockSize...............R1 */

    PUSH {R4,R5}

    /***********************************************
    *  March X - step 1 - clear all block
    ***********************************************/
        
    MOVS    R3, #3
    MOVS    R4, #0

ASM_LABEL(BLOCK_ALLIGNMENT_C) 
    MOV     R2, R1
    ANDS    R2, R3            /* logical AND to find out if block size is byte alligned */  
    CMP     R2, R4            /* compare with zero value */
    BEQ     INITIALIZATION_C
    ADDS    R1, #1            /* increment block size if it is not alligned*/
    B       BLOCK_ALLIGNMENT_C
        
ASM_LABEL(INITIALIZATION_C)        
    ADDS    R3, R0, R1       /* end address.......R3 */        
    LDR     R2, =0x55555555  /* pattern for March */
    LDR     R4, =0xAAAAAAAA  /* pattern for March */

ASM_LABEL(LOOP_X)            /* from startAddress to endAddress write 0x55555555 */
    STR     R2, [R0,#0]  /* write 0x55555555 */
    ADDS    R0, #4       /* increment address */
    CMP     R3, R0       /* compare if the actual address equals the end address */
    BHI     LOOP_X

    /***********************************************
    *  March X - step 2 - read 0x55555555 and write 0xAAAAAAAA - rise direction
    ***********************************************/
    SUBS    R0, R3, R1    /* sub the segment value */

ASM_LABEL(LOOP_X_1)
    LDR     R5, [R0]         /* read 0x55555555 */
    CMP     R5, R2
    BNE     ERROR_X

    STR     R4, [R0,#0]   /* write 0xAAAAAAAA */
    ADDS    R0, #4        /* increment address */
    CMP     R3, R0        /* compare if the actual address equals the end address */
    BHI     LOOP_X_1

    /***********************************************
    *  March X - step 3 - read 0xAAAAAAAA and write 0x55555555 - descent direction
    ***********************************************/
    SUBS    R3, R3, R1
    SUBS    R3, #4      /* startAddress - 4 .........R3 */
    SUBS    R0, #4      /* endAddress - 4 ...........R0 */

ASM_LABEL(LOOP_X_2)
    LDR     R5, [R0]  /* read 0xAAAAAAAA */
    CMP     R5, R4
    BNE     ERROR_X

    STR     R2, [R0,#0]      /* write 0x55555555 */
    SUBS    R0, #4           /* decrement value */
    CMP     R0, R3           /* compare if the actual address equals the start address -4 */
    BHI     LOOP_X_2

    /***********************************************
    *  March X - step 4 - read  5 - descent direction
    ***********************************************/
    ADDS    R0, R3, R1    /* endAddress - 4 ...........R0 */

ASM_LABEL(LOOP_X_3)
    LDR     R5, [R0]      /* read 0x55555555 */
    CMP     R5, R2
    BNE     ERROR_X

    SUBS    R0, #4       /* decrement address */
    CMP     R0, R3       /* compare if the actual address equals the start address -4 */
    BHI     LOOP_X_3

    MOVS    R0, #FS_PASS     /* SEGMENT OK */
    B       SEGMENT_X_OK

ASM_LABEL(ERROR_X)
    LDR     R0, =FS_FAIL_RAM    /* SEGMENT FAIL */

ASM_LABEL(SEGMENT_X_OK)
    POP {R4,R5}
    BX      LR

ASM_PUBLIC_END(FS_CM0_RAM_SegmentMarchX)


 ASM_ALIGN(4)
 ASM_END
