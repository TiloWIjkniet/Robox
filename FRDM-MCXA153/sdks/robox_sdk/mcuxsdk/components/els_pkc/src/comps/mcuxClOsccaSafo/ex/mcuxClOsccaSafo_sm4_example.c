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

#include <mcuxClSession.h>          // Interface to the entire mcuxClSession component
#include <mcuxClOsccaSafo.h>
#include <mcuxCsslFlowProtection.h> // Code flow protection
#include <mcuxClCore_Examples.h>
#include <mcuxClExample_Session_Helper.h>

static const uint32_t plaintext_0[4U] = {
    0x01234567u, 0x89abcdefu, 0xfedcba98u, 0x76543210u
};
static const uint32_t key_0[4U] = {
    0x01234567u, 0x89abcdefu, 0xfedcba98u, 0x76543210u
};
static const uint32_t reference_ciphertext[4U] = {
    0x681edf34u, 0xd206965eu, 0x86b3e94fu, 0x536e4246u
};

static const uint32_t reference_ciphertext_reversed[4U] = {
    0x681edf34u, 0xd206965eu, 0x86b3e94fu, 0x536e4246u
};

static const uint32_t plaintext_1[64U] = {
    0xcc76f7edu, 0x48e797d7u, 0xf3411d2au, 0x3b07383au, 0xbb028be7u, 0x0818bc71u, 0x2584a8f5u, 0x8b930f48u, 0xb076e7f9u, 0xddd0fb19u,
    0x91bbee39u, 0xcfaec42bu, 0xc9728b01u, 0x740d007du, 0x85a2d4c0u, 0x64655f96u, 0x8b5a5eefu, 0x59aa1c92u, 0x83bb6552u, 0x28348541u,
    0x4e1061fbu, 0x3ea4331du, 0x7a8fa63cu, 0x1f757103u, 0xcf9b7077u, 0x1c1c1034u, 0xaa296211u, 0x438212f4u, 0x8ddcbbd9u, 0x8adad563u,
    0x562c4d61u, 0x963a73fbu, 0xc90ae940u, 0x73ef771au, 0xe61d1d62u, 0xc9668e50u, 0xd2f94109u, 0x9ed9d81bu, 0x976f6428u, 0xe9dc1de7u,
    0x051272a2u, 0xd9f0e160u, 0x51ba0963u, 0xa6d9e018u, 0x3d40147eu, 0xf92b006eu, 0xe328080du, 0x9e373d4bu, 0x5618eeaau, 0x027baeacu,
    0x44cecaf7u, 0xcd5f7095u, 0xeb96042eu, 0x9e8de364u, 0x3639b659u, 0x5958768du, 0x4699a2aeu, 0xd75cfa4bu, 0x125d25b0u, 0x88de322eu,
    0xeb0416e3u, 0x84b78c30u, 0xd3d38d3bu, 0x5c4f6358u,
};
static const uint32_t key_1[4U] = {
    0x7cac4fd7u, 0xed3400b9u, 0xa29f1808u, 0xf6281332u
};
static const uint32_t reference_ciphertext_1[64U] = {
    0x4b0505f3u, 0x17af787au, 0x64194eddu, 0xc189924cu, 0x2bdb8fb1u, 0x427fd61au, 0xb2f5aeb6u, 0x4aa3f108u, 0x38b57edcu, 0x0faa4f9au,
    0x9f268b2cu, 0xe84c95b5u, 0x67aa4c1du, 0x854880e9u, 0x6b92ab47u, 0xcb012424u, 0x2c68aaa1u, 0x28955084u, 0x4a16012fu, 0x7a3c435fu,
    0x446cb421u, 0xed73c7d6u, 0xc1d52ff4u, 0x395a7617u, 0xc9f0db0au, 0x44f16b5fu, 0x258269d1u, 0xa656812eu, 0xe371f080u, 0x302b4685u,
    0xcd7ac9f2u, 0xa380c2d9u, 0x8629fdfau, 0x151916bbu, 0x448d362eu, 0xca81f1a8u, 0x0f963c07u, 0x53a559d1u, 0x30034d64u, 0xac36133eu,
    0x59938073u, 0xdf2d6b8au, 0xb8d73d5du, 0x5aacb18du, 0x3a9b6dc4u, 0x822a91c8u, 0x13ed832du, 0x6a4afb4cu, 0xf69b95d2u, 0x29583f2cu,
    0x88f15cd1u, 0xc5a4daf3u, 0x9eeb2451u, 0x403e4e3bu, 0xe99670e5u, 0xc7a6cab7u, 0x4ad0c334u, 0x1177a217u, 0xf9464f98u, 0x5d26b9eau,
    0x9baa5c88u, 0xc98db450u, 0x8e09ca37u, 0x137714a6u
};

