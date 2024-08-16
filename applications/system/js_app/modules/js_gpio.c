#include "../js_modules.h" // IWYU pragma: keep
#include <furi_hal_gpio.h>
#include <furi_hal_resources.h>
#include <expansion/expansion.h>
#include <limits.h>

#define INTERRUPT_QUEUE_LEN 16

typedef struct {
    FuriMessageQueue* interrupt_queue;
} JsGpioInst;

typedef struct {
    mjs_val_t callback;
    mjs_val_t manager;
} JsGpioInterruptMessage;

typedef struct {
    FuriMessageQueue* interrupt_queue;
    JsGpioInterruptMessage message;
} JsGpioIsrContext;

typedef struct {
    const GpioPin* pin;
    GpioMode previous_mode;
    GpioPull previous_pull;
    FuriMessageQueue* interrupt_queue;
    JsGpioIsrContext* isr_context;
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
    if(!mjs_is_object(mode_arg))
        JS_ERROR_AND_RETURN(mjs, MJS_BAD_ARGS_ERROR, "Invalid argument: expected mode object");
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
    if(!direction)
        JS_ERROR_AND_RETURN(
            mjs, MJS_BAD_ARGS_ERROR, "Expected string in \"direction\" field of mode object");
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
        JS_ERROR_AND_RETURN(
            mjs, MJS_BAD_ARGS_ERROR, "Invalid combination of fields in mode object");
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
        JS_ERROR_AND_RETURN(mjs, MJS_BAD_ARGS_ERROR, "Invalid pull mode");
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
 * ```
 */
static void js_gpio_write(struct mjs* mjs) {
    // get argument
    mjs_val_t level_arg = mjs_arg(mjs, 0);
    if(!mjs_is_boolean(level_arg))
        JS_ERROR_AND_RETURN(mjs, MJS_BAD_ARGS_ERROR, "Must be a boolean");
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
 * ```
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
 * @brief Interrupt callback
 */
static void js_gpio_int_cb(void* arg) {
    JsGpioIsrContext* context = (JsGpioIsrContext*)arg;
    furi_message_queue_put(context->interrupt_queue, &context->message, 0);
}

/**
 * @brief Attaches an interrupt handler to a GPIO pin
 * 
 * Example usage:
 * 
 * ```js
 * let gpio = require("gpio");
 * let button = gpio.get("pc1");
 * button.init({ direction: "in", inMode: "interrupt", edge: "rising" });
 * button.attach_handler(function () {
 *     print("Button pressed");
 * });
 * while(true) gpio.process_interrupts(true);
 * ```
 */
static void js_gpio_attach_handler(struct mjs* mjs) {
    // get argument
    mjs_val_t callback_arg = mjs_arg(mjs, 0);
    if(!mjs_is_function(callback_arg))
        JS_ERROR_AND_RETURN(mjs, MJS_BAD_ARGS_ERROR, "Must be a function");

    // get state
    mjs_val_t manager = mjs_get_this(mjs);
    JsGpioPinInst* manager_data =
        (JsGpioPinInst*)(uint32_t)(mjs_get(mjs, manager, INST_PROP_NAME, ~0) & 0xFFFFFFFF);

    // attach interrupt
    if(manager_data->isr_context) free(manager_data->isr_context);
    JsGpioIsrContext* context = malloc(sizeof(JsGpioIsrContext));
    context->interrupt_queue = manager_data->interrupt_queue;
    context->message.callback = callback_arg;
    context->message.manager = manager;
    manager_data->isr_context = context;
    furi_hal_gpio_remove_int_callback(manager_data->pin);
    furi_hal_gpio_add_int_callback(manager_data->pin, js_gpio_int_cb, (void*)context);
    furi_hal_gpio_enable_int_callback(manager_data->pin);

    mjs_return(mjs, MJS_UNDEFINED);
}

/**
 * @brief Detaches an interrupt handler from a GPIO pin
 * 
 * Example usage:
 * 
 * ```js
 * let gpio = require("gpio");
 * let button = gpio.get("pc1");
 * button.init({ direction: "in", inMode: "interrupt", edge: "rising" });
 * button.attach_handler(function () {
 *     print("Button pressed");
 *     button.detach_handler();
 * });
 * while(true) gpio.process_interrupts(true);
 * ```
 */
static void js_gpio_detach_handler(struct mjs* mjs) {
    // get state
    mjs_val_t manager = mjs_get_this(mjs);
    JsGpioPinInst* manager_data =
        (JsGpioPinInst*)(uint32_t)(mjs_get(mjs, manager, INST_PROP_NAME, ~0) & 0xFFFFFFFF);

    // detach interrupt
    if(manager_data->isr_context) free(manager_data->isr_context);
    furi_hal_gpio_remove_int_callback(manager_data->pin);
    furi_message_queue_reset(manager_data->interrupt_queue);

    mjs_return(mjs, MJS_UNDEFINED);
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
        JS_ERROR_AND_RETURN(mjs, MJS_BAD_ARGS_ERROR, "Must be either a string or a number");
    }

    if(!pin_record) JS_ERROR_AND_RETURN(mjs, MJS_BAD_ARGS_ERROR, "Pin not found on device");
    if(pin_record->debug)
        JS_ERROR_AND_RETURN(mjs, MJS_BAD_ARGS_ERROR, "Pin is used for debugging");

    // return pin manager object
    JsGpioInst* module = mjs_get_ptr(mjs, mjs_get(mjs, mjs_get_this(mjs), INST_PROP_NAME, ~0));
    mjs_val_t manager = mjs_mk_object(mjs);
    JsGpioPinInst* manager_data = malloc(sizeof(JsGpioPinInst));
    manager_data->pin = pin_record->pin;
    manager_data->interrupt_queue = module->interrupt_queue;
    mjs_set(mjs, manager, INST_PROP_NAME, ~0, mjs_mk_foreign(mjs, manager_data));
    mjs_set(mjs, manager, "init", ~0, MJS_MK_FN(js_gpio_init));
    mjs_set(mjs, manager, "write", ~0, MJS_MK_FN(js_gpio_write));
    mjs_set(mjs, manager, "read", ~0, MJS_MK_FN(js_gpio_read));
    mjs_set(mjs, manager, "attach_handler", ~0, MJS_MK_FN(js_gpio_attach_handler));
    mjs_set(mjs, manager, "detach_handler", ~0, MJS_MK_FN(js_gpio_detach_handler));
    mjs_return(mjs, manager);
}

