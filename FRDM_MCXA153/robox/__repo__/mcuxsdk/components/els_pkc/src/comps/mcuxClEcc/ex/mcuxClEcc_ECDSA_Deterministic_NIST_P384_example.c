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
 * @file  mcuxClEcc_ECDSA_Deterministic_NIST_P384_example.c
 * @brief Example for the mcuxClEcc component
 *
 * @example mcuxClEcc_ECDSA_Deterministic_NIST_P384_example.c
 * @brief   Example for the mcuxClEcc component deterministic ECDSA signature generation using the test vectors
 *          from from Section A.2.6 of rfc 6979 (test case "With SHA-384, message = "sample"")
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


/* Prime p of NIST P-384 as used in Section A.2.6 of rfc 6979 */
static const ALIGNED uint8_t pP[MCUXCLECC_WEIERECC_NIST_P384_SIZE_PRIMEP] =
{
    /* p = 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFFFF0000000000000000FFFFFFFF [BE] */
    0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu,
    0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFEu,
    0xFFu, 0xFFu, 0xFFu, 0xFFu, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0xFFu, 0xFFu, 0xFFu, 0xFFu
};

/* Base point order n of NIST P-384 as used in Section A.2.6 of rfc 6979 */
static const ALIGNED uint8_t pN[MCUXCLECC_WEIERECC_NIST_P384_SIZE_BASEPOINTORDER] =
{
    /* n = 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFC7634D81F4372DDF581A0DB248B0A77AECEC196ACCC52973 [BE] */
    0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu,
    0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xC7u, 0x63u, 0x4Du, 0x81u, 0xF4u, 0x37u, 0x2Du, 0xDFu,
    0x58u, 0x1Au, 0x0Du, 0xB2u, 0x48u, 0xB0u, 0xA7u, 0x7Au, 0xECu, 0xECu, 0x19u, 0x6Au, 0xCCu, 0xC5u, 0x29u, 0x73u
};

/* Curve parameter a of NIST P-384 as used in Section A.2.6 of rfc 6979 */
static const ALIGNED uint8_t pA[MCUXCLECC_WEIERECC_NIST_P384_SIZE_PRIMEP] =
{
    /* a = 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFFFF0000000000000000FFFFFFFC [BE] */
    0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu,
    0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFEu,
    0xFFu, 0xFFu, 0xFFu, 0xFFu, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0xFFu, 0xFFu, 0xFFu, 0xFCu
};

/* Curve parameter b of NIST P-384 as used in Section A.2.6 of rfc 6979 */
static const ALIGNED uint8_t pB[MCUXCLECC_WEIERECC_NIST_P384_SIZE_PRIMEP] =
{
    /* b = 0xB3312FA7E23EE7E4988E056BE3F82D19181D9C6EFE8141120314088F5013875AC656398D8A2ED19D2A85C8EDD3EC2AEF [BE] */
    0xB3u, 0x31u, 0x2Fu, 0xA7u, 0xE2u, 0x3Eu, 0xE7u, 0xE4u, 0x98u, 0x8Eu, 0x05u, 0x6Bu, 0xE3u, 0xF8u, 0x2Du, 0x19u,
    0x18u, 0x1Du, 0x9Cu, 0x6Eu, 0xFEu, 0x81u, 0x41u, 0x12u, 0x03u, 0x14u, 0x08u, 0x8Fu, 0x50u, 0x13u, 0x87u, 0x5Au,
    0xC6u, 0x56u, 0x39u, 0x8Du, 0x8Au, 0x2Eu, 0xD1u, 0x9Du, 0x2Au, 0x85u, 0xC8u, 0xEDu, 0xD3u, 0xECu, 0x2Au, 0xEFu
};

