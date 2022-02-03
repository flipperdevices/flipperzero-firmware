#include "sd_update_progress.h"

#include <furi_hal.h>

static const char* update_state_text[] = {
    "Unknown",
    "Mounting storage",
    "Opening update file",
    "Validating image",
    "",
    "Writing",
    "Verifying",
    "Completed",
    "Error",
    "Hardware mismatch",
    "Update failed",
};

static SdUpdateProgress progress = {0};

void sdcard_update_progress_set_stage(const SdUpdateState state, const uint8_t progress_pct) {
    bool render_update = (progress.operation != state) ||
                         (((progress_pct - progress.rendered_progress) > 4));
    progress.operation = state;
    progress.progress = progress_pct;
    if(render_update) {
        progress.rendered_progress = progress.progress;
        sdcard_update_progress_render();
    }
}

void sdcard_update_progress_update(const uint8_t progress_pct) {
    sdcard_update_progress_set_stage(progress.operation, progress_pct);
}

void sdcard_update_progress_render() {
    furi_hal_fb_clear();
    u8g2_t* fb = furi_hal_fb_get();
    u8g2_DrawStr(fb, 2, 8, "Update & Recovery Mode");
    u8g2_DrawStr(fb, 2, 21, "SD card .dfu update");
    const char* stage_text = update_state_text[progress.operation];
    u8g2_DrawStr(fb, 16, 39, stage_text);
    u8g2_DrawBox(fb, 12, 47, 104, 12);
    u8g2_SetDrawColor(fb, 0x0);
    u8g2_DrawBox(fb, 13, 48, 102, 10);

    u8g2_SetDrawColor(fb, 0x1);
    uint8_t progress_pct = progress.progress;
    if(progress_pct > 100) {
        progress_pct = 100;
    }
    u8g2_DrawBox(fb, 14, 49, progress_pct, 8);

    furi_hal_fb_present();
}

const SdUpdateProgress* sdcard_update_progress_get() {
    return &progress;
}
