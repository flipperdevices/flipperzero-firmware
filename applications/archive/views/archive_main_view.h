#pragma once

#include <gui/gui_i.h>
#include <gui/view.h>
#include <gui/canvas.h>
#include <gui/elements.h>
#include <furi.h>
#include <storage/storage.h>

#define MAX_LEN_PX 110
#define MAX_NAME_LEN 255
#define FRAME_HEIGHT 12
#define MENU_ITEMS 4

typedef enum {
    ArchiveFileTypeIButton,
    ArchiveFileTypeNFC,
    ArchiveFileTypeSubGhz,
    ArchiveFileTypeLFRFID,
    ArchiveFileTypeIrda,
    ArchiveFileTypeFolder,
    ArchiveFileTypeUnknown,
    AppIdTotal,
} ArchiveFileTypeEnum;

typedef enum {
    ArchiveTabFavorites,
    ArchiveTabLFRFID,
    ArchiveTabSubGhz,
    ArchiveTabNFC,
    ArchiveTabIButton,
    ArchiveTabIrda,
    ArchiveTabBrowser,
    ArchiveTabTotal,
} ArchiveTabEnum;

typedef struct {
    string_t name;
    ArchiveFileTypeEnum type;
    bool fav;
} ArchiveFile_t;

static const char* known_ext[] = {
    [ArchiveFileTypeIButton] = ".ibtn",
    [ArchiveFileTypeNFC] = ".nfc",
    [ArchiveFileTypeSubGhz] = ".sub",
    [ArchiveFileTypeLFRFID] = ".rfid",
    [ArchiveFileTypeIrda] = ".ir",
};

static inline const char* get_tab_ext(ArchiveTabEnum tab) {
    switch(tab) {
    case ArchiveTabIButton:
        return known_ext[ArchiveFileTypeIButton];
    case ArchiveTabNFC:
        return known_ext[ArchiveFileTypeNFC];
    case ArchiveTabSubGhz:
        return known_ext[ArchiveFileTypeSubGhz];
    case ArchiveTabLFRFID:
        return known_ext[ArchiveFileTypeLFRFID];
    case ArchiveTabIrda:
        return known_ext[ArchiveFileTypeIrda];
    default:
        return "*";
    }
}

typedef enum {
    ArchiveBrowserEventRename,
    ArchiveBrowserEventExit,
    ArchiveBrowserEventLeaveDir,
} ArchiveBrowserEvent;

typedef struct ArchiveMainView ArchiveMainView;

typedef void (*ArchiveMainViewCallback)(ArchiveBrowserEvent event, void* context);

void archive_browser_set_callback(
    ArchiveMainView* main_view,
    ArchiveMainViewCallback callback,
    void* context);

View* archive_main_get_view(ArchiveMainView* main_view);

ArchiveMainView* main_view_alloc();
void main_view_free(ArchiveMainView* main_view);

static void ArchiveFile_t_init(ArchiveFile_t* obj) {
    obj->type = ArchiveFileTypeUnknown;
    string_init(obj->name);
}

static void ArchiveFile_t_init_set(ArchiveFile_t* obj, const ArchiveFile_t* src) {
    obj->type = src->type;
    string_init_set(obj->name, src->name);
}

static void ArchiveFile_t_set(ArchiveFile_t* obj, const ArchiveFile_t* src) {
    obj->type = src->type;
    string_set(obj->name, src->name);
}

static void ArchiveFile_t_clear(ArchiveFile_t* obj) {
    string_clear(obj->name);
}

ARRAY_DEF(
    files_array,
    ArchiveFile_t,
    (INIT(API_2(ArchiveFile_t_init)),
     SET(API_6(ArchiveFile_t_set)),
     INIT_SET(API_6(ArchiveFile_t_init_set)),
     CLEAR(API_2(ArchiveFile_t_clear))))

size_t archive_file_array_size(ArchiveMainView* main_view);
void archive_file_array_remove_selected(ArchiveMainView* main_view);
void archive_file_array_clean(ArchiveMainView* main_view);

void archive_view_add_item(ArchiveMainView* main_view, FileInfo* file_info, const char* name);
void archive_browser_update(ArchiveMainView* main_view);

ArchiveFile_t* archive_get_current_file(ArchiveMainView* main_view);
const char* archive_get_path(ArchiveMainView* main_view);
const char* archive_get_name(ArchiveMainView* main_view);
void archive_set_name(ArchiveMainView* main_view, const char* name);

static inline bool is_known_app(ArchiveFileTypeEnum type) {
    return (type != ArchiveFileTypeFolder && type != ArchiveFileTypeUnknown);
}
