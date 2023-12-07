#include <stm32wbxx_ll_dma.h>

#include "led_driver.h"

#define MAX_LED_COUNT 4
// We store the HIGH/LOW durations (2 values) for each color bit (24 bits per LED)
#define LED_DRIVER_BUFFER_SIZE (MAX_LED_COUNT * 2 * 24)
// We use a setinel value to figure out when the timer is complete.
#define LED_DRIVER_TIMER_SETINEL 0xFFU

/** 64 transitions per us @ 64MHz.  Our timing is in NANO_SECONDS */
#define LED_DRIVER_TIMER_NANOSECOND (1000U / (SystemCoreClock / 1000000U))
// Timings for WS2812B
#define LED_DRIVER_T0H 400U
#define LED_DRIVER_T1H 800U
#define LED_DRIVER_T0L 850U
#define LED_DRIVER_T1L 450U

// Wait for 35ms for the DMA to complete. NOTE: 1000 leds*(850ns+450ns)*24 = 32ms
#define LED_DRIVER_SETINEL_WAIT_MS 35

struct LedDriver {
    LL_DMA_InitTypeDef dma_gpio_update;
    LL_DMA_InitTypeDef dma_led_transition_timer;

    const GpioPin* gpio;
    uint32_t gpio_buf[2]; // On/Off for GPIO

    uint8_t timer_buffer[LED_DRIVER_BUFFER_SIZE + 2];
    uint32_t write_pos;
    uint32_t read_pos;

    uint32_t count_leds;
    uint32_t* led_data;
};

static void led_driver_init_dma_gpio_update(LedDriver* led_driver, const GpioPin* gpio) {
    led_driver->gpio = gpio;

    // Memory to Peripheral
    led_driver->dma_gpio_update.Direction = LL_DMA_DIRECTION_MEMORY_TO_PERIPH;
    // Peripheral (GPIO - We populate GPIO port's BSRR register)
    led_driver->dma_gpio_update.PeriphOrM2MSrcAddress = (uint32_t)&gpio->port->BSRR;
    led_driver->dma_gpio_update.PeriphOrM2MSrcIncMode = LL_DMA_PERIPH_NOINCREMENT;
    led_driver->dma_gpio_update.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_WORD;
    // Memory (State to set GPIO)
    led_driver->dma_gpio_update.MemoryOrM2MDstAddress = (uint32_t)led_driver->gpio_buf;
    led_driver->dma_gpio_update.MemoryOrM2MDstIncMode = LL_DMA_MEMORY_INCREMENT;
    led_driver->dma_gpio_update.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_WORD;
    // Data
    led_driver->dma_gpio_update.Mode = LL_DMA_MODE_CIRCULAR;
    led_driver->dma_gpio_update.NbData = 2; // We cycle between two (HIGH/LOW)values
    // When to perform data exchange
    led_driver->dma_gpio_update.PeriphRequest = LL_DMAMUX_REQ_TIM2_UP;
    led_driver->dma_gpio_update.Priority = LL_DMA_PRIORITY_VERYHIGH;
}

static void led_driver_init_dma_led_transition_timer(LedDriver* led_driver) {
    // Timer that triggers based on user data.
    led_driver->dma_led_transition_timer.Direction = LL_DMA_DIRECTION_MEMORY_TO_PERIPH;
    // Peripheral (Timer - We populate TIM2's ARR register)
    led_driver->dma_led_transition_timer.PeriphOrM2MSrcAddress = (uint32_t)&TIM2->ARR;
    led_driver->dma_led_transition_timer.PeriphOrM2MSrcIncMode = LL_DMA_PERIPH_NOINCREMENT;
    led_driver->dma_led_transition_timer.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_WORD;
    // Memory (Timings)
    led_driver->dma_led_transition_timer.MemoryOrM2MDstAddress =
        (uint32_t)led_driver->timer_buffer;
    led_driver->dma_led_transition_timer.MemoryOrM2MDstIncMode = LL_DMA_MEMORY_INCREMENT;
    led_driver->dma_led_transition_timer.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_BYTE;
    // Data
    led_driver->dma_led_transition_timer.Mode = LL_DMA_MODE_NORMAL;
    led_driver->dma_led_transition_timer.NbData = LED_DRIVER_BUFFER_SIZE;
    // When to perform data exchange
    led_driver->dma_led_transition_timer.PeriphRequest = LL_DMAMUX_REQ_TIM2_UP;
    led_driver->dma_led_transition_timer.Priority = LL_DMA_PRIORITY_HIGH;
}

