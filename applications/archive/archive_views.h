#pragma once

#include <gui/gui_i.h>
#include <gui/canvas.h>
#include <gui/elements.h>
#include <furi.h>

#define FILENAME_COUNT 128

typedef struct {
    uint8_t tab_idx;
    uint8_t idx;
    uint8_t list_offset;

    string_t filename[FILENAME_COUNT];

    uint16_t first_file_index;
    uint16_t file_count;

} ArchiveViewModelDefault;

void archive_view_render(Canvas* canvas, void* model);
