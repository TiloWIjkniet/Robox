/*
 * Copyright 2015 Freescale Semiconductor, Inc.
 * Copyright 2015-2021, 2025 NXP
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
 * @brief Digital I/O test common routines - IEC60730 Class B.
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
FS_RESULT FS_DIO_Input(fs_dio_test_t *pTestedPin, bool_t expectedValue)
{
    /* Return status */
    FS_RESULT st = FS_PASS;

    /* Select GPIO port */
    volatile fs_gpio_t *pPort = (fs_gpio_t *)pTestedPin->gpio;

    uint32_t testVal = 0U;
    uint16_t pinNum  = pTestedPin->pinNum;

    if (expectedValue)
    {
        testVal = (1UL << pinNum);
    }

    /* check if pin is configured as input */
    if ((pPort->PDDR & (1UL << pinNum)) != 0U)
    {
      st = FS_FAIL_DIO_INPUT;
    }

    /* check the pin value */
    if (((pPort->PDIR & (1UL << pinNum)) ^ testVal) != 0U)
    {
      if(st == FS_PASS){ /* To prevent rewrite of previsous error */
        st = FS_FAIL_DIO_WRONG_VALUE;
      }
    }

    return st;
}

/*******************************************************************************
 ******************************************************************************/
FS_RESULT FS_DIO_Output(fs_dio_test_t *pTestedPin, uint32_t delay)
{
    /* Return status */
    FS_RESULT st = FS_PASS;

    /* select GPIO port */
    volatile fs_gpio_t *pPort = (fs_gpio_t *)pTestedPin->gpio;

    volatile uint32_t portValue;
    volatile uint32_t i;
    uint32_t portMask = (1UL << pTestedPin->pinNum);

    portValue = (pPort->PDOR & portMask);

    /* check if pin is configured as output */
    if ((pPort->PDDR & portMask) == 0U)
    {
      st = FS_FAIL_DIO_OUTPUT;
    }
    /* set pin to 1 */
    pPort->PSOR = portMask;
    for (i = 0U; i < delay; i++)
    {
        ;
    }

    /* check if pin is set */
    if ((pPort->PDIR & portMask) == 0U)
    {
      if (portValue == 0U){
        pPort->PCOR = portMask;
      }
      if(st == FS_PASS){ /* To prevent rewrite of previsous error */
        st = FS_FAIL_DIO_NOT_SET;
      }
    }

    /* set pin to 0 */
    pPort->PCOR = portMask;
    for (i = 0U; i < delay; i++)
    {
        ;
    }

    /* check if pin is cleared */
    if ((pPort->PDIR & portMask) != 0U)
    {
       if(st == FS_PASS){ /* To prevent rewrite of previsous error */
        st = FS_FAIL_DIO_NOT_CLEAR;
       }
    }

    /* set original value of the pin */
    if (portValue != 0U)
    {
        pPort->PSOR = portMask;
    }
    return st;
}

/*******************************************************************************
 ******************************************************************************/
FS_RESULT FS_DIO_Output_IMXRT(fs_dio_test_imx_t *pTestedPin, uint32_t delay)
{
    /* Return status */
    FS_RESULT st = FS_PASS;

    /* Pointer to GPIO module from argument address */
    volatile fs_gpio_imxrt_t *pGpio = (fs_gpio_imxrt_t *)pTestedPin->gpio;

    uint32_t portMask           = (1UL << pTestedPin->pinNum);
    volatile uint32_t portValue = (pGpio->PSR & portMask);
    volatile uint32_t i;

    /* Check if pin is configured as output */
    if ((pGpio->GDIR & portMask) == 0U)
    {
      st = FS_FAIL_DIO_OUTPUT;
    }
    /* Set pin to 1 */
    pGpio->DR_SET = portMask;
    for (i = 0U; i < delay; i++)
    {
        ;
    }

    /* Check if pin is set */
    if ((pGpio->PSR & portMask) == 0U)
    {
      if(st == FS_PASS){ /* To prevent rewrite of previsous error */
        st = FS_FAIL_DIO_NOT_SET;
      }
    }

    /* Set pin to 0 */
    pGpio->DR_CLEAR = portMask;
    for (i = 0U; i < delay; i++)
    {
        ;
    }

    /* Check if pin is cleared = short to VCC circuit */
    if ((pGpio->PSR & portMask) != 0U)
    {
       if(st == FS_PASS){ /* To prevent rewrite of previsous error */
        st = FS_FAIL_DIO_NOT_CLEAR;
       }
    }

    /* Set original value of the pin */
    if (portValue != 0U)
    {
        pGpio->DR_SET = portMask;
    }
    return st;
}

