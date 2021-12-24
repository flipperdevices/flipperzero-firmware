#include "dolphin/animations/animation_manager.h"
#include "file-worker.h"
#include "flipper_file.h"
#include "furi/record.h"
#include "animation_storage.h"
#include "gui/canvas.h"
#include "m-string.h"
#include "pb.h"
#include "pb_decode.h"
#include "storage/filesystem-api-defines.h"
#include "storage/storage.h"
#include "animation_storage_i.h"
#include "animation_storage.pb.h"
#include <stdint.h>

#define ANIMATION_META "meta"
#define ANIMATION_DIR "/ext/assets/file_animations"
#define MANIFEST_FILE ANIMATION_DIR "/manifest"
#define TAG "ANM_STRG"
#define DEBUG_PB 0

static void animation_storage_free_bubble_animation(BubbleAnimation* animation);
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
    furi_assert(!StorageAnimationList_size(*animation_list));

    Storage* storage = furi_record_open("storage");
    FlipperFile* file = flipper_file_alloc(storage);
    /* Forbid skipping fields */
    flipper_file_set_strict_mode(file, true);
    string_t header;
    string_init(header);

    do {
        uint32_t u32value;
        bool read_completed = false;
        StorageAnimation* storage_animation = NULL;

        if(FSE_OK != storage_sd_status(storage)) break;
        if(!flipper_file_open_existing(file, MANIFEST_FILE)) break;
        if(!flipper_file_read_header(file, header, &u32value)) break;
        if(string_cmp_str(header, "Flipper Animation Manifest")) break;
        do {
            read_completed = false;
            storage_animation = furi_alloc(sizeof(StorageAnimation));
            storage_animation->external = true;
            storage_animation->animation = NULL;

            if(!flipper_file_read_string(file, "Name", storage_animation->meta.name)) break;
            if(!flipper_file_read_uint32(file, "Min butthurt", &u32value, 1)) break;
            storage_animation->meta.min_butthurt = u32value;
            if(!flipper_file_read_uint32(file, "Max butthurt", &u32value, 1)) break;
            storage_animation->meta.max_butthurt = u32value;
            if(!flipper_file_read_uint32(file, "Min level", &u32value, 1)) break;
            storage_animation->meta.min_level = u32value;
            if(!flipper_file_read_uint32(file, "Max level", &u32value, 1)) break;
            storage_animation->meta.max_level = u32value;
            if(!flipper_file_read_uint32(file, "Weight", &u32value, 1)) break;
            storage_animation->meta.weight = u32value;

            read_completed = true;
            StorageAnimationList_push_back(*animation_list, storage_animation);
        } while(1);

        if (!read_completed) {
            animation_storage_free_animation(storage_animation);
        }
    } while(0);

    string_clear(header);
    flipper_file_close(file);
    flipper_file_free(file);











    // add hard-coded animations
    for (int i = 0; i < COUNT_OF(StorageAnimationInternal); ++i) {
        StorageAnimationList_push_back(*animation_list, &StorageAnimationInternal[i]);
    }

    furi_record_close("storage");





    for M_EACH(item, *animation_list, StorageAnimationList_t) {
        FURI_LOG_I(TAG, "Manifest list(%s): \'%s\', butthurt %d/%d, level %d/%d, weight %d",
                (*item)->external ? "ext" : "int",
                (*item)->meta.name,
                (*item)->meta.min_butthurt,
                (*item)->meta.max_butthurt,
                (*item)->meta.min_level,
                (*item)->meta.max_level,
                (*item)->meta.weight);
    }
}


