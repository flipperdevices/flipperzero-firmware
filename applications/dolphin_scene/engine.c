#include <furi.h>
#include "dolphin_scene/dolphin_scene.h"

AppEvent* scene_event_alloc() {
    AppEvent* scene_event = furi_alloc(sizeof(AppEvent));
    scene_event->mqueue = osMessageQueueNew(2, sizeof(AppEvent), NULL);
    furi_check(scene_event->mqueue);

    return scene_event;
}

void dolphin_engine_tick_cb(void* p) {
    osMessageQueueId_t event_queue = p;
    AppEvent tick_event;
    tick_event.type = EventTypeTick;
    osMessageQueuePut(event_queue, (void*)&tick_event, 0, osWaitForever);
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

    scene->event = scene_event_alloc();
    /*SceneState state = {
        .player =
            {
                .x = (SCREEN_WIDTH / 2 - PLAYER_WIDTH / 2) * SCALE,
                .y = (SCREEN_HEIGHT / 2) * SCALE,
            },
        .player_global =
            {
                .x = (SCREEN_WIDTH / 2 - PLAYER_WIDTH / 2) * SCALE,
                .y = (SCREEN_HEIGHT / 2) * SCALE,
            },
        .player_v =
            {
                .x = 0,
                .y = 0,
            },

        .in_boundaries = false,

        .player_anim = 0,

        .glitch_level = 0,
        .glitch_t = 0,
        .poi = 94,
        .action = 1,

    };
    state.screen.x = state.player_global.x - state.player.x;
    state.screen.y = state.player_global.y - state.player.y;
    */

    ValueMutex* scene_mutex = furi_alloc(sizeof(ValueMutex));
    if(scene_mutex == NULL || !init_mutex(scene_mutex, scene, sizeof(SceneState))) {
        printf("[menu_task] cannot create menu mutex\r\n");
        furi_check(0);
    }

    // Open GUI and register view_port
    scene->gui = furi_record_open("gui");

    // Allocate and configure view_port
    scene->view_port = view_port_alloc();

    // Open GUI and register fullscreen view_port
    gui_add_view_port(scene->gui, scene->view_port, GuiLayerFullscreen);

    view_port_draw_callback_set(scene->view_port, scene_redraw, &scene_mutex);
    view_port_input_callback_set(scene->view_port, dolphin_engine_event_cb, scene->event);
    view_port_enabled_set(scene->view_port, false);

    return scene_mutex;
}

int32_t dolphin_scene(void* p) {
    ValueMutex* state_mutex = scene_init();

    SceneState* _state = (SceneState*)acquire_mutex_block(state_mutex);

    osTimerId_t id1 = osTimerNew(dolphin_engine_tick_cb, osTimerPeriodic, _state->event, NULL);
    osTimerStart(id1, 20);

    furi_record_create("scene", state_mutex);

    uint32_t t = xTaskGetTickCount();
    uint32_t prev_t = 0;

    while(1) {
        if(osMessageQueueGet(_state->event, (void*)&_state->event, 0, osWaitForever) == osOK) {
            SceneState* _state = (SceneState*)acquire_mutex_block(state_mutex);

            if(_state->event->type == EventTypeTick) {
                t = xTaskGetTickCount();
                tick_handler(_state, t, (t - prev_t) % 1024);
                prev_t = t;
            } else if(_state->event->type == EventTypeKey) {
                if(_state->event->value.input.key == InputKeyBack &&
                   _state->event->value.input.type == InputTypeLong) {
                    view_port_enabled_set(_state->view_port, false);
                    gui_remove_view_port(_state->gui, _state->view_port);
                    view_port_free(_state->view_port);
                    osMessageQueueDelete(_state->event);
                    osTimerDelete(id1);

                    return 0;
                } else {
                    handle_input(_state, &_state->event->value.input);
                }
            }

            release_mutex(state_mutex, _state);
            view_port_update(_state->view_port);
        }
    }

    return 0;
}