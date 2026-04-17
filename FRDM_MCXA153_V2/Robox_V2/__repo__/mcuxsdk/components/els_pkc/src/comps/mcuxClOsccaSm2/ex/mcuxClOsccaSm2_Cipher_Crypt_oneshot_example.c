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
 * @file:   mcuxClOsccaSm2_Cipher_Crypt_oneshot_example.c
 * @brief:  Example OSCCA SM2 Cipher, include encryption and decryption
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
#include <mcuxClCipher.h>
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
 * Global variables
 ******************************************************************************/
/**
 * @def: BYTELENGTH_M
 * @brief: Length of message in bytes
 */
#define BYTELENGTH_M  (128U)

/* Create buffers for private and public key */
static const uint8_t pSm2PrivateKey[MCUXCLOSCCASM2_SM2P256_SIZE_BASEPOINTORDER] = {
    0x19, 0x42, 0x14, 0xC1, 0xD9, 0x6F, 0x8F, 0x47, 0x46, 0x89, 0xC2, 0xC4, 0x5F, 0x75, 0x5D, 0x8C,
    0x7F, 0xCE, 0x7B, 0x70, 0x99, 0xCC, 0x18, 0x6F, 0x4A, 0x61, 0x40, 0x64, 0xC7, 0x5F, 0x42, 0xAF};

static const uint8_t pSm2PublicKey[MCUXCLOSCCASM2_SM2P256_SIZE_PRIMEP * 2] = {
    0xF2, 0x27, 0xA6, 0xE8, 0x30, 0x92, 0x0E, 0x1D, 0xF8, 0xA3, 0x41, 0x03, 0x33, 0xED, 0x32, 0xC7,
    0x55, 0x6F, 0x80, 0x7C, 0x71, 0xCD, 0x2E, 0x68, 0x51, 0xBD, 0xD1, 0x19, 0x7A, 0x43, 0x49, 0xEA,
    0x03, 0x04, 0x4E, 0x76, 0xB3, 0xD1, 0x0C, 0x61, 0xC2, 0x66, 0x94, 0xF4, 0xC9, 0xD0, 0x12, 0x1F,
    0xD7, 0x8A, 0xB1, 0x2A, 0x06, 0x28, 0x96, 0xD8, 0xBE, 0xB6, 0xD6, 0x7C, 0x59, 0x5C, 0x4C, 0xE3};

/**
 * @def pMessage
 * @brief Test vector: 128-byte message to be encrypted or signed
 */
static const uint8_t pMessage_SM2[BYTELENGTH_M] =
{
    0x77u, 0x69u, 0x6fu, 0xbau,  0xf3u, 0x6bu, 0x49u, 0xcdu,
    0x1cu, 0x0eu, 0x45u, 0x6au,  0xd1u, 0x86u, 0x59u, 0xfeu,
    0xdeu, 0x3fu, 0xcbu, 0x0cu,  0xceu, 0x69u, 0xa1u, 0xccu,
    0x01u, 0xb4u, 0x5au, 0x19u,  0xfeu, 0x58u, 0xdbu, 0x8au,
    0x09u, 0x59u, 0xacu, 0xdeu,  0xf9u, 0x09u, 0x64u, 0x9du,
    0x44u, 0xceu, 0x62u, 0xfcu,  0x5cu, 0x25u, 0xbeu, 0x01u,
    0x3eu, 0xe7u, 0x7fu, 0xe9u,  0x47u, 0xccu, 0x0fu, 0xc7u,
    0x4au, 0x2du, 0xecu, 0x6du,  0xe1u, 0x0eu, 0x9fu, 0x8fu,
    0x00u, 0x5bu, 0xf2u, 0x26u,  0xd0u, 0x72u, 0x5eu, 0x13u,
    0xbau, 0xe2u, 0xc3u, 0x05u,  0xc1u, 0x72u, 0x5cu, 0x9cu,
    0x16u, 0x66u, 0xf1u, 0xcfu,  0xe6u, 0xfdu, 0x4eu, 0x8bu,
    0x77u, 0x3eu, 0xf5u, 0x9cu,  0x73u, 0xf4u, 0x10u, 0xd1u,
    0x84u, 0xf8u, 0xa9u, 0x5bu,  0x20u, 0xaeu, 0x1du, 0x77u,
    0xa7u, 0xd2u, 0xceu, 0x4du,  0xabu, 0x75u, 0x19u, 0x17u,
    0x8eu, 0x42u, 0x88u, 0x85u,  0x53u, 0x06u, 0x48u, 0x60u,
    0xaeu, 0x70u, 0xddu, 0x6fu,  0x5du, 0x15u, 0x14u, 0x97u
};

