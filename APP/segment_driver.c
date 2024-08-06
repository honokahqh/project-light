#include "segment_driver.h"

typedef struct {
    GPIO_TypeDef *port;
    uint32_t pin;
    void (*setPin)(GPIO_TypeDef *port, uint32_t pin, uint8_t state);
} GPIOConfig;

void setGPIOState(GPIO_TypeDef *port, uint32_t pin, uint8_t state) {
    if (state) {
        LL_GPIO_SetOutputPin(port, pin); // 点亮管脚
    } else {
        LL_GPIO_ResetOutputPin(port, pin); // 熄灭管脚
    }
}

GPIOConfig gpioConfigs[7] = {{SMG_1_PORT, SMG_1_PIN, setGPIOState}, {SMG_2_PORT, SMG_2_PIN, setGPIOState},
                             {SMG_3_PORT, SMG_3_PIN, setGPIOState}, {SMG_4_PORT, SMG_4_PIN, setGPIOState},
                             {SMG_5_PORT, SMG_5_PIN, setGPIOState}, {SMG_6_PORT, SMG_6_PIN, setGPIOState},
                             {SMG_7_PORT, SMG_7_PIN, setGPIOState}};

typedef enum {
    IO1 = 0, // GPIOA, LL_GPIO_PIN_5
    IO2,     // GPIOB, LL_GPIO_PIN_1
    IO3,     // GPIOA, LL_GPIO_PIN_6
    IO4,     // GPIOB, LL_GPIO_PIN_2
    IO5,     // GPIOA, LL_GPIO_PIN_7
    IO6,     // GPIOA, LL_GPIO_PIN_12
    IO7      // GPIOA, LL_GPIO_PIN_2
} IOPin;

typedef struct {
    IOPin pin1; // 第一个IO引脚
    IOPin pin2; // 第二个IO引脚
    uint8_t enable;
    uint8_t needFlash;
} SegmentControl;
SegmentControl segmentControls[] = {{IO1, IO2, false}, {IO1, IO3, false}, {IO4, IO1, false}, {IO5, IO1, false},
                                    {IO1, IO4, false}, {IO2, IO1, false}, {IO3, IO1, false}, // smg1

                                    {IO2, IO3, false}, {IO2, IO4, false}, {IO5, IO2, false}, {IO2, IO6, false},
                                    {IO2, IO5, false}, {IO3, IO2, false}, {IO4, IO2, false}, // smg2

                                    {IO5, IO4, false}, {IO3, IO5, false}, {IO4, IO5, false}, {IO6, IO1, false},
                                    {IO6, IO3, false}, {IO4, IO3, false}, {IO5, IO3, false}, // smg3

                                    {IO7, IO6, false}, {IO6, IO7, false}, {IO5, IO6, false}, {IO6, IO4, false},
                                    {IO4, IO6, false}, {IO6, IO5, false}, {IO5, IO7, false}, // smg4

                                    {IO7, IO3, false}, {IO3, IO7, false}, {IO3, IO6, false}, {IO1, IO6, false},
                                    {IO1, IO7, false}, {IO2, IO7, false}, {IO4, IO7, false}, {IO7, IO1, false},
                                    {IO7, IO5, false}, {IO1, IO5, false}, {IO7, IO2, false}, {IO6, IO2, false}};
uint8_t indexMap[41] = {0};
uint8_t indexMapNum;
void setSegmentOff() {
    for (uint8_t i = 0; i < sizeof(gpioConfigs) / sizeof(gpioConfigs[0]); i++) {
        LL_GPIO_SetPinMode(gpioConfigs[i].port, gpioConfigs[i].pin, LL_GPIO_MODE_INPUT);
    }
}

