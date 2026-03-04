/*
 * Copyright 2025 NXP
 * SPDX-License-Identifier: Apache-2.0
 */

#include <iec60730b_test.h>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/device.h>
#include <zephyr/drivers/watchdog.h>
#include <zephyr/task_wdt/task_wdt.h>
#include <zephyr/sys/reboot.h>

/*******************************************************************************
* Declarations
******************************************************************************/
static int safety_init(void);
static void safety_thread(void *arg1, void *arg2, void *arg3);
static void safety_error_handling(int error_code);
static void safety_startup_tests(void);
static void safety_rutime_tests(void);

/* Register safety_tests module for logging */
LOG_MODULE_REGISTER(safety, CONFIG_APP_SAFETY_LOG_LEVEL);

/* System initialization hook for safety tests after reset. */
SYS_INIT(safety_init, POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT);

/*
 * Safety thread stack and creation configuration.
 * 
 * When CONFIG_IEC60730B_TEST_STACK is enabled, creates a custom stack with guard zones
 * for stack overflow detection testing. The guard zones provide additional protection
 * by reserving memory before and after the actual thread stack.
 * 
 * When disabled, uses the standard Zephyr thread definition macro for simpler
 * thread creation without custom stack management.
 */
#ifdef CONFIG_IEC60730B_TEST_STACK

static K_THREAD_STACK_DEFINE(safety_stack, (CONFIG_APP_SAFETY_THREAD_STACK_SIZE + 2 * CONFIG_APP_SAFETY_TEST_STACK_GUARD_SIZE));
#define SAFETY_THREAD_STACK                 &safety_stack[CONFIG_APP_SAFETY_TEST_STACK_GUARD_SIZE]
#define SAFETY_THREAD_STACK_SIZE            (sizeof(safety_stack) - 2 * CONFIG_APP_SAFETY_TEST_STACK_GUARD_SIZE)
#define SAFETY_THREAD_STACK_WRITABLE_SIZE   (K_THREAD_STACK_SIZEOF(safety_stack) - 2 * CONFIG_APP_SAFETY_TEST_STACK_GUARD_SIZE) /* Thread writable stack buffer size */

static struct k_thread safety_thread_data;
static void safety_thread_create(void)
{
    k_tid_t thread_id;

    thread_id = k_thread_create(&safety_thread_data,
                                SAFETY_THREAD_STACK,
                                SAFETY_THREAD_STACK_WRITABLE_SIZE,
                                safety_thread,
                                NULL, NULL, NULL,
                                CONFIG_APP_SAFETY_THREAD_PRIORITY, 0,
                                K_NO_WAIT);

    if (thread_id == NULL) {
        LOG_ERR("Failed to create safety thread");
        safety_error_handling(IEC60730B_TEST_ERROR);
    }

    k_thread_name_set(thread_id, "safety");
}
#else
/* Safety test thread definition and automatic startup configuration. */
K_THREAD_DEFINE(safety, CONFIG_APP_SAFETY_THREAD_STACK_SIZE,
                safety_thread, NULL, NULL, NULL,
                CONFIG_APP_SAFETY_THREAD_PRIORITY, 0, 0);
#endif

int safety_error_code; /* Global error code. */

#ifdef CONFIG_IEC60730B_TEST_RAM
uint8_t safety_test_ram_buffer[CONFIG_APP_SAFETY_TEST_RAM_BUFFER_SIZE];
uint8_t safety_test_ram_backup_buffer[CONFIG_APP_SAFETY_TEST_RAM_BACKUP_BUFFER_SIZE];
#endif /* CONFIG_IEC60730B_TEST_RAM */

