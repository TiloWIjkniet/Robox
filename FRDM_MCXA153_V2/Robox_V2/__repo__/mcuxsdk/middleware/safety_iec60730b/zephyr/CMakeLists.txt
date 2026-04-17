# Copyright 2025 NXP
# SPDX-License-Identifier: Apache-2.0

if(CONFIG_IEC60730B)

    # Set path varriables
    set(IEC60730B_DIR ${ZEPHYR_CURRENT_MODULE_DIR})
    set(IEC60730B_SRC_DIR ${IEC60730B_DIR}/source)
    set(IEC60730B_ZEPHYR_SRC_DIR ${ZEPHYR_CURRENT_MODULE_DIR}/zephyr/src)

    zephyr_library_named(safety_iec60730b)

    # Tests for Cortex-M0+ CPU
    if(CONFIG_CPU_CORTEX_M0PLUS)

        zephyr_include_directories(
            ${IEC60730B_SRC_DIR}/core_test/cm0
            ${IEC60730B_SRC_DIR}/core_test/cm0/flash
            ${IEC60730B_SRC_DIR}/core_test/cm0/programCounter
            ${IEC60730B_SRC_DIR}/core_test/cm0/ram
            ${IEC60730B_SRC_DIR}/core_test/cm0/register
            ${IEC60730B_SRC_DIR}/core_test/cm0/stack
        )

        zephyr_library_sources_ifdef(CONFIG_IEC60730B_TEST_CPU_REG ${IEC60730B_SRC_DIR}/core_test/cm0/register/iec60730b_cm0_reg.S)
        zephyr_library_sources_ifdef(CONFIG_IEC60730B_TEST_RAM ${IEC60730B_SRC_DIR}/core_test/cm0/ram/iec60730b_cm0_ram.S)
        zephyr_library_sources_ifdef(CONFIG_IEC60730B_TEST_PC ${IEC60730B_SRC_DIR}/core_test/cm0/programCounter/iec60730b_cm0_pc.S
                                                              ${IEC60730B_SRC_DIR}/core_test/cm0/programCounter/iec60730b_cm0_pc_object.S)
        zephyr_library_sources_ifdef(CONFIG_IEC60730B_TEST_FLASH ${IEC60730B_SRC_DIR}/core_test/cm0/flash/iec60730b_cm0_flash.S)
        zephyr_library_sources_ifdef(CONFIG_IEC60730B_TEST_STACK ${IEC60730B_SRC_DIR}/core_test/cm0/stack/iec60730b_cm0_stack.S)
        zephyr_library_sources(${IEC60730B_ZEPHYR_SRC_DIR}/iec60730b_test_cm0.c)

    # Tests for Cortex-M4/M7 CPU
    elseif(CONFIG_CPU_CORTEX_M4 OR CONFIG_CPU_CORTEX_M7)

        zephyr_include_directories(
            ${IEC60730B_SRC_DIR}/core_test/cm4_cm7
            ${IEC60730B_SRC_DIR}/core_test/cm4_cm7/flash
            ${IEC60730B_SRC_DIR}/core_test/cm4_cm7/programCounter
            ${IEC60730B_SRC_DIR}/core_test/cm4_cm7/ram
            ${IEC60730B_SRC_DIR}/core_test/cm4_cm7/register
            ${IEC60730B_SRC_DIR}/core_test/cm4_cm7/stack
        )

        zephyr_library_sources_ifdef(CONFIG_IEC60730B_TEST_CPU_REG ${IEC60730B_SRC_DIR}/core_test/cm4_cm7/register/iec60730b_cm4_cm7_reg.S)
        zephyr_library_sources_ifdef(CONFIG_IEC60730B_TEST_CPU_REG_FPU ${IEC60730B_SRC_DIR}/core_test/cm4_cm7/register/iec60730b_cm4_cm7_reg_fpu.S)
        zephyr_library_sources_ifdef(CONFIG_IEC60730B_TEST_RAM ${IEC60730B_SRC_DIR}/core_test/cm4_cm7/ram/iec60730b_cm4_cm7_ram.S)
        zephyr_library_sources_ifdef(CONFIG_IEC60730B_TEST_PC ${IEC60730B_SRC_DIR}/core_test/cm4_cm7/programCounter/iec60730b_cm4_cm7_pc.S
                                                              ${IEC60730B_SRC_DIR}/core_test/cm4_cm7/programCounter/iec60730b_cm4_cm7_pc_object.S)
        zephyr_library_sources_ifdef(CONFIG_IEC60730B_TEST_FLASH ${IEC60730B_SRC_DIR}/core_test/cm4_cm7/flash/iec60730b_cm4_cm7_flash.S)
        zephyr_library_sources_ifdef(CONFIG_IEC60730B_TEST_STACK ${IEC60730B_SRC_DIR}/core_test/cm4_cm7/stack/iec60730b_cm4_cm7_stack.S)
        zephyr_library_sources(${IEC60730B_ZEPHYR_SRC_DIR}/iec60730b_test_cm4_cm7.c)

    # Tests for Cortex-M33 CPU
    elseif(CONFIG_CPU_CORTEX_M33)

        zephyr_include_directories(
            ${IEC60730B_SRC_DIR}/core_test/cm33
            ${IEC60730B_SRC_DIR}/core_test/cm33/flash
            ${IEC60730B_SRC_DIR}/core_test/cm33/programCounter
            ${IEC60730B_SRC_DIR}/core_test/cm33/ram
            ${IEC60730B_SRC_DIR}/core_test/cm33/register
            ${IEC60730B_SRC_DIR}/core_test/cm33/stack
        )

        zephyr_library_sources_ifdef(CONFIG_IEC60730B_TEST_CPU_REG ${IEC60730B_SRC_DIR}/core_test/cm33/register/iec60730b_cm33_reg.S)
        zephyr_library_sources_ifdef(CONFIG_IEC60730B_TEST_CPU_REG_FPU ${IEC60730B_SRC_DIR}/core_test/cm33/register/iec60730b_cm33_reg_fpu.S)
        zephyr_library_sources_ifdef(CONFIG_IEC60730B_TEST_CPU_REG_DSP ${IEC60730B_SRC_DIR}/core_test/cm33/register/iec60730b_cm33_reg_dsp_ext.S)
        zephyr_library_sources_ifdef(CONFIG_IEC60730B_TEST_RAM ${IEC60730B_SRC_DIR}/core_test/cm33/ram/iec60730b_cm33_ram.S)
        zephyr_library_sources_ifdef(CONFIG_IEC60730B_TEST_PC ${IEC60730B_SRC_DIR}/core_test/cm33/programCounter/iec60730b_cm33_pc.S
                                                              ${IEC60730B_SRC_DIR}/core_test/cm33/programCounter/iec60730b_cm33_pc_object.S)
        zephyr_library_sources_ifdef(CONFIG_IEC60730B_TEST_FLASH ${IEC60730B_SRC_DIR}/core_test/cm33/flash/iec60730b_cm33_flash.S)
        zephyr_library_sources_ifdef(CONFIG_IEC60730B_TEST_STACK ${IEC60730B_SRC_DIR}/core_test/cm33/stack/iec60730b_cm33_stack.S)
        zephyr_library_sources(${IEC60730B_ZEPHYR_SRC_DIR}/iec60730b_test_cm33.c)

    else()
        message(FATAL_ERROR "Platform is not supported!")
    endif()

    # Common tests
    zephyr_include_directories(
        ${IEC60730B_SRC_DIR}
        ${IEC60730B_SRC_DIR}/common_test/aio
        ${IEC60730B_SRC_DIR}/common_test/clock
        ${IEC60730B_SRC_DIR}/common_test/dio
        ${IEC60730B_SRC_DIR}/common_test/flash
        ${IEC60730B_SRC_DIR}/common_test/tsi
        ${IEC60730B_SRC_DIR}/common_test/wdog
        ${IEC60730B_SRC_DIR}/compiler
        ${IEC60730B_ZEPHYR_SRC_DIR}
    )

    if(CONFIG_IEC60730B_TEST_AIO)
        zephyr_library_sources(${IEC60730B_SRC_DIR}/common_test/aio/iec60730b_aio.c)
        set_source_files_properties(${IEC60730B_SRC_DIR}/common_test/aio/iec60730b_aio.c PROPERTIES COMPILE_OPTIONS "-Wno-parentheses") # disable warning
    endif()

    if(CONFIG_IEC60730B_TEST_FLASH)
        zephyr_library_sources(${IEC60730B_SRC_DIR}/common_test/flash/iec60730b_invariable_memory.c)
        set_source_files_properties(${IEC60730B_SRC_DIR}/common_test/flash/iec60730b_invariable_memory.c PROPERTIES COMPILE_OPTIONS "-Wno-maybe-uninitialized")  # disable warning
    endif()

    zephyr_library_sources_ifdef(CONFIG_IEC60730B_TEST_CLOCK ${IEC60730B_SRC_DIR}/common_test/clock/iec60730b_clock.c)
    zephyr_library_sources_ifdef(CONFIG_IEC60730B_TEST_DIO ${IEC60730B_SRC_DIR}/common_test/dio/iec60730b_dio.c
                                                           ${IEC60730B_SRC_DIR}/common_test/dio/iec60730b_dio_ext.c )
    zephyr_library_sources_ifdef(CONFIG_IEC60730B_TEST_TSI ${IEC60730B_SRC_DIR}/common_test/tsi/iec60730b_tsi.c)
    zephyr_library_sources_ifdef(CONFIG_IEC60730B_TEST_WDOG ${IEC60730B_SRC_DIR}/common_test/wdog/iec60730b_wdog.c)

else() # CONFIG_IEC60730B=n
    message("Safety iec60730b library excluded from the build. ")
endif()
