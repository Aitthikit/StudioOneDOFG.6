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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define WHEEL_RADIUS 0.05f
#define ROBOT_LENGTH 0.276f
#define ROBOT_WIDTH 0.276f
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc3;
DMA_HandleTypeDef hdma_adc3;

UART_HandleTypeDef hlpuart1;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;
TIM_HandleTypeDef htim5;
TIM_HandleTypeDef htim8;
TIM_HandleTypeDef htim15;
TIM_HandleTypeDef htim16;
TIM_HandleTypeDef htim17;
TIM_HandleTypeDef htim20;

/* USER CODE BEGIN PV */
uint64_t QEIReadRaw;
uint16_t ADC_RawRead[40]={0};
uint8_t Proximity[8] ={0};
uint16_t Switch[3] ={0};
uint32_t EncodeRead[4] ={0};
uint8_t sensorThick = 1;

float angular[4] = {0};
float omega[4] = {0};
float omega_Old[4] = {0};
float omega_Filter[4] = {0};
float omega_Filter_Old[4] = {0};
float speed = 0;
float baseSpeed = 0.23;
float testSpeed = 500;

float PWMmap[4];

float v_x = 0.5f;  // Desired velocity in the X direction (m/s)
float v_y = 0.0f;  // Desired velocity in the Y direction (m/s)
float Robot_omega = 0.0f; // Desired rotational velocity (rad/s)
float wheel_speeds[4];

float base_X = 0.0f;
float base_Y = 0.0f;
float base_Omega = 0.0f;

float cali_X = 0.0f;
float cali_Y = 0.0f;
float cali_Omega = 0.0f;

uint8_t state_ALL = 0;
uint8_t state_Check = 0;
uint8_t state_Clean = 0;
uint8_t n = 0;
uint8_t path = 0;
uint8_t path1[10] = {5,1,0,5,0,1,3,1,0,6};
uint8_t path2[10] = {4,1,0,4,0,1,2,1,0,6};
uint16_t delayCount;


typedef struct
{
// for record New / Old value to calculate dx / dt
uint32_t Position1[2];
uint32_t Position2[2];
uint32_t Position3[2];
uint32_t Position4[2];
uint64_t TimeStamp[2];
float QEIPostion_1turn;
float QEIAngularVelocity1;
float QEIAngularVelocity2;
float QEIAngularVelocity3;
float QEIAngularVelocity4;
}QEI_StructureTypeDef;
QEI_StructureTypeDef QEIdata = {0};
uint64_t _micros = 0;

typedef struct
{
// for record New / Old value to calculate dx / dt
float Output[3];
float Error[3];
float kp;
float ki;
float kd;
float T;
float omega[3];
}PID_StructureTypeDef;
PID_StructureTypeDef Velocontrol = {0};
PID_StructureTypeDef Poscontrol = {0};
PID_StructureTypeDef Velocontrol2 = {0};
PID_StructureTypeDef Poscontrol2 = {0};
PID_StructureTypeDef Velocontrol3 = {0};
PID_StructureTypeDef Poscontrol3 = {0};
PID_StructureTypeDef Velocontrol4 = {0};
PID_StructureTypeDef Poscontrol4 = {0};

enum
{
NEW,OLD,OLDER
};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_LPUART1_UART_Init(void);
static void MX_TIM3_Init(void);
static void MX_ADC3_Init(void);
static void MX_TIM4_Init(void);
static void MX_TIM5_Init(void);
static void MX_TIM8_Init(void);
static void MX_TIM15_Init(void);
static void MX_TIM16_Init(void);
static void MX_TIM17_Init(void);
static void MX_TIM20_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */
void SwitchRead();
void ProximityRead();
void ProximityWork(float x,float y,float omega);

void Motor1(float speed);
void Motor2(float speed);
void Motor3(float speed);
void Motor4(float speed);
void MotorBrush(int onoff);
void MotorBigBrush(int onoff);
void EncoderRead();

void motorMove();
void ForwardMove();
void BackwardMove();
void Slide();
void Turn();
void calculateWheelSpeeds(float v_x, float v_y, float omega, float* wheel_speeds);
void setMotorSpeeds(float* wheel_speeds);

uint64_t micros();
void QEIEncoderPosVel_Update();

void PIDmotor1(float target);
void PIDmotor2(float target);
void PIDmotor3(float target);
void PIDmotor4(float target);
float mapFloat(float x, float in_min, float in_max, float out_min, float out_max);

void nonBlockingDelay(uint16_t delay,uint8_t now_State,uint8_t next_State,uint8_t next_State2);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_DMA_Init();
  MX_LPUART1_UART_Init();
  MX_TIM3_Init();
  MX_ADC3_Init();
  MX_TIM4_Init();
  MX_TIM5_Init();
  MX_TIM8_Init();
  MX_TIM15_Init();
  MX_TIM16_Init();
  MX_TIM17_Init();
  MX_TIM20_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Encoder_Start(&htim3,TIM_CHANNEL_ALL);
  HAL_TIM_Encoder_Start(&htim4,TIM_CHANNEL_ALL);
  HAL_TIM_Encoder_Start(&htim8,TIM_CHANNEL_ALL);
  HAL_TIM_Encoder_Start(&htim20,TIM_CHANNEL_ALL);
  HAL_TIM_PWM_Start(&htim15, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim15, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim16, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim17, TIM_CHANNEL_1);

  HAL_TIM_Base_Start_IT(&htim2);

//  Poscontrol.kp = 1;
//  Poscontrol.ki = 0;
//  Poscontrol.kd = 0;
//  Poscontrol.T = 0.0001;

  Velocontrol.kp = 0.02;
  Velocontrol.ki = 0.061;
  Velocontrol.kd = 0;
  Velocontrol.T = 0.0001;

  Velocontrol2.kp = 0.02;
  Velocontrol2.ki = 0.061;
  Velocontrol2.kd = 0;
  Velocontrol2.T = 0.0001;

  Velocontrol3.kp = 0.02;
  Velocontrol3.ki = 0.061;
  Velocontrol3.kd = 0;
  Velocontrol3.T = 0.0001;

  Velocontrol4.kp = 0.02;
  Velocontrol4.ki = 0.061;
  Velocontrol4.kd = 0;
  Velocontrol4.T = 0.0001;



