
#include "desktop_animation.h"
#include "dolphin/dolphin.h"

typedef enum {
    DolphinMoodHappy,
    DolphinMoodOk,
    DolphinMoodSad,
    DolphinMoodAngry,
    DolphinTotalMoods,
} DolphinMoods;

static const Icon* idle_scenes[DolphinTotalMoods][2] = {
    [DolphinMoodHappy] = {&A_Wink_128x64, &A_WatchingTV_128x64},
    [DolphinMoodOk] = {&A_Wink_128x64, &A_WatchingTV_128x64},
    [DolphinMoodSad] = {&A_Wink_128x64, &A_WatchingTV_128x64},
    [DolphinMoodAngry] = {&A_Wink_128x64, &A_WatchingTV_128x64},
    [DolphinMoodOk] = {&A_Wink_128x64, &A_WatchingTV_128x64},
};

const Icon* desktop_get_icon() {
    Dolphin* dolphin = furi_record_open("dolphin");

    DolphinStats stats = dolphin_stats(dolphin);

    FURI_LOG_E(
        "Desktop",
        "Get icon, state - ic: %d bh: %d timestamp: %ld",
        stats.icounter,
        stats.butthurt,
        stats.timestamp);

    uint8_t new = random() % COUNT_OF(idle_scenes[DolphinMoodHappy]);

    return idle_scenes[DolphinMoodHappy][new];
}
