/*--------------------------------------------------------------------------*/
/* Copyright 2023 NXP                                                       */
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
 * @file  mcuxClEls_Ecc_Verify_Sha_Direct_Hash_in_parallel_example.c
 * @brief Example to show usage of ECDSA Verify and SHA-direct operations in parallel
 *        using the ELS (CLNS component mcuxClEls).
 *        ECC KeyGen and ECDSA Sign are done as preparation for the signature verification.
 */

#include <mcuxClToolchain.h>
#include <mcuxClEls.h> // Interface to the entire mcuxClEls component
#include <mcuxCsslFlowProtection.h>
#include <mcuxClCore_FunctionIdentifiers.h> // Code flow protection
#include <mcuxClCore_Examples.h>
#include <mcuxClExample_ELS_Helper.h>
#include <mcuxClExample_ELS_Key_Helper.h>

/** Pre-hashed data to be signed and verified.
 *  Data needs to be pre-hashed s.t. ECDSA operations can be run in parallel with
 *  SHA direct.
 */
static const uint32_t ecc_digest[MCUXCLELS_HASH_OUTPUT_SIZE_SHA_256 / sizeof(uint32_t)] = {
    0x11111111u, 0x22222222u, 0x33333333u, 0x44444444u,
    0x55555555u, 0x66666666u, 0x77777777u, 0x88888888u
};

/** Data to be hashed and verified (SHA2-256 hashing with SHA direct). */
static const uint8_t ecc_input_to_hash[MCUXCLELS_HASH_BLOCK_SIZE_SHA_256] = {
    0x61u, 0x62u, 0x63u, 0x80u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x18u
};

/** Expected hash value for SHA2-256 SHA direct operation of the data to verify. */
static uint8_t sha256_reference_digest[MCUXCLELS_HASH_OUTPUT_SIZE_SHA_256] = {
    0xbau, 0x78u, 0x16u, 0xbfu, 0x8fu, 0x01u, 0xcfu, 0xeau,
    0x41u, 0x41u, 0x40u, 0xdeu, 0x5du, 0xaeu, 0x22u, 0x23u,
    0xb0u, 0x03u, 0x61u, 0xa3u, 0x96u, 0x17u, 0x7au, 0x9cu,
    0xb4u, 0x10u, 0xffu, 0x61u, 0xf2u, 0x00u, 0x15u, 0xadu
};


/** Destination buffer to receive the hash output of the SHA2-256 hashing. */
static uint8_t sha256_digest[MCUXCLELS_HASH_STATE_SIZE_SHA_256];

/** Destination buffer to receive the signature part r of the verifyOptions operation. */
static uint32_t ecc_signature_r[MCUXCLELS_ECC_SIGNATURE_R_SIZE / sizeof(uint32_t)];

/** Concatenation of the ECC signature and public key, needed for the mcuxClEls_EccVerify_Async operation. */
static uint32_t ecc_signature_and_public_key[(MCUXCLELS_ECC_SIGNATURE_SIZE + MCUXCLELS_ECC_PUBLICKEY_SIZE) / sizeof(uint32_t)];


/**
 * Performs an ECDSA Verify operation, followed by a SHA-direct operation started in parallel.
 * Key generation and ECDSA Sign operations are done for setup only.
 *
 * @retval MCUXCLEXAMPLE_STATUS_OK    The example code completed successfully
 * @retval MCUXCLEXAMPLE_STATUS_ERROR The example code failed */
