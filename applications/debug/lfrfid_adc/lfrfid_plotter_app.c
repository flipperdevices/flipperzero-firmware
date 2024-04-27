#include <furi.h>

#include <gui/gui.h>
#include <gui/elements.h>
#include <gui/view_port.h>

#include <furi_hal_bus.h>
#include <furi_hal_cortex.h>
#include <furi_hal_interrupt.h>
#include <furi_hal_resources.h>
#include <furi_hal_serial.h>
#include <furi_hal_serial_control.h>

#include <stm32wbxx_ll_adc.h>
#include <stm32wbxx_ll_tim.h>
#include <stm32wbxx_ll_dma.h>
#include <stm32wbxx_ll_dmamux.h>

#define TAG "LfRfidPlotter"

#define DATA_INPUT_PIN (gpio_rfid_data_in)
#define DATA_OUTPUT_PIN (gpio_ext_pa7)

#define CARRIER_INPUT_PIN (gpio_rfid_carrier)
#define CARRIER_OUTPUT_PIN (gpio_rfid_carrier_out)

#define CARRIER_FREQ_HZ (125000UL)
#define VREFBUF_STARTUP_DELAY_US (500000UL)
#define RECEIVE_BUFFER_LEN (32 * 1024UL)

typedef enum {
    LfRfidPlotterAppEventExit,
    LfRfidPlotterAppEventHalfTransfer,
    LfRfidPlotterAppEventTransferComplete,
} LfRfidPlotterAppEvent;

typedef struct {
    Gui* gui;
    ViewPort* view_port;
    FuriMessageQueue* queue;
    FuriHalSerialHandle* serial;
    uint16_t buf[RECEIVE_BUFFER_LEN];
    uint16_t shadow_buf[RECEIVE_BUFFER_LEN / 2];
} LfRfidPlotterApp;

static void lfrfid_plotter_app_draw_callback(Canvas* canvas, void* context) {
    UNUSED(context);
    elements_text_box(
        canvas,
        0,
        0,
        canvas_width(canvas),
        canvas_height(canvas),
        AlignCenter,
        AlignCenter,
        "\e#Received data\e# is being\nsent to \e#USART\e#",
        false);
}

static void lfrfid_plotter_app_input_callback(InputEvent* event, void* context) {
    LfRfidPlotterApp* app = context;

    if(event->type == InputTypeShort && event->key == InputKeyBack) {
        const LfRfidPlotterAppEvent app_event = LfRfidPlotterAppEventExit;
        furi_message_queue_put(app->queue, &app_event, FuriWaitForever);
    }
}

static void lfrfid_plotter_app_dma_callback(void* context) {
    LfRfidPlotterApp* app = context;

    if(LL_DMA_IsActiveFlag_HT1(DMA2)) {
        LL_DMA_ClearFlag_HT1(DMA2);

        const LfRfidPlotterAppEvent app_event = LfRfidPlotterAppEventHalfTransfer;
        furi_message_queue_put(app->queue, &app_event, 0);
    }

    if(LL_DMA_IsActiveFlag_TC1(DMA2)) {
        LL_DMA_ClearFlag_TC1(DMA2);

        const LfRfidPlotterAppEvent app_event = LfRfidPlotterAppEventTransferComplete;
        furi_message_queue_put(app->queue, &app_event, 0);
    }
}

static LfRfidPlotterApp* lfrfid_plotter_app_alloc() {
    LfRfidPlotterApp* app = malloc(sizeof(LfRfidPlotterApp));

    app->gui = furi_record_open(RECORD_GUI);

    app->view_port = view_port_alloc();
    app->queue = furi_message_queue_alloc(4, sizeof(LfRfidPlotterAppEvent));

    view_port_draw_callback_set(app->view_port, lfrfid_plotter_app_draw_callback, app);
    view_port_input_callback_set(app->view_port, lfrfid_plotter_app_input_callback, app);

    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);

    return app;
};

static void lfrfid_plotter_app_free(LfRfidPlotterApp* app) {
    furi_message_queue_free(app->queue);

    gui_remove_view_port(app->gui, app->view_port);
    view_port_free(app->view_port);

    furi_record_close(RECORD_GUI);

    free(app);
}

static void lfrfid_plotter_app_adc_init(LfRfidPlotterApp* app) {
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

    LL_ADC_REG_StartConversion(ADC1);

    FURI_LOG_D(TAG, "ADC initialized");
}

static void lfrfid_plotter_app_serial_init(LfRfidPlotterApp* app) {
    app->serial = furi_hal_serial_control_acquire(FuriHalSerialIdUsart);
    furi_hal_serial_init(app->serial, 4000000UL);

    FURI_LOG_D(TAG, "Serial initialized");
}

