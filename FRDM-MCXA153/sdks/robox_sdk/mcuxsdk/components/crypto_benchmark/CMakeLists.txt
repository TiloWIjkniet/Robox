# Copyright 2024-2025 NXP
# SPDX-License-Identifier: BSD-3-Clause

if(OS STREQUAL "Linux")
    cmake_minimum_required(VERSION 3.13)

    project(CryptoBenchmark VERSION 1.0 LANGUAGES C)

    set(CMAKE_INSTALL_PREFIX /usr)
    include(GNUInstallDirs)

    find_package(NXP_SMW REQUIRED)

    add_executable(${PROJECT_NAME})

    add_subdirectory(osal/linux)

    file(GLOB objects_src *.c)

    target_sources(${PROJECT_NAME} PRIVATE ${objects_src})

    target_include_directories(${PROJECT_NAME} PRIVATE
                               ${CMAKE_CURRENT_SOURCE_DIR}/include
                               .
    )

    add_subdirectory(opaque)
    add_subdirectory(transparent)

    if(NOT DEFINED PORT)
        message(WARNING "Port layer is not defined. Default port layer is PSA")
        set(PORT "PSA")
    endif()

    if (PORT STREQUAL "PSA")
        target_link_libraries(${PROJECT_NAME} PUBLIC NXP_SMW::smw)
        add_subdirectory(port/psa)
    elseif(PORT STREQUAL "PKCS11")
        target_link_libraries(${PROJECT_NAME} PUBLIC NXP_SMW::smw_pkcs11)
        get_target_property(SMW_PKCS11_LIB NXP_SMW::smw_pkcs11 IMPORTED_SONAME_RELEASE)
        set(DEFAULT_PKCS11_LIB ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/${SMW_PKCS11_LIB})
        add_subdirectory(port/pkcs11)
    else()
        message(FATAL_ERROR "Port layer is unknown: ${PORT}")
    endif()

    message(STATUS "Port layer is ${PORT}")

    target_sources(${PROJECT_NAME} PRIVATE
                   cases/aead_all.c
                   cases/aes_all.c
                   cases/ctr_drbg_all.c
                   cases/ecdsa_all.c
                   cases/hash_all.c
                   cases/mac_all.c
                   cases/rsa_all.c
    )

    return()
endif()

if(CONFIG_MCUX_COMPONENT_component.crypto_benchmark)
    mcux_component_version(2.0.0)
    mcux_add_source(
        SOURCES crypto_benchmark.c
                crypto_benchmark.h
                cb_helpers.c
                cb_common_config.h
                transparent/cb_rsa.c
                transparent/cb_ecdsa.c
                transparent/cb_ctr_drbg.c
                transparent/cb_mac.c
                transparent/cb_hash.c
                transparent/cb_aes.c
                transparent/cb_aead.c
                opaque/cb_rsa_opaque.c
                opaque/cb_ecdsa_opaque.c
                opaque/cb_mac_opaque.c
                opaque/cb_aes_opaque.c
                opaque/cb_aead_opaque.c
                include/cb_types.h
                include/cb_rsa.h
                include/cb_ecdsa.h
                include/cb_ctr_drbg.h
                include/cb_mac.h
                include/cb_hash.h
                include/cb_aes.h
                include/cb_aead.h
                osal/baremetal/common/cb_init.c
                osal/baremetal/common/cb_tick.c
                osal/baremetal/common/cb_exit.c
        BASE_PATH ${SdkRootDirPath}/components/crypto_benchmark/
    )
    mcux_add_include(
        INCLUDES .
                 include
        BASE_PATH ${SdkRootDirPath}/components/crypto_benchmark/
    )

    mcux_add_macro(
    CC "-DPRINTF_FLOAT_ENABLE=1\
       -DSCANF_FLOAT_ENABLE=0\
       -DPRINTF_ADVANCED_ENABLE=1\
       -DSCANF_ADVANCED_ENABLE=0"
    )
endif()

