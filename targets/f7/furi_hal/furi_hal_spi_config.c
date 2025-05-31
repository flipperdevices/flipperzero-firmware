#include <furi_hal_spi_config.h>
#include <furi_hal_resources.h>    // Provides GPIO pin definitions
#include <furi_hal_spi.h>          // Provides SPI HAL functions and types
#include <furi_hal_bus.h>          // For bus clock enable/disable
#include <furi.h>

// Include necessary LL drivers
#include <stm32wbxx_ll_spi.h>
#include <stm32wbxx_ll_gpio.h>

#define TAG "FuriHalSpiConfig"

/* ======================== SPI Configuration Presets ======================== */
// (Presets remain the same)

const LL_SPI_InitTypeDef furi_hal_spi_preset_2edge_low_8m = {
    .TransferDirection = LL_SPI_FULL_DUPLEX,
    .Mode = LL_SPI_MODE_MASTER,
    .DataWidth = LL_SPI_DATAWIDTH_8BIT,
    .ClockPolarity = LL_SPI_POLARITY_LOW,
    .ClockPhase = LL_SPI_PHASE_2EDGE, // Mode 1
    .NSS = LL_SPI_NSS_SOFT,
    .BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV8, // ~8 MHz @ 64MHz Clock
    .BitOrder = LL_SPI_MSB_FIRST,
    .CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE,
    .CRCPoly = 7,
};

const LL_SPI_InitTypeDef furi_hal_spi_preset_1edge_low_8m = {
    .TransferDirection = LL_SPI_FULL_DUPLEX,
    .Mode = LL_SPI_MODE_MASTER,
    .DataWidth = LL_SPI_DATAWIDTH_8BIT,
    .ClockPolarity = LL_SPI_POLARITY_LOW,
    .ClockPhase = LL_SPI_PHASE_1EDGE, // Mode 0
    .NSS = LL_SPI_NSS_SOFT,
    .BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV8, // ~8 MHz @ 64MHz Clock
    .BitOrder = LL_SPI_MSB_FIRST,
    .CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE,
    .CRCPoly = 7,
};

const LL_SPI_InitTypeDef furi_hal_spi_preset_1edge_low_4m = {
    .TransferDirection = LL_SPI_FULL_DUPLEX,
    .Mode = LL_SPI_MODE_MASTER,
    .DataWidth = LL_SPI_DATAWIDTH_8BIT,
    .ClockPolarity = LL_SPI_POLARITY_LOW,
    .ClockPhase = LL_SPI_PHASE_1EDGE, // Mode 0
    .NSS = LL_SPI_NSS_SOFT,
    .BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV16, // ~4 MHz @ 64MHz Clock
    .BitOrder = LL_SPI_MSB_FIRST,
    .CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE,
    .CRCPoly = 7,
};

// Use the renamed preset _slow instead of _2m
const LL_SPI_InitTypeDef furi_hal_spi_preset_1edge_low_2m = {
    .TransferDirection = LL_SPI_FULL_DUPLEX,
    .Mode = LL_SPI_MODE_MASTER,
    .DataWidth = LL_SPI_DATAWIDTH_8BIT,
    .ClockPolarity = LL_SPI_POLARITY_LOW,
    .ClockPhase = LL_SPI_PHASE_1EDGE, // Mode 0
    .NSS = LL_SPI_NSS_SOFT,
    .BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV32, // ~250 kHz @ 64MHz Clock
    .BitOrder = LL_SPI_MSB_FIRST,
    .CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE,
    .CRCPoly = 7,
};

// Adjusted speed for SD fast preset
const LL_SPI_InitTypeDef furi_hal_spi_preset_1edge_low_16m = {
    .TransferDirection = LL_SPI_FULL_DUPLEX,
    .Mode = LL_SPI_MODE_MASTER,
    .DataWidth = LL_SPI_DATAWIDTH_8BIT,
    .ClockPolarity = LL_SPI_POLARITY_LOW,
    .ClockPhase = LL_SPI_PHASE_1EDGE, // Mode 0
    .NSS = LL_SPI_NSS_SOFT,
    .BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV4, // ~16 MHz @ 64MHz Clock
    .BitOrder = LL_SPI_MSB_FIRST,
    .CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE,
    .CRCPoly = 7,
};


