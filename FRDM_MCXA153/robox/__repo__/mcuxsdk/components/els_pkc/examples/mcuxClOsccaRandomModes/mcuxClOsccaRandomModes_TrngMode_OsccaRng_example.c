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
 * @file  mcuxClOsccaRandomModes_TrngMode_OsccaRng_example.c
 * @brief Example for the mcuxClOsccaRandomModes component
 *
 * @example mcuxClOsccaRandomModes_TrngMode_OsccaRng_example.c
 * @brief   Example for the mcuxClOsccaRandomModes component
 */

#include <mcuxClRandom.h>
#include <mcuxClRandomModes.h>
#include <mcuxClOsccaRandomModes.h>
#include <mcuxClSession.h>
#include <mcuxCsslFlowProtection.h>
#include <mcuxClCore_FunctionIdentifiers.h> // Code flow protection
#include <mcuxClExample_Session_Helper.h>
#include <mcuxClCore_Examples.h>
#include <mcuxClExample_ELS_Helper.h>

/** Performs an example usage of the mcuxClOsccaRandomModes component with OSCCA TRNG mode.
 * @retval true  The example code completed successfully
 * @retval false The example code failed */
MCUXCLEXAMPLE_FUNCTION(mcuxClOsccaRandomModes_TrngMode_OsccaRng_example)
{
    /**************************************************************************/
    /* Preparation                                                            */
    /**************************************************************************/

    /** Initialize ELS, Enable the ELS **/
    if(!mcuxClExample_Els_Init(MCUXCLELS_RESET_DO_NOT_CANCEL))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }

    mcuxClSession_Descriptor_t sessionDesc;
    mcuxClSession_Handle_t session = &sessionDesc;
    MCUXCLEXAMPLE_ALLOCATE_AND_INITIALIZE_SESSION(session, MCUXCLRANDOMMODES_NCINIT_WACPU_SIZE, 0u);

    uint32_t ctx[MCUXCLOSCCARANDOMMODES_OSCCARNG_CONTEXT_SIZE_IN_WORDS];
    MCUX_CSSL_ANALYSIS_START_PATTERN_REINTERPRET_MEMORY_OF_OPAQUE_TYPES()
    mcuxClRandom_Context_t pCtx = (mcuxClRandom_Context_t)ctx;
    MCUX_CSSL_ANALYSIS_STOP_PATTERN_REINTERPRET_MEMORY_OF_OPAQUE_TYPES()

    /* Initialize a DRBG in OSCCA TRNG mode. */
    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(randomInitRet, InitToken, mcuxClRandom_init(
        session,
        MCUX_CSSL_ANALYSIS_START_SUPPRESS_POINTER_INCOMPATIBLE("pCtx has the correct type (mcuxClRandom_Context_t), the cast was valid.")
        pCtx,
        MCUX_CSSL_ANALYSIS_STOP_SUPPRESS_POINTER_INCOMPATIBLE()
        mcuxClOsccaRandomModes_Mode_TRNG));
    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClRandom_init) != InitToken) || (MCUXCLRANDOM_STATUS_OK != randomInitRet))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }

    MCUX_CSSL_FP_FUNCTION_CALL_END();

    /* Initialize the PRNG */
    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(prngInitRet, prngInitToken, mcuxClRandom_ncInit(session));
    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClRandom_ncInit) != prngInitToken) || (MCUXCLRANDOM_STATUS_OK != prngInitRet))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();

    /**************************************************************************/
    /* Generate several random byte strings                                   */
    /**************************************************************************/
    /* Buffers to store the generated random values in. */
    uint8_t random_buffer1[3u];
    uint8_t random_buffer2[16u];
    uint8_t random_buffer3[31u];

    /* Generate random values of smaller amount than one CPU word size. */
    MCUX_CSSL_ANALYSIS_START_SUPPRESS_ESCAPING_LOCAL_ADDRESS("Address of random_buffer1 is for internal use only and does not escape")
    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(rg1_status, generate1_token, mcuxClRandom_generate(
                                        session,
                                        random_buffer1,
                                        sizeof(random_buffer1)));
    MCUX_CSSL_ANALYSIS_STOP_SUPPRESS_ESCAPING_LOCAL_ADDRESS()

    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClRandom_generate) != generate1_token) || (MCUXCLRANDOM_STATUS_OK != rg1_status))
    {
      return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();

    /* Generate random values of multiple of CPU word size. */
    MCUX_CSSL_ANALYSIS_START_SUPPRESS_ESCAPING_LOCAL_ADDRESS("Address of random_buffer2 is for internal use only and does not escape")
    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(rg2_status, generate2_token, mcuxClRandom_generate(
                                        session,
                                        random_buffer2,
                                        sizeof(random_buffer2)));
    MCUX_CSSL_ANALYSIS_STOP_SUPPRESS_ESCAPING_LOCAL_ADDRESS()

    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClRandom_generate) != generate2_token) || (MCUXCLRANDOM_STATUS_OK != rg2_status))
    {
      return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();

    /* Generate random values of larger amount than but not multiple of one CPU word size. */
    MCUX_CSSL_ANALYSIS_START_SUPPRESS_ESCAPING_LOCAL_ADDRESS("Address of random_buffer3 is for internal use only and does not escape")
    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(rg3_status, generate3_token, mcuxClRandom_generate(
                                        session,
                                        random_buffer3,
                                        sizeof(random_buffer3)));
    MCUX_CSSL_ANALYSIS_STOP_SUPPRESS_ESCAPING_LOCAL_ADDRESS()

    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClRandom_generate) != generate3_token) || (MCUXCLRANDOM_STATUS_OK != rg3_status))
    {
      return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();

    /**************************************************************************/
    /* Cleanup                                                                */
    /**************************************************************************/

    /* Random uninit. */
    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(ru_status, uninit_token, mcuxClRandom_uninit(session));

    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClRandom_uninit) != uninit_token) || (MCUXCLRANDOM_STATUS_OK != ru_status))
    {
      return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();

    /** Destroy Session and cleanup Session **/
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
