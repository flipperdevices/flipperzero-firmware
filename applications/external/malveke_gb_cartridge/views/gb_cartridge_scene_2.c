#include "../gb_cartridge_app.h"
#include <furi.h>
#include <furi_hal.h>
#include <input/input.h>
#include <gui/elements.h>
#include <dolphin/dolphin.h>
#include <gui/elements.h>
#include <notification/notification_messages.h>
#include "../helpers/gb_cartridge_speaker.h"
#include "../helpers/sequential_file.h"
#include <stdio.h> // Para sprintf
#include <string.h> // Para strlen

struct GBCartridgeScene2 {
    View* view;
    GBCartridgeScene2Callback callback;
    void* context;
    GBCartridge* app;
};
static uint64_t last_toggle_time = 0;

typedef struct {
    char* event_type;
    int progress;
    int total_rom;
    int transfered;
    int romBanks;
    int elapsed_time;
    int start_time;
    char* cart_dump_rom_filename;
    char* cart_dump_rom_extension;
    char* cart_dump_rom_filename_sequential;
    bool rx_active;
} GameBoyCartridgeROMBackupModel;

void dump_handle_rx_data_cb(uint8_t* buf, size_t len, void* context) {
    UNUSED(len);
    UNUSED(buf);
    GBCartridge* instance = context;
    if(instance->is_writing_rom) {
        storage_file_write(instance->cart_rom, buf, len);
    }
    with_view_model(
        instance->gb_cartridge_scene_2->view,
        GameBoyCartridgeROMBackupModel * model,
        {
            model->transfered += len;
            uint64_t current_time = furi_hal_rtc_get_timestamp();
            model->elapsed_time = current_time - model->start_time;
            if(current_time - last_toggle_time >= 0.2) {
                model->rx_active = !model->rx_active;
                last_toggle_time = current_time;
            }
        },
        true);
}

void gb_cartridge_scene_2_set_callback(
    GBCartridgeScene2* instance,
    GBCartridgeScene2Callback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(callback);
    instance->callback = callback;
    instance->context = context;
}

void gameboy_rom_backup_handle_rx_data_cb(uint8_t* buf, size_t len, void* context) {
    // furi_assert(context);
    UNUSED(len);
    // UNUSED(buf);
    GBCartridge* instance = context;

    with_view_model(
        instance->gb_cartridge_scene_2->view,
        GameBoyCartridgeROMBackupModel * model,
        {
            UNUSED(model);
            cJSON* json = cJSON_Parse((char*)buf);
            if(json == NULL) {
                // furi_string_cat_printf(model->cart_type, "%s\n", "Parse error");
                // model->cart_title_strlen = furi_string_size(model->cart_title);
            } else {
                cJSON* type = cJSON_GetObjectItemCaseSensitive(json, "type");
                if(cJSON_IsString(type) && (type->valuestring != NULL)) {
                    model->event_type = strdup(type->valuestring);
                } else {
                    model->event_type = "None";
                }
                //  Total
                cJSON* total = cJSON_GetObjectItemCaseSensitive(json, "total");
                if(cJSON_IsNumber(total)) {
                    model->total_rom = total->valueint;
                } else {
                    model->total_rom = 0;
                }
                //  Progress
                cJSON* progress = cJSON_GetObjectItemCaseSensitive(json, "progress");
                if(cJSON_IsNumber(progress)) {
                    model->progress = progress->valueint;
                } else {
                    model->progress = 0;
                }
                //  RomBanks
                cJSON* romBanks = cJSON_GetObjectItemCaseSensitive(json, "romBanks");
                if(cJSON_IsNumber(romBanks)) {
                    model->romBanks = romBanks->valueint;
                } else {
                    model->romBanks = 0;
                }
            }
            if(strcmp(model->event_type, "success") == 0) {
                model->progress = 100;
                // cJSON* total = cJSON_GetObjectItemCaseSensitive(json, "total");
                // if(cJSON_IsNumber(total)) {
                //     model->transfered = total->valueint;
                // }
                // if(instance->cart_rom && storage_file_is_open(instance->cart_rom)) {
                //     storage_file_close(instance->cart_rom);
                // }
                notification_success(instance->notification);
            }
        },
        true);
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

void gb_cartridge_scene_2_draw(Canvas* canvas, GameBoyCartridgeROMBackupModel* model) {
    // Clear the screen.
    canvas_set_color(canvas, ColorBlack);

    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontKeyboard);
    canvas_draw_frame(canvas, 0, 23, (128 / 2), 25);

    canvas_set_bitmap_mode(canvas, 1);
    canvas_set_font(canvas, FontPrimary);
    char progressText[32];
    int progress = 0;
    if(model->total_rom > 0 && model->transfered > 0) {
        progress = model->transfered * 100 / model->total_rom;
    }
    snprintf(progressText, sizeof(progressText), "%d%% Dump ROM...", progress);
    canvas_draw_str_aligned(canvas, 128 / 2, 0, AlignCenter, AlignTop, progressText);
    canvas_set_font(canvas, FontSecondary);

    char* filename = strrchr(model->cart_dump_rom_filename_sequential, '/');
    filename++;

    canvas_draw_str_aligned(canvas, 128 / 2, 12, AlignCenter, AlignTop, filename);

    char total_rom_str[20]; // Declarar un buffer para almacenar la versión en cadena del entero
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
        canvas, (128 / 2) + UI_PADDING, 30 + 2, AlignLeft, AlignTop, total_rom_str);
    canvas_draw_str_aligned(
        canvas, (128 / 2) + UI_PADDING, 38 + 2, AlignLeft, AlignTop, transfer_rate_str);

    if(model->rx_active) {
        canvas_draw_icon_ex(canvas, UI_PADDING, 28, &I_ArrowUpFilled_14x15, IconRotation180);
    } else {
        canvas_draw_icon_ex(canvas, UI_PADDING, 28, &I_ArrowUpEmpty_14x15, IconRotation180);
    }

    char totalText[32];
    snprintf(totalText, sizeof(totalText), "%d", model->total_rom);

    drawProgressBar(
        canvas,
        (progress * UI_PROGRESS_ROWS * UI_PROGRESS_COLS) /
            100); // Pinta las primeras 10 cajas de negro

    elements_button_center(canvas, "Start");
}

