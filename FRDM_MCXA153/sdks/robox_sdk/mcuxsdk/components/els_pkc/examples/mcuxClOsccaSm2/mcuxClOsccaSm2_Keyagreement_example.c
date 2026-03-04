/*--------------------------------------------------------------------------*/
/* Copyright 2022-2024 NXP                                                  */
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
 * @file:   mcuxClOsccaSm2_Keyagreement_example.c
 * @brief:  Example OSCCA SM2 key agreement
 */

/******************************************************************************
 * Includes
 ******************************************************************************/
#include <mcuxClSession.h>
#include <mcuxClRandom.h>
#include <mcuxClRandomModes.h>
#include <mcuxClKey.h>
#include <mcuxClOsccaSm2.h>
#include <mcuxClOsccaSm3.h>
#include <mcuxClOsccaSm2_CommonParams.h>
#include <mcuxCsslFlowProtection.h>
#include <mcuxClOscca_FunctionIdentifiers.h>
#include <mcuxClExample_Session_Helper.h>
#include <mcuxClCore_Examples.h>
#include <mcuxClExample_ELS_Helper.h>
#if MCUXCL_FEATURE_RANDOMMODES_OSCCA_TRNG == 1
#include <mcuxClOsccaRandomModes.h>
#else
#include <mcuxClMemory.h>
#include <mcuxClCore_FunctionIdentifiers.h>
#endif

/******************************************************************************
 * Defines
 ******************************************************************************/
/**
 * @brief Maximum of the CPU workarea
 */
#define SIZE_WA_CPU  MCUXCLCORE_MAX(MCUXCLRANDOMMODES_NCINIT_WACPU_SIZE, MCUXCLOSCCASM2_KEYAGREEMENT_SIZEOF_WA_CPU)
/**
 * @def SIZE_WA_PKC
 * @brief Maximum of the pkc workarea
 */
#define SIZE_WA_PKC  MCUXCLOSCCASM2_KEYAGREEMENT_SIZEOF_WA_PKC_256()

/******************************************************************************
 * External variables
 ******************************************************************************/
/* none */

/**
 * @def Message Digest for local party's identifier
 */
static const uint8_t Z_A[] = {0xE4, 0xD1, 0xD0, 0xC3, 0xCA, 0x4C, 0x7F, 0x11, 0xBC, 0x8F, 0xF8, 0xCB, 0x3F, 0x4C, 0x02, 0xA7,
                        0x8F, 0x10, 0x8F, 0xA0, 0x98, 0xE5, 0x1A, 0x66, 0x84, 0x87, 0x24, 0x0F, 0x75, 0xE2, 0x0F, 0x31};

/**
 * @def Message Digest for external party's identifier
 */
static const uint8_t Z_B[] = {0x6B, 0x4B, 0x6D, 0x0E, 0x27, 0x66, 0x91, 0xBD, 0x4A, 0x11, 0xBF, 0x72, 0xF4, 0xFB, 0x50, 0x1A,
                        0xE3, 0x09, 0xFD, 0xAC, 0xB7, 0x2F, 0xA6, 0xCC, 0x33, 0x6E, 0x66, 0x56, 0x11, 0x9A, 0xBD, 0x67};

/**
 * @def private ephemeral key of local party
 */
static const uint8_t rand_A[] = {0x83, 0xA2, 0xC9, 0xC8, 0xB9, 0x6E, 0x5A, 0xF7, 0x0B, 0xD4, 0x80, 0xB4, 0x72, 0x40, 0x9A, 0x9A,
                           0x32, 0x72, 0x57, 0xF1, 0xEB, 0xB7, 0x3F, 0x5B, 0x07, 0x33, 0x54, 0xB2, 0x48, 0x66, 0x85, 0x63};

/**
 * @def private key of local party
 */
