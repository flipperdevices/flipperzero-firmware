#include "dolphin/animations/animation_manager.h"
#include "file-worker.h"
#include "flipper_file.h"
#include "furi/common_defines.h"
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
#include <gui/icon_i.h>

#define ANIMATION_META "meta"
#define ANIMATION_DIR "/ext/dolphin"
#define MANIFEST_FILE ANIMATION_DIR "/manifest"
#define TAG "ANM_STRG"
#define DEBUG_PB 0

static void animation_storage_free_bubbles(BubbleAnimation* animation);
static void animation_storage_free_frames(BubbleAnimation* animation);
void animation_storage_list_free(StorageAnimationList_t animation_list);


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
    BubbleAnimation* animation = (BubbleAnimation*) storage_animation->animation;

    if (storage_animation->external && animation) {
        animation_storage_free_bubbles(animation);
        animation_storage_free_frames(animation);
        free(animation);

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

static void animation_storage_free_frames(BubbleAnimation* animation) {
    furi_assert(animation);
    furi_assert(animation->icons);

    const Icon** icons = animation->icons;
    uint16_t frames = animation->active_frames + animation->passive_frames;
    furi_assert(frames > 0);

    for (int i = 0; i < frames; ++i) {
        if(!icons[i]) continue;

        const Icon* icon = icons[i];
        free((void*) icon);
        for (int j = i; j < frames; ++j) {
            if(icons[j] == icon) {
                icons[j] = NULL;
            }
        }
    }

    free(animation->icons);
    animation->icons = NULL;
}

static Icon* animation_storage_alloc_icon(size_t frame_size) {
    Icon* icon = furi_alloc(sizeof(Icon));
    icon->frames = furi_alloc(sizeof(const uint8_t *));
    icon->frames[0] = furi_alloc(frame_size);
    return icon;
}

static void animation_storage_free_icon(Icon* icon) {
    free((void*) icon->frames[0]);
    free(icon->frames);
    free(icon);
}

bool animation_storage_load_frames(Storage* storage,
                                    const char* name,
                                    BubbleAnimation* animation,
                                    uint32_t* frame_order,
                                    uint32_t width,
                                    uint32_t height) {
    furi_assert(!animation->icons);
    uint16_t frame_order_size = animation->passive_frames + animation->active_frames;

    bool frames_ok = false;
    animation->icons = furi_alloc(sizeof(const Icon*) * frame_order_size);
    File* file = storage_file_alloc(storage);
    FileInfo file_info;
    string_t filename;
    string_init(filename);

    for (int i = 0; i < frame_order_size; ++i) {
        if (animation->icons[i])
            continue;

        frames_ok = false;
        string_printf(filename, ANIMATION_DIR "/%s/frame_%d.bm", name, frame_order[i]);
        size_t filesize = ROUND_UP_TO(width, 8) * height;

        if (storage_common_stat(storage, string_get_cstr(filename), &file_info) != FSE_OK) break;
        if (file_info.size != filesize) {
            FURI_LOG_E(TAG, "Filesize %d, expected: %d (width %d, height %d)",
                            file_info.size, filesize, width, height);
            break;
        }
        if (!storage_file_open(file, string_get_cstr(filename), FSAM_READ, FSOM_OPEN_EXISTING)) {
            FURI_LOG_E(TAG, "Can't open file \'%s\'", string_get_cstr(filename));
            break;
        }

        Icon* icon = animation_storage_alloc_icon(filesize);
        if (storage_file_read(file, (void*) icon->frames[0], filesize) != filesize) {
            FURI_LOG_E(TAG, "Read failed: \'%s\'", string_get_cstr(filename));
            animation_storage_free_icon(icon);
            break;
        }
        icon->frame_count = 1;
        icon->frame_rate = 0;
        icon->height = height;
        icon->width = width;

        /* Claim 1 allocation for 1 files blob and several links to it */
        for (int j = i; j < frame_order_size; ++j) {
            if (frame_order[i] == frame_order[j]) {
                animation->icons[j] = icon;
            }
        }

        frames_ok = true;
        FURI_LOG_D(TAG, "Load \'%s\' OK", string_get_cstr(filename));
    }

    if (!frames_ok) {
        FURI_LOG_E(TAG, "Load \'%s\' failed", string_get_cstr(filename));
        animation_storage_free_frames(animation);
        animation->icons = NULL;
    } else {
        FURI_LOG_D(TAG, "All %d frames loaded", frame_order_size);
        // dbg_
        for (int i = 0; i < frame_order_size; ++i) {
            furi_check(animation->icons[i]);
            furi_check(animation->icons[i]->frames[0]);
        }
    }

    storage_file_free(file);
    string_clear(filename);

    return frames_ok;
}

static bool animation_storage_load_bubbles(BubbleAnimation* animation, FlipperFile* ff) {
    uint32_t u32value;
    string_t str;
    string_init(str);
    bool success = false;
    furi_assert(!animation->frame_bubbles);

    do {
        if(!flipper_file_read_uint32(ff, "Bubble slots", &u32value, 1)) break;
        if (u32value > 20) break;
        animation->frame_bubbles_count = u32value;
        animation->frame_bubbles = furi_alloc(sizeof(FrameBubble*) * animation->frame_bubbles_count);

        uint32_t current_slot = 0;
        for (int i = 0; i < animation->frame_bubbles_count; ++i) {
            animation->frame_bubbles[i] = furi_alloc(sizeof(FrameBubble));
        }
//            } else if (current_slot == -1) {
//                bubble_index = 0;
//                bubble = animation->frame_bubbles[bubble_index];

        FrameBubble* bubble = animation->frame_bubbles[0];
        int8_t index = -1;
        for ( ; index < animation->frame_bubbles_count; ) {
            if(!flipper_file_read_uint32(ff, "Slot", &current_slot, 1)) break;
            if((current_slot != 0) && (index == -1)) break;

            if(current_slot == index) {
                bubble->next_bubble = furi_alloc(sizeof(FrameBubble));
                bubble = bubble->next_bubble;
            } else if (current_slot == index + 1) {
                ++index;
                bubble = animation->frame_bubbles[index];
            } else {
                /* slots have to start from 0, be ascending sorted, and
                 * have exact number of slots as specified in "Bubble slots" */
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
        success = index == animation->frame_bubbles_count;
    } while(0);

    if (!success) {
        if (animation->frame_bubbles) {
            animation_storage_free_bubbles(animation);
        }
    }

    return success;
}

static bool animation_storage_load_animation(const char* name, BubbleAnimation** animation_p) {
    furi_assert(animation_p);
    furi_assert(!*animation_p);
    *animation_p = furi_alloc(sizeof(BubbleAnimation));
    BubbleAnimation* animation = *animation_p;

    uint32_t height = 0;
    uint32_t width = 0;
    uint32_t* u32array = NULL;
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

        if(!flipper_file_read_uint32(ff, "Width", &width, 1)) break;
        if(!flipper_file_read_uint32(ff, "Height", &height, 1)) break;

        if(!flipper_file_read_uint32(ff, "Passive frames", &u32value, 1)) break;
        animation->passive_frames = u32value;
        if(!flipper_file_read_uint32(ff, "Active frames", &u32value, 1)) break;
        animation->active_frames = u32value;

        uint8_t frames = animation->passive_frames + animation->active_frames;
        u32array = furi_alloc(sizeof(uint32_t) * frames);
        if(!flipper_file_read_uint32(ff, "Frames order", u32array, frames)) break;

        /* passive and active frames must be loaded up to this point */
        if (!animation_storage_load_frames(storage, name, animation, u32array, width, height)) break;

        if(!flipper_file_read_uint32(ff, "Active cycles", &u32value, 1)) break;
        animation->active_cycles = u32value;
        if(!flipper_file_read_uint32(ff, "Frame rate", &u32value, 1)) break;
        animation->frame_rate = u32value;
        if(!flipper_file_read_uint32(ff, "Duration", &u32value, 1)) break;
        animation->duration = u32value;
        if(!flipper_file_read_uint32(ff, "Active cooldown", &u32value, 1)) break;
        animation->active_cooldown = u32value;

        if (!animation_storage_load_bubbles(animation, ff)) break;
    } while(0);

    string_clear(str);
    flipper_file_close(ff);
    flipper_file_free(ff);

    if (!success) {
        if (u32array) {
            free(u32array);
        }
    }

    return success;
}

static void animation_storage_free_bubbles(BubbleAnimation* animation) {
    if (!animation->frame_bubbles)
        return;

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
}

void animation_storage_cache_animation(StorageAnimation* storage_animation) {
    furi_assert(storage_animation);

    if (storage_animation->external) {
        if (!storage_animation->animation) {
            animation_storage_load_animation(string_get_cstr(storage_animation->meta.name),
                    (BubbleAnimation**) &storage_animation->animation);
        }
    }

    furi_assert(storage_animation->animation);
}

