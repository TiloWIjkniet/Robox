/*--------------------------------------------------------------------------*/
/* Copyright 2023-2024 NXP                                                  */
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
 * @file  mcuxClKey_Derivation_SP800_56C_example.c
 * @brief Example for the mcuxClKey component, showing a KDF according to the
 *        NIST SP800-56C Standard in OneStep mode Using Hmac-Sha512
 */

#include <mcuxClKey.h>
#include <mcuxClHash.h>
#include <mcuxClHashModes.h>
#include <mcuxClHmac.h>
#include <mcuxClCore_Examples.h>
#include <mcuxClSession.h>
#include <mcuxClCore_FunctionIdentifiers.h> // Code flow protection
#include <mcuxClAes.h>
#include <mcuxClExample_Session_Helper.h>
#include <mcuxClExample_RNG_Helper.h>
#include <mcuxClEls.h>
#include <mcuxClExample_ELS_Helper.h>
#include <mcuxClExample_Key_Helper.h>

MCUX_CSSL_ANALYSIS_START_PATTERN_HYPERLINK_IN_COMMENTS()
/* Official test vectors are not available, thus taken from here:
 * https://github.com/patrickfav/singlestep-kdf/wiki/NIST-SP-800-56C-Rev1:-Non-Official-Test-Vectors
 */
MCUX_CSSL_ANALYSIS_STOP_PATTERN_HYPERLINK_IN_COMMENTS()

/* HMAC-SHA512 with salt */
static const uint8_t sharedSecret[16] = {
    0x8eu, 0x5cu, 0xd5u, 0xf6u, 0xaeu, 0x55u, 0x8fu, 0xfau,
    0x04u, 0xcdu, 0xa2u, 0xfau, 0xd9u, 0x4du, 0xd6u, 0x16u
};

static const uint8_t fixedInfo[12] = {
    0x4au, 0x43u, 0x30u, 0x18u, 0xe5u, 0x1cu, 0x09u, 0xbbu,
    0xd6u, 0x13u, 0x26u, 0xbbu
};

static const uint8_t salt[16] = {
    0x6eu, 0xd9u, 0x3bu, 0x6fu, 0xe5u, 0xb3u, 0x50u, 0x2bu,
    0xb4u, 0x2bu, 0x4cu, 0x0fu, 0xcbu, 0x13u, 0x36u, 0x62u
};

static const uint8_t expectedDerivedKey[68] = {
    0x29u, 0x5du, 0xfbu, 0xebu, 0x54u, 0xecu, 0x0fu, 0xe2u,
    0x4eu, 0xceu, 0x32u, 0xf5u, 0xb8u, 0x7cu, 0x85u, 0x3eu,
    0x69u, 0x9au, 0x62u, 0xe3u, 0x9du, 0x9cu, 0x9eu, 0xe6u,
    0xeeu, 0x78u, 0xf8u, 0xb9u, 0xa0u, 0xeeu, 0x50u, 0xa3u,
    0x6au, 0x82u, 0xe6u, 0x06u, 0x2cu, 0x95u, 0xedu, 0x53u,
    0xbcu, 0x36u, 0x67u, 0x00u, 0xe2u, 0xd0u, 0xe0u, 0x93u,
    0xbfu, 0x75u, 0x2eu, 0xeau, 0x42u, 0x99u, 0x47u, 0x2eu,
    0xebu, 0x4cu, 0x16u, 0xc0u, 0x65u, 0xa6u, 0x76u, 0x8cu,
    0x93u, 0xeeu, 0x87u, 0x11u,
};

/** Performs an example key derivation using the mcuxClKey component.
 * @retval MCUXCLEXAMPLE_STATUS_OK    The example code completed successfully
 * @retval MCUXCLEXAMPLE_STATUS_ERROR The example code failed */
