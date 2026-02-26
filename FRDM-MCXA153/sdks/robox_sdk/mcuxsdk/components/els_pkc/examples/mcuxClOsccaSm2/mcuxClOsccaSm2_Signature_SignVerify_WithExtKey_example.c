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
 * @file:   mcuxClOsccaSm2_Signature_SignVerify_WithExtKey_example.c
 * @brief:  Example OSCCA SM2 signature, including signature generation and signature
 *          verification with extend key
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
#define SIZE_WA_CPU  MCUXCLCORE_MAX(MCUXCLRANDOMMODES_NCINIT_WACPU_SIZE,          \
                     MCUXCLCORE_MAX(MCUXCLOSCCASM2_SIGN_SIZEOF_WA_CPU(MCUXCLOSCCASM2_SM2P256_SIZE_BASEPOINTORDER), \
                                   MCUXCLOSCCASM2_VERIFY_SIZEOF_WA_CPU))
/**
 * @def SIZE_WA_PKC
 * @brief Maximum of the pkc workarea
 */
#define SIZE_WA_PKC  MCUXCLCORE_MAX(\
    MCUXCLOSCCASM2_SIGN_SIZEOF_WA_PKC_256(),   \
    MCUXCLOSCCASM2_VERIFY_SIZEOF_WA_PKC_256())

/******************************************************************************
 * Global variables
 ******************************************************************************/

/* Create buffers for extend private and public key */
static const uint8_t pSm2ExtendedPrivateKey[MCUXCLOSCCASM2_SM2P256_SIZE_BASEPOINTORDER] = {
    //128B2FA8 BD433C6C 068C8D80 3DFF7979 2A519A55 171B1B65 0C23661D 15897263
    0x12u, 0x8Bu, 0x2Fu, 0xA8u, 0xBDu, 0x43u, 0x3Cu, 0x6Cu,
    0x06u, 0x8Cu, 0x8Du, 0x80u, 0x3Du, 0xFFu, 0x79u, 0x79u,
    0x2Au, 0x51u, 0x9Au, 0x55u, 0x17u, 0x1Bu, 0x1Bu, 0x65u,
    0x0Cu, 0x23u, 0x66u, 0x1Du, 0x15u, 0x89u, 0x72u, 0x63u};

static const uint8_t pSm2ExtendedPublicKey[MCUXCLOSCCASM2_SM2P256_SIZE_PRIMEP * 2] = {
    //0AE4C779 8AA0F119 471BEE11 825BE462 02BB79E2 A5844495 E97C04FF 4DF2548A
    0x0Au, 0xE4u, 0xC7u, 0x79u, 0x8Au, 0xA0u, 0xF1u, 0x19u,
    0x47u, 0x1Bu, 0xEEu, 0x11u, 0x82u, 0x5Bu, 0xE4u, 0x62u,
    0x02u, 0xBBu, 0x79u, 0xE2u, 0xA5u, 0x84u, 0x44u, 0x95u,
    0xE9u, 0x7Cu, 0x04u, 0xFFu, 0x4Du, 0xF2u, 0x54u, 0x8Au,
    //7C0240F8 8F1CD4E1 6352A73C 17B7F16F 07353E53 A176D684 A9FE0C6B B798E857
    0x7Cu, 0x02u, 0x40u, 0xF8u, 0x8Fu, 0x1Cu, 0xD4u, 0xE1u,
    0x63u, 0x52u, 0xA7u, 0x3Cu, 0x17u, 0xB7u, 0xF1u, 0x6Fu,
    0x07u, 0x35u, 0x3Eu, 0x53u, 0xA1u, 0x76u, 0xD6u, 0x84u,
    0xA9u, 0xFEu, 0x0Cu, 0x6Bu, 0xB7u, 0x98u, 0xE8u, 0x57u};

/******************************************************************************
 * External variables
 ******************************************************************************/
/* none */

/**
 * @def pMessage Digest
 */
static const uint8_t pDigest[MCUXCLOSCCASM3_OUTPUT_SIZE_SM3] =
{
    //B524F552 CD82B8B0 28476E00 5C377FB1 9A87E6FC 682D48BB 5D42E3D9 B9EFFE76
    0xB5u, 0x24u, 0xF5u, 0x52u, 0xCDu, 0x82u, 0xB8u, 0xB0u,
    0x28u, 0x47u, 0x6Eu, 0x00u, 0x5Cu, 0x37u, 0x7Fu, 0xB1u,
    0x9Au, 0x87u, 0xE6u, 0xFCu, 0x68u, 0x2Du, 0x48u, 0xBBu,
    0x5Du, 0x42u, 0xE3u, 0xD9u, 0xB9u, 0xEFu, 0xFEu, 0x76u
};

