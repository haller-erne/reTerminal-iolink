/**
  ******************************************************************************
  * @file    x_nucleo_eeprom.c
  * @author  SRA-SAIL, STM32ODE
  * @version V2.0.0
  * @date    12-02-2020
  * @brief   This file provides X_NUCLEO_EEPROM shield board 
  *          specific functions
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

/* Includes ------------------------------------------------------------------*/
#include "eeprma2_m24.h"
#include <i2c.h>
#include <stm32f4xx_hal_i2c.h>

#define AT24C04_EEPROM_ADDR			0xa0	// address of AT24CS04 / AT24MAC602 for read/write EEPROM (256 byte)
#define AT24C04_EEPROM_ADDR_SERIAL	0xb0	// address of AT24CS04 / AT24MAC602 for read serial number
#define RD_BIT						0x01
#define CFG_EEPROM_PAGE_SIZE		16
#define SERIAL_NUM_WORD_ADDRESS		0x80	// the word address input would need to be 0x80


int32_t EEPRMA2_M24_Init(uint32_t Instance)
{
	if (EEPRMA2_AT24C04_0 != Instance)
		return 0;

//###	LL_GPIO_ResetOutputPin(WP_EEPROM_GPIO_Port, WP_EEPROM_Pin);

	HAL_StatusTypeDef halSts = HAL_ERROR;
	halSts = HAL_I2C_IsDeviceReady(&hi2c3, AT24C04_EEPROM_ADDR, 500, 10);

	return halSts;
}


int32_t EEPRMA2_M24_DeInit(uint32_t Instance)
{
	if (EEPRMA2_AT24C04_0 != Instance)
		return 1;

	return 0;
}


int32_t EEPRMA2_M24_ReadByte (uint32_t Instance, uint8_t * const pData, const uint32_t TarAddr)
{
	if (EEPRMA2_AT24C04_0 != Instance)
		return 1;

  	uint16_t memAddSize = 1;

	HAL_StatusTypeDef halSts = HAL_ERROR;
	halSts = HAL_I2C_IsDeviceReady(&hi2c3, AT24C04_EEPROM_ADDR, 500, 10);

#ifdef DEBUG
	if (halSts != HAL_OK)
	{
		return halSts;
	}
#endif

  	return HAL_I2C_Mem_Read(&hi2c3, AT24C04_EEPROM_ADDR, TarAddr, memAddSize, pData, 1, 10);

}


int32_t EEPRMA2_M24_ReadData(uint32_t Instance, uint8_t * const pData, const uint32_t TarAddr, const uint16_t Size)
{
	if (EEPRMA2_AT24C04_0 != Instance)
		return 1;

  	uint16_t memAddSize = 1;

	HAL_StatusTypeDef halSts = HAL_ERROR;
	halSts = HAL_I2C_IsDeviceReady(&hi2c3, AT24C04_EEPROM_ADDR, 500, 10);

#ifdef DEBUG
	if (halSts != HAL_OK)
	{
		return halSts;
	}
#endif

  	return HAL_I2C_Mem_Read(&hi2c3, AT24C04_EEPROM_ADDR, TarAddr, memAddSize, pData, Size, 10);
}


int32_t EEPRMA2_M24_ReadSerial(uint32_t Instance, uint8_t * const pData, const uint16_t aSize)
{
	if (EEPRMA2_AT24C04_0 != Instance)
		return 1;

  	uint16_t memAddSize = I2C_MEMADD_SIZE_8BIT;

	HAL_StatusTypeDef halSts = HAL_ERROR;
	halSts = HAL_I2C_IsDeviceReady(&hi2c3, AT24C04_EEPROM_ADDR, 500, 10);

#ifdef DEBUG
	if (halSts != HAL_OK)
	{
		return halSts;
	}
#endif

  	return HAL_I2C_Mem_Read(&hi2c3, AT24C04_EEPROM_ADDR_SERIAL, SERIAL_NUM_WORD_ADDRESS, memAddSize, pData, aSize, 10);
}


int32_t EEPRMA2_M24_WriteByte(uint32_t Instance, uint8_t * const pData, const uint32_t TarAddr)
{
	if (EEPRMA2_AT24C04_0 != Instance)
		return 1;

	uint16_t memAddSize = 1;
	HAL_StatusTypeDef halSts = HAL_ERROR;

	halSts = HAL_I2C_IsDeviceReady(&hi2c3, AT24C04_EEPROM_ADDR, 500, 10);

#ifdef DEBUG
	if (halSts != HAL_OK)
	{
		return halSts;
	}
#endif

	halSts = HAL_I2C_Mem_Write(&hi2c3, AT24C04_EEPROM_ADDR, TarAddr, memAddSize, pData, 1, 10);

	return halSts;

}


