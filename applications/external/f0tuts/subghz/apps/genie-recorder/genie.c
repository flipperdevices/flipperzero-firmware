#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <gui/view.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/submenu.h>
#include <gui/modules/variable_item_list.h>
#include <gui/modules/widget.h>

#include "genie_subghz_receive.h"
#include "genie_about.h"
#include "genie_file.h"

#define TAG "GenieRecord"

#define CLICK_SPEED 2000
const GpioPin* const pin_remote = &gpio_ext_pa7;

typedef enum {
    GenieSubmenuIndexConfig,
    GenieSubmenuIndexStart,
    GenieSubmenuIndexAbout,
} GenieSubmenuIndex;

typedef enum {
    GenieViewSubmenu,
    GenieViewConfig,
    GenieViewStart,
    GenieViewAbout,
} GenieView;

typedef struct {
    ViewDispatcher* view_dispatcher;
    Submenu* submenu;
    View* view;
    Widget* widget_about;
    VariableItemList* variable_item_list_config;
    GenieSubGhz* genie_subghz;
    uint32_t frequency;
    FuriTimer* timer;
    bool processing;
    bool pressed;
    uint32_t click_counter;
    uint32_t rx_counter;
    uint32_t genie_save_counter;
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
static uint32_t genie_navigation_exit_callback(void* context) {
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
static uint32_t genie_navigation_submenu_callback(void* context) {
    UNUSED(context);
    return GenieViewSubmenu;
}

static void genie_submenu_callback(void* context, uint32_t index) {
    GenieApp* app = (GenieApp*)context;
    switch(index) {
    case GenieSubmenuIndexConfig:
        view_dispatcher_switch_to_view(app->view_dispatcher, GenieViewConfig);
        break;
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

static uint32_t setting_frequency_values[] = {315000000, 390000000};
static char* setting_frequency_names[] = {"315 MHz", "390MHz"};
static void genie_setting_frequency_change(VariableItem* item) {
    GenieApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, setting_frequency_names[index]);
    app->frequency = setting_frequency_values[index];
}

static void genie_view_draw_callback(Canvas* canvas, void* model) {
    GenieApp* app = ((GenieAppRef*)model)->ref;
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 5, 10, "Genie Sub-Ghz Recorder!!!");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 5, 20, "A7/GND to Genie remote");
    canvas_draw_str(canvas, 100, 58, (app->pressed) ? "SEND" : "");
    char buffer[30] = {0};
    snprintf(buffer, COUNT_OF(buffer), "Click %ld", app->click_counter);
    canvas_draw_str(canvas, 1, 45, buffer);
    snprintf(
        buffer,
        COUNT_OF(buffer),
        "Got %ld",
        app->genie_save_counter > 0 ? app->genie_save_counter : app->rx_counter);
    canvas_draw_str(canvas, 75, 45, buffer);
    if(app->genie_save_counter < 0x10000) {
        snprintf(buffer, COUNT_OF(buffer), "Remaining codes %ld", 65536 - app->genie_save_counter);
        canvas_draw_str(canvas, 1, 32, buffer);
    } else {
        canvas_draw_str(canvas, 1, 30, "Found all codes!");
    }
    canvas_draw_str(canvas, 5, 55, furi_string_get_cstr(app->key));
}

static bool genie_view_input_callback(InputEvent* event, void* context) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

static uint32_t last_count() {
    uint32_t count = genie_load();
    return count;
}

static uint32_t save_count(uint32_t count, FuriString* key, bool is_genie) {
    FURI_LOG_D(TAG, "%ld,%s", count, furi_string_get_cstr(key));
    genie_save(count, key);
    if(is_genie) {
        return genie_save_bin(furi_string_get_cstr(key));
    }

    return 0;
}

static void __gui_redraw() {
    // Redraw screen
    Gui* gui = furi_record_open(RECORD_GUI);
    gui_direct_draw_acquire(gui);
    gui_direct_draw_release(gui);
}

static void press_button(GenieApp* app) {
    furi_hal_gpio_write(pin_remote, false);
    app->pressed = true;
    __gui_redraw();
}

static void release_button(GenieApp* app) {
    furi_hal_gpio_write(pin_remote, true);
    app->pressed = false;
    __gui_redraw();
}

static void genie_packet(FuriString* buffer, void* context) {
    GenieApp* app = (GenieApp*)context;
    app->processing = true;

    /*
    if(furi_string_search_str(buffer, "KeeLoq 64bit") < furi_string_size(buffer)) {
        release_button(app);
        FURI_LOG_D(TAG, "KeeLoq 64bit packet");
        size_t key_index = furi_string_search_str(buffer, "Key:");
        if(key_index < furi_string_size(buffer)) {
            furi_string_set_n(app->key, buffer, key_index + 4, 16);
            app->rx_counter++;
            save_count(app->click_counter, app->key, false);
        }
    } else 
*/
    if(furi_string_search_str(buffer, "Genie 64bit") < furi_string_size(buffer)) {
        release_button(app);
        FURI_LOG_D(TAG, "Genie 64bit packet");
        size_t key_index = furi_string_search_str(buffer, "Key:");
        if(key_index < furi_string_size(buffer)) {
            furi_string_set_n(app->key, buffer, key_index + 4, 16);
            app->rx_counter++;
            app->genie_save_counter = save_count(app->click_counter, app->key, true);
        }
    }

    app->processing = false;
}

static void genie_tick(void* context) {
    GenieApp* app = (GenieApp*)context;
    if(!app->processing) {
        if(app->genie_save_counter > 0xFFFF) {
            release_button(app);
        } else if(app->pressed) {
            release_button(app);
        } else {
            app->click_counter++;
            press_button(app);
        }
    }
}

static void genie_enter_callback(void* context) {
    GenieApp* app = (GenieApp*)context;
    genie_file_init();
    start_listening(app->genie_subghz, app->frequency, genie_packet, context);
    furi_timer_start(app->timer, furi_ms_to_ticks(CLICK_SPEED));
}

static void genie_exit_callback(void* context) {
    GenieApp* app = (GenieApp*)context;
    app->processing = false;
    release_button(app);
    stop_listening(app->genie_subghz);
    furi_timer_stop(app->timer);
}

static GenieApp* genie_app_alloc() {
    GenieApp* app = (GenieApp*)malloc(sizeof(GenieApp));
    app->genie_subghz = genie_subghz_alloc();
    app->timer = furi_timer_alloc(genie_tick, FuriTimerTypePeriodic, app);
    app->click_counter = 0;
    app->rx_counter = last_count();
    app->genie_save_counter = 0;
    app->key = furi_string_alloc();

    furi_hal_gpio_init_simple(pin_remote, GpioModeOutputOpenDrain);
    release_button(app);

    Gui* gui = furi_record_open(RECORD_GUI);

    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_attach_to_gui(app->view_dispatcher, gui, ViewDispatcherTypeFullscreen);

    app->submenu = submenu_alloc();
    submenu_add_item(app->submenu, "Config", GenieSubmenuIndexConfig, genie_submenu_callback, app);
    submenu_add_item(app->submenu, "Start", GenieSubmenuIndexStart, genie_submenu_callback, app);
    submenu_add_item(app->submenu, "About", GenieSubmenuIndexAbout, genie_submenu_callback, app);
    view_set_previous_callback(submenu_get_view(app->submenu), genie_navigation_exit_callback);
    view_dispatcher_add_view(
        app->view_dispatcher, GenieViewSubmenu, submenu_get_view(app->submenu));
    view_dispatcher_switch_to_view(app->view_dispatcher, GenieViewSubmenu);

    app->variable_item_list_config = variable_item_list_alloc();
    VariableItem* item = variable_item_list_add(
        app->variable_item_list_config,
        "Frequency",
        COUNT_OF(setting_frequency_names),
        genie_setting_frequency_change,
        app);
    uint8_t default_freq_index = 0;
    variable_item_set_current_value_index(item, default_freq_index);
    variable_item_set_current_value_text(item, setting_frequency_names[default_freq_index]);
    app->frequency = setting_frequency_values[default_freq_index];
    view_set_previous_callback(
        variable_item_list_get_view(app->variable_item_list_config),
        genie_navigation_submenu_callback);
    view_dispatcher_add_view(
        app->view_dispatcher,
        GenieViewConfig,
        variable_item_list_get_view(app->variable_item_list_config));

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
    widget_add_text_scroll_element(app->widget_about, 0, 0, 128, 64, GENIE_ABOUT_TEXT);
    view_set_previous_callback(
        widget_get_view(app->widget_about), genie_navigation_submenu_callback);
    view_dispatcher_add_view(
        app->view_dispatcher, GenieViewAbout, widget_get_view(app->widget_about));

    return app;
}

static void genie_app_free(GenieApp* app) {
    genie_subghz_free(app->genie_subghz);
    furi_timer_free(app->timer);
    furi_hal_gpio_init_simple(pin_remote, GpioModeAnalog);
    furi_string_free(app->key);

    view_dispatcher_remove_view(app->view_dispatcher, GenieViewConfig);
    variable_item_list_free(app->variable_item_list_config);
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