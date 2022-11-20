#include <furi.h>
#include "speaker_hal.h"
#include "zero_tracker.h"

/**
 * @brief Note record
 * 
 * AH       AL
 * FEDCBA98 76543210
 * nnnnnnee eedddddd
 * -------- --------
 * nnnnnn            = [0] do nothing, [1..60] note number, [61] note off, [62..63] not used
 *       ee ee       = [0..F] effect 
 *            111222 = [0..63] or [0..7, 0..7] effect data 
 */
typedef uint16_t NoteRecord;

#define NOTE_NONE 0
#define NOTE_C2 1
#define NOTE_Cs2 2
#define NOTE_D2 3
#define NOTE_Ds2 4
#define NOTE_E2 5
#define NOTE_F2 6
#define NOTE_Fs2 7
#define NOTE_G2 8
#define NOTE_Gs2 9
#define NOTE_A2 10
#define NOTE_As2 11
#define NOTE_B2 12
#define NOTE_C3 13
#define NOTE_Cs3 14
#define NOTE_D3 15
#define NOTE_Ds3 16
#define NOTE_E3 17
#define NOTE_F3 18
#define NOTE_Fs3 19
#define NOTE_G3 20
#define NOTE_Gs3 21
#define NOTE_A3 22
#define NOTE_As3 23
#define NOTE_B3 24
#define NOTE_C4 25
#define NOTE_Cs4 26
#define NOTE_D4 27
#define NOTE_Ds4 28
#define NOTE_E4 29
#define NOTE_F4 30
#define NOTE_Fs4 31
#define NOTE_G4 32
#define NOTE_Gs4 33
#define NOTE_A4 34
#define NOTE_As4 35
#define NOTE_B4 36
#define NOTE_C5 37
#define NOTE_Cs5 38
#define NOTE_D5 39
#define NOTE_Ds5 40
#define NOTE_E5 41
#define NOTE_F5 42
#define NOTE_Fs5 43
#define NOTE_G5 44
#define NOTE_Gs5 45
#define NOTE_A5 46
#define NOTE_As5 47
#define NOTE_B5 48
#define NOTE_C6 49
#define NOTE_Cs6 50
#define NOTE_D6 51
#define NOTE_Ds6 52
#define NOTE_E6 53
#define NOTE_F6 54
#define NOTE_Fs6 55
#define NOTE_G6 56
#define NOTE_Gs6 57
#define NOTE_A6 58
#define NOTE_As6 59
#define NOTE_B6 60
#define NOTE_OFF 63

typedef enum {
    EffectArpeggio = 0x00,
    EffectSlideUp = 0x01,
    EffectSlideDown = 0x02,
    EffectSlideToNote = 0x03,
    EffectVibrato = 0x04,

    EffectSetSpeed = 0x0F,
} Effect;

#define EFFECT_DATA_NONE 0

#define EFFECT_DATA_2(x, y) ((x) | ((y) << 3))

#define EFFECT_DATA_GET_X(data) ((data)&0x07)
#define EFFECT_DATA_GET_Y(data) (((data) >> 3) & 0x07)

#define FREQUENCY_UNSET -1.0f

uint8_t record_get_note(NoteRecord note) {
    return note & 0x3F;
}

uint8_t record_get_effect(NoteRecord note) {
    return (note >> 6) & 0xF;
}

uint8_t record_get_effect_data(NoteRecord note) {
    return (note >> 10) & 0x3F;
}

#define RECORD_MAKE(note, effect, data) \
    ((NoteRecord)(((note)&0x3F) | (((effect)&0xF) << 6) | (((data)&0x3F) << 10)))

#define NOTES_PER_OCT 12
const float notes_oct[NOTES_PER_OCT] = {
    130.813f,
    138.591f,
    146.832f,
    155.563f,
    164.814f,
    174.614f,
    184.997f,
    195.998f,
    207.652f,
    220.00f,
    233.082f,
    246.942f,
};

