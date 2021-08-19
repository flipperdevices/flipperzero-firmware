#include "subghz_receiver.h"
#include "../subghz_i.h"
#include <math.h>
#include <furi.h>
#include <furi-hal.h>
#include <input/input.h>
#include <gui/elements.h>
#include <notification/notification-messages.h>

#include <assets_icons.h>

#define FRAME_HEIGHT 12
#define MAX_LEN_PX 100
#define MENU_ITEMS 4

static const Icon* ReceiverItemIcons[] = {
    [TYPE_PROTOCOL_UNKNOWN] = &I_unknown_10px,
    [TYPE_PROTOCOL_STATIC] = &I_Nfc_10px,
    [TYPE_PROTOCOL_DYNAMIC] = &I_sub1_10px,
};

struct SubghzReceiver {
    View* view;
    SubghzReceiverCallback callback;
    void* context;
    SubGhzWorker* worker;
    SubGhzProtocol* protocol;
    SubGhzHistory* history;
};

typedef struct {
    string_t text;
    uint16_t scene;
    SubGhzProtocolCommon* protocol_result;

    uint8_t tab_idx;
    uint8_t menu_idx;
    uint16_t idx;
    uint16_t list_offset;
    uint16_t history_item;
    bool menu;
} SubghzReceiverModel;

void subghz_receiver_set_callback(
    SubghzReceiver* subghz_receiver,
    SubghzReceiverCallback callback,
    void* context) {
    furi_assert(subghz_receiver);
    furi_assert(callback);
    subghz_receiver->callback = callback;
    subghz_receiver->context = context;
}

void subghz_receiver_set_protocol(
    SubghzReceiver* subghz_receiver,
    SubGhzProtocolCommon* protocol_result,
    SubGhzProtocol* protocol) {
    with_view_model(
        subghz_receiver->view, (SubghzReceiverModel * model) {
            model->protocol_result = protocol_result;
            return true;
        });
    subghz_receiver->protocol = protocol;
}

void subghz_receiver_set_worker(SubghzReceiver* subghz_receiver, SubGhzWorker* worker) {
    subghz_receiver->worker = worker;
}

void subghz_receiver_set_history(SubghzReceiver* subghz_receiver, SubGhzHistory* history) {
    subghz_receiver->history = history;
}

static void subghz_receiver_draw_frame(Canvas* canvas, uint16_t idx, bool scrollbar) {
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_box(canvas, 0, 0 + idx * FRAME_HEIGHT, scrollbar ? 122 : 127, FRAME_HEIGHT);

    canvas_set_color(canvas, ColorWhite);
    canvas_draw_dot(canvas, 0, 0 + idx * FRAME_HEIGHT);
    canvas_draw_dot(canvas, 1, 0 + idx * FRAME_HEIGHT);
    canvas_draw_dot(canvas, 0, (0 + idx * FRAME_HEIGHT) + 1);

    canvas_draw_dot(canvas, 0, (0 + idx * FRAME_HEIGHT) + 11);
    canvas_draw_dot(canvas, scrollbar ? 121 : 126, 0 + idx * FRAME_HEIGHT);
    canvas_draw_dot(canvas, scrollbar ? 121 : 126, (0 + idx * FRAME_HEIGHT) + 11);
}

void subghz_receiver_draw(Canvas* canvas, SubghzReceiverModel* model) {
    size_t history_item = model->history_item;
    bool scrollbar = history_item > 4;
    string_t str_buff;
    char cstr_buff[22]; //todo проверитьм аксимальную длинну
    string_init(str_buff);
    // if(scrollbar){
    //     subghz_receiver_draw_frame(canvas, 2, scrollbar);
    //     elements_scrollbar_pos(canvas, 126, 15, 49, model->idx, history_item);

    //     model->scene = 0;
    // }

    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);

    switch(model->scene) {
    case 1:
        canvas_draw_icon(canvas, 0, 0, &I_RFIDDolphinReceive_97x61);
        canvas_invert_color(canvas);
        canvas_draw_box(canvas, 80, 2, 20, 20);
        canvas_invert_color(canvas);
        canvas_draw_icon(canvas, 75, 8, &I_sub1_10px);
        canvas_set_font(canvas, FontPrimary);
        elements_multiline_text(canvas, 63, 40, "Scanning...");
        break;

    default:
        for(size_t i = 0; i < MIN(history_item, MENU_ITEMS); ++i) {
            size_t idx = CLAMP(i + model->list_offset, history_item, 0);
            //ArchiveFile_t* file = files_array_get(model->files, CLAMP(idx, array_size - 1, 0));

            //strlcpy(cstr_buff, string_get_cstr(file->name), string_size(file->name) + 1);
            strlcpy(cstr_buff, "item1", 5 + 1);

            //if(is_known_app(file->type)) archive_trim_file_ext(cstr_buff);
            string_set_str(str_buff, cstr_buff);

            // if(is_known_app(file->type)) {
            //     archive_trim_file_ext(cstr_buff);
            // }

            elements_string_fit_width(canvas, str_buff, scrollbar ? MAX_LEN_PX - 6 : MAX_LEN_PX);

            if(model->idx == idx) {
                subghz_receiver_draw_frame(canvas, i, scrollbar);
            } else {
                canvas_set_color(canvas, ColorBlack);
            }

            canvas_draw_icon(canvas, 2, 0 + i * FRAME_HEIGHT, ReceiverItemIcons[TYPE_PROTOCOL_UNKNOWN]);
            canvas_draw_str(canvas, 15, 10 + i * FRAME_HEIGHT, string_get_cstr(str_buff));
            string_clean(str_buff);
        }
        // canvas_set_font(canvas, FontSecondary);
        // elements_multiline_text(canvas, 0, 10, string_get_cstr(model->text));
        // if(model->protocol_result && model->protocol_result->to_save_string &&
        //    strcmp(model->protocol_result->name, "KeeLoq")) {
        //     elements_button_right(canvas, "Save");
        // }
        break;
    }
    canvas_set_font(canvas, FontPrimary);
    elements_multiline_text(canvas, 60, 61, "433.92 OOK");
    elements_button_left(canvas, "Config");
    string_clear(str_buff);
}

