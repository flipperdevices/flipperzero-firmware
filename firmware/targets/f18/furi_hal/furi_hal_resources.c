#include <furi_hal_resources.h>
#include <furi.h>

#include <stm32wbxx_ll_rcc.h>
#include <stm32wbxx_ll_pwr.h>

const GpioPin vibro_gpio = {.port = GPIOA, .pin = LL_GPIO_PIN_8};
const GpioPin ibutton_gpio = {.port = GPIOB, .pin = LL_GPIO_PIN_14};

const GpioPin gpio_cc1101_g0 = {.port = GPIOA, .pin = LL_GPIO_PIN_1};
const GpioPin gpio_rf_sw_0 = {.port = GPIOC, .pin = LL_GPIO_PIN_4};

const GpioPin gpio_subghz_cs = {.port = GPIOD, .pin = LL_GPIO_PIN_0};
const GpioPin gpio_display_cs = {.port = GPIOC, .pin = LL_GPIO_PIN_11};
const GpioPin gpio_display_rst_n = {.port = GPIOB, .pin = LL_GPIO_PIN_0};
const GpioPin gpio_display_di = {.port = GPIOB, .pin = LL_GPIO_PIN_1};
const GpioPin gpio_sdcard_cs = {.port = GPIOC, .pin = LL_GPIO_PIN_12};
const GpioPin gpio_sdcard_cd = {.port = GPIOC, .pin = LL_GPIO_PIN_10};
const GpioPin gpio_nfc_cs = {.port = GPIOE, .pin = LL_GPIO_PIN_4};

const GpioPin gpio_button_up = {.port = GPIOB, .pin = LL_GPIO_PIN_10};
const GpioPin gpio_button_down = {.port = GPIOC, .pin = LL_GPIO_PIN_6};
const GpioPin gpio_button_right = {.port = GPIOB, .pin = LL_GPIO_PIN_12};
const GpioPin gpio_button_left = {.port = GPIOB, .pin = LL_GPIO_PIN_11};
const GpioPin gpio_button_ok = {.port = GPIOH, .pin = LL_GPIO_PIN_3};
const GpioPin gpio_button_back = {.port = GPIOC, .pin = LL_GPIO_PIN_13};

const GpioPin gpio_spi_d_miso = {.port = GPIOC, .pin = LL_GPIO_PIN_2};
const GpioPin gpio_spi_d_mosi = {.port = GPIOB, .pin = LL_GPIO_PIN_15};
const GpioPin gpio_spi_d_sck = {.port = GPIOD, .pin = LL_GPIO_PIN_1};
const GpioPin gpio_spi_r_miso = {.port = GPIOB, .pin = LL_GPIO_PIN_4};
const GpioPin gpio_spi_r_mosi = {.port = GPIOB, .pin = LL_GPIO_PIN_5};
const GpioPin gpio_spi_r_sck = {.port = GPIOA, .pin = LL_GPIO_PIN_5};

const GpioPin gpio_ext_pc0 = {.port = GPIOC, .pin = LL_GPIO_PIN_0};
const GpioPin gpio_ext_pc1 = {.port = GPIOC, .pin = LL_GPIO_PIN_1};
const GpioPin gpio_ext_pc3 = {.port = GPIOC, .pin = LL_GPIO_PIN_3};
const GpioPin gpio_ext_pb2 = {.port = GPIOB, .pin = LL_GPIO_PIN_2};
const GpioPin gpio_ext_pb3 = {.port = GPIOB, .pin = LL_GPIO_PIN_3};
const GpioPin gpio_ext_pa4 = {.port = GPIOA, .pin = LL_GPIO_PIN_4};
const GpioPin gpio_ext_pa6 = {.port = GPIOA, .pin = LL_GPIO_PIN_6};
const GpioPin gpio_ext_pa7 = {.port = GPIOA, .pin = LL_GPIO_PIN_7};

const GpioPin gpio_nfc_irq_rfid_pull = {.port = GPIOA, .pin = LL_GPIO_PIN_2};
const GpioPin gpio_rfid_carrier_out = {.port = GPIOB, .pin = LL_GPIO_PIN_13};
const GpioPin gpio_rfid_data_in = {.port = GPIOC, .pin = LL_GPIO_PIN_5};
const GpioPin gpio_rfid_carrier = {.port = GPIOA, .pin = LL_GPIO_PIN_15};

