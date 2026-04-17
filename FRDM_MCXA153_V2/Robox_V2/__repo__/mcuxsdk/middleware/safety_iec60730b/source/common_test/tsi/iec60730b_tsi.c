/*
 * Copyright 2015 Freescale Semiconductor, Inc.
 * Copyright 2015-2023, 2025 NXP
 *
 * This software is owned or controlled by NXP and may only be used strictly
 * in accordance with the license terms
 * defined in <distribution-root>/IEC60730-LICENSE.txt file.
 * By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you
 * have read, and that you agree to comply with and are bound by,
 * such license terms.  If you do not agree to be bound by the applicable
 * license terms, then you may not retain, install, activate or otherwise
 * use the software.
 *
 * version 5.0
 *
 * @brief TSI test common routines - IEC60730 Class B.
 *
 */

#include "iec60730b.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* TSI v5 */
/* TSI_Register_Accessor_Macros */
typedef struct
{
    uint32_t GENCS; /**< TSI General Control and Status Register, offset: 0x0 */
    uint32_t DATA;  /**< TSI DATA Register, offset: 0x4 */
    uint32_t TSHD;  /**< TSI Threshold Register, offset: 0x8 */
    uint32_t MODE;  /**< TSI MODE Register, offset: 0xC */
    uint32_t MUL0;  /**< TSI MUTUAL-CAP Register 0, offset: 0x10 */
    uint32_t MUL1;  /**< TSI MUTUAL-CAP Register 1, offset: 0x14 */
    uint32_t SINC;  /**< TSI SINC filter Register, offset: 0x18 */
    uint32_t SSC0;  /**< TSI SSC Register 0, offset: 0x1C */
    uint32_t SSC1;  /**< TSI SSC Register 0, offset: 0x20 */
    uint32_t SSC2;  /**< TSI SSC Register 2, offset: 0x24 */
} fs_tsi_v5_t;

/* TSI v6 */
/** TSI - Register Layout Typedef */
typedef struct {
  union {                /* offset: 0x0 */
    uint32_t CONFIG_MUTUAL;        /**< TSI CONFIG (TSI_CONFIG) for Mutual-Capacitor, offset: 0x0 */
    uint32_t CONFIG;               /**< TSI CONFIG (TSI_CONFIG) for Self-Capacitor, offset: 0x0 */
  };
  uint32_t TSHD;                   /**< TSI Threshold, offset: 0x4 */
  uint32_t GENCS;                  /**< TSI General Control and Status, offset: 0x8 */
  uint32_t MUL;                    /**< TSI Mutual-Capacitance, offset: 0xC */
  uint32_t SINC;                   /**< TSI SINC Filter, offset: 0x10 */
  uint32_t SSC0;                   /**< TSI SSC 0, offset: 0x14 */
  uint32_t SSC1;                   /**< TSI SSC 1, offset: 0x18 */
  uint32_t SSC2;                   /**< TSI SSC 2, offset: 0x1C */
  uint32_t BASELINE;               /**< TSI Baseline, offset: 0x20 */
  uint32_t CHMERGE;                /**< TSI Channel Merge, offset: 0x24 */
  uint32_t SHIELD;                 /**< TSI Shield, offset: 0x28 */
       uint8_t RESERVED_0[212];
  uint32_t DATA;                   /**< TSI Data and Status, offset: 0x100 */
       uint8_t RESERVED_1[4];
  uint32_t MISC;                   /**< TSI Miscellaneous, offset: 0x108 */
  uint32_t TRIG;                   /**< TSI AUTO TRIG, offset: 0x10C */
} fs_tsi_v6_t;

/*******************************************************************************
 * Macros TSI common
 *******************************************************************************/
#define TSI_DATA_TSICNT_MASK   (0xFFFFU)

/*******************************************************************************
 * Macros TSI_v5
 *******************************************************************************/
