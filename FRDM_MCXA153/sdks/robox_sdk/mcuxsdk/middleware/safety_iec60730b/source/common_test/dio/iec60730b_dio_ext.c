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
 * @brief Digital I/O extended test common routines - IEC60730 Class B.
 *
 */

#include "iec60730b.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Implementation
 ******************************************************************************/
/*******************************************************************************
 ******************************************************************************/
FS_RESULT FS_DIO_InputExt(fs_dio_test_t *pTestedPin, fs_dio_test_t *pAdjPin, bool_t testedPinValue, bool_t backupEnable)
{
    /* Return status */
    FS_RESULT st = FS_PASS;

    /* GPIO and PORT pointers definitions */
    volatile fs_gpio_t *pPortTested   = (fs_gpio_t *)pTestedPin->gpio;
    volatile fs_pcr_t *pPcrTested     = (fs_pcr_t *)pTestedPin->pcr;
    volatile fs_gpio_t *pPortAdjacent = (fs_gpio_t *)pAdjPin->gpio;
    volatile fs_pcr_t *pPcrAdjacent   = (fs_pcr_t *)pAdjPin->pcr;

    /* masks for tested and adjacent pins */
    uint32_t testedGpioMask   = (1UL << pTestedPin->pinNum);
    uint32_t adjacentGpioMask = (1UL << pAdjPin->pinNum);
    uint32_t compareValue     = 0U;

    if (testedPinValue)
    {
        compareValue = testedGpioMask;
    }
    /* check if pin is configured as input */
    if ((pPortTested->PDDR & testedGpioMask) != 0U)
    {
         st = FS_FAIL_DIO_INPUT;
    }

    /* check the pin value */
    if (((pPortTested->PDIR & testedGpioMask) ^ compareValue) != 0U)
    { // ^ xor
      if(st == FS_PASS){ /* To prevent rewrite of previsous error */
        st = FS_FAIL_DIO_WRONG_VALUE;
      }
    }

    if (backupEnable)
    { /* if enable, restore values of tested and adjacent pins, GPIO and PORT */
        pPcrTested->PCR[pTestedPin->pinNum] = pTestedPin->sTestedPinBackup.pcr;

        if (pTestedPin->sTestedPinBackup.pddr != 0U)
        {
            pPortTested->PDDR |= pTestedPin->sTestedPinBackup.pddr;
        }else{
            pPortTested->PDDR &= ~pTestedPin->sTestedPinBackup.pddr;
        }

        if (pTestedPin->sTestedPinBackup.pdor != 0U)
        {
            pPortTested->PSOR = testedGpioMask; /* Set pin value */
        }else{
            pPortTested->PCOR = testedGpioMask; /* Clear pin value */
        }

        pPcrAdjacent->PCR[pAdjPin->pinNum] = pAdjPin->sTestedPinBackup.pcr;

        if (pAdjPin->sTestedPinBackup.pddr != 0U)
        {
            pPortAdjacent->PDDR |= pAdjPin->sTestedPinBackup.pddr;
        }else{
            pPortAdjacent->PDDR &= ~pAdjPin->sTestedPinBackup.pddr;
        }

        if (pAdjPin->sTestedPinBackup.pdor != 0U)
        {
            pPortAdjacent->PSOR = adjacentGpioMask; /* Set pin value */
        }else{
            pPortAdjacent->PCOR = adjacentGpioMask; /* Clear pin value */
        }
    }

    return st;
}

/*******************************************************************************
 ******************************************************************************/
FS_RESULT FS_DIO_ShortToSupplySet(fs_dio_test_t *pTestedPin, bool_t shortToVoltage, bool_t backupEnable)
{
    /* Return status */
    FS_RESULT st = FS_PASS;

    /* GPIO and PORT pointers definitions */
    volatile fs_gpio_t *pPortTested = (fs_gpio_t *)pTestedPin->gpio;
    volatile fs_pcr_t *pPcrTested   = (fs_pcr_t *)pTestedPin->pcr;

    uint8_t testPinNum = pTestedPin->pinNum;

    /* mask for tested pin */
    uint32_t testedGpioMask = (1UL << testPinNum);

    /* temp variable for original PORT register content */
    volatile uint32_t testedPcrValue;

    /* if enable, save the actual value of tested pin, GPIO and PORT */
    if (backupEnable)
    {
        pTestedPin->sTestedPinBackup.pcr  = pPcrTested->PCR[testPinNum];
        pTestedPin->sTestedPinBackup.pddr = (pPortTested->PDDR & testedGpioMask);
        pTestedPin->sTestedPinBackup.pdor = (pPortTested->PDOR & testedGpioMask);

        /* set tested pin as input if backup functionality is enable,
         * if not, it is responsibility of application */
        pPortTested->PDDR &= ~testedGpioMask;
    }

    /* check if pin is configured as input */
    if ((pPortTested->PDDR & testedGpioMask) != 0U)
    {
         st = FS_FAIL_DIO_INPUT;
    }

    testedPcrValue = pPcrTested->PCR[testPinNum]; /* value from tested pin register in PORT */
    testedPcrValue &= ~(PORT_PCR_PS_MSK | PORT_PCR_PE_MSK | PORT_PCR_MUX_7_MSK); /* unmask MUX, PS, PE in PORT */
    pPcrTested->PCR[testPinNum] = (testedPcrValue | PORT_PCR_PE_MSK |
                                   PORT_PCR_MUX_1_MSK); /* set GPIO mux, pull-up or pull-down enable in PORT  */

    /* set pull-up or pull-down on tested pin  */
    if (shortToVoltage)
    {
        /* if it is test against Gnd, sets pull-up */
        pPcrTested->PCR[testPinNum] |= PORT_PCR_PS_MSK;
    }

    return st;
}

/*******************************************************************************
 ******************************************************************************/
FS_RESULT FS_DIO_ShortToAdjSet(fs_dio_test_t *pTestedPin,
                               fs_dio_test_t *pAdjPin,
                               bool_t testedPinValue,
                               bool_t backupEnable)
{
    /* Return status */
    FS_RESULT st = FS_PASS;

    /* GPIO and PORT pointers definitions */
    volatile fs_gpio_t *pPortTested   = (fs_gpio_t *)pTestedPin->gpio;
    volatile fs_pcr_t *pPcrTested     = (fs_pcr_t *)pTestedPin->pcr;
    volatile fs_gpio_t *pPortAdjacent = (fs_gpio_t *)pAdjPin->gpio;
    volatile fs_pcr_t *pPcrAdjacent   = (fs_pcr_t *)pAdjPin->pcr;

    uint8_t testPinNum = pTestedPin->pinNum;

    /* masks for tested and adjacent pins */
    uint32_t testedGpioMask   = (1UL << testPinNum);
    uint32_t adjacentGpioMask = (1UL << pAdjPin->pinNum);

    /* temp variable for original PORT register content */
    volatile uint32_t testedPcrValue;

    /* if enable, save the actual value of tested and adjacent pins, GPIO and PORT */
    if (backupEnable)
    {
        pTestedPin->sTestedPinBackup.pcr  = pPcrTested->PCR[testPinNum];
        pTestedPin->sTestedPinBackup.pddr = (pPortTested->PDDR & testedGpioMask);
        pTestedPin->sTestedPinBackup.pdor = (pPortTested->PDOR & testedGpioMask);
        pAdjPin->sTestedPinBackup.pcr     = pPcrAdjacent->PCR[testPinNum];
        pAdjPin->sTestedPinBackup.pddr    = (pPortAdjacent->PDDR & adjacentGpioMask);
        pAdjPin->sTestedPinBackup.pdor    = (pPortAdjacent->PDOR & adjacentGpioMask);

        /* set tested pin as input if backup functionality is enable,
         * set adjacent pin as output if backup functionality is enable,
         * if not, it is responsibility of application */
        pPortTested->PDDR &= ~testedGpioMask;
        pPortAdjacent->PDDR |= adjacentGpioMask;
    }

    /* check if pin is configured as input */
    if ((pPortTested->PDDR & testedGpioMask) != 0U)
    {
         st = FS_FAIL_DIO_INPUT;
    }

    /* check if Adjacent pin is configured as output */
    if ((pPortAdjacent->PDDR & adjacentGpioMask) == 0U)
    {
      if(st == FS_PASS){ /* To prevent rewrite of previsous error */
        st = FS_FAIL_DIO_OUTPUT;
      }
    }

    testedPcrValue = pPcrTested->PCR[testPinNum]; /* value from tested pin register in PORT */
    testedPcrValue &= ~(PORT_PCR_PS_MSK | PORT_PCR_PE_MSK | PORT_PCR_MUX_7_MSK); /* unmask MUX, PS, PE in PORT */
    pPcrTested->PCR[testPinNum] = (testedPcrValue | PORT_PCR_PE_MSK |
                                   PORT_PCR_MUX_1_MSK); /* set GPIO mux, pull-up or pull-down enable in PORT  */

    /* set pull-up or pull-down on tested pin vs. output 1 or output 0 on adjacent pin */
    if (testedPinValue)
    {
        pPcrTested->PCR[testPinNum] |=
            PORT_PCR_PS_MSK; /* if the tested pin value is logical 1, set pull-up, otherwise pull-down remains set */
        pPortAdjacent->PCOR = adjacentGpioMask; /* if tested pin is set to 1, adjacent pin is cleared to 0 in GPIO */
    }
    else
    {
        /* otherwise, if testef pin is set to 0, adjacent pin is set to 1 in GPIO */
        pPortAdjacent->PSOR = adjacentGpioMask;
    }

    return st;
}

