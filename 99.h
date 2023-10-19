#include <stdio.h>
#include <furi.h>
#include <gui/gui.h>

typedef struct {
    Gui* gui;
    ViewPort* view_port;
    FuriMessageQueue* event_queue;
} BeerApp;
