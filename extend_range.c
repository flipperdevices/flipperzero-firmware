#include <furi.h>

#include <gui/gui.h>
#include <input/input.h>

#include <flipper_format/flipper_format.h>

#define PATH_SUBGHZ EXT_PATH("subghz") "/assets/extend_range.txt"
#define KEY_EXTEND_RANGE "use_ext_range_at_own_risk"
#define KEY_IGNORE_DEFAULT "ignore_default_tx_region"

int global_ret_val = -1;

static void extend_range_close_file(FlipperFormat* file) {
    if(file == NULL) {
        furi_record_close(RECORD_STORAGE);
        return;
    }
    flipper_format_file_close(file);
    flipper_format_free(file);
    furi_record_close(RECORD_STORAGE);
}

static FlipperFormat* extend_range_open_file() {
    Storage* storage = furi_record_open(RECORD_STORAGE);
    FlipperFormat* file = flipper_format_file_alloc(storage);
    if(storage_common_stat(storage, PATH_SUBGHZ, NULL) == FSE_OK) {
        if(!flipper_format_file_open_existing(file, PATH_SUBGHZ)) {
            extend_range_close_file(file);
            return NULL;
        }
    } else {
        return NULL;
    }
    return file;
}

void extend_range_draw_callback(Canvas* canvas, void* ctx) {
    UNUSED(ctx);
    canvas_clear(canvas);

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 2, 10, "Extend Range Sub-GHz");

    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 2, 20, "Back -> Exit, OK -> Extend");

    canvas_draw_str(canvas, 2, 30, "Current Status: ");
    switch(global_ret_val) {
    case -1:
        canvas_draw_str(canvas, 2, 40, "Waiting");
        break;
    case 0:
        canvas_draw_str(canvas, 2, 40, "Success. Reboot now.");
        break;
    case 1:
        canvas_draw_str(canvas, 2, 40, "File not found");
        break;
    case 2:
        canvas_draw_str(canvas, 2, 40, "Failed to update extend_range");
        break;
    case 3:
        canvas_draw_str(canvas, 2, 40, "Failed to update ignore_default");
        break;
    default:
        canvas_draw_str(canvas, 2, 40, "Unknown error");
        break;
    }
}

void extend_range_input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);
    FuriMessageQueue* event_queue = ctx;
    furi_message_queue_put(event_queue, input_event, FuriWaitForever);
}

int extend_range() {
    FlipperFormat* file = extend_range_open_file();

    if(file == NULL) {
        return 1;
    }

    if(!flipper_format_update_string_cstr(file, KEY_EXTEND_RANGE, "true")) {
        extend_range_close_file(file);
        return 2;
    }
    if(!flipper_format_update_string_cstr(file, KEY_IGNORE_DEFAULT, "true")) {
        extend_range_close_file(file);
        return 3;
    }

    extend_range_close_file(file);
    return 0;
}

int32_t extend_range_app(void* p) {
    UNUSED(p);
    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));

    // Configure view port & callbacks
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, extend_range_draw_callback, NULL);
    view_port_input_callback_set(view_port, extend_range_input_callback, event_queue);

    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    InputEvent event;
    while(furi_message_queue_get(event_queue, &event, FuriWaitForever) == FuriStatusOk) {
        if(event.type == InputTypeShort && event.key == InputKeyBack) {
            break;
        }
        if(event.key == InputKeyOk) {
            if(event.type == InputTypePress) {
                global_ret_val = extend_range();
            }
        }
    }

    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_message_queue_free(event_queue);

    furi_record_close(RECORD_GUI);

    return 0;
}