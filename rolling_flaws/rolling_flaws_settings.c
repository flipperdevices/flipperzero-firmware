#include "rolling_flaws_settings.h"

void rolling_flaws_setting_change(VariableItem *item, char **names, uint8_t *new_index)
{
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, names[index]);
    *new_index = index;
}

uint32_t setting_frequency_values[] = {315000000, 390000000, 433920000};
char *setting_frequency_names[] = {"315.00", "390.00", "433.92"};
void rolling_flaws_setting_frequency_change(VariableItem *item)
{
    RollingFlaws *app = variable_item_get_context(item);
    rolling_flaws_setting_change(item, setting_frequency_names, &app->model->frequency_index);
}
char *rolling_flaws_setting_frequency_name_get(RollingFlawsModel *model)
{
    return setting_frequency_names[model->frequency_index];
}
uint32_t rolling_flaws_setting_frequency_get(RollingFlawsModel *model)
{
    return setting_frequency_values[model->frequency_index];
}

uint32_t setting_fix_values[] = {0x20000000, 0x201EA8D8, 0x284EE9D5, 0xCAFECAFE};
char *setting_fix_names[] = {"0x20000000", "0x201EA8D8", "0x284EE9D5", "Custom"};
void rolling_flaws_setting_fix_change(VariableItem *item)
{
    RollingFlaws *app = variable_item_get_context(item);
    rolling_flaws_setting_change(item, setting_fix_names, &app->model->fix_index);
}
uint32_t rolling_flaws_setting_fix_get(RollingFlawsModel *model)
{
    if (model->fix_index == COUNT_OF(setting_fix_values) - 1)
    {
        return model->custom_fix;
    }
    return setting_fix_values[model->fix_index];
}
char *rolling_flaws_setting_fix_display_name_get(RollingFlawsModel *model)
{
    return setting_fix_names[model->fix_index];
}

char *setting_protocol_values_mf_name[] = {"DoorHan", "*", "Custom"};
char *setting_protocol_values_base_name[] = {"KeeLoq 64bit", "KeeLoq 64bit", "KeeLoq 64bit"};
char *setting_protocol_names[] = {"KL (DH)", "KL (All)", "KL (Custom)"};
void rolling_flaws_setting_protocol_change(VariableItem *item)
{
    RollingFlaws *app = variable_item_get_context(item);
    rolling_flaws_setting_change(item, setting_protocol_names, &app->model->protocol_index);
}
char *rolling_flaws_setting_protocol_base_name_get(RollingFlawsModel *model)
{
    return setting_protocol_values_base_name[model->protocol_index];
}
char *rolling_flaws_setting_protocol_display_name_get(RollingFlawsModel *model)
{
    return setting_protocol_names[model->protocol_index];
}
const char *rolling_flaws_setting_protocol_mf_name_get(RollingFlawsModel *model)
{
    if (model->protocol_index == COUNT_OF(setting_protocol_values_mf_name) - 1)
    {
        return furi_string_get_cstr(model->custom_mf);
    }
    return setting_protocol_values_mf_name[model->protocol_index];
}
void rolling_flaws_setting_protocol_custom_mf_set(RollingFlawsModel *model, FuriString *mf)
{
    model->protocol_index = COUNT_OF(setting_protocol_values_mf_name) - 1;
    variable_item_set_current_value_index(model->variable_item_protocol, model->protocol_index);
    variable_item_set_current_value_text(
        model->variable_item_protocol, rolling_flaws_setting_protocol_display_name_get(model));

    model->fix_index = COUNT_OF(setting_fix_values) - 1;
    variable_item_set_current_value_index(model->variable_item_fix, model->fix_index);
    variable_item_set_current_value_text(
        model->variable_item_fix, rolling_flaws_setting_fix_display_name_get(model));

    furi_string_set(model->custom_mf, furi_string_get_cstr(mf));
}

bool setting_replay_values[] = {false, true};
char *setting_replay_names[] = {"No", "Yes"};
void rolling_flaws_setting_replay_change(VariableItem *item)
{
    RollingFlaws *app = variable_item_get_context(item);
    rolling_flaws_setting_change(item, setting_replay_names, &app->model->replay_index);
}
bool rolling_flaws_setting_replay_get(RollingFlawsModel *model)
{
    return setting_replay_values[model->replay_index];
}

