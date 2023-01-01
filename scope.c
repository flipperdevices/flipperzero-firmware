#include <furi.h>
#include <furi_hal.h>
#include <furi_hal_resources.h>

#include <gui/gui.h>
#include <input/input.h>

#include "stm32wbxx_hal.h"
#include "stm32wbxx_nucleo.h"

#include "stm32wbxx_ll_adc.h"
#include "stm32wbxx_ll_dma.h"

#include "scope_icons.h"

#define DIGITAL_SCALE_12BITS             ((uint32_t) 0xFFF)

#define VAR_CONVERTED_DATA_INIT_VALUE    (DIGITAL_SCALE_12BITS + 1)
#define VAR_CONVERTED_DATA_INIT_VALUE_16BITS    (0xFFFF + 1U)
#define __ADC_CALC_DATA_VOLTAGE(__VREFANALOG_VOLTAGE__, __ADC_DATA__)       \
  ((__ADC_DATA__) * (__VREFANALOG_VOLTAGE__) / DIGITAL_SCALE_12BITS)
#define VDDA_APPLI                       ((uint32_t)3300)

ADC_HandleTypeDef hadc1 = { 0 };

/* Variables for ADC conversion data */
__IO uint16_t uhADCxConvertedData = VAR_CONVERTED_DATA_INIT_VALUE;      /* ADC group regular conversion data */

/* Variables for ADC conversion data computation to physical values */
uint16_t uhADCxConvertedData_Voltage_mVolt = 0; /* Value of voltage calculated from ADC conversion data (unit: mV) */

void Error_Handler()
{
    while (1) {
    }
}

static void MX_ADC1_Init(void)
{
    hadc1.Instance = ADC1;
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = { 0 };
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_ADC;
    PeriphClkInitStruct.AdcClockSelection = RCC_ADCCLKSOURCE_PLL;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK) {
        Error_Handler();
    }

    /* Peripheral clock enable */
    // __HAL_RCC_ADC_CLK_ENABLE();

    //HAL_Init();
    //SystemClock_Config();
    __HAL_RCC_ADC_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    // __HAL_RCC_ADC_FORCE_RESET();
    //       __HAL_RCC_ADC_RELEASE_RESET();
    //furi_hal_gpio_init_ex(&gpio_ext_pc0, GpioModeAnalog, GpioPullNo, GpioSpeedVeryHigh, GpioAltFnUnused);

    GPIO_InitTypeDef GPIO_initStructre = { 0, 0, 0, 0, 0 };
    GPIO_initStructre.Pin = GPIO_PIN_0; // Provide input to channel 10 of ADC i.e GPIO Pin 0 of Port C
    GPIO_initStructre.Mode = GPIO_MODE_ANALOG;  //GPIO Pin as analog Mode
    GPIO_initStructre.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC, &GPIO_initStructre);   // GPIO Initialization

    hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
    hadc1.Init.Resolution = ADC_RESOLUTION_12B;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
    hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;      //ADC_EOC_SEQ_CONV ; //ADC_EOC_SINGLE_CONV;
    hadc1.Init.LowPowerAutoWait = DISABLE;
    hadc1.Init.ContinuousConvMode = DISABLE;
    //hadc1.Init.ContinuousConvMode = ENABLE;
    hadc1.Init.NbrOfConversion = 1;
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    hadc1.Init.DMAContinuousRequests = DISABLE;
    hadc1.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
    hadc1.Init.OversamplingMode = DISABLE;
    /*
       hadc1.Init.Oversampling.Ratio = ADC_OVERSAMPLING_RATIO_16;
       hadc1.Init.Oversampling.RightBitShift = ADC_RIGHTBITSHIFT_4;
       hadc1.Init.Oversampling.TriggeredMode = ADC_TRIGGEREDMODE_SINGLE_TRIGGER;
       hadc1.Init.Oversampling.OversamplingStopReset = ADC_REGOVERSAMPLING_CONTINUED_MODE;
     */

    if (HAL_ADC_Init(&hadc1) != HAL_OK) {
        Error_Handler();
    }

}

typedef struct {
    uint8_t x, y;
} ImagePosition;

uint16_t val1;

ADC_HandleTypeDef hadc1;

static ImagePosition image_position = {.x = 0,.y = 0 };

void assert_failed(uint8_t * file, uint32_t line)
{
    UNUSED(file);
    UNUSED(line);
    /* USER CODE BEGIN 6 */

    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d", file, line) */

    /* Infinite loop */
    while (1) {
    }
    /* USER CODE END 6 */
}

// Screen is 128x64 px
static void app_draw_callback(Canvas * canvas, void *ctx)
{
    UNUSED(ctx);

    //canvas_clear(canvas);
    //canvas_draw_icon(canvas, image_position.x % 128, image_position.y % 64, &I_dolphin_71x25);
    //char *hello = "hello";
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
    //UNUSED(MX_DMA_Init);
    //UNUSED(MX_GPIO_Init);
    FuriMessageQueue *event_queue =
        furi_message_queue_alloc(8, sizeof(InputEvent));
    /*
       HAL_SYSCFG_VREFBUF_VoltageScalingConfig(SYSCFG_VREFBUF_VOLTAGE_SCALE1);
       HAL_SYSCFG_EnableVREFBUF();
     */
    VREFBUF->CSR |= VREFBUF_CSR_ENVR;

    VREFBUF->CSR &= ~VREFBUF_CSR_HIZ;

    VREFBUF->CSR |= VREFBUF_CSR_VRS;

    while (!(VREFBUF->CSR & VREFBUF_CSR_VRR)) {
    };

    MX_ADC1_Init();

    //Initialize LED on board 
    if (HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED) != HAL_OK) {
        Error_Handler();
    }

    int i = 0;
    // Configure view port
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
        if (furi_message_queue_get(event_queue, &event, 100) ==
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

        ADC_ChannelConfTypeDef sConfig = { 0 };
        sConfig.Channel = ADC_CHANNEL_1;
        sConfig.Rank = ADC_REGULAR_RANK_1;
        sConfig.SamplingTime = ADC_SAMPLETIME_640CYCLES_5;      ////ADC_SAMPLETIME_640CYCLES_5;
        sConfig.SingleDiff = ADC_SINGLE_ENDED;
        sConfig.OffsetNumber = ADC_OFFSET_NONE;
        sConfig.Offset = 0;

        if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
            Error_Handler();
        }

        /* Start ADC group regular conversion */
        if (HAL_ADC_Start(&hadc1) != HAL_OK) {
            /* ADC conversion start error */
            Error_Handler();
        }

        /* Wait till conversion is done */
        if (HAL_ADC_PollForConversion(&hadc1, 1000) != HAL_OK) {
            /* End Of Conversion flag not set on time */
            Error_Handler();
        } else {
            /* Retrieve ADC conversion data */
            uhADCxConvertedData = HAL_ADC_GetValue(&hadc1);

            /* Computation of ADC conversions raw data to physical values           */
            /* using helper macro.                                                  */
            uhADCxConvertedData_Voltage_mVolt =
                __ADC_CALC_DATA_VOLTAGE(VDDA_APPLI, uhADCxConvertedData);
            i++;
            val1 = uhADCxConvertedData_Voltage_mVolt;   // + i;
        }

        /* Start ADC group regular conversion */
        if (HAL_ADC_Stop(&hadc1) != HAL_OK) {
            /* ADC conversion start error */
            Error_Handler();
        }

        view_port_update(view_port);
    }

    view_port_enabled_set(view_port, false);
    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_message_queue_free(event_queue);

    furi_record_close(RECORD_GUI);

    return 0;
}
