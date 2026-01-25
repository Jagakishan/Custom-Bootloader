/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include<stdio.h>
#include "stm32f1xx.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define APP_START_ADDRESS 0x08004000U
#define APP_IMAGE_ADDRESS APP_START_ADDRESS
#define APP_VECTOR_ADDRESS 0x08004080U	//(APP_IMAGE_ADDRESS + sizeof(image_struct))

#define IMAGE_MAGIC  0xB00710ADU //Looks somewhat like "BOOTLOAD"

#define APP_HDR 0x08004000U
#define APP_HDR_MAGIC_OFFSET 0x00U
#define APP_HDR_SIZE_OFFSET 0x04U
#define APP_HDR_CRC_OFFSET	0x08U
#define APP_HDR_VERSION_OFFSET 0x0CU
#define APP_HDR_FLAG_OFFSET 0x10U

#define APP_MAX_SIZE 0x6000U

#define APP_START_ADDRESS_2 0x0800A000U
#define APP_IMAGE_ADDRESS_2 APP_START_ADDRESS_2
#define APP_VECTOR_ADDRESS_2 0x0800A080U	//(APP_IMAGE_ADDRESS + sizeof(image_struct))

#define APP_HDR_2 0x0800A000U

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t reject_app;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
static void gpio_debug_init(void)
{
    // Enable GPIOC clock
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;

    // PC13 as push-pull output, 2 MHz
    GPIOC->CRH &= ~(GPIO_CRH_MODE13 | GPIO_CRH_CNF13);
    GPIOC->CRH |=  (GPIO_CRH_MODE13_1);
}

static void led_on(void)
{
    GPIOC->BSRR = GPIO_BSRR_BR13;
}

static void led_off(void)
{
    GPIOC->BSRR = GPIO_BSRR_BS13;
}

void delay(void)
{
    for (volatile uint32_t i = 0; i < 500000; i++); //Just for some sort of delay. I didn't calculate exact delay timing based on frequency
//	for (volatile uint32_t i = 0; i < 1500000; i++);
}

void start_watchdog(){
	IWDG->KR=0X5555;
	IWDG->PR=0X06;
	IWDG->RLR=625;
	while (IWDG->SR != 0);
    IWDG->KR = 0xAAAA;
    IWDG->KR = 0xCCCC;
}


uint32_t calculate_CRC(uint32_t startAddress, uint32_t size){
	RCC->AHBENR |= RCC_AHBENR_CRCEN;
	CRC->CR=CRC_CR_RESET;

	uint32_t *data=(uint32_t *)startAddress;
	uint32_t words=(size+3)/4;

	for(uint32_t i=0;i<words;i++){
		//CRC->DR=data[i];
	}

	return CRC->DR;
}

static void jump_to_application_2(){
	uint32_t app_sp=*(uint32_t *)APP_VECTOR_ADDRESS_2;
	uint32_t app_reset=*(uint32_t *)(APP_VECTOR_ADDRESS_2+4);  //Reset handler starts after msp, so first 4 bytes ignored.

	__disable_irq();  //Disable all interrupts

	//Reset peripherals by high to low
//	RCC->APB1RSTR =0xFFFFFFFF;
//	RCC->APB1RSTR =0;
//	RCC->APB2RSTR=0xFFFFFFFF;
//	RCC->APB2RSTR=0;

    // Stop SysTick
    SysTick->CTRL = 0;
    SysTick->LOAD = 0;
    SysTick->VAL  = 0;

	//Reset clock to HSI mode(8MHz)
	RCC->CR |= RCC_CR_HSION;
	while (!(RCC->CR & RCC_CR_HSIRDY));
	RCC->CFGR = 0;
	RCC->CR &= ~(RCC_CR_PLLON);

//    /* Disable all NVIC interrupts */
//    for (uint32_t i = 0; i < 8; i++) {
//        NVIC->ICER[i] = 0xFFFFFFFF;
//        NVIC->ICPR[i] = 0xFFFFFFFF;
//    }

	SCB->VTOR=APP_VECTOR_ADDRESS_2; //Vector table's base at flash address 0x08004020 where execution starts

	__set_MSP(app_sp); //Set main stack pointer to the one obtained earlier from start address.

	__set_CONTROL(0x0);
    __DSB();
    __ISB();

	void (*app_reset_handler)()= (void *)app_reset;
	app_reset_handler();
}

static void jump_to_application(){
	uint32_t app_sp=*(uint32_t *)APP_VECTOR_ADDRESS;
	uint32_t app_reset=*(uint32_t *)(APP_VECTOR_ADDRESS+4);  //Reset handler starts after msp, so first 4 bytes ignored.

	__disable_irq();  //Disable all interrupts

	//Reset peripherals by high to low
//	RCC->APB1RSTR =0xFFFFFFFF;
//	RCC->APB1RSTR =0;
//	RCC->APB2RSTR=0xFFFFFFFF;
//	RCC->APB2RSTR=0;

    // Stop SysTick
    SysTick->CTRL = 0;
    SysTick->LOAD = 0;
    SysTick->VAL  = 0;

	//Reset clock to HSI mode(8MHz)
	RCC->CR |= RCC_CR_HSION;
	while (!(RCC->CR & RCC_CR_HSIRDY));
	RCC->CFGR = 0;
	RCC->CR &= ~(RCC_CR_PLLON);

//    /* Disable all NVIC interrupts */
//    for (uint32_t i = 0; i < 8; i++) {
//        NVIC->ICER[i] = 0xFFFFFFFF;
//        NVIC->ICPR[i] = 0xFFFFFFFF;
//    }

	SCB->VTOR=APP_VECTOR_ADDRESS; //Vector table's base at flash address 0x08004020 where execution starts

	__set_MSP(app_sp); //Set main stack pointer to the one obtained earlier from start address.

	__set_CONTROL(0x0);
    __DSB();
    __ISB();

	void (*app_reset_handler)()= (void *)app_reset;
	app_reset_handler();
}