//  HAL_ADC_Start_DMA(&hadc3, ADC_RawRead, 40);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  SwitchRead();
	  ProximityRead();
	  EncoderRead();
	  calculateWheelSpeeds(v_x, v_y, Robot_omega, wheel_speeds);
	  static uint64_t timestamp =0;
	  int64_t currentTime = micros();
	  if(currentTime > timestamp)
	  {
	  timestamp =currentTime + 1000;//us
	  QEIEncoderPosVel_Update();
	  }
//	  PIDposition();
	  if(Switch[0] == 0 && Switch[1] == 1)
	  {
	  MotorBigBrush(1);
	  MotorBrush(1);
	  v_x = 0.2;
		v_y = 0.0;
		Robot_omega = 0.0;
		motorMove();
	  }
	  else if(Switch[0] == 1 && Switch[1] == 0)
	  {
//		PIDmotor1(testSpeed);
//		PIDmotor2(testSpeed);
//		PIDmotor3(testSpeed);
//		PIDmotor4(testSpeed);
		  switch (state_ALL)
		  {
		  	  case 0://FindStartPos
		  		  switch (state_Check)
		  		  {
		  		  	  case 0://Move Left
		  		  		  base_X = 0.0;
		  		  		  base_Y = baseSpeed;
		  		  		  base_Omega = 0.0;
		  		  		  if(Proximity[0] == sensorThick && Proximity[7] == sensorThick)
		  		  		  {
		  		  			delayCount = 0;
							state_Check = 1;
							state_ALL = 1;
							path = 1;
							n = 0;
							state_Clean = path1[n];
		  		  		  }
		  		  		  else
		  		  		  {
		  		  			nonBlockingDelay(10000, 0, 1, 0);
		  		  		  }

		  		  		  break;
		  		  	  case 1://Move Right
		  		  		  base_X = 0.0;
		  		  		  base_Y = -baseSpeed;
						  base_Omega = 0.0;
						  if(Proximity[3] == sensorThick && Proximity[4] == sensorThick)
						  {
							  delayCount = 0;
							  state_Check = 0;
							  state_ALL = 1;
							  path = 2;
							  n = 0;
							  state_Clean = path2[n];
						  }
						  else
						  {
							  nonBlockingDelay(10000, 0, 0, 0);

						  }
						  break;
		  		  }
		  		  break;
		  	  case 1://Clean
		  		  switch(state_Clean)
		  		  {
		  		  case 0://MoveForwardUntillSensorThick
		  			  base_X = baseSpeed;
		  			  base_Y = 0.0;
		  			  base_Omega = 0.0;
				  if((Proximity[1] == sensorThick) && (Proximity[2] == sensorThick))
				  {
					  n++;
					  if(path == 1)
					  {
					  state_Clean = path1[n];
					  }
					  if(path == 2)
					  {
					  state_Clean = path2[n];
					  }
				  }
		  			  break;
		  		  case 1://MoveBackwardUntillSensorThick
		  			  base_X = -baseSpeed;
					  base_Y = 0.0;
					  base_Omega = 0.0;
				  if((Proximity[5] == sensorThick) && (Proximity[6] == sensorThick))
					  {
						  n++;
						  if(path == 1)
						  {
						  state_Clean = path1[n];
						  }
						  if(path == 2)
						  {
						  state_Clean = path2[n];
						  }
					  }
		  			  break;
		  		  case 2://MoveLeftUntillSensorThick
					  base_X = 0.0;
					  base_Y = baseSpeed;
					  base_Omega = 0.0;
				  if(Proximity[0] == sensorThick && Proximity[7] == sensorThick)
					  {
						  n++;
						  if(path == 1)
						  {
						  state_Clean = path1[n];
						  }
						  if(path == 2)
						  {
						  state_Clean = path2[n];
						  }
					  }
					  break;
		  		  case 3://MoveRightUntillSensorThick
					  base_X = 0.0;
					  base_Y = -baseSpeed;
					  base_Omega = 0.0;
				  if(Proximity[3] == sensorThick && Proximity[4] == sensorThick)
					  {
						  n++;
						  if(path == 1)
						  {
						  state_Clean = path1[n];
						  }
						  if(path == 2)
						  {
						  state_Clean = path2[n];
						  }
					  }
					  break;
		  		  case 4://MoveLeftUntillDelay
					  base_X = 0.0;
					  base_Y = baseSpeed;
					  base_Omega = 0.0;
				  if(Proximity[0] == sensorThick && Proximity[7] == sensorThick)
					  {
						  n++;
						  if(path == 1)
						  {
						  state_Clean = path1[n];
						  }
						  if(path == 2)
						  {
						  state_Clean = path2[n];
						  }
					  }
				  else
				  {
					  nonBlockingDelay(1000, 1, n, 0);
				  }
					  break;
		  		  case 5://MoveRightUntillDelay
					  base_X = 0.0;
					  base_Y = -baseSpeed;
					  base_Omega = 0.0;
				  if(Proximity[3] == sensorThick && Proximity[4] == sensorThick)
					  {
						  n++;
						  if(path == 1)
						  {
						  state_Clean = path1[n];
						  }
						  if(path == 2)
						  {
						  state_Clean = path2[n];
						  }
					  }
				  else
				  {
					  nonBlockingDelay(1000, 1, n, 0);
				  }
					  break;
		  		  case 6://End
					  state_ALL = 0;
					  n = 0;
					  break;
		  		  }
		  		  break;
		  		case 2:
		  		base_X = 0;
		  		base_X = 0;
		  		base_Omega = 0;
				Motor1(0);
				Motor2(0);
				Motor3(0);
				Motor4(0);
				MotorBigBrush(0);
				MotorBrush(0);
				break;
		  }
		ProximityWork(base_X,base_Y,base_Omega);
		v_x = base_X + cali_X;
		v_y = base_Y + cali_Y;
		Robot_omega = base_Omega + cali_Omega;
		motorMove();
		MotorBigBrush(1);
		MotorBrush(1);
	  }
	  else
	  {
			Motor1(0);

			Motor2(0);

			Motor3(0);

			Motor4(0);
		  MotorBigBrush(0);
		  MotorBrush(0);
	  }

	  //		  PIDmotor1(wheel_speeds[0]);
	  //		  PIDmotor2(-wheel_speeds[1]);
	  //		  PIDmotor3(-wheel_speeds[3]);
	  //		  PIDmotor4(wheel_speeds[2]);
	  //		  PIDmotor1(baseSpeed);
	  //		  PIDmotor2(baseSpeed);
	  //		  PIDmotor3(baseSpeed);
	  //		  PIDmotor4(baseSpeed);
	  //		Motor1(baseSpeed);
	  //		Motor2(baseSpeed);
	  //		Motor3(baseSpeed);
	  //		Motor4(baseSpeed);