MCUXCLEXAMPLE_FUNCTION(mcuxClKey_Derivation_SP800_56C_example)
{
    /**************************************************************************/
    /* Preparation                                                            */
    /**************************************************************************/

    /** Initialize ELS, Enable the ELS **/
    if(!mcuxClExample_Els_Init(MCUXCLELS_RESET_DO_NOT_CANCEL))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }

    /* Initialize session */
    mcuxClSession_Descriptor_t sessionDesc;
    mcuxClSession_Handle_t session = &sessionDesc;

    MCUXCLEXAMPLE_ALLOCATE_AND_INITIALIZE_SESSION(session, MCUXCLEXAMPLE_MAX_WA(MCUXCLKEY_DERIVATION_NIST_SP800_56C_CPU_WA_SIZE, MCUXCLRANDOMMODES_NCINIT_WACPU_SIZE), 0u);

    /* Initialize the PRNG */
    MCUXCLEXAMPLE_INITIALIZE_PRNG(session);

    /**************************************************************************/
    /* Key setup                                                              */
    /**************************************************************************/

    /* Create and initialize key descriptor structure. */
    uint32_t sharedSecretDesc[MCUXCLKEY_DESCRIPTOR_SIZE_IN_WORDS];
    MCUX_CSSL_ANALYSIS_START_PATTERN_REINTERPRET_MEMORY_OF_OPAQUE_TYPES()
    mcuxClKey_Handle_t sharedSecretHandle = (mcuxClKey_Handle_t) sharedSecretDesc;
    MCUX_CSSL_ANALYSIS_STOP_PATTERN_REINTERPRET_MEMORY_OF_OPAQUE_TYPES()

    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(resultKeyInit1, tokenKeyInit1, mcuxClKey_init(
      /* mcuxClSession_Handle_t session         */ session,
      MCUX_CSSL_ANALYSIS_START_SUPPRESS_POINTER_INCOMPATIBLE("The pointer sharedSecretHandle points to an object of the right type, the cast was valid.")
      /* mcuxClKey_Handle_t key                 */  sharedSecretHandle,
      MCUX_CSSL_ANALYSIS_STOP_SUPPRESS_POINTER_INCOMPATIBLE()
      /* mcuxClKey_Type_t type                  */ mcuxClKey_Type_Aes128,
      /* const uint8_t * pKeyData              */ (const uint8_t *) sharedSecret,
      /* uint32_t keyDataLength                */ sizeof(sharedSecret)
    ));

    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClKey_init) != tokenKeyInit1) || (MCUXCLKEY_STATUS_OK != resultKeyInit1))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();

    /* Load the key to make sure it is aligned to the word boundary for ELS*/
    /* Key buffer for the key in memory. */
    uint32_t key_buffer[MCUXCLAES_AES128_KEY_SIZE_IN_WORDS];

    /* load key into destination memory buffer */
    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(resultLoadMem, tokenLoadMem, mcuxClKey_loadMemory(
      /* mcuxClSession_Handle_t pSession:   */ session,
      /* mcuxClKey_Handle_t key:            */ sharedSecretHandle,
      /* uint32_t * dstData:               */ key_buffer));
    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClKey_loadMemory) != tokenLoadMem) || (MCUXCLKEY_STATUS_OK != resultLoadMem))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();

    /* Set up input parameter structures. */
    MCUXCLBUFFER_INIT_RO(fixedInfoBuf, session, fixedInfo, sizeof(fixedInfo));
    MCUXCLBUFFER_INIT_RO(saltBuf, session, salt, sizeof(salt));
    struct mcuxClKey_DerivationInput inputFixedInfo = {.input=fixedInfoBuf, .size=sizeof(fixedInfo)};
    struct mcuxClKey_DerivationInput inputSalt = {.input=saltBuf, .size=sizeof(salt)};

    mcuxClKey_DerivationInput_t inputs[] = {inputFixedInfo, inputSalt};

    /* Set up output structure. */
    uint8_t derivedKeyBuf[MCUXCLAES_AES256_KEY_SIZE];

    /* Create and initialize derivedKey descriptor structure. */
    uint32_t derivedKeyDesc[MCUXCLKEY_DESCRIPTOR_SIZE_IN_WORDS];
    MCUX_CSSL_ANALYSIS_START_PATTERN_REINTERPRET_MEMORY_OF_OPAQUE_TYPES()
    mcuxClKey_Handle_t derivedKey = (mcuxClKey_Handle_t) derivedKeyDesc;
    MCUX_CSSL_ANALYSIS_STOP_PATTERN_REINTERPRET_MEMORY_OF_OPAQUE_TYPES()

    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(resultKeyInit2, tokenKeyInit2, mcuxClKey_init(
      /* mcuxClSession_Handle_t session         */ session,
      MCUX_CSSL_ANALYSIS_START_SUPPRESS_POINTER_INCOMPATIBLE("The pointer derivedKey points to an object of the right type, the cast was valid.")
      /* mcuxClKey_Handle_t key                 */ derivedKey,
      MCUX_CSSL_ANALYSIS_STOP_SUPPRESS_POINTER_INCOMPATIBLE()
      /* mcuxClKey_Type_t type                  */ mcuxClKey_Type_Aes256,
      /* const uint8_t * pKeyData              */ derivedKeyBuf,
      /* uint32_t keyDataLength                */ sizeof(derivedKeyBuf)
    ));

    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClKey_init) != tokenKeyInit2) || (MCUXCLKEY_STATUS_OK != resultKeyInit2))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();

    /**************************************************************************/
    /* Create Hmac mode and Derivation mode                                   */
    /**************************************************************************/

    uint32_t hmacModeDescBuffer[MCUXCLHMAC_HMAC_MODE_DESCRIPTOR_SIZE_IN_WORDS];
    MCUX_CSSL_ANALYSIS_START_PATTERN_REINTERPRET_MEMORY_OF_OPAQUE_TYPES()
    mcuxClMac_CustomMode_t hmacSha512 = (mcuxClMac_CustomMode_t) hmacModeDescBuffer;
    MCUX_CSSL_ANALYSIS_STOP_PATTERN_REINTERPRET_MEMORY_OF_OPAQUE_TYPES()

    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(hashCreateMode_result, hashCreateMode_token, mcuxClHmac_createHmacMode(
      MCUX_CSSL_ANALYSIS_START_SUPPRESS_POINTER_INCOMPATIBLE("The pointer hmacSha512 is of the right type (mcuxClMac_CustomMode_t)")
      /* mcuxClMac_CustomMode_t mode:     */ hmacSha512,
      MCUX_CSSL_ANALYSIS_STOP_SUPPRESS_POINTER_INCOMPATIBLE()
    /* mcuxClHash_Algo_t hashAlgorithm:   */ mcuxClHash_Algorithm_Sha512)
    );

    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClHmac_createHmacMode) != hashCreateMode_token) || (MCUXCLMAC_STATUS_OK != hashCreateMode_result))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();

    uint32_t derivationModeDescBuffer[MCUXCLKEY_DERIVATION_MODE_DESCRIPTOR_SIZE_IN_WORDS];
    MCUX_CSSL_ANALYSIS_START_PATTERN_REINTERPRET_MEMORY_OF_OPAQUE_TYPES()
    mcuxClKey_DerivationMode_t * pDerivationMode = (mcuxClKey_DerivationMode_t *) derivationModeDescBuffer;
    MCUX_CSSL_ANALYSIS_STOP_PATTERN_REINTERPRET_MEMORY_OF_OPAQUE_TYPES()

    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(resultModeConstruct, tokenModeConstruct, mcuxClKey_Derivation_ModeConstructor_NIST_SP800_56C(
      MCUX_CSSL_ANALYSIS_START_SUPPRESS_POINTER_INCOMPATIBLE("The pointer pDerivationMode is of the right type (mcuxClKey_DerivationMode_t *), the cast was valid.")
      /* mcuxClKey_DerivationMode_t *                      */ pDerivationMode,
      MCUX_CSSL_ANALYSIS_STOP_SUPPRESS_POINTER_INCOMPATIBLE()
      /* const mcuxClKey_DerivationAlgorithmDescriptor_t * */ mcuxClKey_DerivationAlgorithm_NIST_SP800_56C_OneStep,
      /* mcuxClMac_Mode_t                                  */ hmacSha512, // use this when using mac function as PRF
      MCUX_CSSL_ANALYSIS_START_PATTERN_NULL_POINTER_CONSTANT()
      /* mcuxClHash_Algo_t                                 */ NULL, // use this when using hash function as PRF
      MCUX_CSSL_ANALYSIS_STOP_PATTERN_NULL_POINTER_CONSTANT()
      /* uint32_t                                         */ 0u // no options for this mode
    ));

    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClKey_Derivation_ModeConstructor_NIST_SP800_56C) != tokenModeConstruct) || (MCUXCLKEY_STATUS_OK != resultModeConstruct))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();

    /**************************************************************************/
    /* Key Derivation                                                         */
    /**************************************************************************/

    /* Call key derivation function. */
    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(resultDeriv, tokenDeriv, mcuxClKey_derivation(
      /* mcuxClSession_Handle_t pSession         */ session,
      /* mcuxClKey_Derivation_t derivationMode   */ pDerivationMode,
      /* mcuxClKey_Handle_t derivationKey        */ sharedSecretHandle,
      /* mcuxClKey_DerivationInput_t inputs[]    */ inputs,
      /* uint32_t numberOfInputs                */ 2u,
      /* mcuxClKey_Handle_t derivedKey           */ derivedKey
    ));

    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClKey_derivation) != tokenDeriv) || (MCUXCLKEY_STATUS_OK != resultDeriv))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();


    /**************************************************************************/
    /* The derivedKey could now be used for a cryptographic operation.        */
    /**************************************************************************/


    /**************************************************************************/
    /* Verification                                                           */
    /**************************************************************************/

    /* Compare the derived key to the reference value. */
    if(!mcuxClCore_assertEqual(derivedKeyBuf, expectedDerivedKey, MCUXCLAES_AES256_KEY_SIZE))
    {
        return MCUXCLEXAMPLE_STATUS_FAILURE;
    }

    /**************************************************************************/
    /* Cleanup                                                                */
    /**************************************************************************/

    /* Flush the key. */
    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(resultFlush, tokenFlush, mcuxClKey_flush(session, sharedSecretHandle));

    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClKey_flush) != tokenFlush) || (MCUXCLKEY_STATUS_OK != resultFlush))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();

    /** Destroy Session and cleanup Session **/
    if(!mcuxClExample_Session_Clean(session))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }

    if(!mcuxClExample_Els_Disable())
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }

    return MCUXCLEXAMPLE_STATUS_OK;
}
