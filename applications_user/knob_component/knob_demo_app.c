/*
@CodeAllNight
https://github.com/jamisonderek/flipper-zero-tutorials

This project is to learn how to build a custom component (an up-down knob)
for a scene for the Flipper Zero.

TODO: WRITE TUTORIAL ON CREATING THE KNOB COMPONENT.
*/

#include <furi.h>
#include <furi_hal.h>

#include <furi_hal_gpio.h>
#include <furi_hal_resources.h>

#include <gui/gui.h>
#include <locale/locale.h>

#include <gui/scene_manager.h>
#include <gui/view_dispatcher.h>

#define TAG "knob_demo_app"

/////////////////////////////////////////////////////////////////
// Routine for logging messages with a delay.
/////////////////////////////////////////////////////////////////

void message(char* message) {
    FURI_LOG_I(TAG, message);
    furi_delay_ms(10);
}

/////////////////////////////////////////////////////////////////
// Knob - Our component with custom input/draw routines
/////////////////////////////////////////////////////////////////

typedef enum {
    KnobEventDone,
} KnobCustomEvents;

typedef void (*KnobCallback)(void* context, uint32_t index);

typedef struct Knob {
    View* view;
} Knob;

typedef struct KnobModel {
    FuriString* buffer;
    uint16_t counter;
    char* heading;
    KnobCallback callback;
    void* callback_context;
} KnobModel;

// Set a callback to invoke when knob has an event.
// @knob is a pointer to our Knob instance.
// @callback is a function to invoke when we have custom events.
static void knob_set_callback(Knob* knob, KnobCallback callback, void* callback_context) {
    with_view_model(
        knob->view,
        KnobModel * model,
        {
            model->callback_context = callback_context;
            model->callback = callback;
        },
        true);
}

// Invoked when input (button press) is detected.
// @input_even is the event the occured.
// @ctx is a pointer to our Knob instance.
static bool knob_input_callback(InputEvent* input_event, void* ctx) {
    message("knob_input_callback");
    Knob* knob = (Knob*)ctx;

    bool handled = false;

    if(input_event->type == InputTypePress && input_event->key == InputKeyUp) {
        bool updated = false;
        with_view_model(
            knob->view,
            KnobModel * model,
            {
                if(model->counter) {
                    model->counter--;
                    updated = true;
                }
            },
            updated);
        handled = true;
    } else if(input_event->type == InputTypePress && input_event->key == InputKeyDown) {
        with_view_model(
            knob->view,
            KnobModel * model,
            { model->counter++; },
            true); // Render new data.
        handled = true;
    } else if(input_event->type == InputTypePress && input_event->key == InputKeyOk) {
        with_view_model(
            knob->view,
            KnobModel * model,
            {
                if(model->callback) {
                    message("invoking callback");
                    model->callback(model->callback_context, KnobEventDone);
                } else {
                    message("no callback set; use knob_set_callback first.");
                }
            },
            false); // No new data.
        handled = true;
    }

    return handled;
}

// Invoked by the draw callback to render the knob.
// @canvas is the canvas to draw on.
// @ctx is our model.
static void knob_render_callback(Canvas* canvas, void* ctx) {
    message("knob_render_callback");
    KnobModel* model = ctx;

    furi_string_printf(model->buffer, "Knob demo %d", model->counter);

    canvas_set_font(canvas, FontPrimary);

    if(model->heading) {
        canvas_draw_str_aligned(canvas, 64, 5, AlignCenter, AlignTop, model->heading);
    }

    canvas_draw_str_aligned(
        canvas, 15, 30, AlignLeft, AlignTop, furi_string_get_cstr(model->buffer));
}

