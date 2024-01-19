#include "../picopass_i.h"
#include <dolphin/dolphin.h>
#include <picopass_keys.h>

void picopass_scene_read_card_success_widget_callback(
    GuiButtonType result,
    InputType type,
    void* context) {
    furi_assert(context);
    Picopass* picopass = context;

    if(type == InputTypeShort) {
        view_dispatcher_send_custom_event(picopass->view_dispatcher, result);
    }
}

void picopass_scene_read_card_success_on_enter(void* context) {
    Picopass* picopass = context;

    FuriString* csn_str = furi_string_alloc_set("CSN:");
    FuriString* credential_str = furi_string_alloc();
    FuriString* wiegand_str = furi_string_alloc();
    FuriString* key_str = furi_string_alloc();

    dolphin_deed(DolphinDeedNfcReadSuccess);

    // Send notification
    notification_message(picopass->notifications, &sequence_success);

    // Setup view
    PicopassBlock* AA1 = picopass->dev->dev_data.AA1;
    PicopassPacs* pacs = &picopass->dev->dev_data.pacs;
    Widget* widget = picopass->widget;

    uint8_t csn[PICOPASS_BLOCK_LEN] = {0};
    memcpy(csn, AA1[PICOPASS_CSN_BLOCK_INDEX].data, PICOPASS_BLOCK_LEN);
    for(uint8_t i = 0; i < PICOPASS_BLOCK_LEN; i++) {
        furi_string_cat_printf(csn_str, "%02X", csn[i]);
    }

    // We can't test the pacs->key in case it is intentionally all 0's and we can't test the key block since it is populated with the diversified key before each key test, so we approximate with the PACS config block being blank.
    bool no_key = picopass_is_memset(
        AA1[PICOPASS_ICLASS_PACS_CFG_BLOCK_INDEX].data, 0x00, PICOPASS_BLOCK_LEN);
    bool empty = picopass_is_memset(
        AA1[PICOPASS_ICLASS_PACS_CFG_BLOCK_INDEX].data, 0xFF, PICOPASS_BLOCK_LEN);
    bool SE = 0x30 == AA1[PICOPASS_ICLASS_PACS_CFG_BLOCK_INDEX].data[0];
    bool configCard = (AA1[PICOPASS_ICLASS_PACS_CFG_BLOCK_INDEX].data[7] >> 2 & 3) == 2;
    bool secured = (AA1[PICOPASS_CONFIG_BLOCK_INDEX].data[7] & PICOPASS_FUSE_CRYPT10) !=
                   PICOPASS_FUSE_CRYPT0;
    bool hid_csn = picopass_device_hid_csn(picopass->dev);

    if(!secured) {
        furi_string_cat_printf(wiegand_str, "Non-Secured Chip");

        if(!hid_csn) {
            furi_string_cat_printf(credential_str, "Non-HID CSN");
        }

        widget_add_button_element(
            widget,
            GuiButtonTypeRight,
            "More",
            picopass_scene_read_card_success_widget_callback,
            picopass);
    } else if(no_key) {
        furi_string_cat_printf(wiegand_str, "Read Failed");

        if(pacs->se_enabled) {
            furi_string_cat_printf(credential_str, "SE enabled");
        } else if(!hid_csn) {
            furi_string_cat_printf(credential_str, "Non-HID CSN");
        }

        widget_add_button_element(
            widget,
            GuiButtonTypeCenter,
            "Menu",
            picopass_scene_read_card_success_widget_callback,
            picopass);
        widget_add_button_element(
            widget,
            GuiButtonTypeRight,
            "More",
            picopass_scene_read_card_success_widget_callback,
            picopass);
    } else if(pacs->se_enabled) {
        furi_string_cat_printf(credential_str, "SE enabled");
        furi_string_cat_printf(wiegand_str, "SIO");
        widget_add_button_element(
            widget,
            GuiButtonTypeRight,
            "More",
            picopass_scene_read_card_success_widget_callback,
            picopass);
    } else if(configCard) {
        furi_string_cat_printf(wiegand_str, "Config Card");
    } else if(empty) {
        furi_string_cat_printf(wiegand_str, "Empty");
        widget_add_button_element(
            widget,
            GuiButtonTypeCenter,
            "Menu",
            picopass_scene_read_card_success_widget_callback,
            picopass);
    } else if(pacs->bitLength == 0 || pacs->bitLength == 255) {
        // Neither of these are valid.  Indicates the block was all 0x00 or all 0xff
        if(SE) {
            furi_string_cat_printf(wiegand_str, "SIO");
        } else {
            furi_string_cat_printf(wiegand_str, "Invalid PACS");
        }
        widget_add_button_element(
            widget,
            GuiButtonTypeCenter,
            "Menu",
            picopass_scene_read_card_success_widget_callback,
            picopass);
        widget_add_button_element(
            widget,
            GuiButtonTypeRight,
            "More",
            picopass_scene_read_card_success_widget_callback,
            picopass);
    } else {
        size_t bytesLength = 1 + pacs->bitLength / 8;
        furi_string_set(credential_str, "");
        for(uint8_t i = PICOPASS_BLOCK_LEN - bytesLength; i < PICOPASS_BLOCK_LEN; i++) {
            furi_string_cat_printf(credential_str, "%02X", pacs->credential[i]);
        }
        furi_string_cat_printf(wiegand_str, "%d bits", pacs->bitLength);

        if(pacs->sio) {
            furi_string_cat_printf(credential_str, " +SIO");
        }

        if(pacs->key) {
            furi_string_cat_printf(key_str, "Key: ");
            uint8_t key[PICOPASS_BLOCK_LEN];
            memcpy(key, &pacs->key, PICOPASS_BLOCK_LEN);

            bool standard_key = true;
            // Handle DES key being 56bits with parity in LSB
            for(uint8_t i = 0; i < PICOPASS_BLOCK_LEN; i++) {
                if((key[i] & 0xFE) != (picopass_iclass_key[i] & 0xFE)) {
                    standard_key = false;
                    break;
                }
            }

            if(standard_key) {
                furi_string_cat_printf(key_str, "Standard");
            } else {
                for(uint8_t i = 0; i < PICOPASS_BLOCK_LEN; i++) {
                    furi_string_cat_printf(key_str, "%02X", key[i]);
                }
            }
        }

        widget_add_button_element(
            widget,
            GuiButtonTypeRight,
            "More",
            picopass_scene_read_card_success_widget_callback,
            picopass);
    }

    widget_add_button_element(
        widget,
        GuiButtonTypeLeft,
        "Retry",
        picopass_scene_read_card_success_widget_callback,
        picopass);

    widget_add_string_element(
        widget, 64, 5, AlignCenter, AlignCenter, FontSecondary, furi_string_get_cstr(csn_str));
    widget_add_string_element(
        widget, 64, 20, AlignCenter, AlignCenter, FontPrimary, furi_string_get_cstr(wiegand_str));
    widget_add_string_element(
        widget,
        64,
        36,
        AlignCenter,
        AlignCenter,
        FontSecondary,
        furi_string_get_cstr(credential_str));
    widget_add_string_element(
        widget, 64, 46, AlignCenter, AlignCenter, FontSecondary, furi_string_get_cstr(key_str));

    furi_string_free(csn_str);
    furi_string_free(credential_str);
    furi_string_free(wiegand_str);
    furi_string_free(key_str);

    view_dispatcher_switch_to_view(picopass->view_dispatcher, PicopassViewWidget);
}

bool picopass_scene_read_card_success_on_event(void* context, SceneManagerEvent event) {
    Picopass* picopass = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == GuiButtonTypeLeft) {
            consumed = scene_manager_previous_scene(picopass->scene_manager);
        } else if(event.event == GuiButtonTypeRight) {
            // Clear device name
            picopass_device_set_name(picopass->dev, "");
            scene_manager_next_scene(picopass->scene_manager, PicopassSceneCardMenu);
            consumed = true;
        } else if(event.event == GuiButtonTypeCenter) {
            consumed = scene_manager_search_and_switch_to_another_scene(
                picopass->scene_manager, PicopassSceneStart);
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        scene_manager_search_and_switch_to_previous_scene(
            picopass->scene_manager, PicopassSceneStart);
        consumed = true;
    }
    return consumed;
}

void picopass_scene_read_card_success_on_exit(void* context) {
    Picopass* picopass = context;

    // Clear view
    widget_reset(picopass->widget);
}
