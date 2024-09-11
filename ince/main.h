/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

#include "stm32f4xx_ll_iwdg.h"
#include "stm32f4xx_ll_rcc.h"
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_system.h"
#include "stm32f4xx_ll_exti.h"
#include "stm32f4xx_ll_cortex.h"
#include "stm32f4xx_ll_utils.h"
#include "stm32f4xx_ll_pwr.h"
#include "stm32f4xx_ll_dma.h"
#include "stm32f4xx_ll_spi.h"
#include "stm32f4xx_ll_gpio.h"
#include "stm32f4xx_ll_tim.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
extern unsigned long __m_text_rom_bldr_len;     ///!< Size of bootloader section (16k)
extern unsigned long __m_ram_vects_addr;        ///!< Start address of ram vectors (0x1000000)
extern unsigned long __m_ram_vects_len;         ///!< Size of ram vectors (0x0200) - 102 vectors = 0x0198
extern unsigned long __m_appl_addr;             ///!< Start address of application (i.e. flash irq vectors)
extern unsigned long __m_appl_addr_len;         ///!< Size of application section (64k)

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);
/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define SHDN_Pin LL_GPIO_PIN_0
#define SHDN_GPIO_Port GPIOC
#define LED1_Pin LL_GPIO_PIN_1
#define LED1_GPIO_Port GPIOC
#define LED2_Pin LL_GPIO_PIN_2
#define LED2_GPIO_Port GPIOC
#define LED3_Pin LL_GPIO_PIN_3
#define LED3_GPIO_Port GPIOC
#define IO_Link_SPI_NSS_Pin LL_GPIO_PIN_4
#define IO_Link_SPI_NSS_GPIO_Port GPIOA
#define IO_Link_SPI_SCK_Pin LL_GPIO_PIN_5
#define IO_Link_SPI_SCK_GPIO_Port GPIOA
#define IO_Link_SPI_MISO_Pin LL_GPIO_PIN_6
#define IO_Link_SPI_MISO_GPIO_Port GPIOA
#define IO_Link_SPI_MOSI_Pin LL_GPIO_PIN_7
#define IO_Link_SPI_MOSI_GPIO_Port GPIOA
#define IO_Link_SPI_INT_Pin LL_GPIO_PIN_5
#define IO_Link_SPI_INT_GPIO_Port GPIOC
#define IO_Link_SPI_INT_EXTI_IRQn EXTI9_5_IRQn

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
