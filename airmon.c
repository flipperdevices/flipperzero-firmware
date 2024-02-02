#include <gui/gui.h>
#include <gui/view_port.h>
#include <power/power_service/power.h>
#include <notification/notification_messages.h>

#include <core/thread.h>
#include <core/kernel.h>

#include "airmon_icons.h"
#include "airmon_pms.h"
#include "airmon_aqi.h"

#define TAG "Airmon"

#define STR_BUF_SIZE 32

typedef enum {
    AirmonDisplayModePm1_0 = 0,
    AirmonDisplayModePm2_5,
    AirmonDisplayModePm10,
    AirmonDisplayModeAqi,

    AirmonDisplayModeCount
} AirmonDisplayMode;

typedef struct {
    Gui* gui;
    ViewPort* view_port;
    Power* power;
    PowerInfo power_info;
    FuriMessageQueue* event_queue;

    NotificationApp* notifications;

    AirmonDisplayMode display_mode;

    uint32_t last_data_timestamp;
    AirmonPmsContext* pms_context;
} AirmonContext;

typedef enum {
    AirmonEventTypeKey,
} AirmonEventType;

typedef struct {
    AirmonEventType type;
    InputEvent input;
} AirmonEvent;

static const NotificationMessage message_green_165 = {
    .type = NotificationMessageTypeLedGreen,
    .data.led.value = 165,
};

static const NotificationMessage message_red_128 = {
    .type = NotificationMessageTypeLedRed,
    .data.led.value = 128,
};

static const NotificationMessage message_blue_128 = {
    .type = NotificationMessageTypeLedBlue,
    .data.led.value = 128,
};

const NotificationSequence sequence_blink_orange_100 = {
    &message_red_255,
    &message_green_165,
    &message_delay_100,
    NULL,
};

const NotificationSequence sequence_blink_purple_100 = {
    &message_red_128,
    &message_blue_128,
    &message_delay_100,
    NULL,
};

const NotificationSequence sequence_blink_maroon_100 = {
    &message_red_128,
    &message_delay_100,
    NULL,
};

static void airmon_blink(AirmonContext* ctx, int aqi_value) {
    int aqi_level = airmon_aqi_level(aqi_value);
    const NotificationSequence *sequence;
    switch(aqi_level) {
    case 0:
        // Good - Green
        sequence = &sequence_blink_green_100;
        break;
    case 1:
        // Moderate - Yellow
        sequence = &sequence_blink_yellow_100;
        break;
    case 2:
        // Unhealthy for Sensitive Groups - Orange
        sequence = &sequence_blink_orange_100;
        break;
    case 3:
        // Unhealthy - Red
        sequence = &sequence_blink_red_100;
        break;
    case 4:
        // Very Unhealthy - Purple
        sequence = &sequence_blink_purple_100;
        break;
    default:
        // Hazardous - Maroon
        sequence = &sequence_blink_maroon_100;
        break;
    }
    notification_message_block(ctx->notifications, sequence);
}

static void airmon_draw_battery(Canvas* canvas, AirmonContext* ctx) {
    // Sourced from https://github.com/flipperdevices/flipperzero-firmware/blob/09edf66a/applications/services/power/power_service/power.c#L9
    if(!ctx->power_info.gauge_is_ok) {
        return;
    }
    canvas_draw_icon(canvas, 100, 0, &I_Battery_26x8);
    canvas_draw_box(canvas, 102, 2, (ctx->power_info.charge + 4) / 5, 4);
}

