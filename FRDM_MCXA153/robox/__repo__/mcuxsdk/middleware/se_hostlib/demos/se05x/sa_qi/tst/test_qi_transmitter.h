/* Copyright 2022 NXP
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __TST_QI_TX_H__
#define __TST_QI_TX_H__

#define MAX_CMD_SIZE_GET_DIGESTS 2
#define MAX_RSP_SIZE_GET_DIGESTS 2 + (32 /* Digest size */ * 4 /* Total slots */)

#define MAX_CMD_SIZE_GET_CERTIFICATE 4
#define MAX_CMD_SIZE_CHALLENGE 18
#define MAX_RSP_SIZE_CHALLENGE 67

#define NONCE_LEN 16

#define SLOT_ID_MASK_NO_SLOTS 0x0
#define SLOT_ID_MASK_SLOT_0 0x1
#define SLOT_ID_MASK_SLOT_1 0x2
#define SLOT_ID_MASK_SLOT_2 0x4
#define SLOT_ID_MASK_SLOT_3 0x8

#define CMD_GET_CERTIFICATE_MAX_OFFSET 0x07FF
#define CMD_GET_CERTIFICATE_MAX_LENGTH 0x07FF

extern const uint8_t qi_ec_priv_key[];
extern const uint8_t qi_certificate_chain[];
extern const size_t qi_ec_priv_key_len;
extern const size_t qi_certificate_chain_len;

void provisionCredentials(uint8_t slot_id, sss_object_t *pObject);
#endif // __TST_QI_TX_H__
