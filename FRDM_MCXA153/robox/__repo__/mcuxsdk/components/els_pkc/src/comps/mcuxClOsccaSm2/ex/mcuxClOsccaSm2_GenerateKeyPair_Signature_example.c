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
 * @file    mcuxClOsccaSm2_GenerateKeyPair_Signature_example.c
 * @brief   Example OSCCA SM2 key generation, include SM2 key pair generation,
 *          SM2 sign and verify.
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
#include <mcuxClSignature.h>
#include <mcuxCsslFlowProtection.h>
#include <mcuxClOscca_FunctionIdentifiers.h>
#include <mcuxClExample_Session_Helper.h>
#include <mcuxClCore_Examples.h>
#include <mcuxClOsccaSm2_CommonParams.h>
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
#define SIZE_WA_CPU  MCUXCLCORE_MAX(MCUXCLRANDOMMODES_NCINIT_WACPU_SIZE,\
                     MCUXCLCORE_MAX(MCUXCLOSCCASM2_SIGN_SIZEOF_WA_CPU(MCUXCLOSCCASM2_SM2P256_SIZE_BASEPOINTORDER), \
                     MCUXCLCORE_MAX(MCUXCLOSCCASM2_VERIFY_SIZEOF_WA_CPU,     \
                                   MCUXCLOSCCASM2_KEY_GENERATEKEYPAIR_SIZEOF_WA_CPU)))
/**
 * @def SIZE_WA_PKC
 * @brief Maximum of the pkc workarea
 */
#define SIZE_WA_PKC  MCUXCLCORE_MAX(MCUXCLCORE_MAX(  \
    MCUXCLOSCCASM2_SIGN_SIZEOF_WA_PKC_256(),    \
    MCUXCLOSCCASM2_VERIFY_SIZEOF_WA_PKC_256()), \
    MCUXCLOSCCASM2_KEY_GENERATEKEYPAIR_SIZEOF_WA_PKC_256())

/******************************************************************************
 * External variables
 ******************************************************************************/
/* none */

/******************************************************************************
 * Global variables
 ******************************************************************************/

/******************************************************************************
 * Local variables
 ******************************************************************************/
/**
 * @def pMessage Digest
 */
static const uint8_t pDigest_SM2[MCUXCLOSCCASM3_OUTPUT_SIZE_SM3] =
{
    0xF0u,0xB4u,0x3Eu,0x94u,0xBAu,0x45u,0xACu,0xCAu,0xACu,0xE6u,0x92u,0xEDu,0x53u,0x43u,0x82u,0xEBu,
    0x17u,0xE6u,0xABu,0x5Au,0x19u,0xCEu,0x7Bu,0x31u,0xF4u,0x48u,0x6Fu,0xDFu,0xC0u,0xD2u,0x86u,0x40u
};

/******************************************************************************
 * Local and global function declarations
 ******************************************************************************/
/**
 * @brief   Example OSCCA SM2 key generation, include SM2 key pair generation,
 *          SM2 sign and verify.
 *
 * @return
 *    - true if selected algorithm processed successfully
 *    - false if selected algorithm caused an error
 *
 * @pre
 *  none
 *
 * @post
 *   the mcuxClOsccaSm2_GenerateKeyPair_Signature_example function will be triggered
 *
 * @note
 *   none
 *
 * @warning
 *   none
 */
MCUXCLEXAMPLE_FUNCTION(mcuxClOsccaSm2_GenerateKeyPair_Signature_example)
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
    /* Allocate and initialize session with pkcWA on the beginning of PKC RAM */
    MCUXCLEXAMPLE_ALLOCATE_AND_INITIALIZE_SESSION(&session, SIZE_WA_CPU, SIZE_WA_PKC);
