/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2023/12/29
 * Description        : Main program body.
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/*
 *@Note
 *task1 and task2 alternate printing
 */

#include "FreeRTOS.h"
#include "task.h"

/* Global Variable */
TaskHandle_t DefaultTask_Handler;
extern void app_main();

/*********************************************************************
 * @fn      default_task
 *
 * @brief   task1 program.
 *
 * @param  *pvParameters - Parameters point of task1
 *
 * @return  none
 */
void default_task(void*) { app_main(); }

/*********************************************************************
 * @fn      USBFS_RCC_Init
 *
 * @brief   Initializes the usbfs clock configuration.
 *
 * @return  none
 */
void USBFS_RCC_Init(void)
{
  RCC_ClocksTypeDef RCC_ClocksStatus = {0};
  RCC_GetClocksFreq(&RCC_ClocksStatus);
  if (RCC_ClocksStatus.SYSCLK_Frequency == 144000000)
  {
    RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_Div3);
  }
  else if (RCC_ClocksStatus.SYSCLK_Frequency == 96000000)
  {
    RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_Div2);
  }
  else if (RCC_ClocksStatus.SYSCLK_Frequency == 48000000)
  {
    RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_Div1);
  }
#if defined(CH32V20x_D8W) || defined(CH32V20x_D8)
  else if (RCC_ClocksStatus.SYSCLK_Frequency == 240000000 && RCC_USB5PRE_JUDGE() == SET)
  {
    RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_Div5);
  }
#endif
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_USBFS, ENABLE);
}

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  SystemCoreClockUpdate();

  USBFS_RCC_Init();

  xTaskCreate((TaskFunction_t)default_task, (const char*)"default_task", (uint16_t)1024,
              (void*)NULL, (UBaseType_t)5, (TaskHandle_t*)&DefaultTask_Handler);
  vTaskStartScheduler();

  while (1)
  {
  }
}