/*******************************************************************************
 ******************************************************************************/
FS_RESULT FS_DIO_InputExt_IMXRT(fs_dio_test_imx_t *pTestedPin,
                                fs_dio_test_imx_t *pAdjPin,
                                bool_t testedPinValue,
                                bool_t backupEnable)
{
    /* Return status */
    FS_RESULT st = FS_PASS;

    /* GPIO and PORT pointers definitions */
    volatile fs_gpio_imxrt_t *pPortTested = (fs_gpio_imxrt_t *)pTestedPin->gpio;
    volatile uint32_t *pMuxTested         = (uint32_t *)pTestedPin->muxAddr;
    volatile uint32_t *pPadTested         = (uint32_t *)pTestedPin->padAddr;

    volatile fs_gpio_imxrt_t *pPortAdjacent = (fs_gpio_imxrt_t *)pAdjPin->gpio;
    volatile uint32_t *pMuxAdjacent         = (uint32_t *)pAdjPin->muxAddr;
    volatile uint32_t *pPadAdjacent         = (uint32_t *)pAdjPin->padAddr;

    /* masks for tested and adjacent pins */
    uint32_t testedGpioMask   = (1UL << pTestedPin->pinNum);
    uint32_t adjacentGpioMask = (1UL << pAdjPin->pinNum);
    uint32_t compareValue     = 0U;

    if (testedPinValue)
    {
        compareValue = testedGpioMask;
    }
    /* check if pin is configured as input */
    if ((pPortTested->GDIR & testedGpioMask) != 0U)
    {
         st = FS_FAIL_DIO_INPUT;
    }

    /* check the pin value */
    if (((pPortTested->PSR & testedGpioMask) ^ compareValue) != 0U)
    { // ^ xor
      if(st == FS_PASS){ /* To prevent rewrite of previsous error */
        st = FS_FAIL_DIO_WRONG_VALUE;
      }
    }
    /* if enable, restore values of tested and adjacent pins, GPIO and PORT */
    if (backupEnable)
    {
        /* Restore mux options */
        *pMuxTested = pTestedPin->sTestedPinBackup.mux;

        /* Restore pull options */
        *pPadTested &= ~pTestedPin->pullResetMask; /* Clear PKE, PUS, PUE before restore */
        *pPadTested = pTestedPin->sTestedPinBackup.pad;

        /* Restore pin direction */
        if (pTestedPin->sTestedPinBackup.gdir != 0U)
        {
            pPortTested->GDIR |= pTestedPin->sTestedPinBackup.gdir;
        }else{
            pPortTested->GDIR &= ~pTestedPin->sTestedPinBackup.gdir;
        }

        /* Restore pin value */
        if (pTestedPin->sTestedPinBackup.dr != 0U)
        {
            pPortTested->DR_SET = testedGpioMask;
        }else{
            pPortTested->DR_CLEAR = testedGpioMask;
        }

        /* Restore mux options */
        *pMuxAdjacent = pAdjPin->sTestedPinBackup.mux;

        /* Restore pull options */
        *pPadAdjacent &= ~pTestedPin->pullResetMask; /* Clear PKE, PUS, PUE before restore */
        *pPadAdjacent = pAdjPin->sTestedPinBackup.pad;

        /* Restore pin direction */
        if (pAdjPin->sTestedPinBackup.gdir != 0U)
        {
            pPortAdjacent->GDIR |= pAdjPin->sTestedPinBackup.gdir;
        }else{
            pPortAdjacent->GDIR &= ~pAdjPin->sTestedPinBackup.gdir;
        }

        /* Restore pin value */
        if (pAdjPin->sTestedPinBackup.dr != 0U)
        {
            pPortAdjacent->DR_SET = adjacentGpioMask; /* Set pin value */
        }else{
            pPortAdjacent->DR_CLEAR = adjacentGpioMask; /* Clear pin value */
        }
    }

    return st;
}

/*******************************************************************************
 ******************************************************************************/
FS_RESULT FS_DIO_ShortToSupplySet_IMXRT(fs_dio_test_imx_t *pTestedPin, bool_t shortToVoltage, bool_t backupEnable)
{
    /* Return status */
    FS_RESULT st = FS_PASS;

    /* GPIO and PORT pointers definitions */
    volatile fs_gpio_imxrt_t *pPortTested = (fs_gpio_imxrt_t *)pTestedPin->gpio;
    volatile uint32_t *pMuxTested         = (uint32_t *)pTestedPin->muxAddr;
    volatile uint32_t *pPadTested         = (uint32_t *)pTestedPin->padAddr;

    /* mask for tested pin */
    uint32_t testedGpioMask = (1UL << pTestedPin->pinNum);

    /* if enable, save the actual value of tested pin, GPIO and PORT */
    if (backupEnable)
    {
        pTestedPin->sTestedPinBackup.mux  = *pMuxTested;
        pTestedPin->sTestedPinBackup.pad  = *pPadTested;
        pTestedPin->sTestedPinBackup.gdir = (pPortTested->GDIR & testedGpioMask);
        pTestedPin->sTestedPinBackup.dr   = (pPortTested->DR & testedGpioMask);

        /* set tested pin as input if backup functionality is enable,
         * if not, it is responsibility of application */
        pPortTested->GDIR &= ~testedGpioMask;
    }

    /* check if pin is configured as input */
    if ((pPortTested->GDIR & testedGpioMask) != 0U)
    {
         st = FS_FAIL_DIO_INPUT;
    }

    /* Set pull-up or pull-down on tested pin */
    *pPadTested &= ~pTestedPin->pullResetMask; /* Clear PUS, PUE */
    if (shortToVoltage)
    {
        /* Select Pull up, 22K Ohm  */
        *pPadTested |= pTestedPin->pullUpMask;
    }else{
        /* Select Pull down, 100K Ohm */
        *pPadTested |= pTestedPin->pullDownMask;
    }

    return st;
}

/*******************************************************************************
 ******************************************************************************/
