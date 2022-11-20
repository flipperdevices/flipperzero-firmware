#include <furi.h>
#include "zero_tracker.h"
#include "tracker_engine/tracker.h"

// Channel row = {
//     .notes =
//         {
//             //
//             ROW_MAKE(NOTE_A4, 0, 0),
//             ROW_MAKE(NOTE_C3, 0, 0),
//             ROW_MAKE(NOTE_F2, 0, 0),
//             ROW_MAKE(NOTE_C3, 0, 0),
//             //
//             ROW_MAKE(NOTE_E4, 0, 0),
//             ROW_MAKE(NOTE_C3, 0, 0),
//             ROW_MAKE(NOTE_E4, 0, 0),
//             ROW_MAKE(NOTE_OFF, 0, 0),
//             //
//             ROW_MAKE(NOTE_A4, 0, 0),
//             ROW_MAKE(NOTE_A4, 0, 0),
//             ROW_MAKE(NOTE_A4, 0, 0),
//             ROW_MAKE(NOTE_OFF, 0, 0),
//             //
//             ROW_MAKE(NOTE_E5, 0, 0),
//             ROW_MAKE(NOTE_E5, 0, 0),
//             ROW_MAKE(NOTE_E5, 0, 0),
//             ROW_MAKE(NOTE_OFF, 0, 0),
//             //
//             ROW_MAKE(NOTE_D5, 0, 0),
//             ROW_MAKE(NOTE_C3, 0, 0),
//             ROW_MAKE(NOTE_F2, 0, 0),
//             ROW_MAKE(NOTE_C3, 0, 0),
//             //
//             ROW_MAKE(NOTE_C5, 0, 0),
//             ROW_MAKE(NOTE_C3, 0, 0),
//             ROW_MAKE(NOTE_C5, 0, 0),
//             ROW_MAKE(NOTE_OFF, 0, 0),
//             //
//             ROW_MAKE(NOTE_A4, 0, 0),
//             ROW_MAKE(0, 0, 0),
//             ROW_MAKE(0, 0, 0),
//             ROW_MAKE(0, 0, 0),
//             //
//             ROW_MAKE(NOTE_A4, 0, 0),
//             ROW_MAKE(NOTE_A4, 0, 0),
//             ROW_MAKE(NOTE_A4, 0, 0),
//             ROW_MAKE(NOTE_OFF, 0, 0),
//             //
//             ROW_MAKE(NOTE_B4, 0, 0),
//             ROW_MAKE(NOTE_D3, 0, 0),
//             ROW_MAKE(NOTE_G2, 0, 0),
//             ROW_MAKE(NOTE_D3, 0, 0),
//             //
//             ROW_MAKE(NOTE_E4, 0, 0),
//             ROW_MAKE(NOTE_D3, 0, 0),
//             ROW_MAKE(NOTE_E4, 0, 0),
//             ROW_MAKE(NOTE_OFF, 0, 0),
//             //
//             ROW_MAKE(NOTE_A4, 0, 0),
//             ROW_MAKE(NOTE_A4, 0, 0),
//             ROW_MAKE(NOTE_A4, 0, 0),
//             ROW_MAKE(NOTE_OFF, 0, 0),
//             //
//             ROW_MAKE(NOTE_E5, 0, 0),
//             ROW_MAKE(NOTE_E5, 0, 0),
//             ROW_MAKE(NOTE_E5, 0, 0),
//             ROW_MAKE(NOTE_OFF, 0, 0),
//             //
//             ROW_MAKE(NOTE_D5, 0, 0),
//             ROW_MAKE(NOTE_D3, 0, 0),
//             ROW_MAKE(NOTE_G2, 0, 0),
//             ROW_MAKE(NOTE_D3, 0, 0),
//             //
//             ROW_MAKE(NOTE_C5, 0, 0),
//             ROW_MAKE(NOTE_D3, 0, 0),
//             ROW_MAKE(NOTE_C5, 0, 0),
//             ROW_MAKE(NOTE_OFF, 0, 0),
//             //
//             ROW_MAKE(NOTE_A4, 0, 0),
//             ROW_MAKE(0, 0, 0),
//             ROW_MAKE(0, 0, 0),
//             ROW_MAKE(0, 0, 0),
//             //
//             ROW_MAKE(NOTE_A4, 0, 0),
//             ROW_MAKE(NOTE_A4, 0, 0),
//             ROW_MAKE(NOTE_A4, 0, 0),
//             ROW_MAKE(NOTE_OFF, 0, 0),
//         },
// };

