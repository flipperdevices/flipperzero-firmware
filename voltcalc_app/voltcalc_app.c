#include <furi.h>

#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/widget.h>
#include <gui/modules/submenu.h>
#include <gui/modules/text_input.h>

typedef enum {
	VoltcalcScenesMainScene,
	VoltcalcScenesInputScene,
	VoltcalcScenesCalculationsScene,
	VoltcalcScenesAboutScene,
	VoltcalcScenesSceneCount,
} VoltcalcScenesScene;

typedef enum {
	VoltcalcScenesMainView,
	VoltcalcScenesWidgetView,
	VoltcalcScenesTextInputView,
} VoltcalcScenesView;

typedef struct App {
	SceneManager* scene_manager;
	ViewDispatcher* view_dispatcher;
	Widget* widget;
	TextInput* text_input;
} App;


/* Menu stubs */
void voltcalc_scenes_menu_callback(void* context, uint32_t index) {
    UNUSED(context);
    UNUSED(index);
}

/* Scene stubs */
void voltcalc_scenes_main_scene_on_enter(void* context) {
	UNUSED (context);
}

bool voltcalc_scenes_main_scene_on_event(void* context, SceneManagerEvent event) {
	UNUSED (context);
	UNUSED (event);
	return false; // event not handled
}

void voltcalc_scenes_main_on_exit(void* context) {
	UNUSED (context);
}

void voltcalc_scenes_input_scene_on_enter(void* context) {
        UNUSED (context);
}

bool voltcalc_scenes_input_scene_on_event(void* context, SceneManagerEvent event) {
        UNUSED (context);
        UNUSED (event);
        return false; // event not handled
}

void voltcalc_scenes_input_on_exit(void* context) {
        UNUSED (context);
}

void voltcalc_scenes_calculations_scene_on_enter(void* context) {
        UNUSED (context);
}

bool voltcalc_scenes_calculations_scene_on_event(void* context, SceneManagerEvent event) {
        UNUSED (context);
        UNUSED (event);
        return false; // event not handled
}

void voltcalc_scenes_calculations_on_exit(void* context) {
        UNUSED (context);
}

void voltcalc_scenes_about_scene_on_enter(void* context) {
        UNUSED (context);
}

bool voltcalc_scenes_about_scene_on_event(void* context, SceneManagerEvent event) {
        UNUSED (context);
        UNUSED (event);
        return false; // event not handled
}

void voltcalc_scenes_about_on_exit(void* context) {
        UNUSED (context);
}

/*
void voltcalc_scenes_XXX_scene_on_enter(void* context) {
        UNUSED (context);
}

bool voltcalc_scenes_XXX_scene_on_event(void* context, SceneManagerEvent event) {
        UNUSED (context);
        UNUSED (event);
        return false; // event not handled
}

void voltcalc_scenes_XXX_on_exit(void* context) {
        UNUSED (context);
}
*/


/* on_enter handlers */
void (*const voltcalc_scenes_scene_on_enter_handlers[])(void*) = {
    voltcalc_scenes_main_scene_on_enter,
    voltcalc_scenes_input_scene_on_enter,
    voltcalc_scenes_calculations_scene_on_enter,
    voltcalc_scenes_about_scene_on_enter,
};

/* on_event handlers */
void (*const voltcalc_scenes_scene_on_event_handlers[])(void*) = {
    voltcalc_scenes_main_scene_on_event,
    voltcalc_scenes_input_scene_on_event,
    voltcalc_scenes_calculations_scene_on_event,
    voltcalc_scenes_about_scene_on_event,
};

/* on_exit handlers */
void (*const voltcalc_scenes_scene_on_exit_handlers[])(void*) = {
    voltcalc_scenes_main_scene_on_exit,
    voltcalc_scenes_input_scene_on_exit,
    voltcalc_scenes_calculations_scene_on_exit,
    voltcalc_scenes_about_scene_on_exit,
};

/* Scene Manager callback */
static const SceneManagerHandlers voltcalc_scenes_scene_manager_handlers = {
    .on_enter_handlers = voltcalc_scenes_scene_on_enter_handlers,
    .on_event_handlers = voltcalc_scenes_scene_on_event_handlers,
    .on_exit_handlers = voltcalc_scenes_scene_on_exit_handlers,
    .scene_num = VoltcalcScenesSceneCount,
};

/* app scene_custom_callback */
static bool voltcalc_scene_custom_callback(void* context, uint32_t custom_event) {
    furi_assert(context);
    App* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, custom_event);
}

/* scene_back_event */
bool basic_scene_back_event_callback(void* context) {
    furi_assert(context);
    App* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

static App* app_alloc() {
    App* app = malloc(sizeof(App));
    app->scene_manager = scene_manager_alloc(
      &voltcalc_scenes_scene_manager_handlers,
      app);
    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_custom_event_callback(app->view_dispatcher, voltcalc_scene_custom_callback);
    view_dispatcher_set_navigation_event_callback(
        app->view_dispatcher, voltcalc_scene_back_event_callback);
 /* app->submenu = submenu_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        BasicScenesSubmenuView,
        submenu_get_view(app->submenu)); */
    app->main = 
    app->widget = widget_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        VoltcalcScenesWidgetView,
        widget_get_view(app->widget));
    app->text_input = text_input_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        VoltcalcScenesTextInputView,
        text_input_get_view(app->text_input));
    return app;
}

/* free resources */
static void app_free(App* app) {
    furi_assert(app);
    view_dispatcher_remove_view(app->view_dispatcher, VoltcalcScenesMainView);
    view_dispatcher_remove_view(app->view_dispatcher, VoltcalcScenesWidgetView);
    view_dispatcher_remove_view(app->view_dispatcher, VoltcalcScenesTextInputView);
    scene_manager_free(app->scene_manager);
    view_dispatcher_free(app->view_dispatcher);
    submenu_free(app->submenu);
    widget_free(app->widget);
    text_input_free(app->text_input);
    free(app);
}



int32_t voltcalc_app(void* p) {
    UNUSED(p);
    App* app = app_alloc();

    Gui* gui = furi_record_open(RECORD_GUI);
    view_dispatcher_attach_to_gui(
      app->view_dispatcher,
      gui,
      ViewDispatcherTypeFullscreen);
    scene_manager_next_scene(
      app->scene_manager,
      VoltcalcScenesMainScene);
    view_dispatcher_run(app->view_dispatcher);

    app_free(app);
    return 0;
}
