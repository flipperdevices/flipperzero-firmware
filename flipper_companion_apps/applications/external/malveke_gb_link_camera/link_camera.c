#include "link_camera.h"
#include <gui/elements.h>
#include <malveke_gb_link_camera_icons.h>
// #include <u8g2_glue.h>

QRCode qrcode;
uint8_t qrcodeData[((((4 * 8 + 17) * (4 * 8 + 17)) + 7) / 8)];


static uint32_t link_camera_exit(void* context) {
    UNUSED(context);
    return VIEW_NONE;
}

static void view_draw_callback(Canvas* canvas, void* _model) {
    LinkCameraModel* model = _model;
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontPrimary);
    

    if(model->connected) {
        // canvas_draw_str_aligned(canvas, 128/2, 64/3, AlignCenter, AlignCenter, "Connected");
        // canvas_draw_str_aligned(canvas, 128/2, 64/2, AlignCenter, AlignCenter, model->ip);
        /*
        char ssid[64]; // Incrementa el tamaño a 64
        char password[64]; // Incrementa el tamaño a 64
        char qrstr[150]; // Deja qrstr con el tamaño actual
        snprintf(ssid, sizeof(ssid), "%s", "[MALVEKE] Flipper GB Camera");
        snprintf(password, sizeof(password), "%s", "12345678");
        snprintf(qrstr, sizeof(qrstr), "WIFI:S:%s;T:WPA2;P:%s;;", ssid, password);
        qrcode_initText(&qrcode, qrcodeData, 2, 0, qrstr);
        // canvas_draw_frame(canvas, 63, 63, 0, 0);

        for (uint8_t y = 0; y < qrcode.size; y++) {
            // Cada módulo horizontal
            for (uint8_t x = 0; x < qrcode.size; x++) {
                if (qrcode_getModule(&qrcode, x, y)) {
                    if(qrcode_getModule(&qrcode, x, y)) {
                        // canvas_draw_dot(canvas, x, y);
                        int scale = 2;
                        // // Dibuja cada punto dos veces para escalar al doble
                        canvas_draw_dot(canvas, (x * scale) + 2, (y * scale) + 4);
                        canvas_draw_dot(canvas, (x * scale + 1) + 2, (y * scale) + 4);
                        canvas_draw_dot(canvas, (x * scale) + 2, (y * scale + 1) + 4);
                        canvas_draw_dot(canvas, (x * scale + 1) + 2, (y * scale + 1) + 4);
                    }
                }
            }
        }
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str(canvas, 62, 11, "SSID");
        canvas_set_font(canvas, FontSecondary);
        // u8g2_SetFont(&canvas->fb, u8g2_font_5x7_tf); //u8g2_font_micro_tr);
        // canvas_set_font(canvas, FontBatteryPercent);
        canvas_draw_str(canvas, 62, 20, "[MALVEKE] Flipper GB Camera");
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str(canvas, 62, 30, "Password");
        canvas_set_font(canvas, FontSecondary);
        // canvas_set_font(canvas, u8g2_font_5x7_tf);
        canvas_draw_str(canvas, 62, 40, "12345678");

        */
       canvas_set_font(canvas, FontPrimary);
        canvas_draw_str(canvas, 2, 11, "SSID");
        canvas_set_font(canvas, FontSecondary);
        // u8g2_SetFont(&canvas->fb, u8g2_font_5x7_tf); //u8g2_font_micro_tr);
        // canvas_set_font(canvas, FontBatteryPercent);
        canvas_draw_str(canvas, 2, 20, "[MALVEKE] Flipper GB Cam");
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str(canvas, 2, 30, "Password");
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str(canvas, 2, 40, "12345678");

        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str(canvas, 2, 50, "Host");
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str(canvas, 2, 60, model->ip);


    } else {
        canvas_draw_icon(canvas, 60, 7, &I_malveke_67x49);
        canvas_set_font(canvas, FontSecondary);

        canvas_draw_str(canvas, 4, 25, "Connect");
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str(canvas, 4, 35, "MALVEKE");
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str(canvas, 4, 44, "into Flipper");
        elements_button_center(canvas, "Ok"); 
    }
    

    // if (!model->initialized){
    //     canvas_clear(canvas);
    //     canvas_draw_icon(canvas, 74, 16, &I_DolphinCommon_56x48);
    //     canvas_set_font(canvas, FontSecondary);
    //     canvas_draw_str(canvas, 8, 12, "Waiting MALVEKE Board...");
    //     // canvas_draw_str(canvas, 20, 24, "VCC - 3V3/5V");
    //     // canvas_draw_str(canvas, 20, 34, "GND - GND");
    //     // canvas_draw_str(canvas, 20, 44, "U0R - TX");
    //     // canvas_draw_str(canvas, 20, 54, "U0T - RX");
    // }
}
static bool view_input_callback(InputEvent* event, void* context) {
    LinkCameraApp* instance = context;
    UNUSED(instance);
    if (event->type == InputTypePress){
        if (event->key == InputKeyBack){
            const char gbcameraserver_command[] = "gbcameraserver -s\n";
            uart_tx((uint8_t*)gbcameraserver_command, strlen(gbcameraserver_command));

            furi_hal_power_disable_external_3_3v();
            furi_delay_ms(100);
            furi_hal_power_enable_external_3_3v();
            furi_delay_ms(200);
        } else  if (event->key == InputKeyOk){
            const char gbcameraserver_command[] = "gbcameraserver -i\n";
            uart_tx((uint8_t*)gbcameraserver_command, strlen(gbcameraserver_command));
             with_view_model(
                instance->view,
                LinkCameraModel * model,
                {
                    model->initialized = true;
                },
                true);
        }
    }
    return false;
}
static void link_camera_free(LinkCameraApp* app) {
    furi_assert(app);
    // Free views
    view_dispatcher_remove_view(app->view_dispatcher, 0);
    view_free(app->view);
    view_dispatcher_free(app->view_dispatcher);
    // Close gui record
    furi_record_close(RECORD_GUI);
    furi_record_close(RECORD_STORAGE);
    app->gui = NULL;
    // Free rest
    free(app);
}
void handle_rx_data_cb(uint8_t* buf, size_t len, void* context) {
    furi_assert(context);
    UNUSED(len);
    // UNUSED(buf);
    LinkCameraApp* instance = (LinkCameraApp*)context;
    
    // FURI_LOG_I("UART", "[in]: %s", (char*)buf);
    with_view_model(
        instance->view,
        LinkCameraModel * model,
        {
            cJSON* json = cJSON_Parse((char*)buf);
            if(json == NULL) {
                
            } else {
                cJSON* type = cJSON_GetObjectItemCaseSensitive(json, "type");
                if(cJSON_IsString(type) && (type->valuestring != NULL)) {
                    model->event_type = strdup(type->valuestring);
                } else {
                    model->event_type = "None";
                }

                if (strcmp(model->event_type, "connected") == 0) {
                    cJSON* ip = cJSON_GetObjectItemCaseSensitive(json, "ip");
                    if(cJSON_IsString(ip) && (ip->valuestring != NULL)) {
                        model->ip = strdup(ip->valuestring);
                    } else {
                        model->ip = "0.0.0.0";
                    }
                    cJSON* ssid = cJSON_GetObjectItemCaseSensitive(json, "ssid");
                    if(cJSON_IsString(ssid) && (ssid->valuestring != NULL)) {
                        model->ssid = strdup(ssid->valuestring);
                    } else {
                        model->ssid = " - ";
                    }
                    cJSON* password = cJSON_GetObjectItemCaseSensitive(json, "password");
                    if(cJSON_IsString(password) && (password->valuestring != NULL)) {
                        model->password = strdup(password->valuestring);
                    } else {
                        model->password = "******";
                    }
                    model->connected = true;
                }
            }
            
        },
        true);
}
static LinkCameraApp* link_camera_alloc() {
    LinkCameraApp* app = malloc(sizeof(LinkCameraApp));
    // Gui
    app->gui = furi_record_open(RECORD_GUI);
    app->storage = furi_record_open(RECORD_STORAGE);

    app->uart = usart_init(app);
    uart_set_handle_rx_data_cb(app->uart, handle_rx_data_cb);

    // View dispatcher
    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    // Views
    app->view = view_alloc();
    view_set_context(app->view, app);
    view_set_draw_callback(app->view, view_draw_callback);
    view_set_input_callback(app->view, view_input_callback);
    view_allocate_model(app->view, ViewModelTypeLocking, sizeof(LinkCameraModel));

    

    view_set_previous_callback(app->view, link_camera_exit);
    view_dispatcher_add_view(app->view_dispatcher, 0, app->view);
    view_dispatcher_switch_to_view(app->view_dispatcher, 0);

   

    return app;
}

int32_t link_camera_app(void* p) {
    UNUSED(p);

    LinkCameraApp* app = link_camera_alloc();
    view_dispatcher_run(app->view_dispatcher);
    link_camera_free(app);
    
    // furi_hal_power_disable_otg();

    return 0;
}
