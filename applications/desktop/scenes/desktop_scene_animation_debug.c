#include "../desktop_i.h"
#include "../views/desktop_main.h"
#include "applications.h"

#include "desktop/helpers/desktop_animation.h"
#include "gui/scene_manager.h"
#include "gui/view_port.h"
#include <gui/modules/submenu.h>
#include <stdint.h>

#include "../helpers/desktop_animation.h"
#include <m-list.h>
#include <m-array.h>

#define ANIMATION_NO_SD (200)
#define ANIMATION_BAD_BATTERY (201)
#define ANIMATION_CARD_BAD (202)
#define ANIMATION_CARD_NO_DB (203)
#define ANIMATION_CARD_NO_DB_URL (204)
#define ANIMATION_LEVELUP_PENDING (205)

#define EVENT_BIT_SUBMENU (0x40000000)

extern const PairedAnimation idle_animations[17];
extern const PairedAnimation no_sd_animation[1];
extern const PairedAnimation check_battery_animation[1];
extern const PairedAnimation card_bad_animation[1];
extern const PairedAnimation no_db_animation[1];
extern const PairedAnimation no_db_url_animation[1];
extern const PairedAnimation levelup_pending_animation[1];

void desktop_scene_animation_debug_callback(DesktopMainEvent event, void* context) {
    Desktop* desktop = (Desktop*)context;
    view_dispatcher_send_custom_event(desktop->view_dispatcher, event);
}

void desktop_scene_animation_list_submenu_callback(void* context, uint32_t index) {
    Desktop* desktop = (Desktop*)context;
    view_dispatcher_send_custom_event(desktop->view_dispatcher, index);
}

void desktop_scene_animation_debug_on_enter(void* context) {
    Desktop* desktop = (Desktop*)context;

    desktop_main_set_callback(desktop->main_view, desktop_scene_animation_debug_callback, desktop);

    submenu_set_header(desktop->submenu, "-");
    for(int i = 0; i < COUNT_OF(idle_animations); ++i) {
        submenu_add_item(
            desktop->submenu,
            idle_animations[i].basic->name,
            EVENT_BIT_SUBMENU | i,
            desktop_scene_animation_list_submenu_callback,
            desktop);
    }

    submenu_add_item(
        desktop->submenu,
        no_sd_animation[0].basic->name,
        EVENT_BIT_SUBMENU | ANIMATION_NO_SD,
        desktop_scene_animation_list_submenu_callback,
        desktop);
    submenu_add_item(
        desktop->submenu,
        check_battery_animation[0].basic->name,
        EVENT_BIT_SUBMENU | ANIMATION_BAD_BATTERY,
        desktop_scene_animation_list_submenu_callback,
        desktop);

    submenu_add_item(
        desktop->submenu,
        card_bad_animation[0].basic->name,
        EVENT_BIT_SUBMENU | ANIMATION_CARD_BAD,
        desktop_scene_animation_list_submenu_callback,
        desktop);

    submenu_add_item(
        desktop->submenu,
        no_db_animation[0].basic->name,
        EVENT_BIT_SUBMENU | ANIMATION_CARD_NO_DB,
        desktop_scene_animation_list_submenu_callback,
        desktop);

    submenu_add_item(
        desktop->submenu,
        no_db_url_animation[0].basic->name,
        EVENT_BIT_SUBMENU | ANIMATION_CARD_NO_DB_URL,
        desktop_scene_animation_list_submenu_callback,
        desktop);

    submenu_add_item(
        desktop->submenu,
        levelup_pending_animation[0].basic->name,
        EVENT_BIT_SUBMENU | ANIMATION_LEVELUP_PENDING,
        desktop_scene_animation_list_submenu_callback,
        desktop);

    view_dispatcher_switch_to_view(desktop->view_dispatcher, DesktopViewSubmenu);
}

bool desktop_scene_animation_debug_on_event(void* context, SceneManagerEvent event) {
    Desktop* desktop = (Desktop*)context;
    bool consumed = false;
    consumed = true;

    if(event.type == SceneManagerEventTypeCustom) {
        if(EVENT_BIT_SUBMENU & event.event) {
            furi_assert(!desktop->debug_animation);
            event.event &= ~EVENT_BIT_SUBMENU;
            switch(event.event) {
            case ANIMATION_NO_SD:
                desktop->debug_animation = &no_sd_animation[0];
                break;
            case ANIMATION_BAD_BATTERY:
                desktop->debug_animation = &check_battery_animation[0];
                break;
            case ANIMATION_CARD_NO_DB:
                desktop->debug_animation = &no_db_animation[0];
                break;
            case ANIMATION_CARD_NO_DB_URL:
                desktop->debug_animation = &no_db_url_animation[0];
                break;
            case ANIMATION_CARD_BAD:
                desktop->debug_animation = &card_bad_animation[0];
                break;
            case ANIMATION_LEVELUP_PENDING:
                desktop->debug_animation = &levelup_pending_animation[0];
                break;
            default:
                desktop->debug_animation = &idle_animations[event.event];
                break;
            }
            desktop_main_switch_dolphin_animation(
                desktop->main_view,
                desktop->debug_animation->basic->icon,
                desktop->debug_animation->basic->black_status_bar);
            view_dispatcher_switch_to_view(desktop->view_dispatcher, DesktopViewMain);
            desktop->debug_animation_active = false;
        } else {
            if(event.event == DesktopMainEventRightShort) {
                if(desktop->debug_animation->active && !desktop->debug_animation_active) {
                    desktop->debug_animation_active = true;
                    desktop_main_switch_dolphin_animation(
                        desktop->main_view,
                        desktop->debug_animation->active->icon,
                        desktop->debug_animation->active->black_status_bar);
                } else if(desktop->debug_animation_active) {
                    desktop->debug_animation_active = false;
                    desktop_main_switch_dolphin_animation(
                        desktop->main_view,
                        desktop->debug_animation->basic->icon,
                        desktop->debug_animation->basic->black_status_bar);
                }
                furi_assert(desktop->debug_animation);
            } else if(event.event == DesktopMainEventBack) {
                if(desktop->debug_animation) {
                    desktop->debug_animation = NULL;
                    view_dispatcher_switch_to_view(desktop->view_dispatcher, DesktopViewSubmenu);
                } else {
                    scene_manager_previous_scene(desktop->scene_manager);
                }
            }
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        scene_manager_previous_scene(desktop->scene_manager);
    }

    return consumed;
}

void desktop_scene_animation_debug_on_exit(void* context) {
    Desktop* desktop = (Desktop*)context;
    submenu_clean(desktop->submenu);
}
