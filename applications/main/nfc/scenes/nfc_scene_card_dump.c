#include <furi.h>

#include "../nfc_app_i.h"
#include "../helpers/protocol_support/nfc_protocol_support.h"

void nfc_scene_card_dump_on_enter(void* context) {
    NfcApp* app = context;

    const NfcDevice* device = app->nfc_device;
    const MfUltralightData* mfu = nfc_device_get_data(device, NfcProtocolMfUltralight);

    furi_string_reset(app->text_box_store);

    for(size_t i = 0; i < mfu->pages_read; i++) {
        const uint8_t* page_data = mfu->page[i].data;
        for(size_t j = 0; j < MF_ULTRALIGHT_PAGE_SIZE; j += 2) {
            furi_string_cat_printf(
                app->text_box_store, "%02X%02X ", page_data[j], page_data[j + 1]);
        }
    }

    text_box_set_font(app->text_box, TextBoxFontHex);
    text_box_set_text(app->text_box, furi_string_get_cstr(app->text_box_store));
    view_dispatcher_switch_to_view(app->view_dispatcher, NfcViewTextBox);
}

bool nfc_scene_card_dump_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void nfc_scene_card_dump_on_exit(void* context) {
    NfcApp* app = context;

    furi_string_reset(app->text_box_store);
}