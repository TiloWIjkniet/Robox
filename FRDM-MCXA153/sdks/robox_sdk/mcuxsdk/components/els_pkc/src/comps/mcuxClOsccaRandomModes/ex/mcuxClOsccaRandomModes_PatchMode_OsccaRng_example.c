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
 * @file  mcuxClOsccaRandomModes_PatchMode_OsccaRng_example.c
 * @brief Example for the mcuxClOsccaRandomModes component
 *
 * @example mcuxClOsccaRandomModes_PatchMode_OsccaRng_example.c
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

static const uint8_t randomData[] = {0x8au,0x76u,0x90u,0xd2u,0xd9u,0x55u,0x3cu,0x93u,
                                     0x03u,0x52u,0x3au,0x3cu,0xbeu,0xe1u,0x39u,0xa4u,
                                     0xefu,0xf1u,0xc4u,0xbbu,0xa3u,0xc7u,0x09u,0xf3u,
                                     0xb7u,0x14u,0x07u,0xb2u,0xd8u,0x98u,0xa0u,0xaeu};

/******************************************************************************
 * Local and global function declarations
 ******************************************************************************/
static mcuxClRandom_Status_t RNG_Patch_function(
    mcuxClSession_Handle_t session,
    mcuxClRandom_Context_t pCustomCtx,
    uint8_t *pOut,
    uint32_t outLength
)
{
    (void)session;
    (void)pCustomCtx;
    uint32_t indexRandomData = 0u;
    for (uint32_t i = 0u; i < outLength; i++)
    {
        pOut[i] = randomData[indexRandomData];
        MCUX_CSSL_ANALYSIS_START_SUPPRESS_INTEGER_OVERFLOW("indexRandomData can never be equal to UINT32_MAX because sizeof(randomData) is 32.")
        indexRandomData = (indexRandomData + 1u) % sizeof(randomData);
        MCUX_CSSL_ANALYSIS_STOP_SUPPRESS_INTEGER_OVERFLOW()
    }

    return MCUXCLRANDOM_STATUS_OK;
}

/** Performs an example usage of the mcuxClRandom and mcuxClOsccaRandomModes components with patch mode.
 * @retval true  The example code completed successfully
 * @retval false The example code failed */