const GpioPin gpio_infrared_rx = {.port = GPIOA, .pin = LL_GPIO_PIN_0};
const GpioPin gpio_infrared_tx = {.port = GPIOB, .pin = LL_GPIO_PIN_9};

const GpioPin gpio_usart_tx = {.port = GPIOB, .pin = LL_GPIO_PIN_6};
const GpioPin gpio_usart_rx = {.port = GPIOB, .pin = LL_GPIO_PIN_7};

const GpioPin gpio_i2c_power_sda = {.port = GPIOA, .pin = LL_GPIO_PIN_10};
const GpioPin gpio_i2c_power_scl = {.port = GPIOA, .pin = LL_GPIO_PIN_9};

const GpioPin gpio_speaker = {.port = GPIOB, .pin = LL_GPIO_PIN_8};

const GpioPin periph_power = {.port = GPIOA, .pin = LL_GPIO_PIN_3};

const GpioPin gpio_usb_dm = {.port = GPIOA, .pin = LL_GPIO_PIN_11};
const GpioPin gpio_usb_dp = {.port = GPIOA, .pin = LL_GPIO_PIN_12};

const GpioPinRecord gpio_pins[] = {
    {.pin = &gpio_ext_pa7, .name = "PA7", .debug = false},
    {.pin = &gpio_ext_pa6, .name = "PA6", .debug = false},
    {.pin = &gpio_ext_pa4, .name = "PA4", .debug = false},
    {.pin = &gpio_ext_pb3, .name = "PB3", .debug = false},
    {.pin = &gpio_ext_pb2, .name = "PB2", .debug = false},
    {.pin = &gpio_ext_pc3, .name = "PC3", .debug = false},
    {.pin = &gpio_ext_pc1, .name = "PC1", .debug = false},
    {.pin = &gpio_ext_pc0, .name = "PC0", .debug = false},

    /* Dangerous pins, may damage hardware */
    {.pin = &gpio_usart_rx, .name = "PB7", .debug = true},
    {.pin = &gpio_speaker, .name = "PB8", .debug = true},
};

const size_t gpio_pins_count = sizeof(gpio_pins) / sizeof(GpioPinRecord);

const InputPin input_pins[] = {
    {.gpio = &gpio_button_up, .key = InputKeyUp, .inverted = true, .name = "Up"},
    {.gpio = &gpio_button_down, .key = InputKeyDown, .inverted = true, .name = "Down"},
    {.gpio = &gpio_button_right, .key = InputKeyRight, .inverted = true, .name = "Right"},
    {.gpio = &gpio_button_left, .key = InputKeyLeft, .inverted = true, .name = "Left"},
    {.gpio = &gpio_button_ok, .key = InputKeyOk, .inverted = false, .name = "OK"},
    {.gpio = &gpio_button_back, .key = InputKeyBack, .inverted = true, .name = "Back"},
};

const size_t input_pins_count = sizeof(input_pins) / sizeof(InputPin);

