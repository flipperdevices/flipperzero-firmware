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

Channel p_0_channels[] = {
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

Channel p_1_channels[] = {
    {
        .rows =
            {
                //
                ROW_MAKE(NOTE_A4, 0, 0),
                ROW_MAKE(NOTE_C3, 0, 0),
                ROW_MAKE(NOTE_F2, 0, 0),
                ROW_MAKE(NOTE_C3, 0, 0),
                //
                ROW_MAKE(NOTE_E4, 0, 0),
                ROW_MAKE(NOTE_C3, 0, 0),
                ROW_MAKE(NOTE_E4, 0, 0),
                ROW_MAKE(NOTE_OFF, 0, 0),
                //
                ROW_MAKE(NOTE_A4, 0, 0),
                ROW_MAKE(NOTE_A4, 0, 0),
                ROW_MAKE(NOTE_A4, 0, 0),
                ROW_MAKE(NOTE_OFF, 0, 0),
                //
                ROW_MAKE(NOTE_E5, 0, 0),
                ROW_MAKE(NOTE_E5, 0, 0),
                ROW_MAKE(NOTE_E5, 0, 0),
                ROW_MAKE(NOTE_OFF, 0, 0),
                //
                ROW_MAKE(NOTE_D5, 0, 0),
                ROW_MAKE(NOTE_C3, 0, 0),
                ROW_MAKE(NOTE_F2, 0, 0),
                ROW_MAKE(NOTE_C3, 0, 0),
                //
                ROW_MAKE(NOTE_C5, 0, 0),
                ROW_MAKE(NOTE_C3, 0, 0),
                ROW_MAKE(NOTE_C5, 0, 0),
                ROW_MAKE(NOTE_OFF, 0, 0),
                //
                ROW_MAKE(NOTE_A4, 0, 0),
                ROW_MAKE(0, 0, 0),
                ROW_MAKE(0, 0, 0),
                ROW_MAKE(0, 0, 0),
                //
                ROW_MAKE(NOTE_A4, 0, 0),
                ROW_MAKE(NOTE_A4, 0, 0),
                ROW_MAKE(NOTE_A4, 0, 0),
                ROW_MAKE(NOTE_OFF, 0, 0),
                //
                ROW_MAKE(NOTE_B4, 0, 0),
                ROW_MAKE(NOTE_D3, 0, 0),
                ROW_MAKE(NOTE_G2, 0, 0),
                ROW_MAKE(NOTE_D3, 0, 0),
                //
                ROW_MAKE(NOTE_E4, 0, 0),
                ROW_MAKE(NOTE_D3, 0, 0),
                ROW_MAKE(NOTE_E4, 0, 0),
                ROW_MAKE(NOTE_OFF, 0, 0),
                //
                ROW_MAKE(NOTE_A4, 0, 0),
                ROW_MAKE(NOTE_A4, 0, 0),
                ROW_MAKE(NOTE_A4, 0, 0),
                ROW_MAKE(NOTE_OFF, 0, 0),
                //
                ROW_MAKE(NOTE_E5, 0, 0),
                ROW_MAKE(NOTE_E5, 0, 0),
                ROW_MAKE(NOTE_E5, 0, 0),
                ROW_MAKE(NOTE_OFF, 0, 0),
                //
                ROW_MAKE(NOTE_D5, 0, 0),
                ROW_MAKE(NOTE_D3, 0, 0),
                ROW_MAKE(NOTE_G2, 0, 0),
                ROW_MAKE(NOTE_D3, 0, 0),
                //
                ROW_MAKE(NOTE_C5, 0, 0),
                ROW_MAKE(NOTE_D3, 0, 0),
                ROW_MAKE(NOTE_C5, 0, 0),
                ROW_MAKE(NOTE_OFF, 0, 0),
                //
                ROW_MAKE(NOTE_A4, 0, 0),
                ROW_MAKE(0, 0, 0),
                ROW_MAKE(0, 0, 0),
                ROW_MAKE(0, 0, 0),
                //
                ROW_MAKE(NOTE_A4, 0, 0),
                ROW_MAKE(NOTE_A4, 0, 0),
                ROW_MAKE(NOTE_A4, 0, 0),
                ROW_MAKE(NOTE_OFF, 0, 0),
            },
    },
};

Pattern patterns[] = {
    {
        .channels = p_0_channels,
    },
    {
        .channels = p_1_channels,
    },
};

uint8_t order_list[] = {
    0,
    0,
    1,
    0,
};

Song song = {
    .channels_count = 1,
    .patterns_count = 2,
    .patterns = patterns,

    .order_list_size = 4,
    .order_list = order_list,

    .ticks_per_second = 60,
};

void tracker_message(TrackerMessage message, void* context) {
    FuriMessageQueue* queue = context;
    furi_assert(queue);
    furi_message_queue_put(queue, &message, 0);
}

int32_t zero_tracker_app(void* p) {
    UNUSED(p);

    FuriMessageQueue* queue = furi_message_queue_alloc(8, sizeof(TrackerMessage));
    Tracker* tracker = tracker_alloc();
    tracker_set_message_callback(tracker, tracker_message, queue);
    tracker_set_song(tracker, &song);
    tracker_start(tracker);

    while(1) {
        TrackerMessage message;
        FuriStatus status = furi_message_queue_get(queue, &message, portMAX_DELAY);
        if(status == FuriStatusOk) {
            if(message.type == TrackerPositionChanged) {
                uint8_t order_list_index = message.data.position.order_list_index;
                uint8_t row = message.data.position.row;
                uint8_t pattern = song.order_list[order_list_index];
                FURI_LOG_I("Tracker", "O:%d P:%d R:%d", order_list_index, pattern, row);
            } else if(message.type == TrackerEndOfSong) {
                FURI_LOG_I("Tracker", "End of song");
                break;
            }
        }
    }

    tracker_stop(tracker);
    tracker_free(tracker);
    furi_message_queue_free(queue);

    return 0;
}