static void lfrfid_plotter_app_dma_init(LfRfidPlotterApp* app) {
    LL_DMA_InitTypeDef init_struct = {0};
    init_struct.PeriphOrM2MSrcAddress = (uint32_t) & (ADC1->DR);
    init_struct.MemoryOrM2MDstAddress = (uint32_t) & (app->buf[0]);
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

    furi_hal_interrupt_set_isr(FuriHalInterruptIdDma2Ch1, lfrfid_plotter_app_dma_callback, app);

    LL_DMA_EnableIT_HT(DMA2, LL_DMA_CHANNEL_1);
    LL_DMA_EnableIT_TC(DMA2, LL_DMA_CHANNEL_1);

    LL_DMA_EnableChannel(DMA2, LL_DMA_CHANNEL_1);

    FURI_LOG_D(TAG, "DMA initialized");
}

static void lfrfid_plotter_app_serial_write(LfRfidPlotterApp* app, bool tc) {
    const size_t data_size = RECEIVE_BUFFER_LEN * sizeof(uint16_t) / 2UL;
    const size_t data_offset = tc ? data_size : 0;

    const void* data = (void*)app->buf + data_offset;
    void* shadow_data = (void*)app->shadow_buf;

    memcpy(shadow_data, data, data_size);

    furi_hal_serial_tx(app->serial, shadow_data, data_size);
    furi_hal_serial_tx_wait_complete(app->serial);
}

static void lfrfid_plotter_app_comp_init(LfRfidPlotterApp* app) {
    UNUSED(app);

    furi_hal_bus_enable(FuriHalBusTIM2);
    furi_hal_gpio_init_ex(
        &CARRIER_INPUT_PIN, GpioModeAltFunctionPushPull, GpioPullNo, GpioSpeedLow, GpioAltFn1TIM2);

    LL_TIM_InitTypeDef init_struct = {0};
    init_struct.Autoreload = (SystemCoreClock / CARRIER_FREQ_HZ / 4U) + 1;
    LL_TIM_Init(TIM2, &init_struct);

    LL_TIM_IC_SetActiveInput(TIM2, LL_TIM_CHANNEL_CH1, LL_TIM_ACTIVEINPUT_DIRECTTI);
    LL_TIM_SetTriggerInput(TIM2, LL_TIM_TS_TI1FP1);
    LL_TIM_SetTriggerOutput(TIM2, LL_TIM_TRGO_UPDATE);
    LL_TIM_SetOnePulseMode(TIM2, LL_TIM_ONEPULSEMODE_SINGLE);
    LL_TIM_SetSlaveMode(TIM2, LL_TIM_SLAVEMODE_TRIGGER);

    FURI_LOG_D(TAG, "Comparator initialized");
}

static void lfrfid_plotter_app_carrier_init(LfRfidPlotterApp* app) {
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

static void lfrfid_plotter_app_init(LfRfidPlotterApp* app) {
    FURI_LOG_D(TAG, "Application started");

    lfrfid_plotter_app_serial_init(app);
    lfrfid_plotter_app_dma_init(app);
    lfrfid_plotter_app_adc_init(app);
    lfrfid_plotter_app_comp_init(app);
    lfrfid_plotter_app_carrier_init(app);
}

static void lfrfid_plotter_app_deinit(LfRfidPlotterApp* app) {
    UNUSED(app);

    furi_hal_interrupt_set_isr(FuriHalInterruptIdDma2Ch1, NULL, NULL);
    LL_DMA_DeInit(DMA2, LL_DMA_CHANNEL_1);

    furi_hal_bus_disable(FuriHalBusADC);
    furi_hal_bus_disable(FuriHalBusTIM1);
    furi_hal_bus_disable(FuriHalBusTIM2);

    furi_hal_serial_control_release(app->serial);

    FURI_LOG_D(TAG, "Application exited");
}

static void lfrfid_plotter_app_run(LfRfidPlotterApp* app) {
    lfrfid_plotter_app_init(app);

    for(;;) {
        LfRfidPlotterAppEvent event;
        const FuriStatus status = furi_message_queue_get(app->queue, &event, FuriWaitForever);
        furi_check(status == FuriStatusOk);

        if(event == LfRfidPlotterAppEventExit) {
            FURI_LOG_D(TAG, "Back key pressed");
            break;

        } else if(
            event == LfRfidPlotterAppEventHalfTransfer || event == LfRfidPlotterAppEventTransferComplete) {
            lfrfid_plotter_app_serial_write(app, event == LfRfidPlotterAppEventTransferComplete);
        }
    }

    lfrfid_plotter_app_deinit(app);
}

int32_t lfrfid_plotter_app(void* p) {
    UNUSED(p);

    LfRfidPlotterApp* app = lfrfid_plotter_app_alloc();
    lfrfid_plotter_app_run(app);
    lfrfid_plotter_app_free(app);

    return 0;
}
