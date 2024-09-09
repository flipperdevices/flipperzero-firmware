#include "../../js_modules.h" // IWYU pragma: keep
#include "../js_event_loop/js_event_loop.h"
#include <gui/modules/text_input.h>

#define INPUT_QUEUE_SIZE 4

typedef struct {
    TextInput* text_input;
    FuriEventLoop* loop;
    FuriMessageQueue* input_queue;
    JsEventLoopContract* contract;
    size_t buffer_size;
    char* buffer;
    char* header;
} JsGuiTextInput;

static mjs_val_t js_gui_text_input_transformer(FuriEventLoopObject* object, void* context) {
    struct mjs* mjs = context;
    FuriMessageQueue* queue = object;
    FuriString* string;
    furi_check(furi_message_queue_get(queue, &string, FuriWaitForever) == FuriStatusOk);
    mjs_val_t js_string = mjs_mk_string(mjs, furi_string_get_cstr(string), ~0, true);
    furi_string_free(string);
    return js_string;
}

static void js_gui_text_input_callback(void* context) {
    JsGuiTextInput* input = context;
    FuriString* string = furi_string_alloc_set_str(input->buffer);
    furi_check(furi_message_queue_put(input->input_queue, &string, 0) == FuriStatusOk);
}

static void js_gui_text_input_destructor(struct mjs* mjs, mjs_val_t obj) {
    JsGuiTextInput* object = JS_GET_INST(mjs, obj);
    text_input_free(object->text_input);
    furi_event_loop_maybe_unsubscribe(object->loop, object->input_queue);
    furi_message_queue_free(object->input_queue);
    free(object->contract);
    free(object->buffer);
    free(object->header);
    free(object);
}

static void js_gui_text_input_make(struct mjs* mjs) {
    const char* header;
    int32_t min_length, max_length;
    JS_FETCH_ARGS_OR_RETURN(
        mjs, JS_EXACTLY, JS_ARG_STR(&header), JS_ARG_INT32(&min_length), JS_ARG_INT32(&max_length));

    JsGuiTextInput* object = malloc(sizeof(JsGuiTextInput));
    object->text_input = text_input_alloc();
    object->input_queue = furi_message_queue_alloc(INPUT_QUEUE_SIZE, sizeof(FuriString*));
    object->loop = JS_GET_CONTEXT(mjs);
    object->buffer = malloc(max_length + 1);
    object->header = strdup(header);
    text_input_set_minimum_length(object->text_input, min_length);
    text_input_set_header_text(object->text_input, object->header);
    text_input_set_result_callback(
        object->text_input,
        js_gui_text_input_callback,
        object,
        object->buffer,
        max_length + 1,
        true);

    JsEventLoopContract* contract = malloc(sizeof(JsEventLoopContract));
    contract->object = object->input_queue;
    contract->object_type = JsEventLoopObjectTypeQueue;
    contract->event = FuriEventLoopEventIn;
    contract->transformer = js_gui_text_input_transformer;
    contract->transformer_context = mjs;
    object->contract = contract;

    mjs_val_t js_text_input = mjs_mk_object(mjs);
    mjs_set(mjs, js_text_input, INST_PROP_NAME, ~0, mjs_mk_foreign(mjs, object));
    mjs_set(
        mjs, js_text_input, MJS_DESTRUCTOR_PROP_NAME, ~0, MJS_MK_FN(js_gui_text_input_destructor));
    mjs_set(mjs, js_text_input, "input", ~0, mjs_mk_foreign(mjs, object->contract));
    mjs_set(
        mjs,
        js_text_input,
        "_view",
        ~0,
        mjs_mk_foreign(mjs, text_input_get_view(object->text_input)));
    mjs_return(mjs, js_text_input);
}

static void* js_gui_text_input_create(struct mjs* mjs, mjs_val_t* object, JsModules* modules) {
    JsEventLoop* js_loop = js_module_get(modules, "event_loop");
    if(M_UNLIKELY(!js_loop)) return NULL;
    if(M_UNLIKELY(!js_module_get(modules, "gui"))) return NULL;

    mjs_val_t ctor = mjs_mk_object(mjs);
    mjs_set(mjs, ctor, INST_PROP_NAME, ~0, MJS_MK_FN(js_event_loop_get_loop(js_loop)));
    mjs_set(mjs, ctor, "make", ~0, MJS_MK_FN(js_gui_text_input_make));

    *object = ctor;
    return NULL;
}

static const JsModuleDescriptor js_gui_text_input_desc = {
    "gui__text_input",
    js_gui_text_input_create,
    NULL,
    NULL,
};

static const FlipperAppPluginDescriptor plugin_descriptor = {
    .appid = PLUGIN_APP_ID,
    .ep_api_version = PLUGIN_API_VERSION,
    .entry_point = &js_gui_text_input_desc,
};

const FlipperAppPluginDescriptor* js_gui_text_input_ep(void) {
    return &plugin_descriptor;
}