void furi_hal_resources_init_early() {
    furi_hal_gpio_init(&gpio_button_left, GpioModeInput, GpioPullUp, GpioSpeedLow);

    // SD Card stepdown control
    furi_hal_gpio_write(&periph_power, 1);
    furi_hal_gpio_init(&periph_power, GpioModeOutputOpenDrain, GpioPullNo, GpioSpeedLow);

    // Display pins
    furi_hal_gpio_write(&gpio_display_rst_n, 1);
    furi_hal_gpio_init_simple(&gpio_display_rst_n, GpioModeOutputPushPull);
    furi_hal_gpio_init_simple(&gpio_display_di, GpioModeOutputPushPull);

    // Alternative pull configuration for shutdown
    SET_BIT(PWR->PUCRB, LL_GPIO_PIN_0);
    CLEAR_BIT(PWR->PDCRB, LL_GPIO_PIN_0);
    SET_BIT(PWR->CR3, PWR_CR3_APC);

    // Hard reset USB
    furi_hal_gpio_write(&gpio_usb_dm, 1);
    furi_hal_gpio_write(&gpio_usb_dp, 1);
    furi_hal_gpio_init_simple(&gpio_usb_dm, GpioModeOutputOpenDrain);
    furi_hal_gpio_init_simple(&gpio_usb_dp, GpioModeOutputOpenDrain);
    furi_hal_gpio_write(&gpio_usb_dm, 0);
    furi_hal_gpio_write(&gpio_usb_dp, 0);
    furi_delay_us(5); // Device Driven disconnect: 2.5us + extra to compensate cables
    furi_hal_gpio_write(&gpio_usb_dm, 1);
    furi_hal_gpio_write(&gpio_usb_dp, 1);
    furi_hal_gpio_init_simple(&gpio_usb_dm, GpioModeAnalog);
    furi_hal_gpio_init_simple(&gpio_usb_dp, GpioModeAnalog);
    furi_hal_gpio_write(&gpio_usb_dm, 0);
    furi_hal_gpio_write(&gpio_usb_dp, 0);

    // External header pins
    furi_hal_gpio_init(&gpio_ext_pc0, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    furi_hal_gpio_init(&gpio_ext_pc1, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    furi_hal_gpio_init(&gpio_ext_pc3, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    furi_hal_gpio_init(&gpio_ext_pb2, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    furi_hal_gpio_init(&gpio_ext_pb3, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    furi_hal_gpio_init(&gpio_ext_pa4, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    furi_hal_gpio_init(&gpio_ext_pa6, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    furi_hal_gpio_init(&gpio_ext_pa7, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
}

void furi_hal_resources_deinit_early() {
}

void furi_hal_resources_init() {
    // Button pins
    for(size_t i = 0; i < input_pins_count; i++) {
        furi_hal_gpio_init(
            input_pins[i].gpio, GpioModeInterruptRiseFall, GpioPullUp, GpioSpeedLow);
    }

    // Display pins
    furi_hal_gpio_init(&gpio_display_rst_n, GpioModeOutputPushPull, GpioPullNo, GpioSpeedLow);
    furi_hal_gpio_write(&gpio_display_rst_n, 0);

    furi_hal_gpio_init(&gpio_display_di, GpioModeOutputPushPull, GpioPullNo, GpioSpeedLow);
    furi_hal_gpio_write(&gpio_display_di, 0);

    // SD pins
    furi_hal_gpio_init(&gpio_sdcard_cd, GpioModeInput, GpioPullNo, GpioSpeedLow);
    furi_hal_gpio_write(&gpio_sdcard_cd, 0);

    furi_hal_gpio_init(&vibro_gpio, GpioModeAnalog, GpioPullNo, GpioSpeedLow);

    furi_hal_gpio_init(&ibutton_gpio, GpioModeAnalog, GpioPullNo, GpioSpeedLow);

    furi_hal_gpio_init(&gpio_nfc_irq_rfid_pull, GpioModeInterruptRise, GpioPullNo, GpioSpeedLow);

    furi_hal_gpio_init(&gpio_rf_sw_0, GpioModeOutputPushPull, GpioPullNo, GpioSpeedLow);

    NVIC_SetPriority(EXTI0_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 5, 0));
    NVIC_EnableIRQ(EXTI0_IRQn);

    NVIC_SetPriority(EXTI1_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 5, 0));
    NVIC_EnableIRQ(EXTI1_IRQn);

    NVIC_SetPriority(EXTI2_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 5, 0));
    NVIC_EnableIRQ(EXTI2_IRQn);

    NVIC_SetPriority(EXTI3_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 5, 0));
    NVIC_EnableIRQ(EXTI3_IRQn);

    NVIC_SetPriority(EXTI4_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 5, 0));
    NVIC_EnableIRQ(EXTI4_IRQn);

    NVIC_SetPriority(EXTI9_5_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 5, 0));
    NVIC_EnableIRQ(EXTI9_5_IRQn);

    NVIC_SetPriority(EXTI15_10_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 5, 0));
    NVIC_EnableIRQ(EXTI15_10_IRQn);
}
