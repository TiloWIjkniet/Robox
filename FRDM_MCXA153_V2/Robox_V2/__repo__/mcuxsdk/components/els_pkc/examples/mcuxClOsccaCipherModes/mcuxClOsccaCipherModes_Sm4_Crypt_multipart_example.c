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

#include <mcuxClCore_Examples.h> // Defines and assertions for examples
#include <mcuxClExample_Session_Helper.h>
#include <mcuxClCipher.h> // Interface to the entire mcuxClOscca_Cipher component
#include <mcuxClOsccaCipherModes.h> // Interface to the entire mcuxClOscca_Cipher component
#include <mcuxClEls.h> // Interface to the entire mcuxClEls component
#include <mcuxClSession.h> // Interface to the entire mcuxClSession component
#include <mcuxClKey.h> // Interface to the entire mcuxClKey component
#include <mcuxClOsccaSm4.h>
#include <mcuxClRandomModes.h>
#include <mcuxCsslFlowProtection.h> // Code flow protection
#include <mcuxClOscca_FunctionIdentifiers.h>
#include <mcuxClToolchain.h> // memory segment definitions
#include <stdbool.h>  // bool type for the example's return code
#include <mcuxClExample_Session_Helper.h>
#include <mcuxClExample_RNG_Helper.h>
#include <mcuxClCore_Examples.h>
#include <mcuxClExample_ELS_Helper.h>

/**
 * @brief Cryptographic Keys
 */
static const uint8_t sm4EcbKey[] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF,
                                    0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10 };
/**
 * @brief Plaintext
 */
static const uint8_t sm4EcbPtxt[] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF,
                                     0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10 };
static const uint8_t sm4CtrPtxt[] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF,
                                     0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10,
                                     0x01, 0x23 };
/**
 * @brief Expected result
 */
static const uint8_t sm4EcbResult[] = {0x68, 0x1e, 0xdf, 0x34, 0xd2, 0x06, 0x96, 0x5e,
                                       0x86, 0xb3, 0xe9, 0x4f, 0x53, 0x6e, 0x42, 0x46 };

static const uint8_t sm4CtrIV[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                   0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};
static const uint8_t sm4CtrCtxt[] = {0x07, 0xBB, 0xD9, 0x06, 0xB4, 0x0D, 0xA5, 0x42,
                                     0xD4, 0x51, 0x4D, 0x1A, 0x97, 0xFC, 0xCB, 0x7A, 0x6E, 0x24};

static uint8_t const sm4CbcPtxt[] = {0x4B, 0xC6, 0x6D, 0x5A, 0x09, 0x4B, 0xC6, 0x6D, 0x5A, 0x09, 0x4B, 0xC6, 0x6D, 0x5A, 0x09, 0x4B,
                                    0x4B, 0xC6, 0x6D, 0x5A, 0x09, 0x4B, 0xC6, 0x6D, 0x5A, 0x09, 0x4B, 0xC6, 0x6D, 0x5A, 0x09, 0x4B};
static uint8_t const sm4CbcIv[] = {0x66,0x8A,0x75,0x47,0x3A,0x2A,0xF7,0x6D,0x1A,0x2F,0x3B,0xD2,0x41,0xC6,0x13,0xDC};
static uint8_t const sm4CbcKey[] = {0x9C,0x82,0x6B,0x8A,0xD5,0xFA,0x78,0xCD,0x13,0x05,0xAA,0xF9,0xC2,0xB6,0x46,0x6C};
static uint8_t const sm4CbcResult[] = {0x71, 0xAB, 0xDD, 0x2C, 0x08, 0xDF, 0xC9, 0x42, 0xBC, 0xEF, 0xDA, 0xD8, 0x2E, 0xEF, 0x85, 0xDA,
                                        0x76, 0x10, 0xC0, 0xA8, 0x1A, 0xD3, 0x76, 0x0B, 0xC8, 0xAE, 0x8C, 0x39, 0x21, 0x3A, 0xA3, 0x79,
                                        0x33, 0x30, 0x4A, 0x2A, 0x25, 0x8E, 0x1A, 0xFF, 0x7F, 0x14, 0xD8, 0x1C, 0xE4, 0x40, 0x04, 0x2F};
