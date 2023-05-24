#include "tag_ui.h"
#include "tag_app.h"

static void tag_ui_render_callback(Canvas* canvas, void* context) {
    FURI_LOG_T(TAG, "tag_ui_render_callback");
    FURI_LOG_D(TAG, "tag_ui_render_callback assertion: context");
    furi_assert(context);
    TagAppState* state = context;

    if(furi_mutex_acquire(state->data_mutex, 200) != FuriStatusOk) {
        return; // try again next callback
    }

    const char* heading;
    switch(state->mode) {
    case TagAppModeUninitialised:
        heading = "Uninitialised";
        break;
    case TagAppModeReady:
        heading = "Ready";
        break;
    case TagAppModePlaying:
        heading = "Playing";
        break;
    case TagAppModeFinished:
        heading = "Finished";
        break;
    case TagAppModeError:
        // TODO: use error mode to explain known issues to the player
        heading = "Error";
        break;
    case TagAppModeQuit:
        // unlikely to ever be seen by the player
        heading = "Exiting";
        break;
    default:
        heading = "Unknown app state";
        break;
    }

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 5, 8, AlignLeft, AlignCenter, heading);

    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_aligned(canvas, 5, 20, AlignLeft, AlignCenter, "---");

    // seems to do string formatting into a buffer
    // furi_string_printf(data->buffer, "%04u", localCounter);
    // canvas_set_font(canvas, FontSecondary);
    // canvas_draw_str_aligned(
    //     canvas, 64, 42, AlignCenter, AlignTop, furi_string_get_cstr(data->buffer));

    furi_mutex_release(state->data_mutex);
}

static void tag_ui_input_callback(InputEvent* input_event, void* context_q) {
    FURI_LOG_T(TAG, "tag_ui_input_callback");
    FURI_LOG_I(TAG, "Input event received");
    FURI_LOG_D(TAG, "tag_ui_input_callback assertion: context_q");
    furi_assert(context_q);
    FuriMessageQueue* queue = context_q;
    TagEvent event = {.type = TagEventTypeInput, .input = *input_event};
    furi_message_queue_put(queue, &event, FuriWaitForever);
}

void tag_ui_init(TagAppState* state) {
    FURI_LOG_T(TAG, "tag_ui_init");
    state->view_port = view_port_alloc();
    view_port_draw_callback_set(state->view_port, tag_ui_render_callback, state);
    view_port_input_callback_set(state->view_port, tag_ui_input_callback, state->queue);
    state->gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(state->gui, state->view_port, GuiLayerFullscreen);
}

void tag_ui_destroy(TagAppState* state) {
    FURI_LOG_T(TAG, "tag_ui_destroy");
    view_port_enabled_set(state->view_port, false);
    gui_remove_view_port(state->gui, state->view_port);
    view_port_free(state->view_port);
    furi_record_close(RECORD_GUI);
}
