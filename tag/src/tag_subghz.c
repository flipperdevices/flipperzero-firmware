#include "tag_subghz.h"

static TagSubGhzMode mode = SubGhzUninitialised;

// lifecycle methods

TagSubGhzMode tag_subghz_mode_get() {
    return mode;
}

void tag_subghz_init() {
    furi_assert(mode == SubGhzUninitialised);
    // TODO: init
    mode = SubGhzReady;
}