#define TSI_DATA_TSICH_MASK    (0xF8000000U)
#define TSI_DATA_SWTS_MASK     (0x00400000U)
#define TSI_GENCS_EOSF_MASK    (0x4U)
#define TSI_GENCS_OUTRGF_MASK  (0x80000000U)
#define TSI_MUL0_M_SEL_TX_MASK (0x70U)
#define TSI_MUL0_M_SEL_RX_MASK (0x7U)

#define FS_TSI_SET_CHANNEL(pTsi, u8Channel)                \
    (((fs_tsi_v5_t *)pTsi)->DATA) &= ~TSI_DATA_TSICH_MASK; \
    (((fs_tsi_v5_t *)pTsi)->DATA) |= ((((uint32_t)(u8Channel)) << 27UL) & TSI_DATA_TSICH_MASK);

#define FS_TSI_SET_MUT_CHANNEL(pTsi, u8ChannelTx, u8ChannelRx)                           \
    (((fs_tsi_v5_t *)pTsi)->MUL0) &= ~(TSI_MUL0_M_SEL_TX_MASK | TSI_MUL0_M_SEL_RX_MASK); \
    (((fs_tsi_v5_t *)pTsi)->MUL0) |=                                                     \
        ((((uint32_t)(u8ChannelTx) << 4U)) | ((((uint32_t)(u8ChannelRx)) - 6U))); /* mut channel select Tx, Rx-6 */

#define FS_TSI_TRIGGER(pTsi)         (((fs_tsi_v5_t *)pTsi)->DATA) |= TSI_DATA_SWTS_MASK;
#define FS_TSI_IS_EOSF_FLAG(pTsi)    ((((fs_tsi_v5_t *)pTsi)->GENCS) & TSI_GENCS_EOSF_MASK)
#define FS_TSI_CLEAR_EOSF_FLAG(pTsi) ((((fs_tsi_v5_t *)pTsi)->GENCS) |= TSI_GENCS_EOSF_MASK);
#define FS_TSI_READ_RESULT_REG(pTsi) ((((fs_tsi_v5_t *)pTsi)->DATA) & TSI_DATA_TSICNT_MASK)


/*******************************************************************************
 * Macros TSI_v6
 *******************************************************************************/
#define TSI_CONFIG_TSICH_MASK   (0x3EU)
#define TSI_CONFIG_MUTUAL_M_SEL_RX_MASK (0x1F00U)
#define TSI_CONFIG_MUTUAL_M_SEL_TX_MASK (0xE000U)
#define TSI_GENCS_SWTS_MASK             (0x80U)
#define TSI_DATA_EOSF_MASK              (0x8000000U)


#define FS_TSI_SET_CHANNEL_v6(pTsi, u8Channel)                \
    (((fs_tsi_v6_t *)pTsi)->CONFIG) &= ~TSI_CONFIG_TSICH_MASK; \
    (((fs_tsi_v6_t *)pTsi)->CONFIG) |= ((((uint32_t)(u8Channel)) << 1UL) & TSI_CONFIG_TSICH_MASK);

#define FS_TSI_SET_MUT_CHANNEL_v6(pTsi, u8ChannelTx, u8ChannelRx)                           \
    (((fs_tsi_v6_t *)pTsi)->CONFIG_MUTUAL) &= ~(TSI_CONFIG_MUTUAL_M_SEL_TX_MASK | TSI_CONFIG_MUTUAL_M_SEL_RX_MASK); \
    (((fs_tsi_v6_t *)pTsi)->CONFIG_MUTUAL) |=                                                     \
        ((((uint32_t)(u8ChannelTx) << 13U)) | ((((uint32_t)(u8ChannelRx)) - 8U) << 8U)); /* mut channel select Tx, Rx-6 */

