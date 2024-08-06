#ifndef __BSP_DRIVER_H
#define __BSP_DRIVER_H

#include "app.h"
#include "xl32f003xx_ll_Start_Kit.h"

#define KEY_1_PORT GPIOF
#define KEY_1_PIN LL_GPIO_PIN_0

#define KEY_2_PORT GPIOB
#define KEY_2_PIN LL_GPIO_PIN_3

#define KEY_3_PORT GPIOA
#define KEY_3_PIN LL_GPIO_PIN_14

#define KEY_4_PORT GPIOA
#define KEY_4_PIN LL_GPIO_PIN_13

#define FAN_PORT GPIOF
#define FAN_PIN LL_GPIO_PIN_1
#define FAN_ENABLE() LL_GPIO_SetOutputPin(FAN_PORT, FAN_PIN)
#define FAN_DISABLE() LL_GPIO_ResetOutputPin(FAN_PORT, FAN_PIN)

#define Charge_PORT GPIOA
#define Charge_PIN LL_GPIO_PIN_1
#define IsCharging() LL_GPIO_IsInputPinSet(Charge_PORT, Charge_PIN)

#define PWM_WARM_PORT GPIOA
#define PWM_WARM_PIN LL_GPIO_PIN_3
#define PWM_COLD_PORT GPIOA
#define PWM_COLD_PIN LL_GPIO_PIN_4

// 定义色温范围
#define WARM_TEMP 3200
#define COLD_TEMP 6500

void gpio_init(void);
void setPA14_PA15_Mode(uint8_t mode);

// timer1 1ms定时
extern volatile uint32_t sys_ms;
void timer1_init(void);
void setTemperature(int targetTemperature, int lightness);
void setLightOff();
void lightModePeriodProcess();
// 看门狗
void IWDG_Config(void);

// adc
void ADC_init(void);
void ADC_start(void);

extern volatile uint16_t uhADCxConvertedData_Voltage_mVolt;

#endif //