bool subghz_receiver_input(InputEvent* event, void* context) {
    furi_assert(context);
    SubghzReceiver* subghz_receiver = context;

    if(event->type != InputTypeShort) return false;

    bool can_be_saved = false;
    with_view_model(
        subghz_receiver->view, (SubghzReceiverModel * model) {
            can_be_saved =
                (model->protocol_result && model->protocol_result->to_save_string &&
                 strcmp(model->protocol_result->name, "KeeLoq"));
            return false;
        });

    if(event->key == InputKeyBack) {
        return false;
    } else if(event->key == InputKeyLeft) {
        subghz_receiver->callback(SubghzReceverEventBack, subghz_receiver->context);
    } else if(can_be_saved && event->key == InputKeyRight) {
        subghz_receiver->callback(SubghzReceverEventSave, subghz_receiver->context);
    } else if(can_be_saved && event->key == InputKeyUp) {
        with_view_model(
            subghz_receiver->view, (SubghzReceiverModel * model) {
                if(model->idx != 0) model->idx--;
                return true;
            });
    } else if(can_be_saved && event->key == InputKeyDown) {
        with_view_model(
            subghz_receiver->view, (SubghzReceiverModel * model) {
                if(model->idx != subghz_history_get_item(subghz_receiver->history)-1)
                    model->idx++;
                return true;
            });
    }

    return true;
}

void subghz_receiver_text_callback(string_t text, void* context) {
    furi_assert(context);
    SubghzReceiver* subghz_receiver = context;

    with_view_model(
        subghz_receiver->view, (SubghzReceiverModel * model) {
            string_set(model->text, text);
            model->scene = 0;
            return true;
        });
}

void subghz_receiver_protocol_callback(SubGhzProtocolCommon* parser, void* context) {
    furi_assert(context);
    SubghzReceiver* subghz_receiver = context;
    subghz_history_add_to_history(subghz_receiver->history, parser);
    with_view_model(
        subghz_receiver->view, (SubghzReceiverModel * model) {
            model->protocol_result = parser;
            string_clean(model->text);
            model->protocol_result->to_string(model->protocol_result, model->text);
            model->history_item = subghz_history_get_item(subghz_receiver->history);
            model->scene = 0;
            return true;
        });
}

void subghz_receiver_enter(void* context) {
    furi_assert(context);
    SubghzReceiver* subghz_receiver = context;

    with_view_model(
        subghz_receiver->view, (SubghzReceiverModel * model) {
            //model->protocol->to_string(model->protocol, model->text);
            //string_cat(model->text, "Hi");
            model->scene = 1;
            return true;
        });
    //Start CC1101 rx
    subghz_begin(FuriHalSubGhzPresetOokAsync);
    subghz_rx(433920000);

    furi_hal_subghz_start_async_rx(subghz_worker_rx_callback, subghz_receiver->worker);
    subghz_worker_start(subghz_receiver->worker);

    //subghz_protocol_enable_dump_text(subghz_receiver->protocol, subghz_receiver_text_callback, subghz_receiver);
    subghz_protocol_enable_dump(
        subghz_receiver->protocol, subghz_receiver_protocol_callback, subghz_receiver);
}

void subghz_receiver_exit(void* context) {
    furi_assert(context);
    SubghzReceiver* subghz_receiver = context;
    with_view_model(
        subghz_receiver->view, (SubghzReceiverModel * model) {
            string_clean(model->text);
            return true;
        });
    // Stop CC1101
    subghz_worker_stop(subghz_receiver->worker);
    furi_hal_subghz_stop_async_rx();
    subghz_end();
}

SubghzReceiver* subghz_receiver_alloc() {
    SubghzReceiver* subghz_receiver = furi_alloc(sizeof(SubghzReceiver));

    // View allocation and configuration
    subghz_receiver->view = view_alloc();
    view_allocate_model(subghz_receiver->view, ViewModelTypeLocking, sizeof(SubghzReceiverModel));
    view_set_context(subghz_receiver->view, subghz_receiver);
    view_set_draw_callback(subghz_receiver->view, (ViewDrawCallback)subghz_receiver_draw);
    view_set_input_callback(subghz_receiver->view, subghz_receiver_input);
    view_set_enter_callback(subghz_receiver->view, subghz_receiver_enter);
    view_set_exit_callback(subghz_receiver->view, subghz_receiver_exit);

    with_view_model(
        subghz_receiver->view, (SubghzReceiverModel * model) {
            string_init(model->text);
            return true;
        });
    return subghz_receiver;
}

void subghz_receiver_free(SubghzReceiver* subghz_receiver) {
    furi_assert(subghz_receiver);

    with_view_model(
        subghz_receiver->view, (SubghzReceiverModel * model) {
            string_clear(model->text);
            return true;
        });
    view_free(subghz_receiver->view);
    free(subghz_receiver);
}

View* subghz_receiver_get_view(SubghzReceiver* subghz_receiver) {
    furi_assert(subghz_receiver);
    return subghz_receiver->view;
}
