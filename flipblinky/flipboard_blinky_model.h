#pragma once

#include <furi.h>
#include "common/flipboard_model.h"
#include "fxbm.h"

typedef enum FlipboardBlinkySource {
    FlipboardBlinkySourceAssets = 'A',
    FlipboardBlinkySourceFXBM = 'X',
    FlipboardBlinkySourceText = 'T',
} FlipboardBlinkySource;

typedef enum FlipboardBlinkyJustification {
    FlipboardBlinkyJustificationLeft = 'L',
    FlipboardBlinkyJustificationCenter = 'C',
    FlipboardBlinkyJustificationRight = 'R',
} FlipboardBlinkyJustification;

extern FlipboardBlinkySource flipboard_blinky_source_order[];

extern FlipboardBlinkyJustification flipboard_blinky_justification_order[];

typedef struct FlipboardBlinkyRenderModel {
    FlipboardBlinkySource source;
    FlipboardBlinkyJustification justification;
    char line[4][64];
    char path[256];
    bool show_details;
} FlipboardBlinkyRenderModel;

typedef struct FlipboardBlinkyModel {
    FuriTimer* timer;
    uint32_t period_ms;
    uint32_t colors[6];
    uint8_t effect_counter;
    uint8_t effect_id;
    uint8_t max_effect_id;
    uint32_t show_details_until;
    FxbmFile* fxbm;
    FlipboardBlinkyRenderModel render_model;
} FlipboardBlinkyModel;

FlipboardBlinkyModel* flipboard_blinky_model_alloc(FlipboardModel* context);
void flipboard_blinky_model_free(FlipboardBlinkyModel* fbm);
uint8_t flipboard_blinky_get_justification_index(FlipboardBlinkyModel* fbm);
uint8_t flipboard_blinky_get_source_index(FlipboardBlinkyModel* fbm);
FlipboardBlinkySource flipboard_blinky_get_source(FlipboardBlinkyModel* fbm);
FlipboardBlinkyJustification flipboard_blinky_get_justification(FlipboardBlinkyModel* fbm);
void flipboard_blinky_set_source_by_index(FlipboardBlinkyModel* fbm, uint8_t index);
void flipboard_blinky_set_justification_by_index(FlipboardBlinkyModel* fbm, uint8_t index);