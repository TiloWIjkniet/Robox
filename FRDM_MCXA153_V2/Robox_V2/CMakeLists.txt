
# Copyright 2026 NXP
#
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.22.0)
set(board frdmmcxa153)
cmake_path(APPEND conf_file_path ${CMAKE_CURRENT_LIST_DIR} ${board} prj.conf)
set(CONF_FILE ${conf_file_path} ${CONF_FILE})
include(${SdkRootDirPath}/cmake/extension/mcux.cmake)

project(led_blinky_peripheral LANGUAGES C CXX ASM PROJECT_BOARD_PORT_PATH ${board_root}/${board}/demo_apps/led_blinky)

include(${board}/board_files.cmake)
include(${SdkRootDirPath}/CMakeLists.txt)

mcux_add_source(
    SOURCES main.c
            board/board.h

            fsm/fsm.c
            fsm/fsm.h
            fsm/states.h
            fsm/events.h
            fsm/fsm_init.c

            state_logic/state_logic.h

            gpio/gpio.h
            gpio/gpio.c
)

mcux_add_include(
    INCLUDES .
            board
            fsm
            state_logic
            gpio
   
)

mcux_convert_binary(BINARY ${APPLICATION_BINARY_DIR}/${MCUX_SDK_PROJECT_NAME}.bin)
