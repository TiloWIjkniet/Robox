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

#include <mcuxClToolchain.h>
#include <mcuxClSession.h>          // Interface to the entire mcuxClSession component
#include <mcuxClMemory.h>
#include <mcuxClOsccaSafo.h>
#include <mcuxCsslFlowProtection.h> // Code flow protection
#include <mcuxClCore_Examples.h>
#include <mcuxClExample_Session_Helper.h>

#ifdef MCUXCL_FEATURE_HW_SAFO_SM4
/* xorWr_test test vector */
static const uint32_t message[4U] = {
    0x8f4ba8e0U, 0x297da02bU, 0x5cf2f7a2U, 0x4167c487U
};

static const uint32_t xormessage[4U] = {
    0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U
};

/* SM4 Encrypt test vector (1 block message) */
static const uint32_t plaintext[4U] = {
    0x01234567U, 0x89abcdefU, 0xfedcba98U, 0x76543210U
};
static const uint32_t key[4U] = {
    0x01234567U, 0x89abcdefU, 0xfedcba98U, 0x76543210U
};
static const uint32_t reference_ciphertext[4U] = {
    0x681edf34U, 0xd206965eU, 0x86b3e94fU, 0x536e4246U
};

/* SM4 Ctr test vector (4 block message) */
static const ALIGNED uint8_t sm4CtrKey[] = {
    0x01U, 0x23U, 0x45U, 0x67U, 0x89U, 0xABU, 0xCDU, 0xEFU,
    0xFEU, 0xDCU, 0xBAU, 0x98U, 0x76U, 0x54U, 0x32U, 0x10U
};

static const ALIGNED uint8_t sm4CtrPtxt[] = {
    0xAAU, 0xAAU, 0xAAU, 0xAAU, 0xAAU, 0xAAU, 0xAAU, 0xAAU,
    0xBBU, 0xBBU, 0xBBU, 0xBBU, 0xBBU, 0xBBU, 0xBBU, 0xBBU,
    0xCCU, 0xCCU, 0xCCU, 0xCCU, 0xCCU, 0xCCU, 0xCCU, 0xCCU,
    0xDDU, 0xDDU, 0xDDU, 0xDDU, 0xDDU, 0xDDU, 0xDDU, 0xDDU,
    0xEEU, 0xEEU, 0xEEU, 0xEEU, 0xEEU, 0xEEU, 0xEEU, 0xEEU,
    0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU,
    0xAAU, 0xAAU, 0xAAU, 0xAAU, 0xAAU, 0xAAU, 0xAAU, 0xAAU,
    0xBBU, 0xBBU, 0xBBU, 0xBBU, 0xBBU, 0xBBU, 0xBBU, 0xBBU
};

static const ALIGNED uint8_t sm4CtrIv[] = {
    0x00U, 0x01U, 0x02U, 0x03U, 0x04U, 0x05U, 0x06U, 0x07U,
    0x08U, 0x09U, 0x0AU, 0x0BU, 0x0CU, 0x0DU, 0x0EU, 0x0FU
};

static const uint8_t sm4CtrCtxt[] = {
    0xACU, 0x32U, 0x36U, 0xCBU, 0x97U, 0x0CU, 0xC2U, 0x07U,
    0x91U, 0x36U, 0x4CU, 0x39U, 0x5AU, 0x13U, 0x42U, 0xD1U,
    0xA3U, 0xCBU, 0xC1U, 0x87U, 0x8CU, 0x6FU, 0x30U, 0xCDU,
    0x07U, 0x4CU, 0xCEU, 0x38U, 0x5CU, 0xDDU, 0x70U, 0xC7U,
    0xF2U, 0x34U, 0xBCU, 0x0EU, 0x24U, 0xC1U, 0x19U, 0x80U,
    0xFDU, 0x12U, 0x86U, 0x31U, 0x0CU, 0xE3U, 0x7BU, 0x92U,
    0x6EU, 0x02U, 0xFCU, 0xD0U, 0xFAU, 0xA0U, 0xBAU, 0xF3U,
    0x8BU, 0x29U, 0x33U, 0x85U, 0x1DU, 0x82U, 0x45U, 0x14U
};

#endif /* MCUXCL_FEATURE_HW_SAFO_SM4 */

