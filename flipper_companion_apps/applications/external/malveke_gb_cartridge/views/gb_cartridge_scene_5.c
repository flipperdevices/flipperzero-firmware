#include "../gb_cartridge_app.h"
#include <furi.h>
#include <furi_hal.h>
#include <furi_hal_uart.h>
#include <stm32wbxx_ll_lpuart.h>
#include <stm32wbxx_ll_usart.h>
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
// #include <stm32wbxx_ll_uart.h>

// static bool waiting_acknowledgement = true;
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

    bool waiting_ack;


    char* event_title;

    uint32_t offset;
    uint32_t value;

    File* selectedfile;

} GameBoyCartridgeRAMWriteModel;
/*
void gameboy_log_handle_rx_data_cb(uint8_t* buf, size_t len, void* context) {
    furi_assert(context);
    GBCartridge* instance = context;
    UNUSED(len);
    UNUSED(buf);
    UNUSED(instance);
    storage_file_write(instance->cart_log, buf, len);
    // with_view_model(
    //     instance->gb_cartridge_scene_5->view,
    //     GameBoyCartridgeRAMWriteModel * model,
    //     { 
    //         model->transfered -= len;
    //         // uint64_t current_time = furi_hal_rtc_get_timestamp();
    //         // model->elapsed_time = current_time - model->start_time;
    //         // if (current_time - last_toggle_time >= 0.2) {
    //         //     model->rx_active = !model->rx_active;
    //         //     // last_toggle_time = current_time;
    //         // }
    //     },
    //     true);

}
*/
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
/*
static void wait_for_ack (void* context) {
    GBCartridge* instance = context;
    bool waiting_acknowledgement = true;
    while(waiting_acknowledgement) {
        with_view_model(
                instance->gb_cartridge_scene_5->view,
                GameBoyCartridgeRAMWriteModel * model,
                {
                    waiting_acknowledgement = model->waiting_ack;
                },
                true);
        furi_delay_ms(10);
    }
    // if(!waiting_acknowledgement) {
    //     with_view_model(
    //             instance->gb_cartridge_scene_5->view,
    //             GameBoyCartridgeRAMWriteModel * model,
    //             {
    //                 model->waiting_ack = true;
    //             },
    //             true);
    // }
}
*/
// static bool to_hex(char* dest, size_t dest_len, const uint8_t* values, size_t val_len) {
//     if(dest_len < (val_len*2+1)) /* check that dest is large enough */
//         return false;
//     *dest = '\0'; /* in case val_len==0 */
//     while(val_len--) {
//         /* sprintf directly to where dest points */
//         sprintf(dest, "%02X", *values);
//         dest += 2;
//         ++values;
//     }
//     return true;
// }


