#pragma once

#include <gui/gui_i.h>
#include <gui/view.h>
#include <gui/canvas.h>
#include <gui/elements.h>
#include <furi.h>

#define MOODS_TOTAL 3
#define BUTTHURT_MAX 3

typedef enum {
    FlipperPassportEventExit,
} FlipperPassportEvent;

typedef struct FlipperPassportView FlipperPassportView;

typedef void (*FlipperPassportViewCallback)(FlipperPassportEvent event, void* context);

struct FlipperPassportView {
    View* view;
    FlipperPassportViewCallback callback;
    void* context;
};

typedef struct {
    const char* name;
    uint32_t exp;
    uint32_t butthurt;
    uint32_t current_level;
    uint8_t portrait_level;
} FlipperPassportViewModel;

void flipper_world_passport_set_callback(
    FlipperPassportView* main_view,
    FlipperPassportViewCallback callback,
    void* context);

View* flipper_world_passport_get_view(FlipperPassportView* main_view);

FlipperPassportView* flipper_world_passport_alloc();
void flipper_world_passport_free(FlipperPassportView* main_view);
