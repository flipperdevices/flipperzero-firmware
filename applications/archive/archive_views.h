#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <gui/gui_i.h>
#include <gui/canvas.h>
#include <gui/elements.h>
#include <m-string.h>
#include <sys/param.h>
#include <input/input.h>
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

void archive_view_favorites(Canvas* canvas, void* model);

void archive_view_all(Canvas* canvas, void* model);

void archive_view_ibutton(Canvas* canvas, void* model);
