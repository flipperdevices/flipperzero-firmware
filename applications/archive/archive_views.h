#pragma once

#include <gui/gui_i.h>
#include <gui/canvas.h>
#include <gui/elements.h>
#include <furi.h>
#include <filesystem-api.h>

#define FILENAME_COUNT 128
#define MAX_LEN_PX 98
#define FRAME_HEIGHT 12

typedef enum {
    FileTypeIButton,
    FileTypeNFC,
    FileTypeSubOne,
    FileTypeLFRFID,
    FileTypeIrda,
    FileTypeFolder,
    FileTypeUnknown,
    AppIdTotal,
} FileTypeEnum;

typedef struct {
    string_t name;
    FileTypeEnum type;
} ArchiveFile_t;

typedef struct {
    uint8_t tab_idx;
    uint8_t idx;
    uint8_t list_offset;

    ArchiveFile_t files[FILENAME_COUNT];

    uint16_t first_file_index;
    uint16_t file_count;

} ArchiveViewModelDefault;

void archive_view_render(Canvas* canvas, void* model);
