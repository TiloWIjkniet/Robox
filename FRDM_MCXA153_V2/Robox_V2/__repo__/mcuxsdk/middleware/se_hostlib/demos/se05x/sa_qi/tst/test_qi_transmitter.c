/*
 * Copyright 2022 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* ************************************************************************** */
/* Includes                                                                   */
/* ************************************************************************** */

#include <sss_tst.h>
#include <string.h>
#include <se05x_APDU_apis.h>
#include <test_qi_transmitter.h>
#include <sa_qi_transmitter.h>
#include <sa_qi_transmitter_helpers.h>
#include <nxLog_App.h>
#include <limits.h>

/* ************************************************************************** */
/* Local Defines                                                              */
/* ************************************************************************** */

/* ************************************************************************** */
/* Structures and Typedefs                                                    */
/* ************************************************************************** */

/* ************************************************************************** */
/* Global Variables                                                           */
/* ************************************************************************** */

pSe05xSession_t pgSe05xSessionctx = &(((sss_se05x_session_t *)&gtCtx.session)->s_ctx);

/* ************************************************************************** */
/* Static function declarations                                               */
/* ************************************************************************** */

static void runAllTests(void);

/* ************************************************************************** */
/* Public Functions                                                           */
/* ************************************************************************** */

static sss_status_t add_qi_credential(sss_object_t *pKeyObject,
    sss_key_store_t *pKs,
    sss_key_part_t keyPart,
    sss_cipher_type_t cipherType,
    const uint8_t *buff,
    size_t buff_size,
    size_t keyBitLen,
    uint32_t keyId)
{
    sss_status_t status = kStatus_SSS_Success;

    status = sss_key_object_init(pKeyObject, pKs);
    TEST_ASSERT_EQUAL_HEX32_MESSAGE(kStatus_SSS_Success, status, "sss_key_object_init failed");

    status =
        sss_key_object_allocate_handle(pKeyObject, keyId, keyPart, cipherType, buff_size, kKeyObject_Mode_Persistent);
    TEST_ASSERT_EQUAL_HEX32_MESSAGE(kStatus_SSS_Success, status, "sss_key_object_allocate_handle failed");

    status = sss_key_store_set_key(pKs, pKeyObject, buff, buff_size, keyBitLen, NULL, 0);
    TEST_ASSERT_EQUAL_HEX32_MESSAGE(kStatus_SSS_Success, status, "sss_key_store_set_key failed");

    return status;
}

void provisionCredentials(uint8_t slot_id, sss_object_t *pObject)
{
    sss_status_t status = kStatus_SSS_Success;
    uint32_t qi_key_id  = QI_SLOT_ID_TO_KEY_ID(slot_id);
    uint32_t qi_cert_id = QI_SLOT_ID_TO_CERT_ID(slot_id);

    status = add_qi_credential(pObject,
        &gtCtx.ks,
        kSSS_KeyPart_Pair,
        kSSS_CipherType_EC_NIST_P,
        qi_ec_priv_key,
        qi_ec_priv_key_len,
        256,
        qi_key_id);
    TEST_ASSERT_EQUAL_HEX32_MESSAGE(kStatus_SSS_Success, status, "Failed to store Qi private key");

    if (qi_certificate_chain_len > (SIZE_MAX / 8)) {
        LOG_E("Performing unsigned integer multiplication will overflow");
    }
    status = add_qi_credential(pObject,
        &gtCtx.ks,
        kSSS_KeyPart_Default,
        kSSS_CipherType_Binary,
        qi_certificate_chain,
        qi_certificate_chain_len,
        qi_certificate_chain_len * 8,
        qi_cert_id);
    TEST_ASSERT_EQUAL_HEX32_MESSAGE(kStatus_SSS_Success, status, "Failed to store Qi certificate chain");
}

TEST_GROUP(QiTransmitter);

TEST_SETUP(QiTransmitter)
{
    DoFixtureSetupIfNeeded(true);
    DoFactoryReset();
    DoCommonSetUp();
    provisionCredentials(0, &gtCtx.key);
}

TEST_TEAR_DOWN(QiTransmitter)
{
    DoCommonTearDown();
}

TEST(QiTransmitter, GetDigest)
{
    uint8_t cmd_buffer[MAX_CMD_SIZE_GET_DIGESTS]      = {0};
    size_t cmd_size                                   = sizeof(cmd_buffer);
    uint8_t response_buffer[MAX_RSP_SIZE_GET_DIGESTS] = {0};
    size_t response_size                              = sizeof(response_buffer);
    uint8_t slot_id_mask                              = SLOT_ID_MASK_ALL_SLOTS;

    cmd_buffer[0] = (AUTH_PROTOCOL_VERSION << 4) | kQiCommandGetDigests;
    cmd_buffer[1] = 0x00 | (slot_id_mask);
    /* Send GET_DIGESTS command */
    powerTransmitterSendCommand(cmd_buffer, cmd_size, response_buffer, &response_size);
    TEST_ASSERT_GREATER_THAN_MESSAGE(3, response_size, "GET_DIGESTS failed");
}

