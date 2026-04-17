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
 * @brief Invariable memory test common routines - IEC60730 Class B.
 *
 */

#include "iec60730b.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* DCP - Register Layout Typedef */
typedef struct
{
    uint32_t CTRL; /**< DCP control register 0, offset: 0x0 */
    uint8_t RESERVED_0[12];
    uint32_t STAT; /**< DCP status register, offset: 0x10 */
    uint8_t RESERVED_1[12];
    uint32_t CHANNELCTRL; /**< DCP channel control register, offset: 0x20 */
    uint8_t RESERVED_2[12];
    uint32_t CAPABILITY0; /**< DCP capability 0 register, offset: 0x30 */
    uint8_t RESERVED_3[12];
    uint32_t CAPABILITY1; /**< DCP capability 1 register, offset: 0x40 */
    uint8_t RESERVED_4[12];
    uint32_t CONTEXT; /**< DCP context buffer pointer, offset: 0x50 */
    uint8_t RESERVED_5[12];
    uint32_t KEY; /**< DCP key index, offset: 0x60 */
    uint8_t RESERVED_6[12];
    uint32_t KEYDATA; /**< DCP key data, offset: 0x70 */
    uint8_t RESERVED_7[12];
    uint32_t PACKET0; /**< DCP work packet 0 status register, offset: 0x80 */
    uint8_t RESERVED_8[12];
    uint32_t PACKET1; /**< DCP work packet 1 status register, offset: 0x90 */
    uint8_t RESERVED_9[12];
    uint32_t PACKET2; /**< DCP work packet 2 status register, offset: 0xA0 */
    uint8_t RESERVED_10[12];
    uint32_t PACKET3; /**< DCP work packet 3 status register, offset: 0xB0 */
    uint8_t RESERVED_11[12];
    uint32_t PACKET4; /**< DCP work packet 4 status register, offset: 0xC0 */
    uint8_t RESERVED_12[12];
    uint32_t PACKET5; /**< DCP work packet 5 status register, offset: 0xD0 */
    uint8_t RESERVED_13[12];
    uint32_t PACKET6; /**< DCP work packet 6 status register, offset: 0xE0 */
    uint8_t RESERVED_14[28];
    uint32_t CH0CMDPTR; /**< DCP channel 0 command pointer address register, offset: 0x100 */
    uint8_t RESERVED_15[12];
    uint32_t CH0SEMA; /**< DCP channel 0 semaphore register, offset: 0x110 */
    uint8_t RESERVED_16[12];
    uint32_t CH0STAT; /**< DCP channel 0 status register, offset: 0x120 */
    uint8_t RESERVED_17[12];
    uint32_t CH0OPTS; /**< DCP channel 0 options register, offset: 0x130 */
    uint8_t RESERVED_18[12];
    uint32_t CH1CMDPTR; /**< DCP channel 1 command pointer address register, offset: 0x140 */
    uint8_t RESERVED_19[12];
    uint32_t CH1SEMA; /**< DCP channel 1 semaphore register, offset: 0x150 */
    uint8_t RESERVED_20[12];
    uint32_t CH1STAT; /**< DCP channel 1 status register, offset: 0x160 */
    uint8_t RESERVED_21[12];
    uint32_t CH1OPTS; /**< DCP channel 1 options register, offset: 0x170 */
    uint8_t RESERVED_22[12];
    uint32_t CH2CMDPTR; /**< DCP channel 2 command pointer address register, offset: 0x180 */
    uint8_t RESERVED_23[12];
    uint32_t CH2SEMA; /**< DCP channel 2 semaphore register, offset: 0x190 */
    uint8_t RESERVED_24[12];
    uint32_t CH2STAT; /**< DCP channel 2 status register, offset: 0x1A0 */
    uint8_t RESERVED_25[12];
    uint32_t CH2OPTS; /**< DCP channel 2 options register, offset: 0x1B0 */
    uint8_t RESERVED_26[12];
    uint32_t CH3CMDPTR; /**< DCP channel 3 command pointer address register, offset: 0x1C0 */
    uint8_t RESERVED_27[12];
    uint32_t CH3SEMA; /**< DCP channel 3 semaphore register, offset: 0x1D0 */
    uint8_t RESERVED_28[12];
    uint32_t CH3STAT; /**< DCP channel 3 status register, offset: 0x1E0 */
    uint8_t RESERVED_29[12];
    uint32_t CH3OPTS; /**< DCP channel 3 options register, offset: 0x1F0 */
    uint8_t RESERVED_30[524];
    uint32_t DBGSELECT; /**< DCP debug select register, offset: 0x400 */
    uint8_t RESERVED_31[12];
    uint32_t DBGDATA; /**< DCP debug data register, offset: 0x410 */
    uint8_t RESERVED_32[12];
    uint32_t PAGETABLE; /**< DCP page table register, offset: 0x420 */
    uint8_t RESERVED_33[12];
    uint32_t VERSION; /**< DCP version register, offset: 0x430 */
} fs_dcp_t;