//C1 || C3 || C2
static const uint8_t pCipher_SM2[] = {
//C1
    0x04, 0x5F, 0x79, 0x55, 0x94, 0x78, 0x05, 0x54, 0xF0, 0x42, 0x7D, 0x08, 0xB8, 0x55, 0xBF, 0x6C,
    0xC1, 0x16, 0x98, 0x82, 0x12, 0xF7, 0x58, 0x6A, 0x51, 0xC4, 0x6B, 0x7A, 0x33, 0x44, 0xD8, 0xB0,
    0x54, 0x4F, 0xB5, 0xB9, 0xEA, 0xBB, 0x1C, 0xAC, 0x78, 0xBF, 0x6A, 0xC5, 0x28, 0x74, 0x6B, 0xD8,
    0x75, 0x31, 0xE0, 0x0E, 0xF1, 0xEA, 0x85, 0xC4, 0x90, 0xED, 0x53, 0x22, 0x0C, 0x3F, 0xAB, 0xFF, 0xE5,
//C3
    0xDA, 0x6E, 0x6D, 0x4C, 0xF6, 0x23, 0xEA, 0xCB, 0xA9, 0x86, 0xC8, 0x5D, 0xD8, 0x29, 0xAF,0x67,
    0xA7, 0xFD, 0xA2, 0x8B, 0x6E, 0x10, 0xD6, 0xDA, 0x2B, 0xC6, 0x41, 0x33, 0x34, 0xB1, 0xCE,0x25,
//Cipher
    0x07, 0x11, 0x28, 0x09, 0x50, 0x4A, 0xDD, 0xE6, 0x75, 0x7D, 0xE8, 0x7F, 0x68, 0x29, 0xB7, 0x90,
    0xC4, 0xDA, 0xE1, 0xB9, 0x4C, 0x5C, 0x0C, 0x78, 0x21, 0x3C, 0x8D, 0x43, 0x3A, 0xBE, 0xC8, 0x26,
    0x1D, 0x3E, 0xB2, 0x6D, 0x05, 0xBE, 0x93, 0x1B, 0xFD, 0xD8, 0xD2, 0x23, 0x6F, 0x8C, 0x3C, 0xC7,
    0x0A, 0xDA, 0x45, 0xD5, 0xE7, 0x6B, 0x77, 0xEA, 0x7C, 0xE3, 0x25, 0xE6, 0x9B, 0xAA, 0x11, 0x73,
    0x9B, 0xE1, 0x9A, 0x61, 0x42, 0x02, 0x97, 0x52, 0xFD, 0xA8, 0x31, 0x48, 0x16, 0xEE, 0xF8, 0x55,
    0xA6, 0x20, 0x00, 0xAE, 0x15, 0x4B, 0x9F, 0xFE, 0x7F, 0x90, 0xBF, 0xCB, 0x79, 0x08, 0x12, 0x03,
    0xDF, 0x05, 0xBF, 0x3E, 0x34, 0xE1, 0x70, 0xFB, 0x2D, 0xAB, 0x84, 0xA1, 0x86, 0x2A, 0xB1, 0xE7,
    0xEA, 0x0C, 0x17, 0x8D, 0x91, 0x3E, 0xB3, 0xD0, 0xAC, 0x80, 0x43, 0xBB, 0x4D, 0x4A, 0x7E, 0xE6
};


/******************************************************************************
 * Local variables
 ******************************************************************************/
/* none */

/******************************************************************************
 * Local and global function declarations
 ******************************************************************************/
/**
 * @brief:  Example OSCCA SM2 Cipher, including
 *          Encryption and Decryption
 *
 * @return
 *    - true if selected algorithm processed successfully
 *    - false if selected algorithm caused an error
 *
 * @pre
 *  none
 *
 * @post
 *   the mcuxClOsccaSm2_Cipher_Crypt_oneshot_example function will be triggered
 *
 * @note
 *   none
 *
 * @warning
 *   none
 */