#ifdef MCUXCL_FEATURE_HW_SAFO_SM3
/* SM3 Automatic mode, partial processing (4 padded blocks) */
static const uint32_t message_hash[64U] = {
    0x64fce814U, 0xfa17cecfU, 0x9a97c6a8U, 0x15183f0dU, 0xb881d336U, 0x7eb90024U, 0x7d997ee0U, 0x27a25ed2U, 0xaac0a62fU, 0x0718227dU,
    0xd6e82f17U, 0xe6f56301U, 0x1945d3e5U, 0x8002e5c5U, 0xd0dc66e2U, 0x9b55c71cU, 0xde0d6d87U, 0xcd211331U, 0x056b122dU, 0x069c5562U,
    0x10d29e62U, 0xdfdaca25U, 0x87fe07e1U, 0x635bc44fU, 0xd07bb099U, 0x0e6af75cU, 0x9b1f0139U, 0xa117ef56U, 0x39ab73c5U, 0xf7f7793bU,
    0xb2277b97U, 0x49af279bU, 0xf722b9c8U, 0x4a786f12U, 0x9e441112U, 0xf184a9feU, 0x745cd390U, 0xd4f4dadcU, 0x773c31d0U, 0x89c39c2eU,
    0xb610dac9U, 0x73bd5e3fU, 0x13b14bf5U, 0x25b43dd0U, 0xc8591380U, 0xb0424647U, 0x82e6d4b8U, 0x336abcdaU, 0x80000000U, 0x00000000U,
    0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U,
    0x00000000U, 0x00000000U, 0x00000000U, 0x00000600U
};
static const uint32_t reference_hash_hash[8U] = {
    0x5cf5619aU, 0x84adcfc1U, 0x9165d942U, 0x19b32dfcU, 0xd5baecdeU, 0x3fa93ce7U, 0x1e675e62U, 0xe2aa7ce5U
};
#endif /* MCUXCL_FEATURE_HW_SAFO_SM3 */

