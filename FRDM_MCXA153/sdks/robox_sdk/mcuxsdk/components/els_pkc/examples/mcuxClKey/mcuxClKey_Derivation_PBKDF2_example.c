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
 * @file  mcuxClKey_Derivation_PBKDF2_example.c
 * @brief Example for the mcuxClKey component, showing a KDF according to the
 *        RFC2898 Standard (PBKDF2) using Hmac-Sha1
 */

#include <mcuxClKey.h>
#include <mcuxClHash.h>
#include <mcuxClHashModes.h>
#include <mcuxClHmac.h>
#include <mcuxClCore_Examples.h>
#include <mcuxClCore_Macros.h>
#include <mcuxClSession.h>
#include <mcuxClCore_FunctionIdentifiers.h> // Code flow protection
#include <mcuxClAes.h>
#include <mcuxClExample_Session_Helper.h>
#include <mcuxClExample_RNG_Helper.h>
#include <mcuxClEls.h>
#include <mcuxClExample_ELS_Helper.h>
/* Test vectors from RFC 6070 */


static const uint8_t password[8] = {0x70u, 0x61u, 0x73u, 0x73u, 0x77u, 0x6Fu, 0x72u, 0x64u};

static const uint8_t salt[4] = {0x73u, 0x61u, 0x6Cu, 0x74u};

static const uint8_t expected[20] =
{
    0x4bu, 0x00u, 0x79u, 0x01u, 0xb7u, 0x65u, 0x48u, 0x9au,
    0xbeu, 0xadu, 0x49u, 0xd9u, 0x26u, 0xf7u, 0x21u, 0xd0u,
    0x65u, 0xa4u, 0x29u, 0xc1u
};

static const uint32_t iterations = 4096u;

/** Performs an example key derivation using the mcuxClKey component.
 * @retval MCUXCLEXAMPLE_STATUS_OK    The example code completed successfully
 * @retval MCUXCLEXAMPLE_STATUS_ERROR The example code failed */
