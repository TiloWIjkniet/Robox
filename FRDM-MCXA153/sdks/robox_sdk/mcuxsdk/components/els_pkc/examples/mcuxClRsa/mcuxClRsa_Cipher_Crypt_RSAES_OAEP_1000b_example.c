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
 * @file  mcuxClRsa_Cipher_Crypt_RSAES_OAEP_1000b_example.c
 * @brief Example for the @ref mcuxCRsa component.
 *
 * @example mcuxClRsa_Cipher_Crypt_RSAES_OAEP_1000b_example.c
 * @brief   Example for the @ref mcuxCRsa component realize RSA OAEP encrypt/decrypt operation
 *          using @ref mcuxClCipher_crypt.
 * \details Example for the @ref mcuxCRsa component realize encrypt/decrypt operation using
 *          @ref mcuxClCipher_crypt for:
 *          - OAEP en-/decryption mode with SHA-256,
 *          - 1000-bit RSA key,
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

#define RSA_KEY_BIT_LENGTH         (1000u)                   ///< The example uses a 1000-bit key
#define RSA_KEY_BYTE_LENGTH        (RSA_KEY_BIT_LENGTH / 8u) ///< Converting the key-bitlength to bytelength
#define RSA_PUBLIC_EXP_BYTE_LENGTH (3u)                      ///< The public exponent has a length of three bytes
#define RSA_OAEP_LABEL_LENGTH      (0u)                      ///< The label length is set to 0 in this example
#define INPUT_MESSAGE_LENGTH       (48u)                     ///< Arbitrary size of the message to be encrypted/decrypted

/**
 * @brief Example value for public RSA modulus N.
 */
static const uint8_t modulus[RSA_KEY_BYTE_LENGTH] __attribute__ ((aligned (4))) = {
  0xB2u, 0xBFu, 0xB1u, 0x51u, 0xCDu, 0x41u, 0x28u, 0xC4u, 0xFCu, 0x65u, 0xEEu, 0xCBu, 0x78u, 0x7Au, 0x2Fu, 0xDDu,
  0xE6u, 0x54u, 0x26u, 0x33u, 0xFEu, 0xA4u, 0x70u, 0x42u, 0xD3u, 0xCDu, 0x61u, 0x18u, 0xC1u, 0xF4u, 0x25u, 0xB0u,
  0x17u, 0x54u, 0x2Eu, 0x87u, 0x5Eu, 0x80u, 0x61u, 0x77u, 0x45u, 0xC9u, 0xDCu, 0x85u, 0x8Eu, 0x16u, 0x8Eu, 0xC6u,
  0x01u, 0xF0u, 0xB6u, 0x2Au, 0x84u, 0xE2u, 0xF8u, 0xD6u, 0x56u, 0x3Bu, 0xA8u, 0x06u, 0x77u, 0x5Au, 0x84u, 0xCEu,
  0x63u, 0x3Du, 0xBFu, 0x21u, 0xB2u, 0x6Bu, 0xC2u, 0x2Fu, 0xCCu, 0xA7u, 0x02u, 0x23u, 0x26u, 0x60u, 0x8Du, 0xC6u,
  0x80u, 0xC4u, 0x77u, 0x24u, 0x5Eu, 0x92u, 0xFCu, 0x01u, 0x07u, 0x1Cu, 0x35u, 0x97u, 0xB7u, 0x0Du, 0xC0u, 0x74u,
  0x78u, 0x7Cu, 0x27u, 0xACu, 0x5Au, 0x95u, 0xF8u, 0x7Bu, 0x8Au, 0xAAu, 0x7Bu, 0x9Fu, 0x71u, 0xB2u, 0xD8u, 0x42u,
  0xF9u, 0x65u, 0x4Fu, 0x0Fu, 0x57u, 0xDDu, 0x25u, 0xAEu, 0x64u, 0xCAu, 0x7Cu, 0x59u, 0x21u
 };

/**
 * @brief Example value for private RSA exponent d.
 */
