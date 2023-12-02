#include <furi.h>
#include <furi_hal.h>

#include <gui/gui.h>
#include <input/input.h>
#include <dialogs/dialogs.h>
#include <storage/storage.h>
#include <flipper_format/flipper_format.h>

#include <notification/notification.h>
#include <notification/notification_messages.h>

#include "key_generator_icons.h"

#define DIR_PATH "/ext/lfrfid/rfidfuzzer"
#define FILE_PATH "/ext/lfrfid/rfidfuzzer/generated.txt"

FuriString* file_path;
FuriString* key;

// Screen is 128x64 px
static void app_draw_callback(Canvas* canvas, void* ctx) {
    UNUSED(ctx);

    canvas_clear(canvas);
    canvas_draw_icon(canvas, 0, 0, &I_ok_64x64);
    canvas_draw_str(canvas, 80, 24, "Saved as");
    canvas_draw_str(canvas, 75, 36, "lfrfid/rfidfuzzer/");
    canvas_draw_str(canvas, 70, 48, "generated.txt");
}

static void app_input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);

    FuriMessageQueue* event_queue = ctx;
    furi_message_queue_put(event_queue, input_event, FuriWaitForever);
}

static void beep() {
    NotificationApp* notification = furi_record_open(RECORD_NOTIFICATION);
    notification_message(notification, &sequence_success);
    furi_record_close(RECORD_NOTIFICATION);
}

