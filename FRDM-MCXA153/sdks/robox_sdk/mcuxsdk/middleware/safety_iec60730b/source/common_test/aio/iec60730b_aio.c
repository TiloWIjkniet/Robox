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
 * @brief Analog I/O test common routines - IEC60730 Class B.
 *
 */

#include "iec60730b.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Macros
 *******************************************************************************/
#define FS_ADC_SC1_ADCH_MASK           0x1FU
#define FS_ADC_SC1_ADCH_2_MASK         0x3FU
#define FS_ADC_SC1_COCO_MASK           0x80U
#define FS_ADC_RESFIFO_D_MSK           0xFFFFU
#define FS_ADC_CMDL_ADCH_MSK           0x1FU
#define FS_ADC_CMDL_ABSEL_MSK          0x20U
#define FS_ADC_RESFIFO_VALID_MSK       0x80000000U

#define FS_CLIST_NUMBER_MASK 0x0CU
#define FS_SAMPLE_SHIFT_MASK 0x03U

#define FS_AIO_SET_CHANNEL_A234(pAdc, u8Channel) \
    ((pAdc)->sci[0]) &= ~FS_ADC_SC1_ADCH_MASK; \
    ((pAdc)->sci[0]) |= (((uint32_t)(u8Channel)) & FS_ADC_SC1_ADCH_MASK)
#define FS_AIO_IS_COCO_FLAG_A234(pAdc)    (((pAdc)->sci[0]) & FS_ADC_SC1_COCO_MASK)
#define FS_AIO_READ_RESULT_REG_A234(pAdc) ((pAdc)->res[0])
      
#define FS_AIO_SET_CHANNEL_A4_2(pAdc, u8Channel) \
    ((pAdc)->sci[0]) &= ~FS_ADC_SC1_ADCH_2_MASK; \
    ((pAdc)->sci[0]) |= (((uint32_t)(u8Channel)) & FS_ADC_SC1_ADCH_2_MASK)

/* ADC A1 */
#define FS_AIO_SET_CHANNEL_A1(pAdc, u8Channel, u8cmd, u8ABselect) \
    ((pAdc)->cmd[u8cmd].cmdl) &= ~(FS_ADC_CMDL_ADCH_MSK | FS_ADC_CMDL_ABSEL_MSK);   \
    ((pAdc)->cmd[u8cmd].cmdl) |= (FS_ADC_CMDL_ADCH_MSK & ((uint32_t)u8Channel) | (FS_ADC_CMDL_ABSEL_MSK & (u8ABselect << 5)))
#define FS_AIO_SW_TRIGGER_A1(pAdc, u8TriggerEvent) (pAdc)->swtrig = u8TriggerEvent

/* ADC A8 */
#define FS_AIO_SET_CHANNEL_A8(pAdc, u8Channel) \
    ((pAdc)->SC1A) &= ~FS_ADC_SC1_ADCH_2_MASK; \
    ((pAdc)->SC1A) |= (((uint32_t)(u8Channel)) & FS_ADC_SC1_ADCH_2_MASK)
#define FS_AIO_IS_COCO_FLAG_A8(pAdc)    (((pAdc)->SC1A) & FS_ADC_SC1_COCO_MASK)
#define FS_AIO_READ_RESULT_REG_A8(pAdc) ((pAdc)->RA)

/* ADC A5 */
#define FS_ADC_SEQ_CTRL_CHANNELS_MASK 0xFFFU
#define FS_AIO_SET_CHANNEL_A5(pAdc, u8Channel, u8Sequence)    \
    ((pAdc)->SEQ_CTRL[u8Sequence]) &= ~FS_ADC_SEQ_CTRL_CHANNELS_MASK; \
    ((pAdc)->SEQ_CTRL[u8Sequence]) |= (1UL << (u8Channel)) & FS_ADC_SEQ_CTRL_CHANNELS_MASK

