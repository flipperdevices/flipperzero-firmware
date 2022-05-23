#include <furi.h>
#include <gui/gui.h>
#include <input/input.h>
#include <stdlib.h>

#define ZOMBIES_MAX 3
#define PROJECTILES_MAX 10
#define PLAYER_MIN_Y 5
#define PLAYER_MAX_Y 58
#define PLAYER_START_X 8
#define PLAYER_START_Y (PLAYER_MAX_Y - PLAYER_MIN_Y) / 2
#define WALL_X 16

typedef enum {
    EventTypeTick,
    EventTypeKey,
} EventType;

typedef struct {
    EventType type;
    InputEvent input;
} PluginEvent;

typedef struct {
    int x;
    int y;
} Point;

typedef struct { 
    Point position;
    int hp;
} Player; 

typedef struct { 
    Point position;
    int hp;
} Zombie; 

typedef struct { 
    Point position;
} Projectile; 

typedef struct {
    int x;
    int y;

    Player player;
    Zombie* zombies[ZOMBIES_MAX];

    size_t projectiles_count;
    Projectile* projectiles[PROJECTILES_MAX];

    bool input_shoot;
} PluginState; 

static void render_callback(Canvas* const canvas, void* ctx) {
    const PluginState* plugin_state = acquire_mutex((ValueMutex*)ctx, 25);
    if(plugin_state == NULL) {
        return;
    }

    canvas_draw_frame(canvas, 0, 0, 128, 64);

    canvas_set_font(canvas, FontPrimary);

    canvas_draw_str_aligned(canvas, plugin_state->x, plugin_state->y, AlignCenter, AlignCenter, "@");

    for (int i = 0; i < PROJECTILES_MAX; ++i) {
        Projectile* p = plugin_state->projectiles[i];
        if (p != NULL) {
            canvas_draw_circle(canvas, p->position.x, p->position.y, 3);
        }
    }

    canvas_draw_line(canvas, WALL_X, 2, WALL_X, 100);
    canvas_draw_line(canvas, WALL_X + 2, 4, WALL_X + 2, 100);

    //char* info = (char*)malloc(16 * sizeof(char));
    //asprintf(&info, "%d, %d", plugin_state->x, plugin_state->y);
    //canvas_draw_str_aligned(canvas, 32, 16, AlignLeft, AlignBottom, info);
    //free(info);

    if (plugin_state->projectiles_count >= PROJECTILES_MAX) {
        canvas_draw_str_aligned(canvas, 16, 16, AlignLeft, AlignCenter, "OUT OF AMMO");
    }

    release_mutex((ValueMutex*)ctx, plugin_state);
}

static void input_callback(InputEvent* input_event, osMessageQueueId_t event_queue) {
    furi_assert(event_queue); 

    PluginEvent event = {.type = EventTypeKey, .input = *input_event};
    osMessageQueuePut(event_queue, &event, 0, osWaitForever);
}

static void tick(PluginState* const plugin_state) {
    if (plugin_state->input_shoot && (plugin_state->projectiles_count < PROJECTILES_MAX)) {
        Projectile* p = (Projectile*)malloc(sizeof(Projectile));
        p->position.x = plugin_state->x;
        p->position.y = plugin_state->y;

        size_t idx = plugin_state->projectiles_count;
        plugin_state->projectiles[idx] = p;
        plugin_state->projectiles_count += 1;
    }

    for (int i = 0; i < PROJECTILES_MAX; ++i) {
        Projectile* p = plugin_state->projectiles[i];
        if (p != NULL) {
            p->position.x += 3;

            if (p->position.x >= 128) {
                free(p);
                plugin_state->projectiles[i] = NULL;
            }
        }
    }

    plugin_state->input_shoot = false;
}

static void timer_callback(osMessageQueueId_t event_queue) {
    furi_assert(event_queue);

    PluginEvent event = {.type = EventTypeTick};
    osMessageQueuePut(event_queue, &event, 0, 0);
}

static void hello_world_state_init(PluginState* const plugin_state) {
    plugin_state->x = PLAYER_START_X;
    plugin_state->y = PLAYER_START_Y;

    plugin_state->projectiles_count = 0;
} 

int32_t zombiez_app(void* p) { 
    osMessageQueueId_t event_queue = osMessageQueueNew(8, sizeof(PluginEvent), NULL); 
    
    PluginState* plugin_state = malloc(sizeof(PluginState));
    hello_world_state_init(plugin_state);

    ValueMutex state_mutex; 
    if (!init_mutex(&state_mutex, plugin_state, sizeof(PluginState))) {
        FURI_LOG_E("zombiez", "Failed to init plugin state mutex");
        free(plugin_state); 
        return 255;
    }

    // Set system callbacks
    ViewPort* view_port = view_port_alloc(); 
    view_port_draw_callback_set(view_port, render_callback, &state_mutex);
    view_port_input_callback_set(view_port, input_callback, event_queue);
 
    osTimerId_t timer =
        osTimerNew(timer_callback, osTimerPeriodic, event_queue, NULL);
    osTimerStart(timer, osKernelGetTickFreq() / 22);

    // Open GUI and register view_port
    Gui* gui = furi_record_open("gui"); 
    gui_add_view_port(gui, view_port, GuiLayerFullscreen); 

    PluginEvent event; 
    bool isRunning = true;
    while(isRunning) { 
        osStatus_t event_status = osMessageQueueGet(event_queue, &event, NULL, 100);
        PluginState* plugin_state = (PluginState*)acquire_mutex_block(&state_mutex);
        if(event_status == osOK) {
            if(event.type == EventTypeKey) {
                if(event.input.type == InputTypePress) {  
                    switch(event.input.key) {
                    case InputKeyUp: 
                         plugin_state->y--;
                        break; 
                    case InputKeyDown: 
                        plugin_state->y++;
                        break; 
                    case InputKeyOk: 
                        plugin_state->input_shoot = true;
                        break;
                    case InputKeyBack: 
                        isRunning = false;
                        break;
                    default:
                        break;
                    }
                }
                else if(event.input.type == InputTypeRepeat) {  
                    switch(event.input.key) {
                    case InputKeyUp: 
                         plugin_state->y -= 2;
                        break; 
                    case InputKeyDown: 
                        plugin_state->y += 2;
                        break; 
                    default:
                        break;
                    }
                }
            } else if(event.type == EventTypeTick) {
                tick(plugin_state);
            }
        } else {
            FURI_LOG_D("zombiez", "osMessageQueue event timeout");
        }

        view_port_update(view_port);
        release_mutex(&state_mutex, plugin_state);
    }

    view_port_enabled_set(view_port, false);
    gui_remove_view_port(gui, view_port);
    furi_record_close("gui");
    view_port_free(view_port);
    osMessageQueueDelete(event_queue); 

    return 0;
}