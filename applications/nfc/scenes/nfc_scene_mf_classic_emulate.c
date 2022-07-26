#include "../nfc_i.h"
#include <dolphin/dolphin.h>

enum {
    NfcSceneMfClassicEmulateStateWidget,
    NfcSceneMfClassicEmulateStateTextBox,
};

bool nfc_mf_classic_emulate_worker_callback(NfcWorkerEvent event, void* context) {
    UNUSED(event);
    Nfc* nfc = context;

    view_dispatcher_send_custom_event(nfc->view_dispatcher, NfcCustomEventWorkerExit);
    return true;
}

static void nfc_scene_mf_classic_emulate_widget_callback(
    GuiButtonType result,
    InputType type,
    void* context
) {
    furi_assert(context);
    Nfc* nfc = context;
    if(type == InputTypeShort) {
        view_dispatcher_send_custom_event(nfc->view_dispatcher, result);
    }
}

// Add widget with device name or inform that data received
static void nfc_scene_mf_classic_emulate_widget_config(
    Nfc* nfc,
    bool has_data
) {
    Widget* widget = nfc->widget;
    widget_reset(widget);

    widget_add_icon_element(widget, 0, 3, &I_RFIDDolphinSend_97x61);
    if(strcmp(nfc->dev->dev_name, "")) {
        nfc_text_store_set(nfc, "Emulating\n%s", nfc->dev->dev_name);
    } else {
        nfc_text_store_set(nfc, "Emulating\nMf Classic");
    }
    widget_add_string_multiline_element(
        widget,
        56,
        31,
        AlignLeft,
        AlignTop,
        FontPrimary,
        nfc->text_store);

    if(has_data) {
        widget_add_button_element(
            widget, GuiButtonTypeLeft, "Log", nfc_scene_mf_classic_emulate_widget_callback, nfc);
        widget_add_button_element(
            widget, GuiButtonTypeRight, "Save", nfc_scene_mf_classic_emulate_widget_callback, nfc);
    }
}

void nfc_scene_mf_classic_emulate_on_enter(void* context) {
    Nfc* nfc = context;
    DOLPHIN_DEED(DolphinDeedNfcEmulate);

    // Setup view
    nfc_scene_mf_classic_emulate_widget_config(nfc, false);

    // Setup and start worker
    view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewWidget);

    TextBox* text_box = nfc->text_box;
    text_box_set_font(text_box, TextBoxFontHex);
    text_box_set_focus(text_box, TextBoxFocusEnd);
    string_reset(nfc->text_box_store);

    string_init(nfc->dev->dev_data.mf_classic_emulator_output.nonce_log);
    nfc->dev->dev_data.mf_classic_emulator_output.data_changed = false;

    nfc_worker_start(
        nfc->worker,
        NfcWorkerStateMfClassicEmulate,
        &nfc->dev->dev_data,
        nfc_mf_classic_emulate_worker_callback,
        nfc);
    nfc_blink_start(nfc);
}

bool nfc_scene_mf_classic_emulate_on_event(void* context, SceneManagerEvent event) {
    Nfc* nfc = context;
    bool consumed = false;
    NfcMfClassicEmulatorOutput* emulator_output =
        &nfc->dev->dev_data.mf_classic_emulator_output;
    uint32_t state = scene_manager_get_scene_state(nfc->scene_manager, NfcSceneMfClassicEmulate);

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == NfcCustomEventWorkerExit) {
            consumed = true;
            if(string_size(emulator_output->nonce_log)) {
                // Add data button to widget if data is received for the first time
                if(!string_size(nfc->text_box_store)) {
                    nfc_scene_mf_classic_emulate_widget_config(nfc, true);
                }

                // TODO(indutny): worry about concurrency?

                // Copy and display log
                string_set(nfc->text_box_store, emulator_output->nonce_log);
                string_strim(nfc->text_box_store);
                text_box_set_text(nfc->text_box, string_get_cstr(nfc->text_box_store));
            }
        } else if(event.event == GuiButtonTypeLeft && state == NfcSceneMfClassicEmulateStateWidget) {
            view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewTextBox);
            scene_manager_set_scene_state(
                nfc->scene_manager, NfcSceneMfClassicEmulate, NfcSceneMfClassicEmulateStateTextBox);
            consumed = true;
        } else if(event.event == GuiButtonTypeRight && state == NfcSceneMfClassicEmulateStateWidget) {
            // Stop worker without deallocating the nonce log
            nfc_worker_stop(nfc->worker);

            // Check if data changed and save in shadow file
            if(emulator_output->data_changed) {
                nfc_device_save_shadow(nfc->dev, nfc->dev->dev_name);
            }

            // Enter next scene
            scene_manager_next_scene(nfc->scene_manager, NfcSceneSaveName);
            consumed = true;
        } else if(event.event == NfcCustomEventViewExit && state == NfcSceneMfClassicEmulateStateTextBox) {
            view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewWidget);
            scene_manager_set_scene_state(
                nfc->scene_manager, NfcSceneMfClassicEmulate, NfcSceneMfClassicEmulateStateWidget);
            consumed = true;
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        // Close TextBox
        if(state == NfcSceneMfClassicEmulateStateTextBox) {
            view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewWidget);
            scene_manager_set_scene_state(
                nfc->scene_manager, NfcSceneMfClassicEmulate, NfcSceneMfClassicEmulateStateWidget);
            consumed = true;
        } else {
            // Stop worker
            nfc_worker_stop(nfc->worker);
            string_clear(emulator_output->nonce_log);

            // Check if data changed and save in shadow file
            if(emulator_output->data_changed) {
                nfc_device_save_shadow(nfc->dev, nfc->dev->dev_name);
            }
        }
    }

    return consumed;
}

void nfc_scene_mf_classic_emulate_on_exit(void* context) {
    Nfc* nfc = context;

    // Clear view
    widget_reset(nfc->widget);
    string_reset(nfc->text_box_store);

    nfc_blink_stop(nfc);
}
