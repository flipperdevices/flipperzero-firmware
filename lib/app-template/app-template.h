#pragma once
#include "CallbackConnector.h"
#include "flipper.h"
#include "flipper_v2.h"

// simple app class with template variables <state, events>
template <class TState, class TEvents> class AppTemplate {
public:
    AppTemplate();
    ~AppTemplate();
    void input_callback(InputEvent* input_event, void* ctx);
    void draw_callback(CanvasApi* canvas, void* ctx);
    virtual void render(CanvasApi* canvas, TState* state) = 0;
    Widget* widget;
    osMessageQueueId_t event_queue;
    TState state;
    ValueMutex state_mutex;
    GuiApi* gui;

    void acquire_state(void);
    void release_state(void);
};

template <class TState, class TEvents> AppTemplate<TState, TEvents>::AppTemplate() {
    // allocate events queue
    event_queue = osMessageQueueNew(10, sizeof(TEvents), NULL);

    // allocate valuemutex
    // TODO: use plain os mutex?
    if(!init_mutex(&state_mutex, &state, sizeof(TState))) {
        printf("cannot create mutex\n");
        furiac_exit(NULL);
    }

    // allocate widget
    widget = widget_alloc();

    // connect widget with input callback
    auto input_cb_ref = cbc::obtain_connector(this, &AppTemplate::input_callback);
    widget_input_callback_set(widget, input_cb_ref, this);

    // connect widget with draw callback
    auto draw_cb_ref = cbc::obtain_connector(this, &AppTemplate::draw_callback);
    widget_draw_callback_set(widget, draw_cb_ref, this);

    // open gui and add widget
    gui = (GuiApi*)furi_open("gui");
    if(gui == NULL) {
        printf("gui is not available\n");
        furiac_exit(NULL);
    }
    gui->add_widget(gui, widget, GuiLayerFullscreen);
}

template <class TState, class TEvents> AppTemplate<TState, TEvents>::~AppTemplate() {
}

// generic input callback
template <class TState, class TEvents>
void AppTemplate<TState, TEvents>::input_callback(InputEvent* input_event, void* ctx) {
    AppTemplate* app = static_cast<AppTemplate*>(ctx);

    TEvents event;
    event.type = TEvents::EventTypeKey;
    event.value.input = *input_event;
    osMessageQueuePut(app->event_queue, &event, 0, 0);
}

// generic draw callback
template <class TState, class TEvents>
void AppTemplate<TState, TEvents>::draw_callback(CanvasApi* canvas, void* ctx) {
    AppTemplate* app = static_cast<AppTemplate*>(ctx);
    app->acquire_state();

    canvas->clear(canvas);
    app->render(canvas, &state);

    app->release_state();
}

template <class TState, class TEvents> void AppTemplate<TState, TEvents>::acquire_state(void) {
    acquire_mutex(&state_mutex, osWaitForever);
}

template <class TState, class TEvents> void AppTemplate<TState, TEvents>::release_state(void) {
    release_mutex(&state_mutex, &state);
}