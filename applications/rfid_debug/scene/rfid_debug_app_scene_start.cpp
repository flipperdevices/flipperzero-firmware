#include "rfid_debug_app_scene_start.h"
#include <notification/notification_messages.h>

typedef enum {
    SubmenuLFTune,
    SubmenuHFField,
    SubmenuLFField
} SubmenuIndex;

const NotificationSequence notification_both = {
    &message_blink_start_10, // blink cyan fast
    &message_blink_set_color_cyan,

    &message_vibro_on, // buzz twice
    &message_delay_100,
    &message_vibro_off,
    &message_delay_100,
    &message_vibro_on,
    &message_delay_100,
    &message_vibro_off,

    &message_do_not_reset,
    NULL,
};

const NotificationSequence notification_single = {
    &message_blink_start_20, // blink cyan slow
    &message_blink_set_color_blue,

    &message_vibro_on, // buzz once
    &message_delay_100,
    &message_vibro_off,

    &message_do_not_reset,
    NULL,
};

const NotificationSequence notification_neither = {
    &message_blink_stop, // turn off led

    &message_vibro_on, // tiny buzz once
    &message_delay_50,
    &message_vibro_off,
    NULL,
};

void RfidDebugAppSceneStart::draw_menu(RfidDebugApp* app, int32_t selected) {
    auto submenu = app->view_controller.get<SubmenuVM>();
    auto callback = cbc::obtain_connector(this, &RfidDebugAppSceneStart::submenu_callback);

    submenu->clean();

    submenu->add_item("LF Tune", SubmenuLFTune, callback, app);

    if (app->HF_field_enabled) {
        submenu->add_item("HF Field    !! ENABLED !!", SubmenuHFField, callback, app);
    } else {
        submenu->add_item("HF Field", SubmenuHFField, callback, app);
    }

    if (app->LF_field_enabled) {
        submenu->add_item("LF Field    !! ENABLED !!", SubmenuLFField, callback, app);
    } else {
        submenu->add_item("LF Field", SubmenuLFField, callback, app);
    }

    if (selected != -1) {
        submenu->set_selected_item(selected);
    }
}

void RfidDebugAppSceneStart::on_enter(RfidDebugApp* app, bool need_restore) {
    app->notification = (NotificationApp*)furi_record_open(RECORD_NOTIFICATION);
    app->HF_field_enabled = false;

    draw_menu(app, need_restore ? submenu_item_selected : -1);

    app->view_controller.switch_to<SubmenuVM>();
}

bool RfidDebugAppSceneStart::on_event(RfidDebugApp* app, RfidDebugApp::Event* event) {
    bool consumed = false;

    if(event->type == RfidDebugApp::EventType::MenuSelected) {
        submenu_item_selected = event->payload.menu_index;
        switch(event->payload.menu_index) {
        case SubmenuLFTune:
            app->scene_controller.switch_to_next_scene(RfidDebugApp::SceneType::LFTuneScene);
            break;
        }
        consumed = true;
    }

    return consumed;
}

void RfidDebugAppSceneStart::on_exit(RfidDebugApp* app) {
    app->view_controller.get<SubmenuVM>()->clean();
    notification_message(app->notification, &sequence_blink_stop);
    furi_hal_nfc_field_off();
    app->worker.stop_read();
}

void RfidDebugAppSceneStart::submenu_callback(void* context, uint32_t index) {
    RfidDebugApp* app = static_cast<RfidDebugApp*>(context);

    if (index == SubmenuHFField || index == SubmenuLFField) {
        if (index == SubmenuHFField) {
            app->HF_field_enabled = !app->HF_field_enabled;
            if (app->HF_field_enabled) {
                furi_hal_nfc_field_on();
            } else {
                furi_hal_nfc_field_off();
            }
        } else if (index == SubmenuLFField) {
            app->LF_field_enabled = !app->LF_field_enabled;
            if (app->LF_field_enabled) {
                app->worker.start_read();
            } else {
                app->worker.stop_read();
            }
        }

        if (app->LF_field_enabled && app->HF_field_enabled) {
            notification_message(app->notification, &notification_both);
        } else if (app->LF_field_enabled || app->HF_field_enabled) {
            notification_message(app->notification, &notification_single);
        } else {
            notification_message(app->notification, &notification_neither);
        }

        draw_menu(app, index);
    }

     else {
        RfidDebugApp::Event event;

        event.type = RfidDebugApp::EventType::MenuSelected;
        event.payload.menu_index = index;

        app->view_controller.send_event(&event);
    }
}
