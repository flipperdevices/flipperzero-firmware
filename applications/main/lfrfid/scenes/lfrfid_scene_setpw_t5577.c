#include "../lfrfid_i.h"
#include "../helpers/lfrfid_dialog.h"

#define LFRFID_T5577_BITRATE_RF_64 0x00140000
#define LFRFID_T5577_MODULATION_MANCHESTER 0x00008000
#define LFRFID_T5577_PWD 0x00000010
#define LFRFID_T5577_MAXBLOCK_SHIFT 5
#define LFRFID_T5577_TESTMODE_DISABLED 0x60000000
#define LFRFID_T5577_PASSWORD_BLOCK_NO 7

static void lfrfid_setpw_t5577_password_and_config_to_EM(LfRfid* app) {
    Popup* popup = app->popup;
    //char curr_buf[32] = {};
    char curr_buf[40] = {};
    //TODO: use .txt file in resources for passwords.

    //0b00000000000101001000000001000000; //no pwd&aor config block   12,14,17,26  // 0x148040
    //  0b00000000000101001000000001010000; //pwd& noAOR config block   12,14BITRATE,17MODUL,26MAXBLoCK,28PWD  // 0x148050
    //0b00000000000101001000000000000000; //no pwd& noAOR maxblock 0 config block   12,14BITRATE,17MODUL,26MAXBLoCK,28PWD  // 0x148000

    uint32_t em_config_block_data =
        (LFRFID_T5577_BITRATE_RF_64 | LFRFID_T5577_MODULATION_MANCHESTER |
         (2 << LFRFID_T5577_MAXBLOCK_SHIFT) | LFRFID_T5577_PWD);

    if(app->extra_options & LfRfidSetMasterKeyDisableTestMode)
        em_config_block_data |= LFRFID_T5577_TESTMODE_DISABLED;

    uint8_t* pass_p = app->password;
    uint32_t current_password = pass_p[0] << 24 | pass_p[1] << 16 | pass_p[2] << 8 | pass_p[3];
    //uint32_t current_password = app->password;
    //const uint32_t em_pw_block_data = 1976;
    pass_p = app->new_password;
    uint32_t new_password = pass_p[0] << 24 | pass_p[1] << 16 | pass_p[2] << 8 | pass_p[3];
    //uint32_t new_password = app->new_password;

    popup_set_header(popup, "Setting\npassword", 102, 10, AlignCenter, AlignCenter);
    //popup_set_icon(popup, 0, 3, &I_RFIDDolphinSend_97x61);
    popup_set_text(popup, curr_buf, /*92*/ 66, 33, AlignCenter, AlignCenter);
    snprintf(
        curr_buf,
        sizeof(curr_buf),
        "hex: %02X %02X %02X %02X\ndec: %lu",
        pass_p[0],
        pass_p[1],
        pass_p[2],
        pass_p[3],
        new_password);
    view_dispatcher_switch_to_view(app->view_dispatcher, LfRfidViewPopup);
    // DEBUG delay
    furi_delay_ms(3000);
    notification_message(app->notifications, &sequence_blink_start_magenta);
    furi_delay_ms(500);
    bool lock_bit = app->extra_options & LfRfidSetPasswordLockBit;
    bool use_password = app->extra_options & LfRfidUsePassword;

    view_dispatcher_switch_to_view(app->view_dispatcher, LfRfidViewPopup);
    {
        if(use_password)
            t5577_write_page_block_pass_with_start_and_stop(
                0,
                LFRFID_T5577_PASSWORD_BLOCK_NO,
                lock_bit,
                new_password,
                true,
                current_password /*, false*/); // Page 0, block 7
        else
            t5577_write_page_block_simple_with_start_and_stop(
                0,
                LFRFID_T5577_PASSWORD_BLOCK_NO,
                lock_bit,
                new_password /*, false*/); // Page 0, block 7
        // ^ provare questo soltanto per impostare pw
    }
    furi_delay_ms(8);

    if(app->extra_options & LfRfidDisablePasswordMode) em_config_block_data &= ~(LFRFID_T5577_PWD);

    snprintf(
        curr_buf, sizeof(curr_buf), "Setting configuration block:\n%08lX", em_config_block_data);
    view_dispatcher_switch_to_view(app->view_dispatcher, LfRfidViewPopup); //needed?
    // DEBUG delay
    furi_delay_ms(3000);
    furi_delay_ms(500);
    lock_bit = app->extra_options & LfRfidSetConfigurationLockBit;
    {
        if(use_password)
            t5577_write_page_block_pass_with_start_and_stop(
                0, 0, lock_bit, em_config_block_data, true, new_password /*, false*/);
        else
            t5577_write_page_block_simple_with_start_and_stop(
                0, 0, lock_bit, em_config_block_data /*, false*/);
    }
    furi_delay_ms(8);

    // In case password isn't updated use old one, code up there alone doesn't work in removing password, only in setting password
    // Verify if it's needed!
    if(use_password) {
        t5577_write_page_block_pass_with_start_and_stop(
            0, 0, lock_bit, em_config_block_data, true, current_password /*, false*/);
    }
    furi_delay_ms(8);

    notification_message(app->notifications, &sequence_blink_stop);
    popup_reset(app->popup);
}

void lfrfid_scene_setpw_t5577_on_enter(void* context) {
    LfRfid* app = context;
    Popup* popup = app->popup;

    lfrfid_setpw_t5577_password_and_config_to_EM(app);

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

bool lfrfid_scene_setpw_t5577_on_event(void* context, SceneManagerEvent event) {
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

void lfrfid_scene_setpw_t5577_on_exit(void* context) {
    LfRfid* app = context;
    popup_reset(app->popup);
    notification_message_block(app->notifications, &sequence_reset_green);
}