float note_to_freq(uint8_t note) {
    if(note == NOTE_NONE) return 0.0f;
    note = note - NOTE_C2;
    uint8_t octave = note / NOTES_PER_OCT;
    uint8_t note_in_oct = note % NOTES_PER_OCT;
    return notes_oct[note_in_oct] * (1 << octave);
}

float frequency_offset(float frequency, uint8_t semitones) {
    return frequency * (1.0f + ((1.0f / 12.0f) * semitones));
}

#define PATTERN_SIZE 64

typedef struct {
    NoteRecord notes[PATTERN_SIZE];
} NoteRow;

typedef struct {
    uint8_t row_count;
    NoteRow* rows;
} NotePattern;

NoteRow _row = {
    .notes =
        {
            //
            RECORD_MAKE(NOTE_A4, 0, 0),
            RECORD_MAKE(NOTE_C3, 0, 0),
            RECORD_MAKE(NOTE_F2, 0, 0),
            RECORD_MAKE(NOTE_C3, 0, 0),
            //
            RECORD_MAKE(NOTE_E4, 0, 0),
            RECORD_MAKE(NOTE_C3, 0, 0),
            RECORD_MAKE(NOTE_E4, 0, 0),
            RECORD_MAKE(NOTE_OFF, 0, 0),
            //
            RECORD_MAKE(NOTE_A4, 0, 0),
            RECORD_MAKE(NOTE_A4, 0, 0),
            RECORD_MAKE(NOTE_A4, 0, 0),
            RECORD_MAKE(NOTE_OFF, 0, 0),
            //
            RECORD_MAKE(NOTE_E5, 0, 0),
            RECORD_MAKE(NOTE_E5, 0, 0),
            RECORD_MAKE(NOTE_E5, 0, 0),
            RECORD_MAKE(NOTE_OFF, 0, 0),
            //
            RECORD_MAKE(NOTE_D5, 0, 0),
            RECORD_MAKE(NOTE_C3, 0, 0),
            RECORD_MAKE(NOTE_F2, 0, 0),
            RECORD_MAKE(NOTE_C3, 0, 0),
            //
            RECORD_MAKE(NOTE_C5, 0, 0),
            RECORD_MAKE(NOTE_C3, 0, 0),
            RECORD_MAKE(NOTE_C5, 0, 0),
            RECORD_MAKE(NOTE_OFF, 0, 0),
            //
            RECORD_MAKE(NOTE_A4, 0, 0),
            RECORD_MAKE(0, 0, 0),
            RECORD_MAKE(0, 0, 0),
            RECORD_MAKE(0, 0, 0),
            //
            RECORD_MAKE(NOTE_A4, 0, 0),
            RECORD_MAKE(NOTE_A4, 0, 0),
            RECORD_MAKE(NOTE_A4, 0, 0),
            RECORD_MAKE(NOTE_OFF, 0, 0),
            //
            RECORD_MAKE(NOTE_B4, 0, 0),
            RECORD_MAKE(NOTE_D3, 0, 0),
            RECORD_MAKE(NOTE_G2, 0, 0),
            RECORD_MAKE(NOTE_D3, 0, 0),
            //
            RECORD_MAKE(NOTE_E4, 0, 0),
            RECORD_MAKE(NOTE_D3, 0, 0),
            RECORD_MAKE(NOTE_E4, 0, 0),
            RECORD_MAKE(NOTE_OFF, 0, 0),
            //
            RECORD_MAKE(NOTE_A4, 0, 0),
            RECORD_MAKE(NOTE_A4, 0, 0),
            RECORD_MAKE(NOTE_A4, 0, 0),
            RECORD_MAKE(NOTE_OFF, 0, 0),
            //
            RECORD_MAKE(NOTE_E5, 0, 0),
            RECORD_MAKE(NOTE_E5, 0, 0),
            RECORD_MAKE(NOTE_E5, 0, 0),
            RECORD_MAKE(NOTE_OFF, 0, 0),
            //
            RECORD_MAKE(NOTE_D5, 0, 0),
            RECORD_MAKE(NOTE_D3, 0, 0),
            RECORD_MAKE(NOTE_G2, 0, 0),
            RECORD_MAKE(NOTE_D3, 0, 0),
            //
            RECORD_MAKE(NOTE_C5, 0, 0),
            RECORD_MAKE(NOTE_D3, 0, 0),
            RECORD_MAKE(NOTE_C5, 0, 0),
            RECORD_MAKE(NOTE_OFF, 0, 0),
            //
            RECORD_MAKE(NOTE_A4, 0, 0),
            RECORD_MAKE(0, 0, 0),
            RECORD_MAKE(0, 0, 0),
            RECORD_MAKE(0, 0, 0),
            //
            RECORD_MAKE(NOTE_A4, 0, 0),
            RECORD_MAKE(NOTE_A4, 0, 0),
            RECORD_MAKE(NOTE_A4, 0, 0),
            RECORD_MAKE(NOTE_OFF, 0, 0),
        },
};