static const uint8_t pri_key_A[]= {0x6F, 0xCB, 0xA2, 0xEF, 0x9A, 0xE0, 0xAB, 0x90, 0x2B, 0xC3, 0xBD, 0xE3, 0xFF, 0x91, 0x5D, 0x44,
                             0xBA, 0x4C, 0xC7, 0x8F, 0x88, 0xE2, 0xF8, 0xE7, 0xF8, 0x99, 0x6D, 0x3B, 0x8C, 0xCE, 0xED, 0xEE};

/**
 * @def public ephemeral key of local party
 */
static const uint8_t ephemeral_point_A[] = {0x6C, 0xB5, 0x63, 0x38, 0x16, 0xF4, 0xDD, 0x56, 0x0B, 0x1D, 0xEC, 0x45, 0x83, 0x10, 0xCB, 0xCC,
                                      0x68, 0x56, 0xC0, 0x95, 0x05, 0x32, 0x4A, 0x6D, 0x23, 0x15, 0x0C, 0x40, 0x8F, 0x16, 0x2B, 0xF0,
                                      0x0D, 0x6F, 0xCF, 0x62, 0xF1, 0x03, 0x6C, 0x0A, 0x1B, 0x6D, 0xAC, 0xCF, 0x57, 0x39, 0x92, 0x23,
                                      0xA6, 0x5F, 0x7D, 0x7B, 0xF2, 0xD9, 0x63, 0x7E, 0x5B, 0xBB, 0xEB, 0x85, 0x79, 0x61, 0xBF, 0x1A};

/**
 * @def public ephemeral key of external party
 */
static const uint8_t ephemeral_point_B[] = {0x17, 0x99, 0xB2, 0xA2, 0xC7, 0x78, 0x29, 0x53, 0x00, 0xD9, 0xA2, 0x32, 0x5C, 0x68, 0x61, 0x29,
                                      0xB8, 0xF2, 0xB5, 0x33, 0x7B, 0x3D, 0xCF, 0x45, 0x14, 0xE8, 0xBB, 0xC1, 0x9D, 0x90, 0x0E, 0xE5,
                                      0x54, 0xC9, 0x28, 0x8C, 0x82, 0x73, 0x3E, 0xFD, 0xF7, 0x80, 0x8A, 0xE7, 0xF2, 0x7D, 0x0E, 0x73,
                                      0x2F, 0x7C, 0x73, 0xA7, 0xD9, 0xAC, 0x98, 0xB7, 0xD8, 0x74, 0x0A, 0x91, 0xD0, 0xDB, 0x3C, 0xF4};

/**
 * @def public key of external party
 */
static const uint8_t public_key_B[] = {0x24, 0x54, 0x93, 0xD4, 0x46, 0xC3, 0x8D, 0x8C, 0xC0, 0xF1, 0x18, 0x37, 0x46, 0x90, 0xE7, 0xDF,
                                 0x63, 0x3A, 0x8A, 0x4B, 0xFB, 0x33, 0x29, 0xB5, 0xEC, 0xE6, 0x04, 0xB2, 0xB4, 0xF3, 0x7F, 0x43,
                                 0x53, 0xC0, 0x86, 0x9F, 0x4B, 0x9E, 0x17, 0x77, 0x3D, 0xE6, 0x8F, 0xEC, 0x45, 0xE1, 0x49, 0x04,
                                 0xE0, 0xDE, 0xA4, 0x5B, 0xF6, 0xCE, 0xCF, 0x99, 0x18, 0xC8, 0x5E, 0xA0, 0x47, 0xC6, 0x0A, 0x4C};

/**
 * @def agreement key between 2 parties
 */
static const uint8_t expected_common_secret[] = {0x55, 0xB0, 0xAC, 0x62, 0xA6, 0xB9, 0x27, 0xBA, 0x23, 0x70, 0x38, 0x32, 0xC8, 0x53, 0xDE, 0xD4};


/**
 * @def key confirmation for local party
 */
