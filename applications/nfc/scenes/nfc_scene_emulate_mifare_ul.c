#include "../nfc_i.h"
#include <lib/nfc_protocols/mifare_ultralight.h>
#include <dolphin/dolphin.h>

#define NFC_SCENE_EMULATE_MIFARE_UL_LOG_SIZE_MAX (200)

enum {
    // View states
    NfcSceneEmulateMifareUlStateWidget,
    NfcSceneEmulateMifareUlStateTextBox,
    NfcSceneEmulateMifareUlStateMax = 0xFF,
    // State flags
    NfcSceneEmulateMifareUlStateDataChanged = 1 << 8,
    NfcSceneEmulateMifareUlStateAuthAttempted = 1 << 9,
    NfcSceneEmulateMifareUlStateLogButtonShown = 1 << 10,
};

void nfc_emulate_mifare_ul_worker_callback(NfcWorkerEvent event, void* context) {
    UNUSED(event);
    Nfc* nfc = context;

    uint32_t state = scene_manager_get_scene_state(nfc->scene_manager, NfcSceneEmulateMifareUl);
    if(event == NfcWorkerEventSuccess)
        state |= NfcSceneEmulateMifareUlStateDataChanged;
    else if(event == NfcWorkerEventPwdAuth) {
        // Don't update if we're exiting
        if(nfc_worker_get_state(nfc->worker) != NfcWorkerStateStop) {
            // Event data is only available for the duration of this callback, so we're updating the
            // text box right here
            MfUltralightAuth* auth = nfc_worker_get_event_data(nfc->worker);
            if(auth != NULL &&
               string_size(nfc->text_box_store) < NFC_SCENE_EMULATE_MIFARE_UL_LOG_SIZE_MAX) {
                string_cat(nfc->text_box_store, "PWD:");
                for(size_t i = 0; i < sizeof(auth->pwd.raw); ++i) {
                    string_cat_printf(nfc->text_box_store, " %02X", auth->pwd.raw[i]);
                }
                string_push_back(nfc->text_box_store, '\n');
                text_box_set_text(nfc->text_box, string_get_cstr(nfc->text_box_store));
            }
            state |= NfcSceneEmulateMifareUlStateAuthAttempted;
        }
    }
    scene_manager_set_scene_state(nfc->scene_manager, NfcSceneEmulateMifareUl, state);
    view_dispatcher_send_custom_event(nfc->view_dispatcher, NfcCustomEventWorkerExit);
}

void nfc_scene_emulate_mifare_ul_widget_callback(
    GuiButtonType result,
    InputType type,
    void* context) {
    furi_assert(context);
    Nfc* nfc = context;
    if(type == InputTypeShort) {
        view_dispatcher_send_custom_event(nfc->view_dispatcher, result);
    }
}

void nfc_scene_emulate_mifare_ul_widget_config(Nfc* nfc, bool auth_attempted) {
    Widget* widget = nfc->widget;
    widget_reset(widget);

    widget_add_icon_element(widget, 0, 3, &I_RFIDDolphinSend_97x61);
    widget_add_string_multiline_element(
        widget, 56, 31, AlignLeft, AlignTop, FontPrimary, "Emulating\nMf Ultralight");

    if(auth_attempted) {
        widget_add_button_element(
            widget, GuiButtonTypeCenter, "Log", nfc_scene_emulate_mifare_ul_widget_callback, nfc);
    }
}

