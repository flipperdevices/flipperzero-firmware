/***
 * Noptel LRF rangefinder sampler for the Flipper Zero
 * Version: 1.3
 *
 * Speaker control
***/

/*** Types ***/
typedef struct {
    /* Minimum beep duration */
    uint16_t min_beep_duration;

    /* Flag to play a beep, and whether a beep is already playing */
    bool play_beep;
    bool beep_playing;

    /* Timer callback to control the speaker */
    FuriTimer* speaker_control_timer;

} SpeakerControl;

/*** Routines ***/

/** Setup the speaker control **/
void set_speaker_control(SpeakerControl*, uint16_t);

/** Release the speaker control **/
void release_speaker_control(SpeakerControl*);

/** Start a beep **/
void start_beep(SpeakerControl*);
