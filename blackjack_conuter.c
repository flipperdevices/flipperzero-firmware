#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <input/input.h>
#include <notification/notification.h>
#include <notification/notification_messages.h>

#include <stdio.h>

#define HOT_COUNT_THRESHOLD 4
#define BLACKJACK_MENU_TITLE "BlackJack Counter"

int count = 0;

static void vibration_hot_count() {
    char notification_message[50];
    strcpy(notification_message, "Hot Count Threshold!");
    furi_hal_vibro_on(false);
    furi_hal_vibro_on(true);
    furi_delay_ms(200);
    furi_hal_vibro_on(false);
    strcpy(notification_message, "");
    return;
}


static void app_draw_callback(Canvas* canvas, void* ctx) {
    UNUSED(ctx);

    canvas_clear(canvas);
    
    canvas_set_color(canvas, ColorBlack);
    canvas_set_bitmap_mode(canvas, 1);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 15, 8, BLACKJACK_MENU_TITLE);
    
    char count_str[5];
    snprintf(count_str, 5, "%d", count);
    canvas_set_bitmap_mode(canvas, 1);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 60, 20, count_str);

    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 12, 33, "From 2 to 7 (Press Up)");
    canvas_draw_str(canvas, 17, 43, "For 8 & 9 (Press Ok)");
    canvas_draw_str(canvas, 3, 53, "From 10 to Ace (Press Down)");

    canvas_draw_str(canvas, -1, 63, "Wait vibration when count's hot");
}


static void app_input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);

    FuriMessageQueue* event_queue = ctx;
    furi_message_queue_put(event_queue, input_event, FuriWaitForever);
}


int32_t blackjack_counter_main(void* p) {
    UNUSED(p);
    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));
    
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, app_draw_callback, view_port);
    view_port_input_callback_set(view_port, app_input_callback, event_queue);
    
    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    InputEvent event;

    bool running = true;
    while (running) {
        if (furi_message_queue_get(event_queue, &event, 100) == FuriStatusOk) {
            if ((event.type == InputTypePress) || (event.type == InputTypeRepeat)) {
                switch (event.key) {
                    case InputKeyUp:
                        count += 1;
                        break;
                    case InputKeyOk:
                        break;
                    case InputKeyDown:
                        count -= 1;
                        break;
                    default:
                        running = false;
                        break;
                }
                
                if (count >= HOT_COUNT_THRESHOLD || count <= -HOT_COUNT_THRESHOLD) {
                    vibration_hot_count();
                }
            }
            view_port_update(view_port);
        }
    }

    view_port_enabled_set(view_port, false);
    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_message_queue_free(event_queue);

    furi_record_close(RECORD_GUI);

    return 0;
}
