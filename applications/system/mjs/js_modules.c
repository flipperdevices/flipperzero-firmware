#include <core/common_defines.h>
#include "js_modules.h"
#include "modules/js_flipper.h"
#include "modules/js_badusb.h"

typedef void* (*JsModeConstructor)(struct mjs* mjs, mjs_val_t* object);
typedef void (*JsModeDestructor)(void* inst);

static void* js_test_create(struct mjs* mjs, mjs_val_t* object) {
    UNUSED(mjs);
    FURI_LOG_E(TAG, "js_test_create");
    *object = MJS_UNDEFINED;
    return (void*)1;
}

static void js_test_destroy(void* inst) {
    FURI_LOG_E(TAG, "js_test_destroy");
    UNUSED(inst);
}

static const struct {
    char* name;
    JsModeConstructor create;
    JsModeDestructor destroy;
} module_defs[] = {
    {"test", js_test_create, js_test_destroy},
    {"flipper", js_flipper_create, NULL},
    {"badusb", js_badusb_create, js_badusb_destroy}};

struct JsModules {
    struct mjs* mjs;
    void* module_inst[COUNT_OF(module_defs)];
};

JsModules* js_modules_create(struct mjs* mjs) {
    JsModules* modules = malloc(sizeof(JsModules));
    modules->mjs = mjs;
    return modules;
}

void js_modules_destroy(JsModules* modules) {
    for(size_t i = 0; i < COUNT_OF(module_defs); i++) {
        if((module_defs[i].destroy) && (modules->module_inst[i])) {
            module_defs[i].destroy(modules->module_inst[i]);
        }
    }
    free(modules);
}

mjs_val_t js_module_require(JsModules* modules, const char* name, size_t name_len) {
    mjs_val_t module_object = MJS_UNDEFINED;
    for(size_t i = 0; i < COUNT_OF(module_defs); i++) {
        size_t name_compare_len = strlen(module_defs[i].name);

        if(name_compare_len != name_len) {
            continue;
        }

        if(strncmp(name, module_defs[i].name, name_compare_len) == 0) {
            if(modules->module_inst[i]) {
                break;
                // TODO: "already exists" error
            }
            if(module_defs[i].create) {
                modules->module_inst[i] = module_defs[i].create(modules->mjs, &module_object);
            }
            break;
        }
    }
    return module_object;
}