#define FS_TSI_TRIGGER_v6(pTsi)         (((fs_tsi_v6_t *)pTsi)->GENCS) |= TSI_GENCS_SWTS_MASK;
#define FS_TSI_IS_EOSF_FLAG_v6(pTsi)    ((((fs_tsi_v6_t *)pTsi)->DATA) & TSI_DATA_EOSF_MASK)
#define FS_TSI_CLEAR_EOSF_FLAG_v6(pTsi) ((((fs_tsi_v6_t *)pTsi)->DATA) |= TSI_DATA_EOSF_MASK);
#define FS_TSI_READ_RESULT_REG_v6(pTsi) ((((fs_tsi_v6_t *)pTsi)->DATA) & TSI_DATA_TSICNT_MASK)
/*******************************************************************************
 * Implementation
 ******************************************************************************/
/*******************************************************************************
 ******************************************************************************/
void FS_TSI_InputInit(fs_tsi_t *pObj)
{
    pObj->data.tsicnt             = 0U;
    pObj->data.tsicntDelta        = 0;
    pObj->data.tsicntStim         = 0U;
    pObj->inputRegBackup.pcr      = 0U;
    pObj->inputRegBackup.pddr     = 0U;
    pObj->inputRegBackup.pdor     = 0U;
    pObj->tsiCounter.counterValue = 0U;

    pObj->state = FS_TSI_INIT;
}

/*******************************************************************************
 ******************************************************************************/
FS_RESULT FS_TSI_InputStimulate(fs_tsi_t *pObj)
{
    volatile fs_gpio_t *pPortTested = (fs_gpio_t *)pObj->gpioInput.rxGpio;
    volatile fs_pcr_t *pPcrTested   = (fs_pcr_t *)pObj->gpioInput.rxPcr;
    uint32_t testedGpioMask         = 1UL << pObj->gpioInput.rxPinNum;
    uint32_t testedPcrMask          = pObj->gpioInput.rxPinNum;

    /* temp variable for original PORT register content */
    volatile uint32_t testedPcrValue;

    /*Backup reg*/
    pObj->inputRegBackup.pcr  = pPcrTested->PCR[testedPcrMask];
    pObj->inputRegBackup.pddr = (pPortTested->PDDR & testedGpioMask);
    pObj->inputRegBackup.pdor = (pPortTested->PDOR & testedGpioMask);

    /*SET as  GPIO  INPUT */
    pPortTested->PDDR &= ~testedGpioMask; /* set tested pin as input in GPIO*/

    testedPcrValue = pPcrTested->PCR[testedPcrMask];
    testedPcrValue &= ~(PORT_PCR_PS_MSK | PORT_PCR_PE_MSK | PORT_PCR_MUX_7_MSK);

    pPcrTested->PCR[testedPcrMask] =
        (testedPcrValue | PORT_PCR_PE_MSK | PORT_PCR_MUX_1_MSK); /* Pull enable = PULL DOWN */

    if (pObj->stimPolarity != 0U)
    {
        /*UP*/
        pPcrTested->PCR[testedPcrMask] |= (PORT_PCR_PS_MSK); /* Switch to PULL UP */
    }

    return FS_PASS;
}

/*******************************************************************************
 ******************************************************************************/
FS_RESULT FS_TSI_InputRelease(fs_tsi_t *pObj)
{
    volatile fs_gpio_t *pPortTested = (fs_gpio_t *)pObj->gpioInput.rxGpio;
    volatile fs_pcr_t *pPcrTested   = (fs_pcr_t *)pObj->gpioInput.rxPcr;
    uint32_t testedPcrMask          = pObj->gpioInput.rxPinNum;

    /* Restore register */
    pPcrTested->PCR[testedPcrMask] = pObj->inputRegBackup.pcr;

    pPortTested->PDDR &= ~(pObj->inputRegBackup.pddr);
    pPortTested->PDDR |= (pObj->inputRegBackup.pddr);

    pPortTested->PDOR &= ~(pObj->inputRegBackup.pdor);
    pPortTested->PDOR |= (pObj->inputRegBackup.pdor);

    return FS_PASS;
}

