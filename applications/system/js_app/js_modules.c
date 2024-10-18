#include <core/common_defines.h>
#include "js_modules.h"
#include <m-array.h>

#include "modules/js_flipper.h"
#ifdef FW_CFG_unit_tests
#include "modules/js_tests.h"
#endif

#define TAG "JS modules"

// Absolute path is used to make possible plugin load from CLI
#define MODULES_PATH "/ext/apps_data/js_app/plugins"

typedef struct {
    FuriString* name;
    const JsModuleConstructor create;
    const JsModuleDestructor destroy;
    void* context;
} JsModuleData;

// not using:
//   - a dict because ordering is required
//   - a bptree because it forces a sorted ordering
//   - an rbtree because i deemed it more tedious to implement, and with the
//     amount of modules in use (under 10 in the overwhelming majority of cases)
//     i bet it's going to be slower than a plain array
ARRAY_DEF(JsModuleArray, JsModuleData, M_POD_OPLIST);
#define M_OPL_JsModuleArray_t() ARRAY_OPLIST(JsModuleArray)

static const JsModuleDescriptor modules_builtin[] = {
    {"flipper", js_flipper_create, NULL, NULL},
#ifdef FW_CFG_unit_tests
    {"tests", js_tests_create, NULL, NULL},
#endif
};

struct JsModules {
    struct mjs* mjs;
    JsModuleArray_t modules;
    PluginManager* plugin_manager;
    CompositeApiResolver* resolver;
};

JsModules* js_modules_create(struct mjs* mjs, CompositeApiResolver* resolver) {
    JsModules* modules = malloc(sizeof(JsModules));
    modules->mjs = mjs;
    JsModuleArray_init(modules->modules);

    modules->plugin_manager = plugin_manager_alloc(
        PLUGIN_APP_ID, PLUGIN_API_VERSION, composite_api_resolver_get(resolver));

    modules->resolver = resolver;

    return modules;
}

void js_modules_destroy(JsModules* instance) {
    for
        M_EACH(module, instance->modules, JsModuleArray_t) {
            FURI_LOG_T(TAG, "Tearing down %s", furi_string_get_cstr(module->name));
            if(module->destroy) module->destroy(module->context);
            furi_string_free(module->name);
        }
    plugin_manager_free(instance->plugin_manager);
    JsModuleArray_clear(instance->modules);
    free(instance);
}

JsModuleData* js_find_loaded_module(JsModules* instance, const char* name) {
    for
        M_EACH(module, instance->modules, JsModuleArray_t) {
            if(furi_string_cmp_str(module->name, name) == 0) return module;
        }
    return NULL;
}

mjs_val_t js_module_require(JsModules* modules, const char* name, size_t name_len) {
    // Ignore the initial part of the module name
    const char* optional_module_prefix = "@" JS_SDK_VENDOR "/fz-sdk/";
    if(strncmp(name, optional_module_prefix, strlen(optional_module_prefix)) == 0) {
        name += strlen(optional_module_prefix);
    }

    // Check if module is already installed
    JsModuleData* module_inst = js_find_loaded_module(modules, name);
    if(module_inst) { //-V547
        mjs_prepend_errorf(
            modules->mjs, MJS_BAD_ARGS_ERROR, "\"%s\" module is already installed", name);
        return MJS_UNDEFINED;
    }

    bool module_found = false;
    // Check built-in modules
    for(size_t i = 0; i < COUNT_OF(modules_builtin); i++) { //-V1008
        size_t name_compare_len = strlen(modules_builtin[i].name);

        if(name_compare_len != name_len) {
            continue;
        }

        if(strncmp(name, modules_builtin[i].name, name_compare_len) == 0) {
            JsModuleData module = {
                .create = modules_builtin[i].create,
                .destroy = modules_builtin[i].destroy,
                .name = furi_string_alloc_set_str(name),
            };
            JsModuleArray_push_at(modules->modules, 0, module);
            module_found = true;
            FURI_LOG_I(TAG, "Using built-in module %s", name);
            break;
        }
    }

    // External module load
    if(!module_found) {
        FuriString* deslashed_name = furi_string_alloc_set_str(name);
        furi_string_replace_all_str(deslashed_name, "/", "__");
        FuriString* module_path = furi_string_alloc();
        furi_string_printf(
            module_path, "%s/js_%s.fal", MODULES_PATH, furi_string_get_cstr(deslashed_name));
        FURI_LOG_I(
            TAG, "Loading external module %s from %s", name, furi_string_get_cstr(module_path));
        do {
            uint32_t plugin_cnt_last = plugin_manager_get_count(modules->plugin_manager);
            PluginManagerError load_error = plugin_manager_load_single(
                modules->plugin_manager, furi_string_get_cstr(module_path));
            if(load_error != PluginManagerErrorNone) {
                FURI_LOG_E(
                    TAG,
                    "Module %s load error. It may depend on other modules that are not yet loaded.",
                    name);
                break;
            }
            const JsModuleDescriptor* plugin =
                plugin_manager_get_ep(modules->plugin_manager, plugin_cnt_last);
            furi_assert(plugin);

            if(furi_string_cmp_str(deslashed_name, plugin->name) != 0) {
                FURI_LOG_E(TAG, "Module name mismatch %s", plugin->name);
                break;
            }
            JsModuleData module = {
                .create = plugin->create,
                .destroy = plugin->destroy,
                .name = furi_string_alloc_set_str(name),
            };
            JsModuleArray_push_at(modules->modules, 0, module);

            if(plugin->api_interface) {
                FURI_LOG_I(TAG, "Added module API to composite resolver: %s", plugin->name);
                composite_api_resolver_add(modules->resolver, plugin->api_interface);
            }

            module_found = true;
        } while(0);
        furi_string_free(module_path);
        furi_string_free(deslashed_name);
    }

    // Run module constructor
    mjs_val_t module_object = MJS_UNDEFINED;
    if(module_found) {
        module_inst = js_find_loaded_module(modules, name);
        furi_assert(module_inst);
        if(module_inst->create) { //-V779
            module_inst->context = module_inst->create(modules->mjs, &module_object, modules);
        }
    }

    if(module_object == MJS_UNDEFINED) { //-V547
        mjs_prepend_errorf(modules->mjs, MJS_BAD_ARGS_ERROR, "\"%s\" module load fail", name);
    }

    return module_object;
}

