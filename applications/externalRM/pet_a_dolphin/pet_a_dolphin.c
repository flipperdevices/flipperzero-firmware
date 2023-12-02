#include "es_app.h"

#include <core/check.h>
#include <core/record.h>
#include <furi_hal_version.h>

static int32_t decrease_butthurt(void* ctx) {
    esApp* app = (esApp*)ctx;

    app->data->pet_feeder.active = true;

    DolphinEvent event;
    while(app->data->stats.butthurt && app->data->pet_feeder.active) {
        app->data->stats = dolphin_stats(app->data->dolphin);
        event.type = DolphinEventTypeClearLimits;
        event.deed = DolphinDeedPluginGameWin;
        event.flag = NULL;
        furi_check(
            furi_message_queue_put(app->data->dolphin->event_queue, &event, FuriWaitForever) ==
            FuriStatusOk);
        dolphin_deed(DolphinDeedPluginGameWin);
    }

    app->data->pet_feeder.active = false;

    return 0;
}

static void render_callback(Canvas* canvas, void* ctx) {
    esApp* app = (esApp*)ctx;

    char thread_status_str[24];
    char butthurt_str[32];

    snprintf(
        thread_status_str,
        24,
        "Petting a dolphin: %s",
        (app->data->pet_feeder.active ? "Yes" : "No"));

    snprintf(butthurt_str, 32, "Dolphin's butthurt: %lu", app->data->stats.butthurt);

    canvas_draw_str(canvas, 5, 10, thread_status_str);
    canvas_draw_str(canvas, 5, 20, butthurt_str);

    if(app->data->stats.butthurt != 0) {
        char press_ok_str[] = "Press OK to pet a dolphin!";
        canvas_draw_str(canvas, 5, 30, press_ok_str);
    }
}

static void app_logic_cycle(esApp* app) {
    app->data->stats = dolphin_stats(app->data->dolphin);
}

int32_t pet_a_dolphin(void* p) {
    UNUSED(p);
    UNUSED(decrease_butthurt);

    esApp* app = es_app_alloc(&render_callback, &app_logic_cycle);

    furi_thread_set_callback(app->data->pet_feeder.thread, decrease_butthurt);
    furi_thread_set_context(app->data->pet_feeder.thread, app);

    do {
        FuriStatus event_status = furi_message_queue_get(app->queue, &app->event, 100);

        if(event_status == FuriStatusOk) {
            if(app->event.type == EventTypeKey) {
                if(app->event.input.type == InputTypePress) {
                    switch(app->event.input.key) {
                    case InputKeyUp:
                        FURI_LOG_I(TAG, "Up");
                        break;
                    case InputKeyDown:
                        FURI_LOG_I(TAG, "Down");
                        break;
                    case InputKeyRight:
                        FURI_LOG_I(TAG, "Right");
                        break;
                    case InputKeyLeft:
                        FURI_LOG_I(TAG, "Left");
                        break;
                    case InputKeyOk:
                        if(!app->data->pet_feeder.active) {
                            furi_thread_start(app->data->pet_feeder.thread);
                        }
                        break;
                    case InputKeyBack:
                        app->run = false;
                        break;
                    default:
                        break;
                    }
                }
            } else if(app->event.type == EventTypeTick) {
                app_logic_cycle(app);
            }
        } else {
            // event timeout
        }
        view_port_update(app->view_port);
    } while(app->run);

    es_app_free(app);

    return 0;
}
