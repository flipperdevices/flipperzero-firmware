#include "../hex_viewer.h"
#include <furi.h>
#include <furi_hal.h>
#include <input/input.h>
#include <gui/elements.h>

struct HexViewerStartscreen {
    View* view;
    HexViewerStartscreenCallback callback;
    void* context;
};

typedef struct {
    uint8_t file_bytes[HEX_VIEWER_LINES_ON_SCREEN][HEX_VIEWER_BYTES_PER_LINE];
    uint32_t file_offset;
    uint32_t file_read_bytes;
    uint32_t file_size;
    bool mode;
    uint32_t dbg;
} HexViewerStartscreenModel;

void hex_viewer_startscreen_set_callback(
    HexViewerStartscreen* instance,
    HexViewerStartscreenCallback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(callback);
    instance->callback = callback;
    instance->context = context;
}

void hex_viewer_startscreen_draw(Canvas* canvas, HexViewerStartscreenModel* model) {
    canvas_clear(canvas);

    if(!model->file_size) {
        canvas_set_color(canvas, ColorBlack);
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str_aligned(canvas, 64, 10, AlignCenter, AlignTop, "HexViewer v2.0");
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str_aligned(canvas, 64, 22, AlignCenter, AlignTop, "Basic hex viewer");
        canvas_draw_str_aligned(canvas, 64, 32, AlignCenter, AlignTop, "for your Flipper");
        elements_button_center(canvas, "Open");
    } else {
        canvas_set_color(canvas, ColorBlack);

        elements_button_left(canvas, model->mode ? "Addr" : "Text");
        //elements_button_right(canvas, "Info");
        elements_button_center(canvas, "Menu");

        int ROW_HEIGHT = 12;
        int TOP_OFFSET = 10;
        int LEFT_OFFSET = 3;

        uint32_t line_count = model->file_size / HEX_VIEWER_BYTES_PER_LINE;
        if(model->file_size % HEX_VIEWER_BYTES_PER_LINE != 0) line_count += 1;
        uint32_t first_line_on_screen = model->file_offset / HEX_VIEWER_BYTES_PER_LINE;
        if(line_count > HEX_VIEWER_LINES_ON_SCREEN) {
            uint8_t width = canvas_width(canvas);
            elements_scrollbar_pos(
                canvas,
                width,
                0,
                ROW_HEIGHT * HEX_VIEWER_LINES_ON_SCREEN,
                first_line_on_screen, // TODO
                line_count - (HEX_VIEWER_LINES_ON_SCREEN - 1));
        }

        char temp_buf[32];
        uint32_t row_iters = model->file_read_bytes / HEX_VIEWER_BYTES_PER_LINE;
        if(model->file_read_bytes % HEX_VIEWER_BYTES_PER_LINE != 0) row_iters += 1;

        for(uint32_t i = 0; i < row_iters; ++i) {
            uint32_t bytes_left_per_row = model->file_read_bytes - i * HEX_VIEWER_BYTES_PER_LINE;
            bytes_left_per_row = MIN(bytes_left_per_row, HEX_VIEWER_BYTES_PER_LINE);

            if(model->mode) {
                memcpy(temp_buf, model->file_bytes[i], bytes_left_per_row);
                temp_buf[bytes_left_per_row] = '\0';
                for(uint32_t j = 0; j < bytes_left_per_row; ++j)
                    if(!isprint((int)temp_buf[j])) temp_buf[j] = '.';

                canvas_set_font(canvas, FontKeyboard);
                canvas_draw_str(canvas, LEFT_OFFSET, TOP_OFFSET + i * ROW_HEIGHT, temp_buf);
            } else {
                uint32_t addr = model->file_offset + i * HEX_VIEWER_BYTES_PER_LINE;
                snprintf(temp_buf, 32, "%04lX", addr);

                canvas_set_font(canvas, FontKeyboard);
                canvas_draw_str(canvas, LEFT_OFFSET, TOP_OFFSET + i * ROW_HEIGHT, temp_buf);
            }

            char* p = temp_buf;
            for(uint32_t j = 0; j < bytes_left_per_row; ++j)
                p += snprintf(p, 32, "%02X ", model->file_bytes[i][j]);

            canvas_set_font(canvas, FontKeyboard);
            canvas_draw_str(canvas, LEFT_OFFSET + 41, TOP_OFFSET + i * ROW_HEIGHT, temp_buf);
        }

        // Poor man's debug
        // snprintf(temp_buf, 32, "D %02lX", model->dbg);
        // elements_button_right(canvas, temp_buf);
    }
}

