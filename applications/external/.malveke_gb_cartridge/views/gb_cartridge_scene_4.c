#include "../gb_cartridge_app.h"
#include <furi.h>
#include <furi_hal.h>
#include <input/input.h>
#include <gui/elements.h>
#include <dolphin/dolphin.h>
#include <gui/elements.h>
#include <notification/notification_messages.h>
#include <dialogs/dialogs.h>
#include <gui/modules/dialog_ex.h>
#include <toolbox/stream/file_stream.h>
#include "../helpers/gb_cartridge_speaker.h"
#include "../helpers/sequential_file.h"
#include <stdio.h> // Para sprintf
#include <string.h> // Para strlen
#include <lib/toolbox/stream/file_stream.h>

struct GBCartridgeScene4 {
    View* view;
    GBCartridgeScene4Callback callback;
    void* context;
    GBCartridge* app;
};

typedef struct {
    char* event_type;
    int progress;
    int total_rom;
    int transfered;
    int romBanks;
    int elapsed_time;
    int start_time;

    char* cart_dump_rom_filename_sequential;
    bool rx_active;

    char* gameboy_rom_option_selected_text;
} GameBoyCartridgeROMWriteModel;

void gb_cartridge_scene_4_set_callback(
    GBCartridgeScene4* instance,
    GBCartridgeScene4Callback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(callback);
    instance->callback = callback;
    instance->context = context;
}

static void drawProgressBar(Canvas* canvas, int progress) {
    for(int x = 0; x < 64 - 14 - UI_PADDING - UI_PADDING - UI_PADDING - UI_PADDING; x += 5) {
        for(int row = 0; row < 20; row += 5) {
            if(progress > 0) {
                canvas_draw_box(
                    canvas, 14 /*ARROW*/ + UI_PADDING + 2 + x + 4, /*45*/ 26 + row, 4, 4);
                progress--;
            } else {
                canvas_draw_frame(
                    canvas, 14 /*ARROW*/ + UI_PADDING + 2 + x + 4, /*45*/ 26 + row, 4, 4);
            }
        }
    }
}

void gb_cartridge_scene_4_draw(Canvas* canvas, GameBoyCartridgeROMWriteModel* model) {
    // Clear the screen.
    canvas_set_color(canvas, ColorBlack);

    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontKeyboard);
    canvas_draw_frame(canvas, 0, 24, (128 / 2), 25);

    canvas_set_bitmap_mode(canvas, 1);
    canvas_set_font(canvas, FontPrimary);
    char progressText[42];
    int progress = 0;
    if(model->total_rom > 0 && model->transfered > 0) {
        progress = model->transfered * 100 / model->total_rom;
    }
    snprintf(
        progressText,
        sizeof(progressText),
        "%d%% Write ROM... GB: %s",
        progress,
        model->gameboy_rom_option_selected_text);
    canvas_draw_str_aligned(canvas, 128 / 2, 0, AlignCenter, AlignTop, progressText);
    canvas_set_font(canvas, FontSecondary);

    // char *filename = strrchr(model->cart_dump_rom_filename_sequential, '/');
    // filename++;
    // canvas_draw_str_aligned(canvas, 128/2, 12, AlignCenter, AlignTop, filename);

    char total_rom_str[20];
    snprintf(
        total_rom_str,
        sizeof(total_rom_str),
        "of %.2lf MiB",
        (double)(model->total_rom / 1024.0 / 1024.0));

    char transfered_rom_str[20];
    snprintf(
        transfered_rom_str,
        sizeof(transfered_rom_str),
        "%.2lf MiB",
        (double)(model->transfered / 1024.0 / 1024.0));

    // Calcula la Tasa de Transferencia en KiB/s
    char transfer_rate_str[20];
    if(model->transfered > 0 && model->elapsed_time > 0) {
        double transfer_rate_kibps =
            (double)model->transfered / ((double)model->elapsed_time) / (double)1024.0;
        snprintf(transfer_rate_str, sizeof(transfer_rate_str), "%.2lf KiB/s", transfer_rate_kibps);
    } else {
        snprintf(transfer_rate_str, sizeof(transfer_rate_str), "0 KiB/s");
    }

    canvas_draw_str_aligned(
        canvas, (128 / 2) + UI_PADDING, 22 + 2, AlignLeft, AlignTop, transfered_rom_str);
    canvas_draw_str_aligned(
        canvas, (128 / 2) + UI_PADDING, 40 + 2, AlignLeft, AlignTop, total_rom_str);
    canvas_draw_str_aligned(
        canvas, (128 / 2) + UI_PADDING, 48 + 2, AlignLeft, AlignTop, transfer_rate_str);

    if(model->rx_active) {
        canvas_draw_icon_ex(canvas, UI_PADDING, 28, &I_ArrowUpFilled_14x15, IconRotation180);
    } else {
        canvas_draw_icon_ex(canvas, UI_PADDING, 28, &I_ArrowUpEmpty_14x15, IconRotation180);
    }

    char totalText[42];
    snprintf(totalText, sizeof(totalText), "%d", model->total_rom);
    // canvas_draw_str(canvas, 69, 48, totalText);

    drawProgressBar(
        canvas,
        (progress * UI_PROGRESS_ROWS * UI_PROGRESS_COLS) /
            100); // Pinta las primeras 10 cajas de negro
    // free(totalText);

    elements_button_center(canvas, "Write");
}