static int32_t cartridge_writting_worker_thread(void* thread_context) {
    GBCartridge* app = thread_context;
    UNUSED(app);
    /*
    uint8_t buffer[8];
    buffer[0] = 'A';
    buffer[1] = 'B';
    buffer[2] = 'C';
    buffer[3] = 'D';

    buffer[4] = 'E';
    buffer[5] = 'F';
    buffer[6] = 'G';
    buffer[7] = 'H';

    char gbcartridge_start_command[80]; // A reasonably sized buffer.
    snprintf(gbcartridge_start_command, sizeof(gbcartridge_start_command), "gbcartridge -w -a %d\n", 8);
    uart_tx((uint8_t *)gbcartridge_start_command, strlen(gbcartridge_start_command));
    furi_delay_ms(20);

    uint8_t buffer2[4];
    int x = 0;
    for(int i = 0; i < 4; i++) {
        buffer2[x] = buffer[i];
        x++;
    }
    uart_tx(buffer2, sizeof(buffer2));
    x = 0;
    for(int i = 4; i < 8; i++) {
        buffer2[x] = buffer[i];
        x++;
    }
    uart_tx(buffer2, sizeof(buffer2));
    */
    
    // uint8_t data_buffer[BUFFER_SIZE];
    File* file = storage_file_alloc(app->storage);

    if (select_ram_file(app, file))
    {
        uint16_t fileSize = storage_file_size(file);
        
        /*
        uint8_t buffer[BUFFER_SIZE];
        */

       with_view_model(
            app->gb_cartridge_scene_5->view,
            GameBoyCartridgeRAMWriteModel * model,
            {
                model->total_ram = fileSize;
                model->selectedfile = file;
                model->waiting_ack = true;

                
                // 

                // char title[42];
                // snprintf(title, sizeof(title), "%lld", fileSize);
                // // Asignar memoria para event_title y copiar el contenido de title
                // model->event_title = (char*)malloc(strlen(title) + 1);
                
                // strcpy(model->event_title, title);


                //  dev
                // model->offset = 64;
                // uint8_t buffer[64];
                // if(storage_file_seek(file, 64, true)) {
                //     size_t to_read = MIN((size_t)fileSize, sizeof(buffer));
                //     uint16_t bytes_read = storage_file_read(file, buffer, to_read);
                //     if (bytes_read > 0) {
                //         cJSON *monitor = cJSON_CreateObject();
                //         cJSON *name = cJSON_CreateString("Awesome 4K");
                //         cJSON *resolutions = cJSON_CreateArray();

                //         for (size_t i = 0; i < bytes_read; i++) {
                //             cJSON_AddItemToArray(resolutions, cJSON_CreateNumber((int)buffer[i]));
                //         }
                //         cJSON_AddItemToObject(monitor, "resolutions", resolutions);

                //         cJSON_AddItemToObject(monitor, "name", name);
                //         char *string = cJSON_PrintUnformatted(monitor);

                //         FURI_LOG_I(TAG, string);
                        
                //     }
                // }
            },
            true);


        char gbcartridge_start_command[80]; // A reasonably sized buffer.
        snprintf(gbcartridge_start_command, sizeof(gbcartridge_start_command), "gbcartridge -w -a %d %d\n", fileSize, BUFFER_SIZE);
        // snprintf(gbcartridge_start_command, sizeof(gbcartridge_start_command), "gbcartridge -t %d\n", 128);
   
        uart_tx((uint8_t *)gbcartridge_start_command, strlen(gbcartridge_start_command));
        // furi_delay_ms(100);

            // furi_delay_ms(500);

        FURI_LOG_I(TAG, "Start send file..." );
        
        uint8_t payload[BUFFER_SIZE];
        int j = 0;
        
        while(fileSize > 0) {
            size_t to_read = MIN(fileSize, BUFFER_SIZE);
            uint16_t num_bytes = storage_file_read(file, payload, to_read);
            // FURI_LOG_I(TAG, "[%d] fileSize: %d , bytes_read: %d - to read: %d" , j, fileSize, num_bytes,  to_read);
            if(num_bytes > 0) {
                // payload[BUFFER_SIZE] = '\0';
                // furi_hal_console_tx_with_new_line((uint8_t *)payload, num_bytes);
                // uart_tx((uint8_t *)payload, num_bytes);
                FURI_CRITICAL_ENTER();
                // Transmit data
                furi_hal_uart_tx(FuriHalUartIdUSART1, payload, num_bytes);
                if(fileSize - num_bytes == 0) {
                    furi_hal_uart_tx(FuriHalUartIdUSART1, (uint8_t*)("\r\n"),2);
                }
                
                while(!LL_USART_IsActiveFlag_TC(USART1))
                    ;
                FURI_CRITICAL_EXIT();

                
                // Transmit new line symbols
                // uart_tx((uint8_t*)("\n"), 1);
                // Wait for TC flag to be raised for last char
                
                // wait_for_ack(app);
                // with_view_model(
                //             app->gb_cartridge_scene_5->view,
                //             GameBoyCartridgeRAMWriteModel * model,
                //             {
                //                 model->transfered += num_bytes;
                //                 model->waiting_ack = true;
                //             },
                //             true);
                

                 
                // NVIC_DisableIRQ(USART1_IRQn);
                // LL_USART_EnableIT_RXNE_RXFNE(USART1);
                // while(true) {
                //     uint8_t data = LL_USART_ReceiveData8(USART1);
                //     if(data) {
                //         FURI_LOG_I(TAG, "read: %d" , data);
                //         break;
                //     }
                // }
                // NVIC_EnableIRQ(USART1_IRQn);
                fileSize -= num_bytes;
                j++;
            }
        }
        
        FURI_LOG_I(TAG, "Read all file." );
        /*
        // uint8_t data[1];
        int counter = 0;
        while (fileSize > 0) {
            size_t to_read = MIN(fileSize, sizeof(buffer));
            uint16_t bytes_read = storage_file_read(file, buffer, to_read);
            
            if (bytes_read > 0) {

                uart_tx((uint8_t *)buffer, bytes_read);
                
                
                // furi_delay_ms(500);
                // while(!LL_USART_IsActiveFlag_TC(USART1));
                
                // if(counter == 1) {
                //     break;
                // }
                // while(true) {
                //     // while(LL_USART_IsActiveFlag_TXE(USART1))
                //     // ;
                //     uint8_t data = LL_USART_ReceiveData8(USART1);
                //     if(data) {
                //         break;
                //     }
                // }
                // furi_delay_ms(1);
                fileSize -= bytes_read;

                // furi_delay_ms(2);
                
                    with_view_model(
                        app->gb_cartridge_scene_5->view,
                        GameBoyCartridgeRAMWriteModel * model,
                        {
                            model->transfered += bytes_read;
                        },
                        true);
                counter++;
            } else {
                break;
            }
        }
        storage_file_close(file);

        // const char gbcartridge_end_command[] = "gbcartridge -w -a\n";
        // uart_tx((uint8_t *)gbcartridge_end_command, strlen(gbcartridge_end_command));
        storage_file_free(file);
        */
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
    canvas_draw_str_aligned(canvas, 128/2, 12, AlignCenter, AlignTop, model->event_title);

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
    model->event_title = "...";
    model->waiting_ack = true;
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
            cJSON* json = cJSON_Parse((char*)buf);
            if(json == NULL) {
            } else {
                cJSON* type = cJSON_GetObjectItemCaseSensitive(json, "type");
                if(cJSON_IsString(type) && (type->valuestring != NULL)) {
                    model->event_type = strdup(type->valuestring);
                } else {
                    model->event_type = "None";
                }
                cJSON* str = cJSON_GetObjectItemCaseSensitive(json, "str");
                if(cJSON_IsString(str) && (str->valuestring != NULL)) {
                    model->event_title = strdup(str->valuestring);
                } else {
                    model->event_title = "None";
                }

                cJSON* x1 = cJSON_GetObjectItemCaseSensitive(json, "x1");
                if(cJSON_IsString(x1) && (x1->valuestring != NULL)) {
                    FURI_LOG_I(TAG, "X1: %s", strdup(x1->valuestring));
                } 
                cJSON* x2 = cJSON_GetObjectItemCaseSensitive(json, "x2");
                if(cJSON_IsString(x2) && (x2->valuestring != NULL)) {
                    FURI_LOG_I(TAG, "X2: %s", strdup(x2->valuestring));
                } 
                cJSON* x3 = cJSON_GetObjectItemCaseSensitive(json, "x3");
                if(cJSON_IsString(x3) && (x3->valuestring != NULL)) {
                    FURI_LOG_I(TAG, "X3: %s", strdup(x3->valuestring));
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
            if (strcmp(model->event_type, "ack") == 0) {
                FURI_LOG_I(TAG, "ACK: %ld (%d)", model->value, model->total_ram);
                // model->transfered += 1;
                // waiting_acknowledgement = false;
                model->waiting_ack = false;
            }
            if (strcmp(model->event_type, "success") == 0) {
                notification_success(instance->notification);
            }
            else if (strcmp(model->event_type, "read") == 0) {
                // furi_delay_ms(20);
                FURI_LOG_I(TAG, "Read: %ld - %ld (%d)" , model->offset, model->value, model->total_ram);
                // model->value
                // 
                furi_check(storage_file_is_open(model->selectedfile));
                if(storage_file_seek(model->selectedfile, model->offset, true)) {

                    uint8_t buffer[BUFFER_SIZE];
                    size_t remaining = model->total_ram - model->offset;
                    size_t to_read;

                    if (remaining > 0) {

                        to_read = MIN(remaining, (size_t)BUFFER_SIZE);   
                        uint16_t bytes_read = storage_file_read(model->selectedfile, buffer, to_read);
                        if (bytes_read > 0) {
                            // furi_delay_ms(200);
                            furi_hal_console_tx_with_new_line((uint8_t *)buffer, bytes_read);
                            // uart_tx((uint8_t*)"\n", 1);
                            FURI_LOG_I(TAG, "offset: %ld , bytes_read: %d - to read: %d" , model->offset, bytes_read,  to_read);
                            model->transfered += bytes_read;
                        }
                        
                    }
                }
            }
        },
        true);
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
                    uart_set_handle_rx_data_cb(app->lp_uart, NULL);
                    uart_set_handle_rx_data_cb(app->uart, NULL);

                    // if(app->cart_log && storage_file_is_open(app->cart_log)) {
                    //     storage_file_close(app->cart_log);
                    // }
                    
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
        {
            GBCartridge *app = ((GBCartridge *)instance->context);
            uart_set_handle_rx_data_cb(app->uart, gameboy_handle_rx_data_cb);
            cartridge_writting_worker_thread(app);
        }
            // with_view_model(
            //     instance->view,
            //     GameBoyCartridgeRAMWriteModel * model,
            //     {
            //         GBCartridge *app = ((GBCartridge *)instance->context);
            //         // app->cart_log = storage_file_alloc(app->storage);
            //         // char *filename = sequential_file_resolve_path(app->storage, MALVEKE_APP_FOLDER, "logs", "txt");
            //         // if(storage_file_open(app->cart_log, filename, FSAM_WRITE, FSOM_CREATE_ALWAYS)) {
            //             uart_set_handle_rx_data_cb(app->uart, gameboy_handle_rx_data_cb);
            //             // uart_set_handle_rx_data_cb(app->lp_uart, gameboy_log_handle_rx_data_cb);
                        
            //             app->thread = furi_thread_alloc_ex("CartridgeWriterWorker", 2048, cartridge_writting_worker_thread, app);
            //             furi_thread_start(app->thread);
            //         //    furi_delay_ms(10);
            //         //     cartridge_writting_worker_thread(app);
            //         // }
                    

            //         UNUSED(model);
                    
            //         // UNUSED(app);
            //         // uint8_t buffer[BUFFER_SIZE];
            //         // File* file = storage_file_alloc(app->storage);

            //         // if (select_ram_file(app, file))
            //         // {
            //         //     model->total_ram = storage_file_size(file);
            //         //     const char gbcartridge_start_command[] = "gbcartridge -w -o -s\n";
            //         //     uart_tx((uint8_t *)gbcartridge_start_command, strlen(gbcartridge_start_command));
            //         //     furi_delay_ms(500);
                        
            //         //     uint16_t ret = 0;
            //         //     do {
            //         //         ret = storage_file_read(file, buffer, sizeof(buffer) - 1);
            //         //         lp_uart_tx((uint8_t *)buffer, sizeof(buffer));
            //         //         model->transfered += ret;
            //         //     } while(ret > 0);
            //         //     const char gbcartridge_end_command[] = "gbcartridge -w -o -e\n";
            //         //     uart_tx((uint8_t *)gbcartridge_end_command, strlen(gbcartridge_end_command));
            //         //     storage_file_free(file);
            //         // }
            //     },
            //     true);
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
