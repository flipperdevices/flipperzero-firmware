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
 * nnnnnn            = [0] note kill, [1..60] note number, [61..63] service values
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
#define NOTE_KILL 63

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

#define PATTERN_SIZE 64

typedef struct {
    NoteRecord notes[PATTERN_SIZE];
} NoteRow;

typedef struct {
    uint8_t row_count;
    NoteRow* rows;
} NotePattern;

NoteRow row = {
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
            RECORD_MAKE(NOTE_KILL, 0, 0),
            //
            RECORD_MAKE(NOTE_A4, 0, 0),
            RECORD_MAKE(NOTE_A4, 0, 0),
            RECORD_MAKE(NOTE_A4, 0, 0),
            RECORD_MAKE(NOTE_KILL, 0, 0),
            //
            RECORD_MAKE(NOTE_E5, 0, 0),
            RECORD_MAKE(NOTE_E5, 0, 0),
            RECORD_MAKE(NOTE_E5, 0, 0),
            RECORD_MAKE(NOTE_KILL, 0, 0),
            //
            RECORD_MAKE(NOTE_D5, 0, 0),
            RECORD_MAKE(NOTE_C3, 0, 0),
            RECORD_MAKE(NOTE_F2, 0, 0),
            RECORD_MAKE(NOTE_C3, 0, 0),
            //
            RECORD_MAKE(NOTE_C5, 0, 0),
            RECORD_MAKE(NOTE_C3, 0, 0),
            RECORD_MAKE(NOTE_C5, 0, 0),
            RECORD_MAKE(NOTE_KILL, 0, 0),
            //
            RECORD_MAKE(NOTE_A4, 0, 0),
            RECORD_MAKE(0, 0, 0),
            RECORD_MAKE(0, 0, 0),
            RECORD_MAKE(0, 0, 0),
            //
            RECORD_MAKE(NOTE_A4, 0, 0),
            RECORD_MAKE(NOTE_A4, 0, 0),
            RECORD_MAKE(NOTE_A4, 0, 0),
            RECORD_MAKE(NOTE_KILL, 0, 0),
            //
            RECORD_MAKE(NOTE_B4, 0, 0),
            RECORD_MAKE(NOTE_D3, 0, 0),
            RECORD_MAKE(NOTE_G2, 0, 0),
            RECORD_MAKE(NOTE_D3, 0, 0),
            //
            RECORD_MAKE(NOTE_E4, 0, 0),
            RECORD_MAKE(NOTE_D3, 0, 0),
            RECORD_MAKE(NOTE_E4, 0, 0),
            RECORD_MAKE(NOTE_KILL, 0, 0),
            //
            RECORD_MAKE(NOTE_A4, 0, 0),
            RECORD_MAKE(NOTE_A4, 0, 0),
            RECORD_MAKE(NOTE_A4, 0, 0),
            RECORD_MAKE(NOTE_KILL, 0, 0),
            //
            RECORD_MAKE(NOTE_E5, 0, 0),
            RECORD_MAKE(NOTE_E5, 0, 0),
            RECORD_MAKE(NOTE_E5, 0, 0),
            RECORD_MAKE(NOTE_KILL, 0, 0),
            //
            RECORD_MAKE(NOTE_D5, 0, 0),
            RECORD_MAKE(NOTE_D3, 0, 0),
            RECORD_MAKE(NOTE_G2, 0, 0),
            RECORD_MAKE(NOTE_D3, 0, 0),
            //
            RECORD_MAKE(NOTE_C5, 0, 0),
            RECORD_MAKE(NOTE_D3, 0, 0),
            RECORD_MAKE(NOTE_C5, 0, 0),
            RECORD_MAKE(NOTE_KILL, 0, 0),
            //
            RECORD_MAKE(NOTE_A4, 0, 0),
            RECORD_MAKE(0, 0, 0),
            RECORD_MAKE(0, 0, 0),
            RECORD_MAKE(0, 0, 0),
            //
            RECORD_MAKE(NOTE_A4, 0, 0),
            RECORD_MAKE(NOTE_A4, 0, 0),
            RECORD_MAKE(NOTE_A4, 0, 0),
            RECORD_MAKE(NOTE_KILL, 0, 0),
        },
};

uint8_t counter = 0;
uint8_t counter_limit = 2;

uint8_t row_counter = 0;

void tracker_interrupt_body() {
    if(counter == 0) {
        uint8_t note = record_get_note(row.notes[row_counter]);
        if(note == NOTE_KILL) {
            tracker_speaker_stop();
        } else if(note > 0 && note < NOTE_KILL) {
            float freq = note_to_freq(note);
            tracker_speaker_play(freq, 0.5f);
        }
        row_counter = (row_counter + 1) % PATTERN_SIZE;
    }
    counter = (counter + 1) % counter_limit;
}

void tracker_interrupt_cb() {
    tracker_debug_set(true);
    tracker_interrupt_body();
    tracker_debug_set(false);
}

int32_t zero_tracker_app(void* p) {
    UNUSED(p);

    tracker_debug_init();
    tracker_speaker_init();
    tracker_interrupt_init(60.0f, tracker_interrupt_cb, NULL);

    while(1) {
        furi_delay_ms(1000);
    }

    return 0;
}