#include "../can_lin_tools_app_i.h"
#include "../views/can_lin_tools_view_can_hacker2.h"

void can_lin_tools_scene_can_hacker2_callback(
    CanLinToolsCustomEvent event,
    void* context) {
    furi_assert(context);
    CanLinToolsApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, event);
}

static const NotificationSequence notification_app_display_on = {

    &message_display_backlight_on,
    NULL,
};

static void can_lin_tools_scene_can_hacker2_update(void* context) {
    furi_assert(context);
    CanLinToolsApp* app = context;

    uint32_t frequency = 0;
    bool nfc_field = can_lin_tools_app_can_hacker2_is_nfc(app);
    bool rfid_field = can_lin_tools_app_can_hacker2_is_rfid(app, &frequency);

    if(nfc_field || rfid_field)
        notification_message(app->notifications, &notification_app_display_on);

    // can_lin_tools_view_can_hacker2_update(
    //     app->can_lin_tools_can_hacker2, nfc_field, rfid_field, frequency);
}

void can_lin_tools_scene_can_hacker2_on_enter(void* context) {
    furi_assert(context);
    CanLinToolsApp* app = context;

    // Start detection of field presence
    can_lin_tools_app_can_hacker2_start(app);

    view_dispatcher_switch_to_view(app->view_dispatcher, CanLinToolsViewCanHacker2);
}

bool can_lin_tools_scene_can_hacker2_on_event(void* context, SceneManagerEvent event) {
    furi_assert(context);
    CanLinToolsApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeTick) {
        can_lin_tools_scene_can_hacker2_update(app);
    }

    return consumed;
}

void can_lin_tools_scene_can_hacker2_on_exit(void* context) {
    furi_assert(context);
    CanLinToolsApp* app = context;
    // Stop detection of field presence
    can_lin_tools_app_can_hacker2_stop(app);
}
