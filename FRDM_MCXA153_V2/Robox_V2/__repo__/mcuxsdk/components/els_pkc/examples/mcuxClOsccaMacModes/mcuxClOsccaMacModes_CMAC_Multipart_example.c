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

#include <mcuxClMac.h> // Interface to the entire mcuxClMac component
#include <mcuxClOsccaMacModes.h> // Interface to the entire mcuxClOsccaMacModes component
#include <mcuxClSession.h> // Interface to the entire mcuxClSession component
#include <mcuxClKey.h> // Interface to the entire mcuxClKey component
#include <mcuxClOsccaSm4.h>
#include <mcuxCsslFlowProtection.h> // Code flow protection
#include <mcuxClOscca_FunctionIdentifiers.h>
#include <mcuxClToolchain.h> // memory segment definitions
#include <stdbool.h>  // bool type for the example's return code
#include <mcuxClExample_Session_Helper.h>
#include <mcuxClCore_Examples.h>
#include <mcuxClExample_ELS_Helper.h>

/**
 * @brief Cryptographic Keys
 */
static const uint8_t sm4CmacKey[] = { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
                                      0xfe, 0xdc, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10};
/**
 * @brief Plaintext
 */
static const uint8_t sm4CmacPtxt[] = { 0xfb, 0xd1, 0xbe, 0x92, 0x7e, 0x50, 0x3f, 0x16,
                                       0xf9, 0xdd, 0xbe, 0x91, 0x73, 0x53, 0x37, 0x1a,
                                       0xfe, 0xdd, 0xba, 0x97, 0x7e, 0x53, 0x3c, 0x1c,
                                       0xfe, 0xd7, 0xbf, 0x9c, 0x75, 0x5f, 0x3e, 0x11,
                                       0xf0, 0xd8, 0xbc, 0x96, 0x73, 0x5c, 0x34, 0x11,
                                       0xf5, 0xdb, 0xb1, 0x99, 0x7a, 0x5a, 0x32, 0x1f,
                                       0xf6, 0xdf, 0xb4, 0x95, 0x7f, 0x5f, 0x3b, 0x17,
                                       0xfd, 0xdb, 0xb1, 0x9b, 0x76, 0x5c, 0x37};
/**
 * @brief Expected result
 */

static const uint8_t sm4CmacResult[] = { 0x5f, 0x14, 0xc9, 0xa9, 0x20, 0xb2, 0xb4, 0xf0,
                                         0x76, 0xe0, 0xd8, 0xd6, 0xdc, 0x4f, 0xe1, 0xbc};

MCUXCLEXAMPLE_FUNCTION(mcuxClOsccaMacModes_CMAC_Multipart_example)
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
    MCUXCLEXAMPLE_ALLOCATE_AND_INITIALIZE_SESSION(session, MCUXCLOSCCAMACMODES_SM4_MAX_CPU_WA_BUFFER_SIZE, 0u);

    /* Initialize key */
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
      /* const uint8_t * pKeyData              */ sm4CmacKey,
      /* uint32_t keyDataLength                */ sizeof(sm4CmacKey)
    ));

    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClKey_init) != token_ki) || (MCUXCLKEY_STATUS_OK != result_ki))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();

    /* Create a buffer for the context */
    uint32_t ctxBuf[0x6D]; //MCUXCLOSCCAMACMODES_CTX_SIZE_IN_WORDS
    MCUX_CSSL_ANALYSIS_START_PATTERN_REINTERPRET_MEMORY_OF_OPAQUE_TYPES()
    mcuxClMac_Context_t * const pCtx = (mcuxClMac_Context_t *) ctxBuf;
    MCUX_CSSL_ANALYSIS_STOP_PATTERN_REINTERPRET_MEMORY_OF_OPAQUE_TYPES()

    /* Declare message buffer and size. */
    uint8_t macData[sizeof(sm4CmacResult)];
    uint32_t mac_data_size = 0u;

    /**************************************************************************/
    /* Init                                                                   */
    /**************************************************************************/

    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(result_init, token_init, mcuxClMac_init(
    /* mcuxClSession_Handle_t session,          */ session,
    MCUX_CSSL_ANALYSIS_START_SUPPRESS_POINTER_INCOMPATIBLE("The pointer ctx points to an object of the right type, the cast was valid.")
    /* mcuxClMac_Context_t * const pContext,    */ pCtx,
    MCUX_CSSL_ANALYSIS_STOP_SUPPRESS_POINTER_INCOMPATIBLE()
    /* mcuxClKey_Handle_t key,                  */ key,
    /* mcuxClMac_Mode_t mode,                   */ mcuxClOsccaMac_Mode_CMAC
    ));

    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClMac_init) != token_init) || (MCUXCLMAC_STATUS_OK != result_init))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();


    /**************************************************************************/
    /* Process                                                                */
    /**************************************************************************/

    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(result_proc, token_proc, mcuxClMac_process(
    /* mcuxClSession_Handle_t session,         */ session,
    /* mcuxClMac_Context_t * const pContext,   */ pCtx,
    /* const uint8_t * const pIn,             */ sm4CmacPtxt,
    /* uint32_t inLength,                     */ 8u
    ));

    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClMac_process) != token_proc) || (MCUXCLMAC_STATUS_OK != result_proc))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();


    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(result_proc2, token_proc2, mcuxClMac_process(
    /* mcuxClSession_Handle_t session,         */ session,
    /* mcuxClMac_Context_t * const pContext,   */ pCtx,
    /* const uint8_t * const pIn,             */ &sm4CmacPtxt[8u],
    /* uint32_t inLength,                     */ sizeof(sm4CmacPtxt) - 8u
    ));

    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClMac_process) != token_proc2) || (MCUXCLMAC_STATUS_OK != result_proc2))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();

    /**************************************************************************/
    /* Finish                                                                 */
    /**************************************************************************/

    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(result_fin, token_fin, mcuxClMac_finish(
    /* mcuxClSession_Handle_t session,         */ session,
    /* mcuxClMac_Context_t * const pContext,   */ pCtx,
    /* uint8_t * const pOut,                  */ macData,
    /* uint32_t * const pOutLength            */ &mac_data_size
    ));

    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClMac_finish) != token_fin) || (MCUXCLMAC_STATUS_OK != result_fin))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();

    /* Check that the resulting mac size and data match the expectation */
    if(MCUXCLOSCCASM4_CMAC_OUTPUT_SIZE != mac_data_size)
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }

    if (!mcuxClCore_assertEqual(macData, sm4CmacResult, mac_data_size))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }

    /**************************************************************************/
    /* Cleanup                                                                */
    /**************************************************************************/

    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(cleanup_result, cleanup_token, mcuxClSession_cleanup(session));

    if(MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClSession_cleanup) != cleanup_token || MCUXCLSESSION_STATUS_OK != cleanup_result)
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }

    MCUX_CSSL_FP_FUNCTION_CALL_END();

    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(destroy_result, destroy_token, mcuxClSession_destroy(session));

    if(MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClSession_destroy) != destroy_token || MCUXCLSESSION_STATUS_OK != destroy_result)
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }

    MCUX_CSSL_FP_FUNCTION_CALL_END();

    /** Disable the ELS **/
    if(!mcuxClExample_Els_Disable())
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }

    return MCUXCLEXAMPLE_STATUS_OK;
}
