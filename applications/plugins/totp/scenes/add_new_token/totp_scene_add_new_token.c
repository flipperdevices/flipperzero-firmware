#include "totp_scene_add_new_token.h"
#include "../../types/common.h"
#include "../../lib/ui/constants.h"
#include "../scene_director.h"
#include "totp_input_text.h"
#include "../../types/token_info.h"
#include "../../lib/list/list.h"
#include "../../lib/base32/base32.h"
#include "../../lib/config/config.h"
#include "../../lib/ui/ui_controls.h"
#include "../generate_token/totp_scene_generate_token.h"

typedef enum {
    TokenNameTextBox,
    TokenSecretTextBox,
    ConfirmButton,
} Control;

typedef struct {
    char* token_name;
    uint8_t token_name_length;
    char* token_secret;
    uint8_t token_secret_length;
    bool saved;
    Control selected_control;
    InputTextSceneContext* token_name_input_context;
    InputTextSceneContext* token_secret_input_context;
    InputTextSceneState* input_state;
    uint32_t input_started_at;
    int current_token_index;
} SceneState;

void totp_scene_add_new_token_init(PluginState* plugin_state) {
    UNUSED(plugin_state);
}

static void on_token_name_user_comitted(InputTextSceneCallbackResult* result) {
    SceneState* scene_state = result->callback_data;
    free(scene_state->token_name);
    scene_state->token_name = result->user_input;
    scene_state->token_name_length = result->user_input_length;
    scene_state->input_started_at = 0;
    free(result);
}

static void on_token_secret_user_comitted(InputTextSceneCallbackResult* result) {
    SceneState* scene_state = result->callback_data;
    free(scene_state->token_secret);
    scene_state->token_secret = result->user_input;
    scene_state->token_secret_length = result->user_input_length;
    scene_state->input_started_at = 0;
    free(result);
}

void totp_scene_add_new_token_activate(PluginState* plugin_state, const TokenAddEditSceneContext* context) {
    SceneState* scene_state = malloc(sizeof(SceneState));
    plugin_state->current_scene_state = scene_state;
    scene_state->token_name = "Name";
    scene_state->token_name_length = strlen(scene_state->token_name);
    scene_state->token_secret = "Secret";
    scene_state->token_secret_length = strlen(scene_state->token_secret);

    scene_state->token_name_input_context = malloc(sizeof(InputTextSceneContext));
    scene_state->token_name_input_context->header_text = "Enter token name";
    scene_state->token_name_input_context->callback_data = scene_state;
    scene_state->token_name_input_context->callback = on_token_name_user_comitted;

    scene_state->token_secret_input_context = malloc(sizeof(InputTextSceneContext));
    scene_state->token_secret_input_context->header_text = "Enter token secret";
    scene_state->token_secret_input_context->callback_data = scene_state;
    scene_state->token_secret_input_context->callback = on_token_secret_user_comitted;

    scene_state->input_state = NULL;

    if (context == NULL) {
        scene_state->current_token_index = -1;
    } else {
        scene_state->current_token_index = context->current_token_index;
    }
}

void totp_scene_add_new_token_render(Canvas* const canvas, PluginState* plugin_state) {
    SceneState* scene_state = (SceneState *)plugin_state->current_scene_state;
    if (scene_state->input_started_at > 0) {
        totp_input_text_render(canvas, scene_state->input_state);
        return;
    }

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 0, 0, AlignLeft, AlignTop, "Add new token");

    canvas_set_font(canvas, FontSecondary);
    ui_control_text_box_render(canvas, 10, scene_state->token_name, scene_state->selected_control == TokenNameTextBox);
    ui_control_text_box_render(canvas, 27, scene_state->token_secret, scene_state->selected_control == TokenSecretTextBox);
    ui_control_button_render(canvas, SCREEN_WIDTH_CENTER - 24, 48, 48, 13, "Confirm", scene_state->selected_control == ConfirmButton);
}

