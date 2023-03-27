#include <stdio.h>
#include <furi.h>
#include <gui/gui.h>
#include <input/input.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <furi_hal_resources.h>
#include <furi_hal_gpio.h>
#include <notification/notification_messages.h>


#define BORDER_OFFSET 1
#define MARGIN_OFFSET 3
#define BLOCK_HEIGHT 6
#define BLOCK_WIDTH 6

#define FIELD_WIDTH 11
#define FIELD_HEIGHT 24

typedef enum {
    GameStatePlaying,
    GameStateGameOver
} GameState;

typedef struct Point {
    // Also used for offset data, which is sometimes negative
    int8_t x, y;
} Point;

typedef struct {
    bool playField[FIELD_HEIGHT][FIELD_WIDTH];
    uint16_t score;
    int8_t roadStart;
    Point headPosition;
    uint16_t motionSpeed;
    GameState gameState;
    FuriTimer* timer;
} RaceState;

typedef enum {
    EventTypeTick,
    EventTypeKey,
} EventType;

typedef struct {
    EventType type;
    InputEvent input;
} RaceGameEvent;


static void race_game_draw_playfield(Canvas* const canvas, RaceState* race_state) {
    // Playfield: 11 x 24

    for (int y = 0; y < FIELD_HEIGHT; y++) {
        for (int x = 0; x < FIELD_WIDTH; x++) {
            if (race_state->playField[y][x]) {
                uint16_t xOffset = x * 5;
                uint16_t yOffset = y * 5;

                canvas_draw_rframe(
                    canvas, 
                    BORDER_OFFSET + MARGIN_OFFSET + xOffset, 
                    BORDER_OFFSET + MARGIN_OFFSET + yOffset - 1, 
                    BLOCK_WIDTH, 
                    BLOCK_HEIGHT,
                    1
                );
                canvas_draw_dot(
                    canvas, 
                    BORDER_OFFSET + MARGIN_OFFSET + xOffset + 2,
                    BORDER_OFFSET + MARGIN_OFFSET + yOffset + 1
                );
                canvas_draw_dot(
                    canvas, 
                    BORDER_OFFSET + MARGIN_OFFSET + xOffset + 3,
                    BORDER_OFFSET + MARGIN_OFFSET + yOffset + 1
                );
                canvas_draw_dot(
                    canvas, 
                    BORDER_OFFSET + MARGIN_OFFSET + xOffset + 2,
                    BORDER_OFFSET + MARGIN_OFFSET + yOffset + 2
                ); 
            }
        }
    }
}

static void race_game_draw_border(Canvas* canvas) {
    canvas_draw_line(canvas, 0, 0, 0, 127);
    canvas_draw_line(canvas, 0, 127, 63, 127);
    canvas_draw_line(canvas, 63, 127, 63, 0);

    canvas_draw_line(canvas, 2, 0, 2, 125);
    canvas_draw_line(canvas, 2, 125, 61, 125);
    canvas_draw_line(canvas, 61, 125, 61, 0);
}

static void race_game_init_road( RaceState* race_state) {
    int leftRoad = race_state->roadStart;
    int rightRoad = race_state->roadStart+1;
    if (rightRoad == 4) rightRoad=0;    
    for (int y = 0; y < FIELD_HEIGHT; y++) {
        leftRoad++;
        rightRoad++;
        if (leftRoad<4) {
            race_state->playField[y][0] = true;
        }
        if (rightRoad<4) {
            race_state->playField[y][10] = true;
        }
        if (rightRoad==4)
            rightRoad=0;
        if (leftRoad==4)
            leftRoad=0;
    }
}

static void draw_callback(Canvas* canvas, void* ctx) {
    // const RaceState* race_state = furi_mutex_acquire((FuriMutex*)ctx, 25);
    RaceState* race_state = ctx;
    if(race_state == NULL) {
        FURI_LOG_E("RaceGame", "it null");
        return;
    }

    canvas_clear(canvas);
    race_game_draw_border(canvas);
    race_game_init_road(race_state);
    race_game_draw_playfield(canvas, race_state);
    // canvas_set_font(canvas, FontPrimary);
    // canvas_draw_str(canvas, 0, 10, "Hello World!");
}



static void input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);
    FuriMessageQueue* event_queue = ctx;
    RaceGameEvent event = {.type = EventTypeKey, .input = *input_event};
    furi_message_queue_put(event_queue, &event, FuriWaitForever);
}

