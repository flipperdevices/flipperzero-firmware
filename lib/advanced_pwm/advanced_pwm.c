#include "advanced_pwm.h"

#include <furi.h>
#include <furi_hal.h>
#include <furi_hal_resources.h>
#include <math.h>

#include <stm32wbxx_ll_dma.h>
#include <stm32wbxx_ll_tim.h>

struct AdvancedPwmInternals {
    uint32_t gpio_buff[2];
    LL_DMA_InitTypeDef dma_config_gpio;
    LL_DMA_InitTypeDef dma_config_timer;
};

#define TAG "AdvancedPwm"

#define F_TIM (64000000.0)
#define T_TIM 1562 /* 15.625 ns *100 */
#define T_TIM_DIV2 781 /* 15.625 ns / 2 *100 */

AdvancedPwm* advanced_pwm_alloc() {
    AdvancedPwm* signal = malloc(sizeof(AdvancedPwm));
    signal->polarity = true;

    signal->internals = malloc(sizeof(AdvancedPwmInternals));
    signal->internals->dma_config_gpio.Direction = LL_DMA_DIRECTION_MEMORY_TO_PERIPH;
    signal->internals->dma_config_gpio.Mode = LL_DMA_MODE_CIRCULAR;
    signal->internals->dma_config_gpio.PeriphOrM2MSrcIncMode = LL_DMA_PERIPH_NOINCREMENT;
    signal->internals->dma_config_gpio.MemoryOrM2MDstIncMode = LL_DMA_MEMORY_INCREMENT;
    signal->internals->dma_config_gpio.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_WORD;
    signal->internals->dma_config_gpio.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_WORD;
    signal->internals->dma_config_gpio.NbData = 2;
    signal->internals->dma_config_gpio.PeriphRequest = LL_DMAMUX_REQ_TIM2_UP;
    signal->internals->dma_config_gpio.Priority = LL_DMA_PRIORITY_VERYHIGH;

    signal->internals->dma_config_timer.PeriphOrM2MSrcAddress = (uint32_t) & (TIM2->ARR);
    signal->internals->dma_config_timer.Direction = LL_DMA_DIRECTION_MEMORY_TO_PERIPH;
    signal->internals->dma_config_timer.Mode = LL_DMA_MODE_CIRCULAR;
    signal->internals->dma_config_timer.PeriphOrM2MSrcIncMode = LL_DMA_PERIPH_NOINCREMENT;
    signal->internals->dma_config_timer.MemoryOrM2MDstIncMode = LL_DMA_MEMORY_INCREMENT;
    signal->internals->dma_config_timer.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_WORD;
    signal->internals->dma_config_timer.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_WORD;
    signal->internals->dma_config_timer.NbData = 2;
    signal->internals->dma_config_timer.PeriphRequest = LL_DMAMUX_REQ_TIM2_UP;
    signal->internals->dma_config_timer.Priority = LL_DMA_PRIORITY_HIGH;

    return signal;
}

void advanced_pwm_free(AdvancedPwm* signal) {
    furi_assert(signal);

    if(!signal) {
        return;
    }

    free(signal->internals);
    free(signal);
}

void advanced_pwm_reset_gpio(AdvancedPwm* signal) {
    furi_assert(signal);

    if(!signal) {
        return;
    }

    signal->internals->gpio_buff[0] = 0;
    signal->internals->gpio_buff[1] = 0;
}

void advanced_pwm_add_gpio(AdvancedPwm* signal, const GpioPin* gpio) {
    furi_assert(signal);

    if(!signal || !gpio) {
        return;
    }

    signal->internals->dma_config_gpio.PeriphOrM2MSrcAddress = (uint32_t) & (gpio->port->BSRR);

    /* set up signal polarities */
    uint32_t bit_set = gpio->pin;
    uint32_t bit_reset = gpio->pin << 16;

    if(signal->polarity) {
        signal->internals->gpio_buff[0] |= bit_set;
        signal->internals->gpio_buff[1] |= bit_reset;
    } else {
        signal->internals->gpio_buff[0] |= bit_reset;
        signal->internals->gpio_buff[1] |= bit_set;
    }
}

