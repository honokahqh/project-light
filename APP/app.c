#include "app.h"

static struct pt pt100ms;

static int Task_100ms_process(struct pt *pt);

uint32_t timeoutCount;
extern uint16_t adc_data;

void System_Run() {
    PT_INIT(&pt100ms);
    while (1) {
        Task_100ms_process(&pt100ms);
    }
}

/**
 * Task_100ms_process
 * @brief 100ms周期处理
 * @author Honokahqh
 * @date 2024-1-10
 */
static void enableKey_process(); // 开关按键处理
static void modeKey_process();   // 模式按键处理
static void upDownkey_process(); // 普通模式下上下键处理
static void Bat_process();       // 充电状态处理

DevMode_t devMode = Normal;
lightMode_t lightMode = regulate_Temperatue;
#define ADC_TEST 0
int key1_count, key2_count, isCharge, devEnable, subMode = 1, lightness = 50, lightTemperature = WARM_TEMP;
static int Task_100ms_process(struct pt *pt) {
    static uint32_t cnt_100ms = 0;
    PT_BEGIN(pt);
    inputNumber(lightTemperature % 10000);
    setTemperature(lightTemperature, lightness);
    setMode(devMode);
    Bat_process();
    FAN_ENABLE();
    while (1) {
        enableKey_process();
        if (devEnable) {
            modeKey_process();
            upDownkey_process();
            if (cnt_100ms % 6 == 0) {
                Bat_process();
            }
        }
#if ADC_TEST
        inputNumber(adc_data);
#endif
        if (cnt_100ms % 10 == 0) {
            LL_IWDG_ReloadCounter(IWDG);
            ADC_start();
        }
        cnt_100ms++;
        PT_TIMER_DELAY(pt, 100);
    }
    PT_END(pt);
}

void enableKey_process() {
    if (!LL_GPIO_IsInputPinSet(KEY_1_PORT, KEY_1_PIN)) {
        key1_count++;
        if (key1_count > 30) {
            devEnable = ~devEnable;
            setPA14_PA15_Mode(devEnable);
            key1_count = 0;
            if (devEnable) {
                lightMode = regulate_Temperatue;
                inputNumber(lightTemperature % 10000);
                setTemperature(lightTemperature, lightness);
                devMode = Normal;
                setMode(devMode);
                FAN_ENABLE();
            } else {
                closeSegment();
                setLightOff();
                FAN_DISABLE();
            }
        }
    } else {
        key1_count = 0;
    }
}

void modeKey_process() {
    if (!LL_GPIO_IsInputPinSet(KEY_2_PORT, KEY_2_PIN)) {
        key2_count++;
        if (key2_count > 20) {
            (devMode == Normal) ? (devMode = Special) : (devMode = Normal);
            setMode(devMode);
            (devMode == Special) ? inputNumber(subMode) : inputNumber(lightTemperature % 10000);
            key2_count = -100;
        }
    } else {
        if (key2_count > 0 && key2_count < 5 && devMode == Normal) {
            lightMode = !lightMode;
            if (lightMode == regulate_lightness) {
                inputNumber(lightness);
            } else {
                inputNumber(lightTemperature % 10000);
            }
            key2_count = -30; // 设置为两秒的等待时间
        } else if (key2_count < 0) {
            key2_count += 5; // 每次调用增加5
            if (key2_count > -5)
                key2_count = 0; // 重置为0以便再次启用按键
        } else
            key2_count = 0;
    }
}

void upDownkey_process() {
    if (devMode == Special) {
        if (!LL_GPIO_IsInputPinSet(KEY_3_PORT, KEY_3_PIN)) {
            if (subMode < 3) {
                subMode++;
                inputNumber(subMode);
            }
        }
        if (!LL_GPIO_IsInputPinSet(KEY_4_PORT, KEY_4_PIN)) {
            if (subMode > 1) {
                subMode--;
                inputNumber(subMode);
            }
        }
    } else {
        if (!LL_GPIO_IsInputPinSet(KEY_3_PORT, KEY_3_PIN)) {
#if ADC_TEST
            if (lightness < 100) {
                lightness += 5;
                inputNumber(lightness);
            }
#else
            if (lightMode == regulate_lightness) {
                if (lightness < 100) {
                    lightness += 5;
                    inputNumber(lightness);
                }
            } else {
                if (lightTemperature < COLD_TEMP) {
                    lightTemperature += 50;
                    inputNumber(lightTemperature % 10000);
                }
            }
#endif

            setTemperature(lightTemperature, lightness);
        }
        if (!LL_GPIO_IsInputPinSet(KEY_4_PORT, KEY_4_PIN)) {
#if ADC_TEST
            if (lightness > 5) {
                lightness -= 5;
                inputNumber(lightness);
            }
#else
            if (lightMode == regulate_lightness) {
                if (lightness > 5) {
                    lightness -= 5;
                    inputNumber(lightness);
                }
            } else {
                if (lightTemperature > WARM_TEMP) {
                    lightTemperature -= 50;
                    inputNumber(lightTemperature % 10000);
                }
            }
#endif
            setTemperature(lightTemperature, lightness);
        }
    }
}

#define Bat_ADC_MAX 4000
#define Bat_ADC_MIN 2000

#define BatLevel1_Normal 3000
#define BatLevel2_Normal 3100
#define BatLevel3_Normal 3250
// 1A充电电流下,adc会上浮45
#define BatLevel1_Charging (BatLevel1_Normal + 60)
#define BatLevel2_Charging (BatLevel2_Normal + 30)
#define BatLevel3_Charging (BatLevel3_Normal + 10)
#define LightnessFloat 70 // 100%亮度下,adc会下浮70
void Bat_process() {
    static int batLevel_temp, batLevel_real;
    if (adc_data > Bat_ADC_MAX || adc_data < Bat_ADC_MIN)
        return;
    if (IsCharging())
        isCharge = 1;
    else
        isCharge = 0;
    if (devMode == true) {
        adc_data -= LightnessFloat * (float)lightness / 100;
    }
    if (isCharge) {
        if (adc_data < BatLevel1_Charging) {
            batLevel_real = 0;
        } else if (adc_data < BatLevel2_Charging) {
            batLevel_real = 1;
        } else if (adc_data < BatLevel3_Charging) {
            batLevel_real = 2;
        } else {
            batLevel_real = 3;
        }
    } else {
        if (adc_data < BatLevel1_Normal) {
            batLevel_real = 0;
        } else if (adc_data < BatLevel2_Normal) {
            batLevel_real = 1;
        } else if (adc_data < BatLevel3_Normal) {
            batLevel_real = 2;
        } else {
            batLevel_real = 3;
        }
    }

    if (isCharge) { // 充电动画
        setBatteryLevel(batLevel_temp);
        batLevel_temp++;
        if (batLevel_temp > 3)
            batLevel_temp = batLevel_real;
    } else {
        setBatteryLevel(batLevel_real);
    }
    adc_data = 0;
}