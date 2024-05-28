#include "flipboard_blinky_model.h"
#include "app.h"

FlipboardBlinkySource flipboard_blinky_source_order[] = {
    FlipboardBlinkySourceAssets,
    FlipboardBlinkySourceFXBM,
    FlipboardBlinkySourceText};

FlipboardBlinkyJustification flipboard_blinky_justification_order[] = {
    FlipboardBlinkyJustificationLeft,
    FlipboardBlinkyJustificationCenter,
    FlipboardBlinkyJustificationRight};

/**
 * @brief This method is invoked to allocate a FlipboardBlinkyModel*.
 * @param context The FlipboardModel* context.
 * @return FlipboardBlinkyModel*.
*/
FlipboardBlinkyModel* flipboard_blinky_model_alloc(FlipboardModel* context) {
    FlipboardBlinkyModel* fbm = malloc(sizeof(FlipboardBlinkyModel));
    fbm->timer = furi_timer_alloc(flipboard_tick_callback, FuriTimerTypePeriodic, context);
    fbm->period_ms = 200;
    fbm->effect_id = 1;
    fbm->max_effect_id = 6;
    fbm->show_details_until = 0;
    fbm->render_model.source = FlipboardBlinkySourceAssets;
    fbm->render_model.justification = FlipboardBlinkyJustificationCenter;
    fbm->render_model.line[0][0] = '\0';
    fbm->render_model.line[1][0] = '\0';
    fbm->render_model.line[2][0] = '\0';
    fbm->render_model.line[3][0] = '\0';
    strncpy(fbm->render_model.path, "/ext/apps_data/flipboard/blinky.fxbm", 256);
    fbm->fxbm = fxbm_alloc(fbm->render_model.path);
    return fbm;
}

uint8_t flipboard_blinky_get_justification_index(FlipboardBlinkyModel* fbm) {
    for(uint8_t i = 0; i < COUNT_OF(flipboard_blinky_justification_order); i++) {
        if(fbm->render_model.justification == flipboard_blinky_justification_order[i]) {
            return i;
        }
    }
    return 0;
}

uint8_t flipboard_blinky_get_source_index(FlipboardBlinkyModel* fbm) {
    for(uint8_t i = 0; i < COUNT_OF(flipboard_blinky_source_order); i++) {
        if(fbm->render_model.source == flipboard_blinky_source_order[i]) {
            return i;
        }
    }
    return 0;
}

FlipboardBlinkySource flipboard_blinky_get_source(FlipboardBlinkyModel* fbm) {
    return fbm->render_model.source;
}

FlipboardBlinkyJustification flipboard_blinky_get_justification(FlipboardBlinkyModel* fbm) {
    return fbm->render_model.justification;
}

void flipboard_blinky_set_source_by_index(FlipboardBlinkyModel* fbm, uint8_t index) {
    fbm->render_model.source = flipboard_blinky_source_order[index];
}

void flipboard_blinky_set_justification_by_index(FlipboardBlinkyModel* fbm, uint8_t index) {
    fbm->render_model.justification = flipboard_blinky_justification_order[index];
}

/**
 * @brief This method is invoked to free a FlipboardBlinkyModel*.
 * @param fbm The FlipboardBlinkyModel* to free.
*/
void flipboard_blinky_model_free(FlipboardBlinkyModel* fbm) {
    if(fbm->timer) {
        furi_timer_free(fbm->timer);
    }
    free(fbm);
}