TEST(QiTransmitter, GetDigestInvalidAuthProtocol)
{
    uint8_t cmd_buffer[MAX_CMD_SIZE_GET_DIGESTS]      = {0};
    size_t cmd_size                                   = sizeof(cmd_buffer);
    uint8_t response_buffer[MAX_RSP_SIZE_GET_DIGESTS] = {0};
    size_t response_size                              = sizeof(response_buffer);
    uint8_t slot_id_mask                              = SLOT_ID_MASK_ALL_SLOTS;

    cmd_buffer[0] = (AUTH_PROTOCOL_VERSION << 4) | kQiCommandGetDigests;
    cmd_buffer[1] = 0x00 | (slot_id_mask);
    cmd_buffer[0] = 0x20;

    /* Send GET_DIGESTS command */
    powerTransmitterSendCommand(cmd_buffer, cmd_size, response_buffer, &response_size);
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(response_buffer[0],
        ((AUTH_PROTOCOL_VERSION << 4) | kQiResponseError),
        "Authentication message header mismatch");
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(response_buffer[1], kQiErrorUnsupportedProtocol, "Error Code mismatch");
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(response_buffer[2], AUTH_PROTOCOL_VERSION, "Error data mismatch");
}

TEST(QiTransmitter, GetDigestInvalidMessageType)
{
    uint8_t cmd_buffer[MAX_CMD_SIZE_GET_DIGESTS]      = {0};
    size_t cmd_size                                   = sizeof(cmd_buffer);
    uint8_t response_buffer[MAX_RSP_SIZE_GET_DIGESTS] = {0};
    size_t response_size                              = sizeof(response_buffer);
    uint8_t slot_id_mask                              = SLOT_ID_MASK_ALL_SLOTS;

    cmd_buffer[0] = (AUTH_PROTOCOL_VERSION << 4) | kQiCommandGetDigests;
    cmd_buffer[1] = 0x00 | (slot_id_mask);
    cmd_buffer[0] = (AUTH_PROTOCOL_VERSION << 4) | kQiResponseDigest;

    /* Send GET_DIGESTS command */
    powerTransmitterSendCommand(cmd_buffer, cmd_size, response_buffer, &response_size);
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(response_buffer[0],
        ((AUTH_PROTOCOL_VERSION << 4) | kQiResponseError),
        "Authentication message header mismatch");
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(response_buffer[1], kQiErrorInvalidRequest, "Error Code mismatch");
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(response_buffer[2], 0x00, "Error data mismatch");
}

TEST(QiTransmitter, GetDigestInvalidRequest)
{
    uint8_t cmd_buffer[MAX_CMD_SIZE_GET_DIGESTS]      = {0};
    size_t cmd_size                                   = sizeof(cmd_buffer);
    uint8_t response_buffer[MAX_RSP_SIZE_GET_DIGESTS] = {0};
    size_t response_size                              = sizeof(response_buffer);
    uint8_t slot_id_mask                              = SLOT_ID_MASK_NO_SLOTS;

    cmd_buffer[0] = (AUTH_PROTOCOL_VERSION << 4) | kQiCommandGetDigests;
    cmd_buffer[1] = 0x00 | (slot_id_mask);

    /* Send GET_DIGESTS command */
    powerTransmitterSendCommand(cmd_buffer, cmd_size, response_buffer, &response_size);
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(response_buffer[0],
        ((AUTH_PROTOCOL_VERSION << 4) | kQiResponseError),
        "Authentication message header mismatch");
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(response_buffer[1], kQiErrorInvalidRequest, "Error Code mismatch");
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(response_buffer[2], 0x00, "Error data mismatch");
}

TEST(QiTransmitter, GetDigestInvalidSlotMask)
{
    uint8_t cmd_buffer[MAX_CMD_SIZE_GET_DIGESTS]      = {0};
    size_t cmd_size                                   = sizeof(cmd_buffer);
    uint8_t response_buffer[MAX_RSP_SIZE_GET_DIGESTS] = {0};
    size_t response_size                              = sizeof(response_buffer);
    uint8_t slot_id_mask                              = SLOT_ID_MASK_SLOT_1;

    cmd_buffer[0] = (AUTH_PROTOCOL_VERSION << 4) | kQiCommandGetDigests;
    cmd_buffer[1] = 0x00 | (slot_id_mask);

    /* Send GET_DIGESTS command */
    powerTransmitterSendCommand(cmd_buffer, cmd_size, response_buffer, &response_size);
    TEST_ASSERT_EQUAL_MESSAGE(2, response_size, "GET_DIGESTS failed");
}

TEST(QiTransmitter, GetDigestNullCmdBuffer)
{
    size_t cmd_size                                   = MAX_CMD_SIZE_GET_DIGESTS;
    uint8_t response_buffer[MAX_RSP_SIZE_GET_DIGESTS] = {0};
    size_t response_size                              = sizeof(response_buffer);

    /* Send GET_DIGESTS command */
    powerTransmitterSendCommand(NULL, cmd_size, response_buffer, &response_size);
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(response_buffer[0],
        ((AUTH_PROTOCOL_VERSION << 4) | kQiResponseError),
        "Authentication message header mismatch");
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(response_buffer[1], kQiErrorInvalidRequest, "Error Code mismatch");
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(response_buffer[2], 0x00, "Error data mismatch");

    powerTransmitterSendCommand(NULL, 0, response_buffer, &response_size);
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(response_buffer[0],
        ((AUTH_PROTOCOL_VERSION << 4) | kQiResponseError),
        "Authentication message header mismatch");
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(response_buffer[1], kQiErrorInvalidRequest, "Error Code mismatch");
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(response_buffer[2], 0x00, "Error data mismatch");
}

