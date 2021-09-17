#pragma once
#include "../archive_i.h"

#define DEFAULT_TAB_DIR InputKeyRight //default tab swith direction

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
inline bool is_known_app(ArchiveFileTypeEnum type) {
    return (type != ArchiveFileTypeFolder && type != ArchiveFileTypeUnknown);
}

void update_offset(ArchiveMainView* main_view);

size_t archive_file_array_size(ArchiveMainView* main_view);
void archive_file_array_remove_selected(ArchiveMainView* main_view);
void archive_file_array_clean(ArchiveMainView* main_view);

ArchiveFile_t* archive_get_current_file(ArchiveMainView* main_view);
ArchiveTabEnum archive_get_tab(ArchiveMainView* main_view);

void archive_set_tab(ArchiveMainView* main_view, ArchiveTabEnum tab);

uint8_t archive_get_depth(ArchiveMainView* main_view);
const char* archive_get_path(ArchiveMainView* main_view);
const char* archive_get_name(ArchiveMainView* main_view);

void archive_set_name(ArchiveMainView* main_view, const char* name);

void archive_view_add_item(ArchiveMainView* main_view, FileInfo* file_info, const char* name);

void archive_show_file_menu(ArchiveMainView* main_view, bool show);
bool archive_in_file_menu(ArchiveMainView* main_view);

void archive_switch_dir(ArchiveMainView* main_view, const char* path);
void archive_switch_tab(ArchiveMainView* main_view, InputKey key);
void archive_enter_dir(ArchiveMainView* main_view, string_t name);
void archive_leave_dir(ArchiveMainView* main_view);

void archive_browser_update(ArchiveMainView* main_view);
