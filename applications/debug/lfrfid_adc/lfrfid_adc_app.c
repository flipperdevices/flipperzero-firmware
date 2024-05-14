#include <furi.h>

#include <gui/gui.h>
#include <gui/elements.h>
#include <gui/view_port.h>

#include <lib/toolbox/protocols/protocol_dict.h>
#include <lib/lfrfid/protocols/lfrfid_protocols.h>

#include <furi_hal_bus.h>
#include <furi_hal_cortex.h>
#include <furi_hal_interrupt.h>
#include <furi_hal_resources.h>

#include <stm32wbxx_ll_adc.h>
#include <stm32wbxx_ll_tim.h>
#include <stm32wbxx_ll_dma.h>
#include <stm32wbxx_ll_dmamux.h>

#define TAG "LfRfidAdc"

#define DATA_INPUT_PIN (gpio_rfid_data_in)

#define CARRIER_INPUT_PIN (gpio_rfid_carrier)
#define CARRIER_OUTPUT_PIN (gpio_rfid_carrier_out)

#define CARRIER_FREQ_HZ (125000UL)
#define CARRIER_PERIOD_US (1000000UL / CARRIER_FREQ_HZ)

#define VREFBUF_STARTUP_DELAY_US (500000UL)

#define RECEIVE_BUFFER_LEN (2UL * 1024UL)

#define ADC_INPUT_CHANNEL LL_ADC_CHANNEL_14

typedef enum {
    LfRfidAdcAppEventExit,
    LfRfidAdcAppEventHalfTransfer,
    LfRfidAdcAppEventTransferComplete,
} LfRfidAdcAppEvent;

typedef struct {
    Gui* gui;
    ViewPort* view_port;
    FuriMessageQueue* queue;
    ProtocolDict* protocol_dict;
    uint8_t* protocol_data;
    char* protocol_string;
    uint16_t rx_buf[RECEIVE_BUFFER_LEN];
    uint16_t prev_value;
    int32_t prev_dv;
    bool prev_level;
    uint32_t pulse_time;
} LfRfidAdcApp;

static void lfrfid_adc_app_draw_callback(Canvas* canvas, void* context) {
    UNUSED(context);
    elements_text_box(
        canvas,
        0,
        0,
        canvas_width(canvas),
        canvas_height(canvas),
        AlignCenter,
        AlignCenter,
        "\e#Received data\e# is being\ndecoded (check the \e#Logs\e#)",
        false);
}

static void lfrfid_adc_app_input_callback(InputEvent* event, void* context) {
    LfRfidAdcApp* app = context;

    if(event->type == InputTypeShort && event->key == InputKeyBack) {
        const LfRfidAdcAppEvent app_event = LfRfidAdcAppEventExit;
        furi_message_queue_put(app->queue, &app_event, FuriWaitForever);
    }
}

static void lfrfid_adc_app_dma_callback(void* context) {
    LfRfidAdcApp* app = context;

    if(LL_DMA_IsActiveFlag_HT1(DMA2)) {
        LL_DMA_ClearFlag_HT1(DMA2);

        const LfRfidAdcAppEvent app_event = LfRfidAdcAppEventHalfTransfer;
        furi_message_queue_put(app->queue, &app_event, 0);
    }

    if(LL_DMA_IsActiveFlag_TC1(DMA2)) {
        LL_DMA_ClearFlag_TC1(DMA2);

        const LfRfidAdcAppEvent app_event = LfRfidAdcAppEventTransferComplete;
        furi_message_queue_put(app->queue, &app_event, 0);
    }
}

static LfRfidAdcApp* lfrfid_adc_app_alloc() {
    LfRfidAdcApp* app = malloc(sizeof(LfRfidAdcApp));

    app->gui = furi_record_open(RECORD_GUI);

    app->view_port = view_port_alloc();
    app->queue = furi_message_queue_alloc(4, sizeof(LfRfidAdcAppEvent));
    app->protocol_dict = protocol_dict_alloc(lfrfid_protocols, LFRFIDProtocolMax);

    const size_t max_protocol_data_size = protocol_dict_get_max_data_size(app->protocol_dict);
    app->protocol_data = malloc(max_protocol_data_size);
    app->protocol_string = malloc(max_protocol_data_size * 3 + 1);

    view_port_draw_callback_set(app->view_port, lfrfid_adc_app_draw_callback, app);
    view_port_input_callback_set(app->view_port, lfrfid_adc_app_input_callback, app);

    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);

    return app;
};

