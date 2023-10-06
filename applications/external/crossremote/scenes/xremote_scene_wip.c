#include "../xremote.h"

void xremote_scene_wip_on_enter(void* context) {
    furi_assert(context);
    XRemote* app = context;
    Popup* popup = app->popup;

    //popup_set_icon(popup, 0, 2, &I_DolphinMafia_115x62);
    popup_set_header(popup, "SubGhz coming soon", 10, 19, AlignLeft, AlignBottom);
    popup_set_text(popup, "Check back later", 10, 29, AlignLeft, AlignBottom);
    popup_set_text(popup, "Press back long", 10, 39, AlignLeft, AlignBottom);

    popup_set_callback(popup, xremote_popup_closed_callback);
    popup_set_context(popup, context);
    popup_set_timeout(popup, 100);
    popup_enable_timeout(popup);

    view_dispatcher_switch_to_view(app->view_dispatcher, XRemoteViewIdWip);
}

bool xremote_scene_wip_on_event(void* context, SceneManagerEvent event) {
    XRemote* app = context;
    UNUSED(app);
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == XRemoteCustomEventTypePopupClosed) {
        }
        consumed = true;
    }

    return consumed;
}

void xremote_scene_wip_on_exit(void* context) {
    XRemote* app = context;
    UNUSED(app);
}