#ifdef MCUXCL_FEATURE_HW_SAFO_SM4
/* Cover test case for mcuxClOsccaSafo_Drv_enableXorWrite and mcuxClOsccaSafo_Drv_disableXorWrite */
static bool xorWr_test(void)
{
    /* Cover test case for mcuxClOsccaSafo_Drv_enableXorWrite and mcuxClOsccaSafo_Drv_disableXorWrite */
    uint32_t pXorWrOut[4];

    //Copy input to SAFO
    mcuxClOsccaSafo_Drv_load(MCUXCLOSCCASAFO_DRV_DATIN0_INDEX + 0U, message[0]);
    mcuxClOsccaSafo_Drv_load(MCUXCLOSCCASAFO_DRV_DATIN0_INDEX + 1U, message[1]);
    mcuxClOsccaSafo_Drv_load(MCUXCLOSCCASAFO_DRV_DATIN0_INDEX + 2U, message[2]);
    mcuxClOsccaSafo_Drv_load(MCUXCLOSCCASAFO_DRV_DATIN0_INDEX + 3U, message[3]);

    uint32_t ctrl2Backup = mcuxClOsccaSafo_Drv_enableXorWrite();

    //Copy input to SAFO
    mcuxClOsccaSafo_Drv_load(MCUXCLOSCCASAFO_DRV_DATIN0_INDEX + 0U, message[0]);
    mcuxClOsccaSafo_Drv_load(MCUXCLOSCCASAFO_DRV_DATIN0_INDEX + 1U, message[1]);
    mcuxClOsccaSafo_Drv_load(MCUXCLOSCCASAFO_DRV_DATIN0_INDEX + 2U, message[2]);
    mcuxClOsccaSafo_Drv_load(MCUXCLOSCCASAFO_DRV_DATIN0_INDEX + 3U, message[3]);

    pXorWrOut[0] = mcuxClOsccaSafo_Drv_storeInput(MCUXCLOSCCASAFO_DRV_DATIN0_INDEX + 0u);
    pXorWrOut[1] = mcuxClOsccaSafo_Drv_storeInput(MCUXCLOSCCASAFO_DRV_DATIN0_INDEX + 1u);
    pXorWrOut[2] = mcuxClOsccaSafo_Drv_storeInput(MCUXCLOSCCASAFO_DRV_DATIN0_INDEX + 2u);
    pXorWrOut[3] = mcuxClOsccaSafo_Drv_storeInput(MCUXCLOSCCASAFO_DRV_DATIN0_INDEX + 3u);

    if(true != mcuxClCore_assertEqual((const uint8_t *)pXorWrOut, (const uint8_t *)xormessage, sizeof(xormessage)))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }

    mcuxClOsccaSafo_Drv_disableXorWrite();

    //Copy input to SAFO
    mcuxClOsccaSafo_Drv_load(MCUXCLOSCCASAFO_DRV_DATIN0_INDEX + 0U, message[0]);
    mcuxClOsccaSafo_Drv_load(MCUXCLOSCCASAFO_DRV_DATIN0_INDEX + 1U, message[1]);
    mcuxClOsccaSafo_Drv_load(MCUXCLOSCCASAFO_DRV_DATIN0_INDEX + 2U, message[2]);
    mcuxClOsccaSafo_Drv_load(MCUXCLOSCCASAFO_DRV_DATIN0_INDEX + 3U, message[3]);

    //Copy input to SAFO
    mcuxClOsccaSafo_Drv_load(MCUXCLOSCCASAFO_DRV_DATIN0_INDEX + 0U, message[0]);
    mcuxClOsccaSafo_Drv_load(MCUXCLOSCCASAFO_DRV_DATIN0_INDEX + 1U, message[1]);
    mcuxClOsccaSafo_Drv_load(MCUXCLOSCCASAFO_DRV_DATIN0_INDEX + 2U, message[2]);
    mcuxClOsccaSafo_Drv_load(MCUXCLOSCCASAFO_DRV_DATIN0_INDEX + 3U, message[3]);

    pXorWrOut[0] = mcuxClOsccaSafo_Drv_storeInput(MCUXCLOSCCASAFO_DRV_DATIN0_INDEX + 0u);
    pXorWrOut[1] = mcuxClOsccaSafo_Drv_storeInput(MCUXCLOSCCASAFO_DRV_DATIN0_INDEX + 1u);
    pXorWrOut[2] = mcuxClOsccaSafo_Drv_storeInput(MCUXCLOSCCASAFO_DRV_DATIN0_INDEX + 2u);
    pXorWrOut[3] = mcuxClOsccaSafo_Drv_storeInput(MCUXCLOSCCASAFO_DRV_DATIN0_INDEX + 3u);

    if(true != mcuxClCore_assertEqual((const uint8_t *)pXorWrOut, (const uint8_t *)message, sizeof(message)))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }

    mcuxClOsccaSafo_Drv_setControl2(ctrl2Backup);
    return MCUXCLEXAMPLE_STATUS_OK;
}


