#include "../nfc_i.h"
#include <dolphin/dolphin.h>

#define NFC_SCENE_READ_FAIL_SHIFT_ERROR "                "
#define NFC_SCENE_READ_FAIL_SHIFT_UID "              "

enum {
    ReadMifareUlStateShowError,
    ReadMifareUlStateShowUID,
    ReadMifareUlStateShowData,
    ReadMifareUlStateShowMenu,
};

enum SubmenuIndex {
    SubmenuIndexData,
    SubmenuIndexSave,
    SubmenuIndexEmulate,
};

void nfc_scene_read_mifare_ul_fail_dialog_callback(DialogExResult result, void* context) {
    Nfc* nfc = context;

    view_dispatcher_send_custom_event(nfc->view_dispatcher, result);
}

void nfc_scene_read_mifare_ul_fail_submenu_callback(void* context, uint32_t index) {
    Nfc* nfc = context;

    view_dispatcher_send_custom_event(nfc->view_dispatcher, index);
}

static void load_error_dialog(void* context) {
    Nfc* nfc = context;
    // MfUltralightData* mf_ul_data = &nfc->dev->dev_data.mf_ul_data;
    DialogEx* dialog_ex = nfc->dialog_ex;

    dialog_ex_set_center_button_text(dialog_ex, "Info");
    dialog_ex_set_header(dialog_ex, "Failed to read tag!", 64, 8, AlignCenter, AlignCenter);
    dialog_ex_set_icon(dialog_ex, 4, 13, &I_Warning_30x23);
    nfc_text_store_set(
        nfc,
        NFC_SCENE_READ_FAIL_SHIFT_ERROR "Some data could\n" NFC_SCENE_READ_FAIL_SHIFT_ERROR
                                        "not be read.\n"
                                        "Failed to read pages %i - %i",
        99,
        99); // TODO: Show actual page numbers
    dialog_ex_set_text(dialog_ex, nfc->text_store, 4, 17, AlignLeft, AlignTop);
}

static void load_uid_dialog(void* context) {
    Nfc* nfc = context;
    FuriHalNfcDevData* data = &nfc->dev->dev_data.nfc_data;
    MfUltralightData* mf_ul_data = &nfc->dev->dev_data.mf_ul_data;
    DialogEx* dialog_ex = nfc->dialog_ex;

    dialog_ex_set_center_button_text(dialog_ex, "Error");
    dialog_ex_set_header(
        dialog_ex, nfc_mf_ul_type(mf_ul_data->type, true), 64, 8, AlignCenter, AlignCenter);
    dialog_ex_set_icon(dialog_ex, 8, 13, &I_Medium_chip_22x21);
    // Display UID
    nfc_text_store_set(
        nfc,
        NFC_SCENE_READ_FAIL_SHIFT_UID "ATQA: %02X%02X\n" NFC_SCENE_READ_FAIL_SHIFT_UID
                                      "SAK: %02X\nUID: %02X %02X %02X %02X %02X %02X %02X",
        data->atqa[0],
        data->atqa[1],
        data->sak,
        data->uid[0],
        data->uid[1],
        data->uid[2],
        data->uid[3],
        data->uid[4],
        data->uid[5],
        data->uid[6]);
    dialog_ex_set_text(dialog_ex, nfc->text_store, 8, 16, AlignLeft, AlignTop);
}

void nfc_scene_read_mifare_ul_fail_on_enter(void* context) {
    Nfc* nfc = context;
    DOLPHIN_DEED(DolphinDeedNfcReadSuccess);

    // Send notification
    notification_message(nfc->notifications, &sequence_error);

    // Setup dialog view
    load_error_dialog(nfc);
    dialog_ex_set_left_button_text(nfc->dialog_ex, "Retry");
    dialog_ex_set_right_button_text(nfc->dialog_ex, "More");
    dialog_ex_set_context(nfc->dialog_ex, nfc);
    dialog_ex_set_result_callback(nfc->dialog_ex, nfc_scene_read_mifare_ul_fail_dialog_callback);

    // Setup TextBox view
    TextBox* text_box = nfc->text_box;
    MfUltralightData* mf_ul_data = &nfc->dev->dev_data.mf_ul_data;
    text_box_set_font(text_box, TextBoxFontHex);
    for(uint16_t i = 0; i < mf_ul_data->data_size; i += 2) {
        if(!(i % 8) && i) {
            string_push_back(nfc->text_box_store, '\n');
        }
        string_cat_printf(
            nfc->text_box_store, "%02X%02X ", mf_ul_data->data[i], mf_ul_data->data[i + 1]);
    }
    text_box_set_text(text_box, string_get_cstr(nfc->text_box_store));

    // Setup Submenu view
    Submenu* submenu = nfc->submenu;
    submenu_add_item(
        submenu,
        "View data",
        SubmenuIndexData,
        nfc_scene_read_mifare_ul_fail_submenu_callback,
        nfc);
    submenu_add_item(
        submenu, "Save", SubmenuIndexSave, nfc_scene_read_mifare_ul_fail_submenu_callback, nfc);
    submenu_add_item(
        submenu,
        "Emulate",
        SubmenuIndexEmulate,
        nfc_scene_read_mifare_ul_fail_submenu_callback,
        nfc);
    submenu_set_selected_item(nfc->submenu, 0);

    scene_manager_set_scene_state(
        nfc->scene_manager, NfcSceneReadMifareUlFail, ReadMifareUlStateShowError);
    view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewDialogEx);
}

