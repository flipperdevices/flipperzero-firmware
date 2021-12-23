#include "dolphin/animations/animation_manager.h"
#include "file-worker.h"
#include "furi/record.h"
#include "animation_storage.h"
#include "m-string.h"
#include "pb.h"
#include "pb_decode.h"
#include "storage/storage.h"
#include "animation_storage_i.h"
#include "animation_storage.pb.h"


static bool decode_file_meta(pb_istream_t *stream, const pb_field_t *field, void **arg) {
    furi_assert(stream);
    furi_assert(field);
    furi_assert(arg && *arg);
    furi_assert(field->tag == PB_FA_StorageAnimationMetaFile_meta_tag);

    StorageAnimationList_t* animation_list = *arg;
    StorageAnimation* storage_animation = *StorageAnimationList_push_raw(*animation_list);
    return pb_decode(stream, PB_FA_StorageAnimationMeta_fields, &storage_animation->meta);
}

static bool stream_file_read(pb_istream_t *stream, pb_byte_t *buf, size_t count) {
    furi_assert(stream);
    furi_assert(buf);
    furi_assert(count);

    return file_worker_read(stream->state, buf, count);
}

void animation_storage_fill_animation_list(StorageAnimationList_t animation_list) {
    furi_assert(sizeof(StorageAnimationList_t) == sizeof(void*));
    bool result = false;
    furi_assert(StorageAnimationList_size(animation_list));

    Storage* storage = furi_record_open("storage");
    FS_Error sd_status = storage_sd_status(storage);
    if(sd_status == FSE_OK) {
        FileWorker* file_worker = file_worker_alloc(true);
        if(file_worker_open(file_worker, "/ext/assets/meta_file", FSAM_READ, FSOM_OPEN_EXISTING)) {

            pb_istream_t istream = {
                .bytes_left = 0,
                .callback = NULL,
                .errmsg = NULL,
                .state = file_worker,
            };
            PB_FA_StorageAnimationMetaFile meta_file = {
                .meta.arg = &animation_list,
                .meta.funcs.decode = decode_file_meta,
            };
            result = pb_decode(&istream, PB_FA_StorageAnimationMetaFile_fields, &meta_file);
        }
        file_worker_free(file_worker);
        // read file and add StorageAnimationList
    } else {
        // do nothing
    }

    if (!result) {
        StorageAnimationList_reset(animation_list);
    }


    // add hard-coded animations
    for (int i = 0; i < COUNT_OF(StorageAnimationInternal); ++i) {
        StorageAnimationList_push_back(animation_list, &StorageAnimationInternal[i]);
    }

    furi_record_close("storage");
}

void animation_storage_list_free(StorageAnimationList_t animation_list) {


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

