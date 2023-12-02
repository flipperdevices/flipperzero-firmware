#pragma once

#include <furi.h>
#include "rolling_flaws_structs.h"

uint32_t rolling_flaws_setting_frequency_get(RollingFlawsModel* model);
char* rolling_flaws_setting_frequency_name_get(RollingFlawsModel* model);
char* rolling_flaws_setting_protocol_base_name_get(RollingFlawsModel* model);
char* rolling_flaws_setting_protocol_display_name_get(RollingFlawsModel* model);
const char* rolling_flaws_setting_protocol_mf_name_get(RollingFlawsModel* model);
void rolling_flaws_setting_protocol_custom_mf_set(RollingFlawsModel* model, FuriString* mf);
bool rolling_flaws_setting_replay_get(RollingFlawsModel* model);
uint32_t rolling_flaws_setting_window_next_get(RollingFlawsModel* model);
uint32_t rolling_flaws_setting_window_future_get(RollingFlawsModel* model);
uint32_t rolling_flaws_setting_window_future_gap_get(RollingFlawsModel* model);
uint32_t rolling_flaws_setting_fix_get(RollingFlawsModel* model);
bool rolling_flaws_setting_sn_zero_get(RollingFlawsModel* model);
uint8_t rolling_flaws_setting_sn_bits_get(RollingFlawsModel* model);
bool rolling_flaws_setting_count_zero_get(RollingFlawsModel* model);

void rolling_flaw_populate_variable_item_list(RollingFlaws* app);