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
 * @file  mcuxClKey_Derivation_SP800_56C_twostep_cmac_example.c
 * @brief Example for the mcuxClKey component, showing a KDF according to the
 *        NIST SP800-56C Standard in TwoStep mode Using CMAC-AES128
 */

#include <mcuxClKey.h>
#include <mcuxClHash.h>
#include <mcuxClMacModes.h>
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

/* CMAC-AES128 secret data with default salt (all zeros aes128 key size) */
static const uint8_t sharedSecret[16] = {
    0x61u, 0x62u, 0x63u, 0x64u, 0x65u, 0x66u, 0x67u, 0x68u, 0x69u, 0x70u, 0x71u, 0x72u, 0x73u, 0x74u, 0x75u, 0x76u
};

static const uint8_t label[2] = {
    0x61u, 0x62u
};

static const uint8_t context[2] = {
    0x63u, 0x64u
};

static const uint8_t expectedDerivedKey[] = {
    0x50u, 0x00u, 0x18u, 0xbbu, 0x65u, 0xb5u, 0x5du, 0x1fu, 0xf7u, 0xfdu, 0xdbu, 0xd8u, 0x5bu, 0x15u, 0x01u, 0x8f,
    0x7eu, 0xc4u, 0xb7u, 0x90u, 0xffu, 0x28u, 0x8bu, 0x9eu, 0xc4u, 0x2du, 0x49u, 0xaau, 0x3fu, 0xafu, 0xb6u, 0x94,
    0x11u, 0x16u, 0x0eu, 0x78u, 0x32u, 0xb6u, 0xcau, 0xb8u, 0xa4u, 0x44u, 0xfau, 0x7eu, 0x8fu, 0x9du, 0x02u, 0xb7,
    0xabu, 0xf3u, 0x14u, 0xdcu, 0xb6u, 0x74u, 0xb2u, 0x45u, 0xe7u, 0xaeu, 0x82u, 0x7du, 0x06u, 0xadu, 0x4au, 0xdc,
    0x2cu, 0x62u, 0x4bu, 0x38u, 0x0cu, 0x37u, 0x35u, 0x5bu, 0x84u, 0x54u, 0xc7u, 0x75u, 0x89u, 0x7fu, 0xf0u, 0xe0,
    0x1au, 0x62u, 0x8eu, 0x9fu, 0x76u, 0x62u, 0x32u, 0x74u, 0x52u, 0x0cu, 0xceu, 0x93u, 0x34u, 0x0cu, 0xbfu, 0x18,
    0x0fu, 0x00u, 0x9au, 0x23u, 0xaau, 0x8du, 0xd9u, 0xa4u, 0x24u, 0x25u, 0xd5u, 0xecu, 0xc2u, 0xeau, 0xe8u, 0x5f,
    0x29u, 0xc1u, 0xcdu, 0x94u, 0x21u, 0xbeu, 0x95u, 0x72u, 0x3fu, 0x4eu, 0x3cu, 0x8eu, 0x5au, 0x5bu, 0x1a
};



/** Performs an example key derivation using the mcuxClKey component.
 * @retval MCUXCLEXAMPLE_STATUS_OK    The example code completed successfully
 * @retval MCUXCLEXAMPLE_STATUS_ERROR The example code failed */
