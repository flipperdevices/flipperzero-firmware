#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <gui/view.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/submenu.h>
#include <gui/modules/widget.h>
#include <gui/modules/variable_item_list.h>
#include <notification/notification.h>
#include <notification/notification_messages.h>
#include "flipboard_icons.h"

#include "model.h"
#include "keyboard.h"
#include "leds.h"
#include "key_monitor.h"
#include "flipboard_file.h"

#define TAG "FlipboardId"

// Comment this line if you don't want the backlight to be continuously on.
#define BACKLIGHT_ALWAYS_ON yep

typedef enum {
    FlipboardSubmenuIndexConfigure,
    FlipboardSubmenuIndexFlipTheWorld,
    FlipboardSubmenuIndexAbout,
} FlipboardSubmenuIndex;

typedef enum {
    FlipboardViewSubmenu,
    FlipboardViewConfigure,
    FlipboardViewConfigureColors,
    FlipboardViewConfigureKeys,
    FlipboardViewFlipTheWorld,
    FlipboardViewAbout,
} FlipboardView;

typedef struct {
    FlipboardModel* model;
} FlipboardModelRef;

typedef struct {
    ViewDispatcher* view_dispatcher;
    NotificationApp* notifications;
    Submenu* submenu;
    VariableItemList* variable_item_list_config;
    View* view_flip_the_world;
    Widget* widget_about;

    VariableItemList* variable_item_list_colors;
    VariableItemList* variable_item_list_keys;

    FlipboardModel* model;
} Flipboard;

uint32_t setting_switch_color_values[] = {0xFF0000, 0x00FF00, 0x0000FF};
char* setting_switch_color_names[] = {"Red", "Green", "Blue"};

uint16_t setting_switch_key_values[] = {
    0,
    HID_KEYBOARD_A | KEY_MOD_LEFT_SHIFT, // A
    HID_KEYBOARD_B | KEY_MOD_LEFT_SHIFT, // B
    HID_KEYBOARD_C | KEY_MOD_LEFT_SHIFT, // C
    HID_KEYBOARD_A, // a
    HID_KEYBOARD_B, // b
    HID_KEYBOARD_C, // c
};