static void lfrfid_adc_app_free(LfRfidAdcApp* app) {
    free(app->protocol_string);
    free(app->protocol_data);
    protocol_dict_free(app->protocol_dict);
    furi_message_queue_free(app->queue);

    gui_remove_view_port(app->gui, app->view_port);
    view_port_free(app->view_port);

    furi_record_close(RECORD_GUI);

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
    reg_init_struct.DMATransfer = LL_ADC_REG_DMA_TRANSFER_UNLIMITED;
    reg_init_struct.Overrun = LL_ADC_REG_OVR_DATA_OVERWRITTEN;
    furi_check(LL_ADC_REG_Init(ADC1, &reg_init_struct) == SUCCESS);

    LL_ADC_REG_SetTriggerEdge(ADC1, LL_ADC_REG_TRIG_EXT_FALLING);
    LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_1, ADC_INPUT_CHANNEL);

    LL_ADC_SetChannelSamplingTime(ADC1, ADC_INPUT_CHANNEL, LL_ADC_SAMPLINGTIME_6CYCLES_5);
    LL_ADC_SetChannelSingleDiff(ADC1, ADC_INPUT_CHANNEL, LL_ADC_SINGLE_ENDED);

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

    LL_ADC_REG_StartConversion(ADC1);

    FURI_LOG_D(TAG, "ADC initialized");
}

static void lfrfid_adc_app_dma_init(LfRfidAdcApp* app) {
    LL_DMA_InitTypeDef init_struct = {0};
    init_struct.PeriphOrM2MSrcAddress = (uint32_t) & (ADC1->DR);
    init_struct.MemoryOrM2MDstAddress = (uint32_t) & (app->rx_buf[0]);
    init_struct.Direction = LL_DMA_DIRECTION_PERIPH_TO_MEMORY;
    init_struct.Mode = LL_DMA_MODE_CIRCULAR;
    init_struct.PeriphOrM2MSrcIncMode = LL_DMA_PERIPH_NOINCREMENT;
    init_struct.MemoryOrM2MDstIncMode = LL_DMA_MEMORY_INCREMENT;
    init_struct.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_HALFWORD;
    init_struct.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_HALFWORD;
    init_struct.NbData = RECEIVE_BUFFER_LEN;
    init_struct.PeriphRequest = LL_DMAMUX_REQ_ADC1;
    init_struct.Priority = LL_DMA_PRIORITY_VERYHIGH;
    furi_check(LL_DMA_Init(DMA2, LL_DMA_CHANNEL_1, &init_struct) == SUCCESS);

    furi_hal_interrupt_set_isr(FuriHalInterruptIdDma2Ch1, lfrfid_adc_app_dma_callback, app);

    LL_DMA_EnableIT_HT(DMA2, LL_DMA_CHANNEL_1);
    LL_DMA_EnableIT_TC(DMA2, LL_DMA_CHANNEL_1);

    LL_DMA_EnableChannel(DMA2, LL_DMA_CHANNEL_1);

    FURI_LOG_D(TAG, "DMA initialized");
}

static void lfrfid_adc_app_comp_init(LfRfidAdcApp* app) {
    UNUSED(app);

    furi_hal_bus_enable(FuriHalBusTIM2);
    furi_hal_gpio_init_ex(
        &CARRIER_INPUT_PIN,
        GpioModeAltFunctionPushPull,
        GpioPullNo,
        GpioSpeedVeryHigh,
        GpioAltFn2TIM2);
    furi_hal_gpio_init_ex(
        &gpio_ext_pb3, GpioModeAltFunctionPushPull, GpioPullNo, GpioSpeedVeryHigh, GpioAltFn1TIM2);

    LL_TIM_InitTypeDef init_struct = {0};
    init_struct.Autoreload = (SystemCoreClock / CARRIER_FREQ_HZ / 4UL) - 6UL;
    LL_TIM_Init(TIM2, &init_struct);

    LL_TIM_ConfigETR(
        TIM2, LL_TIM_ETR_POLARITY_NONINVERTED, LL_TIM_ETR_PRESCALER_DIV1, LL_TIM_ETR_FILTER_FDIV1);
    LL_TIM_SetTriggerInput(TIM2, LL_TIM_TS_ETRF);
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
    FURI_LOG_D(TAG, "Application started");

    protocol_dict_decoders_start(app->protocol_dict);

    lfrfid_adc_app_dma_init(app);
    lfrfid_adc_app_adc_init(app);
    lfrfid_adc_app_comp_init(app);
    lfrfid_adc_app_carrier_init(app);
}

