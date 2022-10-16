#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <input/input.h>
#include <stdlib.h>

#include "assets_icons.h"

typedef enum {
    EventTypeTick,
    EventTypeKey,
} EventType;

typedef struct {
    EventType type;
    InputEvent input;
} PluginEvent;

static void input_callback(InputEvent* input_event, FuriMessageQueue* event_queue) {
    furi_assert(event_queue);

    PluginEvent event = {.type = EventTypeKey, .input = *input_event};
    furi_message_queue_put(event_queue, &event, FuriWaitForever);
}

static void render_callback(Canvas* const canvas, void* ctx) {
  UNUSED(ctx);

//  canvas_draw_xbm(canvas, 0, 0, dino_width, dino_height, dino_bits);
  canvas_draw_icon(canvas, 0, 0, &I_dino);
}

int32_t trexrunner_app(void* p) {
  UNUSED(p);

  FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(PluginEvent));

//  ValueMutex state_mutex;
//  if (!init_mutex(&state_mutex, minesweeper_state, sizeof(minesweeper_state))) {
//      FURI_LOG_E("t-rex runner", "cannot create mutex\r\n");
//        free(minesweeper_state);
//      return 255;
//  }
  // BEGIN IMPLEMENTATION

  // Set system callbacks
  ViewPort* view_port = view_port_alloc();
  view_port_draw_callback_set(view_port, render_callback, view_port);
  view_port_input_callback_set(view_port, input_callback, event_queue);

  // Open GUI and register view_port
  Gui* gui = furi_record_open("gui");
  gui_add_view_port(gui, view_port, GuiLayerFullscreen);

  PluginEvent event;
  for (bool processing = true; processing;) {
    FuriStatus event_status = furi_message_queue_get(event_queue, &event, 100);
//    Minesweeper* minesweeper_state = (Minesweeper*)acquire_mutex_block(&state_mutex);
    if (event_status == FuriStatusOk) {
      // press events
      if (event.type == EventTypeKey) {
        if (event.input.type == InputTypeShort) {
          switch (event.input.key) {
          case InputKeyUp: break;
          case InputKeyDown: break;
          case InputKeyLeft: break;
          case InputKeyRight: break;
          case InputKeyOk: break;
          case InputKeyBack:
            // Exit the app
            processing = false;
            break;
          }
        }
      }
    } else {
      // event timeout
      ;
    }
    view_port_update(view_port);
//    release_mutex(&state_mutex, minesweeper_state);
  }

  view_port_enabled_set(view_port, false);
  gui_remove_view_port(gui, view_port);
  furi_record_close("gui");
  view_port_free(view_port);
  furi_message_queue_free(event_queue);
//  delete_mutex(&state_mutex);
//  furi_timer_free(minesweeper_state->timer);
//  free(minesweeper_state);

  return 0;
}
