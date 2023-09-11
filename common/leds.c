#include "leds.h"

const GpioPin* const pin_leds = &gpio_ext_pc3;
const GpioPin* const pin_status_led = &gpio_ext_pa7;
#define LED_COUNT 4
uint32_t leds_color[LED_COUNT] = {0};

#define DWT_CYCCNT (0xE0001004UL)
typedef struct {
    volatile uint32_t COUNT; /*!< E0001000 + Offset: 0x004 (R/W)  Cycle Count Register */
} DWT_Internal;
#define DWT_ACCESS ((DWT_Internal*)DWT_CYCCNT)

void flipboard_leds_reset(void* leds) {
    leds_color[0] = 0x2000FF;
    leds_color[1] = 0xFF0000;
    leds_color[2] = 0x00FF00;
    leds_color[3] = 0x0000FF;
    flipboard_leds_update(leds);
}

void* flipboard_leds_alloc() {
    furi_hal_gpio_init_simple(pin_leds, GpioModeOutputPushPull);
    furi_hal_gpio_init_simple(pin_status_led, GpioModeOutputPushPull);

    flipboard_leds_reset(NULL);
    return NULL;
}

void flipboard_leds_free(void* leds) {
    UNUSED(leds);
    for(int i = 0; i < LED_COUNT; i++) {
        flipboard_leds_set(leds, i, 0);
    }
    flipboard_leds_update(leds);

    furi_hal_gpio_init_simple(pin_leds, GpioModeAnalog);
    furi_hal_gpio_init_simple(pin_status_led, GpioModeAnalog);
}

void flipboard_leds_set(void* leds, uint32_t led, uint32_t color) {
    UNUSED(leds);
    leds_color[led] = color;
}

void flipboard_leds_update(void* leds) {
    UNUSED(leds);

    furi_hal_gpio_write(pin_leds, false);
    uint8_t bits[4 * 8 * 3 * 2];
    for(int led = 0, i = 0; led < 4; led++) {
        uint32_t data = leds_color[led];
        UNUSED(data);
        uint32_t mask = 0x008000U;
        for(int j = 0; j < 8; j++) {
            bits[i++] = (data & mask) ? 38 : 14;
            bits[i++] = (data & mask) ? 2 : 20;
            mask >>= 1;
        }
        mask = 0x800000U;
        for(int j = 0; j < 8; j++) {
            bits[i++] = (data & mask) ? 38 : 14;
            bits[i++] = (data & mask) ? 2 : 20;
            mask >>= 1;
        }
        mask = 0x000080U;
        for(int j = 0; j < 8; j++) {
            bits[i++] = (data & mask) ? 38 : 14;
            bits[i++] = (data & mask) ? 2 : 20;
            mask >>= 1;
        }
    }
    furi_delay_us(50);

    furi_kernel_lock();
    size_t i = 0;
    while(i < COUNT_OF(bits)) {
        uint32_t delay = bits[i++];
        furi_hal_gpio_write(pin_leds, true);
        uint32_t start = DWT_ACCESS->COUNT;
        while((DWT_ACCESS->COUNT - start) < delay) {
        }

        furi_hal_gpio_write(pin_leds, false);
        delay = bits[i++];
        start = DWT_ACCESS->COUNT;
        while((DWT_ACCESS->COUNT - start) < delay) {
        }
    }
    furi_kernel_unlock();
    furi_delay_ms(50);
    furi_hal_gpio_write(&gpio_ext_pa7, !furi_hal_gpio_read(&gpio_ext_pa7));
}