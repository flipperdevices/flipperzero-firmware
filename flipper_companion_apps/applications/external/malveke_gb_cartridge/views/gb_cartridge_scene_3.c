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

struct GBCartridgeScene3 {
    View* view;
    GBCartridgeScene3Callback callback;
    void* context;
    GBCartridge* app;
};
static uint64_t last_toggle_time = 0;
// static const NotificationSequence sequence_alarm = {
//     &message_display_backlight_on,
//     &message_red_255,
//     &message_vibro_on,
//     &message_note_c5,
//     &message_delay_100,
//     &message_vibro_off,
//     &message_sound_off,
//     &message_note_c7,
//     &message_delay_500,
//     &message_sound_off,
//     &message_display_backlight_off,
//     &message_red_0,
//     &message_delay_50,
//     NULL,
// };

typedef struct {
    char* event_type;
    int progress;
    int total_ram;
    int transfered;
    int ramBanks;
    int elapsed_time;
    int start_time;
    // char* cart_dump_ram_filename;
    // char* cart_dump_ram_extension;
    char* cart_dump_ram_filename_sequential;
    bool rx_active;
} GameBoyCartridgeRAMBackupModel;

void dump_ram_handle_rx_data_cb(uint8_t* buf, size_t len, void* context) {
    UNUSED(len);
    UNUSED(buf);
    GBCartridge* instance = context;
    if(instance->is_writing_ram) {
        storage_file_write(instance->cart_ram, buf, len);
    }
    with_view_model(
        instance->gb_cartridge_scene_3->view,
        GameBoyCartridgeRAMBackupModel * model,
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

void gb_cartridge_scene_3_set_callback(
    GBCartridgeScene3* instance,
    GBCartridgeScene3Callback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(callback);
    instance->callback = callback;
    instance->context = context;
}
void gameboy_ram_backup_handle_rx_data_cb(uint8_t* buf, size_t len, void* context) {
    furi_assert(context);
    UNUSED(len);
    UNUSED(buf);
    GBCartridge* instance = context;
    with_view_model(
        instance->gb_cartridge_scene_3->view,
        GameBoyCartridgeRAMBackupModel * model,
        {
            UNUSED(model);
            cJSON* json = cJSON_Parse((char*)buf);
            if(json == NULL) {
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
                    model->total_ram = total->valueint;
                } else {
                    model->total_ram = 0;
                }
                //  Progress
                cJSON* progress = cJSON_GetObjectItemCaseSensitive(json, "progress");
                if(cJSON_IsNumber(progress)) {
                    model->progress = progress->valueint;
                } else {
                    model->progress = 0;
                }
                //  RamBanks
                cJSON* ramBanks = cJSON_GetObjectItemCaseSensitive(json, "ramBanks");
                if(cJSON_IsNumber(ramBanks)) {
                    model->ramBanks = ramBanks->valueint;
                } else {
                    model->ramBanks = 0;
                }
            }
            if(strcmp(model->event_type, "success") == 0) {
                model->progress = 100;
                // if(instance->cart_ram && storage_file_is_open(instance->cart_ram)) {
                //     storage_file_close(instance->cart_ram);
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

void gb_cartridge_scene_3_draw(Canvas* canvas, GameBoyCartridgeRAMBackupModel* model) {
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
    if(model->total_ram > 0 && model->transfered > 0) {
        progress = model->transfered * 100 / model->total_ram;
    }
    snprintf(progressText, sizeof(progressText), "%d%% Dump RAM...", progress);
    canvas_draw_str_aligned(canvas, 128 / 2, 0, AlignCenter, AlignTop, progressText);
    canvas_set_font(canvas, FontSecondary);

    char* filename = strrchr(model->cart_dump_ram_filename_sequential, '/');
    filename++;
    canvas_draw_str_aligned(canvas, 128 / 2, 12, AlignCenter, AlignTop, filename);

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

    char totalText[32];
    snprintf(totalText, sizeof(totalText), "%d", model->total_ram);
    // canvas_draw_str(canvas, 69, 38, totalText);

    drawProgressBar(
        canvas,
        (progress * UI_PROGRESS_ROWS * UI_PROGRESS_COLS) /
            100); // Pinta las primeras 10 cajas de negro
    // free(totalText);

    elements_button_center(canvas, "Start");
}

static void gb_cartridge_scene_3_model_init(GameBoyCartridgeRAMBackupModel* const model) {
    model->progress = 0;
    model->total_ram = 0;
    model->transfered = 0;
    model->ramBanks = 0;
    model->elapsed_time = 0;
    model->start_time = 0;
}

bool gb_cartridge_scene_3_input(InputEvent* event, void* context) {
    furi_assert(context);
    GBCartridgeScene3* instance = context;

    if(event->type == InputTypeRelease) {
        switch(event->key) {
        case InputKeyBack:
            with_view_model(
                instance->view,
                GameBoyCartridgeRAMBackupModel * model,
                {
                    UNUSED(model);
                    GBCartridge* app = (GBCartridge*)instance->context;
                    UNUSED(app);
                    model->total_ram = 0;
                    model->transfered = 0;
                    model->elapsed_time = 0;
                    // Unregister rx callback
                    uart_set_handle_rx_data_cb(app->uart, NULL);
                    uart_set_handle_rx_data_cb(app->lp_uart, NULL);

                    app->is_writing_ram = false;
                    if(app->cart_ram && storage_file_is_open(app->cart_ram)) {
                        storage_file_close(app->cart_ram);
                    }
                    instance->callback(GBCartridgeCustomEventScene3Back, instance->context);
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
                GameBoyCartridgeRAMBackupModel * model,
                {
                    GBCartridge* app = ((GBCartridge*)instance->context);
                    UNUSED(app);
                    model->start_time =
                        furi_hal_rtc_get_timestamp(); // Registra el tiempo de inicio
                    app->cart_ram = storage_file_alloc(app->storage);

                    if(storage_file_open(
                           app->cart_ram,
                           model->cart_dump_ram_filename_sequential,
                           FSAM_WRITE,
                           FSOM_CREATE_ALWAYS)) {
                        const char gbcartridge_command[] = "gbcartridge -d -a\n";
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

void gb_cartridge_scene_3_exit(void* context) {
    furi_assert(context);
    GBCartridge* app = context;
    gb_cartridge_stop_all_sound(app);
}

void gb_cartridge_scene_3_enter(void* context) {
    furi_assert(context);
    GBCartridgeScene3* instance = context;
    GBCartridge* app = (GBCartridge*)instance->context;

    UNUSED(app);
    with_view_model(
        app->gb_cartridge_scene_3->view,
        GameBoyCartridgeRAMBackupModel * model,
        {
            // gb_cartridge_scene_3_model_init(model);
            // model->cart_dump_ram_filename  = app->cart_dump_ram_filename;
            // model->cart_dump_ram_extension = app->cart_dump_ram_extension;
            // char *filename = strrchr(sequential_file_resolve_path(app->storage, MALVEKE_APP_FOLDER_RAMS, app->cart_dump_ram_filename, app->cart_dump_ram_extension), '/');
            // filename++;
            char* filename = sequential_file_resolve_path(
                app->storage,
                MALVEKE_APP_FOLDER_RAMS,
                app->cart_dump_ram_filename,
                app->cart_dump_ram_extension);
            model->cart_dump_ram_filename_sequential = filename;
            app->is_writing_ram = true;
            // Register callbacks to receive data
            uart_set_handle_rx_data_cb(
                app->uart,
                gameboy_ram_backup_handle_rx_data_cb); // setup callback for general log rx thread
            uart_set_handle_rx_data_cb(
                app->lp_uart,
                dump_ram_handle_rx_data_cb); // setup callback for general log rx thread
        },
        false);
}

GBCartridgeScene3* gb_cartridge_scene_3_alloc() {
    GBCartridgeScene3* instance = malloc(sizeof(GBCartridgeScene3));
    instance->view = view_alloc();
    view_allocate_model(
        instance->view, ViewModelTypeLocking, sizeof(GameBoyCartridgeRAMBackupModel));
    view_set_context(instance->view, instance);
    view_set_draw_callback(instance->view, (ViewDrawCallback)gb_cartridge_scene_3_draw);
    view_set_input_callback(instance->view, gb_cartridge_scene_3_input);
    view_set_enter_callback(instance->view, gb_cartridge_scene_3_enter);
    view_set_exit_callback(instance->view, gb_cartridge_scene_3_exit);

    with_view_model(
        instance->view,
        GameBoyCartridgeRAMBackupModel * model,
        { gb_cartridge_scene_3_model_init(model); },
        true);

    return instance;
}

void gb_cartridge_scene_3_free(GBCartridgeScene3* instance) {
    furi_assert(instance);

    view_free(instance->view);
    free(instance);
}

View* gb_cartridge_scene_3_get_view(GBCartridgeScene3* instance) {
    furi_assert(instance);

    return instance->view;
}