if(CONFIG_MCUX_COMPONENT_component.crypto_benchmark.port.ele_s400)
    mcux_component_version(2.0.0)
    mcux_add_configuration(
        CC  "-DBENCHMARK_PLATFORM_CONFIG=1 -DUSE_HYPERRAM"
    )
    mcux_add_source(
        SOURCES port/ele_s400/ele.h
                port/ele_s400/ele_init.c
                port/ele_s400/ele_init.h
                port/ele_s400/ele_fw.h
                port/ele_s400/hash.c
                port/ele_s400/mac.c
                port/ele_s400/aead.c
                port/ele_s400/aes.c
                port/ele_s400/rsa.c
                port/ele_s400/ecdsa.c
                port/ele_s400/ctr_drbg.c
                osal/baremetal/s400/include/cb_osal.h
                cases/aead_sxxx.c
                cases/aes_s400.c
                cases/ctr_drbg_all.c
                cases/ecdsa_all.c
                cases/hash_s400.c
                cases/mac_s400.c
                cases/rsa_all.c
        BASE_PATH ${SdkRootDirPath}/components/crypto_benchmark/
    )
    mcux_add_include(
        INCLUDES port/ele_s400
                 osal/baremetal/s400/include
        BASE_PATH ${SdkRootDirPath}/components/crypto_benchmark/
    )
endif()

if(CONFIG_MCUX_COMPONENT_component.crypto_benchmark.port.ele_s200)
    mcux_component_version(2.0.0)
    mcux_add_configuration(
        CC  "-DBENCHMARK_PLATFORM_CONFIG=1"
    )
    mcux_add_source(
        SOURCES port/ele_s200/ele.h
                port/ele_s200/ele_init.c
                port/ele_s200/ele_init.h
                port/ele_s200/hash.c
                port/ele_s200/aead.c
                port/ele_s200/aes.c
                port/ele_s200/rsa.c
                port/ele_s200/ctr_drbg.c
                osal/baremetal/s200/include/cb_osal.h
                cases/aead_sxxx.c
                cases/aes_s200.c
                cases/ctr_drbg_all.c
        BASE_PATH ${SdkRootDirPath}/components/crypto_benchmark/
    )
    mcux_add_include(
        INCLUDES port/ele_s200
                 osal/baremetal/s200/include
        BASE_PATH ${SdkRootDirPath}/components/crypto_benchmark/
    )
endif()

if(CONFIG_MCUX_COMPONENT_component.crypto_benchmark.port.psa)
    mcux_component_version(2.0.0)
    mcux_add_configuration(
        CC  "-DBENCHMARK_PLATFORM_CONFIG=1"
    )
    mcux_add_source(
        SOURCES port/psa/aead.c
                port/psa/aes.c
                port/psa/ecdsa.c
                port/psa/hash.c
                port/psa/mac.c
                port/psa/rsa.c
                port/psa/common_utils.c
                port/psa/include/common_utils.h
                osal/baremetal/psa/include/cb_osal.h
                cases/aead_all.c
                cases/aes_all.c
        BASE_PATH ${SdkRootDirPath}/components/crypto_benchmark/
    )
    mcux_add_include(
        INCLUDES port/psa
                 port/psa/include
                 osal/baremetal/psa/include
        BASE_PATH ${SdkRootDirPath}/components/crypto_benchmark/
    )

    # Preemptively add a bunch of memory to stack and heap
    mcux_add_linker_symbol(
        SYMBOLS "__stack_size__=0x2000\
                 __heap_size__=0x2000"
    )
endif()

if(CONFIG_MCUX_COMPONENT_component.crypto_benchmark.port.custom)
    mcux_component_version(2.0.0)
    mcux_add_configuration(
        CC  "-DBENCHMARK_PLATFORM_CONFIG=1"
    )
    mcux_add_source(
        SOURCES port/custom/wrappers.c
                port/custom/init.c
                osal/baremetal/psa/include/cb_osal.h
        BASE_PATH ${SdkRootDirPath}/components/crypto_benchmark/
    )
    mcux_add_include(
        INCLUDES port/custom
                 osal/baremetal/psa/include
        BASE_PATH ${SdkRootDirPath}/components/crypto_benchmark/
    )
endif()
