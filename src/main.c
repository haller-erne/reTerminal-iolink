/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "iwdg.h"
#include "rtc.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdio.h>
#include "stackBase.h"
#include "demoapp.h"
#include "systemCommand.h"
#include "..\mstkn\mstkn-master\intern\ModBus_Schnittstelle.h"
#include "main_mstkn.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
void SystemClock_Config(void);
static void MX_NVIC_Init(void);
/* USER CODE BEGIN PFP */
void PeriphCommonClock_Config(void);
static void MX_NVIC_Init(void);
/* USER CODE BEGIN PFP */
void SystemClock_Config_401(uint32_t aPLLM);
void SystemClock_Change(uint32_t CoreClock, uint32_t aPLLM);

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */
/**
 * With this "function" you are able to get signature of device.
 *
 * Possible returns:
 *    - 0x0413: STM32F405xx/07xx and STM32F415xx/17xx)
 *    - 0x0419: STM32F42xxx and STM32F43xxx
 *    - 0x0423: STM32F401xB/C
 *    - 0x0433: STM32F401xD/E
 *    - 0x0431: STM32F411xC/E
 *
 * Returned data is in 16-bit mode, but only bits 11:0 are valid, bits 15:12 are always 0.
 * Defined as macro
 */
#define TM_ID_GetSignature()    (DBGMCU->IDCODE & 0x00000FFF)
/**
 * With this "function" you are able to get revision of device.
 *
 * Revisions possible:
 *    - 0x1000: Revision A
 *    - 0x1001: Revision Z
 *    - 0x1003: Revision Y
 *    - 0x1007: Revision 1
 *    - 0x2001: Revision 3
 *
 * Returned data is in 16-bit mode.
 * Defined as macro
 */
#define TM_ID_GetRevision() 	((DBGMCU->IDCODE >> 16) & 0x0000FFFF)


/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_NVIC_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

// Delay for ~100ms using the RTC and measure the number of Systicks
// This can then be used to estimate the actual SYSCLK frequency.
uint32_t calibrate_ticks100ms(int resetWdt)
{
    volatile uint32_t r = RTC->SSR;
    while (RTC->SSR != 25) {
    	if (resetWdt) LL_IWDG_ReloadCounter(IWDG);
    }
    uint32_t t0 = HAL_GetTick();
    while (RTC->SSR != 0) {
    	if (resetWdt) LL_IWDG_ReloadCounter(IWDG);
    }
    uint32_t t1 = HAL_GetTick();
    uint32_t delta = t1 - t0;
    return delta;
}


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
#define NVIC_RAM_VECTOR_ADDRESS   (&__m_ram_vects_addr)  // Location of vectors in RAM

