#include "../gb_cartridge_app.h"
#include <furi.h>
#include <furi_hal.h>
#include <input/input.h>
#include <gui/elements.h>
#include <notification/notification_messages.h>

struct GBCartridgeStartscreen {
    View* view;
    GBCartridgeStartscreenCallback callback;
    void* context;
};


typedef struct {
    int some_value;
} GBCartridgeStartscreenModel;

static const NotificationSequence sequence_alarm = {
    // &message_note_c5,
    // &message_delay_100,
    &message_sound_off,
    // &message_note_c7,
    // &message_delay_500,
    &message_sound_off,
    NULL,
};

void gb_cartridge_startscreen_set_callback(
    GBCartridgeStartscreen* instance,
    GBCartridgeStartscreenCallback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(callback);
    instance->callback = callback;
    instance->context = context;
}
// void screen_gameboy_connect(Canvas* const canvas) {
//     canvas_draw_frame(canvas, 0, 0, 128, 64);
//     canvas_draw_icon(canvas, 1, 21, &I_Connect_me_62x31);
//     canvas_draw_icon(canvas, 0, 53, &I_Background_128x11);
//     canvas_draw_icon(canvas, 80, 0, &I_game_boy);
//     canvas_draw_icon(canvas, 8, 2, &I_Space_65x18);
//     canvas_draw_str(canvas, 18, 13, "Connect GB");
// }
// void screen_gameboy_connected(Canvas* const canvas) {
//     canvas_draw_frame(canvas, 0, 0, 128, 64);
//     canvas_draw_icon(canvas, 1, 21, &I_Connected_62x31);
//     canvas_draw_icon(canvas, 0, 53, &I_Background_128x11);
//     canvas_draw_icon(canvas, 80, 0, &I_game_boy);
//     canvas_draw_icon(canvas, 8, 2, &I_Space_65x18);
//     canvas_draw_str(canvas, 18, 13, "Connected!");
// }
void gb_cartridge_startscreen_draw(Canvas* canvas, GBCartridgeStartscreenModel* model) {
    UNUSED(model);
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    
    
    canvas_draw_icon(canvas, 0 /*128/2 - (42/2)*/, 64/2 - (64/2), &I_cartridge_42x64);

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 64 + 20, 6, AlignCenter, AlignTop, "Connect Malveke"); 
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_aligned(canvas, 64+ 20, 18, AlignCenter, AlignTop, "to the Flipper and"); 
    canvas_draw_str_aligned(canvas, 64+ 20, 28, AlignCenter, AlignTop, "and Insert Game Boy");
    canvas_draw_str_aligned(canvas, 64+ 20, 38, AlignCenter, AlignTop, "Cartridge");
    elements_button_center(canvas, "Ok"); 
}

static void gb_cartridge_startscreen_model_init(GBCartridgeStartscreenModel* const model) {
    model->some_value = 1;
}

bool gb_cartridge_startscreen_input(InputEvent* event, void* context) {
    furi_assert(context); 
    GBCartridgeStartscreen* instance = context;
    if (event->type == InputTypeRelease) {
        switch(event->key) {
            case InputKeyBack:
                with_view_model(
                    instance->view,
                    GBCartridgeStartscreenModel * model,
                    {
                        UNUSED(model);
                        instance->callback(GBCartridgeCustomEventStartscreenBack, instance->context);
                    },
                    true);
                break;
            case InputKeyLeft:
            case InputKeyRight:
            case InputKeyUp:
            case InputKeyDown:
            case InputKeyOk:
                notification_message(((GBCartridge*)instance->context)->notification, &sequence_alarm);
                with_view_model(
                    instance->view,
                    GBCartridgeStartscreenModel* model,
                    {
                        UNUSED(model);
                        instance->callback(GBCartridgeCustomEventStartscreenOk, instance->context);
                    },
                    true);
                break;
            case InputKeyMAX:
                break;
        }
    }
    return true;
}

void gb_cartridge_startscreen_exit(void* context) {
    furi_assert(context);
}

void gb_cartridge_startscreen_enter(void* context) {
    furi_assert(context);
    GBCartridgeStartscreen* instance = (GBCartridgeStartscreen*)context;
    with_view_model(
        instance->view,
        GBCartridgeStartscreenModel * model,
        {
            gb_cartridge_startscreen_model_init(model);
        },
        true
    );
}

GBCartridgeStartscreen* gb_cartridge_startscreen_alloc() {
    GBCartridgeStartscreen* instance = malloc(sizeof(GBCartridgeStartscreen));
    instance->view = view_alloc();
    view_allocate_model(instance->view, ViewModelTypeLocking, sizeof(GBCartridgeStartscreenModel));
    view_set_context(instance->view, instance); // furi_assert crashes in events without this
    view_set_draw_callback(instance->view, (ViewDrawCallback)gb_cartridge_startscreen_draw);
    view_set_input_callback(instance->view, gb_cartridge_startscreen_input);
    //view_set_enter_callback(instance->view, gb_cartridge_startscreen_enter);
    //view_set_exit_callback(instance->view, gb_cartridge_startscreen_exit);

    with_view_model(
        instance->view,
        GBCartridgeStartscreenModel * model,
        {
            gb_cartridge_startscreen_model_init(model);
        },
        true
    );
    
    return instance;
}

void gb_cartridge_startscreen_free(GBCartridgeStartscreen* instance) {
    furi_assert(instance);

    with_view_model(
        instance->view,
        GBCartridgeStartscreenModel * model,
        {
            UNUSED(model);
        },
        true);
    view_free(instance->view);
    free(instance);
}

View* gb_cartridge_startscreen_get_view(GBCartridgeStartscreen* instance) {
    furi_assert(instance);
    return instance->view;
}