/* ========================== SPI Bus Definition (Single Bus) =========================== */

// Define ONE bus for SPI1, keeping the name furi_hal_spi_bus_d
FuriMutex* furi_hal_spi_bus_d_mutex = NULL;

static void furi_hal_spi_bus_d_event_callback(FuriHalSpiBus* bus, FuriHalSpiBusEvent event) {
    furi_assert(bus->spi == SPI1);

    if(event == FuriHalSpiBusEventInit) {
        furi_hal_spi_bus_d_mutex = furi_mutex_alloc(FuriMutexTypeNormal);
        furi_check(furi_hal_spi_bus_d_mutex);
        bus->current_handle = NULL;
    } else if(event == FuriHalSpiBusEventDeinit) {
        furi_mutex_free(furi_hal_spi_bus_d_mutex);
        furi_hal_spi_bus_d_mutex = NULL;
    } else if(event == FuriHalSpiBusEventLock) {
        furi_check(furi_mutex_acquire(furi_hal_spi_bus_d_mutex, FuriWaitForever) == FuriStatusOk);
    } else if(event == FuriHalSpiBusEventUnlock) {
        furi_check(furi_mutex_release(furi_hal_spi_bus_d_mutex) == FuriStatusOk);
    } else if(event == FuriHalSpiBusEventActivate) {
        furi_hal_bus_enable(FuriHalBusSPI1);
    } else if(event == FuriHalSpiBusEventDeactivate) {
        furi_hal_bus_disable(FuriHalBusSPI1);
    }
}

// The single SPI bus structure for SPI1, named furi_hal_spi_bus_d
FuriHalSpiBus furi_hal_spi_bus_d = {
    .spi = SPI1,
    .callback = furi_hal_spi_bus_d_event_callback,
    .current_handle = NULL,
};

// For F7 target compatibility - furi_hal_spi_bus_r is an alias to furi_hal_spi_bus_d
// since F7 uses a single SPI1 bus for both radio and display peripherals
FuriHalSpiBus furi_hal_spi_bus_r = {
    .spi = SPI1,
    .callback = furi_hal_spi_bus_d_event_callback,
    .current_handle = NULL,
};

/* ===================== SPI Bus Handle Event Callbacks ===================== */

#define SPI1_GPIO_ALT_FN LL_GPIO_AF_5

/**
 * @brief Generic event callback for standard SPI devices using SPI1 bus.
 */