static void timer_callback(FuriMessageQueue* event_queue) {
    // Проверяем, что контекст не нулевой
    furi_assert(event_queue);

    RaceGameEvent event = {.type = EventTypeTick};
    furi_message_queue_put(event_queue, &event, 0);
}

static void race_game_init_state(RaceState* race_state) {
    Point p = {20,5};
    race_state->gameState = GameStatePlaying;
    race_state->score = 0;
    race_state->roadStart=0;
    race_state->motionSpeed = 500;
    race_state->headPosition = p;
    memset(race_state->playField, 0, sizeof(race_state->playField));
  //  memset(tetris_state->playField, 0, sizeof(tetris_state->playField));

  //  memcpy(&tetris_state->currPiece, &shapes[rand() % 7], sizeof(tetris_state->currPiece));

    furi_timer_start(race_state->timer, race_state->motionSpeed);
}

static void race_game_process_step(RaceState* race_state) {
    if(race_state->gameState == GameStateGameOver)
        return;
    for (int y = 0; y < FIELD_HEIGHT; y++) {
        for (int x = 0; x < FIELD_WIDTH; x++) {
            race_state->playField[y][x] = false;
        }
    }
    race_state->roadStart++;
    if (race_state->roadStart == 4)
        race_state->roadStart = 0;
    race_state->playField[race_state->headPosition.y][race_state->headPosition.x] = true;
    race_state->playField[race_state->headPosition.y+1][race_state->headPosition.x] = true;
    race_state->playField[race_state->headPosition.y+1][race_state->headPosition.x-1] = true;
    race_state->playField[race_state->headPosition.y+1][race_state->headPosition.x+1] = true;
    race_state->playField[race_state->headPosition.y+2][race_state->headPosition.x] = true;
    race_state->playField[race_state->headPosition.y+3][race_state->headPosition.x-1] = true;
    race_state->playField[race_state->headPosition.y+3][race_state->headPosition.x+1] = true;
    
}

int32_t race_app(void* p) {
    UNUSED(p);
    
    srand(DWT->CYCCNT);
    
    // current custom event element
    RaceGameEvent event;

    RaceState* race_state = malloc(sizeof(RaceState));
    // FuriMutex* state_mutex = furi_mutex_alloc(FuriMutexTypeNormal);;
    
    // Queue on 8 events
    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(RaceGameEvent));


    race_state->timer = furi_timer_alloc(timer_callback, FuriTimerTypePeriodic, event_queue);


    race_game_init_state(race_state);

    

    // Creating viewport
    ViewPort* view_port = view_port_alloc();
    // Set viewpoint orientation to vertical
    view_port_set_orientation(view_port, ViewPortOrientationVertical);

    // creating draw callback without context
    // view_port_draw_callback_set(view_port, draw_callback, &state_mutex);
    view_port_draw_callback_set(view_port, draw_callback, race_state);

    // setting up and all input events go to our event queue
    view_port_input_callback_set(view_port, input_callback, event_queue);

    // creating gui
    Gui* gui = furi_record_open(RECORD_GUI);
    // adding our view port to gui in full screen mode
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);
    
    

    // Бесконечный цикл обработки очереди событий
    for(bool processing = true; processing;) {

//         RaceState* race_state = (RaceState*)furi_mutex_block(&state_mutex, 1000);

        // Выбираем событие из очереди в переменную event (ждем бесконечно долго, если очередь пуста)
        // и проверяем, что у нас получилось это сделать
        furi_check(furi_message_queue_get(event_queue, &event, FuriWaitForever) == FuriStatusOk);

        if(event.type == EventTypeKey) {
            // Если нажата кнопка "назад", то выходим из цикла, а следовательно и из приложения
            switch(event.input.key) {
                case InputKeyBack:
                    processing = false;
                    break;
                case InputKeyRight:
                    race_state->headPosition.x += 1;
                    break;
                case InputKeyLeft:    
                    race_state->headPosition.x -= 1;
                    break;
                default:
                    break;
            }
            if(event.input.key == InputKeyBack) {
                processing = false;
            }
            // Наше событие — это сработавший таймер
        } else if(event.type == EventTypeTick) {
            // Сделаем что-то по таймеру
        }
        race_game_process_step(race_state);
        view_port_update(view_port);
        // furi_mutex_release(&state_mutex, race_state);

    }
    // clearing everything on game exit

    // clear game timer
    furi_timer_free(race_state->timer);

    // clear message queue
    furi_message_queue_free(event_queue);

//    vTaskPrioritySet(timer_task, origTimerPrio);

    // clearing ui
    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_record_close(RECORD_GUI);

    return 0;
}