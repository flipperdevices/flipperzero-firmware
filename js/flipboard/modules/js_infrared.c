#include "../js_modules.h"
#include <infrared.h>
#include <infrared_transmit.h>

static void js_infrared_send_protocol(struct mjs* mjs) {
    size_t num_args = mjs_nargs(mjs);
    if(num_args != 3 || !mjs_is_string(mjs_arg(mjs, 0)) || !mjs_is_number(mjs_arg(mjs, 1)) ||
       !mjs_is_number(mjs_arg(mjs, 2))) {
        mjs_prepend_errorf(
            mjs, MJS_BAD_ARGS_ERROR, "Invalid args (protocolName, address, command)");
        mjs_return(mjs, MJS_UNDEFINED);
        return;
    }

    mjs_val_t protocol_arg = mjs_arg(mjs, 0);
    const char* protocol_name = mjs_get_string(mjs, &protocol_arg, NULL);
    uint32_t address = mjs_get_int(mjs, mjs_arg(mjs, 1));
    uint32_t command = mjs_get_int(mjs, mjs_arg(mjs, 2));

    InfraredMessage message;
    message.protocol = infrared_get_protocol_by_name(protocol_name);
    if(message.protocol == InfraredProtocolUnknown) {
        mjs_prepend_errorf(mjs, MJS_BAD_ARGS_ERROR, "Invalid protocol (%s)", protocol_name);
        mjs_return(mjs, MJS_UNDEFINED);
        return;
    }
    int transmit_count = 1;
    message.address = address;
    message.command = command;
    message.repeat = transmit_count != 1;
    infrared_send(&message, transmit_count);

    mjs_return(mjs, mjs_mk_boolean(mjs, true));
}

static void* js_infrared_create(struct mjs* mjs, mjs_val_t* object) {
    mjs_val_t infrared_obj = mjs_mk_object(mjs);
    mjs_set(mjs, infrared_obj, "sendProtocol", ~0, MJS_MK_FN(js_infrared_send_protocol));
    *object = infrared_obj;
    return (void*)1;
}

static void js_infrared_destroy(void* inst) {
    UNUSED(inst);
}

static const JsModuleDescriptor js_infrared_desc = {
    "infrared",
    js_infrared_create,
    js_infrared_destroy,
};

static const FlipperAppPluginDescriptor plugin_descriptor = {
    .appid = PLUGIN_APP_ID,
    .ep_api_version = PLUGIN_API_VERSION,
    .entry_point = &js_infrared_desc,
};

const FlipperAppPluginDescriptor* js_infrared_ep(void) {
    return &plugin_descriptor;
}