static const uint8_t S1[] = {0x28, 0x4C, 0x8F, 0x19, 0x8F, 0x14, 0x1B, 0x50, 0x2E, 0x81, 0x25, 0x0F, 0x15, 0x81, 0xC7, 0xE9,
                     0xEE, 0xB4, 0xCA, 0x69, 0x90, 0xF9, 0xE0, 0x2D, 0xF3, 0x88, 0xB4, 0x54, 0x71, 0xF5, 0xBC, 0x5C};

/**
 * @def key confirmation for external party
 */
static const uint8_t SA[] = {0x23, 0x44, 0x4D, 0xAF, 0x8E, 0xD7, 0x53, 0x43, 0x66, 0xCB, 0x90, 0x1C, 0x84, 0xB3, 0xBD, 0xBB,
                     0x63, 0x50, 0x4F, 0x40, 0x65, 0xC1, 0x11, 0x6C, 0x91, 0xA4, 0xC0, 0x06, 0x97, 0xE6, 0xCF, 0x7A};

/******************************************************************************
 * Local variables
 ******************************************************************************/
/* none */

/******************************************************************************
 * Local and global function declarations
 ******************************************************************************/
/**
 * @brief:  Example OSCCA SM2 key agreement
 *
 * @return
 *    - true if selected algorithm processed successfully
 *    - false if selected algorithm caused an error
 *
 * @pre
 *  none
 *
 * @post
 *   the mcuxClOsccaSm2_Keyagreement_example function will be triggered
 *
 * @note
 *   none
 *
 * @warning
 *   none
 */
MCUXCLEXAMPLE_FUNCTION(mcuxClOsccaSm2_Keyagreement_example)
{
    /**************************************************************************/
    /* Preparation: RNG initialization, CPU and PKC workarea allocation       */
    /**************************************************************************/
    /** Initialize ELS, Enable the ELS **/
    if(!mcuxClExample_Els_Init(MCUXCLELS_RESET_DO_NOT_CANCEL))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }

    /* Setup one session to be used by all functions called */
    mcuxClSession_Descriptor_t session;
    //Allocate and initialize session with pkcWA on the beginning of PKC RAM
    MCUXCLEXAMPLE_ALLOCATE_AND_INITIALIZE_SESSION(&session, SIZE_WA_CPU, SIZE_WA_PKC);
#if MCUXCL_FEATURE_RANDOMMODES_OSCCA_TRNG == 1
    /* Initialize the RNG context */
    /* We need a context for OSCCA Rng. */
    uint32_t rngCtx[MCUXCLOSCCARANDOMMODES_OSCCARNG_CONTEXT_SIZE_IN_WORDS];
    MCUX_CSSL_ANALYSIS_START_PATTERN_REINTERPRET_MEMORY_OF_OPAQUE_TYPES()
    mcuxClRandom_Context_t pRngCtx = (mcuxClRandom_Context_t)rngCtx;
    MCUX_CSSL_ANALYSIS_STOP_PATTERN_REINTERPRET_MEMORY_OF_OPAQUE_TYPES()

    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(randomInit_result, randomInit_token, mcuxClRandom_init(&session,
        MCUX_CSSL_ANALYSIS_START_SUPPRESS_POINTER_INCOMPATIBLE("pRngCtx has the correct type (mcuxClRandom_Context_t), the cast was valid.")
                                                               pRngCtx,
        MCUX_CSSL_ANALYSIS_STOP_SUPPRESS_POINTER_INCOMPATIBLE()
                                                               mcuxClOsccaRandomModes_Mode_TRNG));
    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClRandom_init) != randomInit_token) || (MCUXCLRANDOM_STATUS_OK != randomInit_result))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();
