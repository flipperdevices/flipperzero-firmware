#pragma once
#include "app-template.h"
#include "ibutton_mode_template.h"

// event enumeration type
typedef uint8_t event_t;

class AppiButtonTestState {
public:
    // state data
    static const uint8_t dallas_address_count = 3;
    uint8_t dallas_address[dallas_address_count][8] = {
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x01, 0x41, 0xCE, 0x67, 0x0F, 0x00, 0x00, 0xB6},
        {0x01, 0xFD, 0x0E, 0x84, 0x01, 0x00, 0x00, 0xDB}};

    uint8_t dallas_address_index;

    static const uint8_t cyfral_address_count = 3;
    uint8_t cyfral_address[cyfral_address_count][4] = {
        {0x00, 0x00, 0x00, 0x00},
        {0xBB, 0xBB, 0x7B, 0xBD},
        {0x7B, 0xDE, 0x7B, 0xDE}};
    uint8_t cyfral_address_index;

    uint8_t mode_index;

    // state initializer
    AppiButtonTestState() {
        mode_index = 0;
        dallas_address_index = 0;
        cyfral_address_index = 0;
    }
};

// events class
class AppiButtonTestEvent {
public:
    // events enum
    static const event_t EventTypeTick = 0;
    static const event_t EventTypeKey = 1;

    // payload
    union {
        InputEvent input;
    } value;

    // event type
    event_t type;
};

// our app derived from base AppTemplate class
// with template variables <state, events>
class AppiButtonTest : public AppTemplate<AppiButtonTestState, AppiButtonTestEvent> {
public:
    const GpioPin* red_led_record;
    const GpioPin* green_led_record;

    static const uint8_t modes_count = 5;
    AppTemplateMode<AppiButtonTestState, AppiButtonTestEvent>* mode[modes_count];

    void run();
    void render(Canvas* canvas);
    void render_dallas_list(Canvas* canvas, AppiButtonTestState* state);
    void render_cyfral_list(Canvas* canvas, AppiButtonTestState* state);

    void blink_red();
    void blink_green();

    void increase_mode();
    void decrease_mode();
    void increase_dallas_address();
    void decrease_dallas_address();
    void increase_cyfral_address();
    void decrease_cyfral_address();
    void switch_to_mode(uint8_t mode_index);
};