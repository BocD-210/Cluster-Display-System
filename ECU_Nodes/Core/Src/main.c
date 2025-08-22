/* USER CODE BEGIN Header */
/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define ADC_SAMPLES     10      // Số mẫu để tính trung bình
#define ADC_THRESHOLD   5       // Ngưỡng thay đổi mới gửi CAN

#define FILTER_SIZE 10			//
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

CAN_HandleTypeDef hcan;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim4;

/* USER CODE BEGIN PV */
volatile uint8_t pc13_led_state = 0;
volatile int16_t brightness = 1;
const int16_t BRIGHTNESS_MIN = 0;
const int16_t BRIGHTNESS_MAX = 100;

uint32_t adc_value = 0;
uint32_t pwm_value = 0;
uint8_t led_state = 0;
uint8_t led_state3 = 0;

volatile uint8_t flag_encoder = 0;
volatile uint8_t flag_button1 = 0;
volatile uint8_t flag_button2 = 0;
volatile uint8_t flag_button3 = 0;

volatile uint16_t encoder_value = 0;
volatile uint8_t  button_value1  = 0;
volatile uint8_t  button_value2  = 1;
volatile uint8_t  button_value3  = 0;

// Lưu giá trị lần trước
static uint16_t prev_encoder_value = 0xFFFF; // khởi tạo khác giá trị thực để lần đầu vẫn gửi
static uint8_t  prev_button_value  = 0;

uint32_t last_encoder_time = 0;
uint32_t last_button_time  = 0;
uint32_t last_button2_time  = 0;
uint32_t last_button3_time  = 0;
const uint32_t debounce_delay = 10; // ms

uint16_t adc_filtered = 0;

uint16_t encoder_buffer[FILTER_SIZE] = {0};
uint8_t encoder_index = 0;
uint16_t filtered_value = 0;

uint8_t led_status_left = 0;
uint8_t led_status_right = 0;
extern TIM_HandleTypeDef htim2;
extern CAN_HandleTypeDef hcan;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_CAN_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM4_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint32_t millis(void)
{
    return HAL_GetTick();
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM4)
    {
        if (led_state == 1)
        {
            //HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_8);
            led_status_left = !led_status_left;
            flag_button1 = 1;
            if(led_status_left)
            	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET);
            else
            	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET);
        }

        if (led_state3 == 1)
		{
			led_status_right = !led_status_right;
			flag_button3 = 1;
			if(led_status_right)
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
			else
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
		}
    }
}

