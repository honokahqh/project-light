#include "bsp_driver.h"

/**
 * gpio_init
 * @brief M1卡-四线SPI + RST ID卡-DinPin + PWM + RST RF433-DinPin + Enable
 * @author Honokahqh
 * @date 2024-1-10
 */
void gpio_init() {
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB);
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOF);

    LL_GPIO_SetPinSpeed(SMG_1_PORT, SMG_1_PIN, LL_GPIO_SPEED_FREQ_VERY_HIGH);
    LL_GPIO_SetPinOutputType(SMG_1_PORT, SMG_1_PIN, LL_GPIO_OUTPUT_PUSHPULL);
    LL_GPIO_SetPinSpeed(SMG_2_PORT, SMG_2_PIN, LL_GPIO_SPEED_FREQ_VERY_HIGH);
    LL_GPIO_SetPinOutputType(SMG_2_PORT, SMG_2_PIN, LL_GPIO_OUTPUT_PUSHPULL);
    LL_GPIO_SetPinSpeed(SMG_3_PORT, SMG_3_PIN, LL_GPIO_SPEED_FREQ_VERY_HIGH);
    LL_GPIO_SetPinOutputType(SMG_3_PORT, SMG_3_PIN, LL_GPIO_OUTPUT_PUSHPULL);
    LL_GPIO_SetPinSpeed(SMG_4_PORT, SMG_4_PIN, LL_GPIO_SPEED_FREQ_VERY_HIGH);
    LL_GPIO_SetPinOutputType(SMG_4_PORT, SMG_4_PIN, LL_GPIO_OUTPUT_PUSHPULL);
    LL_GPIO_SetPinSpeed(SMG_5_PORT, SMG_5_PIN, LL_GPIO_SPEED_FREQ_VERY_HIGH);
    LL_GPIO_SetPinOutputType(SMG_5_PORT, SMG_5_PIN, LL_GPIO_OUTPUT_PUSHPULL);
    LL_GPIO_SetPinSpeed(SMG_6_PORT, SMG_6_PIN, LL_GPIO_SPEED_FREQ_VERY_HIGH);
    LL_GPIO_SetPinOutputType(SMG_6_PORT, SMG_6_PIN, LL_GPIO_OUTPUT_PUSHPULL);
    LL_GPIO_SetPinSpeed(SMG_7_PORT, SMG_7_PIN, LL_GPIO_SPEED_FREQ_VERY_HIGH);
    LL_GPIO_SetPinOutputType(SMG_7_PORT, SMG_7_PIN, LL_GPIO_OUTPUT_PUSHPULL);

    LL_GPIO_SetPinPull(KEY_1_PORT, KEY_1_PIN, LL_GPIO_PULL_UP);
    LL_GPIO_SetPinMode(KEY_1_PORT, KEY_1_PIN, LL_GPIO_MODE_INPUT);
    LL_GPIO_SetPinPull(KEY_2_PORT, KEY_2_PIN, LL_GPIO_PULL_UP);
    LL_GPIO_SetPinMode(KEY_2_PORT, KEY_2_PIN, LL_GPIO_MODE_INPUT);

    LL_GPIO_SetPinMode(FAN_PORT, FAN_PIN, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinSpeed(FAN_PORT, FAN_PIN, LL_GPIO_SPEED_FREQ_VERY_HIGH);
    LL_GPIO_SetPinOutputType(FAN_PORT, FAN_PIN, LL_GPIO_OUTPUT_PUSHPULL);
    FAN_DISABLE();

    LL_GPIO_SetPinMode(Charge_PORT, Charge_PIN, LL_GPIO_MODE_INPUT);
}

void setPA14_PA15_Mode(uint8_t mode) {
    // 根据mode设置PA14和PA15
    if (mode == 1) {
        // 设置为输入模式
        LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_14, LL_GPIO_MODE_INPUT);
        LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_15, LL_GPIO_MODE_INPUT);
        LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_14, LL_GPIO_PULL_UP); // 无上拉无下拉
        LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_15, LL_GPIO_PULL_UP);
    } else {
        // 设置为SW模式（假设默认复用功能号为0，一般为SWD）
        LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_14, LL_GPIO_MODE_ALTERNATE);
        LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_15, LL_GPIO_MODE_ALTERNATE);
        LL_GPIO_SetAFPin_8_15(GPIOA, LL_GPIO_PIN_14, LL_GPIO_AF0_SWJ);
        LL_GPIO_SetAFPin_8_15(GPIOA, LL_GPIO_PIN_15, LL_GPIO_AF0_SWJ);
    }
}

