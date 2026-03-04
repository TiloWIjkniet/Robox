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

static const uint32_t message_1[16U] = {
    0x61626380u, 0x00000000u,
    0x00000000u, 0x00000000u,
    0x00000000u, 0x00000000u,
    0x00000000u, 0x00000000u,
    0x00000000u, 0x00000000u,
    0x00000000u, 0x00000000u,
    0x00000000u, 0x00000000u,
    0x00000000u, 0x00000018u
};
static const uint32_t reference_hash_1[8U] = {
    0x8f4ba8e0u, 0x297da02bu, 0x5cf2f7a2u, 0x4167c487u,
    0xdc10e4e2u, 0xd1f2d46bu, 0x62eeedd9u, 0x66c7f0f4u
};

static const uint32_t message_2[64U] = {
    0x64fce814u, 0xfa17cecfu, 0x9a97c6a8u, 0x15183f0du, 0xb881d336u, 0x7eb90024u, 0x7d997ee0u, 0x27a25ed2u, 0xaac0a62fu, 0x0718227du,
    0xd6e82f17u, 0xe6f56301u, 0x1945d3e5u, 0x8002e5c5u, 0xd0dc66e2u, 0x9b55c71cu, 0xde0d6d87u, 0xcd211331u, 0x056b122du, 0x069c5562u,
    0x10d29e62u, 0xdfdaca25u, 0x87fe07e1u, 0x635bc44fu, 0xd07bb099u, 0x0e6af75cu, 0x9b1f0139u, 0xa117ef56u, 0x39ab73c5u, 0xf7f7793bu,
    0xb2277b97u, 0x49af279bu, 0xf722b9c8u, 0x4a786f12u, 0x9e441112u, 0xf184a9feu, 0x745cd390u, 0xd4f4dadcu, 0x773c31d0u, 0x89c39c2eu,
    0xb610dac9u, 0x73bd5e3fu, 0x13b14bf5u, 0x25b43dd0u, 0xc8591380u, 0xb0424647u, 0x82e6d4b8u, 0x336abcdau, 0x80000000u, 0x00000000u,
    0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
    0x00000000u, 0x00000000u, 0x00000000u, 0x00000600u
};
static const uint32_t reference_hash_2[8U] = {
    0x5cf5619au, 0x84adcfc1u, 0x9165d942u, 0x19b32dfcu, 0xd5baecdeu, 0x3fa93ce7u, 0x1e675e62u, 0xe2aa7ce5u
};