void* js_module_get(JsModules* modules, const char* name) {
    FuriString* module_name = furi_string_alloc_set_str(name);
    JsModuleData* module_inst = js_find_loaded_module(modules, name);
    furi_string_free(module_name);
    return module_inst ? module_inst->context : NULL;
}

typedef enum {
    JsSdkCompatStatusCompatible,
    JsSdkCompatStatusIncompatibleVendor,
    JsSdkCompatStatusFirmwareTooOld,
    JsSdkCompatStatusFirmwareTooNew,
} JsSdkCompatStatus;

/**
 * @brief Checks compatibility between the firmware and the JS SDK version
 *        expected by the script
 */
static JsSdkCompatStatus js_internal_sdk_compatibility_status(
    int32_t exp_major,
    int32_t exp_minor,
    const char* exp_vendor) {
    if(exp_vendor) {
        if(strcmp(exp_vendor, JS_SDK_VENDOR) != 0) return JsSdkCompatStatusIncompatibleVendor;
    }
    if(exp_major < JS_SDK_MAJOR) return JsSdkCompatStatusFirmwareTooNew;
    if(exp_major > JS_SDK_MAJOR || exp_minor > JS_SDK_MINOR)
        return JsSdkCompatStatusFirmwareTooOld;
    return JsSdkCompatStatusCompatible;
}

#define JS_SDK_COMPAT_ARGS                                                                 \
    int32_t major, minor;                                                                  \
    const char* vendor = NULL;                                                             \
    JS_FETCH_ARGS_OR_RETURN(mjs, JS_AT_LEAST, JS_ARG_INT32(&major), JS_ARG_INT32(&minor)); \
    if(mjs_nargs(mjs) == 3) {                                                              \
        mjs_val_t vendor_obj = mjs_arg(mjs, 2);                                            \
        vendor = mjs_get_string(mjs, &vendor_obj, NULL);                                   \
    }

void js_sdk_compatibility_status(struct mjs* mjs) {
    JS_SDK_COMPAT_ARGS;
    JsSdkCompatStatus status = js_internal_sdk_compatibility_status(major, minor, vendor);
    switch(status) {
    case JsSdkCompatStatusCompatible:
        mjs_return(mjs, mjs_mk_string(mjs, "compatible", ~0, 0));
        return;
    case JsSdkCompatStatusIncompatibleVendor:
        mjs_return(mjs, mjs_mk_string(mjs, "incompatibleVendor", ~0, 0));
        return;
    case JsSdkCompatStatusFirmwareTooOld:
        mjs_return(mjs, mjs_mk_string(mjs, "firmwareTooOld", ~0, 0));
        return;
    case JsSdkCompatStatusFirmwareTooNew:
        mjs_return(mjs, mjs_mk_string(mjs, "firmwareTooNew", ~0, 0));
        return;
    }
}

void js_is_sdk_compatible(struct mjs* mjs) {
    JS_SDK_COMPAT_ARGS;
    JsSdkCompatStatus status = js_internal_sdk_compatibility_status(major, minor, vendor);
    mjs_return(mjs, mjs_mk_boolean(mjs, status == JsSdkCompatStatusCompatible));
}

void js_assert_sdk_compatibility(struct mjs* mjs) {
    JS_SDK_COMPAT_ARGS;
    JsSdkCompatStatus status = js_internal_sdk_compatibility_status(major, minor, vendor);
    if(status != JsSdkCompatStatusCompatible) {
        mjs_prepend_errorf(
            mjs,
            MJS_NOT_IMPLEMENTED_ERROR,
            "Incompatible firmware: script requests SDK %ld.%ld, firmware provides SDK %ld.%ld",
            major,
            minor,
            JS_SDK_MAJOR,
            JS_SDK_MINOR);
    }
}
