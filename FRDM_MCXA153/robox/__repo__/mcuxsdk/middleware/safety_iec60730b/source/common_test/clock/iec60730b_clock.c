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
 * @brief Clock test common routines - IEC60730 Class B.
 *
 */

#include "iec60730b.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* GPT - Masks */
#define FS_GPT_CR_EN_MASK  (0x1U)
#define FS_GPT_CR_EN_SHIFT (0U)
#define FS_GPT_CR_EN(x)    (((uint32_t)(((uint32_t)(x)) << GPT_CR_EN_SHIFT)) & GPT_CR_EN_MASK)
#define FS_GPT_SR_ROV_MASK (0x20U)

#define FS_CTIMER_TCR_CRST_MASK 0x2U

/*******************************************************************************
 * Macros
 *******************************************************************************/
#define FS_CLK_COUNTER_INIT 0xffffffffU

/*******************************************************************************
 * Implementation
 ******************************************************************************/
/*******************************************************************************
 ******************************************************************************/
FS_RESULT FS_CLK_Check(uint32_t testContext, uint32_t limitLow, uint32_t limitHigh)
{
    FS_RESULT st = 0;
    if (testContext == FS_CLK_COUNTER_INIT)
    {
        st = FS_CLK_PROGRESS;
    }
    else if (testContext > limitHigh)
    {
        st = FS_FAIL_CLK;
    }
    else if (testContext < limitLow)
    {
        st = FS_FAIL_CLK;
    }
    else
    {
        st = FS_PASS;
    }
    return st;
}

/*******************************************************************************
 ******************************************************************************/
void FS_CLK_Init(uint32_t *pTestContext)
{
    *pTestContext = FS_CLK_COUNTER_INIT;
}

/*******************************************************************************
 ******************************************************************************/
void FS_CLK_LPTMR(fs_lptmr_t *pSafetyTmr, uint32_t *pTestContext)
{
    volatile fs_lptmr_t *pTmr = pSafetyTmr;

    pTmr->CNR     = 0U; /* write any value to synchronize the counter value */
    *pTestContext = pTmr->CNR;
    pTmr->CSR &= ~FS_LPTMR_CSR_TEN_MASK; /* stop the timer, this clears the TFC flag */
    pTmr->CSR |= FS_LPTMR_CSR_TEN_MASK;  /* start the timer */
}

/*******************************************************************************
 ******************************************************************************/
void FS_CLK_RTC(fs_rtc_t *pSafetyTmr, uint32_t *pTestContext)
{
    uint32_t tmp;
    volatile fs_rtc_t *pTmr = pSafetyTmr;

    *pTestContext = pTmr->CNT;

    /* Reset the timer */
    tmp      = pTmr->SC;
    pTmr->SC = 0U;
    pTmr->SC = tmp;
}

/*******************************************************************************
 ******************************************************************************/
void FS_CLK_GPT(fs_gpt_t *pSafetyTmr, uint32_t *pTestContext)
{
    volatile fs_gpt_t *pTmr = pSafetyTmr;
    *pTestContext           = pTmr->CNT;   /* Store value from GPT */
    pTmr->OCR[0]            = 0xFFFFFFFFU; /* Change "compare" value, this cause RESET of CNT to zero */
}

/*******************************************************************************
 ******************************************************************************/
void FS_CLK_WKT_LPC(fs_wkt_t *pSafetyTmr, uint32_t *pTestContext, uint32_t startValue)
{
    volatile fs_wkt_t *pTmr = pSafetyTmr;

    /* Due to non-synchronous of CLOCK source of WKT and core it is necessary to
     * read count register twice. Because it is possible to change count value
     * druing read. There is used an approach, that count is read, some dealy
     * applied and read again. If this reading is not the same, third read
     * happens and this is used for next processing. */

    uint8_t i      = 0U;
    uint32_t temp  = 0xFFFFFFFFU; /*-1*/
    uint32_t temp2 = 0xFFFFFFFFU; /*-1*/

    temp = pTmr->COUNT; /* Read first time */

    for (i = 0U; i < 2U; i++) /*Apply some delay */
    {
        ASM_INLINE("nop");
    }

    temp2 = pTmr->COUNT; /*Read again */
    if (temp != temp2)   /* If result is not the same, read for the third time */
    {
        ASM_INLINE("nop");
        temp2 = pTmr->COUNT;
    }

    *pTestContext = temp2; /*There is use */

    pTmr->CTRL |= 0x2U; /*Clear Flag */
    pTmr->CTRL |= 0x4U; /*Mask bit 2  => HALT timer*/

    pTmr->COUNT = startValue; /* Set start value for Decreasing of Counter => Counter start */
}

/*******************************************************************************
 ******************************************************************************/
void FS_CLK_CTIMER(fs_ctimer_t *pSafetyTmr, uint32_t *pTestContext)
{
    volatile fs_ctimer_t *pTmr = pSafetyTmr;

    *pTestContext = pTmr->TC;

    pTmr->TCR |= FS_CTIMER_TCR_CRST_MASK; /*reset couter*/
    ASM_INLINE("nop");
    pTmr->TCR &= ~(FS_CTIMER_TCR_CRST_MASK);
}

/*******************************************************************************
 ******************************************************************************/
void FS_CLK_STM(fs_stm_t *pSafetyTmr, uint32_t *pTestContext)
{
    volatile fs_stm_t *pTmr = pSafetyTmr;

    *pTestContext = pTmr->CNT;
    pTmr->CNT = 0U; /* Clear counter */
}