static const uint32_t message_3[176U] = {
    0xddf39dd9u, 0xae5da9b0u, 0xf63c9363u, 0x73517479u, 0x3bdea659u, 0x664fad24u, 0xd9dc7a1du, 0x13020b98u, 0xf9795be4u, 0x3ab22f0du,
    0x6dd5512cu, 0x8ef0fcfcu, 0x8e04d67cu, 0x7d547de6u, 0xbbd3e42cu, 0xa9c0cbcdu, 0x5c1911ebu, 0xa08c422fu, 0xbe61d18eu, 0x3383b88bu,
    0xf6cae616u, 0xd6777494u, 0x682bf407u, 0x9692fbe4u, 0x5eb6b789u, 0xdcda4ba0u, 0x67bdc7aau, 0xbc6ec1abu, 0xc2c23449u, 0xc41002ecu,
    0x8eca4259u, 0xa5adff55u, 0x819ddad4u, 0xa9a6a40bu, 0x4733a39bu, 0xb7ab2ff7u, 0x37bc2b6bu, 0x97fc2a3bu, 0x1f92f768u, 0x44f66a3fu,
    0x02927da9u, 0x0c4d6239u, 0x8da51bcau, 0x0740335du, 0x767b3030u, 0x846ef03bu, 0xa8021667u, 0xeb638ad4u, 0xc97756a4u, 0xb482cdfdu,
    0x1fe94fbdu, 0xdab1a577u, 0xf3de673cu, 0x994b2ec9u, 0x60e01031u, 0x3df9c681u, 0x68b8bf13u, 0x2a286368u, 0x0b5517b0u, 0xd7a619c6u,
    0x6b8b2396u, 0xf564898du, 0x349ee1a5u, 0xe43243c0u, 0xa678b960u, 0xa123fcf5u, 0x1838cf35u, 0xfe9115ffu, 0x5b9c9499u, 0xba9c7b92u,
    0x91fed3b7u, 0x1f99733eu, 0x7a80f926u, 0xfe42e20cu, 0xf7140f96u, 0x67c23c37u, 0x365c73cau, 0x9b5278b5u, 0x77b584d3u, 0x439f0f25u,
    0x07534fa6u, 0x3a704682u, 0x3d350918u, 0x4e2f084au, 0x047d7d83u, 0xf7028be1u, 0x0fe09329u, 0xcbe27b00u, 0xfb260f7cu, 0x628ff4d1u,
    0xbc135a7cu, 0xd68543f1u, 0x85961abcu, 0x8924fddau, 0xb89960bfu, 0x5238de7fu, 0x11030b11u, 0xec9f9f70u, 0xf373f7bau, 0x07c80888u,
    0x62168488u, 0xcf42703bu, 0xe311d072u, 0x88684482u, 0x28718ebeu, 0x661d535fu, 0xaa3d9990u, 0xae210488u, 0xbe3c7c5fu, 0x59ca3eacu,
    0x384cba3bu, 0xe3d4719au, 0x66cb6056u, 0x865178ccu, 0xaab98e29u, 0xa6e61b45u, 0xf37a94deu, 0xcb1c32b0u, 0x65529429u, 0x1de43844u,
    0x6c7405e4u, 0x69be23e8u, 0x7254e934u, 0xf8fa1141u, 0xc59be4a6u, 0xf7350e7bu, 0x72b3b025u, 0x12715663u, 0xf7fddcf6u, 0x5263005du,
    0x954c2e58u, 0x88a09eb3u, 0xc7d38046u, 0x8c888f25u, 0x192b6020u, 0xb405f087u, 0xca5058ecu, 0x1dfce096u, 0xf67e3a6fu, 0xb7f33cc6u,
    0xeb82c906u, 0xb6a38e74u, 0xfdb1e842u, 0x5f9e168bu, 0xa7848c03u, 0x7fe036e2u, 0x0a6adcf3u, 0x558dce26u, 0x462b03c3u, 0x8a53afdcu,
    0x0309956du, 0x62e60ee7u, 0x89b63740u, 0x3c74e872u, 0x256d9203u, 0x5f4f7bfcu, 0x720f5699u, 0x4ee76628u, 0x68685db8u, 0x45da2561u,
    0x80000000u, 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
    0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u, 0x00001400u
};
static const uint32_t reference_hash_3[8U] = {
    0xfd3366d1u, 0x7f9ba57au, 0xf93938b7u, 0x30169619u, 0x8ce61e2cu, 0x491c4ef3u, 0x7f866b84u, 0xbe271e39u
};

static const uint32_t message_4[64U] = {
    0x64fce814u, 0xfa17cecfu, 0x9a97c6a8u, 0x15183f0du, 0xb881d336u, 0x7eb90024u, 0x7d997ee0u, 0x27a25ed2u, 0xaac0a62fu, 0x0718227du,
    0xd6e82f17u, 0xe6f56301u, 0x1945d3e5u, 0x8002e5c5u, 0xd0dc66e2u, 0x9b55c71cu, 0xde0d6d87u, 0xcd211331u, 0x056b122du, 0x069c5562u,
    0x10d29e62u, 0xdfdaca25u, 0x87fe07e1u, 0x635bc44fu, 0xd07bb099u, 0x0e6af75cu, 0x9b1f0139u, 0xa117ef56u, 0x39ab73c5u, 0xf7f7793bu,
    0xb2277b97u, 0x49af279bu, 0xf722b9c8u, 0x4a786f12u, 0x9e441112u, 0xf184a9feu, 0x745cd390u, 0xd4f4dadcu, 0x773c31d0u, 0x89c39c2eu,
    0xb610dac9u, 0x73bd5e3fu, 0x13b14bf5u, 0x25b43dd0u, 0xc8591380u, 0xb0424647u, 0x82e6d4b8u, 0x336abcdau, 0x80000000u, 0x00000000u,
    0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
    0x00000000u, 0x00000000u, 0x00000000u, 0x00000600u
};
static const uint32_t reference_hash_4[8U] = {
    0x5cf5619au, 0x84adcfc1u, 0x9165d942u, 0x19b32dfcu, 0xd5baecdeu, 0x3fa93ce7u, 0x1e675e62u, 0xe2aa7ce5u
};

