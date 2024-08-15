#include "../js_modules.h" // IWYU pragma: keep
#include <furi_hal_gpio.h>
#include <furi_hal_resources.h>
#include <expansion/expansion.h>
#include <limits.h>

typedef struct {
    int dummy;
} JsGpioInst;

typedef struct {
    const GpioPin* pin;
    GpioMode previous_mode;
    GpioPull previous_pull;
} JsGpioPinInst;

/**
 * @brief Initializes a GPIO pin according to the provided mode object
 * 
 * Example usage:
 * 
 * ```js
 * let gpio = require("gpio");
 * let led = gpio.get("pc3");
 * led.init({ direction: "out", outMode: "push_pull" });
 * ```
 */
static void js_gpio_init(struct mjs* mjs) {
    // deconstruct mode object
    mjs_val_t mode_arg = mjs_arg(mjs, 0);
    if(!mjs_is_object(mode_arg)) {
        mjs_prepend_errorf(mjs, MJS_BAD_ARGS_ERROR, "Invalid argument: expected mode object");
        mjs_return(mjs, MJS_UNDEFINED);
        return;
    }
    mjs_val_t direction_arg = mjs_get(mjs, mode_arg, "direction", ~0);
    mjs_val_t out_mode_arg = mjs_get(mjs, mode_arg, "outMode", ~0);
    mjs_val_t in_mode_arg = mjs_get(mjs, mode_arg, "inMode", ~0);
    mjs_val_t edge_arg = mjs_get(mjs, mode_arg, "edge", ~0);
    mjs_val_t pull_arg = mjs_get(mjs, mode_arg, "pull", ~0);

    // get strings
    const char* direction = mjs_get_string(mjs, &direction_arg, NULL);
    const char* out_mode = mjs_get_string(mjs, &out_mode_arg, NULL);
    const char* in_mode = mjs_get_string(mjs, &in_mode_arg, NULL);
    const char* edge = mjs_get_string(mjs, &edge_arg, NULL);
    const char* pull = mjs_get_string(mjs, &pull_arg, NULL);
    if(!direction) {
        mjs_prepend_errorf(
            mjs,
            MJS_BAD_ARGS_ERROR,
            "Invalid argument: expected string in \"direction\" field of mode object");
        mjs_return(mjs, MJS_UNDEFINED);
        return;
    }
    if(!out_mode) out_mode = "open_drain";
    if(!in_mode) in_mode = "plain_digital";
    if(!edge) edge = "rising";

    // convert strings to mode
    // FIXME: make me pretty ^_^
    GpioMode mode;
    if(strcmp(direction, "out") == 0 && strcmp(out_mode, "push_pull") == 0) {
        mode = GpioModeOutputPushPull;
    } else if(strcmp(direction, "out") == 0 && strcmp(out_mode, "open_drain") == 0) {
        mode = GpioModeOutputOpenDrain;
    } else if(strcmp(direction, "in") == 0 && strcmp(in_mode, "analog") == 0) {
        mode = GpioModeAnalog;
    } else if(strcmp(direction, "in") == 0 && strcmp(in_mode, "plain_digital") == 0) {
        mode = GpioModeInput;
    } else if(
        strcmp(direction, "in") == 0 && strcmp(in_mode, "interrupt") == 0 &&
        strcmp(edge, "rising") == 0) {
        mode = GpioModeInterruptRise;
    } else if(
        strcmp(direction, "in") == 0 && strcmp(in_mode, "interrupt") == 0 &&
        strcmp(edge, "falling") == 0) {
        mode = GpioModeInterruptFall;
    } else if(
        strcmp(direction, "in") == 0 && strcmp(in_mode, "interrupt") == 0 &&
        strcmp(edge, "both") == 0) {
        mode = GpioModeInterruptRiseFall;
    } else if(
        strcmp(direction, "in") == 0 && strcmp(in_mode, "event") == 0 &&
        strcmp(edge, "rising") == 0) {
        mode = GpioModeEventRise;
    } else if(
        strcmp(direction, "in") == 0 && strcmp(in_mode, "event") == 0 &&
        strcmp(edge, "falling") == 0) {
        mode = GpioModeEventFall;
    } else if(
        strcmp(direction, "in") == 0 && strcmp(in_mode, "event") == 0 &&
        strcmp(edge, "both") == 0) {
        mode = GpioModeEventRiseFall;
    } else {
        mjs_prepend_errorf(
            mjs,
            MJS_BAD_ARGS_ERROR,
            "Invalid argument: invalid combination of fields in mode object");
        mjs_return(mjs, MJS_UNDEFINED);
        return;
    }

    // convert pull
    GpioPull pull_mode;
    if(!pull) {
        pull_mode = GpioPullNo;
    } else if(strcmp(pull, "up") == 0) {
        pull_mode = GpioPullUp;
    } else if(strcmp(pull, "down") == 0) {
        pull_mode = GpioPullDown;
    } else {
        mjs_prepend_errorf(
            mjs,
            MJS_BAD_ARGS_ERROR,
            "Invalid argument: invalid combination of fields in mode object");
        mjs_return(mjs, MJS_UNDEFINED);
        return;
    }

    // get state
    mjs_val_t manager = mjs_get_this(mjs);
    JsGpioPinInst* manager_data =
        (JsGpioPinInst*)(uint32_t)(mjs_get(mjs, manager, INST_PROP_NAME, ~0) & 0xFFFFFFFF);

    // init GPIO
    furi_hal_gpio_init(manager_data->pin, mode, pull_mode, GpioSpeedVeryHigh);

    mjs_return(mjs, MJS_UNDEFINED);
}

