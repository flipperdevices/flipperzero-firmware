#include <core/common_defines.h>
#include "../../js_modules.h"
#include <furi_hal.h>

#include "rgbleds.h"

typedef struct {
    RgbLeds* leds;
} JsRgbledsInst;

typedef struct {
    const GpioPin* pin;
    const char* name;
} GpioPinCtx;

static const GpioPinCtx js_gpio_pins[] = {
    {.pin = &gpio_ext_pa7, .name = "PA7"}, // 2
    {.pin = &gpio_ext_pa6, .name = "PA6"}, // 3
    {.pin = &gpio_ext_pa4, .name = "PA4"}, // 4
    {.pin = &gpio_ext_pb3, .name = "PB3"}, // 5
    {.pin = &gpio_ext_pb2, .name = "PB2"}, // 6
    {.pin = &gpio_ext_pc3, .name = "PC3"}, // 7
    {.pin = &gpio_swclk, .name = "PA14"}, // 10
    {.pin = &gpio_swdio, .name = "PA13"}, // 12
    {.pin = &gpio_usart_tx, .name = "PB6"}, // 13
    {.pin = &gpio_usart_rx, .name = "PB7"}, // 14
    {.pin = &gpio_ext_pc1, .name = "PC1"}, // 15
    {.pin = &gpio_ext_pc0, .name = "PC0"}, // 16
    {.pin = &gpio_ibutton, .name = "PB14"}, // 17
};

static const GpioPin* get_gpio_pin(const char* name) {
    for(size_t i = 0; i < COUNT_OF(js_gpio_pins); i++) {
        if(strcmp(js_gpio_pins[i].name, name) == 0) {
            return js_gpio_pins[i].pin;
        }
    }
    return NULL;
}

static void js_rgbleds_setup(struct mjs* mjs) {
    mjs_val_t obj_inst = mjs_get(mjs, mjs_get_this(mjs), INST_PROP_NAME, ~0);
    JsRgbledsInst* rgbleds = mjs_get_ptr(mjs, obj_inst);
    furi_assert(rgbleds);

    if(mjs_nargs(mjs) != 1 || !mjs_is_object(mjs_arg(mjs, 0))) {
        mjs_prepend_errorf(mjs, MJS_BAD_ARGS_ERROR, "");
        mjs_return(mjs, MJS_UNDEFINED);
        return;
    }

    mjs_val_t pin_obj = mjs_get(mjs, mjs_arg(mjs, 0), "pin", ~0);
    mjs_val_t count_obj = mjs_get(mjs, mjs_arg(mjs, 0), "count", ~0);
    mjs_val_t spec_obj = mjs_get(mjs, mjs_arg(mjs, 0), "spec", ~0);

    if(!mjs_is_string(pin_obj)) {
        mjs_prepend_errorf(mjs, MJS_BAD_ARGS_ERROR, "pin must be a string");
        mjs_return(mjs, MJS_UNDEFINED);
        return;
    }

    if(!mjs_is_number(count_obj)) {
        mjs_prepend_errorf(mjs, MJS_BAD_ARGS_ERROR, "count must be a number");
        mjs_return(mjs, MJS_UNDEFINED);
        return;
    }

    if(!mjs_is_string(spec_obj)) {
        mjs_prepend_errorf(mjs, MJS_BAD_ARGS_ERROR, "spec must be a string");
        mjs_return(mjs, MJS_UNDEFINED);
        return;
    }

    const GpioPin* pin = get_gpio_pin(mjs_get_string(mjs, &pin_obj, NULL));
    if(!pin) {
        mjs_prepend_errorf(mjs, MJS_BAD_ARGS_ERROR, "invalid pin");
        mjs_return(mjs, MJS_UNDEFINED);
        return;
    }

    if(rgbleds->leds) {
        rgbleds_free(rgbleds->leds);
    }

    uint16_t count = mjs_get_int(mjs, count_obj);
    rgbleds->leds = rgbleds_alloc(count, pin);
}

