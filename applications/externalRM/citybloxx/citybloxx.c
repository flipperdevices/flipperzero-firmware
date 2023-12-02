#include <furi.h>
#include <furi_hal.h>

#include <gui/gui.h>
#include <input/input.h>
#include <notification/notification.h>
#include <notification/notification_messages.h>

#include <math.h>

#include "types.h"
#include "blocks.h"
#include "manipulations.h"

#define TICK_VALUE 0.002
#define SIN_MULTIPLIER 10
#define ROTATION (sin(ticks) * SIN_MULTIPLIER)
#define ROTATION_LAST (sin(ticks - TICK_VALUE) * SIN_MULTIPLIER)
#define ROTATION_DIFF (ROTATION - ROTATION_LAST)

#define G_CONSTANT 9.8

#define BLOCK_COUNT 3

bool exiting = false;
bool game_over;

Block* block_list;
uint16_t block_list_i;

int8_t balance_base;
int32_t balance_sum;
uint16_t balance_count;
#define BALANCE_VALUE ((balance_sum / balance_count) - balance_base)

const int32_t base = 80;
const uint8_t house_block_height = 30;

Point top;
Point bottom;
Point ground_point;

Block falling_block;

float ticks;
float fall_tick;

float x_velocity;

uint16_t score;

static void inc_tick(void* ctx) {
    UNUSED(ctx);

    ticks += TICK_VALUE;
}

static void fall(void* ctx) {
    UNUSED(ctx);

    line_group_translate(
        falling_block.lines, falling_block.count, x_velocity, G_CONSTANT * (ticks - fall_tick));

    Point center = line_group_get_center(falling_block.lines, falling_block.count);
    if(center.y >= base + house_block_height / 2) {
        // If we hit the ground or another block
        score++;

        if(block_list_i == BLOCK_COUNT) {
            free(block_list[0].lines);
            for(uint16_t i = 0; i < block_list_i - 1; i++) block_list[i] = block_list[i + 1];
            block_list_i--;
        }
        if(block_list_i == 0) {
            // First block ever
            balance_base = center.x;
        } else {
            // Checking if we are misaligned
            Point center_last = line_group_get_center(
                block_list[block_list_i - 1].lines, block_list[block_list_i - 1].count);
            if(abs(center_last.x - center.x) > 6) {
                // Playing the game over sound
                NotificationApp* notification_app = furi_record_open(RECORD_NOTIFICATION);
                notification_message(notification_app, &sequence_error);
                furi_record_close(RECORD_NOTIFICATION);

                game_over = true;
            }
        }
        balance_sum += center.x;
        balance_count++;
        line_group_translate(
            falling_block.lines,
            falling_block.count,
            0,
            (base + house_block_height / 2) - center.y);
        block_list[block_list_i] = falling_block;
        falling_block.count = 0;
        block_list_i++;
    }
}

static void render_line(Canvas* canvas, Line line) {
    canvas_draw_line(canvas, line.a.x, line.a.y, line.b.x, line.b.y);
}

static void render_line_group(Canvas* canvas, Line* line_group, uint8_t count) {
    for(uint8_t i = 0; i < count; i++) render_line(canvas, line_group[i]);
}

static Block get_house_i() {
    const float rotation = ROTATION;

    Block house = get_house();
    line_group_translate(house.lines, house.count, bottom.x, bottom.y);
    line_group_rotate(house.lines, house.count, top, rotation);
    return house;
}

static Block get_crane_i() {
    const float rotation = ROTATION;

    Block crane = get_crane();
    line_group_translate(crane.lines, crane.count, top.x, top.y);
    line_group_rotate(crane.lines, crane.count, top, rotation);
    return crane;
}

static Block get_ground_i() {
    Block ground = get_ground();
    line_group_translate(ground.lines, ground.count, ground_point.x, ground_point.y);
    return ground;
}

static void release_block() {
    if(falling_block.count != 0) return;
    ground_point.y += 30;

    fall_tick = ticks;

    const float rotation = ROTATION;
    x_velocity = ROTATION_DIFF * -100; // The error here is insignificant enough

    falling_block = get_house_i();
    line_group_rotate_center(falling_block.lines, falling_block.count, -rotation);

    for(uint16_t i = 0; i < block_list_i; i++)
        line_group_translate(block_list[i].lines, block_list[i].count, 0, house_block_height);
}

