#pragma once

#include "../app.h"

//Function Declarations
void uhf_reader_view_delete_draw_callback(Canvas* canvas, void* model);

bool uhf_reader_view_delete_input_callback(InputEvent* event, void* context);

uint32_t uhf_reader_navigation_delete_exit_callback(void* context);

void uhf_reader_view_delete_enter_callback(void* context);

void uhf_reader_view_delete_exit_callback(void* context);

void view_delete_alloc(UHFReaderApp* App);

void view_delete_free(UHFReaderApp* App);