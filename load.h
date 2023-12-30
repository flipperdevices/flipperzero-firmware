#pragma once

#include "common.h"
#include <storage/storage.h>

typedef struct {
    FuriString* solution;
    FuriString* board;
    FuriString* title;
} LevelData;

LevelData* alloc_level_data();
void free_level_data(LevelData* ld);

int load_level_row(uint8_t* pb, const char* psz, const char* pszMax);
bool parse_level_notation(const char* pszLevel, PlayGround* level);
bool load_level(Storage* storage, const char* path, int level, LevelData* levelData);