static void gb_cartridge_scene_4_model_init(GameBoyCartridgeROMWriteModel* const model) {
    model->progress = 0;
    model->total_rom = 0;
    model->transfered = 0;
    model->romBanks = 0;
    model->elapsed_time = 0;
    model->start_time = 0;
}
// static bool select_rom_file(GBCartridge *app, Stream* stream) {
//     bool result = false;
//     FuriString* file_path = furi_string_alloc();
//     furi_string_set(file_path, MALVEKE_APP_FOLDER);
//     DialogsFileBrowserOptions browser_options;
//     dialog_file_browser_set_basic_options(
//         &browser_options, app->gameboy_rom_option_selected_text, NULL);
//     browser_options.base_path = MALVEKE_APP_FOLDER;
//     browser_options.skip_assets = true;

//     // Input events and views are managed by file_browser
//     bool res = dialog_file_browser_show( app->dialogs, file_path, file_path, &browser_options);
//     UNUSED(res);
//     FURI_LOG_I(TAG, "File selected: %s", furi_string_get_cstr(file_path));
//     if(res) {
//         if(!file_stream_open(stream, furi_string_get_cstr(file_path), FSAM_READ, FSOM_OPEN_EXISTING)) {
//             FURI_LOG_D(TAG, "Cannot open file \"%s\"", furi_string_get_cstr(file_path));
//             file_stream_close(stream);
//         } else {
//             FURI_LOG_D(TAG, "Open file \"%s\"", furi_string_get_cstr(file_path));
//             result = true;
//         }
//     }
//     furi_string_free(file_path);
//     return result;
// }
bool gb_cartridge_scene_4_input(InputEvent* event, void* context) {
    furi_assert(context);
    GBCartridgeScene4* instance = context;

    if(event->type == InputTypeRelease) {
        switch(event->key) {
        case InputKeyBack:
            with_view_model(
                instance->view,
                GameBoyCartridgeROMWriteModel * model,
                {
                    UNUSED(model);
                    GBCartridge* app = (GBCartridge*)instance->context;
                    // Unregister rx callback
                    uart_set_handle_rx_data_cb(app->uart, NULL);
                    instance->callback(GBCartridgeCustomEventScene4Back, instance->context);
                },
                true);
            break;
        case InputKeyUp:
        case InputKeyDown:
        case InputKeyLeft:
        case InputKeyRight:
            break;
        case InputKeyOk:
            with_view_model(
                instance->view,
                GameBoyCartridgeROMWriteModel * model,
                {
                    GBCartridge* app = ((GBCartridge*)instance->context);
                    UNUSED(model);
                    UNUSED(app);
                    // uint8_t buffer[BUFFER_SIZE];
                    // size_t bytesRead;
                    // Stream* file_stream = file_stream_alloc(app->storage);

                    // if(select_rom_file(app, file_stream)) {
                    //     const char gbcartridge_start_command[] = "gbcartridge -w -o -s\n";
                    //     uart_tx(app->uart, (uint8_t*)gbcartridge_start_command, strlen(gbcartridge_start_command));
                    //     furi_delay_ms(500);

                    //     while (file_stream_read(file_stream, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
                    //         // Send 64 bytes at a time
                    //         uart_tx(app->lp_uart, (uint8_t*)buffer, bytesRead);
                    //     }

                    //     const char gbcartridge_end_command[] = "gbcartridge -w -o -e\n";
                    //     uart_tx(app->uart, (uint8_t*)gbcartridge_end_command, strlen(gbcartridge_end_command));

                    //     file_stream_close(file_stream);
                    // }
                    // stream_free(file_stream);
                },
                true);
            break;
        case InputKeyMAX:
            break;
        }
    }

    return true;
}

void gb_cartridge_scene_4_exit(void* context) {
    furi_assert(context);
    GBCartridge* app = context;
    gb_cartridge_stop_all_sound(app);
}

void gb_cartridge_scene_4_enter(void* context) {
    furi_assert(context);
    GBCartridgeScene4* instance = context;
    GBCartridge* app = (GBCartridge*)instance->context;

    UNUSED(app);
    with_view_model(
        app->gb_cartridge_scene_4->view,
        GameBoyCartridgeROMWriteModel * model,
        {
            UNUSED(model);
            gb_cartridge_scene_4_model_init(model);
            model->gameboy_rom_option_selected_text = app->gameboy_rom_option_selected_text;
        },
        false);
}

GBCartridgeScene4* gb_cartridge_scene_4_alloc() {
    GBCartridgeScene4* instance = malloc(sizeof(GBCartridgeScene4));
    instance->view = view_alloc();
    view_allocate_model(
        instance->view, ViewModelTypeLocking, sizeof(GameBoyCartridgeROMWriteModel));
    view_set_context(instance->view, instance);
    view_set_draw_callback(instance->view, (ViewDrawCallback)gb_cartridge_scene_4_draw);
    view_set_input_callback(instance->view, gb_cartridge_scene_4_input);
    view_set_enter_callback(instance->view, gb_cartridge_scene_4_enter);
    view_set_exit_callback(instance->view, gb_cartridge_scene_4_exit);

    with_view_model(
        instance->view,
        GameBoyCartridgeROMWriteModel * model,
        { gb_cartridge_scene_4_model_init(model); },
        true);

    return instance;
}

void gb_cartridge_scene_4_free(GBCartridgeScene4* instance) {
    furi_assert(instance);

    view_free(instance->view);
    free(instance);
}

View* gb_cartridge_scene_4_get_view(GBCartridgeScene4* instance) {
    furi_assert(instance);

    return instance->view;
}
