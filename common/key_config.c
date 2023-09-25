#include "key_config_i.h"
#include "flipboard_model.h"

#include <furi.h>

#include "config_colors.h"
#include "config_keys.h"
#include "config_tones.h"

/*
    uint8_t keystrokes_count;
    Keystroke* keystrokes;
*/

typedef struct {
    void* app;
    uint8_t key;
    uint8_t index;
} VariableItemContext;

/*
static void flipboard_setting_keystroke_key_code_changed(VariableItem* item, int i) {
    UNUSED(i);
    uint8_t selection_index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, setting_switch_key_names[selection_index]);
    uint16_t key_code = setting_switch_key_values[selection_index];

    VariableItemContext* vic = variable_item_get_context(item);
    FlipboardModel* model = vic->app->model;

    uint8_t count = key_setting_model_get_keystroke(
                        flipboard_model_get_key_setting_model(model, vic->key), vic->index)
                        .count;
    key_setting_model_set_keystroke(
        flipboard_model_get_key_setting_model(model, vic->key), vic->index, key_code, count);
}

static void flipboard_setting_keystroke_count_changed(VariableItem* item, int i) {
    UNUSED(i);
    uint8_t selection_index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, setting_switch_key_names[selection_index]);
    uint16_t count = selection_index;

    VariableItemContext* vic = variable_item_get_context(item);
    FlipboardModel* model = vic->app->model;

    uint16_t key_code = key_setting_model_get_keystroke(
                            flipboard_model_get_key_setting_model(model, vic->key), vic->index)
                            .key_code;
    key_setting_model_set_keystroke(
        flipboard_model_get_key_setting_model(model, vic->key), vic->index, key_code, count);
}
*/

static void color_up_changed(VariableItem* item) {
    KeySettingModel* ksm = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    key_setting_model_set_color_up(ksm, color_values[index]);
    variable_item_set_current_value_text(item, color_names[index]);
}

static void color_down_changed(VariableItem* item) {
    KeySettingModel* ksm = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    key_setting_model_set_color_down(ksm, color_values[index]);
    variable_item_set_current_value_text(item, color_names[index]);
}

static void tone_changed(VariableItem* item) {
    KeySettingModel* ksm = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    key_setting_model_set_frequency(ksm, freqs_values[index]);
    variable_item_set_current_value_text(item, freq_names[index]);
}

static void populate_variable_item_list_color(
    KeyConfig* key_config,
    KeySettingModel* ksm,
    char* label,
    VariableItemChangeCallback callback,
    uint32_t color) {
    VariableItem* item =
        variable_item_list_add(key_config->item_list, label, COUNT_OF(color_names), callback, ksm);
    uint8_t index = 0;
    for(size_t i = 0; i < COUNT_OF(color_values); i++) {
        if(color == color_values[i]) {
            index = i;
            break;
        }
    }
    variable_item_set_current_value_index(item, index);
    variable_item_set_current_value_text(item, color_names[index]);
}

static void populate_variable_item_list_frequency(
    KeyConfig* key_config,
    KeySettingModel* ksm,
    char* label,
    VariableItemChangeCallback callback,
    float frequency) {
    VariableItem* item =
        variable_item_list_add(key_config->item_list, label, COUNT_OF(freq_names), callback, ksm);
    uint8_t index = 0;
    for(size_t i = 0; i < COUNT_OF(freqs_values); i++) {
        float diff = frequency - freqs_values[i];
        if(diff < 0.0f) diff = -diff;
        if(diff < 1.0f) {
            index = i;
            break;
        }
    }
    variable_item_set_current_value_index(item, index);
    variable_item_set_current_value_text(item, freq_names[index]);
}

static void message_updated(void* context) {
    KeySettingModel* ksm = (KeySettingModel*)context;
    KeyConfig* key_config = (KeyConfig*)key_setting_model_get_key_config(ksm);
    furi_assert(key_config);
    key_setting_model_set_message(ksm, key_setting_model_get_temp_buffer(ksm));
    view_dispatcher_switch_to_view(key_config->view_dispatcher, key_config->view_item_list_id);
}

