#include "animation_manager.h"
#include "furi-hal-delay.h"
#include "portmacro.h"
#include "views/bubble_animation_view.h"
#include "animation_storage.h"

#include <cmsis_os2.h>
#include <dolphin/dolphin.h>
#include <furi/check.h>
#include <furi/pubsub.h>
#include <furi/record.h>
#include <m-string.h>
#include <power/power_service/power.h>
#include <stdint.h>
#include <storage/storage.h>
#include <dolphin/dolphin_i.h>
#include <storage/filesystem-api-defines.h>

#define TAG "ANM_MNGR"

typedef enum {
    AnimationManagerStateIdle,
    AnimationManagerStateBlocking,
    AnimationManagerStateFreezed,
} AnimationManagerState;

struct AnimationManager {
    bool sd_shown_error_db;
    bool sd_shown_error_card_bad;
    AnimationManagerState state;
    FuriPubSubSubscription* pubsub_subscription_storage;
    FuriPubSubSubscription* pubsub_subscription_dolphin;
    BubbleAnimationView* animation_view;
    osTimerId_t idle_animation_timer;
    StorageAnimation* current_animation;
    AnimationManagerInteractCallback interact_callback;
    AnimationManagerSetNewIdleAnimationCallback new_idle_callback;
    AnimationManagerSetNewIdleAnimationCallback check_blocking_callback;
    void* context;
    string_t freezed_animation_name;
    int32_t freezed_animation_time_left;
};

static StorageAnimation* animation_manager_select_idle_animation(AnimationManager* animation_manager);
static void animation_manager_replace_current_animation(AnimationManager* animation_manager, StorageAnimation* storage_animation);
static void animation_manager_start_new_idle(AnimationManager* animation_manager);
static bool animation_manager_check_blocking(AnimationManager* animation_manager);

void animation_manager_set_context(AnimationManager* animation_manager, void* context) {
    furi_assert(animation_manager);
    animation_manager->context = context;
}

void animation_manager_set_new_idle_callback(AnimationManager* animation_manager, AnimationManagerSetNewIdleAnimationCallback callback) {
    furi_assert(animation_manager);
    animation_manager->new_idle_callback = callback;
}

void animation_manager_set_check_callback(AnimationManager* animation_manager, AnimationManagerCheckBlockingCallback callback) {
    furi_assert(animation_manager);
    animation_manager->check_blocking_callback = callback;
}

void animation_manager_set_interact_callback(AnimationManager* animation_manager, AnimationManagerInteractCallback callback) {
    furi_assert(animation_manager);
    animation_manager->interact_callback = callback;
}

static void animation_manager_check_blocking_callback(const void* message, void* context) {
    furi_assert(context);
    AnimationManager* animation_manager = context;
    if (animation_manager->check_blocking_callback) {
        animation_manager->check_blocking_callback(animation_manager->context);
    }
}

static void animation_manager_timer_callback(void* context) {
    furi_assert(context);
    AnimationManager* animation_manager = context;
    if (animation_manager->new_idle_callback) {
        animation_manager->new_idle_callback(animation_manager->context);
    }
}

static void animation_manager_interact_callback(void* context) {
    furi_assert(context);
    AnimationManager* animation_manager = context;
    if (animation_manager->interact_callback) {
        animation_manager->interact_callback(animation_manager->context);
    }
}

/* reaction to animation_manager->interact_callback() */
void animation_manager_check_blocking_process(AnimationManager* animation_manager) {
    furi_assert(animation_manager);

    if(animation_manager->state == AnimationManagerStateIdle) {
        animation_manager_check_blocking(animation_manager);
    }
}

/* reaction to animation_manager->new_idle_callback() */
void animation_manager_new_idle_process(AnimationManager* animation_manager) {
    furi_assert(animation_manager);

    if(animation_manager->state == AnimationManagerStateIdle) {
        animation_manager_start_new_idle(animation_manager);
    }
}

/* reaction to animation_manager->check_blocking_callback() */
void animation_manager_interact_process(AnimationManager* animation_manager) {
    furi_assert(animation_manager);

    if(animation_manager->state == AnimationManagerStateBlocking) {
        animation_manager->state = AnimationManagerStateIdle;
        /* check if new blocking animation has to be displayed */
        bool blocking = false;
        blocking = animation_manager_check_blocking(animation_manager);
        if (!blocking) {
            animation_manager_start_new_idle(animation_manager);
        }
    }
}

static void animation_manager_start_new_idle(AnimationManager* animation_manager) {
    furi_assert(animation_manager);

    StorageAnimation* new_animation = animation_manager_select_idle_animation(animation_manager);
    animation_manager_replace_current_animation(animation_manager, new_animation);
    const BubbleAnimation* bubble_animation = animation_storage_get_bubble_animation(animation_manager->current_animation);
    osTimerStart(animation_manager->idle_animation_timer, bubble_animation->duration * 1000);
}

