/*
 * i2c_AT24MAC.h
 *
 *  Created on: May 28, 2020
 *      Author: harald.schmittle
 */

#ifndef INC_I2C_AT24MAC_H_
#define INC_I2C_AT24MAC_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <flash_config_hm.h>
#include "stm32f4xx_hal.h"
#include <stm32f4xx_hal_i2c.h>
#include <i2c.h>


#define AT24MAC_EEPROM_ADDR		0xa0	// address of AT24MAC402 / AT24MAC602 for read/write EEPROM (256 byte)
#define AT24MAC_MAC_ADDR		0xb0	// address of AT24MAC402 / AT24MAC602 for read MAC and SERNO
#define RD_BIT					0x01

#ifdef __MSTKN_HM_IO_LINK
	#define EEPROM_BASEADDR			0x100
#else
	#define EEPROM_BASEADDR			0x00
#endif

uint8_t AT24MAC_port_read(I2C_HandleTypeDef hi2c, uint8_t aReg, uint8_t aReadLeng, uint8_t *rVal);
HAL_StatusTypeDef AT24MAC_mem_write(I2C_HandleTypeDef hi2c, uint8_t aMemAddress, uint8_t *aWriteBuf);

HAL_StatusTypeDef AT24MAC_WriteDiff_Async(I2C_HandleTypeDef hi2c, const tFlashCFG *aFlash);
HAL_StatusTypeDef AT24MAC_WriteDiff(I2C_HandleTypeDef hi2c, const tFlashCFG *aFlash);
HAL_StatusTypeDef AT24MAC_Write(I2C_HandleTypeDef hi2c, const tFlashCFG *aFlash);
HAL_StatusTypeDef AT24MAC_Read(I2C_HandleTypeDef hi2c, tFlashCFG *rFlash);

uint8_t AT24MAC_GetWriteState(void);

HAL_StatusTypeDef AT24MAC_ReadConfig(I2C_HandleTypeDef hi2c, tFlashCFG* pCfg);

const tFlashCFG* AT24MAC_GetRomConfig();

typedef struct _AT24_Serno
{
	uint8_t SerNo[16];
} AT24_Serno;

HAL_StatusTypeDef AT24MAC_MACRead(I2C_HandleTypeDef hi2c);
HAL_StatusTypeDef AT24MAC_SERNORead(I2C_HandleTypeDef hi2c, AT24_Serno* pSerno);
void GetMAC_Addr(uint8_t *rMacAddr);
void GetHW_TTNR(uint8_t *rTTNR);

#ifdef __cplusplus
}
#endif

#endif /* INC_I2C_AT24MAC_H_ */