static const uint32_t message_5[144U] = {
    0xe14bc03eu, 0x9d3f1e0au, 0x7d67ac8du, 0x69d79ca1u, 0xa0d1da48u, 0xb0a97f3cu, 0xab6e0c91u, 0x5207236du, 0xd77d5064u, 0xb5029523u,
    0xb8541a6du, 0x23e94967u, 0x839ae5c4u, 0x528eb17au, 0x6f786721u, 0x65d5f25fu, 0x15e3b8a5u, 0x785776ecu, 0xeb945ef8u, 0x4af9647fu,
    0xd5ea6106u, 0xcee57ddfu, 0x8ce70e98u, 0xeaf0ac8cu, 0x688adad9u, 0x79d3dedbu, 0xda991dabu, 0x69a65c04u, 0xd5c8c7a9u, 0xc8b3c5d4u,
    0xa36fad20u, 0x59ab1359u, 0x08e146bcu, 0xb65d4240u, 0x2e540195u, 0x4b50b8b6u, 0xf22ee682u, 0x34c7596eu, 0x81dd7dd7u, 0x8f046f1bu,
    0x751fed74u, 0x2bd3825cu, 0x209571c6u, 0xf3db93f1u, 0xe5621a50u, 0xb75840a3u, 0xd7683e48u, 0x40400e92u, 0xadf20de7u, 0x9427cb40u,
    0x4555fffau, 0x951f8a8du, 0xdced49bfu, 0x607eb1d3u, 0xbcc4b3bbu, 0x9fcb0fd0u, 0x92ccfe06u, 0xee7d3a58u, 0xd27fbb65u, 0xfb5a951bu,
    0xee9bb5b1u, 0x0a56d1e0u, 0xa029a767u, 0x4999ef8eu, 0x44999117u, 0x04a69dd9u, 0xfb3db965u, 0x04d792acu, 0x8cdc58a3u, 0xbe385a2eu,
    0x1c59ba88u, 0xa6f6fa7eu, 0xd322c94fu, 0xc516b686u, 0x53c1444fu, 0x1e148be1u, 0x1287ee09u, 0x6a32b847u, 0x1c295efcu, 0x0319bab9u,
    0x0f5f5345u, 0x55abe724u, 0x9a5a1263u, 0x5aa4519fu, 0xef447474u, 0x0927509fu, 0x72eea9a4u, 0x6de4aef3u, 0xb3a579bau, 0x9fe091c5u,
    0xc8a12f14u, 0xd07c0fe1u, 0xf621f113u, 0x0ada1465u, 0xe991337du, 0xb24e6996u, 0x232fcf0cu, 0x79843321u, 0x71c68071u, 0xb8969cd6u,
    0x0f0bf81au, 0xc0b732ffu, 0x17e6b3d9u, 0x796c1bd0u, 0xfd14c87du, 0xacc955d9u, 0x65bf4a14u, 0xbf75b464u, 0xbfd38323u, 0xf09a16beu,
    0x545f1267u, 0x0c1cceffu, 0x11a9281du, 0x3b5d421du, 0x72c1a3b9u, 0xa5e98100u, 0x287b58fdu, 0x1d749b77u, 0x2bf8ca08u, 0x05e963c7u,
    0x462dda32u, 0xb2942a41u, 0x083f196eu, 0x1055c8c9u, 0xc11efe6eu, 0x90c83e4bu, 0x8ef328c1u, 0x60daabeeu, 0x80000000u, 0x00000000u,
    0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
    0x00000000u, 0x00000000u, 0x00000000u, 0x00001000u
};
static const uint32_t reference_hash_5[8U] = {
    0x614fba70u, 0x8ba8c2ffu, 0x1b2d715cu, 0xeea19bd5u, 0xe2e479a2u, 0x1aae6e37u, 0x52ec85e6u, 0xd846ff2eu
};

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

    mcuxClOsccaSafo_Drv_configureSm3(MCUXCLOSCCASAFO_DRV_CONFIG_SM3_AUTOMODE_LOADIV);

    /*
    * START = 1'b1
    * CRYPTO_OP = 3'b111
    * DATOUT_RES = 2'b00 - END_UP
    */

    mcuxClOsccaSafo_Drv_start(MCUXCLOSCCASAFO_DRV_START_SM3);

    (void) mcuxClOsccaSafo_Drv_setByteOrder(MCUXCLOSCCASAFO_DRV_BYTE_ORDER_BE);
    for(int i = 7; i >= 0; i--)
    {
        mcuxClOsccaSafo_Drv_loadFifo(partial_hash[i]);
    }

    /* set SM3 control SFRs to stop the AUTO mode (SM3_STOP = 1'b1) */
    mcuxClOsccaSafo_Drv_stopSm3();

    /* wait for SM3 operation to complete
     * (poll for SGI_STATUS.BUSY)
     */
    mcuxClOsccaSafo_Drv_wait();
}


