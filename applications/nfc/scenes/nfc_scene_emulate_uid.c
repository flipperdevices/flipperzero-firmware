#include "../nfc_i.h"

#define NFC_EMULATE_UID_WORKER_CUSTOM_EVENT (10UL)
#define NFC_EMULATE_UID_TEXTBOX_EVENT (11UL)

enum {
    NfcSceneEmulateUidStateWidget,
    NfcSceneEmulateUidStateTextBox,
};

void nfc_emulate_uid_worker_callback(void* context) {
    furi_assert(context);
    Nfc* nfc = context;
    view_dispatcher_send_custom_event(nfc->view_dispatcher, NFC_EMULATE_UID_WORKER_CUSTOM_EVENT);
}

void nfc_scene_emulate_uid_widget_callback(GuiButtonType result, InputType type, void* context) {
    furi_assert(context);
    Nfc* nfc = context;
    if(type == InputTypeShort) {
        view_dispatcher_send_custom_event(nfc->view_dispatcher, result);
    }
}

void nfc_emulate_uid_textbox_callback(void* context) {
    furi_assert(context);
    Nfc* nfc = context;
    view_dispatcher_send_custom_event(nfc->view_dispatcher, NFC_EMULATE_UID_TEXTBOX_EVENT);
}

// Add widget with device name or inform that data received
static void nfc_scene_emulate_uid_widget_config(Nfc* nfc, bool data_received) {
    NfcDeviceCommonData* data = &nfc->dev->dev_data.nfc_data;
    Widget* widget = nfc->widget;
    widget_clear(widget);
    string_t info_str;
    string_init(info_str);

    widget_add_icon_element(widget, 0, 3, &I_RFIDDolphinSend_97x61);
    widget_add_string_element(widget, 56, 32, AlignLeft, AlignTop, FontPrimary, "Emulating UID");
    if(data_received) {
        string_printf(info_str, "Received data");
        widget_add_button_element(
            widget, GuiButtonTypeCenter, "Data", nfc_scene_emulate_uid_widget_callback, nfc);
    } else if(strcmp(nfc->dev->dev_name, "")) {
        string_printf(info_str, "%s", nfc->dev->dev_name);
    } else {
        for(uint8_t i = 0; i < data->uid_len; i++) {
            string_cat_printf(info_str, "%02X ", data->uid[i]);
        }
    }
    string_strim(info_str);
    widget_add_text_box_element(
        widget, 56, 43, 70, 21, AlignLeft, AlignTop, string_get_cstr(info_str));
    string_clear(info_str);
}

void nfc_scene_emulate_uid_on_enter(void* context) {
    Nfc* nfc = context;

    // Setup Widget
    nfc_scene_emulate_uid_widget_config(nfc, false);
    // Setup TextBox
    TextBox* text_box = nfc->text_box;
    text_box_set_font(text_box, TextBoxFontHex);
    text_box_set_focus(text_box, TextBoxFocusEnd);
    string_reset(nfc->text_box_store);

    // Set Widget state and view
    scene_manager_set_scene_state(
        nfc->scene_manager, NfcSceneEmulateUid, NfcSceneEmulateUidStateWidget);
    view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewWidget);
    // Start worker
    nfc_worker_start(
        nfc->worker,
        NfcWorkerStateEmulate,
        &nfc->dev->dev_data,
        nfc_emulate_uid_worker_callback,
        nfc);
}

bool nfc_scene_emulate_uid_on_event(void* context, SceneManagerEvent event) {
    Nfc* nfc = context;
    NfcReaderRequestData* reader_data = &nfc->dev->dev_data.reader_data;
    uint32_t state = scene_manager_get_scene_state(nfc->scene_manager, NfcSceneEmulateUid);
    bool consumed = false;

    if(event.type == SceneManagerEventTypeTick) {
        notification_message(nfc->notifications, &sequence_blink_blue_10);
        consumed = true;
    } else if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == NFC_EMULATE_UID_WORKER_CUSTOM_EVENT) {
            // Add data button to widget if data is received for the first time
            if(!string_size(nfc->text_box_store)) {
                nfc_scene_emulate_uid_widget_config(nfc, true);
            }
            // Update TextBox data
            string_cat_printf(nfc->text_box_store, "R:");
            for(uint16_t i = 0; i < reader_data->size; i++) {
                string_cat_printf(nfc->text_box_store, " %02X", reader_data->data[i]);
            }
            string_push_back(nfc->text_box_store, '\n');
            text_box_set_text(nfc->text_box, string_get_cstr(nfc->text_box_store));
            consumed = true;
        } else if(event.event == GuiButtonTypeCenter && state == NfcSceneEmulateUidStateWidget) {
            view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewTextBox);
            scene_manager_set_scene_state(
                nfc->scene_manager, NfcSceneEmulateUid, NfcSceneEmulateUidStateTextBox);
            consumed = true;
        } else if(
            event.event == NFC_EMULATE_UID_TEXTBOX_EVENT &&
            state == NfcSceneEmulateUidStateTextBox) {
            view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewWidget);
            scene_manager_set_scene_state(
                nfc->scene_manager, NfcSceneEmulateUid, NfcSceneEmulateUidStateWidget);
            consumed = true;
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        if(state == NfcSceneEmulateUidStateTextBox) {
            view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewWidget);
            scene_manager_set_scene_state(
                nfc->scene_manager, NfcSceneEmulateUid, NfcSceneEmulateUidStateWidget);
            consumed = true;
        }
    }

    return consumed;
}

void nfc_scene_emulate_uid_on_exit(void* context) {
    Nfc* nfc = context;

    // Stop worker
    nfc_worker_stop(nfc->worker);

    // Clear view
    widget_clear(nfc->widget);
    text_box_clean(nfc->text_box);
    string_reset(nfc->text_box_store);
}
