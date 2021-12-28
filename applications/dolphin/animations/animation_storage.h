#pragma once
#include "animation_storage.pb.h"
#include <stdint.h>
#include <m-list.h>
#include "views/bubble_animation_view.h"
#include <m-string.h>

#define HARDCODED_ANIMATION_NAME "tv"
#define NO_SD_ANIMATION_NAME "no_sd"
#define BAD_BATTERY_ANIMATION_NAME "bad_battery"

typedef struct StorageAnimation StorageAnimation;

typedef struct { 
    string_t name; 
    uint8_t min_butthurt; 
    uint8_t max_butthurt; 
    uint8_t min_level; 
    uint8_t max_level; 
    uint8_t weight; 
} StorageAnimationMeta;


LIST_DEF(StorageAnimationList, StorageAnimation*, M_PTR_OPLIST)
#define M_OPL_StorageAnimationList_t() LIST_OPLIST(StorageAnimationList)

void animation_storage_fill_animation_list(StorageAnimationList_t*);
const BubbleAnimation* animation_storage_get_bubble_animation(StorageAnimation*);
void animation_storage_free_storage_animation(StorageAnimation** storage_animation);

StorageAnimation* animation_storage_find_animation(const char* name);
StorageAnimationMeta* animation_storage_get_meta(StorageAnimation* storage_animation);
const BubbleAnimation* animation_storage_get_bubble_animation(StorageAnimation* storage_animation);
void animation_storage_cache_animation(StorageAnimation* storage_animation);

