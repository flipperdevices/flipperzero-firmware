#include <furi.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/submenu.h>

typedef enum { scene1, scene2, scene3 } Scenes;

typedef enum { view1 } Views;

typedef struct App {
    SceneManager* sceneManager;
    ViewDispatcher* viewDispatcher;
    Submenu* submenu;
} App;

void ChangeScene(void* context, uint32_t index) {
    App* app = context;
    switch(index) {
    case scene1:
        scene_manager_next_scene(app->sceneManager, scene1);
        break;
    case scene2:
        scene_manager_next_scene(app->sceneManager, scene2);
        break;
    case scene3:
        scene_manager_next_scene(app->sceneManager, scene3);
        break;
    default:
        break;
    }
}

void scene1_onEnter(void* context) {
    App* app = context;
    submenu_reset(app->submenu);
    submenu_set_header(app->submenu, "The Scene1");
    submenu_add_item(app->submenu, "Scene2", scene2, ChangeScene, app);
    submenu_add_item(app->submenu, "Scene3", scene3, ChangeScene, app);
    furi_hal_gpio_write(&gpio_ext_pa7, true);
}

bool scene1_onEvent(void* contex, SceneManagerEvent event) {
    UNUSED(contex);
    UNUSED(event);
    return false;
}

void scene1_onExit(void* contex) {
    UNUSED(contex);
    furi_hal_gpio_write(&gpio_ext_pa7, false);
}

void scene2_onEnter(void* context) {
    App* app = context;
    submenu_reset(app->submenu);
    submenu_set_header(app->submenu, "The Scene2");
    submenu_add_item(app->submenu, "Scene1", scene1, ChangeScene, app);
    submenu_add_item(app->submenu, "Scene3", scene3, ChangeScene, app);
    furi_hal_gpio_write(&gpio_ext_pa6, true);
}

bool scene2_onEvent(void* contex, SceneManagerEvent event) {
    UNUSED(contex);
    UNUSED(event);
    return false;
}

void scene2_onExit(void* contex) {
    UNUSED(contex);
    furi_hal_gpio_write(&gpio_ext_pa6, false);
}

void scene3_onEnter(void* context) {
    App* app = context;
    submenu_reset(app->submenu);
    submenu_set_header(app->submenu, "The Scene3");
    submenu_add_item(app->submenu, "Scene1", scene1, ChangeScene, app);
    submenu_add_item(app->submenu, "Scene2", scene2, ChangeScene, app);
    furi_hal_gpio_write(&gpio_ext_pa4, true);
}

bool scene3_onEvent(void* contex, SceneManagerEvent event) {
    UNUSED(contex);
    UNUSED(event);
    return false;
}

void scene3_onExit(void* contex) {
    UNUSED(contex);
    furi_hal_gpio_write(&gpio_ext_pa4, false);
}

void (*const onEnterHandlers[])(void*) = {scene1_onEnter, scene2_onEnter, scene3_onEnter};

bool (*const onEventHandlers[])(void*, SceneManagerEvent) = {
    scene1_onEvent,
    scene2_onEvent,
    scene3_onEvent};
void (*const onExitHandlers[])(void*) = {scene1_onExit, scene2_onExit, scene3_onExit};

static const SceneManagerHandlers sceneHandlers = {
    .on_enter_handlers = onEnterHandlers,
    .on_event_handlers = onEventHandlers,
    .on_exit_handlers = onExitHandlers,
    .scene_num = 3};

bool back_event_callback(void* context) {
    furi_assert(context);
    App* app = context;
    return scene_manager_handle_back_event(app->sceneManager);
}

static App* appAlloc() {
    App* app = malloc(sizeof(App));
    app->sceneManager = scene_manager_alloc(&sceneHandlers, app);
    app->viewDispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->viewDispatcher);
    view_dispatcher_set_event_callback_context(app->viewDispatcher, app);
    view_dispatcher_set_navigation_event_callback(app->viewDispatcher, back_event_callback);
    app->submenu = submenu_alloc();
    view_dispatcher_add_view(app->viewDispatcher, view1, submenu_get_view(app->submenu));
    return app;
}

static void appFree(App* app) {
    furi_assert(app);
    submenu_reset(app->submenu);
    view_dispatcher_remove_view(app->viewDispatcher, view1);
    submenu_free(app->submenu);
    view_dispatcher_free(app->viewDispatcher);
    scene_manager_free(app->sceneManager);
    free(app);
}

uint32_t test2(void* p) {
    UNUSED(p);
    furi_hal_gpio_init_simple(&gpio_ext_pa7, GpioModeOutputPushPull);
    furi_hal_gpio_init_simple(&gpio_ext_pa6, GpioModeOutputPushPull);
    furi_hal_gpio_init_simple(&gpio_ext_pa4, GpioModeOutputPushPull);
    furi_hal_gpio_write(&gpio_ext_pa7, false);
    furi_hal_gpio_write(&gpio_ext_pa6, false);
    furi_hal_gpio_write(&gpio_ext_pa4, false);
    App* app = appAlloc();
    Gui* gui = furi_record_open(RECORD_GUI);
    view_dispatcher_attach_to_gui(app->viewDispatcher, gui, ViewDispatcherTypeFullscreen);
    scene_manager_next_scene(app->sceneManager, scene1);
    view_dispatcher_switch_to_view(app->viewDispatcher, view1);
    view_dispatcher_run(app->viewDispatcher);

    appFree(app);
    return 0;
}