const uint8_t test = 0x20;

NoteRow row = {
    .notes =
        {
            // 1/4
            RECORD_MAKE(NOTE_C3, EffectArpeggio, EFFECT_DATA_2(3, 5)),
            RECORD_MAKE(0, EffectArpeggio, EFFECT_DATA_2(3, 5)),
            RECORD_MAKE(NOTE_C4, EffectSlideToNote, test),
            RECORD_MAKE(0, EffectSlideToNote, test),
            //
            RECORD_MAKE(0, EffectSlideToNote, test),
            RECORD_MAKE(0, EffectSlideToNote, test),
            RECORD_MAKE(0, EffectSlideToNote, test),
            RECORD_MAKE(0, EffectSlideToNote, test),
            //
            RECORD_MAKE(0, EffectVibrato, EFFECT_DATA_2(1, 1)),
            RECORD_MAKE(0, EffectVibrato, EFFECT_DATA_2(1, 1)),
            RECORD_MAKE(0, EffectVibrato, EFFECT_DATA_2(1, 1)),
            RECORD_MAKE(0, EffectVibrato, EFFECT_DATA_2(1, 1)),
            //
            RECORD_MAKE(0, EffectVibrato, EFFECT_DATA_2(2, 2)),
            RECORD_MAKE(0, EffectVibrato, EFFECT_DATA_2(2, 2)),
            RECORD_MAKE(0, EffectVibrato, EFFECT_DATA_2(2, 2)),
            RECORD_MAKE(0, EffectVibrato, EFFECT_DATA_2(2, 2)),
            // 2/4
            RECORD_MAKE(NOTE_C3, EffectSlideDown, 0x20),
            RECORD_MAKE(0, EffectSlideDown, 0x20),
            RECORD_MAKE(NOTE_C4, EffectVibrato, EFFECT_DATA_2(3, 3)),
            RECORD_MAKE(0, EffectVibrato, EFFECT_DATA_2(3, 3)),
            //
            RECORD_MAKE(0, EffectVibrato, EFFECT_DATA_2(3, 3)),
            RECORD_MAKE(0, EffectVibrato, EFFECT_DATA_2(3, 3)),
            RECORD_MAKE(0, EffectVibrato, EFFECT_DATA_2(3, 3)),
            RECORD_MAKE(0, EffectVibrato, EFFECT_DATA_2(3, 3)),
            //
            RECORD_MAKE(0, EffectVibrato, EFFECT_DATA_2(3, 3)),
            RECORD_MAKE(0, EffectVibrato, EFFECT_DATA_2(3, 3)),
            RECORD_MAKE(0, EffectVibrato, EFFECT_DATA_2(3, 3)),
            RECORD_MAKE(0, EffectVibrato, EFFECT_DATA_2(3, 3)),
            //
            RECORD_MAKE(0, EffectVibrato, EFFECT_DATA_2(3, 3)),
            RECORD_MAKE(0, EffectVibrato, EFFECT_DATA_2(3, 3)),
            RECORD_MAKE(0, EffectVibrato, EFFECT_DATA_2(3, 3)),
            RECORD_MAKE(NOTE_OFF, EffectVibrato, EFFECT_DATA_2(3, 3)),
            // 3/4
            RECORD_MAKE(NOTE_C3, EffectArpeggio, EFFECT_DATA_2(3, 5)),
            RECORD_MAKE(0, EffectArpeggio, EFFECT_DATA_2(3, 5)),
            RECORD_MAKE(NOTE_OFF, 0, 0),
            RECORD_MAKE(0, 0, 0),
            //
            RECORD_MAKE(0, 0, 0),
            RECORD_MAKE(0, 0, 0),
            RECORD_MAKE(0, 0, 0),
            RECORD_MAKE(0, 0, 0),
            //
            RECORD_MAKE(NOTE_C2, 0, 0),
            RECORD_MAKE(0, 0, 0),
            RECORD_MAKE(0, 0, 0),
            RECORD_MAKE(NOTE_OFF, 0, 0),
            //
            RECORD_MAKE(0, 0, 0),
            RECORD_MAKE(0, 0, 0),
            RECORD_MAKE(0, 0, 0),
            RECORD_MAKE(0, 0, 0),
            // 4/4
            RECORD_MAKE(NOTE_C3, EffectSlideDown, 0x20),
            RECORD_MAKE(0, EffectSlideDown, 0x20),
            RECORD_MAKE(0, EffectSlideDown, 0x20),
            RECORD_MAKE(NOTE_OFF, 0, 0),
            //
            RECORD_MAKE(0, 0, 0),
            RECORD_MAKE(0, 0, 0),
            RECORD_MAKE(0, 0, 0),
            RECORD_MAKE(0, 0, 0),
            //
            RECORD_MAKE(NOTE_C2, 0, 0),
            RECORD_MAKE(0, 0, 0),
            RECORD_MAKE(0, 0, 0),
            RECORD_MAKE(NOTE_OFF, 0, 0),
            //
            RECORD_MAKE(0, 0, 0),
            RECORD_MAKE(0, 0, 0),
            RECORD_MAKE(0, 0, 0),
            RECORD_MAKE(0, 0, 0),
        },
};

