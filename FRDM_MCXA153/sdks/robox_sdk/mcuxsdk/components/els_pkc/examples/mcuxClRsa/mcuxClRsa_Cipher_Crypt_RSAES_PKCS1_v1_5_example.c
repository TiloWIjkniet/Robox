/*--------------------------------------------------------------------------*/
/* Copyright 2020-2024 NXP                                                  */
/*                                                                          */
/* NXP Proprietary. This software is owned or controlled by NXP and may     */
/* only be used strictly in accordance with the applicable license terms.   */
/* By expressly accepting such terms or by downloading, installing,         */
/* activating and/or otherwise using the software, you are agreeing that    */
/* you have read, and that you agree to comply with and are bound by, such  */
/* license terms.  If you do not agree to be bound by the applicable        */
/* license terms, then you may not retain, install, activate or otherwise   */
/* use the software.                                                        */
/*--------------------------------------------------------------------------*/

/**
 * @file  mcuxClRsa_Cipher_Crypt_RSAES_PKCS1_v1_5_example.c
 * @brief Example for the @ref mcuxCRsa component.
 *
 * @example mcuxClRsa_Cipher_Crypt_RSAES_PKCS1_v1_5_example.c
 * @brief   Example for the @ref mcuxCRsa component realize RSA OAEP encrypt/decrypt operation
 *          using @ref mcuxClCipher_crypt.
 * \details Example for the @ref mcuxCRsa component realize encrypt/decrypt operation using
 *          @ref mcuxClCipher_crypt for:
 *          - PKCS1_v1_5 en-/decryption mode,
 *          - 2048-bit RSA key,
 *          - RSA private key in CRT form with DFA.
 */

#include <mcuxClSession.h>
#include <mcuxClKey.h>
#include <mcuxClRandom.h>
#include <mcuxClRandomModes.h>
#include <mcuxClRsa.h>
#include <mcuxClCipher.h>
#include <mcuxClBuffer.h>
#include <mcuxClCore_Examples.h>
#include <mcuxClCore_Macros.h>
#include <mcuxClExample_Session_Helper.h>
#include <mcuxClEls.h> // Interface to the entire mcuxClEls component
#include <mcuxClExample_ELS_Helper.h>

/**********************************************************/
/* Example test vectors                                   */
/**********************************************************/

#define RSA_KEY_BIT_LENGTH         (MCUXCLKEY_SIZE_2048)      ///< The example uses a 2048-bit key
#define RSA_KEY_BYTE_LENGTH        (RSA_KEY_BIT_LENGTH / 8u) ///< Converting the key-bitlength to bytelength
#define RSA_PUBLIC_EXP_BYTE_LENGTH (3u)                      ///< The public exponent has a length of three bytes
#define INPUT_MESSAGE_LENGTH       (64u)                     ///< Arbitrary size of the message to be encrypted/decrypted

/**
 * @brief Example value for public RSA modulus N.
 */
