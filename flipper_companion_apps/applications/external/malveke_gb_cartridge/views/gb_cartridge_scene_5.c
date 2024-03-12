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

static uint64_t last_toggle_time = 0;
struct GBCartridgeScene5 {
    View* view;
    GBCartridgeScene5Callback callback;
    void* context;
    GBCartridge* app;
};

typedef struct {
    char* event_type;
    int progress;
    int total_ram;
    int transfered;
    int ramBanks;
    int elapsed_time;
    int start_time;

    char* cart_dump_ram_filename_sequential;
    bool rx_active;

    char* event_title;

    uint32_t offset;
    uint32_t value;

    File* selectedfile;

} GameBoyCartridgeRAMWriteModel;

static bool select_ram_file(GBCartridge* app, File* file) {
    bool result = false;
    FuriString* file_path = furi_string_alloc();
    furi_string_set(file_path, MALVEKE_APP_FOLDER_RAMS);
    DialogsFileBrowserOptions browser_options;
    dialog_file_browser_set_basic_options(&browser_options, "sav", NULL);
    browser_options.base_path = MALVEKE_APP_FOLDER_RAMS;
    browser_options.skip_assets = true;

    // Input events and views are managed by file_browser
    bool res = dialog_file_browser_show(app->dialogs, file_path, file_path, &browser_options);
    // UNUSED(res);
    // FURI_LOG_I(TAG, "File selected: %s", furi_string_get_cstr(file_path));
    if(res) {
        if(!storage_file_open(file, furi_string_get_cstr(file_path), FSAM_READ, FSOM_OPEN_EXISTING))
        // if (!file_stream_open(stream, furi_string_get_cstr(file_path), FSAM_READ, FSOM_OPEN_EXISTING))
        {
            // FURI_LOG_D(TAG, "Cannot open file \"%s\"", furi_string_get_cstr(file_path));
            // file_stream_close(stream);
        } else {
            // FURI_LOG_D(TAG, "Open file \"%s\"", furi_string_get_cstr(file_path));
            result = true;
        }
    }
    furi_string_free(file_path);
    return result;
}