FS_RESULT FS_DIO_ShortToAdjSet_IMXRT(fs_dio_test_imx_t *pTestedPin,
                                     fs_dio_test_imx_t *pAdjPin,
                                     bool_t testedPinValue,
                                     bool_t backupEnable)
{
    /* Return status */
    FS_RESULT st = FS_PASS;

    /* GPIO and PORT pointers definitions */
    volatile fs_gpio_imxrt_t *pPortTested = (fs_gpio_imxrt_t *)pTestedPin->gpio;
    volatile uint32_t *pMuxTested         = (uint32_t *)pTestedPin->muxAddr;
    volatile uint32_t *pPadTested         = (uint32_t *)pTestedPin->padAddr;

    volatile fs_gpio_imxrt_t *pPortAdjacent = (fs_gpio_imxrt_t *)pAdjPin->gpio;
    volatile uint32_t *pMuxAdjacent         = (uint32_t *)pAdjPin->muxAddr;
    volatile uint32_t *pPadAdjacent         = (uint32_t *)pAdjPin->padAddr;

    /* masks for tested and adjacent pins */
    uint32_t testedGpioMask   = (1UL << pTestedPin->pinNum);
    uint32_t adjacentGpioMask = (1UL << pAdjPin->pinNum);

    /* If enable, save the actual value of tested and adjacent pins, GPIO and PORT */
    if (backupEnable)
    {
        pTestedPin->sTestedPinBackup.mux  = *pMuxTested;
        pTestedPin->sTestedPinBackup.pad  = *pPadTested;
        pTestedPin->sTestedPinBackup.gdir = (pPortTested->GDIR & testedGpioMask);
        pTestedPin->sTestedPinBackup.dr   = (pPortTested->DR & testedGpioMask);

        pAdjPin->sTestedPinBackup.mux  = *pMuxAdjacent;
        pAdjPin->sTestedPinBackup.pad  = *pPadAdjacent;
        pAdjPin->sTestedPinBackup.gdir = (pPortAdjacent->GDIR & adjacentGpioMask);
        pAdjPin->sTestedPinBackup.dr   = (pPortAdjacent->DR & adjacentGpioMask);

        /* set tested pin as input if backup functionality is enable,
         * set adjacent pin as output if backup functionality is enable,
         * if not, it is responsibility of application */
        pPortTested->GDIR &= ~testedGpioMask;
        pPortAdjacent->GDIR |= adjacentGpioMask;
    }

    /* Check if pin is configured as input */
    if ((pPortTested->GDIR & testedGpioMask) != 0U)
    {
         st = FS_FAIL_DIO_INPUT;
    }

    /* check if Adjacent pin is configured as output */
    if ((pPortAdjacent->GDIR & adjacentGpioMask) == 0U)
    {
      if(st == FS_PASS){ /* To prevent rewrite of previsous error */
        st = FS_FAIL_DIO_OUTPUT;
      }
    }
    /* Set pull-up or pull-down on tested pin */
    *pPadTested &= ~pTestedPin->pullResetMask; /* Clear PUS, PUE */
    if (testedPinValue)
    {
        /* Select Pull up, 22K Ohm  */
        *pPadTested |= pTestedPin->pullUpMask;
        pPortAdjacent->DR_CLEAR =
            adjacentGpioMask; /* If tested pin is set to 1, adjacent pin is cleared to 0 in GPIO */
    }else{
        /* Select Pull down, 100K Ohm */
        *pPadTested |= pTestedPin->pullDownMask;
        pPortAdjacent->DR_SET =
            adjacentGpioMask; /* Otherwise, if testef pin is set to 0, adjacent pin is set to 1 in GPIO */
    }

    return st;
}

/*******************************************************************************
 ******************************************************************************/
FS_RESULT FS_DIO_InputExt_IMX8M(fs_dio_test_imx_t *pTestedPin,
                                fs_dio_test_imx_t *pAdjPin,
                                bool_t testedPinValue,
                                bool_t backupEnable)
{
    /* Return status */
    FS_RESULT st = FS_PASS;

    /* GPIO and PORT pointers definitions */
    volatile fs_gpio_imx8m_t *pPortTested = (fs_gpio_imx8m_t *)pTestedPin->gpio;
    volatile uint32_t *pMuxTested         = (uint32_t *)pTestedPin->muxAddr;
    volatile uint32_t *pPadTested         = (uint32_t *)pTestedPin->padAddr;

    volatile fs_gpio_imx8m_t *pPortAdjacent = (fs_gpio_imx8m_t *)pAdjPin->gpio;
    volatile uint32_t *pMuxAdjacent         = (uint32_t *)pAdjPin->muxAddr;
    volatile uint32_t *pPadAdjacent         = (uint32_t *)pAdjPin->padAddr;

    /* masks for tested and adjacent pins */
    uint32_t testedGpioMask   = (1UL << pTestedPin->pinNum);
    uint32_t adjacentGpioMask = (1UL << pAdjPin->pinNum);
    uint32_t compareValue     = 0U;

    if (testedPinValue)
    {
        compareValue = testedGpioMask;
    }
    /* check if pin is configured as input */
    if ((pPortTested->GDIR & testedGpioMask) != 0U)
    {
         st = FS_FAIL_DIO_INPUT;
    }

    /* check the pin value */
    if (((pPortTested->PSR & testedGpioMask) ^ compareValue) != 0U)
    { // ^ xor
      if(st == FS_PASS){ /* To prevent rewrite of previsous error */
        st = FS_FAIL_DIO_WRONG_VALUE;
      }
    }

    /* if enable, restore values of tested and adjacent pins, GPIO and PORT */
    if (backupEnable)
    {
        /* Restore mux options */
        *pMuxTested = pTestedPin->sTestedPinBackup.mux;

        /* Restore pull options */
        *pPadTested &=
            ~(FS_IOMUXC_SW_PAD_CTL_PAD_PUE_MASK | FS_IOMUXC_SW_PAD_CTL_PAD_PE_MASK); /* Clear PE, PUE before restore */
        *pPadTested = pTestedPin->sTestedPinBackup.pad;

        /* Restore pin direction */
        if (pTestedPin->sTestedPinBackup.gdir != 0U)
        {
            pPortTested->GDIR |= pTestedPin->sTestedPinBackup.gdir;
        }else{
            pPortTested->GDIR &= ~pTestedPin->sTestedPinBackup.gdir;
        }
        /* Restore pin value */
        if (pTestedPin->sTestedPinBackup.dr != 0U)
        {
            pPortTested->DR |= testedGpioMask;
        }else{
            pPortTested->DR &= ~testedGpioMask;
        }
        /* Restore mux options */
        *pMuxAdjacent = pAdjPin->sTestedPinBackup.mux;

        /* Restore pull options */
        *pPadAdjacent &=
            ~(FS_IOMUXC_SW_PAD_CTL_PAD_PE_MASK | FS_IOMUXC_SW_PAD_CTL_PAD_PUE_MASK); /* Clear PE, PUE before restore */
        *pPadAdjacent = pAdjPin->sTestedPinBackup.pad;

        /* Restore pin direction */
        if (pAdjPin->sTestedPinBackup.gdir != 0U)
        {
            pPortAdjacent->GDIR |= pAdjPin->sTestedPinBackup.gdir;
        }else{
            pPortAdjacent->GDIR &= ~pAdjPin->sTestedPinBackup.gdir;
        }
        /* Restore pin value */
        if (pAdjPin->sTestedPinBackup.dr != 0U)
        {
            pPortAdjacent->DR |= adjacentGpioMask; /* Set pin value */
        }else{
            pPortAdjacent->DR &= ~adjacentGpioMask; /* Clear pin value */
        }
    }

    return st;
}

/*******************************************************************************
 ******************************************************************************/
