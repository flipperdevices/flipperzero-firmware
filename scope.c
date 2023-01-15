#include <furi.h>
#include <furi_hal.h>
#include <furi_hal_resources.h>
#include <gui/gui.h>
#include <input/input.h>

#include "stm32wbxx_hal.h"
#include "stm32wbxx_hal_tim.h"
#include "stm32wbxx_nucleo.h"
#include "stm32wbxx_hal_adc.h"
#include "scope_icons.h"

#define DIGITAL_SCALE_12BITS             ((uint32_t) 0xFFF)
#define ADC_CONVERTED_DATA_BUFFER_SIZE   ((uint32_t)  64)
#define VAR_CONVERTED_DATA_INIT_VALUE    (DIGITAL_SCALE_12BITS + 1)
#define VAR_CONVERTED_DATA_INIT_VALUE_16BITS    (0xFFFF + 1U)
#define __ADC_CALC_DATA_VOLTAGE(__VREFANALOG_VOLTAGE__, __ADC_DATA__)       \
  ((__ADC_DATA__) * (__VREFANALOG_VOLTAGE__) / DIGITAL_SCALE_12BITS)
#define VDDA_APPLI                       ((uint32_t)3300)

#include <stdlib.h>
#include <string.h>

void Error_Handler()
{
    while (1) {
    }
}

uint16_t i = 0;

static ADC_HandleTypeDef hadc1;
static DMA_HandleTypeDef hdma_adc1;
static TIM_HandleTypeDef htim2;

__IO uint16_t aADCxConvertedData[ADC_CONVERTED_DATA_BUFFER_SIZE];       /* ADC group regular conversion data (array of data) */
__IO uint16_t aADCxConvertedData_Voltage_mVolt[ADC_CONVERTED_DATA_BUFFER_SIZE]; /* Value of voltage calculated from ADC conversion data (unit: mV) (array of data) */
__IO uint8_t ubDmaTransferStatus = 2;   /* Variable set into DMA interruption callback */

void HAL_MspInit(void)
{
}

void HAL_ADC_MspInit(ADC_HandleTypeDef * hadc)
{
    GPIO_InitTypeDef GPIO_InitStruct = { 0 };
    if (hadc->Instance == ADC1) {
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
        if (HAL_DMA_Init(&hdma_adc1) != HAL_OK) {
            Error_Handler();
        }

        __HAL_LINKDMA(hadc, DMA_Handle, hdma_adc1);

        HAL_NVIC_SetPriority(ADC1_IRQn, 15, 0);
        HAL_NVIC_EnableIRQ(ADC1_IRQn);
    }
}

void HAL_ADC_MspDeInit(ADC_HandleTypeDef * hadc)
{
    if (hadc->Instance == ADC1) {
        __HAL_RCC_ADC_CLK_DISABLE();
        HAL_GPIO_DeInit(GPIOC, GPIO_PIN_0);
        HAL_DMA_DeInit(hadc->DMA_Handle);
        HAL_NVIC_DisableIRQ(ADC1_IRQn);
    }
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef * htim_base)
{
    if (htim_base->Instance == TIM2) {
        __HAL_RCC_TIM2_CLK_ENABLE();
        HAL_NVIC_SetPriority(TIM2_IRQn, 15, 0);
        HAL_NVIC_EnableIRQ(TIM2_IRQn);
    }
}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef * htim_base)
{
    if (htim_base->Instance == TIM2) {
        __HAL_RCC_TIM2_CLK_DISABLE();
        HAL_NVIC_DisableIRQ(TIM2_IRQn);
    }
}

void DMA1_Channel1_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_adc1);
}

void ADC1_IRQHandler(void)
{
    HAL_ADC_IRQHandler(&hadc1);
}

void TIM2_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim2);
}

static void MX_ADC1_Init(void)
{
    ADC_ChannelConfTypeDef sConfig = { 0 };
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
    if (HAL_ADC_Init(&hadc1) != HAL_OK) {
        Error_Handler();
    }

    sConfig.Channel = ADC_CHANNEL_1;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_2CYCLE_5;     ////ADC_SAMPLETIME_640CYCLES_5;
    sConfig.SingleDiff = ADC_SINGLE_ENDED;
    sConfig.OffsetNumber = ADC_OFFSET_NONE;
    sConfig.Offset = 0;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
        Error_Handler();
    }
}

static void MX_TIM2_Init(void)
{

    TIM_ClockConfigTypeDef sClockSourceConfig = { 0 };
    TIM_MasterConfigTypeDef sMasterConfig = { 0 };
    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 1;
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = 4194304;        //39999;
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(&htim2) != HAL_OK) {
        Error_Handler();
    }
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK) {
        Error_Handler();
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) !=
        HAL_OK) {
        Error_Handler();
    }
}

static void MX_DMA_Init(void)
{

    __HAL_RCC_DMAMUX1_CLK_ENABLE();
    __HAL_RCC_DMA1_CLK_ENABLE();
    HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 15, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);

}