TEST(QiTransmitter, GetDigestNullRspBuffer)
{
    uint8_t cmd_buffer[MAX_CMD_SIZE_GET_DIGESTS] = {0};
    size_t cmd_size                              = sizeof(cmd_buffer);
    size_t response_size                         = MAX_RSP_SIZE_GET_DIGESTS;
    uint8_t slot_id_mask                         = SLOT_ID_MASK_ALL_SLOTS;

    cmd_buffer[0] = (AUTH_PROTOCOL_VERSION << 4) | kQiCommandGetDigests;
    cmd_buffer[1] = 0x00 | (slot_id_mask);

    /* Send GET_DIGESTS command */
    powerTransmitterSendCommand(cmd_buffer, cmd_size, NULL, &response_size);

    response_size = 0;
    powerTransmitterSendCommand(cmd_buffer, cmd_size, NULL, &response_size);
}

TEST(QiTransmitter, GetDigestInvalidCmdLength)
{
    uint8_t cmd_buffer[MAX_CMD_SIZE_GET_DIGESTS]      = {0};
    size_t cmd_size                                   = sizeof(cmd_buffer);
    uint8_t response_buffer[MAX_RSP_SIZE_GET_DIGESTS] = {0};
    size_t response_size                              = sizeof(response_buffer);
    uint8_t slot_id_mask                              = SLOT_ID_MASK_ALL_SLOTS;

    cmd_buffer[0] = (AUTH_PROTOCOL_VERSION << 4) | kQiCommandGetDigests;
    cmd_buffer[1] = 0x00 | (slot_id_mask);
    /* Send GET_DIGESTS command */
    powerTransmitterSendCommand(cmd_buffer, cmd_size - 1, response_buffer, &response_size);
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(response_buffer[0],
        ((AUTH_PROTOCOL_VERSION << 4) | kQiResponseError),
        "Authentication message header mismatch");
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(response_buffer[1], kQiErrorInvalidRequest, "Error Code mismatch");
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(response_buffer[2], 0x00, "Error data mismatch");

    powerTransmitterSendCommand(cmd_buffer, cmd_size + 1, response_buffer, &response_size);
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(response_buffer[0],
        ((AUTH_PROTOCOL_VERSION << 4) | kQiResponseError),
        "Authentication message header mismatch");
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(response_buffer[1], kQiErrorInvalidRequest, "Error Code mismatch");
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(response_buffer[2], 0x00, "Error data mismatch");
}

TEST(QiTransmitter, GetDigestInvalidRspLength)
{
    uint8_t cmd_buffer[MAX_CMD_SIZE_GET_DIGESTS]      = {0};
    size_t cmd_size                                   = sizeof(cmd_buffer);
    uint8_t response_buffer[MAX_RSP_SIZE_GET_DIGESTS] = {0};
    size_t response_size                              = 2;
    uint8_t slot_id_mask                              = SLOT_ID_MASK_ALL_SLOTS;

    cmd_buffer[0] = (AUTH_PROTOCOL_VERSION << 4) | kQiCommandGetDigests;
    cmd_buffer[1] = 0x00 | (slot_id_mask);
    /* Send GET_DIGESTS command */
    powerTransmitterSendCommand(cmd_buffer, cmd_size, response_buffer, &response_size);
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(response_buffer[0],
        ((AUTH_PROTOCOL_VERSION << 4) | kQiResponseError),
        "Authentication message header mismatch");
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(response_buffer[1], kQiErrorUnspecified, "Error Code mismatch");
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(response_buffer[2], 0x00, "Error data mismatch");
}

TEST(QiTransmitter, ReadCertificate)
{
    uint8_t cmd_buffer[MAX_CMD_SIZE_GET_CERTIFICATE] = {0};
    size_t cmd_size                                  = sizeof(cmd_buffer);
    uint8_t response_buffer[4096]                    = {0};
    size_t response_size                             = sizeof(response_buffer);
    uint16_t offset                                  = 0;
    uint16_t length                                  = 0;
    uint8_t slot_id                                  = 0x00;

    cmd_buffer[0] = (AUTH_PROTOCOL_VERSION << 4) | kQiCommandGetCertificate;
    cmd_buffer[1] = (uint8_t)((offset & 0x0700) >> 3) | (uint8_t)((length & 0x0700) >> 6) | slot_id;
    cmd_buffer[2] = (uint8_t)(offset & 0x00FF);
    cmd_buffer[3] = (uint8_t)(length & 0x00FF);

    /* Send Read_Certificate command */
    powerTransmitterSendCommand(cmd_buffer, cmd_size, response_buffer, &response_size);
    TEST_ASSERT_GREATER_THAN_MESSAGE(3, response_size, "Read_Certificate failed");

    length        = 10;
    cmd_buffer[1] = (uint8_t)((offset & 0x0700) >> 3) | (uint8_t)((length & 0x0700) >> 6) | slot_id;
    cmd_buffer[3] = (uint8_t)(length & 0x00FF);

    /* Send Read_Certificate command */
    powerTransmitterSendCommand(cmd_buffer, cmd_size, response_buffer, &response_size);
    TEST_ASSERT_EQUAL_HEX32_MESSAGE(length + 1, response_size, "Invalid response size");
}

