/*
 * Copyright 2025 NXP
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/types.h>

#ifndef _IEC60730B_TEST_H_
#define _IEC60730B_TEST_H_

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*!
 * @name Safety test error codes
 * @brief Error code definitions for IEC 60730 Class B safety test failures
 */
#define IEC60730B_TEST_OK                       (0)      /* No error */
#define IEC60730B_TEST_ERROR                    (-1)     /* General error */
#define IEC60730B_TEST_CPU_REG_ERROR            (-2)     /* CPU registers test fault */
#define IEC60730B_TEST_CPU_REG_PRIMASK_ERROR    (-3)     /* PRIMASK test fault */
#define IEC60730B_TEST_CPU_REG_NONSTACKED_ERROR (-4)     /* Non-stacked CPU test fault */
#define IEC60730B_TEST_CPU_REG_FLOAT_ERROR      (-5)     /* Floating point registers test fault */
#define IEC60730B_TEST_CPU_REG_CONTROL_ERROR    (-6)     /* CONTROL register test fault */
#define IEC60730B_TEST_CPU_REG_SPECIAL_ERROR    (-7)     /* Special CPU registers test fault */
#define IEC60730B_TEST_WDT_ERROR                (-8)     /* Watchdog error */
#define IEC60730B_TEST_FLASH_ERROR              (-9)     /* Flash test fault */
#define IEC60730B_TEST_RAM_ERROR                (-10)    /* RAM test fault */
#define IEC60730B_TEST_STACK_ERROR              (-11)    /* Stack test fault */
#define IEC60730B_TEST_CLOCK_ERROR              (-12)    /* Clock test fault */
#define IEC60730B_TEST_PC_ERROR                 (-13)    /* Program counter test fault */

#ifdef CONFIG_IEC60730B_TEST_RAM
/*!
 * @brief RAM test algorithm types for IEC 60730 Class B compliance
 * 
 * This enumeration defines the available RAM test algorithms that can be
 * used to detect memory faults as required by IEC 60730 Class B safety
 * standards.
 */
typedef enum {
    IEC60730B_TEST_RAM_TYPE_NONE = 0,    /*!< No RAM test algorithm */
    /**
     * March-C algorithm for RAM testing
     * 
     * Complexity: O(10n) - 10 operations per memory cell
     * 
     * Fault coverage:
     * - Stuck-at faults (SAF): Detects bits permanently stuck at 0 or 1
     * - Address decoder faults (AF): Detects incorrect address decoding
     * - Transition faults (TF): Detects failures when changing bit values
     * - Coupling faults (CF): Detects when one bit affects another
     * - Bridging faults (BF): Detects short circuits between memory lines
     * 
     * Recommended for IEC 60730 Class B due to comprehensive fault coverage
     */
    IEC60730B_TEST_RAM_TYPE_MARCH_C,
    /**
     * March-X algorithm for RAM testing
     * 
     * Complexity: O(6n) - 6 operations per memory cell  
     * 
     * Fault coverage:
     * - Stuck-at faults (SAF): Detects bits permanently stuck at 0 or 1
     * - Address decoder faults (AF): Detects incorrect address decoding
     * - Transition faults (TF): Detects failures when changing bit values
     * - Limited coupling faults (CF): Basic coupling fault detection
     * 
     * Faster execution than March-C but with reduced fault coverage.
     * Suitable when execution time is critical and basic fault detection
     * is sufficient for the safety requirements.
     */
    IEC60730B_TEST_RAM_TYPE_MARCH_X
} iec60730b_test_ram_type_t;
#endif

#ifdef CONFIG_IEC60730B_TEST_FLASH
/*!
 * @brief Flash CRC data type for IEC 60730 Class B compliance
 * 
 * This type definition provides the appropriate CRC data type for flash
 * memory integrity testing. 
 */
#ifdef CONFIG_IEC60730B_TEST_FLASH_CRC32
typedef uint32_t iec60730b_flash_crc_t;
#elif CONFIG_IEC60730B_TEST_FLASH_CRC16
typedef uint16_t iec60730b_flash_crc_t;
#else
#error "Invalid Flash CRC configuration."
#endif
#endif /* CONFIG_IEC60730B_TEST_FLASH */

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * API
 ******************************************************************************/
