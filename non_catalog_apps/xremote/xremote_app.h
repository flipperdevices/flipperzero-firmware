/*!
 *  @file flipper-xremote/xremote_app.h
    @license This project is released under the GNU GPLv3 License
 *  @copyright (c) 2023 Sandro Kalatozishvili (s.kalatoz@gmail.com)
 *
 * @brief Shared functionality and data types between the apps.
 */

#pragma once

#include <furi.h>
#include <gui/gui.h>
#include <gui/view.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/submenu.h>
#include <gui/modules/dialog_ex.h>
#include <gui/modules/text_input.h>
#include <gui/modules/variable_item_list.h>

#include <notification/notification.h>
#include <notification/notification_messages.h>

#include <flipper_format/flipper_format.h>
#include <storage/storage.h>
#include <dialogs/dialogs.h>
#include <infrared_worker.h>

#include "views/xremote_common_view.h"
#include "xc_icons.h"

//////////////////////////////////////////////////////////////////////////////
// XRemote generic functions and definitions
//////////////////////////////////////////////////////////////////////////////

#define XREMOTE_APP_EXTENSION ".ir"
#define XREMOTE_APP_FOLDER ANY_PATH("infrared")
#define XREMOTE_APP_TEXT_MAX 128

#define xremote_app_assert_void(cond) \
    if(!cond) return
#define xremote_app_assert(cond, var) \
    if(!cond) return var

typedef enum { XRemoteAppExitPress, XRemoteAppExitHold } XRemoteAppExit;

XRemoteAppExit xremote_app_get_exit_behavior(uint8_t exit_index);
const char* xremote_app_get_exit_str(XRemoteAppExit exit_behavior);
uint32_t xremote_app_get_exit_index(XRemoteAppExit exit_behavior);

ViewOrientation xremote_app_get_orientation(uint8_t orientation_index);
const char* xremote_app_get_orientation_str(ViewOrientation view_orientation);
uint32_t xremote_app_get_orientation_index(ViewOrientation view_orientation);

//////////////////////////////////////////////////////////////////////////////
// XRemote application settings
//////////////////////////////////////////////////////////////////////////////

typedef struct {
    ViewOrientation orientation;
    XRemoteAppExit exit_behavior;
    uint32_t repeat_count;
} XRemoteAppSettings;

XRemoteAppSettings* xremote_app_settings_alloc();
void xremote_app_settings_free(XRemoteAppSettings* settings);

bool xremote_app_settings_store(XRemoteAppSettings* settings);
bool xremote_app_settings_load(XRemoteAppSettings* settings);

//////////////////////////////////////////////////////////////////////////////
// XRemote gloal context shared between every child application
//////////////////////////////////////////////////////////////////////////////

typedef struct {
    XRemoteAppSettings* app_settings;
    NotificationApp* notifications;
    ViewDispatcher* view_dispatcher;
    FuriString* file_path;
    void* app_argument;
    Gui* gui;
} XRemoteAppContext;

XRemoteAppContext* xremote_app_context_alloc(void* arg);
void xremote_app_context_free(XRemoteAppContext* ctx);

const char* xremote_app_context_get_exit_str(XRemoteAppContext* app_ctx);
void xremote_app_context_notify_led(XRemoteAppContext* app_ctx);
void xremote_app_notification_blink(NotificationApp* notifications);
bool xremote_app_send_signal(XRemoteAppContext* app_ctx, InfraredSignal* signal);
bool xremote_app_browser_select_file(XRemoteAppContext* app_ctx, const char* extension);

//////////////////////////////////////////////////////////////////////////////
// XRemote buttons and custom button pairs
//////////////////////////////////////////////////////////////////////////////

typedef struct {
    XRemoteAppContext* app_ctx;
    InfraredRemote* remote;
    FuriString* custom_up;
    FuriString* custom_down;
    FuriString* custom_left;
    FuriString* custom_right;
    FuriString* custom_ok;
    FuriString* custom_up_hold;
    FuriString* custom_down_hold;
    FuriString* custom_left_hold;
    FuriString* custom_right_hold;
    FuriString* custom_ok_hold;
} XRemoteAppButtons;

void xremote_app_buttons_free(XRemoteAppButtons* buttons);
XRemoteAppButtons* xremote_app_buttons_alloc();
XRemoteAppButtons* xremote_app_buttons_load(XRemoteAppContext* app_ctx);

bool xremote_app_extension_store(XRemoteAppButtons* buttons, FuriString* path);
bool xremote_app_extension_load(XRemoteAppButtons* buttons, FuriString* path);

//////////////////////////////////////////////////////////////////////////////
// XRemote application factory
//////////////////////////////////////////////////////////////////////////////

typedef struct {
    XRemoteClearCallback on_clear;
    XRemoteAppContext* app_ctx;
    XRemoteViewID submenu_id;
    XRemoteViewID view_id;
    XRemoteView* view_ctx;
    Submenu* submenu;
    void* context;
} XRemoteApp;

void xremote_app_submenu_add(
    XRemoteApp* app,
    const char* name,
    uint32_t index,
    SubmenuItemCallback callback);

void xremote_app_submenu_alloc(XRemoteApp* app, uint32_t index, ViewNavigationCallback prev_cb);
void xremote_app_submenu_free(XRemoteApp* app);

void xremote_app_view_alloc2(
    XRemoteApp* app,
    uint32_t view_id,
    XRemoteViewAllocator2 allocator,
    void* model_ctx);
void xremote_app_view_alloc(XRemoteApp* app, uint32_t view_id, XRemoteViewAllocator allocator);
void xremote_app_view_free(XRemoteApp* app);

void xremote_app_view_set_previous_callback(XRemoteApp* app, ViewNavigationCallback callback);
void xremote_app_set_view_context(XRemoteApp* app, void* context, XRemoteClearCallback on_clear);
void xremote_app_set_user_context(XRemoteApp* app, void* context, XRemoteClearCallback on_clear);
void xremote_app_user_context_free(XRemoteApp* app);

bool xremote_app_has_view(XRemoteApp* app, uint32_t view_id);
void xremote_app_switch_to_view(XRemoteApp* app, uint32_t view_id);
void xremote_app_switch_to_submenu(XRemoteApp* app);

XRemoteApp* xremote_app_alloc(XRemoteAppContext* ctx);
void xremote_app_free(XRemoteApp* app);
