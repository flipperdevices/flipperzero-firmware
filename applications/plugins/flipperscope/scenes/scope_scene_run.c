#include <furi.h>
#include <furi_hal.h>
#include <furi_hal_resources.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/submenu.h>
#include <gui/modules/variable_item_list.h>
#include <gui/modules/widget.h>
#include <notification/notification_messages.h>

#include "stm32wbxx_hal.h"
#include "stm32wbxx_hal_tim.h"
#include "stm32wbxx_nucleo.h"
#include "stm32wbxx_hal_adc.h"
#include "../scope_app_i.h"

#define DIGITAL_SCALE_12BITS ((uint32_t)0xFFF)
#define ADC_CONVERTED_DATA_BUFFER_SIZE ((uint32_t)128)
#define VAR_CONVERTED_DATA_INIT_VALUE (DIGITAL_SCALE_12BITS + 1)
#define VAR_CONVERTED_DATA_INIT_VALUE_16BITS (0xFFFF + 1U)
#define __ADC_CALC_DATA_VOLTAGE(__VREFANALOG_VOLTAGE__, __ADC_DATA__) \
    ((__ADC_DATA__) * (__VREFANALOG_VOLTAGE__) / DIGITAL_SCALE_12BITS)
#define VDDA_APPLI ((uint32_t)3300)

// ramVector found from - https://community.nxp.com/t5/i-MX-Processors/Relocate-vector-table-to-ITCM/m-p/1302304
// the aligned aspect is key!
#define TABLE_SIZE 79
uint32_t ramVector[TABLE_SIZE + 1] __attribute__((aligned(512)));

const uint32_t AHBPrescTable[16UL] =
    {1UL, 3UL, 5UL, 1UL, 1UL, 6UL, 10UL, 32UL, 2UL, 4UL, 8UL, 16UL, 64UL, 128UL, 256UL, 512UL};
const uint32_t APBPrescTable[8UL] = {0UL, 0UL, 0UL, 0UL, 1UL, 2UL, 3UL, 4UL};
const uint32_t MSIRangeTable[16UL] = {
    100000UL,
    200000UL,
    400000UL,
    800000UL,
    1000000UL,
    2000000UL,
    4000000UL,
    8000000UL,
    16000000UL,
    24000000UL,
    32000000UL,
    48000000UL,
    0UL,
    0UL,
    0UL,
    0UL}; /* 0UL values are incorrect cases */
char* time;
uint8_t pause = 0;
enum measureenum type;

void Error_Handler() {
    while(1) {
    }
}

static ADC_HandleTypeDef hadc1;
static DMA_HandleTypeDef hdma_adc1;
static TIM_HandleTypeDef htim2;
__IO uint16_t aADCxConvertedData
    [ADC_CONVERTED_DATA_BUFFER_SIZE]; /* ADC group regular conversion data (array of data) */
__IO uint16_t aADCxConvertedData_Voltage_mVoltA
    [ADC_CONVERTED_DATA_BUFFER_SIZE]; /* Value of voltage calculated from ADC conversion data (unit: mV) (array of data) */
__IO uint16_t aADCxConvertedData_Voltage_mVoltB
    [ADC_CONVERTED_DATA_BUFFER_SIZE]; /* Value of voltage calculated from ADC conversion data (unit: mV) (array of data) */
__IO uint8_t ubDmaTransferStatus = 2; /* Variable set into DMA interruption callback */
__IO uint16_t* mvoltWrite = &aADCxConvertedData_Voltage_mVoltA[0];
__IO uint16_t* mvoltDisplay = &aADCxConvertedData_Voltage_mVoltB[0];

void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if(hadc->Instance == ADC1) {
        __HAL_RCC_ADC_CLK_ENABLE();
        __HAL_RCC_GPIOC_CLK_ENABLE();
        GPIO_InitStruct.Pin = GPIO_PIN_0;
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
        hdma_adc1.Instance = DMA1_Channel1;
        hdma_adc1.Init.Request = DMA_REQUEST_ADC1;
        hdma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;
        hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_adc1.Init.MemInc = DMA_MINC_ENABLE;
        hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
        hdma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
        hdma_adc1.Init.Mode = DMA_CIRCULAR;
        hdma_adc1.Init.Priority = DMA_PRIORITY_LOW;
        if(HAL_DMA_Init(&hdma_adc1) != HAL_OK) {
            Error_Handler();
        }
        __HAL_LINKDMA(hadc, DMA_Handle, hdma_adc1);
        HAL_NVIC_SetPriority(ADC1_IRQn, 15, 0);
        HAL_NVIC_EnableIRQ(ADC1_IRQn);
    }
}