Channel channels[] = {
    {
        .rows =
            {
                // 1/4
                ROW_MAKE(NOTE_C3, EffectArpeggio, EFFECT_DATA_2(4, 7)),
                ROW_MAKE(0, EffectArpeggio, EFFECT_DATA_2(4, 7)),
                ROW_MAKE(NOTE_C4, EffectSlideToNote, 0x20),
                ROW_MAKE(0, EffectSlideToNote, 0x20),
                //
                ROW_MAKE(0, EffectSlideToNote, 0x20),
                ROW_MAKE(0, EffectSlideToNote, 0x20),
                ROW_MAKE(0, EffectSlideToNote, 0x20),
                ROW_MAKE(0, EffectSlideToNote, 0x20),
                //
                ROW_MAKE(0, EffectVibrato, EFFECT_DATA_2(1, 1)),
                ROW_MAKE(0, EffectVibrato, EFFECT_DATA_2(1, 1)),
                ROW_MAKE(0, EffectVibrato, EFFECT_DATA_2(1, 1)),
                ROW_MAKE(0, EffectVibrato, EFFECT_DATA_2(1, 1)),
                //
                ROW_MAKE(0, EffectVibrato, EFFECT_DATA_2(2, 2)),
                ROW_MAKE(0, EffectVibrato, EFFECT_DATA_2(2, 2)),
                ROW_MAKE(0, EffectVibrato, EFFECT_DATA_2(2, 2)),
                ROW_MAKE(0, EffectVibrato, EFFECT_DATA_2(2, 2)),
                // 2/4
                ROW_MAKE(NOTE_C3, EffectSlideDown, 0x20),
                ROW_MAKE(0, EffectSlideDown, 0x20),
                ROW_MAKE(NOTE_C4, EffectVibrato, EFFECT_DATA_2(3, 3)),
                ROW_MAKE(0, EffectVibrato, EFFECT_DATA_2(3, 3)),
                //
                ROW_MAKE(0, EffectVibrato, EFFECT_DATA_2(3, 3)),
                ROW_MAKE(0, EffectVibrato, EFFECT_DATA_2(3, 3)),
                ROW_MAKE(0, EffectVibrato, EFFECT_DATA_2(3, 3)),
                ROW_MAKE(0, EffectVibrato, EFFECT_DATA_2(3, 3)),
                //
                ROW_MAKE(0, EffectVibrato, EFFECT_DATA_2(3, 3)),
                ROW_MAKE(0, EffectVibrato, EFFECT_DATA_2(3, 3)),
                ROW_MAKE(0, EffectVibrato, EFFECT_DATA_2(3, 3)),
                ROW_MAKE(0, EffectVibrato, EFFECT_DATA_2(3, 3)),
                //
                ROW_MAKE(0, EffectVibrato, EFFECT_DATA_2(3, 3)),
                ROW_MAKE(0, EffectVibrato, EFFECT_DATA_2(3, 3)),
                ROW_MAKE(0, EffectVibrato, EFFECT_DATA_2(3, 3)),
                ROW_MAKE(NOTE_OFF, EffectVibrato, EFFECT_DATA_2(3, 3)),
                // 3/4
                ROW_MAKE(NOTE_C3, EffectArpeggio, EFFECT_DATA_2(4, 7)),
                ROW_MAKE(0, EffectArpeggio, EFFECT_DATA_2(4, 7)),
                ROW_MAKE(NOTE_OFF, 0, 0),
                ROW_MAKE(0, 0, 0),
                //
                ROW_MAKE(0, 0, 0),
                ROW_MAKE(0, 0, 0),
                ROW_MAKE(0, 0, 0),
                ROW_MAKE(0, 0, 0),
                //
                ROW_MAKE(NOTE_C2, EffectPWM, 60),
                ROW_MAKE(0, EffectPWM, 32),
                ROW_MAKE(0, EffectPWM, 12),
                ROW_MAKE(NOTE_OFF, 0, 0),
                //
                ROW_MAKE(0, 0, 0),
                ROW_MAKE(0, 0, 0),
                ROW_MAKE(0, 0, 0),
                ROW_MAKE(0, 0, 0),
                // 4/4
                ROW_MAKE(NOTE_C3, EffectSlideDown, 0x20),
                ROW_MAKE(0, EffectSlideDown, 0x20),
                ROW_MAKE(0, EffectSlideDown, 0x20),
                ROW_MAKE(NOTE_OFF, 0, 0),
                //
                ROW_MAKE(0, 0, 0),
                ROW_MAKE(0, 0, 0),
                ROW_MAKE(0, 0, 0),
                ROW_MAKE(0, 0, 0),
                //
                ROW_MAKE(NOTE_C2, EffectPWM, 60),
                ROW_MAKE(0, EffectPWM, 32),
                ROW_MAKE(0, EffectPWM, 12),
                ROW_MAKE(NOTE_OFF, 0, 0),
                //
                ROW_MAKE(0, 0, 0),
                ROW_MAKE(0, 0, 0),
                ROW_MAKE(0, 0, 0),
                ROW_MAKE(0, 0, 0),
            },
    },
};

Pattern patterns[] = {
    {
        .channels = channels,
    },
};

uint8_t order_list[] = {
    0,
    0,
};

Song song = {
    .channels_count = 1,
    .patterns_count = 1,
    .patterns = patterns,

    .order_list_size = 2,
    .order_list = order_list,

    .ticks_per_second = 60,
};

int32_t zero_tracker_app(void* p) {
    UNUSED(p);

    Tracker* tracker = tracker_alloc();
    tracker_set_song(tracker, &song);
    tracker_start(tracker);

    while(1) {
        furi_delay_ms(1000);
    }

    tracker_stop(tracker);
    tracker_free(tracker);

    return 0;
}