/*******************************************************************************
 ******************************************************************************/
FS_RESULT FS_TSI_InputCheckNONStimulated(fs_tsi_t *pObj, uint32_t pTsi)
{
    volatile uint16_t result;

    if ((pObj->state == FS_PASS) || (pObj->state == FS_TSI_INIT)) /* First CALL for this channel */
    {                                                             /* Scan not running => START */

        if (pObj->input.txCH == SAFETY_SELFCAP_MODE)    /* SET HW */
        {                                               /* We want to test SELF CAP input */
            FS_TSI_SET_CHANNEL(pTsi, pObj->input.rxCH); /* SET SELFCAP channel */
        }
        else
        {                                                                     /* HW to mutual cap */
            FS_TSI_SET_MUT_CHANNEL(pTsi, pObj->input.txCH, pObj->input.rxCH); /* Set mutual channel */
        }

        FS_TSI_TRIGGER(pTsi);                  /* Start new scan for chanel which set abowe */
        pObj->state = FS_TSI_PROGRESS_NONSTIM; /* Scan for NON stimulated run */
    }
    else if (pObj->state == FS_TSI_PROGRESS_NONSTIM)
    {
        /* Scan for non stimulated test run */
        if (0U != FS_TSI_IS_EOSF_FLAG(pTsi)) /* SCAN COMPLETE! */
        {
            result = (uint16_t)FS_TSI_READ_RESULT_REG(pTsi); /* Get result here */
            FS_TSI_CLEAR_EOSF_FLAG(pTsi);

            pObj->data.tsicnt = result; /* Store data for next test */
            if (result < pObj->limits.low)
            {
                /* check, if converted value fits to limits */
                pObj->state = FS_FAIL_TSI;
                return pObj->state;
            }

            if (result > pObj->limits.high)
            {
                pObj->state = FS_FAIL_TSI;
                return pObj->state;
            }

            pObj->state = FS_TSI_PASS_NONSTIM; /* Non stimulated test OK*/
            return pObj->state;
        } /* End scan complete */
        else
        {
            pObj->tsiCounter.counterValue++;
            if (pObj->tsiCounter.counterValue >= pObj->tsiCounter.counterLimit)
            {
                pObj->state = FS_FAIL_TSI;
                return pObj->state;
            }
        }
    }
    else /*Wrong state*/
    {
        pObj->state = FS_TSI_INCORRECT_CALL;
        return pObj->state;
    }
    return pObj->state;
}

/*******************************************************************************
 ******************************************************************************/