static const uint8_t privExp[RSA_KEY_BYTE_LENGTH] __attribute__ ((aligned (4))) = {
  0x28u, 0xB2u, 0xF0u, 0xE2u, 0xD9u, 0x43u, 0x3Eu, 0xCFu, 0x2Bu, 0x50u, 0xE2u, 0x40u, 0x3Du, 0xDCu, 0x44u, 0x4Du,
  0xD8u, 0x05u, 0xCCu, 0xF5u, 0x05u, 0xC3u, 0xD7u, 0x33u, 0xC0u, 0x1Au, 0x01u, 0x43u, 0xABu, 0xD5u, 0xB5u, 0x47u,
  0x14u, 0xE8u, 0xBBu, 0xF7u, 0x62u, 0x93u, 0x04u, 0x9Eu, 0x2Du, 0xABu, 0xBAu, 0xA4u, 0x46u, 0x27u, 0xE8u, 0xB6u,
  0x38u, 0xF6u, 0xDFu, 0xE3u, 0x6Au, 0x82u, 0x6Bu, 0x7Au, 0x12u, 0x04u, 0x5Fu, 0x4Bu, 0xA9u, 0x9Du, 0x52u, 0x82u,
  0xCFu, 0xC7u, 0x7Cu, 0xF8u, 0x77u, 0xBFu, 0xDEu, 0x6Eu, 0x5Du, 0x40u, 0x97u, 0x8Bu, 0xB8u, 0x41u, 0xE5u, 0xE4u,
  0x76u, 0x93u, 0xB3u, 0x3Du, 0xF0u, 0xF1u, 0xC2u, 0x59u, 0xBAu, 0x3Bu, 0x16u, 0xC1u, 0x80u, 0x3Eu, 0x5Cu, 0xE6u,
  0xD5u, 0x0Eu, 0x97u, 0xAFu, 0xB6u, 0xBDu, 0x99u, 0x20u, 0xFFu, 0xBDu, 0xD2u, 0x51u, 0xB1u, 0x04u, 0x83u, 0xCBu,
  0xADu, 0x3Eu, 0xD3u, 0xE0u, 0x0Bu, 0xCCu, 0xDFu, 0x9Bu, 0xF5u, 0xEAu, 0x28u, 0x19u, 0xE1u
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
  0x63u, 0x64u, 0x65u, 0x66u, 0x67u, 0x68u, 0x69u, 0x6Au, 0x6Bu, 0x6Cu, 0x6Du, 0x6Eu, 0x6Fu, 0x70u, 0x71u, 0x72u
};