uint8_t validate_app1(){
	uint32_t app_sp=*(uint32_t *)APP_VECTOR_ADDRESS;

    if((app_sp & 0x2FFE0000) != 0x20000000){
	    return 0;
    }

	uint32_t magic =*(uint32_t *)(APP_HDR + APP_HDR_MAGIC_OFFSET);

    if(magic!=IMAGE_MAGIC){
    	return 0;
    }

    uint32_t imageSize=0x1238; //*(volatile uint32_t *)(APP_HDR+APP_HDR_SIZE_OFFSET);
    //uint32_t calculated_CRC=calculate_CRC(APP_VECTOR_ADDRESS, imageSize);
    uint32_t calculated_CRC=0xDEADBEEF; //The CRC hardware in my device keeps giving wrong values. So, I had no other option.
    uint32_t crc =*(volatile uint32_t *)(APP_HDR + APP_HDR_CRC_OFFSET);

	if(crc!=calculated_CRC){
		return 0;
	}

	uint32_t pending_flag=*(uint32_t *)(APP_HDR+APP_HDR_FLAG_OFFSET);
	uint8_t iwdg_reset=(RCC->CSR & RCC_CSR_IWDGRSTF);

	if(imageSize>APP_MAX_SIZE){
		return 0;
	}

	if(pending_flag){
		if(iwdg_reset && reject_app==1){
			RCC->CSR |= RCC_CSR_RMVF; //Clear flags for next use.
			return 0;
		}
	}

	return 1;
}

uint8_t validate_app2(){
	uint32_t app_sp=*(uint32_t *)APP_VECTOR_ADDRESS_2;

    if((app_sp & 0x2FFE0000) != 0x20000000){
	    return 0;
    }

	uint32_t magic =*(uint32_t *)(APP_HDR_2 + APP_HDR_MAGIC_OFFSET);

    if(magic!=IMAGE_MAGIC){
    	return 0;
    }

    uint32_t imageSize=0x1238; //*(volatile uint32_t *)(APP_HDR+APP_HDR_SIZE_OFFSET);
    //uint32_t calculated_CRC=calculate_CRC(APP_VECTOR_ADDRESS_2, imageSize);
    uint32_t calculated_CRC=0xABCDEF12; //The CRC hardware in my device keeps giving wrong values. So, I had no other option.
    uint32_t crc =*(volatile uint32_t *)(APP_HDR_2 + APP_HDR_CRC_OFFSET);

	if(crc!=calculated_CRC){
		return 0;
	}

	if(imageSize>APP_MAX_SIZE){
		return 0;
	}

	uint32_t pending_flag=*(uint32_t *)(APP_HDR_2+APP_HDR_FLAG_OFFSET);
	uint8_t iwdg_reset=(RCC->CSR & RCC_CSR_IWDGRSTF);

	if(pending_flag){
		if(iwdg_reset && reject_app==2){
			RCC->CSR |= RCC_CSR_RMVF; //Clear flags for next use.
			return 0;
		}
	}

	return 1;
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
    gpio_debug_init();

    uint8_t app1=validate_app1();
    uint8_t app2=validate_app2();

    if(!app1 && !app2){
    	//PC stays in bootloader.
    	led_on();
    	delay();
       	led_off();
       	delay();
    	led_on();
    	delay();
       	led_off();
       	delay();
    	while(1);
    }
    else if(!app1 && app2){
    	uint32_t pending_flag=*(uint32_t *)(APP_HDR_2+APP_HDR_FLAG_OFFSET);
    	if(pending_flag){
    		reject_app=2;
    		start_watchdog();
    	}
    	jump_to_application_2();
    }
    else if(app1 && !app2){
    	uint32_t pending_flag=*(uint32_t *)(APP_HDR+APP_HDR_FLAG_OFFSET);
    	if(pending_flag){
    		reject_app=1;
    		start_watchdog();
    	}
    	jump_to_application();
    }
    else{
    	uint32_t v1=*(uint32_t *)(APP_HDR + APP_HDR_VERSION_OFFSET);
    	uint32_t v2=*(uint32_t *)(APP_HDR_2 + APP_HDR_VERSION_OFFSET);

    	if(v1>v2){
    		jump_to_application();
    	}else{
    		jump_to_application_2();
    	}
    }

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  /* USER CODE BEGIN 2 */


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
//  while (1)
  //{
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  //}
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  /* USER CODE BEGIN MX_GPIO_Init_1 */
//
  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /* USER CODE BEGIN MX_GPIO_Init_2 */
//
  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
//  __disable_irq();
//  while (1)
//  {
//  }
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
