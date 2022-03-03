#include <furi.h>
#include <furi_hal.h>
#include <applications.h>
#include <assets_icons.h>
#include <loader/loader.h>
#include <storage/storage.h>

#include "updater/updater_i.h"
#include "updater/views/updater_main.h"
#include "updater_scene.h"
//#include "updater_scene_i.h"

#define TAG "UpdaterSrv"

#define MAIN_VIEW_DEFAULT (0UL)

//void updater_scene_main_callback(UpdaterEvent event, void* context) {
//    Updater* updater = (Updater*)context;
//    view_dispatcher_send_custom_event(updater->view_dispatcher, event);
//}

static void sd_mount_callback(const void* message, void* context) {
    Updater* updater = context;
    const StorageStatus* new_status = message;

    view_dispatcher_send_custom_event(
        updater->view_dispatcher,
        *new_status == StorageStatusOK ? UpdaterEventSdMounted : UpdaterEventSdUnmounted);
}

void updater_scene_main_on_enter(void* context) {
    Updater* updater = (Updater*)context;
    UpdaterMainView* main_view = updater->main_view;
    //main_view->

    FuriPubSubSubscription* sub =
        furi_pubsub_subscribe(storage_get_pubsub(updater->storage), &sd_mount_callback, updater);
    updater_main_set_storage_pubsub(main_view, sub);

    //updater_main_set_callback(main_view, updater_scene_main_callback, updater);

    //UpdaterMainSceneState state =
    //    scene_manager_get_scene_state(updater->scene_manager, UpdaterSceneMain);
    //if(state == UpdaterMainSceneStateLockedNoPin) {
    //    updater_locked_lock(updater->locked_view);
    //    view_port_enabled_set(updater->lock_viewport, true);
    //} else if(state == UpdaterMainSceneStateLockedWithPin) {
    //    LOAD_DESKTOP_SETTINGS(&updater->settings);
    //    furi_assert(updater->settings.pincode.length > 0);
    //    updater_locked_lock_pincode(updater->locked_view, updater->settings.pincode);
    //    view_port_enabled_set(updater->lock_viewport, true);
    //    furi_hal_rtc_set_flag(FuriHalRtcFlagLock);
    //    furi_hal_usb_disable();
    //} else {
    //    furi_assert(state == UpdaterMainSceneStateUnlocked);
    //    view_port_enabled_set(updater->lock_viewport, false);
    //}

    view_dispatcher_switch_to_view(updater->view_dispatcher, UpdaterViewMain);
}

bool updater_scene_main_on_event(void* context, SceneManagerEvent event) {
    Updater* updater = (Updater*)context;
    bool consumed = false;
    string_t update_path;

    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case UpdaterEventSdMounted:
            string_init_set(update_path, "/ext"UPDATE_DIR_DEFAULT_PATH);
            if(update_task_init(updater->update_task, update_path)) {
                update_task_start(updater->update_task);
            }
            string_clear(update_path);

            break;
        case UpdaterEventSdUnmounted:
            // TODO: error out, stop worker (it's probably dead actually)
            break;
        case UpdaterEventProgressUpdate:

            break;
        default:
            break;
        }
    }

    return consumed;
}

void updater_scene_main_on_exit(void* context) {
    Updater* updater = (Updater*)context;

    furi_pubsub_unsubscribe(
        storage_get_pubsub(updater->storage), updater_main_get_storage_pubsub(updater->main_view));

    scene_manager_set_scene_state(updater->scene_manager, UpdaterSceneMain, MAIN_VIEW_DEFAULT);
}