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

#include <notification/notification.h>
#include <notification/notification_messages.h>

#include <flipper_format/flipper_format.h>
#include <storage/storage.h>
#include <dialogs/dialogs.h>

#include "views/xremote_common_view.h"
#include "xc_icons.h"

#define xremote_app_assert_void(cond) \
    if(!cond) return
#define xremote_app_assert(cond, var) \
    if(!cond) return var

typedef struct {
    ViewOrientation orientation;
    uint32_t repeat_count;
} XRemoteAppSettings;

XRemoteAppSettings* xremote_app_settings_alloc();
void xremote_app_settings_free(XRemoteAppSettings* settings);

typedef struct {
    XRemoteAppSettings* app_settings;
    NotificationApp* notifications;
    ViewDispatcher* view_dispatcher;
    void* app_argument;
    Gui* gui;
} XRemoteAppContext;

XRemoteAppContext* xremote_app_context_alloc(void* arg);
void xremote_app_context_free(XRemoteAppContext* ctx);

bool xremote_app_settings_store(XRemoteAppSettings* settings);
bool xremote_app_settings_load(XRemoteAppSettings* settings);

typedef XRemoteView* (*XRemoteViewAllocator)(NotificationApp* notifications);
typedef void (*XRemoteAppClearCallback)(void* context);

typedef struct {
    XRemoteAppClearCallback on_clear;
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

void xremote_app_view_alloc(XRemoteApp* app, uint32_t view_id, XRemoteViewAllocator allocator);
void xremote_app_view_free(XRemoteApp* app);

void xremote_app_view_set_previous_callback(XRemoteApp* app, ViewNavigationCallback callback);
void xremote_app_set_view_context(
    XRemoteApp* app,
    void* context,
    XRemoteViewClearCallback on_clear);
void xremote_app_set_user_context(XRemoteApp* app, void* context, XRemoteAppClearCallback on_clear);
void xremote_app_user_context_free(XRemoteApp* app);

bool xremote_app_has_view(XRemoteApp* app, uint32_t view_id);
void xremote_app_switch_to_view(XRemoteApp* app, uint32_t view_id);
void xremote_app_switch_to_submenu(XRemoteApp* app);

XRemoteApp* xremote_app_alloc(XRemoteAppContext* ctx);
void xremote_app_free(XRemoteApp* app);