/// @brief The window_next_values have precedence over past/future values.
uint32_t setting_window_next_values[] = {4, 8, 16, 256, 16384, 32768, 65536};
char *setting_window_next_names[] = {"4", "8", "16", "256", "16384", "32768", "All"};
void rolling_flaws_setting_window_next_change(VariableItem *item)
{
    RollingFlaws *app = variable_item_get_context(item);
    rolling_flaws_setting_change(item, setting_window_next_names, &app->model->window_next_index);
}
uint32_t rolling_flaws_setting_window_next_get(RollingFlawsModel *model)
{
    return setting_window_next_values[model->window_next_index];
}

uint32_t setting_window_future_values[] = {1, 8, 16, 256, 16384, 32768, 65536};
char *setting_window_future_names[] = {"1", "8", "16", "256", "16384", "32768", "All"};
void rolling_flaws_setting_window_future_change(VariableItem *item)
{
    RollingFlaws *app = variable_item_get_context(item);
    rolling_flaws_setting_change(
        item, setting_window_future_names, &app->model->window_future_index);
}
uint32_t rolling_flaws_setting_window_future_get(RollingFlawsModel *model)
{
    return setting_window_future_values[model->window_future_index];
}

uint32_t setting_window_future_gap_values[] = {1, 2, 3, 4};
char *setting_window_future_gap_names[] = {"1", "2", "3", "4"};
void rolling_flaws_setting_window_future_gap_change(VariableItem *item)
{
    RollingFlaws *app = variable_item_get_context(item);
    rolling_flaws_setting_change(
        item, setting_window_future_gap_names, &app->model->window_future_gap_index);
}
uint32_t rolling_flaws_setting_window_future_gap_get(RollingFlawsModel *model)
{
    return setting_window_future_gap_values[model->window_future_gap_index];
}

bool setting_sn_zero_values[] = {false, true};
char *setting_sn_zero_names[] = {"No", "Yes"};
void rolling_flaws_setting_sn_zero_change(VariableItem *item)
{
    RollingFlaws *app = variable_item_get_context(item);
    rolling_flaws_setting_change(item, setting_sn_zero_names, &app->model->sn_zero_index);
}
bool rolling_flaws_setting_sn_zero_get(RollingFlawsModel *model)
{
    return setting_sn_zero_values[model->sn_zero_index];
}

// HCS300 uses 10 bits in discriminator, HCS200 uses 8 bits
uint8_t setting_sn_bits_values[] = {8, 10};
char *setting_sn_bits_names[] = {"8", "10 (dec)"};
void rolling_flaws_setting_sn_bits_change(VariableItem *item)
{
    RollingFlaws *app = variable_item_get_context(item);
    rolling_flaws_setting_change(item, setting_sn_bits_names, &app->model->sn_bits_index);
}
uint8_t rolling_flaws_setting_sn_bits_get(RollingFlawsModel *model)
{
    return setting_sn_bits_values[model->sn_bits_index];
}

bool setting_count_zero_values[] = {false, true};
char *setting_count_zero_names[] = {"No", "Yes"};
void rolling_flaws_setting_count_zero_change(VariableItem *item)
{
    RollingFlaws *app = variable_item_get_context(item);
    rolling_flaws_setting_change(item, setting_sn_zero_names, &app->model->count_zero_index);
}
bool rolling_flaws_setting_count_zero_get(RollingFlawsModel *model)
{
    return setting_count_zero_values[model->count_zero_index];
}

