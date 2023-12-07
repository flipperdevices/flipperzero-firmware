#include <furi.h>
#include <stdlib.h>
#include <gui/gui.h>
#include <furi_hal.h>
#include <locale/locale.h>
#include <furi_hal_gpio.h>
#include <furi_hal_random.h>
#include <furi_hal_resources.h>

#define TAG "guess_number_app"

typedef enum {
    DemoEventTypeKey,
} DemoEventType;

typedef struct {
    DemoEventType type;
    InputEvent input;
} DemoEvent;

typedef struct {
    FuriString* buffer;
    int target_number;
    int player_guess;
    char message[50];
    char log1[50];
    char log2[50];
} DemoData;

typedef struct {
    FuriMessageQueue* queue;
    FuriMutex* mutex;
    DemoData* data;
} DemoContext;

static void reset_game(DemoData* data) {
    data->target_number = rand() % 100;
    data->player_guess = 50;
    strcpy(data->message, "Guess a number 0-99");
    strcpy(data->log1, "Press up/down to guess.");
    strcpy(data->log2, "Press ok to check.");
}

static void guess_number_input_callback(InputEvent* input_event, FuriMessageQueue* queue) {
    furi_assert(queue);
    DemoEvent event = {.type = DemoEventTypeKey, .input = *input_event};
    furi_message_queue_put(queue, &event, FuriWaitForever);
}

static void guess_number_render_callback(Canvas* canvas, void* ctx) {
    DemoContext* demo_context = ctx;
    if(furi_mutex_acquire(demo_context->mutex, 200) != FuriStatusOk) {
        return;
    }

    DemoData* data = demo_context->data;
    furi_string_printf(data->buffer, "Your guess: %d", data->player_guess);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(
        canvas, 64, 22, AlignCenter, AlignBottom, furi_string_get_cstr(data->buffer));
    canvas_draw_str_aligned(canvas, 64, 32, AlignCenter, AlignBottom, data->message);

    canvas_draw_str_aligned(canvas, 64, 52, AlignCenter, AlignBottom, data->log1);
    canvas_draw_str_aligned(canvas, 64, 62, AlignCenter, AlignBottom, data->log2);

    furi_mutex_release(demo_context->mutex);
}

static void process_guess(DemoData* data) {
    if(data->player_guess == data->target_number) {
        strcpy(data->message, "Correct!");
        strcpy(data->log1, "Press back to reset.");
        strcpy(data->log2, "Press left to exit.");
    } else if(data->player_guess < data->target_number) {
        strcpy(data->message, "Too low!");
    } else {
        strcpy(data->message, "Too high!");
    }
}

int32_t guess_number_app(void* p) {
    UNUSED(p);

    srand(furi_hal_random_get());
    DemoContext* demo_context = malloc(sizeof(DemoContext));
    demo_context->mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    demo_context->data = malloc(sizeof(DemoData));
    demo_context->data->buffer = furi_string_alloc();
    reset_game(demo_context->data); // Initialize game

    demo_context->queue = furi_message_queue_alloc(8, sizeof(DemoEvent));
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, guess_number_render_callback, demo_context);
    view_port_input_callback_set(view_port, guess_number_input_callback, demo_context->queue);
    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    DemoEvent event;
    bool processing = true;
    do {
        if(furi_message_queue_get(demo_context->queue, &event, FuriWaitForever) == FuriStatusOk) {
            switch(event.type) {
            case DemoEventTypeKey:
                if(event.input.type == InputTypeShort) {
                    if(event.input.key == InputKeyUp) {
                        demo_context->data->player_guess++;
                    } else if(event.input.key == InputKeyDown) {
                        demo_context->data->player_guess--;
                    } else if(event.input.key == InputKeyOk) {
                        process_guess(demo_context->data);
                    } else if(event.input.key == InputKeyBack) {
                        reset_game(demo_context->data);
                    } else if(event.input.key == InputKeyLeft) {
                        processing = false;
                    }
                }
                break;
            default:
                break;
            }
            view_port_update(view_port);
        } else {
            processing = false;
        }
    } while(processing);

    view_port_enabled_set(view_port, false);
    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_record_close(RECORD_GUI);
    furi_message_queue_free(demo_context->queue);
    furi_mutex_free(demo_context->mutex);
    furi_string_free(demo_context->data->buffer);
    free(demo_context->data);
    free(demo_context);

    return 0;
}