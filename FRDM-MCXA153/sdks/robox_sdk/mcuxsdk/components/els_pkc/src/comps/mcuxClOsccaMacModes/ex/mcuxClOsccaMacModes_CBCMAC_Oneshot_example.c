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
static const uint8_t sm4CbcMacKey[] = { 0xC2, 0x0A, 0x2B, 0xBC, 0x2F, 0x40, 0x92, 0xC1,
                                        0x3F, 0xFF, 0x8E, 0xC6, 0x55, 0xB4, 0xC5, 0x72 };
/**
 * @brief Plaintext
 */
static const uint8_t sm4CbcMacPtxt[] = { 0x0D, 0x43, 0x46, 0x88, 0x01, 0x80, 0x84, 0x08,
                                         0x20, 0x52, 0x3B, 0x87, 0x2A, 0x9E, 0x2F, 0x30,
                                         0x79, 0xFD, 0xF8, 0x0B, 0x4C, 0xCC, 0x02, 0x7A,
                                         0x60, 0xC4, 0xB8, 0xC9, 0x42, 0x8B, 0x97, 0x7C };
/**
 * @brief Expected result
 */
static const uint8_t sm4CbcMacResult[] = { 0xF1, 0x33, 0x9F, 0x63, 0x93, 0x4E, 0x72, 0xE4,
                                           0xA7, 0x8F, 0x04, 0x7D, 0x9A, 0x9E, 0x7C, 0xCA };

MCUXCLEXAMPLE_FUNCTION(mcuxClOsccaMacModes_CBCMAC_Oneshot_example)
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

    /**************************************************************************/
    /* MAC Computation1                                                       */
    /**************************************************************************/
    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(result_ki, token_ki, mcuxClKey_init(
      /* mcuxClSession_Handle_t session         */ session,
      MCUX_CSSL_ANALYSIS_START_SUPPRESS_POINTER_INCOMPATIBLE("The pointer key points to an object of the right type, the cast was valid.")
      /* mcuxClKey_Handle_t key                 */ key,
      MCUX_CSSL_ANALYSIS_STOP_SUPPRESS_POINTER_INCOMPATIBLE()
      /* mcuxClKey_Type_t type                  */ mcuxClKey_Type_SM4,
      /* const uint8_t * pKeyData              */ sm4CbcMacKey,
      /* uint32_t keyDataLength                */ sizeof(sm4CbcMacKey)
    ));

    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClKey_init) != token_ki) || (MCUXCLKEY_STATUS_OK != result_ki))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();

    /* Declare message buffer and size. */
    uint8_t cbcmacData[sizeof(sm4CbcMacResult)] = {0u};
    uint32_t cbcmac_data_size = 0u;

    MCUX_CSSL_ANALYSIS_START_SUPPRESS_ESCAPING_LOCAL_ADDRESS("Address of cbcmacData is for internal use only and does not escape")
    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(result_mc, token_mc, mcuxClMac_compute(
    /* mcuxClSession_Handle_t session, */ session,
    /* mcuxClKey_Handle_t key,         */ key,
    /* mcuxClCipher_Mode_t mode,       */ mcuxClOsccaMac_Mode_CBCMAC_NoPadding,
    /* const uint8_t * const pIn,     */ sm4CbcMacPtxt,
    /* uint32_t inLength,             */ sizeof(sm4CbcMacPtxt),
    /* uint8_t * const pOut,          */ cbcmacData,
    /* uint32_t * const pOutLength    */ &cbcmac_data_size
    ));
    MCUX_CSSL_ANALYSIS_STOP_SUPPRESS_ESCAPING_LOCAL_ADDRESS()

    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClMac_compute) != token_mc) || (MCUXCLMAC_STATUS_OK != result_mc))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();

    /* Check that the resulting mac size and data match the expectation */
    if(MCUXCLOSCCASM4_CBCMAC_OUTPUT_SIZE != cbcmac_data_size)
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }

    if (!mcuxClCore_assertEqual(cbcmacData, sm4CbcMacResult, cbcmac_data_size))
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
