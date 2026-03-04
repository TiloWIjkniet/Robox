#
# Copyright 2024 NXP
# SPDX-License-Identifier: Apache-2.0
#
#

if(CONFIG_MCUX_COMPONENT_middleware.se_hostlib.mwlog)
    mcux_component_version(1.0.0)
    mcux_add_source(
        SOURCES hostlib/hostLib/libCommon/log/nxLog.c
                hostlib/hostLib/libCommon/log/*.h
        BASE_PATH ${SdkRootDirPath}/middleware/se_hostlib/
    )
    mcux_add_include(
        INCLUDES hostlib/hostLib/libCommon/log
        BASE_PATH ${SdkRootDirPath}/middleware/se_hostlib/
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.se_hostlib.commonCloudDemos)
    mcux_component_version(1.0.0)
    mcux_add_configuration(
        CC  "-DSSS_USE_FTR_FILE -DMBEDTLS -DSDK_OS_FREE_RTOS -DUSE_RTOS -DmqttconfigENABLE_METRICS -DSE_CLOUD_MCU_SDK"
    )
    mcux_add_source(
        SOURCES hostlib/hostLib/inc/ledHandler.h
                hostlib/hostLib/libCommon/infra/sm_demo_utils.c
                hostlib/hostLib/libCommon/infra/sm_demo_utils_rtos.c
                sss/plugin/pkcs11/sss_pkcs11_pal_core.c
                sss/plugin/pkcs11/sss_pkcs11_pal.h
                sss/plugin/pkcs11/nxLog_pkcs11.h
                sss/plugin/pkcs11/sss_pkcs11_pal_helpers.c
                sss/plugin/pkcs11/sss_pkcs11_pal_object.c
                sss/plugin/pkcs11/sss_pkcs11_pal_asymm.c
                sss/plugin/pkcs11/sss_pkcs11_pal_symm.c
                sss/plugin/pkcs11/sss_pkcs11_utils.c
                sss/plugin/pkcs11/sss_pkcs11_utils.h
                sss/plugin/pkcs11/base64_decode.c
        BASE_PATH ${SdkRootDirPath}/middleware/se_hostlib/
    )
    mcux_add_include(
        INCLUDES hostlib/hostLib/inc
                 sss/plugin/pkcs11
                 hostlib/hostLib/tstUtil
        BASE_PATH ${SdkRootDirPath}/middleware/se_hostlib/
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.se_hostlib.commonSe050CloudDemos)
    mcux_component_version(1.0.0)
    mcux_add_source(
        SOURCES ChangeLogKSDK.txt
        BASE_PATH ${SdkRootDirPath}/middleware/se_hostlib/
    )
    mcux_add_include(
        INCLUDES sss/inc
                 sss/ex/inc
                 sss/port/ksdk
                 hostlib/hostLib/se05x_03_xx_xx
                 sss/plugin/mbedtls
                 sss/port/ksdk
                 hostlib/hostLib/tstUtil
                 hostlib/hostLib/libCommon/infra
                 hostlib/hostLib/libCommon/log
                 hostlib/hostLib/mbedtls/inc
                 demos/ksdk/common/https_client
        BASE_PATH ${SdkRootDirPath}/middleware/se_hostlib/
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.se_hostlib.commonSe050_ksdk)
    mcux_component_version(1.0.0)
    mcux_add_source(
        SOURCES hostlib/hostLib/platform/inc/*.h
                hostlib/hostLib/platform/ksdk/*.h
                hostlib/hostLib/platform/ksdk/ax_reset.c
                hostlib/hostLib/platform/ksdk/i2c_frdm.c
                hostlib/hostLib/platform/ksdk/i2c_imxrt.c
                hostlib/hostLib/platform/ksdk/i2c_mcxa.c
                hostlib/hostLib/platform/ksdk/i2c_mcxn9.c
                hostlib/hostLib/platform/ksdk/i2c_lpc54xxx.c
                hostlib/hostLib/platform/ksdk/i2c_lpc55sxx.c
                hostlib/hostLib/platform/ksdk/se05x_reset.c
                hostlib/hostLib/platform/ksdk/timer_kinetis.c
                hostlib/hostLib/platform/ksdk/timer_kinetis_bm.c
                hostlib/hostLib/platform/ksdk/timer_kinetis_freertos.c
                hostlib/hostLib/platform/ksdk/timer_kinetis_threadx.c
        BASE_PATH ${SdkRootDirPath}/middleware/se_hostlib/
    )
    mcux_add_include(
        INCLUDES hostlib/hostLib/platform/inc
                 hostlib/hostLib/platform/ksdk
        BASE_PATH ${SdkRootDirPath}/middleware/se_hostlib/
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.se_hostlib.commonSe050smCom)
    mcux_component_version(1.0.0)
    mcux_add_source(
        SOURCES hostlib/hostLib/libCommon/smCom/apduComm.h
                hostlib/hostLib/libCommon/smCom/sci2c_cfg.h
                hostlib/hostLib/libCommon/smCom/smCom.c
                hostlib/hostLib/libCommon/smCom/smCom.h
                hostlib/hostLib/libCommon/smCom/smComT1oI2C.c
                hostlib/hostLib/libCommon/smCom/smComT1oI2C.h
                hostlib/hostLib/libCommon/smCom/T1oI2C/*.h
                hostlib/hostLib/libCommon/smCom/T1oI2C/*.c
        BASE_PATH ${SdkRootDirPath}/middleware/se_hostlib/
    )
    mcux_add_include(
        INCLUDES hostlib/hostLib/libCommon/smCom
                 hostlib/hostLib/libCommon/smCom/T1oI2C
        BASE_PATH ${SdkRootDirPath}/middleware/se_hostlib/
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.se_hostlib.commonSe050infra)
    mcux_component_version(1.0.0)
    mcux_add_source(
        SOURCES hostlib/hostLib/libCommon/infra/global_platf.c
                hostlib/hostLib/libCommon/infra/sm_connect.c
                hostlib/hostLib/libCommon/infra/*.h
                hostlib/hostLib/libCommon/infra/sm_apdu.c
                hostlib/hostLib/libCommon/infra/sm_errors.c
                hostlib/hostLib/libCommon/infra/sm_printf.c
        BASE_PATH ${SdkRootDirPath}/middleware/se_hostlib/
    )
    mcux_add_include(
        INCLUDES hostlib/hostLib/libCommon/infra
        BASE_PATH ${SdkRootDirPath}/middleware/se_hostlib/
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.se_hostlib.commonSe050_sss_ex)
    mcux_component_version(1.0.0)
    mcux_add_source(
        SOURCES sss/ex/inc/*.h
                sss/ex/src/ex_sss_boot.c
                sss/ex/src/ex_sss_boot_connectstring.c
                sss/ex/src/ex_sss_boot_int.h
                sss/ex/src/ex_sss_se05x.c
                sss/ex/src/ex_sss_se05x_auth.c
        BASE_PATH ${SdkRootDirPath}/middleware/se_hostlib/
    )
    mcux_add_include(
        INCLUDES sss/ex/inc
                 sss/ex/src
        BASE_PATH ${SdkRootDirPath}/middleware/se_hostlib/
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.se_hostlib.commonSe050_CurrentApplet)
    mcux_component_version(1.0.0)
    mcux_add_source(
        SOURCES hostlib/hostLib/se05x_03_xx_xx/se05x_APDU.c
                hostlib/hostLib/se05x_03_xx_xx/se05x_APDU.h
                hostlib/hostLib/se05x_03_xx_xx/se05x_APDU_apis.h
                hostlib/hostLib/se05x_03_xx_xx/se05x_APDU_impl.h
                hostlib/hostLib/se05x_03_xx_xx/se05x_04_xx_APDU_apis.h
                hostlib/hostLib/se05x_03_xx_xx/se05x_04_xx_APDU_impl.h
        BASE_PATH ${SdkRootDirPath}/middleware/se_hostlib/
    )
    mcux_add_include(
        INCLUDES hostlib/hostLib/se05x_03_xx_xx
        BASE_PATH ${SdkRootDirPath}/middleware/se_hostlib/
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.se_hostlib.commonSe050_Scp03)
    mcux_component_version(1.0.0)
    mcux_add_source(
        SOURCES hostlib/hostLib/libCommon/nxScp/nxScp03_Com.c
                hostlib/hostLib/libCommon/scp/scp.c
        BASE_PATH ${SdkRootDirPath}/middleware/se_hostlib/
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.se_hostlib.commonSe050)
    mcux_component_version(1.0.0)
    mcux_add_configuration(
        CC  "-DT1oI2C -DT1oI2C_UM11225 -DSSS_USE_FTR_FILE -DSDK_DEBUGCONSOLE_UART -DNO_SECURE_CHANNEL_SUPPORT"
        CX  "-DT1oI2C -DT1oI2C_UM11225"
    )
    mcux_add_source(
        SOURCES ChangeLogKSDK.txt
                sss/port/ksdk/fsl_sss_types.h
                hostlib/hostLib/inc/*.h
                hostlib/hostLib/se05x/src/*.c
                hostlib/hostLib/se05x/src/*.h
                sss/inc/*.h
                sss/src/*.c
                sss/src/se05x/*.c
        BASE_PATH ${SdkRootDirPath}/middleware/se_hostlib/
    )
    mcux_add_include(
        INCLUDES sss/port/ksdk
                 hostlib/hostLib/inc
                 hostlib/hostLib/se05x/src
                 sss/inc
                 .
                 platform
        BASE_PATH ${SdkRootDirPath}/middleware/se_hostlib/
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.se_hostlib.commonSe050_mcxa)
    mcux_component_version(1.0.0)
    mcux_add_configuration(
        CC  "-DT1oI2C -DT1oI2C_UM11225 -DSSS_USE_FTR_FILE -DSDK_DEBUGCONSOLE_UART -DNO_SECURE_CHANNEL_SUPPORT"
        CX  "-DT1oI2C -DT1oI2C_UM11225"
    )
    mcux_add_source(
        SOURCES ChangeLogKSDK.txt
                sss/port/ksdk/fsl_sss_types.h
                hostlib/hostLib/inc/*.h
                hostlib/hostLib/se05x/src/*.c
                hostlib/hostLib/se05x/src/*.h
                sss/inc/*.h
                sss/src/*.c
                sss/src/se05x/*.c
        BASE_PATH ${SdkRootDirPath}/middleware/se_hostlib/
    )
    mcux_add_include(
        INCLUDES sss/port/ksdk
                 hostlib/hostLib/inc
                 hostlib/hostLib/se05x/src
                 sss/inc
                 .
                 platform
        BASE_PATH ${SdkRootDirPath}/middleware/se_hostlib/
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.se_hostlib.cloud_Wifi)
    mcux_component_version(1.0.0)
    mcux_add_configuration(
        CC  "-DLPC_WIFI"
    )
    mcux_add_source(
        SOURCES demos/ksdk/common/wifi_config.h
                demos/ksdk/common/aws_shadow_config.h
                demos/ksdk/common/aws_wifi_config.h
                demos/ksdk/common/iot_wifi.h
        BASE_PATH ${SdkRootDirPath}/middleware/se_hostlib/
    )
    mcux_add_include(
        INCLUDES demos/ksdk/common
        BASE_PATH ${SdkRootDirPath}/middleware/se_hostlib/
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.se_hostlib.tstutils.time)
    mcux_component_version(1.0.0)
    mcux_add_source(
        SOURCES hostlib/hostLib/tstUtil/tst_sm_time.h
                hostlib/hostLib/tstUtil/tst_sm_time_kinetis.c
        BASE_PATH ${SdkRootDirPath}/middleware/se_hostlib/
    )
    mcux_add_include(
        INCLUDES hostlib/hostLib/tstUtil
        BASE_PATH ${SdkRootDirPath}/middleware/se_hostlib/
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.se_hostlib.tstutils)
    mcux_component_version(1.0.0)
    mcux_add_source(
        SOURCES hostlib/hostLib/libCommon/infra/sm_app_boot.c
                hostlib/hostLib/tstUtil/tst_sm_util.c
                hostlib/hostLib/tstUtil/tst_sm_util.h
                hostlib/hostLib/libCommon/infra/sm_demo_utils.c
                hostlib/hostLib/libCommon/infra/sm_demo_utils_rtos.c
        BASE_PATH ${SdkRootDirPath}/middleware/se_hostlib/
    )
    mcux_add_include(
        INCLUDES hostlib/hostLib/tstUtil
        BASE_PATH ${SdkRootDirPath}/middleware/se_hostlib/
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.se_hostlib.mbedtls_alt_demo_common)
    mcux_component_version(1.0.0)
    mcux_add_source(
        SOURCES sss/plugin/mbedtls/ecdh_alt.c
                sss/plugin/mbedtls/rsa_alt.c
                sss/plugin/mbedtls/*.h
                sss/port/ksdk/*.h
                sss/src/mbedtls/*.c
        BASE_PATH ${SdkRootDirPath}/middleware/se_hostlib/
    )
    mcux_add_include(
        INCLUDES hostlib/hostLib/mbedtls/inc
                 sss/plugin/mbedtls
                 sss/port/ksdk
        BASE_PATH ${SdkRootDirPath}/middleware/se_hostlib/
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.se_hostlib.mbedtls_sss)
    mcux_component_version(1.0.0)
    mcux_add_configuration(
        CC  "-DMBEDTLS_CONFIG_FILE=\\\"ksdk_mbedtls_config.h\\\" -DMBEDTLS_USER_CONFIG_FILE=\\\"sss_ksdk_mbedtls_config.h\\\" -DMBEDTLS -DSCP_MODE=C_MAC_C_ENC_R_MAC_R_ENC"
    )
    mcux_add_source(
        SOURCES sss/plugin/mbedtls/*.c
        BASE_PATH ${SdkRootDirPath}/middleware/se_hostlib/
    )
    mcux_add_include(
        INCLUDES sss/plugin/mbedtls
                 sss/port/ksdk
        BASE_PATH ${SdkRootDirPath}/middleware/se_hostlib/
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.se_hostlib.serial_manager_usb_cdc)
    mcux_component_version(1.0.0)
    mcux_add_configuration(
        CC  "-DDEBUG_CONSOLE_TRANSFER_NON_BLOCKING -DUSB_DEVICE_CONFIG_CDC_ACM=1"
        CX  "-DDEBUG_CONSOLE_TRANSFER_NON_BLOCKING -DUSB_DEVICE_CONFIG_CDC_ACM=1"
    )
    mcux_add_source(
        SOURCES hostlib/hostLib/vcom/ksdk/usb_device_descriptor.c
                hostlib/hostLib/vcom/ksdk/usb_device_descriptor.h
        BASE_PATH ${SdkRootDirPath}/middleware/se_hostlib/
    )
    mcux_add_include(
        INCLUDES hostlib/hostLib/vcom/ksdk
        BASE_PATH ${SdkRootDirPath}/middleware/se_hostlib/
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.se_hostlib.mbedtls_sss_mcx)
    mcux_component_version(1.0.0)
    mcux_add_configuration(
        CC  "-DMBEDTLS_CONFIG_FILE=\\\"els_pkc_mbedtls_config.h\\\" -DMBEDTLS -DSCP_MODE=C_MAC_C_ENC_R_MAC_R_ENC"
    )
    mcux_add_source(
        SOURCES sss/src/mbedtls/*.c
        BASE_PATH ${SdkRootDirPath}/middleware/se_hostlib/
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.se_hostlib.sss_lp)
    mcux_component_version(1.0.0)
    mcux_add_configuration(
        CC  "-DSCP_MODE=C_MAC_C_ENC_R_MAC_R_ENC"
    )
endif()