//	  QEIReadRaw = __HAL_TIM_GET_COUNTER(&htim3);

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

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV4;
  RCC_OscInitStruct.PLL.PLLN = 85;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC3_Init(void)
{

  /* USER CODE BEGIN ADC3_Init 0 */

  /* USER CODE END ADC3_Init 0 */

  ADC_MultiModeTypeDef multimode = {0};
  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC3_Init 1 */

  /* USER CODE END ADC3_Init 1 */

  /** Common config
  */
  hadc3.Instance = ADC3;
  hadc3.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc3.Init.Resolution = ADC_RESOLUTION_12B;
  hadc3.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc3.Init.GainCompensation = 0;
  hadc3.Init.ScanConvMode = ADC_SCAN_ENABLE;
  hadc3.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc3.Init.LowPowerAutoWait = DISABLE;
  hadc3.Init.ContinuousConvMode = ENABLE;
  hadc3.Init.NbrOfConversion = 2;
  hadc3.Init.DiscontinuousConvMode = DISABLE;
  hadc3.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc3.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc3.Init.DMAContinuousRequests = ENABLE;
  hadc3.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc3.Init.OversamplingMode = DISABLE;
  if (HAL_ADC_Init(&hadc3) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure the ADC multi-mode
  */
  multimode.Mode = ADC_MODE_INDEPENDENT;
  if (HAL_ADCEx_MultiModeConfigChannel(&hadc3, &multimode) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_2CYCLES_5;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_5;
  sConfig.Rank = ADC_REGULAR_RANK_2;
  if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC3_Init 2 */

  /* USER CODE END ADC3_Init 2 */

}

/**
  * @brief LPUART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_LPUART1_UART_Init(void)
{

  /* USER CODE BEGIN LPUART1_Init 0 */

  /* USER CODE END LPUART1_Init 0 */

  /* USER CODE BEGIN LPUART1_Init 1 */

  /* USER CODE END LPUART1_Init 1 */
  hlpuart1.Instance = LPUART1;
  hlpuart1.Init.BaudRate = 115200;
  hlpuart1.Init.WordLength = UART_WORDLENGTH_8B;
  hlpuart1.Init.StopBits = UART_STOPBITS_1;
  hlpuart1.Init.Parity = UART_PARITY_NONE;
  hlpuart1.Init.Mode = UART_MODE_TX_RX;
  hlpuart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  hlpuart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  hlpuart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  hlpuart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&hlpuart1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&hlpuart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&hlpuart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&hlpuart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN LPUART1_Init 2 */

  /* USER CODE END LPUART1_Init 2 */

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

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 169;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 4294967295;
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
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_Encoder_InitTypeDef sConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 63359;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 1;
  sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 1;
  if (HAL_TIM_Encoder_Init(&htim3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

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

  TIM_Encoder_InitTypeDef sConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 0;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 63359;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 1;
  sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 1;
  if (HAL_TIM_Encoder_Init(&htim4, &sConfig) != HAL_OK)
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
  * @brief TIM5 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM5_Init(void)
{

  /* USER CODE BEGIN TIM5_Init 0 */

  /* USER CODE END TIM5_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM5_Init 1 */

  /* USER CODE END TIM5_Init 1 */
  htim5.Instance = TIM5;
  htim5.Init.Prescaler = 169;
  htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim5.Init.Period = 4294967295;
  htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim5.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim5) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim5, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim5, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM5_Init 2 */

  /* USER CODE END TIM5_Init 2 */

}

/**
  * @brief TIM8 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM8_Init(void)
{

  /* USER CODE BEGIN TIM8_Init 0 */

  /* USER CODE END TIM8_Init 0 */

  TIM_Encoder_InitTypeDef sConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM8_Init 1 */

  /* USER CODE END TIM8_Init 1 */
  htim8.Instance = TIM8;
  htim8.Init.Prescaler = 0;
  htim8.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim8.Init.Period = 63359;
  htim8.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim8.Init.RepetitionCounter = 0;
  htim8.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 1;
  sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 1;
  if (HAL_TIM_Encoder_Init(&htim8, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim8, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM8_Init 2 */

  /* USER CODE END TIM8_Init 2 */

}

/**
  * @brief TIM15 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM15_Init(void)
{

  /* USER CODE BEGIN TIM15_Init 0 */

  /* USER CODE END TIM15_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM15_Init 1 */

  /* USER CODE END TIM15_Init 1 */
  htim15.Instance = TIM15;
  htim15.Init.Prescaler = 169;
  htim15.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim15.Init.Period = 999;
  htim15.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim15.Init.RepetitionCounter = 0;
  htim15.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim15) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim15, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim15) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim15, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim15, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim15, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.BreakFilter = 0;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim15, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM15_Init 2 */

  /* USER CODE END TIM15_Init 2 */
  HAL_TIM_MspPostInit(&htim15);

}

/**
  * @brief TIM16 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM16_Init(void)
{

  /* USER CODE BEGIN TIM16_Init 0 */

  /* USER CODE END TIM16_Init 0 */

  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM16_Init 1 */

  /* USER CODE END TIM16_Init 1 */
  htim16.Instance = TIM16;
  htim16.Init.Prescaler = 169;
  htim16.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim16.Init.Period = 999;
  htim16.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim16.Init.RepetitionCounter = 0;
  htim16.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim16) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim16) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim16, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.BreakFilter = 0;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim16, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM16_Init 2 */

  /* USER CODE END TIM16_Init 2 */
  HAL_TIM_MspPostInit(&htim16);

}

