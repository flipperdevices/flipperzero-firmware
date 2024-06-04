/***
 * Noptel LRF rangefinder sampler for the Flipper Zero
 * Version: 1.9
 *
 * Speaker control
***/

/*** Types ***/
typedef struct {
    /* Beep duration */
    uint16_t beep_duration;

    /* Flag to play a beep, and whether a beep is already playing */
    bool play_beep;
    bool beep_playing;

    /* Timer callback to control the speaker */
    FuriTimer* speaker_control_timer;

} SpeakerControl;

/*** Routines ***/

/** Setup the speaker control **/
void set_speaker_control(SpeakerControl*);

/** Release the speaker control **/
void release_speaker_control(SpeakerControl*);

/** Start a beep **/
void start_beep(SpeakerControl*, uint16_t);
