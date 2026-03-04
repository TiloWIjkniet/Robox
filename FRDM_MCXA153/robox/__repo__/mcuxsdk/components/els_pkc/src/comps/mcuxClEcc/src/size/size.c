/*--------------------------------------------------------------------------*/
/* Copyright 2021-2024 NXP                                                  */
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

#include <mcuxClCore_Platform.h>
#include <mcuxClCore_Macros.h>
#include <mcuxCsslAnalysis.h>

#include <mcuxClPkc_Types.h>

#include <internal/mcuxClHashModes_Internal_Memory.h>
#include <internal/mcuxClRandomModes_Internal_SizeDefinitions.h>
#ifdef MCUXCL_FEATURE_ECC_ECDSA_DETERMINISTIC
#include <internal/mcuxClRandomModes_Internal_HmacDrbg_Functions.h>
#endif /* MCUXCL_FEATURE_ECC_ECDSA_DETERMINISTIC */
#include <internal/mcuxClKey_Types_Internal.h>

#include <mcuxClEcc_Types.h>
#include <internal/mcuxClEcc_Internal.h>
#include <internal/mcuxClEcc_Weier_Internal.h>
#include <internal/mcuxClEcc_ECDSA_Internal.h>

#include <internal/mcuxClEcc_TwEd_Internal_PkcWaLayout.h>

#include <internal/mcuxClEcc_EdDSA_Internal.h>
#include <internal/mcuxClEcc_EdDSA_Internal_Hash.h>
#include <internal/mcuxClHash_Internal.h>

#include <internal/mcuxClEcc_Mont_Internal_PkcWaLayout.h>


#define SIZEOF_ECCCPUWA_T  (MCUXCLCORE_ALIGN_TO_CPU_WORDSIZE(sizeof(mcuxClEcc_CpuWa_t)))

MCUX_CSSL_ANALYSIS_START_PATTERN_DESCRIPTIVE_IDENTIFIER()
MCUX_CSSL_ANALYSIS_START_PATTERN_OBJ_SIZES()

#ifdef MCUXCL_FEATURE_ELS_ACCESS_PKCRAM_WORKAROUND
volatile uint8_t mcuxClEcc_KeyGen_WaCPU_SIZE   [SIZEOF_ECCCPUWA_T + MCUXCLCORE_ALIGN_TO_CPU_WORDSIZE(sizeof(uint16_t) * (ECC_KEYGEN_NO_OF_BUFFERS    + ECC_KEYGEN_NO_OF_VIRTUALS)) + MCUXCLCORE_ALIGN_TO_CPU_WORDSIZE(MCUXCLRANDOMMODES_CPUWA_MAXSIZE) + MCUXCLCORE_ALIGN_TO_CPU_WORDSIZE(MCUXCLECC_WEIERECC_MAX_SIZE_BASEPOINTORDER + 8u)];
#else
volatile uint8_t mcuxClEcc_KeyGen_WaCPU_SIZE   [SIZEOF_ECCCPUWA_T + MCUXCLCORE_ALIGN_TO_CPU_WORDSIZE(sizeof(uint16_t) * (ECC_KEYGEN_NO_OF_BUFFERS    + ECC_KEYGEN_NO_OF_VIRTUALS)) + MCUXCLCORE_ALIGN_TO_CPU_WORDSIZE(MCUXCLRANDOMMODES_CPUWA_MAXSIZE)];
#endif /* MCUXCL_FEATURE_ELS_ACCESS_PKCRAM_WORKAROUND */
#ifdef MCUXCL_FEATURE_ELS_ACCESS_PKCRAM_WORKAROUND
volatile uint8_t mcuxClEcc_Sign_WaCPU_SIZE     [SIZEOF_ECCCPUWA_T + MCUXCLCORE_ALIGN_TO_CPU_WORDSIZE(sizeof(uint16_t) * (ECC_SIGN_NO_OF_BUFFERS      + ECC_SIGN_NO_OF_VIRTUALS)) + MCUXCLCORE_ALIGN_TO_CPU_WORDSIZE(MCUXCLRANDOMMODES_CPUWA_MAXSIZE) + MCUXCLCORE_ALIGN_TO_CPU_WORDSIZE(MCUXCLECC_WEIERECC_MAX_SIZE_BASEPOINTORDER + 8u)];
#else
volatile uint8_t mcuxClEcc_Sign_WaCPU_SIZE     [SIZEOF_ECCCPUWA_T + MCUXCLCORE_ALIGN_TO_CPU_WORDSIZE(sizeof(uint16_t) * (ECC_SIGN_NO_OF_BUFFERS      + ECC_SIGN_NO_OF_VIRTUALS)) + MCUXCLCORE_ALIGN_TO_CPU_WORDSIZE(MCUXCLRANDOMMODES_CPUWA_MAXSIZE)];
#endif /* MCUXCL_FEATURE_ELS_ACCESS_PKCRAM_WORKAROUND */
#ifdef MCUXCL_FEATURE_ECC_ECDSA_DETERMINISTIC
volatile uint8_t mcuxClEcc_Sign_DeterministicECDSA_WaCPU_SIZE  [SIZEOF_ECCCPUWA_T + MCUXCLCORE_ALIGN_TO_CPU_WORDSIZE(sizeof(uint16_t) * (ECC_SIGN_NO_OF_BUFFERS + ECC_SIGN_NO_OF_VIRTUALS)) + \
                                                               MCUXCLCORE_ALIGN_TO_CPU_WORDSIZE(MCUXCLRANDOM_MODE_DESCRIPTOR_SIZE) + MCUXCLCORE_ALIGN_TO_CPU_WORDSIZE(MCUXCLRANDOMMODES_HMAC_DRBG_MODE_DESCRIPTOR_SIZE) + MCUXCLCORE_ALIGN_TO_CPU_WORDSIZE(MCUXCLRANDOMMODES_HMAC_DRBG_MAX_CONTEXT_SIZE) + \
                                                               MCUXCLRANDOMMODES_HMAC_DRBG_GENERATE_WACPU_SIZE ];
