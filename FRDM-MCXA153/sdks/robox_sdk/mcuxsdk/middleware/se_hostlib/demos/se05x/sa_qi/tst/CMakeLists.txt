#
# Copyright 2022 NXP
# SPDX-License-Identifier: Apache-2.0
#

PROJECT(test_qi_transmitter)

FILE(
    GLOB
    files
    *.c
)

ADD_EXECUTABLE(
    ${PROJECT_NAME}
    ${KSDK_STARTUP_FILE} ${files}
)

TARGET_INCLUDE_DIRECTORIES(
    ${PROJECT_NAME}
    PRIVATE .
    PRIVATE ..
)

TARGET_LINK_LIBRARIES(
    ${PROJECT_NAME}
    sa_qi_transmitter_auth
    se_test_common
)

CREATE_BINARY(${PROJECT_NAME})
