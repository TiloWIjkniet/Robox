/* Copyright 2019,2020 NXP
 * SPDX-License-Identifier: Apache-2.0
 */

/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
/* Only for quick testing.... Not for production use ................. */
/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */

/* TO BE Run only under default session */

#include <ex_sss.h>
#include <ex_sss_boot.h>
#include <fsl_sss_se05x_apis.h>
#include <nxLog_App.h>
#include <se05x_APDU.h>
#include <se05x_const.h>
#include <se05x_ecc_curves.h>
#include <se05x_ecc_curves_values.h>
#include <se05x_tlv.h>
#include <sm_types.h>
#include <string.h>

#include "ex_sss_auth.h"
#include "smCom.h"
#include "fsl_sss_se05x_policy.h"

extern void ReInitializeTestSample(ex_sss_boot_ctx_t *pCtx);
extern void AddAttestationKeys(ex_sss_boot_ctx_t *pCtx);
static void ReInitializeTestSampleInternal(ex_sss_boot_ctx_t *pCtx, SE05x_Result_t production_uid_present);

#define POLICY_BUF_LEN 9
#define POLICY_BUF_POLICY_LEN 8
#define POLICY_BUF_POLICY_OFFSET 5

/* clang-format off */
#define TEST_UID 0x04, 0x00, 0x50, 0x01, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x04, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFA

#define kSE05x_AppletResID_0x7FFF0203 0x7FFF0203

/* clang-format ON */

static ex_sss_boot_ctx_t gex_sss_delete_all_boot_ctx;

#define EX_SSS_BOOT_PCONTEXT (&gex_sss_delete_all_boot_ctx)
#define EX_SSS_BOOT_DO_ERASE 1
#define EX_SSS_BOOT_EXPOSE_ARGC_ARGV 0

/* ************************************************************************** */
/* Include "main()" with the platform specific startup code for Plug & Trust  */
/* MW examples which will call ex_sss_entry()                                 */
/* ************************************************************************** */
#include <ex_sss_main_inc.h>

sss_status_t ex_sss_entry(ex_sss_boot_ctx_t *pCtx)
{
    sss_status_t status = kStatus_SSS_Fail;
    sss_se05x_session_t *pSession =
        (sss_se05x_session_t *)&pCtx->session;
    SE05x_Result_t production_uid_present = kSE05x_Result_NA;
    smStatus_t sw_status;

    LOG_E("# se05x_Delete_and_test_provision !!! Only for testing. NOT FOR PRODUCTION USE!!!!");

    sw_status = Se05x_API_CheckObjectExists(
        &pSession->s_ctx, kSE05x_AppletResID_UNIQUE_ID, &production_uid_present);
    if (SM_OK == sw_status && production_uid_present == kSE05x_Result_SUCCESS) {
        uint8_t uid[SE050_MODULE_UNIQUE_ID_LEN] = { 0 };
        uint8_t const_uid[SE050_MODULE_UNIQUE_ID_LEN] = { TEST_UID };
        size_t uidLen = sizeof(uid);
        sw_status = Se05x_API_ReadObject(&pSession->s_ctx,
                                      kSE05x_AppletResID_UNIQUE_ID,
                                      0,
                                      (uint16_t)uidLen,
                                      uid,
                                      &uidLen);
        if (SM_OK == sw_status && 0 == memcmp(const_uid, uid, sizeof(const_uid))) {
            production_uid_present = kSE05x_Result_FAILURE;
        }
    }
    else
    {
        const uint8_t const_uid[SE050_MODULE_UNIQUE_ID_LEN] = { TEST_UID };
        size_t uidLen = sizeof(const_uid);
        LOG_W("kSE05x_AppletResID_UNIQUE_ID Missing. Injecting Dummy KEY!.");
        sw_status = Se05x_API_WriteBinary(&pSession->s_ctx,
            NULL,
            kSE05x_AppletResID_UNIQUE_ID,
            0,
            (uint16_t)uidLen,
            const_uid,
            uidLen);
        LOG_X16_I(sw_status);
        production_uid_present = kSE05x_Result_FAILURE;
    }

    sw_status = Se05x_API_DeleteAll_Iterative(&pSession->s_ctx);
    if (SM_OK != sw_status) {
        LOG_E("Failed Se05x_API_DeleteAll");
        goto cleanup;
    }

    AddAttestationKeys(pCtx);
    ReInitializeTestSample(pCtx);
    ReInitializeTestSampleInternal(pCtx, production_uid_present);

    if (sw_status == SW_OK) {
        status = kStatus_SSS_Success;
    }

cleanup:
    return status;
}

