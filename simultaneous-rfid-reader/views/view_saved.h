#pragma once
#include "../app.h"

//Function Declarations
uint32_t uhf_reader_navigation_saved_exit_callback(void* _context);

void uhf_reader_submenu_saved_callback(void* context, uint32_t index);

void view_saved_menu_alloc(UHFReaderApp* app);

void view_saved_free(UHFReaderApp* app);