MCUXCLEXAMPLE_FUNCTION(mcuxClOsccaRandomModes_PatchMode_OsccaRng_example)
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
    MCUXCLEXAMPLE_ALLOCATE_AND_INITIALIZE_SESSION(session, sizeof(mcuxClSession_Descriptor_t), 0u);

    /* Fill mode descriptor with the relevant data */
    uint32_t customModeDescBytes[(MCUXCLRANDOMMODES_PATCHMODE_DESCRIPTOR_SIZE + sizeof(uint32_t) - 1U)/sizeof(uint32_t)];
    MCUX_CSSL_ANALYSIS_START_PATTERN_REINTERPRET_MEMORY_OF_OPAQUE_TYPES()
    mcuxClRandom_ModeDescriptor_t *mcuxClRandomModes_Mode_Custom = (mcuxClRandom_ModeDescriptor_t *) customModeDescBytes;
    MCUX_CSSL_ANALYSIS_STOP_PATTERN_REINTERPRET_MEMORY_OF_OPAQUE_TYPES()

    /**************************************************************************/
    /* RANDOM Patch Mode creation, use custom function RNG_Patch_function     */
    /**************************************************************************/
    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(cp_status, cp_token, mcuxClRandomModes_createPatchMode(
        MCUX_CSSL_ANALYSIS_START_SUPPRESS_POINTER_INCOMPATIBLE("mcuxClRandomModes_Mode_Custom has the correct type (mcuxClRandom_ModeDescriptor_t), the cast was valid.")
        mcuxClRandomModes_Mode_Custom,
        MCUX_CSSL_ANALYSIS_STOP_SUPPRESS_POINTER_INCOMPATIBLE()
        (mcuxClRandomModes_CustomGenerateAlgorithm_t)RNG_Patch_function,
        MCUX_CSSL_ANALYSIS_START_PATTERN_NULL_POINTER_CONSTANT()
        NULL,
        MCUX_CSSL_ANALYSIS_STOP_PATTERN_NULL_POINTER_CONSTANT()
        128U
    ));

    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClRandomModes_createPatchMode) != cp_token) || (MCUXCLRANDOM_STATUS_OK != cp_status))
    {
      return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();

    /**************************************************************************/
    /* patch mode initialization                                              */
    /**************************************************************************/
    uint32_t ctx[MCUXCLOSCCARANDOMMODES_OSCCARNG_CONTEXT_SIZE_IN_WORDS];
    MCUX_CSSL_ANALYSIS_START_PATTERN_REINTERPRET_MEMORY_OF_OPAQUE_TYPES()
    mcuxClRandom_Context_t rngContextPatched = (mcuxClRandom_Context_t)ctx;
    MCUX_CSSL_ANALYSIS_STOP_PATTERN_REINTERPRET_MEMORY_OF_OPAQUE_TYPES()
    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(ri_status, init_token, mcuxClRandom_init(
        session,
        MCUX_CSSL_ANALYSIS_START_SUPPRESS_POINTER_INCOMPATIBLE("rngContextPatched has the correct type (mcuxClRandom_Context_t), the cast was valid.")
        rngContextPatched,
        MCUX_CSSL_ANALYSIS_STOP_SUPPRESS_POINTER_INCOMPATIBLE()
        mcuxClRandomModes_Mode_Custom
    ));

    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClRandom_init) != init_token) || (MCUXCLRANDOM_STATUS_OK != ri_status))
    {
      return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();

    /**************************************************************************/
    /* Generate several random byte strings                                   */
    /**************************************************************************/
    /* Buffers to store the generated random values in. */
    uint8_t drbg_buffer1[3u];
    uint8_t drbg_buffer2[sizeof(randomData) + 16u];

    /* Generate random values of smaller amount than the size of prepared random data array. */
    MCUX_CSSL_ANALYSIS_START_SUPPRESS_ESCAPING_LOCAL_ADDRESS("Address of drbg_buffer1 is for internal use only and does not escape")
    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(rg1_status, generate1_token, mcuxClRandom_generate(
                                        session,
                                        drbg_buffer1,
                                        sizeof(drbg_buffer1)));
    MCUX_CSSL_ANALYSIS_STOP_SUPPRESS_ESCAPING_LOCAL_ADDRESS()

    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClRandom_generate) != generate1_token) || (MCUXCLRANDOM_STATUS_OK != rg1_status))
    {
      return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();

    /* Check if the generated data meets expectation */
    if(!mcuxClCore_assertEqual(drbg_buffer1, randomData, sizeof(drbg_buffer1)))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }

    /* Generate random values of larger amount than the size of prepared random data array. */
    MCUX_CSSL_ANALYSIS_START_SUPPRESS_ESCAPING_LOCAL_ADDRESS("Address of drbg_buffer2 is for internal use only and does not escape")
    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(rg2_status, generate2_token, mcuxClRandom_generate(
                                        session,
                                        drbg_buffer2,
                                        sizeof(drbg_buffer2)));
    MCUX_CSSL_ANALYSIS_STOP_SUPPRESS_ESCAPING_LOCAL_ADDRESS()

    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClRandom_generate) != generate2_token) || (MCUXCLRANDOM_STATUS_OK != rg2_status))
    {
      return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();

    /* Check if the generated data meets expectation */
    if(!mcuxClCore_assertEqual(drbg_buffer2, randomData, sizeof(randomData)))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    if(!mcuxClCore_assertEqual(drbg_buffer2 + sizeof(randomData), randomData, 16u))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }

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
