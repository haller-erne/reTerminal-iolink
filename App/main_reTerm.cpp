/*
 * main_reTerm.cpp
 *
 *  Created on: May 29, 2024
 *      Author: Ahmad.Abbasian.A
 */

#include <stdio.h>
#include <string.h>

#include <main_reterm.h>
#include "config/config.h"
#include "spi.h"

// SPI status flags
uint8_t _spi2_rx_complete = 1;
uint8_t _spi2_tx_complete = 1;

// SPI transmission and reception buffers
uint8_t spi2_tx_Buffer[36];
uint8_t spi2_rx_Buffer[36];

// Buffers for cyclic PDO and PDI data
uint8_t cyclic_pdo[32];
uint8_t cyclic_pdi[32];

bool iolink_running;         // Status of the IOLink
uint32_t t_spiLastCmd = 0;   // Timestamp of the last SPI command
uint32_t Start_packetTime = 0;	// Time handling for SPI packagee

main_reTerm main_reTerm::instance;

// Constructor: Initializes the SPI interface
main_reTerm::main_reTerm()
{
	LL_SPI_Enable(SPI2);
}

main_reTerm::~main_reTerm()
{
	// TODO Auto-generated destructor stub
}

//----------------------------------------------------------------------------

// Converts 32-byte data to a 36-byte format and transmits it over SPI
void main_reTerm::set32ByteTo36Byte(uint8_t *data32byte, bool iolink_state)
{
	HAL_StatusTypeDef res = HAL_OK;
	uint32_t SPI_packetTime = 0;
	uint8_t tmp[32] ;
    uint32_t len= 0;

    // Copy data to a temporary buffer
	for (int i = 0; i < 32; i++)
	  tmp[i] = data32byte[i];

	   // Build the SPI transmission buffer
		spi2_tx_Buffer [len++] = CMD1;	// DC4
		spi2_tx_Buffer [len++] = 0x20;	//data length = 32 byte )
		spi2_tx_Buffer [len++] = iolink_running;	// IOLink state

	// Append data to the transmission buffer
	for (int i = 0; i < 32; i++)
			spi2_tx_Buffer[len++] = tmp[i];

	 // Calculate and append CRC
     spi2_tx_Buffer[len] = calculateCRC(spi2_tx_Buffer,len);

     // Reinitialize the SPI to ensure a clean state before transmission
		  HAL_SPI_DeInit(&hspi2);
		__HAL_RCC_SPI2_FORCE_RESET();
	    __HAL_RCC_SPI2_RELEASE_RESET();
		  HAL_SPI_Init(&hspi2);

	// Transmit and receive data via SPI
	res = HAL_SPI_TransmitReceive_DMA(&hspi2,(uint8_t*)spi2_tx_Buffer,(uint8_t*)spi2_rx_Buffer,36);
	if (res != HAL_OK)
	   return;

	// Check for timeout
	if (HAL_GetTick() - Start_packetTime > 1000) { // if no answer from display for one second, then ERROR
			SPI_packetTime = 1;	                        // SPI packet parsing timeout detected
		}
}

//----------------------------------------------------------------------------
// Converts 36-byte data to 32-byte format for processing
void main_reTerm::set36ByteTo32Byte(uint8_t* data36byte, bool iolink_state)
{
	uint8_t tmp[32];
//	for (int i = 3, ii = 31; i < 35; i++, ii--)
//		tmp[ii] = data36byte[i];

	// Extract data starting from the fourth byte
	for (int i = 0; i < 32; i++)
		tmp[i] = data36byte[i+3];

	memcpy(cyclic_pdi, &tmp, 32); // Store in cyclic PDI

}