TEST(QiTransmitter, ReadCertificateOffsetx600)
{
    uint8_t cmd_buffer[MAX_CMD_SIZE_GET_CERTIFICATE] = {0};
    size_t cmd_size                                  = sizeof(cmd_buffer);
    uint8_t response_buffer[4096]                    = {0};
    size_t response_size                             = sizeof(response_buffer);
    uint16_t offset                                  = 0x600;
    uint16_t length                                  = 0;
    uint8_t slot_id                                  = 0x00;

    cmd_buffer[0] = (AUTH_PROTOCOL_VERSION << 4) | kQiCommandGetCertificate;
    cmd_buffer[1] = (uint8_t)((offset & 0x0700) >> 3) | (uint8_t)((length & 0x0700) >> 6) | slot_id;
    cmd_buffer[2] = (uint8_t)(offset & 0x00FF);
    cmd_buffer[3] = (uint8_t)(length & 0x00FF);

    /* Send Read_Certificate command */
    powerTransmitterSendCommand(cmd_buffer, cmd_size, response_buffer, &response_size);
    TEST_ASSERT_GREATER_THAN_MESSAGE(3, response_size, "Read_Certificate failed");
}

TEST(QiTransmitter, ReadCertificateInvalidMessageType)
{
    uint8_t cmd_buffer[MAX_CMD_SIZE_GET_CERTIFICATE] = {0};
    size_t cmd_size                                  = sizeof(cmd_buffer);
    uint8_t response_buffer[4096]                    = {0};
    size_t response_size                             = sizeof(response_buffer);

    cmd_buffer[0] = (AUTH_PROTOCOL_VERSION << 4) | kQiResponseDigest;

    /* Send Read_Certificate command */
    powerTransmitterSendCommand(cmd_buffer, cmd_size, response_buffer, &response_size);
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(response_buffer[0],
        ((AUTH_PROTOCOL_VERSION << 4) | kQiResponseError),
        "Authentication message header mismatch");
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(response_buffer[1], kQiErrorInvalidRequest, "Error Code mismatch");
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(response_buffer[2], 0x00, "Error data mismatch");
}

TEST(QiTransmitter, ReadCertificateInvalidSlotId)
{
    uint8_t cmd_buffer[MAX_CMD_SIZE_GET_CERTIFICATE] = {0};
    size_t cmd_size                                  = sizeof(cmd_buffer);
    uint8_t response_buffer[4096]                    = {0};
    size_t response_size                             = sizeof(response_buffer);
    uint16_t offset                                  = CMD_GET_CERTIFICATE_MAX_OFFSET;
    uint16_t length                                  = 0;
    uint8_t slot_id                                  = 0x01;

    cmd_buffer[0] = (AUTH_PROTOCOL_VERSION << 4) | kQiCommandGetCertificate;
    cmd_buffer[1] = (uint8_t)((offset & 0x0700) >> 3) | (uint8_t)((length & 0x0700) >> 6) | slot_id;
    cmd_buffer[2] = (uint8_t)(offset & 0x00FF);
    cmd_buffer[3] = (uint8_t)(length & 0x00FF);

    /* Send Read_Certificate command */
    powerTransmitterSendCommand(cmd_buffer, cmd_size, response_buffer, &response_size);
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(response_buffer[0],
        ((AUTH_PROTOCOL_VERSION << 4) | kQiResponseError),
        "Authentication message header mismatch");
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(response_buffer[1], kQiErrorUnspecified, "Error Code mismatch");
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(response_buffer[2], 0x00, "Error data mismatch");
}

TEST(QiTransmitter, ReadCertificateInvalidOffset)
{
    uint8_t cmd_buffer[MAX_CMD_SIZE_GET_CERTIFICATE] = {0};
    size_t cmd_size                                  = sizeof(cmd_buffer);
    uint8_t response_buffer[4096]                    = {0};
    size_t response_size                             = sizeof(response_buffer);
    uint16_t offset                                  = CMD_GET_CERTIFICATE_MAX_OFFSET;
    uint16_t length                                  = 0;
    uint8_t slot_id                                  = 0x00;

    cmd_buffer[0] = (AUTH_PROTOCOL_VERSION << 4) | kQiCommandGetCertificate;
    cmd_buffer[1] = (uint8_t)((offset & 0x0700) >> 3) | (uint8_t)((length & 0x0700) >> 6) | slot_id;
    cmd_buffer[2] = (uint8_t)(offset & 0x00FF);
    cmd_buffer[3] = (uint8_t)(length & 0x00FF);

    /* Send Read_Certificate command */
    powerTransmitterSendCommand(cmd_buffer, cmd_size, response_buffer, &response_size);
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(response_buffer[0],
        ((AUTH_PROTOCOL_VERSION << 4) | kQiResponseError),
        "Authentication message header mismatch");
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(response_buffer[1], kQiErrorInvalidRequest, "Error Code mismatch");
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(response_buffer[2], 0x00, "Error data mismatch");
}

TEST(QiTransmitter, ReadCertificateInvalidLength)
{
    uint8_t cmd_buffer[MAX_CMD_SIZE_GET_CERTIFICATE] = {0};
    size_t cmd_size                                  = sizeof(cmd_buffer);
    uint8_t response_buffer[4096]                    = {0};
    size_t response_size                             = sizeof(response_buffer);
    uint16_t offset                                  = 0;
    uint16_t length                                  = CMD_GET_CERTIFICATE_MAX_LENGTH;
    uint8_t slot_id                                  = 0x00;

    cmd_buffer[0] = (AUTH_PROTOCOL_VERSION << 4) | kQiCommandGetCertificate;
    cmd_buffer[1] = (uint8_t)((offset & 0x0700) >> 3) | (uint8_t)((length & 0x0700) >> 6) | slot_id;
    cmd_buffer[2] = (uint8_t)(offset & 0x00FF);
    cmd_buffer[3] = (uint8_t)(length & 0x00FF);

    /* Send Read_Certificate command */
    powerTransmitterSendCommand(cmd_buffer, cmd_size, response_buffer, &response_size);
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(response_buffer[0],
        ((AUTH_PROTOCOL_VERSION << 4) | kQiResponseError),
        "Authentication message header mismatch");
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(response_buffer[1], kQiErrorInvalidRequest, "Error Code mismatch");
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(response_buffer[2], 0x00, "Error data mismatch");
}