uint32_t menu_0(void* context) {
    UNUSED(context);
    return (uint32_t)0;
}
uint32_t menu_1(void* context) {
    UNUSED(context);
    return (uint32_t)1;
}
uint32_t menu_2(void* context) {
    UNUSED(context);
    return (uint32_t)2;
}
uint32_t menu_3(void* context) {
    UNUSED(context);
    return (uint32_t)3;
}
uint32_t menu_4(void* context) {
    UNUSED(context);
    return (uint32_t)4;
}
uint32_t menu_5(void* context) {
    UNUSED(context);
    return (uint32_t)5;
}
uint32_t menu_6(void* context) {
    UNUSED(context);
    return (uint32_t)6;
}
uint32_t menu_7(void* context) {
    UNUSED(context);
    return (uint32_t)7;
}
uint32_t menu_8(void* context) {
    UNUSED(context);
    return (uint32_t)8;
}
uint32_t menu_9(void* context) {
    UNUSED(context);
    return (uint32_t)9;
}
uint32_t menu_10(void* context) {
    UNUSED(context);
    return (uint32_t)10;
}

ViewNavigationCallback get_menu(uint32_t i) {
    switch(i) {
    case 1:
        return menu_1;
    case 2:
        return menu_2;
    case 3:
        return menu_3;
    case 4:
        return menu_4;
    case 5:
        return menu_5;
    case 6:
        return menu_6;
    case 7:
        return menu_7;
    case 8:
        return menu_8;
    case 9:
        return menu_9;
    case 10:
        return menu_10;
    default:
        FURI_LOG_E("Flipboard", "Invalid menu index %ld", i);
        return menu_0;
    }
}

static void item_clicked(void* context, uint32_t index) {
    KeySettingModel* ksm = (KeySettingModel*)context;
    uint8_t message_index = key_setting_model_get_message_index(ksm);
    if(index == message_index) {
        KeyConfig* key_config = (KeyConfig*)key_setting_model_get_key_config(ksm);
        furi_assert(key_config);

        text_input_set_header_text(key_config->text_input, "Enter message");
        if(key_setting_model_get_message(ksm)) {
            strncpy(
                key_setting_model_get_temp_buffer(ksm),
                furi_string_get_cstr(key_setting_model_get_message(ksm)),
                key_setting_model_get_temp_buffer_size(ksm) - 1);
        } else {
            key_setting_model_get_temp_buffer(ksm)[0] = 0;
        }

        view_set_previous_callback(
            text_input_get_view(key_config->text_input), get_menu(key_config->view_item_list_id));

        text_input_set_result_callback(
            key_config->text_input,
            message_updated,
            ksm,
            key_setting_model_get_temp_buffer(ksm),
            key_setting_model_get_temp_buffer_size(ksm),
            false);

        view_dispatcher_switch_to_view(
            key_config->view_dispatcher, key_config->view_text_input_id);
    }
}

static void populate_variable_item_list(KeyConfig* key_config, KeySettingModel* ksm) {
    variable_item_list_reset(key_config->item_list);
    uint8_t item_index = 0;

    if(flipboard_model_get_key_setting_model_fields(key_config->model) &
       KeySettingModelFieldColorUp) {
        populate_variable_item_list_color(
            key_config, ksm, "Color up", color_up_changed, key_setting_model_get_color_up(ksm));
        item_index++;
    }

    if(flipboard_model_get_key_setting_model_fields(key_config->model) &
       KeySettingModelFieldColorDown) {
        populate_variable_item_list_color(
            key_config,
            ksm,
            "Color down",
            color_down_changed,
            key_setting_model_get_color_down(ksm));
        item_index++;
    }

    if(flipboard_model_get_key_setting_model_fields(key_config->model) &
       KeySettingModelFieldFrequency) {
        populate_variable_item_list_frequency(
            key_config, ksm, "Frequency", tone_changed, key_setting_model_get_frequency(ksm));
        item_index++;
    }

    if(flipboard_model_get_key_setting_model_fields(key_config->model) &
       KeySettingModelFieldMessage) {
        variable_item_list_add(key_config->item_list, "Message", 0, NULL, NULL);
        variable_item_list_set_enter_callback(key_config->item_list, item_clicked, ksm);
        key_setting_model_set_message_index(ksm, item_index);
        item_index++;
    }

    if(key_config->view_dispatcher) {
        view_dispatcher_switch_to_view(key_config->view_dispatcher, key_config->view_item_list_id);
    }
}