#define DO_SMCOM_SEND(CONN_CTX, CMD_APDU, RSP_BUFFER)               \
    {                                                               \
        rxLen = sizeof((RSP_BUFFER));                               \
        status = (smStatus_t)smCom_TransceiveRaw(CONN_CTX,          \
                                     (uint8_t *)(CMD_APDU),         \
                                     (uint16_t)sizeof((CMD_APDU)),  \
                                     RSP_BUFFER,                    \
                                     &rxLen);                       \
        if (status == SM_OK) {                                      \
            if (( rxLen == 2)                                       \
                && (RSP_BUFFER)[0] == 0x90                          \
                && (RSP_BUFFER)[1] == 0x00) {                       \
                LOG_I(#CMD_APDU " Written");                        \
            } else {                                                \
                LOG_W(#CMD_APDU " Failed");                         \
            }                                                       \
        }                                                           \
        else {                                                      \
            LOG_W(#CMD_APDU " Failed");                             \
        }                                                           \
    }

static void ReInitializeTestSampleInternal(ex_sss_boot_ctx_t *pCtx, SE05x_Result_t production_uid_present)
{
    uint8_t responseBuffer[10];
    U32 rxLen = sizeof(responseBuffer);
    smStatus_t status;
    SE05x_Result_t objExists = kSE05x_Result_NA;
    sss_se05x_session_t *pSession = (sss_se05x_session_t *)&pCtx->session;
    Se05xSession_t *pSe05xSession = &pSession->s_ctx;
    uint8_t hasSession_orig = pSe05xSession->hasSession;
    void* conn_ctx = pSession->s_ctx.conn_ctx;
    uint32_t ecdsa_policy = POLICY_OBJ_ALLOW_DELETE | POLICY_OBJ_ALLOW_READ;
    uint8_t ecdsa_policy_buf[POLICY_BUF_LEN] = {0};
    Se05xPolicy_t policy_for_auth_obj = {0};
    ecdsa_policy_buf[0] = POLICY_BUF_POLICY_LEN;
    ecdsa_policy_buf[POLICY_BUF_POLICY_OFFSET]   = (uint8_t)((ecdsa_policy & 0xFF000000) >> (8 * 3));
    ecdsa_policy_buf[POLICY_BUF_POLICY_OFFSET+1] = (uint8_t)((ecdsa_policy & 0x00FF0000) >> (8 * 2));
    ecdsa_policy_buf[POLICY_BUF_POLICY_OFFSET+2] = (uint8_t)((ecdsa_policy & 0x0000FF00) >> (8 * 1));
    ecdsa_policy_buf[POLICY_BUF_POLICY_OFFSET+3] = (uint8_t)((ecdsa_policy & 0x000000FF) >> (8 * 0));
    policy_for_auth_obj.value = ecdsa_policy_buf;
    policy_for_auth_obj.value_len = POLICY_BUF_LEN;

    // Transport Unlock
    /* clang-format off */
    const uint8_t cmd_transport_unlock[] = {
        0x80, 0x41, 0x03, 0x00, 0x37, 0x11, 0x09, 0x08, 0x7F, 0xFF, 0x02, 0x00, 0x00, 0x04, 0x00, 0x00,
        0x12, 0x02, 0x00, 0x0A, 0x41, 0x04, 0x7F, 0xFF, 0x02, 0x00, 0x43, 0x20, 0x40, 0x41, 0x42, 0x43,
        0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x40, 0x41, 0x42, 0x43,
        0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F };

    const uint8_t ECKey_SE_PublicEcdsakey[] = {
        0x04, 0x3C, 0x9E, 0x47, 0xED, 0xF0, 0x51, 0xA3,
        0x58, 0x9F, 0x67, 0x30, 0x2D, 0x22, 0x56, 0x7C,
        0x2E, 0x17, 0x22, 0x9E, 0x88, 0x83, 0x33, 0x8E,
        0xC3, 0xB7, 0xD5, 0x27, 0xF9, 0xEE, 0x71, 0xD0,
        0xA8, 0x1A, 0xAE, 0x7F, 0xE2, 0x1C, 0xAA, 0x66,
        0x77, 0x78, 0x3A, 0xA8, 0x8D, 0xA6, 0xD6, 0xA8,
        0xAD, 0x5E, 0xC5, 0x3B, 0x10, 0xBC, 0x0B, 0x11,
        0x09, 0x44, 0x82, 0xF0, 0x4D, 0x24, 0xB5, 0xBE,
        0xC4 };
    uint16_t attempt    = SE05x_MaxAttemps_UNLIMITED;
    /* Store FAST SCP Auth ECKA keypair1 to SE */
    /* clang-format off */
    const uint8_t ECKey_SE_ECKAPk_key1[] = {
        0x53, 0xD3, 0xB8, 0x41, 0x9C, 0x81, 0xE8, 0xAF, 0x21, 0xF4, 0x33, 0xF9,
        0x7D, 0xFF, 0xE1, 0x2D, 0x5C, 0xC1, 0xF1, 0x69, 0xC4, 0x71, 0x40, 0x7E, 0x52, 0x56, 0x13, 0xFD,
        0x38, 0xEE, 0xDF, 0x3A };

    const uint8_t ECKey_SE_ECKAPb_key1[] = {
        0x04, 0x3E, 0x50, 0xEB, 0x06, 0x07, 0x43, 0x23, 0xB6, 0x6A,
        0x94, 0x00, 0x65, 0x98, 0x1E, 0xF9, 0x49, 0x7E, 0x88, 0x20, 0x33, 0x9F, 0x97, 0x84, 0x77, 0x00,
        0xA1, 0x25, 0xEB, 0x65, 0x10, 0x42, 0x51, 0x70, 0x2E, 0x97, 0x1E, 0x12, 0x84, 0xBE, 0x12, 0x1D,
        0xBD, 0x60, 0x60, 0x00, 0x20, 0xB3, 0x1E, 0xB3, 0x24, 0x94, 0xAB, 0xFB, 0xB9, 0x46, 0x60, 0x61,
        0xA4, 0x9E, 0x07, 0xA6, 0x20, 0x30, 0xB6 };


    const uint8_t ECKey_SE_ECKAPk_key2[] = {
        0x09, 0x35, 0x64, 0x55, 0xA3, 0x21, 0x5B, 0x23, 0x67, 0xA7, 0xDC, 0x74,
        0x7D, 0xAA, 0xC0, 0x28, 0x07, 0xD6, 0xE6, 0x05, 0x78, 0xB6, 0x85, 0x51, 0x6F, 0xB9, 0xB0, 0xD7,
        0xE1, 0x6A, 0x48, 0x5A, };

    const uint8_t ECKey_SE_ECKAPb_key2[] = {
        0x04, 0x4F, 0x86, 0x68, 0x71, 0x35, 0x7F, 0x9A, 0xCA, 0xA9,
        0x55, 0x4E, 0x1C, 0xA0, 0xA5, 0x8A, 0x39, 0x82, 0xA2, 0x02, 0x6C, 0xBC, 0xBB, 0x58, 0xD0, 0x4F,
        0x51, 0x61, 0x63, 0x4D, 0x21, 0xCE, 0xF7, 0x22, 0x68, 0xE2, 0xF4, 0xF0, 0x9C, 0x62, 0x5F, 0x54,
        0xE5, 0x64, 0x16, 0x23, 0xA4, 0xCD, 0xC9, 0xE9, 0x86, 0xAD, 0x8B, 0xFF, 0xBB, 0x8B, 0xE6, 0xF0,
        0x93, 0xE8, 0x15, 0x12, 0x16, 0x6B, 0x88 };

    const uint8_t ECKey_SE_ECKAPk_key3[] = {
        0x09, 0x35, 0x64, 0x55, 0xA3, 0x21, 0x5B, 0x23, 0x67, 0xA7, 0xDC, 0x74,
        0x7D, 0xAA, 0xC0, 0x28, 0x07, 0xD6, 0xE6, 0x05, 0x78, 0xB6, 0x85, 0x51, 0x6F, 0xB9, 0xB0, 0xD7,
        0xE1, 0x6A, 0x48, 0x5A, };

    const uint8_t ECKey_SE_ECKAPb_key3[] = {
        0x04, 0x4F, 0x86, 0x68, 0x71, 0x35, 0x7F, 0x9A, 0xCA, 0xA9,
        0x55, 0x4E, 0x1C, 0xA0, 0xA5, 0x8A, 0x39, 0x82, 0xA2, 0x02, 0x6C, 0xBC, 0xBB, 0x58, 0xD0, 0x4F,
        0x51, 0x61, 0x63, 0x4D, 0x21, 0xCE, 0xF7, 0x22, 0x68, 0xE2, 0xF4, 0xF0, 0x9C, 0x62, 0x5F, 0x54,
        0xE5, 0x64, 0x16, 0x23, 0xA4, 0xCD, 0xC9, 0xE9, 0x86, 0xAD, 0x8B, 0xFF, 0xBB, 0x8B, 0xE6, 0xF0,
        0x93, 0xE8, 0x15, 0x12, 0x16, 0x6B, 0x88 };

    /* clang-format on */

    if (kSE05x_Result_SUCCESS == production_uid_present) {
        LOG_I("Production UID Found... skipping few steps");
    }
    else {
        status = Se05x_API_CheckObjectExists(pSe05xSession, kSE05x_AppletResID_FEATURE, &objExists);
        if (status == SM_OK && objExists == kSE05x_Result_SUCCESS) {
            LOG_W("kSE05x_AppletResID_FEATURE Object already exists");
        }
        else if (status == SM_OK && objExists == kSE05x_Result_FAILURE) {
            status = Se05x_API_WriteECKey(pSe05xSession,
                (Se05xPolicy_t *)&policy_for_auth_obj,
                attempt,
                kSE05x_AppletResID_FEATURE,
                kSE05x_ECCurve_NIST_P256,
                NULL,
                0,
                ECKey_SE_PublicEcdsakey,
                sizeof(ECKey_SE_PublicEcdsakey),
                (SE05x_INS_t)kSE05x_AttestationType_AUTH,
                kSE05x_KeyPart_Public);
            LOG_I("kSE05x_ECCurve_NIST_P256 status = %04X", status);
        }

        status = Se05x_API_CheckObjectExists(pSe05xSession, kSE05x_AppletResID_KP_ECKEY_USER, &objExists);
        if (status == SM_OK && objExists == kSE05x_Result_SUCCESS) {
            LOG_W("kSE05x_AppletResID_KP_ECKEY_USER Object already exists");
        }
        else if (status == SM_OK && objExists == kSE05x_Result_FAILURE) {
            status = Se05x_API_WriteECKey(pSe05xSession,
                (Se05xPolicy_t *)&policy_for_auth_obj,
                attempt,
                kSE05x_AppletResID_KP_ECKEY_USER,
                kSE05x_ECCurve_NIST_P256,
                ECKey_SE_ECKAPk_key1,
                sizeof(ECKey_SE_ECKAPk_key1),
                ECKey_SE_ECKAPb_key1,
                sizeof(ECKey_SE_ECKAPb_key1),
                (SE05x_INS_t)kSE05x_AttestationType_AUTH,
                kSE05x_KeyPart_Pair);
            LOG_I("kSE05x_AppletResID_KP_ECKEY_USER status = %04X", status);
        }
        status = Se05x_API_CheckObjectExists(pSe05xSession, kSE05x_AppletResID_KP_ECKEY_IMPORT, &objExists);
        if (status == SM_OK && objExists == kSE05x_Result_SUCCESS) {
            LOG_W("kSE05x_AppletResID_KP_ECKEY_IMPORT Object already exists");
        }
        else if (status == SM_OK && objExists == kSE05x_Result_FAILURE) {
            /* Store FAST SCP Auth ECKA keypair2 to SE */
            status = Se05x_API_WriteECKey(pSe05xSession,
                (Se05xPolicy_t *)&policy_for_auth_obj,
                attempt,
                kSE05x_AppletResID_KP_ECKEY_IMPORT,
                kSE05x_ECCurve_NIST_P256,
                ECKey_SE_ECKAPk_key2,
                sizeof(ECKey_SE_ECKAPk_key2),
                ECKey_SE_ECKAPb_key2,
                sizeof(ECKey_SE_ECKAPb_key2),
                (SE05x_INS_t)kSE05x_AttestationType_AUTH,
                kSE05x_KeyPart_Pair);
            LOG_I("kSE05x_AppletResID_KP_ECKEY_IMPORT status = %04X", status);
        }
        status = Se05x_API_CheckObjectExists(pSe05xSession, kSE05x_AppletResID_0x7FFF0203, &objExists);
        if (status == SM_OK && objExists == kSE05x_Result_SUCCESS) {
            LOG_W("kSE05x_AppletResID Object already exists");
        }
        else if (status == SM_OK && objExists == kSE05x_Result_FAILURE) {
            /* Store FAST SCP Auth ECKA keypair3 to SE */
            status = Se05x_API_WriteECKey(pSe05xSession,
                (Se05xPolicy_t *)&policy_for_auth_obj,
                attempt,
                kSE05x_AppletResID_0x7FFF0203,
                kSE05x_ECCurve_NIST_P256,
                ECKey_SE_ECKAPk_key3,
                sizeof(ECKey_SE_ECKAPk_key3),
                ECKey_SE_ECKAPb_key3,
                sizeof(ECKey_SE_ECKAPb_key3),
                (SE05x_INS_t)kSE05x_AttestationType_AUTH,
                kSE05x_KeyPart_Pair);
            LOG_I("kSE05x_AppletResID_0x7FFF0203 status = %04X", status);
        }
    }

    if (kSE05x_Result_SUCCESS == production_uid_present) {
        LOG_I("Production UID Found... skipping few steps");
    }
    else {
        status = Se05x_API_CheckObjectExists(pSe05xSession, kSE05x_AppletResID_TRANSPORT, &objExists);
        if (status == SM_OK && objExists == kSE05x_Result_SUCCESS) {
            LOG_W("Transport Object already exists");
        }
        else if (status == SM_OK && objExists == kSE05x_Result_FAILURE) {
            DO_SMCOM_SEND(conn_ctx, cmd_transport_unlock, responseBuffer);
        }
    }
    pSe05xSession->hasSession = hasSession_orig;
}