/*******************************************************************************
 * Macros
 *******************************************************************************/
#define FS_MODULO_4_MASK 0x3U
#define FS_CRC_CTRL_TOT_MASK                        (0xC0000000U)

/* DCP - Masks */
#define FS_DCP_CTRL_SFTRST(x)               (((uint32_t)(((uint32_t)(x)) << 31U)) & 0x80000000U)
#define FS_DCP_PACKET1_DECR_SEMAPHORE_MASK  (0x2U)
#define FS_DCP_PACKET1_DECR_SEMAPHORE_SHIFT (1U)
#define FS_DCP_PACKET1_DECR_SEMAPHORE(x) \
    (((uint32_t)(((uint32_t)(x)) << FS_DCP_PACKET1_DECR_SEMAPHORE_SHIFT)) & FS_DCP_PACKET1_DECR_SEMAPHORE_MASK)
#define FS_DCP_PACKET1_ENABLE_HASH_MASK  (0x40U)
#define FS_DCP_PACKET1_ENABLE_HASH_SHIFT (6U)
#define FS_DCP_PACKET1_ENABLE_HASH(x) \
    (((uint32_t)(((uint32_t)(x)) << FS_DCP_PACKET1_ENABLE_HASH_SHIFT)) & FS_DCP_PACKET1_ENABLE_HASH_MASK)
#define FS_DCP_PACKET2_HASH_SELECT_MASK  (0xF0000U)
#define FS_DCP_PACKET2_HASH_SELECT_SHIFT (16U)
#define FS_DCP_PACKET2_HASH_SELECT(x) \
    (((uint32_t)(((uint32_t)(x)) << FS_DCP_PACKET2_HASH_SELECT_SHIFT)) & FS_DCP_PACKET2_HASH_SELECT_MASK)
#define FS_DCP_CHXSTAT_TAG_MASK     (0xFF000000U)
#define FS_DCP_CHXSTAT_TAG_SHIFT    (24U)
#define FS_DCP_STAT_ERROR_CODE_MASK (0xFF0000U)
#define FS_DCP_SEMA_VALUE_MASK      (0xFF0000U)

/*******************************************************************************
 * Implementation
 ******************************************************************************/

FS_RESULT FS_FLASH_C_HW16_K(uint32_t startAddress, uint32_t size, FS_CRC_Type * moduleAddress, uint16_t * crcVal)
{
  uint32_t st = 0U;
  
  if(moduleAddress == 0U || crcVal == 0U)
  {
    st =   FS_FAIL_FLASH_NULL_POINTER_C;
  }
  
  moduleAddress->CTRL = 0U;
  
  moduleAddress->CTRL = FS_FLASH_CRC_CTRL_WAS_MASK; /* set WAS bit and reset CRC */
  moduleAddress->ACCESS16BIT.DATAL = *crcVal;
  moduleAddress->CTRL = (FS_CRC_CTRL_TOT_MASK);    /* clear WAS bit and begin to calculate CRC (TOT 0b11) */          
  
  if (size & FS_MODULO_4_MASK)
  {
    if(st == FS_PASS){ /* To prevent rewrite of previsous error */
        st = FS_FAIL_FLASH_MODULO_C;
      }
  }
  if (size == 0U)
  {
    if(st == FS_PASS){ /* To prevent rewrite of previsous error */
      st = FS_FAIL_FLASH_SIZE_C;
      }
  }
  
  
  if(st == 0U)
  {
    size = size >> 2; /* /4 */ 
    while (size--)
    {
      moduleAddress->DATA = *((uint32_t *)startAddress);
      startAddress += 4U;
           
    }
    
    *crcVal = moduleAddress->ACCESS16BIT.DATAL; /* Store final CRC result */
  }
  
  return st;
}

