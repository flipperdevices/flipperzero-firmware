#include "dolphin/animations/animation_manager.h"
#include "furi/record.h"
#include "animation_storage.h"
#include "m-string.h"
#include "storage/storage.h"
#include "animation_storage_i.h"
#include <cstring>


void animation_storage_fill_animation_list(StorageAnimationList_t animation_list) {
    furi_assert(sizeof(StorageAnimationList_t) == sizeof(void*));

    StorageAnimationList_reset(animation_list);

    // add hard-coded animations
    for (int i = 0; i < COUNT_OF(StorageAnimationInternal); ++i) {
        StorageAnimationList_push_back(animation_list, &StorageAnimationInternal[i]);
    }

    Storage* storage = furi_record_open("storage");
    FS_Error sd_status = storage_sd_status(storage);
    if(sd_status == FSE_OK) {
        // read file and add StorageAnimationList
    } else {
        // do nothing
    }

    furi_record_close("storage");

}

StorageAnimation* animation_storage_find_animation(const char* name) {
    // look through internal animations
    for (int i = 0; i < COUNT_OF(StorageAnimationInternal); ++i) {
        if (!strcmp(StorageAnimationInternal[i].meta.name, name)) {
            return &StorageAnimationInternal[i];
        }
    }

    StorageAnimationList_t animation_list;
    StorageAnimationList_init(animation_list);

    for M_EACH(item, animation_list, StorageAnimationList_t) {
        if (!strcmp((*item)->meta.name, name)) {
            animation_storage_cache_animation(storage_animation);
        }
    }
    StorageAnimationList_clear(animation_list);

    furi_assert(iterator);

    if (iterator->external) {
        StorageAnimation* storage_animation = furi_alloc(sizeof(StorageAnimation));
        string_t filename;
        string_init_printf(filename, "%s%s", name, ".anm");
        Storage* storage = furi_record_open("storage");
        FS_Error sd_status = storage_sd_status(storage);
        if(sd_status == FSE_OK) {
            // read file and add StorageAnimationList
        } else {
            // do nothing
        }

        furi_record_close("storage");
        return storage_animation;
    } else {
    }

    return NULL;
}

AnimationMeta* animation_storage_get_meta(StorageAnimation* storage_animation) {
    furi_assert(storage_animation);
    return &storage_animation->meta;
}

const BubbleAnimation* animation_storage_get_bubble_animation(StorageAnimation* storage_animation) {
    furi_assert(storage_animation);
    animation_storage_cache_animation(storage_animation);
    furi_assert(storage_animation->animation);
    return storage_animation->animation;
}

void animation_storage_free_animation(StorageAnimation* storage_animation) {
    furi_assert(storage_animation);

    if (storage_animation->external) {
        // TODO: correct destructor
        free((void*) storage_animation->animation);
        free(storage_animation);
    }
}

void animation_storage_cache_animation(StorageAnimation* storage_animation) {
    furi_assert(storage_animation);

    if (storage_animation->external) {
        if (!storage_animation->animation) {
            // TODO: read file decode Protobuf
        }
    }

    furi_assert(storage_animation->animation);
}