/**
 * @brief Writes a logic value to a GPIO pin 
 * 
 * Example usage:
 * 
 * ```js
 * let gpio = require("gpio");
 * let led = gpio.get("pc3");
 * led.init({ direction: "out", outMode: "push_pull" });
 * led.write(true);
 */
static void js_gpio_write(struct mjs* mjs) {
    // get argument
    mjs_val_t level_arg = mjs_arg(mjs, 0);
    if(!mjs_is_boolean(level_arg)) {
        mjs_prepend_errorf(mjs, MJS_BAD_ARGS_ERROR, "Invalid argument: expected boolean");
        mjs_return(mjs, MJS_UNDEFINED);
        return;
    }
    bool logic_level = mjs_get_bool(mjs, level_arg);

    // get state
    mjs_val_t manager = mjs_get_this(mjs);
    JsGpioPinInst* manager_data =
        (JsGpioPinInst*)(uint32_t)(mjs_get(mjs, manager, INST_PROP_NAME, ~0) & 0xFFFFFFFF);

    // set level
    furi_hal_gpio_write(manager_data->pin, logic_level);

    mjs_return(mjs, MJS_UNDEFINED);
}

/**
 * @brief Reads a logic value from a GPIO pin
 * 
 * Example usage:
 * 
 * ```js
 * let gpio = require("gpio");
 * let button = gpio.get("pc1");
 * button.init({ direction: "in" });
 * if(button.read())
 *     print("hi button!!!!!");
 */
static void js_gpio_read(struct mjs* mjs) {
    // get state
    mjs_val_t manager = mjs_get_this(mjs);
    JsGpioPinInst* manager_data =
        (JsGpioPinInst*)(uint32_t)(mjs_get(mjs, manager, INST_PROP_NAME, ~0) & 0xFFFFFFFF);

    // get level
    bool value = furi_hal_gpio_read(manager_data->pin);

    mjs_return(mjs, mjs_mk_boolean(mjs, value));
}

/**
 * @brief Returns an object that manages a specified pin.
 * 
 * Example usage:
 * 
 * ```js
 * let gpio = require("gpio");
 * let led = gpio.get("pc3");
 * ```
 */
