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
 * @brief CPU registers test routines for Cortex-M0+ core - IEC60730 Class B.
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
 ASM_PUBLIC(FS_CM0_CPU_Register)
 ASM_PUBLIC(FS_CM0_CPU_NonStackedRegister)
 ASM_PUBLIC(FS_CM0_CPU_Primask)
 ASM_PUBLIC(FS_CM0_CPU_SPmain)
 ASM_PUBLIC(FS_CM0_CPU_SPprocess)
 ASM_PUBLIC(FS_CM0_CPU_Control)

/*******************************************************************************
 * Implementation
 ******************************************************************************/
/*******************************************************************************
 ******************************************************************************/
ASM_PUBLIC_BEGIN(FS_CM0_CPU_Register)  
ASM_PUBLIC_FUNC(FS_CM0_CPU_Register)   
ASM_LABEL(FS_CM0_CPU_Register)

    PUSH {R4-R7, LR}

    /* R0, R1 */    
    LDR     R0, =0x55555555    /* load pattern into R0 */
    LDR     R1, =0x55555555    /* load pattern into R1 */   
    CMP     R0, R1          
    BNE     Endless_loop       /* if not the same, jump to endless loop */

    LDR     R0, =0xAAAAAAAA    /* load pattern into R0 */
    LDR     R1, =0xAAAAAAAA    /* load pattern into R1 */
    CMP     R0, R1 // validation_mark FS_CPU_Register r0 Endless_loop
    BNE     Endless_loop   /* if not the same, jump to endless loop */    
    /* APSR */
    LDR     R0, =0x50000000       /* N = 0, Z = 1, C = 0, V = 1 */
    MSR     APSR_nzcvq, R0        /* move pattern into APSR */
    BMI     Endless_loop          /* if N corrupted */
    BNE     Endless_loop          /* if Z corrupted */
    BCS     Endless_loop          /* if C corrupted */
    BVC     Endless_loop          /* if V corrupted */

    LDR     R0, =0xA0000000    /* N = 1, Z = 0, C = 1, V = 0 */
    MSR     APSR_nzcvq, R0        /* move pattern into APSR */
    BPL     Endless_loop          /* if N corrupted */
    BEQ     Endless_loop          /* if Z corrupted */ 
    BCC     Endless_loop          /* if C corrupted */
    BVS     Endless_loop          /* if V corrupted */
    /*  R14 (LR) */
    LDR     R1, =0x55555555    /* load pattern into R1 */
    LDR     R0, =0x55555555    /* load pattern into R0 */
    MOV     LR, R0             /* move pattern into LR*/
    CMP     LR, R1 // validation_mark FS_CPU_Register r1 Endless_loop
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
    CMP     R2, R0  
    BNE     Error_CPU          /* if not the same, jump to Error label */
    LDR     R2, =0xAAAAAAAA    /* load pattern into R2 */
    CMP     R2, R1 // validation_mark FS_CPU_Register r2 None
    BNE     Error_CPU          /* if not the same, jump to Error label */
    /* R3 */       
    LDR     R3, =0x55555555    /* load pattern into R3 */
    CMP     R3, R0  
    BNE     Error_CPU          /* if not the same, jump to Error label */
    LDR     R3, =0xAAAAAAAA    /* load pattern into R3 */
    CMP     R3, R1 // validation_mark FS_CPU_Register r3 None
    BNE     Error_CPU          /* if not the same, jump to Error label */
    /* R4 */        
    LDR     R4, =0x55555555    /* load pattern into R4 */
    CMP     R4, R0   
    BNE     Error_CPU          /* if not the same, jump to Error label */       
    LDR     R4, =0xAAAAAAAA    /* load pattern into R4 */
    CMP     R4, R1 // validation_mark FS_CPU_Register r4 None
    BNE     Error_CPU          /* if not the same, jump to Error label */
    /* R5 */        
    LDR     R5, =0x55555555    /* load pattern into R5 */
    CMP     R5, R0   
    BNE     Error_CPU          /* if not the same, jump to Error label */       
    LDR     R5, =0xAAAAAAAA    /* load pattern into R5 */
    CMP     R5, R1 // validation_mark FS_CPU_Register r5 None
    BNE     Error_CPU          /* if not the same, jump to Error label */
    /* R6 */        
    LDR     R6, =0x55555555    /* load pattern into R6 */
    CMP     R6, R0 // validation_mark FS_CPU_Register r6 None
    BNE     Error_CPU          /* if not the same, jump to Error label */
    LDR     R6, =0xAAAAAAAA    /* load pattern into R6 */
    CMP     R6, R1  
    BNE     Error_CPU          /* if not the same, jump to Error label */
    /* R7 */        
    LDR     R7, =0x55555555    /* load pattern into R7 */
    CMP     R7, R0 // validation_mark FS_CPU_Register r7 None
    BNE     Error_CPU          /* if not the same, jump to Error label */
    LDR     R7, =0xAAAAAAAA    /* load pattern into R7 */
    CMP     R7, R1  
    BNE     Error_CPU          /* if not the same, jump to Error label */   
    /* R12 */
    LDR     R2, =0x55555555    /* load pattern into R2 */
    MOV     R12, R2            /* move pattern into R12 */
    CMP     R12, R0 // validation_mark FS_CPU_Register r12 None
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