TEST(QiTransmitter, ReadCertificateOffsetEqualToLength)
{
    pSe05xSession_t session_ctx                      = pgSe05xSessionctx;
    uint8_t cmd_buffer[MAX_CMD_SIZE_GET_CERTIFICATE] = {0};
    size_t cmd_size                                  = sizeof(cmd_buffer);
    uint8_t response_buffer[4096]                    = {0};
    size_t response_size                             = sizeof(response_buffer);
    uint16_t offset                                  = 0;
    uint16_t length                                  = 0;
    uint8_t slot_id                                  = 0x00;
    uint32_t certChainId                             = QI_SLOT_ID_TO_CERT_ID(slot_id);
    uint16_t objectSize                              = 0;

    smStatus_t retStatus = Se05x_API_ReadSize(session_ctx, certChainId, &objectSize);
    TEST_ASSERT_EQUAL_HEX32_MESSAGE(SM_OK, retStatus, "Se05x_API_ReadSize failed");

    offset = objectSize - DIGEST_SIZE_BYTES;

    cmd_buffer[0] = (AUTH_PROTOCOL_VERSION << 4) | kQiCommandGetCertificate;
    cmd_buffer[1] = (uint8_t)((offset & 0x0700) >> 3) | (uint8_t)((length & 0x0700) >> 6) | slot_id;
    cmd_buffer[2] = (uint8_t)(offset & 0x00FF);
    cmd_buffer[3] = (uint8_t)(length & 0x00FF);

    /* Send Read_Certificate command */
    powerTransmitterSendCommand(cmd_buffer, cmd_size, response_buffer, &response_size);
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(response_buffer[0],
        ((AUTH_PROTOCOL_VERSION << 4) | kQiResponseError),
        "Authentication message header mismatch");
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(response_buffer[1], kQiErrorInvalidRequest, "Error Code mismatch");
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(response_buffer[2], 0x00, "Error data mismatch");
}

TEST(QiTransmitter, ReadCertificateNullCmdBuffer)
{
    size_t cmd_size               = MAX_CMD_SIZE_GET_CERTIFICATE;
    uint8_t response_buffer[4096] = {0};
    size_t response_size          = sizeof(response_buffer);

    /* Send GET_DIGESTS command */
    powerTransmitterSendCommand(NULL, cmd_size, response_buffer, &response_size);
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(response_buffer[0],
        ((AUTH_PROTOCOL_VERSION << 4) | kQiResponseError),
        "Authentication message header mismatch");
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(response_buffer[1], kQiErrorInvalidRequest, "Error Code mismatch");
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(response_buffer[2], 0x00, "Error data mismatch");

    powerTransmitterSendCommand(NULL, 0, response_buffer, &response_size);
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(response_buffer[0],
        ((AUTH_PROTOCOL_VERSION << 4) | kQiResponseError),
        "Authentication message header mismatch");
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(response_buffer[1], kQiErrorInvalidRequest, "Error Code mismatch");
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(response_buffer[2], 0x00, "Error data mismatch");
}

TEST(QiTransmitter, ReadCertificateNullRspBuffer)
{
    uint8_t cmd_buffer[MAX_CMD_SIZE_GET_CERTIFICATE] = {0};
    size_t cmd_size                                  = sizeof(cmd_buffer);
    size_t response_size                             = 4096;
    uint16_t offset                                  = 0;
    uint16_t length                                  = 0;
    uint8_t slot_id                                  = 0x00;

    cmd_buffer[0] = (AUTH_PROTOCOL_VERSION << 4) | kQiCommandGetCertificate;
    cmd_buffer[1] = (uint8_t)((offset & 0x0700) >> 3) | (uint8_t)((length & 0x0700) >> 6) | slot_id;
    cmd_buffer[2] = (uint8_t)(offset & 0x00FF);
    cmd_buffer[3] = (uint8_t)(length & 0x00FF);

    /* Send GET_DIGESTS command */
    powerTransmitterSendCommand(cmd_buffer, cmd_size, NULL, &response_size);

    response_size = 0;
    powerTransmitterSendCommand(cmd_buffer, cmd_size, NULL, &response_size);
}

TEST(QiTransmitter, ReadCertificateInvalidCmdLength)
{
    uint8_t cmd_buffer[MAX_CMD_SIZE_GET_CERTIFICATE] = {0};
    size_t cmd_size                                  = sizeof(cmd_buffer);
    uint8_t response_buffer[4096]                    = {0};
    size_t response_size                             = sizeof(response_buffer);
    uint16_t offset                                  = 0;
    uint16_t length                                  = 0;
    uint8_t slot_id                                  = 0x00;

    cmd_buffer[0] = (AUTH_PROTOCOL_VERSION << 4) | kQiCommandGetCertificate;
    cmd_buffer[1] = (uint8_t)((offset & 0x0700) >> 3) | (uint8_t)((length & 0x0700) >> 6) | slot_id;
    cmd_buffer[2] = (uint8_t)(offset & 0x00FF);
    cmd_buffer[3] = (uint8_t)(length & 0x00FF);

    /* Send Read_Certificate command */
    powerTransmitterSendCommand(cmd_buffer, cmd_size - 1, response_buffer, &response_size);
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(response_buffer[0],
        ((AUTH_PROTOCOL_VERSION << 4) | kQiResponseError),
        "Authentication message header mismatch");
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(response_buffer[1], kQiErrorInvalidRequest, "Error Code mismatch");
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(response_buffer[2], 0x00, "Error data mismatch");

    /* Send Read_Certificate command */
    powerTransmitterSendCommand(cmd_buffer, cmd_size + 1, response_buffer, &response_size);
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(response_buffer[0],
        ((AUTH_PROTOCOL_VERSION << 4) | kQiResponseError),
        "Authentication message header mismatch");
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(response_buffer[1], kQiErrorInvalidRequest, "Error Code mismatch");
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(response_buffer[2], 0x00, "Error data mismatch");
}

