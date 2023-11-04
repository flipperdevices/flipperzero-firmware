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
#include <stdio.h>  // Para sprintf
#include <string.h> // Para strlen

struct GBCartridgeScene5
{
    View *view;
    GBCartridgeScene5Callback callback;
    void *context;
    GBCartridge *app;
};

typedef struct
{
    char *event_type;
    int progress;
    int total_ram;
    int transfered;
    int ramBanks;
    int elapsed_time;
    int start_time;

    char *cart_dump_ram_filename_sequential;
    bool rx_active;

} GameBoyCartridgeRAMWriteModel;


static bool select_ram_file(GBCartridge *app, File *file)
{
    bool result = false;
    FuriString *file_path = furi_string_alloc();
    furi_string_set(file_path, MALVEKE_APP_FOLDER);
    DialogsFileBrowserOptions browser_options;
    dialog_file_browser_set_basic_options(&browser_options, "sav", NULL);
    browser_options.base_path = MALVEKE_APP_FOLDER;
    browser_options.skip_assets = true;

    // Input events and views are managed by file_browser
    bool res = dialog_file_browser_show(app->dialogs, file_path, file_path, &browser_options);
    // UNUSED(res);
    // FURI_LOG_I(TAG, "File selected: %s", furi_string_get_cstr(file_path));
    if (res)
    {   
        if(!storage_file_open(file, furi_string_get_cstr(file_path), FSAM_READ, FSOM_OPEN_EXISTING))
        // if (!file_stream_open(stream, furi_string_get_cstr(file_path), FSAM_READ, FSOM_OPEN_EXISTING))
        {
            // FURI_LOG_D(TAG, "Cannot open file \"%s\"", furi_string_get_cstr(file_path));
            // file_stream_close(stream);
        }
        else
        {
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
    uint8_t buffer[BUFFER_SIZE];
    File* file = storage_file_alloc(app->storage);

    if (select_ram_file(app, file))
    {
        const char gbcartridge_start_command[] = "gbcartridge -w -a -s\n";
        uart_tx((uint8_t *)gbcartridge_start_command, strlen(gbcartridge_start_command));
        furi_delay_ms(100);
        
        uint16_t ret = 0;
        do {
            ret = storage_file_read(file, buffer, sizeof(buffer) - 1);
            // lp_uart_tx((uint8_t *)buffer, sizeof(buffer));
            // uart_tx((uint8_t *)buffer, sizeof(buffer))

            with_view_model(
                app->gb_cartridge_scene_5->view,
                GameBoyCartridgeRAMWriteModel * model,
                {
                    model->total_ram = storage_file_size(file);
                    model->transfered += ret;
                },
                true);
            
        } while(ret > 0);

        const char gbcartridge_end_command[] = "gbcartridge -w -a -e\n";
        uart_tx((uint8_t *)gbcartridge_end_command, strlen(gbcartridge_end_command));
        storage_file_free(file);
    }
    return 0;
}
void gb_cartridge_scene_5_set_callback(
    GBCartridgeScene5 *instance,
    GBCartridgeScene5Callback callback,
    void *context)
{
    furi_assert(instance);
    furi_assert(callback);
    instance->callback = callback;
    instance->context = context;
}
static void drawProgressBar(Canvas *canvas, int progress)
{
    for (int x = 0; x < 64 - 14 - UI_PADDING - UI_PADDING - UI_PADDING - UI_PADDING; x += 5)
    {
        for (int row = 0; row < 20; row += 5)
        {
            if (progress > 0)
            {
                canvas_draw_box(canvas, 14 /*ARROW*/ + UI_PADDING + 2 + x + 4, /*45*/ 26 + row, 4, 4);
                progress--;
            }
            else
            {
                canvas_draw_frame(canvas, 14 /*ARROW*/ + UI_PADDING + 2 + x + 4, /*45*/ 26 + row, 4, 4);
            }
        }
    }
}

void gb_cartridge_scene_5_draw(Canvas *canvas, GameBoyCartridgeRAMWriteModel *model)
{
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
    if (model->total_ram > 0 && model->transfered > 0)
    {
        progress = model->transfered * 100 / model->total_ram;
    }
    snprintf(progressText, sizeof(progressText), "%d%% Write RAM...", progress);
    canvas_draw_str_aligned(canvas, 128 / 2, 0, AlignCenter, AlignTop, progressText);
    canvas_set_font(canvas, FontSecondary);

    // char *filename = strrchr(model->cart_dump_ram_filename_sequential, '/');
    // filename++;
    // canvas_draw_str_aligned(canvas, 128/2, 12, AlignCenter, AlignTop, filename);

    char total_ram_str[20];
    snprintf(total_ram_str, sizeof(total_ram_str), "of %.2lf MiB", (double)(model->total_ram / 1024.0 / 1024.0));

    char transfered_ram_str[20];
    snprintf(transfered_ram_str, sizeof(transfered_ram_str), "%.2lf MiB", (double)(model->transfered / 1024.0 / 1024.0));

    // Calcula la Tasa de Transferencia en KiB/s
    char transfer_rate_str[20];
    if (model->transfered > 0 && model->elapsed_time > 0)
    {
        double transfer_rate_kibps = (double)model->transfered / ((double)model->elapsed_time) / (double)1024.0;
        snprintf(transfer_rate_str, sizeof(transfer_rate_str), "%.2lf KiB/s", transfer_rate_kibps);
    }
    else
    {
        snprintf(transfer_rate_str, sizeof(transfer_rate_str), "0 KiB/s");
    }

    canvas_draw_str_aligned(canvas, (128 / 2) + UI_PADDING, 22 + 2, AlignLeft, AlignTop, transfered_ram_str);
    canvas_draw_str_aligned(canvas, (128 / 2) + UI_PADDING, 40 + 2, AlignLeft, AlignTop, total_ram_str);
    canvas_draw_str_aligned(canvas, (128 / 2) + UI_PADDING, 48 + 2, AlignLeft, AlignTop, transfer_rate_str);

    if (model->rx_active)
    {
        canvas_draw_icon_ex(canvas, UI_PADDING, 28, &I_ArrowUpFilled_14x15, IconRotation180);
    }
    else
    {
        canvas_draw_icon_ex(canvas, UI_PADDING, 28, &I_ArrowUpEmpty_14x15, IconRotation180);
    }

    char totalText[42];
    snprintf(totalText, sizeof(totalText), "%d", model->total_ram);
    // canvas_draw_str(canvas, 69, 48, totalText);

    drawProgressBar(canvas, (progress * UI_PROGRESS_ROWS * UI_PROGRESS_COLS) / 100); // Pinta las primeras 10 cajas de negro
    // free(totalText);

    elements_button_center(canvas, "Write");
}

static void gb_cartridge_scene_5_model_init(GameBoyCartridgeRAMWriteModel *const model)
{
    model->progress = 0;
    model->total_ram = 0;
    model->transfered = 0;
    model->ramBanks = 0;
    model->elapsed_time = 0;
    model->start_time = 0;
}
bool gb_cartridge_scene_5_input(InputEvent *event, void *context)
{
    furi_assert(context);
    GBCartridgeScene5 *instance = context;

    if (event->type == InputTypeRelease)
    {
        switch (event->key)
        {
        case InputKeyBack:
            with_view_model(
                instance->view,
                GameBoyCartridgeRAMWriteModel * model,
                {
                    UNUSED(model);
                    GBCartridge *app = (GBCartridge *)instance->context;
                    // Unregister rx callback
                    uart_set_handle_rx_data_cb(app->uart, NULL);
                    // uart_set_handle_rx_data_cb(app->lp_uart, NULL);
                    instance->callback(GBCartridgeCustomEventScene5Back, instance->context);
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
                GameBoyCartridgeRAMWriteModel * model,
                {
                    GBCartridge *app = ((GBCartridge *)instance->context);
                    app->thread = furi_thread_alloc_ex("CartridgeWriterWorker", 2048, cartridge_writting_worker_thread, app);
                    furi_thread_start(app->thread);

                    UNUSED(model);
                    
                    // UNUSED(app);
                    // uint8_t buffer[BUFFER_SIZE];
                    // File* file = storage_file_alloc(app->storage);

                    // if (select_ram_file(app, file))
                    // {
                    //     model->total_ram = storage_file_size(file);
                    //     const char gbcartridge_start_command[] = "gbcartridge -w -o -s\n";
                    //     uart_tx((uint8_t *)gbcartridge_start_command, strlen(gbcartridge_start_command));
                    //     furi_delay_ms(500);
                        
                    //     uint16_t ret = 0;
                    //     do {
                    //         ret = storage_file_read(file, buffer, sizeof(buffer) - 1);
                    //         lp_uart_tx((uint8_t *)buffer, sizeof(buffer));
                    //         model->transfered += ret;
                    //     } while(ret > 0);
                    //     const char gbcartridge_end_command[] = "gbcartridge -w -o -e\n";
                    //     uart_tx((uint8_t *)gbcartridge_end_command, strlen(gbcartridge_end_command));
                    //     storage_file_free(file);
                    // }
                },
                true);
            break;
        case InputKeyMAX:
            break;
        }
    }

    return true;
}

void gb_cartridge_scene_5_exit(void *context)
{
    furi_assert(context);
    GBCartridge *app = context;
    gb_cartridge_stop_all_sound(app);
}

void gb_cartridge_scene_5_enter(void *context)
{
    furi_assert(context);
    GBCartridgeScene5 *instance = context;
    GBCartridge *app = (GBCartridge *)instance->context;

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

GBCartridgeScene5 *gb_cartridge_scene_5_alloc()
{
    GBCartridgeScene5 *instance = malloc(sizeof(GBCartridgeScene5));
    instance->view = view_alloc();
    view_allocate_model(instance->view, ViewModelTypeLocking, sizeof(GameBoyCartridgeRAMWriteModel));

    view_set_context(instance->view, instance);
    view_set_draw_callback(instance->view, (ViewDrawCallback)gb_cartridge_scene_5_draw);
    view_set_input_callback(instance->view, gb_cartridge_scene_5_input);
    view_set_enter_callback(instance->view, gb_cartridge_scene_5_enter);
    view_set_exit_callback(instance->view, gb_cartridge_scene_5_exit);

    with_view_model(
        instance->view,
        GameBoyCartridgeRAMWriteModel * model,
        {
            gb_cartridge_scene_5_model_init(model);
        },
        true);

    return instance;
}

void gb_cartridge_scene_5_free(GBCartridgeScene5 *instance)
{
    GBCartridge* app = instance->context;
    furi_assert(instance);
    furi_thread_free(app->thread);
    view_free(instance->view);
    free(instance);
}

View *gb_cartridge_scene_5_get_view(GBCartridgeScene5 *instance)
{
    furi_assert(instance);

    return instance->view;
}