static const uint8_t modulus[RSA_KEY_BYTE_LENGTH] __attribute__ ((aligned (4))) = {
  0xBEu, 0xD8u, 0xFFu, 0x2Du, 0xBCu, 0xE9u, 0x6Eu, 0xCBu, 0x7Cu, 0xB6u, 0x86u, 0x86u, 0x6Du, 0x01u, 0x98u, 0x41u,
  0x49u, 0x38u, 0x06u, 0xCAu, 0x50u, 0x8Fu, 0x5Cu, 0xF0u, 0x3Au, 0x02u, 0x90u, 0x90u, 0x5Bu, 0xC5u, 0x1Au, 0xCCu,
  0xE6u, 0x69u, 0x17u, 0xF2u, 0x53u, 0x58u, 0xC0u, 0x94u, 0x93u, 0xEAu, 0x57u, 0x2Bu, 0xC1u, 0x09u, 0x69u, 0x46u,
  0x81u, 0xD3u, 0x15u, 0x4Cu, 0xD5u, 0x23u, 0xBEu, 0x32u, 0x06u, 0xB6u, 0xD0u, 0xEAu, 0x30u, 0xD3u, 0xDDu, 0x65u,
  0x9Bu, 0xE8u, 0xACu, 0xC7u, 0x0Bu, 0x4Cu, 0xA5u, 0x14u, 0xE9u, 0x01u, 0x9Eu, 0x4Eu, 0xEEu, 0x2Fu, 0x57u, 0x8Au,
  0x64u, 0x71u, 0x59u, 0xC9u, 0x4Cu, 0x11u, 0xE2u, 0xE0u, 0xECu, 0xC9u, 0x96u, 0x75u, 0xF4u, 0x92u, 0xDFu, 0x1Eu,
  0x84u, 0x78u, 0xBDu, 0xC4u, 0x3Cu, 0xC1u, 0x03u, 0x8Du, 0x3Cu, 0x4Eu, 0x70u, 0x25u, 0x22u, 0x0Au, 0x15u, 0x0Au,
  0xFFu, 0x9Eu, 0x2Bu, 0x45u, 0x0Cu, 0x72u, 0x11u, 0x0Au, 0xE5u, 0x4Bu, 0x3Cu, 0xCBu, 0x8Au, 0x80u, 0x3Cu, 0x41u,
  0x42u, 0xFEu, 0x78u, 0x34u, 0xF0u, 0x1Au, 0x55u, 0x37u, 0x1Bu, 0x7Du, 0x3Au, 0xEEu, 0x38u, 0x25u, 0x58u, 0x52u,
  0x27u, 0x75u, 0x9Eu, 0x59u, 0x41u, 0xFAu, 0x43u, 0x11u, 0x92u, 0xB9u, 0x70u, 0x17u, 0x1Du, 0x4Bu, 0x11u, 0xDAu,
  0xE0u, 0xF5u, 0xB7u, 0x77u, 0x48u, 0x93u, 0x4Eu, 0x3Bu, 0x68u, 0x60u, 0x08u, 0x86u, 0x57u, 0xD6u, 0x61u, 0xBFu,
  0x4Au, 0x31u, 0x41u, 0xFAu, 0x11u, 0xFBu, 0x3Au, 0x90u, 0x3Au, 0x22u, 0xB8u, 0xE0u, 0x38u, 0x27u, 0xB9u, 0x25u,
  0x8Du, 0x0Eu, 0xDEu, 0x8Au, 0xDCu, 0x65u, 0x04u, 0x7Bu, 0xDFu, 0x4Au, 0xA0u, 0x5Fu, 0x78u, 0x8Fu, 0x7Eu, 0xC5u,
  0x66u, 0xFFu, 0x85u, 0x33u, 0x73u, 0x06u, 0x23u, 0x24u, 0x39u, 0x1Fu, 0x66u, 0x26u, 0x18u, 0x16u, 0x53u, 0x30u,
  0x2Eu, 0x24u, 0xC4u, 0x92u, 0x39u, 0x13u, 0x14u, 0x98u, 0x53u, 0x84u, 0xEAu, 0x99u, 0xDCu, 0x40u, 0x57u, 0x30u,
  0xC4u, 0x2Fu, 0xE7u, 0x89u, 0xB6u, 0x69u, 0x5Du, 0x60u, 0x0Fu, 0x4Bu, 0x1Du, 0x66u, 0x54u, 0x22u, 0x8Du, 0xB1u
 };

/**
 * @brief Example value for prime factor P.
 */
static const uint8_t primeP[RSA_KEY_BYTE_LENGTH/2] __attribute__ ((aligned (4))) = {
  0xC1u, 0x26u, 0x5Cu, 0x6Bu, 0xD6u, 0x5Cu, 0x5Du, 0x57u, 0xBFu, 0xA9u, 0x60u, 0x2Eu, 0xCAu, 0x66u, 0x30u, 0x46u,
  0xD8u, 0x2Au, 0x16u, 0x1Eu, 0xEAu, 0xB3u, 0xD7u, 0xF2u, 0x15u, 0xABu, 0x39u, 0xD4u, 0x9Bu, 0xFCu, 0x4Au, 0xB3u,
  0x67u, 0x8Au, 0xC0u, 0x17u, 0xE7u, 0x43u, 0x6Bu, 0x3Du, 0xF1u, 0xB3u, 0xA3u, 0x31u, 0x13u, 0x21u, 0x3Bu, 0x98u,
  0x53u, 0x14u, 0x73u, 0x7Du, 0x10u, 0xEBu, 0x72u, 0x3Eu, 0x2Eu, 0x08u, 0xC8u, 0xC9u, 0x57u, 0xC7u, 0x45u, 0xDFu,
  0x5Du, 0xD5u, 0x6Eu, 0xF4u, 0xABu, 0x99u, 0x66u, 0x8Cu, 0x5Fu, 0x48u, 0xF0u, 0xD4u, 0x95u, 0xF2u, 0xEBu, 0xCBu,
  0x73u, 0x7Fu, 0x70u, 0x69u, 0x6Eu, 0x81u, 0x5Du, 0x86u, 0xACu, 0xFBu, 0xBDu, 0x02u, 0x97u, 0x5Bu, 0xD3u, 0xEBu,
  0x3Au, 0x4Du, 0xBCu, 0x51u, 0xF5u, 0xA9u, 0x9Bu, 0xC0u, 0xB4u, 0xFCu, 0x6Cu, 0xF9u, 0xE2u, 0xC6u, 0xCAu, 0x5Au,
  0x42u, 0x6Bu, 0x82u, 0x10u, 0xD8u, 0x47u, 0x8Cu, 0xFCu, 0x9Eu, 0x4Bu, 0x11u, 0x8Au, 0xF3u, 0xE1u, 0x4Eu, 0x23u
};

