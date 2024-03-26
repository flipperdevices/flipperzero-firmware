#pragma once

#include <furi.h>
#include <gui/gui.h>
#include <input/input.h>
#include <notification/notification_messages.h>

#include "usb/usb_midi_driver.h"

typedef struct {
    Gui* gui;
    ViewPort* view_port;
    FuriMessageQueue* event_queue;
    uint8_t midi_note;
    NotificationApp* notifications;
    bool should_exit;
} TestMidiApp;

void test_midi_draw_callback(Canvas* canvas, void* ctx);
void test_midi_input_callback(InputEvent* input_event, void* ctx);

TestMidiApp* test_midi_alloc();
void test_midi_free(TestMidiApp* app);

int32_t test_midi_app();