/**
 * @brief Processes GPIO interrupts in either a blocking or a non-blocking
 * fashion
 * 
 * Example usage:
 * 
 * ```js
 * let gpio = require("gpio");
 * let button = gpio.get("pc1");
 * button.init({ direction: "in", inMode: "interrupt", edge: "rising" });
 * button.attach_handler(function () {
 *     print("Button pressed");
 * });
 * while(true) gpio.process_interrupts(true);
 * ```
 */
static void js_gpio_process_interrupts(struct mjs* mjs) {
    // get argument
    mjs_val_t block_arg = mjs_arg(mjs, 0);
    if(!mjs_is_boolean(block_arg))
        JS_ERROR_AND_RETURN(mjs, MJS_BAD_ARGS_ERROR, "Must be a boolean");
    bool block = mjs_get_bool(mjs, block_arg);

    // get new messages
    JsGpioInst* module = mjs_get_ptr(mjs, mjs_get(mjs, mjs_get_this(mjs), INST_PROP_NAME, ~0));
    JsGpioInterruptMessage message;
    while(furi_message_queue_get(
              module->interrupt_queue, (void*)&message, block ? FuriWaitForever : 0) ==
          FuriStatusOk) {
        mjs_call(mjs, NULL, message.callback, MJS_UNDEFINED, 1, message.manager);
    }
}

static void* js_gpio_create(struct mjs* mjs, mjs_val_t* object) {
    JsGpioInst* module = malloc(sizeof(JsGpioInst));
    module->interrupt_queue =
        furi_message_queue_alloc(INTERRUPT_QUEUE_LEN, sizeof(JsGpioInterruptMessage));

    mjs_val_t gpio_obj = mjs_mk_object(mjs);

    mjs_set(mjs, gpio_obj, INST_PROP_NAME, ~0, mjs_mk_foreign(mjs, module));
    mjs_set(mjs, gpio_obj, "get", ~0, MJS_MK_FN(js_gpio_get));
    mjs_set(mjs, gpio_obj, "process_interrupts", ~0, MJS_MK_FN(js_gpio_process_interrupts));
    *object = gpio_obj;

    return (void*)module;
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