#define FS_ADC_SEQ_CTRL_SEQ_ENA_MASK                    0x80000000U
#define FS_AIO_ENABLE_SEQUENCE_A5(pAdc, u8Sequence) ((pAdc)->SEQ_CTRL[u8Sequence]) |= FS_ADC_SEQ_CTRL_SEQ_ENA_MASK

#define FS_ADC_SEQ_CTRL_START_MASK                     0x4000000U
#define FS_AIO_GET_SW_TRIGGER_A5(pAdc, u8Sequence) ((pAdc)->SEQ_CTRL[u8Sequence]) |= FS_ADC_SEQ_CTRL_START_MASK

#define FS_ADC_DAT_DATAVALID_MASK                    0x80000000U
#define FS_AIO_IS_DATA_VALID_A5(pAdc, u8Channel) (((pAdc)->DAT[u8Channel]) & FS_ADC_DAT_DATAVALID_MASK)

#define FS_ADC_DAT_RESULT_MASK  0xFFF0U
#define FS_ADC_DAT_RESULT_SHIFT 4U
#define FS_AIO_READ_RESULT_REG_A5(pAdc, u8Channel) \
    ((((pAdc)->DAT[u8Channel]) & FS_ADC_DAT_RESULT_MASK) >> FS_ADC_DAT_RESULT_SHIFT)


/* ADC A6 */
#define FS_AIO_IS_COCO_FLAG_A6(pAdc)             (((pAdc)->HS) & 1U) /* 1U because only bit 0 is used for SW trigger. */
#define FS_AIO_READ_RESULT_REG_A6(pAdc, channel) ((pAdc)->R[0])

#define FS_ADC_HC_ADCH_MASK 0x1FU
#define FS_AIO_GET_SW_TRIGGER_A6(pAdc, channel) \
    ((pAdc)->HC[0] &= ~FS_ADC_HC_ADCH_MASK);           \
    ((pAdc)->HC[0] |= (channel))

/* LPADC  */
#define FS_ADC_CMDL_ADCH_SHIFT  (0x00UL)


/*******************************************************************************
 * Implementation
 ******************************************************************************/

/*******************************************************************************
******************************************************************************/
FS_RESULT FS_AIO_LimitCheck(uint32_t RawResult, fs_aio_limits_t *pLimits, FS_RESULT *pState )
{
  if (*pState == FS_AIO_SCAN_COMPLETE)
  {
    if (pLimits->low > RawResult){ /* check, if converted value fits to limits */
     *pState = FS_FAIL_AIO;
    }else if (RawResult > pLimits->high){
      *pState = FS_FAIL_AIO;
      } else{
          *pState = FS_PASS;
          }
  }

  return *pState;
}

/*******************************************************************************
 ******************************************************************************/
FS_RESULT FS_AIO_InputSet_A4(fs_aio_test_a23468_t *pObj, fs_aio_a4_t *pAdc)
{
    if (pObj->state == FS_AIO_INIT)
    {
        /* set input */
        FS_AIO_SET_CHANNEL_A234(pAdc, pObj->AdcChannel);
        pObj->state = FS_AIO_PROGRESS;
    }
    return pObj->state;
}

/*******************************************************************************
 ******************************************************************************/
FS_RESULT FS_AIO_InputSet_A4_2(fs_aio_test_a23468_t *pObj, fs_aio_a4_t *pAdc)
{
    if (pObj->state == FS_AIO_INIT)
    {
        /* set input */
        FS_AIO_SET_CHANNEL_A4_2(pAdc, pObj->AdcChannel);
        pObj->state = FS_AIO_PROGRESS;
    }
    return pObj->state;
}

/*******************************************************************************
 ******************************************************************************/
FS_RESULT FS_AIO_ReadResult_A4(fs_aio_test_a23468_t *pObj, fs_aio_a4_t *pAdc)
{
    if (pObj->state == FS_AIO_PROGRESS)
    {
        if (0U != FS_AIO_IS_COCO_FLAG_A234(pAdc))
        {
            pObj->RawResult = (uint16_t)FS_AIO_READ_RESULT_REG_A234(pAdc);
            pObj->state  = FS_AIO_SCAN_COMPLETE;
        }
    }
    return pObj->state;
}

