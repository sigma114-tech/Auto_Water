/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for tskCtrl */
osThreadId_t tskCtrlHandle;
const osThreadAttr_t tskCtrl_attributes = {
  .name = "tskCtrl",
  .stack_size = 768 * 4,
  .priority = (osPriority_t) osPriorityAboveNormal,
};
/* Definitions for tskWeigh */
osThreadId_t tskWeighHandle;
const osThreadAttr_t tskWeigh_attributes = {
  .name = "tskWeigh",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityAboveNormal,
};
/* Definitions for tskAct */
osThreadId_t tskActHandle;
const osThreadAttr_t tskAct_attributes = {
  .name = "tskAct",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityAboveNormal,
};
/* Definitions for tskUI */
osThreadId_t tskUIHandle;
const osThreadAttr_t tskUI_attributes = {
  .name = "tskUI",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for tskCom */
osThreadId_t tskComHandle;
const osThreadAttr_t tskCom_attributes = {
  .name = "tskCom",
  .stack_size = 768 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for tskSafety */
osThreadId_t tskSafetyHandle;
const osThreadAttr_t tskSafety_attributes = {
  .name = "tskSafety",
  .stack_size = 384 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for tskWdg */
osThreadId_t tskWdgHandle;
const osThreadAttr_t tskWdg_attributes = {
  .name = "tskWdg",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityLow,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartCtrl(void *argument);
void StartWeigh(void *argument);
void StartAct(void *argument);
void StartUI(void *argument);
void StartCom(void *argument);
void StartTask06(void *argument);
void StartWdg(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* Hook prototypes */
void vApplicationMallocFailedHook(void);

/* USER CODE BEGIN 5 */
void vApplicationMallocFailedHook(void)
{
   /* vApplicationMallocFailedHook() will only be called if
   configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h. It is a hook
   function that will get called if a call to pvPortMalloc() fails.
   pvPortMalloc() is called internally by the kernel whenever a task, queue,
   timer or semaphore is created. It is also called by various parts of the
   demo application. If heap_1.c or heap_2.c are used, then the size of the
   heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
   FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
   to query the size of free heap space that remains (although it does not
   provide information on how the remaining heap might be fragmented). */
}
/* USER CODE END 5 */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of tskCtrl */
  tskCtrlHandle = osThreadNew(StartCtrl, NULL, &tskCtrl_attributes);

  /* creation of tskWeigh */
  tskWeighHandle = osThreadNew(StartWeigh, NULL, &tskWeigh_attributes);

  /* creation of tskAct */
  tskActHandle = osThreadNew(StartAct, NULL, &tskAct_attributes);

  /* creation of tskUI */
  tskUIHandle = osThreadNew(StartUI, NULL, &tskUI_attributes);

  /* creation of tskCom */
  tskComHandle = osThreadNew(StartCom, NULL, &tskCom_attributes);

  /* creation of tskSafety */
  tskSafetyHandle = osThreadNew(StartTask06, NULL, &tskSafety_attributes);

  /* creation of tskWdg */
  tskWdgHandle = osThreadNew(StartWdg, NULL, &tskWdg_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartCtrl */
/**
  * @brief  Function implementing the tskCtrl thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartCtrl */
void StartCtrl(void *argument)
{
  /* USER CODE BEGIN StartCtrl */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartCtrl */
}

/* USER CODE BEGIN Header_StartWeigh */
/**
* @brief Function implementing the tskWeigh thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartWeigh */
void StartWeigh(void *argument)
{
  /* USER CODE BEGIN StartWeigh */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartWeigh */
}

/* USER CODE BEGIN Header_StartAct */
/**
* @brief Function implementing the tskAct thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartAct */
void StartAct(void *argument)
{
  /* USER CODE BEGIN StartAct */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartAct */
}

/* USER CODE BEGIN Header_StartUI */
/**
* @brief Function implementing the tskUI thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartUI */
void StartUI(void *argument)
{
  /* USER CODE BEGIN StartUI */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartUI */
}

/* USER CODE BEGIN Header_StartCom */
/**
* @brief Function implementing the tskCom thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartCom */
void StartCom(void *argument)
{
  /* USER CODE BEGIN StartCom */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartCom */
}

/* USER CODE BEGIN Header_StartTask06 */
/**
* @brief Function implementing the tskSafety thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask06 */
void StartTask06(void *argument)
{
  /* USER CODE BEGIN StartTask06 */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartTask06 */
}

/* USER CODE BEGIN Header_StartWdg */
/**
* @brief Function implementing the tskWdg thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartWdg */
void StartWdg(void *argument)
{
  /* USER CODE BEGIN StartWdg */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartWdg */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

