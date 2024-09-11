/*
 * i2c_AT24MAC.c
 *
 *  Created on: May 28, 2020
 *      Author: harald.schmittle
 */

#include <string.h>
#include <i2c_AT24MAC.h>
#include <Version.h>
#include <stdio.h>
#include <stm32f4xx_hal_i2c.h>

// Plausibility checks:
#define STATIC_ASSERT(COND,MSG) typedef char static_assertion_##MSG[(COND)?1:-1]
STATIC_ASSERT(sizeof(tFlashCFG) == CFG_FLASH_SIZE, InvalidFlashConfigSize);

static uint8_t _macAddr[6];

// ====== Initial Flash Configuration (actual configuration is stored in EEProm!) =========
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-braces"
__attribute__((used)) __attribute__((section ("._config")))
const tFlashCFG flashCfg = {
#ifndef WATCHDOG_DISABLED       // VERSION  =   (((VER_MAJOR << 8) + VER_MINOR) << 8) + VER_BUILD
    ( VERSION ) | 0x80000000,   // Standardeinstellungen
#else
    ( VERSION ),                // !!! Ohne watchdog
#endif
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, 		//   SlavesConfig[]
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, 		//   SlavesUID[]
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},  // Groups[30]
	{	//   network parameters
		{ 192,168,  1,100 },				// uint8_t IP[4];
		{   0,  0,  0,  0 },				// uint8_t Gateway[4];
		{ 255,255,255,  0 },				// uint8_t Subnet[4];
		"MSTKN",							// Hostname
		{
			.flag_dhcp = 1
		}
	},
	{ 0 }, 					//Stub
       { .val = 0xFFFFFFFF },	// UserCfgFlags
	0x01ED,  				// VendorID
//	0x02BA, 				// Jäger VendorID
	0x0101, 				// Device ID
	INPUT_PAGE_SIZE,
	OUTPUT_PAGE_SIZE,
//#    "***mstkn-sm***",  			// sTTNR: JTAG production programming overwrites this field
    HW_TTNR,  	        			// sTTNR: JTAG production programming overwrites this field
    								//        --> MSTKN-S-S14-0003 (S14 hardware rev 3)
    								//        --> MSTKN-S-T18-0005 (T18 hardware rev 4)
    UNIQUE_ID           			// UNIQUE_ID
};
#pragma GCC diagnostic pop


static uint8_t _I2CMasterBuffer[64];
static uint8_t _I2CSlaveBuffer[64];
static uint32_t _I2CReadLength;
static uint32_t _I2CWriteLength;

static uint32_t _PrgStart;
static uint8_t _WriteState;
static uint8_t _WriteReady;

static tFlashCFG _ReadBack_Cfg;

uint8_t AT24MAC_port_read(I2C_HandleTypeDef hi2c, uint8_t aReg, uint8_t aReadLen, uint8_t *rVal)
{
	// Write SLA(W), address, SLA(R), and read one byte back.
	_I2CWriteLength = 1;
//	I2CReadLength = 1;
	_I2CReadLength = aReadLen;
  	_I2CMasterBuffer[0] = aReg;

  	HAL_StatusTypeDef retW = HAL_I2C_Master_Transmit(&hi2c, AT24MAC_MAC_ADDR, _I2CMasterBuffer, _I2CWriteLength, 100);
//	HAL_Delay(2);
	_I2CSlaveBuffer[0] = aReg;
	HAL_StatusTypeDef retR = HAL_I2C_Master_Receive(&hi2c, AT24MAC_MAC_ADDR | RD_BIT, _I2CSlaveBuffer, _I2CReadLength, 100);

	for (int i = 0; i < aReadLen; i++)
	{
		rVal[i] = _I2CSlaveBuffer[i];
	}

	return 1;
}


