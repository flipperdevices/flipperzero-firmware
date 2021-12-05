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
    
    instance->start_freq = 800000000;
    instance->bandwidth = 2000000;

    // Start CC1011
    furi_hal_subghz_reset();
    furi_hal_subghz_load_preset(FuriHalSubGhzPresetOok650Async);

    // Calibrate and store calibration values for all
    // working frequences 
    
    for (uint8_t i=0; i<DOTS_COUNT; i++) {
        furi_hal_subghz_set_frequency(instance->start_freq + instance->bandwidth *i);
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
        
        for (uint8_t i=0; i<DOTS_COUNT; i++) {
            // Fast frequency hop (chapter 28.2 of CC1101 datasheet)
            furi_hal_subghz_set_frequency_fast(instance->start_freq + instance->bandwidth * i);
            furi_hal_spi_acquire(&furi_hal_spi_bus_handle_subghz);
            cc1101_write_cal_values(&furi_hal_spi_bus_handle_subghz,
                                    calibration_values[0][i],
                                    calibration_values[1][i],
                                    calibration_values[2][i]);
            furi_hal_spi_release(&furi_hal_spi_bus_handle_subghz);
            
            // Read RSSI for current channnel
            instance->rssi_buf[i] = (uint8_t) (-1 * furi_hal_subghz_get_rssi());
        }
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


    return instance;
}


void spectrum_analyzer_worker_free(SpectrumAnalyzerWorker* instance) {
    furi_assert(instance);

    furi_thread_free(instance->thread);

    free(instance);
}

void spectrum_analyzer_worker_start(SpectrumAnalyzerWorker* instance) {
    furi_assert(instance);
    furi_assert(!instance->worker_running);

    instance->worker_running = true;

    furi_thread_start(instance->thread);
}

void spectrum_analyzer_worker_stop(SpectrumAnalyzerWorker* instance) {
    furi_assert(instance);
    furi_assert(instance->worker_running);

    instance->worker_running = false;

    furi_thread_join(instance->thread);
}

bool spectrum_analyzer_worker_is_running(SpectrumAnalyzerWorker* instance) {
    furi_assert(instance);
    return instance->worker_running;
}