char* setting_switch_key_names[] = {"None", "A", "B", "C", "a", "b", "c"};
char* setting_switch_key_repeat_names[] =
    {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15"};

/**
 * @brief      Callback for navigation events
 * @details    This function is called when user press back button.  We return VIEW_NONE to
 *            indicate that we want to exit the application.
 * @param      context  The context
 * @return     next view id
*/
uint32_t flipboard_navigation_exit_callback(void* context) {
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
uint32_t flipboard_navigation_submenu_callback(void* context) {
    UNUSED(context);
    return FlipboardViewSubmenu;
}

void update_leds(Flipboard* app) {
    FlipboardModel* model = app->model;
    flipboard_leds_set(model->leds, 0, setting_switch_color_values[model->setting_sw1up_index]);
    flipboard_leds_set(model->leds, 1, setting_switch_color_values[model->setting_sw2up_index]);
    flipboard_leds_set(model->leds, 2, setting_switch_color_values[model->setting_sw3up_index]);
    flipboard_leds_set(model->leds, 3, setting_switch_color_values[model->setting_sw4up_index]);
    flipboard_leds_update(model->leds);
}

void flipboard_submenu_callback(void* context, uint32_t index) {
    Flipboard* app = (Flipboard*)context;
    switch(index) {
    case FlipboardSubmenuIndexConfigure: {
        update_leds(app);
        view_dispatcher_switch_to_view(app->view_dispatcher, FlipboardViewConfigureKeys);
        break;
    }
    case FlipboardSubmenuIndexFlipTheWorld:
        view_dispatcher_switch_to_view(app->view_dispatcher, FlipboardViewFlipTheWorld);
        break;
    case FlipboardSubmenuIndexAbout:
        view_dispatcher_switch_to_view(app->view_dispatcher, FlipboardViewAbout);
        break;
    default:
        break;
    }
}

/*
uint32_t setting_switch_key_values[] = {
char* setting_switch_key_names[] = {"None", "A", "B", "C", "a", "b", "c"};
uint8_t setting_key1_key_index[10];
uint8_t setting_key1_count_index[10];
*/
void flipboard_setting_key1_change(VariableItem* item, int i) {
    Flipboard* app = variable_item_get_context(item);
    FlipboardModel* model = app->model;
    uint8_t index = variable_item_get_current_value_index(item);
    model->setting_key1_key_index[i] = index;
    variable_item_set_current_value_text(item, setting_switch_key_names[index]);
}
void flipboard_setting_key1_0_change(VariableItem* item) {
    flipboard_setting_key1_change(item, 0);
}
void flipboard_setting_key1_1_change(VariableItem* item) {
    flipboard_setting_key1_change(item, 1);
}
void flipboard_setting_key1_2_change(VariableItem* item) {
    flipboard_setting_key1_change(item, 2);
}
void flipboard_setting_key1_3_change(VariableItem* item) {
    flipboard_setting_key1_change(item, 3);
}
void flipboard_setting_key1_4_change(VariableItem* item) {
    flipboard_setting_key1_change(item, 4);
}
void flipboard_setting_key1_5_change(VariableItem* item) {
    flipboard_setting_key1_change(item, 5);
}
void flipboard_setting_key1_6_change(VariableItem* item) {
    flipboard_setting_key1_change(item, 6);
}
void flipboard_setting_key1_7_change(VariableItem* item) {
    flipboard_setting_key1_change(item, 7);
}
void flipboard_setting_key1_8_change(VariableItem* item) {
    flipboard_setting_key1_change(item, 8);
}
void flipboard_setting_key1_9_change(VariableItem* item) {
    flipboard_setting_key1_change(item, 9);
}

void flipboard_setting_key1repeat_change(VariableItem* item, int i) {
    Flipboard* app = variable_item_get_context(item);
    FlipboardModel* model = app->model;
    uint8_t index = variable_item_get_current_value_index(item);
    model->setting_key1_count_index[i] = index;
    variable_item_set_current_value_text(item, setting_switch_key_repeat_names[index]);
}
void flipboard_setting_key1repeat_0_change(VariableItem* item) {
    flipboard_setting_key1repeat_change(item, 0);
}
void flipboard_setting_key1repeat_1_change(VariableItem* item) {
    flipboard_setting_key1repeat_change(item, 1);
}
void flipboard_setting_key1repeat_2_change(VariableItem* item) {
    flipboard_setting_key1repeat_change(item, 2);
}
void flipboard_setting_key1repeat_3_change(VariableItem* item) {
    flipboard_setting_key1repeat_change(item, 3);
}
void flipboard_setting_key1repeat_4_change(VariableItem* item) {
    flipboard_setting_key1repeat_change(item, 4);
}
void flipboard_setting_key1repeat_5_change(VariableItem* item) {
    flipboard_setting_key1repeat_change(item, 5);
}
void flipboard_setting_key1repeat_6_change(VariableItem* item) {
    flipboard_setting_key1repeat_change(item, 6);
}
void flipboard_setting_key1repeat_7_change(VariableItem* item) {
    flipboard_setting_key1repeat_change(item, 7);
}
void flipboard_setting_key1repeat_8_change(VariableItem* item) {
    flipboard_setting_key1repeat_change(item, 8);
}
void flipboard_setting_key1repeat_9_change(VariableItem* item) {
    flipboard_setting_key1repeat_change(item, 9);
}

void flipboard_setting_x_change(int i, VariableItem* item, uint8_t* setting_index) {
    UNUSED(setting_index);
    uint8_t index = variable_item_get_current_value_index(item);
    *setting_index = index;
    variable_item_set_current_value_text(item, setting_switch_color_names[index]);
    FlipboardModel* model = ((Flipboard*)variable_item_get_context(item))->model;
    flipboard_leds_set(model->leds, i - 1, setting_switch_color_values[index]);
    flipboard_leds_update(model->leds);
}

void flipboard_setting_sw1up_change(VariableItem* item) {
    UNUSED(item);
    flipboard_setting_x_change(
        1, item, &((((Flipboard*)variable_item_get_context(item))->model)->setting_sw1up_index));
}

void flipboard_setting_sw1down_change(VariableItem* item) {
    flipboard_setting_x_change(
        1, item, &((((Flipboard*)variable_item_get_context(item))->model)->setting_sw1down_index));
}

void flipboard_setting_sw2up_change(VariableItem* item) {
    flipboard_setting_x_change(
        2, item, &((((Flipboard*)variable_item_get_context(item))->model)->setting_sw2up_index));
}

void flipboard_setting_sw2down_change(VariableItem* item) {
    flipboard_setting_x_change(
        2, item, &((((Flipboard*)variable_item_get_context(item))->model)->setting_sw2down_index));
}

void flipboard_setting_sw3up_change(VariableItem* item) {
    flipboard_setting_x_change(
        3, item, &((((Flipboard*)variable_item_get_context(item))->model)->setting_sw3up_index));
}

void flipboard_setting_sw3down_change(VariableItem* item) {
    flipboard_setting_x_change(
        3, item, &((((Flipboard*)variable_item_get_context(item))->model)->setting_sw3down_index));
}

void flipboard_setting_sw4up_change(VariableItem* item) {
    flipboard_setting_x_change(
        4, item, &((((Flipboard*)variable_item_get_context(item))->model)->setting_sw4up_index));
}

void flipboard_setting_sw4down_change(VariableItem* item) {
    flipboard_setting_x_change(
        4, item, &((((Flipboard*)variable_item_get_context(item))->model)->setting_sw4down_index));
}

uint8_t setting_1_values[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
char* setting_1_names[] =
    {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F"};

void flipboard_setting_1_change(VariableItem* item) {
    Flipboard* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, setting_1_names[index]);
    FlipboardModel* model = view_get_model(app->view_flip_the_world);
    model->setting_1_index = index;
}

char* setting_2_values[] = {"Hello", "World"};
char* setting_2_names[] = {"Hi", "Bye"};

void flipboard_setting_2_change(VariableItem* item) {
    Flipboard* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, setting_2_names[index]);
    FlipboardModel* model = view_get_model(app->view_flip_the_world);
    model->setting_2_index = index;
}

void flipboard_view_draw_callback(Canvas* canvas, void* model) {
    FlipboardModel* my_model = (FlipboardModel*)model;
    canvas_draw_str(canvas, 25, 15, "FLIP THE WORLD!!!");
    canvas_draw_icon(canvas, 64, 32, &I_glyph_1_7x9);
    canvas_draw_str(canvas, 64, 60, setting_1_names[my_model->setting_1_index]);
}

bool flipboard_view_input_callback(InputEvent* event, void* context) {
    Flipboard* app = (Flipboard*)context;
    UNUSED(context);
    UNUSED(event);
    uint8_t pin_status = 7;
    view_dispatcher_send_custom_event(app->view_dispatcher, pin_status);

    return false;
}

float freqs[] = {
    523.251,
    587.330,
    659.255,
    698.456,
    783.991,
    880.000,
    987.767,
    1046.50,
    1174.66,
    1318.51,
    1396.913,
    1567.92,
    1760,
    1975.533,
    2093,
    2349.318,
};

bool flipboard_debounced_sw_callback(void* context, uint8_t old_key, uint8_t new_key) {
    Flipboard* app = (Flipboard*)context;
    FlipboardModel* model = app->model;

    FURI_LOG_D(TAG, "SW EVENT: old=%d new=%d", old_key, new_key);
    flipboard_leds_set(
        model->leds,
        0,
        IS_SWITCH_PRESSED(new_key, SwitchId1) ?
            setting_switch_color_values[model->setting_sw1down_index] :
            setting_switch_color_values[model->setting_sw1up_index]);
    flipboard_leds_set(
        model->leds,
        1,
        IS_SWITCH_PRESSED(new_key, SwitchId2) ?
            setting_switch_color_values[model->setting_sw2down_index] :
            setting_switch_color_values[model->setting_sw2up_index]);
    flipboard_leds_set(
        model->leds,
        2,
        IS_SWITCH_PRESSED(new_key, SwitchId3) ?
            setting_switch_color_values[model->setting_sw3down_index] :
            setting_switch_color_values[model->setting_sw3up_index]);
    flipboard_leds_set(
        model->leds,
        3,
        IS_SWITCH_PRESSED(new_key, SwitchId4) ?
            setting_switch_color_values[model->setting_sw4down_index] :
            setting_switch_color_values[model->setting_sw4up_index]);
    flipboard_leds_update(model->leds);

    if(new_key != 0) {
        if(new_key == 1) {
            for(int i = 0; i < 10; i++) {
                for(uint8_t r = 0; r <= model->setting_key1_count_index[i]; r++) {
                    uint16_t keycode = setting_switch_key_values[model->setting_key1_key_index[i]];
                    if(keycode == 0) {
                        continue;
                    }
                    flipboard_keyboard_send_keycode(model->keyboard, keycode);
                    flipboard_keyboard_release_all(model->keyboard);
                }
            }
        }

        furi_hal_speaker_start(freqs[new_key - 1], 1.0);
    } else {
        furi_hal_speaker_stop();
    }

    return true;
}

void flipboard_enter_callback(void* context) {
    Flipboard* app = (Flipboard*)context;

    update_leds(app);

    app->model->key_monitor = key_monitor_alloc();
    key_monitor_set_callback(app->model->key_monitor, flipboard_debounced_sw_callback, app);
}

void flipboard_exit_callback(void* context) {
    Flipboard* app = (Flipboard*)context;
    key_monitor_free(app->model->key_monitor);
    app->model->key_monitor = NULL;
    flipboard_leds_reset(app->model->leds);
}

Flipboard* flipboard_alloc() {
    Flipboard* app = (Flipboard*)malloc(sizeof(Flipboard));
    app->model = (FlipboardModel*)malloc(sizeof(FlipboardModel));
    app->model->keyboard = flipboard_keyboard_alloc();
    app->model->leds = flipboard_leds_alloc();
    app->model->key_monitor = NULL;
    flipboard_leds_update(app->model->leds);

    Gui* gui = furi_record_open(RECORD_GUI);

    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_attach_to_gui(app->view_dispatcher, gui, ViewDispatcherTypeFullscreen);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);

    app->submenu = submenu_alloc();
    submenu_add_item(
        app->submenu, "Config", FlipboardSubmenuIndexConfigure, flipboard_submenu_callback, app);
    submenu_add_item(
        app->submenu,
        "Flipboard",
        FlipboardSubmenuIndexFlipTheWorld,
        flipboard_submenu_callback,
        app);
    submenu_add_item(
        app->submenu, "About", FlipboardSubmenuIndexAbout, flipboard_submenu_callback, app);
    view_set_previous_callback(submenu_get_view(app->submenu), flipboard_navigation_exit_callback);
    view_dispatcher_add_view(
        app->view_dispatcher, FlipboardViewSubmenu, submenu_get_view(app->submenu));
    view_dispatcher_switch_to_view(app->view_dispatcher, FlipboardViewSubmenu);

    VariableItem* item;

    flipboard_load(app->model);

    app->variable_item_list_keys = variable_item_list_alloc();
    variable_item_list_reset(app->variable_item_list_keys);
    for(int i = 0; i < 10; i++) {
        // app->model->setting_key1_count_index[i] = 0;
        // app->model->setting_key1_key_index[i] = 0;
        item = variable_item_list_add(
            app->variable_item_list_keys,
            "Key 1",
            COUNT_OF(setting_switch_key_names),
            (i == 0) ? flipboard_setting_key1_0_change :
            (i == 1) ? flipboard_setting_key1_1_change :
            (i == 2) ? flipboard_setting_key1_2_change :
            (i == 3) ? flipboard_setting_key1_3_change :
            (i == 4) ? flipboard_setting_key1_4_change :
            (i == 5) ? flipboard_setting_key1_5_change :
            (i == 6) ? flipboard_setting_key1_6_change :
            (i == 7) ? flipboard_setting_key1_7_change :
            (i == 8) ? flipboard_setting_key1_8_change :
                       flipboard_setting_key1_9_change,
            app);
        variable_item_set_current_value_index(item, app->model->setting_key1_key_index[i]);
        variable_item_set_current_value_text(
            item, setting_switch_key_names[app->model->setting_key1_key_index[i]]);

        item = variable_item_list_add(
            app->variable_item_list_keys,
            "Number of times",
            COUNT_OF(setting_switch_key_repeat_names),
            (i == 0) ? flipboard_setting_key1repeat_0_change :
            (i == 1) ? flipboard_setting_key1repeat_1_change :
            (i == 2) ? flipboard_setting_key1repeat_2_change :
            (i == 3) ? flipboard_setting_key1repeat_3_change :
            (i == 4) ? flipboard_setting_key1repeat_4_change :
            (i == 5) ? flipboard_setting_key1repeat_5_change :
            (i == 6) ? flipboard_setting_key1repeat_6_change :
            (i == 7) ? flipboard_setting_key1repeat_7_change :
            (i == 8) ? flipboard_setting_key1repeat_8_change :
                       flipboard_setting_key1repeat_9_change,
            app);
        variable_item_set_current_value_index(item, app->model->setting_key1_count_index[i]);
        variable_item_set_current_value_text(
            item, setting_switch_key_repeat_names[app->model->setting_key1_count_index[i]]);
    }

    view_set_previous_callback(
        variable_item_list_get_view(app->variable_item_list_keys),
        flipboard_navigation_submenu_callback);
    view_dispatcher_add_view(
        app->view_dispatcher,
        FlipboardViewConfigureKeys,
        variable_item_list_get_view(app->variable_item_list_keys));

    app->variable_item_list_colors = variable_item_list_alloc();
    variable_item_list_reset(app->variable_item_list_colors);

    app->model->setting_sw1up_index = 0;
    app->model->setting_sw1down_index = 0;
    app->model->setting_sw2up_index = 0;
    app->model->setting_sw2down_index = 0;
    app->model->setting_sw3up_index = 0;
    app->model->setting_sw3down_index = 0;
    app->model->setting_sw4up_index = 0;
    app->model->setting_sw4down_index = 0;

    item = variable_item_list_add(
        app->variable_item_list_colors,
        "Switch 1 up",
        COUNT_OF(setting_switch_color_values),
        flipboard_setting_sw1up_change,
        app);
    variable_item_set_current_value_index(item, app->model->setting_sw1up_index);
    variable_item_set_current_value_text(
        item, setting_switch_color_names[app->model->setting_sw1up_index]);

    item = variable_item_list_add(
        app->variable_item_list_colors,
        "Switch 1 down",
        COUNT_OF(setting_switch_color_values),
        flipboard_setting_sw1down_change,
        app);
    variable_item_set_current_value_index(item, app->model->setting_sw1down_index);
    variable_item_set_current_value_text(
        item, setting_switch_color_names[app->model->setting_sw1down_index]);

    item = variable_item_list_add(
        app->variable_item_list_colors,
        "Switch 2 up",
        COUNT_OF(setting_switch_color_values),
        flipboard_setting_sw2up_change,
        app);
    variable_item_set_current_value_index(item, app->model->setting_sw2up_index);
    variable_item_set_current_value_text(
        item, setting_switch_color_names[app->model->setting_sw2up_index]);

    item = variable_item_list_add(
        app->variable_item_list_colors,
        "Switch 2 down",
        COUNT_OF(setting_switch_color_values),
        flipboard_setting_sw2down_change,
        app);
    variable_item_set_current_value_index(item, app->model->setting_sw2down_index);
    variable_item_set_current_value_text(
        item, setting_switch_color_names[app->model->setting_sw2down_index]);

    item = variable_item_list_add(
        app->variable_item_list_colors,
        "Switch 3 up",
        COUNT_OF(setting_switch_color_values),
        flipboard_setting_sw3up_change,
        app);
    variable_item_set_current_value_index(item, app->model->setting_sw3up_index);
    variable_item_set_current_value_text(
        item, setting_switch_color_names[app->model->setting_sw3up_index]);

    item = variable_item_list_add(
        app->variable_item_list_colors,
        "Switch 3 down",
        COUNT_OF(setting_switch_color_values),
        flipboard_setting_sw3down_change,
        app);
    variable_item_set_current_value_index(item, app->model->setting_sw3down_index);
    variable_item_set_current_value_text(
        item, setting_switch_color_names[app->model->setting_sw3down_index]);

    item = variable_item_list_add(
        app->variable_item_list_colors,
        "Switch 4 up",
        COUNT_OF(setting_switch_color_values),
        flipboard_setting_sw4up_change,
        app);
    variable_item_set_current_value_index(item, app->model->setting_sw4up_index);
    variable_item_set_current_value_text(
        item, setting_switch_color_names[app->model->setting_sw4up_index]);

    item = variable_item_list_add(
        app->variable_item_list_colors,
        "Switch 4 down",
        COUNT_OF(setting_switch_color_values),
        flipboard_setting_sw4down_change,
        app);
    variable_item_set_current_value_index(item, app->model->setting_sw4down_index);
    variable_item_set_current_value_text(
        item, setting_switch_color_names[app->model->setting_sw4down_index]);

    view_set_previous_callback(
        variable_item_list_get_view(app->variable_item_list_colors),
        flipboard_navigation_submenu_callback);
    view_dispatcher_add_view(
        app->view_dispatcher,
        FlipboardViewConfigureColors,
        variable_item_list_get_view(app->variable_item_list_colors));

    app->variable_item_list_config = variable_item_list_alloc();
    variable_item_list_reset(app->variable_item_list_config);
    item = variable_item_list_add(
        app->variable_item_list_config,
        "Setting 1",
        COUNT_OF(setting_1_values),
        flipboard_setting_1_change,
        app);
    app->model->setting_1_index = 0;
    variable_item_set_current_value_index(item, app->model->setting_1_index);
    variable_item_set_current_value_text(item, setting_1_names[app->model->setting_1_index]);

    item = variable_item_list_add(
        app->variable_item_list_config,
        "Message",
        COUNT_OF(setting_2_values),
        flipboard_setting_2_change,
        app);
    app->model->setting_2_index = 0;
    variable_item_set_current_value_index(item, app->model->setting_2_index);
    variable_item_set_current_value_text(item, setting_2_names[app->model->setting_2_index]);

    view_set_previous_callback(
        variable_item_list_get_view(app->variable_item_list_config),
        flipboard_navigation_submenu_callback);
    view_dispatcher_add_view(
        app->view_dispatcher,
        FlipboardViewConfigure,
        variable_item_list_get_view(app->variable_item_list_config));

    app->view_flip_the_world = view_alloc();
    view_set_draw_callback(app->view_flip_the_world, flipboard_view_draw_callback);
    view_set_input_callback(app->view_flip_the_world, flipboard_view_input_callback);
    view_set_previous_callback(app->view_flip_the_world, flipboard_navigation_submenu_callback);
    view_set_enter_callback(app->view_flip_the_world, flipboard_enter_callback);
    view_set_exit_callback(app->view_flip_the_world, flipboard_exit_callback);
    view_set_context(app->view_flip_the_world, app);
    view_allocate_model(
        app->view_flip_the_world, ViewModelTypeLockFree, sizeof(FlipboardModelRef));
    FlipboardModelRef* ref = (FlipboardModelRef*)view_get_model(app->view_flip_the_world);
    ref->model = app->model;
    view_dispatcher_add_view(
        app->view_dispatcher, FlipboardViewFlipTheWorld, app->view_flip_the_world);

    app->widget_about = widget_alloc();
    widget_add_text_scroll_element(
        app->widget_about,
        0,
        0,
        128,
        64,
        "This is a sample application.\n---\nReplace code and message\nwith your content!\n\nauthor: @codeallnight\nhttps://discord.com/invite/NsjCvqwPAd\nhttps://youtube.com/@MrDerekJamison");
    view_set_previous_callback(
        widget_get_view(app->widget_about), flipboard_navigation_submenu_callback);
    view_dispatcher_add_view(
        app->view_dispatcher, FlipboardViewAbout, widget_get_view(app->widget_about));

    app->notifications = furi_record_open(RECORD_NOTIFICATION);

#ifdef BACKLIGHT_ALWAYS_ON
    notification_message(app->notifications, &sequence_display_backlight_enforce_on);
#endif

    return app;
}

void flipboard_free(Flipboard* app) {
#ifdef BACKLIGHT_ALWAYS_ON
    notification_message(app->notifications, &sequence_display_backlight_enforce_auto);
#endif

    flipboard_save(app->model);

    furi_record_close(RECORD_NOTIFICATION);

    if(app->model->key_monitor != NULL) {
        key_monitor_free(app->model->key_monitor);
    }

    // free keyboard
    flipboard_keyboard_free(app->model->keyboard);
    flipboard_leds_free(app->model->leds);

    view_dispatcher_remove_view(app->view_dispatcher, FlipboardViewAbout);
    widget_free(app->widget_about);
    view_dispatcher_remove_view(app->view_dispatcher, FlipboardViewFlipTheWorld);
    view_free(app->view_flip_the_world);
    view_dispatcher_remove_view(app->view_dispatcher, FlipboardViewConfigureKeys);
    variable_item_list_free(app->variable_item_list_keys);
    view_dispatcher_remove_view(app->view_dispatcher, FlipboardViewConfigureColors);
    variable_item_list_free(app->variable_item_list_colors);
    view_dispatcher_remove_view(app->view_dispatcher, FlipboardViewConfigure);
    variable_item_list_free(app->variable_item_list_config);
    view_dispatcher_remove_view(app->view_dispatcher, FlipboardViewSubmenu);
    submenu_free(app->submenu);
    view_dispatcher_free(app->view_dispatcher);
    furi_record_close(RECORD_GUI);

    free(app);
}

int32_t flipboard_app(void* p) {
    UNUSED(p);

    Flipboard* app = flipboard_alloc();
    view_dispatcher_run(app->view_dispatcher);
    flipboard_free(app);

    return 0;
}