static int32_t cartridge_writting_worker_thread(void* thread_context) {
    GBCartridge* app = thread_context;
    UNUSED(app);
    File* file = storage_file_alloc(app->storage);

    if(select_ram_file(app, file)) {
        uint16_t fileSize = storage_file_size(file);

        FURI_LOG_I(TAG, "fileSize: %d ", fileSize);
        with_view_model(
            app->gb_cartridge_scene_5->view,
            GameBoyCartridgeRAMWriteModel * model,
            {
                model->total_ram = fileSize;
                model->selectedfile = file;
            },
            true);

        char gbcartridge_start_command[80]; // A reasonably sized buffer.
        snprintf(
            gbcartridge_start_command,
            sizeof(gbcartridge_start_command),
            "gbcartridge -w -a %d\n",
            fileSize);

        uart_tx(app->uart, (uint8_t*)gbcartridge_start_command, strlen(gbcartridge_start_command));

        furi_delay_ms(500); // wait
        uint8_t* the_savefile = NULL;
        size_t savefile_size = 0;
        with_view_model(
            app->gb_cartridge_scene_5->view,
            GameBoyCartridgeRAMWriteModel * model,
            {
                model->event_title = "Transferring...";
                model->transfered = 0;
                model->start_time = furi_hal_rtc_get_timestamp(); // Registra el tiempo de inicio
            },
            true);
        the_savefile = malloc(fileSize); // to be freed by caller
        uint8_t* buf_ptr = the_savefile;
        size_t read = 0;
        while(read < fileSize) {
            size_t to_read = fileSize - read;
            if(to_read > UINT16_MAX) to_read = UINT16_MAX;
            uint16_t now_read = storage_file_read(file, buf_ptr, (uint16_t)to_read);
            read += now_read;
            buf_ptr += now_read;
        }
        savefile_size = read;
        uart_tx(app->uart, (uint8_t*)the_savefile, savefile_size);
        uart_tx(app->uart, (uint8_t*)("\n"), 1);
        with_view_model(
            app->gb_cartridge_scene_5->view,
            GameBoyCartridgeRAMWriteModel * model,
            { model->event_title = "Writing Cartridge..."; },
            true);
        free(the_savefile);

        if(file && storage_file_is_open(file)) {
            storage_file_close(file);
        }
    }

    return 0;
}
void gb_cartridge_scene_5_set_callback(
    GBCartridgeScene5* instance,
    GBCartridgeScene5Callback callback,
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

void gb_cartridge_scene_5_draw(Canvas* canvas, GameBoyCartridgeRAMWriteModel* model) {
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
    if(model->total_ram > 0 && model->transfered > 0) {
        progress = model->transfered * 100 / model->total_ram;
    }
    snprintf(progressText, sizeof(progressText), "%d%% Write RAM...", progress);
    canvas_draw_str_aligned(canvas, 128 / 2, 0, AlignCenter, AlignTop, progressText);
    canvas_set_font(canvas, FontSecondary);

    canvas_draw_str_aligned(canvas, 128 / 2, 12, AlignCenter, AlignTop, model->event_title);

    char total_ram_str[20];
    snprintf(
        total_ram_str,
        sizeof(total_ram_str),
        "of %.2lf MiB",
        (double)(model->total_ram / 1024.0 / 1024.0));

    char transfered_ram_str[20];
    snprintf(
        transfered_ram_str,
        sizeof(transfered_ram_str),
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
        canvas, (128 / 2) + UI_PADDING, 22 + 2, AlignLeft, AlignTop, transfered_ram_str);
    canvas_draw_str_aligned(
        canvas, (128 / 2) + UI_PADDING, 30 + 2, AlignLeft, AlignTop, total_ram_str);
    canvas_draw_str_aligned(
        canvas, (128 / 2) + UI_PADDING, 38 + 2, AlignLeft, AlignTop, transfer_rate_str);

    if(model->rx_active) {
        canvas_draw_icon_ex(canvas, UI_PADDING, 28, &I_ArrowUpFilled_14x15, IconRotation180);
    } else {
        canvas_draw_icon_ex(canvas, UI_PADDING, 28, &I_ArrowUpEmpty_14x15, IconRotation180);
    }

    char totalText[42];
    snprintf(totalText, sizeof(totalText), "%d", model->total_ram);

    drawProgressBar(
        canvas,
        (progress * UI_PROGRESS_ROWS * UI_PROGRESS_COLS) /
            100); // Pinta las primeras 10 cajas de negro

    elements_button_center(canvas, "Write");
}

static void gb_cartridge_scene_5_model_init(GameBoyCartridgeRAMWriteModel* const model) {
    model->progress = 0;
    model->total_ram = 0;
    model->transfered = 0;
    model->ramBanks = 0;
    model->elapsed_time = 0;
    model->start_time = 0;
    model->event_title = "...";
}
void gameboy_handle_rx_data_cb(uint8_t* buf, size_t len, void* context) {
    furi_assert(context);
    UNUSED(len);
    UNUSED(buf);
    GBCartridge* instance = context;

    with_view_model(
        instance->gb_cartridge_scene_5->view,
        GameBoyCartridgeRAMWriteModel * model,
        {
            UNUSED(model);

            uint64_t current_time = furi_hal_rtc_get_timestamp();
            model->elapsed_time = current_time - model->start_time;
            if(current_time - last_toggle_time >= 0.2) {
                model->rx_active = !model->rx_active;
                last_toggle_time = current_time;
            }

            cJSON* json = cJSON_Parse((char*)buf);
            if(json == NULL) {
            } else {
                cJSON* type = cJSON_GetObjectItemCaseSensitive(json, "type");
                if(cJSON_IsString(type) && (type->valuestring != NULL)) {
                    model->event_type = strdup(type->valuestring);
                } else {
                    model->event_type = "None";
                }

                //  offset
                cJSON* offset = cJSON_GetObjectItemCaseSensitive(json, "offset");
                if(cJSON_IsNumber(offset)) {
                    model->offset = offset->valueint;
                } else {
                    model->offset = 0;
                }

                //  value
                cJSON* value = cJSON_GetObjectItemCaseSensitive(json, "value");
                if(cJSON_IsNumber(value)) {
                    model->value = value->valueint;
                } else {
                    model->value = 0;
                }
            }
            if(strcmp(model->event_type, "progress") == 0) {
                //  progress
                cJSON* progress = cJSON_GetObjectItemCaseSensitive(json, "progress");
                if(cJSON_IsNumber(progress)) {
                    model->transfered += progress->valueint;
                }
            }
            if(strcmp(model->event_type, "success") == 0) {
                notification_success(instance->notification);
                model->transfered = model->total_ram;
                model->event_title = "Done!";
            }
        },
        true);
}
bool gb_cartridge_scene_5_input(InputEvent* event, void* context) {
    furi_assert(context);
    GBCartridgeScene5* instance = context;

    if(event->type == InputTypeRelease) {
        switch(event->key) {
        case InputKeyBack:
            with_view_model(
                instance->view,
                GameBoyCartridgeRAMWriteModel * model,
                {
                    UNUSED(model);
                    GBCartridge* app = (GBCartridge*)instance->context;
                    // Unregister rx callback
                    uart_set_handle_rx_data_cb(app->uart, NULL);
                    uart_set_handle_rx_data_cb(app->lp_uart, NULL);

                    instance->callback(GBCartridgeCustomEventScene5Back, instance->context);
                },
                true);
            break;
        case InputKeyUp:
        case InputKeyDown:
        case InputKeyLeft:
        case InputKeyRight:
            break;
        case InputKeyOk: {
            GBCartridge* app = ((GBCartridge*)instance->context);
            uart_set_handle_rx_data_cb(app->uart, gameboy_handle_rx_data_cb);
            cartridge_writting_worker_thread(app);
        } break;
        case InputKeyMAX:
            break;
        }
    }

    return true;
}

void gb_cartridge_scene_5_exit(void* context) {
    furi_assert(context);
    GBCartridge* app = context;
    gb_cartridge_stop_all_sound(app);
}

void gb_cartridge_scene_5_enter(void* context) {
    furi_assert(context);
    GBCartridgeScene5* instance = context;
    GBCartridge* app = (GBCartridge*)instance->context;

    UNUSED(app);
    with_view_model(
        app->gb_cartridge_scene_5->view,
        GameBoyCartridgeRAMWriteModel * model,
        {
            UNUSED(model);
            gb_cartridge_scene_5_model_init(model);
        },
        false);
}

GBCartridgeScene5* gb_cartridge_scene_5_alloc() {
    GBCartridgeScene5* instance = malloc(sizeof(GBCartridgeScene5));
    instance->view = view_alloc();
    view_allocate_model(
        instance->view, ViewModelTypeLocking, sizeof(GameBoyCartridgeRAMWriteModel));

    view_set_context(instance->view, instance);
    view_set_draw_callback(instance->view, (ViewDrawCallback)gb_cartridge_scene_5_draw);
    view_set_input_callback(instance->view, gb_cartridge_scene_5_input);
    view_set_enter_callback(instance->view, gb_cartridge_scene_5_enter);
    view_set_exit_callback(instance->view, gb_cartridge_scene_5_exit);

    with_view_model(
        instance->view,
        GameBoyCartridgeRAMWriteModel * model,
        { gb_cartridge_scene_5_model_init(model); },
        true);

    return instance;
}

void gb_cartridge_scene_5_free(GBCartridgeScene5* instance) {
    GBCartridge* app = instance->context;
    UNUSED(app);
    furi_assert(instance);
    view_free(instance->view);
    free(instance);
}

View* gb_cartridge_scene_5_get_view(GBCartridgeScene5* instance) {
    furi_assert(instance);

    return instance->view;
}
