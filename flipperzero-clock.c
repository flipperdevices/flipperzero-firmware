#include <furi.h>
#include <furi_hal.h>

#include <gui/gui.h>
#include <input/input.h>
#include <storage/storage.h>
#include <notification/notification.h>
#include <notification/notification_messages.h>

#include "clock.h"

#define CFG_FILENAME APP_DATA_PATH("clock.cfg")

#define FRAME_MS 30

typedef struct {
    FuriMutex* mutex;
    ClockConfig cfg;
    uint8_t second;
    uint16_t ms_adjust;
} AppData;

static void app_draw_callback(Canvas* canvas, void* ctx) {
    furi_assert(ctx);
    AppData* app = (AppData*)ctx;
    if(furi_mutex_acquire(app->mutex, FRAME_MS) != FuriStatusOk) return;

    DateTime dt;
    uint32_t tick = furi_get_tick();
    furi_hal_rtc_get_datetime(&dt);
    if(dt.second != app->second) app->ms_adjust = tick % 1000;
    app->second = dt.second;
    draw_clock(canvas, &app->cfg, &dt, (tick - app->ms_adjust) % 1000);
    furi_mutex_release(app->mutex);
}

static void app_input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);
    FuriMessageQueue* event_queue = ctx;
    furi_message_queue_put(event_queue, input_event, FuriWaitForever);
}

static bool cfg_load(File* file, AppData* app) {
    size_t readed = 0;
    if(storage_file_open(file, CFG_FILENAME, FSAM_READ, FSOM_OPEN_EXISTING))
        readed = storage_file_read(file, &app->cfg, sizeof(ClockConfig));
    storage_file_close(file);
    return readed == sizeof(ClockConfig) && app->cfg.version == CONFIG_VERSION;
}

static void cfg_save(File* file, AppData* app) {
    if(furi_mutex_acquire(app->mutex, FuriWaitForever) != FuriStatusOk) return;
    calc_clock_face(&app->cfg);
    if(storage_file_open(file, CFG_FILENAME, FSAM_WRITE, FSOM_CREATE_ALWAYS))
        storage_file_write(file, &app->cfg, sizeof(ClockConfig));
    storage_file_close(file);
    furi_mutex_release(app->mutex);
}

int32_t clock_main(void* p) {
    UNUSED(p);

    AppData* app = malloc(sizeof(AppData));
    furi_assert(app);

    app->mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    app->second = 0;
    app->ms_adjust = 0;

    Storage* storage = furi_record_open(RECORD_STORAGE);
    File* file = storage_file_alloc(storage);

    if(!cfg_load(file, app)) {
        init_clock_config(&app->cfg);
        cfg_save(file, app);
    }

    ViewPort* view_port = view_port_alloc();
    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));

    view_port_draw_callback_set(view_port, app_draw_callback, app);
    view_port_input_callback_set(view_port, app_input_callback, event_queue);

    Gui* gui = (Gui*)furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    NotificationApp* notification = furi_record_open(RECORD_NOTIFICATION);
    notification_message_block(notification, &sequence_display_backlight_enforce_on);

    InputEvent event;
    bool terminate = false;
    while(!terminate) {
        if(furi_message_queue_get(event_queue, &event, FRAME_MS) == FuriStatusOk) {
            if((event.type == InputTypePress) || (event.type == InputTypeRepeat)) {
                switch(event.key) {
                case InputKeyUp:
                    modify_clock_up(&app->cfg);
                    break;
                case InputKeyDown:
                    modify_clock_down(&app->cfg);
                    break;
                case InputKeyLeft:
                    modify_clock_left(&app->cfg);
                    break;
                case InputKeyRight:
                    modify_clock_right(&app->cfg);
                    break;
                case InputKeyOk:
                    modify_clock_ok(&app->cfg);
                    break;
                case InputKeyBack:
                    terminate = true;
                    break;
                default:
                    break;
                }
                if(!terminate) cfg_save(file, app);
            }
        }
        view_port_update(view_port);
    }

    notification_message_block(notification, &sequence_display_backlight_enforce_auto);
    view_port_enabled_set(view_port, false);
    gui_remove_view_port(gui, view_port);
    furi_record_close(RECORD_NOTIFICATION);
    furi_record_close(RECORD_GUI);
    furi_record_close(RECORD_STORAGE);
    furi_message_queue_free(event_queue);
    view_port_free(view_port);
    storage_file_free(file);

    furi_mutex_free(app->mutex);
    free(app);

    return 0;
}