static void hex_viewer_startscreen_model_init(HexViewerStartscreenModel* const model) {
    memset(model->file_bytes, 0, sizeof(model->file_bytes));
    model->file_offset = 0;
    model->file_read_bytes = 0;
    model->file_size = 0;
    model->mode = false;
    model->dbg = 0;
}

static void
    update_local_model_from_app(HexViewer* const app, HexViewerStartscreenModel* const model) {
    memcpy(model->file_bytes, app->model->file_bytes, sizeof(model->file_bytes));
    model->file_offset = app->model->file_offset;
    model->file_read_bytes = app->model->file_read_bytes;
    model->file_size = app->model->file_size;
    //model->mode = app->model->mode;
}

bool hex_viewer_startscreen_input(InputEvent* event, void* context) {
    furi_assert(context);
    HexViewerStartscreen* instance = context;
    HexViewer* app = instance->context; // TO so good, but works
    // TODO InputTypeShort?
    if(event->type == InputTypeRelease || event->type == InputTypeRepeat) {
        switch(event->key) {
        case InputKeyBack:
            with_view_model(
                instance->view,
                HexViewerStartscreenModel * model,
                {
                    instance->callback(HexViewerCustomEventStartscreenBack, instance->context);
                    update_local_model_from_app(instance->context, model);
                },
                true);
            break;
        case InputKeyLeft:
            with_view_model(
                instance->view,
                HexViewerStartscreenModel * model,
                { model->mode = !model->mode; },
                true);
            break;
        case InputKeyRight:
            with_view_model(
                instance->view, HexViewerStartscreenModel * model, { model->dbg = 0; }, true);
            break;
        case InputKeyUp:
            with_view_model(
                instance->view,
                HexViewerStartscreenModel * model,
                {
                    if(app->model->file_offset > 0) {
                        app->model->file_offset -= HEX_VIEWER_BYTES_PER_LINE;
                        if(!hex_viewer_read_file(app)) break; // TODO Do smth
                    }

                    update_local_model_from_app(instance->context, model);
                },
                true);
            break;
        case InputKeyDown:
            with_view_model(
                instance->view,
                HexViewerStartscreenModel * model,
                {
                    uint32_t last_byte_on_screen =
                        app->model->file_offset + app->model->file_read_bytes;
                    if(app->model->file_size > last_byte_on_screen) {
                        app->model->file_offset += HEX_VIEWER_BYTES_PER_LINE;
                        if(!hex_viewer_read_file(app)) break; // TODO Do smth
                    }

                    update_local_model_from_app(instance->context, model);
                },
                true);
            break;
        case InputKeyOk:
            with_view_model(
                instance->view,
                HexViewerStartscreenModel * model,
                {
                    instance->callback(HexViewerCustomEventStartscreenOk, instance->context);
                    update_local_model_from_app(instance->context, model);
                },
                true);
            break;
        case InputKeyMAX:
            break;
        }
    }

    return true;
}

void hex_viewer_startscreen_exit(void* context) {
    furi_assert(context);
}

void hex_viewer_startscreen_enter(void* context) {
    furi_assert(context);
    HexViewerStartscreen* instance = (HexViewerStartscreen*)context;
    with_view_model(
        instance->view,
        HexViewerStartscreenModel * model,
        { update_local_model_from_app(instance->context, model); },
        true);
}

HexViewerStartscreen* hex_viewer_startscreen_alloc() {
    HexViewerStartscreen* instance = malloc(sizeof(HexViewerStartscreen));
    instance->view = view_alloc();
    view_allocate_model(instance->view, ViewModelTypeLocking, sizeof(HexViewerStartscreenModel));
    view_set_context(instance->view, instance);
    view_set_draw_callback(instance->view, (ViewDrawCallback)hex_viewer_startscreen_draw);
    view_set_input_callback(instance->view, hex_viewer_startscreen_input);
    view_set_enter_callback(instance->view, hex_viewer_startscreen_enter);
    view_set_exit_callback(instance->view, hex_viewer_startscreen_exit);

    with_view_model(
        instance->view,
        HexViewerStartscreenModel * model,
        { hex_viewer_startscreen_model_init(model); },
        true);

    return instance;
}

void hex_viewer_startscreen_free(HexViewerStartscreen* instance) {
    furi_assert(instance);

    with_view_model(
        instance->view, HexViewerStartscreenModel * model, { UNUSED(model); }, true);
    view_free(instance->view);
    free(instance);
}

View* hex_viewer_startscreen_get_view(HexViewerStartscreen* instance) {
    furi_assert(instance);
    return instance->view;
}
