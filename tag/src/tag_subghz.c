#include "tag_subghz.h"
#include <lib/subghz/subghz_tx_rx_worker.h>

static TagSubGhzMode mode = SubGhzUninitialised;

// lifecycle methods

TagSubGhzMode tag_subghz_mode_get() {
    return mode;
}

void tag_subghz_init() {
    furi_assert(mode == SubGhzUninitialised);

    mode = SubGhzReady;
}
