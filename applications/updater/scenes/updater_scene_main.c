#include <furi.h>
#include <furi_hal.h>
#include <applications.h>
#include <storage/storage.h>

#include "updater/updater_i.h"
#include "updater/views/updater_main.h"
#include "updater_scene.h"

static void sd_mount_callback(const void* message, void* context) {
    Updater* updater = context;
    const StorageEvent* new_event = message;

    switch(new_event->type) {
    case StorageEventTypeCardMount:
        view_dispatcher_send_custom_event(updater->view_dispatcher, UpdaterCustomEventSdMounted);
        break;
    case StorageEventTypeCardUnmount:
        view_dispatcher_send_custom_event(updater->view_dispatcher, UpdaterCustomEventSdUnmounted);
        break;
    default:
        break;
    }
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

    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case UpdaterCustomEventSdMounted:
            if(update_task_init(updater->update_task)) {
                update_task_start(updater->update_task);
            }

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

    scene_manager_set_scene_state(updater->scene_manager, UpdaterSceneMain, 0);
}