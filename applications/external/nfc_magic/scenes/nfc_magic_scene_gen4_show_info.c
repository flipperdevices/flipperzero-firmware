#include "../nfc_magic_app_i.h"

void nfc_magic_scene_gen4_show_info_widget_callback(
    GuiButtonType result,
    InputType type,
    void* context) {
    NfcMagicApp* instance = context;

    if(type == InputTypeShort) {
        view_dispatcher_send_custom_event(instance->view_dispatcher, result);
    }
}

void nfc_magic_scene_gen4_show_info_on_enter(void* context) {
    NfcMagicApp* instance = context;
    Widget* widget = instance->widget;

    if(scene_manager_get_scene_state(instance->scene_manager, NfcMagicSceneGen4ShowInfo))
        notification_message(instance->notifications, &sequence_success);
    scene_manager_set_scene_state(instance->scene_manager, NfcMagicSceneGen4ShowInfo, false);

    Gen4* gen4 = instance->gen4_data;

    Gen4DirectWriteBlock0Mode dw_mode = gen4->config.data_parsed.direct_write_mode;
    Gen4ShadowMode s_mode = gen4->config.data_parsed.gtu_mode;
    uint8_t ats_len = gen4->config.data_parsed.ats_len;

    FuriString* output = furi_string_alloc();

    furi_string_printf(output, "\e#Gen4\n");

    // Revision
    furi_string_cat_printf(
        output, "Revision: %02X %02X\n", gen4->revision.data[3], gen4->revision.data[4]);

    // Password
    furi_string_cat_printf(
        output,
        "Password: %02X %02X %02X %02X\n",
        gen4->config.data_parsed.password.bytes[0],
        gen4->config.data_parsed.password.bytes[1],
        gen4->config.data_parsed.password.bytes[2],
        gen4->config.data_parsed.password.bytes[3]);

    // Shadow mode
    furi_string_cat_printf(output, "Shadow Mode: %s\n", gen4_get_shadow_mode_name(s_mode));

    // Direct write to block 0 mode
    furi_string_cat_printf(
        output, "Direct Write Mode: %s\n", gen4_get_direct_write_mode_name(dw_mode));

    /////////////////////////////////////////////////////////////////////////////////////////////////////
    furi_string_cat_printf(output, ":::::::::::::[Configured As]::::::::::::::\n");
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    // Configuration type:
    furi_string_cat_printf(output, "%s\n", gen4_get_configuration_name(&gen4->config));

    // UID len
    furi_string_cat_printf(
        output, "UID Length: %s\n", gen4_get_uid_len_num(gen4->config.data_parsed.uid_len_code));

    // ATS
    furi_string_cat_printf(output, "ATS:");
    if(ats_len)
        for(uint8_t i = 0; i < ats_len; i++)
            furi_string_cat_printf(output, " %02X", gen4->config.data_parsed.ats[i]);
    else
        furi_string_cat_printf(output, " No");
    furi_string_cat_printf(output, "\n");

    // ATQA
    furi_string_cat_printf(
        output,
        "ATQA: %02X %02X\n",
        gen4->config.data_parsed.atqa[0],
        gen4->config.data_parsed.atqa[1]);

    // SAK
    furi_string_cat_printf(output, "SAK: %02X\n", gen4->config.data_parsed.sak);

    // Blocks
    furi_string_cat_printf(
        output,
        "Total blocks: %u",
        gen4->config.data_parsed.total_blocks + 1); // config stores the number of the last block

    widget_add_text_scroll_element(widget, 0, 0, 128, 64, furi_string_get_cstr(output));

    widget_add_button_element(
        widget,
        GuiButtonTypeRight,
        "More",
        nfc_magic_scene_gen4_show_info_widget_callback,
        instance);

    furi_string_free(output);
    view_dispatcher_switch_to_view(instance->view_dispatcher, NfcMagicAppViewWidget);
}

bool nfc_magic_scene_gen4_show_info_on_event(void* context, SceneManagerEvent event) {
    NfcMagicApp* instance = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == GuiButtonTypeRight) {
            scene_manager_next_scene(instance->scene_manager, NfcMagicSceneGen4ShowCfg);
            consumed = true;
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        consumed = scene_manager_search_and_switch_to_previous_scene(
            instance->scene_manager, NfcMagicSceneGen4Menu);
    }
    return consumed;
}

void nfc_magic_scene_gen4_show_info_on_exit(void* context) {
    NfcMagicApp* instance = context;

    widget_reset(instance->widget);
}
