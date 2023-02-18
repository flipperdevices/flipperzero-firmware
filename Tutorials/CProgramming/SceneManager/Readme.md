# Using the Scene Manager and View Dispatcher

The Flipper Zero Framework contains a useful library that allows you to split different parts of your application into scenes. Each scene can then render its own views. <br>
This document is not complete, I have a hard time understanding this whole concept myself. Just view it as a set of hints to help you figure things out yourself.<br>

## Benefits
- Helps prevent bloated code<br>
- Allows the use of independent modules<br>
- Handles Mutex operations<br>
- Eliminates the need a loop to keep the application alive<br>
- Handles different events (user input, draw, tick, enter, exit)<br>
<br>
While it is not very easy to add scenes/views, it is very benefitial to the maintainability of your code once it works. I recommend comparing your code with an existing example<br>
when working on this. (use my Game color_guess, or use the Firmwares SubGhz Application as reference)

## Includes required

```
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
```

## Main Application Class 
Your application should have a main class, often referred to as app or instance. <br>
This class contain attributes pointing to <br>
Gui, NotificationApp, FuriMessageQueue, ViewPort, ViewDispatcher and SceneManager<br>

```
typedef struct {
    Gui* gui;
    NotificationApp* notification;
    FuriMessageQueue* event_queue;
    ViewPort* view_port;
    ViewDispatcher* view_dispatcher;
    SceneManager* scene_manager;
} MyMainClass;
```

## Basic Scene/View Setup
Scenes are seperate code blocks used in different situations in an application. A scene normally also triggers a view. The view is more responsible for <br>
rendering on screen and processing user input. The scene on the other side is more responsible for code logic of that programm section. <br>
<br>
In your main programm, when allocating your applications resources, be sure to also allocate your scenes <br>

```
    //Scene additions
    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->view_dispatcher);

    app->scene_manager = scene_manager_alloc(&color_guess_scene_handlers, app);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_navigation_event_callback(app->view_dispatcher, color_guess_navigation_event_callback);
    view_dispatcher_set_tick_event_callback(app->view_dispatcher, color_guess_tick_event_callback, 100);
    view_dispatcher_set_custom_event_callback(app->view_dispatcher, color_guess_custom_event_callback);
    app->submenu = submenu_alloc();

    view_dispatcher_add_view(app->view_dispatcher, ColorGuessViewIdMenu, submenu_get_view(app->submenu));
    app->color_guess_color_set = color_guess_color_set_alloc();
    view_dispatcher_add_view(app->view_dispatcher, ColorGuessViewIdColorSet, color_guess_color_set_get_view(app->color_guess_color_set));
    app->color_guess_play = color_guess_play_alloc();
    view_dispatcher_add_view(app->view_dispatcher, ColorGuessViewIdPlay, color_guess_play_get_view(app->color_guess_play));
```

In this code we create the view_dispatcher and the scene_manager. We set callback events to trigger when an event is done. Every single view we will be using is added to the view_dispatcher here. <br>

## Scene list and events
While the views were directly added, scenes get setup differently. I don't yet understand why this is done so complicated, but here are my findings. <br>
<br>
We add a C file and Header File to define the different patterns used in event handlers (example from color_guess game)<br>

```
#include "color_guess_scene.h"

// Generate scene on_enter handlers array
#define ADD_SCENE(prefix, name, id) prefix##_scene_##name##_on_enter,
void (*const color_guess_on_enter_handlers[])(void*) = {
#include "color_guess_scene_config.h"
};
#undef ADD_SCENE

// Generate scene on_event handlers array
#define ADD_SCENE(prefix, name, id) prefix##_scene_##name##_on_event,
bool (*const color_guess_on_event_handlers[])(void* context, SceneManagerEvent event) = {
#include "color_guess_scene_config.h"
};
#undef ADD_SCENE

// Generate scene on_exit handlers array
#define ADD_SCENE(prefix, name, id) prefix##_scene_##name##_on_exit,
void (*const color_guess_on_exit_handlers[])(void* context) = {
#include "color_guess_scene_config.h"
};
#undef ADD_SCENE

// Initialize scene handlers configuration structure
const SceneManagerHandlers color_guess_scene_handlers = {
    .on_enter_handlers = color_guess_on_enter_handlers,
    .on_event_handlers = color_guess_on_event_handlers,
    .on_exit_handlers = color_guess_on_exit_handlers,
    .scene_num = ColorGuessSceneNum,
};
```

This defines the name patterns we will be using inside our scene classes. it also includes the scene config, a listing of all scenes we will be using this on. <br>
<br>
that means we will be required to have the following functions defined inside each scene<br>
`AppName_scene_SceneName_on_event(void* context, SceneManagerEvent event)`<br>
`AppName_scene_SceneName_on_enter(void* context)`<br>
`AppName_scene_SceneName_on_exit(void* context)`<br>
<br>
Apart from that the scene should also provide a funcion to setup callbacks, this is used later in the views<br>
<br>

```
void color_guess_color_set_callback(ColorGuessCustomEvent event, void* context) {
    furi_assert(context);
    ColorGuess* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, event);
}
``` 
<br>

## Definition of Scenes used
scenes/color_guess_scene_config.h<br>

```
ADD_SCENE(color_guess, start, Start)
ADD_SCENE(color_guess, color_set, ColorSet)
ADD_SCENE(color_guess, play, Play)
```


## Activating the view dispatcher in the scene
Inside a scene, you should trigger the view dispatcher. <br>
<br>
When action taken to leave the event<br>

```
if(!scene_manager_search_and_switch_to_previous_scene(
    app->scene_manager, ColorGuessSceneStart)) {
        scene_manager_stop(app->scene_manager);
        view_dispatcher_stop(app->view_dispatcher);
    }
```
<br>
When launching a view<br>

```
view_dispatcher_switch_to_view(app->view_dispatcher, ColorGuessViewIdColorSet);
```
ColorGuessViewIdColorSet being a enum integer

## Using a View

A view should always be connected with a model (defines states used in view)

```
typedef struct {
    ColorGuessPlayStatus status;
    int cursorpos;
    int digit[6];
    int color;
    int time_spent;
    int timestamp_start;
    int prev_closeness;
    int closeness;
    int difficulty;
    int success;
} ColorGuessPlayModel;
```

Also a view should setup a callback that should be triggered on_enter in the scene<br>

```
void color_guess_play_set_callback(
    ColorGuessPlay* instance,
    ColorGuessPlayCallback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(callback);
    instance->callback = callback;
    instance->context = context;
}
```

Upon allocating the resources of a view, you should also define the callbacks used <br>

```
ColorGuessPlay* color_guess_play_alloc() {
    ColorGuessPlay* instance = malloc(sizeof(ColorGuessPlay));
    instance->view = view_alloc();
    view_allocate_model(instance->view, ViewModelTypeLocking, sizeof(ColorGuessPlayModel));
    view_set_context(instance->view, instance); // furi_assert crashes in events without this
    view_set_draw_callback(instance->view, (ViewDrawCallback)color_guess_play_draw);
    view_set_input_callback(instance->view, color_guess_play_input);
    view_set_enter_callback(instance->view, color_guess_play_enter);
    //view_set_exit_callback(instance->view, color_guess_play_exit);

    with_view_model(
        instance->view,
        ColorGuessPlayModel * model,
        {
            color_guess_play_model_init(model);
        },
        true
    );
    
    return instance;
}
```
Be careful not to forget the view_set_context() call. Without your events will cause Flipper to crash<br>

The command with_view_model() let's you execute code using the model attached to the view. 