/******************************************************************************
 * Local variables
 ******************************************************************************/
/* none */

/******************************************************************************
 * Local and global function declarations
 ******************************************************************************/
/**
 * @brief:  Example OSCCA SM2 signature, including
 *          signature generation and signature verification
 *
 * @return
 *    - true if selected algorithm processed successfully
 *    - false if selected algorithm caused an error
 *
 * @pre
 *  none
 *
 * @post
 *   the mcuxClOsccaSm2_Signature_SignVerify_WithExtKey_example function will be triggered
 *
 * @note
 *   none
 *
 * @warning
 *   none
 */
MCUXCLEXAMPLE_FUNCTION(mcuxClOsccaSm2_Signature_SignVerify_WithExtKey_example)
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
    /* Initialize the RNG context */
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
        MCUX_CSSL_ANALYSIS_START_PATTERN_REINTERPRET_MEMORY_OF_OPAQUE_TYPES()
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
    mcuxClKey_Handle_t privKey = (mcuxClKey_Handle_t) privKeyDesc;
    MCUX_CSSL_ANALYSIS_STOP_PATTERN_REINTERPRET_MEMORY_OF_OPAQUE_TYPES()

    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(ki_priv_result, ki_priv_token, mcuxClKey_init(
      /* mcuxClSession_Handle_t session         */ &session,
      MCUX_CSSL_ANALYSIS_START_SUPPRESS_POINTER_INCOMPATIBLE("The pointer privKey points to an object of the right type, the cast was valid.")
      /* mcuxClKey_Handle_t key                 */ privKey,
      MCUX_CSSL_ANALYSIS_STOP_SUPPRESS_POINTER_INCOMPATIBLE()
      /* mcuxClKey_Type_t type                  */ mcuxClKey_Type_SM2P256_Ext_Private,
      /* const uint8_t * pKeyData              */ pSm2ExtendedPrivateKey,
      /* uint32_t keyDataLength                */ sizeof(pSm2ExtendedPrivateKey)
    ));

    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClKey_init) != ki_priv_token) || (MCUXCLKEY_STATUS_OK != ki_priv_result))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();

    /* Initialize SM2 public key */
    uint32_t pubKeyDesc[MCUXCLKEY_DESCRIPTOR_SIZE_IN_WORDS];
    MCUX_CSSL_ANALYSIS_START_PATTERN_REINTERPRET_MEMORY_OF_OPAQUE_TYPES()
    mcuxClKey_Handle_t pubKey = (mcuxClKey_Handle_t) pubKeyDesc;
    MCUX_CSSL_ANALYSIS_STOP_PATTERN_REINTERPRET_MEMORY_OF_OPAQUE_TYPES()

    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(ki_pub_result, ki_pub_token, mcuxClKey_init(
      /* mcuxClSession_Handle_t session         */ &session,
      MCUX_CSSL_ANALYSIS_START_SUPPRESS_POINTER_INCOMPATIBLE("The pointer pubKey points to an object of the right type, the cast was valid.")
      /* mcuxClKey_Handle_t key                 */ pubKey,
      MCUX_CSSL_ANALYSIS_STOP_SUPPRESS_POINTER_INCOMPATIBLE()
      /* mcuxClKey_Type_t type                  */ mcuxClKey_Type_SM2P256_Ext_Public,
      /* const uint8_t * pKeyData              */ pSm2ExtendedPublicKey,
      /* uint32_t keyDataLength                */ sizeof(pSm2ExtendedPublicKey)
    ));

    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClKey_init) != ki_pub_token) || (MCUXCLKEY_STATUS_OK != ki_pub_result))
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
      /* mcuxClSession_Handle_t session:   */ &session,
      /* mcuxClKey_Handle_t key:           */ privKey,
      /* mcuxClSignature_Mode_t mode:      */ mcuxClSignature_Mode_SM2,
      /* mcuxCl_InputBuffer_t pIn:         */ pDigest,
      /* uint32_t inSize:                 */ sizeof(pDigest),
      /* mcuxCl_Buffer_t pSignature:       */ signature,
      /* uint32_t * const pSignatureSize: */ &signatureSize
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
      /* mcuxClSession_Handle_t session:  */ &session,
      /* mcuxClKey_Handle_t key:          */ pubKey,
      /* mcuxClSignature_Mode_t mode:     */ mcuxClSignature_Mode_SM2,
      /* mcuxCl_InputBuffer_t pIn:        */ pDigest,
      /* uint32_t inSize:                */ sizeof(pDigest),
      /* mcuxCl_InputBuffer_t pSignature: */ signature,
      /* uint32_t signatureSize:         */ signatureSize
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
