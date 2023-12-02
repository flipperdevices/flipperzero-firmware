// Based on https://github.com/csBlueChip/FlipperZero_plugin_howto
// C libs
#include <string.h>

// System libs
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>

// FlipperZero libs
#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <input/input.h>

#define LOG_LEVEL 6
#include "logging.h"

// Local headers
#include "extra_demo.h"
#include "err.h"

#include "EXTRA/extra.h"

bool run = true;

static void cbInput(InputEvent* event, FuriMessageQueue* queue) {
    ENTER;
    furi_assert(queue);
    furi_assert(event);

    // Put an "input" event message on the message queue
    eventMsg_t message = {.id = EVID_KEY, .input = *event};
    furi_message_queue_put(queue, &message, FuriWaitForever);

    LEAVE;
    return;
}
static void cbDraw(Canvas* const canvas, void* ctx) {
    ENTER;
    furi_assert(canvas);
    furi_assert(ctx);

    // There are several built-in fonts available
    //   {FontPrimary, FontSecondary, FontKeyboard, FontBigNumbers}
    // --> applications/gui/canvas.c
    canvas_set_font(canvas, FontSecondary);
    if(extra_registered()) {
        canvas_draw_str(canvas, 0, 12, "Registered!");
    } else {
        canvas_draw_str(canvas, 0, 12, "Press OK to register");
        canvas_draw_str(canvas, 0, 24, "extra commands");
    }

    LEAVE;
    return;
}

static void evTick() {
    ENTER;

    LEAVE;
    return;
}

static inline bool evKey(eventMsg_t* msg, Gui* gui) {
    ENTER;
    furi_assert(msg);
    furi_assert(gui);

    if(msg->input.type == InputTypeRelease && msg->input.key == InputKeyBack) {
        run = false;
        return false; // Signal the plugin to exit
    }
    if(msg->input.type == InputTypeRelease && msg->input.key == InputKeyOk) {
        extra_init_register();
    }
    LEAVE;
    return true;
}

int32_t extra_demo(void) {
    ENTER;

    err_t error = 0; // assume success
    Gui* gui = NULL;
    ViewPort* vpp = NULL;

    // This plugin will register two callbacks
    // The key reading ("input") and timer ("tick") callback functions will put keystrokes in a local message queue
    // --> furi/core/message_queue.h
    FuriMessageQueue* queue = NULL;
    const uint32_t queueSz = 8; // maximum messages in queue

    // The queue will contain plugin event-messages
    // --> local
    eventMsg_t msg = {0};

    INFO("BEGIN");

    // ===== Message queue =====

    // 1. Create a message queue (for up to 8 (keyboard) event messages)
    if(!(queue = furi_message_queue_alloc(queueSz, sizeof(msg)))) {
        ERROR(errs[(error = ERR_MALLOC_QUEUE)]);
        goto bail;
    }

    // ===== Create GUI Interface =====

    // 2. Create a GUI interface
    if(!(gui = furi_record_open("gui"))) {
        ERROR(errs[(error = ERR_NO_GUI)]);
        goto bail;
    }

    // ===== Viewport =====

    // 6. Allocate space on the heap for the viewport
    if(!(vpp = view_port_alloc())) {
        ERROR(errs[(error = ERR_MALLOC_VIEW)]);
        goto bail;
    }

    // 7a. Register a callback for input events
    view_port_input_callback_set(vpp, cbInput, queue);

    // 7b. Register a callback for draw events
    view_port_draw_callback_set(vpp, cbDraw, queue);

    // ===== Start GUI Interface =====

    // 8. Attach the viewport to the GUI
    gui_add_view_port(gui, vpp, GuiLayerFullscreen);

    INFO("INITIALISED");

    // ==================== Main event loop ====================

    if(run) do {
            // Try to read a message from the queue
            // Our run-loop does not poll and is not "busy"
            //   but there is no "do not timeout"/"wait for message"
            //   so we need to use a large timeout and ignore timeout messages
            // --> furi/core/base.h
            FuriStatus status = FuriStatusErrorTimeout;
            while((status = furi_message_queue_get(queue, &msg, 60000)) == FuriStatusErrorTimeout)
                ;

            // Read failed
            if(status != FuriStatusOk) {
                switch(status) {
                case FuriStatusErrorTimeout:
                    DEBUG(errs[DEBUG_QUEUE_TIMEOUT]);
                    break;
                case FuriStatusError:
                    ERROR(errs[(error = ERR_QUEUE_RTOS)]);
                    goto bail;
                case FuriStatusErrorResource:
                    ERROR(errs[(error = ERR_QUEUE_RESOURCE)]);
                    goto bail;
                case FuriStatusErrorParameter:
                    ERROR(errs[(error = ERR_QUEUE_BADPRM)]);
                    goto bail;
                case FuriStatusErrorNoMemory:
                    ERROR(errs[(error = ERR_QUEUE_NOMEM)]);
                    goto bail;
                case FuriStatusErrorISR:
                    ERROR(errs[(error = ERR_QUEUE_ISR)]);
                    goto bail;
                default:
                    ERROR(errs[(error = ERR_QUEUE_UNK)]);
                    goto bail;
                }

                // Read successful
            } else {
                // *** Handle events ***
                switch(msg.id) {
                // ------------------------------------------------------------
                // Timer events
                case EVID_TICK:
                    evTick(); // Animation runs every "tick"
                    break;

                // ------------------------------------------------------------
                // Key events
                case EVID_KEY:
                    run = evKey(&msg, gui);
                    break;

                // ------------------------------------------------------------
                // Unknown event
                default:
                    WARN("Unknown message.ID [%d]", msg.id);
                    break;
                }

                // Update the GUI screen via the viewport
                view_port_update(vpp);
            } // if (ok)
        } while(run);
    INFO("USER EXIT");

// Oh no! A horrible scary "goto" label!
// Quick! Call the Spanish Inquisition! ...Bring the comfy chair!
bail:
    // Unregister commands
    extra_deinit_unregister();

    // Detach the viewport
    gui_remove_view_port(gui, vpp);

    // Destroy the viewport
    if(vpp) {
        view_port_enabled_set(vpp, false);
        view_port_free(vpp);
        vpp = NULL;
    }

    // Close the GUI
    furi_record_close("gui");

    // Destroy the message queue
    if(queue) {
        furi_message_queue_free(queue);
        queue = NULL;
    }

    INFO("CLEAN EXIT ... Exit code: %d", error);
    LEAVE;
    return (int32_t)error;
}
