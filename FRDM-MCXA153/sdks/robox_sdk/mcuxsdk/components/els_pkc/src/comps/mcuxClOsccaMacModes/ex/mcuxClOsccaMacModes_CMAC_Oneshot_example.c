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
static const uint8_t sm4CmacKey1[] = { 0xff, 0xee, 0xdd, 0xcc, 0xbb, 0xaa, 0x99, 0x88,
                                     0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0x00};
/**
* @brief Plaintext
*/
static const uint8_t sm4CmacPtxt1[] = { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
                                      0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10};
/**
* @brief Expected result
*/

static const uint8_t sm4CmacResult1[] = { 0x00, 0xd4, 0x63, 0xb4, 0x9a, 0xf3, 0x52, 0xe2,
                                        0x74, 0xa9, 0x00, 0x55, 0x13, 0x54, 0x2a, 0xd1};

//The different length test data
/**
 * @brief Cryptographic Keys
 */
static const uint8_t sm4CmacKey2[] = { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
                                      0xfe, 0xdc, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10};
/**
 * @brief Plaintext
 */
static const uint8_t sm4CmacPtxt2[] = { 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
                                       0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb,
                                       0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc,
                                       0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd,
                                       0xee};
/**
 * @brief Expected result
 */

static const uint8_t sm4CmacResult2[] = { 0x8a, 0x8a, 0xe9, 0xc0, 0xc8, 0x97, 0x0e, 0x85,
                                         0x21, 0x57, 0x02, 0x10, 0x1a, 0xbf, 0x9c, 0xc6};

//The different length test data
/**
 * @brief Cryptographic Keys
 */
static const uint8_t sm4CmacKey3[] = { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
                                      0xfe, 0xdc, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10};
/**
 * @brief Plaintext
 */
