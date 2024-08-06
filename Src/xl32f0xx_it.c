/**
 ******************************************************************************
 * @file    xl32f0xx_it.c
 * @author  MCU Application Team
 * @Version V1.0.0
 * @Date    2020-10-19
 * @brief   Interrupt Service Routines.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "xl32f0xx_it.h"
#include "app.h"

/* Private includes ----------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private user code ---------------------------------------------------------*/
/* External variables --------------------------------------------------------*/

/******************************************************************************/
/*           Cortex-M0+ Processor Interruption and Exception Handlers         */
/******************************************************************************/
/**
 * @brief This function handles Non maskable interrupt.
 */
void NMI_Handler(void) {}

/**
 * @brief This function handles Hard fault interrupt.
 */
void HardFault_Handler(void) {
    while (1) {
    }
}

/**
 * @brief This function handles System service call via SWI instruction.
 */
void SVC_Handler(void) {}

/**
 * @brief This function handles Pendable request for system service.
 */
void PendSV_Handler(void) {}

/**
 * @brief This function handles System tick timer.
 */
extern uint16_t buzz_buffer[10], buzz_index;
void SysTick_Handler(void) {
    sys_ms++;
    lightModePeriodProcess();
}

/******************************************************************************/
/* XL32F0xx Peripheral Interrupt Handlers                                     */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_xl32f003xx.s).                   */
/******************************************************************************/

/************************ (C) COPYRIGHT Xinling *****END OF FILE****/