static void SM3_Operation_Auto_Mode(const uint32_t *message, uint32_t message_size_words, uint32_t *result_digest, bool partial_hash_reload)
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

    /* setup SGI control SFRs */
    /* DATOUT_RES = 2'b00 - END_UP (load to DATOUT the SM3 result at the end of the current operation)
    * CRYPTO_OP = 3'b111 - SM3
    * START = 1'b1
    */
    mcuxClOsccaSafo_Drv_start(MCUXCLOSCCASAFO_DRV_START_SM3);

    /* load message blocks into SGI_SM3_FIFO SFRs */
    for (uint32_t i = 0U; i < message_size_words; i++)
    {
        mcuxClOsccaSafo_Drv_loadFifo(message[i]);
    }

    /* set SM3 control SFRs to stop the AUTO mode */
    mcuxClOsccaSafo_Drv_stopSm3();

    /* wait for SM3 operation to complete
     * (poll for SGI_STATUS.BUSY)
     */
    mcuxClOsccaSafo_Drv_wait();

    /* read first bank(16 bytes) from the hash result */
    result_digest[0] = mcuxClOsccaSafo_Drv_store(0U);
    result_digest[1] = mcuxClOsccaSafo_Drv_store(1U);
    result_digest[2] = mcuxClOsccaSafo_Drv_store(2U);
    result_digest[3] = mcuxClOsccaSafo_Drv_store(3U);

    /* setup SGI control SFRs */
    /*
     * DATOUT_RES = 2'b10 - TRIGGER_UP  (transfer result contents to DATOUT)
     * START = 1'b1
     */
    mcuxClOsccaSafo_Drv_triggerOutput();

    /* wait for SM3 operation to complete
     * (poll for SGI_STATUS.BUSY)
     */
    mcuxClOsccaSafo_Drv_wait();

    /* read second bank(16 bytes) from the hash result */
    result_digest[4] = mcuxClOsccaSafo_Drv_store(0U);
    result_digest[5] = mcuxClOsccaSafo_Drv_store(1U);
    result_digest[6] = mcuxClOsccaSafo_Drv_store(2U);
    result_digest[7] = mcuxClOsccaSafo_Drv_store(3U);
}

/* Automatic mode (AUTO - the number of processed blocks is determined during the operation based on the amount of data written into the SM3 FIFO) */
/* Steps for executing SM3 hash operation:
 * - setup SM3 control SFRs
 * - setup SGI control SFRs
 * - load all message blocks into SM4_FIFO SFRs
 * - set SM3 control SFRs to stop the AUTO mode
 * - wait for SM3 operation to complete (via pooling busy)
 * - read the hash result
 */