static const uint32_t plaintext_2[16U] = {
    0x2e7c4271u, 0x72299c55u, 0x729651a0u, 0x938b1ce9u, 0x1aa917d3u, 0xd6f8da3cu, 0x482a5628u, 0xc94d9089u,
    0x0a1a0228u, 0x58cc838cu, 0x3c0df801u, 0xb73d7b63u, 0xb5b92e04u, 0x09392591u, 0x7dd04c41u, 0xcf1b811bu
};
static const uint32_t key_2[4U] = {
    0xdcbad818u, 0x947e5137u, 0x7bd311eeu, 0x7fdcba0eu
};
static const uint32_t reference_ciphertext_2[16U] = {
    0x9bbfcf91u, 0x4be86872u, 0x8990bbe9u, 0x1febca41u,
    0xf40528c4u, 0xc4018712u, 0x88b8402au, 0x66e5ea72u,
    0xe46205cbu, 0x3054f659u, 0x539de41du, 0x19df6d8bu,
    0xfc2b185cu, 0xd44052c0u, 0xe8181f78u, 0x6428f1dfu
};

/*
 * decrypt = 0 -> perform encrypt operation
 * decrypt = 1 -> perform decrypt operation
 */
static void SM4_Operation(uint8_t decrypt, const uint32_t *input, const uint32_t *key, uint32_t *result)
{
    /* load plaintext/ciphertext */
    mcuxClOsccaSafo_Drv_load(0, input[3]);
    mcuxClOsccaSafo_Drv_load(1, input[2]);
    mcuxClOsccaSafo_Drv_load(2, input[1]);
    mcuxClOsccaSafo_Drv_load(3, input[0]);

    /* load key */
    mcuxClOsccaSafo_Drv_loadKey(0, key[3]);
    mcuxClOsccaSafo_Drv_loadKey(1, key[2]);
    mcuxClOsccaSafo_Drv_loadKey(2, key[1]);
    mcuxClOsccaSafo_Drv_loadKey(3, key[0]);

    /* Setup control SFRs:
     * SM4_EN = 1'b1
     * CRYPTO_OP = 3'b110 (SM4)
     * DECRYPT = 1'b0 (ENC) / 1'b1 (DEC) ('decrypt' value)
     * DATOUT_RES = 2'b00 (END_UP)
     * INSEL = 'h0
     * INKEYSEL = 'h0
     * OUTSEL = 'h0
     * START = 1'b1
     */
    mcuxClOsccaSafo_Drv_start(MCUXCLOSCCASAFO_DRV_CTRL_SM4_EN
                      | MCUXCLOSCCASAFO_DRV_CTRL_SM4
                      | ((decrypt == 1u)?MCUXCLOSCCASAFO_DRV_CTRL_DEC:MCUXCLOSCCASAFO_DRV_CTRL_ENC)
                      | MCUXCLOSCCASAFO_DRV_CTRL_END_UP
                      | MCUXCLOSCCASAFO_DRV_CTRL_INSEL_DATIN0
                      | MCUXCLOSCCASAFO_DRV_CTRL_INKEYSEL(0u)
                      | MCUXCLOSCCASAFO_DRV_CTRL_OUTSEL_RES);

    /* wait for AES operation to complete (polling 'busy' status bit) */
    mcuxClOsccaSafo_Drv_wait();

    /* read the result */
    result[3] = mcuxClOsccaSafo_Drv_store(0);
    result[2] = mcuxClOsccaSafo_Drv_store(1);
    result[1] = mcuxClOsccaSafo_Drv_store(2);
    result[0] = mcuxClOsccaSafo_Drv_store(3);
}