#endif /* MCUXCL_FEATURE_ECC_ECDSA_DETERMINISTIC */
volatile uint8_t mcuxClEcc_Verify_WaCPU_SIZE   [SIZEOF_ECCCPUWA_T + MCUXCLCORE_ALIGN_TO_CPU_WORDSIZE(sizeof(uint16_t) * (ECC_VERIFY_NO_OF_BUFFERS    + ECC_VERIFY_NO_OF_VIRTUALS)) + MCUXCLCORE_ALIGN_TO_CPU_WORDSIZE(MCUXCLRANDOMMODES_CPUWA_MAXSIZE)];
volatile uint8_t mcuxClEcc_PointMult_WaCPU_SIZE[SIZEOF_ECCCPUWA_T + MCUXCLCORE_ALIGN_TO_CPU_WORDSIZE(sizeof(uint16_t) * (ECC_POINTMULT_NO_OF_BUFFERS + ECC_POINTMULT_NO_OF_VIRTUALS)) + MCUXCLCORE_ALIGN_TO_CPU_WORDSIZE(MCUXCLRANDOMMODES_CPUWA_MAXSIZE)];



volatile uint8_t mcuxClEcc_PKC_wordsize[MCUXCLPKC_WORDSIZE];

volatile uint8_t mcuxClEcc_KeyGen_WaPKC_NoOfBuffers   [ECC_KEYGEN_NO_OF_BUFFERS];
volatile uint8_t mcuxClEcc_Sign_WaPKC_NoOfBuffers     [ECC_SIGN_NO_OF_BUFFERS];
volatile uint8_t mcuxClEcc_Verify_WaPKC_NoOfBuffers   [ECC_VERIFY_NO_OF_BUFFERS];
volatile uint8_t mcuxClEcc_PointMult_WaPKC_NoOfBuffers[ECC_POINTMULT_NO_OF_BUFFERS];


