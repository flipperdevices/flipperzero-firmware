/**
MIT License

Copyright (c) 2023 Vojtech Suk (https://github.com/sukvojte)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#pragma once

#include <gui/view.h>

typedef struct ViewFlasher ViewFlasher;

typedef enum {
    ViewFlasher_Action_ChipInfo = 0,
    ViewFlasher_Action_OpenFile,
    ViewFlasher_Action_EraseChip,
    ViewFlasher_Action_WriteChip,
    ViewFlasher_ActionCount,
} ViewFlasher_View_Action;

typedef void (*ViewFlasherDoActionCallback)(void* context, ViewFlasher_View_Action action);

void view_flasher_register_action_callback(
    ViewFlasher* handle,
    ViewFlasherDoActionCallback cb,
    void* cb_context);

ViewFlasher* view_flasher_alloc();

void view_flasher_free(ViewFlasher* handle);

View* view_flasher_get_view(ViewFlasher* flasher);

void view_flasher_set_animation_status(ViewFlasher* handle, uint8_t enabled);

void view_flasher_display_text(ViewFlasher* handle, const char* message);

void view_flasher_display_clear_text(ViewFlasher* handle);