#ifdef CONFIG_IEC60730B_TEST_FLASH
/* Flash test buffer 1024 bytes */
const uint32_t safety_test_flash_buffer[256] = {
    0x00000001, 0x00000002, 0x00000003, 0x00000004, 0x00000005, 0x00000006, 0x00000007, 0x00000008,
    0x00000009, 0x0000000A, 0x0000000B, 0x0000000C, 0x0000000D, 0x0000000E, 0x0000000F, 0x00000010,
    0x00000011, 0x00000012, 0x00000013, 0x00000014, 0x00000015, 0x00000016, 0x00000017, 0x00000018,
    0x00000019, 0x0000001A, 0x0000001B, 0x0000001C, 0x0000001D, 0x0000001E, 0x0000001F, 0x00000020,
    0x00000021, 0x00000022, 0x00000023, 0x00000024, 0x00000025, 0x00000026, 0x00000027, 0x00000028,
    0x00000029, 0x0000002A, 0x0000002B, 0x0000002C, 0x0000002D, 0x0000002E, 0x0000002F, 0x00000030,
    0x00000031, 0x00000032, 0x00000033, 0x00000034, 0x00000035, 0x00000036, 0x00000037, 0x00000038,
    0x00000039, 0x0000003A, 0x0000003B, 0x0000003C, 0x0000003D, 0x0000003E, 0x0000003F, 0x00000040,
    0xAAAAAAAA, 0x55555555, 0xAAAAAAAA, 0x55555555, 0xAAAAAAAA, 0x55555555, 0xAAAAAAAA, 0x55555555,
    0xAAAAAAAA, 0x55555555, 0xAAAAAAAA, 0x55555555, 0xAAAAAAAA, 0x55555555, 0xAAAAAAAA, 0x55555555,
    0xAAAAAAAA, 0x55555555, 0xAAAAAAAA, 0x55555555, 0xAAAAAAAA, 0x55555555, 0xAAAAAAAA, 0x55555555,
    0xAAAAAAAA, 0x55555555, 0xAAAAAAAA, 0x55555555, 0xAAAAAAAA, 0x55555555, 0xAAAAAAAA, 0x55555555,
    0xAAAAAAAA, 0x55555555, 0xAAAAAAAA, 0x55555555, 0xAAAAAAAA, 0x55555555, 0xAAAAAAAA, 0x55555555,
    0xAAAAAAAA, 0x55555555, 0xAAAAAAAA, 0x55555555, 0xAAAAAAAA, 0x55555555, 0xAAAAAAAA, 0x55555555,
    0xAAAAAAAA, 0x55555555, 0xAAAAAAAA, 0x55555555, 0xAAAAAAAA, 0x55555555, 0xAAAAAAAA, 0x55555555,
    0xAAAAAAAA, 0x55555555, 0xAAAAAAAA, 0x55555555, 0xAAAAAAAA, 0x55555555, 0xAAAAAAAA, 0x55555555,
    0x00000001, 0x00000002, 0x00000004, 0x00000008, 0x00000010, 0x00000020, 0x00000040, 0x00000080,
    0x00000100, 0x00000200, 0x00000400, 0x00000800, 0x00001000, 0x00002000, 0x00004000, 0x00008000,
    0x00010000, 0x00020000, 0x00040000, 0x00080000, 0x00100000, 0x00200000, 0x00400000, 0x00800000,
    0x01000000, 0x02000000, 0x04000000, 0x08000000, 0x10000000, 0x20000000, 0x40000000, 0x80000000,
    0x80000000, 0x40000000, 0x20000000, 0x10000000, 0x08000000, 0x04000000, 0x02000000, 0x01000000,
    0x00800000, 0x00400000, 0x00200000, 0x00100000, 0x00080000, 0x00040000, 0x00020000, 0x00010000,
    0x00008000, 0x00004000, 0x00002000, 0x00001000, 0x00000800, 0x00000400, 0x00000200, 0x00000100,
    0x00000080, 0x00000040, 0x00000020, 0x00000010, 0x00000008, 0x00000004, 0x00000002, 0x00000001,
    0x12345678, 0x9ABCDEF0, 0x13579BDF, 0x2468ACE0, 0xFEDCBA98, 0x76543210, 0x89ABCDEF, 0x01234567,
    0x87654321, 0x0FEDCBA9, 0x13572468, 0xACE02468, 0x97531BDF, 0x86420ECA, 0x75319FDB, 0x64208EC9,
    0x53107FD8, 0x42096EC7, 0x31085FD6, 0x20074EC5, 0x10963FD4, 0x09852EC3, 0x08741FD2, 0x07630EC1,
    0x06521FD0, 0x05410ECF, 0x04309FDE, 0x03208ECD, 0x02107FDC, 0x01096ECB, 0x00985FDA, 0x00874EC9,
    0x00763FD8, 0x00652EC7, 0x00541FD6, 0x00430EC5, 0x00329FD4, 0x00218EC3, 0x00107FD2, 0x00096EC1,
    0x00085FD0, 0x00074ECF, 0x00063FDE, 0x00052ECD, 0x00041FDC, 0x00030ECB, 0x00029FDA, 0x00018EC9,
    0x00007FD8, 0x00006EC7, 0x00005FD6, 0x00004EC5, 0x00003FD4, 0x00002EC3, 0x00001FD2, 0x00000EC1,
    0x0000FD0F, 0x0000ECFE, 0x0000DFED, 0x0000CEDC, 0x0000BDCB, 0x0000ACBA, 0x00009BA9, 0x00008A98
};

/* Pre-calculated CRC value for the test buffer */
#ifdef CONFIG_IEC60730B_TEST_FLASH_CRC32
#define SAFETY_TEST_FLASH_BUFFER_CRC (0x228B7931)
#elif CONFIG_IEC60730B_TEST_FLASH_CRC16
#define SAFETY_TEST_FLASH_BUFFER_CRC (0x00F9)
#else
#error Not supported CONFIG_IEC60730B_TEST_FLASH_xxx
#endif