static bool animation_manager_check_blocking(AnimationManager* animation_manager) {
    furi_assert(animation_manager);

    StorageAnimation* blocking_animation = NULL;
    Storage* storage = furi_record_open("storage");
    FS_Error sd_status = storage_sd_status(storage);

    if(sd_status == FSE_INTERNAL) {
        blocking_animation = animation_storage_find_animation("bad_sd_card");
        animation_manager->sd_shown_error_card_bad = true;
        animation_manager->sd_shown_error_db = false;
    } else if(sd_status == FSE_NOT_READY) {
        animation_manager->sd_shown_error_card_bad = false;
        animation_manager->sd_shown_error_db = false;
    } else if(sd_status == FSE_OK) {
        bool db_exists = storage_common_stat(storage, "/ext/manifest.txt", NULL) == FSE_OK;
        if(db_exists && !animation_manager->sd_shown_error_db) {
            blocking_animation = animation_storage_find_animation("no_db");
            animation_manager->sd_shown_error_db = true;
        }
    }

    Dolphin* dolphin = furi_record_open("dolphin");
    DolphinStats stats = dolphin_stats(dolphin);
    furi_record_close("dolphin");
    if(!blocking_animation && stats.level_up_is_pending) {
        blocking_animation = animation_storage_find_animation("levelup_is_pending");
    }

    if(blocking_animation) {
        osTimerStop(animation_manager->idle_animation_timer);
        animation_manager->state = AnimationManagerStateBlocking;
        animation_manager_replace_current_animation(animation_manager, blocking_animation);
        /* no starting timer because its blocking animation */
    }

    furi_record_close("storage");

    return !!blocking_animation;
}

static void animation_manager_replace_current_animation(AnimationManager* animation_manager, StorageAnimation* storage_animation) {
    furi_assert(storage_animation);
    StorageAnimation* previous_animation = animation_manager->current_animation;

    const BubbleAnimation* animation = animation_storage_get_bubble_animation(storage_animation);
    bubble_animation_view_set_animation(animation_manager->animation_view, animation);
    const char* new_name = string_get_cstr(animation_storage_get_meta(storage_animation)->name);
    FURI_LOG_I(TAG, "Select \'%s\' animation", new_name);
    animation_manager->current_animation = storage_animation;

    if (previous_animation) {
        animation_storage_free_storage_animation(&previous_animation);
    }
}

AnimationManager* animation_manager_alloc(void) {
    AnimationManager* animation_manager = furi_alloc(sizeof(AnimationManager));
    animation_manager->animation_view = bubble_animation_view_alloc();
    string_init(animation_manager->freezed_animation_name);

    animation_manager->idle_animation_timer = osTimerNew(animation_manager_timer_callback, osTimerOnce, animation_manager, NULL);
    bubble_animation_view_set_interact_callback(animation_manager->animation_view, animation_manager_interact_callback, animation_manager);

    Storage* storage = furi_record_open("storage");
    animation_manager->pubsub_subscription_storage = furi_pubsub_subscribe(
            storage_get_pubsub(storage),
            animation_manager_check_blocking_callback,
            animation_manager);
    furi_record_close("storage");

    Dolphin* dolphin = furi_record_open("dolphin");
    animation_manager->pubsub_subscription_dolphin = furi_pubsub_subscribe(
            dolphin_get_pubsub(dolphin),
            animation_manager_check_blocking_callback,
            animation_manager);
    furi_record_close("dolphin");

    animation_manager->state = AnimationManagerStateIdle;
    animation_manager_new_idle_process(animation_manager);

    return animation_manager;
}

void animation_manager_free(AnimationManager* animation_manager) {
    furi_assert(animation_manager);

    Dolphin* dolphin = furi_record_open("dolphin");
    furi_pubsub_unsubscribe(dolphin_get_pubsub(dolphin), animation_manager->pubsub_subscription_dolphin);
    furi_record_close("dolphin");

    Storage* storage = furi_record_open("storage");
    furi_pubsub_unsubscribe(storage_get_pubsub(storage), animation_manager->pubsub_subscription_storage);
    furi_record_close("storage");

    string_clear(animation_manager->freezed_animation_name);
    bubble_animation_view_free(animation_manager->animation_view);
    osTimerDelete(animation_manager->idle_animation_timer);
}

View* animation_manager_get_animation_view(AnimationManager* animation_manager) {
    furi_assert(animation_manager);

    return bubble_animation_get_view(animation_manager->animation_view);
}