/**
 * @brief Example value for prime factor Q.
 */
static const uint8_t primeQ[RSA_KEY_BYTE_LENGTH/2] __attribute__ ((aligned (4))) = {
  0xFCu, 0xF2u, 0xDBu, 0xEFu, 0x1Au, 0x9Eu, 0x4Eu, 0xD5u, 0x74u, 0x1Du, 0xF0u, 0x08u, 0x58u, 0xD4u, 0xEBu, 0xDEu,
  0x88u, 0x45u, 0xADu, 0xC0u, 0xD3u, 0xA6u, 0xA2u, 0x36u, 0x93u, 0xE7u, 0x3Bu, 0x68u, 0x51u, 0x18u, 0x63u, 0x16u,
  0x79u, 0x8Du, 0x4Fu, 0x08u, 0x2Eu, 0xE1u, 0x7Eu, 0xDCu, 0x6Fu, 0x41u, 0x53u, 0x64u, 0xF1u, 0xE0u, 0x3Au, 0xDFu,
  0xD4u, 0x7Du, 0x98u, 0xF8u, 0x93u, 0x23u, 0xEEu, 0x52u, 0xC4u, 0x2Eu, 0x31u, 0x50u, 0xFAu, 0x68u, 0x73u, 0xA0u,
  0x93u, 0xAFu, 0xCFu, 0xA4u, 0x21u, 0xAEu, 0x43u, 0x0Au, 0x3Fu, 0x97u, 0xCAu, 0x58u, 0x61u, 0x60u, 0xB7u, 0xE5u,
  0x78u, 0x35u, 0xD8u, 0xACu, 0x6Fu, 0x11u, 0xBEu, 0x96u, 0xEBu, 0xA9u, 0xA9u, 0x0Cu, 0x5Au, 0xE4u, 0x63u, 0x48u,
  0xBDu, 0x00u, 0x26u, 0xEBu, 0xD7u, 0xDEu, 0x6Au, 0xBDu, 0x0Bu, 0xB8u, 0xA3u, 0x8Au, 0x34u, 0x12u, 0x88u, 0xC9u,
  0x84u, 0x4Du, 0xD3u, 0xA9u, 0x0Au, 0x5Eu, 0xEDu, 0xA9u, 0x2Fu, 0x1Eu, 0x2Bu, 0x09u, 0x2Du, 0x10u, 0x70u, 0x1Bu
};

/**
 * @brief Example value for exponent DP.
 */