void setSegmentOn(uint8_t segment) {
    static int last_segment = 0;
    uint8_t id = segmentControls[last_segment].pin1;
    LL_GPIO_SetPinMode(gpioConfigs[id].port, gpioConfigs[id].pin, LL_GPIO_MODE_INPUT);
    id = segmentControls[last_segment].pin2;
    LL_GPIO_SetPinMode(gpioConfigs[id].port, gpioConfigs[id].pin, LL_GPIO_MODE_INPUT);
    last_segment = segment;

    id = segmentControls[segment].pin1;
    LL_GPIO_SetPinMode(gpioConfigs[id].port, gpioConfigs[id].pin, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetOutputPin(gpioConfigs[id].port, gpioConfigs[id].pin);
    id = segmentControls[segment].pin2;
    LL_GPIO_SetPinMode(gpioConfigs[id].port, gpioConfigs[id].pin, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_ResetOutputPin(gpioConfigs[id].port, gpioConfigs[id].pin);
}

void controlSegment() {
    if (indexMapNum) {
        setSegmentOn(indexMap[indexMapNum - 1]);
        indexMapNum--;
    }
    if (indexMapNum == 0) {
        for (uint8_t i = 0; i < sizeof(segmentControls) / sizeof(segmentControls[0]); i++) {
            if (segmentControls[i].enable)
                indexMap[indexMapNum++] = i;
        }
    }
}

const uint8_t segmentValues[10] = {
    0x3F, // 0
    0x06, // 1
    0x5B, // 2
    0x4F, // 3
    0x66, // 4
    0x6D, // 5
    0x7D, // 6
    0x07, // 7
    0x7F, // 8
    0x6F  // 9
};

void inputNumber(int number) {
    if (number > 9999)
        return;
    // 循环处理每一位数字
    for (int digit = 3; digit >= 0; digit--) {
        int data = segmentValues[number % 10];
        int segmentBaseIndex = digit * 7;
        if (number > 0 || digit == 3) { // 如果number不为0或者是最后一位数字
            for (uint8_t i = 0; i < 7; i++) {
                segmentControls[segmentBaseIndex + i].enable = (data & (1 << i)) ? true : false;
            }
        } else {
            // 如果是前导0，则不显示
            for (uint8_t i = 0; i < 7; i++) {
                segmentControls[segmentBaseIndex + i].enable = false;
            }
        }
        number = number / 10;
    }
}

#define Index_Icon 27
// k1-k12 对应 28-39
// k1-k4 电池外观常亮 k5 电量3 k6 电量2 k7 电量1 k8 NULL k9 NULL k10 mode0 k11 mode1 k12 NULL
void setMode(uint8_t mode) {
    if (mode) {
        segmentControls[Index_Icon + 11].enable = true;
        segmentControls[Index_Icon + 10].enable = false;
    } else {
        segmentControls[Index_Icon + 11].enable = false;
        segmentControls[Index_Icon + 10].enable = true;
    }
}

void setBatteryLevel(uint8_t level) {
    segmentControls[Index_Icon + 1].enable = true;
    segmentControls[Index_Icon + 2].enable = true;
    segmentControls[Index_Icon + 3].enable = true;
    segmentControls[Index_Icon + 4].enable = true;
    switch (level) {
    case 0:
        segmentControls[Index_Icon + 5].enable = false;
        segmentControls[Index_Icon + 6].enable = false;
        segmentControls[Index_Icon + 7].enable = false;
        break;
    case 1:
        segmentControls[Index_Icon + 5].enable = false;
        segmentControls[Index_Icon + 6].enable = false;
        segmentControls[Index_Icon + 7].enable = true;
        break;
    case 2:
        segmentControls[Index_Icon + 5].enable = false;
        segmentControls[Index_Icon + 6].enable = true;
        segmentControls[Index_Icon + 7].enable = true;
        break;
    case 3:
        segmentControls[Index_Icon + 5].enable = true;
        segmentControls[Index_Icon + 6].enable = true;
        segmentControls[Index_Icon + 7].enable = true;
        break;
    default:
        segmentControls[Index_Icon + 5].enable = false;
        segmentControls[Index_Icon + 6].enable = false;
        segmentControls[Index_Icon + 7].enable = false;
        break;
    }
}

void closeSegment() {
    for (uint8_t i = 0; i < sizeof(segmentControls) / sizeof(segmentControls[0]); i++) {
        segmentControls[i].enable = false;
        segmentControls[i].needFlash = false;
        indexMapNum = 0;
        setSegmentOff();
    }
}