#include "../nfc_magic_app_i.h"

void nfc_magic_scene_mf_classic_write_check_widget_callback(
    GuiButtonType result,
    InputType type,
    void* context) {
    NfcMagicApp* instance = context;

    if(type == InputTypeShort) {
        view_dispatcher_send_custom_event(instance->view_dispatcher, result);
    }
}

void nfc_magic_scene_mf_classic_write_check_on_enter(void* context) {
    NfcMagicApp* instance = context;
    Widget* widget = instance->widget;
    Gen2PollerWriteProblem can_write_all = gen2_poller_can_write_everything(instance->target_dev);
    furi_assert(can_write_all != Gen2PollerWriteProblemNoData, "No MFC data in nfc device");

    widget_add_string_element(widget, 3, 0, AlignLeft, AlignTop, FontPrimary, "Write problems");

    if(instance->gen2_poller_is_wipe_mode) {
        widget_add_text_box_element(
            widget,
            0,
            13,
            128,
            54,
            AlignLeft,
            AlignTop,
            "Not all sectors can be wiped\nTry wiping anyway?",
            false);
    } else {
        if(can_write_all == Gen2PollerWriteProblemNone) {
            widget_add_text_box_element(
                widget,
                0,
                13,
                128,
                54,
                AlignLeft,
                AlignTop,
                "This card might not have a\nrewriteable UID\nCloned card may not work\nTry writing anyway?",
                false);
        } else if(can_write_all == Gen2PollerWriteProblemMissingSourceData) {
            widget_add_text_box_element(
                widget,
                0,
                13,
                128,
                54,
                AlignLeft,
                AlignTop,
                "The source dump is incomplete\nCloned card may not work\nTry writing anyway?",
                false);
        } else if(can_write_all == Gen2PollerWriteProblemMissingTargetKeys) {
            widget_add_text_box_element(
                widget,
                0,
                13,
                128,
                54,
                AlignLeft,
                AlignTop,
                "Keys to write some sectors\nare not available\nCloned card may not work\nTry writing anyway?",
                false);
        } else if(can_write_all == Gen2PollerWriteProblemLockedAccessBits) {
            widget_add_text_box_element(
                widget,
                0,
                13,
                128,
                54,
                AlignLeft,
                AlignTop,
                "Target card doesn't allow\nwriting in some cases\nCloned card may not work\nTry writing anyway?",
                false);
        }
    }
    widget_add_button_element(
        widget,
        GuiButtonTypeCenter,
        "Continue",
        nfc_magic_scene_mf_classic_write_check_widget_callback,
        instance);
    widget_add_button_element(
        widget,
        GuiButtonTypeLeft,
        "Back",
        nfc_magic_scene_mf_classic_write_check_widget_callback,
        instance);

    // Setup and start worker
    view_dispatcher_switch_to_view(instance->view_dispatcher, NfcMagicAppViewWidget);
}

bool nfc_magic_scene_mf_classic_write_check_on_event(void* context, SceneManagerEvent event) {
    NfcMagicApp* instance = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == GuiButtonTypeLeft) {
            consumed = scene_manager_search_and_switch_to_previous_scene(
                instance->scene_manager, NfcMagicSceneMfClassicMenu);
        } else if(event.event == GuiButtonTypeCenter) {
            if(instance->gen2_poller_is_wipe_mode) {
                scene_manager_next_scene(instance->scene_manager, NfcMagicSceneWipe);
            } else {
                scene_manager_next_scene(instance->scene_manager, NfcMagicSceneWrite);
            }
            consumed = true;
        }
    }
    return consumed;
}

void nfc_magic_scene_mf_classic_write_check_on_exit(void* context) {
    NfcMagicApp* instance = context;

    widget_reset(instance->widget);
}