#else
    /* Fill mode descriptor with the relevant data */
    uint32_t customModeDescBytes[(MCUXCLRANDOMMODES_PATCHMODE_DESCRIPTOR_SIZE + sizeof(uint32_t) - 1U)/sizeof(uint32_t)];
    mcuxClRandom_ModeDescriptor_t *mcuxClRandomModes_Mode_Custom = (mcuxClRandom_ModeDescriptor_t *) customModeDescBytes;

    /**************************************************************************/
    /* RANDOM Patch Mode creation                                             */
    /**************************************************************************/
    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(randomPatch_result, randomPatch_token, mcuxClRandomModes_createPatchMode(mcuxClRandomModes_Mode_Custom,
                                        (mcuxClRandomModes_CustomGenerateAlgorithm_t)RNG_Patch_function,
                                        NULL,
                                        256U));
    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClRandomModes_createPatchMode) != randomPatch_token) || (MCUXCLRANDOM_STATUS_OK != randomPatch_result))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();
    /**************************************************************************/
    /* patch mode initialization                                              */
    /**************************************************************************/
    uint32_t* rngContextPatched = NULL;
    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(randomInit_result, randomInit_token, mcuxClRandom_init(&session, (mcuxClRandom_Context_t)rngContextPatched, mcuxClRandomModes_Mode_Custom));
    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClRandom_init) != randomInit_token) || (MCUXCLRANDOM_STATUS_OK != randomInit_result))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();