void HAL_ADC_MspDeInit(ADC_HandleTypeDef* hadc) {
    if(hadc->Instance == ADC1) {
        __HAL_RCC_ADC_CLK_DISABLE();
        HAL_GPIO_DeInit(GPIOC, GPIO_PIN_0);
        HAL_DMA_DeInit(hadc->DMA_Handle);
        HAL_NVIC_DisableIRQ(ADC1_IRQn);
    }
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base) {
    if(htim_base->Instance == TIM2) {
        __HAL_RCC_TIM2_CLK_ENABLE();
        HAL_NVIC_SetPriority(TIM2_IRQn, 15, 0);
        HAL_NVIC_EnableIRQ(TIM2_IRQn);
    }
}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* htim_base) {
    if(htim_base->Instance == TIM2) {
        __HAL_RCC_TIM2_CLK_DISABLE();
        HAL_NVIC_DisableIRQ(TIM2_IRQn);
    }
}

void DMA1_Channel1_IRQHandler(void) {
    HAL_DMA_IRQHandler(&hdma_adc1);
}

void ADC1_IRQHandler(void) {
    HAL_ADC_IRQHandler(&hadc1);
}

void TIM2_IRQHandler(void) {
    HAL_TIM_IRQHandler(&htim2);
}

static void MX_ADC1_Init(void) {
    ADC_ChannelConfTypeDef sConfig = {0};
    hadc1.Instance = ADC1;
    hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
    hadc1.Init.Resolution = ADC_RESOLUTION_12B;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
    hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    hadc1.Init.LowPowerAutoWait = DISABLE;
    hadc1.Init.ContinuousConvMode = DISABLE;
    hadc1.Init.NbrOfConversion = 1;
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.ExternalTrigConv = ADC_EXTERNALTRIG_T2_TRGO;
    hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
    hadc1.Init.DMAContinuousRequests = ENABLE;
    hadc1.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
    hadc1.Init.OversamplingMode = DISABLE;
    if(HAL_ADC_Init(&hadc1) != HAL_OK) {
        Error_Handler();
    }
    sConfig.Channel = ADC_CHANNEL_1;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_2CYCLE_5;
    sConfig.SingleDiff = ADC_SINGLE_ENDED;
    sConfig.OffsetNumber = ADC_OFFSET_NONE;
    sConfig.Offset = 0;
    if(HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
        Error_Handler();
    }
}

static void MX_TIM2_Init(uint32_t period) {
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 1;
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = period;
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if(HAL_TIM_Base_Init(&htim2) != HAL_OK) {
        Error_Handler();
    }
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if(HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK) {
        Error_Handler();
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if(HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK) {
        Error_Handler();
    }
}

static void MX_DMA_Init(void) {
    __HAL_RCC_DMAMUX1_CLK_ENABLE();
    __HAL_RCC_DMA1_CLK_ENABLE();
    HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 15, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
}

static void MX_GPIO_Init(void) {
    __HAL_RCC_GPIOC_CLK_ENABLE();
}

void swap(__IO uint16_t** a, __IO uint16_t** b) {
    __IO uint16_t* tmp;
    tmp = *a;
    *a = *b;
    *b = tmp;
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
    UNUSED(hadc);
    uint32_t tmp_index = 0;
    for(tmp_index = (ADC_CONVERTED_DATA_BUFFER_SIZE / 2);
        tmp_index < ADC_CONVERTED_DATA_BUFFER_SIZE;
        tmp_index++) {
        mvoltWrite[tmp_index] = __ADC_CALC_DATA_VOLTAGE(VDDA_APPLI, aADCxConvertedData[tmp_index]);
    }
    ubDmaTransferStatus = 1;
    if(!pause) swap(&mvoltWrite, &mvoltDisplay);
}

void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc) {
    UNUSED(hadc);
    uint32_t tmp_index = 0;
    for(tmp_index = 0; tmp_index < (ADC_CONVERTED_DATA_BUFFER_SIZE / 2); tmp_index++) {
        mvoltWrite[tmp_index] = __ADC_CALC_DATA_VOLTAGE(VDDA_APPLI, aADCxConvertedData[tmp_index]);
    }
    ubDmaTransferStatus = 0;
}

void HAL_ADC_ErrorCallback(ADC_HandleTypeDef* hadc) {
    UNUSED(hadc);
    Error_Handler();
}

static void app_draw_callback(Canvas* canvas, void* ctx) {
    UNUSED(ctx);
    switch(type) {
    case m_time: {
        char buf[50];
        snprintf(buf, 50, "Time: %s", time);
        canvas_draw_str(canvas, 10, 10, buf);
    } break;
    case m_voltage: {
        char buf1[50];
        char buf2[50];
        char buf3[50];
        double max = 0.0;
        double min = 100.0;
        for(uint32_t x = 0; x < ADC_CONVERTED_DATA_BUFFER_SIZE; x++) {
            if(mvoltDisplay[x] < min) min = mvoltDisplay[x];
            if(mvoltDisplay[x] > max) max = mvoltDisplay[x];
        }
        snprintf(buf1, 50, "Max: %.2fV", max / 1000);
        canvas_draw_str(canvas, 10, 10, buf1);
        snprintf(buf2, 50, "Min: %.2fV", min / 1000);
        canvas_draw_str(canvas, 10, 20, buf2);
        snprintf(buf3, 50, "Vpp: %.2fV", (max - min) / 1000);
        canvas_draw_str(canvas, 10, 30, buf3);
    } break;
    default:
        break;
    }

    for(uint32_t x = 1; x < ADC_CONVERTED_DATA_BUFFER_SIZE; x++) {
        uint32_t prev = 64 - (mvoltDisplay[x - 1] / (VDDA_APPLI / 64));
        uint32_t cur = 64 - (mvoltDisplay[x] / (VDDA_APPLI / 64));
        canvas_draw_line(canvas, x - 1, prev, x, cur);
    }

    canvas_draw_line(canvas, 0, 0, 0, 63);
    canvas_draw_line(canvas, 0, 63, 128, 63);
}

