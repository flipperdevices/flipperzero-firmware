#include "../nfc_maker.h"

enum PopupEvent {
    PopupEventExit,
};

static void nfc_maker_scene_save_result_popup_callback(void* context) {
    NfcMaker* app = context;

    view_dispatcher_send_custom_event(app->view_dispatcher, PopupEventExit);
}

void nfc_maker_scene_save_result_on_enter(void* context) {
    NfcMaker* app = context;
    Popup* popup = app->popup;

    FuriString* path =
        furi_string_alloc_printf(NFC_MK_APP_FOLDER "/%s" NFC_MK_APP_EXTENSION, app->save_buf);
    bool success = nfc_device_save(app->nfc_device, furi_string_get_cstr(path));
    furi_string_free(path);

    if(success) {
        popup_set_icon(popup, 36, 5, &I_DolphinDone_80x58);
        popup_set_header(popup, "Saved!", 13, 22, AlignLeft, AlignBottom);
    } else {
        popup_set_icon(popup, 69, 15, &I_WarningDolphinFlip_45x42);
        popup_set_header(popup, "Error!", 13, 22, AlignLeft, AlignBottom);
    }
    popup_set_timeout(popup, 1500);
    popup_set_context(popup, app);
    popup_set_callback(popup, nfc_maker_scene_save_result_popup_callback);
    popup_enable_timeout(popup);

    view_dispatcher_switch_to_view(app->view_dispatcher, NfcMakerViewPopup);
}

bool nfc_maker_scene_save_result_on_event(void* context, SceneManagerEvent event) {
    NfcMaker* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        consumed = true;
        switch(event.event) {
        case PopupEventExit:
            scene_manager_search_and_switch_to_previous_scene(
                app->scene_manager, NfcMakerSceneStart);
            break;
        default:
            break;
        }
    }

    return consumed;
}

void nfc_maker_scene_save_result_on_exit(void* context) {
    NfcMaker* app = context;
    popup_reset(app->popup);
}