MCUXCLEXAMPLE_FUNCTION(mcuxClKey_Derivation_PBKDF2_example)
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

    MCUXCLEXAMPLE_ALLOCATE_AND_INITIALIZE_SESSION(session, MCUXCLEXAMPLE_MAX_WA(MCUXCLKEY_DERIVATION_PBKDF2_CPU_WA_SIZE, MCUXCLRANDOMMODES_NCINIT_WACPU_SIZE), 0u);

    /* Initialize the PRNG */
    MCUXCLEXAMPLE_INITIALIZE_PRNG(session);

    /**************************************************************************/
    /* Key setup                                                              */
    /**************************************************************************/

    /* Create and initialize key descriptor structure. */
    uint32_t passwordDesc[MCUXCLKEY_DESCRIPTOR_SIZE_IN_WORDS];
    mcuxClKey_Handle_t passwordHandle = (mcuxClKey_Handle_t) passwordDesc;

    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(resultKeyInit1, tokenKeyInit1, mcuxClKey_init(
      /* mcuxClSession_Handle_t session         */ session,
      MCUX_CSSL_ANALYSIS_START_SUPPRESS_POINTER_INCOMPATIBLE("The pointer passwordHandle points to an object of the right type, the cast was valid.")
      /* mcuxClKey_Handle_t key                 */ passwordHandle,
      MCUX_CSSL_ANALYSIS_STOP_SUPPRESS_POINTER_INCOMPATIBLE()
      /* mcuxClKey_Type_t type                  */ mcuxClKey_Type_Hmac_variableLength,    /* size of password does not necessarily fit any fixed size key type */
      /* const uint8_t * pKeyData              */ password,
      /* uint32_t keyDataLength                */ sizeof(password)
    ));
    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClKey_init) != tokenKeyInit1) || (MCUXCLKEY_STATUS_OK != resultKeyInit1))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();


    /* Key buffer for the key in memory. */
    uint32_t key_buffer[MCUXCLCORE_NUM_OF_CPUWORDS_CEIL(sizeof(password))];
    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(resultKeyLoadMemory, tokenKeyLoadMemory, mcuxClKey_loadMemory(session, passwordHandle, key_buffer));
    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClKey_loadMemory) != tokenKeyLoadMemory) || (MCUXCLKEY_STATUS_OK != resultKeyLoadMemory))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();

    /* Set up input parameter structures. */
    MCUXCLBUFFER_INIT_RO(saltBuf, session, salt, sizeof(salt));
    struct mcuxClKey_DerivationInput inputSalt = {.input=saltBuf, .size=sizeof(salt)};

    mcuxClKey_DerivationInput_t inputs[] = {inputSalt};

    /* Set up output structure. */
    uint8_t derivedKeyData[sizeof(expected)];

    /* Create and initialize derivedKey descriptor structure. */
    uint32_t derivedKeyDesc[MCUXCLKEY_DESCRIPTOR_SIZE_IN_WORDS];
    mcuxClKey_Handle_t derivedKey = (mcuxClKey_Handle_t) derivedKeyDesc;

    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(resultKeyInit2, tokenKeyInit2, mcuxClKey_init(
      /* mcuxClSession_Handle_t session         */ session,
      MCUX_CSSL_ANALYSIS_START_SUPPRESS_POINTER_INCOMPATIBLE("The pointer derivedKey points to an object of the right type, the cast was valid.")
      /* mcuxClKey_Handle_t key                 */ derivedKey,
      MCUX_CSSL_ANALYSIS_STOP_SUPPRESS_POINTER_INCOMPATIBLE()
      /* mcuxClKey_Type_t type                  */ mcuxClKey_Type_Hmac_variableLength,
      /* const uint8_t * pKeyData              */ derivedKeyData,
      /* uint32_t keyDataLength                */ sizeof(expected)
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
    mcuxClMac_CustomMode_t hmacSha1 = (mcuxClMac_CustomMode_t) hmacModeDescBuffer;

    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(hashCreateMode_result, hashCreateMode_token, mcuxClHmac_createHmacMode(
      MCUX_CSSL_ANALYSIS_START_SUPPRESS_POINTER_INCOMPATIBLE("The pointer hmacSha1 points to an object of the right type, the cast was valid.")
      /* mcuxClMac_CustomMode_t mode:       */ hmacSha1,
      MCUX_CSSL_ANALYSIS_STOP_SUPPRESS_POINTER_INCOMPATIBLE()
      /* mcuxClHash_Algo_t hashAlgorithm:   */ mcuxClHash_Algorithm_Sha1)
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

    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(resultModeConstruct, tokenModeConstruct, mcuxClKey_Derivation_ModeConstructor_PBKDF2(
      MCUX_CSSL_ANALYSIS_START_SUPPRESS_POINTER_INCOMPATIBLE("The pointer pDerivationMode is of the right type (mcuxClKey_DerivationMode_t *), the cast was valid.")
      /* mcuxClKey_DerivationMode_t *                      */ pDerivationMode,
      MCUX_CSSL_ANALYSIS_STOP_SUPPRESS_POINTER_INCOMPATIBLE()
      /* const mcuxClKey_DerivationAlgorithmDescriptor_t * */ mcuxClKey_DerivationAlgorithm_PBKDF2,
      /* mcuxClMac_Mode_t                                  */ hmacSha1,
      /* uint32_t                                         */ iterations
    ));

    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClKey_Derivation_ModeConstructor_PBKDF2) != tokenModeConstruct) || (MCUXCLKEY_STATUS_OK != resultModeConstruct))
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
      /* mcuxClKey_Handle_t derivationKey        */ passwordHandle,
      /* mcuxClKey_DerivationInput_t inputs[]    */ inputs,
      /* uint32_t numberOfInputs                */ 1u,
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
    if(!mcuxClCore_assertEqual(derivedKeyData, expected, sizeof(expected)))
    {
        return MCUXCLEXAMPLE_STATUS_FAILURE;
    }

    /**************************************************************************/
    /* Cleanup                                                                */
    /**************************************************************************/

    /* Flush the key. */
    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(resultFlush, tokenFlush, mcuxClKey_flush(session, passwordHandle));
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