TEST(QiTransmitter, ReadCertificateInvalidRspLength)
{
    uint8_t cmd_buffer[MAX_CMD_SIZE_GET_CERTIFICATE] = {0};
    size_t cmd_size                                  = sizeof(cmd_buffer);
    uint8_t response_buffer[4096]                    = {0};
    size_t response_size                             = 2;
    uint16_t offset                                  = 0;
    uint16_t length                                  = 0;
    uint8_t slot_id                                  = 0x00;

    cmd_buffer[0] = (AUTH_PROTOCOL_VERSION << 4) | kQiCommandGetCertificate;
    cmd_buffer[1] = (uint8_t)((offset & 0x0700) >> 3) | (uint8_t)((length & 0x0700) >> 6) | slot_id;
    cmd_buffer[2] = (uint8_t)(offset & 0x00FF);
    cmd_buffer[3] = (uint8_t)(length & 0x00FF);

    /* Send Read_Certificate command */
    powerTransmitterSendCommand(cmd_buffer, cmd_size, response_buffer, &response_size);
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(response_buffer[0],
        ((AUTH_PROTOCOL_VERSION << 4) | kQiResponseError),
        "Authentication message header mismatch");
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(response_buffer[1], kQiErrorUnspecified, "Error Code mismatch");
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(response_buffer[2], 0x00, "Error data mismatch");
}

TEST(QiTransmitter, ChallengeAuth)
{
    uint8_t cmd_buffer[MAX_CMD_SIZE_CHALLENGE]      = {0};
    size_t cmd_size                                 = sizeof(cmd_buffer);
    uint8_t response_buffer[MAX_RSP_SIZE_CHALLENGE] = {0};
    size_t response_size                            = sizeof(response_buffer);
    uint8_t slot_id                                 = 0X00;
    uint8_t nonce[NONCE_LEN]                        = {0};

    /* Create command buffer for CHALLENGE command */
    cmd_buffer[0] = (AUTH_PROTOCOL_VERSION << 4) | kQiCommandChallenge;
    cmd_buffer[1] = ((0x00) << 3) | ((0x0) << 2) | slot_id;
    memcpy(&cmd_buffer[2], nonce, sizeof(nonce));

    /* Send CHALLENGE command */
    powerTransmitterSendCommand(cmd_buffer, cmd_size, response_buffer, &response_size);
    TEST_ASSERT_GREATER_THAN_MESSAGE(3, response_size, "CHALLENGE failed");
}

TEST(QiTransmitter, ChallengeAuthInvalidSlot)
{
    uint8_t cmd_buffer[MAX_CMD_SIZE_CHALLENGE]      = {0};
    size_t cmd_size                                 = sizeof(cmd_buffer);
    uint8_t response_buffer[MAX_RSP_SIZE_CHALLENGE] = {0};
    size_t response_size                            = sizeof(response_buffer);
    uint8_t slot_id                                 = 0x01;
    uint8_t nonce[NONCE_LEN]                        = {0};

    /* Create command buffer for CHALLENGE command */
    cmd_buffer[0] = (AUTH_PROTOCOL_VERSION << 4) | kQiCommandChallenge;
    cmd_buffer[1] = ((0x00) << 3) | ((0x0) << 2) | slot_id;
    memcpy(&cmd_buffer[2], nonce, sizeof(nonce));

    /* Send CHALLENGE command */
    powerTransmitterSendCommand(cmd_buffer, cmd_size, response_buffer, &response_size);
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(response_buffer[0],
        ((AUTH_PROTOCOL_VERSION << 4) | kQiResponseError),
        "Authentication message header mismatch");
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(response_buffer[1], kQiErrorInvalidRequest, "Error Code mismatch");
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(response_buffer[2], 0x00, "Error data mismatch");
}

TEST(QiTransmitter, ChallengeAuthInvalidAuthProtocol)
{
    uint8_t cmd_buffer[MAX_CMD_SIZE_CHALLENGE]      = {0};
    size_t cmd_size                                 = sizeof(cmd_buffer);
    uint8_t response_buffer[MAX_RSP_SIZE_CHALLENGE] = {0};
    size_t response_size                            = sizeof(response_buffer);
    uint8_t slot_id                                 = 0x01;
    uint8_t nonce[NONCE_LEN]                        = {0};

    /* Create command buffer for CHALLENGE command */
    cmd_buffer[0] = (AUTH_PROTOCOL_VERSION << 4) | kQiCommandChallenge;
    cmd_buffer[1] = ((0x00) << 3) | ((0x0) << 2) | slot_id;
    memcpy(&cmd_buffer[2], nonce, sizeof(nonce));

    /* Corrupt the authentication message header */
    cmd_buffer[0] = 0x20;

    /* Send GET_DIGESTS command */
    powerTransmitterSendCommand(cmd_buffer, cmd_size, response_buffer, &response_size);
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(response_buffer[0],
        ((AUTH_PROTOCOL_VERSION << 4) | kQiResponseError),
        "Authentication message header mismatch");
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(response_buffer[1], kQiErrorUnsupportedProtocol, "Error Code mismatch");
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(response_buffer[2], AUTH_PROTOCOL_VERSION, "Error data mismatch");
}

