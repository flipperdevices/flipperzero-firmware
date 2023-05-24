#include "rad_sens.h"
#include "notifications.h"
#include "rad_sens_i2c.h"

typedef enum {
    WorkerEventReserved = (1 << 0), // Reserved for StreamBuffer internal event
    WorkerEventStop = (1 << 1),
    WorkerEventTick = (1 << 2),
} WorkerEventFlags;

#define WORKER_EVENTS_MASK (WorkerEventStop | WorkerEventTick)

static void rad_sens_view_draw_battery(Canvas* canvas, RadSensModel* model) {
    if(model->info.gauge_is_ok) {
        canvas_draw_icon(canvas, 100, 2, &I_Battery_26x8);
        canvas_draw_box(canvas, 102, 4, model->info.charge / 5, 4);
    }

    if(model->vibro_on) {
        canvas_draw_icon(canvas, 90, 2, &I_vibro_on);
    }
}

static void rad_sens_view_draw_info(Canvas* canvas, RadSensModel* model) {
    FuriString* dyn_intensity =
        furi_string_alloc_printf("%0.1f", ((double)model->dyn_intensity / 10));
    FuriString* stat_intensity = furi_string_alloc_printf(
        "Static intensity: %0.1f uR/h", ((double)model->stat_intensity / 10));
    FuriString* impulse_count = furi_string_alloc_printf("Impulses: %d", model->impulse_count);

    canvas_set_font(canvas, FontSecondary);
    uint8_t height = canvas_current_font_height(canvas);
    canvas_draw_str(canvas, 0, height, "RadSens connected");

    canvas_set_font(canvas, FontBigNumbers);
    uint8_t width_dyn = canvas_string_width(canvas, furi_string_get_cstr(dyn_intensity));
    uint8_t height_dyn = canvas_current_font_height(canvas);

    canvas_draw_str(
        canvas,
        (128 - width_dyn) / 2,
        (64 - height + height_dyn) / 2,
        furi_string_get_cstr(dyn_intensity));

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, (128 + width_dyn) / 2 + 4, (64 - height + height_dyn) / 2, "uR/h");

    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 0, 64 - 1 * height, furi_string_get_cstr(stat_intensity));
    canvas_draw_str(canvas, 0, 64 - 0 * height, furi_string_get_cstr(impulse_count));

    furi_string_free(dyn_intensity);
    furi_string_free(stat_intensity);
    furi_string_free(impulse_count);
}

static void rad_sens_view_dotted_line_v(Canvas* canvas, uint8_t x, uint8_t y1, uint8_t y2) {
    for(uint8_t y = MIN(y1, y2); y < MAX(y1, y2); y += 2) {
        canvas_draw_dot(canvas, x, y);
    }
}

static void rad_sens_view_dotted_line_h(Canvas* canvas, uint8_t x1, uint8_t x2, uint8_t y) {
    for(uint8_t x = MIN(x1, x2); x < MAX(x1, x2); x += 2) {
        canvas_draw_dot(canvas, x, y);
    }
}

static void rad_sens_view_draw_history(Canvas* canvas, RadSensModel* model) {
    // Get Max Intensity
    uint32_t max = 0;
    for(uint8_t i = 0; i < HISTORY_LENGTH; i++) {
        max = MAX(max, model->dyn_intensity_history[i]);
    }

    // Calculate Y Axis Unit
    uint8_t unit = 5;
    for(uint8_t i = 0; i < 4; i++) {
        if(2 * unit > max / 10) {
            break;
        }
        unit *= 2;
    }

    canvas_set_font(canvas, FontSecondary);
    // X Axis
    canvas_draw_line(canvas, 4, 63, 120, 63);
    canvas_draw_str(canvas, 124, 63, "s");

    // Y Axis
    canvas_draw_line(canvas, 4, 63, 4, 8);
    canvas_draw_str(canvas, 0, 7, "uR/h");

    // Time Marks
    rad_sens_view_dotted_line_v(canvas, 94, 8, 63);
    canvas_draw_str(canvas, 96, 62, "30");
    rad_sens_view_dotted_line_v(canvas, 64, 8, 63);
    canvas_draw_str(canvas, 66, 62, "60");
    rad_sens_view_dotted_line_v(canvas, 34, 8, 63);
    canvas_draw_str(canvas, 36, 62, "90");

    // Intensity Marks
    FuriString* unit1_mark = furi_string_alloc_printf("%d", unit);
    FuriString* unit2_mark = furi_string_alloc_printf("%d", unit * 2);

    rad_sens_view_dotted_line_h(canvas, 4, 124, 24);
    canvas_draw_str(canvas, 6, 23, furi_string_get_cstr(unit2_mark));
    rad_sens_view_dotted_line_h(canvas, 4, 124, 44);
    canvas_draw_str(canvas, 6, 43, furi_string_get_cstr(unit1_mark));

    furi_string_free(unit1_mark);
    furi_string_free(unit2_mark);

    // Plot
    for(uint8_t x = 0; x < HISTORY_LENGTH; x++) {
        uint32_t index = (model->dyn_intensity_history_index + x) % HISTORY_LENGTH;
        // magic 2 is 20 pixels per 1 uR/h which is 10 units from RadSens
        uint8_t y = 64 - (model->dyn_intensity_history[index] * 2 / unit);
        canvas_draw_dot(canvas, x + 4, y);
    }

    FuriString* dyn_intensity =
        furi_string_alloc_printf("%0.1f", ((double)model->dyn_intensity / 10));

    uint8_t width_dyn = canvas_string_width(canvas, furi_string_get_cstr(dyn_intensity));
    uint8_t y = 64 - model->dyn_intensity * 2 / unit - 2;

    canvas_draw_str(canvas, 124 - width_dyn, y, furi_string_get_cstr(dyn_intensity));
    furi_string_free(dyn_intensity);
}

