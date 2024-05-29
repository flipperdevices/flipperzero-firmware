#pragma once
#include "../app.h"

//Function Declarations
void uhf_reader_view_epc_info_draw_callback(Canvas* canvas, void* model);

uint32_t uhf_reader_navigation_exit_epc_info_callback(void* context);

void uhf_reader_view_epc_info_enter_callback(void* context);

void uhf_reader_view_epc_info_exit_callback(void* context);

void uhf_reader_view_epc_info_timer_callback(void* context);

void view_epc_info_alloc(UHFReaderApp* App);

void view_epc_info_free(UHFReaderApp* App);