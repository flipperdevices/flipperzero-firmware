#include "../dolphin_i.h"
#include "../views/dolphin_main_view.h"
#include "applications.h"

static void dolphin_switch_to_app(Dolphin* dolphin, const FlipperApplication* flipper_app) {
    furi_assert(dolphin);
    furi_assert(flipper_app);
    furi_assert(flipper_app->app);
    furi_assert(flipper_app->name);

    if(furi_thread_get_state(dolphin->scene_thread) != FuriThreadStateStopped) {
        FURI_LOG_E("Dolphin", "Thread is already running");
        return;
    }

    furi_thread_set_name(dolphin->scene_thread, flipper_app->name);
    furi_thread_set_stack_size(dolphin->scene_thread, flipper_app->stack_size);
    furi_thread_set_callback(dolphin->scene_thread, flipper_app->app);

    furi_thread_start(dolphin->scene_thread);
}

void dolphin_scene_main_callback(DolphinMainEvent event, void* context) {
    Dolphin* dolphin = (Dolphin*)context;
    view_dispatcher_send_custom_event(dolphin->view_dispatcher, event);
}

const void dolphin_scene_main_on_enter(void* context) {
    Dolphin* dolphin = (Dolphin*)context;
    DolphinMainView* main_view = dolphin->main_view;

    dolphin_main_set_callback(main_view, dolphin_scene_main_callback, dolphin);
    view_port_enabled_set(dolphin->lock_viewport, false);

    view_dispatcher_switch_to_view(dolphin->view_dispatcher, DolphinViewMain);
}

const bool dolphin_scene_main_on_event(void* context, SceneManagerEvent event) {
    Dolphin* dolphin = (Dolphin*)context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case DolphinMainEventOpenMenu:
            with_value_mutex(
                dolphin->menu_vm, (Menu * menu) { menu_ok(menu); });
            consumed = true;
            break;

        case DolphinMainEventOpenLockMenu:
            scene_manager_next_scene(dolphin->scene_manager, DolphinSceneLockMenu);
            consumed = true;
            break;

        case DolphinMainEventOpenDebug:
            scene_manager_next_scene(dolphin->scene_manager, DolphinViewDebug);
            consumed = true;
            break;

        case DolphinMainEventOpenArchive:
            dolphin_switch_to_app(dolphin, &FLIPPER_ARCHIVE);
            consumed = true;
            break;

        default:
            break;
        }
    }
    return consumed;
}

const void dolphin_scene_main_on_exit(void* context) {
    // Dolphin* dolphin = (Dolphin*)context;
}