/**
 * timer1_init
 * @brief timer1输出ID卡所需125kHzPWM,timer3用于ID卡-RF433所需的计时
 * @author Honokahqh
 * @date 2024-1-10
 */
#define PWM_ARR 1000
#define PWM_PSC 0
#define PWM_FREQ 24000000 / PWM_ARR / PWM_PSC
volatile uint32_t sys_ms, sys_50us_id;
volatile uint32_t sys_50us_433;
void timer1_init() {
    LL_APB1_GRP2_EnableClock(RCC_APBENR2_TIM1EN);
    LL_APB1_GRP1_EnableClock(RCC_APBENR1_TIM3EN);
    LL_APB1_GRP2_EnableClock(RCC_APBENR2_TIM14EN);

    LL_GPIO_InitTypeDef TIM1CH1MapInit = {0};
    TIM1CH1MapInit.Pin = LL_GPIO_PIN_3;
    TIM1CH1MapInit.Mode = LL_GPIO_MODE_ALTERNATE;
    TIM1CH1MapInit.Alternate = LL_GPIO_AF_13;
    LL_GPIO_Init(GPIOA, &TIM1CH1MapInit);

    LL_GPIO_InitTypeDef TIM14CH1MapInit = {0};
    TIM14CH1MapInit.Pin = LL_GPIO_PIN_4;
    TIM14CH1MapInit.Mode = LL_GPIO_MODE_ALTERNATE;
    TIM14CH1MapInit.Alternate = LL_GPIO_AF_4;
    LL_GPIO_Init(GPIOA, &TIM14CH1MapInit);

    /*配置TIM1*/
    LL_TIM_InitTypeDef TIM1CountInit = {0};

    /***********************************************
    ** 输入时钟：    8000000
    ** 计数模式：    向上计数
    ** 时钟预分频：  8000
    ** 自动重装载值：500
    ** 重复计数值：  0
    ************************************************/
    TIM1CountInit.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
    TIM1CountInit.CounterMode = LL_TIM_COUNTERMODE_UP;
    TIM1CountInit.Prescaler = PWM_PSC;
    TIM1CountInit.Autoreload = PWM_ARR;
    TIM1CountInit.RepetitionCounter = 0;

    LL_TIM_OC_InitTypeDef TIM_OC_InitStruct;
    LL_TIM_OC_StructInit(&TIM_OC_InitStruct);
    TIM_OC_InitStruct.OCMode = LL_TIM_OCMODE_PWM1;
    TIM_OC_InitStruct.OCPolarity = LL_TIM_OCPOLARITY_HIGH;
    TIM_OC_InitStruct.OCNPolarity = LL_TIM_OCPOLARITY_HIGH;
    TIM_OC_InitStruct.OCIdleState = LL_TIM_OCIDLESTATE_LOW;
    TIM_OC_InitStruct.OCNIdleState = LL_TIM_OCIDLESTATE_LOW;
    TIM_OC_InitStruct.CompareValue = 0;

    LL_TIM_OC_Init(TIM1, LL_TIM_CHANNEL_CH1, &TIM_OC_InitStruct);
    /*初始化TIM1*/
    LL_TIM_Init(TIM1, &TIM1CountInit);
    TIM1->BDTR = 0x8000;  // 输出使能
    TIM1->CCMR1 = 0x0068; // PWM1模式 开启CCR1预装载

    /*使能TIM1计数器*/
    LL_TIM_EnableCounter(TIM1);
    TIM1->CCER = 1; // 启动pwm输出

    /*使能TIM14计数器*/
    LL_TIM_InitTypeDef TIM14CountInit = {0};
    TIM14CountInit.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
    TIM14CountInit.CounterMode = LL_TIM_COUNTERMODE_UP;
    TIM14CountInit.Prescaler = PWM_PSC;
    TIM14CountInit.Autoreload = PWM_ARR;
    TIM14CountInit.RepetitionCounter = 0;

    LL_TIM_OC_InitTypeDef TIM3_OC_InitStruct;
    LL_TIM_OC_StructInit(&TIM3_OC_InitStruct);
    TIM3_OC_InitStruct.OCMode = LL_TIM_OCMODE_PWM1;
    TIM3_OC_InitStruct.OCPolarity = LL_TIM_OCPOLARITY_HIGH;
    TIM3_OC_InitStruct.OCNPolarity = LL_TIM_OCPOLARITY_HIGH;
    TIM3_OC_InitStruct.OCIdleState = LL_TIM_OCIDLESTATE_LOW;
    TIM3_OC_InitStruct.OCNIdleState = LL_TIM_OCIDLESTATE_LOW;
    TIM3_OC_InitStruct.CompareValue = 0;

    LL_TIM_OC_Init(TIM14, LL_TIM_CHANNEL_CH1, &TIM3_OC_InitStruct);

    LL_TIM_Init(TIM14, &TIM14CountInit);
    TIM14->BDTR = 0x8000;  // 输出使能
    TIM14->CCMR1 = 0x0068; // PWM1模式 开启CCR1预装载
    LL_TIM_EnableCounter(TIM14);
    TIM14->CCER = 1; // 启动pwm输出

    // LL_TIM_EnableCounter(TIM1);
    // TIM1->CCER = 1; // 启动pwm输出

    // LL_APB1_GRP2_EnableClock(RCC_APBENR2_TIM14EN);

    // LL_TIM_OC_Init(TIM14, LL_TIM_CHANNEL_CH1, &TIM_OC_InitStruct);
    // /*初始化TIM1*/
    // LL_TIM_Init(TIM14, &TIM1CountInit);
    // TIM14->BDTR = 0x8000;  // 输出使能
    // TIM14->CCMR1 = 0x0068; // PWM1模式 开启CCR1预装载

    // /*使能TIM1计数器*/
    // LL_TIM_EnableCounter(TIM14);
    // TIM14->CCER = 1; // 启动pwm输出

    TIM1CountInit.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
    TIM1CountInit.CounterMode = LL_TIM_COUNTERMODE_UP;
    TIM1CountInit.Prescaler = 80 - 1;
    TIM1CountInit.Autoreload = 100 - 1;
    TIM1CountInit.RepetitionCounter = 0;

    /*初始化TIM3*/
    LL_TIM_Init(TIM3, &TIM1CountInit);

    /*使能UPDATE中断*/
    LL_TIM_EnableIT_UPDATE(TIM3);

    /*使能TIM1计数器*/
    LL_TIM_EnableCounter(TIM3);

    /*开启UPDATE中断请求*/
    NVIC_EnableIRQ(TIM3_IRQn);
    NVIC_SetPriority(TIM3_IRQn, 0);
}