static const uint8_t dp[RSA_KEY_BYTE_LENGTH/2] __attribute__ ((aligned (4))) = {
  0x82u, 0xABu, 0x62u, 0x21u, 0x2Eu, 0x5Fu, 0x44u, 0x62u, 0xE5u, 0xEEu, 0x3Fu, 0x7Cu, 0xC8u, 0x3Fu, 0x03u, 0xF0u,
  0x19u, 0xB3u, 0xB7u, 0x4Du, 0x69u, 0x39u, 0x0Cu, 0x21u, 0xE1u, 0xD8u, 0xFAu, 0x01u, 0xC5u, 0x19u, 0x94u, 0xABu,
  0xF4u, 0xA3u, 0xA0u, 0xBBu, 0x4Bu, 0x20u, 0x88u, 0x3Fu, 0xDAu, 0xF1u, 0xCDu, 0xB8u, 0x98u, 0x99u, 0x86u, 0x08u,
  0xD2u, 0x43u, 0xE6u, 0xB1u, 0xB8u, 0xADu, 0xA0u, 0x97u, 0x42u, 0x6Bu, 0x7Cu, 0xF3u, 0x01u, 0xE8u, 0x75u, 0x73u,
  0xDCu, 0xB6u, 0x55u, 0x1Fu, 0x3Fu, 0xACu, 0x42u, 0xFDu, 0x3Au, 0x45u, 0x4Du, 0x70u, 0x74u, 0x95u, 0x68u, 0x42u,
  0x36u, 0xBCu, 0x03u, 0x9Fu, 0xC0u, 0x3Bu, 0xD2u, 0xBBu, 0x16u, 0xF2u, 0x23u, 0xF7u, 0xC9u, 0xD0u, 0x3Cu, 0xF9u,
  0x49u, 0x73u, 0x67u, 0xB1u, 0x07u, 0x02u, 0x9Cu, 0xB5u, 0x6Du, 0x7Bu, 0xCCu, 0x79u, 0xEDu, 0x9Au, 0xD1u, 0x30u,
  0xE8u, 0xF8u, 0x74u, 0x80u, 0xD2u, 0xE0u, 0xEDu, 0x17u, 0xC6u, 0x3Bu, 0x40u, 0xFEu, 0x01u, 0x69u, 0xEEu, 0x83u
};

/**
 * @brief Example value for exponent DQ.
 */
static const uint8_t dq[RSA_KEY_BYTE_LENGTH/2] __attribute__ ((aligned (4))) = {
  0xB0u, 0xBEu, 0x7Du, 0xA1u, 0x10u, 0x07u, 0x67u, 0xECu, 0x4Cu, 0x6Bu, 0x92u, 0xCAu, 0x32u, 0x4Fu, 0xECu, 0xD4u,
  0x1Cu, 0x82u, 0x1Bu, 0x8Bu, 0xAEu, 0x18u, 0x34u, 0x26u, 0x50u, 0xA8u, 0x74u, 0xE1u, 0x4Au, 0x30u, 0xF1u, 0x23u,
  0xC6u, 0x21u, 0x50u, 0x04u, 0xD6u, 0xC5u, 0x27u, 0xA0u, 0x9Du, 0x78u, 0x96u, 0xEDu, 0xE4u, 0xF8u, 0x9Au, 0x0Au,
  0xC6u, 0x6Eu, 0x50u, 0x51u, 0xF8u, 0x76u, 0x55u, 0xD3u, 0xADu, 0x52u, 0xDDu, 0x90u, 0xC8u, 0xB7u, 0xEDu, 0x7Bu,
  0x59u, 0x56u, 0xB2u, 0x8Eu, 0xECu, 0x1Du, 0xD8u, 0xA8u, 0x33u, 0x91u, 0x3Bu, 0x89u, 0x0Fu, 0xD9u, 0xC6u, 0x05u,
  0x68u, 0x3Eu, 0xAFu, 0xBCu, 0xA5u, 0x0Bu, 0x50u, 0x12u, 0x22u, 0x6Eu, 0xF5u, 0x39u, 0x35u, 0xD5u, 0x79u, 0xEEu,
  0x5Cu, 0x69u, 0xDBu, 0xC8u, 0x55u, 0x99u, 0x0Bu, 0x1Au, 0x37u, 0x33u, 0x77u, 0xCAu, 0x5Cu, 0xE2u, 0x4Au, 0x84u,
  0x0Cu, 0x97u, 0x58u, 0xFBu, 0x37u, 0xCCu, 0xE6u, 0xE1u, 0x9Du, 0x93u, 0xC5u, 0xDCu, 0x6Eu, 0x89u, 0x9Au, 0xDBu
};

/**
 * @brief Example value for qInv.
 */
