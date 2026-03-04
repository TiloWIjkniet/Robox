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
 * @file  mcuxClRsa_Cipher_Crypt_RSAES_OAEP_example.c
 * @brief Example for the @ref mcuxCRsa component.
 *
 * @example mcuxClRsa_Cipher_Crypt_RSAES_OAEP_example.c
 * @brief   Example for the @ref mcuxCRsa component realize RSA OAEP encrypt/decrypt operation
 *          using @ref mcuxClCipher_crypt.
 * \details Example for the @ref mcuxCRsa component realize encrypt/decrypt operation using
 *          @ref mcuxClCipher_crypt for:
 *          - OAEP en-/decryption mode with SHA-256,
 *          - 2048-bit RSA key,
 *          - RSA private key in plain form.
 */

#include <mcuxClSession.h>
#include <mcuxClKey.h>
#include <mcuxClRandom.h>
#include <mcuxClRandomModes.h>
#include <mcuxClRsa.h>
#include <mcuxClHash.h>
#include <mcuxClHashModes.h>
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
#define RSA_OAEP_LABEL_LENGTH      (0u)                      ///< The label length is set to 0 in this example
#define INPUT_MESSAGE_LENGTH       (64u)                     ///< Arbitrary size of the message to be encrypted/decrypted

/**
 * @brief Example value for public RSA modulus N.
 */
static const uint8_t modulus[RSA_KEY_BYTE_LENGTH] __attribute__ ((aligned (4))) = {
  0xd3u, 0x24u, 0x96u, 0xe6u, 0x2du, 0x16u, 0x34u, 0x6eu, 0x06u, 0xe7u, 0xa3u, 0x1cu, 0x12u, 0x0au, 0x21u, 0xb5u,
  0x45u, 0x32u, 0x32u, 0x35u, 0xeeu, 0x1du, 0x90u, 0x72u, 0x1du, 0xceu, 0xaau, 0xd4u, 0x6du, 0xc4u, 0xceu, 0xbdu,
  0x80u, 0xc1u, 0x34u, 0x5au, 0xffu, 0x95u, 0xb1u, 0xddu, 0xf8u, 0x71u, 0xebu, 0xb7u, 0xf2u, 0x0fu, 0xedu, 0xb6u,
  0xe4u, 0x2eu, 0x67u, 0xa0u, 0xccu, 0x59u, 0xb3u, 0x9fu, 0xfdu, 0x31u, 0xe9u, 0x83u, 0x42u, 0xf4u, 0x0au, 0xd9u,
  0xafu, 0xf9u, 0x3cu, 0x3cu, 0x51u, 0xcfu, 0x5fu, 0x3cu, 0x8au, 0xd0u, 0x64u, 0xb8u, 0x33u, 0xf9u, 0xacu, 0x34u,
  0x22u, 0x9au, 0x3eu, 0xd3u, 0xddu, 0x29u, 0x41u, 0xbeu, 0x12u, 0x5bu, 0xc5u, 0xa2u, 0x0cu, 0xb6u, 0xd2u, 0x31u,
  0xb6u, 0xd1u, 0x84u, 0x7eu, 0xc4u, 0xfeu, 0xaeu, 0x2bu, 0x88u, 0x46u, 0xcfu, 0x00u, 0xc4u, 0xc6u, 0xe7u, 0x5au,
  0x51u, 0x32u, 0x65u, 0x7au, 0x68u, 0xecu, 0x04u, 0x38u, 0x36u, 0x46u, 0x34u, 0xeau, 0xf8u, 0x27u, 0xf9u, 0xbbu,
  0x51u, 0x6cu, 0x93u, 0x27u, 0x48u, 0x1du, 0x58u, 0xb8u, 0xffu, 0x1eu, 0xa4u, 0xc0u, 0x1fu, 0xa1u, 0xa2u, 0x57u,
  0xa9u, 0x4eu, 0xa6u, 0xd4u, 0x72u, 0x60u, 0x3bu, 0x3fu, 0xb3u, 0x24u, 0x53u, 0x22u, 0x88u, 0xeau, 0x3au, 0x97u,
  0x43u, 0x53u, 0x59u, 0x15u, 0x33u, 0xa0u, 0xebu, 0xbeu, 0xf2u, 0x9du, 0xf4u, 0xf8u, 0xbcu, 0x4du, 0xdbu, 0xf8u,
  0x8eu, 0x47u, 0x1fu, 0x1du, 0xa5u, 0x00u, 0xb8u, 0xf5u, 0x7bu, 0xb8u, 0xc3u, 0x7cu, 0xa5u, 0xeau, 0x17u, 0x7cu,
  0x4eu, 0x8au, 0x39u, 0x06u, 0xb7u, 0xc1u, 0x42u, 0xf7u, 0x78u, 0x8cu, 0x45u, 0xeau, 0xd0u, 0xc9u, 0xbcu, 0x36u,
  0x92u, 0x48u, 0x3au, 0xd8u, 0x13u, 0x61u, 0x11u, 0x45u, 0xb4u, 0x1fu, 0x9cu, 0x01u, 0x2eu, 0xf2u, 0x87u, 0xbeu,
  0x8bu, 0xbfu, 0x93u, 0x19u, 0xcfu, 0x4bu, 0x91u, 0x84u, 0xdcu, 0x8eu, 0xffu, 0x83u, 0x58u, 0x9bu, 0xe9u, 0x0cu,
  0x54u, 0x81u, 0x14u, 0xacu, 0xfau, 0x5au, 0xbfu, 0x79u, 0x54u, 0xbfu, 0x9fu, 0x7au, 0xe5u, 0xb4u, 0x38u, 0xb5u
 };