LedDriver* led_driver_alloc(int count_leds, const GpioPin* gpio) {
    furi_assert(gpio);
    furi_assert(count_leds && count_leds <= MAX_LED_COUNT);

    LedDriver* led_driver = malloc(sizeof(LedDriver));
    led_driver_init_dma_gpio_update(led_driver, gpio);
    led_driver_init_dma_led_transition_timer(led_driver);
    led_driver->led_data = malloc(MAX_LED_COUNT * sizeof(uint32_t));

    led_driver->count_leds = count_leds;

    return led_driver;
}

void led_driver_free(LedDriver* led_driver) {
    furi_assert(led_driver);

    free(led_driver->led_data);
    free(led_driver);
}

uint32_t led_driver_set_led(LedDriver* led_driver, uint32_t index, uint32_t rrggbb) {
    furi_assert(led_driver);
    if(index >= led_driver->count_leds) {
        return 0xFFFFFFFF;
    }

    uint32_t previous = led_driver->led_data[index];
    led_driver->led_data[index] = rrggbb;
    return previous;
}

uint32_t led_driver_get_led(LedDriver* led_driver, uint32_t index) {
    furi_assert(led_driver);
    if(index >= led_driver->count_leds) {
        return 0xFFFFFFFF;
    }

    return led_driver->led_data[index];
}

static void led_driver_start_dma(LedDriver* led_driver) {
    furi_assert(led_driver);

    LL_DMA_Init(DMA1, LL_DMA_CHANNEL_1, &led_driver->dma_gpio_update);
    LL_DMA_Init(DMA1, LL_DMA_CHANNEL_2, &led_driver->dma_led_transition_timer);

    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_1);
    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_2);
}

static void led_driver_start_timer() {
    furi_hal_bus_enable(FuriHalBusTIM2);

    LL_TIM_SetCounterMode(TIM2, LL_TIM_COUNTERMODE_UP);
    LL_TIM_SetClockDivision(TIM2, LL_TIM_CLOCKDIVISION_DIV1);
    LL_TIM_SetPrescaler(TIM2, 0);
    // Updated by led_driver->dma_led_transition_timer.PeriphOrM2MSrcAddress
    LL_TIM_SetAutoReload(TIM2, LED_DRIVER_TIMER_SETINEL);
    LL_TIM_SetCounter(TIM2, 0);

    LL_TIM_EnableCounter(TIM2);
    LL_TIM_EnableUpdateEvent(TIM2);
    LL_TIM_EnableDMAReq_UPDATE(TIM2);
    LL_TIM_GenerateEvent_UPDATE(TIM2);
}

static void led_driver_stop_timer() {
    LL_TIM_DisableCounter(TIM2);
    LL_TIM_DisableUpdateEvent(TIM2);
    LL_TIM_DisableDMAReq_UPDATE(TIM2);
    furi_hal_bus_disable(FuriHalBusTIM2);
}

static void led_driver_stop_dma() {
    LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_1);
    LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_2);
    LL_DMA_ClearFlag_TC1(DMA1);
    LL_DMA_ClearFlag_TC2(DMA1);
}

static void led_driver_spin_lock(LedDriver* led_driver) {
    const uint32_t prev_timer = DWT->CYCCNT;
    const uint32_t wait_time = LED_DRIVER_SETINEL_WAIT_MS * SystemCoreClock / 1000;

    do {
        /* Make sure it's started (allow 100 ticks), but then check for sentinel value. */
        if(TIM2->ARR == LED_DRIVER_TIMER_SETINEL && DWT->CYCCNT - prev_timer > 100) {
            break;
        }

        // 0xFF is fairly quick, make sure we didn't miss it.
        if((DWT->CYCCNT - prev_timer > wait_time)) {
            FURI_LOG_D(
                "Demo", "0xFF not found (ARR 0x%08lx, read %lu)", TIM2->ARR, led_driver->read_pos);
            led_driver->read_pos = led_driver->write_pos - 1;
            break;
        }
    } while(true);
}

static void led_driver_add_period_length(LedDriver* led_driver, uint32_t length) {
    led_driver->timer_buffer[led_driver->write_pos++] = length;
    led_driver->timer_buffer[led_driver->write_pos] = LED_DRIVER_TIMER_SETINEL;
}