void nfc_scene_emulate_mifare_ul_on_enter(void* context) {
    Nfc* nfc = context;
    uint32_t state = scene_manager_get_scene_state(nfc->scene_manager, NfcSceneEmulateMifareUl);
    DOLPHIN_DEED(DolphinDeedNfcEmulate);

    // Setup Widget
    nfc_scene_emulate_mifare_ul_widget_config(nfc, false);
    state &= ~NfcSceneEmulateMifareUlStateLogButtonShown;
    // Setup TextBox
    TextBox* text_box = nfc->text_box;
    text_box_set_font(text_box, TextBoxFontHex);
    text_box_set_focus(text_box, TextBoxFocusEnd);
    string_reset(nfc->text_box_store);
    
    if(strcmp(nfc->dev->dev_name, "")) {
        nfc_text_store_set(nfc, "     %s", nfc->dev->dev_name);
    }
    popup_set_text(popup, nfc->text_store, 30, 56, AlignLeft, AlignTop);
    
    // Set Widget state and view
    state = (state & ~NfcSceneEmulateMifareUlStateMax) | NfcSceneEmulateMifareUlStateWidget;
    scene_manager_set_scene_state(nfc->scene_manager, NfcSceneEmulateMifareUl, state);
    view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewWidget);
    // Start worker
    nfc_worker_start(
        nfc->worker,
        NfcWorkerStateEmulateMifareUltralight,
        &nfc->dev->dev_data,
        nfc_emulate_mifare_ul_worker_callback,
        nfc);
    nfc_blink_start(nfc);
}

bool nfc_scene_emulate_mifare_ul_on_event(void* context, SceneManagerEvent event) {
    Nfc* nfc = context;
    uint32_t state = scene_manager_get_scene_state(nfc->scene_manager, NfcSceneEmulateMifareUl);
    bool consumed = false;

    if(event.type == SceneManagerEventTypeTick) {
        consumed = true;
    } else if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == NfcCustomEventWorkerExit) {
            if(state & NfcSceneEmulateMifareUlStateAuthAttempted) {
                if(!(state & NfcSceneEmulateMifareUlStateLogButtonShown)) {
                    // Add log button to widget not already showing
                    nfc_scene_emulate_mifare_ul_widget_config(nfc, true);
                    state |= NfcSceneEmulateMifareUlStateLogButtonShown;
                }
                // The text box update logic is handled in the worker callback
                state &= ~NfcSceneEmulateMifareUlStateAuthAttempted;
                scene_manager_set_scene_state(nfc->scene_manager, NfcSceneEmulateUid, state);
                consumed = true;
            }
        } else if(
            event.event == GuiButtonTypeCenter &&
            (state & NfcSceneEmulateMifareUlStateMax) == NfcSceneEmulateMifareUlStateWidget) {
            view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewTextBox);
            state = (state & ~NfcSceneEmulateMifareUlStateMax) |
                    NfcSceneEmulateMifareUlStateTextBox;
            scene_manager_set_scene_state(nfc->scene_manager, NfcSceneEmulateUid, state);
            consumed = true;
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        if((state & NfcSceneEmulateMifareUlStateMax) == NfcSceneEmulateMifareUlStateTextBox) {
            view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewWidget);
            state = (state & ~NfcSceneEmulateMifareUlStateMax) |
                    NfcSceneEmulateMifareUlStateWidget;
            scene_manager_set_scene_state(nfc->scene_manager, NfcSceneEmulateUid, state);
            consumed = true;
        }
    }
    return consumed;
}

void nfc_scene_emulate_mifare_ul_on_exit(void* context) {
    Nfc* nfc = context;
    uint32_t state = scene_manager_get_scene_state(nfc->scene_manager, NfcSceneEmulateMifareUl);

    // Stop worker
    nfc_worker_stop(nfc->worker);
    // Check if data changed and save in shadow file
    if(state & NfcSceneEmulateMifareUlStateDataChanged) {
        state &= ~NfcSceneEmulateMifareUlStateDataChanged;
        scene_manager_set_scene_state(nfc->scene_manager, NfcSceneEmulateMifareUl, state);
        nfc_device_save_shadow(nfc->dev, nfc->dev->dev_name);
    }

    // Clear view
    widget_reset(nfc->widget);
    text_box_reset(nfc->text_box);
    string_reset(nfc->text_box_store);

    nfc_blink_stop(nfc);
}