void TIM3_IRQHandler() {
    LL_TIM_ClearFlag_UPDATE(TIM3);
    controlSegment();
}

/**
 * IWDG_Config
 * @brief 看门狗初始化
 * @author Honokahqh
 * @date 2023-12-16
 */
void IWDG_Config(void) {
    /* 使能LSI */
    LL_RCC_LSI_Enable();
    while (LL_RCC_LSI_IsReady() == 0U) {
        ;
    }

    /* 使能IWDG */
    LL_IWDG_Enable(IWDG);
    /* 开启写权限 */
    LL_IWDG_EnableWriteAccess(IWDG);
    /* 设置IWDG分频 */
    LL_IWDG_SetPrescaler(IWDG, LL_IWDG_PRESCALER_32); // T=1MS
    /* 设置喂狗事件*/
    LL_IWDG_SetReloadCounter(IWDG, 3000); // 1ms*1000=1s
    /* IWDG初始化*/
    while (LL_IWDG_IsReady(IWDG) == 0U) {
        ;
    }
    /*喂狗*/
    LL_IWDG_ReloadCounter(IWDG);
}

/**
 * ADC_init
 * @brief ADC初始化,用于温度采集
 * @author Honokahqh
 * @date 2023-12-16
 */
/* Private define ------------------------------------------------------------*/
#define ADC_DELAY_CALIB_ENABLE_CPU_CYCLES (LL_ADC_DELAY_CALIB_ENABLE_ADC_CYCLES * 32)
#define VAR_CONVERTED_DATA_INIT_VALUE (__LL_ADC_DIGITAL_SCALE(LL_ADC_RESOLUTION_12B) + 1)

