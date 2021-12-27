#include "cmsis_os2.h"
#include "dolphin/animations/animation_manager.h"
#include "dolphin/dolphin.h"
#include "furi/check.h"
#include "furi/pubsub.h"
#include "furi/record.h"
#include "m-string.h"
#include "storage/filesystem-api-defines.h"
#include "views/bubble_animation_view.h"
#include "dolphin/dolphin_i.h"
#include "animation_storage.h"
#include "storage/storage.h"

#define TAG "ANM_MNGR"

typedef enum {
    AnimationManagerStateIdle,
    AnimationManagerStateBlocking,
} AnimationManagerState;

struct AnimationManager {
    bool sd_shown_error_db;
    bool sd_shown_error_card_bad;
    AnimationManagerState state;
    FuriPubSubSubscription* pubsub_storage;
    BubbleAnimationView* animation_view;
    osTimerId_t idle_animation_timer;
    StorageAnimation* current_animation;
    AnimationManagerInteractCallback interact_callback;
    AnimationManagerSetNewIdleAnimationCallback new_idle_callback;
    AnimationManagerSetNewIdleAnimationCallback check_blocking_callback;
    void* context;
    string_t freezed_animation_name;
};

static StorageAnimation* animation_manager_select_idle_animation(AnimationManager* animation_manager);
static void animation_manager_replace_current_animation(AnimationManager* animation_manager, StorageAnimation* storage_animation);

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

void animation_manager_interact(AnimationManager* animation_manager) {
    furi_assert(animation_manager);

    if(animation_manager->state == AnimationManagerStateBlocking) {
        animation_manager->state = AnimationManagerStateIdle;
        /* check if new blocking animation has to be displayed */
        animation_manager->check_blocking_callback(animation_manager->context);
    }
}

void animation_manager_check_blocking(AnimationManager* animation_manager) {
    furi_assert(animation_manager);

    if(animation_manager->state == AnimationManagerStateBlocking) {
        return;
    }

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
    }

    furi_record_close("storage");
}

// Switch to 'next' animation
// if no previously prepared 'next' animation - select new one
// OK
void animation_manager_start_new_idle_animation(AnimationManager* animation_manager) {
    furi_assert(animation_manager);

    if(animation_manager->state == AnimationManagerStateBlocking) {
        return;
    }
    if(string_size(animation_manager->freezed_animation_name)) {
        return;
    }

    TickType_t dbg_started = xTaskGetTickCount();
    StorageAnimation* new_animation = animation_manager_select_idle_animation(animation_manager);
    animation_manager_replace_current_animation(animation_manager, new_animation);
    FURI_LOG_I(TAG, "upload animation \'%s\' for %d ms", string_get_cstr(animation_storage_get_meta(new_animation)->name), xTaskGetTickCount() - dbg_started);
}

static void animation_manager_replace_current_animation(AnimationManager* animation_manager, StorageAnimation* storage_animation) {
    furi_assert(storage_animation);
    StorageAnimation* previous_animation = animation_manager->current_animation;

    const char* old_name = animation_manager->current_animation
        ? string_get_cstr(animation_storage_get_meta(animation_manager->current_animation)->name)
        : "NULL";
    FURI_LOG_I(TAG, "Remove \'%s\' animation", old_name);   // dbg_

    const BubbleAnimation* animation = animation_storage_get_bubble_animation(storage_animation);
    bubble_animation_view_set_animation(animation_manager->animation_view, animation);
    const char* new_name = string_get_cstr(animation_storage_get_meta(storage_animation)->name);
    FURI_LOG_I(TAG, "Start \'%s\' animation for %d sec", new_name, animation->duration);
    osTimerStart(animation_manager->idle_animation_timer, animation->duration * 1000);
    animation_manager->current_animation = storage_animation;

    if (previous_animation) {
        animation_storage_free_storage_animation(&previous_animation);
    }

    FURI_LOG_I(TAG, "free mem %d", xPortGetFreeHeapSize());
}

