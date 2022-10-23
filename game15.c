#include <furi.h>
#include <gui/gui.h>
#include <notification/notification.h>
#include <notification/notification_messages.h>
#include <storage/storage.h>

#include "sandbox.h"

#define FPS 20
#define CELL_WIDTH 10
#define CELL_HEIGHT 8
#define MOVE_TICKS 5
#define KEY_STACK_SIZE 16
#define TOP_RECORD_DIRECTORY "/ext/apps/Games"
#define TOP_RECORD_FILENAME TOP_RECORD_DIRECTORY "/game15.top"

typedef enum {
    DirectionNone,
    DirectionUp,
    DirectionDown,
    DirectionLeft,
    DirectionRight
} direction_e;

typedef enum { ScenePlay, SceneWin } scene_e;

typedef struct {
    uint8_t cell_index;
    uint8_t zero_index;
    uint8_t move_direction;
    uint8_t move_ticks;
} moving_cell_t;

static scene_e scene;
static uint8_t board[16];
static uint16_t top_record;
static uint16_t move_count;
static uint32_t tick_count;
static NotificationApp* notification;
static moving_cell_t moving_cell;

static uint8_t keys[KEY_STACK_SIZE];
static uint8_t key_stack_head = 0;

static const uint8_t pic_cells[] = {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0x30, 0xfc, 0x38, 0xfc, 0x30, 0xfc, 0x30, 0xfc, 0x30, 0xfc, 0x30, 0xfc, 0x30, 0xfc, 0x30, 0xfc,
    0x78, 0xfc, 0xcc, 0xfc, 0xcc, 0xfc, 0x60, 0xfc, 0x30, 0xfc, 0x18, 0xfc, 0x0c, 0xfc, 0xfc, 0xfc,
    0x78, 0xfc, 0xcc, 0xfc, 0xcc, 0xfc, 0x60, 0xfc, 0xc0, 0xfc, 0xcc, 0xfc, 0xcc, 0xfc, 0x78, 0xfc,
    0x70, 0xfc, 0x78, 0xfc, 0x68, 0xfc, 0x6c, 0xfc, 0x6c, 0xfc, 0xec, 0xfc, 0xfc, 0xfc, 0x60, 0xfc,
    0xfc, 0xfc, 0x0c, 0xfc, 0x0c, 0xfc, 0x7c, 0xfc, 0xc0, 0xfc, 0xcc, 0xfc, 0xcc, 0xfc, 0x78, 0xfc,
    0x78, 0xfc, 0x0c, 0xfc, 0x0c, 0xfc, 0x7c, 0xfc, 0xcc, 0xfc, 0xcc, 0xfc, 0xcc, 0xfc, 0x78, 0xfc,
    0xfc, 0xfc, 0xc0, 0xfc, 0xc0, 0xfc, 0xc0, 0xfc, 0xc0, 0xfc, 0xc0, 0xfc, 0xc0, 0xfc, 0xc0, 0xfc,
    0x78, 0xfc, 0xcc, 0xfc, 0xcc, 0xfc, 0x78, 0xfc, 0xcc, 0xfc, 0xcc, 0xfc, 0xcc, 0xfc, 0x78, 0xfc,
    0x78, 0xfc, 0xcc, 0xfc, 0xcc, 0xfc, 0xcc, 0xfc, 0xf8, 0xfc, 0xc0, 0xfc, 0xc0, 0xfc, 0x78, 0xfc,
    0xe6, 0xfd, 0x37, 0xff, 0x36, 0xff, 0x36, 0xff, 0x36, 0xff, 0x36, 0xff, 0x36, 0xff, 0xe6, 0xfd,
    0x8c, 0xfd, 0xce, 0xfd, 0x8c, 0xfd, 0x8c, 0xfd, 0x8c, 0xfd, 0x8c, 0xfd, 0x8c, 0xfd, 0x8c, 0xfd,
    0xe6, 0xfd, 0x37, 0xff, 0x36, 0xff, 0x86, 0xfd, 0xc6, 0xfc, 0x66, 0xfc, 0x36, 0xfc, 0xf6, 0xff,
    0xe6, 0xfd, 0x37, 0xff, 0x36, 0xff, 0x86, 0xfd, 0x06, 0xff, 0x36, 0xff, 0x36, 0xff, 0xe6, 0xfd,
    0xc6, 0xfd, 0xe7, 0xfd, 0xa6, 0xfd, 0xb6, 0xfd, 0xb6, 0xfd, 0xb6, 0xff, 0xf6, 0xff, 0x86, 0xfd,
    0xf6, 0xff, 0x37, 0xfc, 0x36, 0xfc, 0xf6, 0xfd, 0x06, 0xff, 0x36, 0xff, 0x36, 0xff, 0xe6, 0xfd,
};

