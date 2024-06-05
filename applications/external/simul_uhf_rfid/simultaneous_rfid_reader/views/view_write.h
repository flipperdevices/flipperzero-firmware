#pragma once

#include "../app.h"

//Function Declarations
void uhf_reader_view_write_draw_callback(Canvas* canvas, void* model);

void uhf_reader_view_write_timer_callback(void* context);

void uhf_reader_view_write_enter_callback(void* context);

void uhf_reader_view_write_exit_callback(void* context);

bool uhf_reader_view_write_custom_event_callback(uint32_t event, void* context);

bool uhf_reader_view_write_input_callback(InputEvent* event, void* context);

void uhf_reader_epc_value_text_updated(void* context);

uint32_t uhf_reader_navigation_write_callback(void* context);

uint32_t uhf_reader_navigation_write_exit_callback(void* context);

void view_write_alloc(UHFReaderApp* App);

void view_write_free(UHFReaderApp* App);