FS_RESULT FS_TSI_InputCheckStimulated(fs_tsi_t *pObj, uint32_t pTsi)
{
    volatile uint16_t result;

    if (pObj->state == FS_TSI_PASS_NONSTIM) /* First CALL for this channel */
    {
        /* Scan not running => START */
        if (pObj->input.txCH == SAFETY_SELFCAP_MODE) /* SET HW */
        {
            /* We want to test SELF CAP input */
            FS_TSI_SET_CHANNEL(pTsi, pObj->input.rxCH); /* SET SELFCAP channel */
        }
        else
        {
            /* HW to mutual cap */
            FS_TSI_SET_MUT_CHANNEL(pTsi, pObj->input.txCH, pObj->input.rxCH); /* Set mutual channel */
        }

        if (FS_FAIL_TSI == FS_TSI_InputStimulate(pObj)) /* IF pull  fail => return fail */
        {
            pObj->state = FS_FAIL_TSI;
            return pObj->state;
        }

        FS_TSI_TRIGGER(pTsi);               /* Start new scan for chanel which set abowe */
        pObj->state = FS_TSI_PROGRESS_STIM; /* Scan for NON stimulated run */
    }
    else if (pObj->state == FS_TSI_PROGRESS_STIM)
    {
        /* Scan for non stimulated test run */
        if (0U != FS_TSI_IS_EOSF_FLAG(pTsi)) /* SCAN COMPLETE! */
        {
            result = (uint16_t)FS_TSI_READ_RESULT_REG(pTsi); /* Get result here */
            FS_TSI_CLEAR_EOSF_FLAG(pTsi);

            /* Disable stimulation (pull resistor on pin) */
            if (FS_FAIL_TSI == FS_TSI_InputRelease(pObj)) /* IF pull  fail => return fail */
            {
                pObj->state = FS_FAIL_TSI;
                return pObj->state;
            }

            pObj->data.tsicntStim  = result;
            pObj->data.tsicntDelta = (int32_t)result - (int32_t)pObj->data.tsicnt;

            uint32_t signExpected;
            uint32_t signMeasured;
            int32_t deltaPercentageAbs;
            int32_t deltaMeasuredAbs;

            signExpected =
                (((uint32_t)(pObj->limits.delta)) &
                 0x80000000U); /* mask 32-bit value to specify the MSB, whether the number is positive or negative */
            signMeasured = (((uint32_t)(pObj->data.tsicntDelta)) & 0x80000000U);
            if ((signExpected ^ signMeasured) != 0U) /* if signs are different => Fail */
            {
                pObj->state = FS_FAIL_TSI; /* if deltas have different sign */
                return pObj->state;
            }
            else
            {
                /* calculate absolute values (remove sign)*/
                deltaPercentageAbs = (int32_t)(
                    ((pObj->limits.deltaPer < (int32_t)0) ? (-pObj->limits.deltaPer) : (pObj->limits.deltaPer)));
                deltaMeasuredAbs = (int32_t)(
                    ((pObj->data.tsicntDelta < (int32_t)0) ? (-pObj->data.tsicntDelta) : (pObj->data.tsicntDelta)));
                if (deltaMeasuredAbs < deltaPercentageAbs) /* check if delta is big enough */
                {
                    pObj->state = FS_FAIL_TSI;
                    return pObj->state;
                }
            }

            pObj->state = FS_TSI_PASS_STIM;
            return pObj->state;
        }
        else
        {
            pObj->tsiCounter.counterValue++;
            if (pObj->tsiCounter.counterValue >= pObj->tsiCounter.counterLimit)
            {
                pObj->state = FS_FAIL_TSI;
                return pObj->state;
            }
        }
    }
    else
    {
        pObj->state = FS_TSI_INCORRECT_CALL;
        return pObj->state;
    }

    return pObj->state;
}


/*******************************************************************************
 ******************************************************************************/
FS_RESULT FS_TSI_InputCheckNONStimulated_v6(fs_tsi_t *pObj, uint32_t pTsi)
{
    volatile uint16_t result;

    if ((pObj->state == FS_PASS) || (pObj->state == FS_TSI_INIT)) /* First CALL for this channel */
    {                                                             /* Scan not running => START */

        if (pObj->input.txCH == SAFETY_SELFCAP_MODE)    /* SET HW */
        {                                               /* We want to test SELF CAP input */
            FS_TSI_SET_CHANNEL_v6(pTsi, pObj->input.rxCH); /* SET SELFCAP channel */
        }
        else
        {                                                                     /* HW to mutual cap */
            FS_TSI_SET_MUT_CHANNEL_v6(pTsi, pObj->input.txCH, pObj->input.rxCH); /* Set mutual channel */
        }

        FS_TSI_TRIGGER_v6(pTsi);                  /* Start new scan for chanel which set abowe */
        pObj->state = FS_TSI_PROGRESS_NONSTIM; /* Scan for NON stimulated run */
    }
    else if (pObj->state == FS_TSI_PROGRESS_NONSTIM)
    {
        /* Scan for non stimulated test run */
        if (0U != FS_TSI_IS_EOSF_FLAG_v6(pTsi)) /* SCAN COMPLETE! */
        {
            result = (uint16_t)FS_TSI_READ_RESULT_REG_v6(pTsi); /* Get result here */
            FS_TSI_CLEAR_EOSF_FLAG_v6(pTsi);

            pObj->data.tsicnt = result; /* Store data for next test */
            if (result < pObj->limits.low)
            {
                /* check, if converted value fits to limits */
                pObj->state = FS_FAIL_TSI;
                return pObj->state;
            }

            if (result > pObj->limits.high)
            {
                pObj->state = FS_FAIL_TSI;
                return pObj->state;
            }

            pObj->state = FS_TSI_PASS_NONSTIM; /* Non stimulated test OK*/
            return pObj->state;
        } /* End scan complete */
        else
        {
            pObj->tsiCounter.counterValue++;
            if (pObj->tsiCounter.counterValue >= pObj->tsiCounter.counterLimit)
            {
                pObj->state = FS_FAIL_TSI;
                return pObj->state;
            }
        }
    }
    else /*Wrong state*/
    {
        pObj->state = FS_TSI_INCORRECT_CALL;
        return pObj->state;
    }
    return pObj->state;
}