//ENCODER + TIMER + SWITCH INTERRUPT

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	uint32_t now = millis();

    if (GPIO_Pin == GPIO_PIN_1) // PA1 = CLK
    {
    	if ((now - last_encoder_time) < debounce_delay) return;
			last_encoder_time = now;

        if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_2) == GPIO_PIN_RESET) {
            // Quay nguoc
            if (brightness > BRIGHTNESS_MIN)
                brightness--;
        } else {
            // Quay thuan
            if (brightness < BRIGHTNESS_MAX)
                brightness++;
        }

       //Capnhap trang thai led thong qua timer2_CH1
        __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, brightness);

        encoder_value = brightness;

        // ===== Lưu giá trị vào buffer l�?c trung bình =====
	   encoder_buffer[encoder_index] = encoder_value;
	   encoder_index = (encoder_index + 1) % FILTER_SIZE;

	   // Tính trung bình
	   uint32_t sum = 0;
	   for (uint8_t i = 0; i < FILTER_SIZE; i++) {
		   sum += encoder_buffer[i];
	   }
	   filtered_value = sum / FILTER_SIZE;

	   // Chỉ set flag khi giá trị đã l�?c thay đổi
	   if (filtered_value != prev_encoder_value) {
		   prev_encoder_value = filtered_value;
		   flag_encoder = 1; // Báo cho main loop gửi CAN
	   }

    }
    else if (GPIO_Pin == GPIO_PIN_0) // PA0 = Switch
	{
    	if ((now - last_button_time) < debounce_delay) return;
			last_button_time = now;

		uint8_t pin_state = (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_SET) ? 1 : 0;
		//HAL_Delay(50);
    	if (pin_state != prev_button_value) {
			prev_button_value = pin_state;

			if (pin_state == 1) {
				button_value1 = !button_value1;
				led_state = !led_state;
				//HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, led_state ? GPIO_PIN_RESET : GPIO_PIN_SET);

				if(!led_state)
				   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8,GPIO_PIN_RESET);
			}
			else{
				led_status_left = 0;
				flag_button1 = 1;
			}
		}
	}

    else if (GPIO_Pin == GPIO_PIN_4) // PA4 = Switch 2
    {
        if ((now - last_button2_time) < debounce_delay) return;
        last_button2_time = now;

        static uint8_t prev_button_value2 = 0;
        static uint8_t led_state2 = 0;

        uint8_t pin_state = (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4) == GPIO_PIN_SET) ? 1 : 0;
        if (pin_state != prev_button_value2) {
            prev_button_value2 = pin_state;

            if (pin_state == 1) {
                button_value2 = !button_value2;
                led_state2 = !led_state2;
                HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, led_state2 ? GPIO_PIN_RESET : GPIO_PIN_SET);
            }

            flag_button2 = 1;
        }
    }
    else if (GPIO_Pin == GPIO_PIN_5) // PA4 = Switch 2
   {
	   if ((now - last_button3_time) < debounce_delay) return;
	   last_button3_time = now;

	   static uint8_t prev_button_value3 = 0;
	   //static uint8_t led_state3 = 0;

	   uint8_t pin_state = (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5) == GPIO_PIN_SET) ? 1 : 0;
	   //HAL_Delay(50);
	   if (pin_state != prev_button_value3) {
		   prev_button_value3 = pin_state;

		   if (pin_state == 1) {
			   button_value3 = !button_value3;
			   led_state3 = !led_state3;
			   //HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, led_state3 ? GPIO_PIN_RESET : GPIO_PIN_SET);

			   if(!led_state3)
				   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
		   }
		   else{
			led_status_right = 0;
			flag_button3 = 1;
		   }
	   }
   }
}

