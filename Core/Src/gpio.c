/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
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
#include "gpio.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, State_LED1_Pin|State_LED2_Pin|State_LED3_Pin|State_LED4_Pin
                          |State_LED5_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, Display_SA_Pin|Display_SB_Pin|Display_SC_Pin|Display_SD_Pin
                          |Display_SE_Pin|Display_SF_Pin|Display_SG_Pin|Display_SDP_Pin
                          |Display_238D_A0_Pin|Display_238D_A1_Pin|Display_238D_A2_Pin|Display_238D_EN_Pin
                          |State_LED6_Pin|Low_Alarm_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, Control_Supply_Pin|Control_Drainage_Pin|Control_Speed1_Pin|Control_Speed2_Pin
                          |Control_Speed3_Pin|Control_Motor_ON_OFF_Pin|Control_FM_Pin|State_LED7_Pin
                          |State_LED8_Pin|USART3_485_RE_DE_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : S_IN_EX_Pin S_Add_Water_Pin S_Stop_Add_Pin S_Drainage_Water_Pin
                           S_Stop_Drainage_Pin S_Simulation_Pin S_Weight__Pin S_Weight_C9_Pin
                           S_Auto_Hand_Pin S_Mode_Pin S_Speed_Pin */
  GPIO_InitStruct.Pin = S_IN_EX_Pin|S_Add_Water_Pin|S_Stop_Add_Pin|S_Drainage_Water_Pin
                          |S_Stop_Drainage_Pin|S_Simulation_Pin|S_Weight__Pin|S_Weight_C9_Pin
                          |S_Auto_Hand_Pin|S_Mode_Pin|S_Speed_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : State_LED1_Pin State_LED2_Pin State_LED3_Pin State_LED4_Pin
                           State_LED5_Pin */
  GPIO_InitStruct.Pin = State_LED1_Pin|State_LED2_Pin|State_LED3_Pin|State_LED4_Pin
                          |State_LED5_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : Display_SA_Pin Display_SB_Pin Display_SC_Pin Display_SD_Pin
                           Display_SE_Pin Display_SF_Pin Display_SG_Pin Display_SDP_Pin
                           Display_238D_A0_Pin Display_238D_A1_Pin Display_238D_A2_Pin Display_238D_EN_Pin
                           State_LED6_Pin Low_Alarm_Pin */
  GPIO_InitStruct.Pin = Display_SA_Pin|Display_SB_Pin|Display_SC_Pin|Display_SD_Pin
                          |Display_SE_Pin|Display_SF_Pin|Display_SG_Pin|Display_SDP_Pin
                          |Display_238D_A0_Pin|Display_238D_A1_Pin|Display_238D_A2_Pin|Display_238D_EN_Pin
                          |State_LED6_Pin|Low_Alarm_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : Control_Supply_Pin Control_Drainage_Pin Control_Speed1_Pin Control_Speed2_Pin
                           Control_Speed3_Pin Control_Motor_ON_OFF_Pin Control_FM_Pin State_LED7_Pin
                           State_LED8_Pin USART3_485_RE_DE_Pin */
  GPIO_InitStruct.Pin = Control_Supply_Pin|Control_Drainage_Pin|Control_Speed1_Pin|Control_Speed2_Pin
                          |Control_Speed3_Pin|Control_Motor_ON_OFF_Pin|Control_FM_Pin|State_LED7_Pin
                          |State_LED8_Pin|USART3_485_RE_DE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : SPI2_DRDY_Pin */
  GPIO_InitStruct.Pin = SPI2_DRDY_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(SPI2_DRDY_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 6, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */
