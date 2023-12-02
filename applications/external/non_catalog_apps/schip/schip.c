#include <furi.h>
#include <furi_hal.h>

#include <gui/gui.h>
#include <input/input.h>
#include <dialogs/dialogs.h>
#include <storage/storage.h>

#define TAG "schip"
#define FONT_SIZE 180
#define MEM_SIZE 0x1000
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define REGISTERS_COUNT 16
#define STACK_SIZE 16
#define KEYPAD_WIDTH 4
#define KEYPAD_HEIGHT 4
#define BUTTONS_COUNT 16
#define ROM_START 0x200
#define FILES_PATH "/ext/schip"
#define FILE_EXTENSION ".ch8"
#define RPL_COUNT 8

const uint8_t font[FONT_SIZE] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80, // F
    0x3C, 0x7E, 0xE7, 0xC3, 0xC3, 0xC3, 0xC3, 0xE7, 0x7E, 0x3C, // 0h
    0x18, 0x38, 0x58, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3C, // 1h
    0x3E, 0x7F, 0xC3, 0x06, 0x0C, 0x18, 0x30, 0x60, 0xFF, 0xFF, // 2h
    0x3C, 0x7E, 0xC3, 0x03, 0x0E, 0x0E, 0x03, 0xC3, 0x7E, 0x3C, // 3h
    0x06, 0x0E, 0x1E, 0x36, 0x66, 0xC6, 0xFF, 0xFF, 0x06, 0x06, // 4h
    0xFF, 0xFF, 0xC0, 0xC0, 0xFC, 0xFE, 0x03, 0xC3, 0x7E, 0x3C, // 5h
    0x3E, 0x7C, 0xC0, 0xC0, 0xFC, 0xFE, 0xC3, 0xC3, 0x7E, 0x3C, // 6h
    0xFF, 0xFF, 0x03, 0x06, 0x0C, 0x18, 0x30, 0x60, 0x60, 0x60, // 7h
    0x3C, 0x7E, 0xC3, 0xC3, 0x7E, 0x7E, 0xC3, 0xC3, 0x7E, 0x3C, // 8h
    0x3C, 0x7E, 0xC3, 0xC3, 0x7F, 0x3F, 0x03, 0x03, 0x3E, 0x7C // 9h
};
uint8_t memory[MEM_SIZE];
bool screen[SCREEN_WIDTH][SCREEN_HEIGHT];
bool hires = false;
uint8_t registers[REGISTERS_COUNT];
uint16_t I = 0;
uint16_t PC = ROM_START;
uint8_t time_delay = 0;
uint8_t time_sound = 0;
uint16_t stack[STACK_SIZE];
uint8_t stack_pointer;
uint8_t rpl[RPL_COUNT];

bool keyboard_open = false;
uint8_t keyboard_x = 0;
uint8_t keyboard_y = 0;

bool buttons[BUTTONS_COUNT];
uint8_t keypad[BUTTONS_COUNT] = {1, 2, 3, 12, 4, 5, 6, 13, 7, 8, 9, 14, 10, 0, 11, 15};

bool run = true;

FuriString* file_path;

static void app_draw_callback(Canvas* canvas, void* ctx) {
    UNUSED(ctx);

    canvas_clear(canvas);
    canvas_set_font(canvas, FontSecondary);
    if(keyboard_open) {
        for(uint8_t x = 0; x < KEYPAD_WIDTH; x++)
            for(uint8_t y = 0; y < KEYPAD_HEIGHT; y++)
                canvas_draw_frame(canvas, 32 + x * 16, y * 16, 16, 16);
        canvas_draw_frame(canvas, 33 + keyboard_x * 16, 1 + keyboard_y * 16, 14, 14);

        canvas_draw_str(canvas, 38 + 16 * 0, 12 + 16 * 0, "1");
        canvas_draw_str(canvas, 38 + 16 * 1, 12 + 16 * 0, "2");
        canvas_draw_str(canvas, 38 + 16 * 2, 12 + 16 * 0, "3");
        canvas_draw_str(canvas, 38 + 16 * 3, 12 + 16 * 0, "C");
        canvas_draw_str(canvas, 38 + 16 * 0, 12 + 16 * 1, "4");
        canvas_draw_str(canvas, 38 + 16 * 1, 12 + 16 * 1, "5");
        canvas_draw_str(canvas, 38 + 16 * 2, 12 + 16 * 1, "6");
        canvas_draw_str(canvas, 38 + 16 * 3, 12 + 16 * 1, "D");
        canvas_draw_str(canvas, 38 + 16 * 0, 12 + 16 * 2, "7");
        canvas_draw_str(canvas, 38 + 16 * 1, 12 + 16 * 2, "8");
        canvas_draw_str(canvas, 38 + 16 * 2, 12 + 16 * 2, "9");
        canvas_draw_str(canvas, 38 + 16 * 3, 12 + 16 * 2, "E");
        canvas_draw_str(canvas, 38 + 16 * 0, 12 + 16 * 3, "A");
        canvas_draw_str(canvas, 38 + 16 * 1, 12 + 16 * 3, "0");
        canvas_draw_str(canvas, 38 + 16 * 2, 12 + 16 * 3, "B");
        canvas_draw_str(canvas, 38 + 16 * 3, 12 + 16 * 3, "F");
    } else {
        for(uint8_t x = 0; x < SCREEN_WIDTH; x++)
            for(uint8_t y = 0; y < SCREEN_HEIGHT; y++)
                if(screen[x][y]) canvas_draw_dot(canvas, x, y);
    }
}

static void app_input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);

    FuriMessageQueue* event_queue = ctx;
    furi_message_queue_put(event_queue, input_event, FuriWaitForever);
}

static bool nthbit(uint8_t n, uint8_t k) {
    return (n & (1 << k)) >> k == 1;
}

static bool toggle_pixel(uint8_t x, uint8_t y) {
    bool collision = false;
    if(hires) {
        collision = screen[x][y];
        screen[x][y] = !screen[x][y];
    } else {
        collision = screen[x * 2][y * 2];
        screen[x * 2][y * 2] = !screen[x * 2][y * 2];
        screen[x * 2 + 1][y * 2] = !screen[x * 2 + 1][y * 2];
        screen[x * 2][y * 2 + 1] = !screen[x * 2][y * 2 + 1];
        screen[x * 2 + 1][y * 2 + 1] = !screen[x * 2 + 1][y * 2 + 1];
    }
    return collision;
}

// TODO fix this:
/*
: draw-title
	draw-x := 16
	draw-y := 16
	dir := 32
	i := sprite-title
	loop
		sprite draw-x draw-y 0
		i += dir
		draw-y += 16
		if draw-y > TITLE_HEIGHT then draw-x += 16
		if draw-y > TITLE_HEIGHT then draw-y := 16
		if draw-x != TITLE_WIDTH then
	again
	dir := key
;
*/