uint16_t Read_ADC(void){
	uint32_t sum = 0;
	    for (uint8_t i = 0; i < ADC_SAMPLES; i++) {
	        HAL_ADC_Start(&hadc1);
	        if (HAL_ADC_PollForConversion(&hadc1, 100) == HAL_OK) {
	        	adc_value = HAL_ADC_GetValue(&hadc1);

				pwm_value = (adc_value * 1000) / 4095;

				__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, pwm_value);

	            sum += HAL_ADC_GetValue(&hadc1);
	        }
	        HAL_ADC_Stop(&hadc1);
	        HAL_Delay(2); // delay ngắn để tránh đ�?c liên tục
	    }
	    return (uint16_t)(sum / ADC_SAMPLES); // Trả v�? trung bình
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

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
  MX_ADC1_Init();
  MX_CAN_Init();
  MX_TIM2_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1); //Start PWM on TIM2 CH1
  __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 0);

  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2); // Start PWM on TIM2 CH2

  HAL_TIM_Base_Start_IT(&htim4); // Bắt đầu Timer


  HAL_CAN_Start(&hcan); // Start CAN

  CAN_TxHeaderTypeDef TxHeader;

  uint8_t TxData[8];
  uint32_t TxMailbox;

  TxHeader.IDE = CAN_ID_STD;
  TxHeader.RTR = CAN_RTR_DATA;
  static uint16_t prev_adc_value = 0xFFFF;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  uint16_t adc_now = Read_ADC();
	  if ( (adc_now > prev_adc_value + ADC_THRESHOLD) ||
		   (adc_now < prev_adc_value - ADC_THRESHOLD) ) {

		  prev_adc_value = adc_now;

		  TxHeader.StdId = 0x101;
		  TxHeader.DLC = 2;
		  TxData[0] = (adc_now >> 8) & 0xFF;
		  TxData[1] = adc_now & 0xFF;
		  HAL_CAN_AddTxMessage(&hcan, &TxHeader, TxData, &TxMailbox);
	  }

	  // Gửi encoder nếu có flag
	  if (flag_encoder) {
		  flag_encoder = 0;
		  TxHeader.StdId = 0x100; // ID cho encoder
		  TxHeader.DLC = 2;
		  TxData[0] = (filtered_value >> 8) & 0xFF;
		  TxData[1] = filtered_value & 0xFF;
		  HAL_CAN_AddTxMessage(&hcan, &TxHeader, TxData, &TxMailbox);
	  }

	  // Gửi button nếu có flag : Xi Nhan trái
	  if (flag_button1) {
		  flag_button1 = 0;

		  TxHeader.StdId = 0x103; // ID cho button

		  TxHeader.DLC = 1;
		  //TxData[0] = button_value1;
		  TxData[0] = led_status_left;
		  HAL_CAN_AddTxMessage(&hcan, &TxHeader, TxData, &TxMailbox);
	  }

	  // Gửi button nếu có flag : �?ÈN
	  if (flag_button2) {
		  flag_button2 = 0;

		  TxHeader.StdId = 0x105; // ID cho button

		  TxHeader.DLC = 1;
		  TxData[0] = button_value2;
		  HAL_CAN_AddTxMessage(&hcan, &TxHeader, TxData, &TxMailbox);
	  }

	  // Gửi button nếu có flag : Xi Nhan Phải
	  if (flag_button3) {
		  flag_button3 = 0;

		  TxHeader.StdId = 0x104; // ID cho button

		  TxHeader.DLC = 1;
		  TxData[0] = led_status_right;
		  HAL_CAN_AddTxMessage(&hcan, &TxHeader, TxData, &TxMailbox);
	  }

	  HAL_StatusTypeDef status = HAL_CAN_AddTxMessage(&hcan, &TxHeader, TxData, &TxMailbox);
	  if (status != HAL_OK) {
		  // Blink LED để báo lỗi
		  HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
	  }

  }
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL8;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV4;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */
  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_3;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief CAN Initialization Function
  * @param None
  * @retval None
  */
static void MX_CAN_Init(void)
{

  /* USER CODE BEGIN CAN_Init 0 */

  /* USER CODE END CAN_Init 0 */

  /* USER CODE BEGIN CAN_Init 1 */

  /* USER CODE END CAN_Init 1 */
  hcan.Instance = CAN1;
  hcan.Init.Prescaler = 16;
  hcan.Init.Mode = CAN_MODE_NORMAL;
  hcan.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan.Init.TimeSeg1 = CAN_BS1_2TQ;
  hcan.Init.TimeSeg2 = CAN_BS2_1TQ;
  hcan.Init.TimeTriggeredMode = DISABLE;
  hcan.Init.AutoBusOff = DISABLE;
  hcan.Init.AutoWakeUp = DISABLE;
  hcan.Init.AutoRetransmission = DISABLE;
  hcan.Init.ReceiveFifoLocked = DISABLE;
  hcan.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN_Init 2 */

  CAN_FilterTypeDef sFilterConfig;
  sFilterConfig.FilterBank = 0;
  sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
  sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
  sFilterConfig.FilterIdHigh = 0x0000;
  sFilterConfig.FilterIdLow = 0x0000;
  sFilterConfig.FilterMaskIdHigh = 0x0000;
  sFilterConfig.FilterMaskIdLow = 0x0000;
  sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
  sFilterConfig.FilterActivation = ENABLE;
  sFilterConfig.SlaveStartFilterBank = 14;

  HAL_CAN_ConfigFilter(&hcan, &sFilterConfig);

  /* USER CODE END CAN_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 31;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 100;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 31999;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 499;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8, GPIO_PIN_SET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PA0 PA4 PA5 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_4|GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PA1 */
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PA2 */
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB6 PB7 PB8 */
  GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

  HAL_NVIC_SetPriority(EXTI4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
