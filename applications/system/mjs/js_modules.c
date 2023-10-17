#include <core/common_defines.h>
#include "js_modules.h"
#include <m-dict.h>
#include "modules/js_flipper.h"
#include "modules/js_badusb.h"
#include "modules/js_notification.h"
#include "modules/js_dialog.h"

typedef void* (*JsModeConstructor)(struct mjs* mjs, mjs_val_t* object);
typedef void (*JsModeDestructor)(void* inst);

typedef struct {
    JsModeConstructor create;
    JsModeDestructor destroy;
    void* context;
} JsModuleData;

DICT_DEF2(JsModuleDict, FuriString*, FURI_STRING_OPLIST, JsModuleData, M_POD_OPLIST);

static const struct {
    char* name;
    JsModeConstructor create;
    JsModeDestructor destroy;
} modules_builtin[] = {
    {"flipper", js_flipper_create, NULL},
    {"badusb", js_badusb_create, js_badusb_destroy},
    {"notification", js_notification_create, js_notification_destroy},
    {"dialog", js_dialog_create, NULL},
};

struct JsModules {
    struct mjs* mjs;
    JsModuleDict_t module_dict;
    PluginManager* plugin_manager;
};

JsModules* js_modules_create(struct mjs* mjs, CompositeApiResolver* resolver) {
    JsModules* modules = malloc(sizeof(JsModules));
    modules->mjs = mjs;
    JsModuleDict_init(modules->module_dict);

    modules->plugin_manager = plugin_manager_alloc(
        PLUGIN_APP_ID, PLUGIN_API_VERSION, composite_api_resolver_get(resolver));

    return modules;
}

void js_modules_destroy(JsModules* modules) {
    JsModuleDict_it_t it;
    for(JsModuleDict_it(it, modules->module_dict); !JsModuleDict_end_p(it);
        JsModuleDict_next(it)) {
        const JsModuleDict_itref_t* module_itref = JsModuleDict_cref(it);
        if(module_itref->value.destroy) {
            module_itref->value.destroy(module_itref->value.context);
        }
    }
    plugin_manager_free(modules->plugin_manager);
    JsModuleDict_clear(modules->module_dict);
    free(modules);
}

mjs_val_t js_module_require(JsModules* modules, const char* name, size_t name_len) {
    FuriString* module_name = furi_string_alloc_set_str(name);
    // Check if module is already installed
    JsModuleData* module_inst = JsModuleDict_get(modules->module_dict, module_name);
    if(module_inst) {
        furi_string_free(module_name);
        // TODO: "already exists" error
        return MJS_UNDEFINED;
    }

    bool module_found = false;
    // Check built-in modules
    for(size_t i = 0; i < COUNT_OF(modules_builtin); i++) {
        size_t name_compare_len = strlen(modules_builtin[i].name);

        if(name_compare_len != name_len) {
            continue;
        }

        if(strncmp(name, modules_builtin[i].name, name_compare_len) == 0) {
            JsModuleData module = {
                .create = modules_builtin[i].create, .destroy = modules_builtin[i].destroy};
            JsModuleDict_set_at(modules->module_dict, module_name, module);
            module_found = true;
            FURI_LOG_I(TAG, "Using built-in module %s", name);
            break;
        }
    }

    // External module load
    if(!module_found) {
        FuriString* module_path = furi_string_alloc();
        furi_string_printf(module_path, "%s/%s.fal", APP_DATA_PATH("plugins"), name);
        FURI_LOG_I(TAG, "Loading external module %s", furi_string_get_cstr(module_path));
        do {
            PluginManagerError load_error = plugin_manager_load_single(
                modules->plugin_manager, furi_string_get_cstr(module_path));
            if(load_error != PluginManagerErrorNone) {
                break;
            }
            // TODO: get ep struct, compare name, add to dict
            module_found = true;
        } while(0);
        furi_string_free(module_path);
    }

    // Run module constructor
    mjs_val_t module_object = MJS_UNDEFINED;
    if(module_found) {
        module_inst = JsModuleDict_get(modules->module_dict, module_name);
        furi_assert(module_inst);
        if(module_inst->create) {
            module_inst->context = module_inst->create(modules->mjs, &module_object);
        }
    }

    furi_string_free(module_name);

    return module_object;
}