#ifdef CONFIG_IEC60730B_TEST_CPU_REG
/*!
 * @brief Test CPU registers for IEC 60730 Class B compliance
 * 
 * This function performs a comprehensive test of CPU registers to detect
 * stuck-at faults and ensure proper register functionality as required
 * by IEC 60730 Class B.
 * 
 * @return 0 on success, negative on failure
 */
int iec60730b_test_cpu_reg(void);
#endif

#ifdef CONFIG_IEC60730B_TEST_RAM
/*!
 * @brief Test RAM memory for IEC 60730 Class B compliance
 * 
 * This function performs comprehensive RAM testing using the specified algorithm
 * to detect memory faults such as stuck-at bits, coupling faults, and addressing
 * faults as required by IEC 60730 Class B.
 * 
 * @param ram Pointer to the RAM memory area to be tested
 * @param ram_size Size of the RAM memory area in bytes
 * @param backup Pointer to backup memory area for data preservation
 * @param backup_size Size of the backup memory area in bytes
 * @param type RAM test algorithm type to be used
 * 
 * @return 0 on success, negative on failure
 */
int iec60730b_test_ram(uint8_t *ram, size_t ram_size, uint8_t *backup, size_t backup_size, iec60730b_test_ram_type_t type);
#endif

#ifdef CONFIG_IEC60730B_TEST_PC
/*!
 * @brief Test Program Counter for IEC 60730 Class B compliance
 * 
 * This function performs Program Counter testing to detect execution flow
 * corruption and ensure proper program execution as required by IEC 60730
 * Class B. The test validates that the processor correctly
 * handles jumps, calls, and returns.
 * 
 * @return 0 on success, negative error code on failure
 */
int iec60730b_test_pc(void);
#endif

#ifdef CONFIG_IEC60730B_TEST_STACK
/*!
 * @brief Initialize stack testing for IEC 60730 Class B compliance
 * 
 * This function initializes the stack testing mechanism by setting up
 * guard patterns and boundaries to detect stack overflow and underflow
 * conditions as required by IEC 60730 Class B.
 * 
 * @param stack_start Starting address of the stack memory area
 * @param stack_size Size of the stack memory area in bytes
 * @param guard_size Size of the guard area in bytes. The guard zones provide additional protection
 *                   by reserving memory before and after the actual thread stack.
 * @param guard_pattern Pattern value used for guard area initialization
 * 
 * @return 0 on success, negative on failure
 */
int iec60730b_test_stack_init(void *stack_start, size_t stack_size, size_t guard_size, uint32_t guard_pattern);

/*!
 * @brief Test stack integrity for IEC 60730 Class B compliance
 * 
 * This function performs stack integrity testing by checking guard patterns
 * and boundaries to detect stack overflow and underflow conditions as
 * required by IEC 60730 Class B.
 * 
 * @param stack_start Starting address of the stack memory area
 * @param stack_size Size of the stack memory area in bytes
 * @param guard_size Size of the guard area in bytes. The guard zones provide additional protection
 *                   by reserving memory before and after the actual thread stack.
 * @param guard_pattern Pattern value used for guard area verification
 * 
 * @return 0 on success, negative on failure
 */
int iec60730b_test_stack(void *stack_start, size_t stack_size, size_t guard_size, uint32_t guard_pattern);
#endif

#ifdef CONFIG_IEC60730B_TEST_FLASH
/*!
 * @brief Test flash memory using CRC for IEC 60730 Class B compliance
 * 
 * This function performs flash memory integrity testing using CRC checksum
 * calculation to detect memory corruption and ensure data integrity as
 * required by IEC 60730 Class B. The test calculates a CRC checksum over
 * the specified memory region and compares it against the expected value.
 * 
 * @param start Starting address of the flash memory area to test
 * @param size Size of the flash memory area in bytes
 * @param crc_expected Expected CRC checksum value for comparison
 * 
 * @return 0 on success, negative on failure
 */
int iec60730b_test_flash_crc(const void *start, size_t size, iec60730b_flash_crc_t crc_expected);
#endif

#ifdef __cplusplus
}
#endif

#endif /* _IEC60730B_TEST_H_ */
