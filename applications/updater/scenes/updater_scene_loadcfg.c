#include <furi.h>
#include <furi_hal.h>
#include <applications.h>
#include <assets_icons.h>
#include <loader/loader.h>
#include <storage/storage.h>

#include "updater/updater_i.h"
#include "updater_scene.h"
//#include "updater_scene_i.h"

#define TAG "UpdaterSrv"

void updater_scene_loadcfg_apply_callback(GuiButtonType result, InputType type, void* context) {
    furi_assert(context);
    Updater* updater = context;
    if(type != InputTypeShort) {
        return;
    }

    if((result == GuiButtonTypeRight)) {
        view_dispatcher_send_custom_event(updater->view_dispatcher, UpdaterCustomEventApplyUpdate);
    } else if((result == GuiButtonTypeLeft)) {
        view_dispatcher_send_custom_event(updater->view_dispatcher, UpdaterCustomEventCancelUpdate);
    }
}

void updater_scene_loadcfg_on_enter(void* context) {
    Updater* updater = (Updater*)context;
    UpdaterManifestProcessingState* pending_upd = updater->pending_update =
        malloc(sizeof(UpdaterManifestProcessingState));
    pending_upd->manifest = update_manifest_alloc();

    if(update_manifest_init(pending_upd->manifest, updater->startup_arg)) {
        //if (false) {
        string_init_printf(
            pending_upd->message,
            "Install '%s'?",
            //string_get_cstr(pending_upd->manifest->version)
            "UPDATE");
        widget_add_button_element(
            updater->widget,
            GuiButtonTypeRight,
            "Install",
            updater_scene_loadcfg_apply_callback,
            updater);
    } else {
        string_init_set(pending_upd->message, "Invalid update manifest");
    }

    widget_add_button_element(
        updater->widget,
        GuiButtonTypeLeft,
        "Cancel",
        updater_scene_loadcfg_apply_callback,
        updater);

    widget_add_string_multiline_element(
        updater->widget,
        64,
        13,
        AlignCenter,
        AlignCenter,
        FontSecondary,
        //"alloetoti");
        string_get_cstr(pending_upd->message));

    view_dispatcher_switch_to_view(updater->view_dispatcher, UpdaterViewWidget);
}

bool updater_scene_loadcfg_on_event(void* context, SceneManagerEvent event) {
    Updater* updater = (Updater*)context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeBack) {
        view_dispatcher_stop(updater->view_dispatcher);
        return true;
    } else if(event.type == SceneManagerEventTypeTick) {
    } else if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case UpdaterCustomEventApplyUpdate:
            // TODO: implement
            //scene_manager_previous_scene(updater->scene_manager);
            view_dispatcher_stop(updater->view_dispatcher);
            return true;
        case UpdaterCustomEventCancelUpdate:
            view_dispatcher_stop(updater->view_dispatcher);
            return true;
        default:
            break;
        }
    }

    return consumed;
}

void updater_scene_loadcfg_on_exit(void* context) {
    Updater* updater = (Updater*)context;

    if(updater->pending_update) {
        update_manifest_free(updater->pending_update->manifest);
        string_clear(updater->pending_update->message);
    }

    widget_reset(updater->widget);
    free(updater->pending_update);
}