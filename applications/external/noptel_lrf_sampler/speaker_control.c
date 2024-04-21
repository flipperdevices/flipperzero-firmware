/***
 * Noptel LRF rangefinder sampler for the Flipper Zero
 * Version: 1.4
 *
 * Speaker control
***/

/*** Includes ***/
#include <furi_hal.h>

#include "noptel_lrf_sampler.h"

/*** Routines ***/

/** Speaker control timer callback **/
static void speaker_control_timer_callback(void* ctx) {
    SpeakerControl* spc = (SpeakerControl*)ctx;

    /* Should we start beeping? */
    if(spc->play_beep) {
        /* Start the speaker beeping if it wasn't beeping already */
        if(!spc->beep_playing && furi_hal_speaker_acquire(500)) {
            furi_hal_speaker_start(beep_frequency, 1);
            spc->beep_playing = true;
        }

        spc->play_beep = false;

        /* Reschedule ourselves in one minimum beep duration */
        furi_timer_start(spc->speaker_control_timer, min_beep_duration);
    }

    /* If the speaker is beeping, stop it and don't reschedule ourselves */
    else if(spc->beep_playing && furi_hal_speaker_is_mine()) {
        furi_hal_speaker_stop();
        furi_hal_speaker_release();
        spc->beep_playing = false;
    }
}

/** Setup the speaker control **/
void set_speaker_control(SpeakerControl* spc, uint16_t min_beep_duration) {
    /* Configure the minimum beep duration duration */
    spc->min_beep_duration = min_beep_duration;

    /* No beep is currently playing */
    spc->play_beep = false;
    spc->beep_playing = false;

    /* Setup the timer to control the speaker */
    spc->speaker_control_timer =
        furi_timer_alloc(speaker_control_timer_callback, FuriTimerTypeOnce, spc);
}

/** Release the speaker control **/
void release_speaker_control(SpeakerControl* spc) {
    /* Make sure the speaker has stopped beeping */
    spc->play_beep = false;
    furi_delay_ms(min_beep_duration * 2);

    /* Stop and free the speaker control timer callback */
    furi_timer_stop(spc->speaker_control_timer);
    furi_timer_free(spc->speaker_control_timer);
}

/** Start a beep **/
void start_beep(SpeakerControl* spc) {
    /* Raise the flag for the speaker control timer callback to start a beep */
    spc->play_beep = true;

    /* Schedule the speaker control timer control to run asap */
    furi_timer_start(spc->speaker_control_timer, 1);
}