static void js_gpio_get(struct mjs* mjs) {
    mjs_val_t name_arg = mjs_arg(mjs, 0);
    const char* name_string = mjs_get_string(mjs, &name_arg, NULL);
    const GpioPinRecord* pin_record = NULL;

    // parse input argument to a pin pointer
    if(name_string) {
        // find pin with matching name ignoring case
        FuriString* name_fstr = furi_string_alloc();
        furi_string_set(name_fstr, name_string);
        for(size_t i = 0; i < gpio_pins_count; i++) {
            if(furi_string_cmpi_str(name_fstr, gpio_pins[i].name) == 0) {
                pin_record = &gpio_pins[i];
                break;
            }
        }
        furi_string_free(name_fstr);
    } else if(mjs_is_number(name_arg)) {
        // find pin with matching number
        int name_int = mjs_get_int(mjs, name_arg);
        for(size_t i = 0; i < gpio_pins_count; i++) {
            if(name_int == gpio_pins[i].number) {
                pin_record = &gpio_pins[i];
                break;
            }
        }
    } else {
        mjs_prepend_errorf(
            mjs, MJS_BAD_ARGS_ERROR, "Invalid argument: must be either a string or a number");
        mjs_return(mjs, MJS_UNDEFINED);
        return;
    }

    if(!pin_record) {
        mjs_prepend_errorf(mjs, MJS_BAD_ARGS_ERROR, "Invalid argument: pin not found on device");
        mjs_return(mjs, MJS_UNDEFINED);
        return;
    }

    if(pin_record->debug) {
        mjs_prepend_errorf(mjs, MJS_BAD_ARGS_ERROR, "Invalid argument: pin is used for debugging");
        mjs_return(mjs, MJS_UNDEFINED);
        return;
    }

    // return pin manager object
    mjs_val_t manager = mjs_mk_object(mjs);
    JsGpioPinInst* manager_data = malloc(sizeof(JsGpioPinInst));
    manager_data->pin = pin_record->pin;
    mjs_set(mjs, manager, INST_PROP_NAME, ~0, mjs_mk_foreign(mjs, manager_data));
    mjs_set(mjs, manager, "init", ~0, MJS_MK_FN(js_gpio_init));
    mjs_set(mjs, manager, "write", ~0, MJS_MK_FN(js_gpio_write));
    mjs_set(mjs, manager, "read", ~0, MJS_MK_FN(js_gpio_read));
    mjs_return(mjs, manager);
}

static void* js_gpio_create(struct mjs* mjs, mjs_val_t* object) {
    JsGpioInst* gpio = malloc(sizeof(JsGpioInst));

    mjs_val_t gpio_obj = mjs_mk_object(mjs);
    mjs_set(mjs, gpio_obj, INST_PROP_NAME, ~0, mjs_mk_foreign(mjs, gpio));
    mjs_set(mjs, gpio_obj, "get", ~0, MJS_MK_FN(js_gpio_get));
    *object = gpio_obj;

    return (void*)gpio;
}

static void js_gpio_destroy(void* inst) {
    if(inst != NULL) {
        JsGpioInst* gpio = (JsGpioInst*)inst;
        // TODO: release resources
        free(gpio);
    }

    // TODO: reset pins

    expansion_enable(furi_record_open(RECORD_EXPANSION));
    furi_record_close(RECORD_EXPANSION);
}

static const JsModuleDescriptor js_gpio_desc = {
    "gpio",
    js_gpio_create,
    js_gpio_destroy,
};

static const FlipperAppPluginDescriptor plugin_descriptor = {
    .appid = PLUGIN_APP_ID,
    .ep_api_version = PLUGIN_API_VERSION,
    .entry_point = &js_gpio_desc,
};

const FlipperAppPluginDescriptor* js_gpio_ep(void) {
    return &plugin_descriptor;
}
