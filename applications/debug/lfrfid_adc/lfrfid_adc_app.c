#include <furi.h>

#include <furi_hal_bus.h>
#include <furi_hal_cortex.h>
#include <furi_hal_interrupt.h>
#include <furi_hal_resources.h>

#include <stm32wbxx_ll_adc.h>
#include <stm32wbxx_ll_tim.h>
#include <stm32wbxx_ll_gpio.h>

#define TAG "LfRfidAdc"

#define DATA_INPUT_PIN (gpio_rfid_data_in)
#define DATA_OUTPUT_PIN (gpio_ext_pa7)

#define CARRIER_INPUT_PIN (gpio_rfid_carrier)
#define CARRIER_OUTPUT_PIN (gpio_rfid_carrier_out)

#define CARRIER_FREQ_HZ (125000UL)

#define VREFBUF_STARTUP_DELAY_US (500000UL)

typedef struct {
    uint8_t dummy;
} LfRfidAdcApp;

static LfRfidAdcApp* lfrfid_adc_app_alloc() {
    LfRfidAdcApp* app = malloc(sizeof(LfRfidAdcApp));
    return app;
};

static void lfrfid_adc_app_free(LfRfidAdcApp* app) {
    free(app);
}

static void lfrfid_adc_app_adc_init(LfRfidAdcApp* app) {
    UNUSED(app);

    FuriHalCortexTimer timer;

    furi_hal_gpio_init_simple(&DATA_INPUT_PIN, GpioModeAnalog);

    LL_VREFBUF_SetVoltageScaling(LL_VREFBUF_VOLTAGE_SCALE0);
    LL_VREFBUF_SetTrimming(LL_VREFBUF_SC0_GetCalibration() & 0x3FUL);

    LL_VREFBUF_Enable();
    LL_VREFBUF_DisableHIZ();

    timer = furi_hal_cortex_timer_get(VREFBUF_STARTUP_DELAY_US);

    while(!LL_VREFBUF_IsVREFReady()) {
        furi_check(!furi_hal_cortex_timer_is_expired(timer));
    }

    furi_hal_bus_enable(FuriHalBusADC);

    LL_ADC_CommonInitTypeDef common_init_struct = {0};
    common_init_struct.CommonClock = LL_ADC_CLOCK_SYNC_PCLK_DIV1;
    furi_check(LL_ADC_CommonInit(ADC1_COMMON, &common_init_struct) == SUCCESS);

    LL_ADC_InitTypeDef init_struct = {0};
    init_struct.Resolution = LL_ADC_RESOLUTION_12B;
    init_struct.DataAlignment = LL_ADC_DATA_ALIGN_RIGHT;
    init_struct.LowPowerMode = LL_ADC_LP_MODE_NONE;
    furi_check(LL_ADC_Init(ADC1, &init_struct) == SUCCESS);

    LL_ADC_REG_InitTypeDef reg_init_struct = {0};
    reg_init_struct.TriggerSource = LL_ADC_REG_TRIG_EXT_TIM2_TRGO;
    reg_init_struct.SequencerLength = LL_ADC_REG_SEQ_SCAN_DISABLE;
    reg_init_struct.SequencerDiscont = LL_ADC_REG_SEQ_DISCONT_DISABLE;
    reg_init_struct.ContinuousMode = LL_ADC_REG_CONV_SINGLE;
    reg_init_struct.DMATransfer = LL_ADC_REG_DMA_TRANSFER_NONE;
    reg_init_struct.Overrun = LL_ADC_REG_OVR_DATA_OVERWRITTEN;
    furi_check(LL_ADC_REG_Init(ADC1, &reg_init_struct) == SUCCESS);

    LL_ADC_REG_SetTriggerEdge(ADC1, LL_ADC_REG_TRIG_EXT_FALLING);
    LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_1, LL_ADC_CHANNEL_14);

    LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_14, LL_ADC_SAMPLINGTIME_2CYCLES_5);
    LL_ADC_SetChannelSingleDiff(ADC1, LL_ADC_CHANNEL_14, LL_ADC_SINGLE_ENDED);

    LL_ADC_DisableDeepPowerDown(ADC1);
    LL_ADC_EnableInternalRegulator(ADC1);

    timer = furi_hal_cortex_timer_get(LL_ADC_DELAY_INTERNAL_REGUL_STAB_US);
    while(!furi_hal_cortex_timer_is_expired(timer))
        ;

    LL_ADC_StartCalibration(ADC1, LL_ADC_SINGLE_ENDED);
    while(LL_ADC_IsCalibrationOnGoing(ADC1))
        ;

    const size_t end = DWT->CYCCNT + LL_ADC_DELAY_CALIB_ENABLE_ADC_CYCLES;
    while(DWT->CYCCNT < end)
        ;

    LL_ADC_ClearFlag_ADRDY(ADC1);
    LL_ADC_Enable(ADC1);

    while(LL_ADC_IsActiveFlag_ADRDY(ADC1))
        ;

    FURI_LOG_D(TAG, "ADC initialized");
}