/* Cover test case for mcuxClOsccaSafo_Drv_incrementData and mcuxClOsccaSafo_Drv_dataOut_res */
static void sm4Ctr_test(const uint32_t *sm4_key, const uint32_t *pIv, const uint32_t *input, uint32_t inSize, uint32_t *pOut, uint32_t* outSize)
{
    /* Flush SAFO key registers */
    mcuxClOsccaSafo_Drv_enableFlush(MCUXCLOSCCASAFO_DRV_FLUSH_KEY);
    mcuxClOsccaSafo_Drv_init(0U);
    *outSize = 0u;
    mcuxClOsccaSafo_Drv_wait();

    (void)mcuxClOsccaSafo_Drv_setByteOrder(MCUXCLOSCCASAFO_DRV_BYTE_ORDER_LE);
    mcuxClOsccaSafo_Drv_loadKey(0u, sm4_key[3]);
    mcuxClOsccaSafo_Drv_loadKey(1u, sm4_key[2]);
    mcuxClOsccaSafo_Drv_loadKey(2u, sm4_key[1]);
    mcuxClOsccaSafo_Drv_loadKey(3u, sm4_key[0]);

    mcuxClOsccaSafo_Drv_load(MCUXCLOSCCASAFO_DRV_DATIN1_INDEX + 0u, pIv[3]);  /* load the IV in DATIN1 */
    mcuxClOsccaSafo_Drv_load(MCUXCLOSCCASAFO_DRV_DATIN1_INDEX + 1u, pIv[2]);  /* load the IV in DATIN1 */
    mcuxClOsccaSafo_Drv_load(MCUXCLOSCCASAFO_DRV_DATIN1_INDEX + 2u, pIv[1]);  /* load the IV in DATIN1 */
    mcuxClOsccaSafo_Drv_load(MCUXCLOSCCASAFO_DRV_DATIN1_INDEX + 3u, pIv[0]);  /* load the IV in DATIN1 */

    uint32_t remainingBytes = inSize;
    while(remainingBytes >= 16u)
    {
        //Process the first block, which may be smaller than 16u
        //Copy input to SAFO
        mcuxClOsccaSafo_Drv_load(MCUXCLOSCCASAFO_DRV_DATIN0_INDEX + 0u, input[3]);  /* load the IV in DATIN1 */
        mcuxClOsccaSafo_Drv_load(MCUXCLOSCCASAFO_DRV_DATIN0_INDEX + 1u, input[2]);  /* load the IV in DATIN1 */
        mcuxClOsccaSafo_Drv_load(MCUXCLOSCCASAFO_DRV_DATIN0_INDEX + 2u, input[1]);  /* load the IV in DATIN1 */
        mcuxClOsccaSafo_Drv_load(MCUXCLOSCCASAFO_DRV_DATIN0_INDEX + 3u, input[0]);  /* load the IV in DATIN1 */

        /* Keep track of the input bytes that are already copied */
        input += 16u / sizeof(uint32_t);

        //start_up
        (void)mcuxClOsccaSafo_Drv_dataOut_res(MCUXCLOSCCASAFO_DRV_CTRL_END_UP |
                       MCUXCLOSCCASAFO_DRV_CTRL_ENC    |
                       MCUXCLOSCCASAFO_DRV_CTRL_INSEL_DATIN1 |
                       MCUXCLOSCCASAFO_DRV_CTRL_OUTSEL_RES_XOR_DATIN0 |
                       MCUXCLOSCCASAFO_DRV_CTRL_INKEYSEL_KEY0 |
                       MCUXCLOSCCASAFO_DRV_CTRL_SM4_EN         |         // enable SM4 kernel
                       MCUXCLOSCCASAFO_DRV_CTRL_SM4            |         // SM4 operation
                       MCUXCLOSCCASAFO_DRV_CTRL_START);
        //Increase counter value
        mcuxClOsccaSafo_Drv_incrementData(MCUXCLOSCCASAFO_DRV_DATIN1_INDEX, 16u);

        //wait for finish
        mcuxClOsccaSafo_Drv_wait();

        //Copy result to user
        pOut[3] = mcuxClOsccaSafo_Drv_storeInput(MCUXCLOSCCASAFO_DRV_DATOUT_INDEX + 0u);
        pOut[2] = mcuxClOsccaSafo_Drv_storeInput(MCUXCLOSCCASAFO_DRV_DATOUT_INDEX + 1u);
        pOut[1] = mcuxClOsccaSafo_Drv_storeInput(MCUXCLOSCCASAFO_DRV_DATOUT_INDEX + 2u);
        pOut[0] = mcuxClOsccaSafo_Drv_storeInput(MCUXCLOSCCASAFO_DRV_DATOUT_INDEX + 3u);
        pOut += 16u / sizeof(uint32_t);
        MCUX_CSSL_ANALYSIS_START_SUPPRESS_INTEGER_OVERFLOW("*outSize will be increased by a maximum total of inSize.")
        *outSize += 16u;
        MCUX_CSSL_ANALYSIS_STOP_SUPPRESS_INTEGER_OVERFLOW()
        remainingBytes -= 16u;
    }

    /*
     * Iterate over remaining blocks. Here, every block is assumed to be of full size
     */
    if(remainingBytes > 0u)
    {
        uint32_t pPaddingBuf[4] = {0u};
        uint32_t pPaddingOut[4] = {0u};
        /* Copy the padding to the output and update pOutLength accordingly. */
        (void)mcuxClMemory_copy((uint8_t *)pPaddingBuf, (const uint8_t *)input, remainingBytes, remainingBytes);
        //Copy input to SAFO
        mcuxClOsccaSafo_Drv_load(MCUXCLOSCCASAFO_DRV_DATIN0_INDEX + 0u, pPaddingBuf[3]);  /* load the IV in DATIN1 */
        mcuxClOsccaSafo_Drv_load(MCUXCLOSCCASAFO_DRV_DATIN0_INDEX + 1u, pPaddingBuf[2]);  /* load the IV in DATIN1 */
        mcuxClOsccaSafo_Drv_load(MCUXCLOSCCASAFO_DRV_DATIN0_INDEX + 2u, pPaddingBuf[1]);  /* load the IV in DATIN1 */
        mcuxClOsccaSafo_Drv_load(MCUXCLOSCCASAFO_DRV_DATIN0_INDEX + 3u, pPaddingBuf[0]);  /* load the IV in DATIN1 */

        (void)mcuxClOsccaSafo_Drv_dataOut_res(MCUXCLOSCCASAFO_DRV_CTRL_END_UP |
                       MCUXCLOSCCASAFO_DRV_CTRL_ENC    |
                       MCUXCLOSCCASAFO_DRV_CTRL_INSEL_DATIN1 |
                       MCUXCLOSCCASAFO_DRV_CTRL_OUTSEL_RES_XOR_DATIN0 |
                       MCUXCLOSCCASAFO_DRV_CTRL_INKEYSEL_KEY0 |
                       MCUXCLOSCCASAFO_DRV_CTRL_SM4_EN         |         // enable SM4 kernel
                       MCUXCLOSCCASAFO_DRV_CTRL_SM4            |         // SM4 operation
                       MCUXCLOSCCASAFO_DRV_CTRL_START);
        //wait for finish
        mcuxClOsccaSafo_Drv_wait();

        //Copy result to user
        pPaddingOut[3] = mcuxClOsccaSafo_Drv_storeInput(MCUXCLOSCCASAFO_DRV_DATOUT_INDEX + 0u);
        pPaddingOut[2] = mcuxClOsccaSafo_Drv_storeInput(MCUXCLOSCCASAFO_DRV_DATOUT_INDEX + 1u);
        pPaddingOut[1] = mcuxClOsccaSafo_Drv_storeInput(MCUXCLOSCCASAFO_DRV_DATOUT_INDEX + 2u);
        pPaddingOut[0] = mcuxClOsccaSafo_Drv_storeInput(MCUXCLOSCCASAFO_DRV_DATOUT_INDEX + 3u);
        /* Copy the padding to the output and update pOutLength accordingly. */
        (void)mcuxClMemory_copy((uint8_t *)pOut, (const uint8_t *)pPaddingOut, remainingBytes, remainingBytes);
        MCUX_CSSL_ANALYSIS_START_SUPPRESS_INTEGER_OVERFLOW("*outSize will have been increased by exactly inSize after this operation - it cannot overflow.")
        *outSize += remainingBytes;
        MCUX_CSSL_ANALYSIS_STOP_SUPPRESS_INTEGER_OVERFLOW()
    }
}

