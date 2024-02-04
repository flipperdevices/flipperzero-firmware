#include "genie_app.h"

#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <gui/view.h>
#include <gui/modules/submenu.h>
#include <gui/modules/variable_item_list.h>
#include <gui/modules/widget.h>

#include "genie_subghz_receive.h"
#include "genie_file.h"
#include "genie_submenu.h"
#include "genie_send.h"
#include "genie_config.h"
#include "genie_learn.h"
#include "genie_about.h"
#include "genie_ini.h"

typedef enum {
    GenieViewSubmenu,
    GenieViewSend,
    GenieViewConfig,
    GenieViewLearn,
    GenieViewAbout,
} GenieView;

struct GenieApp {
    ViewDispatcher* view_dispatcher;
    GenieSubmenu* submenu;
    GenieSend* send_view;
    View* learn_view;
    Widget* widget_about;
    GenieConfig* genie_config;
    GenieSubGhz* genie_subghz;
    uint32_t frequency;
    FuriTimer* timer;
    bool processing;
    bool pressed;
    uint32_t click_counter;
    uint32_t try_counter;
    uint32_t rx_counter;
    uint32_t genie_save_counter;
    FuriString* key;
    FuriString* genie_saved_file_path;
};

extern const GpioPin* const pin_remote;

ViewDispatcher* genie_app_get_view_dispatcher(GenieApp* app) {
    return app->view_dispatcher;
}

GenieConfig* genie_app_get_genie_config(GenieApp* app) {
    return app->genie_config;
}

GenieSubGhz* genie_app_get_subghz(GenieApp* app) {
    return app->genie_subghz;
}

void genie_app_start_timer(GenieApp* app, uint32_t ms) {
    furi_timer_start(app->timer, furi_ms_to_ticks(ms));
    app->try_counter = 0;
    app->click_counter = 0;
}

void genie_app_stop_timer(GenieApp* app) {
    furi_timer_stop(app->timer);
}

void genie_app_set_frequency(GenieApp* app, uint32_t frequency) {
    app->frequency = frequency;
}

uint32_t genie_app_get_frequency(GenieApp* app) {
    return app->frequency;
}

void genie_app_gpio_send(GenieApp* app, bool sending_signal) {
    app->pressed = sending_signal;
}

bool genie_app_is_sending_signal(GenieApp* app) {
    return app->pressed;
}

uint32_t genie_app_get_click_counter(GenieApp* app) {
    return app->click_counter;
}

void genie_app_increment_click_counter(GenieApp* app) {
    app->click_counter++;
    app->try_counter++;
}

bool genie_app_has_no_signal(GenieApp* app) {
    UNUSED(app);
    return app->try_counter > 3;
}

FuriString* genie_app_get_key(GenieApp* app) {
    return app->key;
}

const char* genie_app_get_file_path(GenieApp* app) {
    return furi_string_get_cstr(app->genie_saved_file_path);
}

void genie_app_update_file_path(GenieApp* app, const char* file_path) {
    furi_string_set(app->genie_saved_file_path, file_path);
}

void genie_app_update_save_counter(GenieApp* app, uint32_t num_saved) {
    app->genie_save_counter = num_saved;
}

uint32_t genie_app_get_save_counter(GenieApp* app) {
    return app->genie_save_counter;
}

uint32_t genie_app_get_rx_counter(GenieApp* app) {
    return app->rx_counter;
}

void genie_app_set_processing_packet(GenieApp* app, bool processing_packet) {
    app->processing = processing_packet;
}

bool genie_app_is_processing_packet(GenieApp* app) {
    return app->processing;
}
void genie_app_received_key(GenieApp* app, FuriString* buffer) {
    size_t key_index = furi_string_search_str(buffer, "Key:");
    furi_string_set_n(app->key, buffer, key_index + 4, 16);
    app->rx_counter++;
    app->try_counter = 0;
}

static void __gui_redraw() {
    // Redraw screen
    Gui* gui = furi_record_open(RECORD_GUI);
    gui_direct_draw_acquire(gui);
    gui_direct_draw_release(gui);
}

