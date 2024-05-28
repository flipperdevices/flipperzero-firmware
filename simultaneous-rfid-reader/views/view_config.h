#pragma once

#include "../app.h"

//Function Declarations
uint32_t uhf_reader_navigation_configure_callback(void* _context);

void uhf_reader_setting_1_change(VariableItem* item);


void uhf_reader_setting_2_text_updated(void* context);

uint32_t uhf_reader_navigation_config_submenu_callback(void* _context);

void uhf_reader_setting_3_change(VariableItem* item);

void uhf_reader_setting_item_clicked(void* context, uint32_t index);

void view_config_alloc(UHFReaderApp* app);

void view_config_free(UHFReaderApp* app);

void power_menu_alloc(UHFReaderApp* app);