// Constructs and sends an error packet
void main_reTerm::setErrorPacket()
{
	HAL_StatusTypeDef res = HAL_OK;
    uint32_t len= 0;

    // Build the error packet
	spi2_tx_Buffer [len++] = 16;	// Error Command
	spi2_tx_Buffer [len++] = 0x20;	//data length = 32 byte )
	spi2_tx_Buffer [len++] = 0;	//

	// Fill the rest with zeros
	for (int i = 0; i < 32; i++)
		spi2_tx_Buffer[len++] = 0;

	// Calculate and append CRC
    spi2_tx_Buffer[len] = calculateCRC(spi2_tx_Buffer,len);

    // Reinitialize SPI for transmission
	HAL_SPI_DeInit(&hspi2);
	__HAL_RCC_SPI2_FORCE_RESET();
	__HAL_RCC_SPI2_RELEASE_RESET();
	HAL_SPI_Init(&hspi2);

	// Transmit error packet
	res = HAL_SPI_TransmitReceive_DMA(&hspi2,(uint8_t*)spi2_tx_Buffer,(uint8_t*)spi2_rx_Buffer,36);
		  if (res != HAL_OK)
			  return;
}

//----------------------------------------------------------------------------
// Sets the IOLink running state
void main_reTerm::SetIOLinkState(bool isRunning)
{
	iolink_running = isRunning;
}

// Exchanges data with IOLink; returns true if successful
bool main_reTerm::ExchangeIOLinkData(const uint8_t pdo[32], uint8_t pdi[32])
{
	LL_EXTI_DisableIT_0_31(LL_EXTI_LINE_5); //Disable interrupt
	memcpy(pdi, cyclic_pdi, sizeof(cyclic_pdi));
	memcpy(cyclic_pdo, pdo, sizeof(cyclic_pdo));
	LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_5); //Enable interrupt

	uint32_t ticker = HAL_GetTick();
	// Check for SPI timeout
	if (ticker - t_spiLastCmd > 1000) {
		// SPI timeout detected
		return false;
	}
	return true;
}
//----------------------------------------------------------------------------
// Handles commands received over SPI
void HandleCommandReceived()
{
	bool fValid = false;
	uint16_t len = 0;

	// Analyze the header bytes 0..2
	SPI_CMD_HDR* pHdr = (SPI_CMD_HDR*)spi2_rx_Buffer;

    // Record start time
	Start_packetTime = HAL_GetTick();

	// Calculate and compare checksum
	len = pHdr->len;

	// Calculate checksum
	uint8_t calculatedCRC = main_reTerm::instance.calculateCRC(spi2_rx_Buffer, len + 3);

	// Compare checksum
	if (calculatedCRC == spi2_rx_Buffer[len + 3])
			fValid = true;
	else
		fValid = false;

	// Process command based on validity
    if (fValid) {
	    switch (pHdr->cmd){
	        case CMD1:			// exchange cyclic IO
		        t_spiLastCmd = HAL_GetTick();
		        main_reTerm::instance.set32ByteTo36Byte(cyclic_pdo, iolink_running);
		        main_reTerm::instance.set36ByteTo32Byte(spi2_rx_Buffer, iolink_running);
		        break;

	         case CMD2:			// Execute commands related to this function
	        	 break;

	          default:
		        // unknown command???
		        main_reTerm::instance.setErrorPacket();
		        break;
	  }
   }else {
	   // Handle invalid packet
	   main_reTerm::instance.setErrorPacket();
}
}

//----------------------------------------------------------------------------
// Calculates the CRC for a given data buffer
//Function: calculateCRC()
//input:    Data, length
//output:   crc
//Description:   calculateCRC (...
//----------------------------------------------------------------------------
uint8_t main_reTerm::calculateCRC(uint8_t* data, uint16_t length) {
    uint8_t crc = 0x00;  // Initial CRC value
    for (uint16_t i = 0; i < length; i++) {
        crc ^= data[i];  // XOR with data
        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ 0x07; // // Polynomial for CRC calculation
            } else {
                crc <<= 1;
            }
        }
    }
    return crc; // Return calculated CRC
}

//----------------------------------------------------------------------------
// Initializes the reTerm communication
void main_reTerm::reTerm_init(void)
{
  HAL_SPI_TransmitReceive_DMA(&hspi2,(uint8_t*)spi2_tx_Buffer,(uint8_t*)spi2_rx_Buffer,36);

}

//----------------------------------------------------------------------------
// Callback function for SPI transmission/reception completion
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{

if(hspi->Instance==SPI2)
{
  HandleCommandReceived(); // Handle received command
}

}
