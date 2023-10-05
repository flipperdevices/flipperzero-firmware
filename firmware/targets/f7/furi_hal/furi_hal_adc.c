#include <furi.h>
#include <furi_hal.h>
#include <stm32wbxx_ll_adc.h>
#include <stm32wbxx_ll_pwr.h>

static FuriSemaphore* adc_lock = NULL;

void furi_hal_adc_init() {
    adc_lock = furi_semaphore_alloc(1, 1);
}

FuriHalAdcHandle* furi_hal_adc_aquire(size_t timeout) {
    furi_check(adc_lock);
    if(furi_semaphore_acquire(adc_lock, timeout) == FuriStatusOk) {
        furi_hal_bus_enable(FuriHalBusADC);
        return (FuriHalAdcHandle*)ADC1;
    } else {
        return NULL;
    }
}

void furi_hal_adc_release(FuriHalAdcHandle* handle) {
    furi_check(handle);
    furi_check(handle == (FuriHalAdcHandle*)ADC1);
    furi_hal_bus_disable(FuriHalBusADC);
    furi_semaphore_release(adc_lock);
}