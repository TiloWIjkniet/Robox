
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

            display/display.c
            display/display.h

            drivers/gpio/gpio.c
            drivers/gpio/gpio.h

            drivers/audio/audio.h
            drivers/audio/audio.c

            drivers/buzzer/buzzer.h
            drivers/buzzer/buzzer.c

            drivers/esp/eso.h
            drivers/esp/esp.c

            drivers/fingerprint_scanner/fingerprint_scanner.c
            drivers/fingerprint_scanner/fingerprint_scanner.h

            drivers/hexDisplay/hexDisplay.h
            drivers/hexDisplay/hexDisplay.c

            drivers/hm10/hm10.h
            drivers/hm10/hm10.c

            drivers/key/key.h
            drivers/key/key.c

            drivers/keypad/keypad.h
            drivers/keypad/keypad.c

            drivers/switch/switch.h
            drivers/switch/switch.c

            drivers/time/time.c
            drivers/time/time.h
            drivers/time/delay.h
            drivers/time/delay.c
)

mcux_add_include(
    INCLUDES .
            board
            fsm
            state_logic
            display
            drivers
                gpio
                audio
                buzzer
                esp
                fingerprint_scanner
                hexDisplay
                hm10
                key
                keypad
                switch
                time
   
)

mcux_convert_binary(BINARY ${APPLICATION_BINARY_DIR}/${MCUX_SDK_PROJECT_NAME}.bin)