/*******************************************************************************
 ******************************************************************************/
FS_RESULT FS_TSI_InputStimulate_v6(fs_tsi_t *pObj)
{
    volatile fs_rgpio_imxrt_t *pPortTested = (fs_rgpio_imxrt_t *)pObj->gpioInput.rxGpio;
    volatile fs_pcr_t *pPcrTested   = (fs_pcr_t *)pObj->gpioInput.rxPcr;
    uint32_t testedGpioMask         = 1UL << pObj->gpioInput.rxPinNum;
    uint32_t testedPcrMask          = pObj->gpioInput.rxPinNum;

    /* temp variable for original PORT register content */
    volatile uint32_t testedPcrValue;

    /*Backup reg*/
    pObj->inputRegBackup.pcr  = pPcrTested->PCR[testedPcrMask];
    pObj->inputRegBackup.pddr = (pPortTested->PDDR & testedGpioMask);
    pObj->inputRegBackup.pdor = (pPortTested->PDOR & testedGpioMask);

    /*SET as  GPIO  INPUT */
    pPortTested->PDDR &= ~testedGpioMask; /* set tested pin as input in GPIO*/

    testedPcrValue = pPcrTested->PCR[testedPcrMask];
    testedPcrValue &= ~(PORT_PCR_PS_MSK | PORT_PCR_PE_MSK);
    pPcrTested->PCR[testedPcrMask] = (testedPcrValue | PORT_PCR_PE_MSK | PORT_PCR_IBE_MASK); /* Pull enable = PULL DOWN */

    if (pObj->stimPolarity != 0U)
    {
        /*UP*/
        pPcrTested->PCR[testedPcrMask] |= (PORT_PCR_PS_MSK); /* Switch to PULL UP */
    }

    return FS_PASS;
}

/*******************************************************************************
 ******************************************************************************/
FS_RESULT FS_TSI_InputRelease_v6(fs_tsi_t *pObj)
{
    volatile fs_rgpio_imxrt_t *pPortTested = (fs_rgpio_imxrt_t *)pObj->gpioInput.rxGpio;
    volatile fs_pcr_t *pPcrTested   = (fs_pcr_t *)pObj->gpioInput.rxPcr;
    uint32_t testedPcrMask          = pObj->gpioInput.rxPinNum;

    /* Restore register */
    pPcrTested->PCR[testedPcrMask] = pObj->inputRegBackup.pcr;

    pPortTested->PDDR &= ~(pObj->inputRegBackup.pddr);
    pPortTested->PDDR |= (pObj->inputRegBackup.pddr);

    pPortTested->PDOR &= ~(pObj->inputRegBackup.pdor);
    pPortTested->PDOR |= (pObj->inputRegBackup.pdor);

    return FS_PASS;
}

/*******************************************************************************
 ******************************************************************************/
