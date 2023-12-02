#ifndef APP_DEFINES_H
#define APP_DEFINES_H

#define GPIO_PIN_COUNT 8
#define ANIMATE_FRAME_TIME_MS 133
#define FRAME_TIME 66.666666

typedef void (*DrawView)(Canvas* canvas, void* ctx);
typedef void (*HandleInput)(InputEvent* event, void* ctx);

typedef enum { MAIN_VIEW, CONFIG_MENU_VIEW } enum_view;

typedef enum {
    GPIO_MODE_INPUT,
    GPIO_MODE_INPUT_PULLUP,
    GPIO_MODE_OUTPUT,
    GPIO_MODE_UNSET
} GpioUserMode;

typedef enum {
    GPIO_VALUE_TRUE,
    GPIO_VALUE_FALSE,
    GPIO_VALUE_INPUT,
    GPIO_VALUE_NONE
} GpioUserValue;

typedef enum { CONFIG_MENU_MODE, CONFIG_MENU_VALUE, CONFIG_MENU_INPUT } ConfigMenuOptions;

typedef struct {
    GpioUserMode mode;
    GpioUserValue value;
    int gp_idx_input;
    bool changed;
    GpioUserMode prev_mode;
} GPIOPinUserSelection;

typedef struct {
    int selected;
    enum_view view;
    int wiggle_frame;
    size_t prev_frame_time;
    size_t elapsed_time;
    double result;
    double freq_var;
    double elapsed_var;
    ConfigMenuOptions config_menu_selected;
} ViewerState;

//  5V  A7  A6  A4  B3  B2  C3 GND SET
//
//
//  3V SWC GND SIO  TX  RX  C1  C0  1W GND

typedef enum {
    PIN_5V = 0,
    PIN_A7,
    PIN_A6,
    PIN_A4,
    PIN_B3,
    PIN_B2,
    PIN_C3,
    GEARIC,
    PIN_3V,
    PIN_SWC,
    PIN_SIO,
    PIN_TX,
    PIN_RX,
    PIN_C1,
    PIN_C0,
    PIN_1W,
    PIN_GND_08,
    PIN_GND_11,
    PIN_GND_18,
    NONE
} enum_view_element;

typedef struct {
    enum_view_element element;
    enum_view_element opposite;
    bool selectable;
    bool editable;
    bool top_row;
    bool pull_out;
    int gp_idx;
    uint8_t x_pos;
    uint8_t y_pos;
    const char* name;
    Icon* icon;
    Icon* selected_icon;
} ViewElement;

typedef struct {
    uint8_t element_idx;
    const GpioPin* pin;
    GpioMode mode;
    GpioPull pull;
    GpioSpeed speed;
    double value;
    const char* name;
    bool unset;
    bool found;
    bool input;
    GPIOPinUserSelection user;
} GPIOPin;

// GPIO enums from firmware/targets/f7/furi_hal/furi_hal_gpio.h

// /**
//  * Gpio modes
//  */
// typedef enum {
//     *GpioModeInput,
//     *GpioModeOutputPushPull,
//     GpioModeOutputOpenDrain,
//     GpioModeAltFunctionPushPull,
//     GpioModeAltFunctionOpenDrain,
//     *GpioModeAnalog,
//     GpioModeInterruptRise,
//     GpioModeInterruptFall,
//     GpioModeInterruptRiseFall,
//     GpioModeEventRise,
//     GpioModeEventFall,
//     GpioModeEventRiseFall,
// } GpioMode;

// /**
//  * Gpio pull modes
//  */
// typedef enum {
//     GpioPullNo,
//     GpioPullUp,
//     GpioPullDown,
// } GpioPull;

#endif