void animation_storage_fill_animation_list2(StorageAnimationList_t* animation_list) {
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
                string_get_cstr((*item)->meta.name),
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
        if (!string_cmp_str(StorageAnimationInternal[i].meta.name, name)) {
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

StorageAnimationMeta* animation_storage_get_meta(StorageAnimation* storage_animation) {
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
        animation_storage_free_bubble_animation((BubbleAnimation*) storage_animation->animation);
        string_clear(storage_animation->meta.name);
        free(storage_animation);
    }
}

static bool animation_storage_cast_align(string_t align_str, Align* align) {
    if (!string_cmp_str(align_str, "Bottom")) {
        *align = AlignBottom;
    } else if (!string_cmp_str(align_str, "Top")) {
        *align = AlignTop;
    } else if (!string_cmp_str(align_str, "Left")) {
        *align = AlignLeft;
    } else if (!string_cmp_str(align_str, "Right")) {
        *align = AlignRight;
    } else if (!string_cmp_str(align_str, "Center")) {
        *align = AlignCenter;
    } else {
        return false;
    }

    return true;
}

static bool animation_storage_load_bubble_animation(const char* name, BubbleAnimation* animation) {
    furi_assert(animation);

    uint32_t height = 0;
    uint32_t width = 0;
    Storage* storage = furi_record_open("storage");
    FlipperFile* ff = flipper_file_alloc(storage);
    /* Forbid skipping fields */
    flipper_file_set_strict_mode(ff, true);
    string_t str;
    string_init(str);
    animation->frame_bubbles = NULL;

    bool success = false;
    do {
        uint32_t u32value;

        if(FSE_OK != storage_sd_status(storage)) break;

        string_printf(str, ANIMATION_DIR "/%s/" ANIMATION_META, name);
        if(!flipper_file_open_existing(ff, string_get_cstr(str))) break;
        if(!flipper_file_read_header(ff, str, &u32value)) break;
        if(string_cmp_str(str, "Flipper Animation")) break;

        if(!flipper_file_read_uint32(ff, "Height", &height, 1)) break;
        if(!flipper_file_read_uint32(ff, "Width", &width, 1)) break;


        if(!flipper_file_read_uint32(ff, "Passive frames", &u32value, 1)) break;
        animation->passive_frames = u32value;
        if(!flipper_file_read_uint32(ff, "Active frames", &u32value, 1)) break;
        animation->active_frames = u32value;

        uint8_t frames = animation->passive_frames + animation->active_frames;
        uint32_t* u32array = furi_alloc(sizeof(uint32_t) * frames);
        if(!flipper_file_read_uint32(ff, "Frames order", &u32value, frames)) break;
        animation->icons = furi_alloc(sizeof(const Icon*) * frames);

/* upload 1 frame 1 time, but keep many links to 1 blob */
        for (int i = 0; i < frames; ++i) {
            if (animation->icons[i])
                continue;
            animation->icons[i] = furi_alloc(width * height / 8 + 1);

            string_t filename;
            string_init_printf(filename, ANIMATION_DIR "/%s/frame_%d.bm", name, u32array[i]);
            /* read file here */
            File file;
            if (!storage_file_open(&file, string_get_cstr(filename), FSAM_READ, FSOM_OPEN_EXISTING)) goto exit;
            storage_file_read(&file, animation->icons[i]->/* no access to field */

            for (int j = i + 1; j < frames; ++j) {
                if (u32array[i] == u32array[j]) {
                    animation->icons[j] = animation->icons[i];
                }
            }
        }




        if(!flipper_file_read_uint32(ff, "Passive frames", &u32value, 1)) break;
        animation->passive_frames = u32value;





        if(!flipper_file_read_uint32(ff, "Active cycles", &u32value, 1)) break;
        animation->active_cycles = u32value;
        if(!flipper_file_read_uint32(ff, "Frame rate", &u32value, 1)) break;
        animation->frame_rate = u32value;
        if(!flipper_file_read_uint32(ff, "Duration", &u32value, 1)) break;
        animation->duration = u32value;
        if(!flipper_file_read_uint32(ff, "Active cooldown", &u32value, 1)) break;
        animation->active_cooldown = u32value;
        if(!flipper_file_read_uint32(ff, "Bubble slots", &u32value, 1)) break;
        if (u32value > 20) break;
        animation->frame_bubbles_count = u32value;
        animation->frame_bubbles = furi_alloc(sizeof(FrameBubble*) * animation->frame_bubbles_count);

        FrameBubble* bubble = animation->frame_bubbles[0];
        int8_t bubble_cnt = -1;
        uint32_t current_slot = -1;
        for ( ; bubble_cnt < animation->frame_bubbles_count; ) {
            if(!flipper_file_read_uint32(ff, "Slot", &current_slot, 1)) break;
            if(bubble_cnt == current_slot) {
                bubble->next_bubble = furi_alloc(sizeof(FrameBubble*));
                bubble = bubble->next_bubble;
            } else if (current_slot == bubble_cnt + 1) {
                ++bubble_cnt;
                bubble = animation->frame_bubbles[bubble_cnt];
            } else {
                break;
            }

            if(!flipper_file_read_uint32(ff, "X", &u32value, 1)) break;
            bubble->bubble.x = u32value;
            if(!flipper_file_read_uint32(ff, "Y", &u32value, 1)) break;
            bubble->bubble.y = u32value;

            if(!flipper_file_read_string(ff, "Text", str)) break;
            if(string_size(str) > 100) break;
            bubble->bubble.str = furi_alloc(string_size(str) + 1);
            strcpy((char*) bubble->bubble.str, string_get_cstr(str));

            if(!flipper_file_read_string(ff, "AlignH", str)) break;
            if(!animation_storage_cast_align(str, &bubble->bubble.horizontal)) break;
            if(!flipper_file_read_string(ff, "AlignV", str)) break;
            if(!animation_storage_cast_align(str, &bubble->bubble.vertical)) break;

            if(!flipper_file_read_uint32(ff, "StartFrame", &u32value, 1)) break;
            bubble->starts_at_frame = u32value;
            if(!flipper_file_read_uint32(ff, "EndFrame", &u32value, 1)) break;
            bubble->ends_at_frame = u32value;
        }

        success = bubble_cnt == animation->frame_bubbles_count;
    } while(0);

    string_clear(str);
    flipper_file_close(ff);
    flipper_file_free(ff);

exit:
    if (!success) {
        animation_storage_free_bubble_animation(animation);
    }

    return success;
}

static void animation_storage_free_bubble_animation(BubbleAnimation* animation) {
    for (int i = 0; i < animation->frame_bubbles_count; ) {
        FrameBubble* bubble = animation->frame_bubbles[i];

        if (bubble == NULL)
            break;

        while(bubble->next_bubble != NULL) {
            bubble = animation->frame_bubbles[i]->next_bubble;
        }

        if (bubble->bubble.str) {
            free((void*) bubble->bubble.str);
        }
        if (bubble == animation->frame_bubbles[i]) {
            ++i;
        }
        free(bubble);
    }
    free(animation->frame_bubbles);

    for(int i = 0; i < (animation->passive_frames + animation->active_frames); ++i) {
        if (animation->icons[i])
            break;
        free((void*)animation->icons[i]);
    }
    free((void*)animation->icons);
    free(animation);
}

//            string_t meta_animation_path;
//            string_init_printf(meta_animation_path,
//                    ANIMATION_DIR "/%s",
//                    string_get_cstr(storage_animation->meta.name));
//            string_clear(meta_animation_path);
void animation_storage_cache_animation(StorageAnimation* storage_animation) {
    furi_assert(storage_animation);

    if (storage_animation->external) {
        if (!storage_animation->animation) {
            animation_storage_load_bubble_animation(string_get_cstr(storage_animation->meta.name),
                    (BubbleAnimation*) storage_animation->animation);
        }
    }

    furi_assert(storage_animation->animation);
}