int32_t EEPRMA2_M24_WriteData(uint32_t Instance, uint8_t * const pData, const uint32_t TarAddr, const uint16_t Size)
{
	if (EEPRMA2_AT24C04_0 != Instance)
		return 1;

	uint16_t memAddSize = 1;
	HAL_StatusTypeDef halSts = HAL_ERROR;

	uint32_t iNumberOfPage;
	uint32_t targetAddress = TarAddr;

	/*to handle dynamically start writing address*/
	if (targetAddress >= CFG_EEPROM_PAGE_SIZE)
	{
		iNumberOfPage =  Size / CFG_EEPROM_PAGE_SIZE;
	    if ((targetAddress % CFG_EEPROM_PAGE_SIZE) > 0)
	    {
	    	iNumberOfPage += 1;
	    }
	}
	else
	{
	    iNumberOfPage = ( targetAddress + Size ) / CFG_EEPROM_PAGE_SIZE;
	}

	uint32_t iRemainder = ( targetAddress + Size ) % CFG_EEPROM_PAGE_SIZE;
	uint8_t * pageIndex = pData;

	if (iRemainder > 0)
	{
		iNumberOfPage += 1;
	}

	halSts = HAL_I2C_IsDeviceReady(&hi2c3, AT24C04_EEPROM_ADDR, 500, 10);
	if (halSts != HAL_OK)
	    return HAL_TIMEOUT;

	if (targetAddress == 0)       /*If target address from which read/write will be done starts from 0*/
	{
		for (int index = 0;index < iNumberOfPage; index++)
	    {
	        uint32_t iSize = CFG_EEPROM_PAGE_SIZE;
	        if (index + 1 == iNumberOfPage)     /*For last page alignments*/
	    	   iSize = iRemainder;


	        halSts = HAL_I2C_Mem_Write(&hi2c3, AT24C04_EEPROM_ADDR, targetAddress, memAddSize, pageIndex, iSize, 10);

	        // EEPROM PageWrite/ByteWrite 5ms
	        HAL_Delay(6);
	        targetAddress += iSize;
	        pageIndex += iSize;
	        while ( HAL_I2C_IsDeviceReady(&hi2c3, AT24C04_EEPROM_ADDR, 500, 10) != HAL_OK ) {};
	    }
		return halSts;
	}
	else
	{
	    for(int index = 0; index < iNumberOfPage; index++)
	    {
	        uint32_t iSize = CFG_EEPROM_PAGE_SIZE;
	        if (index == 0) /*To align initial writing address*/
	        {
	        	if (targetAddress <= CFG_EEPROM_PAGE_SIZE)
	        		iSize = (CFG_EEPROM_PAGE_SIZE - targetAddress) > 0 ? (CFG_EEPROM_PAGE_SIZE - targetAddress) : CFG_EEPROM_PAGE_SIZE;
	        	else
	        		iSize = CFG_EEPROM_PAGE_SIZE - (targetAddress % CFG_EEPROM_PAGE_SIZE);
	        }

	        if (index+1 == iNumberOfPage) /*For last page alignments*/
	        	iSize = iRemainder;

		    halSts = HAL_I2C_Mem_Write(&hi2c3, AT24C04_EEPROM_ADDR, targetAddress, memAddSize, pageIndex, iSize, 10);

		    if (index + 1 < iNumberOfPage)
		    {
		        // EEPROM PageWrite/ByteWrite 5ms
		        HAL_Delay(6);
	    	    targetAddress += iSize;
	    	    pageIndex += iSize;

		        while ( HAL_I2C_IsDeviceReady(&hi2c3, AT24C04_EEPROM_ADDR, 500, 10) != HAL_OK ) {};
		    }
	   }
	   return halSts;
	}

	return halSts;
}


int32_t EEPRMA2_M24_WritePage(uint32_t Instance, uint8_t * const pData, const uint32_t aPage, const uint16_t Size)
{
	if (EEPRMA2_AT24C04_0 != Instance)
		return 1;

	uint16_t memAddSize = 1;
	HAL_StatusTypeDef halSts = HAL_ERROR;

	uint32_t targetAddress = CFG_EEPROM_PAGE_SIZE * aPage;	// start address in eeprom

	uint8_t * pageIndex = pData;

	halSts = HAL_I2C_IsDeviceReady(&hi2c3, AT24C04_EEPROM_ADDR, 500, 10);
	if (halSts != HAL_OK)
	    return HAL_TIMEOUT;

	uint32_t iSize = CFG_EEPROM_PAGE_SIZE;

	halSts = HAL_I2C_Mem_Write(&hi2c3, AT24C04_EEPROM_ADDR, targetAddress, memAddSize, pageIndex, iSize, 10);

	return halSts;
}

/******************* (C) COPYRIGHT 2018 STMicroelectronics *****END OF FILE****/