TEST(QiTransmitter, ChallengeAuthInvalidMessageType)
{
    uint8_t cmd_buffer[MAX_CMD_SIZE_CHALLENGE]      = {0};
    size_t cmd_size                                 = sizeof(cmd_buffer);
    uint8_t response_buffer[MAX_RSP_SIZE_CHALLENGE] = {0};
    size_t response_size                            = sizeof(response_buffer);
    uint8_t slot_id                                 = 0x01;
    uint8_t nonce[NONCE_LEN]                        = {0};

    /* Create command buffer for CHALLENGE command */
    cmd_buffer[0] = (AUTH_PROTOCOL_VERSION << 4) | kQiCommandChallenge;
    cmd_buffer[1] = ((0x00) << 3) | ((0x0) << 2) | slot_id;
    memcpy(&cmd_buffer[2], nonce, sizeof(nonce));

    /* Corrupt the authentication message header */
    cmd_buffer[0] = (AUTH_PROTOCOL_VERSION << 4) | kQiResponseDigest;

    /* Send Read_Certificate command */
    powerTransmitterSendCommand(cmd_buffer, cmd_size, response_buffer, &response_size);
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(response_buffer[0],
        ((AUTH_PROTOCOL_VERSION << 4) | kQiResponseError),
        "Authentication message header mismatch");
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(response_buffer[1], kQiErrorInvalidRequest, "Error Code mismatch");
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(response_buffer[2], 0x00, "Error data mismatch");
}

TEST(QiTransmitter, ChallengeAuthNullCmdBuffer)
{
    uint8_t cmd_buffer[MAX_CMD_SIZE_CHALLENGE]      = {0};
    size_t cmd_size                                 = sizeof(cmd_buffer);
    uint8_t response_buffer[MAX_RSP_SIZE_CHALLENGE] = {0};
    size_t response_size                            = sizeof(response_buffer);
    uint8_t slot_id                                 = 0x01;
    uint8_t nonce[NONCE_LEN]                        = {0};

    /* Create command buffer for CHALLENGE command */
    cmd_buffer[0] = (AUTH_PROTOCOL_VERSION << 4) | kQiCommandChallenge;
    cmd_buffer[1] = ((0x00) << 3) | ((0x0) << 2) | slot_id;
    memcpy(&cmd_buffer[2], nonce, sizeof(nonce));

    /* Send CHALLENGE command */
    powerTransmitterSendCommand(NULL, cmd_size, response_buffer, &response_size);
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(response_buffer[0],
        ((AUTH_PROTOCOL_VERSION << 4) | kQiResponseError),
        "Authentication message header mismatch");
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(response_buffer[1], kQiErrorInvalidRequest, "Error Code mismatch");
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(response_buffer[2], 0x00, "Error data mismatch");

    powerTransmitterSendCommand(NULL, 0, response_buffer, &response_size);
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(response_buffer[0],
        ((AUTH_PROTOCOL_VERSION << 4) | kQiResponseError),
        "Authentication message header mismatch");
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(response_buffer[1], kQiErrorInvalidRequest, "Error Code mismatch");
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(response_buffer[2], 0x00, "Error data mismatch");
}

TEST(QiTransmitter, ChallengeAuthNullRspBuffer)
{
    uint8_t cmd_buffer[MAX_CMD_SIZE_CHALLENGE] = {0};
    size_t cmd_size                            = sizeof(cmd_buffer);
    size_t response_size                       = MAX_RSP_SIZE_CHALLENGE;
    uint8_t slot_id                            = 0x01;
    uint8_t nonce[NONCE_LEN]                   = {0};

    /* Create command buffer for CHALLENGE command */
    cmd_buffer[0] = (AUTH_PROTOCOL_VERSION << 4) | kQiCommandChallenge;
    cmd_buffer[1] = ((0x00) << 3) | ((0x0) << 2) | slot_id;
    memcpy(&cmd_buffer[2], nonce, sizeof(nonce));

    /* Send GET_DIGESTS command */
    powerTransmitterSendCommand(cmd_buffer, cmd_size, NULL, &response_size);

    response_size = 0;
    powerTransmitterSendCommand(cmd_buffer, cmd_size, NULL, &response_size);
}