// Allocates a Knob instance.
Knob* knob_alloc() {
    message("knob_alloc");
    Knob* knob = malloc(sizeof(Knob));
    knob->view = view_alloc();

    // context passed to input_callback.
    view_set_context(knob->view, knob);

    // context passed to render.
    view_allocate_model(knob->view, ViewModelTypeLockFree, sizeof(KnobModel));
    with_view_model(
        knob->view,
        KnobModel * model,
        {
            model->buffer = furi_string_alloc();
            model->counter = 0;
            model->heading = NULL;
        },
        true);

    view_set_draw_callback(knob->view, knob_render_callback);
    view_set_input_callback(knob->view, knob_input_callback);
    return knob;
}

// Free a Knob instance.
// @knob pointer to a Knob instance.
void knob_free(Knob* knob) {
    message("knob_free");
    furi_assert(knob);
    with_view_model(
        knob->view, KnobModel * model, { furi_string_free(model->buffer); }, true);
    view_free(knob->view);
    free(knob);
}

// Gets the view associated with our Knob.
// @knob pointer to a Knob instance.
View* knob_get_view(Knob* knob) {
    message("knob_get_view");
    furi_assert(knob);
    return knob->view;
}

// Gets the current counter value for a given Knob instance.
// @knob pointer to a Knob instance.
uint32_t knob_get_counter(Knob* knob) {
    message("knob_get_counter");
    furi_assert(knob);

    uint32_t value = 0;
    with_view_model(
        knob->view, KnobModel * model, { value = model->counter; }, false);

    return value;
}

// Set the counter value for a given Knob instance.
// @knob pointer to a Knob instance.
void knob_set_counter(Knob* knob, uint32_t count) {
    with_view_model(
        knob->view, KnobModel * model, { model->counter = count; }, true);
}

// Sets the heading for displaying our knob.
// @knob pointer to a Knob instance.
// @heading the kind of knob.
void knob_set_heading(Knob* knob, char* heading) {
    with_view_model(
        knob->view, KnobModel * model, { model->heading = heading; }, true);
}

/////////////////////////////////////////////////////////////////
// This is our application.
/////////////////////////////////////////////////////////////////

typedef struct App {
    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;
    Knob* knob;
    float volume;
    float frequency;
} App;

typedef enum {
    AppSceneSetVolume,
    AppSceneSetFrequency,
    AppSceneNum,
} appScene;

typedef enum {
    AppViewSetCounter = 200,
} appViews;

typedef enum {
    AppKnobEventDone,
} AppKnobCustomEvents;

void app_knob_callback(void* context, uint32_t index) {
    message("app_knob_callback");
    App* app = context;
    if(index == KnobEventDone) {
        view_dispatcher_send_custom_event(app->view_dispatcher, AppKnobEventDone);
    }
}

bool app_scene_start_custom_callback(void* context, uint32_t custom_event) {
    message("app_scene_start_custom_callback");
    furi_assert(context);
    App* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, custom_event);
}