ASM_PUBLIC_END(FS_CM0_CPU_Register)

/*******************************************************************************
 ******************************************************************************/
ASM_PUBLIC_BEGIN(FS_CM0_CPU_NonStackedRegister)
ASM_PUBLIC_FUNC(FS_CM0_CPU_NonStackedRegister)
ASM_LABEL(FS_CM0_CPU_NonStackedRegister)
 
    PUSH {LR}

    /* saving registers content on the stack */
    SUB     SP, SP, #0x10
    MOV     R0, R11
    STR     R0, [SP, #0xC]
    MOV     R0, R10
    STR     R0, [SP, #0x8]
    MOV     R0, R9
    STR     R0, [SP, #0x4]
    MOV     R0, R8
    STR     R0, [SP, #0x0]
    LDR     R0, =0x55555555   /* load pattern into R0 */
    LDR     R1, =0xAAAAAAAA   /* load pattern into R1 */    
    /* R8 */       
    LDR     R2, =0x55555555   /* load pattern into R2 */
    MOV     R8, R2            /* move pattern into R8 */
    CMP     R8, R0 // validation_mark FS_CPU_NonStackedRegister r8 None
    BNE     ErrorNonStacked   /* if not the same, jump to ErrorNonStacked label */
    MOV     R8, R1            /* move pattern into R8 */
    CMP     R8, R1 
    BNE     ErrorNonStacked   /* if not the same, jump to ErrorNonStacked label */  
    /* R9 */    
    LDR     R2, =0x55555555   /* load pattern into R2 */  
    MOV     R9, R2            /* move pattern into R9 */
    CMP     R9, R0 // validation_mark FS_CPU_NonStackedRegister r9 None
    BNE     ErrorNonStacked   /* if not the same, jump to ErrorNonStacked label */
    MOV     R9, R1            /* move pattern into R9 */
    CMP     R9, R1 
    BNE     ErrorNonStacked   /* if not the same, jump to ErrorNonStacked label */  
    /* R10 */
    LDR     R2, =0x55555555   /* load pattern into R2 */ 
    MOV     R10, R2           /* move pattern into R10 */
    CMP     R10, R0 // validation_mark FS_CPU_NonStackedRegister r10 None
    BNE     ErrorNonStacked   /* if not the same, jump to ErrorNonStacked label */
    MOV     R10, R1           /* move pattern into R10 */
    CMP     R10, R1  
    BNE     ErrorNonStacked   /* if not the same, jump to ErrorNonStacked label */  
    /* R11 */
    LDR     R2, =0x55555555   /* load pattern into R2 */
    MOV     R11, R2           /* move pattern into R11 */
    CMP     R11, R0 // validation_mark FS_CPU_NonStackedRegister r11 None
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
    LDR     R1, [SP, #0x0]
    MOV     R8, R1
    LDR     R1, [SP, #0x4]
    MOV     R9, R1
    LDR     R1, [SP, #0x8]
    MOV     R10, R1
    LDR     R1, [SP, #0xC]
    MOV     R11, R1
    ADD     SP, SP, #0x10
    POP    {PC}
    ASM_LITERAL

ASM_PUBLIC_END(FS_CM0_CPU_NonStackedRegister)

/*******************************************************************************
 ******************************************************************************/
ASM_PUBLIC_BEGIN(FS_CM0_CPU_Primask)
ASM_PUBLIC_FUNC(FS_CM0_CPU_Primask)
ASM_LABEL(FS_CM0_CPU_Primask)

    MRS     R1, PRIMASK     /* backup of PRIMASK */              
    LDR     R2, =0x00000001         /* load pattern into R2 */ 
    MSR     PRIMASK, R2     /* move pattern into PRIMASK */         
    MRS     R3, PRIMASK     /* move pattern from PRIMASK to R3 */   
    CMP     R3, #0x00000001                                                                         // validation_mark FS_CPU_Primask r3 None
    BNE     ErrorPRIMASK            /* if not the same, jump to ErrorPRIMASK label */

    LDR     R3, =0x00000000         /* load pattern into R3 */ 
    MSR     PRIMASK, R3     /* move pattern into PRIMASK */
    MRS     R2, PRIMASK     /* move pattern from PRIMASK to R2 */                
    MSR     PRIMASK, R1     /* restore PRIMASK */                   
    CMP     R3, R2                  
    BNE     ErrorPRIMASK            /* if not the same, jump to ErrorPRIMASK label */       

    MOVS    R0, #FS_PASS  /* move PASS return into R0 */
    B       ContinuePRIMASK             /* jump over the Error label */

ASM_LABEL(ErrorPRIMASK)                 /* Error label */
    LDR    R0, =FS_FAIL_CPU_PRIMASK  /* move FAIL return into R0 */

ASM_LABEL(ContinuePRIMASK)
    BX     LR

ASM_PUBLIC_END(FS_CM0_CPU_Primask)

/*******************************************************************************
 ******************************************************************************/
ASM_PUBLIC_BEGIN(FS_CM0_CPU_SPmain)
ASM_PUBLIC_FUNC(FS_CM0_CPU_SPmain)
ASM_LABEL(FS_CM0_CPU_SPmain)

    PUSH    {R4,R5, LR}

    MOV     R3, SP               /* backup of SP */
    MRS     R4, CONTROL          /* Backup of CONTROL */
    MOVS    R1, #0x00000000      /* pattern for SP_main */
    MSR     CONTROL, R1          /* move pattern into CONTROL */
    MOV     R5, SP               /* backup SP for case of different state of CONTROL */

    LDR     R1, =0x55555554      /* load pattern into R1 */
    LDR     R2, =0x55555554      /* load pattern into R2 */
    MOV     SP, R1               /* move pattern into SP */
    MOV     R2, SP               /* move SP into R2 */
    CMP     R1, R2               /* compare R1 with R2 */ // validation_mark FS_CPU_SPmain r1 Error_SP_main
    BNE     Error_SP_main        /* if not the same, jump to ErrorSP_main label */

    LDR     R1, =0xAAAAAAA8      /* load pattern into R1 */
    MOV     SP, R1               /* move pattern into SP */
    MOV     R2, SP               /* move SP into R2 */
    CMP     R1, R2               /* compare R1 with R2 */
    BNE     Error_SP_main        /* if not the same, jump to ErrorSP_main label */

    B       Continue_SP_main     /* jump over the ErrorSP_main label */

ASM_LABEL(Error_SP_main)         /* Endless loop */
    CPSID   i                    /* Disable interrupts */
    MSR     CONTROL, R4          /* load original value into CONTROL */
    B       Error_SP_main

ASM_LABEL(Continue_SP_main)    
    MOV     SP, R5                      /* restore SP before restoring CONTROL */
    MSR     CONTROL, R4                 /* load original value into CONTROL */
    MOV     SP, R3              /* restore SP */ 
    MOVS    R0, #FS_PASS  /* move PASS return into R0 */
    POP     {R4,R5, PC}                 

ASM_PUBLIC_END(FS_CM0_CPU_SPmain)

/*******************************************************************************
 ******************************************************************************/
ASM_PUBLIC_BEGIN(FS_CM0_CPU_SPprocess)
ASM_PUBLIC_FUNC(FS_CM0_CPU_SPprocess)
ASM_LABEL(FS_CM0_CPU_SPprocess)

        PUSH    {R4,R5, LR}

        MOV     R3, SP               /* backup of SP */
        MRS     R4, CONTROL          /* Backup of CONTROL */
        MOVS    R1, #0x00000002      /* pattern for SP_process */
        MSR     CONTROL, R1          /* move pattern into CONTROL */
        MOV     R5, SP               /* backup SP for case of different state of CONTROL */
        
        LDR     R1, =0x55555554      /* load pattern into R1 */
        LDR     R2, =0x55555554      /* load pattern into R2 */       
        MOV     SP, R1               /* move pattern into SP */ 
        MOV     R2, SP               /* move SP into R2 */
        CMP     R1, R2               /* compare R1 with R2 */
        BNE     Error_SP_process     /* if not the same, jump to ErrorSP_main label */
               
        LDR     R1, =0xAAAAAAA8      /* load pattern into R1 */           
        MOV     SP, R1               /* move pattern into SP */     
        CMP     SP, R1 // validation_mark FS_CPU_SPprocess r1 Error_SP_process
        BNE     Error_SP_process     /* if not the same, jump to ErrorSP_main label */
         
        B       Continue_SP_process  /* jump over the ErrorSP_main label */
        
ASM_LABEL(Error_SP_process)          /* Endless loop */
        CPSID   i                    /* Disable interrupts */
        MSR     CONTROL, R4          /* load original value into CONTROL */
        B       Error_SP_process
        
ASM_LABEL(Continue_SP_process)   
        MOV     SP, R5                      /* restore SP before restoring CONTROL */
        MSR     CONTROL, R4                 /* load original value into CONTROL */
        MOV     SP, R3                      /* restore SP */ 
        MOVS    R0, #FS_PASS  /* move PASS return into R0 */
        POP     {R4,R5, PC}

ASM_PUBLIC_END(FS_CM0_CPU_SPprocess)

/*******************************************************************************
 ******************************************************************************/
ASM_PUBLIC_BEGIN(FS_CM0_CPU_Control)
ASM_PUBLIC_FUNC(FS_CM0_CPU_Control)
ASM_LABEL(FS_CM0_CPU_Control)

    MOV     R3, SP             /* store value of SP */
    MRS     R2, CONTROL        /* Backup of CONTROL in R0 */
    MOVS    R0, #0x00000002    /* pattern for SP_process */
    MSR     CONTROL, R0        /* move pattern into CONTROL */
    MRS     R1, CONTROL        /* move pattern from CONTROL */

    CMP     R1, #0x00000002 // validation_mark FS_CPU_Control r1 None
    BNE     Error_Control      /* if not the same, jump to ErrorCONTROL label */

    MOVS    R0, #0x00000000    /* pattern for SP_main */      
    MSR     CONTROL, R0        /* move pattern into CONTROL */
    MRS     R1, CONTROL        /* move pattern from CONTROL */

    CMP     R1, #00000000
    BNE     Error_Control      /* if not the same, jump to ErrorCONTROL label */ 

    MSR     CONTROL, R2        /* restore CONTROL */
    MOVS    R0, #FS_PASS          /* move PASS return into R0 */
    B       Continue_Control   /* jump over the ErrorCONTROL label */

ASM_LABEL(Error_Control)       /* ErrorCONTROL label */
    LDR     R0, =FS_FAIL_CPU_CONTROL  /* move the FAIL return into R0 */
    MOV     SP, R3             /* in case of error state, with CONTROL changed, to use original Stack */
        
ASM_LABEL(Continue_Control)        
    BX      LR

ASM_PUBLIC_END(FS_CM0_CPU_Control)


 ASM_ALIGN(4)
 ASM_END
