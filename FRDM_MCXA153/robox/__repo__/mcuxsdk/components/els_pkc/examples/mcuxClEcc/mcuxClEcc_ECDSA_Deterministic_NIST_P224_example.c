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
 * @file  mcuxClEcc_ECDSA_Deterministic_NIST_P224_example.c
 * @brief Example for the mcuxClEcc component
 *
 * @example mcuxClEcc_ECDSA_Deterministic_NIST_P224_example.c
 * @brief   Example for the mcuxClEcc component deterministic ECDSA signature generation using the test vectors
 *          from from Section A.2.4 of rfc 6979 (test case "With SHA-256, message = "sample"")
 */

#include <mcuxClToolchain.h>
#include <mcuxClBuffer.h>
#include <mcuxClEcc.h>
#include <mcuxClHash.h>
#include <mcuxClHashModes.h>
#include <mcuxClHmac.h>
#include <mcuxClPkc.h>
#include <mcuxClCore_Examples.h>
#include <mcuxClExample_Session_Helper.h>
#include <mcuxClExample_ELS_Helper.h>
#include <mcuxCsslFlowProtection.h>
#include <mcuxClCore_FunctionIdentifiers.h> // Code flow protection
#include <mcuxClExample_RNG_Helper.h>


/* Prime p of NIST P-224 as used in Section A.2.4 of rfc 6979 */
static const ALIGNED uint8_t pP[MCUXCLECC_WEIERECC_NIST_P224_SIZE_PRIMEP] =
{
    /* p = 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF000000000000000000000001 [BE] */
    0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x01u
};

/* Base point order n of NIST P-224 as used in Section A.2.4 of rfc 6979 */
static const ALIGNED uint8_t pN[MCUXCLECC_WEIERECC_NIST_P224_SIZE_BASEPOINTORDER] =
{
    /* n = 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFF16A2E0B8F03E13DD29455C5C2A3D [BE] */
    0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0x16u, 0xA2u,
    0xE0u, 0xB8u, 0xF0u, 0x3Eu, 0x13u, 0xDDu, 0x29u, 0x45u, 0x5Cu, 0x5Cu, 0x2Au, 0x3Du
};

/* Curve parameter a of NIST P-224 as used in Section A.2.4 of rfc 6979 */
static const ALIGNED uint8_t pA[MCUXCLECC_WEIERECC_NIST_P224_SIZE_PRIMEP] =
{
    /* a = 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFE [BE] */
    0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFEu,
    0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFEu
};

/* Curve parameter b of NIST P-224 as used in Section A.2.4 of rfc 6979 */
static const ALIGNED uint8_t pB[MCUXCLECC_WEIERECC_NIST_P224_SIZE_PRIMEP] =
{
    /* b = 0xB4050A850C04B3ABF54132565044B0B7D7BFD8BA270B39432355FFB4 [BE] */
    0xB4u, 0x05u, 0x0Au, 0x85u, 0x0Cu, 0x04u, 0xB3u, 0xABu, 0xF5u, 0x41u, 0x32u, 0x56u, 0x50u, 0x44u, 0xB0u, 0xB7u,
    0xD7u, 0xBFu, 0xD8u, 0xBAu, 0x27u, 0x0Bu, 0x39u, 0x43u, 0x23u, 0x55u, 0xFFu, 0xB4u
};

/* Base point of NIST P-224 as used in Section A.2.4 of rfc 6979 */
static const ALIGNED uint8_t pG[2u * MCUXCLECC_WEIERECC_NIST_P224_SIZE_PRIMEP] =
{
    /* G.x = 0xB70E0CBD6BB4BF7F321390B94A03C1D356C21122343280D6115C1D21 [BE] */
    0xB7u, 0x0Eu, 0x0Cu, 0xBDu, 0x6Bu, 0xB4u, 0xBFu, 0x7Fu, 0x32u, 0x13u, 0x90u, 0xB9u, 0x4Au, 0x03u, 0xC1u, 0xD3u,
    0x56u, 0xC2u, 0x11u, 0x22u, 0x34u, 0x32u, 0x80u, 0xD6u, 0x11u, 0x5Cu, 0x1Du, 0x21u,
    /* G.y = 0xBD376388B5F723FB4C22DFE6CD4375A05A07476444D5819985007E34 [BE] */
    0xBDu, 0x37u, 0x63u, 0x88u, 0xB5u, 0xF7u, 0x23u, 0xFBu, 0x4Cu, 0x22u, 0xDFu, 0xE6u, 0xCDu, 0x43u, 0x75u, 0xA0u,
    0x5Au, 0x07u, 0x47u, 0x64u, 0x44u, 0xD5u, 0x81u, 0x99u, 0x85u, 0x00u, 0x7Eu, 0x34u
};

