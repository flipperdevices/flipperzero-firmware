#include <furi.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/widget.h>
#include <gui/modules/submenu.h>
#include <gui/modules/text_input.h>

typedef enum {
    BasicScenesMainMenuScene,
    BasicScenesLotteryScene,
    BasicScenesGreetingInputScene,
    BasicScenesGreetingMessageScene,
    BasicScenesSceneCount,
} BasicScenesScene;

typedef enum {
    BasicScenesSubmenuView,
    BasicScenesWidgetView,
    BasicScenesTextInputView,
} BasicScenesView;

typedef struct App {
    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;
    Submenu* submenu;
    Widget* widget;
    TextInput* text_input;
    char* user_name;
    uint8_t user_name_size;
} App;

typedef enum {
    BasicScenesMainMenuSceneLottoNumbers,
    BasicScenesMainMenuSceneGreeting,
} BasicScenesMainMenuSceneIndex;

typedef enum {
    BasicScenesMainMenuSceneLottoNumbersEvent,
    BasicScenesMainMenuSceneGreetingEvent,
} BasicScenesMainMenuEvent;

typedef enum {
    BasicScenesGreetingInputSceneSaveEvent,
} BasicScenesGreetingInputEvent;

void basic_scenes_menu_callback(void* context, uint32_t index) {
    App* app = context;
    switch(index) {
    case BasicScenesMainMenuSceneLottoNumbers:
        scene_manager_handle_custom_event(
            app->scene_manager, BasicScenesMainMenuSceneLottoNumbersEvent);
        break;
    case BasicScenesMainMenuSceneGreeting:
        scene_manager_handle_custom_event(
            app->scene_manager, BasicScenesMainMenuSceneGreetingEvent);
        break;
    }
}

void basic_scenes_main_menu_scene_on_enter(void* context) {
    App* app = context;
    submenu_reset(app->submenu);
    submenu_set_header(app->submenu, "Basic Scenes Demo");
    submenu_add_item(
        app->submenu,
        "Lotto Numbers",
        BasicScenesMainMenuSceneLottoNumbers,
        basic_scenes_menu_callback,
        app);
    submenu_add_item(
        app->submenu,
        "Greeting",
        BasicScenesMainMenuSceneGreeting,
        basic_scenes_menu_callback,
        app);
    view_dispatcher_switch_to_view(app->view_dispatcher, BasicScenesSubmenuView);
}
bool basic_scenes_main_menu_scene_on_event(void* context, SceneManagerEvent event) {
    App* app = context;
    bool consumed = false;
    switch(event.type) {
    case SceneManagerEventTypeCustom:
        switch(event.event) {
        case BasicScenesMainMenuSceneLottoNumbersEvent:
            scene_manager_next_scene(app->scene_manager, BasicScenesLotteryScene);
            consumed = true;
            break;
        case BasicScenesMainMenuSceneGreetingEvent:
            scene_manager_next_scene(app->scene_manager, BasicScenesGreetingInputScene);
            consumed = true;
            break;
        }
        break;
    default:
        break;
    }
    return consumed;
}

void basic_scenes_main_menu_scene_on_exit(void* context) {
    App* app = context;
    submenu_reset(app->submenu);
}

void basic_scenes_lottery_scene_on_enter(void* context) {
    App* app = context;
    widget_reset(app->widget);
    widget_add_string_element(
        app->widget, 25, 15, AlignLeft, AlignCenter, FontPrimary, "Lotto numbers:");
    widget_add_string_element(
        app->widget, 30, 35, AlignLeft, AlignCenter, FontBigNumbers, "0 4 2");
    view_dispatcher_switch_to_view(app->view_dispatcher, BasicScenesWidgetView);
}
bool basic_scenes_lottery_scene_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false; // event not handled.
}
void basic_scenes_lottery_scene_on_exit(void* context) {
    UNUSED(context);
}

void basic_scenes_text_input_callback(void* context) {
    App* app = context;
    scene_manager_handle_custom_event(app->scene_manager, BasicScenesGreetingInputSceneSaveEvent);
}

void basic_scenes_greeting_input_scene_on_enter(void* context) {
    App* app = context;
    bool clear_text = true;
    text_input_reset(app->text_input);
    text_input_set_header_text(app->text_input, "Enter your name");
    text_input_set_result_callback(
        app->text_input,
        basic_scenes_text_input_callback,
        app,
        app->user_name,
        app->user_name_size,
        clear_text);
    view_dispatcher_switch_to_view(app->view_dispatcher, BasicScenesTextInputView);
}
bool basic_scenes_greeting_input_scene_on_event(void* context, SceneManagerEvent event) {
    App* app = context;
    bool consumed = false;
    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == BasicScenesGreetingInputSceneSaveEvent) {
            scene_manager_next_scene(app->scene_manager, BasicScenesGreetingMessageScene);
            consumed = true;
        }
    }
    return consumed;
}
void basic_scenes_greeting_input_scene_on_exit(void* context) {
    UNUSED(context);
}

