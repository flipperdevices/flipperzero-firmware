#include "../nfc_i.h"

void nfc_scene_felica_service_data_on_enter(void* context) {
    Nfc* nfc = context;
    FelicaSelectState* select_state = &nfc->felica_select;
    FelicaSystem* system = select_state->selected_system;
    FelicaService* service = select_state->selected_service;
    TextBox* text_box = nfc->text_box;

    if(system->code == LITE_SYSTEM_CODE && service->number == 0) {
        text_box_set_font(text_box, TextBoxFontHex);
        furi_string_cat_str(nfc->text_box_store, "S_PAD:\n");
        for(int i = 0; i < REG_LITE_BLOCK; i++) {
            FelicaBlock* block = *FelicaBlockList_cget(service->blocks, i);
            for(uint16_t i = 0; i < FELICA_BLOCK_SIZE; i += 2) {
                if(!(i % 8) && i) {
                    furi_string_push_back(nfc->text_box_store, '\n');
                }
                if(block != NULL) {
                    furi_string_cat_printf(nfc->text_box_store, "%02X%02X ", block->data[i], block->data[i + 1]);
                } else {
                    furi_string_cat_printf(nfc->text_box_store, "???? ");
                }
            }
        }

        furi_string_cat_str(nfc->text_box_store, "REG:\n");
        FelicaBlock* block = *FelicaBlockList_cget(service->blocks, REG_LITE_BLOCK);
        for(uint16_t i = 0; i < FELICA_BLOCK_SIZE; i += 2) {
            if(!(i % 8) && i) {
                furi_string_push_back(nfc->text_box_store, '\n');
            }
            if(block != NULL) {
                furi_string_cat_printf(nfc->text_box_store, "%02X%02X ", block->data[i], block->data[i + 1]);
            } else {
                furi_string_cat_printf(nfc->text_box_store, "???? ");
            }
        }

        furi_string_cat_str(nfc->text_box_store, "RC:\n");
        block = *FelicaBlockList_cget(service->blocks, RC_LITE_BLOCK);
        for(uint16_t i = 0; i < FELICA_BLOCK_SIZE; i += 2) {
            if(!(i % 8) && i) {
                furi_string_push_back(nfc->text_box_store, '\n');
            }
            furi_string_cat_printf(nfc->text_box_store, "%02X%02X ", block->data[i], block->data[i + 1]);
        }

        furi_string_cat_str(nfc->text_box_store, "MAC:\n");
        block = *FelicaBlockList_cget(service->blocks, MAC_LITE_BLOCK);
        for(uint16_t i = 0; i < FELICA_BLOCK_SIZE; i += 2) {
            if(!(i % 8) && i) {
                furi_string_push_back(nfc->text_box_store, '\n');
            }
            furi_string_cat_printf(nfc->text_box_store, "%02X%02X ", block->data[i], block->data[i + 1]);
        }

        furi_string_cat_str(nfc->text_box_store, "ID:\n");
        block = *FelicaBlockList_cget(service->blocks, ID_LITE_BLOCK);
        for(uint16_t i = 0; i < FELICA_BLOCK_SIZE; i += 2) {
            if(!(i % 8) && i) {
                furi_string_push_back(nfc->text_box_store, '\n');
            }
            furi_string_cat_printf(nfc->text_box_store, "%02X%02X ", block->data[i], block->data[i + 1]);
        }

        furi_string_cat_str(nfc->text_box_store, "D_ID:\n");
        block = *FelicaBlockList_cget(service->blocks, DEVICE_ID_LITE_BLOCK);
        for(uint16_t i = 0; i < FELICA_BLOCK_SIZE; i += 2) {
            if(!(i % 8) && i) {
                furi_string_push_back(nfc->text_box_store, '\n');
            }
            furi_string_cat_printf(nfc->text_box_store, "%02X%02X ", block->data[i], block->data[i + 1]);
        }

        furi_string_cat_str(nfc->text_box_store, "CKV:\n");
        block = *FelicaBlockList_cget(service->blocks, CARD_KEY_VER_LITE_BLOCK);
        for(uint16_t i = 0; i < FELICA_BLOCK_SIZE; i += 2) {
            if(!(i % 8) && i) {
                furi_string_push_back(nfc->text_box_store, '\n');
            }
            furi_string_cat_printf(nfc->text_box_store, "%02X%02X ", block->data[i], block->data[i + 1]);
        }

        furi_string_cat_str(nfc->text_box_store, "MC:\n");
        block = *FelicaBlockList_cget(service->blocks, MEM_CONFIG_LITE_BLOCK);
        for(uint16_t i = 0; i < FELICA_BLOCK_SIZE; i += 2) {
            if(!(i % 8) && i) {
                furi_string_push_back(nfc->text_box_store, '\n');
            }
            furi_string_cat_printf(nfc->text_box_store, "%02X%02X ", block->data[i], block->data[i + 1]);
        }

        furi_string_cat_str(nfc->text_box_store, "WCNT:\n");
        block = *FelicaBlockList_cget(service->blocks, WRITE_COUNT_LITE_BLOCK);
        for(uint16_t i = 0; i < FELICA_BLOCK_SIZE; i += 2) {
            if(!(i % 8) && i) {
                furi_string_push_back(nfc->text_box_store, '\n');
            }
            furi_string_cat_printf(nfc->text_box_store, "%02X%02X ", block->data[i], block->data[i + 1]);
        }

        furi_string_cat_str(nfc->text_box_store, "MAC_A:\n");
        block = *FelicaBlockList_cget(service->blocks, MAC_A_LITE_BLOCK);
        for(uint16_t i = 0; i < FELICA_BLOCK_SIZE; i += 2) {
            if(!(i % 8) && i) {
                furi_string_push_back(nfc->text_box_store, '\n');
            }
            furi_string_cat_printf(nfc->text_box_store, "%02X%02X ", block->data[i], block->data[i + 1]);
        }

        furi_string_cat_str(nfc->text_box_store, "CRC_CHECK:\n");
        block = *FelicaBlockList_cget(service->blocks, CRC_CHECK_LITE_BLOCK);
        for(uint16_t i = 0; i < FELICA_BLOCK_SIZE; i += 2) {
            if(!(i % 8) && i) {
                furi_string_push_back(nfc->text_box_store, '\n');
            }
            furi_string_cat_printf(nfc->text_box_store, "%02X%02X ", block->data[i], block->data[i + 1]);
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