/* Cover test case for mcuxClOsccaSafo_Drv_enableOutputToKey and mcuxClOsccaSafo_Drv_disableOutputToKey */
/*
 * decrypt = 0 -> perform encrypt operation
 * decrypt = 1 -> perform decrypt operation
 */
static void sm4_Operation(uint8_t decrypt, const uint32_t *input, const uint32_t *sm4_key, uint32_t *result)
{
    /* Flush SAFO key registers */
    mcuxClOsccaSafo_Drv_enableFlush(MCUXCLOSCCASAFO_DRV_FLUSH_KEY);
    mcuxClOsccaSafo_Drv_init(0U);
    mcuxClOsccaSafo_Drv_wait();

    /* load plaintext/ciphertext */
    mcuxClOsccaSafo_Drv_load(0U, input[3]);
    mcuxClOsccaSafo_Drv_load(1U, input[2]);
    mcuxClOsccaSafo_Drv_load(2U, input[1]);
    mcuxClOsccaSafo_Drv_load(3U, input[0]);

    /* load key */
    mcuxClOsccaSafo_Drv_loadKey(0U, sm4_key[3]);
    mcuxClOsccaSafo_Drv_loadKey(1U, sm4_key[2]);
    mcuxClOsccaSafo_Drv_loadKey(2U, sm4_key[1]);
    mcuxClOsccaSafo_Drv_loadKey(3U, sm4_key[0]);

    (void) mcuxClOsccaSafo_Drv_enableOutputToKey(MCUXCLOSCCASAFO_DRV_KEY2_INDEX);

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
    result[3] = mcuxClOsccaSafo_Drv_storeKey(0U);
    result[2] = mcuxClOsccaSafo_Drv_storeKey(1U);
    result[1] = mcuxClOsccaSafo_Drv_storeKey(2U);
    result[0] = mcuxClOsccaSafo_Drv_storeKey(3U);

    mcuxClOsccaSafo_Drv_disableOutputToKey();
}
#endif /* MCUXCL_FEATURE_HW_SAFO_SM4 */

