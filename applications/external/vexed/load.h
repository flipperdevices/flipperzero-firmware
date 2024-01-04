#pragma once

#include <storage/storage.h>
#include "common.h"

#define ASSETS_LEVELS_COUNT 9
#define MAX_LEVELS_PER_SET 100

extern char* assetLevels[];

typedef struct {
    uint16_t moves;
    bool spoiled;
} LevelScore;

typedef struct {
    FuriString* id;
    FuriString* title;
    FuriString* author;
    FuriString* description;
    FuriString* url;
    uint8_t maxLevel;
    LevelScore scores[MAX_LEVELS_PER_SET];
    uint8_t pars[MAX_LEVELS_PER_SET];
} LevelSet;

typedef struct {
    FuriString* solution;
    FuriString* board;
    FuriString* title;
    unsigned int gamePar;
} LevelData;

typedef struct {
    FuriString** ids;
    int count;
} LevelList;

//-----------------------------------------------------------------------------

LevelSet* alloc_level_set();
void free_level_set(LevelSet* ls);
LevelData* alloc_level_data();
void free_level_data(LevelData* ld);
void init_level_list(LevelList* ls, int capacity);
void free_level_list(LevelList* ls);

//-----------------------------------------------------------------------------

bool ensure_paths(Storage* storage);
bool level_set_id_to_path(Storage* storage, FuriString* levelSetId, size_t maxSize, char* path);

//-----------------------------------------------------------------------------

bool load_level_set(
    Storage* storage,
    FuriString* levelSetId,
    LevelSet* levelSet,
    FuriString* errorMsg);
bool load_last_level(FuriString* lastLevelSetId, uint8_t* levelNo);
bool save_last_level(FuriString* lastLevelSetId, uint8_t levelNo);
bool load_set_scores(Storage* storage, FuriString* levelSetId, LevelScore* scores);
bool save_set_scores(FuriString* levelSetId, LevelScore* scores);
void delete_progress(LevelScore* scores);

//-----------------------------------------------------------------------------

int load_level_row(uint8_t* pb, const char* psz, const char* pszMax);
bool parse_level_notation(const char* pszLevel, PlayGround* level);
bool load_level(
    Storage* storage,
    FuriString* levelSetId,
    int level,
    LevelData* levelData,
    FuriString* errorMsg);

//-----------------------------------------------------------------------------

void list_extra_levels(Storage* storage, LevelList* levelList);
void mark_set_invalid(Storage* storage, FuriString* levelSetId, FuriString* errorMsg);