/*******************************************************************************
 ******************************************************************************/
FS_RESULT FS_AIO_InputSet_A23(fs_aio_test_a23468_t *pObj, fs_aio_a23_t *pAdc)
{
    if (pObj->state == FS_AIO_INIT)
    {
        /* set input */
        FS_AIO_SET_CHANNEL_A234(pAdc, pObj->AdcChannel);
        pObj->state = FS_AIO_PROGRESS;
    }
    return pObj->state;
}

/*******************************************************************************
 ******************************************************************************/
FS_RESULT FS_AIO_ReadResult_A23(fs_aio_test_a23468_t *pObj, fs_aio_a23_t *pAdc)
{
    if (pObj->state == FS_AIO_PROGRESS)
    {
        if (0U != FS_AIO_IS_COCO_FLAG_A234(pAdc))
        {
            pObj->RawResult = (uint16_t)FS_AIO_READ_RESULT_REG_A234(pAdc);
            pObj->state  = FS_AIO_SCAN_COMPLETE;
        }
    }
    return pObj->state;
}


/*******************************************************************************
 ******************************************************************************/
FS_RESULT FS_AIO_InputSet_A6(fs_aio_test_a23468_t *pObj, fs_aio_a6_t *pAdc)
{
    if (pObj->state == FS_AIO_INIT)
    {
        /* set input */
        FS_AIO_GET_SW_TRIGGER_A6(pAdc, pObj->AdcChannel); /* Do SW trigger */
        pObj->state = FS_AIO_PROGRESS;
    }
    return pObj->state;
}

/*******************************************************************************
 ******************************************************************************/
FS_RESULT FS_AIO_ReadResult_A6(fs_aio_test_a23468_t *pObj, fs_aio_a6_t *pAdc)
{
    if (pObj->state == FS_AIO_PROGRESS)
    {
        if (0U != FS_AIO_IS_COCO_FLAG_A6(pAdc))
        {
            pObj->RawResult = (uint16_t)FS_AIO_READ_RESULT_REG_A6(pAdc,pObj->AdcChannel);
            pObj->state  = FS_AIO_SCAN_COMPLETE;
        }
    }
    return pObj->state;
}

/*******************************************************************************
 ******************************************************************************/
FS_RESULT FS_AIO_InputSet_A1(fs_aio_test_a1_t *pObj, fs_aio_a1_t *pAdc)
{
  if (pObj->state == FS_AIO_INIT)
    {
        /* enter the channel number (0-31), and channel side (A or B) to the selected command buffer register */
        FS_AIO_SET_CHANNEL_A1(pAdc, pObj->AdcChannel, pObj->commandBuffer - 1U, pObj->SideSelect);
        /* Write 1 to the SWTRIG register to do a software trigger */
        FS_AIO_SW_TRIGGER_A1(pAdc, pObj->softwareTriggerEvent);
        pObj->state = FS_AIO_PROGRESS;
    }
    return pObj->state;
}
/*******************************************************************************
 ******************************************************************************/
FS_RESULT FS_AIO_ReadResult_A1(fs_aio_test_a1_t *pObj, fs_aio_a1_t *pAdc)
{
    uint32_t resFifo;
    if (pObj->state == FS_AIO_PROGRESS)
    {
        resFifo = pAdc-> resfifo;
        if (FS_ADC_RESFIFO_VALID_MSK & resFifo) /* if the VALID bit is set, FIFO is holding the data */
        {
            pObj->RawResult = (uint16_t)((resFifo & FS_ADC_RESFIFO_D_MSK) >> 3U); /* read from the FIFO data bits and shift to the right by 3 */
            pObj->state  = FS_AIO_SCAN_COMPLETE;
        }
    }
    return pObj->state;
}


/*******************************************************************************
 ******************************************************************************/