/*******************************************************************************
 ******************************************************************************/
FS_RESULT FS_DIO_Output_IMX8M(fs_dio_test_imx_t *pTestedPin, uint32_t delay)
{
    /* Return status */
    FS_RESULT st = FS_PASS;

    /* Pointer to GPIO module from argument address */
    volatile fs_gpio_imx8m_t *pGpio = (fs_gpio_imx8m_t *)pTestedPin->gpio;

    uint32_t portMask           = (1UL << pTestedPin->pinNum);
    volatile uint32_t portValue = (pGpio->PSR & portMask);
    volatile uint32_t i;

    /* Check if pin is configured as output */
    if ((pGpio->GDIR & portMask) == 0U)
    {
      st = FS_FAIL_DIO_OUTPUT;
    }
    /* Set pin to 1 */
    pGpio->DR |= portMask;
    for (i = 0U; i < delay; i++)
    {
        ;
    }

    /* Check if pin is set */
    if ((pGpio->PSR & portMask) == 0U)
    {
      if(st == FS_PASS){ /* To prevent rewrite of previsous error */
        st = FS_FAIL_DIO_NOT_SET;
      }
    }

    /* Set pin to 0 */
    pGpio->DR &= ~portMask;
    for (i = 0U; i < delay; i++)
    {
        ;
    }

    /* Check if pin is cleared = short to VCC circuit */
    if ((pGpio->PSR & portMask) != 0U)
    {
       if(st == FS_PASS){ /* To prevent rewrite of previsous error */
        st = FS_FAIL_DIO_NOT_CLEAR;
       }
    }

    /* Set original value of the pin */
    if (portValue != 0U)
    {
        pGpio->DR |= portMask;
    }

    return st;
}

/*******************************************************************************
 ******************************************************************************/
FS_RESULT FS_DIO_Output_LPC(fs_dio_test_lpc_t *pTestedPin, uint32_t delay)
{
    /* Return status */
    FS_RESULT st = FS_PASS;

    volatile uint8_t portValue;
    volatile uint32_t i;
    uint32_t portMask = (1UL << pTestedPin->pinNum);
    volatile uint32_t *pin_iocon = (uint32_t *)pTestedPin->pPort_Iocon;

    /* select pointer to register */
    volatile uint8_t *pByte = pTestedPin->pPort_byte;
    volatile uint32_t *pDir = pTestedPin->pPort_dir;

    /* Check if pin has "digimode" bit and this is enabled - only for part of LPC device,
       structure parameter iocon_digimode must be set by user in safety_test_items.c */
    if (pTestedPin->iocon_digimode)
    {
     if ( ((*pin_iocon)&FS_IOCON_PIO_DIGIMODE_MASK) == 0U)
       {
            st = FS_FAIL_DIO_MODE;
       }

    }

    /* check if pin is configured as output */
    if (((*pDir) & (portMask)) == 0U)
    {
      if(st == FS_PASS){ /* To prevent rewrite of previsous error */
      st = FS_FAIL_DIO_OUTPUT;
      }
    }

    /* Back up the pin value */
    portValue = (*pByte);

    /* set pin to 1 */
    *pByte = 1U;

    /* Delay is necessary due to GPIO peripheral speed */
    for (i = 0; i < delay; i++)
    {
        ;
    }

    /* check if pin is set */
    if ((*pByte) == 0U)
    {   /* Set back original value */
      if (portValue == 0U)
      {
        *pByte = 0U;
      }
      if(st == FS_PASS){ /* To prevent rewrite of previsous error */
        st = FS_FAIL_DIO_NOT_SET;
      }
    }

    /* set pin to 0 */
    *pByte = 0U;

    /* Delay is necessary due to GPIO peripheral speed */
    for (i = 0U; i < delay; i++)
    {
        ;
    }

    /* check if pin is cleared */
    if (*pByte != 0U)
    {
       if(st == FS_PASS){ /* To prevent rewrite of previsous error */
        st = FS_FAIL_DIO_NOT_CLEAR;
       }
    }

    /* set original value of the pin */
    if (portValue != 0U)
    {
        *pByte = 1;
    }

    return st;
}