void press_button(GenieApp* app) {
    furi_hal_gpio_write(pin_remote, false);
    genie_app_gpio_send(app, true);
    __gui_redraw();
}

void release_button(GenieApp* app) {
    furi_hal_gpio_write(pin_remote, true);
    genie_app_gpio_send(app, false);
    __gui_redraw();
}

static void genie_tick(void* context) {
    GenieApp* app = (GenieApp*)context;
    if(!genie_app_is_processing_packet(app)) {
        if(genie_app_get_save_counter(app) > 0xFFFF) {
            release_button(app);
        } else if(genie_app_is_sending_signal(app)) {
            release_button(app);
        } else if(!genie_app_has_no_signal(app)) {
            genie_app_increment_click_counter(app);
            press_button(app);
        }
    }
}

GenieApp* genie_app_alloc() {
    GenieApp* app = (GenieApp*)malloc(sizeof(GenieApp));
    app->genie_subghz = genie_subghz_alloc();
    app->rx_counter = genie_load();
    app->try_counter = 0;
    app->click_counter = 0;
    app->genie_save_counter = 0;
    app->key = furi_string_alloc();
    app->genie_saved_file_path = furi_string_alloc();
    app->timer = furi_timer_alloc(genie_tick, FuriTimerTypePeriodic, app);

    furi_hal_gpio_init_simple(pin_remote, GpioModeOutputOpenDrain);
    release_button(app);

    genie_ini_load(app);

    Gui* gui = furi_record_open(RECORD_GUI);

    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_attach_to_gui(app->view_dispatcher, gui, ViewDispatcherTypeFullscreen);

    app->submenu = genie_submenu_alloc(app);
    View* submenu_view = genie_submenu_get_view(app->submenu);
    view_dispatcher_add_view(app->view_dispatcher, GenieViewSubmenu, submenu_view);
    view_dispatcher_switch_to_view(app->view_dispatcher, GenieViewSubmenu);

    app->send_view = genie_send_alloc(app);
    View* send_view = genie_send_get_view(app->send_view);
    view_dispatcher_add_view(app->view_dispatcher, GenieViewSend, send_view);

    app->genie_config = genie_config_alloc(app);
    View* config_view = genie_config_get_view(app->genie_config);
    view_dispatcher_add_view(app->view_dispatcher, GenieViewConfig, config_view);

    app->learn_view = genie_learn_alloc(app);
    view_dispatcher_add_view(app->view_dispatcher, GenieViewLearn, app->learn_view);

    app->widget_about = widget_alloc();
    widget_add_text_scroll_element(app->widget_about, 0, 0, 128, 64, GENIE_ABOUT_TEXT);
    view_set_previous_callback(
        widget_get_view(app->widget_about), genie_navigation_submenu_callback);
    view_dispatcher_add_view(
        app->view_dispatcher, GenieViewAbout, widget_get_view(app->widget_about));

    return app;
}

void genie_app_free(GenieApp* app) {
    genie_ini_save(app);

    genie_subghz_free(app->genie_subghz);
    furi_timer_free(app->timer);
    furi_hal_gpio_init_simple(pin_remote, GpioModeAnalog);
    furi_string_free(app->key);
    furi_string_free(app->genie_saved_file_path);

    view_dispatcher_remove_view(app->view_dispatcher, GenieViewSend);
    genie_send_free(app->send_view);
    view_dispatcher_remove_view(app->view_dispatcher, GenieViewConfig);
    genie_config_free(app->genie_config);
    view_dispatcher_remove_view(app->view_dispatcher, GenieViewAbout);
    widget_free(app->widget_about);
    view_dispatcher_remove_view(app->view_dispatcher, GenieViewLearn);
    genie_learn_free(app->learn_view);
    view_dispatcher_remove_view(app->view_dispatcher, GenieViewSubmenu);
    genie_submenu_free(app->submenu);
    view_dispatcher_free(app->view_dispatcher);
    furi_record_close(RECORD_GUI);

    free(app);
}
