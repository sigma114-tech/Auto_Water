/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define S_IN_EX_Pin GPIO_PIN_13
#define S_IN_EX_GPIO_Port GPIOC
#define S_Add_Water_Pin GPIO_PIN_14
#define S_Add_Water_GPIO_Port GPIOC
#define S_Stop_Add_Pin GPIO_PIN_15
#define S_Stop_Add_GPIO_Port GPIOC
#define State_LED1_Pin GPIO_PIN_0
#define State_LED1_GPIO_Port GPIOC
#define State_LED2_Pin GPIO_PIN_1
#define State_LED2_GPIO_Port GPIOC
#define State_LED3_Pin GPIO_PIN_2
#define State_LED3_GPIO_Port GPIOC
#define State_LED4_Pin GPIO_PIN_3
#define State_LED4_GPIO_Port GPIOC
#define Display_SA_Pin GPIO_PIN_0
#define Display_SA_GPIO_Port GPIOA
#define Display_SB_Pin GPIO_PIN_1
#define Display_SB_GPIO_Port GPIOA
#define Display_SC_Pin GPIO_PIN_2
#define Display_SC_GPIO_Port GPIOA
#define Display_SD_Pin GPIO_PIN_3
#define Display_SD_GPIO_Port GPIOA
#define Display_SE_Pin GPIO_PIN_4
#define Display_SE_GPIO_Port GPIOA
#define Display_SF_Pin GPIO_PIN_5
#define Display_SF_GPIO_Port GPIOA
#define Display_SG_Pin GPIO_PIN_6
#define Display_SG_GPIO_Port GPIOA
#define Display_SDP_Pin GPIO_PIN_7
#define Display_SDP_GPIO_Port GPIOA
#define State_LED5_Pin GPIO_PIN_4
#define State_LED5_GPIO_Port GPIOC
#define S_Drainage_Water_Pin GPIO_PIN_5
#define S_Drainage_Water_GPIO_Port GPIOC
#define Control_Supply_Pin GPIO_PIN_0
#define Control_Supply_GPIO_Port GPIOB
#define Control_Drainage_Pin GPIO_PIN_1
#define Control_Drainage_GPIO_Port GPIOB
#define Control_Speed1_Pin GPIO_PIN_2
#define Control_Speed1_GPIO_Port GPIOB
#define SPI2_DRDY_Pin GPIO_PIN_12
#define SPI2_DRDY_GPIO_Port GPIOB
#define SPI2_DRDY_EXTI_IRQn EXTI15_10_IRQn
#define S_Stop_Drainage_Pin GPIO_PIN_6
#define S_Stop_Drainage_GPIO_Port GPIOC
#define S_Simulation_Pin GPIO_PIN_7
#define S_Simulation_GPIO_Port GPIOC
#define S_Weight__Pin GPIO_PIN_8
#define S_Weight__GPIO_Port GPIOC
#define S_Weight_C9_Pin GPIO_PIN_9
#define S_Weight_C9_GPIO_Port GPIOC
#define Display_238D_A0_Pin GPIO_PIN_8
#define Display_238D_A0_GPIO_Port GPIOA
#define Display_238D_A1_Pin GPIO_PIN_9
#define Display_238D_A1_GPIO_Port GPIOA
#define Display_238D_A2_Pin GPIO_PIN_10
#define Display_238D_A2_GPIO_Port GPIOA
#define Display_238D_EN_Pin GPIO_PIN_11
#define Display_238D_EN_GPIO_Port GPIOA
#define State_LED6_Pin GPIO_PIN_12
#define State_LED6_GPIO_Port GPIOA
#define Low_Alarm_Pin GPIO_PIN_15
#define Low_Alarm_GPIO_Port GPIOA
#define S_Auto_Hand_Pin GPIO_PIN_10
#define S_Auto_Hand_GPIO_Port GPIOC
#define S_Mode_Pin GPIO_PIN_11
#define S_Mode_GPIO_Port GPIOC
#define S_Speed_Pin GPIO_PIN_12
#define S_Speed_GPIO_Port GPIOC
#define Control_Speed2_Pin GPIO_PIN_3
#define Control_Speed2_GPIO_Port GPIOB
#define Control_Speed3_Pin GPIO_PIN_4
#define Control_Speed3_GPIO_Port GPIOB
#define Control_Motor_ON_OFF_Pin GPIO_PIN_5
#define Control_Motor_ON_OFF_GPIO_Port GPIOB
#define Control_FM_Pin GPIO_PIN_6
#define Control_FM_GPIO_Port GPIOB
#define State_LED7_Pin GPIO_PIN_7
#define State_LED7_GPIO_Port GPIOB
#define State_LED8_Pin GPIO_PIN_8
#define State_LED8_GPIO_Port GPIOB
#define USART3_485_RE_DE_Pin GPIO_PIN_9
#define USART3_485_RE_DE_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
