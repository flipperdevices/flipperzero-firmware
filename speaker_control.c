/***
 * Noptel LRF rangefinder sampler for the Flipper Zero
 * Version: 1.2
 *
 * Speaker control
***/

/*** Includes ***/
#include <furi_hal.h>

#include "noptel_lrf_sampler.h"



/*** Routines ***/

/** Speaker control timer callback **/
void speaker_control_timer_callback(void *ctx) {

  App *app = (App *)ctx;
  SamplerModel *sampler_model = view_get_model(app->sample_view);

  /* Should we start beeping? */
  if(sampler_model->play_beep) {

    /* Start the speaker beeping if it wasn't beeping already */
    if(!sampler_model->beep_playing && furi_hal_speaker_acquire(500)) {
      furi_hal_speaker_start(1000, 1);
      sampler_model->beep_playing = true;
    }

    sampler_model->play_beep = false;

    /* Reschedule ourselves in one minimum beep duration */
    furi_timer_start(app->speaker_control_timer, min_beep_duration);
  }

  /* If the speaker is beeping, stop it and don't reschedule ourselves */
  else
    if(sampler_model->beep_playing && furi_hal_speaker_is_mine()) {
      furi_hal_speaker_stop();
      furi_hal_speaker_release();
      sampler_model->beep_playing = false;
  }
}