// STM32F411RE
// CORE: 16 vectors = 64 bytes from 0x00 to 0x3F
// MCU Peripherals: 86 vectors = 344 bytes from 0x40 to 0x197
// Total: 102 vectors = 408 bytes (0x198) to be reserved in RAM
// 16th handler is SysTick_Handler, 102nd is SPI4_IRQHandler (in startup_stm32f401xc.s), STM32F401 has no SPI4
#define NVIC_NUM_VECTORS      102
#define NVIC_USER_IRQ_OFFSET  16

	__disable_irq(); //Should still be disabled from when you jumped out of the bootloader, but disable again for safety.
	uint32_t *old_vectors = (uint32_t *)(&__m_appl_addr); //Or wherever you placed your main application
	uint32_t *vectors = (uint32_t*)NVIC_RAM_VECTOR_ADDRESS;
	for (int i = 0; i < NVIC_NUM_VECTORS; i++)
	{
		vectors[i] = old_vectors[i];
	}
	SCB->VTOR = (uint32_t)NVIC_RAM_VECTOR_ADDRESS;

	__enable_irq(); //Re-enable IRQ. In theory your vector table should now be copied and interrupt *should* work.


  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  //#if new_clock
    uint16_t rev = TM_ID_GetRevision();
    uint16_t sign = TM_ID_GetSignature();

    // The controller runs in HSI-Mode (only to start everything so far)
    // We will now switch to HSE-Mode - assuming there is a 24MHz external crystal
    // Note that this is the "safe" assumption. If instead an external 12MHz crystal
    // is connected, then the CPUs internal frequency (SYSCLK) will be lower (which
    // is safe).
    // The overall aim is to setup SYSCLK to have 84MHz.

  	// Setup the PLL for SYSCLK=84MHz, assume external crystal 24MHz
  	// HSE_VALUE = 24000000U --> divider = 12
  	SystemClock_Change(84000000, (uint32_t)12);

  	// Initialize the real time clock to validate the actual HSE frequency
    MX_RTC_Init();

    // Delay for ~100ms using the RTC and measure the number of Systicks
    // This can then be used to estimate the actual SYSCLK frequency.
    uint32_t delta = calibrate_ticks100ms(0);
    if (delta < 75)
    {
    	// Wrong clock was selected above - we seem to run with a 12MHz crystal
    	// HSE_VALUE = 12000000U --> divider = 6
    	SystemClock_Change(84000000, (uint32_t)6);
    }
    uint32_t delta2 = calibrate_ticks100ms(0);

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_SPI1_Init();
  MX_IWDG_Init();
  MX_I2C3_Init();
  MX_TIM3_Init();
  MX_RTC_Init();
  MX_TIM2_Init();
  MX_TIM5_Init();

  /* Initialize interrupts */
  MX_NVIC_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start_IT(&htim3);

  Stack::instance.configure();
  DemoApp::configure();

  // in GPIO.C this INT must not be ENABLE, there it is to early
  // but here it must be ENABLE
  NVIC_EnableIRQ(EXTI9_5_IRQn);

  /* Enable interrupts and timers */
  LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_5);  // Phy Interrupt
  LL_TIM_EnableCounter(TIM2);            // TIM22
  LL_TIM_EnableIT_UPDATE(TIM2);

  DemoApp& theApp = DemoApp::instance;


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	    __WFI();

		LL_IWDG_ReloadCounter(IWDG);

		if (GetPC_Cmd() == 1)
		{
			Stack::instance.stopInterrupt();

			EEPROM::poll();	// if write not IDLE
			theApp.run_IO();

			if (Get_uart1_tx_ready())
			{
				SetPC_Cmd(0);	// set PC-Command to zero, PC-BootLoader command ready
			}
		}
		else

			{
			// check if it's time to run user code
			const Stack::Parameter* paramWrite;
			if (Stack::instance.canRunUserCode(paramWrite))
			{
			  Stack::instance.stopInterrupt();
			  theApp.run(paramWrite);

			  if (!SystemCommand::isReady(SystemCommand::SYSCMD_BACK_TO_BOX))
			  {
				Stack::instance.restartInterrupt();
			  }
			}
	    }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_0);
  while(LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_0)
  {
  }
  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE2);
  LL_RCC_HSI_SetCalibTrimming(16);
  LL_RCC_HSI_Enable();

   /* Wait till HSI is ready */
  while(LL_RCC_HSI_IsReady() != 1)
  {

  }
  LL_RCC_LSI_Enable();

   /* Wait till LSI is ready */
  while(LL_RCC_LSI_IsReady() != 1)
  {

  }
  LL_PWR_EnableBkUpAccess();
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_2);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSI);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSI)
  {

  }
  LL_SetSystemCoreClock(16000000);

   /* Update the time base */
  if (HAL_InitTick (TICK_INT_PRIORITY) != HAL_OK)
  {
    Error_Handler();
  }
  LL_RCC_SetTIMPrescaler(LL_RCC_TIM_PRESCALER_TWICE);
}

/**
  * @brief NVIC Configuration.
  * @retval None
  */
static void MX_NVIC_Init(void)
{
  /* TIM3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(TIM3_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(TIM3_IRQn);
  /* USART1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(USART1_IRQn);
  /* USART2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(USART2_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(USART2_IRQn);
}

/* USER CODE BEGIN 4 */
void SystemClock_Change(uint32_t CoreClock, uint32_t aPLLM)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  // First reset the clock configuration to the default reset state
  HAL_RCC_DeInit();
  RCC->CR |= RCC_CR_HSEON;					// enable/start HSE
  while (!(RCC->CR & RCC_CR_HSERDY));		// wait until HSE is ready

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = aPLLM;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 7;

  HAL_StatusTypeDef sts;
  if ((sts = HAL_RCC_OscConfig(&RCC_OscInitStruct)) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }

  /** fix the SystemCoreClock value and re-intialize SYSTICK
   */
  SystemCoreClockUpdate();
  SystemCoreClock = CoreClock;
  HAL_InitTick (HAL_GetTickPrio());
}


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