FS_RESULT FS_DIO_ShortToSupplySet_IMX8M(fs_dio_test_imx_t *pTestedPin, bool_t shortToVoltage, bool_t backupEnable)
{
    /* Return status */
    FS_RESULT st = FS_PASS;

    /* GPIO and PORT pointers definitions */
    volatile fs_gpio_imx8m_t *pPortTested = (fs_gpio_imx8m_t *)pTestedPin->gpio;
    volatile uint32_t *pMuxTested         = (uint32_t *)pTestedPin->muxAddr;
    volatile uint32_t *pPadTested         = (uint32_t *)pTestedPin->padAddr;

    /* mask for tested pin */
    uint32_t testedGpioMask = (1UL << pTestedPin->pinNum);

    /* if enable, save the actual value of tested pin, GPIO and PORT */
    if (backupEnable)
    {
        pTestedPin->sTestedPinBackup.mux  = *pMuxTested;
        pTestedPin->sTestedPinBackup.pad  = *pPadTested;
        pTestedPin->sTestedPinBackup.gdir = (pPortTested->GDIR & testedGpioMask);
        pTestedPin->sTestedPinBackup.dr   = (pPortTested->DR & testedGpioMask);

        pPortTested->GDIR &= ~testedGpioMask; /* set tested pin as input if backup functionality is enable, */
    }                                         /* if not, it is responsibility of application */

    /* check if pin is configured as input */
    if ((pPortTested->GDIR & testedGpioMask) != 0U)
    {
         st = FS_FAIL_DIO_INPUT;
    }
    /* Set pull-up or pull-down on tested pin */
    *pPadTested &= ~(FS_IOMUXC_SW_PAD_CTL_PAD_PE_MASK | FS_IOMUXC_SW_PAD_CTL_PAD_PUE_MASK); /* Clear PE, PUE */
    if (shortToVoltage)
    {
        /* Select Pull up, 22K Ohm */
        *pPadTested |= (FS_IOMUXC_SW_PAD_CTL_PAD_PE_MASK | FS_IOMUXC_SW_PAD_CTL_PAD_PUE_MASK);
    }else{
        /* Select Pull down, 100K Ohm */
        *pPadTested |= FS_IOMUXC_SW_PAD_CTL_PAD_PE_MASK;
    }

    return st;
}

/*******************************************************************************
 ******************************************************************************/
FS_RESULT FS_DIO_ShortToAdjSet_IMX8M(fs_dio_test_imx_t *pTestedPin,
                                     fs_dio_test_imx_t *pAdjPin,
                                     bool_t testedPinValue,
                                     bool_t backupEnable)
{
    /* Return status */
    FS_RESULT st = FS_PASS;

    /* GPIO and PORT pointers definitions */
    volatile fs_gpio_imx8m_t *pPortTested = (fs_gpio_imx8m_t *)pTestedPin->gpio;
    volatile uint32_t *pMuxTested         = (uint32_t *)pTestedPin->muxAddr;
    volatile uint32_t *pPadTested         = (uint32_t *)pTestedPin->padAddr;

    volatile fs_gpio_imx8m_t *pPortAdjacent = (fs_gpio_imx8m_t *)pAdjPin->gpio;
    volatile uint32_t *pMuxAdjacent         = (uint32_t *)pAdjPin->muxAddr;
    volatile uint32_t *pPadAdjacent         = (uint32_t *)pAdjPin->padAddr;

    /* masks for tested and adjacent pins */
    uint32_t testedGpioMask   = (1UL << pTestedPin->pinNum);
    uint32_t adjacentGpioMask = (1UL << pAdjPin->pinNum);

    /* If enable, save the actual value of tested and adjacent pins, GPIO and PORT */
    if (backupEnable)
    {
        pTestedPin->sTestedPinBackup.mux  = *pMuxTested;
        pTestedPin->sTestedPinBackup.pad  = *pPadTested;
        pTestedPin->sTestedPinBackup.gdir = (pPortTested->GDIR & testedGpioMask);
        pTestedPin->sTestedPinBackup.dr   = (pPortTested->DR & testedGpioMask);

        pAdjPin->sTestedPinBackup.mux  = *pMuxAdjacent;
        pAdjPin->sTestedPinBackup.pad  = *pPadAdjacent;
        pAdjPin->sTestedPinBackup.gdir = (pPortAdjacent->GDIR & adjacentGpioMask);
        pAdjPin->sTestedPinBackup.dr   = (pPortAdjacent->DR & adjacentGpioMask);

        pPortTested->GDIR &= ~testedGpioMask;    /* set tested pin as input if backup functionality is enable, */
        pPortAdjacent->GDIR |= adjacentGpioMask; /* set adjacent pin as output if backup functionality is enable, */
                                                 /* if not, it is responsibility of application */
    }

    /* Check if pin is configured as input */
    if ((pPortTested->GDIR & testedGpioMask) != 0U)
    {
         st = FS_FAIL_DIO_INPUT;
    }

    /* check if Adjacent pin is configured as output */
    if ((pPortAdjacent->GDIR & adjacentGpioMask) == 0U)
    {
      if(st == FS_PASS){ /* To prevent rewrite of previsous error */
        st = FS_FAIL_DIO_OUTPUT;
      }
    }
    /* Set pull-up or pull-down on tested pin */
    *pPadTested &= ~(FS_IOMUXC_SW_PAD_CTL_PAD_PE_MASK | FS_IOMUXC_SW_PAD_CTL_PAD_PUE_MASK); /* Clear PE, PUE */
    if (testedPinValue)
    {
        /* Select Pull up, 22K Ohm  */
        *pPadTested |= (FS_IOMUXC_SW_PAD_CTL_PAD_PE_MASK | FS_IOMUXC_SW_PAD_CTL_PAD_PUE_MASK);
        pPortAdjacent->DR &= ~adjacentGpioMask; /* If tested pin is set to 1, adjacent pin is cleared to 0 in GPIO */
    }else{
        /* Select Pull down, 100K Ohm */
        *pPadTested |= FS_IOMUXC_SW_PAD_CTL_PAD_PE_MASK;
        pPortAdjacent->DR |=
            adjacentGpioMask; /* Otherwise, if testef pin is set to 0, adjacent pin is set to 1 in GPIO */
    }

    return st;
}

/*******************************************************************************
 ******************************************************************************/
FS_RESULT FS_DIO_InputExt_LPC(fs_dio_test_lpc_t *pTestedPin,
                              fs_dio_test_lpc_t *pAdjPin,
                              bool_t testedPinValue,
                              bool_t backupEnable)
{
    /* Return status */
    FS_RESULT st = FS_PASS;

    /* GPIO and PORT pointers definitions */
    volatile uint8_t *pByteTested   = (uint8_t *)pTestedPin->pPort_byte;
    volatile uint32_t *pDirTested   = (uint32_t *)pTestedPin->pPort_dir;
    volatile uint32_t *pIoconTested = (uint32_t *)pTestedPin->pPort_Iocon;

    volatile uint8_t *pByteAdjacent   = (uint8_t *)pAdjPin->pPort_byte;
    volatile uint32_t *pDirAdjacent   = (uint32_t *)pAdjPin->pPort_dir;
    volatile uint32_t *pIoconAdjacent = (uint32_t *)pAdjPin->pPort_Iocon;

    /* masks for tested and adjacent pins */
    uint32_t testedGpioMask = (1UL << pTestedPin->pinNum);
    uint32_t compareValue   = 0;

    if (testedPinValue)
    {
        compareValue = 1U;
    }

    /* Check if pin has "digimode" bit and this is enabled - only for part of LPC device,
       structure parameter iocon_digimode must be set by user in safety_test_items.c */
    if (pTestedPin->iocon_digimode)
    {
     if ( ((*pIoconTested)&FS_IOCON_PIO_DIGIMODE_MASK) == 0U)
       {
         st = FS_FAIL_DIO_MODE;
       }
    }

    /* check if pin is configured as input */
    if (((*pDirTested) & testedGpioMask) != 0U)
    {
      if(st == FS_PASS){ /* To prevent rewrite of previsous error */
         st = FS_FAIL_DIO_INPUT;
      }
    }
    /* check the pin value */
    if (((*pByteTested) ^ compareValue) != 0U)
    { // ^ xor
      if(st == FS_PASS){ /* To prevent rewrite of previsous error */
        st = FS_FAIL_DIO_WRONG_VALUE;
      }

    }

    if (backupEnable)
    {
        /* if enable, restore values of tested and adjacent Bit, DIR and IOCON */
        *pDirTested &= ~(pTestedPin->sTestedPinBackup.dir_backup);
        *pDirTested |= pTestedPin->sTestedPinBackup.dir_backup;   /* DIR is bit field => Must be MASK */
        *pByteTested  = pTestedPin->sTestedPinBackup.byte_backup; /* Pointer to concrete registessr */
        *pIoconTested = pTestedPin->sTestedPinBackup.iocon_backup;

        *pDirAdjacent &= ~(pAdjPin->sTestedPinBackup.dir_backup);
        *pDirAdjacent |= pAdjPin->sTestedPinBackup.dir_backup;   /* DIR is bit field => Must be MASK */
        *pByteAdjacent  = pAdjPin->sTestedPinBackup.byte_backup; /* Pointer to concrete registessr */
        *pIoconAdjacent = pAdjPin->sTestedPinBackup.iocon_backup;
    }

    return st;
}