void ADC_init() {
    LL_ADC_Reset(ADC1);
    /* 使能GPIOA时钟 */
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);

    /* 配置管脚PA4为模拟输入 */
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_0, LL_GPIO_MODE_ANALOG);

    /* 使能ADC1时钟 */
    LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_ADC1);

    if (__LL_ADC_IS_ENABLED_ALL_COMMON_INSTANCE() == 0) {
        /* 配置内部转换通道 */
        LL_ADC_SetCommonPathInternalCh(__LL_ADC_COMMON_INSTANCE(ADC1), LL_ADC_PATH_INTERNAL_NONE);
    }

    /* 设置ADC时钟 */
    LL_ADC_SetClock(ADC1, LL_ADC_CLOCK_SYNC_PCLK_DIV2);

    /* 设置12位分辨率 */
    LL_ADC_SetResolution(ADC1, LL_ADC_RESOLUTION_12B);

    /* 设置数据右对齐 */
    LL_ADC_SetResolution(ADC1, LL_ADC_DATA_ALIGN_RIGHT);

    /* 设置低功耗模式无 */
    LL_ADC_SetLowPowerMode(ADC1, LL_ADC_LP_MODE_NONE);

    /* 设置通道转换时间 */
    LL_ADC_SetSamplingTimeCommonChannels(ADC1, LL_ADC_SAMPLINGTIME_239CYCLES_5);

    /* 设置触发源为TIM1 TRGO */
    LL_ADC_REG_SetTriggerSource(ADC1, LL_ADC_REG_TRIG_SOFTWARE);

    /* 设置转换模式为单次转换 */
    LL_ADC_REG_SetContinuousMode(ADC1, LL_ADC_REG_CONV_SINGLE);

    /* 设置DMA模式为不开启 */
    LL_ADC_REG_SetDMATransfer(ADC1, LL_ADC_REG_DMA_TRANSFER_NONE);

    /* 设置过载管理模式为覆盖上一个值 */
    LL_ADC_REG_SetOverrun(ADC1, LL_ADC_REG_OVR_DATA_OVERWRITTEN);

    /* 设置不连续模式为不使能 */
    LL_ADC_REG_SetSequencerDiscont(ADC1, LL_ADC_REG_SEQ_DISCONT_DISABLE);

    /* 设置通道4为转换通道 */
    LL_ADC_REG_SetSequencerChannels(ADC1, LL_ADC_CHANNEL_0);

    /* 使能EOC中断 */
    LL_ADC_EnableIT_EOC(ADC1);

    NVIC_SetPriority(ADC_COMP_IRQn, 0);
    NVIC_EnableIRQ(ADC_COMP_IRQn);
}

/**
 * ADC_start
 * @brief 采样
 * @author Honokahqh
 * @date 2023-12-16
 */
void ADC_start(void) {
    __IO uint32_t wait_loop_index = 0;
#if (USE_TIMEOUT == 1)
    uint32_t Timeout = 0;
#endif

    if (LL_ADC_IsEnabled(ADC1) == 0) {
        /* 使能校准 */
        LL_ADC_StartCalibration(ADC1);

#if (USE_TIMEOUT == 1)
        Timeout = ADC_CALIBRATION_TIMEOUT_MS;
#endif

        while (LL_ADC_IsCalibrationOnGoing(ADC1) != 0) {
#if (USE_TIMEOUT == 1)
            /* 检测校准是否超时 */
            if (LL_SYSTICK_IsActiveCounterFlag()) {
                if (Timeout-- == 0) {
                }
            }
#endif
        }

        /* ADC校准结束和使能ADC之间的延时 */
        wait_loop_index = (ADC_DELAY_CALIB_ENABLE_CPU_CYCLES >> 1);
        while (wait_loop_index != 0) {
            wait_loop_index--;
        }

        /* 使能ADC */
        LL_ADC_Enable(ADC1);
    }
    LL_ADC_REG_StartConversion(ADC1);
}

volatile uint16_t uhADCxConvertedData = VAR_CONVERTED_DATA_INIT_VALUE;
volatile uint16_t uhADCxConvertedData_Voltage_mVolt = 0;
uint16_t adc_data;
/**
 * APP_AdcGrpRegularUnitaryConvCompleteCallback
 * @brief ADC callback
 * @author Honokahqh
 * @date 2023-12-16
 */
void APP_AdcGrpRegularUnitaryConvCompleteCallback() { adc_data = LL_ADC_REG_ReadConversionData12(ADC1); }

/**
 * ADC_COMP_IRQHandler
 * @brief ADC 中断
 * @author Honokahqh
 * @date 2023-12-16
 */
void ADC_COMP_IRQHandler(void) {
    /* 检测是不是转换结束触发的中断 */
    if (LL_ADC_IsActiveFlag_EOC(ADC1) != 0) {
        /* 清空ADC EOC 中断 */
        LL_ADC_ClearFlag_EOC(ADC1);

        /* 调用中断处理函数 */
        APP_AdcGrpRegularUnitaryConvCompleteCallback();
    }
}
#define PWM_Max 550
#define PWM_MIN 20
#define PWM_RANGE (PWM_Max - PWM_MIN)
// 假设的PWM调节函数
void setWarmLightDutyCycle(int dutyCycle);
void setColdLightDutyCycle(int dutyCycle);