FuriTimer* timer;
static void tick(void* context) {
    UNUSED(context);
    if(!run) return;
    furi_check(PC < MEM_SIZE - 1);
    uint8_t d1 = memory[PC] / 16;
    uint8_t d2 = memory[PC] % 16;
    uint8_t d3 = memory[PC + 1] / 16;
    uint8_t d4 = memory[PC + 1] % 16;
    uint16_t d5 = d2 * 256 + d3 * 16 + d4; // I
    uint16_t d6 = d1 * 4096 + d2 * 256 + d3 * 16 + d4; // Whole instruction
    // FURI_LOG_T(TAG, "Executing instruction %x at address %x", d6, PC);
    uint8_t d7 = d3 * 16 + d4; // Byte
    bool next = true;

    if(d1 == 0x0 && d2 == 0x0 && d3 == 0xc) {
        // TODO: scroll down d4 px
    } else if(d6 == 0x00e0) {
        for(uint8_t x = 0; x < SCREEN_WIDTH; x++)
            for(uint8_t y = 0; y < SCREEN_HEIGHT; y++) screen[x][y] = false;
    } else if(d6 == 0x00ee) {
        PC = stack[stack_pointer--];
    } else if(d6 == 0x00fb) {
        // TODO: scroll right 4 px
    } else if(d6 == 0x00fc) {
        // TODO: scroll left 4 px
    } else if(d6 == 0x00fd) {
        run = false;
    } else if(d6 == 0x00fe) {
        hires = false;
    } else if(d6 == 0x00ff) {
        hires = true;
    } else if(d1 == 0x0 || d1 == 0x1) {
        PC = d5;
        next = false;
    } else if(d1 == 0x2) {
        stack[++stack_pointer] = PC;
        PC = d5;
        next = false;
    } else if(d1 == 0x3) {
        if(registers[d2] == d7) PC += 2;
    } else if(d1 == 0x4) {
        if(registers[d2] != d7) PC += 2;
    } else if(d1 == 0x5) {
        if(registers[d2] == registers[d3]) PC += 2;
    } else if(d1 == 0x6) {
        registers[d2] = d7;
    } else if(d1 == 0x7) {
        registers[d2] += d7;
    } else if(d1 == 0x8 && d4 == 0x0) {
        registers[d2] = registers[d3];
    } else if(d1 == 0x8 && d4 == 0x1) {
        registers[d2] |= registers[d3];
    } else if(d1 == 0x8 && d4 == 0x2) {
        registers[d2] &= registers[d3];
    } else if(d1 == 0x8 && d4 == 0x3) {
        registers[d2] ^= registers[d3];
    } else if(d1 == 0x8 && d4 == 0x4) {
        uint16_t v = registers[d2] + registers[d3];
        registers[0xf] = v > 0xff;
        registers[d2] = v % 0x100;
    } else if(d1 == 0x8 && d4 == 0x5) {
        registers[0xf] = registers[d2] > registers[d3];
        registers[d2] -= registers[d3];
    } else if(d1 == 0x8 && d4 == 0x6) {
        registers[0xf] = registers[d2] % 2 == 1;
        registers[d2] >>= 1;
    } else if(d1 == 0x8 && d4 == 0x7) {
        registers[0xf] = registers[d3] > registers[d2];
        registers[d2] = registers[d3] - registers[d2];
    } else if(d1 == 0x8 && d4 == 0xe) {
        registers[0xf] = registers[d2] / 128 == 1;
        registers[d2] <<= 1;
    } else if(d1 == 0x9) {
        if(registers[d2] != registers[d3]) PC += 2;
    } else if(d1 == 0xa) {
        I = d5;
    } else if(d1 == 0xb) {
        PC = d5 + registers[0x0];
        next = false;
    } else if(d1 == 0xc) {
        registers[d2] = (rand() % 0x100) & d7;
    } else if(d1 == 0xd) {
        bool collision = false;
        if(d4 == 0)
            for(uint8_t i = 0; i < 16; i++) {
                for(uint8_t j = 0; j < 8; j++)
                    if(nthbit(memory[I + i * 2], 7 - j))
                        collision |= toggle_pixel(registers[d2] + j, registers[d3] + i);
                for(uint8_t j = 0; j < 8; j++)
                    if(nthbit(memory[I + i * 2 + 1], 7 - j))
                        collision |= toggle_pixel(registers[d2] + 8 + j, registers[d3] + i);
            }
        else
            for(uint8_t i = 0; i < d4; i++) {
                for(uint8_t j = 0; j < 8; j++)
                    if(nthbit(memory[I + i], 7 - j))
                        collision |= toggle_pixel(registers[d2] + j, registers[d3] + i);
            }
        registers[0xf] = collision;
    } else if(d1 == 0xe && d7 == 0x9e) {
        if(buttons[registers[d2]]) PC += 2;
    } else if(d1 == 0xe && d7 == 0xa1) {
        if(!buttons[registers[d2]]) PC += 2;
    } else if(d1 == 0xf && d7 == 0x07) {
        registers[d2] = time_delay;
    } else if(d1 == 0xf && d7 == 0x0a) {
        next = false;
        for(uint8_t i = 0; i < BUTTONS_COUNT; i++)
            if(buttons[i]) {
                next = true;
                registers[d2] = i;
                break;
            }
    } else if(d1 == 0xf && d7 == 0x15) {
        time_delay = registers[d2];
    } else if(d1 == 0xf && d7 == 0x18) {
        time_sound = registers[d2];
    } else if(d1 == 0xf && d7 == 0x1e) {
        I += registers[d2];
    } else if(d1 == 0xf && d7 == 0x29) {
        I = 5 * registers[d2];
    } else if(d1 == 0xf && d7 == 0x30) {
        I = 10 * (registers[d2] - 0x10) + 80;
    } else if(d1 == 0xf && d7 == 0x33) {
        memory[I] = registers[d2] / 100;
        memory[I + 1] = (registers[d2] / 10) % 10;
        memory[I + 2] = registers[d2] % 10;
    } else if(d1 == 0xf && d7 == 0x55) {
        for(uint8_t i = 0; i <= d2; i++) memory[I + i] = registers[i];
    } else if(d1 == 0xf && d7 == 0x65) {
        for(uint8_t i = 0; i <= d2; i++) registers[i] = memory[I + i];
    } else if(d1 == 0xf && d7 == 0x75) {
        for(uint8_t i = 0; i <= d2; i++) rpl[I + i] = registers[i];
    } else if(d1 == 0xf && d7 == 0x85) {
        for(uint8_t i = 0; i <= d2; i++) registers[i] = rpl[I + i];
    }

    if(next) PC += 2;
}

FuriTimer* timer_delay;
static void tick_delay(void* context) {
    UNUSED(context);
    if(time_delay > 0) time_delay--;
}

FuriTimer* timer_sound;
//TODO: fix sound
/*bool speaker_on = false;
static void tick_sound(void* context) {
	UNUSED(context);
	if(time_sound > 0) time_sound--;
	if(time_sound > 0 && !speaker_on) {
		furi_hal_speaker_start(800, 1.f);
		speaker_on = true;
	} else if(time_sound == 0 && speaker_on) {
		furi_hal_speaker_stop();
		speaker_on = false;
	}
}*/
static void tick_sound(void* context) {
    UNUSED(context);
    if(time_sound > 0) time_sound--;
    if(time_sound > 0)
        furi_hal_vibro_on(true);
    else
        furi_hal_vibro_on(false);
}