#ifdef MCUXCL_FEATURE_HW_SAFO_SM3
static void Load_Partial_Hash(uint32_t *partial_hash)
{
    /*
    * SM3_EN = 1'b1
    * SM3_STOP = 1'b0
    * HASH_RELOAD = 1'b0
    * SM3_HIGH_LIM = 4'b1111 (SM3 FIFO high limit)
    * SM3_LOW_LIM  = 4'b0000 (SM3 FIFO low limit)
    * HASH_RELOAD = 1'b1
    * NO_AUTO_INIT = 1'b1
    * SM3_MODE = 1'b1 (SM3 automatic mode)
    */

    mcuxClOsccaSafo_Drv_configureSm3(MCUXCLOSCCASAFO_DRV_CONFIG_SM3_AUTOMODE_LOADDATA_USELOADEDIV);
    mcuxClOsccaSafo_Drv_enableHashReload();

    /*
    * START = 1'b1
    * CRYPTO_OP = 3'b111
    * DATOUT_RES = 2'b00 - END_UP
    */

    mcuxClOsccaSafo_Drv_start(MCUXCLOSCCASAFO_DRV_START_SM3);

    (void)mcuxClOsccaSafo_Drv_setByteOrder(MCUXCLOSCCASAFO_DRV_BYTE_ORDER_BE);
    for(int i = 7; i >= 0; i--)
    {
        mcuxClOsccaSafo_Drv_loadFifo(partial_hash[i]);
    }

    /* set SM3 control SFRs to stop the AUTO mode (SM3_STOP = 1'b1) */
    mcuxClOsccaSafo_Drv_stopSm3();

    /* wait for SM3 operation to complete
     * (poll for SAFO_STATUS.BUSY)
     */
    mcuxClOsccaSafo_Drv_wait();
}

static void sm3_Operation_Auto_Mode(const uint32_t *msg, uint32_t msg_size_words, uint32_t *result_digest, bool partial_hash_reload)
{
    /* setup SM3 control_sm3 SFRs */
    /*
    * SM3_EN = 1'b1
    * SM3_STOP = 1'b0
    * HASH_RELOAD = 1'b0
    * SM3_HIGH_LIM = 4'b1111 (SM3 FIFO high limit)
    * SM3_LOW_LIM  = 4'b0000 (SM3 FIFO low limit)
    * SM3_MODE = 1'b1 (SM3 automatic mode)
    */
    mcuxClOsccaSafo_Drv_configureSm3(MCUXCLOSCCASAFO_DRV_CONFIG_SM3_AUTOMODE_LOADDATA_USELOADEDIV);

    if (partial_hash_reload)
    {
        /* NO_AUTO_INIT = 1'b1 (no SM3 automatic HASH initialisation) */
        mcuxClOsccaSafo_Drv_disableIvAutoInit();
    }
    else
    {
        /* NO_AUTO_INIT = 1'b0 (SM3 automatic HASH initialisation) */
        mcuxClOsccaSafo_Drv_enableIvAutoInit();
    }

    /* setup SAFO control SFRs */
    /* DATOUT_RES = 2'b00 - END_UP (load to DATOUT the SM3 result at the end of the current operation)
    * CRYPTO_OP = 3'b111 - SM3
    * START = 1'b1
    */
    mcuxClOsccaSafo_Drv_start(MCUXCLOSCCASAFO_DRV_START_SM3);

    /* load message blocks into SAFO_SM3_FIFO SFRs */
    for (uint32_t i = 0; i < msg_size_words; i++)
    {
        mcuxClOsccaSafo_Drv_loadFifo(msg[i]);
    }

    /* set SM3 control SFRs to stop the AUTO mode */
    mcuxClOsccaSafo_Drv_stopSm3();

    /* wait for SM3 operation to complete
     * (poll for SAFO_STATUS.BUSY)
     */
    mcuxClOsccaSafo_Drv_wait();

    /* read first bank(16 bytes) from the hash result */
    result_digest[0] = mcuxClOsccaSafo_Drv_store(0U);
    result_digest[1] = mcuxClOsccaSafo_Drv_store(1U);
    result_digest[2] = mcuxClOsccaSafo_Drv_store(2U);
    result_digest[3] = mcuxClOsccaSafo_Drv_store(3U);

    /* setup SAFO control SFRs */
    /*
     * DATOUT_RES = 2'b10 - TRIGGER_UP  (transfer result contents to DATOUT)
     * START = 1'b1
     */
    mcuxClOsccaSafo_Drv_triggerOutput();

    /* wait for SM3 operation to complete
     * (poll for SAFO_STATUS.BUSY)
     */
    mcuxClOsccaSafo_Drv_wait();

    /* read second bank(16 bytes) from the hash result */
    result_digest[4] = mcuxClOsccaSafo_Drv_store(0U);
    result_digest[5] = mcuxClOsccaSafo_Drv_store(1U);
    result_digest[6] = mcuxClOsccaSafo_Drv_store(2U);
    result_digest[7] = mcuxClOsccaSafo_Drv_store(3U);
}
#endif /* MCUXCL_FEATURE_HW_SAFO_SM3 */