MCUXCLEXAMPLE_FUNCTION(mcuxClEls_Ecc_Verify_Sha_Direct_Hash_in_parallel_example)
{
    /*****************************************************/
    /* Preparation                                       */
    /*****************************************************/

    /* Initialize and enable the ELS */
    if(!mcuxClExample_Els_Init(MCUXCLELS_RESET_DO_NOT_CANCEL))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }

    /*
     * Generate signing key
     */
    mcuxClEls_EccKeyGenOption_t keyGenOptions = {0u};
    keyGenOptions.bits.kgsrc = MCUXCLELS_ECC_OUTPUTKEY_RANDOM;
    keyGenOptions.bits.kgsign = MCUXCLELS_ECC_PUBLICKEY_SIGN_DISABLE;
    keyGenOptions.bits.kgsign_rnd = MCUXCLELS_ECC_NO_RANDOM_DATA;

    mcuxClEls_KeyProp_t keyPropPrivKey = {0u};
    MCUX_CSSL_ANALYSIS_START_PATTERN_0U_1U_ARE_UNSIGNED()
    keyPropPrivKey.bits.upprot_priv = MCUXCLELS_KEYPROPERTY_PRIVILEGED_FALSE;
    keyPropPrivKey.bits.upprot_sec = MCUXCLELS_KEYPROPERTY_SECURE_TRUE;
    MCUX_CSSL_ANALYSIS_STOP_PATTERN_0U_1U_ARE_UNSIGNED()

    mcuxClEls_KeyIndex_t privKeyIdx = 10u;
    uint8_t *pSignatureAndPubKey = (uint8_t *) ecc_signature_and_public_key;
    MCUX_CSSL_ANALYSIS_START_PATTERN_NULL_POINTER_CONSTANT()
    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(result, token, mcuxClEls_EccKeyGen_Async(
        /* mcuxClEls_EccKeyGenOption_t options:        */  keyGenOptions,
        /* mcuxClEls_KeyIndex_t signingKeyIdx:         */  (mcuxClEls_KeyIndex_t) 0u,
        /* mcuxClEls_KeyIndex_t privateKeyIdx:         */  privKeyIdx,
        /* mcuxClEls_KeyProp_t generatedKeyProperties: */  keyPropPrivKey,
        /* uint8_t const * pRandomData:               */  NULL,
        /* uint8_t * pPublicKey:                      */  pSignatureAndPubKey + MCUXCLELS_ECC_SIGNATURE_SIZE
    ));
    MCUX_CSSL_ANALYSIS_STOP_PATTERN_NULL_POINTER_CONSTANT()

    if ((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClEls_EccKeyGen_Async) != token) || (MCUXCLELS_STATUS_OK_WAIT != result))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();

    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(result, token, mcuxClEls_WaitForOperation(MCUXCLELS_ERROR_FLAGS_CLEAR));
    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClEls_WaitForOperation) != token) || (MCUXCLELS_STATUS_OK != result))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();

    /*
     * Sign message digest, create the signature
     */
    mcuxClEls_EccSignOption_t signOptions = {0u};
    signOptions.bits.echashchl = MCUXCLELS_ECC_HASHED;

    MCUX_CSSL_ANALYSIS_START_PATTERN_NULL_POINTER_CONSTANT()
    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(result, token, mcuxClEls_EccSign_Async(
        /* mcuxClEls_EccSignOption_t options: */  signOptions,
        /* mcuxClEls_KeyIndex_t keyIdx:       */  privKeyIdx,
        /* uint8_t const * pInputHash:       */  (const uint8_t *) ecc_digest,
        /* uint8_t const * pInputMessage:    */  NULL,
        /* size_t inputMessageLength:        */  (size_t) 0u,
        /* uint8_t * pOutput:                */  pSignatureAndPubKey
    ));
    MCUX_CSSL_ANALYSIS_STOP_PATTERN_NULL_POINTER_CONSTANT()

    if ((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClEls_EccSign_Async) != token) || (MCUXCLELS_STATUS_OK_WAIT != result))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();

    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(result, token, mcuxClEls_WaitForOperation(MCUXCLELS_ERROR_FLAGS_CLEAR));
    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClEls_WaitForOperation) != token) || (MCUXCLELS_STATUS_OK != result))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();


    /*****************************************************/
    /* Use the ELS SHA-direct feature to generate the    */
    /* hash digest of the next data to verify while a    */
    /* ECDSA Verify operation is still ongoing.          */
    /*****************************************************/

    /*
     * Enable SHA direct mode
     */
    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(result, token, mcuxClEls_ShaDirect_Enable());
    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClEls_ShaDirect_Enable) != token) || (MCUXCLELS_STATUS_OK != result))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();

    /*
     * Verify signature of pre-hashed data
     */

    /* To start an ECDSA Verify (or Sign) operation while SHA direct is enabled, it must be ensured that
       hashing of the challenge is disabled - the digest must be pre-hashed. */
    mcuxClEls_EccVerifyOption_t verifyOptions = {0u};
    verifyOptions.bits.echashchl = MCUXCLELS_ECC_HASHED;

    /* Start ECDSA Verify */
    MCUX_CSSL_ANALYSIS_START_PATTERN_NULL_POINTER_CONSTANT()
    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(result, token, mcuxClEls_EccVerify_Async(
        /* mcuxClEls_EccVerifyOption_t options:  */  verifyOptions,
        /* uint8_t const * pInputHash:          */  (const uint8_t *) ecc_digest,
        /* uint8_t const * pInputMessage:       */  NULL,   // must be NULL s.t. SHA direct can be run in parallel
        /* size_t inputMessageLength:           */  (size_t) 0u,
        /* uint8_t const * pSignatureAndPubKey: */  (const uint8_t *) ecc_signature_and_public_key,
        /* uint8_t * pOutput:                   */  (uint8_t *) ecc_signature_r
    ));
    MCUX_CSSL_ANALYSIS_STOP_PATTERN_NULL_POINTER_CONSTANT()

    if ((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClEls_EccVerify_Async) != token) || (MCUXCLELS_STATUS_OK_WAIT != result))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();

    /* Do not call mcuxClEls_WaitForOperation yet, to allow for a Hash operation to be started in parallel using
       the SHA direct mode. */

    /*
     * Hash the next data to verify while the current ECDSA verification
     * is still ongoing.
     */
    mcuxClEls_HashOption_t hashOptions = {0u};
    hashOptions.bits.hashini = MCUXCLELS_HASH_INIT_ENABLE;
    hashOptions.bits.hashoe = MCUXCLELS_HASH_OUTPUT_ENABLE;
    hashOptions.bits.hashmd = MCUXCLELS_HASH_MODE_SHA_256;

    MCUX_CSSL_ANALYSIS_START_PATTERN_NULL_POINTER_CONSTANT()
    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(result, token, mcuxClEls_Hash_ShaDirect(
        /* mcuxClEls_HashOption_t options:                   */  hashOptions,
        /* uint8_t const * pInput:                          */  ecc_input_to_hash,
        /* size_t inputLength:                              */  sizeof(ecc_input_to_hash),
        /* uint8_t * pDigest:                               */  sha256_digest,
        /* mcuxClEls_TransferToRegisterFunction_t pCallback: */  NULL,
        /* void * pCallerData:                              */  NULL
    ));
    MCUX_CSSL_ANALYSIS_STOP_PATTERN_NULL_POINTER_CONSTANT()

    if ((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClEls_Hash_ShaDirect) != token) || (MCUXCLELS_STATUS_OK != result))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();

    /* Wait for mcuxClEls_EccVerify_Async to finish */
    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(result, token, mcuxClEls_WaitForOperation(MCUXCLELS_ERROR_FLAGS_CLEAR));
    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClEls_WaitForOperation) != token) || (MCUXCLELS_STATUS_OK != result))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();


    /*
     * The newly generated message digest (@ref sha256_digest) can be used again for
     * signature verification of pre-hashed data, while the hash of the next data can be
     * generated again in parallel.
     * Once all data is hashed and verified, SHA direct mode shall be manually disabled.
     */

    /* Disable SHA direct mode - must be done when ELS is not busy */
    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(result, token, mcuxClEls_ShaDirect_Disable());
    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClEls_ShaDirect_Disable) != token) || (MCUXCLELS_STATUS_OK != result))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();


    /*****************************************************/
    /* Verify results and perform clean-ups              */
    /*****************************************************/

    /* Get the ELS HW state to check the ecdsa verify bit */
    mcuxClEls_HwState_t state;
    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(result, token, mcuxClEls_GetHwState(&state));

    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClEls_GetHwState) != token) || (MCUXCLELS_STATUS_OK != result))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();

    /* Check if the signature verification was OK */
    if(MCUXCLELS_STATUS_ECDSAVFY_OK != state.bits.ecdsavfy)
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }

    /* Verify the generated hash digest against the reference */
    if(!mcuxClCore_assertEqual(sha256_digest, sha256_reference_digest, sizeof(sha256_reference_digest)))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }

    /* Delete key in keystore */
    if(!mcuxClExample_Els_KeyDelete(privKeyIdx))
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