/**
 * @brief Example value for private RSA exponent d.
 */
static const uint8_t privExp[RSA_KEY_BYTE_LENGTH] __attribute__ ((aligned (4))) = {
  0x15u, 0x5fu, 0xe6u, 0x60u, 0xcdu, 0xdeu, 0xaau, 0x17u, 0x1bu, 0x5eu, 0xd6u, 0xbdu, 0xd0u, 0x3bu, 0xb3u, 0x56u,
  0xe0u, 0xf6u, 0xe8u, 0x6bu, 0x5au, 0x3cu, 0x26u, 0xf3u, 0xceu, 0x7du, 0xaeu, 0x00u, 0x8cu, 0x4eu, 0x38u, 0xa9u,
  0xa9u, 0x7fu, 0xa5u, 0x97u, 0xb2u, 0xb9u, 0x0au, 0x45u, 0x10u, 0xd2u, 0x23u, 0x8du, 0x3fu, 0x15u, 0x8au, 0xb8u,
  0x91u, 0x97u, 0xfbu, 0x08u, 0xa5u, 0xb7u, 0x4cu, 0xfeu, 0x5cu, 0xc8u, 0xf1u, 0x3du, 0x47u, 0x09u, 0x62u, 0x91u,
  0xd0u, 0x05u, 0x38u, 0xaau, 0x58u, 0x93u, 0xd8u, 0x2du, 0xceu, 0x55u, 0xb3u, 0x64u, 0x8cu, 0x6au, 0x71u, 0x9au,
  0xe3u, 0x87u, 0xdeu, 0xe5u, 0x5eu, 0xc5u, 0xbeu, 0xf0u, 0x89u, 0x76u, 0x3du, 0xe7u, 0x1eu, 0x47u, 0x61u, 0xb7u,
  0x03u, 0xadu, 0x69u, 0x2eu, 0xd6u, 0x2du, 0x7cu, 0x1fu, 0x4fu, 0x0fu, 0xf0u, 0x03u, 0xc1u, 0x67u, 0xebu, 0x62u,
  0xd2u, 0xc6u, 0x79u, 0xccu, 0x6fu, 0x13u, 0xb9u, 0x87u, 0xa1u, 0x42u, 0xf1u, 0x37u, 0x7au, 0x40u, 0xbdu, 0xc0u,
  0xa0u, 0x36u, 0x60u, 0x72u, 0x94u, 0x40u, 0x14u, 0x63u, 0xa3u, 0x0eu, 0x82u, 0x91u, 0x2bu, 0x42u, 0x8au, 0x1du,
  0x3fu, 0x80u, 0xb5u, 0xd0u, 0xd3u, 0x3eu, 0xa8u, 0x4eu, 0x8bu, 0xb6u, 0x4cu, 0x36u, 0x22u, 0xb9u, 0xbeu, 0xe3u,
  0x56u, 0xf1u, 0x2cu, 0x6au, 0x19u, 0x0eu, 0x55u, 0x7bu, 0xbfu, 0x25u, 0xe1u, 0x10u, 0x80u, 0x7bu, 0x85u, 0xcau,
  0xd5u, 0x1bu, 0x39u, 0x87u, 0x57u, 0x08u, 0x06u, 0xbeu, 0x81u, 0xf3u, 0x71u, 0x3fu, 0x5du, 0x17u, 0x40u, 0x74u,
  0x99u, 0xa5u, 0xdeu, 0xdau, 0xc0u, 0xf3u, 0xe3u, 0xbcu, 0x79u, 0x96u, 0x35u, 0x95u, 0xf8u, 0xe0u, 0xcfu, 0x01u,
  0x29u, 0x1du, 0xc1u, 0x02u, 0x09u, 0xc0u, 0x6eu, 0xb6u, 0x0eu, 0x2eu, 0x9cu, 0x47u, 0xecu, 0x91u, 0x42u, 0xedu,
  0xa5u, 0xf3u, 0xb7u, 0x0au, 0xc6u, 0x7fu, 0x72u, 0xbfu, 0x52u, 0xb3u, 0x31u, 0x37u, 0xd1u, 0x49u, 0xb6u, 0xf6u,
  0x06u, 0xe4u, 0x59u, 0x61u, 0x7du, 0xaau, 0x8eu, 0x10u, 0x18u, 0xa8u, 0x14u, 0x1du, 0x89u, 0x4eu, 0xcau, 0xffu
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


MCUXCLEXAMPLE_FUNCTION(mcuxClRsa_Cipher_Crypt_RSAES_OAEP_example)
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
  mcuxClRsa_KeyData_Plain_t privKeyStruct = {
                              .modulus.pKeyEntryData = (uint8_t*)modulus,
                              .modulus.keyEntryLength = RSA_KEY_BYTE_LENGTH,
                              .exponent.pKeyEntryData = (uint8_t*)privExp,
                              .exponent.keyEntryLength = sizeof(privExp)
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
    /* mcuxClKey_Type_t type                  */ mcuxClKey_Type_Rsa_PrivatePlain_2048,
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

  uint32_t sizeOfPubKey = (uint32_t)sizeof(pubKeyStruct);
  MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(keyInitPub_result, keyInitPub_token, mcuxClKey_init(
    /* mcuxClSession_Handle_t session         */ session,
    MCUX_CSSL_ANALYSIS_START_SUPPRESS_POINTER_INCOMPATIBLE("The pointer pubKey is of the right type (mcuxClKey_Handle_t)")
    /* mcuxClKey_Handle_t key                 */ pubKey,
    MCUX_CSSL_ANALYSIS_STOP_SUPPRESS_POINTER_INCOMPATIBLE()
    /* mcuxClKey_Type_t type                  */ mcuxClKey_Type_Rsa_Public_2048,
    /* uint8_t * pKeyData                    */ (uint8_t *) &pubKeyStruct,
    /* uint32_t keyDataLength                */ sizeOfPubKey
  ));
  if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClKey_init) != keyInitPub_token) || (MCUXCLKEY_STATUS_OK != keyInitPub_result))
  {
      return MCUXCLEXAMPLE_STATUS_ERROR;
  }
  MCUX_CSSL_FP_FUNCTION_CALL_END();

  /**************************************************************************/
  /* Preparation: setup RSA OAEP encrypt mode with SHA-256                  */
  /**************************************************************************/

  /* Fill mode descriptor with the relevant data for the selected padding and hash algorithms */
  uint32_t cipherModeBytes[MCUXCLRSA_CIPHER_MODE_SIZE_IN_WORD];
  MCUX_CSSL_ANALYSIS_START_PATTERN_REINTERPRET_MEMORY_OF_OPAQUE_TYPES()
  mcuxClCipher_ModeDescriptor_t *pCipherMode = (mcuxClCipher_ModeDescriptor_t *) cipherModeBytes;
  MCUX_CSSL_ANALYSIS_STOP_PATTERN_REINTERPRET_MEMORY_OF_OPAQUE_TYPES()

  mcuxClRsa_CipherModeConstructor_RSAES_OAEP_Encrypt(
    MCUX_CSSL_ANALYSIS_START_SUPPRESS_POINTER_INCOMPATIBLE("The pointer pCipherMode points to an object of the right type, the cast was valid.")
    /* mcuxClCipher_ModeDescriptor_t * pCipherMode: */ pCipherMode,
    MCUX_CSSL_ANALYSIS_STOP_SUPPRESS_POINTER_INCOMPATIBLE()
    /* mcuxClHash_Algo_t hashAlgorithm: */ mcuxClHash_Algorithm_Sha256
    );

  /**************************************************************************/
  /* Encryption                                                             */
  /**************************************************************************/

  uint8_t encryptedData[RSA_KEY_BYTE_LENGTH];
  uint32_t encryptedSize = 0u;

  MCUXCLBUFFER_INIT_RO(plainDataBuf, session, plainData, INPUT_MESSAGE_LENGTH);
  MCUXCLBUFFER_INIT(encryptedDataBuf, session, encryptedData, RSA_KEY_BYTE_LENGTH);
  uint32_t sizeOfPlainData = (uint32_t)sizeof(plainData);
  MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(encrypt_result, encrypt_token, mcuxClCipher_crypt(
    /* mcuxClSession_Handle_t session          */ session,
    /* const mcuxClKey_Handle_t key            */ pubKey,
    /* mcuxClCipher_Mode_t mode                */ pCipherMode,
    MCUX_CSSL_ANALYSIS_START_PATTERN_NULL_POINTER_CONSTANT()
    /* mcuxCl_InputBuffer_t pIv                */ NULL, /* label for OAEP decoding, set to NULL if no label is provided */
    MCUX_CSSL_ANALYSIS_STOP_PATTERN_NULL_POINTER_CONSTANT()
    /* uint32_t ivLength                      */ RSA_OAEP_LABEL_LENGTH, /* label length */
    /* mcuxCl_InputBuffer_t pIn                */ plainDataBuf,
    /* uint32_t inLength                      */ sizeOfPlainData,
    /* mcuxCl_Buffer_t pOut                    */ encryptedDataBuf,
    /* uint32_t * const pOutLength            */ &encryptedSize
  ));
  if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClCipher_crypt) != encrypt_token) || (MCUXCLCIPHER_STATUS_OK != encrypt_result))
  {
      return MCUXCLEXAMPLE_STATUS_ERROR;
  }
  MCUX_CSSL_FP_FUNCTION_CALL_END();

  if(encryptedSize != sizeof(encryptedData))
  {
    return MCUXCLEXAMPLE_STATUS_ERROR;
  }

  /**************************************************************************/
  /* Preparation: setup RSA OAEP decrypt mode with SHA-256                  */
  /**************************************************************************/

  mcuxClRsa_CipherModeConstructor_RSAES_OAEP_Decrypt(
    /* mcuxClCipher_ModeDescriptor_t * pCipherMode: */ pCipherMode,
    /* mcuxClHash_Algo_t hashAlgorithm: */ mcuxClHash_Algorithm_Sha256
    );

  /**************************************************************************/
  /* Decryption                                                             */
  /**************************************************************************/

  uint32_t decryptedSize = 0u;
  uint8_t decryptedData[INPUT_MESSAGE_LENGTH];
  MCUXCLBUFFER_INIT(decryptedDataBuf, session, decryptedData, INPUT_MESSAGE_LENGTH);
  MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(decrypt_result, decrypt_token, mcuxClCipher_crypt(
    /* mcuxClSession_Handle_t session         */ session,
    /* const mcuxClKey_Handle_t key           */ privKey,
    /* mcuxClCipher_Mode_t mode               */ pCipherMode,
    MCUX_CSSL_ANALYSIS_START_PATTERN_NULL_POINTER_CONSTANT()
    /* mcuxCl_InputBuffer_t pIv               */ NULL, /* label for OAEP decoding, set to NULL if no label is provided */
    MCUX_CSSL_ANALYSIS_STOP_PATTERN_NULL_POINTER_CONSTANT()
    /* uint32_t ivLength                     */ RSA_OAEP_LABEL_LENGTH, /* label length */
    /* mcuxCl_InputBuffer_t pIn               */ (mcuxCl_InputBuffer_t)encryptedDataBuf,
    /* uint32_t inLength                     */ encryptedSize,
    /* mcuxCl_Buffer_t pOut                   */ decryptedDataBuf,
    /* uint32_t * const pOutLength           */ &decryptedSize
  ));
  if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClCipher_crypt) != decrypt_token) || (MCUXCLCIPHER_STATUS_OK != decrypt_result))
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
