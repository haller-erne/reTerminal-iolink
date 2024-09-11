/**
  ******************************************************************************
  * @file    EEPRMA2_m24.h
  * @author  SRA-SAIL,STM32ODE 
  * @version V2.0.0
  * @date    14-Aug-2020
  * @brief   This file contains definitions for the x_nucleo_eeprom.c
  *          board specific functions.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2020 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __EEPRMA2_M24_H
#define __EEPRMA2_M24_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
//#include "eeprma2_conf.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup X_NUCLEO_EEPRMA2
  * @{
  */

/* Exported types ------------------------------------------------------------*/


/**
 * @brief  EEPROM enumerator definition
 */
  
/**

  * @}
  */
/* Exported constants --------------------------------------------------------*/
/** @defgroup EEPRMA2_M24_Exported_Constants   EEPRMA M24 Exported Constants
  * @{
  */

#define EEPRMA2_M24_INSTANCES_NBR		1U
#define EEPRMA2_AT24C04_0                        0U
#define EEPRMA2_M24C02_0                         0U
#define EEPRMA2_M24256_0                         1U
#define EEPRMA2_M24M01_0                         2U  
#define EEPRMA2_M24xx                            7U



/* External variables --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
/** @defgroup EEPRMA2_M24_Exported_Functions   EEPRMA M24 Exported Functions
  * @{
  */

int32_t EEPRMA2_M24_Init(uint32_t Instance);
int32_t EEPRMA2_M24_DeInit(uint32_t Instance);
//int32_t EEPRMA2_M24_IsDeviceReady( uint32_t Instance, const uint32_t Trials );
int32_t EEPRMA2_M24_ReadByte (uint32_t Instance, uint8_t * const pData, const uint32_t TarAddr);
int32_t EEPRMA2_M24_ReadData(uint32_t Instance, uint8_t * const pData, const uint32_t TarAddr, const uint16_t Size);
int32_t EEPRMA2_M24_ReadSerial(uint32_t Instance, uint8_t * const pData, const uint16_t aSize);
//int32_t EEPRMA2_M24_ReadPage(uint32_t Instance, uint8_t * const pData, const uint32_t TarAddr, const uint16_t Size);
int32_t EEPRMA2_M24_WriteByte (uint32_t Instance, uint8_t * const pData, const uint32_t TarAddr);
int32_t EEPRMA2_M24_WriteData(uint32_t Instance, uint8_t * const pData, const uint32_t TarAddr, const uint16_t Size);
int32_t EEPRMA2_M24_WritePage(uint32_t Instance, uint8_t * const pData, const uint32_t aPage, const uint16_t Size);
//int32_t EEPRMA2_M24_WriteID(uint32_t Instance, uint8_t * const pData, const uint32_t TarAddr, const uint16_t Size );
//int32_t EEPRMA2_M24_ReadID(uint32_t Instance, uint8_t * const pData, const uint32_t TarAddr, const uint16_t Size );
//int32_t EEPRMA2_M24_LockStatus( uint32_t Instance, uint8_t * pData);
//int32_t EEPRMA2_M24_LockID( uint32_t Instance);
//void EEPRMA2_M24MemorySizeLocator(uint32_t Instance);
/**
  * @}
  */
//void EEPROMEX_CTRL_HIGH(const uint8_t pin);
//void EEPROMEX_CTRL_LOW(const uint8_t pin);
/**
  * @}
  */

/**
  * @}
  */ 

#ifdef __cplusplus
  }
#endif
#endif /* __X_NUCLEO_EEPRMA_H */

/******************* (C) COPYRIGHT 2018 STMicroelectronics *****END OF FILE****/