int32_t key_generator_main(void* p) {
    UNUSED(p);
    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));

    // Configure view port
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, app_draw_callback, view_port);
    view_port_input_callback_set(view_port, app_input_callback, event_queue);

    // Register view port in GUI
    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    // Selecting file
    file_path = furi_string_alloc_set_str("/ext/lfrfid");
    key = furi_string_alloc_set_str("");
    DialogsFileBrowserOptions browser_options;
    dialog_file_browser_set_basic_options(&browser_options, ".rfid", &I_icon_10x10);
    browser_options.base_path = "/ext/lfrfid";
    DialogsApp* dialogs = furi_record_open(RECORD_DIALOGS);
    bool running = dialog_file_browser_show(dialogs, file_path, file_path, &browser_options);
    furi_record_close(RECORD_DIALOGS);

    if(running) {
        Storage* storage = furi_record_open(RECORD_STORAGE);
        FlipperFormat* format = flipper_format_file_alloc(storage);

        // Parsing file
        flipper_format_file_open_existing(format, furi_string_get_cstr(file_path));
        flipper_format_read_string(format, "Data", key);
        furi_string_replace_str(key, " ", "", 0);

        flipper_format_file_close(format);
        flipper_format_free(format);

        if(!storage_dir_exists(storage, DIR_PATH)) storage_simply_mkdir(storage, DIR_PATH);

        File* file = storage_file_alloc(storage);
        bool ok = storage_file_open(file, FILE_PATH, FSAM_WRITE, FSOM_OPEN_ALWAYS);
        if(ok) {
            storage_file_write(file, (uint8_t*)furi_string_get_cstr(key), 2);
            storage_file_write(file, (uint8_t*)"11111111\r\n", 10);
            storage_file_write(file, (uint8_t*)furi_string_get_cstr(key), 2);
            storage_file_write(file, (uint8_t*)"22222222\r\n", 10);
            storage_file_write(file, (uint8_t*)furi_string_get_cstr(key), 2);
            storage_file_write(file, (uint8_t*)"33333333\r\n", 10);
            storage_file_write(file, (uint8_t*)furi_string_get_cstr(key), 2);
            storage_file_write(file, (uint8_t*)"44444444\r\n", 10);
            storage_file_write(file, (uint8_t*)furi_string_get_cstr(key), 2);
            storage_file_write(file, (uint8_t*)"55555555\r\n", 10);
            storage_file_write(file, (uint8_t*)furi_string_get_cstr(key), 2);
            storage_file_write(file, (uint8_t*)"66666666\r\n", 10);
            storage_file_write(file, (uint8_t*)furi_string_get_cstr(key), 2);
            storage_file_write(file, (uint8_t*)"77777777\r\n", 10);
            storage_file_write(file, (uint8_t*)furi_string_get_cstr(key), 2);
            storage_file_write(file, (uint8_t*)"88888888\r\n", 10);
            storage_file_write(file, (uint8_t*)furi_string_get_cstr(key), 2);
            storage_file_write(file, (uint8_t*)"99999999\r\n", 10);
            storage_file_write(file, (uint8_t*)furi_string_get_cstr(key), 2);
            storage_file_write(file, (uint8_t*)"AAAAAAAA\r\n", 10);
            storage_file_write(file, (uint8_t*)furi_string_get_cstr(key), 2);
            storage_file_write(file, (uint8_t*)"BBBBBBBB\r\n", 10);
            storage_file_write(file, (uint8_t*)furi_string_get_cstr(key), 2);
            storage_file_write(file, (uint8_t*)"CCCCCCCC\r\n", 10);
            storage_file_write(file, (uint8_t*)furi_string_get_cstr(key), 2);
            storage_file_write(file, (uint8_t*)"DDDDDDDD\r\n", 10);
            storage_file_write(file, (uint8_t*)furi_string_get_cstr(key), 2);
            storage_file_write(file, (uint8_t*)"EEEEEEEE\r\n", 10);
            storage_file_write(file, (uint8_t*)furi_string_get_cstr(key), 2);
            storage_file_write(file, (uint8_t*)"FFFFFFFF\r\n", 10);
            storage_file_write(file, (uint8_t*)furi_string_get_cstr(key), 2);
            storage_file_write(file, (uint8_t*)"12345678\r\n", 10);
            storage_file_write(file, (uint8_t*)furi_string_get_cstr(key), 2);
            storage_file_write(file, (uint8_t*)"01234567\r\n", 10);
            storage_file_write(file, (uint8_t*)furi_string_get_cstr(key), 2);
            storage_file_write(file, (uint8_t*)"98765432\r\n", 10);
            storage_file_write(file, (uint8_t*)furi_string_get_cstr(key), 2);
            storage_file_write(file, (uint8_t*)"FEDCBA98\r\n", 10);

            storage_file_write(file, (uint8_t*)furi_string_get_cstr(key), 4);
            storage_file_write(file, (uint8_t*)"111111\r\n", 8);
            storage_file_write(file, (uint8_t*)furi_string_get_cstr(key), 4);
            storage_file_write(file, (uint8_t*)"222222\r\n", 8);
            storage_file_write(file, (uint8_t*)furi_string_get_cstr(key), 4);
            storage_file_write(file, (uint8_t*)"333333\r\n", 8);
            storage_file_write(file, (uint8_t*)furi_string_get_cstr(key), 4);
            storage_file_write(file, (uint8_t*)"444444\r\n", 8);
            storage_file_write(file, (uint8_t*)furi_string_get_cstr(key), 4);
            storage_file_write(file, (uint8_t*)"555555\r\n", 8);
            storage_file_write(file, (uint8_t*)furi_string_get_cstr(key), 4);
            storage_file_write(file, (uint8_t*)"666666\r\n", 8);
            storage_file_write(file, (uint8_t*)furi_string_get_cstr(key), 4);
            storage_file_write(file, (uint8_t*)"777777\r\n", 8);
            storage_file_write(file, (uint8_t*)furi_string_get_cstr(key), 4);
            storage_file_write(file, (uint8_t*)"888888\r\n", 8);
            storage_file_write(file, (uint8_t*)furi_string_get_cstr(key), 4);
            storage_file_write(file, (uint8_t*)"999999\r\n", 8);
            storage_file_write(file, (uint8_t*)furi_string_get_cstr(key), 4);
            storage_file_write(file, (uint8_t*)"AAAAAA\r\n", 8);
            storage_file_write(file, (uint8_t*)furi_string_get_cstr(key), 4);
            storage_file_write(file, (uint8_t*)"BBBBBB\r\n", 8);
            storage_file_write(file, (uint8_t*)furi_string_get_cstr(key), 4);
            storage_file_write(file, (uint8_t*)"CCCCCC\r\n", 8);
            storage_file_write(file, (uint8_t*)furi_string_get_cstr(key), 4);
            storage_file_write(file, (uint8_t*)"DDDDDD\r\n", 8);
            storage_file_write(file, (uint8_t*)furi_string_get_cstr(key), 4);
            storage_file_write(file, (uint8_t*)"EEEEEE\r\n", 8);
            storage_file_write(file, (uint8_t*)furi_string_get_cstr(key), 4);
            storage_file_write(file, (uint8_t*)"FFFFFF\r\n", 8);
            storage_file_write(file, (uint8_t*)furi_string_get_cstr(key), 4);
            storage_file_write(file, (uint8_t*)"123456\r\n", 8);
            storage_file_write(file, (uint8_t*)furi_string_get_cstr(key), 4);
            storage_file_write(file, (uint8_t*)"012345\r\n", 8);
            storage_file_write(file, (uint8_t*)furi_string_get_cstr(key), 4);
            storage_file_write(file, (uint8_t*)"987654\r\n", 8);
            storage_file_write(file, (uint8_t*)furi_string_get_cstr(key), 4);
            storage_file_write(file, (uint8_t*)"FEDCBA\r\n", 8);
        }
        storage_file_close(file);
        storage_file_free(file);

        furi_record_close(RECORD_STORAGE);

        beep();
    }

    InputEvent event;

    while(running) {
        if(furi_message_queue_get(event_queue, &event, 100) == FuriStatusOk) {
            if(event.type == InputTypePress && event.key == InputKeyBack) running = false;
        }
        view_port_update(view_port);
    }

    view_port_enabled_set(view_port, false);
    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_message_queue_free(event_queue);

    furi_record_close(RECORD_GUI);

    return 0;
}