static void rad_sens_view_draw_error(Canvas* canvas, RadSensModel* model) {
    canvas_set_font(canvas, FontSecondary);
    uint8_t height = canvas_current_font_height(canvas);
    canvas_draw_str(
        canvas,
        0,
        (64 + height) / 2,
        model->connected ? "RadSens not detected" : "RadSens not connected");
}

static void rad_sens_view_draw_callback(Canvas* canvas, void* _model) {
    RadSensModel* model = _model;
    furi_assert(model);

    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);

    if(model->verified) {
        if(model->show_history) {
            rad_sens_view_draw_history(canvas, model);
        } else {
            rad_sens_view_draw_info(canvas, model);
        }
    } else {
        rad_sens_view_draw_error(canvas, model);
    }

    rad_sens_view_draw_battery(canvas, model);
}

static bool rad_sens_view_input_callback(InputEvent* event, void* context) {
    RadSensApp* app = context;
    furi_assert(app);
    bool consumed = false;

    if(event->type == InputTypeShort) {
        switch(event->key) {
        case InputKeyUp:
            consumed = true;
            app->model->vibro_on = !app->model->vibro_on;
            break;
        case InputKeyRight:
            app->model->show_history = true;
            break;
        case InputKeyLeft:
            app->model->show_history = false;
            break;
        default:
            break;
        }
    }

    return consumed;
}

static uint32_t rad_sens_exit(void* context) {
    UNUSED(context);
    return VIEW_NONE;
}

static void rad_sens_get_battery_info(RadSensApp* app, RadSensModel* model) {
    power_get_info(app->power, &model->info);
}

static int32_t rad_sens_worker(void* context) {
    furi_assert(context);
    RadSensApp* app = context;

    while(1) {
        uint32_t events =
            furi_thread_flags_wait(WORKER_EVENTS_MASK, FuriFlagWaitAny, FuriWaitForever);
        furi_check((events & FuriFlagError) == 0);

        if(events & WorkerEventStop) break;
        if(events & WorkerEventTick) {
            bool done = false;
            uint16_t new_impulse_count = 0;
            uint8_t vibro = 0;
            with_view_model(
                app->view,
                RadSensModel * model,
                {
                    done = rad_sens_read_data(model);
                    new_impulse_count = model->new_impulse_count > 2 ? 3 :
                                                                       model->new_impulse_count;
                    vibro = model->vibro_on ? 1 : 0;
                    if(done) {
                        model->dyn_intensity_history[model->dyn_intensity_history_index] =
                            model->dyn_intensity;
                        model->dyn_intensity_history_index++;
                        model->dyn_intensity_history_index %= HISTORY_LENGTH;
                    }
                    rad_sens_get_battery_info(app, model);
                },
                true);

            if(!done) {
                notification_message(app->notification, &sequence_notification_fail);
            } else {
                notification_message(
                    app->notification, sequence_notification_imps[new_impulse_count][vibro]);
            }
        }
    }

    return 0;
}

static void rad_sens_timer_callback(void* context) {
    furi_assert(context);
    RadSensApp* app = context;
    furi_thread_flags_set(furi_thread_get_id(app->worker_thread), WorkerEventTick);
}

