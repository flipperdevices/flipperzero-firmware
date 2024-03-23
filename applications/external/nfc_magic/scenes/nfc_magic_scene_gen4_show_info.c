#include "../nfc_magic_app_i.h"
#include "gui/modules/widget_elements/widget_element.h"
#include "gui/scene_manager.h"
#include "protocols/gen4/gen4_poller_i.h"
#include <bit_lib.h>

#define CONFIG_SIZE (32)

static char* gen4_get_shadow_mode_name(Gen4PollerShadowMode mode) {
    switch(mode) {
    case Gen4PollerShadowModePreWrite:
        return "Pre-Write";
    case Gen4PollerShadowModeRestore:
        return "Restore";
    case Gen4PollerShadowModeDisabled:
        return "Disabled";
    case Gen4PollerShadowModeHighSpeedDisabled:
        return "Disabled (High-speed)";
    case Gen4PollerShadowModeSplit:
        return "Split";
    default:
        return "Unknown";
    }
}

static char* gen4_get_direct_write_mode_name(Gen4PollerDirectWriteBlock0Mode mode) {
    switch(mode) {
    case Gen4PollerDirectWriteBlock0ModeEnabled:
        return "Enabled";
    case Gen4PollerDirectWriteBlock0ModeDisabled:
        return "Disabled";
    case Gen4PollerDirectWriteBlock0ModeDefault:
        return "Default";
    default:
        return "Unknown";
    }
}

static char* gen4_get_uid_len(uint8_t byte) {
    switch(byte) {
    case 0:
        return "4";
    case 1:
        return "7";
    default:
        return "Unknown";
    }
}

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

    notification_message(instance->notifications, &sequence_success);

    Gen4PollerDirectWriteBlock0Mode dw_mode = instance->gen4_config[29];
    Gen4PollerShadowMode s_mode = instance->gen4_config[6];
    uint8_t ats_len = instance->gen4_config[7];

    FuriString* output = furi_string_alloc();

    // Revision
    furi_string_cat_printf(
        output, "Revision: %02X %02X\n", instance->gen4_revision[3], instance->gen4_revision[4]);

    // Password
    furi_string_cat_printf(
        output, "Password: %08llX\n", bit_lib_bytes_to_num_be(instance->gen4_config + 2, 4));

    // Shadow mode
    furi_string_cat_printf(output, "Shadow Mode: %s\n", gen4_get_shadow_mode_name(s_mode));

    // Direct write to block 0 mode
    furi_string_cat_printf(
        output, "Direct Write Mode: %s\n", gen4_get_direct_write_mode_name(dw_mode));

    /////////////////////////////////////////////////////////////////////////////////////////////////////
    furi_string_cat_printf(output, ":::::::::::::[Configured As]::::::::::::::\n");
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    // UID len
    furi_string_cat_printf(output, "UID Length: %s\n", gen4_get_uid_len(instance->gen4_config[1]));

    // ATS
    furi_string_cat_printf(output, "ATS:");
    if(ats_len)
        for(uint8_t i = 0; i < ats_len; i++)
            furi_string_cat_printf(output, " %02X", instance->gen4_config[8 + i]);
    else
        furi_string_cat_printf(output, " No");
    furi_string_cat_printf(output, "\n");

    // ATQA
    furi_string_cat_printf(
        output,
        "ATQA: %02X %02X\n",
        instance->gen4_config[25],
        instance->gen4_config[24]); // invert for big endian

    // SAK
    furi_string_cat_printf(output, "SAK: %02X\n", instance->gen4_config[26]);

    // Blocks
    furi_string_cat_printf(
        output,
        "Total blocks: %u",
        instance->gen4_config[28] + 1); // config stores the number of the last block

    widget_add_string_element(widget, 3, 4, AlignLeft, AlignTop, FontPrimary, "Gen4 Info");

    widget_add_text_scroll_element(widget, 3, 17, 124, 50, furi_string_get_cstr(output));

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