bool nfc_scene_read_mifare_ul_fail_on_event(void* context, SceneManagerEvent event) {
    Nfc* nfc = context;
    bool consumed = false;
    uint32_t state = scene_manager_get_scene_state(nfc->scene_manager, NfcSceneReadMifareUlFail);

    if(state == ReadMifareUlStateShowError || state == ReadMifareUlStateShowUID) {
        if(event.type == SceneManagerEventTypeCustom) {
            if(event.event == DialogExResultLeft) {
                // Return to auto read if possible, only if we didn't come from script select.
                if(scene_manager_has_previous_scene(nfc->scene_manager, NfcSceneScriptsMenu) ||
                   scene_manager_has_previous_scene(nfc->scene_manager, NfcSceneCardMenu)) {
                    scene_manager_previous_scene(nfc->scene_manager);
                } else {
                    if(!scene_manager_search_and_switch_to_previous_scene(
                           nfc->scene_manager, NfcSceneReadCard)) {
                        scene_manager_previous_scene(nfc->scene_manager);
                    }
                }
                consumed = true;
            } else if(event.event == DialogExResultRight) {
                view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewMenu);
                scene_manager_set_scene_state(
                    nfc->scene_manager, NfcSceneReadMifareUlFail, ReadMifareUlStateShowMenu);
                consumed = true;
            } else if(event.event == DialogExResultCenter) {
                if(state == ReadMifareUlStateShowError) { // Go to UID
                    load_uid_dialog(nfc);
                    scene_manager_set_scene_state(
                        nfc->scene_manager, NfcSceneReadMifareUlFail, ReadMifareUlStateShowUID);
                    consumed = true;
                } else { // Go to error
                    load_error_dialog(nfc);
                    scene_manager_set_scene_state(
                        nfc->scene_manager, NfcSceneReadMifareUlFail, ReadMifareUlStateShowError);
                    consumed = true;
                }
            }
        } else if(event.type == SceneManagerEventTypeBack) {
            // UID back press
            if(!scene_manager_search_and_switch_to_previous_scene(
                   nfc->scene_manager, NfcSceneCardMenu) &&
               !scene_manager_search_and_switch_to_previous_scene(
                   nfc->scene_manager, NfcSceneScriptsMenu) &&
               !scene_manager_search_and_switch_to_previous_scene(
                   nfc->scene_manager, NfcSceneStart)) {
                scene_manager_previous_scene(nfc->scene_manager);
            }
            consumed = true;
        }
    } else if(state == ReadMifareUlStateShowData) {
        if(event.type == SceneManagerEventTypeBack) {
            view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewDialogEx);
            load_error_dialog(nfc);
            scene_manager_set_scene_state(
                nfc->scene_manager, NfcSceneReadMifareUlFail, ReadMifareUlStateShowError);
            consumed = true;
        }
    } else if(state == ReadMifareUlStateShowMenu) {
        if(event.type == SceneManagerEventTypeCustom) {
            if(event.event == SubmenuIndexSave) {
                nfc->dev->format = NfcDeviceSaveFormatMifareUl;
                // Clear device name
                nfc_device_set_name(nfc->dev, "");
                scene_manager_next_scene(nfc->scene_manager, NfcSceneSaveName);
                consumed = true;
            } else if(event.event == SubmenuIndexEmulate) {
                scene_manager_next_scene(nfc->scene_manager, NfcSceneEmulateMifareUl);
                consumed = true;
            } else if(event.event == SubmenuIndexData) {
                view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewTextBox);
                scene_manager_set_scene_state(
                    nfc->scene_manager, NfcSceneReadMifareUlFail, ReadMifareUlStateShowData);
                consumed = true;
            }
        } else if(event.type == SceneManagerEventTypeBack) {
            view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewDialogEx);
            load_error_dialog(nfc);
            scene_manager_set_scene_state(
                nfc->scene_manager, NfcSceneReadMifareUlFail, ReadMifareUlStateShowError);
            consumed = true;
        }
    }

    return consumed;
}

void nfc_scene_read_mifare_ul_fail_on_exit(void* context) {
    Nfc* nfc = context;

    // Clean views
    dialog_ex_reset(nfc->dialog_ex);
    text_box_reset(nfc->text_box);
    string_reset(nfc->text_box_store);
    submenu_reset(nfc->submenu);
}