/*******************************************************************************
 ******************************************************************************/
FS_RESULT FS_DIO_ShortToSupplySet_LPC(fs_dio_test_lpc_t *pTestedPin, bool_t shortToVoltage, bool_t backupEnable)
{
    /* Return status */
    FS_RESULT st = FS_PASS;

    /* GPIO and PORT pointers definitions */
    volatile uint8_t *pByteTested   = (uint8_t *)pTestedPin->pPort_byte;
    volatile uint32_t *pDirTested   = (uint32_t *)pTestedPin->pPort_dir;
    volatile uint32_t *pIoconTested = (uint32_t *)pTestedPin->pPort_Iocon;

    /* Pointer to correspond IOCON register */
    volatile uint32_t *pin_iocon = (uint32_t *)pTestedPin->pPort_Iocon;
    /* masks for tested  pins */
    uint32_t testedGpioMask = (1UL << pTestedPin->pinNum);

    if (backupEnable)
    {
        /* if enable, save the actual value of tested and adjacent BYTE, DIR and IOCON */
        pTestedPin->sTestedPinBackup.byte_backup  = (*pByteTested);
        pTestedPin->sTestedPinBackup.dir_backup   = (*pDirTested & testedGpioMask);
        pTestedPin->sTestedPinBackup.iocon_backup = (*pIoconTested);

        /* set tested pin as input if backup functionality is enable,
         * if not, it is responsibility of application */
        *pDirTested &= ~testedGpioMask;
    }

    /* Check if pin has "digimode" bit and this is enabled - only for part of LPC device,
       structure parameter iocon_digimode must be set by user in safety_test_items.c */
    if (pTestedPin->iocon_digimode)
    {
     if ( ((*pin_iocon)&FS_IOCON_PIO_DIGIMODE_MASK) == 0)
       {
            st = FS_FAIL_DIO_MODE;
       }
    }

    /* check if pin is configured as input */
    if (((*pDirTested) & testedGpioMask) != 0U)
    {
      if(st == FS_PASS){ /* To prevent rewrite of previsous error */
        st = FS_FAIL_DIO_INPUT;
      }
    }
    /* set pull-up or pull-down on tested pin  */
    *pIoconTested &= ~(FS_IOCON_PIO_2BIT_MASK << pTestedPin->iocon_mode_shift);

    if (shortToVoltage)
    {
        *pIoconTested |=
            (FS_IOCON_PIO_PULL_UP << pTestedPin->iocon_mode_shift); /* if it is test against Gnd, sets pull-up */
    }else{
        *pIoconTested |=
            (FS_IOCON_PIO_PULL_DOWN << pTestedPin->iocon_mode_shift); /* if it is test against VCC, sets pull-down */
    }

    return st;
}

/*******************************************************************************
 ******************************************************************************/
FS_RESULT FS_DIO_ShortToAdjSet_LPC(fs_dio_test_lpc_t *pTestedPin,
                                   fs_dio_test_lpc_t *pAdjPin,
                                   bool_t testedPinValue,
                                   bool_t backupEnable)
{
    /* Return status */
    FS_RESULT st = FS_PASS;

    /* GPIO and PORT pointers definitions */
    volatile uint8_t *pByteTested   = (uint8_t *)pTestedPin->pPort_byte;
    volatile uint32_t *pDirTested   = (uint32_t *)pTestedPin->pPort_dir;
    volatile uint32_t *pIoconTested = (uint32_t *)pTestedPin->pPort_Iocon;

    volatile uint8_t *pByteAdjacent = (uint8_t *)pAdjPin->pPort_byte;
    volatile uint32_t *pDirAdjacent = (uint32_t *)pAdjPin->pPort_dir;
    volatile uint32_t *pIoconAdjacent = (uint32_t *)pAdjPin->pPort_Iocon;

    /* Pointer to correspond IOCON register */
    volatile uint32_t *pin_iocon = (uint32_t *)pTestedPin->pPort_Iocon;

    /* masks for tested and adjacent pins */
    uint32_t testedGpioMask   = (1UL << pTestedPin->pinNum);
    uint32_t adjacentGpioMask = (1UL << pAdjPin->pinNum);

    if (backupEnable)
    { /* if enable, save the actual value of tested and adjacent BYTE, DIR and IOCON */
        pTestedPin->sTestedPinBackup.byte_backup  = (*pByteTested);
        pTestedPin->sTestedPinBackup.dir_backup   = (*pDirTested & testedGpioMask);
        pTestedPin->sTestedPinBackup.iocon_backup = (*pIoconTested);

        pAdjPin->sTestedPinBackup.byte_backup  = (*pByteAdjacent);
        pAdjPin->sTestedPinBackup.dir_backup   = (*pDirAdjacent & adjacentGpioMask);
        pAdjPin->sTestedPinBackup.iocon_backup = (*pIoconTested);

        /* set tested pin as input if backup functionality is enable,
         * set adjacent pin as output if backup functionality is enable,
         * if not, it is responsibility of application */
        *pDirTested &= ~testedGpioMask;
        *pDirAdjacent |= adjacentGpioMask;
    }

     /* Check if pin has "digimode" bit and this is enabled - only for part of LPC device,
     structure parameter iocon_digimode must be set by user in safety_test_items.c */
     if (pTestedPin->iocon_digimode)
    {
     if ( ((*pin_iocon)&FS_IOCON_PIO_DIGIMODE_MASK) == 0)
       {
            st = FS_FAIL_DIO_MODE;
       }
    }

    /* Check if pin has "digimode" bit and this is enabled - only for part of LPC device,
    structure parameter iocon_digimode must be set by user in safety_test_items.c */
    if (pAdjPin->iocon_digimode)
    {
     if ( ((*pIoconAdjacent)&FS_IOCON_PIO_DIGIMODE_MASK) == 0)
       {
         st = FS_FAIL_DIO_MODE;
       }
    }

    /* check if pin is configured as input */
    if (((*pDirTested) & testedGpioMask) != 0U)
    {
      if(st == FS_PASS){ /* To prevent rewrite of previsous error */
        st = FS_FAIL_DIO_INPUT;
      }
    }

        /* check if Adjacent pin is configured as output */
    if (((*pDirAdjacent) & adjacentGpioMask) == 0U)
    {
      if(st == FS_PASS){ /* To prevent rewrite of previsous error */
        st = FS_FAIL_DIO_OUTPUT;
      }
    }

    /* set pull-up or pull-down on tested pin vs. output 1 or output 0 on adjacent pin */
    *pIoconTested &= ~(FS_IOCON_PIO_2BIT_MASK << pTestedPin->iocon_mode_shift);
    if (testedPinValue)
    {
        *pIoconTested |= (FS_IOCON_PIO_PULL_UP
                          << pTestedPin->iocon_mode_shift); /* if the tested pin value is logical 1, set pull-up */
        *pByteAdjacent = 0U; /* if tested pin is set to 1, adjacent pin is cleared to 0 in GPIO */
    }else{
        *pIoconTested |= (FS_IOCON_PIO_PULL_DOWN
                          << pTestedPin->iocon_mode_shift); /* if the tested pin value is logical 0, set pull-DOWN */
        *pByteAdjacent = 1U; /* otherwise, if testef pin is set to 0, adjacent pin is set to 1 in GPIO */
    }

    return st;
}

/*******************************************************************************
 ******************************************************************************/
