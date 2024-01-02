#pragma once

#include <storage/storage.h>
#include "common.h"

#define ASSETS_LEVELS_COUNT 9
extern char* assetLevels[];

typedef struct {
    FuriString* id;
    FuriString* title;
    FuriString* author;
    FuriString* description;
    FuriString* url;
    uint8_t maxLevel;
} LevelSet;

typedef struct {
    FuriString* solution;
    FuriString* board;
    FuriString* title;
    unsigned int gamePar;
} LevelData;

LevelSet* alloc_level_set();
void free_level_set(LevelSet* ls);
LevelData* alloc_level_data();
void free_level_data(LevelData* ld);

bool load_level_set(Storage* storage, FuriString* levelSetId, LevelSet* levelSet);
bool load_last_level(FuriString* lastLevelSetId, uint8_t* levelNo);
bool save_last_level(FuriString* lastLevelSetId, uint8_t levelNo);
void delete_progress();

int load_level_row(uint8_t* pb, const char* psz, const char* pszMax);
bool parse_level_notation(const char* pszLevel, PlayGround* level);
bool load_level(Storage* storage, FuriString* levelSetId, int level, LevelData* levelData);

bool level_set_id_to_path(Storage* storage, FuriString* levelSetId, size_t maxSize, char* path);