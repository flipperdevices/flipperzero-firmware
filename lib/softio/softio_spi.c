#include "softio_spi.h"
#include <stm32wbxx_ll_tim.h>
#include <furi_hal_bus.h>
#include <furi_hal_gpio.h>
#include <furi_hal_resources.h>
#include <furi_hal_interrupt.h>

#define SOFTSPI_TIM        TIM17
#define SOFTSPI_TIM_BUS    FuriHalBusTIM17
#define SOFTSPI_TIM_IRQ    FuriHalInterruptIdTim1TrgComTim17
#define SOFTSPI_TIM_FQ_KHZ 64000UL

typedef struct {
    SoftIoSpiConfig* config;
    const uint8_t* tx_buffer;
    uint8_t* rx_buffer;
    FuriSemaphore* done_semaphore;
    uint8_t
        sck_level : 1; // <! which edge (1=rising, 0=falling) to transmit on; reception is done on the opposite edge
    uint8_t done         : 1;
    uint8_t out_level    : 1;
    uint8_t clk_polarity : 1;
    size_t size;
    uint8_t bit;
} SoftspiTimerIsrContext;

void softio_spi_acquire(SoftIoSpiConfig* config) {
    furi_check(config);
    furi_hal_gpio_write(config->cs, true);
    furi_hal_gpio_write(config->sck, config->clk_polarity);
    furi_hal_gpio_write(config->mosi, false);
    furi_hal_gpio_init(config->mosi, GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh);
    furi_hal_gpio_init(config->sck, GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh);
    furi_hal_gpio_init(config->miso, GpioModeInput, GpioPullNo, GpioSpeedVeryHigh);
    furi_hal_gpio_init(config->cs, GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh);
    furi_hal_gpio_write(config->cs, false);
}

void softio_spi_release(SoftIoSpiConfig* config) {
    furi_check(config);
    furi_hal_gpio_write(config->cs, true);
    furi_hal_gpio_init(config->cs, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    furi_hal_gpio_init(config->mosi, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    furi_hal_gpio_init(config->sck, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    furi_hal_gpio_init(config->miso, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
}

static void softio_spi_timer_isr(void* param) {
    if(!LL_TIM_IsActiveFlag_UPDATE(SOFTSPI_TIM)) return;

    do {
        SoftspiTimerIsrContext* context = param;

        furi_hal_gpio_write(context->config->sck, context->sck_level);
        if(context->done) break;

        if(context->sck_level == context->out_level) {
            // TX edge
            if(context->tx_buffer)
                furi_hal_gpio_write(
                    context->config->mosi, (*context->tx_buffer >> context->bit) & 1);
        } else {
            // RX edge
            if(context->rx_buffer)
                *context->rx_buffer |= furi_hal_gpio_read(context->config->miso) << context->bit;

            if(context->bit == 0) {
                // entire byte transmitted
                if(context->tx_buffer) context->tx_buffer++;
                if(context->rx_buffer) context->rx_buffer++;
                if(!--context->size) {
                    furi_semaphore_release(context->done_semaphore);
                    context->done = 1;
                } else {
                    context->bit = 7;
                }
            } else {
                context->bit--;
            }
        }

        context->sck_level = !context->sck_level;
    } while(0);

    LL_TIM_ClearFlag_UPDATE(SOFTSPI_TIM);
}

void softio_spi_trx(
    SoftIoSpiConfig* config,
    const uint8_t* tx_buffer,
    uint8_t* rx_buffer,
    size_t size,
    uint32_t timeout) {
    furi_check(config);
    // NOTE: serious low-level optimization is needed in order to push this further
    // this check is here to prevent a lockup caused by never-ending timer interrupts
    furi_check(config->clk_fq_khz <= 200);
    if(size == 0) return;

    SoftspiTimerIsrContext context = {
        .config = config,
        .tx_buffer = tx_buffer,
        .rx_buffer = rx_buffer,
        .size = size,
        .bit = 7,
        .done_semaphore = furi_semaphore_alloc(1, 0),
        .sck_level = config->clk_polarity,
        .done = 0,
        .out_level = config->clk_polarity ^ config->clk_phase,
    };

    furi_hal_bus_enable(SOFTSPI_TIM_BUS);
    furi_hal_interrupt_set_isr_ex(
        SOFTSPI_TIM_IRQ, FuriHalInterruptPriorityHighest, softio_spi_timer_isr, &context);
    LL_TIM_SetPrescaler(SOFTSPI_TIM, 0);
    LL_TIM_SetCounterMode(SOFTSPI_TIM, LL_TIM_COUNTERMODE_UP);
    LL_TIM_SetAutoReload(
        SOFTSPI_TIM, SOFTSPI_TIM_FQ_KHZ / config->clk_fq_khz / 2); // f_ISR = 2 f_CLK
    LL_TIM_DisableARRPreload(SOFTSPI_TIM);
    LL_TIM_SetRepetitionCounter(SOFTSPI_TIM, 0);
    LL_TIM_SetClockDivision(SOFTSPI_TIM, LL_TIM_CLOCKDIVISION_DIV1);
    LL_TIM_SetClockSource(SOFTSPI_TIM, LL_TIM_CLOCKSOURCE_INTERNAL);
    LL_TIM_GenerateEvent_UPDATE(SOFTSPI_TIM);
    LL_TIM_EnableIT_UPDATE(SOFTSPI_TIM);
    LL_TIM_EnableCounter(SOFTSPI_TIM);

    furi_semaphore_acquire(context.done_semaphore, timeout);

    furi_hal_interrupt_set_isr(SOFTSPI_TIM_IRQ, NULL, NULL);
    furi_hal_bus_disable(SOFTSPI_TIM_BUS);
    furi_semaphore_free(context.done_semaphore);
}

void softio_spi_tx(SoftIoSpiConfig* config, const uint8_t* buffer, size_t size, uint32_t timeout) {
    furi_check(buffer);
    softio_spi_trx(config, buffer, NULL, size, timeout);
}

void softio_spi_rx(SoftIoSpiConfig* config, uint8_t* buffer, size_t size, uint32_t timeout) {
    furi_check(buffer);
    softio_spi_trx(config, NULL, buffer, size, timeout);
}
