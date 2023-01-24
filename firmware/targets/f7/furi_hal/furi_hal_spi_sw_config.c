#include <furi_hal_spi_sw_config.h>
#include <furi_hal_resources.h>

// TODO: may be not needed
FuriMutex* furi_hal_spi_sw_bus_mutex = NULL;

static void furi_hal_spi_sw_bus_event_callback(FuriHalSpiBus* bus, FuriHalSpiBusEvent event) {
    if(event == FuriHalSpiBusEventInit) {
        furi_hal_spi_sw_bus_mutex = furi_mutex_alloc(FuriMutexTypeNormal);
        bus->current_handle = NULL;
    } else if(event == FuriHalSpiBusEventDeinit) {
        furi_mutex_free(furi_hal_spi_sw_bus_mutex);
    } else if(event == FuriHalSpiBusEventLock) {
        furi_check(furi_mutex_acquire(furi_hal_spi_sw_bus_mutex, FuriWaitForever) == FuriStatusOk);
    } else if(event == FuriHalSpiBusEventUnlock) {
        furi_check(furi_mutex_release(furi_hal_spi_sw_bus_mutex) == FuriStatusOk);
    } else if(event == FuriHalSpiBusEventActivate) {
    } else if(event == FuriHalSpiBusEventDeactivate) {
    }
}

FuriHalSpiBus furi_hal_spi_sw_bus = {
    .spi = NULL,
    .callback = furi_hal_spi_sw_bus_event_callback,
};

static void furi_hal_spi_sw_bus_handle_subghz_event_callback(
    FuriHalSpiBusHandle* handle,
    FuriHalSpiBusHandleEvent event) {
    if(event == FuriHalSpiBusHandleEventInit) {
        furi_hal_gpio_write(handle->cs, true);
        furi_hal_gpio_init(handle->cs, GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh);
    } else if(event == FuriHalSpiBusHandleEventDeinit) {
        furi_hal_gpio_write(handle->cs, true);
        furi_hal_gpio_init(handle->cs, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    } else if(event == FuriHalSpiBusHandleEventActivate) {
        furi_hal_gpio_init(handle->miso, GpioModeInput, GpioPullNo, GpioSpeedVeryHigh);
        furi_hal_gpio_init(handle->mosi, GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh);
        furi_hal_gpio_init(handle->sck, GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh);

        furi_hal_gpio_write(handle->cs, false);
    } else if(event == FuriHalSpiBusHandleEventDeactivate) {
        furi_hal_gpio_write(handle->cs, true);

        furi_hal_gpio_init(handle->miso, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
        furi_hal_gpio_init(handle->mosi, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
        furi_hal_gpio_init(handle->sck, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    }
}

FuriHalSpiBusHandle furi_hal_spi_sw_bus_handle_subghz_ext = {
    .bus = &furi_hal_spi_sw_bus,
    .callback = furi_hal_spi_sw_bus_handle_subghz_event_callback,
    .miso = &gpio_ext_pa6,
    .mosi = &gpio_ext_pa7,
    .sck = &gpio_ext_pa4,
    .cs = &gpio_ext_pb2,
};