static StorageAnimation* animation_manager_select_idle_animation(AnimationManager* animation_manager) {
    StorageAnimationList_t animation_list;
    StorageAnimationList_init(animation_list);
    animation_storage_fill_animation_list(&animation_list);

    Power* power = furi_record_open("power");
    PowerInfo info;
    power_get_info(power, &info);
    bool battery_is_well = power_is_battery_well(&info);
    furi_record_close("power");

    Storage* storage = furi_record_open("storage");
    FS_Error sd_status = storage_sd_status(storage);
    furi_record_close("storage");

    Dolphin* dolphin = furi_record_open("dolphin");
    DolphinStats stats = dolphin_stats(dolphin);
    uint32_t whole_weight = 0;

    StorageAnimationList_it_t it;
    for(StorageAnimationList_it(it, animation_list); !StorageAnimationList_end_p(it); StorageAnimationList_next(it)) {
        StorageAnimation* storage_animation = *StorageAnimationList_ref(it);
        const StorageAnimationMeta* meta = animation_storage_get_meta(storage_animation);
        bool skip_animation = false;
        if (battery_is_well && !string_cmp_str(meta->name, BAD_BATTERY_ANIMATION_NAME)) {
            skip_animation = true;
        } else if ((sd_status != FSE_NOT_READY) && !string_cmp_str(meta->name, NO_SD_ANIMATION_NAME)) {
            skip_animation = true;
        } else if ((stats.butthurt < meta->min_butthurt) || (stats.butthurt > meta->max_butthurt)) {
            skip_animation = true;
        } else if ((stats.level < meta->min_level) || (stats.level > meta->max_level)) {
            skip_animation = true;
        }

        if (skip_animation) {
            FURI_LOG_D(TAG, "skip \'%s\' animation", string_get_cstr(meta->name));
            animation_storage_free_storage_animation(&storage_animation);
            StorageAnimationList_remove(animation_list, it);
        } else {
            whole_weight += meta->weight;
        }
    }

    uint32_t lucky_number = random() % whole_weight;
    uint32_t weight = 0;

    StorageAnimation* selected = NULL;
    for M_EACH(item, animation_list, StorageAnimationList_t) {
        if(lucky_number < weight) {
            break;
        }
        weight += animation_storage_get_meta(*item)->weight;
        selected = *item;
    }

    for M_EACH(item, animation_list, StorageAnimationList_t) {
        if (*item != selected) {
            animation_storage_free_storage_animation(item);
        }
    }

    StorageAnimationList_clear(animation_list);
    furi_record_close("dolphin");

    /* cache animation, if failed - choose reliable animation */
    if (!animation_storage_get_bubble_animation(selected)) {
        animation_storage_free_storage_animation(&selected);
        selected = animation_storage_find_animation(HARDCODED_ANIMATION_NAME);
    }

    furi_assert(selected);
    return selected;
}

void animation_manager_unload_and_stall_animation(AnimationManager* animation_manager) {
    furi_assert(animation_manager);
    furi_assert(animation_manager->current_animation);
    furi_assert(!string_size(animation_manager->freezed_animation_name));
    furi_assert(animation_manager->state != AnimationManagerStateFreezed);
    animation_manager->state = AnimationManagerStateFreezed;

    animation_manager->freezed_animation_time_left = xTimerGetExpiryTime(animation_manager->idle_animation_timer) - xTaskGetTickCount();
    if (animation_manager->freezed_animation_time_left < 0) {
        animation_manager->freezed_animation_time_left = 0;
    }
    osTimerStop(animation_manager->idle_animation_timer);

    StorageAnimationMeta* meta = animation_storage_get_meta(animation_manager->current_animation);
    /* copy str, not move, because it can be internal animation */
    string_set(animation_manager->freezed_animation_name, meta->name);
    FURI_LOG_D(TAG, "Unload & Stall, freezed animation name \'%s\'", string_get_cstr(animation_manager->freezed_animation_name));

    bubble_animation_freeze(animation_manager->animation_view);
    animation_storage_free_storage_animation(&animation_manager->current_animation);
}

void animation_manager_load_and_continue_animation(AnimationManager* animation_manager) {
    furi_assert(animation_manager);
    furi_assert(!animation_manager->current_animation);
    furi_assert(string_size(animation_manager->freezed_animation_name));
    furi_assert(animation_manager->state == AnimationManagerStateFreezed);
    animation_manager->state = AnimationManagerStateIdle;

    /* check if we missed some system notifications, and set current_animation */
    animation_manager_check_blocking(animation_manager);

    /* if no blocking - try restore last one idle */
    if (!animation_manager->current_animation) {
        StorageAnimation* restore_animation = animation_storage_find_animation(string_get_cstr(animation_manager->freezed_animation_name));
        if (restore_animation) {
            animation_manager_replace_current_animation(animation_manager, restore_animation);
            if (animation_manager->freezed_animation_time_left) {
                osTimerStart(animation_manager->idle_animation_timer, animation_manager->freezed_animation_time_left);
            }
        }
    }

    /* if can't restore previous idle animation - select new */
    if (!animation_manager->current_animation) {
        animation_manager_start_new_idle(animation_manager);
    }
    FURI_LOG_D(TAG, "Load & Continue with \'%s\'", string_get_cstr(animation_storage_get_meta(animation_manager->current_animation)->name));

    bubble_animation_unfreeze(animation_manager->animation_view);
    string_reset(animation_manager->freezed_animation_name);
    furi_assert(animation_manager->current_animation);
}

