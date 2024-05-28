#pragma once
#include "../app.h"

//Function Declarations
void uhf_reader_submenu_tag_info_callback(void* context, uint32_t index);

uint32_t uhf_reader_navigation_epc_info_callback(void* context);

void uhf_reader_rename_text_updated(void* context);

uint32_t uhf_reader_navigation_tag_action_exit_callback(void* context);

void view_tag_actions_alloc(UHFReaderApp* App);

void view_tag_actions_free(UHFReaderApp* App);