/**
  * @brief  PageWrite (16 byte) an amount of data in blocking mode to a specific memory address
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @param  DevAddress Target device address: The device 7 bits address value
  *         in datasheet must be shifted to the left before calling the interface
  * @param  MemAddress Internal memory address
  * @param  MemAddSize Size of internal memory address
  * @param  pData Pointer to data buffer
  * @param  Size Amount of data to be sent
  * @param  Timeout Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef AT24MAC_mem_write(I2C_HandleTypeDef hi2c, uint8_t aMemAddress, uint8_t *aWriteBuf)
{
	uint16_t memAddSize = 1;
	uint16_t writeLen = CFG_EEPROM_PAGE_SIZE;
	HAL_StatusTypeDef halSts = HAL_ERROR;

	halSts = HAL_I2C_IsDeviceReady(&hi2c, AT24MAC_EEPROM_ADDR, 500, 10);

#ifdef DEBUG
	if (halSts != HAL_OK)
		return halSts;
#endif

	halSts = HAL_I2C_Mem_Write(&hi2c, AT24MAC_EEPROM_ADDR, aMemAddress, memAddSize, aWriteBuf, writeLen, 10);

	return halSts;
}


HAL_StatusTypeDef AT24MAC_mem_read(I2C_HandleTypeDef hi2c, uint16_t aDevAddress, uint8_t aMemAddress, uint8_t aReadLen, uint8_t *rVal)
{
  	uint16_t memAddSize = 1;

	HAL_StatusTypeDef halSts = HAL_ERROR;
	halSts = HAL_I2C_IsDeviceReady(&hi2c, AT24MAC_EEPROM_ADDR, 500, 10);

#ifdef DEBUG
	if (halSts != HAL_OK)
		return halSts;
#endif

  	return HAL_I2C_Mem_Read(&hi2c, aDevAddress, aMemAddress, memAddSize, rVal, aReadLen, 10);
}

#if 0
HAL_StatusTypeDef I2C_RequestMemoryWriteAck(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint32_t Timeout, uint32_t Tickstart)
{
  /* Generate Start */
  SET_BIT(hi2c->Instance->CR1, I2C_CR1_START);

  /* Wait until SB flag is set */
  if (I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_SB, RESET, Timeout, Tickstart) != HAL_OK)
  {
    if (READ_BIT(hi2c->Instance->CR1, I2C_CR1_START) == I2C_CR1_START)
    {
      hi2c->ErrorCode = HAL_I2C_WRONG_START;
    }
    return HAL_TIMEOUT;
  }

  /* Send slave address */
  hi2c->Instance->DR = I2C_7BIT_ADD_WRITE(DevAddress);

  /* Wait until ADDR flag is set */
  if (I2C_WaitOnMasterAddressFlagUntilTimeout(hi2c, I2C_FLAG_ADDR, Timeout, Tickstart) != HAL_OK)
  {
    return HAL_ERROR;
  }

  /* Clear ADDR flag */
  __HAL_I2C_CLEAR_ADDRFLAG(hi2c);

  /* Wait until TXE flag is set */
  if (I2C_WaitOnTXEFlagUntilTimeout(hi2c, Timeout, Tickstart) != HAL_OK)
  {
    if (hi2c->ErrorCode == HAL_I2C_ERROR_AF)
    {
      /* Generate Stop */
      SET_BIT(hi2c->Instance->CR1, I2C_CR1_STOP);
    }
    return HAL_ERROR;
  }
}
#endif

/*#####################################################################
 * read MAC-Address, 8 byte - for 6 byte MAC-Address without byte 3 and byte 4
 */
HAL_StatusTypeDef AT24MAC_MACRead(I2C_HandleTypeDef hi2c)
{
	HAL_StatusTypeDef halSts = HAL_OK;
	uint8_t baseAddrMAC = 0x98;
	uint8_t macAddr[8] = {0,0,0,0,0,0,0,0};

	if ( (halSts = (AT24MAC_mem_read(hi2c, AT24MAC_MAC_ADDR, baseAddrMAC, 8, macAddr)) ) == HAL_OK)
	{
		for (int i = 0, ii = 0; i < 8; i++)
		{
			if ( (i != 3) && (i != 4) )	// bei 64bit EUI diese beiden nicht
			{
				_macAddr[ii] = macAddr[i];
				ii++;
			}
		}
	}
	else
	{
		;
	}
	return halSts;
}


/*#####################################################################
 * read 16 byte Serial-Number
 */
HAL_StatusTypeDef AT24MAC_SERNORead(I2C_HandleTypeDef hi2c, AT24_Serno* pSerno)
{
	uint8_t baseAddrSerNo = 0x80;
	HAL_StatusTypeDef halStatus = AT24MAC_mem_read(hi2c, AT24MAC_MAC_ADDR, baseAddrSerNo, 16, pSerno->SerNo);

	return halStatus;
}


/*
 * write complete CONFIG to EEPROM in 16 pages with 16 bytes
 * the complete config is in *aFlash
 */