static const uint8_t sm4CmacPtxt3[] = { 0xfb, 0xd1, 0xbe, 0x92, 0x7e, 0x50, 0x3f, 0x16,
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

static const uint8_t sm4CmacResult3[] = { 0x5f, 0x14, 0xc9, 0xa9, 0x20, 0xb2, 0xb4, 0xf0,
                                         0x76, 0xe0, 0xd8, 0xd6, 0xdc, 0x4f, 0xe1, 0xbc};

MCUXCLEXAMPLE_FUNCTION(mcuxClOsccaMacModes_CMAC_Oneshot_example)
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
    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(result_ki1, token_ki1, mcuxClKey_init(
      /* mcuxClSession_Handle_t session         */ session,
      MCUX_CSSL_ANALYSIS_START_SUPPRESS_POINTER_INCOMPATIBLE("The pointer key points to an object of the right type, the cast was valid.")
      /* mcuxClKey_Handle_t key                 */ key,
      MCUX_CSSL_ANALYSIS_STOP_SUPPRESS_POINTER_INCOMPATIBLE()
      /* mcuxClKey_Type_t type                  */ mcuxClKey_Type_SM4,
      /* const uint8_t * pKeyData              */ sm4CmacKey1,
      /* uint32_t keyDataLength                */ sizeof(sm4CmacKey1)
    ));

    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClKey_init) != token_ki1) || (MCUXCLKEY_STATUS_OK != result_ki1))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();

    /* Declare message buffer and size. */
    uint8_t macData[sizeof(sm4CmacResult1)] = {0u};
    uint32_t mac_data_size = 0u;

    MCUX_CSSL_ANALYSIS_START_SUPPRESS_ESCAPING_LOCAL_ADDRESS("Address of macData is for internal use only and does not escape")
    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(result_mc, token_mc, mcuxClMac_compute(
    /* mcuxClSession_Handle_t session, */ session,
    /* mcuxClKey_Handle_t key,         */ key,
    /* mcuxClCipher_Mode_t mode,       */ mcuxClOsccaMac_Mode_CMAC,
    /* const uint8_t * const pIn,     */ sm4CmacPtxt1,
    /* uint32_t inLength,             */ sizeof(sm4CmacPtxt1),
    /* uint8_t * const pOut,          */ macData,
    /* uint32_t * const pOutLength    */ &mac_data_size
    ));
    MCUX_CSSL_ANALYSIS_STOP_SUPPRESS_ESCAPING_LOCAL_ADDRESS()

    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClMac_compute) != token_mc) || (MCUXCLMAC_STATUS_OK != result_mc))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();

    /* Check that the resulting mac size and data match the expectation */
    if(MCUXCLOSCCASM4_CMAC_OUTPUT_SIZE != mac_data_size)
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }

    if (!mcuxClCore_assertEqual(macData, sm4CmacResult1, mac_data_size))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }

    /**************************************************************************/
    /* MAC Computation2                                                       */
    /**************************************************************************/
    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(result_ki2, token_ki2, mcuxClKey_init(
      /* mcuxClSession_Handle_t session         */ session,
      MCUX_CSSL_ANALYSIS_START_SUPPRESS_POINTER_INCOMPATIBLE("The pointer key points to an object of the right type, the cast was valid.")
      /* mcuxClKey_Handle_t key                 */ key,
      MCUX_CSSL_ANALYSIS_STOP_SUPPRESS_POINTER_INCOMPATIBLE()
      /* mcuxClKey_Type_t type                  */ mcuxClKey_Type_SM4,
      /* const uint8_t * pKeyData              */ sm4CmacKey2,
      /* uint32_t keyDataLength                */ sizeof(sm4CmacKey2)
    ));

    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClKey_init) != token_ki2) || (MCUXCLKEY_STATUS_OK != result_ki2))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();

    /* Declare message buffer and size. */
    uint8_t macData2[sizeof(sm4CmacResult2)] = {0u};
    uint32_t mac_data_size2 = 0u;

    MCUX_CSSL_ANALYSIS_START_SUPPRESS_ESCAPING_LOCAL_ADDRESS("Address of macData2 is for internal use only and does not escape")
    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(result_mc2, token_mc2, mcuxClMac_compute(
    /* mcuxClSession_Handle_t session, */ session,
    /* mcuxClKey_Handle_t key,         */ key,
    /* mcuxClCipher_Mode_t mode,       */ mcuxClOsccaMac_Mode_CMAC,
    /* const uint8_t * const pIn,     */ sm4CmacPtxt2,
    /* uint32_t inLength,             */ sizeof(sm4CmacPtxt2),
    /* uint8_t * const pOut,          */ macData2,
    /* uint32_t * const pOutLength    */ &mac_data_size2
    ));
    MCUX_CSSL_ANALYSIS_STOP_SUPPRESS_ESCAPING_LOCAL_ADDRESS()

    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClMac_compute) != token_mc2) || (MCUXCLMAC_STATUS_OK != result_mc2))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();

    /* Check that the resulting mac size and data match the expectation */
    if(MCUXCLOSCCASM4_CMAC_OUTPUT_SIZE != mac_data_size2)
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }

    if (!mcuxClCore_assertEqual(macData2, sm4CmacResult2, mac_data_size2))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }

    /**************************************************************************/
    /* MAC Computation3                                                       */
    /**************************************************************************/
    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(result_ki3, token_ki3, mcuxClKey_init(
      /* mcuxClSession_Handle_t session         */ session,
      MCUX_CSSL_ANALYSIS_START_SUPPRESS_POINTER_INCOMPATIBLE("The pointer key points to an object of the right type, the cast was valid.")
      /* mcuxClKey_Handle_t key                 */ key,
      MCUX_CSSL_ANALYSIS_STOP_SUPPRESS_POINTER_INCOMPATIBLE()
      /* mcuxClKey_Type_t type                  */ mcuxClKey_Type_SM4,
      /* const uint8_t * pKeyData              */ sm4CmacKey3,
      /* uint32_t keyDataLength                */ sizeof(sm4CmacKey3)
    ));

    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClKey_init) != token_ki3) || (MCUXCLKEY_STATUS_OK != result_ki3))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();

    /* Declare message buffer and size. */
    uint8_t macData3[sizeof(sm4CmacResult3)] = {0u};
    uint32_t mac_data_size3 = 0u;

    MCUX_CSSL_ANALYSIS_START_SUPPRESS_ESCAPING_LOCAL_ADDRESS("Address of macData3 is for internal use only and does not escape")
    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(result_mc3, token_mc3, mcuxClMac_compute(
    /* mcuxClSession_Handle_t session, */ session,
    /* mcuxClKey_Handle_t key,         */ key,
    /* mcuxClCipher_Mode_t mode,       */ mcuxClOsccaMac_Mode_CMAC,
    /* const uint8_t * const pIn,     */ sm4CmacPtxt3,
    /* uint32_t inLength,             */ sizeof(sm4CmacPtxt3),
    /* uint8_t * const pOut,          */ macData3,
    /* uint32_t * const pOutLength    */ &mac_data_size3
    ));
    MCUX_CSSL_ANALYSIS_STOP_SUPPRESS_ESCAPING_LOCAL_ADDRESS()

    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClMac_compute) != token_mc3) || (MCUXCLMAC_STATUS_OK != result_mc3))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();

    /* Check that the resulting mac size and data match the expectation */
    if(MCUXCLOSCCASM4_CMAC_OUTPUT_SIZE != mac_data_size3)
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }

    if(!mcuxClCore_assertEqual(macData3, sm4CmacResult3, mac_data_size3))
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