volatile uint8_t mcuxClEcc_KeyGen_WaPKC_Size_128   [(ECC_KEYGEN_NO_OF_BUFFERS) * (MCUXCLPKC_ALIGN_TO_PKC_WORDSIZE(16) + MCUXCLPKC_WORDSIZE)];
volatile uint8_t mcuxClEcc_KeyGen_WaPKC_Size_256   [(ECC_KEYGEN_NO_OF_BUFFERS) * (MCUXCLPKC_ALIGN_TO_PKC_WORDSIZE(32) + MCUXCLPKC_WORDSIZE)];
volatile uint8_t mcuxClEcc_KeyGen_WaPKC_Size_384   [(ECC_KEYGEN_NO_OF_BUFFERS) * (MCUXCLPKC_ALIGN_TO_PKC_WORDSIZE(48) + MCUXCLPKC_WORDSIZE)];
volatile uint8_t mcuxClEcc_KeyGen_WaPKC_Size_512   [(ECC_KEYGEN_NO_OF_BUFFERS) * (MCUXCLPKC_ALIGN_TO_PKC_WORDSIZE(64) + MCUXCLPKC_WORDSIZE)];
volatile uint8_t mcuxClEcc_KeyGen_WaPKC_Size_640   [(ECC_KEYGEN_NO_OF_BUFFERS) * (MCUXCLPKC_ALIGN_TO_PKC_WORDSIZE(80) + MCUXCLPKC_WORDSIZE)];

volatile uint8_t mcuxClEcc_Sign_WaPKC_Size_128   [(ECC_SIGN_NO_OF_BUFFERS) * (MCUXCLPKC_ALIGN_TO_PKC_WORDSIZE(16) + MCUXCLPKC_WORDSIZE)];
volatile uint8_t mcuxClEcc_Sign_WaPKC_Size_256   [(ECC_SIGN_NO_OF_BUFFERS) * (MCUXCLPKC_ALIGN_TO_PKC_WORDSIZE(32) + MCUXCLPKC_WORDSIZE)];
volatile uint8_t mcuxClEcc_Sign_WaPKC_Size_384   [(ECC_SIGN_NO_OF_BUFFERS) * (MCUXCLPKC_ALIGN_TO_PKC_WORDSIZE(48) + MCUXCLPKC_WORDSIZE)];
volatile uint8_t mcuxClEcc_Sign_WaPKC_Size_512   [(ECC_SIGN_NO_OF_BUFFERS) * (MCUXCLPKC_ALIGN_TO_PKC_WORDSIZE(64) + MCUXCLPKC_WORDSIZE)];
volatile uint8_t mcuxClEcc_Sign_WaPKC_Size_640   [(ECC_SIGN_NO_OF_BUFFERS) * (MCUXCLPKC_ALIGN_TO_PKC_WORDSIZE(80) + MCUXCLPKC_WORDSIZE)];

volatile uint8_t mcuxClEcc_Verify_WaPKC_Size_128  [(ECC_VERIFY_NO_OF_BUFFERS) * (MCUXCLPKC_ALIGN_TO_PKC_WORDSIZE(16) + MCUXCLPKC_WORDSIZE)];
volatile uint8_t mcuxClEcc_Verify_WaPKC_Size_256  [(ECC_VERIFY_NO_OF_BUFFERS) * (MCUXCLPKC_ALIGN_TO_PKC_WORDSIZE(32) + MCUXCLPKC_WORDSIZE)];
volatile uint8_t mcuxClEcc_Verify_WaPKC_Size_384  [(ECC_VERIFY_NO_OF_BUFFERS) * (MCUXCLPKC_ALIGN_TO_PKC_WORDSIZE(48) + MCUXCLPKC_WORDSIZE)];
volatile uint8_t mcuxClEcc_Verify_WaPKC_Size_512  [(ECC_VERIFY_NO_OF_BUFFERS) * (MCUXCLPKC_ALIGN_TO_PKC_WORDSIZE(64) + MCUXCLPKC_WORDSIZE)];
volatile uint8_t mcuxClEcc_Verify_WaPKC_Size_640  [(ECC_VERIFY_NO_OF_BUFFERS) * (MCUXCLPKC_ALIGN_TO_PKC_WORDSIZE(80) + MCUXCLPKC_WORDSIZE)];

/* ECDSA signature protocol descriptor size */
volatile uint8_t mcuxClEcc_ECDSA_SignatureProtocolDescriptor_SIZE[MCUXCLCORE_ALIGN_TO_CPU_WORDSIZE(sizeof(mcuxClEcc_ECDSA_SignatureProtocolDescriptor_t))];

