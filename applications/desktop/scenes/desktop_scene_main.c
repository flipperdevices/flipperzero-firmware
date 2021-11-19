#include "../desktop_i.h"
#include "../views/desktop_main.h"
#include "applications.h"
#include "assets_icons.h"
#include "dolphin/dolphin.h"
#include "furi/pubsub.h"
#include "furi/record.h"
#include "storage/storage-glue.h"
#include <loader/loader.h>
#include <m-list.h>
#define MAIN_VIEW_DEFAULT (0UL)


static void desktop_switch_to_app(Desktop* desktop, const FlipperApplication* flipper_app) {
    furi_assert(desktop);
    furi_assert(flipper_app);
    furi_assert(flipper_app->app);
    furi_assert(flipper_app->name);

    if(furi_thread_get_state(desktop->scene_thread) != FuriThreadStateStopped) {
        FURI_LOG_E("Desktop", "Thread is already running");
        return;
    }

    furi_thread_set_name(desktop->scene_thread, flipper_app->name);
    furi_thread_set_stack_size(desktop->scene_thread, flipper_app->stack_size);
    furi_thread_set_callback(desktop->scene_thread, flipper_app->app);

    furi_thread_start(desktop->scene_thread);
}

static void desktop_scene_main_storage_status_changed_callback(const void* message, void* context) {
    furi_assert(context);
    Desktop* desktop = context;
    StorageStatus status = *(const StorageStatus*) message;
    switch(status) {
    case StorageStatusOK:
    case StorageStatusNotAccessible:
    case StorageStatusNotReady:
    case StorageStatusErrorInternal:
        /* do nothing */
        break;
    case StorageStatusNoFS:
        break;
    case StorageStatusNotMounted:
    default:
        furi_assert(0);
    }

//    if (!desktop->update_animation_flag) {
        view_dispatcher_send_custom_event(desktop->view_dispatcher, DesktopMainEventUpdateAnimation);
//        desktop->update_animation_flag = true;
//    }
}

void desktop_scene_main_callback(DesktopMainEvent event, void* context) {
    Desktop* desktop = (Desktop*)context;
    view_dispatcher_send_custom_event(desktop->view_dispatcher, event);
}

static void desktop_scene_main_animation_changed_callback(void* context) {
    furi_assert(context);
    Desktop* desktop = context;
//    if (!desktop->update_animation_flag) {
        view_dispatcher_send_custom_event(desktop->view_dispatcher, DesktopMainEventUpdateAnimation);
//        desktop->update_animation_flag = true;
//    }
}

void desktop_scene_main_on_enter(void* context) {
    Desktop* desktop = (Desktop*)context;
    DesktopMainView* main_view = desktop->main_view;

    Storage* storage = furi_record_open("storage");
    desktop->storage_changed = furi_pubsub_subscribe(storage_get_pubsub(storage),
            desktop_scene_main_storage_status_changed_callback,
            desktop);
    furi_record_close("storage");

    desktop_main_set_callback(main_view, desktop_scene_main_callback, desktop);
    view_port_enabled_set(desktop->lock_viewport, false);

    if(scene_manager_get_scene_state(desktop->scene_manager, DesktopSceneMain) ==
       DesktopMainEventUnlocked) {
        desktop_main_unlocked(desktop->main_view);
    }

    desktop_animation_activate(desktop->animation);
    desktop_animation_set_animation_changed_callback(desktop->animation, desktop_scene_main_animation_changed_callback, desktop);
    const Icon* icon = desktop_animation_get_animation(desktop->animation);
    desktop_main_switch_dolphin_animation(desktop->main_view, icon);
    view_dispatcher_switch_to_view(desktop->view_dispatcher, DesktopViewMain);
}

bool desktop_scene_main_on_event(void* context, SceneManagerEvent event) {
    Desktop* desktop = (Desktop*)context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
            case DesktopMainEventOpenMenu:
                loader_show_menu();
                consumed = true;
                break;

            case DesktopMainEventOpenLockMenu:
                scene_manager_next_scene(desktop->scene_manager, DesktopSceneLockMenu);
                consumed = true;
                break;

            case DesktopMainEventOpenDebug:
                scene_manager_next_scene(desktop->scene_manager, DesktopSceneDebug);
                consumed = true;
                break;

            case DesktopMainEventOpenArchive:
                desktop_switch_to_app(desktop, &FLIPPER_ARCHIVE);
                consumed = true;
                break;

            case DesktopMainEventOpenFavorite:
                LOAD_DESKTOP_SETTINGS(&desktop->settings);
                desktop_switch_to_app(desktop, &FLIPPER_APPS[desktop->settings.favorite]);
                consumed = true;
                break;

            case DesktopMainEventUpdateAnimation: {
                FURI_LOG_I("Desktop", "EventUpdateAnimation");
                desktop->update_animation_flag = false;
                const Icon* icon = desktop_animation_get_animation(desktop->animation);
                desktop_main_switch_dolphin_animation(desktop->main_view, icon);
                consumed = true;
                break;
            }

            case DesktopMainEventRightShort: {
                DesktopAnimationState state = desktop_animation_handle_right(desktop->animation);
                if (state == DesktopAnimationStateLevelUpIsPending) {
                    FURI_LOG_W("DBG", "go to LevelUp scene");
                    scene_manager_next_scene(desktop->scene_manager, DesktopSceneLevelUp);
                }
                break;
            }

            default:
                break;
        }

        if(event.event != DesktopMainEventUpdateAnimation) {
//            FURI_LOG_W("DBG", "main_scene event %lu, activate animation", event.type);
            desktop_animation_activate(desktop->animation);
        }
    } else if(event.type != SceneManagerEventTypeTick) {
//        FURI_LOG_W("DBG", "main_scene event %lu, activate animation", event.type);
        desktop_animation_activate(desktop->animation);
    }

    return consumed;
}

void desktop_scene_main_on_exit(void* context) {
    Desktop* desktop = (Desktop*)context;

    desktop_animation_set_animation_changed_callback(desktop->animation, NULL, NULL);
    scene_manager_set_scene_state(desktop->scene_manager, DesktopSceneMain, MAIN_VIEW_DEFAULT);
    desktop_main_reset_hint(desktop->main_view);
}