MCUXCLEXAMPLE_FUNCTION(mcuxClKey_Derivation_SP800_56C_twostep_cmac_example)
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
    mcuxClKey_Handle_t sharedSecretHandle = (mcuxClKey_Handle_t) sharedSecretDesc;


    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(resultKeyInit1, tokenKeyInit1, mcuxClKey_init(
      /* mcuxClSession_Handle_t session         */ session,
      MCUX_CSSL_ANALYSIS_START_SUPPRESS_POINTER_INCOMPATIBLE("The pointer sharedSecretHandle points to an object of the right type, the cast was valid.")
      /* mcuxClKey_Handle_t key                 */ sharedSecretHandle,
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

    /* Key buffer for the key in memory. */
    uint32_t key_buffer[MCUXCLAES_AES128_KEY_SIZE_IN_WORDS];

    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(resultKeyLoad, tokenKeyLoad, mcuxClKey_loadMemory(
      /* mcuxClSession_Handle_t session         */ session,
      /* mcuxClKey_Handle_t key                 */ sharedSecretHandle,
      /* const uint8_t * pKeyData              */ key_buffer
    ));

    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClKey_loadMemory) != tokenKeyLoad) || (MCUXCLKEY_STATUS_OK != resultKeyLoad))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();

    /* Set up input parameter structures. */
    uint8_t* pSalt = NULL;
    MCUXCLBUFFER_INIT_RO(labelBuf, session, label, sizeof(label));
    MCUXCLBUFFER_INIT_RO(contextBuf, session, context, sizeof(context));
    MCUXCLBUFFER_INIT_RO(pSaltBuf, session, pSalt, 16u);
    struct mcuxClKey_DerivationInput inputLabel = {.input=labelBuf, .size=sizeof(label)};
    struct mcuxClKey_DerivationInput inputContext = {.input=contextBuf, .size=sizeof(context)};
    /* Salt size (16,24,32) has to be defined even if salt not provided (will be filled with zeros internally) */
    struct mcuxClKey_DerivationInput inputSalt = {.input=pSaltBuf, .size=16u};

    mcuxClKey_DerivationInput_t inputs[] = {inputLabel, inputContext, inputSalt};

    /* Set up output structure. */
    uint8_t derivedKeyBuf[sizeof(expectedDerivedKey)];

    /* Create and initialize derivedKey descriptor structure. */
    uint32_t derivedKeyDesc[MCUXCLKEY_DESCRIPTOR_SIZE_IN_WORDS];
    mcuxClKey_Handle_t derivedKey = (mcuxClKey_Handle_t) derivedKeyDesc;

    /* Type of output key (Hmac_variableLength) was chosen to indicate no specific restrictions on output length */
    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(resultKeyInit2, tokenKeyInit2, mcuxClKey_init(
      /* mcuxClSession_Handle_t session         */ session,
      MCUX_CSSL_ANALYSIS_START_SUPPRESS_POINTER_INCOMPATIBLE("The pointer derivedKey points to an object of the right type, the cast was valid.")
      /* mcuxClKey_Handle_t key                 */ derivedKey,
      MCUX_CSSL_ANALYSIS_STOP_SUPPRESS_POINTER_INCOMPATIBLE()
      /* mcuxClKey_Type_t type                  */ mcuxClKey_Type_Hmac_variableLength,
      /* const uint8_t * pKeyData              */ derivedKeyBuf,
      /* uint32_t keyDataLength                */ sizeof(derivedKeyBuf)
    ));

    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClKey_init) != tokenKeyInit2) || (MCUXCLKEY_STATUS_OK != resultKeyInit2))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();

    /**************************************************************************/
    /* Create CMAC Derivation mode                                   */
    /**************************************************************************/
    uint32_t derivationModeDescBuffer[MCUXCLKEY_DERIVATION_MODE_DESCRIPTOR_SIZE_IN_WORDS];
    MCUX_CSSL_ANALYSIS_START_PATTERN_REINTERPRET_MEMORY_OF_OPAQUE_TYPES()
    mcuxClKey_DerivationMode_t * pDerivationMode = (mcuxClKey_DerivationMode_t *) derivationModeDescBuffer;
    MCUX_CSSL_ANALYSIS_STOP_PATTERN_REINTERPRET_MEMORY_OF_OPAQUE_TYPES()

    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(resultModeConstruct, tokenModeConstruct, mcuxClKey_Derivation_ModeConstructor_NIST_SP800_56C(
      MCUX_CSSL_ANALYSIS_START_SUPPRESS_POINTER_INCOMPATIBLE("The pointer pDerivationMode is of the right type (mcuxClKey_DerivationMode_t *), the cast was valid.")
      /* mcuxClKey_DerivationMode_t *                      */ pDerivationMode,
      MCUX_CSSL_ANALYSIS_STOP_SUPPRESS_POINTER_INCOMPATIBLE()
      /* const mcuxClKey_DerivationAlgorithmDescriptor_t * */ mcuxClKey_DerivationAlgorithm_NIST_SP800_56C_TwoStep,
      /* mcuxClMac_Mode_t                                  */ mcuxClMac_Mode_CMAC, // use this when using mac function as PRF
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
      /* uint32_t numberOfInputs                */ 3u,
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
    if(!mcuxClCore_assertEqual(derivedKeyBuf, expectedDerivedKey, sizeof(expectedDerivedKey)))
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