static void airmon_draw_callback(Canvas* canvas, void* ctx) {
    static char buffer[STR_BUF_SIZE];

    AirmonContext* context = ctx;
    AirmonPmsContext* pms_context = context->pms_context;

    const uint8_t canvas_h = canvas_height(canvas);
    const uint8_t canvas_w = canvas_width(canvas);

    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);

    uint16_t pm_value;
    uint16_t aqi_value;
    uint32_t data_timestamp;

    // Obtain PM/AQI values
    furi_mutex_acquire(pms_context->mutex, FuriWaitForever);
    switch(context->display_mode) {
    case AirmonDisplayModePm1_0:
        pm_value = pms_context->pms_data.pm1_0at;
        break;
    case AirmonDisplayModePm2_5:
        pm_value = pms_context->pms_data.pm2_5at;
        break;
    case AirmonDisplayModePm10:
        pm_value = pms_context->pms_data.pm10at;
        break;
    default:
        pm_value = 0;
        break;
    }
    aqi_value = airmon_aqi(pms_context->pms_data.pm2_5at, pms_context->pms_data.pm10at);
    data_timestamp = pms_context->pms_data_timestamp;
    furi_mutex_release(pms_context->mutex);

    bool data_is_valid = (furi_get_tick() - data_timestamp < furi_ms_to_ticks(5000));

    // Draw PM/AQI value
    canvas_set_font(canvas, FontBigNumbers);
    if(data_is_valid) {
        snprintf(buffer, STR_BUF_SIZE, "%d", 
            (context->display_mode == AirmonDisplayModeAqi) ? aqi_value : pm_value);
    } else {
        snprintf(buffer, STR_BUF_SIZE, "--");
    }
    canvas_draw_str_aligned(canvas, canvas_w / 2, canvas_h / 2, AlignCenter, AlignBottom, buffer);

    // Draw PM/AQI value legend (density units of measurement / air quality level description)
    const uint8_t legend_y = canvas_h / 2;
    if(context->display_mode == AirmonDisplayModeAqi) {
        canvas_set_font(canvas, FontSecondary);
        snprintf(buffer, STR_BUF_SIZE, "%s", data_is_valid ? airmon_aqi_category(aqi_value) : "Unknown");
        canvas_draw_str_aligned(canvas, canvas_w / 2, legend_y + 5, AlignCenter, AlignTop, buffer);
    } else {
        const Icon* density_icon = &I_Density_33x11;
        const uint8_t density_icon_w = icon_get_width(density_icon);
        canvas_draw_icon(canvas, (canvas_w - density_icon_w) / 2, legend_y + 3, density_icon);
    }

    // Draw mode bottom menu
    canvas_set_font(canvas, FontSecondary);
    const uint8_t label_w = canvas_w / AirmonDisplayModeCount;
    const uint8_t label_h = canvas_current_font_height(canvas);
    const uint8_t label_y = canvas_h - label_h - 1;
    for(AirmonDisplayMode mode = 0; mode < AirmonDisplayModeCount; mode++) {
        uint8_t label_x = mode * label_w;
        bool mode_active = mode == context->display_mode;
        if(mode_active) {
            canvas_draw_box(canvas, label_x, label_y, label_w, label_h);
            canvas_invert_color(canvas);
        }
        const char* label_str;
        switch(mode) {
        case AirmonDisplayModePm1_0:
            label_str = "PM1.0";
            break;
        case AirmonDisplayModePm2_5:
            label_str = "PM2.5";
            break;
        case AirmonDisplayModePm10:
            label_str = "PM10";
            break;
        case AirmonDisplayModeAqi:
            label_str = "AQI";
            break;
        default:
            label_str = "";
            break;
        }
        const uint8_t label_str_w = canvas_string_width(canvas, label_str);
        canvas_draw_str(
            canvas, label_x + (label_w - label_str_w) / 2, label_y + label_h - 2, label_str);
        if(mode_active) {
            canvas_invert_color(canvas);
        }
    }

    airmon_draw_battery(canvas, context);
    
    // Blink LED if data was updated
    if(data_timestamp > context->last_data_timestamp) {
        airmon_blink(context, aqi_value);
        context->last_data_timestamp = data_timestamp;
    }
}

static void airmon_input_callback(InputEvent* input_event, void* ctx) {
    AirmonContext* context = ctx;
    AirmonEvent event = {.type = AirmonEventTypeKey, .input = *input_event};
    furi_message_queue_put(context->event_queue, &event, FuriWaitForever);
}

static AirmonContext* airmon_context_alloc() {
    AirmonContext* ctx = malloc(sizeof(AirmonContext));

    ctx->pms_context = airmon_pms_context_alloc();
    if(!ctx->pms_context) {
        free(ctx);
        return NULL;
    }

    ctx->last_data_timestamp = furi_get_tick();

    ctx->display_mode = AirmonDisplayModePm2_5;

    ctx->event_queue = furi_message_queue_alloc(8, sizeof(AirmonEvent));

    ctx->view_port = view_port_alloc();
    view_port_draw_callback_set(ctx->view_port, airmon_draw_callback, ctx);
    view_port_input_callback_set(ctx->view_port, airmon_input_callback, ctx);

    ctx->gui = furi_record_open(RECORD_GUI);
    ctx->power = furi_record_open(RECORD_POWER);
    ctx->notifications = furi_record_open(RECORD_NOTIFICATION);

    gui_add_view_port(ctx->gui, ctx->view_port, GuiLayerFullscreen);

    return ctx;
}

static void airmon_run(AirmonContext* ctx) {
    airmon_pms_init(ctx->pms_context);

    AirmonEvent event;
    bool processing = true;
    while(processing) {
        FuriStatus event_status = furi_message_queue_get(ctx->event_queue, &event, 1000);

        if(event_status == FuriStatusOk) {
            // press events
            if(event.type == AirmonEventTypeKey) {
                if(event.input.type == InputTypePress) {
                    switch(event.input.key) {
                    case InputKeyBack:
                        processing = false;
                        break;
                    case InputKeyLeft:
                        if(ctx->display_mode-- == 0) {
                            ctx->display_mode = AirmonDisplayModeCount - 1;
                        }
                        break;
                    case InputKeyRight:
                        if(++ctx->display_mode >= AirmonDisplayModeCount) {
                            ctx->display_mode -= AirmonDisplayModeCount;
                        }
                        break;
                    default:
                        break;
                    }
                }
            }
        } else {
            FURI_LOG_D(TAG, "FuriMessageQueue: event timeout");
        }

        power_get_info(ctx->power, &ctx->power_info);

        view_port_update(ctx->view_port);
    }

    airmon_pms_deinit(ctx->pms_context);
}

static void airmon_context_free(AirmonContext* ctx) {
    view_port_enabled_set(ctx->view_port, false);
    gui_remove_view_port(ctx->gui, ctx->view_port);

    view_port_free(ctx->view_port);
    furi_message_queue_free(ctx->event_queue);

    furi_record_close(RECORD_GUI);
    furi_record_close(RECORD_POWER);
    furi_record_close(RECORD_NOTIFICATION);

    airmon_pms_context_free(ctx->pms_context);

    free(ctx);
}

/* The application's entry point. Execution starts from here. */
int32_t airmon_app(void* p) {
    UNUSED(p);

    // Allocate all of the necessary structures
    AirmonContext* ctx = airmon_context_alloc();
    if(!ctx) {
        return 255;
    }

    // Start the applicaton's main loop. It won't return until the application was requested to exit
    airmon_run(ctx);

    // Release all resources
    airmon_context_free(ctx);

    return 0;
}