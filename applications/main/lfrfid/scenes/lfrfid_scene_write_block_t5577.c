#include "../lfrfid_i.h"

static void lfrfid_write_block_t5577_password_and_config_to_EM(LfRfid* app) {
    Popup* popup = app->popup;
    char curr_buf[50] = {};
    //char curr_buf[32] = {};
    //TODO: use .txt file in resources for passwords.

    uint8_t* pass_p = app->password;
    uint32_t current_password = pass_p[0] << 24 | pass_p[1] << 16 | pass_p[2] << 8 | pass_p[3];
    //const uint32_t em_pw_block_data = 1976;
    //uint32_t new_password = app->new_password;
    uint8_t page = app->write_page;
    uint8_t block = app->write_block;
    uint8_t* data_p = app->new_key_data;
    uint32_t data = data_p[0] << 24 | data_p[1] << 16 | data_p[2] << 8 | data_p[3];

    popup_set_header(popup, "Writing\nblock", 102, 10, AlignCenter, AlignCenter);
    //popup_set_icon(popup, 0, 3, &I_RFIDDolphinSend_97x61);
    popup_set_text(popup, curr_buf, /*92*/ 66, 33, AlignCenter, AlignCenter);
    snprintf(
        curr_buf,
        sizeof(curr_buf),
        "Page %u  Block %u\nHex: %02X %02X %02X %02X\nDec: %lu",
        page,
        block,
        data_p[0],
        data_p[1],
        data_p[2],
        data_p[3],
        data);
    view_dispatcher_switch_to_view(app->view_dispatcher, LfRfidViewPopup);
    // DEBUG delay
    furi_delay_ms(3000);
    notification_message(app->notifications, &sequence_blink_start_magenta);
    furi_delay_ms(500);
    bool lock_bit = app->extra_options & LfRfidWriteBlockLockBit;
    bool use_password = app->extra_options & LfRfidUsePassword;

    view_dispatcher_switch_to_view(app->view_dispatcher, LfRfidViewPopup);
    if(use_password)
        t5577_write_page_block_pass_with_start_and_stop(
            page, block, lock_bit, data, true, current_password /*, false*/);
    else
        t5577_write_page_block_simple_with_start_and_stop(page, block, lock_bit, data /*, false*/);
    furi_delay_ms(8);

    notification_message(app->notifications, &sequence_blink_stop);
    popup_reset(app->popup);
}

void lfrfid_scene_write_block_t5577_on_enter(void* context) {
    LfRfid* app = context;
    Popup* popup = app->popup;

    lfrfid_write_block_t5577_password_and_config_to_EM(app);

    notification_message(app->notifications, &sequence_success);
    popup_set_header(popup, "Done!", 102, 10, AlignCenter, AlignTop);
    popup_set_icon(popup, 0, 7, &I_RFIDDolphinSuccess_108x57);
    popup_set_context(popup, app);
    popup_set_callback(popup, lfrfid_popup_timeout_callback);
    popup_set_timeout(popup, 1500);
    popup_enable_timeout(popup);

    view_dispatcher_switch_to_view(app->view_dispatcher, LfRfidViewPopup);
    notification_message_block(app->notifications, &sequence_set_green_255);
}

bool lfrfid_scene_write_block_t5577_on_event(void* context, SceneManagerEvent event) {
    LfRfid* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeBack) {
        consumed = true; // Ignore Back button presses
    } else if(event.type == SceneManagerEventTypeCustom && event.event == LfRfidEventPopupClosed) {
        scene_manager_search_and_switch_to_previous_scene(
            app->scene_manager, LfRfidSceneExtraActions);
        consumed = true;
    }
    return consumed;
}

void lfrfid_scene_write_block_t5577_on_exit(void* context) {
    LfRfid* app = context;
    popup_reset(app->popup);
    notification_message_block(app->notifications, &sequence_reset_green);
}