int32_t schip_main(void* p) {
    for(int i = 0; i < FONT_SIZE; i++) memory[i] = font[i];

    UNUSED(p);
    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));

    // Configure view port
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, app_draw_callback, view_port);
    view_port_input_callback_set(view_port, app_input_callback, event_queue);

    // Register view port in GUI
    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    InputEvent event;

    // void* speaker = (void*)furi_hal_speaker_acquire(1000);

    Storage* storage = furi_record_open(RECORD_STORAGE);
    storage_simply_mkdir(storage, FILES_PATH);

    file_path = furi_string_alloc_set_str(FILES_PATH);
    DialogsFileBrowserOptions browser_options;
    dialog_file_browser_set_basic_options(&browser_options, FILE_EXTENSION, NULL);
    browser_options.base_path = FILES_PATH;
    DialogsApp* dialogs = furi_record_open(RECORD_DIALOGS);
    bool running = dialog_file_browser_show(dialogs, file_path, file_path, &browser_options);
    furi_record_close(RECORD_DIALOGS);

    if(running) {
        File* file = storage_file_alloc(storage);
        furi_check(storage_file_open(
            file, furi_string_get_cstr(file_path), FSAM_READ, FSOM_OPEN_EXISTING));
        uint8_t read_byte;
        uint16_t read_size;
        uint16_t p = ROM_START;
        do {
            read_size = storage_file_read(file, &read_byte, 1);
            memory[p] = read_byte;
            p++;

        } while(read_size > 0);
        storage_file_close(file);
        storage_file_free(file);
    }

    furi_record_close(RECORD_STORAGE);

    // We have to round up to 17 here

    timer = furi_timer_alloc(tick, FuriTimerTypePeriodic, NULL);
    furi_timer_start(timer, 1);

    timer_delay = furi_timer_alloc(tick_delay, FuriTimerTypePeriodic, NULL);
    furi_timer_start(timer_delay, 17);

    timer_sound = furi_timer_alloc(tick_sound, FuriTimerTypePeriodic, NULL);
    furi_timer_start(timer_sound, 17);

    while(running) {
        if(furi_message_queue_get(event_queue, &event, 100) == FuriStatusOk) {
            if(event.type == InputTypeLong && event.key == InputKeyBack) running = false;
            if(event.type == InputTypePress && event.key == InputKeyBack)
                keyboard_open = !keyboard_open;
            if(keyboard_open) {
                if(event.type == InputTypePress && event.key == InputKeyLeft) keyboard_x--;
                if(event.type == InputTypePress && event.key == InputKeyDown) keyboard_y++;
                if(event.type == InputTypePress && event.key == InputKeyRight) keyboard_x++;
                if(event.type == InputTypePress && event.key == InputKeyUp) keyboard_y--;

                keyboard_x %= KEYPAD_WIDTH;
                keyboard_y %= KEYPAD_HEIGHT;

                if(event.type == InputTypePress && event.key == InputKeyOk)
                    buttons[keypad[keyboard_y * 4 + keyboard_x]] = true;
                if(event.type == InputTypeRelease && event.key == InputKeyOk)
                    buttons[keypad[keyboard_y * 4 + keyboard_x]] = false;
            } else {
                if(event.type == InputTypePress && event.key == InputKeyUp) buttons[5] = true;
                if(event.type == InputTypeRelease && event.key == InputKeyUp) buttons[5] = false;
                if(event.type == InputTypePress && event.key == InputKeyLeft) buttons[7] = true;
                if(event.type == InputTypeRelease && event.key == InputKeyLeft) buttons[7] = false;
                if(event.type == InputTypePress && event.key == InputKeyDown) buttons[8] = true;
                if(event.type == InputTypeRelease && event.key == InputKeyDown) buttons[8] = false;
                if(event.type == InputTypePress && event.key == InputKeyRight) buttons[9] = true;
                if(event.type == InputTypeRelease && event.key == InputKeyRight)
                    buttons[9] = false;
                if(event.type == InputTypePress && event.key == InputKeyOk) buttons[6] = true;
                if(event.type == InputTypeRelease && event.key == InputKeyOk) buttons[6] = false;
            }
        }
        view_port_update(view_port);
    }

    furi_timer_stop(timer);
    furi_timer_free(timer);

    furi_timer_stop(timer_delay);
    furi_timer_free(timer_delay);

    furi_timer_stop(timer_sound);
    furi_timer_free(timer_sound);

    // furi_hal_speaker_release(speaker);

    view_port_enabled_set(view_port, false);
    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_message_queue_free(event_queue);

    furi_record_close(RECORD_GUI);

    return 0;
}