static const uint8_t qInv[RSA_KEY_BYTE_LENGTH/2] __attribute__ ((aligned (4))) = {
  0x66u, 0xB2u, 0x11u, 0x6Fu, 0x95u, 0xF8u, 0x21u, 0x42u, 0xC3u, 0xAEu, 0x71u, 0xBDu, 0x49u, 0x1Du, 0x2Eu, 0xF9u,
  0x8Du, 0xE8u, 0xEFu, 0xBEu, 0x98u, 0xB3u, 0xD2u, 0x36u, 0xD5u, 0x34u, 0x48u, 0x2Bu, 0xF8u, 0x3Eu, 0xB1u, 0x85u,
  0xF4u, 0x87u, 0x3Bu, 0x16u, 0xD3u, 0xEEu, 0x2Cu, 0xCEu, 0xA9u, 0x05u, 0xDBu, 0x59u, 0x0Fu, 0x73u, 0x5Cu, 0x33u,
  0xEAu, 0x70u, 0xF7u, 0xF3u, 0xF6u, 0x88u, 0x7Cu, 0xC1u, 0x1Du, 0x87u, 0xDDu, 0xA0u, 0x33u, 0x1Cu, 0xAEu, 0x6Du,
  0x08u, 0xA4u, 0x5Cu, 0x3Fu, 0x41u, 0x5Cu, 0x1Cu, 0x18u, 0x7Cu, 0xB8u, 0x45u, 0x53u, 0x57u, 0x9Au, 0x91u, 0x1Fu,
  0x41u, 0xF9u, 0x1Du, 0x9Au, 0x9Au, 0x1Eu, 0x1Du, 0xFCu, 0x75u, 0x36u, 0x42u, 0xE5u, 0x6Bu, 0x21u, 0x9Cu, 0x67u,
  0xF2u, 0x66u, 0xFBu, 0x62u, 0xC4u, 0xE9u, 0xF8u, 0x51u, 0x1Du, 0xD9u, 0xBDu, 0xB8u, 0x25u, 0xD8u, 0xE5u, 0x60u,
  0x9Du, 0x3Cu, 0xA1u, 0xDEu, 0x05u, 0xDCu, 0x29u, 0x2Cu, 0x4Au, 0x55u, 0xEDu, 0xF6u, 0xADu, 0xF2u, 0xC4u, 0xDFu
};

/**
 * @brief Example value for public RSA exponent e.
 */
static const uint8_t pubExp[RSA_PUBLIC_EXP_BYTE_LENGTH] __attribute__ ((aligned (4))) = {
  0x01u, 0x00u, 0x01u
};

/**
 * @brief Example plaintext to be encrypted.
 */
static const uint8_t plainData[INPUT_MESSAGE_LENGTH] = {
  0x61u, 0x62u, 0x63u, 0x64u, 0x65u, 0x66u, 0x67u, 0x68u, 0x69u, 0x6Au, 0x6Bu, 0x6Cu, 0x6Du, 0x6Eu, 0x6Fu, 0x70u,
  0x62u, 0x63u, 0x64u, 0x65u, 0x66u, 0x67u, 0x68u, 0x69u, 0x6Au, 0x6Bu, 0x6Cu, 0x6Du, 0x6Eu, 0x6Fu, 0x70u, 0x71u,
  0x63u, 0x64u, 0x65u, 0x66u, 0x67u, 0x68u, 0x69u, 0x6Au, 0x6Bu, 0x6Cu, 0x6Du, 0x6Eu, 0x6Fu, 0x70u, 0x71u, 0x72u,
  0x64u, 0x65u, 0x66u, 0x67u, 0x68u, 0x69u, 0x6Au, 0x6Bu, 0x6Cu, 0x6Du, 0x6Eu, 0x6Fu, 0x70u, 0x71u, 0x72u, 0x73u
};


