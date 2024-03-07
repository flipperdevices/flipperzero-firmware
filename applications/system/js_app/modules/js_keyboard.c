#include "../js_modules.h"
#include <gui/modules/text_input.h>
#include <gui/modules/byte_input.h>
#include <gui/view_dispatcher.h>

#define membersof(x) (sizeof(x) / sizeof(x[0]))

typedef struct {
    char* data;
    TextInput* textinput;
    ByteInput* byteinputview;
    ViewDispatcher* view_dispatcher;
    uint8_t* byteinput;
} JSkeyboardInst;

static void ret_bad_args(struct mjs* mjs, const char* error) {
    mjs_prepend_errorf(mjs, MJS_BAD_ARGS_ERROR, "%s", error);
    mjs_return(mjs, MJS_UNDEFINED);
}

char nibble_to_hex_character(uint8_t nibble) {
    nibble &= 0xF;
    if(nibble < 10) {
        return '0' + nibble;
    } else {
        return 'A' + (nibble - 10);
    }
}

char* bytes_to_hex_string(uint8_t* bytes, size_t num_bytes) {
    char* hex_string = (char*)malloc(num_bytes * 2 + 1);
    if(hex_string == NULL) {
        FURI_LOG_D("LOG", "Memory allocation failed\n");
        return NULL;
    }

    for(size_t i = 0; i < num_bytes; ++i) {
        hex_string[i * 2] = nibble_to_hex_character(bytes[i] >> 4);
        hex_string[i * 2 + 1] = nibble_to_hex_character(bytes[i] & 0xF);
    }
    hex_string[num_bytes * 2] = '\0';

    return hex_string;
}

static bool get_str_arg(struct mjs* mjs, size_t index, const char** value) {
    mjs_val_t str_obj = mjs_arg(mjs, index);
    if(!mjs_is_string(str_obj)) {
        ret_bad_args(mjs, "Argument must be a string");
        return false;
    }
    size_t str_len = 0;
    *value = mjs_get_string(mjs, &str_obj, &str_len);
    if((str_len == 0) || (*value == NULL)) {
        ret_bad_args(mjs, "Bad string argument");
        return false;
    }
    return true;
}

static bool get_int_arg(struct mjs* mjs, size_t index, int* value) {
    mjs_val_t int_obj = mjs_arg(mjs, index);
    if(!mjs_is_number(int_obj)) {
        return false;
    }
    *value = (int)mjs_get_int(mjs, int_obj);
    return true;
}

static JSkeyboardInst* get_this_ctx(struct mjs* mjs) {
    mjs_val_t obj_inst = mjs_get(mjs, mjs_get_this(mjs), INST_PROP_NAME, ~0);
    JSkeyboardInst* storage = mjs_get_ptr(mjs, obj_inst);
    furi_assert(storage);
    return storage;
}

void text_input_callback(void* context) {
    JSkeyboardInst* keyboardinst = (JSkeyboardInst*)context;
    view_dispatcher_stop(keyboardinst->view_dispatcher);
}

void byte_input_callback(void* context) {
    JSkeyboardInst* keyboardinst = (JSkeyboardInst*)context;
    view_dispatcher_stop(keyboardinst->view_dispatcher);
}

static void js_keyboard_text(struct mjs* mjs) {
    JSkeyboardInst* keyboardinst = get_this_ctx(mjs);

    int MaxInputLength;
    if(!get_int_arg(mjs, 0, &MaxInputLength)) return;

    const char* defaultText = "";
    get_str_arg(mjs, 1, &defaultText);

    int ShouldSelect;
    mjs_val_t bool_obj = mjs_arg(mjs, 2);
    ShouldSelect = (int)mjs_get_bool(mjs, bool_obj);

    if(keyboardinst->textinput && keyboardinst->view_dispatcher) {
        if(strlen(defaultText) > 0) {
            text_input_set_header_text(keyboardinst->textinput, defaultText);
        }

        view_dispatcher_attach_to_gui(
            keyboardinst->view_dispatcher,
            furi_record_open(RECORD_GUI),
            ViewDispatcherTypeFullscreen);
        furi_record_close(RECORD_GUI);

        text_input_set_result_callback(
            keyboardinst->textinput,
            text_input_callback,
            keyboardinst,
            keyboardinst->data,
            MaxInputLength,
            ShouldSelect);

        view_dispatcher_switch_to_view(keyboardinst->view_dispatcher, 0);

        view_dispatcher_run(keyboardinst->view_dispatcher);
    }

    text_input_reset(keyboardinst->textinput);

    mjs_return(mjs, mjs_mk_string(mjs, keyboardinst->data, strlen(keyboardinst->data), 1));
}