FS_RESULT FS_DIO_InputExt_MCX(fs_dio_test_t *pTestedPin, fs_dio_test_t *pAdjPin, bool_t testedPinValue, bool_t backupEnable)
{
    /* Return status */
    FS_RESULT st = FS_PASS;

    /* GPIO and PORT pointers definitions */
    volatile fs_rgpio_imxrt_t *pPortTested   = (fs_rgpio_imxrt_t *)pTestedPin->gpio;
    volatile fs_pcr_t *pPcrTested     = (fs_pcr_t *)pTestedPin->pcr;
    volatile fs_rgpio_imxrt_t *pPortAdjacent = (fs_rgpio_imxrt_t *)pAdjPin->gpio;
    volatile fs_pcr_t *pPcrAdjacent   = (fs_pcr_t *)pAdjPin->pcr;

    /* masks for tested and adjacent pins */
    uint32_t testedGpioMask   = (1UL << pTestedPin->pinNum);
    uint32_t adjacentGpioMask = (1UL << pAdjPin->pinNum);
    uint32_t compareValue     = 0U;

    if (testedPinValue)
    {
        compareValue = testedGpioMask;
    }
    
    /* check if pin is configured as input */
    if ((pPortTested->PDDR & testedGpioMask) != 0U)
    {
         st = FS_FAIL_DIO_INPUT;
    }

    /* check the pin value */
    if (((pPortTested->PDIR & testedGpioMask) ^ compareValue) != 0U)
    { // ^ xor
      if(st == FS_PASS){ /* To prevent rewrite of previsous error */
        st = FS_FAIL_DIO_WRONG_VALUE;
      }
    }

    if (backupEnable)
    { /* if enable, restore values of tested and adjacent pins, GPIO and PORT */
        pPcrTested->PCR[pTestedPin->pinNum] = pTestedPin->sTestedPinBackup.pcr;

        if (pTestedPin->sTestedPinBackup.pddr != 0U)
        {
            pPortTested->PDDR |= pTestedPin->sTestedPinBackup.pddr;
        }else{
            pPortTested->PDDR &= ~pTestedPin->sTestedPinBackup.pddr;
        }

        if (pTestedPin->sTestedPinBackup.pdor != 0U)
        {
            pPortTested->PSOR = testedGpioMask; /* Set pin value */
        }else{
            pPortTested->PCOR = testedGpioMask; /* Clear pin value */
        }

        pPcrAdjacent->PCR[pAdjPin->pinNum] = pAdjPin->sTestedPinBackup.pcr;

        if (pAdjPin->sTestedPinBackup.pddr != 0U)
        {
            pPortAdjacent->PDDR |= pAdjPin->sTestedPinBackup.pddr;
        }else{
            pPortAdjacent->PDDR &= ~pAdjPin->sTestedPinBackup.pddr;
        }

        if (pAdjPin->sTestedPinBackup.pdor != 0U)
        {
            pPortAdjacent->PSOR = adjacentGpioMask; /* Set pin value */
        }else{
            pPortAdjacent->PCOR = adjacentGpioMask; /* Clear pin value */
        }
    }

    return st;
}

/*******************************************************************************
 ******************************************************************************/
FS_RESULT FS_DIO_ShortToSupplySet_MCX(fs_dio_test_t *pTestedPin, bool_t shortToVoltage, bool_t backupEnable)
{
    /* Return status */
    FS_RESULT st = FS_PASS;

    /* GPIO and PORT pointers definitions */
    volatile fs_rgpio_imxrt_t *pPortTested = (fs_rgpio_imxrt_t *)pTestedPin->gpio;
    volatile fs_pcr_t *pPcrTested   = (fs_pcr_t *)pTestedPin->pcr;

    uint8_t testPinNum = pTestedPin->pinNum;

    /* mask for tested pin */
    uint32_t testedGpioMask = (1UL << testPinNum);

    /* temp variable for original PORT register content */
    volatile uint32_t testedPcrValue;

    /* if enable, save the actual value of tested pin, GPIO and PORT */
    if (backupEnable)
    {
        pTestedPin->sTestedPinBackup.pcr  = pPcrTested->PCR[testPinNum];
        pTestedPin->sTestedPinBackup.pddr = (pPortTested->PDDR & testedGpioMask);
        pTestedPin->sTestedPinBackup.pdor = (pPortTested->PDOR & testedGpioMask);

        /* set tested pin as input if backup functionality is enable,
         * if not, it is responsibility of application */
        pPortTested->PDDR &= ~testedGpioMask;
    }

    /* check if pin is configured as input */
    if ((pPortTested->PDDR & testedGpioMask) != 0U)
    {
         st = FS_FAIL_DIO_INPUT;
    }

    testedPcrValue = pPcrTested->PCR[testPinNum]; /* value from tested pin register in PORT */
    testedPcrValue &= ~(PORT_PCR_PS_MSK | PORT_PCR_PE_MSK); /* unmask PS, PE in PORT */
    pPcrTested->PCR[testPinNum] = (testedPcrValue | PORT_PCR_PE_MSK | PORT_PCR_IBE_MASK); /* set pull-up or pull-down enable in PORT  */

    /* set pull-up or pull-down on tested pin  */
    if (shortToVoltage)
    {
        /* if it is test against Gnd, sets pull-up */
        pPcrTested->PCR[testPinNum] |= PORT_PCR_PS_MSK;
    }

    return st;
}

/*******************************************************************************
 ******************************************************************************/
FS_RESULT FS_DIO_ShortToAdjSet_MCX(fs_dio_test_t *pTestedPin,
                               fs_dio_test_t *pAdjPin,
                               bool_t testedPinValue,
                               bool_t backupEnable)
{
    /* Return status */
    FS_RESULT st = FS_PASS;

    /* GPIO and PORT pointers definitions */
    volatile fs_rgpio_imxrt_t *pPortTested   = (fs_rgpio_imxrt_t *)pTestedPin->gpio;
    volatile fs_pcr_t *pPcrTested     = (fs_pcr_t *)pTestedPin->pcr;
    volatile fs_rgpio_imxrt_t *pPortAdjacent = (fs_rgpio_imxrt_t *)pAdjPin->gpio;
    volatile fs_pcr_t *pPcrAdjacent   = (fs_pcr_t *)pAdjPin->pcr;

    uint8_t testPinNum = pTestedPin->pinNum;

    /* masks for tested and adjacent pins */
    uint32_t testedGpioMask   = (1UL << testPinNum);
    uint32_t adjacentGpioMask = (1UL << pAdjPin->pinNum);

    /* temp variable for original PORT register content */
    volatile uint32_t testedPcrValue;

    /* if enable, save the actual value of tested and adjacent pins, GPIO and PORT */
    if (backupEnable)
    {
        pTestedPin->sTestedPinBackup.pcr  = pPcrTested->PCR[testPinNum];
        pTestedPin->sTestedPinBackup.pddr = (pPortTested->PDDR & testedGpioMask);
        pTestedPin->sTestedPinBackup.pdor = (pPortTested->PDOR & testedGpioMask);
        pAdjPin->sTestedPinBackup.pcr     = pPcrAdjacent->PCR[testPinNum];
        pAdjPin->sTestedPinBackup.pddr    = (pPortAdjacent->PDDR & adjacentGpioMask);
        pAdjPin->sTestedPinBackup.pdor    = (pPortAdjacent->PDOR & adjacentGpioMask);

        /* set tested pin as input if backup functionality is enable,
         * set adjacent pin as output if backup functionality is enable,
         * if not, it is responsibility of application */
        pPortTested->PDDR &= ~testedGpioMask;
        pPortAdjacent->PDDR |= adjacentGpioMask;
    }

    /* check if pin is configured as input */
    if ((pPortTested->PDDR & testedGpioMask) != 0U)
    {
         st = FS_FAIL_DIO_INPUT;
    }

    /* check if Adjacent pin is configured as output */
    if ((pPortAdjacent->PDDR & adjacentGpioMask) == 0U)
    {
      if(st == FS_PASS){ /* To prevent rewrite of previsous error */
        st = FS_FAIL_DIO_OUTPUT;
      }
    }

    testedPcrValue = pPcrTested->PCR[testPinNum]; /* value from tested pin register in PORT */
    testedPcrValue &= ~(PORT_PCR_PS_MSK | PORT_PCR_PE_MSK); /* unmask PS, PE in PORT */
    pPcrTested->PCR[testPinNum] = (testedPcrValue | PORT_PCR_PE_MSK | PORT_PCR_IBE_MASK); /* set pull-up or pull-down enable in PORT  */

    /* set pull-up or pull-down on tested pin vs. output 1 or output 0 on adjacent pin */
    if (testedPinValue)
    {
        pPcrTested->PCR[testPinNum] |= PORT_PCR_PS_MSK; /* if the tested pin value is logical 1, set pull-up, otherwise pull-down remains set */
        pPortAdjacent->PCOR = adjacentGpioMask; /* if tested pin is set to 1, adjacent pin is cleared to 0 in GPIO */
    }
    else
    {
        /* otherwise, if testef pin is set to 0, adjacent pin is set to 1 in GPIO */
        pPortAdjacent->PSOR = adjacentGpioMask;
    }

    return st;
}

