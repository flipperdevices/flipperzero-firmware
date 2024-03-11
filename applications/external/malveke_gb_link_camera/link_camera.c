#include "link_camera.h"
#include <gui/elements.h>
#include <malveke_gb_link_camera_icons.h>

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
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str(canvas, 2, 11, "SSID");
        canvas_set_font(canvas, FontSecondary);
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
}
static bool view_input_callback(InputEvent* event, void* context) {
    LinkCameraApp* instance = context;
    UNUSED(instance);
    if(event->type == InputTypePress) {
        if(event->key == InputKeyBack) {
            uart_set_handle_rx_data_cb(instance->uart, NULL);
            uart_free(instance->uart);

            furi_hal_power_disable_external_3_3v();
            furi_delay_ms(100);
            furi_hal_power_enable_external_3_3v();
            furi_delay_ms(200);
        } else if(event->key == InputKeyOk) {
            furi_hal_power_disable_external_3_3v();
            furi_delay_ms(100);
            furi_hal_power_enable_external_3_3v();
            furi_delay_ms(200);

            with_view_model(
                instance->view, LinkCameraModel * model, { model->initialized = true; }, true);
            uart_tx(instance->uart, (uint8_t*)("c"), 1);
            uart_tx(instance->uart, (uint8_t*)("\n"), 1);
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
    LinkCameraApp* instance = (LinkCameraApp*)context;

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

                if(strcmp(model->event_type, "connected") == 0) {
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

    return 0;
}
