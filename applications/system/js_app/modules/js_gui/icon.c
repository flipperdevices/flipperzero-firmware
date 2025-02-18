#include "../../js_modules.h"
#include <assets_icons.h>

typedef struct {
    const char* name;
    const Icon* data;
} IconDefinition;

#define ICON_DEF(icon)                   \
    (IconDefinition) {                   \
        .name = #icon, .data = &I_##icon \
    }

static const IconDefinition builtin_icons[] = {
    ICON_DEF(DolphinWait_59x54),
    ICON_DEF(js_script_10px),
};

static void js_gui_icon_get_builtin(struct mjs* mjs) {
    const char* icon_name;
    JS_FETCH_ARGS_OR_RETURN(mjs, JS_EXACTLY, JS_ARG_STR(&icon_name));

    for(size_t i = 0; i < COUNT_OF(builtin_icons); i++) {
        if(strcmp(icon_name, builtin_icons[i].name) == 0) {
            mjs_return(mjs, mjs_mk_foreign(mjs, (void*)builtin_icons[i].data));
            return;
        }
    }

    JS_ERROR_AND_RETURN(mjs, MJS_BAD_ARGS_ERROR, "no such built-in icon");
}

static void* js_gui_icon_create(struct mjs* mjs, mjs_val_t* object, JsModules* modules) {
    UNUSED(modules);
    *object = mjs_mk_object(mjs);
    JS_ASSIGN_MULTI(mjs, *object) {
        JS_FIELD("getBuiltin", MJS_MK_FN(js_gui_icon_get_builtin));
    }
    return NULL;
}

static void js_gui_icon_destroy(void* inst) {
    UNUSED(inst);
}

static const JsModuleDescriptor js_gui_icon_desc = {
    "gui__icon",
    js_gui_icon_create,
    js_gui_icon_destroy,
    NULL,
};

static const FlipperAppPluginDescriptor plugin_descriptor = {
    .appid = PLUGIN_APP_ID,
    .ep_api_version = PLUGIN_API_VERSION,
    .entry_point = &js_gui_icon_desc,
};

const FlipperAppPluginDescriptor* js_gui_icon_ep(void) {
    return &plugin_descriptor;
}