bool app_back_event_callback(void* context) {
    furi_assert(context);
    App* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

void app_scene_set_volume_on_enter(void* context) {
    message("app_scene_set_volume_on_enter");
    App* app = context;

    Knob* knob = app->knob;
    knob_set_callback(knob, app_knob_callback, app);
    knob_set_heading(knob, "VOLUME");
    knob_set_counter(knob, (uint32_t)app->volume);

    view_dispatcher_switch_to_view(app->view_dispatcher, AppViewSetCounter);
}

bool app_scene_set_volume_on_event(void* context, SceneManagerEvent event) {
    message("app_scene_set_volume_on_event");
    App* app = context;

    bool consumed = false;
    if(event.type == SceneManagerEventTypeBack) {
        // Back button pressed
    } else if(event.type == SceneManagerEventTypeCustom) {
        consumed = true;
        if(event.event == AppKnobEventDone) {
            FURI_LOG_I(TAG, "Custom Event!");
            uint32_t counter = knob_get_counter(app->knob);
            FURI_LOG_I(TAG, "The counter is %ld.", counter);
            app->volume = counter;

            scene_manager_next_scene(app->scene_manager, AppSceneSetFrequency);
        }
    }
    return consumed;
}

void app_scene_set_volume_on_exit(void* context) {
    message("app_scene_set_volume_on_exit");
    UNUSED(context);
}

void app_scene_set_frequency_on_enter(void* context) {
    message("app_scene_set_frequency_on_enter");
    App* app = context;

    Knob* knob = app->knob;
    knob_set_callback(knob, app_knob_callback, app);
    knob_set_heading(knob, "FREQUENCY");
    knob_set_counter(knob, (uint32_t)app->frequency);

    view_dispatcher_switch_to_view(app->view_dispatcher, AppViewSetCounter);
}

bool app_scene_set_frequency_on_event(void* context, SceneManagerEvent event) {
    message("app_scene_set_frequency_on_event");
    App* app = context;

    bool consumed = false;
    if(event.type == SceneManagerEventTypeBack) {
        // Back button pressed
    } else if(event.type == SceneManagerEventTypeCustom) {
        consumed = true;
        if(event.event == AppKnobEventDone) {
            FURI_LOG_I(TAG, "Custom Event!");
            uint32_t counter = knob_get_counter(app->knob);
            FURI_LOG_I(TAG, "The counter is %ld.", counter);
            app->frequency = counter;
            scene_manager_next_scene(app->scene_manager, AppSceneSetVolume);
        }
    }
    return consumed;
}

void app_scene_set_frequency_on_exit(void* context) {
    message("app_scene_set_frequency_on_exit");
    UNUSED(context);
}

void (*const app_on_enter_handlers[])(void*) = {
    app_scene_set_volume_on_enter,
    app_scene_set_frequency_on_enter,
};

bool (*const app_on_event_handlers[])(void* context, SceneManagerEvent event) = {
    app_scene_set_volume_on_event,
    app_scene_set_frequency_on_event,
};

void (*const app_on_exit_handlers[])(void* context) = {
    app_scene_set_volume_on_exit,
    app_scene_set_frequency_on_exit,
};

const SceneManagerHandlers app_scene_handlers = {
    .on_enter_handlers = app_on_enter_handlers,
    .on_event_handlers = app_on_event_handlers,
    .on_exit_handlers = app_on_exit_handlers,
    .scene_num = AppSceneNum,
};

App* knob_app_alloc() {
    message("knob_app_alloc");
    App* app = malloc(sizeof(App));
    app->frequency = 440;
    app->volume = 50;
    app->scene_manager = scene_manager_alloc(&app_scene_handlers, app);
    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_custom_event_callback(
        app->view_dispatcher, app_scene_start_custom_callback);
    view_dispatcher_set_navigation_event_callback(app->view_dispatcher, app_back_event_callback);
    app->knob = knob_alloc();
    view_dispatcher_add_view(app->view_dispatcher, AppViewSetCounter, knob_get_view(app->knob));

    return app;
}

void knob_app_free(App* app) {
    message("knob_app_free");
    view_dispatcher_remove_view(app->view_dispatcher, AppViewSetCounter);
    knob_free(app->knob);
    view_dispatcher_free(app->view_dispatcher);
    scene_manager_free(app->scene_manager);
    free(app);
}

int32_t knob_demo_app(void* p) {
    UNUSED(p);
    message("knob_demo_app");

    App* app = knob_app_alloc();
    message("knob_app_alloc");

    Gui* gui = furi_record_open(RECORD_GUI);

    view_dispatcher_attach_to_gui(app->view_dispatcher, gui, ViewDispatcherTypeFullscreen);
    message("view_dispatcher_attach_to_gui");

    scene_manager_next_scene(app->scene_manager, AppSceneSetVolume);
    message("scene_manager_next_scene");

    view_dispatcher_run(app->view_dispatcher);
    message("view_dispatcher_run");

    // Free resources
    message("Freeing resources...");
    knob_app_free(app);
    furi_record_close(RECORD_GUI);

    return 0;
}