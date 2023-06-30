#include <gui/scene_manager.h>
#include <applications.h>
#include <furi_hal.h>
#include <toolbox/saved_struct.h>
#include <stdbool.h>
#include <loader/loader.h>
#include <cfw.h>

#include "../desktop_i.h"
#include <desktop/desktop_settings.h>
#include "../views/desktop_view_lock_menu.h"
#include "desktop_scene_i.h"
#include "desktop_scene.h"
#include <power/power_service/power.h>
#include "../helpers/pin.h"

#define TAG "DesktopSceneLock"

void desktop_scene_lock_menu_callback(DesktopEvent event, void* context) {
    Desktop* desktop = (Desktop*)context;
    view_dispatcher_send_custom_event(desktop->view_dispatcher, event);
}

void desktop_scene_lock_menu_on_enter(void* context) {
    Desktop* desktop = (Desktop*)context;

    DESKTOP_SETTINGS_LOAD(&desktop->settings);
    scene_manager_set_scene_state(desktop->scene_manager, DesktopSceneLockMenu, 0);
    desktop_lock_menu_set_callback(desktop->lock_menu, desktop_scene_lock_menu_callback, desktop);
    desktop_lock_menu_set_dummy_mode_state(desktop->lock_menu, desktop->settings.dummy_mode);
    desktop_lock_menu_set_stealth_mode_state(
        desktop->lock_menu, furi_hal_rtc_is_flag_set(FuriHalRtcFlagStealthMode));
    if(CFW_SETTINGS()->lock_menu_type) {
        desktop_lock_menu_set_idx(desktop->lock_menu, 3);
    } else {
        desktop_lock_menu_set_idx(desktop->lock_menu, 0);
    }

    Gui* gui = furi_record_open(RECORD_GUI);
    gui_set_hide_statusbar(gui, true);
    furi_record_close(RECORD_GUI);

    view_dispatcher_switch_to_view(desktop->view_dispatcher, DesktopViewIdLockMenu);
}

void desktop_scene_lock_menu_save_settings(Desktop* desktop) {
    if(desktop->lock_menu->save_notification) {
        notification_message_save_settings(desktop->lock_menu->notification);
        desktop->lock_menu->save_notification = false;
    }
    if(desktop->lock_menu->save_cfw) {
        CFW_SETTINGS_SAVE();
        desktop->lock_menu->save_cfw = false;
    }
}

bool desktop_scene_lock_menu_on_event(void* context, SceneManagerEvent event) {
    Desktop* desktop = (Desktop*)context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case DesktopLockMenuEventExit:
            desktop_scene_lock_menu_save_settings(desktop);
            scene_manager_search_and_switch_to_previous_scene(
                desktop->scene_manager, DesktopSceneMain);
            break;
        case DesktopLockMenuEventLock:
            desktop_scene_lock_menu_save_settings(desktop);
            scene_manager_set_scene_state(desktop->scene_manager, DesktopSceneLockMenu, 0);
            desktop_lock(desktop);
            consumed = true;
            break;
        case DesktopLockMenuEventLockShutdown:
            desktop_scene_lock_menu_save_settings(desktop);
            scene_manager_set_scene_state(desktop->scene_manager, DesktopSceneLockMenu, 0);
            desktop_lock(desktop);
            consumed = true;
            Power* power = furi_record_open(RECORD_POWER);
            furi_delay_ms(666);
            power_off(power);
            furi_record_close(RECORD_POWER);
            break;
        case DesktopLockMenuEventDummyModeOn:
            desktop_set_dummy_mode_state(desktop, true);
            scene_manager_search_and_switch_to_previous_scene(
                desktop->scene_manager, DesktopSceneMain);
            break;
        case DesktopLockMenuEventDummyModeOff:
            desktop_set_dummy_mode_state(desktop, false);
            scene_manager_search_and_switch_to_previous_scene(
                desktop->scene_manager, DesktopSceneMain);
            break;
        case DesktopLockMenuEventStealthModeOn:
            desktop_set_stealth_mode_state(desktop, true);
            scene_manager_search_and_switch_to_previous_scene(
                desktop->scene_manager, DesktopSceneMain);
            break;
        case DesktopLockMenuEventStealthModeOff:
            desktop_set_stealth_mode_state(desktop, false);
            scene_manager_search_and_switch_to_previous_scene(
                desktop->scene_manager, DesktopSceneMain);
            break;
        default:
            break;
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        scene_manager_set_scene_state(desktop->scene_manager, DesktopSceneLockMenu, 0);
    }
    return consumed;
}

void desktop_scene_lock_menu_on_exit(void* context) {
    UNUSED(context);
    Desktop* desktop = (Desktop*)context;
    desktop_scene_lock_menu_save_settings(desktop);

    Gui* gui = furi_record_open(RECORD_GUI);
    gui_set_hide_statusbar(gui, false);
    furi_record_close(RECORD_GUI);
}