/* Base point of NIST P-384 as used in Section A.2.6 of rfc 6979 */
static const ALIGNED uint8_t pG[2u * MCUXCLECC_WEIERECC_NIST_P384_SIZE_PRIMEP] =
{
    /* G.x = 0xAA87CA22BE8B05378EB1C71EF320AD746E1D3B628BA79B9859F741E082542A385502F25DBF55296C3A545E3872760AB7 [BE] */
    0xAAu, 0x87u, 0xCAu, 0x22u, 0xBEu, 0x8Bu, 0x05u, 0x37u, 0x8Eu, 0xB1u, 0xC7u, 0x1Eu, 0xF3u, 0x20u, 0xADu, 0x74u,
    0x6Eu, 0x1Du, 0x3Bu, 0x62u, 0x8Bu, 0xA7u, 0x9Bu, 0x98u, 0x59u, 0xF7u, 0x41u, 0xE0u, 0x82u, 0x54u, 0x2Au, 0x38u,
    0x55u, 0x02u, 0xF2u, 0x5Du, 0xBFu, 0x55u, 0x29u, 0x6Cu, 0x3Au, 0x54u, 0x5Eu, 0x38u, 0x72u, 0x76u, 0x0Au, 0xB7u,
    /* G.y = 0x3617DE4A96262C6F5D9E98BF9292DC29F8F41DBD289A147CE9DA3113B5F0B8C00A60B1CE1D7E819D7A431D7C90EA0E5F [BE] */
    0x36u, 0x17u, 0xDEu, 0x4Au, 0x96u, 0x26u, 0x2Cu, 0x6Fu, 0x5Du, 0x9Eu, 0x98u, 0xBFu, 0x92u, 0x92u, 0xDCu, 0x29u,
    0xF8u, 0xF4u, 0x1Du, 0xBDu, 0x28u, 0x9Au, 0x14u, 0x7Cu, 0xE9u, 0xDAu, 0x31u, 0x13u, 0xB5u, 0xF0u, 0xB8u, 0xC0u,
    0x0Au, 0x60u, 0xB1u, 0xCEu, 0x1Du, 0x7Eu, 0x81u, 0x9Du, 0x7Au, 0x43u, 0x1Du, 0x7Cu, 0x90u, 0xEAu, 0x0Eu, 0x5Fu
};

/* Input message taken from Section A.2.6 of rfc 6979 */
static const ALIGNED uint8_t pPrivKey[MCUXCLECC_WEIERECC_NIST_P384_SIZE_PRIVATEKEY] =
{
    /* private key = 0x6B9D3DAD2E1B8C1C05B19875B6659F4DE23C3B667BF297BA9AA47740787137D896D5724E4C70A825F872C9EA60D2EDF5 [BE] */
    0x6Bu, 0x9Du, 0x3Du, 0xADu, 0x2Eu, 0x1Bu, 0x8Cu, 0x1Cu, 0x05u, 0xB1u, 0x98u, 0x75u, 0xB6u, 0x65u, 0x9Fu, 0x4Du,
    0xE2u, 0x3Cu, 0x3Bu, 0x66u, 0x7Bu, 0xF2u, 0x97u, 0xBAu, 0x9Au, 0xA4u, 0x77u, 0x40u, 0x78u, 0x71u, 0x37u, 0xD8u,
    0x96u, 0xD5u, 0x72u, 0x4Eu, 0x4Cu, 0x70u, 0xA8u, 0x25u, 0xF8u, 0x72u, 0xC9u, 0xEAu, 0x60u, 0xD2u, 0xEDu, 0xF5u
};

/* Input message taken from Section A.2.6 of rfc 6979 (test case "With SHA-384, message = "sample"") */
static const ALIGNED uint8_t pMessage[6u] =
{
    /* message ="sample" [utf-8] */
    0x73u, 0x61u, 0x6Du, 0x70u, 0x6Cu, 0x65u
};