static void lfrfid_adc_app_deinit(LfRfidAdcApp* app) {
    UNUSED(app);

    furi_hal_interrupt_set_isr(FuriHalInterruptIdDma2Ch1, NULL, NULL);
    LL_DMA_DeInit(DMA2, LL_DMA_CHANNEL_1);

    furi_hal_bus_disable(FuriHalBusADC);
    furi_hal_bus_disable(FuriHalBusTIM1);
    furi_hal_bus_disable(FuriHalBusTIM2);

    FURI_LOG_D(TAG, "Application exited");
}

static void lfrfid_adc_app_print_protocol(LfRfidAdcApp* app, ProtocolId protocol_id) {
    const size_t data_size = protocol_dict_get_data_size(app->protocol_dict, protocol_id);
    protocol_dict_get_data(app->protocol_dict, protocol_id, app->protocol_data, data_size);

    char* sp = app->protocol_string;

    for(size_t i = 0; i < data_size; ++i) {
        sp += snprintf(sp, 4, "%02X ", app->protocol_data[i]);
    }

    *sp = '\0';

    const char* protocol_name = protocol_dict_get_name(app->protocol_dict, protocol_id);
    FURI_LOG_I(TAG, "Protocol: %s Data: %s", protocol_name, app->protocol_string);
}

static void lfrfid_adc_app_decode_data(LfRfidAdcApp* app, bool tc) {
    const size_t data_len = RECEIVE_BUFFER_LEN / 2UL;
    const size_t data_offset = tc ? data_len : 0;

    const uint16_t* data = app->rx_buf + data_offset;

    for(size_t i = 0; i < data_len; ++i) {
        const uint16_t value = data[i];

        const int32_t dv = (int32_t)value - app->prev_value;
        const int32_t sum_dv = dv + app->prev_dv;

        // TODO: Dynamic noise threshold?
        // Trial and error: values between 20...40 give decent results
        const int noise_threshold = 28;

        const bool same_sign = (dv > 0) == (app->prev_dv > 0);
        const bool above_threshold = abs(sum_dv) > noise_threshold;

        if(same_sign && above_threshold) {
            // Registering the previous level, not the new one
            const bool level = !(sum_dv > 0);

            if(level != app->prev_level) { // Running decoder only on level changes
                const ProtocolId protocol_id =
                    protocol_dict_decoders_feed(app->protocol_dict, level, app->pulse_time);

                if(protocol_id != PROTOCOL_NO) {
                    lfrfid_adc_app_print_protocol(app, protocol_id);
                }

                app->prev_level = level;
                app->pulse_time = 0;
            }
        }

        app->pulse_time += CARRIER_PERIOD_US;
        app->prev_value = value;
        app->prev_dv = dv;
    }
}

static void lfrfid_adc_app_run(LfRfidAdcApp* app) {
    lfrfid_adc_app_init(app);

    for(;;) {
        LfRfidAdcAppEvent event;
        const FuriStatus status = furi_message_queue_get(app->queue, &event, FuriWaitForever);
        furi_check(status == FuriStatusOk);

        if(event == LfRfidAdcAppEventExit) {
            FURI_LOG_D(TAG, "Back key pressed");
            break;

        } else if(
            event == LfRfidAdcAppEventHalfTransfer || event == LfRfidAdcAppEventTransferComplete) {
            lfrfid_adc_app_decode_data(app, event == LfRfidAdcAppEventTransferComplete);
        }
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