inline static void furi_hal_spi_bus_generic_handle_event_callback(
    const FuriHalSpiBusHandle* handle,
    FuriHalSpiBusHandleEvent event,
    const LL_SPI_InitTypeDef* preset) {

    if(event == FuriHalSpiBusHandleEventInit) {
        furi_hal_gpio_write(handle->cs, true);
        furi_hal_gpio_init(handle->cs, GpioModeOutputPushPull, GpioPullUp, GpioSpeedVeryHigh);
    } else if(event == FuriHalSpiBusHandleEventDeinit) {
        furi_hal_gpio_write(handle->cs, true);
        furi_hal_gpio_init(handle->cs, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    } else if(event == FuriHalSpiBusHandleEventActivate) {
        LL_SPI_Init(handle->bus->spi, (LL_SPI_InitTypeDef*)preset);
        // LL_SPI_SetRxFIFOThreshold(handle->bus->spi, LL_SPI_RX_FIFO_TH_QUARTER); // Commented out
        LL_SPI_Enable(handle->bus->spi);

        furi_hal_gpio_init_ex(handle->miso, GpioModeAltFunctionPushPull, GpioPullUp, GpioSpeedVeryHigh, SPI1_GPIO_ALT_FN);
        furi_hal_gpio_init_ex(handle->mosi, GpioModeAltFunctionPushPull, GpioPullUp, GpioSpeedVeryHigh, SPI1_GPIO_ALT_FN);
        furi_hal_gpio_init_ex(handle->sck, GpioModeAltFunctionPushPull, GpioPullUp, GpioSpeedVeryHigh, SPI1_GPIO_ALT_FN);

        furi_hal_gpio_write(handle->cs, false);
    } else if(event == FuriHalSpiBusHandleEventDeactivate) {
        furi_hal_gpio_write(handle->cs, true);
        while(LL_SPI_IsActiveFlag_BSY(handle->bus->spi)) {};
        LL_SPI_Disable(handle->bus->spi);
        furi_hal_gpio_init(handle->miso, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
        furi_hal_gpio_init(handle->mosi, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
        furi_hal_gpio_init(handle->sck, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    }
}

/**
 * @brief Specific callback for NFC handle (ST25R3916).
 */
inline static void furi_hal_spi_bus_nfc_handle_event_callback(
    const FuriHalSpiBusHandle* handle,
    FuriHalSpiBusHandleEvent event,
    const LL_SPI_InitTypeDef* preset) {
    // ... (implementation as before) ...
    if(event == FuriHalSpiBusHandleEventInit) {
        furi_hal_gpio_write(handle->cs, true);
        furi_hal_gpio_init(handle->cs, GpioModeOutputPushPull, GpioPullUp, GpioSpeedVeryHigh);
    } else if(event == FuriHalSpiBusHandleEventDeinit) {
        furi_hal_gpio_write(handle->cs, true);
        furi_hal_gpio_init(handle->cs, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
        furi_hal_gpio_init(handle->miso, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
        furi_hal_gpio_init(handle->mosi, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
        furi_hal_gpio_init(handle->sck, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    } else if(event == FuriHalSpiBusHandleEventActivate) {
        LL_SPI_Init(handle->bus->spi, (LL_SPI_InitTypeDef*)preset);
        LL_SPI_Enable(handle->bus->spi);
        furi_hal_gpio_init_ex(handle->miso, GpioModeAltFunctionPushPull, GpioPullUp, GpioSpeedVeryHigh, SPI1_GPIO_ALT_FN);
        furi_hal_gpio_init_ex(handle->mosi, GpioModeAltFunctionPushPull, GpioPullUp, GpioSpeedVeryHigh, SPI1_GPIO_ALT_FN);
        furi_hal_gpio_init_ex(handle->sck, GpioModeAltFunctionPushPull, GpioPullUp, GpioSpeedVeryHigh, SPI1_GPIO_ALT_FN);
        furi_hal_gpio_write(handle->cs, false);
    } else if(event == FuriHalSpiBusHandleEventDeactivate) {
        furi_hal_gpio_write(handle->cs, true);
        while(LL_SPI_IsActiveFlag_BSY(handle->bus->spi)) {};
        LL_SPI_Disable(handle->bus->spi);
        furi_hal_gpio_init(handle->miso, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
        furi_hal_gpio_init(handle->mosi, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
        furi_hal_gpio_init(handle->sck, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    }
}

/**
 * @brief Event callback for external SPI handles.
 * Assumes shared SPI pins but potentially different GPIO pull settings if needed.
 * Defined before its wrapper.
 */
inline static void furi_hal_spi_bus_external_handle_event_callback(
    const FuriHalSpiBusHandle* handle,
    FuriHalSpiBusHandleEvent event,
    const LL_SPI_InitTypeDef* preset) {

    if(event == FuriHalSpiBusHandleEventInit) {
        // Initialize CS pin as Output Push-Pull, HIGH (inactive) with PullUp
        furi_hal_gpio_write(handle->cs, true);
        furi_hal_gpio_init(handle->cs, GpioModeOutputPushPull, GpioPullUp, GpioSpeedVeryHigh);
    } else if(event == FuriHalSpiBusHandleEventDeinit) {
        // Reset CS pin to Analog state
        furi_hal_gpio_write(handle->cs, true);
        furi_hal_gpio_init(handle->cs, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    } else if(event == FuriHalSpiBusHandleEventActivate) {
        // Configure and Enable SPI Peripheral
        LL_SPI_Init(handle->bus->spi, (LL_SPI_InitTypeDef*)preset);
        // LL_SPI_SetRxFIFOThreshold(handle->bus->spi, LL_SPI_RX_FIFO_TH_QUARTER); // Commented out
        LL_SPI_Enable(handle->bus->spi);

        // Configure GPIOs for SPI Function - Using GpioPullUp like generic callback
        furi_hal_gpio_init_ex(handle->miso, GpioModeAltFunctionPushPull, GpioPullUp, GpioSpeedVeryHigh, SPI1_GPIO_ALT_FN);
        furi_hal_gpio_init_ex(handle->mosi, GpioModeAltFunctionPushPull, GpioPullUp, GpioSpeedVeryHigh, SPI1_GPIO_ALT_FN);
        furi_hal_gpio_init_ex(handle->sck, GpioModeAltFunctionPushPull, GpioPullUp, GpioSpeedVeryHigh, SPI1_GPIO_ALT_FN);

        // Assert Chip Select
        furi_hal_gpio_write(handle->cs, false);
    } else if(event == FuriHalSpiBusHandleEventDeactivate) {
        // De-assert Chip Select
        furi_hal_gpio_write(handle->cs, true);

        // Disable SPI Peripheral
        while(LL_SPI_IsActiveFlag_BSY(handle->bus->spi)) {};
        LL_SPI_Disable(handle->bus->spi);

        // Reset GPIOs to Analog
        furi_hal_gpio_init(handle->miso, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
        furi_hal_gpio_init(handle->mosi, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
        furi_hal_gpio_init(handle->sck, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    }
}


// --- Wrapper Callbacks ---

 void furi_hal_spi_bus_handle_display_event_callback(
    const FuriHalSpiBusHandle* handle,
    FuriHalSpiBusHandleEvent event) {
    furi_hal_spi_bus_generic_handle_event_callback(handle, event, &furi_hal_spi_preset_1edge_low_4m);
}

static void furi_hal_spi_bus_handle_sd_fast_event_callback(
    const FuriHalSpiBusHandle* handle,
    FuriHalSpiBusHandleEvent event) {
    furi_hal_spi_bus_generic_handle_event_callback(handle, event, &furi_hal_spi_preset_1edge_low_16m);
}

static void furi_hal_spi_bus_handle_sd_slow_event_callback(
    const FuriHalSpiBusHandle* handle,
    FuriHalSpiBusHandleEvent event) {
    furi_hal_spi_bus_generic_handle_event_callback(handle, event, &furi_hal_spi_preset_1edge_low_2m);
}

static void furi_hal_spi_bus_handle_subghz_event_callback(
    const FuriHalSpiBusHandle* handle,
    FuriHalSpiBusHandleEvent event) {
    furi_hal_spi_bus_generic_handle_event_callback(handle, event, &furi_hal_spi_preset_1edge_low_8m);
}

static void furi_hal_spi_bus_handle_nfc_wrapper_event_callback(
    const FuriHalSpiBusHandle* handle,
    FuriHalSpiBusHandleEvent event) {
    furi_hal_spi_bus_nfc_handle_event_callback(handle, event, &furi_hal_spi_preset_2edge_low_8m);
}

// Wrapper for External handles - calls the specific external callback implementation
static void furi_hal_spi_bus_handle_external_wrapper_event_callback(
    const FuriHalSpiBusHandle* handle,
    FuriHalSpiBusHandleEvent event) {
    furi_hal_spi_bus_external_handle_event_callback(handle, event, &furi_hal_spi_preset_1edge_low_2m);
}


/* ======================== SPI Bus Handle Definitions ======================= */

const FuriHalSpiBusHandle furi_hal_spi_bus_handle_display = {
    .bus = &furi_hal_spi_bus_d,
    .callback = furi_hal_spi_bus_handle_display_event_callback,
    .miso = &gpio_spi_d_miso,
    .mosi = &gpio_spi_d_mosi,
    .sck = &gpio_spi_d_sck,
    .cs = &gpio_display_cs,
};

const FuriHalSpiBusHandle furi_hal_spi_bus_handle_sd_fast = {
    .bus = &furi_hal_spi_bus_d,
    .callback = furi_hal_spi_bus_handle_sd_fast_event_callback,
    .miso = &gpio_spi_d_miso,
    .mosi = &gpio_spi_d_mosi,
    .sck = &gpio_spi_d_sck,
    .cs = &gpio_sdcard_cs,
};

const FuriHalSpiBusHandle furi_hal_spi_bus_handle_sd_slow = {
    .bus = &furi_hal_spi_bus_d,
    .callback = furi_hal_spi_bus_handle_sd_slow_event_callback,
    .miso = &gpio_spi_d_miso,
    .mosi = &gpio_spi_d_mosi,
    .sck = &gpio_spi_d_sck,
    .cs = &gpio_sdcard_cs,
};

const FuriHalSpiBusHandle furi_hal_spi_bus_handle_subghz = {
    .bus = &furi_hal_spi_bus_d,
    .callback = furi_hal_spi_bus_handle_subghz_event_callback,
    .miso = &gpio_spi_d_miso,
    .mosi = &gpio_spi_d_mosi,
    .sck = &gpio_spi_d_sck,
    .cs = &gpio_subghz_cs,
};

const FuriHalSpiBusHandle furi_hal_spi_bus_handle_nfc = {
    .bus = &furi_hal_spi_bus_d,
    .callback = furi_hal_spi_bus_handle_nfc_wrapper_event_callback,
    .miso = &gpio_spi_d_miso,
    .mosi = &gpio_spi_d_mosi,
    .sck = &gpio_spi_d_sck,
    .cs = &gpio_nfc_cs,
};

// External Handles reinstated - pointing to shared SPI pins
const FuriHalSpiBusHandle furi_hal_spi_bus_handle_external = {
    .bus = &furi_hal_spi_bus_d,
    .callback = furi_hal_spi_bus_handle_external_wrapper_event_callback, // Use wrapper
    .miso = &gpio_spi_d_miso, // Use shared pin
    .mosi = &gpio_spi_d_mosi, // Use shared pin
    .sck = &gpio_spi_d_sck,   // Use shared pin
    .cs = &gpio_ext_pa4,      // Ensure this pin is defined and unique
};

const FuriHalSpiBusHandle furi_hal_spi_bus_handle_external_extra = {
    .bus = &furi_hal_spi_bus_d,
    .callback = furi_hal_spi_bus_handle_external_wrapper_event_callback, // Use wrapper
    .miso = &gpio_spi_d_miso, // Use shared pin
    .mosi = &gpio_spi_d_mosi, // Use shared pin
    .sck = &gpio_spi_d_sck,   // Use shared pin
    .cs = &gpio_ext_pc3,      // Ensure this pin is defined and unique
};


/* ====================== Initialization Functions ======================= */

void furi_hal_spi_config_init_early(void) {
    furi_hal_spi_bus_init(&furi_hal_spi_bus_d);
    furi_hal_spi_bus_handle_init(&furi_hal_spi_bus_handle_display);
}

void furi_hal_spi_config_deinit_early(void) {
    furi_hal_spi_bus_handle_deinit(&furi_hal_spi_bus_handle_display);
    furi_hal_spi_bus_deinit(&furi_hal_spi_bus_d);
}

void furi_hal_spi_config_init(void) {
    furi_hal_spi_bus_handle_init(&furi_hal_spi_bus_handle_sd_slow);
    furi_hal_spi_bus_handle_init(&furi_hal_spi_bus_handle_sd_fast);
    furi_hal_spi_bus_handle_init(&furi_hal_spi_bus_handle_subghz);
    furi_hal_spi_bus_handle_init(&furi_hal_spi_bus_handle_nfc);
    // Initialize external handles
    furi_hal_spi_bus_handle_init(&furi_hal_spi_bus_handle_external);
    furi_hal_spi_bus_handle_init(&furi_hal_spi_bus_handle_external_extra);

    FURI_LOG_I(TAG, "Init OK");
}