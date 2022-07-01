#include <furi.h>
#include <gui/gui.h>

#include <input/input.h>
#include <stdlib.h>

#define TAG "GameOfLife"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define TOTAL_PIXELS SCREEN_WIDTH* SCREEN_HEIGHT

typedef enum {
    EventTypeTick,
    EventTypeKey,
} EventType;

typedef struct {
    EventType type;
    InputEvent input;
} AppEvent;

typedef struct {
    bool revive;
    int evo;
} AppState;

unsigned char new[TOTAL_PIXELS] = {};
unsigned char old[TOTAL_PIXELS] = {};
unsigned char* fields[] = {new, old};

int current = 0;
int next = 1;

unsigned char get_cell(int x, int y) {
    if(x <= 0 || x >= SCREEN_WIDTH) return 0;
    if(y <= 0 || y >= SCREEN_HEIGHT) return 0;

    int pix = (y * SCREEN_WIDTH) + x;
    return fields[current][pix];
}

int count_neightbors(int x, int y) {
    return get_cell(x + 1, y - 1) + get_cell(x - 1, y - 1) + get_cell(x - 1, y + 1) +
           get_cell(x + 1, y + 1) + get_cell(x + 1, y) + get_cell(x - 1, y) + get_cell(x, y - 1) +
           get_cell(x, y + 1);
}

static void update_field(AppState* const app_state) {
    if(app_state->revive) {
        for(int i = 0; i < TOTAL_PIXELS; ++i) {
            if((random() % 100) == 1) {
                fields[current][i] = 1;
            }
            app_state->revive = false;
        }
    }

    for(int i = 0; i < TOTAL_PIXELS; ++i) {
        int x = i % SCREEN_WIDTH;
        int y = (int)(i / SCREEN_WIDTH);

        int v = get_cell(x, y);
        int n = count_neightbors(x, y);

        if(v && n == 3) {
            ++app_state->evo;
        } else if(v && (n < 2 || n > 3)) {
            ++app_state->evo;
            v = 0;
        } else if(!v && n == 3) {
            ++app_state->evo;
            v = 1;
        }

        fields[next][i] = v;
    }

    next ^= current;
    current ^= next;
    next ^= current;

    if(app_state->evo < TOTAL_PIXELS) {
        app_state->revive = true;
        app_state->evo = 0;
    }
}

static void input_callback(InputEvent* input_event, osMessageQueueId_t event_queue) {
    furi_assert(event_queue);

    AppEvent event = {.type = EventTypeKey, .input = *input_event};
    osMessageQueuePut(event_queue, &event, 0, 0);
}

static void render_callback(Canvas* canvas, void* ctx) {
    AppState* app_state = (AppState*)acquire_mutex((ValueMutex*)ctx, 25);
    canvas_clear(canvas);

    for(int i = 0; i < TOTAL_PIXELS; ++i) {
        int x = i % SCREEN_WIDTH;
        int y = (int)(i / SCREEN_WIDTH);
        if(fields[current][i] == 1) canvas_draw_dot(canvas, x, y);
    }

    release_mutex((ValueMutex*)ctx, app_state);
}

int32_t game_of_life_app(void* p) {
    osMessageQueueId_t event_queue = osMessageQueueNew(1, sizeof(AppEvent), NULL);
    furi_check(event_queue);

    AppState* app_state = malloc(sizeof(AppState));

    ValueMutex state_mutex;
    if(!init_mutex(&state_mutex, app_state, sizeof(AppState))) {
        FURI_LOG_E(TAG, "cannot create mutex\r\n");
        free(app_state);
        return 255;
    }

    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, render_callback, &state_mutex);
    view_port_input_callback_set(view_port, input_callback, event_queue);

    Gui* gui = furi_record_open("gui");
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    AppEvent event;
    for(bool processing = true; processing;) {
        osStatus_t event_status = osMessageQueueGet(event_queue, &event, NULL, 25);
        AppState* app_state = (AppState*)acquire_mutex_block(&state_mutex);

        if(event_status == osOK && event.type == EventTypeKey &&
           event.input.type == InputTypePress) {
            if(event.input.key == InputKeyBack) {
                // furiac_exit(NULL);
                processing = false;
                return 0;
            }
        }

        update_field(app_state);

        view_port_update(view_port);
        release_mutex(&state_mutex, app_state);
    }

    view_port_enabled_set(view_port, false);
    gui_remove_view_port(gui, view_port);
    furi_record_close("gui");
    view_port_free(view_port);
    osMessageQueueDelete(event_queue);
    free(app_state);

    return 0;
}