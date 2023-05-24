#include "tag_subghz.h"
#include "tag_app.h"

static TagSubGhzMode mode = SubGhzUninitialised;

// lifecycle methods

TagSubGhzMode tag_subghz_mode_get() {
    return mode;
}

void tag_subghz_init() {
    FURI_LOG_T(TAG, "tag_subghz_init");
    FURI_LOG_D(TAG, "tag_subghz_init assertion: mode == SubGhzUninitialised");
    furi_assert(mode == SubGhzUninitialised);
    // TODO: init
    mode = SubGhzReady;
}