AnimationManager* animation_manager_alloc(void) {
    AnimationManager* animation_manager = furi_alloc(sizeof(AnimationManager));
    animation_manager->animation_view = bubble_animation_view_alloc();
    bubble_animation_view_set_interact_callback(animation_manager->animation_view, animation_manager_interact_callback, animation_manager);
    animation_manager->idle_animation_timer = osTimerNew(animation_manager_timer_callback, osTimerOnce, animation_manager, NULL);

    Storage* storage = furi_record_open("storage");
    animation_manager->pubsub_storage = furi_pubsub_subscribe(storage_get_pubsub(storage), animation_manager_check_blocking_callback, animation_manager);
    furi_record_close("storage");
    string_init(animation_manager->freezed_animation_name);

    animation_manager_start_new_idle_animation(animation_manager);

    return animation_manager;
}

void animation_manager_free(AnimationManager* animation_manager) {
    furi_assert(animation_manager);

    Storage* storage = furi_record_open("storage");
    furi_pubsub_unsubscribe(storage_get_pubsub(storage), animation_manager->pubsub_storage);
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

    Dolphin* dolphin = furi_record_open("dolphin");
    DolphinStats stats = dolphin_stats(dolphin);
    uint32_t whole_weight = 0;

    StorageAnimationList_it_t it;
    for(StorageAnimationList_it(it, animation_list); !StorageAnimationList_end_p(it); StorageAnimationList_next(it)) {
        StorageAnimation* storage_animation = *StorageAnimationList_ref(it);
        const StorageAnimationMeta* meta = animation_storage_get_meta(storage_animation);
        if ((stats.butthurt >= meta->min_butthurt)
            && (stats.butthurt <= meta->max_butthurt)
            && (stats.level >= meta->min_level)
            && (stats.level <= meta->max_level)) {
            whole_weight += meta->weight;
        } else {
            animation_storage_free_storage_animation(&storage_animation);
            StorageAnimationList_remove(animation_list, it);
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

// called inside Desktop thread -> no need to untie with message queue
void animation_manager_unload_and_stall_animation(AnimationManager* animation_manager) {
    furi_assert(animation_manager);
    furi_assert(animation_manager->current_animation);
    furi_assert(!string_size(animation_manager->freezed_animation_name));

    FURI_LOG_I(TAG, "unload and stall: started, heap: %d", xPortGetFreeHeapSize());
    osTimerStop(animation_manager->idle_animation_timer);

    StorageAnimationMeta* meta = animation_storage_get_meta(animation_manager->current_animation);
    /* copy str, not move, because it can be internal animation */
    string_set(animation_manager->freezed_animation_name, meta->name);
    FURI_LOG_I(TAG, "Unload & stall, freezed animation name \'%s\'", string_get_cstr(animation_manager->freezed_animation_name)); // dbg_

    bubble_animation_freeze(animation_manager->animation_view);
    animation_storage_free_storage_animation(&animation_manager->current_animation);
    FURI_LOG_I(TAG, "unload and stall: ended, heap: %d", xPortGetFreeHeapSize());
}

void animation_manager_load_and_continue_animation(AnimationManager* animation_manager) {
    furi_assert(animation_manager);
    furi_assert(!animation_manager->current_animation);
    furi_assert(string_size(animation_manager->freezed_animation_name));

    /* check if we missed some system notifications, and set current_animation */
    animation_manager_check_blocking(animation_manager);
    StorageAnimation* restore_animation = animation_manager->current_animation;
    FURI_LOG_I(TAG, "Load & Continue, freezed animation name \'%s\'", string_get_cstr(animation_manager->freezed_animation_name)); // dbg_

    if (!restore_animation) {
        restore_animation = animation_storage_find_animation(string_get_cstr(animation_manager->freezed_animation_name));
    }
    if (!restore_animation) {
        restore_animation = animation_manager_select_idle_animation(animation_manager);
    }
    furi_assert(restore_animation);
    FURI_LOG_I(TAG, "Load & Continue with \'%s\'", string_get_cstr(animation_storage_get_meta(restore_animation)->name)); // dbg_
    animation_manager_replace_current_animation(animation_manager, restore_animation);
    bubble_animation_unfreeze(animation_manager->animation_view);
    string_reset(animation_manager->freezed_animation_name);
}

//      TODO на завтра:
// * добавить анимацию мёртвой SD-карты и поставить правильные мета для 'tv'
// * добавить синхронизацию в лоадер для выгрузки-загрузки анимаций
// * проверить что если не получится загрузить полностью анимацию - выберется запасная
// * проверить что после unload вынуть SD-карту и всё норм будет работать


