
#include "desktop_animation.h"

static const Icon* idle_scenes[] = {&A_Wink_128x64, &A_WatchingTV_128x64};

const Icon* desktop_get_icon() {
#if 0
    Dolphin* dolphin = furi_record_open("dolphin");
    DolphinStats stats = dolphin_stats(dolphin);
#endif

    uint8_t new = random() % COUNT_OF(idle_scenes);
    return idle_scenes[new];
}
