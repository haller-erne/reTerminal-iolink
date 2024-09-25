/*
 * main_reTerm.h
 *
 *  Created on: May 29, 2024
 *      Author: ahmad.abbasian.a
 */
#include <stdio.h>
#include <i2c.h>

#ifndef MAIN_RETERM_H_
#define MAIN_RETERM_H_

#pragma pack(push, 1)
struct SPI_CMD_HDR {
	uint8_t		cmd;
	uint8_t		len;
	uint8_t		opt;
};
#pragma pack(pop)

class main_reTerm {

protected:

	typedef struct _registerDisplay
		{
			uint16_t reg;
			uint32_t sndData;
			uint32_t rcvData;
			uint32_t sndCount;
			uint32_t rcvCount;
			uint8_t valueType;	// 0->integer, 1->float
		} registerDisplay;

		registerDisplay _vri[10];
		registerDisplay _vrg[6];

	typedef struct _SystemFlag
		{
			uint8_t ioLink_Set_Packet;
			uint8_t ioLink_Get_Packet;
			uint8_t SPI_Set_Packet;
		} SystemFlag;
		SystemFlag SysFlag;

		uint32_t _timeOut = 0;			// wait for answers



public:

		enum dispRegSts
		{
			clear_CS		,
			transmit		,	// transmit data to display
			transmit2		,	// transmit second part data to display
			waitSetReg		,
			waitSendReg		,
			waitTransmitted ,	// wait for transmitted data - send receive for acknowledge
			waitAcknowledge ,	// wait for acknowledge
			receivedAcknowledge ,	// acknowledge received

			idle					= 0xff
		};

	//! The one and only instance of this class
	static main_reTerm instance;
	main_reTerm();
	virtual ~main_reTerm();
	void HandleCommandReceived(void);
	void Get_SPI_Data(uint8_t*);
	void set32ByteTo36Byte(uint8_t*,bool);
	void set36ByteTo32Byte(uint8_t*,bool);
	void setErrorPacket();
	void reTerm_init(void);
	void SetIOLinkState(bool);
	bool ExchangeIOLinkData(const uint8_t pdo[32], uint8_t pdi[32]);
	uint8_t buffer2bcc(uint8_t *dat, uint8_t anz);
	uint8_t calculateCRC(uint8_t* data, uint16_t length);
	void buffer2crc16(uint8_t *aDat, uint16_t *pCRC, uint32_t aAnz);
	void crc16 (uint8_t aDat, volatile uint16_t *rCrc);
private:


};

// routines for interrupt outside class
//void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi);
//void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi);

#endif /* reTerm_H_ */
