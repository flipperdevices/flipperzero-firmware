#include <limits.h>
#include <furi.h>
#include <furi_hal.h>
#include <furi_hal_gpio.h>
#include <stm32wbxx_ll_dma.h>
#include <stm32wbxx_ll_dmamux.h>
#include <stm32wbxx_ll_tim.h>
#include <stm32wbxx_ll_exti.h>

#include "pulse_reader.h"


#define GPIO_PIN_MAP(pin, prefix)               \
    (((pin) == (LL_GPIO_PIN_0))  ? prefix##0 :  \
     ((pin) == (LL_GPIO_PIN_1))  ? prefix##1 :  \
     ((pin) == (LL_GPIO_PIN_2))  ? prefix##2 :  \
     ((pin) == (LL_GPIO_PIN_3))  ? prefix##3 :  \
     ((pin) == (LL_GPIO_PIN_4))  ? prefix##4 :  \
     ((pin) == (LL_GPIO_PIN_5))  ? prefix##5 :  \
     ((pin) == (LL_GPIO_PIN_6))  ? prefix##6 :  \
     ((pin) == (LL_GPIO_PIN_7))  ? prefix##7 :  \
     ((pin) == (LL_GPIO_PIN_8))  ? prefix##8 :  \
     ((pin) == (LL_GPIO_PIN_9))  ? prefix##9 :  \
     ((pin) == (LL_GPIO_PIN_10)) ? prefix##10 : \
     ((pin) == (LL_GPIO_PIN_11)) ? prefix##11 : \
     ((pin) == (LL_GPIO_PIN_12)) ? prefix##12 : \
     ((pin) == (LL_GPIO_PIN_13)) ? prefix##13 : \
     ((pin) == (LL_GPIO_PIN_14)) ? prefix##14 : \
                                   prefix##15)

#define GET_DMAMUX_EXTI_LINE(pin) GPIO_PIN_MAP(pin, LL_DMAMUX_REQ_GEN_EXTI_LINE)



PulseReader* pulse_reader_alloc(const GpioPin* gpio, uint32_t size) {
    PulseReader* signal = malloc(sizeof(PulseReader));

    signal->timer_buffer = malloc(size * sizeof(uint32_t));
    signal->dma_channel = LL_DMA_CHANNEL_4;
    signal->gpio = gpio;
    signal->size = size;
    signal->timer_value = 0;
    signal->pos = 0;

    signal->unit = PulseReaderUnitPicosecond;
    signal->bit_time = 1;

    return signal;
}

void pulse_reader_set_timebase(PulseReader* signal, PulseReaderUnit unit) {
    signal->unit = unit;
}

void pulse_reader_set_bittime(PulseReader* signal, uint32_t bit_time) {
    signal->bit_time = bit_time;
}

void pulse_reader_free(PulseReader* signal) {
    free(signal->timer_buffer);
    free(signal);
}

uint32_t pulse_reader_samples(PulseReader* signal) {
    uint32_t dma_pos = signal->size - (uint32_t)LL_DMA_GetDataLength(DMA1, signal->dma_channel);

    return ((signal->pos + signal->size) - dma_pos) % signal->size;
}

void pulse_reader_stop(PulseReader* signal) {

    LL_DMA_DisableChannel(DMA1, signal->dma_channel);
    LL_DMAMUX_DisableRequestGen(NULL, LL_DMAMUX_REQ_GEN_0);
    LL_TIM_DisableCounter(TIM2);
}

void pulse_reader_start(PulseReader* signal) {

    memset(signal->timer_buffer, 0x00, signal->size * sizeof(uint32_t));

    /* configure DMA to read from a timer peripheral */
    LL_DMA_InitTypeDef dma_config = {};
    dma_config.Direction = LL_DMA_DIRECTION_PERIPH_TO_MEMORY;
    dma_config.PeriphOrM2MSrcAddress = (uint32_t) &(TIM2->CNT);
    dma_config.PeriphOrM2MSrcIncMode = LL_DMA_PERIPH_NOINCREMENT;
    dma_config.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_WORD;
    dma_config.MemoryOrM2MDstAddress = (uint32_t) signal->timer_buffer;
    dma_config.MemoryOrM2MDstIncMode = LL_DMA_MEMORY_INCREMENT;
    dma_config.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_WORD;
    dma_config.Mode = LL_DMA_MODE_CIRCULAR;
    dma_config.NbData = signal->size; /* executes LL_DMA_SetDataLength */
    dma_config.PeriphRequest = LL_DMAMUX_REQ_GENERATOR0; /* executes LL_DMA_SetPeriphRequest */
    dma_config.Priority = LL_DMA_PRIORITY_VERYHIGH;
    
    /* start counter */
    LL_TIM_DisableCounter(TIM2);
    LL_TIM_SetCounterMode(TIM2, LL_TIM_COUNTERMODE_UP);
    LL_TIM_SetClockDivision(TIM2, LL_TIM_CLOCKDIVISION_DIV1);
    LL_TIM_SetPrescaler(TIM2, 0);
    LL_TIM_SetAutoReload(TIM2, 0xFFFFFFFF);
    LL_TIM_SetCounter(TIM2, 0);
    LL_TIM_EnableCounter(TIM2);

    /* make sure request generation is disabled before modifying registers */
    LL_DMAMUX_DisableRequestGen(NULL, LL_DMAMUX_REQ_GEN_0);
    /* generator 0 gets fed by EXTI_LINEn */
    LL_DMAMUX_SetRequestSignalID(NULL, LL_DMAMUX_REQ_GEN_0, GET_DMAMUX_EXTI_LINE(signal->gpio->pin));
    /* trigger on rising edge of the interrupt */
    LL_DMAMUX_SetRequestGenPolarity(NULL, LL_DMAMUX_REQ_GEN_0, LL_DMAMUX_REQ_GEN_POL_RISING);
    /* now enable request generation again */
    LL_DMAMUX_EnableRequestGen(NULL, LL_DMAMUX_REQ_GEN_0);

    /* we need the EXTI to be configured as interrupt generating line, but no ISR registered */
    furi_hal_gpio_init_ex(signal->gpio, GpioModeInterruptRiseFall, GpioPullNo, GpioSpeedVeryHigh, GpioAltFnUnused);

    /* capture current timer */
    signal->pos = 0;
    signal->start_level = furi_hal_gpio_read(signal->gpio);
    signal->timer_value = TIM2->CNT;

    /* now set up DMA with these settings */
    LL_DMA_DisableChannel(DMA1, signal->dma_channel);
    LL_DMA_Init(DMA1, signal->dma_channel, &dma_config);
    LL_DMA_EnableChannel(DMA1, signal->dma_channel);
}

uint32_t pulse_reader_receive(PulseReader* signal, int timeout_us) {

    uint32_t start_time = DWT->CYCCNT;
    uint32_t timeout_ticks = timeout_us * (F_CPU/1000000);

    do {
        /* get the DMA's next write position by reading "remaining length" register */
        uint32_t dma_pos = signal->size - (uint32_t)LL_DMA_GetDataLength(DMA1, signal->dma_channel);

        /* the DMA has advanced in the ringbuffer */
        if(dma_pos != signal->pos) {

            uint32_t delta = signal->timer_buffer[signal->pos] - signal->timer_value;

            signal->timer_value = signal->timer_buffer[signal->pos];
            signal->pos++;
            signal->pos %= signal->size;
            
            uint32_t delta_unit = delta * signal->unit;
            uint32_t bits = (delta_unit + signal->bit_time / 2) / signal->bit_time;

            return bits;
        }

        /* check for timeout */
        uint32_t elapsed = DWT->CYCCNT - start_time;

        if(elapsed > timeout_ticks) {
            return PULSE_READER_NO_EDGE;
        }

        furi_delay_ms(0);

    } while(true);
}
