#include "dolphin/animations/animation_manager.h"
#include "file-worker.h"
#include "furi/record.h"
#include "animation_storage.h"
#include "m-string.h"
#include "pb.h"
#include "pb_decode.h"
#include "storage/filesystem-api-defines.h"
#include "storage/storage.h"
#include "animation_storage_i.h"
#include "animation_storage.pb.h"

#define MANIFEST_FILE "/ext/assets/file_animations/manifest"
#define TAG "ANM_STRG"
#define DEBUG_PB 0

void animation_storage_list_free(StorageAnimationList_t animation_list);

static bool decode_file_meta(pb_istream_t *stream, const pb_field_t *field, void **arg) {
    furi_assert(stream);
    furi_assert(field);
    furi_assert(arg && *arg);
    furi_assert(field->tag == PB_FA_StorageAnimationMetaFile_meta_tag);

    StorageAnimationList_t* animation_list = *arg;
    StorageAnimation* storage_animation = furi_alloc(sizeof(StorageAnimation));
    StorageAnimationList_push_back(*animation_list, storage_animation);
    storage_animation->external = true;
    storage_animation->animation = NULL;
    return pb_decode(stream, PB_FA_StorageAnimationMeta_fields, &storage_animation->meta);
}

static bool stream_file_read(pb_istream_t *stream, pb_byte_t *buf, size_t count) {
    furi_assert(stream);
    furi_assert(buf);
    furi_assert(count);

#if DEBUG_PB == 1
    static size_t dbg_buf_cnt = 0;

    if ((dbg_buf_cnt + count) > sizeof(dbg_buffer)) {
        furi_assert(0);
        stream->bytes_left = 0;
        return false;
    } else {
        memcpy(buf, &dbg_buffer[dbg_buf_cnt], count);
        dbg_buf_cnt += count;
        return true;
    }
#else
    uint16_t read = storage_file_read(stream->state, buf, count);
    furi_assert(read <= stream->bytes_left);
    if (read != count) {
        stream->bytes_left = 0;
    }
    return read == count;
#endif
}

void animation_storage_fill_animation_list(StorageAnimationList_t* animation_list) {
    furi_assert(sizeof(StorageAnimationList_t) == sizeof(void*));
    FileInfo fileinfo;
    bool result = false;
    furi_assert(!StorageAnimationList_size(*animation_list));

    Storage* storage = furi_record_open("storage");
    FS_Error sd_status = storage_sd_status(storage);
    if(sd_status == FSE_OK) {
        File* file = storage_file_alloc(storage);
        FS_Error error = storage_common_stat(storage, MANIFEST_FILE, &fileinfo);
        result = (error == FSE_OK) && fileinfo.size;
        if(result && storage_file_open(file, MANIFEST_FILE, FSAM_READ, FSOM_OPEN_EXISTING)) {
            pb_istream_t istream = {
#if DEBUG_PB == 1
                .bytes_left = sizeof(dbg_buffer),
#else
                .bytes_left = fileinfo.size,
#endif
                .callback = stream_file_read,
                .errmsg = NULL,
                .state = file,
            };
            PB_FA_StorageAnimationMetaFile meta_file = {
                .meta.arg = animation_list,
                .meta.funcs.decode = decode_file_meta,
            };
            result = pb_decode(&istream, PB_FA_StorageAnimationMetaFile_fields, &meta_file);

            if (!result) {
                FURI_LOG_E(TAG, "Decode failed, reason: \'%s\'", istream.errmsg ? istream.errmsg : "unknown");
                animation_storage_list_free(*animation_list);
            }
            furi_assert(0);
        }
        storage_file_close(file);
        storage_file_free(file);
    }

    // add hard-coded animations
    for (int i = 0; i < COUNT_OF(StorageAnimationInternal); ++i) {
        StorageAnimationList_push_back(*animation_list, &StorageAnimationInternal[i]);
    }

    furi_record_close("storage");

    for M_EACH(item, *animation_list, StorageAnimationList_t) {
        FURI_LOG_I(TAG, "Manifest list(%s): \'%s\', butthurt %d/%d, level %d/%d, weight %d",
                (*item)->external ? "ext" : "int",
                *item, (*item)->meta.name,
                (*item)->meta.min_butthurt,
                (*item)->meta.max_butthurt,
                (*item)->meta.min_level,
                (*item)->meta.max_level,
                (*item)->meta.weight);
    }
}