volatile uint8_t mcuxClKey_Agreement_ECDH_WaPKC_Size_128 [(ECC_POINTMULT_NO_OF_BUFFERS) * (MCUXCLPKC_ALIGN_TO_PKC_WORDSIZE(16) + MCUXCLPKC_WORDSIZE)];
volatile uint8_t mcuxClKey_Agreement_ECDH_WaPKC_Size_256 [(ECC_POINTMULT_NO_OF_BUFFERS) * (MCUXCLPKC_ALIGN_TO_PKC_WORDSIZE(32) + MCUXCLPKC_WORDSIZE)];
volatile uint8_t mcuxClKey_Agreement_ECDH_WaPKC_Size_384 [(ECC_POINTMULT_NO_OF_BUFFERS) * (MCUXCLPKC_ALIGN_TO_PKC_WORDSIZE(48) + MCUXCLPKC_WORDSIZE)];
volatile uint8_t mcuxClKey_Agreement_ECDH_WaPKC_Size_512 [(ECC_POINTMULT_NO_OF_BUFFERS) * (MCUXCLPKC_ALIGN_TO_PKC_WORDSIZE(64) + MCUXCLPKC_WORDSIZE)];
volatile uint8_t mcuxClKey_Agreement_ECDH_WaPKC_Size_640 [(ECC_POINTMULT_NO_OF_BUFFERS) * (MCUXCLPKC_ALIGN_TO_PKC_WORDSIZE(80) + MCUXCLPKC_WORDSIZE)];





volatile uint8_t mcuxClEcc_PointMult_WaPKC_Size_128 [(ECC_POINTMULT_NO_OF_BUFFERS) * (MCUXCLPKC_ALIGN_TO_PKC_WORDSIZE(16) + MCUXCLPKC_WORDSIZE)];
volatile uint8_t mcuxClEcc_PointMult_WaPKC_Size_256 [(ECC_POINTMULT_NO_OF_BUFFERS) * (MCUXCLPKC_ALIGN_TO_PKC_WORDSIZE(32) + MCUXCLPKC_WORDSIZE)];
volatile uint8_t mcuxClEcc_PointMult_WaPKC_Size_384 [(ECC_POINTMULT_NO_OF_BUFFERS) * (MCUXCLPKC_ALIGN_TO_PKC_WORDSIZE(48) + MCUXCLPKC_WORDSIZE)];
volatile uint8_t mcuxClEcc_PointMult_WaPKC_Size_512 [(ECC_POINTMULT_NO_OF_BUFFERS) * (MCUXCLPKC_ALIGN_TO_PKC_WORDSIZE(64) + MCUXCLPKC_WORDSIZE)];
volatile uint8_t mcuxClEcc_PointMult_WaPKC_Size_640 [(ECC_POINTMULT_NO_OF_BUFFERS) * (MCUXCLPKC_ALIGN_TO_PKC_WORDSIZE(80) + MCUXCLPKC_WORDSIZE)];

MCUX_CSSL_ANALYSIS_STOP_PATTERN_OBJ_SIZES()