#endif

    /* Initialize the PRNG */
    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(prngInit_result, prngInit_token, mcuxClRandom_ncInit(&session));
    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClRandom_ncInit) != prngInit_token) || (MCUXCLRANDOM_STATUS_OK != prngInit_result))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();

    /****************************************************************/
    /* Preparation: setup SM2 key                                   */
    /****************************************************************/
    /* Initialize SM2 private key */
    uint32_t privKeyDesc[MCUXCLKEY_DESCRIPTOR_SIZE_IN_WORDS];
    MCUX_CSSL_ANALYSIS_START_PATTERN_REINTERPRET_MEMORY_OF_OPAQUE_TYPES()
    mcuxClKey_Handle_t privKeyA = (mcuxClKey_Handle_t) privKeyDesc;
    MCUX_CSSL_ANALYSIS_STOP_PATTERN_REINTERPRET_MEMORY_OF_OPAQUE_TYPES()

    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(ki_priv_result, ki_priv_token, mcuxClKey_init(
      /* mcuxClSession_Handle_t session         */ &session,
      MCUX_CSSL_ANALYSIS_START_SUPPRESS_POINTER_INCOMPATIBLE("The pointer privKeyA points to an object of the right type, the cast was valid.")
      /* mcuxClKey_Handle_t key                 */ privKeyA,
      MCUX_CSSL_ANALYSIS_STOP_SUPPRESS_POINTER_INCOMPATIBLE()
      /* mcuxClKey_Type_t type                  */ mcuxClKey_Type_SM2P256_Ext_Private,
      /* const uint8_t * pKeyData              */ pri_key_A,
      /* uint32_t keyDataLength                */ sizeof(pri_key_A)
    ));

    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClKey_init) != ki_priv_token) || (MCUXCLKEY_STATUS_OK != ki_priv_result))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();

    /* Initialize SM2 public key */
    uint32_t pubKeyDesc[MCUXCLKEY_DESCRIPTOR_SIZE_IN_WORDS];
    MCUX_CSSL_ANALYSIS_START_PATTERN_REINTERPRET_MEMORY_OF_OPAQUE_TYPES()
    mcuxClKey_Handle_t pubKeyB = (mcuxClKey_Handle_t) pubKeyDesc;
    MCUX_CSSL_ANALYSIS_STOP_PATTERN_REINTERPRET_MEMORY_OF_OPAQUE_TYPES()

    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(ki_pub_result, ki_pub_token, mcuxClKey_init(
      /* mcuxClSession_Handle_t session         */ &session,
      MCUX_CSSL_ANALYSIS_START_SUPPRESS_POINTER_INCOMPATIBLE("The pointer pubKeyB points to an object of the right type, the cast was valid.")
      /* mcuxClKey_Handle_t key                 */ pubKeyB,
      MCUX_CSSL_ANALYSIS_STOP_SUPPRESS_POINTER_INCOMPATIBLE()
      /* mcuxClKey_Type_t type                  */ mcuxClKey_Type_SM2P256_Ext_Public,
      /* const uint8_t * pKeyData              */ public_key_B,
      /* uint32_t keyDataLength                */ sizeof(public_key_B)
    ));

    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClKey_init) != ki_pub_token) || (MCUXCLKEY_STATUS_OK != ki_pub_result))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();

    /****************************************************************/
    /* OSCCA SM2 key agreement                                      */
    /****************************************************************/
    uint8_t confirmationToInitor[MCUXCLOSCCASM3_OUTPUT_SIZE_SM3];
    uint8_t confirmationToResponder[MCUXCLOSCCASM3_OUTPUT_SIZE_SM3];

    mcuxClKey_Agreement_AdditionalInput_t additionalInputs[MCUXCLOSCCASM2_KEYAGREEMENT_NUM_OF_ADDITIONAL_INPUTS] = {
            {.input = Z_A, .size = sizeof(Z_A)},
            {.input = Z_B, .size = sizeof(Z_B)},
            {.input = rand_A, .size = sizeof(rand_A)},
            {.input = ephemeral_point_A, .size = sizeof(ephemeral_point_A)},
            {.input = ephemeral_point_B, .size = sizeof(ephemeral_point_B)},
            {.input = confirmationToInitor, .size = sizeof(confirmationToInitor)},
            {.input = confirmationToResponder, .size = sizeof(confirmationToResponder)}
        };

    uint8_t output[sizeof(expected_common_secret)];
    uint32_t outputLen = sizeof(expected_common_secret);
    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(ka_result, ka_token, mcuxClKey_agreement(
      /* mcuxClSession_Handle_t session    */ &session,
      /* mcuxClKey_Agreement_t agreement   */ mcuxClOsccaSm2_Agreement_Initiator,
      MCUX_CSSL_ANALYSIS_START_SUPPRESS_POINTER_INCOMPATIBLE("The pointer privKeyA points to an object of the right type, the cast was valid.")
      /* mcuxClKey_Handle_t key            */ privKeyA,
      MCUX_CSSL_ANALYSIS_STOP_SUPPRESS_POINTER_INCOMPATIBLE()
      MCUX_CSSL_ANALYSIS_START_SUPPRESS_POINTER_INCOMPATIBLE("The pointer pubKeyB points to an object of the right type, the cast was valid.")
      /* mcuxClKey_Handle_t key            */ pubKeyB,
      MCUX_CSSL_ANALYSIS_STOP_SUPPRESS_POINTER_INCOMPATIBLE()
      /* mcuxClKey_AgreementInput_t        */ additionalInputs,
      /* uint32_t numberOfInputs          */ MCUXCLOSCCASM2_KEYAGREEMENT_NUM_OF_ADDITIONAL_INPUTS,
      /* uint8_t * pOut                   */ output,
      /* uint32_t * const pOutLength      */ &outputLen
    )); /* determine a shared key on based on public and private inputs */

    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClKey_agreement) != ka_token) || (MCUXCLKEY_STATUS_OK != ka_result))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();

    /* Check the result of the key agreement with expected output */
    if (!mcuxClCore_assertEqual(expected_common_secret, output, sizeof(expected_common_secret)))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }

    /* Check the confirmation result of the key agreement with expected output */
    if (!mcuxClCore_assertEqual(confirmationToInitor, S1, sizeof(S1)))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }

    /* Check the confirmation result of the key agreement with expected output */
    if (!mcuxClCore_assertEqual(confirmationToResponder, SA, sizeof(SA)))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }

    /** Destroy Session and cleanup Session **/
    if(!mcuxClExample_Session_Clean(&session))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }

    /** Disable the ELS **/
    if(!mcuxClExample_Els_Disable())
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }

    return MCUXCLEXAMPLE_STATUS_OK;
}