void rolling_flaw_populate_variable_item_list(RollingFlaws *app)
{
    app->variable_item_list_config = variable_item_list_alloc();
    variable_item_list_reset(app->variable_item_list_config);

    VariableItem *item;
    item = variable_item_list_add(
        app->variable_item_list_config,
        "Frequency",
        COUNT_OF(setting_frequency_names),
        rolling_flaws_setting_frequency_change,
        app);
    app->model->frequency_index = 2; // 433.92
    variable_item_set_current_value_index(item, app->model->frequency_index);
    variable_item_set_current_value_text(
        item, setting_frequency_names[app->model->frequency_index]);

    item = variable_item_list_add(
        app->variable_item_list_config,
        "Protocol",
        COUNT_OF(setting_protocol_names),
        rolling_flaws_setting_protocol_change,
        app);
    app->model->protocol_index = 0; // KeeLoq (DoorHan)
    variable_item_set_current_value_index(item, app->model->protocol_index);
    variable_item_set_current_value_text(item, setting_protocol_names[app->model->protocol_index]);
    app->model->variable_item_protocol = item;

    item = variable_item_list_add(
        app->variable_item_list_config,
        "Fix [Btn+SN]",
        COUNT_OF(setting_fix_names),
        rolling_flaws_setting_fix_change,
        app);
    app->model->fix_index = 2; // 0x284EE9D5
    variable_item_set_current_value_index(item, app->model->fix_index);
    variable_item_set_current_value_text(item, setting_fix_names[app->model->fix_index]);
    app->model->variable_item_fix = item;

    item = variable_item_list_add(
        app->variable_item_list_config,
        "Replay attack",
        COUNT_OF(setting_replay_names),
        rolling_flaws_setting_replay_change,
        app);
    app->model->replay_index = 0; // Disabled
    variable_item_set_current_value_index(item, app->model->replay_index);
    variable_item_set_current_value_text(item, setting_replay_names[app->model->replay_index]);

    item = variable_item_list_add(
        app->variable_item_list_config,
        "Window [next]",
        COUNT_OF(setting_window_next_names),
        rolling_flaws_setting_window_next_change,
        app);
    app->model->window_next_index = 2; // 16 codes.
    variable_item_set_current_value_index(item, app->model->window_next_index);
    variable_item_set_current_value_text(
        item, setting_window_next_names[app->model->window_next_index]);

    item = variable_item_list_add(
        app->variable_item_list_config,
        "Window [future]",
        COUNT_OF(setting_window_future_names),
        rolling_flaws_setting_window_future_change,
        app);
    app->model->window_future_index = 5; // 32768 codes.
    variable_item_set_current_value_index(item, app->model->window_future_index);
    variable_item_set_current_value_text(
        item, setting_window_future_names[app->model->window_future_index]);

    item = variable_item_list_add(
        app->variable_item_list_config,
        "Window [gap]",
        COUNT_OF(setting_window_future_gap_names),
        rolling_flaws_setting_window_future_gap_change,
        app);
    app->model->window_future_gap_index = 1; // 2 codes.
    variable_item_set_current_value_index(item, app->model->window_future_gap_index);
    variable_item_set_current_value_text(
        item, setting_window_future_gap_names[app->model->window_future_gap_index]);

    item = variable_item_list_add(
        app->variable_item_list_config,
        "SN00/cfw*",
        COUNT_OF(setting_sn_zero_names),
        rolling_flaws_setting_sn_zero_change,
        app);
    app->model->sn_zero_index = 0; // Disabled
    variable_item_set_current_value_index(item, app->model->sn_zero_index);
    variable_item_set_current_value_text(item, setting_sn_zero_names[app->model->sn_zero_index]);

    item = variable_item_list_add(
        app->variable_item_list_config,
        "SN bits/cfw*",
        COUNT_OF(setting_sn_bits_names),
        rolling_flaws_setting_sn_bits_change,
        app);
    app->model->sn_bits_index = 0; // 8-bits
    variable_item_set_current_value_index(item, app->model->sn_bits_index);
    variable_item_set_current_value_text(item, setting_sn_bits_names[app->model->sn_bits_index]);

    item = variable_item_list_add(
        app->variable_item_list_config,
        "Count 0 opens",
        COUNT_OF(setting_count_zero_names),
        rolling_flaws_setting_count_zero_change,
        app);
    app->model->count_zero_index = 0; // No - count of 0 is not an open.
    variable_item_set_current_value_index(item, app->model->count_zero_index);
    variable_item_set_current_value_text(
        item, setting_count_zero_names[app->model->count_zero_index]);
}