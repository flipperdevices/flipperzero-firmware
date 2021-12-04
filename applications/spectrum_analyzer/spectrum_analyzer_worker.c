/*
 * spectrum_analyzer_worker.c
 *
 *  Created on: Dec 4, 2021
 *      Author: forthe
 */
#include "spectrum_analyzer.h"
#include "spectrum_analyzer_worker.h"

#include <furi.h>
#include <cc1101.h>

 uint8_t calibration_values[3][DOTS_COUNT];

static int32_t spectrum_analyzer_worker_thread(void* context) {
	SpectrumAnalyzerWorker* instance = context;
    
    instance->start_freq = 400000000;
    instance->end_freq = 500000000;
    uint32_t bandwidth = (instance->end_freq - instance->start_freq)/(DOTS_COUNT-1);

    // Start CC1011
    furi_hal_subghz_reset();
    furi_hal_subghz_load_preset(FuriHalSubGhzPresetOok650Async);

    // Calibrate and store calibration values for all
    // working frequences 
    
    for (uint8_t i=0; i<DOTS_COUNT; i++) {
        furi_hal_subghz_set_frequency(instance->start_freq + bandwidth*i);
        furi_hal_spi_acquire(&furi_hal_spi_bus_handle_subghz);
        cc1101_read_cal_values(&furi_hal_spi_bus_handle_subghz,
                            &calibration_values[0][i],
                            &calibration_values[1][i],
                            &calibration_values[2][i]);
        furi_hal_spi_release(&furi_hal_spi_bus_handle_subghz);
    }
    
    // Start receiver
    furi_hal_subghz_flush_rx();
    furi_hal_subghz_rx(); 
    
    while(instance->worker_running) {
    	osDelay(1000);

        // Read RSSI for current channnel
        

        // Fast frequency hop (chapter 28.2 of CC1101 datasheet)

    }

    return 0;
}

SpectrumAnalyzerWorker* spectrum_analyzer_worker_alloc() {
	SpectrumAnalyzerWorker* instance = furi_alloc(sizeof(SpectrumAnalyzerWorker));

    instance->thread = furi_thread_alloc();
    furi_thread_set_name(instance->thread, "SpectrumAnalyzerWorker");
    furi_thread_set_stack_size(instance->thread, 2048);
    furi_thread_set_context(instance->thread, instance);
    furi_thread_set_callback(instance->thread, spectrum_analyzer_worker_thread);
    instance->worker_running = 1;
    furi_thread_start(instance->thread);

    return instance;
}


void spectrum_analyzer_worker_free(SpectrumAnalyzerWorker* instance) {
    furi_assert(instance);
    instance->worker_running = 0;
    furi_thread_join(instance->thread);

    furi_thread_free(instance->thread);

    free(instance);
}
