#include <furi.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/submenu.h>

typedef enum { scene1 } Scenes;

typedef enum { view1 } Views;

typedef struct App {
    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;
    Submenu* submenu;
} App;

typedef enum { Option1, Option2 } MenuOptions;

void test1_menu_callback(void* context, uint32_t index) {
    UNUSED(context);
    if(index == 0)
        furi_hal_gpio_write(&gpio_ext_pc1, true);
    else
        furi_hal_gpio_write(&gpio_ext_pc1, false);
}

void test1_main_menu_scene_on_enter(void* context) {
    App* app = context;
    submenu_reset(app->submenu);
    submenu_set_header(app->submenu, "Electronic Cats");
    submenu_add_item(app->submenu, "ON", Option1, test1_menu_callback, app);
    submenu_add_item(app->submenu, "OFF", Option2, test1_menu_callback, app);
    furi_hal_gpio_write(&gpio_ext_pc1, false);
    view_dispatcher_switch_to_view(app->view_dispatcher, view1);
}

bool test1_main_menu_scene_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void test1_main_menu_scene_on_exit(void* context) {
    App* app = context;
    submenu_reset(app->submenu);
}

void (*const test1_scene_on_enter_handlers[])(void*) = {
    test1_main_menu_scene_on_enter,
};

bool (*const test1_scene_on_event_handlers[])(void*, SceneManagerEvent) = {
    test1_main_menu_scene_on_event,
};

void (*const test1_scene_on_exit_handlers[])(void*) = {
    test1_main_menu_scene_on_exit,
};

static const SceneManagerHandlers test1_scene_manager_handlers = {
    .on_enter_handlers = test1_scene_on_enter_handlers,
    .on_event_handlers = test1_scene_on_event_handlers,
    .on_exit_handlers = test1_scene_on_exit_handlers,
    .scene_num = 1,
};

bool basic_scene_back_event_callback(void* context) {
    furi_assert(context);
    App* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

static App* app_alloc() {
    App* app = malloc(sizeof(App));
    app->scene_manager = scene_manager_alloc(&test1_scene_manager_handlers, app);
    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_navigation_event_callback(
        app->view_dispatcher, basic_scene_back_event_callback);
    app->submenu = submenu_alloc();
    view_dispatcher_add_view(app->view_dispatcher, view1, submenu_get_view(app->submenu));
    return app;
}

static void app_free(App* app) {
    furi_assert(app);
    view_dispatcher_remove_view(app->view_dispatcher, view1);
    scene_manager_free(app->scene_manager);
    view_dispatcher_free(app->view_dispatcher);
    submenu_free(app->submenu);
    free(app);
}

int32_t test_app(void* p) {
    UNUSED(p);
    furi_hal_gpio_init_simple(&gpio_ext_pc1, GpioModeOutputPushPull);
    App* app = app_alloc();
    Gui* gui = furi_record_open(RECORD_GUI);
    view_dispatcher_attach_to_gui(app->view_dispatcher, gui, ViewDispatcherTypeFullscreen);
    scene_manager_next_scene(app->scene_manager, scene1);
    view_dispatcher_run(app->view_dispatcher);

    furi_hal_gpio_init_simple(&gpio_ext_pc1, GpioModeAnalog);
    app_free(app);
    return 0;
}