#include <furi.h>
#include "dolphin_scene/dolphin_scene.h"

void dolphin_engine_tick_cb(void* p) {
    osMessageQueueId_t event_queue = p;
    AppEvent tick_event;
    tick_event.type = EventTypeTick;
    osMessageQueuePut(event_queue, (void*)&tick_event, 0, 0);
}

static void dolphin_engine_event_cb(InputEvent* input_event, void* ctx) {
    osMessageQueueId_t event_queue = ctx;

    AppEvent event;
    event.type = EventTypeKey;
    event.value.input = *input_event;
    osMessageQueuePut(event_queue, (void*)&event, 0, osWaitForever);
}

ValueMutex* scene_init() {
    SceneState* scene = furi_alloc(sizeof(SceneState));
    scene->player.y = DOLPHIN_DEFAULT_Y;
    scene->player.x = DOLPHIN_CENTER;

    scene->screen.x = scene->player.x;
    scene->screen.y = scene->player.y;

    scene->ui.mqueue = osMessageQueueNew(2, sizeof(AppEvent), NULL);

    ValueMutex* scene_mutex = furi_alloc(sizeof(ValueMutex));
    if(scene_mutex == NULL || !init_mutex(scene_mutex, scene, sizeof(SceneState))) {
        printf("[menu_task] cannot create menu mutex\r\n");
        furi_check(0);
    }

    // Open GUI and register view_port
    scene->ui.gui = furi_record_open("gui");

    // Allocate and configure view_port
    scene->ui.view_port = view_port_alloc();

    // Open GUI and register fullscreen view_port
    gui_add_view_port(scene->ui.gui, scene->ui.view_port, GuiLayerFullscreen);

    view_port_draw_callback_set(scene->ui.view_port, scene_redraw, scene_mutex);
    view_port_input_callback_set(scene->ui.view_port, dolphin_engine_event_cb, scene->ui.mqueue);
    view_port_enabled_set(scene->ui.view_port, false);

    scene->ui.timer = osTimerNew(dolphin_engine_tick_cb, osTimerPeriodic, scene->ui.mqueue, NULL);

    return scene_mutex;
}

int32_t dolphin_scene(void* p) {
    ValueMutex* state_mutex = scene_init();

    furi_record_create("scene", state_mutex);

    SceneState* _state = (SceneState*)acquire_mutex_block(state_mutex);

    osTimerStart(_state->ui.timer, 40);

    uint32_t t = xTaskGetTickCount();
    uint32_t prev_t = 0;

    osMessageQueueId_t q = _state->ui.mqueue;
    release_mutex(state_mutex, _state);
    while(1) {
        AppEvent event;
        if(osMessageQueueGet(q, &event, 0, osWaitForever) == osOK) {
            SceneState* _state = (SceneState*)acquire_mutex_block(state_mutex);
            if(event.type == EventTypeTick) {
                t = xTaskGetTickCount();
                tick_handler(_state, t, (t - prev_t) % 1024);
                prev_t = t;
            } else if(event.type == EventTypeKey) {
                if(event.value.input.key == InputKeyBack &&
                   event.value.input.type == InputTypeShort) {
                    view_port_enabled_set(_state->ui.view_port, false);
                } else {
                    handle_input(_state, &event.value.input);
                }
            }
            release_mutex(state_mutex, _state);
            view_port_update(_state->ui.view_port);
        }
    }
    /*
    view_port_enabled_set(_state->view_port, false);
    gui_remove_view_port(_state->gui, _state->view_port);
    view_port_free(_state->view_port);
    osMessageQueueDelete(_state->mqueue);
    osTimerDelete(_state->timer);
    */
    return 0;
}