MCUXCLEXAMPLE_FUNCTION(mcuxClRsa_Cipher_Crypt_RSAES_OAEP_1000b_example)
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

  /* Initialize RSA private plain key type descriptor */
  uint32_t keyTypeDesc_RsaPrivatePlain_1000[MCUXCLKEY_TYPEDESCRIPTOR_SIZE_IN_WORDS];
  MCUX_CSSL_ANALYSIS_START_PATTERN_REINTERPRET_MEMORY_OF_OPAQUE_TYPES()
  mcuxClKey_TypeDescriptor_t *pKeyTypeDesc_RsaPrivatePlain_1000 = (mcuxClKey_TypeDescriptor_t *) keyTypeDesc_RsaPrivatePlain_1000;
  MCUX_CSSL_ANALYSIS_STOP_PATTERN_REINTERPRET_MEMORY_OF_OPAQUE_TYPES()

  MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(kt_priv_status, kt_priv_token, mcuxClRsa_PrivatePlain_KeyType_ModeConstructor(
    MCUX_CSSL_ANALYSIS_START_SUPPRESS_POINTER_INCOMPATIBLE("The pointer pKeyTypeDesc_RsaPrivatePlain_1000 points to an object of the right type, the cast was valid.")
    /* mcuxClKey_TypeDescriptor_t * pKeyType */ pKeyTypeDesc_RsaPrivatePlain_1000,
    MCUX_CSSL_ANALYSIS_STOP_SUPPRESS_POINTER_INCOMPATIBLE()
    /* mcuxClKey_Size_t keySize              */ RSA_KEY_BYTE_LENGTH
    ));
  if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClRsa_PrivatePlain_KeyType_ModeConstructor) != kt_priv_token) || (MCUXCLRSA_STATUS_OK != kt_priv_status))
  {
      return MCUXCLEXAMPLE_STATUS_ERROR;
  }
  MCUX_CSSL_FP_FUNCTION_CALL_END();

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
    /* mcuxClKey_Type_t type                  */ (mcuxClKey_Type_t) &keyTypeDesc_RsaPrivatePlain_1000,
    /* uint8_t * pKeyData                    */ (uint8_t *) &privKeyStruct,
    /* uint32_t keyDataLength                */ sizeOfPrivKey
  ));
  if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClKey_init) != keyInitPriv_token) || (MCUXCLKEY_STATUS_OK != keyInitPriv_result))
  {
      return MCUXCLEXAMPLE_STATUS_ERROR;
  }
  MCUX_CSSL_FP_FUNCTION_CALL_END();

  /* Initialize RSA private plain key type descriptor */
  uint32_t keyTypeDesc_RsaPublic_1000[MCUXCLKEY_TYPEDESCRIPTOR_SIZE_IN_WORDS];
  MCUX_CSSL_ANALYSIS_START_PATTERN_REINTERPRET_MEMORY_OF_OPAQUE_TYPES()
  mcuxClKey_TypeDescriptor_t *pkeyTypeDesc_RsaPublic_1000 = (mcuxClKey_TypeDescriptor_t *) keyTypeDesc_RsaPublic_1000;
  MCUX_CSSL_ANALYSIS_STOP_PATTERN_REINTERPRET_MEMORY_OF_OPAQUE_TYPES()

  MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(kt_pub_status, kt_pub_token, mcuxClRsa_Public_KeyType_ModeConstructor(
    MCUX_CSSL_ANALYSIS_START_SUPPRESS_POINTER_INCOMPATIBLE("The pointer pkeyTypeDesc_RsaPublic_1000 points to an object of the right type, the cast was valid.")
    /* mcuxClKey_TypeDescriptor_t * pKeyType */ pkeyTypeDesc_RsaPublic_1000,
    MCUX_CSSL_ANALYSIS_STOP_SUPPRESS_POINTER_INCOMPATIBLE()
    /* mcuxClKey_Size_t keySize              */ RSA_KEY_BYTE_LENGTH
    ));
  if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClRsa_Public_KeyType_ModeConstructor) != kt_pub_token) || (MCUXCLRSA_STATUS_OK != kt_pub_status))
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
    /* mcuxClKey_Type_t type                  */ (mcuxClKey_Type_t) &keyTypeDesc_RsaPublic_1000,
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

  uint32_t sizeOfPlainData = (uint32_t)sizeof(plainData);
  MCUXCLBUFFER_INIT_RO(plainDataBuf, session, plainData, INPUT_MESSAGE_LENGTH);
  MCUXCLBUFFER_INIT(encryptedDataBuf, session, encryptedData, RSA_KEY_BYTE_LENGTH);
  MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(encrypt_result, encrypt_token, mcuxClCipher_crypt(
    /* mcuxClSession_Handle_t session          */ session,
    /* const mcuxClKey_Handle_t key            */ pubKey,
    /* mcuxClCipher_Mode_t mode                */ pCipherMode,
    MCUX_CSSL_ANALYSIS_START_PATTERN_NULL_POINTER_CONSTANT()
    /* mcuxCl_InputBuffer_t pIv                */ NULL, /* label for OAEP decoding, set to NULL if no label is provided */
    MCUX_CSSL_ANALYSIS_STOP_PATTERN_NULL_POINTER_CONSTANT()
    /* uint32_t ivLength                      */ (uint32_t)RSA_OAEP_LABEL_LENGTH, /* label length */
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
    /* uint32_t ivLength                     */ (uint32_t)RSA_OAEP_LABEL_LENGTH, /* label length */
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
