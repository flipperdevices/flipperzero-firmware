/*
 * spectrum_analyzer_worker.c
 *
 *  Created on: Dec 4, 2021
 *      Author: forthe
 */
#include "spectrum_analyzer.h"
#include "spectrum_analyzer_worker.h"
#include <furi.h>


static int32_t spectrum_analyzer_worker_thread(void* context) {
	SpectrumAnalyzerWorker* instance = context;
	//Start CC1101
	furi_hal_subghz_reset();
	furi_hal_subghz_load_preset(FuriHalSubGhzPresetOok650Async);
	furi_hal_subghz_set_frequency(433920000);
	furi_hal_subghz_flush_rx();
	furi_hal_subghz_rx();
    while(instance->worker_running)
    {
    	osDelay(100);
    	float step = (instance->start_freq - instance->end_freq)/DOTS_COUNT;
		float cur_freq = instance->start_freq

    	for (uint16_t dot = 0; dot < DOTS_COUNT; dot++)
    	{
    		furi_hal_subghz_idle();
			frequency = furi_hal_subghz_set_frequency(subghz_frequencies[i]);
			furi_hal_subghz_rx();
			osDelay(3);
			float rssi = furi_hal_subghz_get_rssi();
			// [TODO] Закинуть рсси и частоту в модель
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
