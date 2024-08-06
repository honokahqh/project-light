/**
 ******************************************************************************
 * @file           : main.h
 * @brief          : Header for main.c file.
 *                   This file contains the common defines of the application.
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
/* Includes ------------------------------------------------------------------*/
#include "xl32f0xx_ll_rcc.h"
#include "xl32f0xx_ll_bus.h"
#include "xl32f0xx_ll_system.h"
#include "xl32f0xx_ll_exti.h"
#include "xl32f0xx_ll_cortex.h"
#include "xl32f0xx_ll_utils.h"
#include "xl32f0xx_ll_pwr.h"
#include "xl32f0xx_ll_dma.h"
#include "xl32f0xx_ll_tim.h"
#include "xl32f0xx_ll_adc.h"
#include "xl32f0xx_ll_gpio.h"
#include "xl32f0xx_ll_usart.h"
#include "xl32f0xx_ll_iwdg.h"

#include "bsp_driver.h"
#include "xl32f003xx_ll_Start_Kit.h"

#include "pt.h"
#include "app.h"
#if defined(USE_FULL_ASSERT)
#include "xl32_assert.h"
#endif /* USE_FULL_ASSERT */

  /* Private includes ----------------------------------------------------------*/
  /* Exported types ------------------------------------------------------------*/
  /* Exported constants --------------------------------------------------------*/
  /* Exported macro ------------------------------------------------------------*/
  /* Exported functions prototypes ---------------------------------------------*/
  void Error_Handler(void);
  void APP_UsartIRQCallback(USART_TypeDef *USARTx);
  void APP_DmaChannel1IRQCallback(void);
  void APP_DmaChannel2_3_IRQCallback(void);

/* Private defines -----------------------------------------------------------*/

void m1_init(void);
void RFID_Read(void);
char PCD_DP1322ES_TypeA_GetUID(void);
#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT Xinling *****END OF FILE****/
