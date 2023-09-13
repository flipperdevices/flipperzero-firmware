#include "speaker_i.h"

static int32_t speaker_worker(void* context);

Speaker* speaker_alloc(void) {
    Speaker* speaker = (Speaker*)malloc(sizeof(Speaker));
    speaker->is_running = true;
    speaker->frequency = 0.0f;
    speaker->volume = 1.0f;
    speaker->thread = furi_thread_alloc_ex("KeyMonitorWorker", 1024, speaker_worker, speaker);
    furi_thread_start(speaker->thread);

    return speaker;
}

void speaker_set_frequency(Speaker* speaker, float frequency) {
    speaker->frequency = frequency;
}

void speaker_set_volume(Speaker* speaker, float volume) {
    speaker->volume = volume;
}

void speaker_free(Speaker* speaker) {
    speaker->is_running = false;
    furi_thread_join(speaker->thread);
    furi_thread_free(speaker->thread);
    free(speaker);
}

static int32_t speaker_worker(void* context) {
    Speaker* speaker = (Speaker*)context;

    furi_check(furi_hal_speaker_acquire(1000));

    float frequency = 0;
    float volume = 0;
    while(speaker->is_running) {
        if(frequency != speaker->frequency || volume != speaker->volume) {
            frequency = speaker->frequency;
            volume = speaker->volume;
            if(frequency == 0) {
                furi_hal_speaker_stop();
            } else {
                furi_hal_speaker_start(frequency, volume);
            }
        }
        furi_delay_ms(10);
    };

    furi_hal_speaker_stop();
    furi_hal_speaker_release();

    return 0;
}