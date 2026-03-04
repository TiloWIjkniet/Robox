/*--------------------------------------------------------------------------*/
/* Copyright 2022, 2024 NXP                                                 */
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

#include <mcuxClSession.h>          // Interface to the entire mcuxClSession component
#include <mcuxClHash.h>             // Interface to the entire mcuxClHash component
#include <mcuxClOsccaSm3.h>
#include <mcuxCsslFlowProtection.h> // Code flow protection
#include <mcuxClOscca_FunctionIdentifiers.h>
#include <mcuxClCore_Examples.h>
#include <mcuxClExample_Session_Helper.h>
#include <mcuxClExample_ELS_Helper.h>

/* random vector */
static const uint8_t data1[] = {
   0x61u, 0x62u
};
static const uint8_t data2[] = {
   0x63u
};

static const uint8_t hashExpected[MCUXCLOSCCASM3_OUTPUT_SIZE_SM3] = {
                                        0x66, 0xc7, 0xf0, 0xf4, 0x62, 0xee, 0xed, 0xd9,
                                        0xd1, 0xf2, 0xd4, 0x6b, 0xdc, 0x10, 0xe4, 0xe2,
                                        0x41, 0x67, 0xc4, 0x87, 0x5c, 0xf2, 0xf7, 0xa2,
                                        0x29, 0x7d, 0xa0, 0x2b, 0x8f, 0x4b, 0xa8, 0xe0
                                    };

MCUXCLEXAMPLE_FUNCTION(mcuxClOsccaSm3_streaming_example)
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

    MCUXCLEXAMPLE_ALLOCATE_AND_INITIALIZE_SESSION(session, MCUXCLOSCCASM3_PROCESS_CPU_WA_BUFFER_SIZE_SM3, 0u);

    /**************************************************************************/
    /* Hash computation                                                       */
    /**************************************************************************/

    uint8_t hash[MCUXCLOSCCASM3_OUTPUT_SIZE_SM3];

    uint32_t context[MCUXCLOSCCASM3_CONTEXT_SIZE_IN_WORDS];
    MCUX_CSSL_ANALYSIS_START_PATTERN_REINTERPRET_MEMORY_OF_OPAQUE_TYPES()
    mcuxClHash_Context_t pContext = (mcuxClHash_Context_t) context;
    MCUX_CSSL_ANALYSIS_STOP_PATTERN_REINTERPRET_MEMORY_OF_OPAQUE_TYPES()

    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(result1, token1, mcuxClHash_init(
    /* mcuxCLSession_Handle_t session: */ session,
    MCUX_CSSL_ANALYSIS_START_SUPPRESS_POINTER_INCOMPATIBLE("pContext has the correct type and the cast was save and valid.")
    /* mcuxClHash_Context_t context:   */ pContext,
    MCUX_CSSL_ANALYSIS_STOP_SUPPRESS_POINTER_INCOMPATIBLE()
    /* mcuxClHash_Algo_t  algo:        */ mcuxClOsccaSm3_Algorithm_Sm3
    ));
    // mcuxClHash_init is a flow-protected function: Check the protection token and the return value
    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClHash_init) != token1) || (MCUXCLHASH_STATUS_OK != result1))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();

    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(result2, token2, mcuxClHash_process(
    /* mcuxCLSession_Handle_t session:     */ session,
    MCUX_CSSL_ANALYSIS_START_SUPPRESS_POINTER_INCOMPATIBLE("pContext has the correct type and the cast was save and valid.")
    /* mcuxClHash_Context_t context:       */ pContext,
    MCUX_CSSL_ANALYSIS_STOP_SUPPRESS_POINTER_INCOMPATIBLE()
    /* const uint8_t * const in:          */ data1,
    /* uint32_t inLength:                 */ sizeof(data1)
    ));
    // mcuxClHash_process is a flow-protected function: Check the protection token and the return value
    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClHash_process) != token2) || (MCUXCLHASH_STATUS_OK != result2))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();

    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(result3, token3, mcuxClHash_process(
    /* mcuxCLSession_Handle_t session:       */ session,
    MCUX_CSSL_ANALYSIS_START_SUPPRESS_POINTER_INCOMPATIBLE("pContext has the correct type and the cast was save and valid.")
    /* mcuxClHash_Context_t context:         */ pContext,
    MCUX_CSSL_ANALYSIS_STOP_SUPPRESS_POINTER_INCOMPATIBLE()
    /* const uint8_t * const in:            */ data2,
    /* uint32_t inLength:                   */ sizeof(data2)
    ));
    // mcuxClHash_process is a flow-protected function: Check the protection token and the return value
    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClHash_process) != token3) || (MCUXCLHASH_STATUS_OK != result3))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();

    uint32_t outSize = 0u;
    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(result4, token4, mcuxClHash_finish(
    /* mcuxCLSession_Handle_t session:      */ session,
    MCUX_CSSL_ANALYSIS_START_SUPPRESS_POINTER_INCOMPATIBLE("pContext has the correct type and the cast was save and valid.")
    /* mcuxClHash_Context_t context:        */ pContext,
    MCUX_CSSL_ANALYSIS_STOP_SUPPRESS_POINTER_INCOMPATIBLE()
    /* mcuxCl_Buffer_t pOut                 */ hash,
    /* uint32_t *const pOutSize            */ &outSize
    ));
    // mcuxClHash_finish is a flow-protected function: Check the protection token and the return value
    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClHash_finish) != token4) || (MCUXCLHASH_STATUS_OK != result4))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();

    /**************************************************************************/
    /* Verification                                                           */
    /**************************************************************************/

    /* Check that the resulting hash size and data match the expectation */
    if(MCUXCLOSCCASM3_OUTPUT_SIZE_SM3 != outSize)
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }

    if (!mcuxClCore_assertEqual(hash, hashExpected, sizeof(hash)))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }

    /**************************************************************************/
    /* Destroy the current session                                            */
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
