#include "../cfw_app.h"

enum TextInputResult {
    TextInputResultOk,
    TextInputResultError,
};

static void cfw_app_scene_protocols_frequencies_add_text_input_callback(void* context) {
    CfwApp* app = context;

    char* end;
    uint32_t value = strtol(app->subghz_freq_buffer, &end, 0) * 10000;
    if(*end || !furi_hal_subghz_is_frequency_valid(value)) {
        view_dispatcher_send_custom_event(app->view_dispatcher, TextInputResultError);
        return;
    }
    bool is_hopper =
        scene_manager_get_scene_state(app->scene_manager, CfwAppSceneProtocolsFrequenciesAdd);
    if(is_hopper) {
        FrequencyList_push_back(app->subghz_hopper_freqs, value);
    } else {
        FrequencyList_push_back(app->subghz_static_freqs, value);
    }
    app->save_subghz_frequencies = true;
    view_dispatcher_send_custom_event(app->view_dispatcher, TextInputResultOk);
}

void cfw_app_scene_protocols_frequencies_add_on_enter(void* context) {
    CfwApp* app = context;
    TextInput* text_input = app->text_input;

    text_input_set_header_text(text_input, "Format: 12356");

    strlcpy(app->subghz_freq_buffer, "", CFW_SUBGHZ_FREQ_BUFFER_SIZE);

    text_input_set_result_callback(
        text_input,
        cfw_app_scene_protocols_frequencies_add_text_input_callback,
        app,
        app->subghz_freq_buffer,
        CFW_SUBGHZ_FREQ_BUFFER_SIZE,
        true);

    view_dispatcher_switch_to_view(app->view_dispatcher, CfwAppViewTextInput);
}

void callback_return(void* context) {
    CfwApp* app = context;
    scene_manager_previous_scene(app->scene_manager);
}

bool cfw_app_scene_protocols_frequencies_add_on_event(void* context, SceneManagerEvent event) {
    CfwApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        consumed = true;
        switch(event.event) {
        case TextInputResultOk:
            scene_manager_previous_scene(app->scene_manager);
            break;
        case TextInputResultError:
            popup_set_header(app->popup, "Invalid value!", 64, 26, AlignCenter, AlignCenter);
            popup_set_text(
                app->popup, "Frequency was not added...", 64, 40, AlignCenter, AlignCenter);
            popup_set_callback(app->popup, callback_return);
            popup_set_context(app->popup, app);
            popup_set_timeout(app->popup, 1000);
            popup_enable_timeout(app->popup);
            view_dispatcher_switch_to_view(app->view_dispatcher, CfwAppViewPopup);
            break;
        default:
            break;
        }
    }

    return consumed;
}

void cfw_app_scene_protocols_frequencies_add_on_exit(void* context) {
    CfwApp* app = context;
    text_input_reset(app->text_input);
}