static void led_driver_add_period(LedDriver* led_driver, uint16_t duration_ns) {
    furi_assert(led_driver);

    uint32_t reload_value = duration_ns / LED_DRIVER_TIMER_NANOSECOND;

    if(reload_value > 255) {
        FURI_LOG_E("Demo", "reload_value: %ld", reload_value);
    }
    furi_check(reload_value > 0);
    furi_check(reload_value < 256);

    led_driver_add_period_length(led_driver, reload_value - 1);
}

static void led_driver_add_color(LedDriver* led_driver, uint32_t rrggbb) {
    UNUSED(rrggbb);

    uint32_t ggrrbb = (rrggbb & 0xFF) | ((rrggbb & 0xFF00) << 8) | ((rrggbb & 0xFF0000) >> 8);

    for(int i = 23; i >= 0; i--) {
        if(ggrrbb & (1 << i)) {
            led_driver_add_period(led_driver, LED_DRIVER_T0L);
            led_driver_add_period(led_driver, LED_DRIVER_T1L);
        } else {
            led_driver_add_period(led_driver, LED_DRIVER_T0H);
            led_driver_add_period(led_driver, LED_DRIVER_T1H);
        }
    }
}

void led_driver_transmit(LedDriver* led_driver) {
    furi_assert(led_driver);

    furi_assert(!led_driver->read_pos);
    furi_assert(!led_driver->write_pos);

    furi_hal_gpio_init(led_driver->gpio, GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh);
    furi_hal_gpio_write(led_driver->gpio, false);

    const uint32_t bit_set = led_driver->gpio->pin << GPIO_BSRR_BS0_Pos;
    const uint32_t bit_reset = led_driver->gpio->pin << GPIO_BSRR_BR0_Pos;

    // Always start with HIGH
    led_driver->gpio_buf[0] = bit_set;
    led_driver->gpio_buf[1] = bit_reset;

    for(size_t i = 0; i < LED_DRIVER_BUFFER_SIZE; i++) {
        led_driver->timer_buffer[i] = LED_DRIVER_TIMER_SETINEL;
    }
    for(size_t i = 0; i < led_driver->count_leds; i++) {
        led_driver_add_color(led_driver, led_driver->led_data[i]);
    }
    led_driver->dma_led_transition_timer.NbData = led_driver->write_pos + 1;

    FURI_CRITICAL_ENTER();

    led_driver_start_dma(led_driver);
    led_driver_start_timer();

    led_driver_spin_lock(led_driver);

    led_driver_stop_timer();
    led_driver_stop_dma();

    FURI_CRITICAL_EXIT();

    memset(led_driver->timer_buffer, LED_DRIVER_TIMER_SETINEL, LED_DRIVER_BUFFER_SIZE);
    led_driver->read_pos = 0;
    led_driver->write_pos = 0;
}

/*
int32_t main_led_test(void* _p) {
    UNUSED(_p);

    uint16_t num_leds = MAX_LED_COUNT;
    LedDriver* led_driver = led_driver_alloc(num_leds, &gpio_ext_pc3);

    uint32_t* data[80];
    for(int i = 0; i < 80; i++) {
        data[i] = malloc(16 * 16 * sizeof(uint32_t));
    }

    for(int j = 0; j < num_leds; j++) {
        uint8_t red = rand() % 2;
        uint8_t green = rand() % 4;
        uint8_t blue = rand() % 4;
        data[0][j] = red << 16 | green << 8 | blue;
    }
    data[0][0] = 0x000F00;

    for(int i = 1; i < 80; i++) {
        for(int j = 1; j < num_leds; j++) {
            uint8_t red = rand() % 2;
            uint8_t green = rand() % 4;
            uint8_t blue = rand() % 4;
            data[i][j] = red << 16 | green << 8 | blue;
            data[i][j] = data[i - 1][j - 1];
        }
        data[i][0] = data[i - 1][num_leds - 1];
        // for(int j = 0; j < num_leds; j++) {
        //     if(data[i - 1][j] == 0x000F00) {
        //         data[i][j] = 0x000F00;
        //     }
        // }
        data[i][rand() % num_leds] = 0x000F00;
    }

    int counter = 0;
    while(true) {
        uint32_t i = counter++ % 80;
        for(int j = 0; j < num_leds; j++) {
            led_driver_set_led(led_driver, j, data[i][j]);
        }
        led_driver_transmit(led_driver);
        furi_delay_ms(20);
    }

    for(int i = 0; i < 80; i++) {
        free(data[i]);
    }

    return 0;
}
*/