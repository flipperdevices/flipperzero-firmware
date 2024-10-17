#pragma once

#include <gui/view.h>
#include <notification/notification.h>
#include "desktop_events.h"

#define HINT_TIMEOUT 2

typedef struct DesktopLockMenuView DesktopLockMenuView;

typedef void (*DesktopLockMenuViewCallback)(DesktopEvent event, void* context);

struct DesktopLockMenuView {
    View* view;
    DesktopLockMenuViewCallback callback;
    void* context;
    NotificationApp* notification_record;
};

typedef struct {
    uint8_t idx;
    bool dummy_mode;
    bool stealth_mode;
    uint8_t display_brightness_index;
    bool notification_settings_changed;
} DesktopLockMenuViewModel;

void desktop_lock_menu_set_callback(
    DesktopLockMenuView* lock_menu,
    DesktopLockMenuViewCallback callback,
    void* context);

View* desktop_lock_menu_get_view(DesktopLockMenuView* lock_menu);
void desktop_lock_menu_set_dummy_mode_state(DesktopLockMenuView* lock_menu, bool dummy_mode);
void desktop_lock_menu_set_stealth_mode_state(DesktopLockMenuView* lock_menu, bool stealth_mode);
void desktop_lock_menu_set_idx(DesktopLockMenuView* lock_menu, uint8_t idx);
void desktop_lock_menu_set_notification_record(
    DesktopLockMenuView* lock_menu,
    NotificationApp* notification);
bool desktop_lock_menu_did_notification_settings_change(DesktopLockMenuView* lock_menu);
void desktop_lock_menu_refresh_notification_values(DesktopLockMenuView* lock_menu);
DesktopLockMenuView* desktop_lock_menu_alloc(void);
void desktop_lock_menu_free(DesktopLockMenuView* lock_menu);