// Screen is 128x64 px
static void app_draw_callback(Canvas* canvas, void* ctx) {
    UNUSED(ctx);

    canvas_set_font(canvas, FontPrimary);

    if(exiting) return;

    if(game_over) {
        canvas_draw_str(canvas, 2, 70, "Game over");
        return;
    }

    canvas_clear(canvas);

    canvas_draw_frame(canvas, 0, 0, 64, 128);

    if(falling_block.count == 0) {
        Block house = get_house_i();
        render_line_group(canvas, house.lines, house.count);
        free(house.lines);
    }

    Block crane = get_crane_i();
    render_line_group(canvas, crane.lines, crane.count);
    free(crane.lines);

    if(ground_point.y < 128) {
        Block ground = get_ground_i();
        render_line_group(canvas, ground.lines, ground.count);
        free(ground.lines);
    }

    if(falling_block.count != 0)
        render_line_group(canvas, falling_block.lines, falling_block.count);

    const Point rot_base = {balance_base, base};

    Block* block_list_copy = malloc(sizeof(Block) * BLOCK_COUNT);
    const int16_t deg = BALANCE_VALUE / 4;
    for(uint16_t i = 0; i < block_list_i; i++) {
        block_list_copy[i] = block_list[i];
        size_t size = sizeof(Line) * HOUSE_LINE_CO;
        block_list_copy[i].lines = malloc(size);
        memcpy(block_list_copy[i].lines, block_list[i].lines, size);
        line_group_rotate(block_list_copy[i].lines, block_list_copy[i].count, rot_base, deg);
    }

    for(uint16_t i = 0; i < block_list_i; i++)
        render_line_group(canvas, block_list_copy[i].lines, block_list_copy[i].count);

    for(uint16_t i = 0; i < block_list_i; i++) free(block_list_copy[i].lines);
    free(block_list_copy);

    FuriString* str = furi_string_alloc();
    furi_string_printf(str, "%hu", score);
    canvas_draw_str(canvas, 4, 16, furi_string_get_cstr(str));
    furi_string_free(str);
}

static void app_input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);

    FuriMessageQueue* event_queue = ctx;
    furi_message_queue_put(event_queue, input_event, FuriWaitForever);
}

static void restart() {
    block_list = malloc(sizeof(Block) * BLOCK_COUNT);

    block_list_i = 0;

    balance_base = 0;
    balance_sum = 0;
    balance_count = 0;

    top.x = 32;
    top.y = 0;
    bottom.x = top.x;
    bottom.y = top.y + 15;
    ground_point.x = 32;
    ground_point.y = base;

    falling_block.count = 0;
    falling_block.lines = NULL;

    ticks = 0;
    fall_tick = 0;

    x_velocity = 0;

    score = 0;

    game_over = false;
}

int32_t citybloxx_main(void* p) {
    UNUSED(p);

    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));

    // Configure view port
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, app_draw_callback, view_port);
    view_port_input_callback_set(view_port, app_input_callback, event_queue);
    view_port_set_orientation(view_port, ViewPortOrientationVertical);

    // Register view port in GUI
    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    InputEvent event;

    restart();

    FuriTimer* timer = furi_timer_alloc(inc_tick, FuriTimerTypePeriodic, NULL);
    furi_timer_start(timer, 1);
    FuriTimer* fall_timer = furi_timer_alloc(fall, FuriTimerTypePeriodic, NULL);
    furi_timer_start(fall_timer, 100);

    bool running = true;
    while(running) {
        if(furi_message_queue_get(event_queue, &event, 100) == FuriStatusOk) {
            if(event.type == InputTypeLong && event.key == InputKeyBack) running = false;
            if(event.type == InputTypePress && event.key == InputKeyOk)
                (game_over ? restart() : release_block());
        }
        view_port_update(view_port);
    }

    exiting = true;

    for(uint16_t i = 0; i < block_list_i; i++) free(block_list[i].lines);
    free(block_list);

    furi_timer_stop(timer);
    furi_timer_free(timer);
    furi_timer_stop(fall_timer);
    furi_timer_free(fall_timer);

    view_port_enabled_set(view_port, false);
    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_message_queue_free(event_queue);

    furi_record_close(RECORD_GUI);

    return 0;
}
