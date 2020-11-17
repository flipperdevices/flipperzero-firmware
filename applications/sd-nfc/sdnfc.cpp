#include "app-template.h"

extern "C" {
#include <rfal_analogConfig.h>
#include <rfal_rf.h>
#include <rfal_nfc.h>
#include <rfal_nfca.h>
#include <st25r3916.h>
#include <st25r3916_irq.h>
}

#include "fatfs/ff.h"
#include "stm32_adafruit_sd.h"

// event enumeration type
typedef uint8_t event_t;

// app state class
class AppSdNFCState {
public:
    // state data
    const char* name;

    // state initializer
    AppSdNFCState() {
        name = "sd nfc test";
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
    GpioPin* red_led_record;
    GpioPin* green_led_record;

    void run();
    void render(CanvasApi* canvas);
    void set_error(const char* text);
    void set_text(const char* text);
};

// start app
void AppSdNFC::run() {
    // create pin
    GpioPin red_led = led_gpio[0];
    GpioPin green_led = led_gpio[1];

    // TODO open record
    red_led_record = &red_led;
    green_led_record = &green_led;

    // configure pin
    gpio_init(red_led_record, GpioModeOutputOpenDrain);
    gpio_init(green_led_record, GpioModeOutputOpenDrain);

    uint8_t rfal_result = rfalNfcInitialize();
    if(rfal_result) {
        set_error("rfal init fail");
    }

    rfal_result = rfalLowPowerModeStart();
    if(rfal_result) {
        set_error("rfal low p fail");
    }

    delay(100);

    uint8_t bsp_result = BSP_SD_Init();
    if(bsp_result) {
        set_error("sd init fail");
    }

    gpio_write(green_led_record, false);
    set_text("all good");

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
                }
            }
        }

        // signal to force gui update
        update_gui();
    };
}

// render app
void AppSdNFC::render(CanvasApi* canvas) {
    canvas->set_color(canvas, ColorBlack);
    canvas->set_font(canvas, FontPrimary);
    canvas->draw_str(canvas, 2, 12, state.name);
}

void AppSdNFC::set_error(const char* text) {
    gpio_write(red_led_record, false);
    set_text(text);
    update_gui();
    while(1)
        ;
}

void AppSdNFC::set_text(const char* text) {
    acquire_state();
    state.name = text;
    release_state();
}

// app enter function
extern "C" void sdnfc(void* p) {
    AppSdNFC* app = new AppSdNFC();
    app->run();
}