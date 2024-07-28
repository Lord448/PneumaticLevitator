/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd.h"
#include "ugui.h"
#include "UI.h"
#include "COM.h"
#include "GPUResMan.h"
#include "DiagAppl.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef StaticTask_t osStaticThreadDef_t;
typedef StaticSemaphore_t osStaticSemaphoreDef_t;
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;
DMA_HandleTypeDef hdma_spi1_tx;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim11;

UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_tx;

DMA_HandleTypeDef hdma_memtomem_dma2_stream3;
DMA_HandleTypeDef hdma_memtomem_dma2_stream4;
DMA_HandleTypeDef hdma_memtomem_dma2_stream5;
DMA_HandleTypeDef hdma_memtomem_dma2_stream0;
/* Definitions for TaskIdle */
osThreadId_t TaskIdleHandle;
uint32_t TaskIdleBuffer[ 128 ];
osStaticThreadDef_t TaskIdleControlBlock;
const osThreadAttr_t TaskIdle_attributes = {
  .name = "TaskIdle",
  .cb_mem = &TaskIdleControlBlock,
  .cb_size = sizeof(TaskIdleControlBlock),
  .stack_mem = &TaskIdleBuffer[0],
  .stack_size = sizeof(TaskIdleBuffer),
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for TaskUI */
osThreadId_t TaskUIHandle;
uint32_t TaskUIBuffer[ 10240 ];
osStaticThreadDef_t TaskUIControlBlock;
const osThreadAttr_t TaskUI_attributes = {
  .name = "TaskUI",
  .cb_mem = &TaskUIControlBlock,
  .cb_size = sizeof(TaskUIControlBlock),
  .stack_mem = &TaskUIBuffer[0],
  .stack_size = sizeof(TaskUIBuffer),
  .priority = (osPriority_t) osPriorityBelowNormal,
};
/* Definitions for TaskBlink */
osThreadId_t TaskBlinkHandle;
uint32_t TaskBlinkBuffer[ 128 ];
osStaticThreadDef_t TaskBlinkControlBlock;
const osThreadAttr_t TaskBlink_attributes = {
  .name = "TaskBlink",
  .cb_mem = &TaskBlinkControlBlock,
  .cb_size = sizeof(TaskBlinkControlBlock),
  .stack_mem = &TaskBlinkBuffer[0],
  .stack_size = sizeof(TaskBlinkBuffer),
  .priority = (osPriority_t) osPriorityBelowNormal,
};
/* Definitions for TaskLeds */
osThreadId_t TaskLedsHandle;
uint32_t TaskLedsBuffer[ 128 ];
osStaticThreadDef_t TaskLedsControlBlock;
const osThreadAttr_t TaskLeds_attributes = {
  .name = "TaskLeds",
  .cb_mem = &TaskLedsControlBlock,
  .cb_size = sizeof(TaskLedsControlBlock),
  .stack_mem = &TaskLedsBuffer[0],
  .stack_size = sizeof(TaskLedsBuffer),
  .priority = (osPriority_t) osPriorityBelowNormal,
};
/* Definitions for TaskWdgM */
osThreadId_t TaskWdgMHandle;
uint32_t TaskWdgMBuffer[ 128 ];
osStaticThreadDef_t TaskWdgMControlBlock;
const osThreadAttr_t TaskWdgM_attributes = {
  .name = "TaskWdgM",
  .cb_mem = &TaskWdgMControlBlock,
  .cb_size = sizeof(TaskWdgMControlBlock),
  .stack_mem = &TaskWdgMBuffer[0],
  .stack_size = sizeof(TaskWdgMBuffer),
  .priority = (osPriority_t) osPriorityBelowNormal,
};
/* Definitions for TaskCOM */
osThreadId_t TaskCOMHandle;
uint32_t TaskCOMBuffer[ 128 ];
osStaticThreadDef_t TaskCOMControlBlock;
const osThreadAttr_t TaskCOM_attributes = {
  .name = "TaskCOM",
  .cb_mem = &TaskCOMControlBlock,
  .cb_size = sizeof(TaskCOMControlBlock),
  .stack_mem = &TaskCOMBuffer[0],
  .stack_size = sizeof(TaskCOMBuffer),
  .priority = (osPriority_t) osPriorityBelowNormal,
};
/* Definitions for TaskDiagAppl */
osThreadId_t TaskDiagApplHandle;
uint32_t TaskDiagApplBuffer[ 128 ];
osStaticThreadDef_t TaskDiagApplControlBlock;
const osThreadAttr_t TaskDiagAppl_attributes = {
  .name = "TaskDiagAppl",
  .cb_mem = &TaskDiagApplControlBlock,
  .cb_size = sizeof(TaskDiagApplControlBlock),
  .stack_mem = &TaskDiagApplBuffer[0],
  .stack_size = sizeof(TaskDiagApplBuffer),
  .priority = (osPriority_t) osPriorityBelowNormal,
};
/* Definitions for TaskGPUResMan */
osThreadId_t TaskGPUResManHandle;
uint32_t TaskGPUResManBuffer[ 256 ];
osStaticThreadDef_t TaskGPUResManControlBlock;
const osThreadAttr_t TaskGPUResMan_attributes = {
  .name = "TaskGPUResMan",
  .cb_mem = &TaskGPUResManControlBlock,
  .cb_size = sizeof(TaskGPUResManControlBlock),
  .stack_mem = &TaskGPUResManBuffer[0],
  .stack_size = sizeof(TaskGPUResManBuffer),
  .priority = (osPriority_t) osPriorityAboveNormal,
};
/* Definitions for xFIFOSetGPUReq */
osMessageQueueId_t xFIFOSetGPUReqHandle;
const osMessageQueueAttr_t xFIFOSetGPUReq_attributes = {
  .name = "xFIFOSetGPUReq"
};
/* Definitions for xFIFOGetGPUBuf */
osMessageQueueId_t xFIFOGetGPUBufHandle;
const osMessageQueueAttr_t xFIFOGetGPUBuf_attributes = {
  .name = "xFIFOGetGPUBuf"
};
/* Definitions for xSemaphoreCOMReady */
osSemaphoreId_t xSemaphoreCOMReadyHandle;
const osSemaphoreAttr_t xSemaphoreCOMReady_attributes = {
  .name = "xSemaphoreCOMReady"
};
/* Definitions for xSemaphoreDMACplt3 */
osSemaphoreId_t xSemaphoreDMACplt3Handle;
osStaticSemaphoreDef_t xSemaphoreDMACplt3ControlBlock;
const osSemaphoreAttr_t xSemaphoreDMACplt3_attributes = {
  .name = "xSemaphoreDMACplt3",
  .cb_mem = &xSemaphoreDMACplt3ControlBlock,
  .cb_size = sizeof(xSemaphoreDMACplt3ControlBlock),
};
/* Definitions for xSemaphoreDMACplt4 */
osSemaphoreId_t xSemaphoreDMACplt4Handle;
osStaticSemaphoreDef_t xSemaphoreDMACplt4ControlBlock;
const osSemaphoreAttr_t xSemaphoreDMACplt4_attributes = {
  .name = "xSemaphoreDMACplt4",
  .cb_mem = &xSemaphoreDMACplt4ControlBlock,
  .cb_size = sizeof(xSemaphoreDMACplt4ControlBlock),
};
/* Definitions for xSemaphoreDMACplt5 */
osSemaphoreId_t xSemaphoreDMACplt5Handle;
osStaticSemaphoreDef_t xSemaphoreDMACplt5ControlBlock;
const osSemaphoreAttr_t xSemaphoreDMACplt5_attributes = {
  .name = "xSemaphoreDMACplt5",
  .cb_mem = &xSemaphoreDMACplt5ControlBlock,
  .cb_size = sizeof(xSemaphoreDMACplt5ControlBlock),
};
/* Definitions for xSemaphoreDMACplt0 */
osSemaphoreId_t xSemaphoreDMACplt0Handle;
osStaticSemaphoreDef_t xSemaphoreDMACplt0ControlBlock;
const osSemaphoreAttr_t xSemaphoreDMACplt0_attributes = {
  .name = "xSemaphoreDMACplt0",
  .cb_mem = &xSemaphoreDMACplt0ControlBlock,
  .cb_size = sizeof(xSemaphoreDMACplt0ControlBlock),
};
/* Definitions for xEventFinishedInit */
osEventFlagsId_t xEventFinishedInitHandle;
const osEventFlagsAttr_t xEventFinishedInit_attributes = {
  .name = "xEventFinishedInit"
};
/* Definitions for xEventDID */
osEventFlagsId_t xEventDIDHandle;
const osEventFlagsAttr_t xEventDID_attributes = {
  .name = "xEventDID"
};
/* Definitions for xEventDTC */
osEventFlagsId_t xEventDTCHandle;
const osEventFlagsAttr_t xEventDTC_attributes = {
  .name = "xEventDTC"
};
/* USER CODE BEGIN PV */
osMemoryPoolId_t MemoryPool8;  /*Generic Memory Pool designed for members of 1 Byte size*/
osMemoryPoolId_t MemoryPool16; /*Generic Memory Pool designed for members of 2 Byte size*/
osMemoryPoolId_t MemoryPool32; /*Generic Memory Pool designed for members of 4 Byte size*/

osMemoryPoolId_t MemoryPool16_UI_PixelsValue; /*Component Specific Memory Pool*/
osMemoryPoolId_t MemoryPool16_UI_PixelsIndex; /*Component Specific Memory Pool*/
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM1_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_TIM11_Init(void);
void vTaskIdle(void *argument);
extern void vTaskUI(void *argument);
void vTaskBlink(void *argument);
void vTaskLeds(void *argument);
void vTaskWdgM(void *argument);
extern void vTaskCOM(void *argument);
extern void vTaskDiagAppl(void *argument);
extern void vTaskGPUResMan(void *argument);

/* USER CODE BEGIN PFP */

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
  MX_SPI1_Init();
  MX_TIM1_Init();
  MX_USART1_UART_Init();
  MX_TIM11_Init();
  /* USER CODE BEGIN 2 */
  memoryPoolInit();
  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* creation of xSemaphoreCOMReady */
  xSemaphoreCOMReadyHandle = osSemaphoreNew(1, 1, &xSemaphoreCOMReady_attributes);

  /* creation of xSemaphoreDMACplt3 */
  xSemaphoreDMACplt3Handle = osSemaphoreNew(1, 1, &xSemaphoreDMACplt3_attributes);

  /* creation of xSemaphoreDMACplt4 */
  xSemaphoreDMACplt4Handle = osSemaphoreNew(1, 1, &xSemaphoreDMACplt4_attributes);

  /* creation of xSemaphoreDMACplt5 */
  xSemaphoreDMACplt5Handle = osSemaphoreNew(1, 1, &xSemaphoreDMACplt5_attributes);

  /* creation of xSemaphoreDMACplt0 */
  xSemaphoreDMACplt0Handle = osSemaphoreNew(1, 1, &xSemaphoreDMACplt0_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of xFIFOSetGPUReq */
  xFIFOSetGPUReqHandle = osMessageQueueNew (16, sizeof(GPUReq_t), &xFIFOSetGPUReq_attributes);

  /* creation of xFIFOGetGPUBuf */
  xFIFOGetGPUBufHandle = osMessageQueueNew (16, sizeof(void **), &xFIFOGetGPUBuf_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of TaskIdle */
  TaskIdleHandle = osThreadNew(vTaskIdle, NULL, &TaskIdle_attributes);

  /* creation of TaskUI */
  TaskUIHandle = osThreadNew(vTaskUI, NULL, &TaskUI_attributes);

  /* creation of TaskBlink */
  TaskBlinkHandle = osThreadNew(vTaskBlink, NULL, &TaskBlink_attributes);

  /* creation of TaskLeds */
  TaskLedsHandle = osThreadNew(vTaskLeds, NULL, &TaskLeds_attributes);

  /* creation of TaskWdgM */
  TaskWdgMHandle = osThreadNew(vTaskWdgM, NULL, &TaskWdgM_attributes);

  /* creation of TaskCOM */
  TaskCOMHandle = osThreadNew(vTaskCOM, NULL, &TaskCOM_attributes);

  /* creation of TaskDiagAppl */
  TaskDiagApplHandle = osThreadNew(vTaskDiagAppl, NULL, &TaskDiagAppl_attributes);

  /* creation of TaskGPUResMan */
  TaskGPUResManHandle = osThreadNew(vTaskGPUResMan, NULL, &TaskGPUResMan_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* Create the event(s) */
  /* creation of xEventFinishedInit */
  xEventFinishedInitHandle = osEventFlagsNew(&xEventFinishedInit_attributes);

  /* creation of xEventDID */
  xEventDIDHandle = osEventFlagsNew(&xEventDID_attributes);

  /* creation of xEventDTC */
  xEventDTCHandle = osEventFlagsNew(&xEventDTC_attributes);

  /* USER CODE BEGIN RTOS_EVENTS */

  /* USER CODE END RTOS_EVENTS */

  /* Start scheduler */
  osKernelStart();
  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
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
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 12;
  RCC_OscInitStruct.PLL.PLLN = 96;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_Encoder_InitTypeDef sConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 0;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 65535;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI1;
  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 0;
  sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 0;
  if (HAL_TIM_Encoder_Init(&htim1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief TIM11 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM11_Init(void)
{

  /* USER CODE BEGIN TIM11_Init 0 */

  /* USER CODE END TIM11_Init 0 */

  /* USER CODE BEGIN TIM11_Init 1 */

  /* USER CODE END TIM11_Init 1 */
  htim11.Instance = TIM11;
  htim11.Init.Prescaler = 0;
  htim11.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim11.Init.Period = 1000-1;
  htim11.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim11.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim11) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM11_Init 2 */

  /* USER CODE END TIM11_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 921600;
  huart1.Init.WordLength = UART_WORDLENGTH_9B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_EVEN;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * Enable DMA controller clock
  * Configure DMA for memory to memory transfers
  *   hdma_memtomem_dma2_stream3
  *   hdma_memtomem_dma2_stream4
  *   hdma_memtomem_dma2_stream5
  *   hdma_memtomem_dma2_stream0
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* Configure DMA request hdma_memtomem_dma2_stream3 on DMA2_Stream3 */
  hdma_memtomem_dma2_stream3.Instance = DMA2_Stream3;
  hdma_memtomem_dma2_stream3.Init.Channel = DMA_CHANNEL_0;
  hdma_memtomem_dma2_stream3.Init.Direction = DMA_MEMORY_TO_MEMORY;
  hdma_memtomem_dma2_stream3.Init.PeriphInc = DMA_PINC_ENABLE;
  hdma_memtomem_dma2_stream3.Init.MemInc = DMA_MINC_ENABLE;
  hdma_memtomem_dma2_stream3.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  hdma_memtomem_dma2_stream3.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
  hdma_memtomem_dma2_stream3.Init.Mode = DMA_NORMAL;
  hdma_memtomem_dma2_stream3.Init.Priority = DMA_PRIORITY_MEDIUM;
  hdma_memtomem_dma2_stream3.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
  hdma_memtomem_dma2_stream3.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
  hdma_memtomem_dma2_stream3.Init.MemBurst = DMA_MBURST_SINGLE;
  hdma_memtomem_dma2_stream3.Init.PeriphBurst = DMA_PBURST_SINGLE;
  if (HAL_DMA_Init(&hdma_memtomem_dma2_stream3) != HAL_OK)
  {
    Error_Handler( );
  }

  /* Configure DMA request hdma_memtomem_dma2_stream4 on DMA2_Stream4 */
  hdma_memtomem_dma2_stream4.Instance = DMA2_Stream4;
  hdma_memtomem_dma2_stream4.Init.Channel = DMA_CHANNEL_0;
  hdma_memtomem_dma2_stream4.Init.Direction = DMA_MEMORY_TO_MEMORY;
  hdma_memtomem_dma2_stream4.Init.PeriphInc = DMA_PINC_ENABLE;
  hdma_memtomem_dma2_stream4.Init.MemInc = DMA_MINC_ENABLE;
  hdma_memtomem_dma2_stream4.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
  hdma_memtomem_dma2_stream4.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
  hdma_memtomem_dma2_stream4.Init.Mode = DMA_NORMAL;
  hdma_memtomem_dma2_stream4.Init.Priority = DMA_PRIORITY_MEDIUM;
  hdma_memtomem_dma2_stream4.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
  hdma_memtomem_dma2_stream4.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
  hdma_memtomem_dma2_stream4.Init.MemBurst = DMA_MBURST_SINGLE;
  hdma_memtomem_dma2_stream4.Init.PeriphBurst = DMA_PBURST_SINGLE;
  if (HAL_DMA_Init(&hdma_memtomem_dma2_stream4) != HAL_OK)
  {
    Error_Handler( );
  }

  /* Configure DMA request hdma_memtomem_dma2_stream5 on DMA2_Stream5 */
  hdma_memtomem_dma2_stream5.Instance = DMA2_Stream5;
  hdma_memtomem_dma2_stream5.Init.Channel = DMA_CHANNEL_0;
  hdma_memtomem_dma2_stream5.Init.Direction = DMA_MEMORY_TO_MEMORY;
  hdma_memtomem_dma2_stream5.Init.PeriphInc = DMA_PINC_ENABLE;
  hdma_memtomem_dma2_stream5.Init.MemInc = DMA_MINC_ENABLE;
  hdma_memtomem_dma2_stream5.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
  hdma_memtomem_dma2_stream5.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
  hdma_memtomem_dma2_stream5.Init.Mode = DMA_NORMAL;
  hdma_memtomem_dma2_stream5.Init.Priority = DMA_PRIORITY_MEDIUM;
  hdma_memtomem_dma2_stream5.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
  hdma_memtomem_dma2_stream5.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
  hdma_memtomem_dma2_stream5.Init.MemBurst = DMA_MBURST_SINGLE;
  hdma_memtomem_dma2_stream5.Init.PeriphBurst = DMA_PBURST_SINGLE;
  if (HAL_DMA_Init(&hdma_memtomem_dma2_stream5) != HAL_OK)
  {
    Error_Handler( );
  }

  /* Configure DMA request hdma_memtomem_dma2_stream0 on DMA2_Stream0 */
  hdma_memtomem_dma2_stream0.Instance = DMA2_Stream0;
  hdma_memtomem_dma2_stream0.Init.Channel = DMA_CHANNEL_0;
  hdma_memtomem_dma2_stream0.Init.Direction = DMA_MEMORY_TO_MEMORY;
  hdma_memtomem_dma2_stream0.Init.PeriphInc = DMA_PINC_DISABLE;
  hdma_memtomem_dma2_stream0.Init.MemInc = DMA_MINC_ENABLE;
  hdma_memtomem_dma2_stream0.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
  hdma_memtomem_dma2_stream0.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
  hdma_memtomem_dma2_stream0.Init.Mode = DMA_NORMAL;
  hdma_memtomem_dma2_stream0.Init.Priority = DMA_PRIORITY_MEDIUM;
  hdma_memtomem_dma2_stream0.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
  hdma_memtomem_dma2_stream0.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
  hdma_memtomem_dma2_stream0.Init.MemBurst = DMA_MBURST_SINGLE;
  hdma_memtomem_dma2_stream0.Init.PeriphBurst = DMA_PBURST_SINGLE;
  if (HAL_DMA_Init(&hdma_memtomem_dma2_stream0) != HAL_OK)
  {
    Error_Handler( );
  }

  /* DMA interrupt init */
  /* DMA2_Stream2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);
  /* DMA2_Stream7_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream7_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream7_IRQn);

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
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_GP_GPIO_Port, LED_GP_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LCD_DC_Pin|LCD_RST_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LCD_CS_Pin|LCD_BLK_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LED_COMM_Pin|LED_USB_Pin|LED_CONTROL_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LED_GP_Pin */
  GPIO_InitStruct.Pin = LED_GP_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_GP_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : Down_IT_Pin */
  GPIO_InitStruct.Pin = Down_IT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(Down_IT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : Left_IT_Pin Ok_IT_Pin Right_IT_Pin Up_IT_Pin
                           EncoderSW_Pin */
  GPIO_InitStruct.Pin = Left_IT_Pin|Ok_IT_Pin|Right_IT_Pin|Up_IT_Pin
                          |EncoderSW_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : LCD_DC_Pin LCD_RST_Pin */
  GPIO_InitStruct.Pin = LCD_DC_Pin|LCD_RST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : LCD_CS_Pin LCD_BLK_Pin */
  GPIO_InitStruct.Pin = LCD_CS_Pin|LCD_BLK_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : LED_COMM_Pin LED_USB_Pin LED_CONTROL_Pin */
  GPIO_InitStruct.Pin = LED_COMM_Pin|LED_USB_Pin|LED_CONTROL_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  HAL_NVIC_SetPriority(EXTI1_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

  HAL_NVIC_SetPriority(EXTI2_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI2_IRQn);

  HAL_NVIC_SetPriority(EXTI3_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI3_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* USER CODE BEGIN Header_vTaskIdle */
/**
  * @brief  Function implementing the TaskIdle thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_vTaskIdle */
void vTaskIdle(void *argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_vTaskBlink */
/**
* @brief Function implementing the TaskBlink thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_vTaskBlink */
void vTaskBlink(void *argument)
{
  /* USER CODE BEGIN vTaskBlink */

  for(;;)
  {
    HAL_GPIO_TogglePin(LED_GP_GPIO_Port, LED_GP_Pin);
    osDelay(pdMS_TO_TICKS(250));
  }
  /* USER CODE END vTaskBlink */
}

/* USER CODE BEGIN Header_vTaskLeds */
/**
* @brief Function implementing the TaskLeds thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_vTaskLeds */
void vTaskLeds(void *argument)
{
  /* USER CODE BEGIN vTaskLeds */
  /* Infinite loop */
  for(;;)
  {
    HAL_GPIO_TogglePin(LED_COMM_GPIO_Port, LED_COMM_Pin);
    osDelay(pdMS_TO_TICKS(100));
    HAL_GPIO_TogglePin(LED_USB_GPIO_Port, LED_USB_Pin);
    osDelay(pdMS_TO_TICKS(100));
    HAL_GPIO_TogglePin(LED_CONTROL_GPIO_Port, LED_CONTROL_Pin);
    osDelay(pdMS_TO_TICKS(100));

  }
  /* USER CODE END vTaskLeds */
}

/* USER CODE BEGIN Header_vTaskWdgM */
/**
* @brief Function implementing the TaskWdgM thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_vTaskWdgM */
void vTaskWdgM(void *argument)
{
  /* USER CODE BEGIN vTaskWdgM */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END vTaskWdgM */
}

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