MCUXCLEXAMPLE_FUNCTION(mcuxClOsccaCipherModes_Sm4_Crypt_multipart_example)
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
    {
        MCUXCLEXAMPLE_ALLOCATE_AND_INITIALIZE_SESSION(session, MCUXCLEXAMPLE_MAX_WA(MCUXCLOSCCACIPHER_SM4_PROCESS_CPU_WA_BUFFER_SIZE, MCUXCLRANDOMMODES_NCINIT_WACPU_SIZE), 0u);
    }

    /* Initlize the prng */
    MCUXCLEXAMPLE_INITIALIZE_PRNG(session);

    /* Initialize session2 */
    mcuxClSession_Descriptor_t sessionDesc2;
    mcuxClSession_Handle_t session2 = &sessionDesc2;
    {
        MCUXCLEXAMPLE_ALLOCATE_AND_INITIALIZE_SESSION(session2, MCUXCLEXAMPLE_MAX_WA(MCUXCLOSCCACIPHER_SM4_PROCESS_CPU_WA_BUFFER_SIZE, MCUXCLRANDOMMODES_NCINIT_WACPU_SIZE), 0u);
    }

    /* Initlize the prng */
    MCUXCLEXAMPLE_INITIALIZE_PRNG(session2);

    /* Initialize key1 */
    uint32_t keyDesc[MCUXCLKEY_DESCRIPTOR_SIZE_IN_WORDS];
    MCUX_CSSL_ANALYSIS_START_PATTERN_REINTERPRET_MEMORY_OF_OPAQUE_TYPES()
    mcuxClKey_Handle_t key = (mcuxClKey_Handle_t) keyDesc;
    MCUX_CSSL_ANALYSIS_STOP_PATTERN_REINTERPRET_MEMORY_OF_OPAQUE_TYPES()

    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(result_ki, token_ki, mcuxClKey_init(
      /* mcuxClSession_Handle_t session         */ session,
      MCUX_CSSL_ANALYSIS_START_SUPPRESS_POINTER_INCOMPATIBLE("The pointer key points to an object of the right type, the cast was valid.")
      /* mcuxClKey_Handle_t key                 */ key,
      MCUX_CSSL_ANALYSIS_STOP_SUPPRESS_POINTER_INCOMPATIBLE()
      /* mcuxClKey_Type_t type                  */ mcuxClKey_Type_SM4,
      /* const uint8_t * pKeyData              */ sm4EcbKey,
      /* uint32_t keyDataLength                */ sizeof(sm4EcbKey)
    ));

    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClKey_init) != token_ki) || (MCUXCLKEY_STATUS_OK != result_ki))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();

    /* Initialize key2 */
    uint32_t keyDesc2[MCUXCLKEY_DESCRIPTOR_SIZE_IN_WORDS];
    MCUX_CSSL_ANALYSIS_START_PATTERN_REINTERPRET_MEMORY_OF_OPAQUE_TYPES()
    mcuxClKey_Handle_t key2 = (mcuxClKey_Handle_t) keyDesc2;
    MCUX_CSSL_ANALYSIS_STOP_PATTERN_REINTERPRET_MEMORY_OF_OPAQUE_TYPES()

    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(result_ki2, token_ki2, mcuxClKey_init(
      /* mcuxClSession_Handle_t session         */ session2,
      MCUX_CSSL_ANALYSIS_START_SUPPRESS_POINTER_INCOMPATIBLE("The pointer key points to an object of the right type, the cast was valid.")
      /* mcuxClKey_Handle_t key                 */ key2,
      MCUX_CSSL_ANALYSIS_STOP_SUPPRESS_POINTER_INCOMPATIBLE()
      /* mcuxClKey_Type_t type                  */ mcuxClKey_Type_SM4,
      /* const uint8_t * pKeyData              */ sm4CbcKey,
      /* uint32_t keyDataLength                */ sizeof(sm4CbcKey)
    ));

    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClKey_init) != token_ki2) || (MCUXCLKEY_STATUS_OK != result_ki2))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();

    /* Create a buffer for the context */
    uint32_t ctxBuf[MCUXCLOSCCACIPHER_SM4_CONTEXT_SIZE_IN_WORDS];
    MCUX_CSSL_ANALYSIS_START_PATTERN_REINTERPRET_MEMORY_OF_OPAQUE_TYPES()
    mcuxClCipher_Context_t * const pCtx = (mcuxClCipher_Context_t *) ctxBuf;
    MCUX_CSSL_ANALYSIS_STOP_PATTERN_REINTERPRET_MEMORY_OF_OPAQUE_TYPES()
    /* Create a buffer for the context2 */
    uint32_t ctxBuf2[MCUXCLOSCCACIPHER_SM4_CONTEXT_SIZE_IN_WORDS];
    MCUX_CSSL_ANALYSIS_START_PATTERN_REINTERPRET_MEMORY_OF_OPAQUE_TYPES()
    mcuxClCipher_Context_t * const pCtx2 = (mcuxClCipher_Context_t *) ctxBuf2;
    MCUX_CSSL_ANALYSIS_STOP_PATTERN_REINTERPRET_MEMORY_OF_OPAQUE_TYPES()

    /**************************************************************************/
    /* Ecb Enc Init                                                           */
    /**************************************************************************/

    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(result_init, token_init, mcuxClCipher_init(
      /* mcuxClSession_Handle_t session,          */ session,
      MCUX_CSSL_ANALYSIS_START_SUPPRESS_POINTER_INCOMPATIBLE("The pointer pCtx points to an object of the right type, the cast was valid.")
      /* mcuxClCipher_Context_t * const pContext, */ pCtx,
      MCUX_CSSL_ANALYSIS_STOP_SUPPRESS_POINTER_INCOMPATIBLE()
      MCUX_CSSL_ANALYSIS_START_SUPPRESS_POINTER_INCOMPATIBLE("The pointer key points to an object of the right type, the cast was valid.")
      /* mcuxClKey_Handle_t key,                  */ key,
      MCUX_CSSL_ANALYSIS_STOP_SUPPRESS_POINTER_INCOMPATIBLE()
      /* mcuxClCipher_Mode_t mode,                */ mcuxClOscca_Cipher_Mode_SM4_ECB_ENC_NoPadding,
      MCUX_CSSL_ANALYSIS_START_PATTERN_NULL_POINTER_CONSTANT()
      /* const uint8_t * const pIv,              */ NULL,
      MCUX_CSSL_ANALYSIS_STOP_PATTERN_NULL_POINTER_CONSTANT()
      /* uint32_t ivLength,                      */ 0
    ));

    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClCipher_init) != token_init) || (MCUXCLCIPHER_STATUS_OK != result_init))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();

    /**************************************************************************/
    /* Cbc Enc Init                                                           */
    /**************************************************************************/

    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(result_init2, token_init2, mcuxClCipher_init(
      /* mcuxClSession_Handle_t session,          */ session2,
      MCUX_CSSL_ANALYSIS_START_SUPPRESS_POINTER_INCOMPATIBLE("The pointer pCtx2 is of the right type (mcuxClCipher_Context_t), the cast was valid.")
      /* mcuxClCipher_Context_t * const pContext, */ pCtx2,
      MCUX_CSSL_ANALYSIS_STOP_SUPPRESS_POINTER_INCOMPATIBLE()
      MCUX_CSSL_ANALYSIS_START_SUPPRESS_POINTER_INCOMPATIBLE("The pointer key2 is of the right type (mcuxClKey_Handle_t), the cast was valid.")
      /* mcuxClKey_Handle_t key,                  */ key2,
      MCUX_CSSL_ANALYSIS_STOP_SUPPRESS_POINTER_INCOMPATIBLE()
      /* mcuxClCipher_Mode_t mode,                */ mcuxClOscca_Cipher_Mode_SM4_CBC_ENC_PaddingISO9797_1_Method2,
      /* const uint8_t * const pIv,              */ sm4CbcIv,
      /* uint32_t ivLength,                      */ sizeof(sm4CbcIv)
    ));

    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClCipher_init) != token_init2) || (MCUXCLCIPHER_STATUS_OK != result_init2))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();

    /**************************************************************************/
    /* Ecb Process                                                            */
    /**************************************************************************/

	/* Declare message buffer and size. */
    uint8_t msg_ecb_enc[32];
    uint32_t msg_ecb_size = 0u;

    MCUX_CSSL_ANALYSIS_START_SUPPRESS_ESCAPING_LOCAL_ADDRESS("Address of msg_ecb_enc is for internal use only and does not escape")
    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(result_proc, token_proc, mcuxClCipher_process(
      /* mcuxClSession_Handle_t session,         */ session,
      /* mcuxClCipher_Context_t * const pContext */ pCtx,
      /* const uint8_t * const pIn,             */ sm4EcbPtxt,
      /* uint32_t inLength,                     */ sizeof(sm4EcbPtxt),
      /* uint8_t * const pOut,                  */ msg_ecb_enc,
      /* uint32_t * const pOutLength            */ &msg_ecb_size
    ));
    MCUX_CSSL_ANALYSIS_STOP_SUPPRESS_ESCAPING_LOCAL_ADDRESS()

    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClCipher_process) != token_proc) || (MCUXCLCIPHER_STATUS_OK != result_proc))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();

    /**************************************************************************/
    /* Cbc Process                                                            */
    /**************************************************************************/

	/* Declare message buffer and size. */
    uint8_t msg_cbc_enc[sizeof(sm4CbcPtxt) + 16u];
    uint32_t msg_cbc_size = 0u;

    MCUX_CSSL_ANALYSIS_START_SUPPRESS_ESCAPING_LOCAL_ADDRESS("Address of msg_cbc_enc is for internal use only and does not escape")
    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(result_proc2, token_proc2, mcuxClCipher_process(
      /* mcuxClSession_Handle_t session,         */ session2,
      /* mcuxClCipher_Context_t * const pContext */ pCtx2,
      /* const uint8_t * const pIn,             */ sm4CbcPtxt,
      /* uint32_t inLength,                     */ sizeof(sm4CbcPtxt),
      /* uint8_t * const pOut,                  */ msg_cbc_enc,
      /* uint32_t * const pOutLength            */ &msg_cbc_size
    ));
    MCUX_CSSL_ANALYSIS_STOP_SUPPRESS_ESCAPING_LOCAL_ADDRESS()

    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClCipher_process) != token_proc2) || (MCUXCLCIPHER_STATUS_OK != result_proc2))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();


    /**************************************************************************/
    /* Ecb Finish                                                             */
    /**************************************************************************/

    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(result_fin, token_fin, mcuxClCipher_finish(
      /* mcuxClSession_Handle_t session,         */ session,
      /* mcuxClCipher_Context_t * const pContext */ pCtx,
      /* uint8_t * const pOut,                  */ &msg_ecb_enc[msg_ecb_size],
      /* uint32_t * const pOutLength            */ &msg_ecb_size
    ));

    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClCipher_finish) != token_fin) || (MCUXCLCIPHER_STATUS_OK != result_fin))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();

    if (true != mcuxClCore_assertEqual(msg_ecb_enc, sm4EcbResult, msg_ecb_size))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }

    /**************************************************************************/
    /* Cbc Finish                                                             */
    /**************************************************************************/

    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(result_fin2, token_fin2, mcuxClCipher_finish(
      /* mcuxClSession_Handle_t session,         */ session2,
      /* mcuxClCipher_Context_t * const pContext */ pCtx2,
      /* uint8_t * const pOut,                  */ &msg_cbc_enc[msg_cbc_size],
      /* uint32_t * const pOutLength            */ &msg_cbc_size
    ));

    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClCipher_finish) != token_fin2) || (MCUXCLCIPHER_STATUS_OK != result_fin2))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();

    if (true != mcuxClCore_assertEqual(msg_cbc_enc, sm4CbcResult, msg_cbc_size))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }

    /* Declare message buffer and size. */
    uint8_t msg_ctr_enc[sizeof(sm4CtrPtxt)];
    uint32_t msg_ctr_size = 0u;

    /**************************************************************************/
    /* Init                                                                   */
    /**************************************************************************/

    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(result_init, token_init, mcuxClCipher_init(
      /* mcuxClSession_Handle_t session,          */ session,
      MCUX_CSSL_ANALYSIS_START_SUPPRESS_POINTER_INCOMPATIBLE("The pointer pCtx points to an object of the right type, the cast was valid.")
      /* mcuxClCipher_Context_t * const pContext, */ pCtx,
      MCUX_CSSL_ANALYSIS_STOP_SUPPRESS_POINTER_INCOMPATIBLE()
      MCUX_CSSL_ANALYSIS_START_SUPPRESS_POINTER_INCOMPATIBLE("The pointer key points to an object of the right type, the cast was valid.")
      /* mcuxClKey_Handle_t key,                  */ key,
      MCUX_CSSL_ANALYSIS_STOP_SUPPRESS_POINTER_INCOMPATIBLE()
      /* mcuxClCipher_Mode_t mode,                */ mcuxClOscca_Cipher_Mode_SM4_CTR_ENC,
      /* const uint8_t * const pIv,              */ sm4CtrIV,
      /* uint32_t ivLength,                      */ sizeof(sm4CtrIV)
    ));

    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClCipher_init) != token_init) || (MCUXCLCIPHER_STATUS_OK != result_init))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();


    /**************************************************************************/
    /* Process                                                                */
    /**************************************************************************/

    MCUX_CSSL_ANALYSIS_START_SUPPRESS_ESCAPING_LOCAL_ADDRESS("Address of msg_ctr_enc is for internal use only and does not escape")
    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(result_proc, token_proc, mcuxClCipher_process(
      /* mcuxClSession_Handle_t session,         */ session,
      /* mcuxClCipher_Context_t * const pContext */ pCtx,
      /* const uint8_t * const pIn,             */ sm4CtrPtxt,
      /* uint32_t inLength,                     */ 16u,
      /* uint8_t * const pOut,                  */ msg_ctr_enc,
      /* uint32_t * const pOutLength            */ &msg_ctr_size
    ));
    MCUX_CSSL_ANALYSIS_STOP_SUPPRESS_ESCAPING_LOCAL_ADDRESS()

    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClCipher_process) != token_proc) || (MCUXCLCIPHER_STATUS_OK != result_proc))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();

    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(result_proc, token_proc, mcuxClCipher_process(
      /* mcuxClSession_Handle_t session,         */ session,
      /* mcuxClCipher_Context_t * const pContext */ pCtx,
      /* const uint8_t * const pIn,             */ sm4CtrPtxt + 16u,
      /* uint32_t inLength,                     */ sizeof(sm4CtrPtxt)-16u,
      /* uint8_t * const pOut,                  */ msg_ctr_enc+msg_ctr_size,
      /* uint32_t * const pOutLength            */ &msg_ctr_size
    ));

    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClCipher_process) != token_proc) || (MCUXCLCIPHER_STATUS_OK != result_proc))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();


    /**************************************************************************/
    /* Finish                                                                 */
    /**************************************************************************/

    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(result_fin, token_fin, mcuxClCipher_finish(
      /* mcuxClSession_Handle_t session,         */ session,
      /* mcuxClCipher_Context_t * const pContext */ pCtx,
      /* uint8_t * const pOut,                  */ &msg_ctr_enc[msg_ctr_size],
      /* uint32_t * const pOutLength            */ &msg_ctr_size
    ));

    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClCipher_finish) != token_fin) || (MCUXCLCIPHER_STATUS_OK != result_fin))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();

    if (true != mcuxClCore_assertEqual(msg_ctr_enc, sm4CtrCtxt, msg_ctr_size))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }

    /**************************************************************************/
    /* Cleanup                                                                */
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

    return MCUXCLEXAMPLE_STATUS_OK;
}
