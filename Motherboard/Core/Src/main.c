/* USER CODE BEGIN Header */
/**
 * @file      main.c
 * @author    Pedro Rojo (pedroeroca@outlook.com)
 *
 * @brief     Main program Body
 *
 * @date      May 29, 2024
 *
 * @license   This Source Code Form is subject to the terms of
 *            the Mozilla Public License, v. 2.0. If a copy of
 *            the MPL was not distributed with this file, You
 *            can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * @copyright Copyright (c) 2024
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "DistanceSensor.h"
#include "DiagAppl.h"
#include "PID.h"
#include "FAN.h"
#include "COM.h"
#include "Signals.h"
#include "NVM.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef StaticTask_t osStaticThreadDef_t;
typedef StaticQueue_t osStaticMessageQDef_t;
typedef StaticTimer_t osStaticTimerDef_t;
typedef StaticSemaphore_t osStaticSemaphoreDef_t;
typedef StaticEventGroup_t osStaticEventGroupDef_t;
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/*Documenting the peripheals*/

/**
I2C_HandleTypeDef hi2c1; //Connection to the VL53L0X
I2C_HandleTypeDef hi2c2; //Connection to the AT24C04
DMA_HandleTypeDef hdma_i2c2_rx;
DMA_HandleTypeDef hdma_i2c2_tx;

IWDG_HandleTypeDef hiwdg; //Watchdog timer @ 500ms

RTC_HandleTypeDef hrtc; //RTC for date control

Requirements on timers
Acknowledge with Daughter Board @ 10ms (Period desired by COM Engineer)
Fan RPM measure (Input Capture)
Fan PWM controller @ 24KHz (manufacturer recommended frequency)

TIM_HandleTypeDef htim1; //Pending for application
TIM_HandleTypeDef htim2; //CPU Load measure for the OS
TIM_HandleTypeDef htim3; //Fan PWM controller @ 24KHz (manufacturer recommended frequency)
TIM_HandleTypeDef htim4; //Fan RPM measure (Input Capture) @
TIM_HandleTypeDef htim5; //Pending for application

UART_HandleTypeDef huart1; //Daughter board connection
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart1_tx;
 */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;
I2C_HandleTypeDef hi2c2;
DMA_HandleTypeDef hdma_i2c2_rx;
DMA_HandleTypeDef hdma_i2c2_tx;

IWDG_HandleTypeDef hiwdg;

RTC_HandleTypeDef hrtc;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;
TIM_HandleTypeDef htim5;

UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_tx;

