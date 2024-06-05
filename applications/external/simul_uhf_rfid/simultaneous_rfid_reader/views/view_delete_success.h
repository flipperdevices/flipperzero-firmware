#pragma once
#include "../app.h"

//Function Declarations
void uhf_reader_view_delete_success_draw_callback(Canvas* canvas, void* model);

bool uhf_reader_view_delete_success_input_callback(InputEvent* event, void* context);

void uhf_reader_view_delete_success_enter_callback(void* context);

void uhf_reader_view_delete_success_exit_callback(void* context);

void view_delete_success_alloc(UHFReaderApp* App);

void view_delete_success_free(UHFReaderApp* App);