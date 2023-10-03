#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <gui/view.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/submenu.h>
#include <gui/modules/widget.h>

#include "genie_subghz_receive.h"
#include "genie_file.h"

#define TAG "GenieRecord"

//#define FREQUENCY 390000000
#define FREQUENCY 315000000
#define CLICK_SPEED 2000
const GpioPin* const pin_remote = &gpio_ext_pa7;

typedef enum {
    GenieSubmenuIndexStart,
    GenieSubmenuIndexAbout,
} GenieSubmenuIndex;

typedef enum {
    GenieViewSubmenu,
    GenieViewStart,
    GenieViewAbout,
} GenieView;

typedef struct {
    ViewDispatcher* view_dispatcher;
    Submenu* submenu;
    View* view;
    Widget* widget_about;
    GenieSubGhz* genie_subghz;
    FuriTimer* timer;
    bool processing;
    bool pressed;
    uint32_t counter;
    uint32_t save_counter;
    FuriString* key;
} GenieApp;

typedef struct {
    GenieApp* ref;
} GenieAppRef;

/**
 * @brief      Callback for navigation events
 * @details    This function is called when user press back button.  We return VIEW_NONE to
 *            indicate that we want to exit the application.
 * @param      context  The context
 * @return     next view id
*/
uint32_t genie_navigation_exit_callback(void* context) {
    UNUSED(context);
    return VIEW_NONE;
}

/**
 * @brief      Callback for navigation events
 * @details    This function is called when user press back button.  We return VIEW_NONE to
 *            indicate that we want to exit the application.
 * @param      context  The context
 * @return     next view id
*/
uint32_t genie_navigation_submenu_callback(void* context) {
    UNUSED(context);
    return GenieViewSubmenu;
}

void genie_submenu_callback(void* context, uint32_t index) {
    GenieApp* app = (GenieApp*)context;
    switch(index) {
    case GenieSubmenuIndexStart:
        view_dispatcher_switch_to_view(app->view_dispatcher, GenieViewStart);
        break;
    case GenieSubmenuIndexAbout:
        view_dispatcher_switch_to_view(app->view_dispatcher, GenieViewAbout);
        break;
    default:
        break;
    }
}

void genie_view_draw_callback(Canvas* canvas, void* model) {
    GenieApp* app = ((GenieAppRef*)model)->ref;
    canvas_draw_str(canvas, 5, 15, "Genie Sub-Ghz Recorder!!!");
    canvas_draw_str(canvas, 5, 30, "Make sure A7 is connected");
    canvas_draw_str(canvas, 100, 58, (app->pressed) ? "SEND" : "");
    char buffer[20] = {0};
    snprintf(buffer, COUNT_OF(buffer), "%ld", app->counter);
    canvas_draw_str(canvas, 50, 45, buffer);
    snprintf(buffer, COUNT_OF(buffer), "%ld", app->save_counter);
    canvas_draw_str(canvas, 96, 45, buffer);
    canvas_draw_str(canvas, 5, 55, furi_string_get_cstr(app->key));
}

bool genie_view_input_callback(InputEvent* event, void* context) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

uint32_t last_count() {
    uint32_t count = genie_load();
    return count;
}

void save_count(uint32_t count, FuriString* key) {
    FURI_LOG_I(TAG, "%ld,%s", count, furi_string_get_cstr(key));
    genie_save(count, key);
}

void __gui_redraw() {
    // Redraw screen
    Gui* gui = furi_record_open(RECORD_GUI);
    gui_direct_draw_acquire(gui);
    gui_direct_draw_release(gui);
}

void press_button(GenieApp* app) {
    furi_hal_gpio_write(pin_remote, false);
    app->pressed = true;
    __gui_redraw();
}

void release_button(GenieApp* app) {
    furi_hal_gpio_write(pin_remote, true);
    app->pressed = false;
    __gui_redraw();
}

void genie_packet(FuriString* buffer, void* context) {
    GenieApp* app = (GenieApp*)context;
    app->processing = true;
    release_button(app);

    FURI_LOG_I(TAG, "PACKET:\r\n%s", furi_string_get_cstr(buffer));
    if(furi_string_search_str(buffer, "KeeLoq 64bit") < furi_string_size(buffer)) {
        FURI_LOG_I(TAG, "KeeLoq 64bit found");
        size_t key_index = furi_string_search_str(buffer, "Key:");
        if(key_index < furi_string_size(buffer)) {
            FURI_LOG_I(TAG, "Key found");
            furi_string_set_n(app->key, buffer, key_index + 4, 16);
            app->save_counter++;
            save_count(app->counter, app->key);
        }
    }

    app->processing = false;
}

