#---------------------------------------------
# Copyright 2024 NXP.
# SPDX-License-Identifier: BSD-3-Clause
#---------------------------------------------

if (CONFIG_MCUX_COMPONENT_component.els_pkc.doc.rw61x)
    mcux_add_source(
        SOURCES
                doc/rw61x/html/*.*
                doc/rw61x/html/search/*.*
                LICENSE.txt
                softwareContentRegister.txt
                ReleaseNotes.txt
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )

    mcux_add_include(
        INCLUDES ./
        BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.doc.lpc)
    mcux_add_source(
        SOURCES
                doc/lpc/html/*.*
                doc/lpc/html/search/*.*
                LICENSE.txt
                softwareContentRegister.txt
                ReleaseNotes.txt
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
    mcux_add_include(
        INCLUDES ./
        BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.doc.mcxn)
    mcux_add_source(
        SOURCES
                doc/mcxn/html/*.*
                doc/mcxn/html/search/*.*
                LICENSE.txt
                softwareContentRegister.txt
                ReleaseNotes.txt
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
    mcux_add_include(
        INCLUDES ./
        BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.doc.mimxrt)
    mcux_add_source(
        SOURCES
                doc/mimxrt/html/*.*
                doc/mimxrt/html/search/*.*
                LICENSE.txt
                softwareContentRegister.txt
                ReleaseNotes.txt
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
    mcux_add_include(
        INCLUDES ./
        BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.static_lib.mcxn)
    mcux_add_source(
        SOURCES static_library/mcxn/libclns.a
                static_library/mcxn/libclns.a.libsize
                static_library/mcxn/libclns.a.objsize
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
    mcux_add_include(
        INCLUDES ./
        BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.static_lib.rw61x)
    mcux_add_source(
        SOURCES static_library/rw61x/libclns.a
                static_library/rw61x/libclns.a.libsize
                static_library/rw61x/libclns.a.objsize
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
    mcux_add_include(
        INCLUDES ./
        BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.static_lib.lpc)
    mcux_add_source(
        SOURCES static_library/lpc/libclns.a
                static_library/lpc/libclns.a.libsize
                static_library/lpc/libclns.a.objsize
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
    mcux_add_include(
        INCLUDES ./
        BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.static_lib.mimxrt)
    mcux_add_source(
        SOURCES static_library/mimxrt/libclns.a
                static_library/mimxrt/libclns.a.libsize
                static_library/mimxrt/libclns.a.objsize
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
    mcux_add_include(
        INCLUDES ./
        BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.common)
    mcux_add_source(
        SOURCES src/comps/common/src/mcuxClOscca_CommonOperations.c
                src/comps/common/inc/mcuxClOscca_FunctionIdentifiers.h
                src/comps/common/inc/mcuxClOscca_Memory.h
                src/comps/common/inc/mcuxClOscca_PlatformTypes.h
                src/comps/common/inc/mcuxClOscca_Types.h
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
    mcux_add_include(
        INCLUDES ./
                 src/comps/common/inc
                 BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.aead)
    mcux_add_source(
        SOURCES src/comps/mcuxClAead/src/mcuxClAead.c
                src/comps/mcuxClAead/inc/mcuxClAead.h
                src/comps/mcuxClAead/inc/mcuxClAead_Constants.h
                src/comps/mcuxClAead/inc/mcuxClAead_Functions.h
                src/comps/mcuxClAead/inc/mcuxClAead_Types.h
                src/comps/mcuxClAead/inc/internal/mcuxClAead_Ctx.h
                src/comps/mcuxClAead/inc/internal/mcuxClAead_Descriptor.h
                src/comps/mcuxClAead/inc/internal/mcuxClAead_Internal_Functions.h
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
    mcux_add_include(
        INCLUDES ./
                 src/comps/mcuxClAead/inc
                 src/comps/mcuxClAead/inc/internal
                 BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.aead_modes)
    mcux_add_source(
        SOURCES src/comps/mcuxClAeadModes/src/mcuxClAeadModes_Els_AesCcm.c
                src/comps/mcuxClAeadModes/src/mcuxClAeadModes_Els_AesCcmEngine.c
                src/comps/mcuxClAeadModes/src/mcuxClAeadModes_Els_AesGcm.c
                src/comps/mcuxClAeadModes/src/mcuxClAeadModes_Els_AesGcmEngine.c
                src/comps/mcuxClAeadModes/src/mcuxClAeadModes_Els_Modes.c
                src/comps/mcuxClAeadModes/src/mcuxClAeadModes_Els_Multipart.c
                src/comps/mcuxClAeadModes/src/mcuxClAeadModes_Els_Oneshot.c
                src/comps/mcuxClAeadModes/inc/mcuxClAeadModes.h
#                 src/comps/mcuxClAeadModes/inc/mcuxClAeadModes_MemoryConsumption.h
                src/comps/mcuxClAeadModes/inc/mcuxClAeadModes_Modes.h
                src/comps/mcuxClAeadModes/inc/internal/mcuxClAeadModes_Common.h
                src/comps/mcuxClAeadModes/inc/internal/mcuxClAeadModes_Common_Constants.h
                src/comps/mcuxClAeadModes/inc/internal/mcuxClAeadModes_Common_Functions.h
                src/comps/mcuxClAeadModes/inc/internal/mcuxClAeadModes_Els_Algorithms.h
                src/comps/mcuxClAeadModes/inc/internal/mcuxClAeadModes_Els_Functions.h
                src/comps/mcuxClAeadModes/inc/internal/mcuxClAeadModes_Els_Types.h
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
    mcux_add_include(
        INCLUDES ./
                 src/comps/mcuxClAeadModes/inc
                 src/comps/mcuxClAeadModes/inc/internal
                 BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.aes)
    mcux_add_source(
        SOURCES src/comps/mcuxClAes/src/mcuxClAes_KeyTypes.c
                src/comps/mcuxClAes/inc/mcuxClAes.h
                src/comps/mcuxClAes/inc/mcuxClAes_Constants.h
                src/comps/mcuxClAes/inc/mcuxClAes_KeyTypes.h
                src/comps/mcuxClAes/inc/mcuxClAes_Types.h
                src/comps/mcuxClAes/inc/internal/mcuxClAes_Ctx.h
                src/comps/mcuxClAes/inc/internal/mcuxClAes_Internal_Constants.h
                src/comps/mcuxClAes/inc/internal/mcuxClAes_Internal_Functions.h
                src/comps/mcuxClAes/inc/internal/mcuxClAes_Wa.h
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
    mcux_add_include(
        INCLUDES ./
                 src/comps/mcuxClAes/inc
                 src/comps/mcuxClAes/inc/internal
                 BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.buffer)
    mcux_add_source(
        SOURCES src/comps/mcuxClBuffer/src/mcuxClBuffer.c
                src/comps/mcuxClBuffer/inc/mcuxClBuffer.h
                src/comps/mcuxClBuffer/inc/mcuxClBuffer_Cfg.h
                src/comps/mcuxClBuffer/inc/mcuxClBuffer_Constants.h
                src/comps/mcuxClBuffer/inc/mcuxClBuffer_Impl.h
                src/comps/mcuxClBuffer/inc/mcuxClBuffer_Pointer.h
                src/comps/mcuxClBuffer/inc/internal/mcuxClBuffer_Internal.h
                src/comps/mcuxClBuffer/inc/internal/mcuxClBuffer_Internal_Pointer.h
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
    mcux_add_include(
        INCLUDES ./
                 src/comps/mcuxClBuffer/inc
                 src/comps/mcuxClBuffer/inc/internal
                 BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.cipher)
    mcux_add_source(
        SOURCES src/comps/mcuxClCipher/src/mcuxClCipher.c
                src/comps/mcuxClCipher/inc/mcuxClCipher.h
                src/comps/mcuxClCipher/inc/mcuxClCipher_Constants.h
                src/comps/mcuxClCipher/inc/mcuxClCipher_Functions.h
                src/comps/mcuxClCipher/inc/mcuxClCipher_Types.h
                src/comps/mcuxClCipher/inc/internal/mcuxClCipher_Internal.h
                src/comps/mcuxClCipher/inc/internal/mcuxClCipher_Internal_Constants.h
                src/comps/mcuxClCipher/inc/internal/mcuxClCipher_Internal_Functions.h
                src/comps/mcuxClCipher/inc/internal/mcuxClCipher_Internal_Types.h
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
    mcux_add_include(
        INCLUDES ./
                 src/comps/mcuxClCipher/inc
                 src/comps/mcuxClCipher/inc/internal
                 BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.cipher_modes)
    mcux_add_source(
        SOURCES src/comps/mcuxClCipherModes/src/mcuxClCipherModes_Common_Helper.c
                src/comps/mcuxClCipherModes/src/mcuxClCipherModes_Crypt_Els_Modes.c
                src/comps/mcuxClCipherModes/src/mcuxClCipherModes_Els_Aes.c
                src/comps/mcuxClCipherModes/src/mcuxClCipherModes_Els_AesEngine.c
                src/comps/mcuxClCipherModes/src/mcuxClCipherModes_Els_Aes_Internal.c
                src/comps/mcuxClCipherModes/inc/mcuxClCipherModes.h
#                 src/comps/mcuxClCipherModes/inc/mcuxClCipherModes_MemoryConsumption.h
                src/comps/mcuxClCipherModes/inc/mcuxClCipherModes_Modes.h
                src/comps/mcuxClCipherModes/inc/internal/mcuxClCipherModes_Common.h
                src/comps/mcuxClCipherModes/inc/internal/mcuxClCipherModes_Common_Constants.h
                src/comps/mcuxClCipherModes/inc/internal/mcuxClCipherModes_Common_Helper.h
                src/comps/mcuxClCipherModes/inc/internal/mcuxClCipherModes_Common_Wa.h
                src/comps/mcuxClCipherModes/inc/internal/mcuxClCipherModes_Els_Algorithms.h
                src/comps/mcuxClCipherModes/inc/internal/mcuxClCipherModes_Els_Functions.h
                src/comps/mcuxClCipherModes/inc/internal/mcuxClCipherModes_Els_Types.h
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
    mcux_add_include(
        INCLUDES ./
                 src/comps/mcuxClCipherModes/inc
                 src/comps/mcuxClCipherModes/inc/internal
                 BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.core)
    mcux_add_source(
        SOURCES src/comps/mcuxClCore/inc/mcuxClCore_Examples.h
                src/comps/mcuxClCore/inc/mcuxClCore_FunctionIdentifiers.h
                src/comps/mcuxClCore/inc/mcuxClCore_Macros.h
                src/comps/mcuxClCore/inc/mcuxClCore_Platform.h
                src/comps/mcuxClCore/inc/mcuxClCore_Toolchain.h
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
    mcux_add_include(
        INCLUDES ./
                 src/comps/mcuxClCore/inc/
                 BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.crc)
    mcux_add_source(
        SOURCES src/comps/mcuxClCrc/src/mcuxClCrc.c
                src/comps/mcuxClCrc/src/mcuxClCrc_Sw.c
                src/comps/mcuxClCrc/inc/mcuxClCrc.h
                src/comps/mcuxClCrc/inc/internal/mcuxClCrc_Internal_Constants.h
                src/comps/mcuxClCrc/inc/internal/mcuxClCrc_Internal_Functions.h
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
    mcux_add_include(
        INCLUDES ./
                 src/comps/mcuxClCrc/inc
                 src/comps/mcuxClCrc/inc/internal
                 BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.ecc_deterministic)
    mcux_add_source(
        SOURCES src/comps/mcuxClEcc/src/mcuxClEcc_DeterministicECDSA.c
                src/comps/mcuxClEcc/src/mcuxClEcc_DeterministicECDSA_Internal_BlindedSecretKeyGen.c
                src/comps/mcuxClEcc/src/mcuxClEcc_DeterministicECDSA_Internal_BlindedSecretKeyGen_FUP.c
                src/comps/mcuxClEcc/inc/mcuxClEcc_DeterministicECDSA_Internal_BlindedSecretKeyGen_FUP.h
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
    mcux_add_include(
        INCLUDES ./
                 src/comps/mcuxClEcc/inc/
                 src/comps/mcuxClEcc/inc/internal
                 BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.ecc)
    mcux_add_source(
        SOURCES src/comps/mcuxClEcc/src/mcuxClEcc_Constants.c
                src/comps/mcuxClEcc/src/mcuxClEcc_ECDSA_GenerateProtocolDescriptor.c
                src/comps/mcuxClEcc/src/mcuxClEcc_ECDSA_Internal_BlindedSecretKeyGen.c
                src/comps/mcuxClEcc/src/mcuxClEcc_ECDSA_Internal_BlindedSecretKeyGen_FUP.c
                src/comps/mcuxClEcc/src/mcuxClEcc_EdDSA_GenerateKeyPair.c
                src/comps/mcuxClEcc/src/mcuxClEcc_EdDSA_GenerateKeyPair_FUP.c
                src/comps/mcuxClEcc/src/mcuxClEcc_EdDSA_GenerateSignature.c
                src/comps/mcuxClEcc/src/mcuxClEcc_EdDSA_GenerateSignatureMode.c
                src/comps/mcuxClEcc/src/mcuxClEcc_EdDSA_GenerateSignature_FUP.c
                src/comps/mcuxClEcc/src/mcuxClEcc_EdDSA_InitPrivKeyInputMode.c
                src/comps/mcuxClEcc/src/mcuxClEcc_EdDSA_Internal_CalcHashModN.c
                src/comps/mcuxClEcc/src/mcuxClEcc_EdDSA_Internal_CalcHashModN_FUP.c
                src/comps/mcuxClEcc/src/mcuxClEcc_EdDSA_Internal_DecodePoint_Ed25519.c
                src/comps/mcuxClEcc/src/mcuxClEcc_EdDSA_Internal_DecodePoint_FUP.c
                src/comps/mcuxClEcc/src/mcuxClEcc_EdDSA_Internal_EncodePoint.c
                src/comps/mcuxClEcc/src/mcuxClEcc_EdDSA_Internal_SetupEnvironment.c
                src/comps/mcuxClEcc/src/mcuxClEcc_EdDSA_Internal_SignatureMechanisms.c
                src/comps/mcuxClEcc/src/mcuxClEcc_EdDSA_VerifySignature.c
                src/comps/mcuxClEcc/src/mcuxClEcc_Internal_BlindedScalarMult.c
                src/comps/mcuxClEcc/src/mcuxClEcc_Internal_Convert_FUP.c
                src/comps/mcuxClEcc/src/mcuxClEcc_Internal_GenerateMultiplicativeBlinding.c
                src/comps/mcuxClEcc/src/mcuxClEcc_Internal_InterleaveScalar.c
                src/comps/mcuxClEcc/src/mcuxClEcc_Internal_InterleaveTwoScalars.c
                src/comps/mcuxClEcc/src/mcuxClEcc_Internal_Interleave_FUP.c
                src/comps/mcuxClEcc/src/mcuxClEcc_Internal_PointComparison_FUP.c
                src/comps/mcuxClEcc/src/mcuxClEcc_Internal_RecodeAndReorderScalar.c
                src/comps/mcuxClEcc/src/mcuxClEcc_Internal_SetupEnvironment.c
                src/comps/mcuxClEcc/src/mcuxClEcc_Internal_SetupEnvironment_FUP.c
                src/comps/mcuxClEcc/src/mcuxClEcc_Internal_Types.c
                src/comps/mcuxClEcc/src/mcuxClEcc_KeyTypes.c
                src/comps/mcuxClEcc/src/mcuxClEcc_MontDH_GenerateKeyPair.c
                src/comps/mcuxClEcc/src/mcuxClEcc_MontDH_KeyAgreement.c
                src/comps/mcuxClEcc/src/mcuxClEcc_Mont_Internal_DhSetupEnvironment.c
                src/comps/mcuxClEcc/src/mcuxClEcc_Mont_Internal_MontDhX.c
                src/comps/mcuxClEcc/src/mcuxClEcc_Mont_Internal_MontDhX_FUP.c
                src/comps/mcuxClEcc/src/mcuxClEcc_Mont_Internal_SecureScalarMult_XZMontLadder.c
                src/comps/mcuxClEcc/src/mcuxClEcc_Mont_Internal_SecureScalarMult_XZMontLadder_FUP.c
                src/comps/mcuxClEcc/src/mcuxClEcc_TwEd_Internal_FixScalarMult.c
                src/comps/mcuxClEcc/src/mcuxClEcc_TwEd_Internal_PlainFixScalarMult25519.c
                src/comps/mcuxClEcc/src/mcuxClEcc_TwEd_Internal_PlainPtrSelectComb.c
                src/comps/mcuxClEcc/src/mcuxClEcc_TwEd_Internal_PlainPtrSelectML.c
                src/comps/mcuxClEcc/src/mcuxClEcc_TwEd_Internal_PlainVarScalarMult.c
                src/comps/mcuxClEcc/src/mcuxClEcc_TwEd_Internal_PointArithmetic.c
                src/comps/mcuxClEcc/src/mcuxClEcc_TwEd_Internal_PointArithmetic_FUP.c
                src/comps/mcuxClEcc/src/mcuxClEcc_TwEd_Internal_PointArithmeticEd25519.c
                src/comps/mcuxClEcc/src/mcuxClEcc_TwEd_Internal_PointArithmeticEd25519_FUP.c
                src/comps/mcuxClEcc/src/mcuxClEcc_TwEd_Internal_PointSubtraction_FUP.c
                src/comps/mcuxClEcc/src/mcuxClEcc_TwEd_Internal_PointValidation_FUP.c
                src/comps/mcuxClEcc/src/mcuxClEcc_TwEd_Internal_PrecPointImportAndValidate.c
                src/comps/mcuxClEcc/src/mcuxClEcc_TwEd_Internal_VarScalarMult.c
                src/comps/mcuxClEcc/src/mcuxClEcc_TwEd_Internal_VarScalarMult_FUP.c
                src/comps/mcuxClEcc/src/mcuxClEcc_WeierECC_Internal_BlindedSecretKeyGen.c
                src/comps/mcuxClEcc/src/mcuxClEcc_WeierECC_Internal_BlindedSecretKeyGen_FUP.c
                src/comps/mcuxClEcc/src/mcuxClEcc_WeierECC_Internal_GenerateKeyPair.c
#                 src/comps/mcuxClEcc/src/mcuxClEcc_WeierECC_Internal_KeyAgreement_ECDH.c
                src/comps/mcuxClEcc/src/mcuxClEcc_WeierECC_Internal_SetupEnvironment.c
                src/comps/mcuxClEcc/src/mcuxClEcc_Weier_Internal_ConvertPoint_FUP.c
                src/comps/mcuxClEcc/src/mcuxClEcc_Weier_Internal_FUP.c
                src/comps/mcuxClEcc/src/mcuxClEcc_Weier_Internal_PointArithmetic.c
                src/comps/mcuxClEcc/src/mcuxClEcc_Weier_Internal_PointArithmetic_FUP.c
                src/comps/mcuxClEcc/src/mcuxClEcc_Weier_Internal_PointCheck.c
                src/comps/mcuxClEcc/src/mcuxClEcc_Weier_Internal_PointCheck_FUP.c
                src/comps/mcuxClEcc/src/mcuxClEcc_Weier_Internal_PointMult.c
                src/comps/mcuxClEcc/src/mcuxClEcc_Weier_Internal_SecurePointMult_CoZMontLadder.c
                src/comps/mcuxClEcc/src/mcuxClEcc_Weier_Internal_SecurePointMult_CoZMontLadder_FUP.c
                src/comps/mcuxClEcc/src/mcuxClEcc_Weier_Internal_SetupEnvironment.c
                src/comps/mcuxClEcc/src/mcuxClEcc_Weier_KeyGen.c
                src/comps/mcuxClEcc/src/mcuxClEcc_Weier_KeyGen_FUP.c
                src/comps/mcuxClEcc/src/mcuxClEcc_Weier_PointMult.c
                src/comps/mcuxClEcc/src/mcuxClEcc_Weier_PointMult_FUP.c
                src/comps/mcuxClEcc/src/mcuxClEcc_Weier_Sign.c
                src/comps/mcuxClEcc/src/mcuxClEcc_Weier_Sign_FUP.c
                src/comps/mcuxClEcc/src/mcuxClEcc_Weier_Verify.c
                src/comps/mcuxClEcc/src/mcuxClEcc_Weier_Verify_FUP.c
                src/comps/mcuxClEcc/inc/mcuxClEcc.h
                src/comps/mcuxClEcc/inc/mcuxClEcc_Constants.h
                src/comps/mcuxClEcc/inc/mcuxClEcc_EdDSA_GenerateKeyPair_FUP.h
                src/comps/mcuxClEcc/inc/mcuxClEcc_EdDSA_GenerateSignature_FUP.h
                src/comps/mcuxClEcc/inc/mcuxClEcc_EdDSA_Internal_CalcHashModN_FUP.h
                src/comps/mcuxClEcc/inc/mcuxClEcc_EdDSA_Internal_DecodePoint_FUP.h
                src/comps/mcuxClEcc/inc/mcuxClEcc_Functions.h
                src/comps/mcuxClEcc/inc/mcuxClEcc_Internal_Convert_FUP.h
                src/comps/mcuxClEcc/inc/mcuxClEcc_Internal_Interleave_FUP.h
                src/comps/mcuxClEcc/inc/mcuxClEcc_Internal_PointComparison_FUP.h
                src/comps/mcuxClEcc/inc/mcuxClEcc_Internal_SetupEnvironment_FUP.h
                src/comps/mcuxClEcc/inc/mcuxClEcc_KeyTypes.h
#                 src/comps/mcuxClEcc/inc/mcuxClEcc_MemoryConsumption.h
                src/comps/mcuxClEcc/inc/mcuxClEcc_Mont_Internal_MontDhX_FUP.h
                src/comps/mcuxClEcc/inc/mcuxClEcc_Mont_Internal_SecureScalarMult_XZMontLadder_FUP.h
                src/comps/mcuxClEcc/inc/mcuxClEcc_TwEd_Internal_PointArithmeticEd25519_FUP.h
                src/comps/mcuxClEcc/inc/mcuxClEcc_TwEd_Internal_PointSubtraction_FUP.h
                src/comps/mcuxClEcc/inc/mcuxClEcc_TwEd_Internal_PointArithmetic_FUP.h
                src/comps/mcuxClEcc/inc/mcuxClEcc_TwEd_Internal_PointValidation_FUP.h
                src/comps/mcuxClEcc/inc/mcuxClEcc_TwEd_Internal_VarScalarMult_FUP.h
                src/comps/mcuxClEcc/inc/mcuxClEcc_Types.h
                src/comps/mcuxClEcc/inc/mcuxClEcc_WeierECC_Internal_BlindedSecretKeyGen_FUP.h
                src/comps/mcuxClEcc/inc/mcuxClEcc_Weier_Internal_ConvertPoint_FUP.h
                src/comps/mcuxClEcc/inc/mcuxClEcc_Weier_Internal_FUP.h
                src/comps/mcuxClEcc/inc/mcuxClEcc_Weier_Internal_PointArithmetic_FUP.h
                src/comps/mcuxClEcc/inc/mcuxClEcc_Weier_Internal_PointCheck_FUP.h
                src/comps/mcuxClEcc/inc/mcuxClEcc_Weier_Internal_SecurePointMult_CoZMontLadder_FUP.h
                src/comps/mcuxClEcc/inc/mcuxClEcc_Weier_KeyGen_FUP.h
                src/comps/mcuxClEcc/inc/mcuxClEcc_Weier_PointMult_FUP.h
                src/comps/mcuxClEcc/inc/mcuxClEcc_Weier_Sign_FUP.h
                src/comps/mcuxClEcc/inc/mcuxClEcc_Weier_Verify_FUP.h
                src/comps/mcuxClEcc/inc/internal/mcuxClEcc_ECDH_Internal.h
                src/comps/mcuxClEcc/inc/internal/mcuxClEcc_ECDSA_Internal.h
                src/comps/mcuxClEcc/inc/internal/mcuxClEcc_ECDSA_Internal_FUP.h
                src/comps/mcuxClEcc/inc/internal/mcuxClEcc_EdDSA_Internal.h
                src/comps/mcuxClEcc/inc/internal/mcuxClEcc_EdDSA_Internal_FUP.h
                src/comps/mcuxClEcc/inc/internal/mcuxClEcc_EdDSA_Internal_Hash.h
                src/comps/mcuxClEcc/inc/internal/mcuxClEcc_Internal.h
                src/comps/mcuxClEcc/inc/internal/mcuxClEcc_Internal_FUP.h
                src/comps/mcuxClEcc/inc/internal/mcuxClEcc_Internal_PkcWaLayout.h
                src/comps/mcuxClEcc/inc/internal/mcuxClEcc_Internal_Random.h
                src/comps/mcuxClEcc/inc/internal/mcuxClEcc_Internal_SecurePointSelect.h
                src/comps/mcuxClEcc/inc/internal/mcuxClEcc_Internal_UPTRT_access.h
                src/comps/mcuxClEcc/inc/internal/mcuxClEcc_Mont_Internal.h
                src/comps/mcuxClEcc/inc/internal/mcuxClEcc_Mont_Internal_FUP.h
                src/comps/mcuxClEcc/inc/internal/mcuxClEcc_Mont_Internal_PkcWaLayout.h
                src/comps/mcuxClEcc/inc/internal/mcuxClEcc_TwEd_Internal.h
                src/comps/mcuxClEcc/inc/internal/mcuxClEcc_TwEd_Internal_FUP.h
                src/comps/mcuxClEcc/inc/internal/mcuxClEcc_TwEd_Internal_PkcWaLayout.h
                src/comps/mcuxClEcc/inc/internal/mcuxClEcc_WeierEcc_KeyGenerate_Internal.h
                src/comps/mcuxClEcc/inc/internal/mcuxClEcc_Weier_Internal.h
                src/comps/mcuxClEcc/inc/internal/mcuxClEcc_Weier_Internal_FP.h
                src/comps/mcuxClEcc/inc/internal/mcuxClEcc_Weier_Internal_FUP.h
                src/comps/mcuxClEcc/inc/internal/mcuxClEcc_Weier_Internal_PkcWaLayout.h
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
    mcux_add_include(
        INCLUDES ./
                 src/comps/mcuxClEcc/inc/
                 src/comps/mcuxClEcc/inc/internal
                 BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.els_header_only)
    mcux_add_source(
        SOURCES src/comps/mcuxClEls/inc/mcuxClEls.h
                src/comps/mcuxClEls/inc/mcuxClEls_Aead.h
                src/comps/mcuxClEls/inc/mcuxClEls_Cipher.h
                src/comps/mcuxClEls/inc/mcuxClEls_Cmac.h
                src/comps/mcuxClEls/inc/mcuxClEls_Crc.h
                src/comps/mcuxClEls/inc/mcuxClEls_Ecc.h
                src/comps/mcuxClEls/inc/mcuxClEls_Hash.h
                src/comps/mcuxClEls/inc/mcuxClEls_Hmac.h
                src/comps/mcuxClEls/inc/mcuxClEls_Kdf.h
                src/comps/mcuxClEls/inc/mcuxClEls_mapping.h
                src/comps/mcuxClEls/inc/mcuxClEls_Rng.h
                src/comps/mcuxClEls/inc/mcuxClEls_Types.h
                src/comps/mcuxClEls/inc/mcuxClEls_KeyManagement.h
                src/comps/mcuxClEls/inc/internal/mcuxClEls_Internal.h
                src/comps/mcuxClEls/inc/internal/mcuxClEls_Internal_mapping.h
                src/comps/mcuxClEls/inc/internal/mcuxClEls_SfrAccess.h
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
    mcux_add_include(
        INCLUDES ./
                 src/comps/mcuxClEls/inc
                 src/comps/mcuxClEls/inc/internal
                 BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.els_common)
    mcux_add_source(
        SOURCES src/comps/mcuxClEls/src/mcuxClEls_Common.c
                src/comps/mcuxClEls/inc/mcuxClEls_Common.h
                src/comps/mcuxClEls/inc/internal/mcuxClEls_Internal_Common.h
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
    mcux_add_include(
        INCLUDES ./
                 src/comps/mcuxClEls/inc
                 src/comps/mcuxClEls/inc/internal
                 BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.standalone_gdet)
    mcux_add_source(
        SOURCES src/comps/mcuxClEls/src/mcuxClEls_GlitchDetector.c
                src/comps/mcuxClEls/inc/mcuxClEls_GlitchDetector.h
                includes/mcuxClEls_GlitchDetector.h
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
    mcux_add_include(
        INCLUDES ./
                 src/comps/mcuxClEls/inc/
                 BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.els)
    mcux_add_source(
        SOURCES src/comps/mcuxClEls/src/mcuxClEls_Aead.c
                src/comps/mcuxClEls/src/mcuxClEls_Cipher.c
                src/comps/mcuxClEls/src/mcuxClEls_Cmac.c
                src/comps/mcuxClEls/src/mcuxClEls_Ecc.c
                src/comps/mcuxClEls/src/mcuxClEls_Hash.c
                src/comps/mcuxClEls/src/mcuxClEls_Hmac.c
                src/comps/mcuxClEls/src/mcuxClEls_Kdf.c
                src/comps/mcuxClEls/src/mcuxClEls_Rng.c
                src/comps/mcuxClEls/src/mcuxClEls_KeyManagement.c
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
    mcux_add_include(
        INCLUDES ./
    )
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.glikey)
    mcux_add_source(
        SOURCES src/comps/mcuxClGlikey/src/mcuxClGlikey.c
                src/comps/mcuxClGlikey/inc/mcuxClGlikey.h
                src/comps/mcuxClGlikey/inc/internal/mcuxClGlikey_SfrAccess.h
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
    mcux_add_include(
        INCLUDES ./
                 src/comps/mcuxClGlikey/inc
                 src/comps/mcuxClGlikey/inc/internal
                 BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.hash)
    mcux_add_source(
        SOURCES src/comps/mcuxClHash/src/mcuxClHash_api_multipart_common.c
                src/comps/mcuxClHash/src/mcuxClHash_api_multipart_compute.c
                src/comps/mcuxClHash/src/mcuxClHash_api_oneshot_compute.c
                src/comps/mcuxClHash/src/mcuxClHash_Internal.c
                src/comps/mcuxClHash/inc/mcuxClHash.h
                src/comps/mcuxClHash/inc/mcuxClHash_Constants.h
                src/comps/mcuxClHash/inc/mcuxClHash_Functions.h
#                 src/comps/mcuxClHash/inc/mcuxClHash_MemoryConsumption.h
                src/comps/mcuxClHash/inc/mcuxClHash_Types.h
                src/comps/mcuxClHash/inc/internal/mcuxClHash_Internal.h
                src/comps/mcuxClHash/inc/internal/mcuxClHash_Internal_Memory.h
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
    mcux_add_include(
        INCLUDES ./
                 src/comps/mcuxClHash/inc
                 src/comps/mcuxClHash/inc/internal
                 BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.hashmodes)
    mcux_add_source(
        SOURCES src/comps/mcuxClHashModes/src/mcuxClHashModes_Core_c_sha1.c
                src/comps/mcuxClHashModes/src/mcuxClHashModes_Core_els_sha2.c
                src/comps/mcuxClHashModes/src/mcuxClHashModes_Internal_c_sha1.c
                src/comps/mcuxClHashModes/src/mcuxClHashModes_Internal_els_sha2.c
                src/comps/mcuxClHashModes/inc/mcuxClHashModes.h
                src/comps/mcuxClHashModes/inc/mcuxClHashModes_Algorithms.h
                src/comps/mcuxClHashModes/inc/mcuxClHashModes_Constants.h
                src/comps/mcuxClHashModes/inc/mcuxClHashModes_Functions.h
#                 src/comps/mcuxClHashModes/inc/mcuxClHashModes_MemoryConsumption.h
                src/comps/mcuxClHashModes/inc/internal/mcuxClHashModes_Core_c_sha1.h
                src/comps/mcuxClHashModes/inc/internal/mcuxClHashModes_Core_els_sha2.h
                src/comps/mcuxClHashModes/inc/internal/mcuxClHashModes_Internal.h
                src/comps/mcuxClHashModes/inc/internal/mcuxClHashModes_Internal_els_sha2.h
                src/comps/mcuxClHashModes/inc/internal/mcuxClHashModes_Internal_Memory.h
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
    mcux_add_include(
        INCLUDES ./
                 src/comps/mcuxClHashModes/inc
                 src/comps/mcuxClHashModes/inc/internal
                 BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.hmac)
    mcux_add_source(
        SOURCES src/comps/mcuxClHmac/src/mcuxClHmac_Els.c
                src/comps/mcuxClHmac/src/mcuxClHmac_Functions.c
                src/comps/mcuxClHmac/src/mcuxClHmac_Helper.c
                src/comps/mcuxClHmac/src/mcuxClHmac_KeyTypes.c
                src/comps/mcuxClHmac/src/mcuxClHmac_Modes.c
                src/comps/mcuxClHmac/src/mcuxClHmac_Sw.c
                src/comps/mcuxClHmac/inc/mcuxClHmac.h
                src/comps/mcuxClHmac/inc/mcuxClHmac_Constants.h
                src/comps/mcuxClHmac/inc/mcuxClHmac_Functions.h
                src/comps/mcuxClHmac/inc/mcuxClHmac_KeyTypes.h
#                 src/comps/mcuxClHmac/inc/mcuxClHmac_MemoryConsumption.h
                src/comps/mcuxClHmac/inc/mcuxClHmac_Modes.h
                src/comps/mcuxClHmac/inc/internal/mcuxClHmac_Core_Functions_Els.h
                src/comps/mcuxClHmac/inc/internal/mcuxClHmac_Core_Functions_Sw.h
                src/comps/mcuxClHmac/inc/internal/mcuxClHmac_Internal_Functions.h
                src/comps/mcuxClHmac/inc/internal/mcuxClHmac_Internal_Memory.h
                src/comps/mcuxClHmac/inc/internal/mcuxClHmac_Internal_Types.h
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
    mcux_add_include(
        INCLUDES ./
                 src/comps/mcuxClHmac/inc/
                 src/comps/mcuxClHmac/inc/internal
                 BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.key_derivation)
    mcux_add_source(
        SOURCES src/comps/mcuxClKey/src/mcuxClKey_agreement.c
                src/comps/mcuxClKey/src/mcuxClKey_agreement_selftest.c
                src/comps/mcuxClKey/src/mcuxClKey_Derivation.c
                src/comps/mcuxClKey/src/mcuxClKey_Derivation_HKDF.c
                src/comps/mcuxClKey/src/mcuxClKey_Derivation_NIST_SP800_108.c
                src/comps/mcuxClKey/src/mcuxClKey_Derivation_NIST_SP800_56C.c
                src/comps/mcuxClKey/src/mcuxClKey_Derivation_PBKDF2.c
                src/comps/mcuxClKey/src/mcuxClKey_generate_keypair.c
                src/comps/mcuxClKey/inc/mcuxClKey_DerivationAlgorithms.h
                src/comps/mcuxClKey/inc/mcuxClKey_DerivationAlgorithms_HKDF.h
                src/comps/mcuxClKey/inc/mcuxClKey_DerivationAlgorithms_NIST_SP800_108.h
                src/comps/mcuxClKey/inc/mcuxClKey_DerivationAlgorithms_NIST_SP800_56C.h
                src/comps/mcuxClKey/inc/mcuxClKey_DerivationAlgorithms_PBKDF2.h
                src/comps/mcuxClKey/inc/internal/mcuxClKey_Derivation_Helper.h
                src/comps/mcuxClKey/inc/internal/mcuxClKey_DerivationAlgorithms_NIST_SP800_108_Internal.h
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
    mcux_add_include(
        INCLUDES ./
                 src/comps/mcuxClKey/inc
                 src/comps/mcuxClKey/inc/internal
                 BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.key)
    mcux_add_source(
        SOURCES src/comps/mcuxClKey/src/mcuxClKey.c
                src/comps/mcuxClKey/src/mcuxClKey_Protection.c
                src/comps/mcuxClKey/inc/mcuxClKey.h
                src/comps/mcuxClKey/inc/mcuxClKey_Constants.h
                src/comps/mcuxClKey/inc/mcuxClKey_Functions.h
#                 src/comps/mcuxClKey/inc/mcuxClKey_MemoryConsumption.h
                src/comps/mcuxClKey/inc/mcuxClKey_ProtectionMechanisms.h
                src/comps/mcuxClKey/inc/mcuxClKey_Types.h
                src/comps/mcuxClKey/inc/internal/mcuxClKey_Functions_Internal.h
                src/comps/mcuxClKey/inc/internal/mcuxClKey_Internal.h
                src/comps/mcuxClKey/inc/internal/mcuxClKey_Protection_Internal.h
                src/comps/mcuxClKey/inc/internal/mcuxClKey_Types_Internal.h
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
    mcux_add_include(
        INCLUDES ./
                 src/comps/mcuxClKey/inc
                 src/comps/mcuxClKey/inc/internal
                 BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.mac)
    mcux_add_source(
        SOURCES src/comps/mcuxClMac/src/mcuxClMac.c
                src/comps/mcuxClMac/inc/mcuxClMac.h
                src/comps/mcuxClMac/inc/mcuxClMac_Constants.h
                src/comps/mcuxClMac/inc/mcuxClMac_Functions.h
                src/comps/mcuxClMac/inc/mcuxClMac_Types.h
                src/comps/mcuxClMac/inc/internal/mcuxClMac_Ctx.h
                src/comps/mcuxClMac/inc/internal/mcuxClMac_Internal_Constants.h
                src/comps/mcuxClMac/inc/internal/mcuxClMac_Internal_Functions.h
                src/comps/mcuxClMac/inc/internal/mcuxClMac_Internal_Types.h
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
    mcux_add_include(
        INCLUDES ./
                 src/comps/mcuxClMac/inc/
                 src/comps/mcuxClMac/inc/internal
                 BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.mac_modes)
    mcux_add_source(
        SOURCES src/comps/mcuxClMacModes/src/mcuxClMacModes_Common.c
                src/comps/mcuxClMacModes/src/mcuxClMacModes_Common_Modes.c
                src/comps/mcuxClMacModes/src/mcuxClMacModes_Els_Cbcmac.c
                src/comps/mcuxClMacModes/src/mcuxClMacModes_Els_Cmac.c
                src/comps/mcuxClMacModes/src/mcuxClMacModes_Els_Functions.c
                src/comps/mcuxClMacModes/inc/mcuxClMacModes.h
                src/comps/mcuxClMacModes/inc/mcuxClMacModes_Constants.h
                src/comps/mcuxClMacModes/inc/mcuxClMacModes_Functions.h
#                 src/comps/mcuxClMacModes/inc/mcuxClMacModes_MemoryConsumption.h
                src/comps/mcuxClMacModes/inc/mcuxClMacModes_Modes.h
                src/comps/mcuxClMacModes/inc/internal/mcuxClMacModes_Common_Algorithms.h
                src/comps/mcuxClMacModes/inc/internal/mcuxClMacModes_Common_Constants.h
                src/comps/mcuxClMacModes/inc/internal/mcuxClMacModes_Common_Functions.h
                src/comps/mcuxClMacModes/inc/internal/mcuxClMacModes_Common_Memory.h
                src/comps/mcuxClMacModes/inc/internal/mcuxClMacModes_Common_Types.h
                src/comps/mcuxClMacModes/inc/internal/mcuxClMacModes_Common_Wa.h
                src/comps/mcuxClMacModes/inc/internal/mcuxClMacModes_Els_Cbcmac.h
                src/comps/mcuxClMacModes/inc/internal/mcuxClMacModes_Els_Cmac.h
                src/comps/mcuxClMacModes/inc/internal/mcuxClMacModes_Els_Ctx.h
                src/comps/mcuxClMacModes/inc/internal/mcuxClMacModes_Els_Types.h
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
    mcux_add_include(
        INCLUDES ./
                 src/comps/mcuxClMacModes/inc/
                 src/comps/mcuxClMacModes/inc/internal
                 BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.math)
    mcux_add_source(
        SOURCES src/comps/mcuxClMath/src/mcuxClMath_ExactDivide.c
                src/comps/mcuxClMath/src/mcuxClMath_ExactDivideOdd.c
                src/comps/mcuxClMath/src/mcuxClMath_ExactDivideOdd_FUP.c
                src/comps/mcuxClMath/src/mcuxClMath_ModExp_SqrMultL2R.c
                src/comps/mcuxClMath/src/mcuxClMath_ModInv.c
                src/comps/mcuxClMath/src/mcuxClMath_ModInv_FUP.c
                src/comps/mcuxClMath/src/mcuxClMath_NDash.c
                src/comps/mcuxClMath/src/mcuxClMath_NDash_FUP.c
                src/comps/mcuxClMath/src/mcuxClMath_QDash.c
                src/comps/mcuxClMath/src/mcuxClMath_QDash_FUP.c
                src/comps/mcuxClMath/src/mcuxClMath_ReduceModEven.c
                src/comps/mcuxClMath/src/mcuxClMath_SecModExp.c
                src/comps/mcuxClMath/src/mcuxClMath_SecModExp_FUP.c
                src/comps/mcuxClMath/src/mcuxClMath_Utils.c
                src/comps/mcuxClMath/inc/mcuxClMath.h
                src/comps/mcuxClMath/inc/mcuxClMath_ExactDivideOdd_FUP.h
                src/comps/mcuxClMath/inc/mcuxClMath_Functions.h
                src/comps/mcuxClMath/inc/mcuxClMath_ModInv_FUP.h
                src/comps/mcuxClMath/inc/mcuxClMath_NDash_FUP.h
                src/comps/mcuxClMath/inc/mcuxClMath_QDash_FUP.h
                src/comps/mcuxClMath/inc/mcuxClMath_SecModExp_FUP.h
                src/comps/mcuxClMath/inc/mcuxClMath_Types.h
                src/comps/mcuxClMath/inc/internal/mcuxClMath_ExactDivideOdd_FUP.h
                src/comps/mcuxClMath/inc/internal/mcuxClMath_Internal_ExactDivideOdd.h
                src/comps/mcuxClMath/inc/internal/mcuxClMath_Internal_ModInv.h
                src/comps/mcuxClMath/inc/internal/mcuxClMath_Internal_NDash.h
                src/comps/mcuxClMath/inc/internal/mcuxClMath_Internal_QDash.h
                src/comps/mcuxClMath/inc/internal/mcuxClMath_Internal_SecModExp.h
                src/comps/mcuxClMath/inc/internal/mcuxClMath_Internal_Utils.h
                src/comps/mcuxClMath/inc/internal/mcuxClMath_ModInv_FUP.h
                src/comps/mcuxClMath/inc/internal/mcuxClMath_NDash_FUP.h
                src/comps/mcuxClMath/inc/internal/mcuxClMath_QDash_FUP.h
                src/comps/mcuxClMath/inc/internal/mcuxClMath_SecModExp_FUP.h
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
    mcux_add_include(
        INCLUDES ./
                 src/comps/mcuxClMath/inc/
                 src/comps/mcuxClMath/inc/internal
                 BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.memory)
    mcux_add_source(
        SOURCES src/comps/mcuxClMemory/src/mcuxClMemory.c
                src/comps/mcuxClMemory/inc/mcuxClMemory.h
                src/comps/mcuxClMemory/inc/mcuxClMemory_Clear.h
                src/comps/mcuxClMemory/inc/mcuxClMemory_Constants.h
                src/comps/mcuxClMemory/inc/mcuxClMemory_Copy.h
                src/comps/mcuxClMemory/inc/mcuxClMemory_Copy_Reversed.h
                src/comps/mcuxClMemory/inc/mcuxClMemory_Endianness.h
                src/comps/mcuxClMemory/inc/mcuxClMemory_Set.h
                src/comps/mcuxClMemory/inc/mcuxClMemory_Types.h
                src/comps/mcuxClMemory/inc/mcuxClMemory_Xor.h
#                 src/comps/mcuxClMemory/inc/internal/mcuxClMemory_ClearSecure_Internal.h
#                 src/comps/mcuxClMemory/inc/internal/mcuxClMemory_Clear_Internal.h
#                 src/comps/mcuxClMemory/inc/internal/mcuxClMemory_CompareDPASecure_Internal.h
#                 src/comps/mcuxClMemory/inc/internal/mcuxClMemory_CompareSecure_Internal.h
#                 src/comps/mcuxClMemory/inc/internal/mcuxClMemory_Compare_Internal.h
#                 src/comps/mcuxClMemory/inc/internal/mcuxClMemory_CopySecurePow2_Internal.h
#                 src/comps/mcuxClMemory/inc/internal/mcuxClMemory_CopySecure_Internal.h
#                 src/comps/mcuxClMemory/inc/internal/mcuxClMemory_CopySecure_Reversed_Internal.h
#                 src/comps/mcuxClMemory/inc/internal/mcuxClMemory_CopyWords_Internal.h
#                 src/comps/mcuxClMemory/inc/internal/mcuxClMemory_Copy_Internal.h
#                 src/comps/mcuxClMemory/inc/internal/mcuxClMemory_Copy_Reversed_Internal.h
                src/comps/mcuxClMemory/inc/internal/mcuxClMemory_Internal.h
#                 src/comps/mcuxClMemory/inc/internal/mcuxClMemory_SetSecure_Internal.h
#                 src/comps/mcuxClMemory/inc/internal/mcuxClMemory_Set_Internal.h
                src/comps/mcuxCsslMemory/src/mcuxCsslMemory_Clear.c
                src/comps/mcuxCsslMemory/src/mcuxCsslMemory_Compare.c
                src/comps/mcuxCsslMemory/src/mcuxCsslMemory_Copy.c
#                 src/comps/mcuxCsslMemory/src/mcuxCsslMemory_Internal_SecureCompare_Stub.c
                src/comps/mcuxCsslMemory/src/mcuxCsslMemory_Set.c
                src/comps/mcuxCsslMemory/inc/mcuxCsslMemory.h
                src/comps/mcuxCsslMemory/inc/mcuxCsslMemory_Clear.h
                src/comps/mcuxCsslMemory/inc/mcuxCsslMemory_Compare.h
                src/comps/mcuxCsslMemory/inc/mcuxCsslMemory_Constants.h
                src/comps/mcuxCsslMemory/inc/mcuxCsslMemory_Copy.h
                src/comps/mcuxCsslMemory/inc/mcuxCsslMemory_Set.h
                src/comps/mcuxCsslMemory/inc/mcuxCsslMemory_Types.h
                src/comps/mcuxCsslMemory/inc/internal/mcuxCsslMemory_Internal_Compare_asm.h
                src/comps/mcuxCsslMemory/inc/internal/mcuxCsslMemory_Internal_Copy_asm.h
#                 src/comps/mcuxCsslMemory/inc/internal/mcuxCsslMemory_Internal_SecureCompare.h
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
    mcux_add_include(
        INCLUDES ./
                 src/comps/mcuxClMemory/inc
                 src/comps/mcuxClMemory/inc/internal
                 src/comps/mcuxCsslMemory/inc
                 src/comps/mcuxCsslMemory/inc/internal
                 BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.oscca)
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.oscca_aeadmodes)
    mcux_add_source(
        SOURCES src/comps/mcuxClOsccaAeadModes/src/mcuxClOsccaAeadModes_Ccm_Common.c
                src/comps/mcuxClOsccaAeadModes/src/mcuxClOsccaAeadModes_Constants.c
                src/comps/mcuxClOsccaAeadModes/src/mcuxClOsccaAeadModes_EngineCcm.c
                src/comps/mcuxClOsccaAeadModes/src/mcuxClOsccaAeadModes_SkeletonCcm.c
                src/comps/mcuxClOsccaAeadModes/src/mcuxClOsccaAeadModes_SM4Ctr.c
                src/comps/mcuxClOsccaAeadModes/src/mcuxClOsccaAeadModes_SM4_Multipart.c
                src/comps/mcuxClOsccaAeadModes/src/mcuxClOsccaAeadModes_SM4_OneShot.c
                src/comps/mcuxClOsccaAeadModes/inc/mcuxClOsccaAeadModes.h
#                 src/comps/mcuxClOsccaAeadModes/inc/mcuxClOsccaAeadModes_MemoryConsumption.h
                src/comps/mcuxClOsccaAeadModes/inc/mcuxClOsccaAeadModes_Modes.h
                src/comps/mcuxClOsccaAeadModes/inc/internal/mcuxClOsccaAeadModes_Internal_Algorithms.h
                src/comps/mcuxClOsccaAeadModes/inc/internal/mcuxClOsccaAeadModes_Internal_Functions.h
                src/comps/mcuxClOsccaAeadModes/inc/internal/mcuxClOsccaAeadModes_Internal_Types.h
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
    mcux_add_include(
        INCLUDES ./
                 src/comps/mcuxClOsccaAeadModes/inc
                 src/comps/mcuxClOsccaAeadModes/inc/internal
                 BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.oscca_ciphermodes)
    mcux_add_source(
        SOURCES src/comps/mcuxClOsccaCipherModes/src/mcuxClOsccaCipherModes_Constants.c
                src/comps/mcuxClOsccaCipherModes/src/mcuxClOsccaCipherModes_SM4_Crypt.c
                src/comps/mcuxClOsccaCipherModes/src/mcuxClOsccaCipherModes_SM4_Crypt_Internal.c
                src/comps/mcuxClOsccaCipherModes/inc/mcuxClOsccaCipherModes.h
                src/comps/mcuxClOsccaCipherModes/inc/mcuxClOsccaCipherModes_Constants.h
#                 src/comps/mcuxClOsccaCipherModes/inc/mcuxClOsccaCipherModes_MemoryConsumption.h
                src/comps/mcuxClOsccaCipherModes/inc/internal/mcuxClOsccaCipherModes_Algorithms.h
                src/comps/mcuxClOsccaCipherModes/inc/internal/mcuxClOsccaCipherModes_Internal_Functions.h
                src/comps/mcuxClOsccaCipherModes/inc/internal/mcuxClOsccaCipherModes_Internal_Types.h
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
    mcux_add_include(
        INCLUDES ./
                 src/comps/mcuxClOsccaCipherModes/inc
                 src/comps/mcuxClOsccaCipherModes/inc/internal
                 BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.oscca_macmodes)
    mcux_add_source(
        SOURCES src/comps/mcuxClOsccaMacModes/src/mcuxClOsccaMacModes.c
                src/comps/mcuxClOsccaMacModes/src/mcuxClOsccaMacModes_CBCMac.c
                src/comps/mcuxClOsccaMacModes/src/mcuxClOsccaMacModes_CMac.c
                src/comps/mcuxClOsccaMacModes/src/mcuxClOsccaMacModes_Helper.c
                src/comps/mcuxClOsccaMacModes/src/mcuxClOsccaMacModes_KeyTypes.c
                src/comps/mcuxClOsccaMacModes/src/mcuxClOsccaMacModes_Modes.c
                src/comps/mcuxClOsccaMacModes/inc/mcuxClOsccaMacModes.h
                src/comps/mcuxClOsccaMacModes/inc/mcuxClOsccaMacModes_Constants.h
                src/comps/mcuxClOsccaMacModes/inc/mcuxClOsccaMacModes_KeyTypes.h
#                 src/comps/mcuxClOsccaMacModes/inc/mcuxClOsccaMacModes_MemoryConsumption.h
                src/comps/mcuxClOsccaMacModes/inc/mcuxClOsccaMacModes_Modes.h
                src/comps/mcuxClOsccaMacModes/inc/internal/mcuxClOsccaMacModes_Algorithms.h
                src/comps/mcuxClOsccaMacModes/inc/internal/mcuxClOsccaMacModes_Ctx.h
                src/comps/mcuxClOsccaMacModes/inc/internal/mcuxClOsccaMacModes_Internal_Functions.h
                src/comps/mcuxClOsccaMacModes/inc/internal/mcuxClOsccaMacModes_SM4_CBCMAC.h
                src/comps/mcuxClOsccaMacModes/inc/internal/mcuxClOsccaMacModes_SM4_CMAC.h
                src/comps/mcuxClOsccaMacModes/inc/internal/mcuxClOsccaMacModes_Types.h
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
    mcux_add_include(
        INCLUDES ./
                 src/comps/mcuxClOsccaMacModes/inc
                 src/comps/mcuxClOsccaMacModes/inc/internal
                 BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.oscca_pkc)
    mcux_add_source(
        SOURCES src/comps/mcuxClOsccaPkc/src/mcuxClOsccaPkc.c
                src/comps/mcuxClOsccaPkc/inc/mcuxClOsccaPkc.h
                src/comps/mcuxClOsccaPkc/inc/mcuxClOsccaPkc_Functions.h
                src/comps/mcuxClOsccaPkc/inc/mcuxClOsccaPkc_Types.h
                src/comps/mcuxClOsccaPkc/inc/internal/mcuxClOsccaPkc_FupMacros.h
                src/comps/mcuxClOsccaPkc/inc/internal/mcuxClOsccaPkc_Macros.h
                src/comps/mcuxClOsccaPkc/inc/internal/mcuxClOsccaPkc_Operations.h
                src/comps/mcuxClOsccaPkc/inc/internal/mcuxClOsccaPkc_SfrAccess.h
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
    mcux_add_include(
        INCLUDES ./
                 src/comps/mcuxClOsccaPkc/inc
                 src/comps/mcuxClOsccaPkc/inc/internal
                 BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.oscca_randommodes)
    mcux_add_source(
        SOURCES src/comps/mcuxClOsccaRandomModes/src/mcuxClOsccaRandomModes_OsccaMode.c
                src/comps/mcuxClOsccaRandomModes/inc/mcuxClOsccaRandomModes.h
                src/comps/mcuxClOsccaRandomModes/inc/mcuxClOsccaRandomModes_Constants.h
#                 src/comps/mcuxClOsccaRandomModes/inc/mcuxClOsccaRandomModes_MemoryConsumption.h
                src/comps/mcuxClOsccaRandomModes/inc/internal/mcuxClOsccaRandomModes_Private_RNG.h
                src/comps/mcuxClOsccaRandomModes/inc/internal/mcuxClOsccaRandomModes_Private_Types.h
                src/comps/mcuxClOsccaRandomModes/inc/internal/mcuxClOsccaRandomModes_SfrAccess.h
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
    mcux_add_include(
        INCLUDES ./
                 src/comps/mcuxClOsccaRandomModes/inc
                 src/comps/mcuxClOsccaRandomModes/inc/internal
                 BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.oscca_safo)
    mcux_add_source(
        SOURCES src/comps/mcuxClOsccaSafo/src/mcuxClOsccaSafo_Drv.c
                src/comps/mcuxClOsccaSafo/inc/mcuxClOsccaSafo.h
                src/comps/mcuxClOsccaSafo/inc/mcuxClOsccaSafo_Constants.h
                src/comps/mcuxClOsccaSafo/inc/mcuxClOsccaSafo_Functions.h
                src/comps/mcuxClOsccaSafo/inc/mcuxClOsccaSafo_SfrAccess.h
                src/comps/mcuxClOsccaSafo/inc/mcuxClOsccaSafo_Sfr_Ctrl.h
                src/comps/mcuxClOsccaSafo/inc/mcuxClOsccaSafo_Sfr_RegBank.h
                src/comps/mcuxClOsccaSafo/inc/mcuxClOsccaSafo_Sfr_Status.h
                src/comps/mcuxClOsccaSafo/inc/mcuxClOsccaSafo_Types.h
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
    mcux_add_include(
        INCLUDES ./
                 src/comps/mcuxClOsccaSafo/inc
                 BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.oscca_sm2)
    mcux_add_source(
        SOURCES src/comps/mcuxClOsccaSm2/src/mcuxClOsccaSm2_Cipher_Crypt.c
                src/comps/mcuxClOsccaSm2/src/mcuxClOsccaSm2_ComputePrehash.c
                src/comps/mcuxClOsccaSm2/src/mcuxClOsccaSm2_Constants.c
                src/comps/mcuxClOsccaSm2/src/mcuxClOsccaSm2_CryptoUtils.c
                src/comps/mcuxClOsccaSm2/src/mcuxClOsccaSm2_Decrypt.c
                src/comps/mcuxClOsccaSm2/src/mcuxClOsccaSm2_Ecc.c
                src/comps/mcuxClOsccaSm2/src/mcuxClOsccaSm2_EccUtils.c
                src/comps/mcuxClOsccaSm2/src/mcuxClOsccaSm2_EncDec_Internal.c
                src/comps/mcuxClOsccaSm2/src/mcuxClOsccaSm2_Encrypt.c
                src/comps/mcuxClOsccaSm2/src/mcuxClOsccaSm2_FUP.c
                src/comps/mcuxClOsccaSm2/src/mcuxClOsccaSm2_GenerateKeyPair.c
                src/comps/mcuxClOsccaSm2/src/mcuxClOsccaSm2_Helper.c
                src/comps/mcuxClOsccaSm2/src/mcuxClOsccaSm2_InvertPrivateKey.c
                src/comps/mcuxClOsccaSm2/src/mcuxClOsccaSm2_Keyagreement.c
                src/comps/mcuxClOsccaSm2/src/mcuxClOsccaSm2_Keyagreement_SelfTest.c
                src/comps/mcuxClOsccaSm2/src/mcuxClOsccaSm2_KeyExchange.c
                src/comps/mcuxClOsccaSm2/src/mcuxClOsccaSm2_KeyTypes.c
                src/comps/mcuxClOsccaSm2/src/mcuxClOsccaSm2_SelfTest.c
                src/comps/mcuxClOsccaSm2/src/mcuxClOsccaSm2_Sign.c
                src/comps/mcuxClOsccaSm2/src/mcuxClOsccaSm2_Signature_Internal.c
                src/comps/mcuxClOsccaSm2/src/mcuxClOsccaSm2_Signature_PreHash.c
                src/comps/mcuxClOsccaSm2/src/mcuxClOsccaSm2_Signature_SelfTest.c
                src/comps/mcuxClOsccaSm2/src/mcuxClOsccaSm2_Verify.c
                src/comps/mcuxClOsccaSm2/inc/mcuxClOsccaSm2.h
                src/comps/mcuxClOsccaSm2/inc/mcuxClOsccaSm2_Constants.h
                src/comps/mcuxClOsccaSm2/inc/mcuxClOsccaSm2_Functions.h
                src/comps/mcuxClOsccaSm2/inc/mcuxClOsccaSm2_FUP.h
                src/comps/mcuxClOsccaSm2/inc/mcuxClOsccaSm2_KeyTypes.h
#                 src/comps/mcuxClOsccaSm2/inc/mcuxClOsccaSm2_MemoryConsumption.h
                src/comps/mcuxClOsccaSm2/inc/mcuxClOsccaSm2_ModeConstants.h
                src/comps/mcuxClOsccaSm2/inc/mcuxClOsccaSm2_SelfTest.h
                src/comps/mcuxClOsccaSm2/inc/mcuxClOsccaSm2_Types.h
                src/comps/mcuxClOsccaSm2/inc/internal/mcuxClOsccaSm2_Internal.h
                src/comps/mcuxClOsccaSm2/inc/internal/mcuxClOsccaSm2_Internal_ConstructTypes.h
                src/comps/mcuxClOsccaSm2/inc/internal/mcuxClOsccaSm2_Internal_CryptoUtils.h
                src/comps/mcuxClOsccaSm2/inc/internal/mcuxClOsccaSm2_Internal_Ecc.h
                src/comps/mcuxClOsccaSm2/inc/internal/mcuxClOsccaSm2_Internal_FP.h
                src/comps/mcuxClOsccaSm2/inc/internal/mcuxClOsccaSm2_Internal_Functions.h
                src/comps/mcuxClOsccaSm2/inc/internal/mcuxClOsccaSm2_Internal_FUP.h
                src/comps/mcuxClOsccaSm2/inc/internal/mcuxClOsccaSm2_Internal_Hash.h
                src/comps/mcuxClOsccaSm2/inc/internal/mcuxClOsccaSm2_Internal_Helper.h
                src/comps/mcuxClOsccaSm2/inc/internal/mcuxClOsccaSm2_Internal_PkcWaLayout.h
                src/comps/mcuxClOsccaSm2/inc/internal/mcuxClOsccaSm2_Internal_Types.h
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
    mcux_add_include(
        INCLUDES ./
                 src/comps/mcuxClOsccaSm2/inc
                 src/comps/mcuxClOsccaSm2/inc/internal
                 BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.oscca_sm3)
    mcux_add_source(
        SOURCES src/comps/mcuxClOsccaSm3/src/mcuxClOsccaSm3_core_sm3.c
                src/comps/mcuxClOsccaSm3/src/mcuxClOsccaSm3_internal_sm3.c
                src/comps/mcuxClOsccaSm3/inc/mcuxClOsccaSm3.h
                src/comps/mcuxClOsccaSm3/inc/mcuxClOsccaSm3_Algorithms.h
                src/comps/mcuxClOsccaSm3/inc/mcuxClOsccaSm3_Constants.h
#                 src/comps/mcuxClOsccaSm3/inc/mcuxClOsccaSm3_MemoryConsumption.h
                src/comps/mcuxClOsccaSm3/inc/internal/mcuxClOsccaSm3_Core_sm3.h
                src/comps/mcuxClOsccaSm3/inc/internal/mcuxClOsccaSm3_Internal.h
                src/comps/mcuxClOsccaSm3/inc/internal/mcuxClOsccaSm3_Internal_sm3.h
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
    mcux_add_include(
        INCLUDES ./
                 src/comps/mcuxClOsccaSm3/inc
                 src/comps/mcuxClOsccaSm3/inc/internal
                 BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.oscca_sm4)
    mcux_add_source(
        SOURCES src/comps/mcuxClOsccaSm4/src/mcuxClOsccaSm4_CommonOperations.c
                src/comps/mcuxClOsccaSm4/src/mcuxClOsccaSm4_Core.c
                src/comps/mcuxClOsccaSm4/src/mcuxClOsccaSm4_KeyTypes.c
                src/comps/mcuxClOsccaSm4/src/mcuxClOsccaSm4_LoadKey.c
                src/comps/mcuxClOsccaSm4/inc/mcuxClOsccaSm4.h
                src/comps/mcuxClOsccaSm4/inc/mcuxClOsccaSm4_KeyTypes.h
                src/comps/mcuxClOsccaSm4/inc/internal/mcuxClOsccaSm4_Internal.h
                src/comps/mcuxClOsccaSm4/inc/internal/mcuxClOsccaSm4_Internal_Constants.h
                src/comps/mcuxClOsccaSm4/inc/internal/mcuxClOsccaSm4_Internal_Functions.h
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
    mcux_add_include(
        INCLUDES ./
                 src/comps/mcuxClOsccaSm4/inc
                 src/comps/mcuxClOsccaSm4/inc/internal
                 BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.padding)
    mcux_add_source(
        SOURCES src/comps/mcuxClPadding/src/mcuxClPadding.c
                src/comps/mcuxClPadding/inc/mcuxClPadding.h
                src/comps/mcuxClPadding/inc/mcuxClPadding_Constants.h
                src/comps/mcuxClPadding/inc/mcuxClPadding_Types.h
                src/comps/mcuxClPadding/inc/internal/mcuxClPadding_Functions_Internal.h
                src/comps/mcuxClPadding/inc/internal/mcuxClPadding_Internal.h
                src/comps/mcuxClPadding/inc/internal/mcuxClPadding_Types_Internal.h
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
    mcux_add_include(
        INCLUDES ./
                 src/comps/mcuxClPadding/inc/
                 src/comps/mcuxClPadding/inc/internal/
                 BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.pkc)
    mcux_add_source(
        SOURCES src/comps/mcuxClPkc/src/mcuxClPkc_Calculate.c
                src/comps/mcuxClPkc/src/mcuxClPkc_ImportExport.c
                src/comps/mcuxClPkc/src/mcuxClPkc_Initialize.c
                src/comps/mcuxClPkc/src/mcuxClPkc_UPTRT.c
                src/comps/mcuxClPkc/inc/mcuxClPkc.h
                src/comps/mcuxClPkc/inc/mcuxClPkc_Functions.h
                src/comps/mcuxClPkc/inc/mcuxClPkc_Types.h
                src/comps/mcuxClPkc/inc/internal/mcuxClPkc_FupMacros.h
                src/comps/mcuxClPkc/inc/internal/mcuxClPkc_ImportExport.h
                src/comps/mcuxClPkc/inc/internal/mcuxClPkc_Inline_Functions.h
                src/comps/mcuxClPkc/inc/internal/mcuxClPkc_Macros.h
                src/comps/mcuxClPkc/inc/internal/mcuxClPkc_Operations.h
                src/comps/mcuxClPkc/inc/internal/mcuxClPkc_Resource.h
                src/comps/mcuxClPkc/inc/internal/mcuxClPkc_SfrAccess.h
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
    mcux_add_include(
        INCLUDES ./
                 src/comps/mcuxClPkc/inc
                 src/comps/mcuxClPkc/inc/internal
                 BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.prng)
    mcux_add_source(
        SOURCES src/comps/mcuxClPrng/src/mcuxClPrng_ELS.c
                src/comps/mcuxClPrng/inc/internal/mcuxClPrng_Internal.h
                src/comps/mcuxClPrng/inc/internal/mcuxClPrng_Internal_Constants.h
                src/comps/mcuxClPrng/inc/internal/mcuxClPrng_Internal_ELS.h
                src/comps/mcuxClPrng/inc/internal/mcuxClPrng_Internal_Functions.h
                src/comps/mcuxClPrng/inc/internal/mcuxClPrng_Internal_Types.h
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
    mcux_add_include(
        INCLUDES ./
                 src/comps/mcuxClPrng/inc
                 src/comps/mcuxClPrng/inc/internal
                 BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.psa_driver)
    mcux_add_source(
        SOURCES src/comps/mcuxClPsaDriver/src/mcuxClPsaDriver_Aead.c
                src/comps/mcuxClPsaDriver/src/mcuxClPsaDriver_Cipher.c
                src/comps/mcuxClPsaDriver/src/mcuxClPsaDriver_DER_functions.c
                src/comps/mcuxClPsaDriver/src/mcuxClPsaDriver_export_public_key.c
                src/comps/mcuxClPsaDriver/src/mcuxClPsaDriver_generate_ecp_key.c
                src/comps/mcuxClPsaDriver/src/mcuxClPsaDriver_Hash.c
                src/comps/mcuxClPsaDriver/src/mcuxClPsaDriver_Key.c
                src/comps/mcuxClPsaDriver/src/mcuxClPsaDriver_Mac.c
                src/comps/mcuxClPsaDriver/src/mcuxClPsaDriver_Rsa.c
                src/comps/mcuxClPsaDriver/src/mcuxClPsaDriver_Sign.c
                src/comps/mcuxClPsaDriver/src/mcuxClPsaDriver_UpdateKeyStatus.c
                src/comps/mcuxClPsaDriver/src/mcuxClPsaDriver_Verify.c
                src/comps/mcuxClPsaDriver/inc/mcuxClPsaDriver.h
                src/comps/mcuxClPsaDriver/inc/mcuxClPsaDriver_Constants.h
                src/comps/mcuxClPsaDriver/inc/mcuxClPsaDriver_MemoryConsumption.h
                src/comps/mcuxClPsaDriver/inc/mcuxClPsaDriver_Oracle.h
                src/comps/mcuxClPsaDriver/inc/els_pkc_crypto_primitives.h
                src/comps/mcuxClPsaDriver/inc/els_pkc_crypto_composites.h
                src/comps/mcuxClPsaDriver/inc/internal/mcuxClPsaDriver_ExternalMacroWrappers.h
                src/comps/mcuxClPsaDriver/inc/internal/mcuxClPsaDriver_Functions.h
                src/comps/mcuxClPsaDriver/inc/internal/mcuxClPsaDriver_Internal.h
                src/comps/mcuxClPsaDriver/inc/internal/mcuxClPsaDriver_Types.h
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
    mcux_add_include(
        INCLUDES ./
                 src/comps/mcuxClPsaDriver/inc
                 src/comps/mcuxClPsaDriver/inc/internal
                 BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.random)
    mcux_add_source(
        SOURCES src/comps/mcuxClRandom/src/mcuxClRandom_DRBG.c
                src/comps/mcuxClRandom/src/mcuxClRandom_PRNG.c
                src/comps/mcuxClRandom/inc/mcuxClRandom.h
                src/comps/mcuxClRandom/inc/mcuxClRandom_Constants.h
                src/comps/mcuxClRandom/inc/mcuxClRandom_Functions.h
                src/comps/mcuxClRandom/inc/mcuxClRandom_Types.h
                src/comps/mcuxClRandom/inc/internal/mcuxClRandom_Internal_Functions.h
                src/comps/mcuxClRandom/inc/internal/mcuxClRandom_Internal_Memory.h
                src/comps/mcuxClRandom/inc/internal/mcuxClRandom_Internal_Types.h
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
    mcux_add_include(
        INCLUDES ./
                 src/comps/mcuxClRandom/inc
                 src/comps/mcuxClRandom/inc/internal
                 BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.random_modes_hmacdrbg)
    mcux_add_source(
        SOURCES src/comps/mcuxClRandomModes/src/mcuxClRandomModes_HmacDrbg.c
                src/comps/mcuxClRandomModes/inc/internal/mcuxClRandomModes_Internal_HmacDrbg_Functions.h
                src/comps/mcuxClRandomModes/inc/internal/mcuxClRandomModes_Private_HmacDrbg.h
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
    mcux_add_include(
        INCLUDES ./
                 src/comps/mcuxClRandomModes/inc
                 src/comps/mcuxClRandomModes/inc/internal
                 BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.random_modes)
    mcux_add_source(
        SOURCES src/comps/mcuxClRandomModes/src/mcuxClRandomModes_CtrDrbg_Els.c
                src/comps/mcuxClRandomModes/src/mcuxClRandomModes_ElsMode.c
                src/comps/mcuxClRandomModes/src/mcuxClRandomModes_PatchMode.c
                src/comps/mcuxClRandomModes/src/mcuxClRandomModes_TestMode.c
                src/comps/mcuxClRandomModes/inc/mcuxClRandomModes.h
                src/comps/mcuxClRandomModes/inc/mcuxClRandomModes_Constants.h
                src/comps/mcuxClRandomModes/inc/mcuxClRandomModes_Functions_PatchMode.h
                src/comps/mcuxClRandomModes/inc/mcuxClRandomModes_Functions_TestMode.h
#                 src/comps/mcuxClRandomModes/inc/mcuxClRandomModes_MemoryConsumption.h
                src/comps/mcuxClRandomModes/inc/internal/mcuxClRandomModes_Internal_SizeDefinitions.h
                src/comps/mcuxClRandomModes/inc/internal/mcuxClRandomModes_Private_CtrDrbg.h
                src/comps/mcuxClRandomModes/inc/internal/mcuxClRandomModes_Private_CtrDrbg_BlockCipher.h
                src/comps/mcuxClRandomModes/inc/internal/mcuxClRandomModes_Private_Drbg.h
                src/comps/mcuxClRandomModes/inc/internal/mcuxClRandomModes_Private_NormalMode.h
                src/comps/mcuxClRandomModes/inc/internal/mcuxClRandomModes_Private_PatchMode.h
                src/comps/mcuxClRandomModes/inc/internal/mcuxClRandomModes_Private_PrDisabled.h
                src/comps/mcuxClRandomModes/inc/internal/mcuxClRandomModes_Private_TestMode.h
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
    mcux_add_include(
        INCLUDES ./
                 src/comps/mcuxClRandomModes/inc
                 src/comps/mcuxClRandomModes/inc/internal
                 BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.random_modes_ctr)
    mcux_add_source(
        SOURCES src/comps/mcuxClRandomModes/src/mcuxClRandomModes_CtrDrbg.c
                src/comps/mcuxClRandomModes/src/mcuxClRandomModes_CtrDrbg_PrDisabled.c
                src/comps/mcuxClRandomModes/src/mcuxClRandomModes_NormalMode.c
                src/comps/mcuxClRandomModes/src/mcuxClRandomModes_PrDisabled.c
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
    mcux_add_include(
        INCLUDES ./
                 src/comps/mcuxClRandomModes/inc
                 src/comps/mcuxClRandomModes/inc/internal
                 BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.rsa_oaep)
    mcux_add_source(
        SOURCES src/comps/mcuxClRsa/src/mcuxClRsa_KeyTypes.c
                src/comps/mcuxClRsa/src/mcuxClRsa_ModeConstructors.c
                src/comps/mcuxClRsa/src/mcuxClRsa_OaepDecode.c
                src/comps/mcuxClRsa/src/mcuxClRsa_OaepEncode.c
                src/comps/mcuxClRsa/src/mcuxClRsa_pkcs1v15Decode_decrypt.c
                src/comps/mcuxClRsa/src/mcuxClRsa_Pkcs1v15Encode_encrypt.c
                src/comps/mcuxClRsa/src/mcuxClRsa_Util_Decrypt.c
                src/comps/mcuxClRsa/src/mcuxClRsa_Util_Encrypt.c
                src/comps/mcuxClRsa/inc/mcuxClRsa_KeyTypes.h
                src/comps/mcuxClRsa/inc/mcuxClRsa_ModeConstructors.h
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
    mcux_add_include(
        INCLUDES ./
                 src/comps/mcuxClRsa/inc
                 src/comps/mcuxClRsa/inc/internal
                 BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.rsa)
    mcux_add_source(
        SOURCES src/comps/mcuxClRsa/src/mcuxClRsa_ComputeD.c
                src/comps/mcuxClRsa/src/mcuxClRsa_ComputeD_FUP.c
                src/comps/mcuxClRsa/src/mcuxClRsa_GenerateProbablePrime.c
                src/comps/mcuxClRsa/src/mcuxClRsa_getMillerRabinTestIterations.c
                src/comps/mcuxClRsa/src/mcuxClRsa_KeyGeneration_Crt.c
                src/comps/mcuxClRsa/src/mcuxClRsa_KeyGeneration_Crt_FUP.c
                src/comps/mcuxClRsa/src/mcuxClRsa_KeyGeneration_Helper.c
                src/comps/mcuxClRsa/src/mcuxClRsa_KeyGeneration_Plain.c
                src/comps/mcuxClRsa/src/mcuxClRsa_Mgf1.c
                src/comps/mcuxClRsa/src/mcuxClRsa_MillerRabinTest.c
                src/comps/mcuxClRsa/src/mcuxClRsa_MillerRabinTest_FUP.c
                src/comps/mcuxClRsa/src/mcuxClRsa_ModInv.c
                src/comps/mcuxClRsa/src/mcuxClRsa_Pkcs1v15Encode_sign.c
                src/comps/mcuxClRsa/src/mcuxClRsa_Pkcs1v15Verify.c
                src/comps/mcuxClRsa/src/mcuxClRsa_PrivateCrt.c
                src/comps/mcuxClRsa/src/mcuxClRsa_PrivateCrt_FUP.c
                src/comps/mcuxClRsa/src/mcuxClRsa_PrivatePlain.c
                src/comps/mcuxClRsa/src/mcuxClRsa_PssEncode.c
                src/comps/mcuxClRsa/src/mcuxClRsa_PssVerify.c
                src/comps/mcuxClRsa/src/mcuxClRsa_Public.c
                src/comps/mcuxClRsa/src/mcuxClRsa_PublicExp.c
                src/comps/mcuxClRsa/src/mcuxClRsa_RemoveBlinding.c
                src/comps/mcuxClRsa/src/mcuxClRsa_RemoveBlinding_FUP.c
                src/comps/mcuxClRsa/src/mcuxClRsa_Sign.c
                src/comps/mcuxClRsa/src/mcuxClRsa_Sign_NoEMSA.c
                src/comps/mcuxClRsa/src/mcuxClRsa_TestPQDistance.c
                src/comps/mcuxClRsa/src/mcuxClRsa_TestPQDistance_FUP.c
                src/comps/mcuxClRsa/src/mcuxClRsa_TestPrimeCandidate.c
                src/comps/mcuxClRsa/src/mcuxClRsa_TestPrimeCandidate_FUP.c
                src/comps/mcuxClRsa/src/mcuxClRsa_Verify.c
                src/comps/mcuxClRsa/src/mcuxClRsa_VerifyE.c
                src/comps/mcuxClRsa/src/mcuxClRsa_Verify_NoEMSA.c
                src/comps/mcuxClRsa/inc/mcuxClRsa.h
                src/comps/mcuxClRsa/inc/mcuxClRsa_ComputeD_FUP.h
                src/comps/mcuxClRsa/inc/mcuxClRsa_Constants.h
                src/comps/mcuxClRsa/inc/mcuxClRsa_Functions.h
                src/comps/mcuxClRsa/inc/mcuxClRsa_KeyGeneration_Crt_FUP.h
#                 src/comps/mcuxClRsa/inc/mcuxClRsa_MemoryConsumption.h
                src/comps/mcuxClRsa/inc/mcuxClRsa_MillerRabinTest_FUP.h
                src/comps/mcuxClRsa/inc/mcuxClRsa_PrivateCrt_FUP.h
                src/comps/mcuxClRsa/inc/mcuxClRsa_RemoveBlinding_FUP.h
                src/comps/mcuxClRsa/inc/mcuxClRsa_TestPQDistance_FUP.h
                src/comps/mcuxClRsa/inc/mcuxClRsa_TestPrimeCandidate_FUP.h
                src/comps/mcuxClRsa/inc/mcuxClRsa_Types.h
                src/comps/mcuxClRsa/inc/internal/mcuxClRsa_ComputeD_FUP.h
                src/comps/mcuxClRsa/inc/internal/mcuxClRsa_Internal_Functions.h
                src/comps/mcuxClRsa/inc/internal/mcuxClRsa_Internal_Macros.h
                src/comps/mcuxClRsa/inc/internal/mcuxClRsa_Internal_MemoryConsumption.h
                src/comps/mcuxClRsa/inc/internal/mcuxClRsa_Internal_PkcDefs.h
                src/comps/mcuxClRsa/inc/internal/mcuxClRsa_Internal_PkcTypes.h
                src/comps/mcuxClRsa/inc/internal/mcuxClRsa_Internal_Types.h
                src/comps/mcuxClRsa/inc/internal/mcuxClRsa_KeyGeneration_Crt_FUP.h
                src/comps/mcuxClRsa/inc/internal/mcuxClRsa_MillerRabinTest_FUP.h
                src/comps/mcuxClRsa/inc/internal/mcuxClRsa_PrivateCrt_FUP.h
                src/comps/mcuxClRsa/inc/internal/mcuxClRsa_RemoveBlinding_FUP.h
                src/comps/mcuxClRsa/inc/internal/mcuxClRsa_TestPQDistance_FUP.h
                src/comps/mcuxClRsa/inc/internal/mcuxClRsa_TestPrimeCandidate_FUP.h
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
    mcux_add_include(
        INCLUDES ./
                 src/comps/mcuxClRsa/inc
                 src/comps/mcuxClRsa/inc/internal
                 BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.session)
    mcux_add_source(
        SOURCES src/comps/mcuxClSession/src/mcuxClSession.c
                src/comps/mcuxClSession/inc/mcuxClSession.h
                src/comps/mcuxClSession/inc/mcuxClSession_Functions.h
#                 src/comps/mcuxClSession/inc/mcuxClSession_MemoryConsumption.h
                src/comps/mcuxClSession/inc/mcuxClSession_Types.h
                src/comps/mcuxClSession/inc/internal/mcuxClSession_Internal.h
                src/comps/mcuxClSession/inc/internal/mcuxClSession_Internal_EntryExit.h
                src/comps/mcuxClSession/inc/internal/mcuxClSession_Internal_EntryExit_RegularReturn.h
                src/comps/mcuxClSession/inc/internal/mcuxClSession_Internal_Functions.h
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
    mcux_add_include(
        INCLUDES ./
                 src/comps/mcuxClSession/inc
                 src/comps/mcuxClSession/inc/internal
                 BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.signature)
    mcux_add_source(
        SOURCES src/comps/mcuxClSignature/src/mcuxClSignature.c
                src/comps/mcuxClSignature/inc/mcuxClSignature.h
                src/comps/mcuxClSignature/inc/mcuxClSignature_Constants.h
                src/comps/mcuxClSignature/inc/mcuxClSignature_Functions.h
#                 src/comps/mcuxClSignature/inc/mcuxClSignature_MemoryConsumption.h
                src/comps/mcuxClSignature/inc/mcuxClSignature_Types.h
                src/comps/mcuxClSignature/inc/internal/mcuxClSignature_Internal.h
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
    mcux_add_include(
        INCLUDES ./
                 src/comps/mcuxClSignature/inc
                 src/comps/mcuxClSignature/inc/internal
                 BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.trng)
    mcux_add_source(
        SOURCES src/comps/mcuxClTrng/inc/internal/mcuxClTrng_Internal.h
                src/comps/mcuxClTrng/inc/internal/mcuxClTrng_Internal_Constants.h
                src/comps/mcuxClTrng/inc/internal/mcuxClTrng_Internal_Functions.h
                src/comps/mcuxClTrng/inc/internal/mcuxClTrng_Internal_SA_TRNG.h
                src/comps/mcuxClTrng/inc/internal/mcuxClTrng_Internal_Types.h
                src/comps/mcuxClTrng/inc/internal/mcuxClTrng_SfrAccess.h
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
    mcux_add_include(
        INCLUDES ./
                 src/comps/mcuxClTrng/inc
                 src/comps/mcuxClTrng/inc/internal
                 BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.trng.type_els)
    mcux_add_source(
        SOURCES src/comps/mcuxClRandomModes/src/mcuxClRandomModes_NormalMode.c
                src/comps/mcuxClTrng/src/mcuxClTrng_ELS.c
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
    mcux_add_include(
        INCLUDES ./
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.trng.type_rng4)
    mcux_add_source(
        SOURCES src/comps/mcuxClTrng/src/mcuxClTrng_SA_TRNG.c
                src/comps/mcuxClTrng/inc/internal/mcuxClTrng_SfrAccess.h
                src/comps/mcuxClTrng/inc/internal/mcuxClTrng_Internal_SA_TRNG.h
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
    mcux_add_include(
        INCLUDES ./
                 src/comps/mcuxClTrng/inc
                 src/comps/mcuxClTrng/inc/internal
                 BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.pre_processor)
    mcux_add_source(
        SOURCES src/comps/mcuxCsslCPreProcessor/inc/mcuxCsslAnalysis.h
                src/comps/mcuxCsslCPreProcessor/inc/mcuxCsslCPreProcessor.h
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
    mcux_add_include(
        INCLUDES ./
                 src/comps/mcuxCsslCPreProcessor/inc
                 BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.data_integrity)
    mcux_add_source(
        SOURCES src/comps/mcuxCsslDataIntegrity/inc/mcuxCsslDataIntegrity.h
                src/comps/mcuxCsslDataIntegrity/inc/mcuxCsslDataIntegrity_Cfg.h
                src/comps/mcuxCsslDataIntegrity/inc/mcuxCsslDataIntegrity_Impl.h
                src/comps/mcuxCsslDataIntegrity/inc/mcuxCsslDataIntegrity_None.h
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
    mcux_add_include(
        INCLUDES ./
                 src/comps/mcuxCsslDataIntegrity/inc/
                 BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.flow_protection)
    mcux_add_source(
        SOURCES src/comps/mcuxCsslFlowProtection/inc/mcuxCsslFlowProtection.h
                src/comps/mcuxCsslFlowProtection/inc/mcuxCsslFlowProtection_Cfg.h
                src/comps/mcuxCsslFlowProtection/inc/mcuxCsslFlowProtection_FunctionIdentifiers.h
                src/comps/mcuxCsslFlowProtection/inc/mcuxCsslFlowProtection_Impl.h
                src/comps/mcuxCsslFlowProtection/inc/mcuxCsslFlowProtection_SecureCounter_Common.h
                src/comps/mcuxCsslFlowProtection/inc/mcuxCsslFlowProtection_SecureCounter_Local.h
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
    mcux_add_include(
        INCLUDES ./
                 src/comps/mcuxCsslFlowProtection/inc
                 BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.param_integrity)
    mcux_add_source(
        SOURCES src/comps/mcuxCsslParamIntegrity/src/mcuxCsslParamIntegrity32.c
                src/comps/mcuxCsslParamIntegrity/inc/mcuxCsslParamIntegrity.h
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
    mcux_add_include(
        INCLUDES ./
                 src/comps/mcuxCsslParamIntegrity/inc
                 BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.secure_counter)
    mcux_add_source(
        SOURCES src/comps/mcuxCsslSecureCounter/inc/mcuxCsslSecureCounter.h
                src/comps/mcuxCsslSecureCounter/inc/mcuxCsslSecureCounter_Cfg.h
                src/comps/mcuxCsslSecureCounter/inc/mcuxCsslSecureCounter_Impl.h
                src/comps/mcuxCsslSecureCounter/inc/mcuxCsslSecureCounter_None.h
                src/comps/mcuxCsslSecureCounter/inc/mcuxCsslSecureCounter_SW_Local.h
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
    mcux_add_include(
        INCLUDES ./
                 src/comps/mcuxCsslSecureCounter/inc
                 BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc)
    mcux_add_source(
        SOURCES src/inc/mcuxCl_clns.h
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
    mcux_add_include(
        INCLUDES ./
                 src/inc
                 BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.toolchain)
    mcux_add_source(
        SOURCES /src/compiler/mcuxClToolchain.h
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
    mcux_add_include(
        INCLUDES ./
                 src/compiler
                 BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.platform.mcxn)
    mcux_add_source(
        SOURCES src/platforms/mcxn/platform_specific_headers.h
                src/platforms/mcxn/mcuxClConfig.h
                src/platforms/mcxn/mcux_els.c
                src/platforms/mcxn/mcux_els.h
                src/platforms/mcxn/mcux_pkc.c
                src/platforms/mcxn/mcux_pkc.h
                src/platforms/mcxn/inc/ip_css_constants.h
                src/platforms/mcxn/inc/ip_css_design_configuration.h
                src/platforms/mcxn/inc/ip_platform.h
                includes/platform/mcxn/mcuxClAeadModes_MemoryConsumption.h
                includes/platform/mcxn/mcuxClCipherModes_MemoryConsumption.h
                includes/platform/mcxn/mcuxClEcc_MemoryConsumption.h
                includes/platform/mcxn/mcuxClHashModes_MemoryConsumption.h
                includes/platform/mcxn/mcuxClHash_MemoryConsumption.h
                includes/platform/mcxn/mcuxClHmac_MemoryConsumption.h
                includes/platform/mcxn/mcuxClKey_MemoryConsumption.h
                includes/platform/mcxn/mcuxClMacModes_MemoryConsumption.h
                includes/platform/mcxn/mcuxClOsccaSm3_MemoryConsumption.h
                includes/platform/mcxn/mcuxClRandomModes_MemoryConsumption.h
                includes/platform/mcxn/mcuxClRsa_MemoryConsumption.h
                includes/platform/mcxn/mcuxClSession_MemoryConsumption.h
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
    mcux_add_include(
        INCLUDES ./
                 src/platforms/mcxn
                 src/platforms/mcxn/inc
                 includes/platform/mcxn
                 BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.platform.rw61x_inf_header_only)
    mcux_add_source(
        SOURCES src/platforms/rw61x/platform_specific_headers.h
                src/platforms/rw61x/mcuxClConfig.h
                src/platforms/rw61x/inc/ip_css_constants.h
                src/platforms/rw61x/inc/ip_css_design_configuration.h
                src/platforms/rw61x/inc/ip_platform.h
#                 includes/platform/rw61x/mcuxClPsaDriver_MemoryConsumption.h
                includes/platform/rw61x/mcuxClAeadModes_MemoryConsumption.h
                includes/platform/rw61x/mcuxClCipherModes_MemoryConsumption.h
                includes/platform/rw61x/mcuxClEcc_MemoryConsumption.h
                includes/platform/rw61x/mcuxClHashModes_MemoryConsumption.h
                includes/platform/rw61x/mcuxClHash_MemoryConsumption.h
                includes/platform/rw61x/mcuxClHmac_MemoryConsumption.h
                includes/platform/rw61x/mcuxClKey_MemoryConsumption.h
                includes/platform/rw61x/mcuxClMacModes_MemoryConsumption.h
                includes/platform/rw61x/mcuxClRandomModes_MemoryConsumption.h
                includes/platform/rw61x/mcuxClRsa_MemoryConsumption.h
                includes/platform/rw61x/mcuxClSession_MemoryConsumption.h
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
    mcux_add_include(
        INCLUDES ./
                 src/platforms/rw61x
                 src/platforms/rw61x/inc
                 includes/platform/rw61x
                 BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.platform.rw61x_interface_files)
    mcux_add_source(
        SOURCES src/platforms/rw61x/mcux_els.c
                src/platforms/rw61x/mcux_els.h
                src/platforms/rw61x/mcux_pkc.c
                src/platforms/rw61x/mcux_pkc.h
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
    mcux_add_include(
        INCLUDES ./
                 src/platforms/rw61x
                 BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.platform.lpc)
    mcux_add_source(
        SOURCES src/platforms/lpc/platform_specific_headers.h
                src/platforms/lpc/mcuxClConfig.h
                src/platforms/lpc/inc/ip_css_constants.h
                src/platforms/lpc/inc/ip_css_design_configuration.h
                src/platforms/lpc/inc/ip_platform.h
                src/platforms/lpc/mcux_els.c
                src/platforms/lpc/mcux_els.h
                src/platforms/lpc/mcux_pkc.c
                src/platforms/lpc/mcux_pkc.h
                includes/platform/lpc/mcuxClAeadModes_MemoryConsumption.h
                includes/platform/lpc/mcuxClCipherModes_MemoryConsumption.h
                includes/platform/lpc/mcuxClEcc_MemoryConsumption.h
                includes/platform/lpc/mcuxClHashModes_MemoryConsumption.h
                includes/platform/lpc/mcuxClHash_MemoryConsumption.h
                includes/platform/lpc/mcuxClHmac_MemoryConsumption.h
                includes/platform/lpc/mcuxClKey_MemoryConsumption.h
                includes/platform/lpc/mcuxClMacModes_MemoryConsumption.h
                includes/platform/lpc/mcuxClRandomModes_MemoryConsumption.h
                includes/platform/lpc/mcuxClRsa_MemoryConsumption.h
                includes/platform/lpc/mcuxClSession_MemoryConsumption.h
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
    mcux_add_include(
        INCLUDES ./
                 src/platforms/lpc
                 src/platforms/lpc/inc
                 includes/platform/lpc
                 BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.platform.mimxrt)
    mcux_add_source(
        SOURCES src/platforms/mimxrt/platform_specific_headers.h
                src/platforms/mimxrt/mcuxClConfig.h
                src/platforms/mimxrt/inc/ip_css_constants.h
                src/platforms/mimxrt/inc/ip_css_design_configuration.h
                src/platforms/mimxrt/inc/ip_platform.h
                src/platforms/mimxrt/mcux_els.c
                src/platforms/mimxrt/mcux_els.h
                src/platforms/mimxrt/mcux_pkc.c
                src/platforms/mimxrt/mcux_pkc.h
                includes/platform/mimxrt/mcuxClAeadModes_MemoryConsumption.h
                includes/platform/mimxrt/mcuxClCipherModes_MemoryConsumption.h
                includes/platform/mimxrt/mcuxClEcc_MemoryConsumption.h
                includes/platform/mimxrt/mcuxClHash_MemoryConsumption.h
                includes/platform/mimxrt/mcuxClHashModes_MemoryConsumption.h
                includes/platform/mimxrt/mcuxClHmac_MemoryConsumption.h
                includes/platform/mimxrt/mcuxClKey_MemoryConsumption.h
                includes/platform/mimxrt/mcuxClMacModes_MemoryConsumption.h
                includes/platform/mimxrt/mcuxClOsccaAeadModes_MemoryConsumption.h
                includes/platform/mimxrt/mcuxClOsccaCipherModes_MemoryConsumption.h
                includes/platform/mimxrt/mcuxClOsccaMacModes_MemoryConsumption.h
                includes/platform/mimxrt/mcuxClOsccaRandomModes_MemoryConsumption.h
                includes/platform/mimxrt/mcuxClOsccaSm2_MemoryConsumption.h
                includes/platform/mimxrt/mcuxClOsccaSm3_MemoryConsumption.h
                includes/platform/mimxrt/mcuxClRandomModes_MemoryConsumption.h
                includes/platform/mimxrt/mcuxClRsa_MemoryConsumption.h
                includes/platform/mimxrt/mcuxClSession_MemoryConsumption.h
                includes/platform/mimxrt/mcuxClSignature_MemoryConsumption.h
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
    mcux_add_include(
        INCLUDES ./
                 src/platforms/mimxrt
                 src/platforms/mimxrt/inc
                 includes/platform/mimxrt
                 BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.examples)
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.examples_memory)
    mcux_add_source(
        SOURCES examples/mcuxCsslMemory/mcuxCsslMemory_Clear_example.c
                examples/mcuxCsslMemory/mcuxCsslMemory_Compare_example.c
                examples/mcuxCsslMemory/mcuxCsslMemory_Copy_example.c
                examples/mcuxCsslMemory/mcuxCsslMemory_Set_example.c
                examples/mcuxCsslMemory/inc/mcuxCsslMemory_Examples.h
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
    mcux_add_include(
        INCLUDES ./
                 examples/mcuxCsslFlowProtection/inc/
                 examples/mcuxCsslMemory/inc/
                 src/comps/mcuxClExample/inc/
                 src/comps/mcuxClBuffer/inc/
                 BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.examples_flow_protection)
    mcux_add_source(
        SOURCES examples/mcuxCsslFlowProtection/mcuxCsslFlowProtection_example.c
                examples/mcuxCsslFlowProtection/inc/mcuxCsslExamples.h
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
    mcux_add_include(
        INCLUDES ./
                 examples/mcuxCsslFlowProtection/inc/
                 examples/mcuxCsslMemory/inc/
                 src/comps/mcuxClExample/inc/
                 src/comps/mcuxClBuffer/inc/
                 BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.examples_rsa)
    mcux_add_source(
        SOURCES examples/mcuxClRsa/mcuxClRsa_sign_NoEncode_example.c
                examples/mcuxClRsa/mcuxClRsa_sign_pss_sha2_256_example.c
                examples/mcuxClRsa/mcuxClRsa_verify_NoVerify_example.c
                examples/mcuxClRsa/mcuxClRsa_verify_pssverify_sha2_256_example.c
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
    mcux_add_include(
        INCLUDES ./
                 examples/mcuxCsslFlowProtection/inc/
                 examples/mcuxCsslMemory/inc/
                 src/comps/mcuxClExample/inc/
                 src/comps/mcuxClBuffer/inc/
                 BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.examples_random_modes)
    mcux_add_source(
        SOURCES examples/mcuxClRandomModes/mcuxClRandomModes_CtrDrbg_AES256_DRG3_example.c
                examples/mcuxClRandomModes/mcuxClRandomModes_CtrDrbg_AES256_DRG4_example.c
                examples/mcuxClRandomModes/mcuxClRandomModes_CtrDrbg_AES256_ELS_example.c
                examples/mcuxClRandomModes/mcuxClRandomModes_Different_Sessions_example.c
                examples/mcuxClRandomModes/mcuxClRandomModes_PatchMode_CtrDrbg_AES256_DRG3_example.c
                examples/mcuxClRandomModes/mcuxClRandomModes_TestMode_CtrDrbg_AES256_DRG4_example.c
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
    mcux_add_include(
        INCLUDES ./
                 examples/mcuxCsslFlowProtection/inc/
                 examples/mcuxCsslMemory/inc/
                 src/comps/mcuxClExample/inc/
                 src/comps/mcuxClBuffer/inc/
                 BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.examples_mac_modes)
    mcux_add_source(
        SOURCES examples/mcuxClMacModes/mcuxClMacModes_Els_Cbcmac_Aes128_Oneshot_example.c
                examples/mcuxClMacModes/mcuxClMacModes_Els_Cbcmac_Aes256_Multipart_PaddingZero_example.c
                examples/mcuxClMacModes/mcuxClMacModes_Els_Cmac_Aes128_Oneshot_example.c
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
    mcux_add_include(
        INCLUDES ./
                 examples/mcuxCsslFlowProtection/inc/
                 examples/mcuxCsslMemory/inc/
                 src/comps/mcuxClExample/inc/
                 src/comps/mcuxClBuffer/inc/
                 BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.examples_key)
    mcux_add_source(
        SOURCES examples/mcuxClKey/mcuxClKey_example.c
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
    mcux_add_include(
        INCLUDES ./
                 examples/mcuxCsslFlowProtection/inc/
                 examples/mcuxCsslMemory/inc/
                 src/comps/mcuxClExample/inc/
                 src/comps/mcuxClBuffer/inc/
                 BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.examples_hmac)
    mcux_add_source(
        SOURCES examples/mcuxClHmac/mcuxClHmac_Els_Oneshot_External_Key_example.c
                examples/mcuxClHmac/mcuxClHmac_Sw_Multipart_example.c
                examples/mcuxClHmac/mcuxClHmac_Sw_Oneshot_example.c
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
    mcux_add_include(
        INCLUDES ./
                 examples/mcuxCsslFlowProtection/inc/
                 examples/mcuxCsslMemory/inc/
                 src/comps/mcuxClExample/inc/
                 src/comps/mcuxClBuffer/inc/
                 BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.examples_hash_modes)
    mcux_add_source(
        SOURCES examples/mcuxClHashModes/mcuxClHashModes_sha1_longMsgOneshot_example.c
                examples/mcuxClHashModes/mcuxClHashModes_sha1_oneshot_example.c
                examples/mcuxClHashModes/mcuxClHashModes_sha1_streaming_example.c
                examples/mcuxClHashModes/mcuxClHashModes_sha224_oneshot_example.c
                examples/mcuxClHashModes/mcuxClHashModes_sha256_longMsgOneshot_example.c
                examples/mcuxClHashModes/mcuxClHashModes_sha256_oneshot_example.c
                examples/mcuxClHashModes/mcuxClHashModes_sha256_streaming_example.c
                examples/mcuxClHashModes/mcuxClHashModes_sha384_oneshot_example.c
                examples/mcuxClHashModes/mcuxClHashModes_sha512_224_oneshot_example.c
                examples/mcuxClHashModes/mcuxClHashModes_sha512_256_oneshot_example.c
                examples/mcuxClHashModes/mcuxClHashModes_sha512_256_streaming_example.c
                examples/mcuxClHashModes/mcuxClHashModes_sha512_oneshot_example.c
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
    mcux_add_include(
        INCLUDES ./
                 examples/mcuxCsslFlowProtection/inc/
                 examples/mcuxCsslMemory/inc/
                 src/comps/mcuxClExample/inc/
                 src/comps/mcuxClBuffer/inc/
                 BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.examples_els)
    mcux_add_source(
        SOURCES examples/mcuxClEls/mcuxClEls_Cipher_Aes128_Cbc_Encrypt_example.c
                examples/mcuxClEls/mcuxClEls_Cipher_Aes128_Ecb_Encrypt_example.c
                examples/mcuxClEls/mcuxClEls_Common_Get_Info_example.c
                examples/mcuxClEls/mcuxClEls_Ecc_Keygen_Sign_Verify_example.c
                examples/mcuxClEls/mcuxClEls_Hash_HW_Security_Counter_example.c
                examples/mcuxClEls/mcuxClEls_Hash_Sha224_One_Block_example.c
                examples/mcuxClEls/mcuxClEls_Hash_Sha256_One_Block_example.c
                examples/mcuxClEls/mcuxClEls_Hash_Sha384_One_Block_example.c
                examples/mcuxClEls/mcuxClEls_Hash_Sha512_One_Block_example.c
                examples/mcuxClEls/mcuxClEls_Rng_Prng_Get_Random_example.c
                examples/mcuxClEls/mcuxClEls_Tls_Master_Key_Session_Keys_example.c
                examples/mcuxClEls/mcuxClEls_Cipher_Aes128_Cbc_Encrypt_example.c
                examples/mcuxClEls/mcuxClEls_Cipher_Aes128_Ecb_Encrypt_example.c
                examples/mcuxClEls/mcuxClEls_Common_Get_Info_example.c
                examples/mcuxClEls/mcuxClEls_Ecc_Keygen_Sign_Verify_example.c
                examples/mcuxClRandomModes/mcuxClRandomModes_ELS_example.c
                src/comps/mcuxClExample/inc/mcuxClExample_ELS_Helper.h
                src/comps/mcuxClExample/inc/mcuxClExample_ELS_Key_Helper.h
                src/comps/mcuxClExample/inc/mcuxClExample_Key_Helper.h
                src/comps/mcuxClExample/inc/mcuxClExample_RFC3394_Helper.h
                src/comps/mcuxClExample/inc/mcuxClExample_RNG_Helper.h
                src/comps/mcuxClExample/inc/mcuxClExample_Session_Helper.h
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
    mcux_add_include(
        INCLUDES ./
                 examples/mcuxCsslFlowProtection/inc/
                 examples/mcuxCsslMemory/inc/
                 src/comps/mcuxClExample/inc/
                 src/comps/mcuxClBuffer/inc/
                 BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.examples_ecc)
    mcux_add_source(
        SOURCES examples/mcuxClEcc/mcuxClEcc_EdDSA_Ed25519ctx_example.c
                examples/mcuxClEcc/mcuxClEcc_EdDSA_Ed25519ph_example.c
                examples/mcuxClEcc/mcuxClEcc_EdDSA_Ed25519_example.c
                examples/mcuxClEcc/mcuxClEcc_EdDSA_GenerateSignature_Ed25519_example.c
                examples/mcuxClEcc/mcuxClEcc_EdDSA_VerifySignature_Ed25519_example.c
                examples/mcuxClEcc/mcuxClEcc_MontDH_Curve25519_example.c
                examples/mcuxClEcc/mcuxClEcc_MontDH_Curve448_example.c
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
    mcux_add_include(
        INCLUDES ./
                 examples/mcuxCsslFlowProtection/inc/
                 examples/mcuxCsslMemory/inc/
                 src/comps/mcuxClExample/inc/
                 src/comps/mcuxClBuffer/inc/
                 BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.examples_aead)
    mcux_add_source(
        SOURCES examples/mcuxClAeadModes/mcuxClAeadModes_Els_Ccm_Aes128_Multipart_example.c
                examples/mcuxClAeadModes/mcuxClAeadModes_Els_Ccm_Aes128_Oneshot_example.c
                examples/mcuxClAeadModes/mcuxClAeadModes_Els_Gcm_Aes128_Oneshot_example.c
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
    mcux_add_include(
        INCLUDES ./
                 examples/mcuxCsslFlowProtection/inc/
                 examples/mcuxCsslMemory/inc/
                 src/comps/mcuxClExample/inc/
                 src/comps/mcuxClBuffer/inc/
                 BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.examples_cipher_modes)
    mcux_add_source(
        SOURCES examples/mcuxClCipherModes/mcuxClCipherModes_Els_Cbc_Aes128_Multipart_example.c
                examples/mcuxClCipherModes/mcuxClCipherModes_Els_Cbc_Aes128_Oneshot_example.c
                examples/mcuxClCipherModes/mcuxClCipherModes_Els_Cbc_Aes128_Oneshot_PaddingZero_example.c
                examples/mcuxClCipherModes/mcuxClCipherModes_Els_Ctr_Aes128_Multipart_example.c
                examples/mcuxClCipherModes/mcuxClCipherModes_Els_Ctr_Aes128_Oneshot_example.c
                examples/mcuxClCipherModes/mcuxClCipherModes_Els_Ecb_Aes128_Multipart_example.c
                examples/mcuxClCipherModes/mcuxClCipherModes_Els_Ecb_Aes128_Multipart_PaddingPKCS7_example.c
                examples/mcuxClCipherModes/mcuxClCipherModes_Els_Ecb_Aes128_Oneshot_example.c
                examples/mcuxClCipherModes/mcuxClCipherModes_Els_Ecb_Aes128_Oneshot_PaddingPKCS7_example.c
                examples/mcuxClCipherModes/mcuxClCipherModes_Els_Ecb_Aes128_Oneshot_PaddingZero_example.c
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
    mcux_add_include(
        INCLUDES ./
                 examples/mcuxCsslFlowProtection/inc/
                 examples/mcuxCsslMemory/inc/
                 src/comps/mcuxClExample/inc/
                 src/comps/mcuxClBuffer/inc/
                 BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
endif()

if (CONFIG_MCUX_COMPONENT_component.els_pkc.psa_driver_examples)
    mcux_add_source(
        SOURCES examples/mcuxClPsaDriver/mcuxClPsaDriver_aead_ccm_multipart_example.c
                examples/mcuxClPsaDriver/mcuxClPsaDriver_aead_ccm_oneshot_example.c
                examples/mcuxClPsaDriver/mcuxClPsaDriver_aead_gcm_multipart_example.c
                examples/mcuxClPsaDriver/mcuxClPsaDriver_aead_gcm_oneshot_example.c
                examples/mcuxClPsaDriver/mcuxClPsaDriver_aes_example.c
                examples/mcuxClPsaDriver/mcuxClPsaDriver_cipher_decrypt.c
                examples/mcuxClPsaDriver/mcuxClPsaDriver_cipher_multipart_CBC.c
                examples/mcuxClPsaDriver/mcuxClPsaDriver_cipher_multipart_CTR.c
                examples/mcuxClPsaDriver/mcuxClPsaDriver_cipher_multipart_ECB.c
                examples/mcuxClPsaDriver/mcuxClPsaDriver_eccsecp224k1_sign_verify_hash_example.c
                examples/mcuxClPsaDriver/mcuxClPsaDriver_ecdsa_keygen_oracleMemory_sign_verify_hash_example.c
                examples/mcuxClPsaDriver/mcuxClPsaDriver_ecdsa_keygen_oracleS50_sign_verify_hash_example.c
                examples/mcuxClPsaDriver/mcuxClPsaDriver_ecdsa_sign_verify_hash_example.c
                examples/mcuxClPsaDriver/mcuxClPsaDriver_ecdsa_sign_verify_message_example.c
                examples/mcuxClPsaDriver/mcuxClPsaDriver_keygen_export_public_key_brainpoolpr1_example.c
                examples/mcuxClPsaDriver/mcuxClPsaDriver_keygen_export_public_key_mont_curve25519_example.c
                examples/mcuxClPsaDriver/mcuxClPsaDriver_keygen_export_public_key_rsa_example.c
                examples/mcuxClPsaDriver/mcuxClPsaDriver_keygen_export_public_key_secpk1_example.c
                examples/mcuxClPsaDriver/mcuxClPsaDriver_keygen_export_public_key_secpr1_example.c
                examples/mcuxClPsaDriver/mcuxClPsaDriver_key_agreement_CURVE_25519_example.c
                examples/mcuxClPsaDriver/mcuxClPsaDriver_key_agreement_SECP_R1_example.c
                examples/mcuxClPsaDriver/mcuxClPsaDriver_mac_oneshot_example.c
                examples/mcuxClPsaDriver/mcuxClPsaDriver_mac_sign_multipart_example.c
                examples/mcuxClPsaDriver/mcuxClPsaDriver_mac_verify_multipart_example.c
                examples/mcuxClPsaDriver/mcuxClPsaDriver_rsa_PKCS1V15_sign_verify_message_example.c
                examples/mcuxClPsaDriver/mcuxClPsaDriver_rsa_PSS_sign_verify_hash_example.c
                examples/mcuxClPsaDriver/mcuxClPsaDriver_sha224_oneshot_example.c
                examples/mcuxClPsaDriver/mcuxClPsaDriver_sha256_abort_example.c
                examples/mcuxClPsaDriver/mcuxClPsaDriver_sha256_clone_example.c
                examples/mcuxClPsaDriver/mcuxClPsaDriver_sha256_multipart_example.c
                examples/mcuxClPsaDriver/mcuxClPsaDriver_sha256_oneshot_example.c
                examples/mcuxClPsaDriver/mcuxClPsaDriver_sha384_oneshot_example.c
                examples/mcuxClPsaDriver/mcuxClPsaDriver_sha512_oneshot_example.c
                examples/mcuxClPsaDriver/mcuxClPsaDriver_truncated_mac_oneshot_example.c
                BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
    mcux_add_include(
        INCLUDES ./
                 examples/mcuxCsslFlowProtection/inc
                 BASE_PATH ${SdkRootDirPath}/components/els_pkc/
    )
endif()
