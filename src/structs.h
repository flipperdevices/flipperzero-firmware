#ifndef __structs_h__
#define __structs_h__

#include <gui/gui.h>
#include <gui/view_port.h>
#include <core/thread.h>
#include <core/message_queue.h>

/* Application context structure */
struct ApplicationContext {
    Gui* gui;
    ViewPort* view_port;
    FuriThread* secondary_thread;
    FuriMessageQueue* event_queue;
};

#endif