/* Definitions for TaskWdgM */
osThreadId_t TaskWdgMHandle;
uint32_t TaskWdgMBuffer[ 256 ];
osStaticThreadDef_t TaskWdgMControlBlock;
const osThreadAttr_t TaskWdgM_attributes = {
  .name = "TaskWdgM",
  .cb_mem = &TaskWdgMControlBlock,
  .cb_size = sizeof(TaskWdgMControlBlock),
  .stack_mem = &TaskWdgMBuffer[0],
  .stack_size = sizeof(TaskWdgMBuffer),
  .priority = (osPriority_t) osPriorityRealtime,
};
/* Definitions for TaskModeManager */
osThreadId_t TaskModeManagerHandle;
uint32_t TaskModeManagerBuffer[ 128 ];
osStaticThreadDef_t TaskModeManagerControlBlock;
const osThreadAttr_t TaskModeManager_attributes = {
  .name = "TaskModeManager",
  .cb_mem = &TaskModeManagerControlBlock,
  .cb_size = sizeof(TaskModeManagerControlBlock),
  .stack_mem = &TaskModeManagerBuffer[0],
  .stack_size = sizeof(TaskModeManagerBuffer),
  .priority = (osPriority_t) osPriorityAboveNormal,
};
/* Definitions for TaskPID */
osThreadId_t TaskPIDHandle;
uint32_t TaskPIDBuffer[ 256 ];
osStaticThreadDef_t TaskPIDControlBlock;
const osThreadAttr_t TaskPID_attributes = {
  .name = "TaskPID",
  .cb_mem = &TaskPIDControlBlock,
  .cb_size = sizeof(TaskPIDControlBlock),
  .stack_mem = &TaskPIDBuffer[0],
  .stack_size = sizeof(TaskPIDBuffer),
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for TaskCOM */
osThreadId_t TaskCOMHandle;
uint32_t TaskCOMBuffer[ 512 ];
osStaticThreadDef_t TaskCOMControlBlock;
const osThreadAttr_t TaskCOM_attributes = {
  .name = "TaskCOM",
  .cb_mem = &TaskCOMControlBlock,
  .cb_size = sizeof(TaskCOMControlBlock),
  .stack_mem = &TaskCOMBuffer[0],
  .stack_size = sizeof(TaskCOMBuffer),
  .priority = (osPriority_t) osPriorityNormal,
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
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for TaskLeds */
osThreadId_t TaskLedsHandle;
uint32_t TaskLedsBuffer[ 64 ];
osStaticThreadDef_t TaskLedsControlBlock;
const osThreadAttr_t TaskLeds_attributes = {
  .name = "TaskLeds",
  .cb_mem = &TaskLedsControlBlock,
  .cb_size = sizeof(TaskLedsControlBlock),
  .stack_mem = &TaskLedsBuffer[0],
  .stack_size = sizeof(TaskLedsBuffer),
  .priority = (osPriority_t) osPriorityAboveNormal,
};
/* Definitions for TaskSensor */
osThreadId_t TaskSensorHandle;
uint32_t TaskSensorBuffer[ 256 ];
osStaticThreadDef_t TaskSensorControlBlock;
const osThreadAttr_t TaskSensor_attributes = {
  .name = "TaskSensor",
  .cb_mem = &TaskSensorControlBlock,
  .cb_size = sizeof(TaskSensorControlBlock),
  .stack_mem = &TaskSensorBuffer[0],
  .stack_size = sizeof(TaskSensorBuffer),
  .priority = (osPriority_t) osPriorityAboveNormal,
};
/* Definitions for TaskFAN */
osThreadId_t TaskFANHandle;
uint32_t TaskFANBuffer[ 256 ];
osStaticThreadDef_t TaskFANControlBlock;
const osThreadAttr_t TaskFAN_attributes = {
  .name = "TaskFAN",
  .cb_mem = &TaskFANControlBlock,
  .cb_size = sizeof(TaskFANControlBlock),
  .stack_mem = &TaskFANBuffer[0],
  .stack_size = sizeof(TaskFANBuffer),
  .priority = (osPriority_t) osPriorityAboveNormal,
};
/* Definitions for xFIFO_COM */
osMessageQueueId_t xFIFO_COMHandle;
uint8_t xFIFO_COMBuffer[ 16 * sizeof( PDU_t ) ];
osStaticMessageQDef_t xFIFO_COMControlBlock;
const osMessageQueueAttr_t xFIFO_COM_attributes = {
  .name = "xFIFO_COM",
  .cb_mem = &xFIFO_COMControlBlock,
  .cb_size = sizeof(xFIFO_COMControlBlock),
  .mq_mem = &xFIFO_COMBuffer,
  .mq_size = sizeof(xFIFO_COMBuffer)
};
/* Definitions for xFIFO_Distance */
osMessageQueueId_t xFIFO_DistanceHandle;
uint8_t xFIFO_DistanceBuffer[ 4 * sizeof( uint16_t ) ];
osStaticMessageQDef_t xFIFO_DistanceControlBlock;
const osMessageQueueAttr_t xFIFO_Distance_attributes = {
  .name = "xFIFO_Distance",
  .cb_mem = &xFIFO_DistanceControlBlock,
  .cb_size = sizeof(xFIFO_DistanceControlBlock),
  .mq_mem = &xFIFO_DistanceBuffer,
  .mq_size = sizeof(xFIFO_DistanceBuffer)
};
/* Definitions for xFIFO_ControlAction */
osMessageQueueId_t xFIFO_ControlActionHandle;
const osMessageQueueAttr_t xFIFO_ControlAction_attributes = {
  .name = "xFIFO_ControlAction"
};
/* Definitions for xFIFO_PIDConfigs */
osMessageQueueId_t xFIFO_PIDConfigsHandle;
const osMessageQueueAttr_t xFIFO_PIDConfigs_attributes = {
  .name = "xFIFO_PIDConfigs"
};
/* Definitions for xFIFO_DiagsLong */
osMessageQueueId_t xFIFO_DiagsLongHandle;
const osMessageQueueAttr_t xFIFO_DiagsLong_attributes = {
  .name = "xFIFO_DiagsLong"
};
/* Definitions for xFIFO_DiagShort */
osMessageQueueId_t xFIFO_DiagShortHandle;
const osMessageQueueAttr_t xFIFO_DiagShort_attributes = {
  .name = "xFIFO_DiagShort"
};
/* Definitions for xFIFO_COMDistance */
osMessageQueueId_t xFIFO_COMDistanceHandle;
uint8_t xFIFO_COMDistanceBuffer[ 16 * sizeof( int16_t ) ];
osStaticMessageQDef_t xFIFO_COMDistanceControlBlock;
const osMessageQueueAttr_t xFIFO_COMDistance_attributes = {
  .name = "xFIFO_COMDistance",
  .cb_mem = &xFIFO_COMDistanceControlBlock,
  .cb_size = sizeof(xFIFO_COMDistanceControlBlock),
  .mq_mem = &xFIFO_COMDistanceBuffer,
  .mq_size = sizeof(xFIFO_COMDistanceBuffer)
};
/* Definitions for xFIFO_COMRPM */
osMessageQueueId_t xFIFO_COMRPMHandle;
uint8_t xFIFO_COMRPMBuffer[ 16 * sizeof( int16_t ) ];
osStaticMessageQDef_t xFIFO_COMRPMControlBlock;
const osMessageQueueAttr_t xFIFO_COMRPM_attributes = {
  .name = "xFIFO_COMRPM",
  .cb_mem = &xFIFO_COMRPMControlBlock,
  .cb_size = sizeof(xFIFO_COMRPMControlBlock),
  .mq_mem = &xFIFO_COMRPMBuffer,
  .mq_size = sizeof(xFIFO_COMRPMBuffer)
};
/* Definitions for xTimer_UARTSend */
osTimerId_t xTimer_UARTSendHandle;
osStaticTimerDef_t xTimer_ControlBlock;
const osTimerAttr_t xTimer_UARTSend_attributes = {
  .name = "xTimer_UARTSend",
  .cb_mem = &xTimer_ControlBlock,
  .cb_size = sizeof(xTimer_ControlBlock),
};
/* Definitions for xTimer_Ack */
osTimerId_t xTimer_AckHandle;
osStaticTimerDef_t xTimer_AckControlBlock;
const osTimerAttr_t xTimer_Ack_attributes = {
  .name = "xTimer_Ack",
  .cb_mem = &xTimer_AckControlBlock,
  .cb_size = sizeof(xTimer_AckControlBlock),
};
/* Definitions for xSemaphore_PID */
osSemaphoreId_t xSemaphore_PIDHandle;
osStaticSemaphoreDef_t xSemaphore_PIDControlBlock;
const osSemaphoreAttr_t xSemaphore_PID_attributes = {
  .name = "xSemaphore_PID",
  .cb_mem = &xSemaphore_PIDControlBlock,
  .cb_size = sizeof(xSemaphore_PIDControlBlock),
};
/* Definitions for xSemaphore_PID_Init */
osSemaphoreId_t xSemaphore_PID_InitHandle;
osStaticSemaphoreDef_t xSemaphore_PID_InitControlBlock;
const osSemaphoreAttr_t xSemaphore_PID_Init_attributes = {
  .name = "xSemaphore_PID_Init",
  .cb_mem = &xSemaphore_PID_InitControlBlock,
  .cb_size = sizeof(xSemaphore_PID_InitControlBlock),
};
/* Definitions for xSemaphore_DMA_TransferCplt */
osSemaphoreId_t xSemaphore_DMA_TransferCpltHandle;
osStaticSemaphoreDef_t xSemaphore_DMA_TransferCpltControlBlock;
const osSemaphoreAttr_t xSemaphore_DMA_TransferCplt_attributes = {
  .name = "xSemaphore_DMA_TransferCplt",
  .cb_mem = &xSemaphore_DMA_TransferCpltControlBlock,
  .cb_size = sizeof(xSemaphore_DMA_TransferCpltControlBlock),
};
/* Definitions for xSemaphore_MemoryPoolUsed */
osSemaphoreId_t xSemaphore_MemoryPoolUsedHandle;
osStaticSemaphoreDef_t xSemaphore_MemoryPoolUsedControlBlock;
const osSemaphoreAttr_t xSemaphore_MemoryPoolUsed_attributes = {
  .name = "xSemaphore_MemoryPoolUsed",
  .cb_mem = &xSemaphore_MemoryPoolUsedControlBlock,
  .cb_size = sizeof(xSemaphore_MemoryPoolUsedControlBlock),
};
/* Definitions for xSemaphore_SensorTxCplt */
osSemaphoreId_t xSemaphore_SensorTxCpltHandle;
osStaticSemaphoreDef_t xSemaphore_SensorTxCpltControlBlock;
const osSemaphoreAttr_t xSemaphore_SensorTxCplt_attributes = {
  .name = "xSemaphore_SensorTxCplt",
  .cb_mem = &xSemaphore_SensorTxCpltControlBlock,
  .cb_size = sizeof(xSemaphore_SensorTxCpltControlBlock),
};
/* Definitions for xSemaphore_SensorRxCplt */
osSemaphoreId_t xSemaphore_SensorRxCpltHandle;
osStaticSemaphoreDef_t xSemaphore_SensorRxCpltControlBlock;
const osSemaphoreAttr_t xSemaphore_SensorRxCplt_attributes = {
  .name = "xSemaphore_SensorRxCplt",
  .cb_mem = &xSemaphore_SensorRxCpltControlBlock,
  .cb_size = sizeof(xSemaphore_SensorRxCpltControlBlock),
};
/* Definitions for xSemaphore_Ack */
osSemaphoreId_t xSemaphore_AckHandle;
osStaticSemaphoreDef_t xSemaphore_AckControlBlock;
const osSemaphoreAttr_t xSemaphore_Ack_attributes = {
  .name = "xSemaphore_Ack",
  .cb_mem = &xSemaphore_AckControlBlock,
  .cb_size = sizeof(xSemaphore_AckControlBlock),
};
/* Definitions for xSemaphore_InitDaughter */
osSemaphoreId_t xSemaphore_InitDaughterHandle;
const osSemaphoreAttr_t xSemaphore_InitDaughter_attributes = {
  .name = "xSemaphore_InitDaughter"
};
/* Definitions for xEvent_FatalError */
osEventFlagsId_t xEvent_FatalErrorHandle;
osStaticEventGroupDef_t xEvent_FatalErrorControlBlock;
const osEventFlagsAttr_t xEvent_FatalError_attributes = {
  .name = "xEvent_FatalError",
  .cb_mem = &xEvent_FatalErrorControlBlock,
  .cb_size = sizeof(xEvent_FatalErrorControlBlock),
};
/* Definitions for xEvent_ControlModes */
osEventFlagsId_t xEvent_ControlModesHandle;
osStaticEventGroupDef_t xEvent_ControlModesControlBlock;
const osEventFlagsAttr_t xEvent_ControlModes_attributes = {
  .name = "xEvent_ControlModes",
  .cb_mem = &xEvent_ControlModesControlBlock,
  .cb_size = sizeof(xEvent_ControlModesControlBlock),
};
/* Definitions for xEvent_Diagnostics */
osEventFlagsId_t xEvent_DiagnosticsHandle;
osStaticEventGroupDef_t xEvent_DiagnosticsControlBlock;
const osEventFlagsAttr_t xEvent_Diagnostics_attributes = {
  .name = "xEvent_Diagnostics",
  .cb_mem = &xEvent_DiagnosticsControlBlock,
  .cb_size = sizeof(xEvent_DiagnosticsControlBlock),
};
/* Definitions for xEvent_USB */
osEventFlagsId_t xEvent_USBHandle;
osStaticEventGroupDef_t xEvent_USBControlBlock;
const osEventFlagsAttr_t xEvent_USB_attributes = {
  .name = "xEvent_USB",
  .cb_mem = &xEvent_USBControlBlock,
  .cb_size = sizeof(xEvent_USBControlBlock),
};
/* USER CODE BEGIN PV */
osMemoryPoolId_t MemoryPoolNVM;  /* Memory Pool for NVM data allocation*/

char ResBuffer[64];
uint8_t ReceiveFlag;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_IWDG_Init(void);
static void MX_RTC_Init(void);
static void MX_I2C1_Init(void);
static void MX_I2C2_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM4_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM5_Init(void);
void vTaskWdgM(void *argument);
extern void vTaskModeManager(void *argument);
extern void vTaskPID(void *argument);
extern void vTaskCOM(void *argument);
extern void vTaskDiagAppl(void *argument);
extern void vTaskLeds(void *argument);
extern void vTaskSensor(void *argument);
extern void vTaskFAN(void *argument);
extern void vTimer_UARTSendCallback(void *argument);
extern void vTimer_Ack_Callback(void *argument);

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
  MX_IWDG_Init();
  MX_RTC_Init();
  MX_I2C1_Init();
  MX_I2C2_Init();
  MX_USART1_UART_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_TIM5_Init();
  /* USER CODE BEGIN 2 */
  MemoryPoolNVM = osMemoryPoolNew(EEPROM_SIZE, sizeof(uint8_t), NULL);
  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* creation of xSemaphore_PID */
  xSemaphore_PIDHandle = osSemaphoreNew(1, 1, &xSemaphore_PID_attributes);

  /* creation of xSemaphore_PID_Init */
  xSemaphore_PID_InitHandle = osSemaphoreNew(1, 1, &xSemaphore_PID_Init_attributes);

  /* creation of xSemaphore_DMA_TransferCplt */
  xSemaphore_DMA_TransferCpltHandle = osSemaphoreNew(1, 1, &xSemaphore_DMA_TransferCplt_attributes);

  /* creation of xSemaphore_MemoryPoolUsed */
  xSemaphore_MemoryPoolUsedHandle = osSemaphoreNew(1, 1, &xSemaphore_MemoryPoolUsed_attributes);

  /* creation of xSemaphore_SensorTxCplt */
  xSemaphore_SensorTxCpltHandle = osSemaphoreNew(1, 1, &xSemaphore_SensorTxCplt_attributes);

  /* creation of xSemaphore_SensorRxCplt */
  xSemaphore_SensorRxCpltHandle = osSemaphoreNew(1, 1, &xSemaphore_SensorRxCplt_attributes);

  /* creation of xSemaphore_Ack */
  xSemaphore_AckHandle = osSemaphoreNew(1, 1, &xSemaphore_Ack_attributes);

  /* creation of xSemaphore_InitDaughter */
  xSemaphore_InitDaughterHandle = osSemaphoreNew(1, 1, &xSemaphore_InitDaughter_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* Create the timer(s) */
  /* creation of xTimer_UARTSend */
  xTimer_UARTSendHandle = osTimerNew(vTimer_UARTSendCallback, osTimerPeriodic, NULL, &xTimer_UARTSend_attributes);

  /* creation of xTimer_Ack */
  xTimer_AckHandle = osTimerNew(vTimer_Ack_Callback, osTimerOnce, NULL, &xTimer_Ack_attributes);

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of xFIFO_COM */
  xFIFO_COMHandle = osMessageQueueNew (16, sizeof(PDU_t), &xFIFO_COM_attributes);

  /* creation of xFIFO_Distance */
  xFIFO_DistanceHandle = osMessageQueueNew (4, sizeof(uint16_t), &xFIFO_Distance_attributes);

  /* creation of xFIFO_ControlAction */
  xFIFO_ControlActionHandle = osMessageQueueNew (16, sizeof(float), &xFIFO_ControlAction_attributes);

  /* creation of xFIFO_PIDConfigs */
  xFIFO_PIDConfigsHandle = osMessageQueueNew (16, sizeof(PIDConfigs), &xFIFO_PIDConfigs_attributes);

  /* creation of xFIFO_DiagsLong */
  xFIFO_DiagsLongHandle = osMessageQueueNew (16, sizeof(DiagPDU_t), &xFIFO_DiagsLong_attributes);

  /* creation of xFIFO_DiagShort */
  xFIFO_DiagShortHandle = osMessageQueueNew (16, sizeof(PDU_t), &xFIFO_DiagShort_attributes);

  /* creation of xFIFO_COMDistance */
  xFIFO_COMDistanceHandle = osMessageQueueNew (16, sizeof(int16_t), &xFIFO_COMDistance_attributes);

  /* creation of xFIFO_COMRPM */
  xFIFO_COMRPMHandle = osMessageQueueNew (16, sizeof(int16_t), &xFIFO_COMRPM_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of TaskWdgM */
  TaskWdgMHandle = osThreadNew(vTaskWdgM, NULL, &TaskWdgM_attributes);

  /* creation of TaskModeManager */
  TaskModeManagerHandle = osThreadNew(vTaskModeManager, NULL, &TaskModeManager_attributes);

  /* creation of TaskPID */
  TaskPIDHandle = osThreadNew(vTaskPID, NULL, &TaskPID_attributes);

  /* creation of TaskCOM */
  TaskCOMHandle = osThreadNew(vTaskCOM, NULL, &TaskCOM_attributes);

  /* creation of TaskDiagAppl */
  TaskDiagApplHandle = osThreadNew(vTaskDiagAppl, NULL, &TaskDiagAppl_attributes);

  /* creation of TaskLeds */
  TaskLedsHandle = osThreadNew(vTaskLeds, NULL, &TaskLeds_attributes);

  /* creation of TaskSensor */
  TaskSensorHandle = osThreadNew(vTaskSensor, NULL, &TaskSensor_attributes);

  /* creation of TaskFAN */
  TaskFANHandle = osThreadNew(vTaskFAN, NULL, &TaskFAN_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* Create the event(s) */
  /* creation of xEvent_FatalError */
  xEvent_FatalErrorHandle = osEventFlagsNew(&xEvent_FatalError_attributes);

  /* creation of xEvent_ControlModes */
  xEvent_ControlModesHandle = osEventFlagsNew(&xEvent_ControlModes_attributes);

  /* creation of xEvent_Diagnostics */
  xEvent_DiagnosticsHandle = osEventFlagsNew(&xEvent_Diagnostics_attributes);

  /* creation of xEvent_USB */
  xEvent_USBHandle = osEventFlagsNew(&xEvent_USB_attributes);

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE
                              |RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 192;
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
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 400000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.ClockSpeed = 400000;
  hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

/**
  * @brief IWDG Initialization Function
  * @param None
  * @retval None
  */
static void MX_IWDG_Init(void)
{

  /* USER CODE BEGIN IWDG_Init 0 */

  /* USER CODE END IWDG_Init 0 */

  /* USER CODE BEGIN IWDG_Init 1 */

  /* USER CODE END IWDG_Init 1 */
  hiwdg.Instance = IWDG;
  hiwdg.Init.Prescaler = IWDG_PRESCALER_4;
  hiwdg.Init.Reload = 4000;
  if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN IWDG_Init 2 */

  /* USER CODE END IWDG_Init 2 */

}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_12;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

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

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 2;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 63999;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
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
  htim2.Init.Prescaler = 96-1;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 0xffffffff;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
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

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 3999;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

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
  TIM_IC_InitTypeDef sConfigIC = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 0;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 65535;
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
  if (HAL_TIM_IC_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim4, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
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
  htim5.Init.Prescaler = 14;
  htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim5.Init.Period = 63999;
  htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim5.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
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
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream2_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream2_IRQn);
  /* DMA1_Stream7_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream7_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream7_IRQn);
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
  HAL_GPIO_WritePin(LED_OR_GPIO_Port, LED_OR_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, RunningLed_Pin|GPULed_Pin|WP_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, EnableFAN_Pin|TOF_XSHUT_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LED_OR_Pin */
  GPIO_InitStruct.Pin = LED_OR_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_OR_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : DevMode_IT_Pin */
  GPIO_InitStruct.Pin = DevMode_IT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(DevMode_IT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : RunningLed_Pin GPULed_Pin WP_Pin */
  GPIO_InitStruct.Pin = RunningLed_Pin|GPULed_Pin|WP_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : EnableFAN_Pin TOF_XSHUT_Pin */
  GPIO_InitStruct.Pin = EnableFAN_Pin|TOF_XSHUT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* USER CODE BEGIN Header_vTaskWdgM */
/**
* @brief Function implementing the TaskWdgM thread.
* 			 Due to simple code handling here
* @note  Checking the possibilities on a window watch dog timer
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_vTaskWdgM */
void vTaskWdgM(void *argument)
{
  /* init code for USB_DEVICE */
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN 5 */
	const TickType_t ticksForResetWDG = pdMS_TO_TICKS(500); //WDG @ 500ms
	TickType_t ticks;

  NVM_Init();

	HAL_IWDG_Refresh(&hiwdg);
	ticks = osKernelGetTickCount();
  /* Infinite loop */
  for(;;)
  {
    HAL_IWDG_Refresh(&hiwdg);
    ticks += ticksForResetWDG;
    osDelayUntil(ticks);
  }
  /* USER CODE END 5 */
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