MCUX_CSSL_ANALYSIS_START_PATTERN_OBJ_SIZES()
#ifdef MCUXCL_FEATURE_ELS_ACCESS_PKCRAM_WORKAROUND
volatile uint8_t mcuxClEcc_MontDH_GenerateKeyPair_Curve25519_WaCPU_SIZE[SIZEOF_ECCCPUWA_T + MCUXCLCORE_ALIGN_TO_CPU_WORDSIZE(sizeof(uint16_t) * (ECC_MONTDH_NO_OF_BUFFERS + ECC_MONTDH_NO_OF_VIRTUALS)) + MCUXCLECC_MONTDH_CURVE25519_SIZE_BASEPOINTORDER + MCUXCLCORE_ALIGN_TO_CPU_WORDSIZE(MCUXCLRANDOMMODES_CPUWA_MAXSIZE)];
#else
volatile uint8_t mcuxClEcc_MontDH_GenerateKeyPair_Curve25519_WaCPU_SIZE[SIZEOF_ECCCPUWA_T + MCUXCLCORE_ALIGN_TO_CPU_WORDSIZE(sizeof(uint16_t) * (ECC_MONTDH_NO_OF_BUFFERS + ECC_MONTDH_NO_OF_VIRTUALS)) + MCUXCLCORE_ALIGN_TO_CPU_WORDSIZE(MCUXCLRANDOMMODES_CPUWA_MAXSIZE)];
#endif /* MCUXCL_FEATURE_ELS_ACCESS_PKCRAM_WORKAROUND */
volatile uint8_t mcuxClEcc_MontDH_KeyAgreement_Curve25519_WaCPU_SIZE[SIZEOF_ECCCPUWA_T + MCUXCLCORE_ALIGN_TO_CPU_WORDSIZE(sizeof(uint16_t) * (ECC_MONTDH_NO_OF_BUFFERS + ECC_MONTDH_NO_OF_VIRTUALS))];
volatile uint8_t mcuxClEcc_MontDH_GenerateKeyPair_Curve25519_WaPKC_SIZE[ECC_MONTDH_NO_OF_BUFFERS * (MCUXCLPKC_ALIGN_TO_PKC_WORDSIZE(MCUXCLECC_MONTDH_CURVE25519_SIZE_PRIMEP) + MCUXCLPKC_WORDSIZE)];
volatile uint8_t mcuxClEcc_MontDH_KeyAgreement_Curve25519_WaPKC_SIZE[ECC_MONTDH_NO_OF_BUFFERS * (MCUXCLPKC_ALIGN_TO_PKC_WORDSIZE(MCUXCLECC_MONTDH_CURVE25519_SIZE_PRIMEP) + MCUXCLPKC_WORDSIZE)];
#ifdef MCUXCL_FEATURE_ELS_ACCESS_PKCRAM_WORKAROUND
volatile uint8_t mcuxClEcc_MontDH_GenerateKeyPair_Curve448_WaCPU_SIZE[SIZEOF_ECCCPUWA_T + MCUXCLCORE_ALIGN_TO_CPU_WORDSIZE(sizeof(uint16_t) * (ECC_MONTDH_NO_OF_BUFFERS + ECC_MONTDH_NO_OF_VIRTUALS)) + MCUXCLECC_MONTDH_CURVE448_SIZE_BASEPOINTORDER + MCUXCLCORE_ALIGN_TO_CPU_WORDSIZE(MCUXCLRANDOMMODES_CPUWA_MAXSIZE)];
#else
volatile uint8_t mcuxClEcc_MontDH_GenerateKeyPair_Curve448_WaCPU_SIZE[SIZEOF_ECCCPUWA_T + MCUXCLCORE_ALIGN_TO_CPU_WORDSIZE(sizeof(uint16_t) * (ECC_MONTDH_NO_OF_BUFFERS + ECC_MONTDH_NO_OF_VIRTUALS)) + MCUXCLCORE_ALIGN_TO_CPU_WORDSIZE(MCUXCLRANDOMMODES_CPUWA_MAXSIZE)];
#endif /* MCUXCL_FEATURE_ELS_ACCESS_PKCRAM_WORKAROUND */
volatile uint8_t mcuxClEcc_MontDH_KeyAgreement_Curve448_WaCPU_SIZE[SIZEOF_ECCCPUWA_T + MCUXCLCORE_ALIGN_TO_CPU_WORDSIZE(sizeof(uint16_t) * (ECC_MONTDH_NO_OF_BUFFERS + ECC_MONTDH_NO_OF_VIRTUALS))];
volatile uint8_t mcuxClEcc_MontDH_GenerateKeyPair_Curve448_WaPKC_SIZE[ECC_MONTDH_NO_OF_BUFFERS * (MCUXCLPKC_ALIGN_TO_PKC_WORDSIZE(MCUXCLECC_MONTDH_CURVE448_SIZE_PRIMEP) + MCUXCLPKC_WORDSIZE)];
volatile uint8_t mcuxClEcc_MontDH_KeyAgreement_Curve448_WaPKC_SIZE[ECC_MONTDH_NO_OF_BUFFERS * (MCUXCLPKC_ALIGN_TO_PKC_WORDSIZE(MCUXCLECC_MONTDH_CURVE448_SIZE_PRIMEP) + MCUXCLPKC_WORDSIZE)];
MCUX_CSSL_ANALYSIS_STOP_PATTERN_OBJ_SIZES()



#define SIZEOF_EDDSA_UPTRT  MCUXCLCORE_ALIGN_TO_CPU_WORDSIZE((sizeof(uint16_t)) * (ECC_EDDSA_NO_OF_VIRTUALS + ECC_EDDSA_NO_OF_BUFFERS))