void basic_scenes_greeting_message_scene_on_enter(void* context) {
    App* app = context;
    widget_reset(app->widget);
    FuriString* message = furi_string_alloc();
    furi_string_printf(message, "Hello,\n%s!", app->user_name);
    widget_add_string_multiline_element(
        app->widget, 5, 15, AlignLeft, AlignCenter, FontPrimary, furi_string_get_cstr(message));
    furi_string_free(message);
    view_dispatcher_switch_to_view(app->view_dispatcher, BasicScenesWidgetView);
}
bool basic_scenes_greeting_message_scene_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false; // event not handled.
}
void basic_scenes_greeting_message_scene_on_exit(void* context) {
    App* app = context;
    widget_reset(app->widget);
}

void (*const basic_scenes_scene_on_enter_handlers[])(void*) = {
    basic_scenes_main_menu_scene_on_enter,
    basic_scenes_lottery_scene_on_enter,
    basic_scenes_greeting_input_scene_on_enter,
    basic_scenes_greeting_message_scene_on_enter,
};

bool (*const basic_scenes_scene_on_event_handlers[])(void*, SceneManagerEvent) = {
    basic_scenes_main_menu_scene_on_event,
    basic_scenes_lottery_scene_on_event,
    basic_scenes_greeting_input_scene_on_event,
    basic_scenes_greeting_message_scene_on_event,
};

void (*const basic_scenes_scene_on_exit_handlers[])(void*) = {
    basic_scenes_main_menu_scene_on_exit,
    basic_scenes_lottery_scene_on_exit,
    basic_scenes_greeting_input_scene_on_exit,
    basic_scenes_greeting_message_scene_on_exit,
};

static const SceneManagerHandlers basic_scenes_scene_manager_handlers = {
    .on_enter_handlers = basic_scenes_scene_on_enter_handlers,
    .on_event_handlers = basic_scenes_scene_on_event_handlers,
    .on_exit_handlers = basic_scenes_scene_on_exit_handlers,
    .scene_num = BasicScenesSceneCount,
};

static bool basic_scene_custom_callback(void* context, uint32_t custom_event) {
    furi_assert(context);
    App* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, custom_event);
}

bool basic_scene_back_event_callback(void* context) {
    furi_assert(context);
    App* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

static App* app_alloc() {
    App* app = malloc(sizeof(App));
    app->user_name_size = 16;
    app->user_name = malloc(app->user_name_size);
    app->scene_manager = scene_manager_alloc(&basic_scenes_scene_manager_handlers, app);
    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_custom_event_callback(app->view_dispatcher, basic_scene_custom_callback);
    view_dispatcher_set_navigation_event_callback(
        app->view_dispatcher, basic_scene_back_event_callback);
    app->submenu = submenu_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, BasicScenesSubmenuView, submenu_get_view(app->submenu));
    app->widget = widget_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, BasicScenesWidgetView, widget_get_view(app->widget));
    app->text_input = text_input_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, BasicScenesTextInputView, text_input_get_view(app->text_input));
    return app;
}

static void app_free(App* app) {
    furi_assert(app);
    view_dispatcher_remove_view(app->view_dispatcher, BasicScenesSubmenuView);
    view_dispatcher_remove_view(app->view_dispatcher, BasicScenesWidgetView);
    view_dispatcher_remove_view(app->view_dispatcher, BasicScenesTextInputView);
    scene_manager_free(app->scene_manager);
    view_dispatcher_free(app->view_dispatcher);
    submenu_free(app->submenu);
    widget_free(app->widget);
    text_input_free(app->text_input);
    free(app);
}

int32_t basic_scenes_app(void* p) {
    UNUSED(p);
    App* app = app_alloc();

    Gui* gui = furi_record_open(RECORD_GUI);
    view_dispatcher_attach_to_gui(app->view_dispatcher, gui, ViewDispatcherTypeFullscreen);
    scene_manager_next_scene(app->scene_manager, BasicScenesMainMenuScene);
    view_dispatcher_run(app->view_dispatcher);

    app_free(app);
    return 0;
}
