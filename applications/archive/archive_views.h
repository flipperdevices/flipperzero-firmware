#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <gui/gui_i.h>
#include <gui/canvas.h>
#include <gui/elements.h>

#include <input/input.h>
#include <furi.h>

typedef struct {
    uint8_t tab_idx;
    uint8_t idx;
    const char** list;
    uint8_t list_offset;
} ArchiveViewModelDefault;

void archive_view_favorites(Canvas* canvas, void* model);

void archive_view_all(Canvas* canvas, void* model);

void archive_view_ibutton(Canvas* canvas, void* model);