#if MCUXCL_FEATURE_RANDOMMODES_OSCCA_TRNG == 1
    /* Initialize the RNG context */
    /* We need a context for OSCCA Rng. */
    uint32_t rngCtx[MCUXCLOSCCARANDOMMODES_OSCCARNG_CONTEXT_SIZE_IN_WORDS];
    MCUX_CSSL_ANALYSIS_START_PATTERN_REINTERPRET_MEMORY_OF_OPAQUE_TYPES()
    mcuxClRandom_Context_t pRngCtx = (mcuxClRandom_Context_t)rngCtx;
    MCUX_CSSL_ANALYSIS_STOP_PATTERN_REINTERPRET_MEMORY_OF_OPAQUE_TYPES()

    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(randomInit_result, randomInit_token, mcuxClRandom_init(
                                                               &session,
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
    /* Allocate space for and initialize private key handle for SM2 private key */
    uint32_t privKeyDesc[MCUXCLKEY_DESCRIPTOR_SIZE_IN_WORDS];
    MCUX_CSSL_ANALYSIS_START_PATTERN_REINTERPRET_MEMORY_OF_OPAQUE_TYPES()
    mcuxClKey_Handle_t privKey = (mcuxClKey_Handle_t) privKeyDesc;
    MCUX_CSSL_ANALYSIS_STOP_PATTERN_REINTERPRET_MEMORY_OF_OPAQUE_TYPES()
    uint8_t pPrivKeyData[MCUXCLOSCCASM2_SM2P256_SIZE_PRIVATEKEY];

    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(ki_priv_result, ki_priv_token, mcuxClKey_init(
      /* mcuxClSession_Handle_t session         */ &session,
      MCUX_CSSL_ANALYSIS_START_SUPPRESS_POINTER_INCOMPATIBLE("The pointer privKey points to an object of the right type, the cast was valid.")
      /* mcuxClKey_Handle_t key                 */ privKey,
      MCUX_CSSL_ANALYSIS_STOP_SUPPRESS_POINTER_INCOMPATIBLE()
      /* mcuxClKey_Type_t type                  */ mcuxClKey_Type_SM2P256_Std_Private,
      /* const uint8_t * pKeyData              */ pPrivKeyData,
      /* uint32_t keyDataLength                */ MCUXCLOSCCASM2_SM2P256_SIZE_PRIVATEKEY
    ));

    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClKey_init) != ki_priv_token) || (MCUXCLKEY_STATUS_OK != ki_priv_result))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();

    /* Allocate space for and initialize private key handle for SM2 public key */
    uint32_t pubKeyDesc[MCUXCLKEY_DESCRIPTOR_SIZE_IN_WORDS];
    MCUX_CSSL_ANALYSIS_START_PATTERN_REINTERPRET_MEMORY_OF_OPAQUE_TYPES()
    mcuxClKey_Handle_t pubKey = (mcuxClKey_Handle_t) pubKeyDesc;
    MCUX_CSSL_ANALYSIS_STOP_PATTERN_REINTERPRET_MEMORY_OF_OPAQUE_TYPES()
    uint8_t pPubKeyData[MCUXCLOSCCASM2_SM2P256_SIZE_PUBLICKEY];

    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(ki_pub_result, ki_pub_token, mcuxClKey_init(
      /* mcuxClSession_Handle_t session         */ &session,
      MCUX_CSSL_ANALYSIS_START_SUPPRESS_POINTER_INCOMPATIBLE("The pointer pubKey points to an object of the right type, the cast was valid.")
      /* mcuxClKey_Handle_t key                 */ pubKey,
      MCUX_CSSL_ANALYSIS_STOP_SUPPRESS_POINTER_INCOMPATIBLE()
      /* mcuxClKey_Type_t type                  */ mcuxClKey_Type_SM2P256_Std_Public,
      /* const uint8_t * pKeyData              */ pPubKeyData,
      /* uint32_t keyDataLength                */ MCUXCLOSCCASM2_SM2P256_SIZE_PUBLICKEY
    ));

    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClKey_init) != ki_pub_token) || (MCUXCLKEY_STATUS_OK != ki_pub_result))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();

    /**************************************************************************/
    /* Key pair generation for SM2 key Pair                                   */
    /**************************************************************************/
    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(gkp_result, gkp_token, mcuxClKey_generate_keypair(
      /* mcuxClSession_Handle_t pSession    */ &session,
      /* mcuxClKey_Generation_t generation  */ mcuxClKey_Generation_SM2,
      MCUX_CSSL_ANALYSIS_START_SUPPRESS_POINTER_INCOMPATIBLE("The pointer privKey points to an object of the right type, the cast was valid.")
      /* mcuxClKey_Handle_t privKey         */ privKey,
      MCUX_CSSL_ANALYSIS_STOP_SUPPRESS_POINTER_INCOMPATIBLE()
      MCUX_CSSL_ANALYSIS_START_SUPPRESS_POINTER_INCOMPATIBLE("The pointer pubKey points to an object of the right type, the cast was valid.")
      /* mcuxClKey_Handle_t pubKey          */ pubKey
      MCUX_CSSL_ANALYSIS_STOP_SUPPRESS_POINTER_INCOMPATIBLE()
    ));
    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClKey_generate_keypair) != gkp_token) || (MCUXCLKEY_STATUS_OK != gkp_result))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();

    /****************************************************************/
    /* OSCCA SM2 signature generation                               */
    /****************************************************************/
    uint8_t signature[MCUXCLOSCCASM2_SM2P256_SIZE_SIGNATURE];
    uint32_t signatureSize = 0;
    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(ss_result, ss_token, mcuxClSignature_sign(
      /* mcuxClSession_Handle_t session    */ &session,
      MCUX_CSSL_ANALYSIS_START_SUPPRESS_POINTER_INCOMPATIBLE("The pointer privKey points to an object of the right type, the cast was valid.")
      /* mcuxClKey_Handle_t privKey        */ privKey,
      MCUX_CSSL_ANALYSIS_STOP_SUPPRESS_POINTER_INCOMPATIBLE()
      /* mcuxClSignature_Mode_t mode       */ mcuxClSignature_Mode_SM2,
      /* mcuxCl_InputBuffer_t pIn          */ pDigest_SM2,
      /* uint32_t inSize                  */ sizeof(pDigest_SM2),
      /* mcuxCl_Buffer_t pSignature        */ signature,
      /* uint32_t * const pSignatureSize  */ &signatureSize
    ));

    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClSignature_sign) != ss_token) || (MCUXCLSIGNATURE_STATUS_OK != ss_result))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();

    /****************************************************************/
    /* OSCCA SM2 signature verification                             */
    /****************************************************************/
    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(sv_result, sv_token, mcuxClSignature_verify(
      /* mcuxClSession_Handle_t session   */ &session,
      MCUX_CSSL_ANALYSIS_START_SUPPRESS_POINTER_INCOMPATIBLE("The pointer pubKey points to an object of the right type, the cast was valid.")
      /* mcuxClKey_Handle_t pubKey        */ pubKey,
      MCUX_CSSL_ANALYSIS_STOP_SUPPRESS_POINTER_INCOMPATIBLE()
      /* mcuxClSignature_Mode_t mode      */ mcuxClSignature_Mode_SM2,
      /* mcuxCl_InputBuffer_t pIn         */ pDigest_SM2,
      /* uint32_t inSize                 */ sizeof(pDigest_SM2),
      /* mcuxCl_InputBuffer_t pSignature  */ signature,
      /* uint32_t signatureSize          */ signatureSize
    ));

    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClSignature_verify) != sv_token) || (MCUXCLSIGNATURE_STATUS_OK != sv_result))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();

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
