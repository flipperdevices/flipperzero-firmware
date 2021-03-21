#pragma once
#include <furi.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/submenu.h>
#include "lf-rfid-event.h"

class LfrfidAppViewManager {
public:
    enum class ViewType : uint8_t {
        Submenu,
    };

    osMessageQueueId_t event_queue;

    LfrfidAppViewManager();
    ~LfrfidAppViewManager();

    void switch_to(ViewType type);

    void receive_event(LfrfidEvent* event);
    void send_event(LfrfidEvent* event);

    Submenu* get_submenu();

private:
    ViewDispatcher* view_dispatcher;
    Gui* gui;

    uint32_t previous_view_callback(void* context);

    // view elements
    Submenu* submenu;
};