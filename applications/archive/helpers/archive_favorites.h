#pragma once
#include "file-worker.h"
#include "../views/archive_main_view.h"

#define ARCHIVE_FAV_PATH "/any/favorites.txt"
#define ARCHIVE_FAV_TEMP_PATH "/any/favorites.tmp"

void archive_add_to_favorites(string_t file_path, string_t name);
bool archive_favorites_rename(string_t file_path, string_t selected, string_t dst);
bool archive_favorites_delete(string_t file_path, string_t selected);
bool archive_is_favorite(string_t file_path, string_t selected);
bool archive_favorites_read(ArchiveMainView* archive_view);