#endif /* CONFIG_IEC60730B_TEST_FLASH */

/* Task watchdog */
#ifdef CONFIG_APP_SAFETY_TASK_WATCHDOG

#if DT_NODE_HAS_STATUS_OKAY(DT_ALIAS(watchdog0))
#define WDT_NODE DT_ALIAS(watchdog0)
#else
#define WDT_NODE DT_INVALID_NODE
#endif

static int safety_task_wdt_init(void);
static void safety_task_wdt_callback(int channel_id, void *user_data);
static int safety_task_wdt_id = -1;

/* System initialization hook for task watchdog initialization. */
SYS_INIT(safety_task_wdt_init, APPLICATION, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT);

#endif /* CONFIG_APP_SAFETY_TASK_WATCHDOG */

/*******************************************************************************
 * Code
 ******************************************************************************/

/*
 *  Safety initialization function executed during system startup.
 */
static int safety_init(void)
{
    LOG_INF("Perform startup safety tests:");
    safety_startup_tests();

#ifdef CONFIG_IEC60730B_TEST_STACK
    safety_thread_create();
#endif

    return 0;
}

#ifdef CONFIG_APP_SAFETY_TASK_WATCHDOG
/*
 * Safety task watchdog initialization function executed during system startup.
 */
static int safety_task_wdt_init(void)
{
    int result;
    const struct device *const hw_wdt_dev = DEVICE_DT_GET_OR_NULL(WDT_NODE);

    if (!device_is_ready(hw_wdt_dev)) {
        LOG_WRN("Hardware watchdog not ready");
        result = task_wdt_init(NULL);
    } else {
        result = task_wdt_init(hw_wdt_dev);
    }

    if (result != 0) {
        LOG_ERR("task wdt init failure: %d\n", result);
        safety_error_handling(IEC60730B_TEST_ERROR);
    }

    /* Add a new task watchdog channel with the safety callback function */
    safety_task_wdt_id = task_wdt_add(CONFIG_APP_SAFETY_TASK_WATCHDOG_TIMEOUT_MS, safety_task_wdt_callback, NULL);
    if (safety_task_wdt_id < 0) {
        LOG_ERR("Failed to add task WDT channel");
        safety_error_handling(IEC60730B_TEST_ERROR);
    }
    LOG_INF("Task WDT channel %d added with timeout %d ms", safety_task_wdt_id, CONFIG_APP_SAFETY_TASK_WATCHDOG_TIMEOUT_MS);

    return 0;
}

/*
 * Callback function for task watchdog timeout events.
 */
static void safety_task_wdt_callback(int channel_id, void *user_data)
{
    ARG_UNUSED(user_data);
    LOG_WRN("Task watchdog channel %d timeout", channel_id);

    safety_error_handling(IEC60730B_TEST_WDT_ERROR);
}
#endif /* CONFIG_APP_SAFETY_TASK_WATCHDOG */

/*
 * Safety test thread function that executes periodic runtime safety tests.
 */
static void safety_thread(void *arg1, void *arg2, void *arg3)
{
    ARG_UNUSED(arg1);
    ARG_UNUSED(arg2);
    ARG_UNUSED(arg3);

    LOG_INF("Safety test thread started");

    /* Thread function runs indefinitely */
    while (1) {
        /* Perform runtime safety tests */
        safety_rutime_tests();

        /* Sleep before next iteration */
        k_msleep(CONFIG_APP_SAFETY_PERIOD_MS);
    }
}

/*
 * Safety error handling
 */
static void safety_error_handling(int error_code)
{
    safety_error_code = error_code;
    LOG_ERR("Safety error detected: 0x%08x", safety_error_code);

#ifdef CONFIG_APP_SAFETY_ERROR_ACTION_INFINITE_LOOP
    LOG_INF("Entering infinite loop");
    (void)irq_lock(); /* Disable interrupts */
    while (1){
#ifdef CONFIG_APP_SAFETY_TASK_WATCHDOG
        task_wdt_feed(safety_task_wdt_id);
#endif /* CONFIG_APP_SAFETY_TASK_WATCHDOG */
    }
#elif defined(CONFIG_APP_SAFETY_ERROR_ACTION_RESET)
    LOG_INF("Performing immediate cold reset");
    sys_reboot(SYS_REBOOT_COLD);
#endif
}

/*******************************************************************************
 * ADD YOUR SAFETY TESTS HERE
 ******************************************************************************/
/*
 * Executes safety tests during system startup.
 *
 * This function performs various safety tests that are required to be executed
 * during system initialization.
 * If any test fails, the safety error handling mechanism is invoked.
 */
