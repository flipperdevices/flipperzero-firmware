#include "speaker_i.h"

static int32_t speaker_worker(void* context);

/**
 * @brief Allocates a new speaker.
 * @details Allocates a new speaker.  The internal Flipper Zero speaker is
 * used to play tones.  Defaults the frequency to 0 and the volume to 1.0
 * (maximum volume).
 * @return The new speaker.
*/
Speaker* speaker_alloc(void) {
    Speaker* speaker = (Speaker*)malloc(sizeof(Speaker));
    speaker->is_running = true;
    speaker->frequency = 0.0f;
    speaker->volume = 1.0f;
    speaker->thread = furi_thread_alloc_ex("KeyMonitorWorker", 1024, speaker_worker, speaker);
    furi_thread_start(speaker->thread);

    return speaker;
}

/**
 * @brief Frees a speaker.
 * @param speaker The speaker to free.
*/
void speaker_free(Speaker* speaker) {
    speaker->is_running = false;
    furi_thread_join(speaker->thread);
    furi_thread_free(speaker->thread);
    free(speaker);
}

/**
 * @brief Sets the frequency of the tone to play.
 * @param speaker The speaker to set the frequency for.
 * @param frequency The frequency to play (in Hz).
*/
void speaker_set_frequency(Speaker* speaker, float frequency) {
    speaker->frequency = frequency;
}

/**
 * @brief Sets the volume of the tone to play.
 * @param speaker The speaker to set the volume for.
 * @param volume The volume to play (0.0 - 1.0).
*/
void speaker_set_volume(Speaker* speaker, float volume) {
    speaker->volume = volume;
}

/**
 * @brief The worker thread for the speaker.
 * @details The worker thread for the speaker.  This thread is responsible
 * for playing tones on the internal Flipper Zero speaker. The frequency
 * and volume are set by the speaker_set_frequency and speaker_set_volume
 * functions. Every 10ms the thread checks to see if the frequency or volume
 * has changed.  If so, it will start or stop the speaker as needed.
 * @param context The speaker context.
 * @return 0 when thread exits.
*/
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