void genie_tick(void* context) {
    GenieApp* app = (GenieApp*)context;
    if(!app->processing) {
        if(app->pressed) {
            release_button(app);
        } else {
            app->counter++;
            press_button(app);
        }
    }
}

void genie_enter_callback(void* context) {
    GenieApp* app = (GenieApp*)context;
    genie_file_init();
    start_listening(app->genie_subghz, FREQUENCY, genie_packet, context);
    furi_timer_start(app->timer, furi_ms_to_ticks(CLICK_SPEED));
}

void genie_exit_callback(void* context) {
    GenieApp* app = (GenieApp*)context;
    app->processing = false;
    release_button(app);
    stop_listening(app->genie_subghz);
    furi_timer_stop(app->timer);
    genie_file_close();
}

GenieApp* genie_app_alloc() {
    GenieApp* app = (GenieApp*)malloc(sizeof(GenieApp));
    app->genie_subghz = genie_subghz_alloc();
    app->timer = furi_timer_alloc(genie_tick, FuriTimerTypePeriodic, app);
    app->counter = last_count();
    app->save_counter = app->counter;
    app->key = furi_string_alloc();

    furi_hal_gpio_init_simple(pin_remote, GpioModeOutputOpenDrain);
    release_button(app);

    Gui* gui = furi_record_open(RECORD_GUI);

    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_attach_to_gui(app->view_dispatcher, gui, ViewDispatcherTypeFullscreen);

    app->submenu = submenu_alloc();
    submenu_add_item(app->submenu, "Start", GenieSubmenuIndexStart, genie_submenu_callback, app);
    submenu_add_item(app->submenu, "About", GenieSubmenuIndexAbout, genie_submenu_callback, app);
    view_set_previous_callback(submenu_get_view(app->submenu), genie_navigation_exit_callback);
    view_dispatcher_add_view(
        app->view_dispatcher, GenieViewSubmenu, submenu_get_view(app->submenu));
    view_dispatcher_switch_to_view(app->view_dispatcher, GenieViewSubmenu);

    app->view = view_alloc();
    view_set_draw_callback(app->view, genie_view_draw_callback);
    view_set_input_callback(app->view, genie_view_input_callback);
    view_set_previous_callback(app->view, genie_navigation_submenu_callback);
    view_set_context(app->view, app);
    view_set_enter_callback(app->view, genie_enter_callback);
    view_set_exit_callback(app->view, genie_exit_callback);
    view_dispatcher_add_view(app->view_dispatcher, GenieViewStart, app->view);
    view_allocate_model(app->view, ViewModelTypeLockFree, sizeof(GenieAppRef));
    GenieAppRef* r = (GenieAppRef*)view_get_model(app->view);
    r->ref = app;

    app->widget_about = widget_alloc();
    widget_add_text_scroll_element(
        app->widget_about,
        0,
        0,
        128,
        64,
        "Genie garage door recorder.\n---\nConnect door to pin A7!\n\nauthor: @codeallnight\nhttps://discord.com/invite/NsjCvqwPAd\nhttps://youtube.com/@MrDerekJamison");
    view_set_previous_callback(
        widget_get_view(app->widget_about), genie_navigation_submenu_callback);
    view_dispatcher_add_view(
        app->view_dispatcher, GenieViewAbout, widget_get_view(app->widget_about));

    return app;
}

void genie_app_free(GenieApp* app) {
    genie_subghz_free(app->genie_subghz);
    furi_timer_free(app->timer);
    furi_hal_gpio_init_simple(pin_remote, GpioModeAnalog);
    furi_string_free(app->key);

    view_dispatcher_remove_view(app->view_dispatcher, GenieViewAbout);
    widget_free(app->widget_about);
    view_dispatcher_remove_view(app->view_dispatcher, GenieViewStart);
    view_free(app->view);
    view_dispatcher_remove_view(app->view_dispatcher, GenieViewSubmenu);
    submenu_free(app->submenu);
    view_dispatcher_free(app->view_dispatcher);
    furi_record_close(RECORD_GUI);

    free(app);
}

int32_t genie_record_app(void* p) {
    UNUSED(p);

    GenieApp* app = genie_app_alloc();
    view_dispatcher_run(app->view_dispatcher);
    genie_app_free(app);
    return 0;
}