static void gb_cartridge_scene_2_model_init(GameBoyCartridgeROMBackupModel* const model) {
    // model->screen_text = 0;
    model->progress = 0;
    model->total_rom = 0;
    model->transfered = 0;
    model->romBanks = 0;
    model->elapsed_time = 0;
    model->start_time = 0;
}

bool gb_cartridge_scene_2_input(InputEvent* event, void* context) {
    furi_assert(context);
    GBCartridgeScene2* instance = context;

    if(event->type == InputTypeRelease) {
        switch(event->key) {
        case InputKeyBack:
            with_view_model(
                instance->view,
                GameBoyCartridgeROMBackupModel * model,
                {
                    UNUSED(model);
                    GBCartridge* app = (GBCartridge*)instance->context;
                    UNUSED(app);

                    model->total_rom = 0;
                    model->transfered = 0;
                    model->elapsed_time = 0;

                    // // Unregister rx callback
                    uart_set_handle_rx_data_cb(app->uart, NULL);
                    uart_set_handle_rx_data_cb(app->lp_uart, NULL);
                    //  Close file
                    app->is_writing_rom = false;
                    if(app->cart_rom && storage_file_is_open(app->cart_rom)) {
                        storage_file_close(app->cart_rom);
                    }
                    notification_message(
                        app->notification, &sequence_display_backlight_enforce_auto);
                    instance->callback(GBCartridgeCustomEventScene2Back, instance->context);
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
                GameBoyCartridgeROMBackupModel * model,
                {
                    GBCartridge* app = ((GBCartridge*)instance->context);
                    UNUSED(app);
                    model->start_time =
                        furi_hal_rtc_get_timestamp(); // Registra el tiempo de inicio
                    app->cart_rom = storage_file_alloc(app->storage);

                    if(storage_file_open(
                           app->cart_rom,
                           model->cart_dump_rom_filename_sequential,
                           FSAM_WRITE,
                           FSOM_CREATE_ALWAYS)) {
                        const char gbcartridge_command[] = "gbcartridge -d -o\n";
                        uart_tx(
                            app->uart, (uint8_t*)gbcartridge_command, strlen(gbcartridge_command));
                    } else {
                        dialog_message_show_storage_error(app->dialogs, "Cannot open dump file");
                    }
                },
                true);

            break;
        case InputKeyMAX:
            break;
        }
    }

    return true;
}

void gb_cartridge_scene_2_exit(void* context) {
    furi_assert(context);
    GBCartridge* app = context;
    gb_cartridge_stop_all_sound(app);
}

void gb_cartridge_scene_2_enter(void* context) {
    furi_assert(context);
    GBCartridgeScene2* instance = context;
    GBCartridge* app = (GBCartridge*)instance->context;
    //  backlight on
    notification_message(app->notification, &sequence_display_backlight_enforce_on);
    // furi_hal_power_disable_otg();
    UNUSED(app);
    // dolphin_deed(DolphinDeedPluginStart);
    with_view_model(
        app->gb_cartridge_scene_2->view,
        GameBoyCartridgeROMBackupModel * model,
        {
            model->cart_dump_rom_filename = app->cart_dump_rom_filename;
            model->cart_dump_rom_extension = app->cart_dump_rom_extension;
            model->total_rom = app->rom_banks * 16 * 1024;
            // char *filename = strrchr(model->cart_dump_rom_filename_sequential, '/');
            // filename++;
            char* filename = sequential_file_resolve_path(
                app->storage,
                MALVEKE_APP_FOLDER_ROMS,
                app->cart_dump_rom_filename,
                model->cart_dump_rom_extension);
            model->cart_dump_rom_filename_sequential = filename;
            // Register callbacks to receive data
            uart_set_handle_rx_data_cb(
                app->uart,
                gameboy_rom_backup_handle_rx_data_cb); // setup callback for general log rx thread
            uart_set_handle_rx_data_cb(
                app->lp_uart, dump_handle_rx_data_cb); // setup callback for general log rx thread
            app->is_writing_rom = true;
        },
        false);
}

GBCartridgeScene2* gb_cartridge_scene_2_alloc() {
    GBCartridgeScene2* instance = malloc(sizeof(GBCartridgeScene2));
    instance->view = view_alloc();
    view_allocate_model(
        instance->view, ViewModelTypeLocking, sizeof(GameBoyCartridgeROMBackupModel));
    view_set_context(instance->view, instance);
    view_set_draw_callback(instance->view, (ViewDrawCallback)gb_cartridge_scene_2_draw);
    view_set_input_callback(instance->view, gb_cartridge_scene_2_input);
    view_set_enter_callback(instance->view, gb_cartridge_scene_2_enter);
    view_set_exit_callback(instance->view, gb_cartridge_scene_2_exit);

    with_view_model(
        instance->view,
        GameBoyCartridgeROMBackupModel * model,
        { gb_cartridge_scene_2_model_init(model); },
        true);

    return instance;
}

void gb_cartridge_scene_2_free(GBCartridgeScene2* instance) {
    furi_assert(instance);

    view_free(instance->view);
    free(instance);
}

View* gb_cartridge_scene_2_get_view(GBCartridgeScene2* instance) {
    furi_assert(instance);

    return instance->view;
}