static const uint8_t pic_digits[] = {
    0xf0, 0xf2, 0xf2, 0xf2, 0xf2, 0xf0, 0xf9, 0xf8, 0xf9, 0xf9, 0xf9, 0xf0, 0xf0, 0xf2, 0xf3,
    0xf1, 0xfc, 0xf0, 0xf0, 0xf3, 0xf1, 0xf3, 0xf2, 0xf0, 0xf3, 0xf1, 0xf2, 0xf2, 0xf0, 0xf3,
    0xf0, 0xfc, 0xf0, 0xf3, 0xf2, 0xf0, 0x00, 0x0c, 0x00, 0x02, 0x02, 0x00, 0x00, 0x03, 0x03,
    0x03, 0x03, 0x03, 0x00, 0x02, 0x00, 0x02, 0x02, 0x00, 0x00, 0x02, 0x00, 0x03, 0x03, 0x00,
};

static const uint8_t pic_top[] = {11, 4, 0x88, 0xf8, 0xad, 0xfa, 0xad, 0xf8, 0x8d, 0xfe};
static const uint8_t pic_move[] =
    {17, 4, 0x2e, 0x2a, 0xfe, 0xa4, 0xaa, 0xff, 0xaa, 0x2a, 0xff, 0x2e, 0x36, 0xfe};
static const uint8_t pic_time[] = {15, 4, 0xa8, 0x8b, 0x2d, 0xe9, 0xad, 0xca, 0xad, 0x8b};

static const uint8_t pic_puzzled[] = {
    0xff, 0xcf, 0x00, 0xf3, 0xff, 0xfc, 0x3f, 0x03, 0xc0, 0xff, 0xf3, 0x0f, 0xdc, 0xff, 0xcf,
    0x00, 0xf3, 0xff, 0xfc, 0x3f, 0x03, 0xc0, 0xff, 0xf3, 0x0f, 0xdc, 0x03, 0xcc, 0x00, 0x03,
    0x38, 0x00, 0x0e, 0x03, 0xc0, 0x00, 0x30, 0x30, 0xdc, 0x03, 0xcc, 0x00, 0x03, 0x1c, 0x00,
    0x07, 0x03, 0xc0, 0x00, 0x30, 0x30, 0xdc, 0xff, 0xcf, 0x00, 0x03, 0x0e, 0x80, 0x03, 0x03,
    0xc0, 0xff, 0x33, 0xc0, 0xdc, 0xff, 0xcf, 0x00, 0x03, 0x07, 0xc0, 0x01, 0x03, 0xc0, 0xff,
    0x33, 0xc0, 0xdc, 0x03, 0xc0, 0x00, 0x83, 0x03, 0xe0, 0x00, 0x03, 0xc0, 0x00, 0x30, 0xc0,
    0xd0, 0x03, 0xc0, 0x00, 0xc3, 0x01, 0x70, 0x00, 0x03, 0xc0, 0x00, 0x30, 0xc0, 0xd0, 0x03,
    0xc0, 0xff, 0xf3, 0xff, 0xfc, 0x3f, 0xff, 0xcf, 0xff, 0xf3, 0xff, 0xdc, 0x03, 0xc0, 0xff,
    0xf3, 0xff, 0xfc, 0x3f, 0xff, 0xcf, 0xff, 0xf3, 0xff, 0xdc};

static void key_stack_init() {
    key_stack_head = 0;
}

static uint8_t key_stack_pop() {
    return keys[--key_stack_head];
}

static bool key_stack_is_empty() {
    return key_stack_head == 0;
}

static int key_stack_push(uint8_t value) {
    if(key_stack_head != KEY_STACK_SIZE) {
        keys[key_stack_head] = value;
        key_stack_head++;
        return key_stack_head;
    } else
        return -1;
}

static void storage_top_record_load() {
    Storage* storage = furi_record_open(RECORD_STORAGE);
    File* file = storage_file_alloc(storage);

    if(storage_file_open(file, TOP_RECORD_FILENAME, FSAM_READ, FSOM_OPEN_EXISTING))
        storage_file_read(file, &top_record, 2);
    storage_file_close(file);
    storage_file_free(file);
    furi_record_close(RECORD_STORAGE);
}