static uint32_t rad_sens_load_value() {
    uint32_t value = 0;
    bool result;
    Storage* api = furi_record_open(RECORD_STORAGE);
    File* file = storage_file_alloc(api);
    const char* path = ANY_PATH("rad_sens.txt");
    result = storage_file_open(file, path, FSAM_READ, FSOM_OPEN_EXISTING);
    if(result) {
        uint64_t size = storage_file_size(file);
        char* str = malloc(sizeof(char) * (size + 1));
        memset(str, 0, size + 1);

        uint8_t bytes_count = storage_file_read(file, str, size);
        if(bytes_count > 0) {
            value = strtol(str, NULL, 10);
        }
        free(str);

        storage_file_close(file);
    }

    storage_file_free(file);
    furi_record_close(RECORD_STORAGE);

    return value;
}

static void rad_sens_save_value(uint32_t value) {
    bool result;
    Storage* api = furi_record_open(RECORD_STORAGE);
    File* file = storage_file_alloc(api);
    const char* path = ANY_PATH("rad_sens.txt");
    result = storage_file_open(file, path, FSAM_WRITE, FSOM_CREATE_ALWAYS);
    if(result) {
        FuriString* str = furi_string_alloc_printf("%ld", value);
        storage_file_write(file, furi_string_get_cstr(str), furi_string_size(str));
        furi_string_free(str);

        storage_file_close(file);
    }

    storage_file_free(file);
    furi_record_close(RECORD_STORAGE);
}

static RadSensApp* rad_sens_app_alloc() {
    RadSensApp* app = malloc(sizeof(RadSensApp));

    // Gui
    app->gui = furi_record_open(RECORD_GUI);
    app->notification = furi_record_open(RECORD_NOTIFICATION);
    app->power = furi_record_open(RECORD_POWER);

    // View dispatcher
    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->view_dispatcher);

    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    // Views
    app->view = view_alloc();
    view_set_context(app->view, app);
    view_set_draw_callback(app->view, rad_sens_view_draw_callback);
    view_set_input_callback(app->view, rad_sens_view_input_callback);
    view_allocate_model(app->view, ViewModelTypeLocking, sizeof(RadSensModel));
    with_view_model(
        app->view,
        RadSensModel * model,
        {
            app->model = model;
            model->vibro_on = false;
            model->connected = false;
            model->dyn_intensity = 0;
            model->stat_intensity = 0;
            model->new_impulse_count = 0;
            model->impulse_count = rad_sens_load_value();

            model->show_history = false;
            for(uint8_t i = 0; i < HISTORY_LENGTH; i++) {
                model->dyn_intensity_history[i] = 0;
            }
            model->dyn_intensity_history_index = 0;

            rad_sens_read_data(model);
            rad_sens_get_battery_info(app, model);
        },
        true);

    view_set_previous_callback(app->view, rad_sens_exit);
    view_dispatcher_add_view(app->view_dispatcher, 0, app->view);
    view_dispatcher_switch_to_view(app->view_dispatcher, 0);

    app->worker_thread = furi_thread_alloc_ex("RadSensWorker", 1024, rad_sens_worker, app);
    furi_thread_start(app->worker_thread);

    app->timer = furi_timer_alloc(rad_sens_timer_callback, FuriTimerTypePeriodic, app);
    furi_timer_start(app->timer, furi_ms_to_ticks(1000));

    return app;
}

static void rad_sens_app_free(RadSensApp* app) {
    furi_assert(app);

    furi_timer_stop(app->timer);
    furi_timer_free(app->timer);

    furi_thread_flags_set(furi_thread_get_id(app->worker_thread), WorkerEventStop);
    furi_thread_join(app->worker_thread);
    furi_thread_free(app->worker_thread);

    // Free views
    view_dispatcher_remove_view(app->view_dispatcher, 0);

    with_view_model(
        app->view,
        RadSensModel * model,
        {
            rad_sens_save_value(model->impulse_count);

            model->connected = false;
            model->dyn_intensity = 0;
            model->stat_intensity = 0;
            model->impulse_count = 0;
        },
        true);
    view_free(app->view);
    view_dispatcher_free(app->view_dispatcher);

    // Close gui record
    furi_record_close(RECORD_GUI);
    app->gui = NULL;
    furi_record_close(RECORD_NOTIFICATION);
    app->notification = NULL;
    furi_record_close(RECORD_POWER);
    app->power = NULL;

    // Free rest
    free(app);
}

int32_t rad_sens_app(void* p) {
    UNUSED(p);
    RadSensApp* app = rad_sens_app_alloc();
    view_dispatcher_run(app->view_dispatcher);
    rad_sens_app_free(app);
    return 0;
}
