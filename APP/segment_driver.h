#ifndef __SEGMENT_DRIVER_H
#define __SEGMENT_DRIVER_H

#include "app.h"

#define SMG_1_PORT GPIOA
#define SMG_1_PIN LL_GPIO_PIN_5
#define SMG_2_PORT GPIOA
#define SMG_2_PIN LL_GPIO_PIN_6
#define SMG_3_PORT GPIOA
#define SMG_3_PIN LL_GPIO_PIN_7
#define SMG_4_PORT GPIOB
#define SMG_4_PIN LL_GPIO_PIN_1
#define SMG_5_PORT GPIOB
#define SMG_5_PIN LL_GPIO_PIN_2
#define SMG_6_PORT GPIOA
#define SMG_6_PIN LL_GPIO_PIN_12
#define SMG_7_PORT GPIOA
#define SMG_7_PIN LL_GPIO_PIN_2


void controlSegment();
void setSegmentOff();
void inputNumber(int number);
void setMode(uint8_t mode);
void setBatteryLevel(uint8_t level);
void closeSegment();
#endif