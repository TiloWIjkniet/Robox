/*
 * Copyright 2025 NXP
 * SPDX-License-Identifier: Apache-2.0
 */

#include "iec60730b_test.h"
#include <iec60730b.h>
#include <iec60730b_core.h>

#ifdef CONFIG_IEC60730B_TEST_CPU_REG
/*
 * Test CPU registers for IEC 60730 Class B compliance.
 */
int iec60730b_test_cpu_reg(void)
{
#ifdef CONFIG_IEC60730B_TEST_CPU_REG_DSP
    /* stacked CPU registers */
    if (FS_CM33_CPU_Register() == FS_FAIL_CPU_REGISTER){
        return IEC60730B_TEST_CPU_REG_ERROR;
    }
#else   
    /* stacked CPU registers */
    if (FS_CM33_CPU_Register_NDSP() == FS_FAIL_CPU_REGISTER){
        return IEC60730B_TEST_CPU_REG_ERROR;
    }
#endif

    /* non-stacked CPU registers */
    if (FS_CM33_CPU_NonStackedRegister() == FS_FAIL_CPU_NONSTACKED_REGISTER){
        return IEC60730B_TEST_CPU_REG_NONSTACKED_ERROR;
    }
    
    /* PRIMASK Secure */
    if (FS_CM33_CPU_Primask_S() == FS_FAIL_CPU_PRIMASK){
        return IEC60730B_TEST_CPU_REG_PRIMASK_ERROR;
    }
    
#ifdef CONFIG_IEC60730B_TEST_CPU_REG_TZ
    /* PRIMASK Non-Secure */
    if (FS_CM33_CPU_Primask_NS() == FS_FAIL_CPU_PRIMASK){
        return IEC60730B_TEST_CPU_REG_PRIMASK_ERROR;
    }
#endif
 
    /* SP main Secure */
    FS_CM33_CPU_SPmain_S();
    
#ifdef CONFIG_IEC60730B_TEST_CPU_REG_TZ
    /* SP main Non-Secure */
    FS_CM33_CPU_SPmain_NS();
#endif

    /* SP main limit Secure */
    FS_CM33_CPU_SPmain_Limit_S();
    
#ifdef CONFIG_IEC60730B_TEST_CPU_REG_TZ
    /* SP main limit Non-Secure */
    FS_CM33_CPU_SPmain_Limit_NS();
#endif

    /* SP process Secure */
    FS_CM33_CPU_SPprocess_S();
    
#ifdef CONFIG_IEC60730B_TEST_CPU_REG_TZ
    /* SP process Non-Secure */
    FS_CM33_CPU_SPprocess_NS();
#endif

    /* SP process limit Secure */
    FS_CM33_CPU_SPprocess_Limit_S();
    
#ifdef CONFIG_IEC60730B_TEST_CPU_REG_TZ
    /* SP process limit Non-Secure */
    FS_CM33_CPU_SPprocess_Limit_NS();
#endif

    if (FS_CM33_CPU_Control_S() == FS_FAIL_CPU_CONTROL){
        return IEC60730B_TEST_CPU_REG_CONTROL_ERROR;
    }
    
#ifdef CONFIG_IEC60730B_TEST_CPU_REG_TZ
    /* CONTROL Non-Secure */
    if (FS_CM33_CPU_Control_NS() == FS_FAIL_CPU_CONTROL){
        return IEC60730B_TEST_CPU_REG_CONTROL_ERROR;
    }
#endif

    /* Special Secure */
    if (FS_CM33_CPU_Special8PriorityLevels_S() == FS_FAIL_CPU_SPECIAL){
        return IEC60730B_TEST_CPU_REG_SPECIAL_ERROR;
    }
    
#ifdef CONFIG_IEC60730B_TEST_CPU_REG_TZ
    /* Special Non-Secure */
    if (FS_CM33_CPU_Special8PriorityLevels_NS() == FS_FAIL_CPU_SPECIAL){
        return IEC60730B_TEST_CPU_REG_SPECIAL_ERROR;
    }
#endif

#ifdef CONFIG_IEC60730B_TEST_CPU_REG_FPU
    if (FS_CM33_CPU_Float1() == FS_FAIL_CPU_FLOAT_1){
        return IEC60730B_TEST_CPU_REG_FLOAT_ERROR;
    }

    if (FS_CM33_CPU_Float2() == FS_FAIL_CPU_FLOAT_2){
        return IEC60730B_TEST_CPU_REG_FLOAT_ERROR;
    }
#endif

    return IEC60730B_TEST_OK;
}
#endif /* CONFIG_IEC60730B_TEST_CPU_REG */