HAL_StatusTypeDef AT24MAC_Write(I2C_HandleTypeDef hi2c, const tFlashCFG *aFlash)
{
	uint8_t cnt = 0;
	uint8_t baseAddr = EEPROM_BASEADDR;
	uint8_t writeBuf[CFG_EEPROM_PAGE_SIZE];
	const uint8_t *pNewCfg = (const uint8_t*)aFlash;

	int i = 0, j = 0;
	HAL_StatusTypeDef halResult = HAL_OK;

#ifdef DEBUG
	uint32_t diffTime = 0;
	uint32_t startTime = HAL_GetTick();
#endif
	for (i = 0; i < CFG_FLASH_SIZE; i += CFG_EEPROM_PAGE_SIZE )	// 16 pages a 16 byte
	{
		for (j = 0; j < CFG_EEPROM_PAGE_SIZE; j++)
		{
			writeBuf[j] = pNewCfg[i+j];
		}
		baseAddr = i;
		halResult = AT24MAC_mem_write(hi2c, baseAddr, writeBuf);

#if 0
		cnt = 0;
		while ( (halResult != HAL_OK) && (cnt < 5) )
		{
			cnt++;
			halResult = AT24MAC_mem_write(hi2c, baseAddr, writeBuf);
//			HAL_Delay(1);
		}
#endif

		HAL_Delay(5);	// for 34LC02
		if (halResult != HAL_OK)
			return halResult;	// if an error here, then not break -> return this error
	}
	// verify
	HAL_Delay(10);
	halResult = HAL_I2C_IsDeviceReady(&hi2c, AT24MAC_EEPROM_ADDR, 500, 10);

	tFlashCFG readback;
	halResult = AT24MAC_Read(hi2c, &readback);

#ifdef DEBUG
	diffTime = HAL_GetTick() - startTime;
#endif

	int ret = memcmp(&readback, aFlash, sizeof(readback));
	if (0 != ret) {
		printf("!!!!ERROR writing EEPROM!!!!\n");
		///! @TODO Eigentlich die() --> Hardware kaputt!
		///! @TODO write funktioniert aktuell nie!
		return HAL_ERROR;
	}
	return halResult;
}


/*
 * write only different pages from CONFIG to EEPROM in pages with 8/16 bytes
 * we have EEPROMS with 8bytes pages and with 16bytes pages
 * we can write 16bytes with 8bytes twice
 * the complete config is in *aFlash
 */
HAL_StatusTypeDef AT24MAC_WriteDiff(I2C_HandleTypeDef hi2c, const tFlashCFG *aFlash)
{
	int cnt = 0;
	uint8_t baseAddr = EEPROM_BASEADDR;
	uint8_t writeBuf[CFG_EEPROM_PAGE_SIZE];
	uint8_t readBuf[CFG_EEPROM_PAGE_SIZE];
	const uint8_t *pNewCfg = (const uint8_t*)aFlash;
	tFlashCFG readback;

	int i = 0, j = 0;
	HAL_StatusTypeDef halResult = HAL_OK;

#ifdef DEBUG
	uint32_t diffTime = 0;
	uint32_t startTime = HAL_GetTick();
#endif
	halResult = AT24MAC_Read(hi2c, &readback);
	uint8_t *pReadCfg = &readback;

	for (i = 0; i < CFG_FLASH_SIZE; i += CFG_EEPROM_PAGE_SIZE )	// 16 pages a 16 byte
	{
		for (j = 0; j < CFG_EEPROM_PAGE_SIZE; j++)
		{
			writeBuf[j] = pNewCfg[i+j];
			readBuf[j] = pReadCfg[i+j];
		}
		cnt = memcmp(writeBuf, readBuf, CFG_EEPROM_PAGE_SIZE);

		if (cnt != 0)	// there is a difference between eeprom and ram
		{
			baseAddr = i;
			halResult = AT24MAC_mem_write(hi2c, baseAddr, writeBuf);
		}
		HAL_Delay(5);	// for 34LC02
		if (halResult != HAL_OK)
			return halResult;	// if an error here, then not break -> return this error
	}
	// verify
	HAL_Delay(10);
	halResult = HAL_I2C_IsDeviceReady(&hi2c, AT24MAC_EEPROM_ADDR, 500, 10);

	halResult = AT24MAC_Read(hi2c, &readback);

#ifdef DEBUG
	diffTime = HAL_GetTick() - startTime;
#endif

	int ret = memcmp(&readback, aFlash, sizeof(readback));
	if (0 != ret) {
		printf("!!!!ERROR writing EEPROM!!!!\n");
		///! @TODO Eigentlich die() --> Hardware kaputt!
		///! @TODO write funktioniert aktuell nie!
		return HAL_ERROR;
	}
	return halResult;
}


/*
 * write only different pages from CONFIG to EEPROM in pages with 8/16 bytes
 * we have EEPROMS with 8bytes pages and with 16bytes pages
 * we can write 16bytes with 8bytes twice
 * the complete config is in *aFlash
 */