static void js_keyboard_byte(struct mjs* mjs) {
    JSkeyboardInst* keyboardinst = get_this_ctx(mjs);

    int MaxInputLength;
    if(!get_int_arg(mjs, 0, &MaxInputLength)) return;

    const char* defaultText;
    get_str_arg(mjs, 1, &defaultText);

    if(keyboardinst->byteinputview && keyboardinst->view_dispatcher) {
        if(strlen(defaultText) > 0) {
            byte_input_set_header_text(keyboardinst->byteinputview, defaultText);
        }

        view_dispatcher_attach_to_gui(
            keyboardinst->view_dispatcher,
            furi_record_open(RECORD_GUI),
            ViewDispatcherTypeFullscreen);
        furi_record_close(RECORD_GUI);

        byte_input_set_result_callback(
            keyboardinst->byteinputview,
            byte_input_callback,
            NULL,
            keyboardinst,
            keyboardinst->byteinput,
            10);

        view_dispatcher_switch_to_view(keyboardinst->view_dispatcher, 1);

        view_dispatcher_run(keyboardinst->view_dispatcher);
    }

    FURI_LOG_D("SHIT", "DID THING");
    keyboardinst->data = bytes_to_hex_string(keyboardinst->byteinput, 10);
    FURI_LOG_D("SHIT", "DID THING 1");
    mjs_return(mjs, mjs_mk_string(mjs, keyboardinst->data, strlen(keyboardinst->data), 1));
    FURI_LOG_D("SHIT", "DID THING 2");
}

static void* js_keyboard_create(struct mjs* mjs, mjs_val_t* object) {
    JSkeyboardInst* keyboardinst = malloc(sizeof(JSkeyboardInst));
    mjs_val_t keyboard_obj = mjs_mk_object(mjs);
    mjs_set(mjs, keyboard_obj, INST_PROP_NAME, ~0, mjs_mk_foreign(mjs, keyboardinst));
    mjs_set(mjs, keyboard_obj, "text", ~0, MJS_MK_FN(js_keyboard_text));
    //mjs_set(mjs, keyboard_obj, "byte", ~0, MJS_MK_FN(js_keyboard_byte)); // NULL POINTER DEREFERENCE
    keyboardinst->byteinputview = byte_input_alloc();
    keyboardinst->textinput = text_input_alloc();
    keyboardinst->view_dispatcher = view_dispatcher_alloc();
    keyboardinst->data = malloc(100);
    keyboardinst->byteinput = malloc(100);
    view_dispatcher_enable_queue(keyboardinst->view_dispatcher);
    view_dispatcher_add_view(
        keyboardinst->view_dispatcher, 0, text_input_get_view(keyboardinst->textinput));
    view_dispatcher_add_view(
        keyboardinst->view_dispatcher, 1, byte_input_get_view(keyboardinst->byteinputview));
    *object = keyboard_obj;
    return keyboardinst;
}

static void js_keyboard_destroy(void* inst) {
    JSkeyboardInst* insts = (JSkeyboardInst*)inst;
    byte_input_free(insts->byteinputview);
    text_input_free(insts->textinput);
    view_dispatcher_free(insts->view_dispatcher);
    free(insts->data);
    free(insts);
}

static const JsModuleDescriptor js_keyboard_desc = {
    "keyboard",
    js_keyboard_create,
    js_keyboard_destroy,
};

static const FlipperAppPluginDescriptor keyboard_plugin_descriptor = {
    .appid = PLUGIN_APP_ID,
    .ep_api_version = PLUGIN_API_VERSION,
    .entry_point = &js_keyboard_desc,
};

const FlipperAppPluginDescriptor* js_keyboard_ep(void) {
    UNUSED(js_keyboard_byte);
    return &keyboard_plugin_descriptor;
}