FS_RESULT FS_FLASH_C_HW32_K(uint32_t startAddress, uint32_t size, FS_CRC_Type * moduleAddress, uint32_t * crcVal)
{
  uint32_t st = 0U;
  
  if(moduleAddress == 0U || crcVal == 0U)
  {
    st =   FS_FAIL_FLASH_NULL_POINTER_C;
  }
  
  moduleAddress->CTRL = 0U; 
  moduleAddress->CTRL = FS_FLASH_CRC_CTRL_TCRC_MASK; /* set TCRC bit to use 32-bit CRC protocol */
  moduleAddress->GPOLY = 0x04C11DB7;                 /* set polynomial value for CRC32 */
  moduleAddress->CTRL = FS_FLASH_CRC_CTRL_WAS_MASK | FS_FLASH_CRC_CTRL_TCRC_MASK;  /* set WAS & TCRC bit and reset CRC */    
  moduleAddress->DATA = *crcVal;                     /* store seed value into data register */
  moduleAddress->CTRL = FS_FLASH_CRC_CTRL_TCRC_MASK | FS_CRC_CTRL_TOT_MASK; /* clear WAS bit, keep TCRC set and begin to calculate CRC (TOT 0b11) */          
  
  if (size & FS_MODULO_4_MASK)
  {
    if(st == FS_PASS){ /* To prevent rewrite of previsous error */
        st = FS_FAIL_FLASH_MODULO_C;
      }
  }
  
  if (size == 0U)
  {
    if(st == FS_PASS){ /* To prevent rewrite of previsous error */
      st = FS_FAIL_FLASH_SIZE_C;
      }
  }
  
  if(st == 0U)
  {
    size = size >> 2; /* /4 */ 
    while (size--)
    {
      moduleAddress->DATA = *((uint32_t *)startAddress);
      startAddress += 4U; 
    }
    
    *crcVal = moduleAddress->DATA; /* Store final CRC result */
  }
  
  return st;
}

FS_RESULT FS_FLASH_C_HW16_L(uint32_t startAddress, uint32_t size, FS_CRC_L_Type * moduleAddress, uint16_t * crcVal)
{
  uint32_t st = 0U;
  
  if(moduleAddress == 0U || crcVal == 0U)
  {
    st =   FS_FAIL_FLASH_NULL_POINTER_C;
  }
  
   moduleAddress->MODE = 0U;
   moduleAddress->SEED = *crcVal;
  
  if (size == 0U)
  {
    if(st == FS_PASS){ /* To prevent rewrite of previsous error */
      st = FS_FAIL_FLASH_SIZE_C;
      }
  } 
   
  if(st == 0U)
  {
    while (size--)
    {
      moduleAddress->ACCESS8BIT.DATALL = *((uint8_t *)startAddress);
      startAddress += 1U;
           
    }
    
    *crcVal = (uint16_t)moduleAddress->SUM; /* Store final CRC result */
  }
  
  return st;
}

