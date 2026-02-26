
# Copyright 2026 NXP
#
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.22.0)

include(${SdkRootDirPath}/cmake/extension/mcux.cmake)

project(led_blinky_peripheral LANGUAGES C CXX ASM PROJECT_BOARD_PORT_PATH ${board_root}/${board}/demo_apps/led_blinky)

include(${SdkRootDirPath}/CMakeLists.txt)


mcux_add_source(
    SOURCES main.c
            fifo/fifo.c
            fsm/fsm.c
            fsm/fsm_config.c
            serial/retarget.c
            serial/serial.c
            state_logic/idle.c
            state_logic/dev_page.c
            state_logic/
)

mcux_add_include(
    INCLUDES .
            fifo
            fsm
            serial
            state_logic
   
)
include(${SdkRootDirPath}/${board_root}/${board}/demo_apps/led_blinky/reconfig.cmake OPTIONAL)

mcux_convert_binary(BINARY ${APPLICATION_BINARY_DIR}/${MCUX_SDK_PROJECT_NAME}.bin)
