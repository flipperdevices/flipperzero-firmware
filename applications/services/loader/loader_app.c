#include "loader.h"
#include "loader_i.h"
#include "loader_menu.h"
#include <applications.h>

#define TAG "Loader"

// api

LoaderStatus loader_start(Loader* instance, const char* name, const char* args) {
    LoaderMessage message;
    LoaderMessageStartResult result;

    message.type = LoaderMessageTypeStartByName;
    message.start.name = name;
    message.start.args = args;
    message.api_lock = api_lock_alloc_locked();
    message.start_result = &result;
    furi_message_queue_put(instance->queue, &message, FuriWaitForever);
    api_lock_wait_unlock_and_free(message.api_lock);
    return result.status_value;
}

bool loader_lock(Loader* instance) {
    UNUSED(instance);
    furi_crash("Not implemented");
    return false;
}

void loader_unlock(Loader* instance) {
    UNUSED(instance);
    furi_crash("Not implemented");
}

bool loader_is_locked(Loader* instance) {
    LoaderMessage message;
    LoaderMessageStartIsLockedResult result;
    message.type = LoaderMessageTypeIsLocked;
    message.api_lock = api_lock_alloc_locked();
    message.is_locked_result = &result;
    furi_message_queue_put(instance->queue, &message, FuriWaitForever);
    api_lock_wait_unlock_and_free(message.api_lock);
    return result.bool_value;
}

void loader_show_menu(Loader* instance) {
    LoaderMessage message;
    message.type = LoaderMessageTypeShowMenu;
    furi_message_queue_put(instance->queue, &message, FuriWaitForever);
}

FuriPubSub* loader_get_pubsub(Loader* instance) {
    // it's safe to return pubsub without locking
    // because it's never freed and loader is never exited
    return instance->pubsub;
}

static const FlipperApplication* loader_get_application_by_appid(const char* appid) {
    const FlipperApplication* app = NULL;
    for(size_t i = 0; i < FLIPPER_APPS_COUNT; i++) {
        if(strcmp(appid, FLIPPER_APPS[i].appid) == 0) {
            app = &FLIPPER_APPS[i];
            break;
        }
    }

    if(!app) {
        for(size_t i = 0; i < FLIPPER_SETTINGS_APPS_COUNT; i++) {
            if(strcmp(appid, FLIPPER_SETTINGS_APPS[i].appid) == 0) {
                app = &FLIPPER_SETTINGS_APPS[i];
                break;
            }
        }
    }

    return app;
}

static void loader_menu_closed_callback(void* context) {
    Loader* instance = context;
    LoaderMessage message;
    message.type = LoaderMessageTypeMenuClosed;
    furi_message_queue_put(instance->queue, &message, FuriWaitForever);
}

static void loader_menu_click_callback(const char* name, void* context) {
    Loader* instance = context;
    loader_start(instance, name, NULL);
}

// static void loader_app_closed_callback(void* context) {
//     Loader* instance = context;
//     LoaderMessage message;
//     message.type = LoaderMessageTypeAppClosed;
//     furi_message_queue_put(instance->queue, &message, FuriWaitForever);
// }

// implementation

static Loader* loader_alloc() {
    Loader* instance = malloc(sizeof(Loader));
    instance->pubsub = furi_pubsub_alloc();
    instance->queue = furi_message_queue_alloc(1, sizeof(LoaderMessage));
    instance->loader_menu = NULL;
    return instance;
}

static LoaderStatus loader_do_start_by_name(Loader* instance, const char* name, const char* args) {
    UNUSED(instance);
    UNUSED(name);
    UNUSED(args);
    FURI_LOG_I(TAG, "Starting %s with args %s", name, args ? args : "");
    return LoaderStatusErrorInternal;
}

static void loader_do_menu_show(Loader* instance) {
    FURI_LOG_I(TAG, "Trying to show menu");
    if(!instance->loader_menu) {
        FURI_LOG_I(TAG, "Showing menu");
        instance->loader_menu = loader_menu_alloc();
        loader_menu_set_closed_callback(
            instance->loader_menu, loader_menu_closed_callback, instance);
        loader_menu_set_click_callback(
            instance->loader_menu, loader_menu_click_callback, instance);
        loader_menu_start(instance->loader_menu);
    }
}

static void loader_do_menu_closed(Loader* instance) {
    FURI_LOG_I(TAG, "Trying to close menu");
    if(instance->loader_menu) {
        FURI_LOG_I(TAG, "Closing menu");
        loader_menu_stop(instance->loader_menu);
        loader_menu_free(instance->loader_menu);
        instance->loader_menu = NULL;
    }
}

static bool loader_do_is_locked(Loader* instance) {
    UNUSED(instance);
    FURI_LOG_I(TAG, "Checking if locked");
    return false;
}

static void loader_do_app_closed(Loader* instance) {
    FURI_LOG_I(TAG, "App closed");
    UNUSED(instance);
}

// app

int32_t loader_srv(void* p) {
    UNUSED(p);
    Loader* instance = loader_alloc();
    furi_record_create(RECORD_LOADER, instance);

    LoaderMessage message;
    while(true) {
        if(furi_message_queue_get(instance->queue, &message, FuriWaitForever) == FuriStatusOk) {
            switch(message.type) {
            case LoaderMessageTypeStartByName:
                message.start_result->status_value =
                    loader_do_start_by_name(instance, message.start.name, message.start.args);
                api_lock_unlock(message.api_lock);
                break;
            case LoaderMessageTypeShowMenu:
                loader_do_menu_show(instance);
                break;
            case LoaderMessageTypeMenuClosed:
                loader_do_menu_closed(instance);
                break;
            case LoaderMessageTypeIsLocked:
                message.is_locked_result->bool_value = loader_do_is_locked(instance);
                api_lock_unlock(message.api_lock);
                break;
            case LoaderMessageTypeAppClosed:
                loader_do_app_closed(instance);
                break;
            }
        }
    }

    return 0;
}