MCUX_CSSL_ANALYSIS_START_PATTERN_OBJ_SIZES()
volatile uint8_t mcuxClEcc_EdDSA_GenerateKeyPair_Ed25519_WaCPU_SIZE[SIZEOF_ECCCPUWA_T + SIZEOF_EDDSA_UPTRT
                                                                   + MCUXCLCORE_ALIGN_TO_CPU_WORDSIZE(MCUXCLRANDOMMODES_CPUWA_MAXSIZE)
                                                                   + MCUXCLCORE_ALIGN_TO_CPU_WORDSIZE(MCUXCLECC_EDDSA_ED25519_SIZE_PRIVATEKEY)
                                                                   + MCUXCLHASH_INTERNAL_WACPU_SIZE_SHA2_512];
volatile uint8_t mcuxClEcc_EdDSA_GenerateSignature_Ed25519_WaCPU_SIZE[SIZEOF_ECCCPUWA_T + SIZEOF_EDDSA_UPTRT
                                                                     + MCUXCLCORE_ALIGN_TO_CPU_WORDSIZE(sizeof(mcuxClHash_ContextDescriptor_t) + MCUXCLHASH_STATE_SIZE_SHA_512 + MCUXCLHASH_BLOCK_SIZE_SHA_512)
                                                                     + MCUXCLHASH_INTERNAL_WACPU_SIZE_SHA2_512];
volatile uint8_t mcuxClEcc_EdDSA_VerifySignature_Ed25519_WaCPU_SIZE[SIZEOF_ECCCPUWA_T + SIZEOF_EDDSA_UPTRT
                                                                     + MCUXCLCORE_ALIGN_TO_CPU_WORDSIZE(sizeof(mcuxClHash_ContextDescriptor_t) + MCUXCLHASH_STATE_SIZE_SHA_512 + MCUXCLHASH_BLOCK_SIZE_SHA_512)
                                                                     + MCUXCLHASH_INTERNAL_WACPU_SIZE_SHA2_512];

/* byteLenP = byteLenN in both Ed25519 and Ed448. */
volatile uint8_t mcuxClEcc_EdDSA_GenerateKeyPair_Ed25519_WaPKC_SIZE[ECC_EDDSA_NO_OF_BUFFERS * (MCUXCLPKC_ALIGN_TO_PKC_WORDSIZE(MCUXCLECC_EDDSA_ED25519_SIZE_PRIMEP) + MCUXCLPKC_WORDSIZE)];
volatile uint8_t mcuxClEcc_EdDSA_GenerateSignature_Ed25519_WaPKC_SIZE[ECC_EDDSA_NO_OF_BUFFERS * (MCUXCLPKC_ALIGN_TO_PKC_WORDSIZE(MCUXCLECC_EDDSA_ED25519_SIZE_PRIMEP) + MCUXCLPKC_WORDSIZE)];
volatile uint8_t mcuxClEcc_EdDSA_VerifySignature_Ed25519_WaPKC_SIZE[ECC_EDDSA_NO_OF_BUFFERS * (MCUXCLPKC_ALIGN_TO_PKC_WORDSIZE(MCUXCLECC_EDDSA_ED25519_SIZE_PRIMEP) + MCUXCLPKC_WORDSIZE)];


/* EdDSA key pair generation descriptor size */
volatile uint8_t mcuxClEcc_EdDSA_GenerateKeyPairDescriptor_SIZE[MCUXCLCORE_ALIGN_TO_CPU_WORDSIZE(sizeof(mcuxClEcc_EdDSA_GenerateKeyPairDescriptor_t))];

/* EdDSA signature mode generation descriptor size */
volatile uint8_t mcuxClEcc_EdDSA_SignatureProtocolDescriptor_SIZE[MCUXCLCORE_ALIGN_TO_CPU_WORDSIZE(sizeof(mcuxClEcc_EdDSA_SignatureProtocolDescriptor_t))];


MCUX_CSSL_ANALYSIS_STOP_PATTERN_OBJ_SIZES()

MCUX_CSSL_ANALYSIS_STOP_PATTERN_DESCRIPTIVE_IDENTIFIER()