/* Input message taken from Section A.2.4 of rfc 6979 */
static const ALIGNED uint8_t pPrivKey[MCUXCLECC_WEIERECC_NIST_P224_SIZE_PRIVATEKEY] =
{
    /* private key = 0xF220266E1105BFE3083E03EC7A3A654651F45E37167E88600BF257C1 [BE] */
    0xF2u, 0x20u, 0x26u, 0x6Eu, 0x11u, 0x05u, 0xBFu, 0xE3u, 0x08u, 0x3Eu, 0x03u, 0xECu, 0x7Au, 0x3Au, 0x65u, 0x46u,
    0x51u, 0xF4u, 0x5Eu, 0x37u, 0x16u, 0x7Eu, 0x88u, 0x60u, 0x0Bu, 0xF2u, 0x57u, 0xC1u
};

/* Input message taken from Section A.2.4 of rfc 6979 (test case "With SHA-256, message = "sample"") */
static const ALIGNED uint8_t pMessage[6u] =
{
    /* message ="sample" [utf-8] */
    0x73u, 0x61u, 0x6Du, 0x70u, 0x6Cu, 0x65u
};

/* Reference signature taken from Section A.2.4 of rfc 6979 (test case "With SHA-256, message = "sample"") */
static const ALIGNED uint8_t pRefSignature[MCUXCLECC_WEIERECC_NIST_P224_SIZE_SIGNATURE] =
{
    0x61u, 0xAAu, 0x3Du, 0xA0u, 0x10u, 0xE8u, 0xE8u, 0x40u, 0x6Cu, 0x65u, 0x6Bu, 0xC4u, 0x77u, 0xA7u, 0xA7u, 0x18u,
    0x98u, 0x95u, 0xE7u, 0xE8u, 0x40u, 0xCDu, 0xFEu, 0x8Fu, 0xF4u, 0x23u, 0x07u, 0xBAu,
    0xBCu, 0x81u, 0x40u, 0x50u, 0xDAu, 0xB5u, 0xD2u, 0x37u, 0x70u, 0x87u, 0x94u, 0x94u, 0xF9u, 0xE0u, 0xA6u, 0x80u,
    0xDCu, 0x1Au, 0xF7u, 0x16u, 0x19u, 0x91u, 0xBDu, 0xE6u, 0x92u, 0xB1u, 0x01u, 0x01u
};