static void storage_top_record_save() {
    Storage* storage = furi_record_open(RECORD_STORAGE);

    if(storage_common_stat(storage, TOP_RECORD_DIRECTORY, NULL) == FSE_NOT_EXIST) {
        if(!storage_simply_mkdir(storage, TOP_RECORD_DIRECTORY)) {
            return;
        }
    }

    File* file = storage_file_alloc(storage);
    if(storage_file_open(file, TOP_RECORD_FILENAME, FSAM_WRITE, FSOM_CREATE_ALWAYS))
        storage_file_write(file, &top_record, 2);
    storage_file_close(file);
    storage_file_free(file);
    furi_record_close(RECORD_STORAGE);
}

static void set_moving_cell_by_direction(direction_e direction) {
    moving_cell.move_direction = DirectionNone;
    moving_cell.zero_index = 0xff;

    for(int i = 0; i < 16; i++) {
        if(!board[i]) {
            moving_cell.zero_index = i;
            break;
        }
    }
    if(moving_cell.zero_index == 0xff) return;

    uint8_t x = moving_cell.zero_index % 4;
    uint8_t y = moving_cell.zero_index / 4;

    moving_cell.cell_index = moving_cell.zero_index;

    if(direction == DirectionUp && y < 3)
        moving_cell.cell_index += 4;
    else if(direction == DirectionDown && y > 0)
        moving_cell.cell_index -= 4;
    else if(direction == DirectionLeft && x < 3)
        moving_cell.cell_index++;
    else if(direction == DirectionRight && x > 0)
        moving_cell.cell_index--;
    else
        return;

    moving_cell.move_ticks = 0;
    moving_cell.move_direction = direction;
}

static bool is_board_has_solution() {
    uint8_t i, j, inv = 0;
    for(i = 0; i < 16; ++i)
        if(board[i])
            for(j = 0; j < i; ++j)
                if(board[j] > board[i]) ++inv;
    for(i = 0; i < 16; ++i)
        if(board[i] == 0) inv += 1 + i / 4;

    return inv % 2 == 0;
}

static void board_init() {
    for(int i = 0; i < 16; i++) {
        board[i] = (i + 1) % 16;
    }

    do {
        for(int i = 15; i >= 1; i--) {
            int j = rand() % (i + 1);
            uint8_t tmp = board[j];
            board[j] = board[i];
            board[i] = tmp;
        }
    } while(!is_board_has_solution());
}

static void game_init() {
    scene = ScenePlay;
    move_count = 0;
    tick_count = 0;
    storage_top_record_load();
    moving_cell.move_direction = DirectionNone;
    board_init();
    key_stack_init();
}

static bool is_board_solved() {
    for(int i = 0; i < 16; i++)
        if(((i + 1) % 16) != board[i]) return false;
    return true;
}

static void game_tick() {
    switch(scene) {
    case ScenePlay:
        tick_count++;

        if(moving_cell.move_direction == DirectionNone && !key_stack_is_empty()) {
            set_moving_cell_by_direction(key_stack_pop());
            if(moving_cell.move_direction == DirectionNone) {
                notification_message(notification, &sequence_single_vibro);
                key_stack_init();
            }
        }

        if(moving_cell.move_direction != DirectionNone) {
            moving_cell.move_ticks++;
            if(moving_cell.move_ticks == MOVE_TICKS) {
                board[moving_cell.zero_index] = board[moving_cell.cell_index];
                board[moving_cell.cell_index] = 0;
                moving_cell.move_direction = DirectionNone;
                move_count++;
            }
            if(is_board_solved()) {
                notification_message(notification, &sequence_double_vibro);
                if(move_count < top_record || top_record == 0) {
                    top_record = move_count;
                    storage_top_record_save();
                }
                scene = SceneWin;
            }
        }
        break;

    case SceneWin:
        if(!key_stack_is_empty()) game_init();
        break;
    }
}

static void draw_cell(Canvas* canvas, uint8_t x, uint8_t y, uint8_t cell_number) {
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_rframe(canvas, x, y, 18, 14, 1);
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_xbm(canvas, x + 4, y + 3, CELL_WIDTH, CELL_HEIGHT, pic_cells + cell_number * 16);
}