MCUXCLEXAMPLE_FUNCTION(mcuxClOsccaSafo_sm3_automode_example)
{

    mcuxClOsccaSafo_Drv_init(MCUXCLOSCCASAFO_DRV_BYTE_ORDER_BE);

    /******************************************************************************************************************/
    /******************************************************************************************************************/
    /* SM3 Automatic mode, one-shot processing (1 padded block)
     * Input:
     *  message         = "616263"
     *  (message_padded = "61626380000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000018", 64 bytes = 1 SM3-block)
     * Output:
     *  sm3_hash        = "66c7f0f462eeedd9d1f2d46bdc10e4e24167c4875cf2f7a2297da02b8f4ba8e0"
     */
    uint32_t result_digest_1[8U];

    /* SM3 Automatic mode, no intermediate hash value */
    SM3_Operation_Auto_Mode(message_1, 16U, result_digest_1, false);
    /* check if actual result is equal to expected result */
    if (!mcuxClCore_assertEqual((const uint8_t *)reference_hash_1, (const uint8_t *)result_digest_1, sizeof(result_digest_1)))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }

    /******************************************************************************************************************/
    /******************************************************************************************************************/
    /* SM3 Automatic mode, one-shot processing (4 padded blocks).
     * Input:
     *  message        = "64fce814fa17cecf9a97c6a815183f0db881d3367eb900247d997ee027a25ed2aac0a62f0718227dd6e82f17e6f563011945d3e58002e5c5d0dc66e29b55c71cde0d6d87cd211331056b122d069c556210d29e62dfdaca2587fe07e1635bc44fd07bb0990e6af75c9b1f0139a117ef5639ab73c5f7f7793bb2277b9749af279bf722b9c84a786f129e441112f184a9fe745cd390d4f4dadc773c31d089c39c2eb610dac973bd5e3f13b14bf525b43dd0c8591380b042464782e6d4b8336abcda"
     *  message_padded = "64fce814fa17cecf9a97c6a815183f0db881d3367eb900247d997ee027a25ed2aac0a62f0718227dd6e82f17e6f563011945d3e58002e5c5d0dc66e29b55c71cde0d6d87cd211331056b122d069c556210d29e62dfdaca2587fe07e1635bc44fd07bb0990e6af75c9b1f0139a117ef5639ab73c5f7f7793bb2277b9749af279bf722b9c84a786f129e441112f184a9fe745cd390d4f4dadc773c31d089c39c2eb610dac973bd5e3f13b14bf525b43dd0c8591380b042464782e6d4b8336abcda80000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000600"
     *  (192 bytes = 3 SM3 blocks unpadded)
     * Output:
     *  sm3_hash = "e2aa7ce51e675e623fa93ce7d5baecde19b32dfc9165d94284adcfc15cf5619a"
     */
    uint32_t result_digest_2[8U];

    /* SM3 Automatic mode, no intermediate hash value */
    SM3_Operation_Auto_Mode(message_2, 64U, result_digest_2, false);
    /* check if actual result is equal to expected result */
    if (!mcuxClCore_assertEqual((const uint8_t *)reference_hash_2, (const uint8_t *)result_digest_2, sizeof(result_digest_2)))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }

    /******************************************************************************************************************/
    /******************************************************************************************************************/
    /* SM3 Automatic mode, one-shot processing (11 padded blocks).
     * Input:
     *  message        = "ddf39dd9ae5da9b0f63c9363735174793bdea659664fad24d9dc7a1d13020b98f9795be43ab22f0d6dd5512c8ef0fcfc8e04d67c7d547de6bbd3e42ca9c0cbcd5c1911eba08c422fbe61d18e3383b88bf6cae616d6777494682bf4079692fbe45eb6b789dcda4ba067bdc7aabc6ec1abc2c23449c41002ec8eca4259a5adff55819ddad4a9a6a40b4733a39bb7ab2ff737bc2b6b97fc2a3b1f92f76844f66a3f02927da90c4d62398da51bca0740335d767b3030846ef03ba8021667eb638ad4c97756a4b482cdfd1fe94fbddab1a577f3de673c994b2ec960e010313df9c68168b8bf132a2863680b5517b0d7a619c66b8b2396f564898d349ee1a5e43243c0a678b960a123fcf51838cf35fe9115ff5b9c9499ba9c7b9291fed3b71f99733e7a80f926fe42e20cf7140f9667c23c37365c73ca9b5278b577b584d3439f0f2507534fa63a7046823d3509184e2f084a047d7d83f7028be10fe09329cbe27b00fb260f7c628ff4d1bc135a7cd68543f185961abc8924fddab89960bf5238de7f11030b11ec9f9f70f373f7ba07c8088862168488cf42703be311d0728868448228718ebe661d535faa3d9990ae210488be3c7c5f59ca3eac384cba3be3d4719a66cb6056865178ccaab98e29a6e61b45f37a94decb1c32b0655294291de438446c7405e469be23e87254e934f8fa1141c59be4a6f7350e7b72b3b02512715663f7fddcf65263005d954c2e5888a09eb3c7d380468c888f25192b6020b405f087ca5058ec1dfce096f67e3a6fb7f33cc6eb82c906b6a38e74fdb1e8425f9e168ba7848c037fe036e20a6adcf3558dce26462b03c38a53afdc0309956d62e60ee789b637403c74e872256d92035f4f7bfc720f56994ee7662868685db845da2561"
     *  message_padded = "ddf39dd9ae5da9b0f63c9363735174793bdea659664fad24d9dc7a1d13020b98f9795be43ab22f0d6dd5512c8ef0fcfc8e04d67c7d547de6bbd3e42ca9c0cbcd5c1911eba08c422fbe61d18e3383b88bf6cae616d6777494682bf4079692fbe45eb6b789dcda4ba067bdc7aabc6ec1abc2c23449c41002ec8eca4259a5adff55819ddad4a9a6a40b4733a39bb7ab2ff737bc2b6b97fc2a3b1f92f76844f66a3f02927da90c4d62398da51bca0740335d767b3030846ef03ba8021667eb638ad4c97756a4b482cdfd1fe94fbddab1a577f3de673c994b2ec960e010313df9c68168b8bf132a2863680b5517b0d7a619c66b8b2396f564898d349ee1a5e43243c0a678b960a123fcf51838cf35fe9115ff5b9c9499ba9c7b9291fed3b71f99733e7a80f926fe42e20cf7140f9667c23c37365c73ca9b5278b577b584d3439f0f2507534fa63a7046823d3509184e2f084a047d7d83f7028be10fe09329cbe27b00fb260f7c628ff4d1bc135a7cd68543f185961abc8924fddab89960bf5238de7f11030b11ec9f9f70f373f7ba07c8088862168488cf42703be311d0728868448228718ebe661d535faa3d9990ae210488be3c7c5f59ca3eac384cba3be3d4719a66cb6056865178ccaab98e29a6e61b45f37a94decb1c32b0655294291de438446c7405e469be23e87254e934f8fa1141c59be4a6f7350e7b72b3b02512715663f7fddcf65263005d954c2e5888a09eb3c7d380468c888f25192b6020b405f087ca5058ec1dfce096f67e3a6fb7f33cc6eb82c906b6a38e74fdb1e8425f9e168ba7848c037fe036e20a6adcf3558dce26462b03c38a53afdc0309956d62e60ee789b637403c74e872256d92035f4f7bfc720f56994ee7662868685db845da256180000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001400"
     *  (640 bytes = 10 SM3 blocks unpadded)
     * Output:
     *  sm3_hash = "be271e397f866b84491c4ef38ce61e2c30169619f93938b77f9ba57afd3366d1"
     */
    uint32_t result_digest_3[8U];

    /* SM3 Automatic mode, no intermediate hash value */
    SM3_Operation_Auto_Mode(message_3, 176U, result_digest_3, false);
    /* check if actual result is equal to expected result */
    if (!mcuxClCore_assertEqual((const uint8_t *)reference_hash_3, (const uint8_t *)result_digest_3, sizeof(result_digest_3)))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }


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
    uint32_t result_digest_4[8U];

    /* SM3 Automatic mode, partial hash processing (load the partial HASH value while SGI_SM3_CTRL.HASH_RELOAD is set to 1'b1) */
    for (uint8_t i = 0U; i < 4U; i++)
    {
        bool partial_hash_reload = (i != 0U) ? true : false;
        SM3_Operation_Auto_Mode(&message_4[i * 16U], 16U, result_digest_4, partial_hash_reload); /* process one SM3 block (64 bytes) per call */
        if (i != 3U)
        {
            Load_Partial_Hash(result_digest_4);
        }
    }
    /* check if actual result is equal to expected result */
    if (!mcuxClCore_assertEqual((const uint8_t *)reference_hash_4, (const uint8_t *)result_digest_4, sizeof(result_digest_4)))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }

    /******************************************************************************************************************/
    /******************************************************************************************************************/
    /* SM3 Automatic mode, partial processing (9 padded blocks).
     * Input:
     *  message        = "e14bc03e9d3f1e0a7d67ac8d69d79ca1a0d1da48b0a97f3cab6e0c915207236dd77d5064b5029523b8541a6d23e94967839ae5c4528eb17a6f78672165d5f25f15e3b8a5785776eceb945ef84af9647fd5ea6106cee57ddf8ce70e98eaf0ac8c688adad979d3dedbda991dab69a65c04d5c8c7a9c8b3c5d4a36fad2059ab135908e146bcb65d42402e5401954b50b8b6f22ee68234c7596e81dd7dd78f046f1b751fed742bd3825c209571c6f3db93f1e5621a50b75840a3d7683e4840400e92adf20de79427cb404555fffa951f8a8ddced49bf607eb1d3bcc4b3bb9fcb0fd092ccfe06ee7d3a58d27fbb65fb5a951bee9bb5b10a56d1e0a029a7674999ef8e4499911704a69dd9fb3db96504d792ac8cdc58a3be385a2e1c59ba88a6f6fa7ed322c94fc516b68653c1444f1e148be11287ee096a32b8471c295efc0319bab90f5f534555abe7249a5a12635aa4519fef4474740927509f72eea9a46de4aef3b3a579ba9fe091c5c8a12f14d07c0fe1f621f1130ada1465e991337db24e6996232fcf0c7984332171c68071b8969cd60f0bf81ac0b732ff17e6b3d9796c1bd0fd14c87dacc955d965bf4a14bf75b464bfd38323f09a16be545f12670c1cceff11a9281d3b5d421d72c1a3b9a5e98100287b58fd1d749b772bf8ca0805e963c7462dda32b2942a41083f196e1055c8c9c11efe6e90c83e4b8ef328c160daabee"
     *  message_padded = "e14bc03e9d3f1e0a7d67ac8d69d79ca1a0d1da48b0a97f3cab6e0c915207236dd77d5064b5029523b8541a6d23e94967839ae5c4528eb17a6f78672165d5f25f15e3b8a5785776eceb945ef84af9647fd5ea6106cee57ddf8ce70e98eaf0ac8c688adad979d3dedbda991dab69a65c04d5c8c7a9c8b3c5d4a36fad2059ab135908e146bcb65d42402e5401954b50b8b6f22ee68234c7596e81dd7dd78f046f1b751fed742bd3825c209571c6f3db93f1e5621a50b75840a3d7683e4840400e92adf20de79427cb404555fffa951f8a8ddced49bf607eb1d3bcc4b3bb9fcb0fd092ccfe06ee7d3a58d27fbb65fb5a951bee9bb5b10a56d1e0a029a7674999ef8e4499911704a69dd9fb3db96504d792ac8cdc58a3be385a2e1c59ba88a6f6fa7ed322c94fc516b68653c1444f1e148be11287ee096a32b8471c295efc0319bab90f5f534555abe7249a5a12635aa4519fef4474740927509f72eea9a46de4aef3b3a579ba9fe091c5c8a12f14d07c0fe1f621f1130ada1465e991337db24e6996232fcf0c7984332171c68071b8969cd60f0bf81ac0b732ff17e6b3d9796c1bd0fd14c87dacc955d965bf4a14bf75b464bfd38323f09a16be545f12670c1cceff11a9281d3b5d421d72c1a3b9a5e98100287b58fd1d749b772bf8ca0805e963c7462dda32b2942a41083f196e1055c8c9c11efe6e90c83e4b8ef328c160daabee80000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001000"
     *  (512 bytes = 8 SM3 blocks unpadded)
     * Output:
     *  sm3_hash = "d846ff2e52ec85e61aae6e37e2e479a2eea19bd51b2d715c8ba8c2ff614fba70"
     */
    uint32_t result_digest_5[8U];

    /* SM3 Automatic mode, partial hash processing (load the partial HASH value while SGI_SM3_CTRL.HASH_RELOAD is set to 1'b1) */
    for (uint8_t i = 0U; i < 3U; i++)
    {
        bool partial_hash_reload = (i != 0U) ? true : false;
        SM3_Operation_Auto_Mode(&message_5[i * 48U], 48U, result_digest_5, partial_hash_reload); /* process 3 SM3 blocks (192 bytes) per call */
        if (i != 2U)
        {
            Load_Partial_Hash(result_digest_5);
        }
    }
    /* check if actual result is equal to expected result */
    if (!mcuxClCore_assertEqual((const uint8_t *)reference_hash_5, (const uint8_t *)result_digest_5, sizeof(result_digest_5)))
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