/**
  * @brief TIM17 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM17_Init(void)
{

  /* USER CODE BEGIN TIM17_Init 0 */

  /* USER CODE END TIM17_Init 0 */

  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM17_Init 1 */

  /* USER CODE END TIM17_Init 1 */
  htim17.Instance = TIM17;
  htim17.Init.Prescaler = 169;
  htim17.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim17.Init.Period = 999;
  htim17.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim17.Init.RepetitionCounter = 0;
  htim17.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim17) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim17) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim17, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.BreakFilter = 0;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim17, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM17_Init 2 */

  /* USER CODE END TIM17_Init 2 */
  HAL_TIM_MspPostInit(&htim17);

}

/**
  * @brief TIM20 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM20_Init(void)
{

  /* USER CODE BEGIN TIM20_Init 0 */

  /* USER CODE END TIM20_Init 0 */

  TIM_Encoder_InitTypeDef sConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM20_Init 1 */

  /* USER CODE END TIM20_Init 1 */
  htim20.Instance = TIM20;
  htim20.Init.Prescaler = 0;
  htim20.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim20.Init.Period = 63359;
  htim20.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim20.Init.RepetitionCounter = 0;
  htim20.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 2;
  sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 2;
  if (HAL_TIM_Encoder_Init(&htim20, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim20, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM20_Init 2 */

  /* USER CODE END TIM20_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMAMUX1_CLK_ENABLE();
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LD2_Pin|GPIO_O7M2_Pin|GPIO_O6M1_Pin|GPIO_O8M2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_O11_Pin|GPIO_O5M1_Pin|GPIO_O3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_O10MBBB_Pin|GPIO_O4M4B_Pin|GPIO_O1M4_Pin|GPIO_O9M3_Pin
                          |GPIO_O2M4_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIO_O12M3_GPIO_Port, GPIO_O12M3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : GPIOProx10_Pin GPIOProx9_Pin GPIOProx1_Pin GPIO_InPUT12_Pin */
  GPIO_InitStruct.Pin = GPIOProx10_Pin|GPIOProx9_Pin|GPIOProx1_Pin|GPIO_InPUT12_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : GPIOProx5_Pin GPIOProx6_Pin GPIOProx7_Pin GPIOProx3_Pin */
  GPIO_InitStruct.Pin = GPIOProx5_Pin|GPIOProx6_Pin|GPIOProx7_Pin|GPIOProx3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : LD2_Pin GPIO_O7M2_Pin GPIO_O6M1_Pin GPIO_O8M2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin|GPIO_O7M2_Pin|GPIO_O6M1_Pin|GPIO_O8M2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : SWin1_Pin */
  GPIO_InitStruct.Pin = SWin1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(SWin1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : GPIOProx8_Pin GPIO_INPUT10_Pin */
  GPIO_InitStruct.Pin = GPIOProx8_Pin|GPIO_INPUT10_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : SWin2_Pin */
  GPIO_InitStruct.Pin = SWin2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(SWin2_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : GPIO_O11_Pin GPIO_O5M1_Pin GPIO_O3_Pin */
  GPIO_InitStruct.Pin = GPIO_O11_Pin|GPIO_O5M1_Pin|GPIO_O3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : GPIO_O10MBBB_Pin GPIO_O4M4B_Pin GPIO_O1M4_Pin GPIO_O9M3_Pin
                           GPIO_O2M4_Pin */
  GPIO_InitStruct.Pin = GPIO_O10MBBB_Pin|GPIO_O4M4B_Pin|GPIO_O1M4_Pin|GPIO_O9M3_Pin
                          |GPIO_O2M4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : GPIO_O12M3_Pin */
  GPIO_InitStruct.Pin = GPIO_O12M3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIO_O12M3_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void SwitchRead()
{
	Switch[0] = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_5);
	Switch[1] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_11);
}
void ProximityRead()
{
	Proximity[0] =  HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_1);
	Proximity[1] = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_3);
	Proximity[2] = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_15);
	Proximity[3] = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_0);
	Proximity[4] =HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0);
	Proximity[5] = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4);
	Proximity[6] = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0);
	Proximity[7] = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1);
}
void Motor1(float speed){
	if(speed > 0)
	{
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, 0);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, 1);
		__HAL_TIM_SET_COMPARE(&htim15, TIM_CHANNEL_2, fabs(speed));
	}
	else
	{
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, 1);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, 0);
		__HAL_TIM_SET_COMPARE(&htim15, TIM_CHANNEL_2, fabs(speed));

	}
}
void Motor2(float speed){
	if(speed > 0)
	{
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, 0);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, 1);
		__HAL_TIM_SET_COMPARE(&htim15, TIM_CHANNEL_1, fabs(speed));
	}
	else
	{
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, 1);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, 0);
		__HAL_TIM_SET_COMPARE(&htim15, TIM_CHANNEL_1, fabs(speed));

	}
}
void Motor3(float speed){
	if(speed > 0)
	{
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, 1);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, 0);
		__HAL_TIM_SET_COMPARE(&htim16, TIM_CHANNEL_1, fabs(speed));
	}
	else
	{
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, 0);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, 1);
		__HAL_TIM_SET_COMPARE(&htim16, TIM_CHANNEL_1, fabs(speed));

	}
}
void Motor4(float speed){
	if(speed > 0)
	{
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, 0);
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, 1);
		__HAL_TIM_SET_COMPARE(&htim17, TIM_CHANNEL_1, fabs(speed));
	}
	else
	{
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, 1);
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, 0);
		__HAL_TIM_SET_COMPARE(&htim17, TIM_CHANNEL_1, fabs(speed));

	}
}
void motorMove()
{
	PIDmotor1(wheel_speeds[0]);
	PIDmotor2(-wheel_speeds[1]);
	PIDmotor3(-wheel_speeds[3]);
	PIDmotor4(wheel_speeds[2]);
}
void MotorBrush(int onoff)
{
	if(onoff == 1)
	{
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, 1);
	}
	else
	{
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, 0);
	}
}
void MotorBigBrush(int onoff)
{
	if(onoff == 1)
	{
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, 1);
	}
	else
	{
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, 0);
	}
}
void EncoderRead()
{
	EncodeRead[0] = __HAL_TIM_GET_COUNTER(&htim4);
	EncodeRead[1] = __HAL_TIM_GET_COUNTER(&htim3);
	EncodeRead[2] = __HAL_TIM_GET_COUNTER(&htim8);
	EncodeRead[3] = __HAL_TIM_GET_COUNTER(&htim20);
}

