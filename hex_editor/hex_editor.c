#include <stdio.h>
#include <furi.h>
#include <gui/gui.h>
#include <gui/elements.h>
#include <dialogs/dialogs.h>
#include <input/input.h>
#include <notification/notification_messages.h>

#include <storage/storage.h>
#include <stream/stream.h>
#include <stream/buffered_file_stream.h>
#include <toolbox/stream/file_stream.h>

#include <hex_editor_icons.h>
#include <assets_icons.h>

#define TAG "HexEditor"

#define STEP 6u

typedef struct {
    uint32_t file_offset;
    uint32_t file_read_bytes;
    uint32_t file_size;
    uint8_t string_offset;
    char editable_char;
    Stream* stream;
    bool mode;
} HexEditorModel;

typedef struct {
    HexEditorModel* model;

    FuriMessageQueue* input_queue;

    ViewPort* view_port;
    Gui* gui;
    Storage* storage;

    FuriString* buffer;
} HexEditor;

static void draw_callback(Canvas* canvas, void* ctx) {
    HexEditor* hex_editor = ctx;

    canvas_clear(canvas);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 0, 10, "Line and mode:");

    canvas_set_font(canvas, FontSecondary);

    uint8_t com_str_offset = 0;
    uint8_t local_offset = MAX(hex_editor->model->string_offset - 5, 0);
    // TODO UTF ?
    for(uint8_t i = 0; i < MIN((uint8_t)128 / STEP, furi_string_size(hex_editor->buffer)); i++) {
        if(i + local_offset >= furi_string_size(hex_editor->buffer)) {
            break;
        }
        char a = furi_string_get_char(hex_editor->buffer, i + local_offset);
        canvas_draw_glyph(
            canvas, 0 + com_str_offset + (STEP - canvas_glyph_width(canvas, a)) / 2, 20, a);

        com_str_offset += STEP;
    }

    canvas_draw_icon(
        canvas,
        0 + MIN(hex_editor->model->string_offset, 5) * STEP - 4 + STEP / 2,
        21,
        &I_Pin_arrow_up_7x9);

    if(hex_editor->model->mode) {
        elements_button_left(canvas, "ASCII -");
        elements_button_right(canvas, "ASCII +");
        elements_button_center(canvas, "");

        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str(canvas, 0, 45, "edit");
    } else {
        elements_button_left(canvas, "");
        elements_button_right(canvas, "");
        elements_button_center(canvas, "chmod");

        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str(canvas, 0, 45, "seek");
    }

    canvas_draw_glyph(canvas, 30, 45, hex_editor->model->editable_char);
}

static void input_callback(InputEvent* input_event, void* ctx) {
    // Проверяем, что контекст не нулевой
    furi_assert(ctx);
    HexEditor* hex_editor = ctx;

    furi_message_queue_put(hex_editor->input_queue, input_event, 100);
}

static HexEditor* hex_editor_alloc() {
    HexEditor* instance = malloc(sizeof(HexEditor));

    instance->model = malloc(sizeof(HexEditorModel));
    memset(instance->model, 0x0, sizeof(HexEditorModel));

    instance->model->editable_char = ' ';

    instance->input_queue = furi_message_queue_alloc(8, sizeof(InputEvent));

    instance->view_port = view_port_alloc();
    view_port_draw_callback_set(instance->view_port, draw_callback, instance);
    view_port_input_callback_set(instance->view_port, input_callback, instance);

    instance->gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(instance->gui, instance->view_port, GuiLayerFullscreen);

    instance->storage = furi_record_open(RECORD_STORAGE);

    instance->buffer = furi_string_alloc();

    return instance;
}