static void js_rgbleds_set(struct mjs* mjs) {
    mjs_val_t obj_inst = mjs_get(mjs, mjs_get_this(mjs), INST_PROP_NAME, ~0);
    JsRgbledsInst* rgbleds = mjs_get_ptr(mjs, obj_inst);
    furi_assert(rgbleds);

    if(!rgbleds->leds) {
        mjs_prepend_errorf(mjs, MJS_INTERNAL_ERROR, "LEDs not setup");
        mjs_return(mjs, MJS_UNDEFINED);
        return;
    }

    uint32_t color;
    if(mjs_nargs(mjs) == 2) {
        if(mjs_is_number(mjs_arg(mjs, 0)) && mjs_is_number(mjs_arg(mjs, 1))) {
            color = mjs_get_int(mjs, mjs_arg(mjs, 1));
        } else if(mjs_is_number(mjs_arg(mjs, 0)) && mjs_is_object(mjs_arg(mjs, 1))) {
            mjs_val_t red_obj = mjs_get(mjs, mjs_arg(mjs, 1), "red", ~0);
            mjs_val_t green_obj = mjs_get(mjs, mjs_arg(mjs, 1), "green", ~0);
            mjs_val_t blue_obj = mjs_get(mjs, mjs_arg(mjs, 1), "blue", ~0);
            if(!mjs_is_number(red_obj) || !mjs_is_number(green_obj) || !mjs_is_number(blue_obj)) {
                mjs_prepend_errorf(mjs, MJS_BAD_ARGS_ERROR, "");
                mjs_return(mjs, MJS_UNDEFINED);
                return;
            }
            color = (mjs_get_int(mjs, red_obj) << 16) | (mjs_get_int(mjs, green_obj) << 8) |
                    mjs_get_int(mjs, blue_obj);
        } else {
            mjs_prepend_errorf(mjs, MJS_BAD_ARGS_ERROR, "");
            mjs_return(mjs, MJS_UNDEFINED);
            return;
        }
    } else if(mjs_nargs(mjs) == 4) {
        if(!mjs_is_number(mjs_arg(mjs, 0)) || !mjs_is_number(mjs_arg(mjs, 1)) ||
           !mjs_is_number(mjs_arg(mjs, 2)) || !mjs_is_number(mjs_arg(mjs, 3))) {
            mjs_prepend_errorf(mjs, MJS_BAD_ARGS_ERROR, "");
            mjs_return(mjs, MJS_UNDEFINED);
            return;
        }

        color = (mjs_get_int(mjs, mjs_arg(mjs, 1)) << 16) |
                (mjs_get_int(mjs, mjs_arg(mjs, 2)) << 8) | mjs_get_int(mjs, mjs_arg(mjs, 3));
    } else {
        mjs_prepend_errorf(mjs, MJS_BAD_ARGS_ERROR, "");
        mjs_return(mjs, MJS_UNDEFINED);
        return;
    }

    uint16_t index = mjs_get_int(mjs, mjs_arg(mjs, 0));

    uint16_t original_color = rgbleds_get(rgbleds->leds, index);
    rgbleds_set(rgbleds->leds, index, color);
    mjs_return(mjs, mjs_mk_number(mjs, original_color));
}

static void js_rgbleds_get(struct mjs* mjs) {
    mjs_val_t obj_inst = mjs_get(mjs, mjs_get_this(mjs), INST_PROP_NAME, ~0);
    JsRgbledsInst* rgbleds = mjs_get_ptr(mjs, obj_inst);
    furi_assert(rgbleds);

    if(!rgbleds->leds) {
        mjs_prepend_errorf(mjs, MJS_INTERNAL_ERROR, "LEDs not setup");
        mjs_return(mjs, MJS_UNDEFINED);
        return;
    }

    if(mjs_nargs(mjs) != 1 || !mjs_is_number(mjs_arg(mjs, 0))) {
        mjs_prepend_errorf(mjs, MJS_BAD_ARGS_ERROR, "");
        mjs_return(mjs, MJS_UNDEFINED);
        return;
    }

    uint16_t index = mjs_get_int(mjs, mjs_arg(mjs, 0));
    mjs_return(mjs, mjs_mk_number(mjs, rgbleds_get(rgbleds->leds, index)));
}

static void js_rgbleds_update(struct mjs* mjs) {
    mjs_val_t obj_inst = mjs_get(mjs, mjs_get_this(mjs), INST_PROP_NAME, ~0);
    JsRgbledsInst* rgbleds = mjs_get_ptr(mjs, obj_inst);
    furi_assert(rgbleds);

    if(!rgbleds->leds) {
        mjs_prepend_errorf(mjs, MJS_INTERNAL_ERROR, "LEDs not setup");
        mjs_return(mjs, MJS_UNDEFINED);
        return;
    }

    rgbleds_update(rgbleds->leds);
}

static void* js_rgbleds_create(struct mjs* mjs, mjs_val_t* object) {
    JsRgbledsInst* rgbleds = malloc(sizeof(JsRgbledsInst));
    rgbleds->leds = NULL;
    mjs_val_t rgbleds_obj = mjs_mk_object(mjs);
    mjs_set(mjs, rgbleds_obj, INST_PROP_NAME, ~0, mjs_mk_foreign(mjs, rgbleds));
    mjs_set(mjs, rgbleds_obj, "setup", ~0, MJS_MK_FN(js_rgbleds_setup));
    mjs_set(mjs, rgbleds_obj, "set", ~0, MJS_MK_FN(js_rgbleds_set));
    mjs_set(mjs, rgbleds_obj, "get", ~0, MJS_MK_FN(js_rgbleds_get));
    mjs_set(mjs, rgbleds_obj, "update", ~0, MJS_MK_FN(js_rgbleds_update));
    *object = rgbleds_obj;
    return rgbleds;
}

static void js_rgbleds_destroy(void* inst) {
    JsRgbledsInst* rgbleds = inst;
    if(rgbleds->leds) {
        rgbleds_free(rgbleds->leds);
    }
    free(rgbleds);
}

static const JsModuleDescriptor js_rgbleds_desc = {
    "rgbleds",
    js_rgbleds_create,
    js_rgbleds_destroy,
};

static const FlipperAppPluginDescriptor plugin_descriptor = {
    .appid = PLUGIN_APP_ID,
    .ep_api_version = PLUGIN_API_VERSION,
    .entry_point = &js_rgbleds_desc,
};

const FlipperAppPluginDescriptor* js_rgbleds_ep(void) {
    return &plugin_descriptor;
}
