#include "../nfc_i.h"
#include <dolphin/dolphin.h>

void nfc_scene_passport_read_auth_widget_callback(GuiButtonType result, InputType type, void* context) {
    Nfc* nfc = context;
    if(type == InputTypeShort) {
        view_dispatcher_send_custom_event(nfc->view_dispatcher, result);
    }
}

void nfc_scene_passport_read_auth_on_enter(void* context) {
    Nfc* nfc = context;
    MrtdData* mrtd_data = &nfc->dev->dev_data.mrtd_data;

    Widget* widget = nfc->widget;

    // Setup Custom Widget view
    string_t temp_str;
    string_init_printf(temp_str, "\e#Passport\n");
    string_cat_printf(temp_str, "Authenticated: %d\n", mrtd_data->auth_success);
    // TODO: indicate BAC / PACE used

    uint16_t lds_version = mrtd_data->files.EF_COM.lds_version;
    string_cat_printf(temp_str, "LDS version: %d.%d\n", lds_version/100, lds_version%100);

    uint32_t unicode_version = mrtd_data->files.EF_COM.unicode_version;
    string_cat_printf(temp_str, "Unicode version: %d.%d.%d\n", unicode_version/10000, unicode_version/100%100, unicode_version%100);

    string_cat_printf(temp_str, "Avail.files: ");
    for(size_t i=0; i<MAX_EFCOM_TAGS; ++i) {
        uint8_t tag = mrtd_data->files.EF_COM.tag_list[i];
        const EFFile* file = mrtd_tag_to_file(tag);
        if(file->tag) {
            if(i > 0) string_cat_printf(temp_str, ", ");
            string_cat_printf(temp_str, "%s", file->name);
        }
    }
    string_cat_printf(temp_str, "\n");

    EF_DIR_contents* EF_DIR = &mrtd_data->files.EF_DIR;
    if(EF_DIR->applications_count > 0) {
        string_cat_printf(temp_str, "Apps:\n");
        for(uint8_t i=0; i<EF_DIR->applications_count; ++i) {
            for(uint8_t n=0; n<sizeof(AIDValue); ++n) {
                string_cat_printf(temp_str, "%02X ", EF_DIR->applications[i][n]);
            }
            string_cat_printf(temp_str, "\n");
        }
    }

    /*
    char iso_type = FURI_BIT(data->sak, 5) ? '4' : '3';
    //TODO: NFC-B?
    string_cat_printf(temp_str, "ISO 14443-%c (NFC-A)\n", iso_type);
    string_cat_printf(temp_str, "UID:");
    for(size_t i = 0; i < data->uid_len; i++) {
        string_cat_printf(temp_str, " %02X", data->uid[i]);
    }
    string_cat_printf(temp_str, "\nATQA: %02X %02X ", data->atqa[1], data->atqa[0]);
    string_cat_printf(temp_str, " SAK: %02X", data->sak);
    */

    widget_add_text_scroll_element(widget, 0, 0, 128, 52, string_get_cstr(temp_str));
    string_clear(temp_str);

    widget_add_button_element(
        nfc->widget, GuiButtonTypeLeft, "Retry", nfc_scene_passport_read_auth_widget_callback, nfc);
    /*
    widget_add_button_element(
        nfc->widget, GuiButtonTypeCenter, "Auth", nfc_scene_passport_read_auth_widget_callback, nfc);
    widget_add_button_element(
        nfc->widget, GuiButtonTypeRight, "More", nfc_scene_passport_read_auth_widget_callback, nfc);
    */

    view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewWidget);
}

bool nfc_scene_passport_read_auth_on_event(void* context, SceneManagerEvent event) {
    Nfc* nfc = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == GuiButtonTypeLeft) {
            scene_manager_next_scene(nfc->scene_manager, NfcSceneRetryConfirm);
            consumed = true;
        } else if(event.event == GuiButtonTypeCenter) {
            //scene_manager_next_scene(nfc->scene_manager, NfcScenePassportAuth);
            //consumed = true;
        } else if(event.event == GuiButtonTypeRight) {
            //scene_manager_next_scene(nfc->scene_manager, NfcScenePassportMenu);
            //consumed = true;
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        scene_manager_next_scene(nfc->scene_manager, NfcSceneExitConfirm);
        consumed = true;
    }
    return consumed;
}

void nfc_scene_passport_read_auth_on_exit(void* context) {
    Nfc* nfc = context;

    // Clear view
    widget_reset(nfc->widget);
}