FS_RESULT FS_TSI_InputCheckStimulated_v6(fs_tsi_t *pObj, uint32_t pTsi)
{
    volatile uint16_t result;

    if (pObj->state == FS_TSI_PASS_NONSTIM) /* First CALL for this channel */
    {
        /* Scan not running => START */
        if (pObj->input.txCH == SAFETY_SELFCAP_MODE) /* SET HW */
        {
            /* We want to test SELF CAP input */
            FS_TSI_SET_CHANNEL_v6(pTsi, pObj->input.rxCH); /* SET SELFCAP channel */
        }
        else
        {
            /* HW to mutual cap */
            FS_TSI_SET_MUT_CHANNEL_v6(pTsi, pObj->input.txCH, pObj->input.rxCH); /* Set mutual channel */
        }

        if (FS_FAIL_TSI == FS_TSI_InputStimulate_v6(pObj)) /* IF pull  fail => return fail */
        {
            pObj->state = FS_FAIL_TSI;
            return pObj->state;
        }

        FS_TSI_TRIGGER_v6(pTsi);               /* Start new scan for chanel which set abowe */
        pObj->state = FS_TSI_PROGRESS_STIM; /* Scan for NON stimulated run */
    }
    else if (pObj->state == FS_TSI_PROGRESS_STIM)
    {
        /* Scan for non stimulated test run */
        if (0U != FS_TSI_IS_EOSF_FLAG_v6(pTsi)) /* SCAN COMPLETE! */
        {
            result = (uint16_t)FS_TSI_READ_RESULT_REG_v6(pTsi); /* Get result here */
            FS_TSI_CLEAR_EOSF_FLAG_v6(pTsi);

            /* Disable stimulation (pull resistor on pin) */
            if (FS_FAIL_TSI == FS_TSI_InputRelease_v6(pObj)) /* IF pull  fail => return fail */
            {
                pObj->state = FS_FAIL_TSI;
                return pObj->state;
            }

            pObj->data.tsicntStim  = result;
            pObj->data.tsicntDelta = (int32_t)result - (int32_t)pObj->data.tsicnt;

            uint32_t signExpected;
            uint32_t signMeasured;
            int32_t deltaPercentageAbs;
            int32_t deltaMeasuredAbs;

            signExpected =
                (((uint32_t)(pObj->limits.delta)) &
                 0x80000000U); /* mask 32-bit value to specify the MSB, whether the number is positive or negative */
            signMeasured = (((uint32_t)(pObj->data.tsicntDelta)) & 0x80000000U);
            if ((signExpected ^ signMeasured) != 0U) /* if signs are different => Fail */
            {
                pObj->state = FS_FAIL_TSI; /* if deltas have different sign */
                return pObj->state;
            }
            else
            {
                /* calculate absolute values (remove sign)*/
                deltaPercentageAbs = (int32_t)(
                    ((pObj->limits.deltaPer < (int32_t)0) ? (-pObj->limits.deltaPer) : (pObj->limits.deltaPer)));
                deltaMeasuredAbs = (int32_t)(
                    ((pObj->data.tsicntDelta < (int32_t)0) ? (-pObj->data.tsicntDelta) : (pObj->data.tsicntDelta)));
                if (deltaMeasuredAbs < deltaPercentageAbs) /* check if delta is big enough */
                {
                    pObj->state = FS_FAIL_TSI;
                    return pObj->state;
                }
            }

            pObj->state = FS_TSI_PASS_STIM;
            return pObj->state;
        }
        else
        {
            pObj->tsiCounter.counterValue++;
            if (pObj->tsiCounter.counterValue >= pObj->tsiCounter.counterLimit)
            {
                pObj->state = FS_FAIL_TSI;
                return pObj->state;
            }
        }
    }
    else
    {
        pObj->state = FS_TSI_INCORRECT_CALL;
        return pObj->state;
    }

    return pObj->state;
}