/* Automatic mode (AUTO - the number of processed blocks is determined during the operation based on the amount of data written into the SM3 FIFO) */
/* Steps for executing SM3 hash operation:
 * - setup SM3 control SFRs
 * - setup SAFO control SFRs
 * - load all message blocks into SM4_FIFO SFRs
 * - set SM3 control SFRs to stop the AUTO mode
 * - wait for SM3 operation to complete (via pooling busy)
 * - read the hash result
 */
MCUXCLEXAMPLE_FUNCTION(mcuxClOsccaSafo_Cover_Public_apis_example)
{
    mcuxClOsccaSafo_Drv_enableFlush(MCUXCLOSCCASAFO_DRV_FLUSH_ALL);
    mcuxClOsccaSafo_Drv_init(MCUXCLOSCCASAFO_DRV_BYTE_ORDER_BE);

#ifdef MCUXCL_FEATURE_HW_SAFO_SM4
    /******************************************************************************************************************/
    /******************************************************************************************************************/
    if(true != xorWr_test())
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    if(mcuxClOsccaSafo_Drv_isStatusError())
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }

    /******************************************************************************************************************/
    /******************************************************************************************************************/
    /*
    SM4 Encrypt test vector (1 block message)
    Input:
        message = 0123456789abcdeffedcba9876543210
        key = 0123456789abcdeffedcba9876543210

    Output (encrypt operation):
        ciphertext = 681edf34d206965e86b3e94f536e4246
    */
    uint32_t result_ciphertext[4U] = {0U};

    sm4_Operation(0x00, plaintext, key, result_ciphertext);
    if(mcuxClOsccaSafo_Drv_isStatusError())
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    /* check if actual result is equal to expected result */
    if (true != mcuxClCore_assertEqual((const uint8_t *)result_ciphertext, (const uint8_t *)reference_ciphertext, sizeof(reference_ciphertext)))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }

    /******************************************************************************************************************/
    /******************************************************************************************************************/
    /*
    SM4 Ctr test vector (4 block message)
    Input:
        message = AAAAAAAAAAAAAAAABBBBBBBBBBBBBBBB CCCCCCCCCCCCCCCCDDDDDDDDDDDDDDDD
                  EEEEEEEEEEEEEEEEFFFFFFFFFFFFFFFF AAAAAAAAAAAAAAAABBBBBBBBBBBBBBBB
        key = 0123456789abcdeffedcba9876543210
        iv = 000102030405060708090A0B0C0D0E0F

    Output (encrypt operation):
        ciphertext = AC3236CB970CC20791364C395A1342D1 A3CBC1878C6F30CD074CCE385CDD70C7
                     F234BC0E24C11980FD1286310CE37B92 6E02FCD0FAA0BAF38B2933851D824514
    */
    uint32_t cipherCtrtext[4U] = {0U};
    uint32_t outLen = 0u;

    MCUX_CSSL_ANALYSIS_START_SUPPRESS_POINTER_CASTING("sm4CtrKey, sm4CtrIv, sm4CtrPtxt are aligned using the ALIGNED keyword to facilitate example function")
    sm4Ctr_test((const uint32_t *)sm4CtrKey, (const uint32_t *)sm4CtrIv, (const uint32_t *)sm4CtrPtxt, sizeof(sm4CtrPtxt), cipherCtrtext, &outLen);
    MCUX_CSSL_ANALYSIS_STOP_SUPPRESS_POINTER_CASTING()
    if(mcuxClOsccaSafo_Drv_isStatusError())
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    /* check if actual result is equal to expected result */
    if (true != mcuxClCore_assertEqual((const uint8_t *)cipherCtrtext, (const uint8_t *)sm4CtrCtxt, outLen))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }
#endif /* MCUXCL_FEATURE_HW_SAFO_SM4 */
#ifdef MCUXCL_FEATURE_HW_SAFO_SM3
    /******************************************************************************************************************/
    /******************************************************************************************************************/
    /* SM3 Automatic mode, partial processing (4 padded blocks).
     * Input:
     *  message        = "64fce814fa17cecf9a97c6a815183f0db881d3367eb900247d997ee027a25ed2aac0a62f0718227dd6e82f17e6f563011945d3e58002e5c5d0dc66e29b55c71cde0d6d87cd211331056b122d069c556210d29e62dfdaca2587fe07e1635bc44fd07bb0990e6af75c9b1f0139a117ef5639ab73c5f7f7793bb2277b9749af279bf722b9c84a786f129e441112f184a9fe745cd390d4f4dadc773c31d089c39c2eb610dac973bd5e3f13b14bf525b43dd0c8591380b042464782e6d4b8336abcda"
     *  message_padded = "64fce814fa17cecf9a97c6a815183f0db881d3367eb900247d997ee027a25ed2aac0a62f0718227dd6e82f17e6f563011945d3e58002e5c5d0dc66e29b55c71cde0d6d87cd211331056b122d069c556210d29e62dfdaca2587fe07e1635bc44fd07bb0990e6af75c9b1f0139a117ef5639ab73c5f7f7793bb2277b9749af279bf722b9c84a786f129e441112f184a9fe745cd390d4f4dadc773c31d089c39c2eb610dac973bd5e3f13b14bf525b43dd0c8591380b042464782e6d4b8336abcda80000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000600"
     *  (192 bytes = 3 SM3 blocks unpadded)
     * Output:
     *  sm3_hash = "e2aa7ce51e675e623fa93ce7d5baecde19b32dfc9165d94284adcfc15cf5619a"
     */
    mcuxClOsccaSafo_Drv_enableFlush(MCUXCLOSCCASAFO_DRV_FLUSH_ALL);
    mcuxClOsccaSafo_Drv_init(MCUXCLOSCCASAFO_DRV_BYTE_ORDER_BE);
    uint32_t result_digest[8U];
    /* SM3 Automatic mode, partial hash processing (load the partial HASH value while SAFO_SM3_CTRL.HASH_RELOAD is set to 1'b1) */
    for (uint8_t i = 0U; i < 4U; i++)
    {
        bool partial_hash_reload = (i != 0U);
        sm3_Operation_Auto_Mode(&message_hash[i * 16U], 16U, result_digest, partial_hash_reload); /* process one SM3 block (64 bytes) per call */
        if (i != 3U)
        {
            Load_Partial_Hash(result_digest);
        }
    }
    if(mcuxClOsccaSafo_Drv_isStatusError())
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    /* check if actual result is equal to expected result */
    for (uint32_t word = 0U; word < 8U; word++)
    {
        if (reference_hash_hash[word] != result_digest[word])
        {
            return MCUXCLEXAMPLE_STATUS_ERROR;
        }
    }
#endif /* MCUXCL_FEATURE_HW_SAFO_SM3 */
    uint32_t closeRet = mcuxClOsccaSafo_Drv_close();
    if(MCUXCLOSCCASAFO_STATUS_ERROR == closeRet)
    {
        /* If error flush whole SAFO */
        mcuxClOsccaSafo_Drv_enableFlush(MCUXCLOSCCASAFO_DRV_FLUSH_ALL);
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    return MCUXCLEXAMPLE_STATUS_OK;
}