static void safety_startup_tests(void)
{
    int result;

    LOG_INF("== Executing Start-up tests ==");

#ifdef CONFIG_IEC60730B_TEST_CPU_REG
    LOG_INF("CPU Registers test");
    result = iec60730b_test_cpu_reg();
    if(result < 0){
        safety_error_handling(result);
    }
#endif /* CONFIG_IEC60730B_TEST_CPU_REG */

#ifdef CONFIG_IEC60730B_TEST_RAM
    LOG_INF("RAM test");
    result = iec60730b_test_ram(safety_test_ram_buffer, sizeof(safety_test_ram_buffer),
                                safety_test_ram_backup_buffer, sizeof(safety_test_ram_backup_buffer),
                                IEC60730B_TEST_RAM_TYPE_MARCH_C);
    if (result < 0) {
        safety_error_handling(result);
    }
#endif /* CONFIG_IEC60730B_TEST_RAM */

#ifdef CONFIG_IEC60730B_TEST_PC
    LOG_INF("PC test");
    result = iec60730b_test_pc();
    if (result < 0) {
        safety_error_handling(result);
    }
#endif /* CONFIG_IEC60730B_TEST_PC */

#ifdef CONFIG_IEC60730B_TEST_STACK
    LOG_INF("Stack test initialization");
    result = iec60730b_test_stack_init(SAFETY_THREAD_STACK,
                                       SAFETY_THREAD_STACK_SIZE,
                                       CONFIG_APP_SAFETY_TEST_STACK_GUARD_SIZE,
                                       CONFIG_APP_SAFETY_TEST_STACK_GUARD_PATTERN);
    if (result < 0) {
        safety_error_handling(result);
    }
#endif /* CONFIG_IEC60730B_TEST_STACK */

#ifdef CONFIG_IEC60730B_TEST_FLASH
    LOG_INF("Flash test");
    result = iec60730b_test_flash_crc(safety_test_flash_buffer,
                                      sizeof(safety_test_flash_buffer),
                                      SAFETY_TEST_FLASH_BUFFER_CRC);
    if (result < 0) {
        safety_error_handling(result);
    }
#endif /* CONFIG_IEC60730B_TEST_FLASH */
}

/*
 * Executes safety tests during system runtime.
 *
 * This function performs various safety tests that are required to be executed
 * during normal system operation.
 * If any test fails, the safety error handling mechanism is invoked.
 */
static void safety_rutime_tests(void)
{
   int result;

    LOG_INF("== Executing Run-time tests ==");

#ifdef CONFIG_IEC60730B_TEST_CPU_REG
    LOG_INF("CPU Registers test");
    result = iec60730b_test_cpu_reg();
    if(result < 0){
        safety_error_handling(result);
    }
#endif /* CONFIG_IEC60730B_TEST_CPU_REG */

#ifdef CONFIG_IEC60730B_TEST_RAM
    LOG_INF("RAM test");
    result = iec60730b_test_ram(safety_test_ram_buffer, sizeof(safety_test_ram_buffer),
                                safety_test_ram_backup_buffer, sizeof(safety_test_ram_backup_buffer),
                                IEC60730B_TEST_RAM_TYPE_MARCH_X);
    if (result < 0) {
        safety_error_handling(result);
    }
#endif /* CONFIG_IEC60730B_TEST_RAM */

#ifdef CONFIG_IEC60730B_TEST_PC
    LOG_INF("PC test");
    result = iec60730b_test_pc();
    if (result < 0) {
        safety_error_handling(result);
    }
#endif /* CONFIG_IEC60730B_TEST_PC */

#ifdef CONFIG_IEC60730B_TEST_STACK
    LOG_INF("Stack test");
    result = iec60730b_test_stack(SAFETY_THREAD_STACK,
                                  SAFETY_THREAD_STACK_SIZE,
                                  CONFIG_APP_SAFETY_TEST_STACK_GUARD_SIZE,
                                  CONFIG_APP_SAFETY_TEST_STACK_GUARD_PATTERN);
    if (result < 0) {
        safety_error_handling(result);
    }
#endif /* CONFIG_IEC60730B_TEST_STACK */

#ifdef CONFIG_IEC60730B_TEST_FLASH
    LOG_INF("Flash test");
    result = iec60730b_test_flash_crc(safety_test_flash_buffer,
                                      sizeof(safety_test_flash_buffer),
                                      SAFETY_TEST_FLASH_BUFFER_CRC);
    if (result < 0) {
        safety_error_handling(result);
    }
#endif /* CONFIG_IEC60730B_TEST_FLASH */

#ifdef CONFIG_APP_SAFETY_TASK_WATCHDOG
    task_wdt_feed(safety_task_wdt_id);
#endif
}