MCUXCLEXAMPLE_FUNCTION(mcuxClEcc_ECDSA_Deterministic_NIST_P224_example)
{
    /******************************************/
    /* Set up the environment                 */
    /******************************************/

    /* Initialize ELS, Enable the ELS */
    if(!mcuxClExample_Els_Init(MCUXCLELS_RESET_DO_NOT_CANCEL))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }

    /* Setup one session to be used by all functions called */
    mcuxClSession_Descriptor_t session;
    mcuxClSession_Handle_t sessionHandle = &session;

    /* Allocate and initialize PKC workarea */
    MCUXCLEXAMPLE_ALLOCATE_AND_INITIALIZE_SESSION(sessionHandle, MCUXCLEXAMPLE_MAX_WA(MCUXCLECC_SIGN_DETERMINISTIC_ECDSA_WACPU_SIZE, MCUXCLRANDOMMODES_NCINIT_WACPU_SIZE), MCUXCLECC_SIGN_WAPKC_SIZE_256);

    MCUXCLEXAMPLE_INITIALIZE_PRNG(sessionHandle);

    /* Choose hash algorithm to be used for both, the message hashing and the HMAC computations
     * involved in deterministic ECDSA signature generation */
    mcuxClHash_Algo_t hashAlgorithm = mcuxClHash_Algorithm_Sha256;

    /**************************************************************************/
    /* Generate an HMAC mode for deterministic ECDSA                          */
    /**************************************************************************/

    uint32_t hmacModeDescBytes[MCUXCLHMAC_HMAC_MODE_DESCRIPTOR_SIZE_IN_WORDS];
    MCUX_CSSL_ANALYSIS_START_PATTERN_REINTERPRET_MEMORY_OF_OPAQUE_TYPES()
    mcuxClMac_CustomMode_t hmacMode = (mcuxClMac_CustomMode_t) hmacModeDescBytes;
    MCUX_CSSL_ANALYSIS_STOP_PATTERN_REINTERPRET_MEMORY_OF_OPAQUE_TYPES()

    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(hashCreateMode_result, hashCreateMode_token, mcuxClHmac_createHmacMode(
    MCUX_CSSL_ANALYSIS_START_SUPPRESS_POINTER_INCOMPATIBLE("The pointer hmacMode points to an object of the right type, the cast was valid.")
    /* mcuxClMac_CustomMode_t mode:       */ hmacMode,
    MCUX_CSSL_ANALYSIS_STOP_SUPPRESS_POINTER_INCOMPATIBLE()
    /* mcuxClHash_Algo_t hashAlgorithm:   */ hashAlgorithm)
    );

    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClHmac_createHmacMode) != hashCreateMode_token) || (MCUXCLMAC_STATUS_OK != hashCreateMode_result))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();

    /**************************************************************************/
    /* Generate a deterministic ECDSA protocol descriptor                     */
    /**************************************************************************/

    /* Allocate space for the deterministic ECDSA signature protocol descriptor. */
    uint32_t signatureProtocolDescBytes[MCUXCLECC_ECDSA_SIGNATURE_PROTOCOL_DESCRIPTOR_SIZE_IN_WORDS];
    MCUX_CSSL_ANALYSIS_START_PATTERN_REINTERPRET_MEMORY_OF_OPAQUE_TYPES()
    mcuxClEcc_ECDSA_SignatureProtocolDescriptor_t *pSignatureProtocolDesc = (mcuxClEcc_ECDSA_SignatureProtocolDescriptor_t *) signatureProtocolDescBytes;
    MCUX_CSSL_ANALYSIS_STOP_PATTERN_REINTERPRET_MEMORY_OF_OPAQUE_TYPES()

    /* Generate deterministic ECDSA protocol descriptor */
    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(genProtocolDesc_result, protocolDesc_token, mcuxClEcc_ECDSA_GenerateProtocolDescriptor_Deterministic(
    /* mcuxClSession_Handle_t session                                      */ sessionHandle,
    MCUX_CSSL_ANALYSIS_START_SUPPRESS_POINTER_INCOMPATIBLE("The pointer pSignatureProtocolDesc points to an object of the right type, the cast was valid.")
    /* mcuxClEcc_ECDSA_SignatureProtocolDescriptor_t *pProtocolDescriptor  */ pSignatureProtocolDesc,
    MCUX_CSSL_ANALYSIS_STOP_SUPPRESS_POINTER_INCOMPATIBLE()
    /* mcuxClMac_Mode_t hmacMode                                           */ hmacMode));

    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClEcc_ECDSA_GenerateProtocolDescriptor_Deterministic) != protocolDesc_token)
        || (MCUXCLECC_STATUS_OK != genProtocolDesc_result))
    {
      return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();


    /**************************************************************************/
    /* Hash the message to be signed                                          */
    /**************************************************************************/

    ALIGNED uint8_t pHash[MCUXCLHASH_OUTPUT_SIZE_SHA_256];
    uint32_t hashOutputSize = 0u;
    MCUXCLBUFFER_INIT_RO(buffMessage, NULL, pMessage, sizeof(pMessage));
    MCUXCLBUFFER_INIT(buffHash, NULL, pHash, sizeof(pHash));

    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(hashCompute_result, hashCompute_token, mcuxClHash_compute(
    /* mcuxClSession_Handle_t session: */ sessionHandle,
    /* mcuxClHash_Algo_t algorithm:    */ hashAlgorithm,
    /* mcuxCl_InputBuffer_t pIn:       */ buffMessage,
    /* uint32_t inSize:               */ sizeof(pMessage),
    /* mcuxCl_Buffer_t pOut            */ buffHash,
    /* uint32_t *const pOutSize,      */ &hashOutputSize
    ));

    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClHash_compute) != hashCompute_token) || (MCUXCLHASH_STATUS_OK != hashCompute_result))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();

    if(sizeof(pHash) != hashOutputSize)
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }


    /**************************************************************************/
    /* Perform deterministic ECDSA signature generation                       */
    /**************************************************************************/

    ALIGNED uint8_t pSignature[MCUXCLECC_WEIERECC_NIST_P224_SIZE_SIGNATURE];

    MCUXCLBUFFER_INIT_RO(buffA, NULL, pA, MCUXCLECC_WEIERECC_NIST_P224_SIZE_PRIMEP);
    MCUXCLBUFFER_INIT_RO(buffB, NULL, pB, MCUXCLECC_WEIERECC_NIST_P224_SIZE_PRIMEP);
    MCUXCLBUFFER_INIT_RO(buffP, NULL, pP, MCUXCLECC_WEIERECC_NIST_P224_SIZE_PRIMEP);
    MCUXCLBUFFER_INIT_RO(buffG, NULL, pG, MCUXCLECC_WEIERECC_NIST_P224_SIZE_PRIMEP * 2u);
    MCUXCLBUFFER_INIT_RO(buffN, NULL, pN, MCUXCLECC_WEIERECC_NIST_P224_SIZE_BASEPOINTORDER);

    MCUXCLBUFFER_INIT(buffSignature, NULL, pSignature, MCUXCLECC_WEIERECC_NIST_P224_SIZE_SIGNATURE);

    mcuxClEcc_Sign_Param_t params;
    params.curveParam.pA = buffA;
    params.curveParam.pB = buffB;
    params.curveParam.pP = buffP;
    params.curveParam.pG = buffG;
    params.curveParam.pN = buffN;
    params.curveParam.misc = mcuxClEcc_DomainParam_misc_Pack(MCUXCLECC_WEIERECC_NIST_P224_SIZE_BASEPOINTORDER, MCUXCLECC_WEIERECC_NIST_P224_SIZE_PRIMEP);
    params.pHash = buffHash;
    params.pPrivateKey = pPrivKey;
    params.pSignature = buffSignature;
    params.optLen = mcuxClEcc_Sign_Param_optLen_Pack(MCUXCLHASH_OUTPUT_SIZE_SHA_256);
    params.pMode = pSignatureProtocolDesc;

    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(detECDSASign_result, detECDSASign_token, mcuxClEcc_Sign(
    /* mcuxClSession_Handle_t pSession        */ sessionHandle,
    /* const mcuxClEcc_Sign_Param_t * pParam */ &params));

    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClEcc_Sign) != detECDSASign_token)
        || (MCUXCLECC_STATUS_OK != detECDSASign_result))
    {
      return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();


    /**************************************************************************/
    /* Compare the generated signature to the reference and clean up          */
    /**************************************************************************/

    /* Compare the generated signature to the reference. */
    if(!mcuxClCore_assertEqual(pSignature, pRefSignature, MCUXCLECC_WEIERECC_NIST_P224_SIZE_SIGNATURE))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }

    /* Destroy Session and cleanup Session */
    if(!mcuxClExample_Session_Clean(sessionHandle))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }

    /* Disable the ELS */
    if(!mcuxClExample_Els_Disable())
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }

    return MCUXCLEXAMPLE_STATUS_OK;
}