static void app_input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);
    FuriMessageQueue* event_queue = ctx;
    furi_message_queue_put(event_queue, input_event, FuriWaitForever);
}

void scope_scene_run_widget_callback(GuiButtonType result, InputType type, void* context) {
    ScopeApp* app = context;
    if(type == InputTypeShort) {
        view_dispatcher_send_custom_event(app->view_dispatcher, result);
    }
}

void scope_scene_run_on_enter(void* context) {
    ScopeApp* app = context;
    for(uint32_t i = 0; i < COUNT_OF(time_list); i++) {
        if(time_list[i].time == app->time) {
            time = time_list[i].str;
            break;
        }
    }
    pause = 0;
    type = app->measurement;

    __disable_irq();
    memcpy(ramVector, (uint32_t*)(FLASH_BASE | SCB->VTOR), sizeof(uint32_t) * TABLE_SIZE);
    SCB->VTOR = (uint32_t)ramVector;
    ramVector[27] = (uint32_t)DMA1_Channel1_IRQHandler;
    ramVector[34] = (uint32_t)ADC1_IRQHandler;
    ramVector[44] = (uint32_t)TIM2_IRQHandler;
    __enable_irq();

    HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));

    uint32_t tmp_index_adc_converted_data = 0;
    MX_GPIO_Init();
    MX_DMA_Init();

    uint32_t period = (uint32_t)((double)HAL_RCC_GetPCLK1Freq() * app->time);
    MX_TIM2_Init(period);

    VREFBUF->CSR |= VREFBUF_CSR_ENVR;
    VREFBUF->CSR &= ~VREFBUF_CSR_HIZ;
    VREFBUF->CSR |= VREFBUF_CSR_VRS;
    while(!(VREFBUF->CSR & VREFBUF_CSR_VRR)) {
    };

    MX_ADC1_Init();

    for(tmp_index_adc_converted_data = 0;
        tmp_index_adc_converted_data < ADC_CONVERTED_DATA_BUFFER_SIZE;
        tmp_index_adc_converted_data++) {
        aADCxConvertedData[tmp_index_adc_converted_data] = VAR_CONVERTED_DATA_INIT_VALUE;
        aADCxConvertedData_Voltage_mVoltA[tmp_index_adc_converted_data] = 0;
        aADCxConvertedData_Voltage_mVoltB[tmp_index_adc_converted_data] = 0;
    }

    if(HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED) != HAL_OK) {
        Error_Handler();
    }

    if(HAL_TIM_Base_Start(&htim2) != HAL_OK) {
        Error_Handler();
    }

    if(HAL_ADC_Start_DMA(&hadc1, (uint32_t*)aADCxConvertedData, ADC_CONVERTED_DATA_BUFFER_SIZE) !=
       HAL_OK) {
        Error_Handler();
    }

    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, app_draw_callback, view_port);
    view_port_input_callback_set(view_port, app_input_callback, event_queue);

    // Register view port in GUI
    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);
    InputEvent event;
    bool running = true;
    while(running) {
        if(furi_message_queue_get(event_queue, &event, 100) == FuriStatusOk) {
            if((event.type == InputTypePress) || (event.type == InputTypeRepeat)) {
                switch(event.key) {
                case InputKeyLeft:
                    break;
                case InputKeyRight:
                    break;
                case InputKeyUp:
                    break;
                case InputKeyDown:
                    break;
                case InputKeyOk:
                    pause ^= 1;
                    break;
                default:
                    running = false;
                    break;
                }
            }
        }
        view_port_update(view_port);
    }

    HAL_ADC_Stop_DMA(&hadc1);
    __disable_irq();
    SCB->VTOR = 0;
    __enable_irq();

    view_port_enabled_set(view_port, false);
    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);

    furi_record_close(RECORD_GUI);
    scene_manager_previous_scene(app->scene_manager);
    submenu_set_selected_item(app->submenu, 0);
}

bool scope_scene_run_on_event(void* context, SceneManagerEvent event) {
    ScopeApp* app = context;
    bool consumed = false;
    UNUSED(app);
    UNUSED(event);
    return consumed;
}

void scope_scene_run_on_exit(void* context) {
    ScopeApp* app = context;
    // Clear views
    widget_reset(app->widget);
}
