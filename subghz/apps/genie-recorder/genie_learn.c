#include "genie_learn.h"
#include "genie_app.h"
#include "genie_submenu.h"
#include "genie_subghz_receive.h"
#include "genie_file.h"

#ifdef TAG
#undef TAG
#endif
#define TAG "GenieLearn"

typedef struct GenieApp GenieApp;

typedef struct {
    GenieApp* ref;
} GenieAppRef;

#define CLICK_SPEED 2000

static void genie_learn_draw_callback(Canvas* canvas, void* model) {
    GenieApp* app = ((GenieAppRef*)model)->ref;
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 5, 10, "Genie Sub-Ghz Recorder!!!");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 5, 20, "A7/GND to Genie remote");
    char buffer[30] = {0};
    snprintf(buffer, COUNT_OF(buffer), "Click %ld", genie_app_get_click_counter(app));
    canvas_draw_str(canvas, 1, 40, buffer);
    uint32_t count = genie_app_get_save_counter(app);
    if(count == 0) {
        snprintf(buffer, COUNT_OF(buffer), "Prev %ld", genie_app_get_rx_counter(app));
    } else {
        snprintf(buffer, COUNT_OF(buffer), "Got %ld", count);
    }
    canvas_draw_str(canvas, 75, 40, buffer);

    if(count < 0x10000) {
        uint32_t remaining;
        if(count == 0) {
            remaining = 65536 - genie_app_get_rx_counter(app);
        } else {
            remaining = 65536 - count;
        }
        snprintf(buffer, COUNT_OF(buffer), "Remaining codes %ld", remaining);
        canvas_draw_str(canvas, 1, 30, buffer);
    } else {
        canvas_draw_str(canvas, 1, 30, "Found all codes!");
    }

    canvas_draw_str(canvas, 5, 50, furi_string_get_cstr(genie_app_get_key(app)));

    if(genie_app_has_no_signal(app)) {
        canvas_draw_str(canvas, 1, 60, "NO SIGNAL FROM REMOTE?");
    } else if(genie_app_is_sending_signal(app)) {
        canvas_draw_str(canvas, 100, 60, "SEND");
    }
}

static bool genie_learn_input_callback(InputEvent* event, void* context) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

static uint32_t save_count(uint32_t count, FuriString* key, bool is_genie) {
    FURI_LOG_D(TAG, "%ld,%s", count, furi_string_get_cstr(key));
    genie_save(count, key);
    if(is_genie) {
        return genie_save_bin(furi_string_get_cstr(key));
    }

    return 0;
}

static void genie_packet(FuriString* buffer, void* context) {
    GenieApp* app = (GenieApp*)context;
    genie_app_set_processing_packet(app, true);
    if(furi_string_search_str(buffer, "Genie 64bit") < furi_string_size(buffer)) {
        release_button(app);
        FURI_LOG_D(TAG, "Genie 64bit packet");
        size_t key_index = furi_string_search_str(buffer, "Key:");
        if(key_index < furi_string_size(buffer)) {
            genie_app_received_key(app, buffer);
            uint32_t click_counter = genie_app_get_click_counter(app);
            FuriString* key = genie_app_get_key(app);
            uint32_t num_saved = save_count(click_counter, key, true);
            genie_app_update_save_counter(app, num_saved);
        }
    }

    genie_app_set_processing_packet(app, false);
}

static void genie_learn_enter_callback(void* context) {
    GenieApp* app = (GenieApp*)context;
    genie_file_init();
    uint32_t frequency = genie_app_get_frequency(app);
    GenieSubGhz* subghz = genie_app_get_subghz(app);
    start_listening(subghz, frequency, genie_packet, context);
    genie_app_start_timer(app, CLICK_SPEED);
}

static void genie_learn_exit_callback(void* context) {
    GenieApp* app = (GenieApp*)context;
    genie_app_set_processing_packet(app, false);
    release_button(app);
    GenieSubGhz* subghz = genie_app_get_subghz(app);
    stop_listening(subghz);
    genie_app_stop_timer(app);
}

View* genie_learn_alloc(void* app) {
    View* view = view_alloc();
    view_set_draw_callback(view, genie_learn_draw_callback);
    view_set_input_callback(view, genie_learn_input_callback);
    view_set_previous_callback(view, genie_navigation_submenu_callback);
    view_set_context(view, app);
    view_set_enter_callback(view, genie_learn_enter_callback);
    view_set_exit_callback(view, genie_learn_exit_callback);
    view_allocate_model(view, ViewModelTypeLockFree, sizeof(GenieAppRef));
    GenieAppRef* r = (GenieAppRef*)view_get_model(view);
    r->ref = app;

    return view;
}

void genie_learn_free(View* view) {
    view_free(view);
}