#ifdef CONFIG_IEC60730B_TEST_RAM
/*
 * Test RAM memory using specified algorithm for IEC 60730 Class B compliance.
 */
int iec60730b_test_ram(uint8_t *ram, size_t ram_size, uint8_t *backup, size_t backup_size, iec60730b_test_ram_type_t type)
{
    tFcn march_type;

    switch (type) {
        case IEC60730B_TEST_RAM_TYPE_MARCH_C:
            march_type = FS_CM33_RAM_SegmentMarchC;
            break;
        case IEC60730B_TEST_RAM_TYPE_MARCH_X:
            march_type = FS_CM33_RAM_SegmentMarchX;
            break;
        default:
            return IEC60730B_TEST_ERROR; /* Not supported */
    }

    if (FS_CM33_RAM_AfterReset((uint32_t)ram, (uint32_t)(ram + ram_size), (uint32_t)backup_size, (uint32_t)backup, march_type) == FS_FAIL_RAM){
        return IEC60730B_TEST_RAM_ERROR;
    }

    return IEC60730B_TEST_OK;
}
#endif /* CONFIG_IEC60730B_TEST_RAM */

#ifdef CONFIG_IEC60730B_TEST_PC
/*
 * Test Program Counter for IEC 60730 Class B compliance
 */
int iec60730b_test_pc(void)
{
    static uint32_t pc_test_in_ram;
    uint32_t pc_test_flag = 0;
    int result;

#ifdef CONFIG_MPU
    /* Temporarily disable memory protection 
     * to allow execution/modification of RAM pattern address */
    extern void arm_core_mpu_disable(void);
    arm_core_mpu_disable();
#endif

    if (FS_CM33_PC_Test((uint32_t)&pc_test_in_ram, FS_PC_Object, &pc_test_flag) == FS_FAIL_PC) {
        result = IEC60730B_TEST_PC_ERROR;
    } else {
        result = IEC60730B_TEST_OK;
    }

#ifdef CONFIG_MPU
    /* Re-enable memory protection */
    extern void arm_core_mpu_enable(void);
    arm_core_mpu_enable();
#endif

    return result;
}
#endif /* CONFIG_IEC60730B_TEST_PC */

#ifdef CONFIG_IEC60730B_TEST_STACK
/*
 * Initialize stack testing for IEC 60730 Class B compliance
 */
int iec60730b_test_stack_init(void *stack_start, size_t stack_size, size_t guard_size, uint32_t guard_pattern)
{
    FS_CM33_STACK_Init(guard_pattern, (uint32_t)stack_start - sizeof(guard_pattern), (uint32_t)stack_start + stack_size, guard_size);

    return IEC60730B_TEST_OK;
}

/*
 * Test stack integrity for IEC 60730 Class B compliance
 */
int iec60730b_test_stack(void *stack_start, size_t stack_size, size_t guard_size, uint32_t guard_pattern)
{
    if(FS_CM33_STACK_Test(guard_pattern, (uint32_t)stack_start - sizeof(guard_pattern), (uint32_t)stack_start + stack_size, guard_size) == FS_FAIL_STACK) {
        return IEC60730B_TEST_STACK_ERROR;
    }

    return IEC60730B_TEST_OK;
}
#endif /* CONFIG_IEC60730B_TEST_STACK */

#ifdef CONFIG_IEC60730B_TEST_FLASH
/*
 * Test flash memory integrity using CRC32 for IEC 60730 Class B compliance
 */
int iec60730b_test_flash_crc(const void *start, size_t size, iec60730b_flash_crc_t crc_expected)
{
    iec60730b_flash_crc_t crc_result;

#ifdef CONFIG_IEC60730B_TEST_FLASH_CRC32
    crc_result = FS_CM33_FLASH_SW32((uint32_t) start, size, 0, (iec60730b_flash_crc_t)CONFIG_IEC60730B_TEST_FLASH_CRC_SEED);
#elif CONFIG_IEC60730B_TEST_FLASH_CRC16
    crc_result = FS_CM33_FLASH_SW16((uint32_t) start, size, 0, (iec60730b_flash_crc_t)CONFIG_IEC60730B_TEST_FLASH_CRC_SEED);
#else
    #error Not supported CONFIG_IEC60730B_TEST_FLASH_xxx
#endif
    if(crc_result != crc_expected ) {
        return IEC60730B_TEST_FLASH_ERROR;
    }

    return IEC60730B_TEST_OK;
}
#endif /* CONFIG_IEC60730B_TEST_FLASH */