void calculateWheelSpeeds(float v_x, float v_y, float omega, float* wheel_speeds) {
    wheel_speeds[0] = ((180.0f/3.141592) / WHEEL_RADIUS) * (v_x - v_y - (ROBOT_LENGTH + ROBOT_WIDTH) * omega);
    wheel_speeds[1] = ((180.0f/3.141592) / WHEEL_RADIUS) * (v_x + v_y + (ROBOT_LENGTH + ROBOT_WIDTH) * omega);
    wheel_speeds[2] = ((180.0f/3.141592) / WHEEL_RADIUS) * (v_x + v_y - (ROBOT_LENGTH + ROBOT_WIDTH) * omega);
    wheel_speeds[3] = ((180.0f/3.141592) / WHEEL_RADIUS) * (v_x - v_y + (ROBOT_LENGTH + ROBOT_WIDTH) * omega);
}

void setMotorSpeeds(float* wheel_speeds) {
//    setMotorSpeed(1, wheel_speeds[0]);
//    setMotorSpeed(2, wheel_speeds[1]);
//    setMotorSpeed(3, wheel_speeds[2]);
//    setMotorSpeed(4, wheel_speeds[3]);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
if(htim == &htim2)
{
_micros += UINT32_MAX;
}
}
uint64_t micros()
{
return __HAL_TIM_GET_COUNTER(&htim2)+_micros;
}

void QEIEncoderPosVel_Update()
{
//collect data
QEIdata.TimeStamp[NEW] = micros();
QEIdata.Position1[NEW] = EncodeRead[0];
QEIdata.Position2[NEW] = EncodeRead[1];
QEIdata.Position3[NEW] = EncodeRead[2];
QEIdata.Position4[NEW] = EncodeRead[3];
//Postion 1 turn calculation
QEIdata.QEIPostion_1turn = QEIdata.Position1[NEW] % 3960;
//calculate dx
int32_t diffPosition1 = QEIdata.Position1[NEW] - QEIdata.Position1[OLD];
int32_t diffPosition2 = QEIdata.Position2[NEW] - QEIdata.Position2[OLD];
int32_t diffPosition3 = QEIdata.Position3[NEW] - QEIdata.Position3[OLD];
int32_t diffPosition4 = QEIdata.Position4[NEW] - QEIdata.Position4[OLD];
//Handle Warp around
if(diffPosition1 > 32256)
diffPosition1 -= 63360;
if(diffPosition1 < -32256)
diffPosition1 += 63360;
if(diffPosition2 > 32256)
diffPosition2 -= 63360;
if(diffPosition2 < -32256)
diffPosition2 += 63360;
if(diffPosition3 > 32256)
diffPosition3 -= 63360;
if(diffPosition3 < -32256)
diffPosition3 += 63360;
if(diffPosition4 > 32256)
diffPosition4 -= 63360;
if(diffPosition4 < -32256)
diffPosition4 += 63360;
//calculate dt
float diffTime = (QEIdata.TimeStamp[NEW]-QEIdata.TimeStamp[OLD]) * 0.000001;
//calculate anglar velocity
QEIdata.QEIAngularVelocity1 = diffPosition1 / diffTime;
QEIdata.QEIAngularVelocity2 = diffPosition2 / diffTime;
QEIdata.QEIAngularVelocity3 = diffPosition3 / diffTime;
QEIdata.QEIAngularVelocity4 = diffPosition4 / diffTime;

angular[0] = ((QEIdata.Position1[NEW]*360)/(4*990));
angular[1] = ((QEIdata.Position2[NEW]*360)/(4*990));
angular[2] = ((QEIdata.Position3[NEW]*360)/(4*990));
angular[3] = ((QEIdata.Position4[NEW]*360)/(4*990));

omega[0] = ((QEIdata.QEIAngularVelocity1*360)/(4*990));
omega[1] = ((QEIdata.QEIAngularVelocity2*360)/(4*990));
omega[2] = ((QEIdata.QEIAngularVelocity3*360)/(4*990));
omega[3] = ((QEIdata.QEIAngularVelocity4*360)/(4*990));

omega_Filter[0] = (0.969*omega_Filter_Old[0])+(0.0155*omega[0])+(0.0155*omega_Old[0]);
omega_Old[0] = omega[0];
omega_Filter_Old[0] = omega_Filter[0];

omega_Filter[1] = (0.969*omega_Filter_Old[1])+(0.0155*omega[1])+(0.0155*omega_Old[1]);
omega_Old[1] = omega[1];
omega_Filter_Old[1] = omega_Filter[1];

omega_Filter[2] = (0.969*omega_Filter_Old[2])+(0.0155*omega[2])+(0.0155*omega_Old[2]);
omega_Old[2] = omega[2];
omega_Filter_Old[2] = omega_Filter[2];

omega_Filter[3] = (0.969*omega_Filter_Old[3])+(0.0155*omega[3])+(0.0155*omega_Old[3]);
omega_Old[3] = omega[3];
omega_Filter_Old[3] = omega_Filter[3];
//speed_fill = (0.969*speed_fill_1)+(0.0155*speed)+(0.0155*speed_1);
//speed_1 = speed;
//speed_fill_1 = speed_fill;
//store value for next loop
QEIdata.Position1[OLD] = QEIdata.Position1[NEW];
QEIdata.Position2[OLD] = QEIdata.Position2[NEW];
QEIdata.Position3[OLD] = QEIdata.Position3[NEW];
QEIdata.Position4[OLD] = QEIdata.Position4[NEW];

QEIdata.TimeStamp[OLD]=QEIdata.TimeStamp[NEW];
}

