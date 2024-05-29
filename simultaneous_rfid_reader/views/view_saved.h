#pragma once
#include "../app.h"

//Function Declarations
uint32_t uhf_reader_navigation_saved_exit_callback(void* context);

void uhf_reader_submenu_saved_callback(void* context, uint32_t index);

void view_saved_menu_alloc(UHFReaderApp* App);

void view_saved_free(UHFReaderApp* App);