void FS_CM4_CM7_FLASH_HW32_DCP(uint32_t startAddress,
                               uint32_t size,
                               uint32_t moduleAddress,
                               uint32_t crcVal,
                               fs_flash_dcp_channels_t channel,
                               fs_flash_dcp_state_t *psDCPState,
                               uint32_t tag)
{
    volatile uint32_t *pStatReg;
    volatile uint32_t *pCmdReg;
    volatile uint32_t *pSemReg;
    volatile uint32_t *pResult;
    volatile uint32_t *pState;
    volatile uint32_t *pPacket;
    volatile uint32_t shift = 0U;

    volatile fs_dcp_t *module = (fs_dcp_t *)moduleAddress;

    switch (channel)
    {
        case DCP_CH0:
            pStatReg = &module->CH0STAT;
            pCmdReg  = &module->CH0CMDPTR;
            pSemReg  = &module->CH0SEMA;
            pResult  = &psDCPState->CH0Result;
            pState   = &psDCPState->CH0State;
            pPacket  = psDCPState->CH0Packet;
            shift    = 172U;
            break;

        case DCP_CH1:
            pStatReg = &module->CH1STAT;
            pCmdReg  = &module->CH1CMDPTR;
            pSemReg  = &module->CH1SEMA;
            pResult  = &psDCPState->CH1Result;
            pState   = &psDCPState->CH1State;
            pPacket  = psDCPState->CH1Packet;
            shift    = 120U;
            break;

        case DCP_CH2:
            pStatReg = &module->CH2STAT;
            pCmdReg  = &module->CH2CMDPTR;
            pSemReg  = &module->CH2SEMA;
            pResult  = &psDCPState->CH2Result;
            pState   = &psDCPState->CH2State;
            pPacket  = psDCPState->CH2Packet;
            shift    = 68U;
            break;

        case DCP_CH3:
            pStatReg = &module->CH3STAT;
            pCmdReg  = &module->CH3CMDPTR;
            pSemReg  = &module->CH3SEMA;
            pResult  = &psDCPState->CH3Result;
            pState   = &psDCPState->CH3State;
            pPacket  = psDCPState->CH3Packet;
            shift    = 16U;
            break;

        default:
            /* Never happens, all enum cases are tested above */
            break;
    }

    /* Check status register of channel, if it is busy -> do nothing */
    if ((module->STAT & (uint32_t)channel) == 0U)
    {
        /* Check if last state == BUSY (it means that some calculation on appropriate channel has finished) -> save the
         * result */
        if (*pState == FS_FLASH_DCP_BUSY)
        {
          /* Check if calculation is complete */
          if(((pPacket[7]) & 0x1U)==0x1U)
          {
            /* Check if result matches my computation tag */
            if ((*pStatReg & FS_DCP_CHXSTAT_TAG_MASK) == (uint32_t)(tag << FS_DCP_CHXSTAT_TAG_SHIFT))
            {
                /* Temporary vaiable due to MISRA compliance (side effect). */
                uint32_t tmpSemReg  = *pSemReg;
                uint32_t tmpStatReg = *pStatReg;

                /* Check status & semaphore register for error. */
                if (!((0U == (tmpSemReg & FS_DCP_SEMA_VALUE_MASK)) &&
                      (0U == (tmpStatReg & FS_DCP_STAT_ERROR_CODE_MASK))))
                {
                    *pState = FS_FAIL_FLASH_DCP;
                }
                else
                {
                    /* Save calculated CRC
                     * In Security Reference Manual, there is a table called DCP context buffer layout.
                     * The following command just read the first 4 bytes of Hash context of appropriate channel. */
                    *pResult = *(uint32_t *)(module->CONTEXT + shift);

                    __asm("DSB"); /* Wait for the data to complete */
                    __asm("ISB");

                    /* Set status */
                    *pState = FS_FLASH_DCP_AVAILABLE;
                }
            }
          }
        }
        else /* Channel is available -> start new calculation */
        {
            /* Load init value */
            *(uint32_t *)(module->CONTEXT + shift) = crcVal;

            /* Fill the packet items */
            pPacket[0] = 0U; /* Next command address - not used */
            /* Control 0 - decrement semaphore = 1, enable hashing = 1, set calculation tag */
            pPacket[1] = FS_DCP_PACKET1_DECR_SEMAPHORE(1U) | FS_DCP_PACKET1_ENABLE_HASH(1U) |
                         (uint32_t)(tag << FS_DCP_CHXSTAT_TAG_SHIFT);
            pPacket[2] = FS_DCP_PACKET2_HASH_SELECT(1U); /* Control 1 - hash algorithm select CRC32 */
            pPacket[3] = (uint32_t)startAddress;         /* Source buffer - start address */
            pPacket[4] = (uint32_t)startAddress;         /* Destination buffer - same address */
            pPacket[5] = size;                           /* Buffer size - size of input data */
            pPacket[6] = 0U;                             /* Payload pointer - not used */
            pPacket[7] = 0U;                             /* Status */

            /* Set packet address to CHxCMDPTR */
            *pCmdReg = (uint32_t)pPacket;

            __asm("DSB"); /* Wait for the data to complete */
            __asm("ISB");

            /* Request the calculation (set the channel semaphore CHxSEMA to 1 -> process one packet) */
            *pSemReg = 1U;

            /* Set status in psDCPState structure on corresponding channel */
            *pState = FS_FLASH_DCP_BUSY;
        }
    }
}

/*******************************************************************************
 ******************************************************************************/