//void PIDposition()
//{
//	static uint64_t timestamp3 =0;
//	int64_t currentTime = micros();
//	if(currentTime > timestamp3)
//	  {
////			Poscontrol.Error[NEW] = targetPos - angular[0];//
//////			Poscontrol.Output[NEW] = ((((Poscontrol.kp*Poscontrol.T)+(Poscontrol.ki*Poscontrol.T*Poscontrol.T)+(Poscontrol.kd))*Poscontrol.Error[NEW])-(((Poscontrol.kp*Poscontrol.T)+(2*Poscontrol.kd))*Poscontrol.Error[OLD])+(Poscontrol.kd*Poscontrol.Error[OLDER])+(Poscontrol.Output[OLD]*Poscontrol.T))/Poscontrol.T;
////			Poscontrol.Output[NEW] = ((((Poscontrol.kp*2*Poscontrol.T)+(Poscontrol.ki*Poscontrol.T*Poscontrol.T)+(2*Poscontrol.kd))*Poscontrol.Error[NEW])-(((Poscontrol.kp*2*Poscontrol.T)-(Poscontrol.ki*Poscontrol.T*Poscontrol.T)+(4*Poscontrol.kd))*Poscontrol.Error[OLD])+(2*Poscontrol.kd*Poscontrol.Error[OLDER])+(Poscontrol.Output[OLD]*2*Poscontrol.T))/(2*Poscontrol.T);
////			Poscontrol.Error[OLDER] = Poscontrol.Error[OLD];
////			Poscontrol.Error[OLD] = Poscontrol.Error[NEW];
////			Poscontrol.Output[OLDER] = Poscontrol.Output[OLD];
////			Poscontrol.Output[OLD] = Poscontrol.Output[NEW];
//			Velocontrol.omega[NEW] = omega[0];
//			Velocontrol.Error[NEW] = targetSpeed - Velocontrol.omega[NEW];//-speed_fill
////			ONE = (((Velocontrol.kp*2*Velocontrol.T)+(Velocontrol.ki*Velocontrol.T*Velocontrol.T)+(2*Velocontrol.kd))*Velocontrol.Error[NEW])/(2*Velocontrol.T);
////			TWO = (((Velocontrol.kp*2*Velocontrol.T)-(Velocontrol.ki*Velocontrol.T*Velocontrol.T)+(4*Velocontrol.kd))*Velocontrol.Error[OLD])/(2*Velocontrol.T);
////			THREE = (2*Velocontrol.kd*Velocontrol.Error[OLDER])/(2*Velocontrol.T);
////			FOUR = (Velocontrol.Output[OLD]*2*Velocontrol.T)/(2*Velocontrol.T);
////			Velocontrol.Output[NEW] = (ONE-TWO+THREE+FOUR);//
////			Velocontrol.Output[NEW] = ((((Velocontrol.kp*2*Velocontrol.T)+(Velocontrol.ki*Velocontrol.T*Velocontrol.T)+(2*Velocontrol.kd))*Velocontrol.Error[NEW])-(((Velocontrol.kp*2*Velocontrol.T)-(Velocontrol.ki*Velocontrol.T*Velocontrol.T)+(4*Velocontrol.kd))*Velocontrol.Error[OLD])+(2*Velocontrol.kd*Velocontrol.Error[OLDER])+(Velocontrol.Output[OLD]*2*Velocontrol.T))/(2*Velocontrol.T);
//			Velocontrol.Output[NEW] = Velocontrol.Output[OLD] + (Velocontrol.kp*(Velocontrol.Error[NEW]-Velocontrol.Error[OLD]))+(Velocontrol.ki*(Velocontrol.Error[NEW]*Velocontrol.T))-(Velocontrol.kd*((Velocontrol.omega[NEW]-(2*Velocontrol.omega[OLD])+(Velocontrol.omega[OLDER]))/Velocontrol.T));
//			if(Velocontrol.Output[NEW]>12)Velocontrol.Output[NEW]=12;
//			if(Velocontrol.Output[NEW]<-12)Velocontrol.Output[NEW]=-12;
//			Velocontrol.omega[OLDER] = Velocontrol.omega[OLD];
//			Velocontrol.omega[OLD]  = Velocontrol.omega[NEW];
//			Velocontrol.Error[OLDER] = Velocontrol.Error[OLD];
//			Velocontrol.Error[OLD] = Velocontrol.Error[NEW];
//			Velocontrol.Output[OLD] = Velocontrol.Output[NEW];
//
////			Velocontrol.Error[NEW] = Poscontrol.Output[NEW]+q_Velo-speed_fill;//-speed_fill
////			Velocontrol.Output[NEW] = ((((Velocontrol.kp*Velocontrol.T)+(Velocontrol.ki*Velocontrol.T*Velocontrol.T)+(Velocontrol.kd))*Velocontrol.Error[NEW])-(((Velocontrol.kp*Velocontrol.T)+(2*Velocontrol.kd))*Velocontrol.Error[OLD])+(Velocontrol.kd*Velocontrol.Error[OLDER])+(Velocontrol.Output[OLD]*Velocontrol.T))/Velocontrol.T;//
////			if(Velocontrol.Output[NEW]>42500)Velocontrol.Output[NEW]=42500;
////			if(Velocontrol.Output[NEW]<-42500)Velocontrol.Output[NEW]=-42500;
////			Velocontrol.Error[OLDER] = Velocontrol.Error[OLD];
////			Velocontrol.Error[OLD] = Velocontrol.Error[NEW];
////			Velocontrol.Output[OLDER] = Velocontrol.Output[OLD];
////			Velocontrol.Output[OLD] = Velocontrol.Output[NEW];
//	//					if(fabs(Pos_Target-Pos) <= 0.1)start = 0;
//			timestamp3 =currentTime + 100;
//	  }
//		PWMmap = mapFloat(Velocontrol.Output[NEW],-12.0,12.0,-999,999);
//		Motor1(PWMmap);
//}
void PIDmotor1(float target)
{
	static uint64_t timestamp3 =0;
	int64_t currentTime = micros();
	if(currentTime > timestamp3)
	  {
			Velocontrol.omega[NEW] = omega_Filter[0];
			Velocontrol.Error[NEW] = target - Velocontrol.omega[NEW];
			Velocontrol.Output[NEW] = Velocontrol.Output[OLD] + (Velocontrol.kp*(Velocontrol.Error[NEW]-Velocontrol.Error[OLD]))+(Velocontrol.ki*(Velocontrol.Error[NEW]*Velocontrol.T))-(Velocontrol.kd*((Velocontrol.omega[NEW]-(2*Velocontrol.omega[OLD])+(Velocontrol.omega[OLDER]))/Velocontrol.T));
			if(Velocontrol.Output[NEW]>12)Velocontrol.Output[NEW]=12;
			if(Velocontrol.Output[NEW]<-12)Velocontrol.Output[NEW]=-12;
			Velocontrol.omega[OLDER] = Velocontrol.omega[OLD];
			Velocontrol.omega[OLD]  = Velocontrol.omega[NEW];
			Velocontrol.Error[OLDER] = Velocontrol.Error[OLD];
			Velocontrol.Error[OLD] = Velocontrol.Error[NEW];
			Velocontrol.Output[OLD] = Velocontrol.Output[NEW];
			timestamp3 =currentTime + 100;
	  }
		PWMmap[0] = mapFloat(Velocontrol.Output[NEW],-12.0,12.0,-999,999);
		Motor1(PWMmap[0]);
}
void PIDmotor2(float target)
{
	static uint64_t timestamp4 =0;
	int64_t currentTime = micros();
	if(currentTime > timestamp4)
	  {
			Velocontrol2.omega[NEW] = omega_Filter[1];
			Velocontrol2.Error[NEW] = target - Velocontrol2.omega[NEW];
			Velocontrol2.Output[NEW] = Velocontrol2.Output[OLD] + (Velocontrol2.kp*(Velocontrol2.Error[NEW]-Velocontrol2.Error[OLD]))+(Velocontrol2.ki*(Velocontrol2.Error[NEW]*Velocontrol2.T))-(Velocontrol2.kd*((Velocontrol2.omega[NEW]-(2*Velocontrol2.omega[OLD])+(Velocontrol2.omega[OLDER]))/Velocontrol2.T));
			if(Velocontrol2.Output[NEW]>12)Velocontrol2.Output[NEW]=12;
			if(Velocontrol2.Output[NEW]<-12)Velocontrol2.Output[NEW]=-12;
			Velocontrol2.omega[OLDER] = Velocontrol2.omega[OLD];
			Velocontrol2.omega[OLD]  = Velocontrol2.omega[NEW];
			Velocontrol2.Error[OLDER] = Velocontrol2.Error[OLD];
			Velocontrol2.Error[OLD] = Velocontrol2.Error[NEW];
			Velocontrol2.Output[OLD] = Velocontrol2.Output[NEW];
			timestamp4 =currentTime + 100;
	  }
		PWMmap[1] = mapFloat(Velocontrol2.Output[NEW],-12.0,12.0,-999,999);
		Motor2(PWMmap[1]);
}
void PIDmotor3(float target)
{
	static uint64_t timestamp5 =0;
	int64_t currentTime = micros();
	if(currentTime > timestamp5)
	  {
			Velocontrol3.omega[NEW] = omega_Filter[2];
			Velocontrol3.Error[NEW] = target - Velocontrol3.omega[NEW];
			Velocontrol3.Output[NEW] = Velocontrol3.Output[OLD] + (Velocontrol3.kp*(Velocontrol3.Error[NEW]-Velocontrol3.Error[OLD]))+(Velocontrol3.ki*(Velocontrol3.Error[NEW]*Velocontrol3.T))-(Velocontrol3.kd*((Velocontrol3.omega[NEW]-(2*Velocontrol3.omega[OLD])+(Velocontrol3.omega[OLDER]))/Velocontrol3.T));
			if(Velocontrol3.Output[NEW]>12)Velocontrol3.Output[NEW]=12;
			if(Velocontrol3.Output[NEW]<-12)Velocontrol3.Output[NEW]=-12;
			Velocontrol3.omega[OLDER] = Velocontrol3.omega[OLD];
			Velocontrol3.omega[OLD]  = Velocontrol3.omega[NEW];
			Velocontrol3.Error[OLDER] = Velocontrol3.Error[OLD];
			Velocontrol3.Error[OLD] = Velocontrol3.Error[NEW];
			Velocontrol3.Output[OLD] = Velocontrol3.Output[NEW];
			timestamp5 =currentTime + 100;
	  }
		PWMmap[2] = mapFloat(Velocontrol3.Output[NEW],-12.0,12.0,-999,999);
		Motor3(PWMmap[2]);
}
void PIDmotor4(float target)
{
	static uint64_t timestamp6 =0;
	int64_t currentTime = micros();
	if(currentTime > timestamp6)
	  {
			Velocontrol4.omega[NEW] = omega_Filter[3];
			Velocontrol4.Error[NEW] = target - Velocontrol4.omega[NEW];
			Velocontrol4.Output[NEW] = Velocontrol4.Output[OLD] + (Velocontrol4.kp*(Velocontrol4.Error[NEW]-Velocontrol4.Error[OLD]))+(Velocontrol4.ki*(Velocontrol4.Error[NEW]*Velocontrol4.T))-(Velocontrol4.kd*((Velocontrol4.omega[NEW]-(2*Velocontrol4.omega[OLD])+(Velocontrol4.omega[OLDER]))/Velocontrol4.T));
			if(Velocontrol4.Output[NEW]>12)Velocontrol4.Output[NEW]=12;
			if(Velocontrol4.Output[NEW]<-12)Velocontrol4.Output[NEW]=-12;
			Velocontrol4.omega[OLDER] = Velocontrol4.omega[OLD];
			Velocontrol4.omega[OLD]  = Velocontrol4.omega[NEW];
			Velocontrol4.Error[OLDER] = Velocontrol4.Error[OLD];
			Velocontrol4.Error[OLD] = Velocontrol4.Error[NEW];
			Velocontrol4.Output[OLD] = Velocontrol4.Output[NEW];
			timestamp6 =currentTime + 100;
	  }
		PWMmap[3] = mapFloat(Velocontrol4.Output[NEW],-12.0,12.0,-999,999);
		Motor4(PWMmap[3]);
}
float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
void ProximityWork(float x,float y,float omega)
{
	if(x > 0)
	{
		if(Proximity[0] == sensorThick && Proximity[7] == sensorThick)
		{
			cali_X = 0.0;
			cali_Y = -x;
			cali_Omega = 0.0;
		}
		else if(Proximity[3] == sensorThick && Proximity[4] == sensorThick)
		{
			cali_X = 0.0;
			cali_Y = x;
			cali_Omega = 0.0;
		}
		else if(Proximity[0] == sensorThick)
		{
			cali_X = 0.0;
			cali_Y = 0.0;
			cali_Omega = -x/2;
		}
		else if(Proximity[3] == sensorThick)
		{
			cali_X = 0.0;
			cali_Y = 0.0;
			cali_Omega = x/2;
		}
		else if(Proximity[4] == sensorThick)
		{
			cali_X = 0.0;
			cali_Y = 0.0;
			cali_Omega = -x/2;
		}
		else if(Proximity[7] == sensorThick)
		{
			cali_X = 0.0;
			cali_Y = 0.0;
			cali_Omega = x/2;
		}
		else if(Proximity[1] == sensorThick)
		{
			cali_X = -x;
			cali_Y = 0.0;
			cali_Omega = x;
		}
		else if(Proximity[2] == sensorThick)
		{
			cali_X = -x;
			cali_Y = 0.0;
			cali_Omega = -x;
		}
		else
		{
			cali_X = 0.0;
			cali_Y = 0.0;
			cali_Omega = 0.0;
		}
	}
	else if(x < 0)
	{
		if(Proximity[0] == sensorThick && Proximity[7] == sensorThick)
		{
			cali_X = 0.0;
			cali_Y = x;
			cali_Omega = 0.0;
		}
		else if(Proximity[3] == sensorThick && Proximity[4] == sensorThick)
		{
			cali_X = 0.0;
			cali_Y = -x;
			cali_Omega = 0.0;
		}
		else if(Proximity[0] == sensorThick)
		{
			cali_X = 0.0;
			cali_Y = 0.0;
			cali_Omega = x/2;
		}
		else if(Proximity[3] == sensorThick)
		{
			cali_X = 0.0;
			cali_Y = 0.0;
			cali_Omega = -x/2;
		}
		else if(Proximity[4] == sensorThick)
		{
			cali_X = 0.0;
			cali_Y = 0.0;
			cali_Omega = x/2;
		}
		else if(Proximity[7] == sensorThick)
		{
			cali_X = 0.0;
			cali_Y = 0.0;
			cali_Omega = -x/2;
		}
		else if(Proximity[5] == sensorThick)
		{
			cali_X = -x;
			cali_Y = 0.0;
			cali_Omega = -x;
		}
		else if(Proximity[6] == sensorThick)
		{
			cali_X = -x;
			cali_Y = 0.0;
			cali_Omega = x;
		}
		else
		{
			cali_X = 0.0;
			cali_Y = 0.0;
			cali_Omega = 0.0;
		}
	}
	else if(y > 0)
	{
		if(Proximity[1] == sensorThick && Proximity[2] == sensorThick)
		{
			cali_X = -y;
			cali_Y = 0.0;
			cali_Omega = 0.0;
		}
		else if(Proximity[6] == sensorThick && Proximity[5] == sensorThick)
		{
			cali_X = y;
			cali_Y = 0.0;
			cali_Omega = 0.0;
		}
		else if(Proximity[1] == sensorThick)
		{
			cali_X = 0.0;
			cali_Y = 0.0;
			cali_Omega = y/2;
		}
		else if(Proximity[6] == sensorThick)
		{
			cali_X = 0.0;
			cali_Y = 0.0;
			cali_Omega = -y/2;
		}
		else if(Proximity[2] == sensorThick)
		{
			cali_X = 0.0;
			cali_Y = 0.0;
			cali_Omega = -y/2;
		}
		else if(Proximity[5] == sensorThick)
		{
			cali_X = 0.0;
			cali_Y = 0.0;
			cali_Omega = y/2;
		}
		else if(Proximity[0] == sensorThick)
		{
			cali_X = 0.0;
			cali_Y = -y;
			cali_Omega = -y;
		}
		else if(Proximity[7] == sensorThick)
		{
			cali_X = 0.0;
			cali_Y = -y;
			cali_Omega = y;
		}
		else
		{
			cali_X = 0.0;
			cali_Y = 0.0;
			cali_Omega = 0.0;
		}
	}
	else if(y < 0)
	{
		if(Proximity[1] == sensorThick && Proximity[2] == sensorThick)
		{
			cali_X = y;
			cali_Y = 0.0;
			cali_Omega = 0.0;
		}
		else if(Proximity[6] == sensorThick && Proximity[5] == sensorThick)
		{
			cali_X = -y;
			cali_Y = 0.0;
			cali_Omega = 0.0;
		}
		else if(Proximity[1] == sensorThick)
		{
			cali_X = 0.0;
			cali_Y = 0.0;
			cali_Omega = -y/2;
		}
		else if(Proximity[6] == sensorThick)
		{
			cali_X = 0.0;
			cali_Y = 0.0;
			cali_Omega = y/2;
		}
		else if(Proximity[2] == sensorThick)
		{
			cali_X = 0.0;
			cali_Y = 0.0;
			cali_Omega = y/2;
		}
		else if(Proximity[5] == sensorThick)
		{
			cali_X = 0.0;
			cali_Y = 0.0;
			cali_Omega = -y/2;
		}
		else if(Proximity[4] == sensorThick)
		{
			cali_X = 0.0;
			cali_Y = -y;
			cali_Omega = y;
		}
		else if(Proximity[3] == sensorThick)
		{
			cali_X = 0.0;
			cali_Y = -y;
			cali_Omega = -y;
		}
		else
		{
			cali_X = 0.0;
			cali_Y = 0.0;
			cali_Omega = 0.0;
		}
	}
	else
	{
		cali_X = 0;
		cali_Y = 0;
		cali_Omega = 0;
	}
}
void nonBlockingDelay(uint16_t delay,uint8_t now_State,uint8_t next_State,uint8_t next_State2)
{
	static uint64_t timecheck = 0;
	if(timecheck < HAL_GetTick())
	  {
		delayCount++;
		timecheck = HAL_GetTick()+100;
	  }
	if (delayCount > (delay/100))
	  {
		delayCount = 0;
		if(now_State == 0)
		{
			state_Check = next_State;
		}
		if(now_State == 1)
		{
			if(path == 1)
			{
			state_Clean = path1[next_State+1];
			}
			if(path == 2)
			{
			state_Clean = path2[next_State+1];
			}
		}
	  }
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
