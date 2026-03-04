/*
* Copyright 2010-2015 Amazon.com, Inc. or its affiliates. All Rights Reserved.
* Copyright 2017,2020 NXP. Not a Contribution
*
* Licensed under the Apache License, Version 2.0 (the "License").
* You may not use this file except in compliance with the License.
* A copy of the License is located at
*
*  http://aws.amazon.com/apache2.0
*
* or in the "license" file accompanying this file. This file is distributed
* on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
* express or implied. See the License for the specific language governing
* permissions and limitations under the License.
*/

/**
* @file subscribe_publish_sample.c
* @brief simple MQTT publish and subscribe on the same topic
*
* This example takes the parameters from the aws_iot_config.h file and establishes a connection to the AWS IoT MQTT
* Platform.
* It subscribes and publishes to the same topic - "sdkTest/sub"
*
* If all the certs are correct, you should see the messages received by the application in a loop.
*
*/

#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "FreeRTOS.h"
#include "gcpJWTDemo.h"
#include "gcp_iot_config.h"
#include "gcp_default_root_certificates.h"
#include "core_json.h"
#include "ledHandler.h"
#include "task.h"
#include "ex_sss.h"
#include "core_mqtt.h"
#include "common_cloud_config.h"

#if !defined (SE_CLOUD_MCU_SDK)
#include "git_version.h"
#else
#include "board.h"
#define BUILD_GIT_VERSION_TOP_SHA1 "NA"
#define BUILD_BOARD_INFO BOARD_NAME
#endif

#if AX_EMBEDDED
#include "clock_config.h"
#include "fsl_device_registers.h"
#include "ksdk_mbedtls.h"
#ifdef LPC_ENET
#include "lwip/netif.h"
#include "ethernetif.h"
#include "lwip/dhcp.h"
#include "lwip/opt.h"
#include "lwip/prot/dhcp.h"
#include "lwip/tcpip.h"
#include "netif/ethernet.h"
#endif

#include "pin_mux.h"
#if SSS_HAVE_MBEDTLS_ALT_A71CH
#include "ax_mbedtls.h"
#endif
#if SSS_HAVE_MBEDTLS_ALT_SSS
#include "sss_mbedtls.h"
#endif
#include "sm_demo_utils.h"
#include "board.h"
#endif

#include "nxLog_App.h"

/*******************************************************************************
* Definitions
******************************************************************************/

#define ENABLE_TIME_PROFILING 0

/*******************************************************************************
* Prototypes
******************************************************************************/
int HandleReceivedMsg(char *sJsonString, uint16_t len);

extern BaseType_t mqttDemoMain(char *pUserName, char *pPassword, char *pPayload);

/*******************************************************************************
* Variables
******************************************************************************/

#if ENABLE_TIME_PROFILING
extern volatile uint32_t gtimer_kinetis_msticks;
#endif
/**
* @brief This parameter will avoid infinite loop of publish and exit the program after certain number of publishes
*/
uint32_t publishCount = 4;

/*******************************************************************************
* Config
******************************************************************************/

/*******************************************************************************
* Code
******************************************************************************/

void demoPubCallbackHandler(MQTTContext_t * pxMqttContext,
                              MQTTPacketInfo_t * pxPacketInfo,
                              MQTTDeserializedInfo_t * pxDeserializedInfo)
{
    /* Received a published message */
    HandleReceivedMsg((char *)pxDeserializedInfo->pPublishInfo->pPayload, pxDeserializedInfo->pPublishInfo->payloadLength);

}

int HandleReceivedMsg(char *sJsonString, uint16_t len)
{
    ledColor_t led = LED_INVALID;
    ledState_t state = LED_OFF;
    char* valuePtr;
    size_t valueSize = 0U;

    if(JSON_Search(sJsonString, len, "user", 4, &valuePtr, &valueSize) == JSONSuccess) {
        //printf("User %s\n", valuePtr);
    }

    if (JSON_Search(sJsonString, len, "red", 3, &valuePtr, &valueSize) == JSONSuccess){
        printf("LED Color: Red");
        led = RED;
    }
    else if(JSON_Search(sJsonString, len, "green", 5, &valuePtr, &valueSize) == JSONSuccess) {
        printf("LED Color: Green");
        led = GREEN;
    }
    else if(JSON_Search(sJsonString, len, "blue", 4, &valuePtr, &valueSize) == JSONSuccess) {
        printf("LED Color: Blue");
        led = BLUE;
    }
    else {
        //printf("Key not found\n");
    }

    if (strncmp(valuePtr, "on", valueSize) == 0) {
        printf("- state: LED ON\n");
        state = LED_ON;
    }
    else if (strncmp(valuePtr, "off", valueSize) == 0) {
        printf("- state: LED OFF\n");
        state = LED_OFF;
    }
    else if (strncmp(valuePtr, "toggle", valueSize) == 0) {
        printf("- state: LED TOGGLE\n");
        state = LED_TOGGLE;
    }
    else {
        led = LED_INVALID;
        //printf("Unexpected value: %.*s\n", valuePtr);
    }

    if (LED_INVALID != led) {
        led_handler(led, state);
    }

    return 0;
}

void gcpPubSub(void *ctx)
{
    int i = 0;
    BaseType_t xResult = pdFAIL;
    char cPayload[100];
    unsigned char b64token[JWT_PASSWORLD_BUFFER_LEN] = {0};
    size_t b64tokenLen = sizeof(b64token);
   char *gcpJWToken = (char *)&b64token[0];
    mbedtls_pk_context *pk = (mbedtls_pk_context *)ctx;
#if AX_EMBEDDED
    U8 AxUID[MODULE_UNIQUE_ID_LEN] = {0};
#endif

#if ENABLE_TIME_PROFILING
    volatile uint32_t previousTime, currentTime;
#endif

#if AX_EMBEDDED
    BOARD_InitNetwork_MAC(AxUID);
#endif
    // xResult = IotSdk_Init();
    // if (xResult != pdPASS) {
    //     LOG_E("IotSdk_Init() failed");
    //     LED_RED_ON();
    //     goto exit;
    // }

    // xResult = SOCKETS_Init();
    // if (xResult != pdPASS) {
    //     LOG_E("SOCKETS_Init failed");
    //     goto exit;
    // }

    /* Get current EPOCH for JWT */
    unsigned long utc_epoch = gcpNTPGetEpoch();

    if (0 == utc_epoch) {
        LED_RED_ON();
        LED_BLUE_ON();
        utc_epoch = gcpNTPGetEpoch();
        LED_BLUE_OFF();
    }
    if (0 == utc_epoch) {
        LED_RED_ON();
        LED_BLUE_ON();
        utc_epoch = gcpNTPGetEpoch();
        LED_BLUE_OFF();
    }
    if (0 == utc_epoch) {
        LED_RED_ON();
        xResult = pdFAIL;
        goto exit;
    }
    else {
        // The ratio of output bytes to input bytes is 4:3 (33% overhead). Specifically, given an input of n bytes, the output will be
        // 4*(n/3) bytes long, including padding characters.
        gcpCreateJWTTokenES256(b64token, &b64tokenLen, GCP_PROJECT_NAME, utc_epoch, 10 * 60 * 60, pk);
    }

    sprintf(cPayload, "{\"msg\" : \"%s : %d\"}", "hello from SDK QOS0", (int)i++);
    xResult = mqttDemoMain("NA", gcpJWToken, cPayload);

exit:
    if (xResult != pdPASS) {
        LOG_E("GCP demo failed");
    }
    else {
        LOG_I("GCP demo passed");
    }
    LOG_I("Demo Example Finished");
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}