static void board_draw(Canvas* canvas) {
    for(int i = 0; i < 16; i++) {
        if(board[i]) {
            if(moving_cell.move_direction == DirectionNone || moving_cell.cell_index != i)
                draw_cell(canvas, (i % 4) * 20 + 7, (i / 4) * 16 + 1, board[i]);
            if(moving_cell.move_direction != DirectionNone && moving_cell.cell_index == i) {
                uint8_t from_x = (moving_cell.cell_index % 4) * 20 + 7;
                uint8_t from_y = (moving_cell.cell_index / 4) * 16 + 1;
                uint8_t to_x = (moving_cell.zero_index % 4) * 20 + 7;
                uint8_t to_y = (moving_cell.zero_index / 4) * 16 + 1;
                int now_x = from_x + (to_x - from_x) * moving_cell.move_ticks / MOVE_TICKS;
                int now_y = from_y + (to_y - from_y) * moving_cell.move_ticks / MOVE_TICKS;
                draw_cell(canvas, now_x, now_y, board[i]);
            }
        }
    }
}

static void number_draw(Canvas* canvas, uint8_t y, uint32_t value) {
    uint8_t x = 121;
    while(true) {
        uint8_t digit = value % 10;
        canvas_draw_xbm(canvas, x, y, 4, 6, pic_digits + digit * 6);
        x -= 5;
        value = value / 10;
        if(!value) break;
    }
}

static void plate_draw(
    Canvas* canvas,
    uint8_t y,
    const uint8_t* header,
    uint32_t value,
    bool dont_draw_zero_value) {
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_rbox(canvas, 92, y, 35, 19, 2);
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_xbm(canvas, 95, y + 3, header[0], header[1], &header[2]);
    if((!value && !dont_draw_zero_value) || value) number_draw(canvas, y + 10, value);
}

static void info_draw(Canvas* canvas) {
    plate_draw(canvas, 1, pic_top, top_record, true);
    plate_draw(canvas, 22, pic_move, move_count, false);
    plate_draw(canvas, 43, pic_time, tick_count / FPS, false);
}

static void render_callback(Canvas* const canvas) {
    canvas_set_color(canvas, ColorWhite);
    canvas_draw_box(canvas, 0, 0, 128, 64);

    if(scene == ScenePlay || scene == SceneWin) {
        canvas_set_color(canvas, ColorBlack);
        board_draw(canvas);
        info_draw(canvas);

        canvas_set_color(canvas, ColorWhite);
        canvas_draw_box(canvas, 0, 0, 128, 64);
        canvas_set_color(canvas, ColorBlack);
        canvas_draw_str(canvas, 10, 10, "0123456789");
    }
    if(scene == SceneWin) {
        canvas_set_color(canvas, ColorWhite);
        for(int x = 0; x < 128; x += 2) {
            for(int y = 0; y < 64; y++) {
                canvas_draw_dot(canvas, x + (y % 2 == 1 ? 0 : 1), y);
            }
        }
        canvas_draw_box(canvas, 7, 20, 114, 24);
        canvas_set_color(canvas, ColorBlack);
        canvas_draw_box(canvas, 8, 21, 112, 22);
        canvas_set_color(canvas, ColorWhite);
        canvas_draw_box(canvas, 10, 23, 108, 18);
        canvas_set_color(canvas, ColorBlack);
        canvas_draw_xbm(canvas, 14, 27, 100, 10, pic_puzzled);
    }
}

static void game_event_handler(GameEvent const event) {
    if(event.type == EventTypeKey) {
        if(event.input.type == InputTypePress) {
            switch(event.input.key) {
            case InputKeyUp:
                key_stack_push(DirectionUp);
                break;
            case InputKeyDown:
                key_stack_push(DirectionDown);
                break;
            case InputKeyRight:
                key_stack_push(DirectionRight);
                break;
            case InputKeyLeft:
                key_stack_push(DirectionLeft);
                break;
            case InputKeyOk:
                key_stack_push(DirectionNone);
                break;
            case InputKeyBack:
                sandbox_loop_exit();
                break;
            }
        }
    } else if(event.type == EventTypeTick) {
        game_tick();
    }
}

static void game_alloc() {
    srand(DWT->CYCCNT);
    key_stack_init();
    notification = furi_record_open(RECORD_NOTIFICATION);
    notification_message_block(notification, &sequence_display_backlight_enforce_on);
}

static void game_free() {
    notification_message_block(notification, &sequence_display_backlight_enforce_auto);
    furi_record_close(RECORD_NOTIFICATION);
}

int32_t game15_app() {
    game_alloc();
    game_init();
    sandbox_init(
        FPS, (SandboxRenderCallback)render_callback, (SandboxEventHandler)game_event_handler);
    sandbox_loop();
    sandbox_free();
    game_free();
    return 0;
}
