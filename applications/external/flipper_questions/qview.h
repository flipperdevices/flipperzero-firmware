#ifndef QVIEW
#define QVIEW

#include <gui/view.h>
#include <furi.h>
#include <furi_hal.h>
#include <gui/elements.h>
#include "qdb.h"

typedef struct QView {
    View* view;
} QView;

typedef struct QModel {
    uint8_t gi;
    uint16_t qi;
    q_char** questions;
} QModel;

void q_draw_callback(Canvas* canvas, void* context);

QView* q_main_view_alloc();

void q_main_view_free(QView* view);

void q_reset_view(QView* view, const uint8_t gi);

#endif