bool totp_scene_add_new_token_handle_event(PluginEvent* const event, PluginState* plugin_state) {
    if(event->type == EventTypeKey) {
        SceneState* scene_state = (SceneState *)plugin_state->current_scene_state;
        if (scene_state->input_started_at > 0 && furi_get_tick() - scene_state->input_started_at > 300) {
            return totp_input_text_handle_event(event, scene_state->input_state);
        }

        if (event->input.type == InputTypeLong && event->input.key == InputKeyBack) {
            return false;
        } else if(event->input.type == InputTypePress) {
            switch(event->input.key) {
                case InputKeyUp:
                    if (scene_state->selected_control > TokenNameTextBox) {
                        scene_state->selected_control--;
                    }
                    break;
                case InputKeyDown:
                    if (scene_state->selected_control < ConfirmButton) {
                        scene_state->selected_control++;
                    }
                    break;
                case InputKeyRight:
                    break;
                case InputKeyLeft:
                    break;
                case InputKeyOk:
                    switch (scene_state->selected_control) {
                        case TokenNameTextBox:
                            if (scene_state->input_state != NULL) {
                                totp_input_text_free(scene_state->input_state);
                            }
                            scene_state->input_state = totp_input_text_activate(scene_state->token_name_input_context);
                            scene_state->input_started_at = furi_get_tick();
                            break;
                        case TokenSecretTextBox:
                            if (scene_state->input_state != NULL) {
                                totp_input_text_free(scene_state->input_state);
                            }
                            scene_state->input_state = totp_input_text_activate(scene_state->token_secret_input_context);
                            scene_state->input_started_at = furi_get_tick();
                            break;
                        case ConfirmButton: {
                            TokenInfo* tokenInfo = malloc(sizeof(TokenInfo));
                            tokenInfo->name = malloc(scene_state->token_name_length + 1);
                            strcpy(tokenInfo->name, scene_state->token_name);

                            token_info_set_secret(tokenInfo, scene_state->token_secret, scene_state->token_secret_length, &plugin_state->iv[0]);
                            
                            if (plugin_state->tokens_list == NULL) {
                                plugin_state->tokens_list = list_init_head(tokenInfo);
                            } else {
                                list_add(plugin_state->tokens_list, tokenInfo);
                            }
                            plugin_state->tokens_count++;

                            Storage* cfg_storage = totp_open_storage();
                            FlipperFormat* cfg_file = totp_open_config_file(cfg_storage);

                            flipper_format_seek_to_end(cfg_file);
                            flipper_format_write_string_cstr(cfg_file, TOTP_CONFIG_KEY_TOKEN_NAME, tokenInfo->name);
                            flipper_format_write_hex(cfg_file, TOTP_CONFIG_KEY_TOKEN_SECRET, tokenInfo->token, tokenInfo->token_length);

                            totp_close_config_file(cfg_file);
                            totp_close_storage();

                            GenerateTokenSceneContext generate_scene_context = { .current_token_index = plugin_state->tokens_count - 1 };
                            totp_scene_director_activate_scene(plugin_state, TotpSceneGenerateToken, &generate_scene_context);
                            break;
                        }
                    }
                    break;
                case InputKeyBack:
                    if (scene_state->current_token_index >= 0) {
                        GenerateTokenSceneContext generate_scene_context = { .current_token_index = scene_state->current_token_index };
                        totp_scene_director_activate_scene(plugin_state, TotpSceneGenerateToken, &generate_scene_context);
                    } else {
                        totp_scene_director_activate_scene(plugin_state, TotpSceneGenerateToken, NULL);
                    }
                    break;
            }
        }
    }
    return true;
}

void totp_scene_add_new_token_deactivate(PluginState* plugin_state) {
    if (plugin_state->current_scene_state == NULL) return;
    SceneState* scene_state = (SceneState *)plugin_state->current_scene_state;
    free(scene_state->token_name);
    free(scene_state->token_secret);

    free(scene_state->token_name_input_context->header_text);
    free(scene_state->token_name_input_context);

    free(scene_state->token_secret_input_context->header_text);
    free(scene_state->token_secret_input_context);

    if (scene_state->input_state != NULL) {
        totp_input_text_free(scene_state->input_state);
    }

    free(plugin_state->current_scene_state);
    plugin_state->current_scene_state = NULL;
}

void totp_scene_add_new_token_free(PluginState* plugin_state) {
    UNUSED(plugin_state);
}