static void hex_editor_free(HexEditor* instance) {
    furi_record_close(RECORD_STORAGE);

    gui_remove_view_port(instance->gui, instance->view_port);
    view_port_free(instance->view_port);
    furi_record_close(RECORD_GUI);

    furi_message_queue_free(instance->input_queue);

    if(instance->model->stream) {
        buffered_file_stream_close(instance->model->stream);
        stream_free(instance->model->stream);
    }

    furi_string_free(instance->buffer);

    free(instance->model);
    free(instance);
}

static bool hex_editor_open_file(HexEditor* hex_editor, const char* file_path) {
    furi_assert(hex_editor);
    furi_assert(file_path);

    hex_editor->model->stream = buffered_file_stream_alloc(hex_editor->storage);

    if(!buffered_file_stream_open(
           hex_editor->model->stream, file_path, FSAM_READ_WRITE, FSOM_OPEN_EXISTING)) {
        FURI_LOG_E(TAG, "Unable to open stream: %s", file_path);
        return false;
    };

    hex_editor->model->file_size = stream_size(hex_editor->model->stream);

    return true;
}

int32_t hex_editor_app(void* p) {
    UNUSED(p);

    HexEditor* hex_editor = hex_editor_alloc();

    FuriString* file_path;
    file_path = furi_string_alloc();

    do {
        if(p && strlen(p)) {
            furi_string_set(file_path, (const char*)p);
        } else {
            furi_string_set(file_path, STORAGE_EXT_PATH_PREFIX);

            DialogsFileBrowserOptions browser_options;
            dialog_file_browser_set_basic_options(&browser_options, "*", &I_edit_10px);
            browser_options.hide_ext = false;
            browser_options.hide_dot_files = false;

            DialogsApp* dialogs = furi_record_open(RECORD_DIALOGS);
            bool res = dialog_file_browser_show(dialogs, file_path, file_path, &browser_options);

            furi_record_close(RECORD_DIALOGS);
            if(!res) {
                FURI_LOG_I(TAG, "No file selected");
                break;
            }
        }

        FURI_LOG_I(TAG, "File selected: %s", furi_string_get_cstr(file_path));

        if(!hex_editor_open_file(hex_editor, furi_string_get_cstr(file_path))) break;

        if(!stream_read_line(hex_editor->model->stream, hex_editor->buffer)) {
            FURI_LOG_T(TAG, "No keys left in dict");
            break;
        }

        InputEvent event;
        int8_t offset_modifier;
        while(1) {
            // Выбираем событие из очереди в переменную event (ждем бесконечно долго, если очередь пуста)
            // и проверяем, что у нас получилось это сделать
            furi_check(
                furi_message_queue_get(hex_editor->input_queue, &event, FuriWaitForever) ==
                FuriStatusOk);

            // Если нажата кнопка "назад", то выходим из цикла, а следовательно и из приложения
            if(event.type == InputTypeShort || event.type == InputTypeRepeat) {
                if(!hex_editor->model->mode) {
                    offset_modifier = 1;
                    if(event.type == InputTypeRepeat) {
                        offset_modifier = 3;
                    }
                    if(event.key == InputKeyRight) {
                        hex_editor->model->string_offset += offset_modifier;
                        if(hex_editor->model->string_offset >=
                           furi_string_size(hex_editor->buffer)) {
                            // dengeros
                            hex_editor->model->string_offset -=
                                furi_string_size(hex_editor->buffer);
                        }
                    }
                    if(event.key == InputKeyLeft) {
                        if(hex_editor->model->string_offset - offset_modifier < 0) {
                            // dengeros
                            hex_editor->model->string_offset +=
                                furi_string_size(hex_editor->buffer);
                        }
                        hex_editor->model->string_offset -= offset_modifier;
                    }
                    if(event.key == InputKeyDown) {
                        hex_editor->model->string_offset = 0;
                        if(!stream_read_line(hex_editor->model->stream, hex_editor->buffer)) {
                            FURI_LOG_T(TAG, "No keys left in dict");
                        }
                    }
                    if(event.key == InputKeyUp) {
                        hex_editor->model->string_offset = 0;

                        if(!stream_seek(hex_editor->model->stream, -1, StreamOffsetFromCurrent)) {
                            FURI_LOG_E(TAG, "Unable to seek stream");
                            break;
                        }
                        // NOT work on first line
                        stream_seek_to_char(
                            hex_editor->model->stream, '\n', StreamDirectionBackward);

                        if(!stream_seek_to_char(
                               hex_editor->model->stream, '\n', StreamDirectionBackward)) {
                            stream_rewind(hex_editor->model->stream);
                        } else {
                            if(!stream_seek(
                                   hex_editor->model->stream, 1, StreamOffsetFromCurrent)) {
                                FURI_LOG_E(TAG, "Unable to seek stream");
                                break;
                            }
                        }

                        if(!stream_read_line(hex_editor->model->stream, hex_editor->buffer)) {
                            FURI_LOG_T(TAG, "No keys left in dict");
                            break;
                        }
                    }

                    if(event.key == InputKeyOk) {
                        hex_editor->model->editable_char = furi_string_get_char(
                            hex_editor->buffer, hex_editor->model->string_offset);

                        hex_editor->model->mode = 1;
                    }
                } else {
                    offset_modifier = 1;
                    if(event.type == InputTypeRepeat) {
                        offset_modifier = 4;
                    }
                    if(event.key == InputKeyRight) {
                        hex_editor->model->editable_char += offset_modifier;
                    }
                    if(event.key == InputKeyLeft) {
                        hex_editor->model->editable_char -= offset_modifier;
                    }

                    if(event.key == InputKeyOk) {
                        if(!stream_seek(hex_editor->model->stream, -1, StreamOffsetFromCurrent)) {
                            FURI_LOG_E(TAG, "Unable to seek stream");
                            break;
                        }
                        if(!stream_seek_to_char(
                               hex_editor->model->stream, '\n', StreamDirectionBackward)) {
                            FURI_LOG_E(TAG, "Unable to stream_seek_to_char n");
                            // first line
                            stream_seek(hex_editor->model->stream, 0, StreamOffsetFromStart);
                            if(!stream_seek(
                                   hex_editor->model->stream,
                                   hex_editor->model->string_offset,
                                   StreamOffsetFromCurrent)) {
                                FURI_LOG_E(TAG, "Unable to seek string_offset");
                            }
                        } else {
                            if(!stream_seek(
                                   hex_editor->model->stream,
                                   hex_editor->model->string_offset + 1,
                                   StreamOffsetFromCurrent)) {
                                FURI_LOG_E(TAG, "Unable to seek string_offset +1");
                            }
                        }

                        if(!stream_write_char(
                               hex_editor->model->stream, hex_editor->model->editable_char)) {
                            FURI_LOG_E(TAG, "Unable to stream_write_char");
                            break;
                        }

                        hex_editor->model->editable_char = ' ';

                        hex_editor->model->mode = 0;

                        if(!stream_seek_to_char(
                               hex_editor->model->stream, '\n', StreamDirectionBackward)) {
                            FURI_LOG_E(TAG, "Unable to stream_seek_to_char n 2");
                            // first line
                            stream_seek(hex_editor->model->stream, 0, StreamOffsetFromStart);
                        } else {
                            if(!stream_seek(
                                   hex_editor->model->stream, 1, StreamOffsetFromCurrent)) {
                                FURI_LOG_E(TAG, "Unable to seek stream");
                                break;
                            }
                        }

                        if(!stream_read_line(hex_editor->model->stream, hex_editor->buffer)) {
                            FURI_LOG_T(TAG, "No keys left in dict");
                            break;
                        }
                    }
                }
            }
            if(event.key == InputKeyBack) {
                break;
            }
            // ?
            view_port_update(hex_editor->view_port);
        }
    } while(false);

    furi_string_free(file_path);
    hex_editor_free(hex_editor);

    return 0;
}