/* Copyright 2018,2020 NXP
* SPDX-License-Identifier: Apache-2.0
*/

#include "azureDemo.h"
#include "azure_iot_config.h"
#include "azure_client_credential_keys.h"

#if defined(SSS_USE_FTR_FILE)
#include "fsl_sss_ftr.h"
#else
#include "fsl_sss_ftr_default.h"
#endif

#include <stdio.h>
#include <ledHandler.h>
#include "nxLog_App.h"

#if SSS_HAVE_MBEDTLS_ALT_SSS
#include "sss_mbedtls.h"
#include "ex_sss.h"
#include "sm_printf.h"
#endif

#if AX_EMBEDDED
#include <FreeRTOS.h>
#include "board.h"
#include "clock_config.h"
#include "fsl_debug_console.h"
#include "fsl_device_registers.h"
#include "ksdk_mbedtls.h"
#include "pin_mux.h"
#include "task.h"
#include "sm_demo_utils.h"
#if defined(MIMXRT1062_SERIES) || defined (MIMXRT1061_SERIES)
#include "fsl_dcp.h"
#include "fsl_iomuxc.h"
#include "fsl_trng.h"
#include "pin_mux.h"

#endif

#ifdef CPU_LPC54018
#include "fsl_power.h"
#endif

#endif

#if SSS_HAVE_MBEDTLS_ALT_SSS

#include <fsl_sss_api.h>

#endif

void azure_task_lwip(void *ctx)
{
#if SSS_HAVE_MBEDTLS_ALT_SSS || SSS_HAVE_MBEDTLS_ALT_A71CH

    /*keyJITR_DEVICE_CERTIFICATE_AUTHORITY_PEM Not used variable to avoid warning set it to NULL*/
    keyJITR_DEVICE_CERTIFICATE_AUTHORITY_PEM = NULL;
    LOG_I("Azure Secure Element example ...\n\n");

    // #if (SSS_HAVE_MBEDTLS_ALT_A71CH && AX_EMBEDDED)
    //     mbedtls_pk_init(&pk);
    // #endif

#if AX_EMBEDDED
    LED_RED_OFF();
    LED_GREEN_OFF();
    LED_BLUE_ON();

    LED_BLUE_OFF();
#endif
#endif

    azurePubSub();

    while (1) {
#if AX_EMBEDDED
        __disable_irq();
        __WFI(); /* Never exit */
#else
        LOG_I("HALT!");
        getchar();
#endif
    }
}
