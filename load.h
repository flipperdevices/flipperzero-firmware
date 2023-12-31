#pragma once

#include <storage/storage.h>
#include "common.h"

typedef struct {
    FuriString* title;
    FuriString* author;
    FuriString* description;
    FuriString* file;
    uint8_t maxLevel;
} LevelSet;

LevelSet* alloc_level_set();
void free_level_set(LevelSet* ls);
bool load_level_set(Storage* storage, const char* path, LevelSet* levelSet);

//-----------------------------------------------------------------------------

typedef struct {
    FuriString* solution;
    FuriString* board;
    FuriString* title;
    unsigned int gamePar;
} LevelData;

LevelData* alloc_level_data();
void free_level_data(LevelData* ld);

int load_level_row(uint8_t* pb, const char* psz, const char* pszMax);
bool parse_level_notation(const char* pszLevel, PlayGround* level);
bool load_level(Storage* storage, const char* path, int level, LevelData* levelData);
