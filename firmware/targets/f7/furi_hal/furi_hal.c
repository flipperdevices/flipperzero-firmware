#include <furi_hal.h>

#include <stm32wbxx_ll_cortex.h>

#include <fatfs.h>

#define TAG "FuriHal"

static void gpio_init(void);

void furi_hal_init() {
    furi_hal_rtc_init();
    furi_hal_interrupt_init();
    furi_hal_delay_init();

    gpio_init();
    FURI_LOG_I(TAG, "GPIO OK");

    furi_hal_bootloader_init();
    furi_hal_version_init();

    furi_hal_spi_init();

    furi_hal_ibutton_init();
    FURI_LOG_I(TAG, "iButton OK");
    furi_hal_speaker_init();
    FURI_LOG_I(TAG, "Speaker OK");

    furi_hal_crypto_init();

    // VCP + USB
    furi_hal_usb_init();
    furi_hal_vcp_init();
    FURI_LOG_I(TAG, "USB OK");

    furi_hal_i2c_init();

    // High Level
    furi_hal_power_init();
    furi_hal_light_init();
    furi_hal_vibro_init();
    furi_hal_subghz_init();
    furi_hal_nfc_init();
    furi_hal_rfid_init();
    furi_hal_bt_init();
    furi_hal_compress_icon_init();

    // FreeRTOS glue
    furi_hal_os_init();

    // FatFS driver initialization
    MX_FATFS_Init();
    FURI_LOG_I(TAG, "FATFS OK");

    // Partial null pointer dereference protection
    LL_MPU_Disable();
    LL_MPU_ConfigRegion(
        LL_MPU_REGION_NUMBER0,
        0x00,
        0x0,
        LL_MPU_REGION_SIZE_1MB | LL_MPU_REGION_PRIV_RO_URO | LL_MPU_ACCESS_BUFFERABLE |
            LL_MPU_ACCESS_CACHEABLE | LL_MPU_ACCESS_SHAREABLE | LL_MPU_TEX_LEVEL1 |
            LL_MPU_INSTRUCTION_ACCESS_ENABLE);
    LL_MPU_Enable(LL_MPU_CTRL_PRIVILEGED_DEFAULT);
}

static void gpio_init(void) {
    // Button pins
    for(size_t i = 0; i < input_pins_count; i++) {
        furi_hal_gpio_init(
            input_pins[i].gpio, GpioModeInterruptRiseFall, GpioPullUp, GpioSpeedLow);
    }

    // External header pins
    furi_hal_gpio_init(&gpio_ext_pc0, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    furi_hal_gpio_init(&gpio_ext_pc1, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    furi_hal_gpio_init(&gpio_ext_pc3, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    furi_hal_gpio_init(&gpio_ext_pb2, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    furi_hal_gpio_init(&gpio_ext_pb3, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    furi_hal_gpio_init(&gpio_ext_pa4, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    furi_hal_gpio_init(&gpio_ext_pa6, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    furi_hal_gpio_init(&gpio_ext_pa7, GpioModeAnalog, GpioPullNo, GpioSpeedLow);

    // Display pins
    furi_hal_gpio_init(&gpio_display_rst, GpioModeOutputPushPull, GpioPullNo, GpioSpeedLow);
    furi_hal_gpio_write(&gpio_display_rst, 0);

    furi_hal_gpio_init(&gpio_display_di, GpioModeOutputPushPull, GpioPullNo, GpioSpeedLow);
    furi_hal_gpio_write(&gpio_display_di, 0);

    // SD pins
    furi_hal_gpio_init(&gpio_sdcard_cd, GpioModeInput, GpioPullNo, GpioSpeedLow);
    furi_hal_gpio_write(&gpio_sdcard_cd, 0);

    furi_hal_gpio_init(&vibro_gpio, GpioModeAnalog, GpioPullNo, GpioSpeedLow);

    furi_hal_gpio_init(&ibutton_gpio, GpioModeAnalog, GpioPullNo, GpioSpeedLow);

    furi_hal_gpio_init(&gpio_rfid_pull, GpioModeInterruptRise, GpioPullNo, GpioSpeedLow);

    furi_hal_gpio_init(&gpio_rf_sw_0, GpioModeOutputPushPull, GpioPullNo, GpioSpeedLow);

    furi_hal_gpio_init(&periph_power, GpioModeOutputOpenDrain, GpioPullNo, GpioSpeedLow);

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

void furi_hal_init_critical() {
    furi_hal_clock_init();
    furi_hal_console_init();
}