TEST(QiTransmitter, ChallengeAuthInvalidCmdLength)
{
    uint8_t cmd_buffer[MAX_CMD_SIZE_CHALLENGE]      = {0};
    size_t cmd_size                                 = sizeof(cmd_buffer);
    uint8_t response_buffer[MAX_RSP_SIZE_CHALLENGE] = {0};
    size_t response_size                            = sizeof(response_buffer);
    uint8_t slot_id                                 = 0X00;
    uint8_t nonce[NONCE_LEN]                        = {0};

    /* Create command buffer for CHALLENGE command */
    cmd_buffer[0] = (AUTH_PROTOCOL_VERSION << 4) | kQiCommandChallenge;
    cmd_buffer[1] = ((0x00) << 3) | ((0x0) << 2) | slot_id;
    memcpy(&cmd_buffer[2], nonce, sizeof(nonce));

    /* Send CHALLENGE command */
    powerTransmitterSendCommand(cmd_buffer, cmd_size - 1, response_buffer, &response_size);
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(response_buffer[0],
        ((AUTH_PROTOCOL_VERSION << 4) | kQiResponseError),
        "Authentication message header mismatch");
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(response_buffer[1], kQiErrorInvalidRequest, "Error Code mismatch");
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(response_buffer[2], 0x00, "Error data mismatch");

    powerTransmitterSendCommand(cmd_buffer, cmd_size + 1, response_buffer, &response_size);
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(response_buffer[0],
        ((AUTH_PROTOCOL_VERSION << 4) | kQiResponseError),
        "Authentication message header mismatch");
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(response_buffer[1], kQiErrorInvalidRequest, "Error Code mismatch");
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(response_buffer[2], 0x00, "Error data mismatch");
}

TEST(QiTransmitter, ChallengeAuthInvalidRspLength)
{
    uint8_t cmd_buffer[MAX_CMD_SIZE_CHALLENGE]      = {0};
    size_t cmd_size                                 = sizeof(cmd_buffer);
    uint8_t response_buffer[MAX_RSP_SIZE_CHALLENGE] = {0};
    size_t response_size                            = 2;
    uint8_t slot_id                                 = 0X00;
    uint8_t nonce[NONCE_LEN]                        = {0};

    /* Create command buffer for CHALLENGE command */
    cmd_buffer[0] = (AUTH_PROTOCOL_VERSION << 4) | kQiCommandChallenge;
    cmd_buffer[1] = ((0x00) << 3) | ((0x0) << 2) | slot_id;
    memcpy(&cmd_buffer[2], nonce, sizeof(nonce));

    /* Send CHALLENGE command */
    powerTransmitterSendCommand(cmd_buffer, cmd_size, response_buffer, &response_size);
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(response_buffer[0],
        ((AUTH_PROTOCOL_VERSION << 4) | kQiResponseError),
        "Authentication message header mismatch");
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(response_buffer[1], kQiErrorUnspecified, "Error Code mismatch");
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(response_buffer[2], 0x00, "Error data mismatch");
}

TEST_GROUP_RUNNER(QiTransmitter)
{
    RUN_TEST_CASE(QiTransmitter, GetDigest);
    RUN_TEST_CASE(QiTransmitter, GetDigestInvalidAuthProtocol);
    RUN_TEST_CASE(QiTransmitter, GetDigestInvalidMessageType);
    RUN_TEST_CASE(QiTransmitter, GetDigestInvalidRequest);
    RUN_TEST_CASE(QiTransmitter, GetDigestInvalidSlotMask);
    RUN_TEST_CASE(QiTransmitter, GetDigestNullCmdBuffer);
    RUN_TEST_CASE(QiTransmitter, GetDigestNullRspBuffer);
    RUN_TEST_CASE(QiTransmitter, GetDigestInvalidCmdLength);
    RUN_TEST_CASE(QiTransmitter, GetDigestInvalidRspLength);

    RUN_TEST_CASE(QiTransmitter, ReadCertificate)
    RUN_TEST_CASE(QiTransmitter, ReadCertificateOffsetx600)
    RUN_TEST_CASE(QiTransmitter, ReadCertificateInvalidSlotId)
    RUN_TEST_CASE(QiTransmitter, ReadCertificateInvalidMessageType)
    RUN_TEST_CASE(QiTransmitter, ReadCertificateInvalidOffset)
    RUN_TEST_CASE(QiTransmitter, ReadCertificateInvalidLength)
    RUN_TEST_CASE(QiTransmitter, ReadCertificateOffsetEqualToLength)
    RUN_TEST_CASE(QiTransmitter, ReadCertificateNullCmdBuffer)
    RUN_TEST_CASE(QiTransmitter, ReadCertificateNullRspBuffer)
    RUN_TEST_CASE(QiTransmitter, ReadCertificateInvalidCmdLength)
    RUN_TEST_CASE(QiTransmitter, ReadCertificateInvalidRspLength)

    RUN_TEST_CASE(QiTransmitter, ChallengeAuth)
    RUN_TEST_CASE(QiTransmitter, ChallengeAuthInvalidAuthProtocol)
    RUN_TEST_CASE(QiTransmitter, ChallengeAuthInvalidMessageType)
    RUN_TEST_CASE(QiTransmitter, ChallengeAuthInvalidSlot)
    RUN_TEST_CASE(QiTransmitter, ChallengeAuthNullCmdBuffer)
    RUN_TEST_CASE(QiTransmitter, ChallengeAuthNullRspBuffer)
    RUN_TEST_CASE(QiTransmitter, ChallengeAuthInvalidCmdLength)
    RUN_TEST_CASE(QiTransmitter, ChallengeAuthInvalidRspLength)
}

int main(int argc, const char *argv[])
{
    int ret;
#if AX_EMBEDDED
    ex_sss_boot_direct();
#endif
    ret = UnityMain(argc, argv, runAllTests);
    DoFixtureTearDownIfNeeded();
    return ret;
}

/* ************************************************************************** */
/* Private Functions                                                          */
/* ************************************************************************** */

static void runAllTests(void)
{
    RUN_TEST_GROUP(QiTransmitter);
    RUN_TEST_GROUP(TestCommon);
}
