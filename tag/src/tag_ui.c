#include "tag_ui.h"
#include "tag_app.h"

static void tag_ui_render_mode(Canvas* canvas, TagAppState* state) {
    // all other modes
    const char* heading;
    const char* detail = "This app is not playable yet.";
    switch(state->mode) {
    case TagAppModeUninitialised:
        heading = "Uninitialised";
        break;
    case TagAppModeReady:
        heading = "Ready";
        break;
    case TagAppModeFinished:
        heading = "Finished";
        break;
    case TagAppModeError:
        // TODO: use error mode to explain any known issues to the player
        heading = "Error";
        break;
    case TagAppModeQuit:
        // unlikely to ever be seen by the player
        heading = "Exiting";
        break;
    default:
        FURI_LOG_E(TAG, "unexpected app mode");
        heading = "Unexpected app mode";
        detail = "---";
        break;
    }

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 5, 8, AlignLeft, AlignTop, heading);
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_aligned(canvas, 5, 18, AlignLeft, AlignTop, detail);

    canvas_set_font(canvas, FontSecondary);
    if(state->data->last_ir_address > 0) {
        furi_string_printf(
            state->data->string_buffer,
            "add: %lu, cmd: %lu", // %lu = long unsigned
            state->data->last_ir_address,
            state->data->last_ir_command);
        canvas_draw_str_aligned(
            canvas, 5, 20, AlignLeft, AlignTop, furi_string_get_cstr(state->data->string_buffer));
    } else {
        canvas_draw_str_aligned(canvas, 5, 28, AlignLeft, AlignTop, "---");
    }
}

#define HEALTH_BAR_left 67
#define HEALTH_BAR_width 50
#define HEALTH_BAR_bottom 48
#define HEALTH_BAR_height 8

static void tag_ui_render_play(Canvas* canvas, TagAppState* state) {
    FURI_LOG_T(TAG, "tag_ui_render_play");
    UNUSED(state);
    canvas_draw_icon(canvas, 0, 0, &I_background);

    if(state->data->energy < ENERGY_MAX) {
        uint8_t removal_width =
            state->data->energy * ((float)HEALTH_BAR_width / (float)ENERGY_MAX);
        canvas_set_color(canvas, ColorWhite);

        char* comment;
        asprintf(&comment, "nrg: %f", (double)state->data->energy); // TODO: no worky
        canvas_draw_str(canvas, 0, 20, comment);

        canvas_draw_box(
            canvas,
            (HEALTH_BAR_left + HEALTH_BAR_width) - removal_width,
            HEALTH_BAR_bottom,
            removal_width,
            HEALTH_BAR_height);
    }
}

static void tag_ui_render_callback(Canvas* canvas, void* context) {
    FURI_LOG_T(TAG, "tag_ui_render_callback");
    FURI_LOG_D(TAG, "tag_ui_render_callback assertion: context");
    furi_assert(context);
    TagAppState* state = context;

    if(furi_mutex_acquire(state->data_mutex, 200) != FuriStatusOk) {
        return; // try again next callback
    }

    // configure display based on mode and status
    if(state->mode == TagAppModePlaying) {
        tag_ui_render_play(canvas, state);
    } else {
        tag_ui_render_mode(canvas, state);
    }

    // always release the mutex
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
