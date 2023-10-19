#pragma once

#include <stdio.h>
#include <furi.h>
#include <gui/gui.h>
#include <gui/elements.h>

typedef struct {
    Gui* gui;
    ViewPort* view_port;
    FuriMessageQueue* event_queue;
    uint8_t pos;
} BeerApp;
