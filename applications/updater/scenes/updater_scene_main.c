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

//void updater_scene_main_callback(UpdaterCustomEvent event, void* context) {
//    Updater* updater = (Updater*)context;
//    view_dispatcher_send_custom_event(updater->view_dispatcher, event);
//}

static void sd_mount_callback(const void* message, void* context) {
    Updater* updater = context;
    const StorageStatus* new_status = message;

    view_dispatcher_send_custom_event(
        updater->view_dispatcher,
        *new_status == StorageStatusOK ? UpdaterCustomEventSdMounted : UpdaterCustomEventSdUnmounted);
}

void updater_scene_main_on_enter(void* context) {
    Updater* updater = (Updater*)context;
    UpdaterMainView* main_view = updater->main_view;

    FuriPubSubSubscription* sub =
        furi_pubsub_subscribe(storage_get_pubsub(updater->storage), &sd_mount_callback, updater);
    updater_main_set_storage_pubsub(main_view, sub);

    view_dispatcher_switch_to_view(updater->view_dispatcher, UpdaterViewMain);
}

bool updater_scene_main_on_event(void* context, SceneManagerEvent event) {
    Updater* updater = (Updater*)context;
    bool consumed = false;
    string_t update_path;

    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case UpdaterCustomEventSdMounted:
            string_init_set(update_path, "/ext" UPDATE_DIR_DEFAULT_REL_PATH);
            if(update_task_init(updater->update_task, update_path)) {
                update_task_start(updater->update_task);
            }
            string_clear(update_path);

            break;
        case UpdaterCustomEventSdUnmounted:
            // TODO: error out, stop worker (it's probably dead actually)
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