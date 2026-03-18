
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

            fifo/fifo.c
            fifo/fifo.h

            fsm/fsm.c
            fsm/fsm_config.c
            fsm/states.h
            fsm/fsm.h
            fsm/events.h
            
            serial2lupuart/lpuart2.c
            lupuaserial2lupuartrt2/lpuart2.h

            serial/retarget.c
            serial/serial.c
            serial/serial.h

            state_logic/idle.c
            state_logic/dev_page.c
            state_logic/rooms.c
            state_logic/state_lifecycle.c
            state_logic/game_logic.c
            state_logic/state_logic.h
            state_logic/game_logic.h
            state_logic/game_data.h
            state_logic/display_template.h
            state_logic/display_template.c

            util/keypad.c
            util/keypad.h
            util/time_millis.c
            util/time_millis.h
            util/buzzer.c
            util/buzzer.h
            util/touch_sensor.c
            util/touch_sensor.h
            util/hexDisplay.c
            util/hexDisplay.h
            
            board/board.h
)

mcux_add_include(
    INCLUDES .
            board
            fifo
            fsm
            serial
            state_logic
            util
            serial2lupuart
   
)

mcux_convert_binary(BINARY ${APPLICATION_BINARY_DIR}/${MCUX_SDK_PROJECT_NAME}.bin)