MCUXCLEXAMPLE_FUNCTION(mcuxClOsccaSafo_sm4_example)
{
    mcuxClOsccaSafo_Drv_enableFlush(MCUXCLOSCCASAFO_DRV_FLUSH_ALL);
    mcuxClOsccaSafo_Drv_init(0U);

    /*
    Encrypt test vector (1 block message)
    Input:
        message = 0123456789abcdeffedcba9876543210
        key = 0123456789abcdeffedcba9876543210

    Output (encrypt operation):
        ciphertext = 681edf34d206965e86b3e94f536e4246
    */
    uint32_t result_ciphertext[4U] = {0U};

    SM4_Operation(0x00, plaintext_0, key_0, result_ciphertext);
    /* check if actual result is equal to expected result */
    if (true != mcuxClCore_assertEqual((const uint8_t *)result_ciphertext, (const uint8_t *)reference_ciphertext, sizeof(reference_ciphertext)))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }


    /*
    Decrypt test vector (1 block message)
    Input:
        message = 681edf34d206965e86b3e94f536e4246
        key = 0123456789abcdeffedcba9876543210

    Output (decrypt operation):
        ciphertext = 0123456789abcdeffedcba9876543210
    */
    uint32_t result_ciphertextRev[4U] = {0U};

    SM4_Operation(0x01, reference_ciphertext_reversed, key_0, result_ciphertextRev);
    /* check if actual result is equal to expected result */
    if (true != mcuxClCore_assertEqual((const uint8_t *)result_ciphertextRev, (const uint8_t *)plaintext_0, sizeof(plaintext_0)))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }

    /*
    Encrypt test vector (16 blocks message)
    Input:
        message = 2e7c427172299c55729651a0938b1ce91aa917d3d6f8da3c482a5628c94d90890a1a022858cc838c3c0df801b73d7b63b5b92e04093925917dd04c41cf1b811b
        key = dcbad818947e51377bd311ee7fdcba0e

    Output (encrypt operation):
        ciphertext = 9bbfcf914be868728990bbe91febca41f40528c4c401871288b8402a66e5ea72e46205cb3054f659539de41d19df6d8bfc2b185cd44052c0e8181f786428f1df
    */
    uint32_t result_ciphertext_1[64U];
    for (uint8_t i = 0; i < 16U; i++)
    {
        SM4_Operation(0x00, &plaintext_1[i * 4u], key_1, &result_ciphertext_1[i * 4u]);  /* process one SM4 block (16 bytes) per call */
    }
    /* check if actual result is equal to expected result */
    if (true != mcuxClCore_assertEqual((const uint8_t *)result_ciphertext_1, (const uint8_t *)reference_ciphertext_1, sizeof(reference_ciphertext_1)))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }

    /*
    Decrypt test vector (4 blocks message)
    Input:
        message = 2e7c427172299c55729651a0938b1ce91aa917d3d6f8da3c482a5628c94d90890a1a022858cc838c3c0df801b73d7b63b5b92e04093925917dd04c41cf1b811b
        key = dcbad818947e51377bd311ee7fdcba0e

    Output (encrypt operation):
        ciphertext = 9bbfcf914be868728990bbe91febca41f40528c4c401871288b8402a66e5ea72e46205cb3054f659539de41d19df6d8bfc2b185cd44052c0e8181f786428f1df
    */
    uint32_t result_ciphertext_2[16U];
    for (uint8_t i = 0U; i < 4U; i++)
    {
        SM4_Operation(0x01, &plaintext_2[i * 4u], key_2, &result_ciphertext_2[i * 4u]);  /* process one SM4 block (16 bytes) per call */
    }
    /* check if actual result is equal to expected result */
    if (true != mcuxClCore_assertEqual((const uint8_t *)result_ciphertext_2, (const uint8_t *)reference_ciphertext_2, sizeof(reference_ciphertext_2)))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }

    uint32_t closeRet = mcuxClOsccaSafo_Drv_close();
    if(MCUXCLOSCCASAFO_STATUS_ERROR == closeRet)
    {
        /* If error flush whole SAFO */
        mcuxClOsccaSafo_Drv_enableFlush(MCUXCLOSCCASAFO_DRV_FLUSH_ALL);
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }

    return MCUXCLEXAMPLE_STATUS_OK;
}