typedef struct {
    uint8_t tick;
    uint8_t tick_limit;
    uint8_t row;
} SongState;

SongState song_state = {
    .tick = 0,
    .tick_limit = 2,
    .row = 0,
};

typedef struct {
    uint8_t speed;
    uint8_t depth;
    int8_t direction;
    int8_t value;
} Vibrato;

typedef struct {
    float frequency;
    float frequency_target;
    bool play;
    Vibrato vibrato;
} ChannelState;

ChannelState ch_state = {
    .frequency = 0,
    .frequency_target = FREQUENCY_UNSET,
    .play = false,
};

void tracker_interrupt_body() {
    uint8_t note = record_get_note(row.notes[song_state.row]);
    uint8_t effect = record_get_effect(row.notes[song_state.row]);
    uint8_t data = record_get_effect_data(row.notes[song_state.row]);

    // load frequency from note at tick 0
    if(song_state.tick == 0) {
        if(note == NOTE_OFF) {
            ch_state.play = false;
        } else if((note > NOTE_NONE) && (note < NOTE_OFF)) {
            ch_state.play = true;

            // reset vibrato
            ch_state.vibrato.speed = 0;
            ch_state.vibrato.depth = 0;
            ch_state.vibrato.value = 0;
            ch_state.vibrato.direction = 0;

            if(effect == EffectSlideToNote) {
                ch_state.frequency_target = note_to_freq(note);
            } else {
                ch_state.frequency = note_to_freq(note);
                ch_state.frequency_target = FREQUENCY_UNSET;
            }
        }

        if(effect == EffectSetSpeed) {
            song_state.tick_limit = data;
        }
    }

    if(ch_state.play) {
        float frequency;

        if((effect == EffectSlideUp || effect == EffectSlideDown) && data != EFFECT_DATA_NONE) {
            // apply slide effect
            ch_state.frequency += (effect == EffectSlideUp ? 1 : -1) * data;
        } else if(effect == EffectSlideToNote) {
            // apply slide to note effect, if target frequency is set
            if(ch_state.frequency_target > 0) {
                if(ch_state.frequency_target > ch_state.frequency) {
                    ch_state.frequency += data;
                    if(ch_state.frequency > ch_state.frequency_target) {
                        ch_state.frequency = ch_state.frequency_target;
                        ch_state.frequency_target = FREQUENCY_UNSET;
                    }
                } else if(ch_state.frequency_target < ch_state.frequency) {
                    ch_state.frequency -= data;
                    if(ch_state.frequency < ch_state.frequency_target) {
                        ch_state.frequency = ch_state.frequency_target;
                        ch_state.frequency_target = FREQUENCY_UNSET;
                    }
                }
            }
        }

        frequency = ch_state.frequency;

        // apply arpeggio effect
        if(effect == EffectArpeggio) {
            if(data != EFFECT_DATA_NONE) {
                if((song_state.tick % 3) == 1) {
                    uint8_t note_offset = EFFECT_DATA_GET_X(data);
                    frequency = frequency_offset(frequency, note_offset);
                } else if((song_state.tick % 3) == 2) {
                    uint8_t note_offset = EFFECT_DATA_GET_Y(data);
                    frequency = frequency_offset(frequency, note_offset);
                }
            }
        } else if(effect == EffectVibrato) {
            // apply vibrato effect, data = speed, depth
            uint8_t vibrato_speed = EFFECT_DATA_GET_X(data);
            uint8_t vibrato_depth = EFFECT_DATA_GET_Y(data);

            // update vibrato parameters if speed or depth is non-zero
            if(vibrato_speed != 0) ch_state.vibrato.speed = vibrato_speed;
            if(vibrato_depth != 0) ch_state.vibrato.depth = vibrato_depth;

            // update vibrato value
            ch_state.vibrato.value += ch_state.vibrato.direction * ch_state.vibrato.speed;

            // change direction if value is at the limit
            if(ch_state.vibrato.value > ch_state.vibrato.depth) {
                ch_state.vibrato.direction = -1;
            } else if(ch_state.vibrato.value < -ch_state.vibrato.depth) {
                ch_state.vibrato.direction = 1;
            } else if(ch_state.vibrato.direction == 0) {
                // set initial direction, if it is not set
                ch_state.vibrato.direction = 1;
            }

            frequency += frequency * (((1.0f / 12.0f) / 7.0f) * ch_state.vibrato.value);
        }

        tracker_speaker_play(frequency, 0.5f);
    } else {
        tracker_speaker_stop();
    }

    song_state.tick++;
    if(song_state.tick >= song_state.tick_limit) {
        song_state.tick = 0;

        // next note
        song_state.row = (song_state.row + 1) % PATTERN_SIZE;
    }
}