MCUXCLEXAMPLE_FUNCTION(mcuxClOsccaSm2_Cipher_Crypt_oneshot_example)
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
    MCUXCLEXAMPLE_ALLOCATE_AND_INITIALIZE_SESSION(&session, MCUXCLEXAMPLE_MAX_WA(MCUXCLOSCCASM2_CIPHER_ENCDEC_SIZEOF_WA_CPU(MCUXCLOSCCASM2_SM2P256_SIZE_PRIMEP), MCUXCLRANDOMMODES_NCINIT_WACPU_SIZE),
                                                           MCUXCLOSCCASM2_CIPHER_ENCDEC_SIZEOF_WA_PKC(MCUXCLOSCCASM2_SM2P256_SIZE_PRIMEP, MCUXCLOSCCASM2_SM2P256_SIZE_BASEPOINTORDER));
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
    /* Initialize SM2 private key */
    uint32_t privKeyDesc[MCUXCLKEY_DESCRIPTOR_SIZE_IN_WORDS];
    MCUX_CSSL_ANALYSIS_START_PATTERN_REINTERPRET_MEMORY_OF_OPAQUE_TYPES()
    mcuxClKey_Handle_t privKey = (mcuxClKey_Handle_t) privKeyDesc;
    MCUX_CSSL_ANALYSIS_STOP_PATTERN_REINTERPRET_MEMORY_OF_OPAQUE_TYPES()

    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(result_pirv_ki, token_pirv_ki, mcuxClKey_init(
      /* mcuxClSession_Handle_t session         */ &session,
      MCUX_CSSL_ANALYSIS_START_SUPPRESS_POINTER_INCOMPATIBLE("privKey has the correct type (mcuxClKey_Handle_t), the cast was valid.")
      /* mcuxClKey_Handle_t key                 */ privKey,
      MCUX_CSSL_ANALYSIS_STOP_SUPPRESS_POINTER_INCOMPATIBLE()
      /* mcuxClKey_Type_t type                  */ mcuxClKey_Type_SM2P256_Std_Private,
      /* const uint8_t * pKeyData              */ pSm2PrivateKey,
      /* uint32_t keyDataLength                */ sizeof(pSm2PrivateKey)
    ));

    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClKey_init) != token_pirv_ki) || (MCUXCLKEY_STATUS_OK != result_pirv_ki))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();

    /* Initialize SM2 public key */
    uint32_t pubKeyDesc[MCUXCLKEY_DESCRIPTOR_SIZE_IN_WORDS];
    MCUX_CSSL_ANALYSIS_START_PATTERN_REINTERPRET_MEMORY_OF_OPAQUE_TYPES()
    mcuxClKey_Handle_t pubKey = (mcuxClKey_Handle_t) pubKeyDesc;
    MCUX_CSSL_ANALYSIS_STOP_PATTERN_REINTERPRET_MEMORY_OF_OPAQUE_TYPES()

    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(result_pub_ki, token_pub_ki, mcuxClKey_init(
      /* mcuxClSession_Handle_t session         */ &session,
      MCUX_CSSL_ANALYSIS_START_SUPPRESS_POINTER_INCOMPATIBLE("pubKey has the correct type (mcuxClKey_Handle_t), the cast was valid.")
      /* mcuxClKey_Handle_t key                 */ pubKey,
      MCUX_CSSL_ANALYSIS_STOP_SUPPRESS_POINTER_INCOMPATIBLE()
      /* mcuxClKey_Type_t type                  */ mcuxClKey_Type_SM2P256_Std_Public,
      /* const uint8_t * pKeyData              */ pSm2PublicKey,
      /* uint32_t keyDataLength                */ sizeof(pSm2PublicKey)
    ));

    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClKey_init) != token_pub_ki) || (MCUXCLKEY_STATUS_OK != result_pub_ki))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();

    /**************************************************************************/
    /* SM2 Decryption                                                         */
    /**************************************************************************/
    uint8_t msg_dec[BYTELENGTH_M];
    uint32_t msg_dec_size = 0u;

    MCUX_CSSL_ANALYSIS_START_SUPPRESS_ESCAPING_LOCAL_ADDRESS("Address of mcg_dec is for internal use only and does not escape")
    MCUX_CSSL_ANALYSIS_START_PATTERN_NULL_POINTER_CONSTANT()
    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(result_dec, token_dec, mcuxClCipher_crypt(
    /* mcuxClSession_Handle_t session, */ &session,
    /* mcuxClKey_Handle_t key,         */ privKey,
    /* mcuxClCipher_Mode_t mode,       */ mcuxClCipher_Mode_SM2_DEC,
    /* const uint8_t * const pIv,     */ NULL,
    /* uint32_t ivLength,             */ 0,
    /* const uint8_t * const pIn,     */ pCipher_SM2,
    /* uint32_t inLength,             */ sizeof(pCipher_SM2),
    /* uint8_t * const pOut,          */ msg_dec,
    /* uint32_t * const pOutLength    */ &msg_dec_size
    ));
    MCUX_CSSL_ANALYSIS_STOP_PATTERN_NULL_POINTER_CONSTANT()
    MCUX_CSSL_ANALYSIS_STOP_SUPPRESS_ESCAPING_LOCAL_ADDRESS()

    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClCipher_crypt) != token_dec) || (MCUXCLCIPHER_STATUS_OK != result_dec))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();

    /* Check that the resultingdecrypted msg size and data match the expectation */
    if(BYTELENGTH_M != msg_dec_size)
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }

    if (!mcuxClCore_assertEqual(msg_dec, pMessage_SM2, msg_dec_size))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }

    /****************************************************************/
    /* OSCCA SM2 Encryption                                         */
    /****************************************************************/
    uint8_t msg_enc[BYTELENGTH_M + 2 * MCUXCLOSCCASM2_SM2P256_SIZE_PRIMEP + 1U + MCUXCLOSCCASM3_OUTPUT_SIZE_SM3];
    uint32_t msg_enc_size = 0u;

    MCUX_CSSL_ANALYSIS_START_PATTERN_NULL_POINTER_CONSTANT()
    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(result_enc, token_enc, mcuxClCipher_crypt(
    /* mcuxClSession_Handle_t session, */ &session,
    /* mcuxClKey_Handle_t key,         */ pubKey,
    /* mcuxClCipher_Mode_t mode,       */ mcuxClCipher_Mode_SM2_ENC,
    /* const uint8_t * const pIv,     */ NULL,
    /* uint32_t ivLength,             */ 0u,
    /* const uint8_t * const pIn,     */ pMessage_SM2,
    /* uint32_t inLength,             */ BYTELENGTH_M,
    /* uint8_t * const pOut,          */ msg_enc,
    /* uint32_t * const pOutLength    */ &msg_enc_size
    ));
    MCUX_CSSL_ANALYSIS_STOP_PATTERN_NULL_POINTER_CONSTANT()

    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClCipher_crypt) != token_enc) || (MCUXCLCIPHER_STATUS_OK != result_enc))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();

    /**************************************************************************/
    /* OSCCA SM2 Decryption                                                   */
    /**************************************************************************/
    uint8_t msg_dec2[BYTELENGTH_M];
    uint32_t msg_dec_size2 = 0u;

    MCUX_CSSL_ANALYSIS_START_PATTERN_NULL_POINTER_CONSTANT()
    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(result_dec2, token_dec2, mcuxClCipher_crypt(
    /* mcuxClSession_Handle_t session, */ &session,
    /* mcuxClKey_Handle_t key,         */ privKey,
    /* mcuxClCipher_Mode_t mode,       */ mcuxClCipher_Mode_SM2_DEC,
    /* const uint8_t * const pIv,     */ NULL,
    /* uint32_t ivLength,             */ 0,
    /* const uint8_t * const pIn,     */ msg_enc,
    /* uint32_t inLength,             */ msg_enc_size,
    /* uint8_t * const pOut,          */ msg_dec2,
    /* uint32_t * const pOutLength    */ &msg_dec_size2
    ));
    MCUX_CSSL_ANALYSIS_STOP_PATTERN_NULL_POINTER_CONSTANT()

    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClCipher_crypt) != token_dec2) || (MCUXCLCIPHER_STATUS_OK != result_dec2))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();

    /* Check that the resultingdecrypted msg size and data match the expectation */
    if(BYTELENGTH_M != msg_dec_size2)
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }

    if (!mcuxClCore_assertEqual(msg_dec2, pMessage_SM2, msg_dec_size2))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }

    /**************************************************************************/
    /* Destroy the current session                                            */
    /**************************************************************************/

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
