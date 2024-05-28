#pragma once
#include "../app.h"

//Function Declarations
void uhf_reader_view_read_draw_callback(Canvas* canvas, void* model);

bool uhf_reader_view_read_input_callback(InputEvent* event, void* context);

uint32_t uhf_reader_navigation_read_submenu_callback(void* context);

uint32_t uhf_reader_navigation_read_callback(void* context);

void uhf_reader_view_read_enter_callback(void* context);

void uhf_reader_view_read_exit_callback(void* context);

bool uhf_reader_view_read_custom_event_callback(uint32_t event, void* context);

void uhf_reader_view_read_timer_callback(void* context);

void uhf_reader_save_text_updated(void* context);

void view_read_alloc(UHFReaderApp* App);

void view_read_free(UHFReaderApp* App);

void saved_input_alloc(UHFReaderApp* App);