void advanced_pwm_del_gpio(AdvancedPwm* signal, const GpioPin* gpio) {
    furi_assert(signal);

    if(!signal || !gpio) {
        return;
    }

    /* set up signal polarities */
    uint32_t bit_set = gpio->pin;
    uint32_t bit_reset = gpio->pin << 16;

    if(signal->polarity) {
        signal->internals->gpio_buff[0] &= ~bit_set;
        signal->internals->gpio_buff[1] &= ~bit_reset;
    } else {
        signal->internals->gpio_buff[0] &= ~bit_reset;
        signal->internals->gpio_buff[1] &= ~bit_set;
    }
}

void advanced_pwm_set_frequency(AdvancedPwm* signal, uint32_t freq, uint32_t duty_x100) {
    furi_assert(signal);

    if(!signal) {
        return;
    }
    uint64_t period = ADVANCED_PWM_NS(1000000000 / freq);

    uint32_t high = (uint32_t)((period * duty_x100) / 10000);
    uint32_t low = (uint32_t)(period - high);

    advanced_pwm_set_timings(signal, high, low);
}

void advanced_pwm_set_timings(AdvancedPwm* signal, uint32_t high, uint32_t low) {
    furi_assert(signal);

    if(!signal) {
        return;
    }
    signal->edge_timings[0] = MAX(1ULL, high);
    signal->edge_timings[1] = MAX(1ULL, low);

    advanced_pwm_prepare_arr(signal);
}

bool advanced_pwm_get_polarity(AdvancedPwm* signal) {
    furi_assert(signal);

    return signal->polarity;
}

void advanced_pwm_set_polarity(AdvancedPwm* signal, bool polarity) {
    furi_assert(signal);

    signal->polarity = polarity;

    advanced_pwm_prepare_arr(signal);
}

void advanced_pwm_prepare_arr(AdvancedPwm* signal) {
    furi_assert(signal);

    /* set up edge timings */
    for(size_t pos = 0; pos < 2; pos++) {
        uint32_t pulse_ticks = (signal->edge_timings[pos] + T_TIM_DIV2) / T_TIM;

        signal->reload_reg_buff[pos] = MAX(2ULL, pulse_ticks) - 1;
    }
}

static void advanced_pwm_stop_dma() {
    LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_1);
    LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_2);
    LL_DMA_ClearFlag_TC1(DMA1);
    LL_DMA_ClearFlag_TC2(DMA1);
}

static void advanced_pwm_stop_timer() {
    LL_TIM_DisableCounter(TIM2);
    LL_TIM_SetCounter(TIM2, 0);
}

static bool advanced_pwm_setup_dma(AdvancedPwm* signal) {
    furi_assert(signal);

    signal->internals->dma_config_gpio.MemoryOrM2MDstAddress =
        (uint32_t)signal->internals->gpio_buff;
    signal->internals->dma_config_timer.MemoryOrM2MDstAddress = (uint32_t)signal->reload_reg_buff;

    /* set up DMA channel 1 and 2 for GPIO and timer copy operations */
    LL_DMA_Init(DMA1, LL_DMA_CHANNEL_1, &signal->internals->dma_config_gpio);
    LL_DMA_Init(DMA1, LL_DMA_CHANNEL_2, &signal->internals->dma_config_timer);

    /* enable both DMA channels */
    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_1);
    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_2);

    return true;
}

static void advanced_pwm_setup_timer() {
    advanced_pwm_stop_timer();

    LL_TIM_SetCounterMode(TIM2, LL_TIM_COUNTERMODE_UP);
    LL_TIM_SetClockDivision(TIM2, LL_TIM_CLOCKDIVISION_DIV1);
    LL_TIM_SetPrescaler(TIM2, 0);
    LL_TIM_SetAutoReload(TIM2, 10);
    LL_TIM_SetCounter(TIM2, 0);
    LL_TIM_EnableUpdateEvent(TIM2);
    LL_TIM_EnableDMAReq_UPDATE(TIM2);
}

static void advanced_pwm_start_timer() {
    LL_TIM_GenerateEvent_UPDATE(TIM2);
    LL_TIM_EnableCounter(TIM2);
}

void advanced_pwm_start(AdvancedPwm* signal) {
    furi_assert(signal);

    advanced_pwm_prepare_arr(signal);

    advanced_pwm_setup_dma(signal);
    advanced_pwm_setup_timer();
    advanced_pwm_start_timer();
}

void advanced_pwm_stop(AdvancedPwm* signal) {
    furi_assert(signal);

    advanced_pwm_stop_timer();
    advanced_pwm_stop_dma();
}