MCUXCLEXAMPLE_FUNCTION(mcuxClRsa_Cipher_Crypt_RSAES_PKCS1_v1_5_example)
{
  /**************************************************************************/
  /* Preparation: setup session                                             */
  /**************************************************************************/

  /** Initialize ELS, Enable the ELS **/
  if(!mcuxClExample_Els_Init(MCUXCLELS_RESET_DO_NOT_CANCEL))
  {
    return MCUXCLEXAMPLE_STATUS_ERROR;
  }

  #define CPU_WA_BUFFER_SIZE MCUXCLCORE_MAX(MCUXCLCORE_MAX(\
                                              MCUXCLRANDOMMODES_NCINIT_WACPU_SIZE,\
                                              MCUXCLRSA_ENCRYPT_WACPU_SIZE(RSA_KEY_BIT_LENGTH)),\
                                              MCUXCLRSA_DECRYPT_WACPU_SIZE(RSA_KEY_BIT_LENGTH))
  #define PKC_WA_BUFFER_SIZE MCUXCLCORE_MAX(MCUXCLRSA_ENCRYPT_WAPKC_SIZE(RSA_KEY_BIT_LENGTH),\
                                              MCUXCLRSA_DECRYPT_WAPKC_SIZE(RSA_KEY_BIT_LENGTH))


  mcuxClSession_Descriptor_t sessionDesc;
  mcuxClSession_Handle_t session = &sessionDesc;
  //Allocate and initialize session
  MCUXCLEXAMPLE_ALLOCATE_AND_INITIALIZE_SESSION(session,
                                              CPU_WA_BUFFER_SIZE,
                                              PKC_WA_BUFFER_SIZE);

  /**************************************************************************/
  /* Initialize the PRNG                                                    */
  /**************************************************************************/
  MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(prngInit_result, prngInit_token, mcuxClRandom_ncInit(session));
  if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClRandom_ncInit) != prngInit_token) || (MCUXCLRANDOM_STATUS_OK != prngInit_result))
  {
      return MCUXCLEXAMPLE_STATUS_ERROR;
  }
  MCUX_CSSL_FP_FUNCTION_CALL_END();

  /**************************************************************************/
  /* Preparation: setup RSA key                                             */
  /**************************************************************************/

  /* Allocation of key data buffers, which contain RSA key parameters */
  MCUX_CSSL_ANALYSIS_START_SUPPRESS_DISCARD_CONST("Required by API, elements are not modified in executed scenario")
  mcuxClRsa_KeyData_Crt_t privKeyStruct = {
                              .p.pKeyEntryData = (uint8_t*)primeP,
                              .p.keyEntryLength = sizeof(primeP),
                              .q.pKeyEntryData = (uint8_t*)primeQ,
                              .q.keyEntryLength = sizeof(primeQ),
                              .qInv.pKeyEntryData = (uint8_t*)qInv,
                              .qInv.keyEntryLength = sizeof(qInv),
                              .dp.pKeyEntryData = (uint8_t*)dp,
                              .dp.keyEntryLength = sizeof(dp),
                              .dq.pKeyEntryData = (uint8_t*)dq,
                              .dq.keyEntryLength = sizeof(dq),
                              .e.pKeyEntryData = (uint8_t*)pubExp,
                              .e.keyEntryLength = sizeof(pubExp)

  };
  MCUX_CSSL_ANALYSIS_STOP_SUPPRESS_DISCARD_CONST()

  MCUX_CSSL_ANALYSIS_START_SUPPRESS_DISCARD_CONST("Required by API, elements are not modified in executed scenario")
  mcuxClRsa_KeyData_Plain_t pubKeyStruct = {
                              .modulus.pKeyEntryData = (uint8_t*)modulus,
                              .modulus.keyEntryLength = RSA_KEY_BYTE_LENGTH,
                              .exponent.pKeyEntryData = (uint8_t*)pubExp,
                              .exponent.keyEntryLength = sizeof(pubExp)
  };
  MCUX_CSSL_ANALYSIS_STOP_SUPPRESS_DISCARD_CONST()

  /* Initialize RSA private key */
  uint32_t privKeyDesc[MCUXCLKEY_DESCRIPTOR_SIZE_IN_WORDS];
  MCUX_CSSL_ANALYSIS_START_PATTERN_REINTERPRET_MEMORY_OF_OPAQUE_TYPES()
  mcuxClKey_Handle_t privKey = (mcuxClKey_Handle_t) privKeyDesc;
  MCUX_CSSL_ANALYSIS_STOP_PATTERN_REINTERPRET_MEMORY_OF_OPAQUE_TYPES()

  uint32_t sizeOfPrivKey = (uint32_t)sizeof(privKeyStruct);
  MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(keyInitPriv_result, keyInitPriv_token, mcuxClKey_init(
    /* mcuxClSession_Handle_t session         */ session,
    MCUX_CSSL_ANALYSIS_START_SUPPRESS_POINTER_INCOMPATIBLE("The pointer privKey points to an object of the right type, the cast was valid.")
    /* mcuxClKey_Handle_t key                 */ privKey,
    MCUX_CSSL_ANALYSIS_STOP_SUPPRESS_POINTER_INCOMPATIBLE()
    /* mcuxClKey_Type_t type                  */ mcuxClKey_Type_Rsa_PrivateCRT_DFA_2048,
    /* uint8_t * pKeyData                    */ (uint8_t *) &privKeyStruct,
    /* uint32_t keyDataLength                */ sizeOfPrivKey
  ));
  if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClKey_init) != keyInitPriv_token) || (MCUXCLKEY_STATUS_OK != keyInitPriv_result))
  {
      return MCUXCLEXAMPLE_STATUS_ERROR;
  }
  MCUX_CSSL_FP_FUNCTION_CALL_END();

  /* Initialize RSA public key */
  uint32_t pubKeyDesc[MCUXCLKEY_DESCRIPTOR_SIZE_IN_WORDS];
  MCUX_CSSL_ANALYSIS_START_PATTERN_REINTERPRET_MEMORY_OF_OPAQUE_TYPES()
  mcuxClKey_Handle_t pubKey = (mcuxClKey_Handle_t) pubKeyDesc;
  MCUX_CSSL_ANALYSIS_STOP_PATTERN_REINTERPRET_MEMORY_OF_OPAQUE_TYPES()

  MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(keyInitPub_result, keyInitPub_token, mcuxClKey_init(
    /* mcuxClSession_Handle_t session         */ session,
    MCUX_CSSL_ANALYSIS_START_SUPPRESS_POINTER_INCOMPATIBLE("The pointer pubKey is of the right type (mcuxClKey_Handle_t)")
    /* mcuxClKey_Handle_t key                 */ pubKey,
    MCUX_CSSL_ANALYSIS_STOP_SUPPRESS_POINTER_INCOMPATIBLE()
    /* mcuxClKey_Type_t type                  */ mcuxClKey_Type_Rsa_Public_2048,
    /* uint8_t * pKeyData                    */ (uint8_t *) &pubKeyStruct,
    /* uint32_t keyDataLength                */ (uint32_t)sizeof(pubKeyStruct)
  ));
  if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClKey_init) != keyInitPub_token) || (MCUXCLKEY_STATUS_OK != keyInitPub_result))
  {
      return MCUXCLEXAMPLE_STATUS_ERROR;
  }
  MCUX_CSSL_FP_FUNCTION_CALL_END();

  /**************************************************************************/
  /* Preparation: setup RSAES_PKCS1_v1_5 encrypt                            */
  /**************************************************************************/

  /* Fill mode descriptor with the relevant data for the selected padding and hash algorithms */
  uint32_t cipherModeBytes[MCUXCLRSA_CIPHER_MODE_SIZE_IN_WORD];
  MCUX_CSSL_ANALYSIS_START_PATTERN_REINTERPRET_MEMORY_OF_OPAQUE_TYPES()
  mcuxClCipher_ModeDescriptor_t *pCipherMode = (mcuxClCipher_ModeDescriptor_t *) cipherModeBytes;
  MCUX_CSSL_ANALYSIS_STOP_PATTERN_REINTERPRET_MEMORY_OF_OPAQUE_TYPES()

  MCUX_CSSL_ANALYSIS_START_SUPPRESS_POINTER_INCOMPATIBLE("The pointer pCipherMode points to an object of the right type, the cast was valid.")
  mcuxClRsa_CipherModeConstructor_RSAES_PKCS1_v1_5_Encrypt(/* mcuxClCipher_ModeDescriptor_t * pCipherMode: */ pCipherMode);
  MCUX_CSSL_ANALYSIS_STOP_SUPPRESS_POINTER_INCOMPATIBLE()

  /**************************************************************************/
  /* Encryption                                                             */
  /**************************************************************************/

  uint8_t encryptedData[RSA_KEY_BYTE_LENGTH];
  uint32_t encryptedSize = 0u;

  MCUXCLBUFFER_INIT_RO(plainDataBuf, session, plainData, INPUT_MESSAGE_LENGTH);
  MCUXCLBUFFER_INIT(encryptedDataBuf, session, encryptedData, RSA_KEY_BYTE_LENGTH);
  uint32_t sizeOfPlainData = (uint32_t)sizeof(plainData);

  MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(cipherEncrypt_result, cipherEncrypt_token, mcuxClCipher_crypt(
    /* mcuxClSession_Handle_t session          */ session,
    /* const mcuxClKey_Handle_t key            */ pubKey,
    /* mcuxClCipher_Mode_t mode                */ pCipherMode,
    MCUX_CSSL_ANALYSIS_START_PATTERN_NULL_POINTER_CONSTANT()
    /* mcuxCl_InputBuffer_t pIv                */ NULL, /* Unused for RSAES-PKCS1-v1_5 */
    MCUX_CSSL_ANALYSIS_STOP_PATTERN_NULL_POINTER_CONSTANT()
    /* uint32_t ivLength                      */ 0u,
    /* mcuxCl_InputBuffer_t pIn                */ plainDataBuf,
    /* uint32_t inLength                      */ sizeOfPlainData,
    /* mcuxCl_Buffer_t pOut                    */ encryptedDataBuf,
    /* uint32_t * const pOutLength            */ &encryptedSize
  ));
  if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClCipher_crypt) != cipherEncrypt_token) || (MCUXCLCIPHER_STATUS_OK != cipherEncrypt_result))
  {
      return MCUXCLEXAMPLE_STATUS_ERROR;
  }
  MCUX_CSSL_FP_FUNCTION_CALL_END();

  if(encryptedSize != sizeof(encryptedData))
  {
    return MCUXCLEXAMPLE_STATUS_ERROR;
  }

  /**************************************************************************/
  /* Preparation: setup RSAES_PKCS1_v1_5 decrypt                            */
  /**************************************************************************/
  mcuxClRsa_CipherModeConstructor_RSAES_PKCS1_v1_5_Decrypt(/* mcuxClCipher_ModeDescriptor_t * pCipherMode: */ pCipherMode);

  /**************************************************************************/
  /* Decryption                                                             */
  /**************************************************************************/

  uint32_t decryptedSize = 0u;
  uint8_t decryptedData[INPUT_MESSAGE_LENGTH];
  MCUXCLBUFFER_INIT(decryptedDataBuf, session, decryptedData, INPUT_MESSAGE_LENGTH);

  MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(cipherDecrypt_result, cipherDecrypt_token, mcuxClCipher_crypt(
    /* mcuxClSession_Handle_t session         */ session,
    /* const mcuxClKey_Handle_t key           */ privKey,
    /* mcuxClCipher_Mode_t mode               */ pCipherMode,
    MCUX_CSSL_ANALYSIS_START_PATTERN_NULL_POINTER_CONSTANT()
    /* mcuxCl_InputBuffer_t pIv               */ NULL, /* Unused for RSAES-PKCS1-v1_5 */
    MCUX_CSSL_ANALYSIS_STOP_PATTERN_NULL_POINTER_CONSTANT()
    /* uint32_t ivLength                     */ 0u,
    /* mcuxCl_InputBuffer_t pIn               */ (mcuxCl_InputBuffer_t)encryptedDataBuf,
    /* uint32_t inLength                     */ encryptedSize,
    /* mcuxCl_Buffer_t pOut                   */ decryptedDataBuf,
    /* uint32_t * const pOutLength           */ &decryptedSize
  ));
  if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClCipher_crypt) != cipherDecrypt_token) || (MCUXCLCIPHER_STATUS_OK != cipherDecrypt_result))
  {
      return MCUXCLEXAMPLE_STATUS_ERROR;
  }
  MCUX_CSSL_FP_FUNCTION_CALL_END();

  if(decryptedSize != sizeof(decryptedData))
  {
    return MCUXCLEXAMPLE_STATUS_ERROR;
  }

  /**************************************************************************/
  /* Destroy the current session                                            */
  /**************************************************************************/

  if(!mcuxClExample_Session_Clean(session))
  {
    return MCUXCLEXAMPLE_STATUS_ERROR;
  }

  /** Disable the ELS **/
  if(!mcuxClExample_Els_Disable())
  {
    return MCUXCLEXAMPLE_STATUS_ERROR;
  }
  /**************************************************************************/
  /* Verification                                                           */
  /**************************************************************************/

  if(!mcuxClCore_assertEqual(plainData, decryptedData, sizeof(plainData)))
  {
    return MCUXCLEXAMPLE_STATUS_ERROR;
  }

  return MCUXCLEXAMPLE_STATUS_OK;
}

