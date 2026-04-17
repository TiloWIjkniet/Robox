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

#include <mcuxClSession.h>          // Interface to the entire mcuxClSession component
#include <mcuxClHash.h>             // Interface to the entire mcuxClHash component
#include <mcuxClOsccaSm3.h>
#include <mcuxCsslFlowProtection.h> // Code flow protection
#include <mcuxClCore_Examples.h>
#include <mcuxClExample_Session_Helper.h>
#include <mcuxClExample_ELS_Helper.h>

static const uint8_t data[3] = {
    0x61u, 0x62u, 0x63u
};

static const uint8_t hashExpected[MCUXCLOSCCASM3_OUTPUT_SIZE_SM3] = {
                                        0x66, 0xc7, 0xf0, 0xf4, 0x62, 0xee, 0xed, 0xd9,
                                        0xd1, 0xf2, 0xd4, 0x6b, 0xdc, 0x10, 0xe4, 0xe2,
                                        0x41, 0x67, 0xc4, 0x87, 0x5c, 0xf2, 0xf7, 0xa2,
                                        0x29, 0x7d, 0xa0, 0x2b, 0x8f, 0x4b, 0xa8, 0xe0
                                    };

MCUXCLEXAMPLE_FUNCTION(mcuxClOsccaSm3_oneshot_example)
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

    MCUXCLEXAMPLE_ALLOCATE_AND_INITIALIZE_SESSION(session, MCUXCLOSCCASM3_COMPUTE_CPU_WA_BUFFER_SIZE_SM3, 0u);

    /**************************************************************************/
    /* Hash computation                                                       */
    /**************************************************************************/

    uint8_t hash[MCUXCLOSCCASM3_OUTPUT_SIZE_SM3];
    uint32_t hashOutputSize = 0u;
    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(hc_result, hc_token, mcuxClHash_compute(
    /* mcuxClSession_Handle_t session: */       session,
    /* mcuxClHash_Algo_t algorithm:    */       mcuxClOsccaSm3_Algorithm_Sm3,
    /* mcuxCl_InputBuffer_t pIn:       */       data,
    /* uint32_t inSize:               */       sizeof(data),
    /* mcuxCl_Buffer_t pOut            */       hash,
    /* uint32_t *const pOutSize,      */       &hashOutputSize
    ));

    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClHash_compute) != hc_token) || (MCUXCLHASH_STATUS_OK != hc_result))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();

    /**************************************************************************/
    /* Verification                                                           */
    /**************************************************************************/

    /* Check that the resulting hash size and data match the expectation */
    if(MCUXCLOSCCASM3_OUTPUT_SIZE_SM3 != hashOutputSize)
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
