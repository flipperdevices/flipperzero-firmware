#include "../nfc_i.h"

void nfc_scene_felica_service_data_on_enter(void* context) {
    Nfc* nfc = context;
    FelicaSelectState* select_state = &nfc->felica_select;
    FelicaSystem* system = select_state->selected_system;
    //FelicaService* service = select_state->selected_service;
    TextBox* text_box = nfc->text_box;

    if(system->code == LITE_SYSTEM_CODE) {
        FelicaLiteInfo* lite_info = &system->lite_info;
        uint8_t* data;
        text_box_set_font(text_box, TextBoxFontHex);
        furi_string_cat_str(nfc->text_box_store, "S_PAD:\n");
        for(int i = 0; i < REG_LITE_BLOCK; i++) {
            data = lite_info->S_PAD[i];
            for(uint16_t i = 0; i < FELICA_BLOCK_SIZE; i += 2) {
                if(!(i % 8) && i) {
                    furi_string_push_back(nfc->text_box_store, '\n');
                }
                if(data != NULL) {
                    furi_string_cat_printf(nfc->text_box_store, "%02X%02X ", data[i], data[i + 1]);
                } else {
                    furi_string_cat_printf(nfc->text_box_store, "???? ");
                }
            }
        }

        furi_string_cat_str(nfc->text_box_store, "REG:\n");
        data = lite_info->REG;
        for(uint16_t i = 0; i < FELICA_BLOCK_SIZE; i += 2) {
            if(!(i % 8) && i) {
                furi_string_push_back(nfc->text_box_store, '\n');
            }
            if(data != NULL) {
                furi_string_cat_printf(nfc->text_box_store, "%02X%02X ", data[i], data[i + 1]);
            } else {
                furi_string_cat_printf(nfc->text_box_store, "???? ");
            }
        }

        furi_string_cat_str(nfc->text_box_store, "MAC:\n");
        data = lite_info->MAC;
        for(uint16_t i = 0; i < 8; i += 2) {
            if(!(i % 8) && i) {
                furi_string_push_back(nfc->text_box_store, '\n');
            }
            furi_string_cat_printf(nfc->text_box_store, "%02X%02X ", data[i], data[i + 1]);
        }

        furi_string_cat_printf(nfc->text_box_store, "DFC: %04X\n", lite_info->data_format_code);

        furi_string_cat_str(nfc->text_box_store, "ID data:\n");
        data = lite_info->ID_value;
        for(uint16_t i = 0; i < 6; i += 2) {
            furi_string_cat_printf(nfc->text_box_store, "%02X%02X ", data[i], data[i + 1]);
        }
        furi_string_cat_str(nfc->text_box_store, "\n");

        furi_string_cat_printf(nfc->text_box_store, "CKV: %04X\n", lite_info->card_key_version);

        furi_string_cat_str(nfc->text_box_store, "MC:\n");
        data = lite_info->memory_config;
        for(uint16_t i = 0; i < FELICA_BLOCK_SIZE; i += 2) {
            if(!(i % 8) && i) {
                furi_string_push_back(nfc->text_box_store, '\n');
            }
            furi_string_cat_printf(nfc->text_box_store, "%02X%02X ", data[i], data[i + 1]);
        }

        furi_string_cat_printf(nfc->text_box_store, "WCNT: %06lX\n", lite_info->write_count);

        furi_string_cat_str(nfc->text_box_store, "MAC_A:\n");
        data = lite_info->MAC_A;
        for(uint16_t i = 0; i < 8; i += 2) {
            if(!(i % 8) && i) {
                furi_string_push_back(nfc->text_box_store, '\n');
            }
            furi_string_cat_printf(nfc->text_box_store, "%02X%02X ", data[i], data[i + 1]);
        }
    }
    text_box_set_text(text_box, furi_string_get_cstr(nfc->text_box_store));

    view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewTextBox);
}

bool nfc_scene_felica_service_data_on_event(void* context, SceneManagerEvent event) {
    Nfc* nfc = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeBack) {
        consumed = scene_manager_previous_scene(nfc->scene_manager);
    }

    return consumed;
}

void nfc_scene_felica_service_data_on_exit(void* context) {
    Nfc* nfc = context;

    // Clean view
    text_box_reset(nfc->text_box);
    furi_string_reset(nfc->text_box_store);
}