// 色温调节函数
static int targetTemperature_last = 0, lightness_last = 50;
void setTemperature(int targetTemperature, int lightness) {
    if (targetTemperature < WARM_TEMP) {
        targetTemperature = WARM_TEMP;
    } else if (targetTemperature > COLD_TEMP) {
        targetTemperature = COLD_TEMP;
    }

    // 计算占空比
    float warmDutyCycle = ((float)(COLD_TEMP - targetTemperature)) / (COLD_TEMP - WARM_TEMP);
    float coldDutyCycle = ((float)(targetTemperature - WARM_TEMP)) / (COLD_TEMP - WARM_TEMP);

    float duty = warmDutyCycle * PWM_RANGE * ((float)lightness / 100);
    setWarmLightDutyCycle((int)duty);
    duty = coldDutyCycle * PWM_ARR * ((float)lightness / 100);
    setColdLightDutyCycle((int)duty);

    targetTemperature_last = targetTemperature;
    lightness_last = lightness;
}

void setTemperature_noRecord(int targetTemperature, int lightness) {
    if (targetTemperature < WARM_TEMP) {
        targetTemperature = WARM_TEMP;
    } else if (targetTemperature > COLD_TEMP) {
        targetTemperature = COLD_TEMP;
    }

    // 计算占空比
    float warmDutyCycle = ((float)(COLD_TEMP - targetTemperature)) / (COLD_TEMP - WARM_TEMP);
    float coldDutyCycle = ((float)(targetTemperature - WARM_TEMP)) / (COLD_TEMP - WARM_TEMP);

    float duty = warmDutyCycle * PWM_RANGE * ((float)lightness / 100);
    setWarmLightDutyCycle((int)duty);
    duty = coldDutyCycle * PWM_ARR * ((float)lightness / 100);
    setColdLightDutyCycle((int)duty);
    ;
}

// 示例设置占空比的函数
void setWarmLightDutyCycle(int dutyCycle) {
    if (dutyCycle > PWM_Max) {
        TIM1->CCR1 = PWM_Max;
    } else if (dutyCycle < PWM_MIN) {
        TIM1->CCR1 = 0;
    } else {
        TIM1->CCR1 = dutyCycle;
    }
}

void setColdLightDutyCycle(int dutyCycle) {
    if (dutyCycle > PWM_Max) {
        TIM14->CCR1 = PWM_Max;
    } else if (dutyCycle < PWM_MIN) {
        TIM14->CCR1 = 0;
    } else {
        TIM14->CCR1 = dutyCycle;
    }
}

void setLightOff() {
    TIM1->CCR1 = 0;
    TIM14->CCR1 = 0;
}

extern int devEnable, subMode;
extern DevMode_t devMode;
typedef enum { Down = 0, Up = 1 } direction_t;
void lightModePeriodProcess() {
    static int count_1ms = 0;
    static int percent = 100, subMode_last = 0, temper, lightness, temper_last = 0;
    static direction_t direction = Down;
    if (!devMode || !devEnable)
        return;
    count_1ms++;
    if (subMode_last != subMode) {
        subMode_last = subMode;
        temper = targetTemperature_last;
        temper_last = targetTemperature_last;
        lightness = lightness_last;
    }
    switch (subMode) {
    case 1: // 亮度渐变
        if (count_1ms % 30 == 0) {
            if (direction == Down) {
                percent -= 1;
                if (percent <= 0) {
                    direction = Up;
                    percent = 0;
                }
            } else {
                percent += 1;
                if (percent >= 100) {
                    direction = Down;
                    percent = 100;
                }
            }
            setTemperature_noRecord(temper, percent);
        }
        break;
    case 2: // 闪烁
        if (count_1ms % 400 == 200) {
            setTemperature_noRecord(temper, lightness);
        } else if (count_1ms % 400 == 0) {
            setLightOff();
        }
        /* code */
        break;
    case 3: // 色温渐变
        if (count_1ms % 30 == 0) {
            if (direction == Down) {
                temper_last -= 50;
                if (temper_last <= WARM_TEMP) {
                    direction = Up;
                    temper_last = WARM_TEMP;
                }
            } else {
                temper_last += 50;
                if (temper_last >= COLD_TEMP) {
                    direction = Down;
                    temper_last = COLD_TEMP;
                }
            }
            setTemperature_noRecord(temper_last, lightness);
        }
        break;
    default:
        break;
    }
}