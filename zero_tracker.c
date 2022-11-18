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

uint8_t record_get_note(NoteRecord note) {
    return note & 0x3F;
}

uint8_t record_get_effect(NoteRecord note) {
    return (note >> 6) & 0xF;
}

uint8_t record_get_effect_data(NoteRecord note) {
    return (note >> 10) & 0x3F;
}

#define PATTERN_SIZE 64

typedef struct {
    NoteRecord notes[PATTERN_SIZE];
} NoteRow;

typedef struct {
    uint8_t row_count;
    NoteRow* rows;
} NotePattern;

void tracker_interrupt_body() {
    tracker_debug_set(true);
    tracker_debug_set(false);
}

int32_t zero_tracker_app(void* p) {
    UNUSED(p);

    tracker_debug_init();
    tracker_speaker_init();
    tracker_interrupt_init(120.0f, tracker_interrupt_body, NULL);

    while(1) {
        furi_delay_ms(1000);
    }

    return 0;
}