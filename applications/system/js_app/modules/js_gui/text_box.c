#include "../../js_modules.h" // IWYU pragma: keep
#include "../js_event_loop/js_event_loop.h"
#include <gui/modules/text_box.h>

typedef struct {
    TextBox* text_box;
    char* text;
} JsTextBox;

static void js_gui_text_box_destructor(struct mjs* mjs, mjs_val_t obj) {
    JsTextBox* object = JS_GET_INST(mjs, obj);
    text_box_free(object->text_box);
    free(object->text);
    free(object);
}

static void js_gui_text_box_set_text(struct mjs* mjs) {
    const char* text;
    JS_FETCH_ARGS_OR_RETURN(mjs, JS_EXACTLY, JS_ARG_STR(&text));
    JsTextBox* object = JS_GET_CONTEXT(mjs);
    free(object->text);
    object->text = strdup(text);
    text_box_set_text(object->text_box, object->text);
}

static void js_gui_text_box_make(struct mjs* mjs) {
    char *font_str, *focus_str;
    JS_FETCH_ARGS_OR_RETURN(mjs, JS_EXACTLY, JS_ARG_STR(&font_str), JS_ARG_STR(&focus_str));
    TextBoxFont font;
    if(strcmp(font_str, "text") == 0) {
        font = TextBoxFontText;
    } else if(strcmp(font_str, "hex") == 0) {
        font = TextBoxFontHex;
    } else {
        JS_ERROR_AND_RETURN(
            mjs, MJS_BAD_ARGS_ERROR, "argument 0: expected either \"text\" or \"hex\"");
    }
    TextBoxFocus focus;
    if(strcmp(focus_str, "start") == 0) {
        focus = TextBoxFocusStart;
    } else if(strcmp(focus_str, "end") == 0) {
        focus = TextBoxFocusEnd;
    } else {
        JS_ERROR_AND_RETURN(
            mjs, MJS_BAD_ARGS_ERROR, "argument 1: expected either \"start\" or \"end\"");
    }

    JsTextBox* object = malloc(sizeof(JsTextBox));
    object->text_box = text_box_alloc();
    text_box_set_font(object->text_box, font);
    text_box_set_focus(object->text_box, focus);

    mjs_val_t js_text_box = mjs_mk_object(mjs);
    mjs_set(mjs, js_text_box, INST_PROP_NAME, ~0, mjs_mk_foreign(mjs, object));
    mjs_set(mjs, js_text_box, MJS_DESTRUCTOR_PROP_NAME, ~0, MJS_MK_FN(js_gui_text_box_destructor));
    mjs_set(mjs, js_text_box, "setText", ~0, MJS_MK_FN(js_gui_text_box_set_text));
    mjs_set(
        mjs, js_text_box, "_view", ~0, mjs_mk_foreign(mjs, text_box_get_view(object->text_box)));
    mjs_return(mjs, js_text_box);
}

static void* js_gui_text_box_create(struct mjs* mjs, mjs_val_t* object, JsModules* modules) {
    JsEventLoop* js_loop = js_module_get(modules, "event_loop");
    if(M_UNLIKELY(!js_loop)) return NULL;
    if(M_UNLIKELY(!js_module_get(modules, "gui"))) return NULL;

    mjs_val_t ctor = mjs_mk_object(mjs);
    mjs_set(mjs, ctor, INST_PROP_NAME, ~0, MJS_MK_FN(js_event_loop_get_loop(js_loop)));
    mjs_set(mjs, ctor, "make", ~0, MJS_MK_FN(js_gui_text_box_make));

    *object = ctor;
    return NULL;
}

static const JsModuleDescriptor js_gui_text_box_desc = {
    "gui__text_box",
    js_gui_text_box_create,
    NULL,
    NULL,
};

static const FlipperAppPluginDescriptor plugin_descriptor = {
    .appid = PLUGIN_APP_ID,
    .ep_api_version = PLUGIN_API_VERSION,
    .entry_point = &js_gui_text_box_desc,
};

const FlipperAppPluginDescriptor* js_gui_text_box_ep(void) {
    return &plugin_descriptor;
}