/*******************************************************************************
 ******************************************************************************/
FS_RESULT FS_DIO_InputExt_RGPIO(fs_dio_test_rgpio_t *pTestedPin, fs_dio_test_rgpio_t *pAdjPin, bool_t testedPinValue, bool_t backupEnable)
{
    /* Return status */
    FS_RESULT st = FS_PASS;

    /* GPIO and PORT pointers definitions */
    volatile fs_rgpio_imxrt_t *pPortTested   = (fs_rgpio_imxrt_t *)pTestedPin->gpio;
    volatile fs_rgpio_imxrt_t *pPortAdjacent = (fs_rgpio_imxrt_t *)pAdjPin->gpio;

    /* masks for tested and adjacent pins */
    uint32_t testedGpioMask   = (1UL << pTestedPin->pinNum);
    uint32_t adjacentGpioMask = (1UL << pAdjPin->pinNum);
    uint32_t compareValue     = 0U;

    if (testedPinValue)
    {
        compareValue = testedGpioMask;
    }

    /* check if pin is configured as input */
    if ((pPortTested->PDDR & testedGpioMask) != 0U)
    {
         st = FS_FAIL_DIO_INPUT;
    }

    /* check the pin value */
    if (((pPortTested->PDIR & testedGpioMask) ^ compareValue) != 0U)
    { // ^ xor
      if(st == FS_PASS){ /* To prevent rewrite of previsous error */
        st = FS_FAIL_DIO_WRONG_VALUE;
      }
    }

    if (backupEnable)
    { /* if enable, restore values of tested and adjacent pins, GPIO and PORT */

        if (pTestedPin->sTestedPinBackup.pddr != 0U)
        {
            pPortTested->PDDR |= pTestedPin->sTestedPinBackup.pddr;
        }else{
            pPortTested->PDDR &= ~pTestedPin->sTestedPinBackup.pddr;
        }

        if (pTestedPin->sTestedPinBackup.pdor != 0U)
        {
            pPortTested->PSOR = testedGpioMask; /* Set pin value */
        }else{
            pPortTested->PCOR = testedGpioMask; /* Clear pin value */
        }


        if (pAdjPin->sTestedPinBackup.pddr != 0U)
        {
            pPortAdjacent->PDDR |= pAdjPin->sTestedPinBackup.pddr;
        }else{
            pPortAdjacent->PDDR &= ~pAdjPin->sTestedPinBackup.pddr;
        }

        if (pAdjPin->sTestedPinBackup.pdor != 0U)
        {
            pPortAdjacent->PSOR = adjacentGpioMask; /* Set pin value */
        }else{
            pPortAdjacent->PCOR = adjacentGpioMask; /* Clear pin value */
        }
    }

    return st;
}

/*******************************************************************************
 ******************************************************************************/
FS_RESULT FS_DIO_ShortToSupplySet_RGPIO(fs_dio_test_rgpio_t *pTestedPin, bool_t shortToVoltage, bool_t backupEnable)
{
    /* Return status */
    FS_RESULT st = FS_PASS;

    /* GPIO and PORT pointers definitions */
    volatile fs_rgpio_imxrt_t *pPortTested = (fs_rgpio_imxrt_t *)pTestedPin->gpio;
    volatile uint32_t *pMuxTested         = (uint32_t *)pTestedPin->muxAddr;
    volatile uint32_t *pPadTested         = (uint32_t *)pTestedPin->padAddr;

    uint8_t testPinNum = pTestedPin->pinNum;

    /* mask for tested pin */
    uint32_t testedGpioMask = (1UL << testPinNum);

    /* if enable, save the actual value of tested pin, GPIO and PORT */
    if (backupEnable)
    {
        pTestedPin->sTestedPinBackup.mux  = *pMuxTested;
        pTestedPin->sTestedPinBackup.pad  = *pPadTested;
        pTestedPin->sTestedPinBackup.pddr = (pPortTested->PDDR & testedGpioMask);
        pTestedPin->sTestedPinBackup.pdor = (pPortTested->PDOR & testedGpioMask);

        /* set tested pin as input if backup functionality is enable,
         * if not, it is responsibility of application */
        pPortTested->PDDR &= ~testedGpioMask;
    }

    /* check if pin is configured as input */
    if ((pPortTested->PDDR & testedGpioMask) != 0U)
    {
         st = FS_FAIL_DIO_INPUT;
    }

    /* Set pull-up or pull-down on tested pin */
    *pPadTested &= ~pTestedPin->pullResetMask; /* Clear PUS, PUE */
    if (shortToVoltage)
    {
        *pPadTested |= pTestedPin->pullUpMask;   /* Select Pull up, 22K Ohm  */
    }
    else
    {
        *pPadTested |= pTestedPin->pullDownMask; /* Select Pull down, 100K Ohm */
    }

    return st;
}

/*******************************************************************************
 ******************************************************************************/
FS_RESULT FS_DIO_ShortToAdjSet_RGPIO(fs_dio_test_rgpio_t *pTestedPin,
                                     fs_dio_test_rgpio_t *pAdjPin,
                                     bool_t testedPinValue,
                                     bool_t backupEnable)
{
    /* Return status */
    FS_RESULT st = FS_PASS;

    /* GPIO and PORT pointers definitions */
    volatile fs_rgpio_imxrt_t *pPortTested   = (fs_rgpio_imxrt_t *)pTestedPin->gpio;
    volatile uint32_t *pMuxTested         = (uint32_t *)pTestedPin->muxAddr;
    volatile uint32_t *pPadTested            = (uint32_t *)pTestedPin->padAddr;

    volatile fs_rgpio_imxrt_t *pPortAdjacent = (fs_rgpio_imxrt_t *)pAdjPin->gpio;
    volatile uint32_t *pMuxAdjacent         = (uint32_t *)pAdjPin->muxAddr;
    volatile uint32_t *pPadAdjacent          = (uint32_t *)pAdjPin->padAddr;

    uint8_t testPinNum = pTestedPin->pinNum;

    /* masks for tested and adjacent pins */
    uint32_t testedGpioMask   = (1UL << testPinNum);
    uint32_t adjacentGpioMask = (1UL << pAdjPin->pinNum);

    /* if enable, save the actual value of tested and adjacent pins, GPIO and PORT */
    if (backupEnable)
    {
        pTestedPin->sTestedPinBackup.mux  = *pMuxTested;
        pTestedPin->sTestedPinBackup.pad  = *pPadTested;
        pTestedPin->sTestedPinBackup.pddr = (pPortTested->PDDR & testedGpioMask);
        pTestedPin->sTestedPinBackup.pdor = (pPortTested->PDOR & testedGpioMask);

        pAdjPin->sTestedPinBackup.mux  = *pMuxAdjacent;
        pAdjPin->sTestedPinBackup.pad  = *pPadAdjacent;
        pAdjPin->sTestedPinBackup.pddr    = (pPortAdjacent->PDDR & adjacentGpioMask);
        pAdjPin->sTestedPinBackup.pdor    = (pPortAdjacent->PDOR & adjacentGpioMask);

        /* set tested pin as input if backup functionality is enable,
         * set adjacent pin as output if backup functionality is enable,
         * if not, it is responsibility of application */
        pPortTested->PDDR &= ~testedGpioMask;
        pPortAdjacent->PDDR |= adjacentGpioMask;
    }

    /* check if tested pin is configured as input */
    if ((pPortTested->PDDR & testedGpioMask) != 0U)
    {
         st = FS_FAIL_DIO_INPUT;
    }

    /* check if adjacent pin is configured as output */
    if ((pPortAdjacent->PDDR & adjacentGpioMask) == 0U)
    {
      if(st == FS_PASS){ /* To prevent rewrite of previsous error */
        st = FS_FAIL_DIO_OUTPUT;
      }
    }

    /* Set pull-up or pull-down on tested pin */
    *pPadTested &= ~pTestedPin->pullResetMask; /* Clear PUS, PUE */
    if (testedPinValue)
    {
        *pPadTested |= pTestedPin->pullUpMask;  /* Select Pull up, 22K Ohm  */
        pPortAdjacent->PCOR = adjacentGpioMask; /* if tested pin is set to 1, adjacent pin is cleared to 0 in GPIO */
    }
    else
    {
        *pPadTested |= pTestedPin->pullDownMask; /* Select Pull down, 100K Ohm */
        pPortAdjacent->PSOR = adjacentGpioMask;  /* otherwise, if testef pin is set to 0, adjacent pin is set to 1 in GPIO */
    }

    return st;
}

