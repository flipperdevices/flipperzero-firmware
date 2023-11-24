#pragma once

#include <doc_i.h>
#include <storage/storage.h>

#define DOC_BACKUP_FILE_EXTENSION ".lvl"
#define DOC_APP_FOLDER "/data/"

bool doc_get_backup_path(Doc* app);

bool doc_dolphin_state_load_from_path(DolphinState* dolphin_state, const char* path);
bool doc_dolphin_state_load(DolphinState* dolphin_state);

bool doc_dolphin_state_save_to_path(DolphinState* dolphin_state, const char* path, const char* file_name);
bool doc_dolphin_state_save(DolphinState* dolphin_state, bool restart);