static void item_callback(void* context, uint32_t index) {
    KeyConfig* key_config = (KeyConfig*)context;
    FlipboardModel* model = key_config->model;
    KeySettingModel* ksm = flipboard_model_get_key_setting_model(model, index);
    if(!ksm) {
        FURI_LOG_E("TAG", "Index=%ld ksm=NULL", index);
    } else {
        FURI_LOG_D("TAG", "Index=%ld KSM.key_id=%d", index, key_setting_model_get_key_id(ksm));
    }

    furi_assert(ksm && key_setting_model_get_key_id(ksm) == index);
    key_setting_model_set_key_config(ksm, key_config);
    populate_variable_item_list(key_config, ksm);
}

KeyConfig* key_config_alloc(FlipboardModel* model, uint32_t config_view_id) {
    KeyConfig* key_config = (KeyConfig*)malloc(sizeof(KeyConfig));
    key_config->view_dispatcher = NULL;
    key_config->model = model;
    key_config->text_input = text_input_alloc();
    key_config->view_text_input_id = 0;
    key_config->menu_keys = submenu_alloc();
    key_config->view_menu_keys_id = config_view_id;
    key_config->item_list = variable_item_list_alloc();
    key_config->view_item_list_id = 0;
    view_set_previous_callback(
        variable_item_list_get_view(key_config->item_list), get_menu(config_view_id));

    FuriString* key_name = furi_string_alloc();

    bool single = flipboard_model_get_single_button_mode(model);

    int display_count = 0;
    for(int i = 1; i < 16;) {
        display_count++;
        furi_string_printf(key_name, "Key %d", display_count);
        submenu_add_item(
            key_config->menu_keys, furi_string_get_cstr(key_name), i, item_callback, key_config);
        if(single) {
            i = i << 1;
        } else {
            i++;
        }
    }
    submenu_set_header(key_config->menu_keys, "Configure Key");

    return key_config;
}

View* key_config_get_view(KeyConfig* key_config) {
    return submenu_get_view(key_config->menu_keys);
}

uint32_t key_config_get_view_id(KeyConfig* key_config) {
    return key_config->view_menu_keys_id;
}

void key_config_register_dispatcher(KeyConfig* key_config, ViewDispatcher* view_dispatcher) {
    key_config->view_dispatcher = view_dispatcher;
}

void key_config_register_variable_item_list(
    KeyConfig* key_config,
    uint32_t variable_item_list_view_id) {
    furi_assert(key_config->view_dispatcher != NULL);
    key_config->view_item_list_id = variable_item_list_view_id;
    view_dispatcher_add_view(
        key_config->view_dispatcher,
        key_config->view_item_list_id,
        variable_item_list_get_view(key_config->item_list));
}

void key_config_register_text_input(KeyConfig* key_config, uint32_t text_input_id) {
    furi_assert(key_config->view_dispatcher != NULL);
    key_config->view_text_input_id = text_input_id;
    view_dispatcher_add_view(
        key_config->view_dispatcher,
        key_config->view_text_input_id,
        text_input_get_view(key_config->text_input));
}

void key_config_free(KeyConfig* key_config) {
    if(key_config->view_dispatcher != NULL) {
        if(key_config->view_item_list_id) {
            view_dispatcher_remove_view(
                key_config->view_dispatcher, key_config->view_item_list_id);
        }

        if(key_config->view_text_input_id) {
            view_dispatcher_remove_view(
                key_config->view_dispatcher, key_config->view_text_input_id);
        }
    }
    variable_item_list_free(key_config->item_list);
    submenu_free(key_config->menu_keys);
    text_input_free(key_config->text_input);
    free(key_config);
}