static void lfrfid_adc_app_comp_init(LfRfidAdcApp* app) {
    UNUSED(app);

    furi_hal_bus_enable(FuriHalBusTIM2);

    furi_hal_gpio_init(&CARRIER_INPUT_PIN, GpioModeInput, GpioPullNo, GpioSpeedLow);

    // Absolutely necessary for the trigger feature to work
    LL_GPIO_SetPinMode(CARRIER_INPUT_PIN.port, CARRIER_INPUT_PIN.pin, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetAFPin_8_15(CARRIER_INPUT_PIN.port, CARRIER_INPUT_PIN.pin, LL_GPIO_AF_1);

    LL_TIM_InitTypeDef init_struct = {0};
    init_struct.Autoreload = (SystemCoreClock / CARRIER_FREQ_HZ / 4U) - 1;
    LL_TIM_Init(TIM2, &init_struct);

    LL_TIM_IC_SetActiveInput(TIM2, LL_TIM_CHANNEL_CH1, LL_TIM_ACTIVEINPUT_DIRECTTI);
    LL_TIM_SetTriggerInput(TIM2, LL_TIM_TS_TI1FP1);
    LL_TIM_SetTriggerOutput(TIM2, LL_TIM_TRGO_UPDATE);
    LL_TIM_SetOnePulseMode(TIM2, LL_TIM_ONEPULSEMODE_SINGLE);
    LL_TIM_SetSlaveMode(TIM2, LL_TIM_SLAVEMODE_TRIGGER);

    FURI_LOG_D(TAG, "Comparator initialized");
}

static void lfrfid_adc_app_carrier_init(LfRfidAdcApp* app) {
    UNUSED(app);

    furi_hal_bus_enable(FuriHalBusTIM1);

    furi_hal_gpio_init_ex(
        &CARRIER_OUTPUT_PIN, GpioModeAltFunctionPushPull, GpioPullNo, GpioSpeedLow, GpioAltFn1TIM1);

    LL_TIM_InitTypeDef init_struct = {0};
    init_struct.Autoreload = (SystemCoreClock / CARRIER_FREQ_HZ) - 1;
    LL_TIM_Init(TIM1, &init_struct);

    LL_TIM_OC_InitTypeDef oc_init_struct = {0};
    oc_init_struct.OCMode = LL_TIM_OCMODE_PWM1;
    oc_init_struct.OCNState = LL_TIM_OCSTATE_ENABLE;
    oc_init_struct.CompareValue = init_struct.Autoreload / 2;
    LL_TIM_OC_Init(TIM1, LL_TIM_CHANNEL_CH1, &oc_init_struct);

    LL_TIM_EnableCounter(TIM1);
    LL_TIM_EnableAllOutputs(TIM1);

    FURI_LOG_D(TAG, "Carrier initialized");
}

static void lfrfid_adc_app_init(LfRfidAdcApp* app) {
    lfrfid_adc_app_adc_init(app);
    lfrfid_adc_app_comp_init(app);
    lfrfid_adc_app_carrier_init(app);
}

static void lfrfid_adc_app_deinit(LfRfidAdcApp* app) {
    UNUSED(app);
}

static void lfrfid_adc_app_run(LfRfidAdcApp* app) {
    lfrfid_adc_app_init(app);

    FURI_LOG_D(TAG, "Application started");

    LL_ADC_REG_StartConversion(ADC1);

    for(;;) {
        while(!LL_ADC_IsActiveFlag_EOC(ADC1))
            ;

        FURI_LOG_D(TAG, "Converted value: %hu", LL_ADC_REG_ReadConversionData12(ADC1));
        furi_delay_ms(250);
    }

    lfrfid_adc_app_deinit(app);
}

int32_t lfrfid_adc_app(void* p) {
    UNUSED(p);

    LfRfidAdcApp* app = lfrfid_adc_app_alloc();
    lfrfid_adc_app_run(app);
    lfrfid_adc_app_free(app);

    return 0;
}