static void MX_GPIO_Init(void)
{

    __HAL_RCC_GPIOC_CLK_ENABLE();

}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef * hadc)
{
    UNUSED(hadc);
    uint32_t tmp_index = 0;

    for (tmp_index = (ADC_CONVERTED_DATA_BUFFER_SIZE / 2);
         tmp_index < ADC_CONVERTED_DATA_BUFFER_SIZE; tmp_index++) {
        aADCxConvertedData_Voltage_mVolt[tmp_index] =
            __ADC_CALC_DATA_VOLTAGE(VDDA_APPLI,
                                    aADCxConvertedData[tmp_index]);
    }


    ubDmaTransferStatus = 1;
}

void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef * hadc)
{
    UNUSED(hadc);
    uint32_t tmp_index = 0;

    for (tmp_index = 0; tmp_index < (ADC_CONVERTED_DATA_BUFFER_SIZE / 2);
         tmp_index++) {
        aADCxConvertedData_Voltage_mVolt[tmp_index] =
            __ADC_CALC_DATA_VOLTAGE(VDDA_APPLI,
                                    aADCxConvertedData[tmp_index]);
    }

    ubDmaTransferStatus = 0;
}

void HAL_ADC_ErrorCallback(ADC_HandleTypeDef * hadc)
{
    UNUSED(hadc);
    Error_Handler();
}

typedef struct {
    uint8_t x, y;
} ImagePosition;

uint16_t val1;

static ImagePosition image_position = {.x = 0,.y = 0 };

void assert_failed(uint8_t * file, uint32_t line)
{
    UNUSED(file);
    UNUSED(line);
    while (1) {
    }
}

// Screen is 128x64 px
static void app_draw_callback(Canvas * canvas, void *ctx)
{
    UNUSED(ctx);
    char buf[50];
    snprintf(buf, 50, "%d", val1);
    canvas_draw_str(canvas, 10, 10, buf);
    //canvas_draw_dot(canvas, image_position.x % 128, image_position.y % 64);
    canvas_draw_line(canvas, 0, 0, 0, 63);
    canvas_draw_line(canvas, 0, 63, 128, 63);
}

static void app_input_callback(InputEvent * input_event, void *ctx)
{
    furi_assert(ctx);
    FuriMessageQueue *event_queue = ctx;
    furi_message_queue_put(event_queue, input_event, FuriWaitForever);
}


int32_t scope_main(void *p)
{
    UNUSED(p);
    HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
    FuriMessageQueue *event_queue =
        furi_message_queue_alloc(8, sizeof(InputEvent));

    uint32_t tmp_index_adc_converted_data = 0;
    MX_GPIO_Init();
    MX_DMA_Init();
    MX_TIM2_Init();


    VREFBUF->CSR |= VREFBUF_CSR_ENVR;
    VREFBUF->CSR &= ~VREFBUF_CSR_HIZ;
    VREFBUF->CSR |= VREFBUF_CSR_VRS;
    while (!(VREFBUF->CSR & VREFBUF_CSR_VRR)) {
    };

    MX_ADC1_Init();
    for (tmp_index_adc_converted_data = 0;
         tmp_index_adc_converted_data < ADC_CONVERTED_DATA_BUFFER_SIZE;
         tmp_index_adc_converted_data++) {
        aADCxConvertedData[tmp_index_adc_converted_data] =
            VAR_CONVERTED_DATA_INIT_VALUE;
    }

    if (HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED) != HAL_OK) {
        Error_Handler();
    }

    if (HAL_TIM_Base_Start(&htim2) != HAL_OK) {
        Error_Handler();
    }

    if (HAL_ADC_Start_DMA(&hadc1,
                          (uint32_t *) aADCxConvertedData,
                          ADC_CONVERTED_DATA_BUFFER_SIZE) != HAL_OK) {
        Error_Handler();
    }

    ViewPort *view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, app_draw_callback, view_port);
    view_port_input_callback_set(view_port, app_input_callback,
                                 event_queue);

    // Register view port in GUI
    Gui *gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);
    InputEvent event;
    bool running = true;
    
    while (running) {
        if (furi_message_queue_get(event_queue, &event, 102) ==
            FuriStatusOk) {
            if ((event.type == InputTypePress)
                || (event.type == InputTypeRepeat)) {
                switch (event.key) {
                case InputKeyLeft:
                    image_position.x -= 2;
                    break;
                case InputKeyRight:
                    image_position.x += 2;
                    break;
                case InputKeyUp:
                    image_position.y -= 2;
                    break;
                case InputKeyDown:
                    image_position.y += 2;
                    break;
                default:
                    running = false;
                    break;
                }
            }
        }

        val1 = aADCxConvertedData[0];
        view_port_update(view_port);
    }

    view_port_enabled_set(view_port, false);
    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_message_queue_free(event_queue);

    furi_record_close(RECORD_GUI);

    return 0;
}