void tracker_interrupt_cb() {
    tracker_debug_set(true);
    tracker_interrupt_body();
    tracker_debug_set(false);
}

void log_record(NoteRecord record) {
    uint8_t note = record_get_note(record);
    uint8_t effect = record_get_effect(record);
    uint8_t data = record_get_effect_data(record);

    printf("Note: %u, Effect: %u, Data: %u", note, effect, data);

    if(effect == EffectArpeggio) {
        uint8_t note_offset = EFFECT_DATA_GET_X(data);
        uint8_t note_offset2 = EFFECT_DATA_GET_Y(data);
        printf(" (Arpeggio: %u, %u)", note_offset, note_offset2);

        float frequency = note_to_freq(note);
        float new_frequency = frequency_offset(frequency, note_offset);
        float new_frequency2 = frequency_offset(frequency, note_offset2);
        printf(
            " (Freq: %f, %f, %f)",
            (double)frequency,
            (double)new_frequency,
            (double)new_frequency2);
    }

    printf("\r\n");
}

int32_t zero_tracker_app(void* p) {
    UNUSED(p);

    tracker_debug_init();
    tracker_speaker_init();
    tracker_interrupt_init(60.0f, tracker_interrupt_cb, NULL);
    // log_record(row.notes[0]);

    while(1) {
        furi_delay_ms(1000);
    }

    return 0;
}