/* Reference signature taken from Section A.2.6 of rfc 6979 (test case "With SHA-384, message = "sample"") */
static const ALIGNED uint8_t pRefSignature[MCUXCLECC_WEIERECC_NIST_P384_SIZE_SIGNATURE] =
{
    0x94u, 0xEDu, 0xBBu, 0x92u, 0xA5u, 0xECu, 0xB8u, 0xAAu, 0xD4u, 0x73u, 0x6Eu, 0x56u, 0xC6u, 0x91u, 0x91u, 0x6Bu,
    0x3Fu, 0x88u, 0x14u, 0x06u, 0x66u, 0xCEu, 0x9Fu, 0xA7u, 0x3Du, 0x64u, 0xC4u, 0xEAu, 0x95u, 0xADu, 0x13u, 0x3Cu,
    0x81u, 0xA6u, 0x48u, 0x15u, 0x2Eu, 0x44u, 0xACu, 0xF9u, 0x6Eu, 0x36u, 0xDDu, 0x1Eu, 0x80u, 0xFAu, 0xBEu, 0x46u,
    0x99u, 0xEFu, 0x4Au, 0xEBu, 0x15u, 0xF1u, 0x78u, 0xCEu, 0xA1u, 0xFEu, 0x40u, 0xDBu, 0x26u, 0x03u, 0x13u, 0x8Fu,
    0x13u, 0x0Eu, 0x74u, 0x0Au, 0x19u, 0x62u, 0x45u, 0x26u, 0x20u, 0x3Bu, 0x63u, 0x51u, 0xD0u, 0xA3u, 0xA9u, 0x4Fu,
    0xA3u, 0x29u, 0xC1u, 0x45u, 0x78u, 0x6Eu, 0x67u, 0x9Eu, 0x7Bu, 0x82u, 0xC7u, 0x1Au, 0x38u, 0x62u, 0x8Au, 0xC8u
};

MCUXCLEXAMPLE_FUNCTION(mcuxClEcc_ECDSA_Deterministic_NIST_P384_example)
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
    MCUXCLEXAMPLE_ALLOCATE_AND_INITIALIZE_SESSION(sessionHandle, MCUXCLEXAMPLE_MAX_WA(MCUXCLECC_SIGN_DETERMINISTIC_ECDSA_WACPU_SIZE, MCUXCLRANDOMMODES_NCINIT_WACPU_SIZE), MCUXCLECC_SIGN_WAPKC_SIZE_384);

    MCUXCLEXAMPLE_INITIALIZE_PRNG(sessionHandle);

    /* Choose hash algorithm to be used for both, the message hashing and the HMAC computations
     * involved in deterministic ECDSA signature generation */
    mcuxClHash_Algo_t hashAlgorithm = mcuxClHash_Algorithm_Sha384;

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

    ALIGNED uint8_t pHash[MCUXCLHASH_OUTPUT_SIZE_SHA_384];
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

    ALIGNED uint8_t pSignature[MCUXCLECC_WEIERECC_NIST_P384_SIZE_SIGNATURE];

    MCUXCLBUFFER_INIT_RO(buffA, NULL, pA, MCUXCLECC_WEIERECC_NIST_P384_SIZE_PRIMEP);
    MCUXCLBUFFER_INIT_RO(buffB, NULL, pB, MCUXCLECC_WEIERECC_NIST_P384_SIZE_PRIMEP);
    MCUXCLBUFFER_INIT_RO(buffP, NULL, pP, MCUXCLECC_WEIERECC_NIST_P384_SIZE_PRIMEP);
    MCUXCLBUFFER_INIT_RO(buffG, NULL, pG, MCUXCLECC_WEIERECC_NIST_P384_SIZE_PRIMEP * 2u);
    MCUXCLBUFFER_INIT_RO(buffN, NULL, pN, MCUXCLECC_WEIERECC_NIST_P384_SIZE_BASEPOINTORDER);

    MCUXCLBUFFER_INIT(buffSignature, NULL, pSignature, MCUXCLECC_WEIERECC_NIST_P384_SIZE_SIGNATURE);

    mcuxClEcc_Sign_Param_t params;
    params.curveParam.pA = buffA;
    params.curveParam.pB = buffB;
    params.curveParam.pP = buffP;
    params.curveParam.pG = buffG;
    params.curveParam.pN = buffN;
    params.curveParam.misc = mcuxClEcc_DomainParam_misc_Pack(MCUXCLECC_WEIERECC_NIST_P384_SIZE_BASEPOINTORDER, MCUXCLECC_WEIERECC_NIST_P384_SIZE_PRIMEP);
    params.pHash = buffHash;
    params.pPrivateKey = pPrivKey;
    params.pSignature = buffSignature;
    params.optLen = mcuxClEcc_Sign_Param_optLen_Pack(MCUXCLHASH_OUTPUT_SIZE_SHA_384);
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
    if(!mcuxClCore_assertEqual(pSignature, pRefSignature, MCUXCLECC_WEIERECC_NIST_P384_SIZE_SIGNATURE))
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