FS_RESULT FS_AIO_InputSet_A5(fs_aio_test_a5_t *pObj, fs_aio_a5_t *pAdc)
{
    if (pObj->state == FS_AIO_INIT)
    {
        /* set input */
        FS_AIO_SET_CHANNEL_A5(pAdc, pObj->AdcChannel, pObj->sequence); /* Set channel*/
        FS_AIO_ENABLE_SEQUENCE_A5(pAdc, pObj->sequence);                           /* Enable correspond sequence*/
        FS_AIO_GET_SW_TRIGGER_A5(pAdc, pObj->sequence);                            /* Do SW trigger */
        pObj->state = FS_AIO_PROGRESS;
    }
    return pObj->state;
}

/*******************************************************************************
 ******************************************************************************/
FS_RESULT FS_AIO_ReadResult_A5(fs_aio_test_a5_t *pObj, fs_aio_a5_t *pAdc)
{
    if (pObj->state == FS_AIO_PROGRESS)
    {
        if (0U != FS_AIO_IS_DATA_VALID_A5(pAdc, pObj->AdcChannel))
        {
            pObj->RawResult = (uint16_t)(FS_AIO_READ_RESULT_REG_A5(pAdc, pObj->AdcChannel));
            pObj->state  = FS_AIO_SCAN_COMPLETE;
        }
    }
    return pObj->state;
}



/*******************************************************************************
 ******************************************************************************/
FS_RESULT FS_AIO_InputSet_A7(fs_aio_test_a7_t *pObj, fs_aio_a7_t *pAdc)
{

    uint8_t clist_index   = 0U;
    uint16_t sample_shift = 0U;

    if (pObj->state == FS_AIO_INIT)
    {
        clist_index = ((pObj->Sample & FS_CLIST_NUMBER_MASK) >> 2U); /* give Clist index (0-3 ) */
        /* Give shift in CLIST reg, 4 is shift for */
        sample_shift = ((((uint16_t)pObj->Sample) & FS_SAMPLE_SHIFT_MASK) * 4U); /* Mask for concrete sample */

        (pAdc)->clist[clist_index] &= ~(((uint16_t)0x000FU) << sample_shift); /*Clear this sample reg*/

        /* Set select Input from pInputs[x] to select SAMPLE from pSamples[x]*/
        (pAdc)->clist[clist_index] |= (uint16_t)(pObj->AdcChannel)
                                      << sample_shift; /*Set new  sample  settings*/

        pObj->state = FS_AIO_PROGRESS;
    }
    return pObj->state;
}

/*******************************************************************************
 ******************************************************************************/
FS_RESULT FS_AIO_ReadResult_A7(fs_aio_test_a7_t *pObj, fs_aio_a7_t *pAdc)
{
    if (pObj->state == FS_AIO_PROGRESS)
    {
        if (0U != (((pAdc)->rdy) & (0x1UL << (uint16_t)(pObj->Sample))))
        { /*Shift because of 12b to 16bit representation */
            pObj->RawResult = (pAdc)->rslt[pObj->Sample] >> 3U;
            pObj->state  = FS_AIO_SCAN_COMPLETE;
        }
    }
    return pObj->state;
}

/*******************************************************************************
 ******************************************************************************/
FS_RESULT FS_AIO_InputSet_A8(fs_aio_test_a23468_t *pObj, fs_aio_a8_t *pAdc)
{
    if (pObj->state == FS_AIO_INIT)
    {
        /* set input */
        FS_AIO_SET_CHANNEL_A8(pAdc, pObj->AdcChannel);
        pObj->state = FS_AIO_PROGRESS;
    }
    return pObj->state;
}

/*******************************************************************************
 ******************************************************************************/
FS_RESULT FS_AIO_ReadResult_A8(fs_aio_test_a23468_t *pObj, fs_aio_a8_t *pAdc)
{
    if (pObj->state == FS_AIO_PROGRESS)
    {
        if (0U != FS_AIO_IS_COCO_FLAG_A8(pAdc))
        {
            pObj->RawResult = (uint16_t)FS_AIO_READ_RESULT_REG_A8(pAdc);
            pObj->state  = FS_AIO_SCAN_COMPLETE;
        }
    }
    return pObj->state;
}