void animation_storage_list_free(StorageAnimationList_t animation_list) {
    for M_EACH(item, animation_list, StorageAnimationList_t) {
        animation_storage_free_animation(*item);
    }
}

StorageAnimation* animation_storage_find_animation(const char* name) {
    // look through internal animations
    for (int i = 0; i < COUNT_OF(StorageAnimationInternal); ++i) {
        if (!strcmp(StorageAnimationInternal[i].meta.name, name)) {
            return &StorageAnimationInternal[i];
        }
    }

//    StorageAnimationList_t animation_list;
//    StorageAnimationList_init(animation_list);
//
//    for M_EACH(item, animation_list, StorageAnimationList_t) {
//        if (!strcmp((*item)->meta.name, name)) {
//            animation_storage_cache_animation(storage_animation);
//        }
//    }
//    StorageAnimationList_clear(animation_list);
//
//    furi_assert(iterator);
//
//    if (iterator->external) {
//        StorageAnimation* storage_animation = furi_alloc(sizeof(StorageAnimation));
//        string_t filename;
//        string_init_printf(filename, "%s%s", name, ".anm");
//        Storage* storage = furi_record_open("storage");
//        FS_Error sd_status = storage_sd_status(storage);
//        if(sd_status == FSE_OK) {
//            // read file and add StorageAnimationList
//        } else {
//            // do nothing
//        }
//
//        furi_record_close("storage");
//        return storage_animation;
//    } else {
//    }

    return NULL;
}

PB_FA_StorageAnimationMeta* animation_storage_get_meta(StorageAnimation* storage_animation) {
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





#if 0
void animation_storage_fill_animation_list(StorageAnimationList_t* animation_list) {
    furi_assert(sizeof(StorageAnimationList_t) == sizeof(void*));
    FileInfo fileinfo;
    bool result = false;
    furi_assert(!StorageAnimationList_size(*animation_list));

    Storage* storage = furi_record_open("storage");
    FS_Error sd_status = storage_sd_status(storage);
    if(sd_status == FSE_OK) {
        File* file = storage_file_alloc(storage);
        FS_Error error = storage_common_stat(storage, MANIFEST_FILE, &fileinfo);
        result = (error == FSE_OK) && fileinfo.size;
        if(result && storage_file_open(file, MANIFEST_FILE, FSAM_READ, FSOM_OPEN_EXISTING)) {
            pb_istream_t istream = {
#if DEBUG_PB == 1
                .bytes_left = sizeof(dbg_buffer),
#else
                .bytes_left = fileinfo.size,
#endif
                .callback = stream_file_read,
                .errmsg = NULL,
                .state = file,
            };
            PB_FA_StorageAnimationMetaFile meta_file = {
                .meta.arg = animation_list,
                .meta.funcs.decode = decode_file_meta,
            };
            result = pb_decode(&istream, PB_FA_StorageAnimationMetaFile_fields, &meta_file);

            if (!result) {
                FURI_LOG_E(TAG, "Decode failed, reason: \'%s\'", istream.errmsg ? istream.errmsg : "unknown");
                animation_storage_list_free(*animation_list);
            }
            furi_assert(0);
        }
        storage_file_close(file);
        storage_file_free(file);
    }

    // add hard-coded animations
    for (int i = 0; i < COUNT_OF(StorageAnimationInternal); ++i) {
        StorageAnimationList_push_back(*animation_list, &StorageAnimationInternal[i]);
    }

    furi_record_close("storage");

    for M_EACH(item, *animation_list, StorageAnimationList_t) {
        FURI_LOG_I(TAG, "Manifest list(%s): \'%s\', butthurt %d/%d, level %d/%d, weight %d",
                (*item)->external ? "ext" : "int",
                *item, (*item)->meta.name,
                (*item)->meta.min_butthurt,
                (*item)->meta.max_butthurt,
                (*item)->meta.min_level,
                (*item)->meta.max_level,
                (*item)->meta.weight);
    }
}


#endif

void animation_storage_cache_animation(StorageAnimation* storage_animation) {
    furi_assert(storage_animation);

    if (storage_animation->external) {
        if (!storage_animation->animation) {
            // TODO: read file decode Protobuf
        }
    }

    furi_assert(storage_animation->animation);
}





