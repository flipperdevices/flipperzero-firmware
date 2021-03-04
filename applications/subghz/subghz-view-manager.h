#pragma once
#include <furi.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/submenu.h>
#include "subghz-event.h"

class SubghzAppViewManager {
public:
    enum class ViewType : uint8_t {
        Submenu,
    };

    osMessageQueueId_t event_queue;

    SubghzAppViewManager();
    ~SubghzAppViewManager();

    void switch_to(ViewType type);

    Submenu* get_submenu();

    void receive_event(SubghzEvent* event);
    void send_event(SubghzEvent* event);

private:
    ViewDispatcher* view_dispatcher;
    Gui* gui;

    uint32_t previous_view_callback(void* context);

    // view elements
    Submenu* submenu;
};