HAL_StatusTypeDef AT24MAC_WriteDiff_Async(I2C_HandleTypeDef hi2c, const tFlashCFG *aFlash)
{
	int cnt = 0;
	uint8_t baseAddr = 0x00;
	uint8_t writeBuf[CFG_EEPROM_PAGE_SIZE];
	uint8_t readBuf[CFG_EEPROM_PAGE_SIZE];
	const uint8_t *pNewCfg = (const uint8_t*)aFlash;

	int i = 0, j = 0;
	HAL_StatusTypeDef halResult = HAL_OK;

#ifdef DEBUG
	uint32_t diffTime = 0;
	uint32_t startTime = HAL_GetTick();
#endif
	uint8_t pReadCfg[sizeof(_ReadBack_Cfg)];

	// write EEPROM async
	switch(_WriteState)
	{
	case 0:		// begin
		halResult = AT24MAC_Read(hi2c, &_ReadBack_Cfg);
//		pReadCfg = &_ReadBack_Cfg;
		memcpy(pReadCfg, &_ReadBack_Cfg, sizeof(_ReadBack_Cfg));

		_WriteState = 1;
		break;

	case 1:
		for (i = 0; i < CFG_FLASH_SIZE; i += CFG_EEPROM_PAGE_SIZE )	// 16 pages a 16 byte
		{
			for (j = 0; j < CFG_EEPROM_PAGE_SIZE; j++)
			{
				writeBuf[j] = pNewCfg[i+j];
				readBuf[j] = pReadCfg[i+j];
			}
		}
		cnt = memcmp(writeBuf, readBuf, CFG_EEPROM_PAGE_SIZE);

		if (cnt != 0)	// there is a difference between eeprom and ram
		{
			baseAddr = i;
			halResult = AT24MAC_mem_write(hi2c, baseAddr, writeBuf);

			_PrgStart = HAL_GetTick();
			_WriteState = 2;
		}

		if (i >= CFG_FLASH_SIZE)
			_WriteReady = 0;
		break;

	case 2:
		//HAL_Delay(5);	// for 34LC02
		if ( (HAL_GetTick() - _PrgStart) >= 5)
		{
			if (_WriteReady)
			{
				_PrgStart = HAL_GetTick();
				_WriteState = 3;
			}
			else
				_WriteState = 1;
		}
		break;

	case 3:
		// verify
		// HAL_Delay(10);	before read again
		if ( (HAL_GetTick() - _PrgStart) >= 10)
		{
			halResult = HAL_I2C_IsDeviceReady(&hi2c, AT24MAC_EEPROM_ADDR, 500, 10);

			halResult = AT24MAC_Read(hi2c, &_ReadBack_Cfg);

			int ret = memcmp(&_ReadBack_Cfg, aFlash, sizeof(_ReadBack_Cfg));
			if (0 != ret)
			{
				printf("!!!!ERROR writing EEPROM!!!!\n");
				///! @TODO Eigentlich die() --> Hardware kaputt!
				///! @TODO write funktioniert aktuell nie!
				halResult = HAL_ERROR;
			}
			_WriteState = 0;
		}

		break;

	default:
		break;
	}

	return halResult;
}


/*
 * read complete CONFIG from EEPROM in 16 pages with 16 bytes
 * returns complete config in *rFlash
 */
HAL_StatusTypeDef AT24MAC_Read(I2C_HandleTypeDef hi2c, tFlashCFG *rFlash)
{
	uint8_t baseAddr = EEPROM_BASEADDR;
	uint8_t readBuf[CFG_EEPROM_PAGE_SIZE];

	uint8_t ramCfg[CFG_FLASH_SIZE];

	int i = 0, j = 0;
	HAL_StatusTypeDef halResult = HAL_OK;

	for (i = 0; i < CFG_FLASH_SIZE; i += CFG_EEPROM_PAGE_SIZE)
	{
		baseAddr = i;
		halResult = AT24MAC_mem_read(hi2c, AT24MAC_EEPROM_ADDR, baseAddr, CFG_EEPROM_PAGE_SIZE, readBuf);

		if (halResult != HAL_OK)
			break;

		for (j = 0; j < CFG_EEPROM_PAGE_SIZE; j++)
		{
			ramCfg[i+j] = readBuf[j];
		}
	}
	if (halResult == HAL_OK)
	{
		memcpy(rFlash, &ramCfg, CFG_FLASH_SIZE);
	}

	return halResult;
}


uint8_t AT24MAC_GetWriteState()
{
	return _WriteState;
}

/*
 * read complete config in static _ramCfg
 */
HAL_StatusTypeDef AT24MAC_ReadConfig(I2C_HandleTypeDef hi2c, tFlashCFG* pCfg)
{
	HAL_StatusTypeDef halResult = HAL_OK;

	halResult = AT24MAC_Read(hi2c, pCfg);

	return halResult;
}


void GetMAC_Addr(uint8_t *rMacAddr)
{
	for (int i = 0; i < 6; i++)
		rMacAddr[i] = _macAddr[i];
}


void GetHW_TTNR(uint8_t *rTTNR)
{
	memcpy(rTTNR, flashCfg.sTTNR, 16);
}


const tFlashCFG* AT24MAC_GetRomConfig()
{
	return &flashCfg;
}