/*******************************************************************************
 ******************************************************************************/
FS_RESULT FS_DIO_InputExt_SIUL2(fs_dio_test_siul2_t *pTestedPin, fs_dio_test_siul2_t *pAdjPin, bool_t testedPinValue, bool_t backupEnable)
{
    /* Return status */
    FS_RESULT st = FS_PASS;

    /* SIUL2 pointers definitions */
    volatile fs_siul2_t *pPortTested   = (fs_siul2_t *)pTestedPin->siul2;
    volatile fs_siul2_t *pPortAdjacent = (fs_siul2_t *)pAdjPin->siul2;

    /* check if tested pin is configured as input */
    if ((pPortTested->MSCR[pTestedPin->pinNum] & SIUL2_MSCR_IBE_MASK) == 0)
    {
         st = FS_FAIL_DIO_INPUT;
    }

    /* check the pin value */
    if ((pPortTested->GPDI[pTestedPin->pinNumGPDxN]) != (uint8_t)testedPinValue)
    {
      if(st == FS_PASS){ /* To prevent rewrite of previsous error */
        st = FS_FAIL_DIO_WRONG_VALUE;
      }
    }

    /* if enable, restore values of tested and adjacent pins */
    if (backupEnable)
    { 
        pPortTested->MSCR[pTestedPin->pinNum] = pTestedPin->sTestedPinBackup.mscr;
        pPortTested->GPDO[pTestedPin->pinNumGPDxN] = pTestedPin->sTestedPinBackup.gpdo;
        
        pPortAdjacent->MSCR[pAdjPin->pinNum] = pAdjPin->sTestedPinBackup.mscr;
        pPortAdjacent->GPDO[pAdjPin->pinNumGPDxN] = pAdjPin->sTestedPinBackup.gpdo;
    }

    return st;
}

/*******************************************************************************
 ******************************************************************************/
FS_RESULT FS_DIO_ShortToSupplySet_SIUL2(fs_dio_test_siul2_t *pTestedPin, bool_t shortToVoltage, bool_t backupEnable)
{
    /* Return status */
    FS_RESULT st = FS_PASS;

    /* SIUL2 pointers definitions */
    volatile fs_siul2_t *pPortTested = (fs_siul2_t *)pTestedPin->siul2;

    /* if enable, save the actual value of tested pin registers */
    if (backupEnable)
    {       
        pTestedPin->sTestedPinBackup.mscr = pPortTested->MSCR[pTestedPin->pinNum];
        pTestedPin->sTestedPinBackup.gpdo = pPortTested->GPDO[pTestedPin->pinNumGPDxN];

        /* set tested pin as input if backup functionality is enable,
         * if not, it is responsibility of application */       
        pPortTested->MSCR[pTestedPin->pinNum] &= ~SIUL2_MSCR_OBE_MASK;
        pPortTested->MSCR[pTestedPin->pinNum] |= SIUL2_MSCR_IBE_MASK;
    }

    /* check if pin is configured as input */
    if ((pPortTested->MSCR[pTestedPin->pinNum] & SIUL2_MSCR_IBE_MASK) == 0)
    {
         st = FS_FAIL_DIO_INPUT;
    }

    /* set pull-up or pull-down on tested pin  */
    if (shortToVoltage)
    {
        /* if it is test against Gnd, sets pull-up */        
        pPortTested->MSCR[pTestedPin->pinNum] |= SIUL2_MSCR_PUS_MASK;
        pPortTested->MSCR[pTestedPin->pinNum] |= SIUL2_MSCR_PUE_MASK;
    }
    else
    {
        /* set pull-down resistor */        
        pPortTested->MSCR[pTestedPin->pinNum] &= ~SIUL2_MSCR_PUS_MASK;
        pPortTested->MSCR[pTestedPin->pinNum] |= SIUL2_MSCR_PUE_MASK;
    }

    return st;
}

/*******************************************************************************
 ******************************************************************************/
FS_RESULT FS_DIO_ShortToAdjSet_SIUL2(fs_dio_test_siul2_t *pTestedPin,
                               fs_dio_test_siul2_t *pAdjPin,
                               bool_t testedPinValue,
                               bool_t backupEnable)
{
    /* Return status */
    FS_RESULT st = FS_PASS;

    /* SIUL2 pointers definitions */
    volatile fs_siul2_t *pPortTested   = (fs_siul2_t *)pTestedPin->siul2;
    volatile fs_siul2_t *pPortAdjacent = (fs_siul2_t *)pAdjPin->siul2;

    /* if enable, save the actual value of tested and adjacent pin registers */
    if (backupEnable)
    {
        pTestedPin->sTestedPinBackup.mscr = pPortTested->MSCR[pTestedPin->pinNum];
        pTestedPin->sTestedPinBackup.gpdo = pPortTested->GPDO[pTestedPin->pinNumGPDxN];
        
        pAdjPin->sTestedPinBackup.mscr = pPortAdjacent->MSCR[pAdjPin->pinNum];
        pAdjPin->sTestedPinBackup.gpdo = pPortAdjacent->GPDO[pAdjPin->pinNumGPDxN];

        /* set tested pin as input if backup functionality is enable,
         * set adjacent pin as output if backup functionality is enable,
         * if not, it is responsibility of application */    
        pPortTested->MSCR[pTestedPin->pinNum] &= ~SIUL2_MSCR_OBE_MASK;
        pPortTested->MSCR[pTestedPin->pinNum] |= SIUL2_MSCR_IBE_MASK;
        
        pPortAdjacent->MSCR[pAdjPin->pinNum] &= ~SIUL2_MSCR_IBE_MASK;
        pPortAdjacent->MSCR[pAdjPin->pinNum] |= SIUL2_MSCR_OBE_MASK;
    }

    /* check if tested pin is configured as input */
    if ((pPortTested->MSCR[pTestedPin->pinNum] & SIUL2_MSCR_IBE_MASK) == 0)
    {
         st = FS_FAIL_DIO_INPUT;
    }

    /* check if adjacent pin is configured as output */
    if ((pPortAdjacent->MSCR[pAdjPin->pinNum] & SIUL2_MSCR_OBE_MASK) == 0)
    {
      if(st == FS_PASS){ /* To prevent rewrite of previsous error */
        st = FS_FAIL_DIO_OUTPUT;
      }
    }

    /* set pull-up or pull-down on tested pin vs. output 1 or output 0 on adjacent pin */
    if (testedPinValue)
    {
        /* if the tested pin value is logical 1, set pull-up, otherwise pull-down remains set */
        pPortTested->MSCR[pTestedPin->pinNum] |= SIUL2_MSCR_PUS_MASK;
        pPortTested->MSCR[pTestedPin->pinNum] |= SIUL2_MSCR_PUE_MASK;
        
        /* if tested pin is set to 1, adjacent pin is cleared to 0 in GPIO */
        pPortAdjacent->GPDO[pAdjPin->pinNumGPDxN] = 0U;
    }
    else
    {
        /* otherwise, if testef pin is set to 0, adjacent pin is set to 1 in GPIO */
        pPortAdjacent->GPDO[pAdjPin->pinNumGPDxN] = 1U;
    }

    return st;
}