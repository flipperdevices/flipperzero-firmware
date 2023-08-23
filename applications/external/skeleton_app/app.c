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
#include "my_app_id_42_icons.h"

#define TAG "MyAppId"

// Comment this line if you don't want the backlight to be continuously on.
#define BACKLIGHT_ALWAYS_ON yep

typedef enum {
    MyAppSubmenuIndexConfigure,
    MyAppSubmenuIndexFlipTheWorld,
    MyAppSubmenuIndexAbout,
} MyAppSubmenuIndex;

typedef enum {
    MyAppViewSubmenu,
    MyAppViewConfigure,
    MyAppViewFlipTheWorld,
    MyAppViewAbout,
} MyAppView;

typedef struct {
    ViewDispatcher* view_dispatcher;
    NotificationApp* notifications;
    Submenu* submenu;
    VariableItemList* variable_item_list_config;
    View* view_flip_the_world;
    Widget* widget_about;
} MyApp;

typedef struct {
    uint32_t setting_1_index;
} MyModel;

/**
 * @brief      Callback for navigation events
 * @details    This function is called when user press back button.  We return VIEW_NONE to
 *            indicate that we want to exit the application.
 * @param      context  The context
 * @return     next view id
*/
uint32_t my_app_navigation_exit_callback(void* context) {
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
uint32_t my_app_navigation_submenu_callback(void* context) {
    UNUSED(context);
    return MyAppViewSubmenu;
}

void my_app_submenu_callback(void* context, uint32_t index) {
    MyApp* app = (MyApp*)context;
    switch(index) {
    case MyAppSubmenuIndexConfigure:
        view_dispatcher_switch_to_view(app->view_dispatcher, MyAppViewConfigure);
        break;
    case MyAppSubmenuIndexFlipTheWorld:
        view_dispatcher_switch_to_view(app->view_dispatcher, MyAppViewFlipTheWorld);
        break;
    case MyAppSubmenuIndexAbout:
        view_dispatcher_switch_to_view(app->view_dispatcher, MyAppViewAbout);
        break;
    default:
        break;
    }
}

uint8_t setting_1_values[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
char* setting_1_names[] =
    {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F"};

void my_app_setting_1_change(VariableItem* item) {
    MyApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, setting_1_names[index]);
    MyModel* model = view_get_model(app->view_flip_the_world);
    model->setting_1_index = index;
}

void my_app_view_draw_callback(Canvas* canvas, void* model) {
    MyModel* my_model = (MyModel*)model;
    canvas_draw_str(canvas, 25, 15, "FLIP THE WORLD!!!");
    canvas_draw_icon(canvas, 64, 32, &I_glyph_1_7x9);
    canvas_draw_str(canvas, 64, 60, setting_1_names[my_model->setting_1_index]);
}

bool my_app_view_input_callback(InputEvent* event, void* context) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

MyApp* my_app_alloc() {
    MyApp* app = (MyApp*)malloc(sizeof(MyApp));

    Gui* gui = furi_record_open(RECORD_GUI);

    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_attach_to_gui(app->view_dispatcher, gui, ViewDispatcherTypeFullscreen);

    app->submenu = submenu_alloc();
    submenu_add_item(
        app->submenu, "Config", MyAppSubmenuIndexConfigure, my_app_submenu_callback, app);
    submenu_add_item(
        app->submenu,
        "Flip the world",
        MyAppSubmenuIndexFlipTheWorld,
        my_app_submenu_callback,
        app);
    submenu_add_item(app->submenu, "About", MyAppSubmenuIndexAbout, my_app_submenu_callback, app);
    view_set_previous_callback(submenu_get_view(app->submenu), my_app_navigation_exit_callback);
    view_dispatcher_add_view(
        app->view_dispatcher, MyAppViewSubmenu, submenu_get_view(app->submenu));
    view_dispatcher_switch_to_view(app->view_dispatcher, MyAppViewSubmenu);

    app->variable_item_list_config = variable_item_list_alloc();
    variable_item_list_reset(app->variable_item_list_config);
    VariableItem* item = variable_item_list_add(
        app->variable_item_list_config,
        "Setting 1",
        COUNT_OF(setting_1_values),
        my_app_setting_1_change,
        app);
    uint8_t setting_1_index = 0;
    variable_item_set_current_value_index(item, setting_1_index);
    variable_item_set_current_value_text(item, setting_1_names[setting_1_index]);
    view_set_previous_callback(
        variable_item_list_get_view(app->variable_item_list_config),
        my_app_navigation_submenu_callback);
    view_dispatcher_add_view(
        app->view_dispatcher,
        MyAppViewConfigure,
        variable_item_list_get_view(app->variable_item_list_config));

    app->view_flip_the_world = view_alloc();
    view_set_draw_callback(app->view_flip_the_world, my_app_view_draw_callback);
    view_set_input_callback(app->view_flip_the_world, my_app_view_input_callback);
    view_set_previous_callback(app->view_flip_the_world, my_app_navigation_submenu_callback);
    view_allocate_model(app->view_flip_the_world, ViewModelTypeLockFree, sizeof(MyModel));
    MyModel* model = view_get_model(app->view_flip_the_world);
    model->setting_1_index = setting_1_index;
    view_dispatcher_add_view(
        app->view_dispatcher, MyAppViewFlipTheWorld, app->view_flip_the_world);

    app->widget_about = widget_alloc();
    widget_add_text_scroll_element(
        app->widget_about,
        0,
        0,
        128,
        64,
        "This is a sample application.\n---\nReplace code and message\nwith your content!\n\nauthor: @codeallnight\nhttps://discord.com/invite/NsjCvqwPAd\nhttps://youtube.com/@MrDerekJamison");
    view_set_previous_callback(
        widget_get_view(app->widget_about), my_app_navigation_submenu_callback);
    view_dispatcher_add_view(
        app->view_dispatcher, MyAppViewAbout, widget_get_view(app->widget_about));

    app->notifications = furi_record_open(RECORD_NOTIFICATION);

#ifdef BACKLIGHT_ALWAYS_ON
    notification_message(app->notifications, &sequence_display_backlight_enforce_on);
#endif

    return app;
}

void my_app_free(MyApp* app) {
#ifdef BACKLIGHT_ALWAYS_ON
    notification_message(app->notifications, &sequence_display_backlight_enforce_auto);
#endif
    furi_record_close(RECORD_NOTIFICATION);

    view_dispatcher_remove_view(app->view_dispatcher, MyAppViewAbout);
    widget_free(app->widget_about);
    view_dispatcher_remove_view(app->view_dispatcher, MyAppViewFlipTheWorld);
    view_free(app->view_flip_the_world);
    view_dispatcher_remove_view(app->view_dispatcher, MyAppViewConfigure);
    variable_item_list_free(app->variable_item_list_config);
    view_dispatcher_remove_view(app->view_dispatcher, MyAppViewSubmenu);
    submenu_free(app->submenu);
    view_dispatcher_free(app->view_dispatcher);
    furi_record_close(RECORD_GUI);

    free(app);
}

int32_t my_app_name_app(void* p) {
    UNUSED(p);

    MyApp* app = my_app_alloc();
    view_dispatcher_run(app->view_dispatcher);

    my_app_free(app);
    return 0;
}