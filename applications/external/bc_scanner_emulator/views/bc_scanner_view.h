#pragma once

#include <gui/view.h>
#include "../bc_scanner_script.h"

typedef struct BarCodeView BarCodeView;
typedef void (*BarCodeOkCallback)(InputType type, void* context);

BarCodeView* bc_scanner_alloc();

void bc_scanner_free(BarCodeView* bar_code);

View* bc_scanner_get_view(BarCodeView* bar_code);

void bc_scanner_set_ok_callback(BarCodeView* bar_code, BarCodeOkCallback callback, void* context);

void bc_scanner_set_file_name(BarCodeView* bar_code, const char* name);

void bc_scanner_set_state(BarCodeView* bar_code, BarCodeState* st);
