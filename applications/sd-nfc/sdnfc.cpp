#include "app-template.h"

// event enumeration type
typedef uint8_t event_t;

// app state class
class AppSdNFCState {
public:
    // state data
    uint8_t example_data;

    // state initializer
    AppSdNFCState() {
        example_data = 12;
    }
};

// app events class
class AppSdNFCEvent {
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
class AppSdNFC : public AppTemplate<AppSdNFCState, AppSdNFCEvent> {
public:
    void run();
    void render(CanvasApi* canvas);
};

// start app
void AppSdNFC::run() {
    AppSdNFCEvent event;
    while(1) {
        if(get_event(&event, 1000)) {
            if(event.type == AppSdNFCEvent::EventTypeKey) {
                // press events
                if(event.value.input.state && event.value.input.input == InputBack) {
                    printf("bye!\n");
                    exit();
                }

                if(event.value.input.state && event.value.input.input == InputUp) {
                    // to read or write state you need to execute
                    // acquire modify release state
                    acquire_state();
                    state.example_data = 24;
                    release_state();
                }
            }
        }

        // signal to force gui update
        update_gui();
    };
}

// render app
void AppSdNFC::render(CanvasApi* canvas) {
    // here you dont need to call acquire_state or release_state
    // to read or write app state, that already handled by caller
    canvas->set_color(canvas, ColorBlack);
    canvas->set_font(canvas, FontPrimary);
    canvas->draw_str(canvas, 2, state.example_data, "Example app");
}

// app enter function
extern "C" void sdnfc(void* p) {
    AppSdNFC* app = new AppSdNFC();
    app->run();
}