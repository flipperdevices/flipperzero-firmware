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
#define PARALLEL_OBSTACLES 3

typedef enum { GameStatePlaying, GameStateGameOver } GameState;

typedef struct Point {
    // Also used for offset data, which is sometimes negative
    int8_t x, y;
} Point;

typedef enum { CarObstacle } ObstacleType;

typedef struct Obstacle {
    ObstacleType type;
    Point position;
    bool isAlive;
} Obstacle;

typedef struct {
    bool playField[FIELD_HEIGHT][FIELD_WIDTH];
    uint16_t score;
    Obstacle obstacles[PARALLEL_OBSTACLES];
    int8_t roadStart;
    int8_t level;
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

    for(int y = 0; y < FIELD_HEIGHT; y++) {
        for(int x = 0; x < FIELD_WIDTH; x++) {
            if(race_state->playField[y][x]) {
                uint16_t xOffset = x * 5;
                uint16_t yOffset = y * 5;

                canvas_draw_rframe(
                    canvas,
                    BORDER_OFFSET + MARGIN_OFFSET + xOffset,
                    BORDER_OFFSET + MARGIN_OFFSET + yOffset - 1,
                    BLOCK_WIDTH,
                    BLOCK_HEIGHT,
                    1);
                canvas_draw_dot(
                    canvas,
                    BORDER_OFFSET + MARGIN_OFFSET + xOffset + 2,
                    BORDER_OFFSET + MARGIN_OFFSET + yOffset + 1);
                canvas_draw_dot(
                    canvas,
                    BORDER_OFFSET + MARGIN_OFFSET + xOffset + 3,
                    BORDER_OFFSET + MARGIN_OFFSET + yOffset + 1);
                canvas_draw_dot(
                    canvas,
                    BORDER_OFFSET + MARGIN_OFFSET + xOffset + 2,
                    BORDER_OFFSET + MARGIN_OFFSET + yOffset + 2);
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

static void race_game_init_road(RaceState* race_state) {
    int leftRoad = race_state->roadStart;
    int rightRoad = race_state->roadStart + 1;
    if(rightRoad == 4) rightRoad = 0;
    for(int y = 0; y < FIELD_HEIGHT; y++) {
        leftRoad++;
        rightRoad++;
        if(leftRoad < 4) {
            race_state->playField[y][0] = true;
        }
        if(rightRoad < 4) {
            race_state->playField[y][10] = true;
        }
        if(rightRoad == 4) rightRoad = 0;
        if(leftRoad == 4) leftRoad = 0;
    }
}

static void draw_callback(Canvas* canvas, void* ctx) {
    RaceState* race_state = ctx;
    if(race_state == NULL) {
        FURI_LOG_E("RaceGame", "state is null");
        return;
    }

    canvas_clear(canvas);

    race_game_draw_border(canvas);
    race_game_draw_playfield(canvas, race_state);
    /*
    // output player score, looks not good
    if(race_state->gameState == GameStatePlaying) {
        char buffer2[6];
        snprintf(buffer2, sizeof(buffer2), "%u", race_state->score);
        canvas_draw_str_aligned(canvas, 48, 10, AlignRight, AlignBottom, buffer2);
    }
    else  */
    if(race_state->gameState == GameStateGameOver) {
        // 128 x 64
        canvas_set_color(canvas, ColorWhite);
        canvas_draw_box(canvas, 1, 52, 62, 24);

        canvas_set_color(canvas, ColorBlack);
        canvas_draw_frame(canvas, 1, 52, 62, 24);

        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str(canvas, 4, 63, "Game Over");

        char buffer[13];
        snprintf(buffer, sizeof(buffer), "Score: %u", race_state->score);
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str_aligned(canvas, 32, 73, AlignCenter, AlignBottom, buffer);
    }
}

static void input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);
    FuriMessageQueue* event_queue = ctx;
    RaceGameEvent event = {.type = EventTypeKey, .input = *input_event};
    furi_message_queue_put(event_queue, &event, FuriWaitForever);
}

static void timer_callback(FuriMessageQueue* event_queue) {
    furi_assert(event_queue);
    RaceGameEvent event = {.type = EventTypeTick};
    furi_message_queue_put(event_queue, &event, 0);
}

static void race_game_init_state(RaceState* race_state) {
    race_state->gameState = GameStatePlaying;
    race_state->score = 0;
    race_state->level = 0;
    race_state->roadStart = 0;
    race_state->motionSpeed = 500;
    Point p = {.x = 5, .y = 20};
    race_state->headPosition = p;
    for(int i = 0; i < PARALLEL_OBSTACLES; i++) {
        Obstacle obstacle = {.type = CarObstacle, .isAlive = false, .position = {.x = 0, .y = 0}};
        race_state->obstacles[i] = obstacle;
    }
    memset(race_state->playField, 0, sizeof(race_state->playField));
    furi_timer_start(race_state->timer, race_state->motionSpeed);
}

static void race_game_draw_car(RaceState* race_state, Point p, bool changeState) {
    static Point pointsToCheck[] = {{0, 0}, {0, 1}, {-1, 1}, {1, 1}, {0, 2}, {-1, 3}, {1, 3}};
    for(int i = 0; i < 7; i++) {
        if(p.x + pointsToCheck[i].x > -1 && p.y + pointsToCheck[i].y > -1 &&
           p.x + pointsToCheck[i].x < FIELD_WIDTH && p.y + pointsToCheck[i].y < FIELD_HEIGHT) {
            if(changeState &&
               race_state->playField[p.y + pointsToCheck[i].y][p.x + pointsToCheck[i].x]) {
                race_state->gameState = GameStateGameOver;
            }
            race_state->playField[p.y + pointsToCheck[i].y][p.x + pointsToCheck[i].x] = true;
        }
    }
}
static void race_game_move_obstacles(RaceState* race_state) {
    for(int i = 0; i < PARALLEL_OBSTACLES; i++) {
        if(race_state->obstacles[i].isAlive) {
            race_state->obstacles[i].position.y++;
            if(race_state->obstacles[i].position.y > (FIELD_HEIGHT - 1))
                race_state->obstacles[i].isAlive = false;
        }
    }
}
static void race_game_spawn_obstacles(RaceState* race_state) {
    if(race_state->score % 90 == 0) {
        int aliveObjects = 0;

        for(int i = 0; i < PARALLEL_OBSTACLES; i++) {
            if(race_state->obstacles[i].isAlive) {
                aliveObjects++;
            }
        }
        if(aliveObjects < PARALLEL_OBSTACLES) {
            for(int i = 0; i < PARALLEL_OBSTACLES; i++) {
                if(!race_state->obstacles[i].isAlive) {
                    race_state->obstacles[i].isAlive = true;
                    race_state->obstacles[i].position.y = -4;
                    race_state->obstacles[i].position.x = (rand() % 3) * 3 + 2;
                    break;
                }
            }
        }
    }
}
static void race_game_process_step(RaceState* race_state, bool moveRoad) {
    if(race_state->gameState == GameStateGameOver) return;

    // calculate field boundaries
    if(race_state->headPosition.x < 2) race_state->headPosition.x = 2;
    if(race_state->headPosition.x > (FIELD_WIDTH - 3))
        race_state->headPosition.x = (FIELD_WIDTH - 3);
    if(race_state->headPosition.y < 0) race_state->headPosition.y = 0;
    if(race_state->headPosition.y > (FIELD_HEIGHT - 4))
        race_state->headPosition.y = (FIELD_HEIGHT - 4);
    for(int y = 0; y < FIELD_HEIGHT; y++) {
        for(int x = 0; x < FIELD_WIDTH; x++) {
            race_state->playField[y][x] = false;
        }
    }

    if(moveRoad) {
        race_state->score += 10;
        race_state->roadStart++;
        if(race_state->roadStart == 4) race_state->roadStart = 0;
        race_game_spawn_obstacles(race_state);
        race_game_move_obstacles(race_state);
        if(race_state->score % 500 == 0) {
            if(race_state->level < 9) {
                race_state->level++;
                race_state->motionSpeed = race_state->motionSpeed - 50;
                furi_timer_stop(race_state->timer);
                furi_timer_start(race_state->timer, race_state->motionSpeed);
            }
        }
    }

    for(int i = 0; i < PARALLEL_OBSTACLES; i++) {
        if(race_state->obstacles[i].isAlive) {
            race_game_draw_car(race_state, race_state->obstacles[i].position, false);
        }
    }
    race_game_draw_car(race_state, race_state->headPosition, true);
    race_game_init_road(race_state);
}

int32_t race_app(void* p) {
    UNUSED(p);

    srand(DWT->CYCCNT);

    // current custom event element
    RaceGameEvent event;

    RaceState* race_state = malloc(sizeof(RaceState));
    FuriMutex* state_mutex = furi_mutex_alloc(FuriMutexTypeRecursive);

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
        furi_mutex_acquire(state_mutex, 1000);
        //         RaceState* race_state = (RaceState*)furi_mutex_block(&state_mutex, 1000);

        // Выбираем событие из очереди в переменную event (ждем бесконечно долго, если очередь пуста)
        // и проверяем, что у нас получилось это сделать
        furi_check(furi_message_queue_get(event_queue, &event, FuriWaitForever) == FuriStatusOk);
        bool moveRoad = false;
        if(event.type == EventTypeKey) {
            // Если нажата кнопка "назад", то выходим из цикла, а следовательно и из приложения
            switch(event.input.key) {
            case InputKeyBack:
                processing = false;
                break;
            case InputKeyOk:
                if(race_state->gameState == GameStateGameOver) {
                    race_game_init_state(race_state);
                }
                break;
            case InputKeyUp:
                race_state->headPosition.y -= 1;
                break;
            case InputKeyDown:
                race_state->headPosition.y += 1;
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
            // Наше событие — это сработавший таймер
        } else if(event.type == EventTypeTick) {
            //Move road and obstacles with timer ticks
            moveRoad = true;
        }
        race_game_process_step(race_state, moveRoad);
        view_port_update(view_port);
        furi_mutex_release(state_mutex);
    }
    // clearing everything on game exit

    // clear game timer
    furi_timer_free(race_state->timer);

    // clear message queue
    furi_message_queue_free(event_queue);

    // clearing ui
    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_record_close(RECORD_GUI);

    return 0;
}