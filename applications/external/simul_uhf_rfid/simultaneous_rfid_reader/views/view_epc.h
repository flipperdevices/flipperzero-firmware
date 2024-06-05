#pragma once
#include "../app.h"

//Function Declarations
void uhf_reader_view_epc_timer_callback(void* context);

void uhf_reader_view_epc_draw_callback(Canvas* canvas, void* model);

uint32_t uhf_reader_navigation_exit_view_epc_callback(void* context);

void uhf_reader_view_epc_enter_callback(void* context);

void uhf_reader_view_epc_exit_callback(void* context);

void view_epc_alloc(UHFReaderApp* App);

void view_epc_free(UHFReaderApp* App);