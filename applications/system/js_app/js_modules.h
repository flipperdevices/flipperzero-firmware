#pragma once

#include <stdint.h>
#include "js_thread_i.h"
#include <flipper_application/flipper_application.h>
#include <flipper_application/plugins/plugin_manager.h>
#include <flipper_application/plugins/composite_resolver.h>

#define PLUGIN_APP_ID      "js"
#define PLUGIN_API_VERSION 1

// boilerplate reducing macros

/**
 * @brief Fetches the arguments passed to a JS function
 * 
 * This macro expects pointers to `mjs_val_t` variables that it will populate.
 * If the number of arguments differs from the expected value, it sets the JS
 * return value to `undefined` and returns from the C function.
 * 
 * @warning This macro executes `return;` by design in case of an argument count
 * mismatch
 */
#define JS_FETCH_ARGS_OR_RETURN(mjs, ...)                   \
    mjs_val_t* _js_args[] = {__VA_ARGS__};                  \
    int _js_arg_cnt = sizeof(_js_args) / sizeof(*_js_args); \
    if(mjs_nargs(mjs) != _js_arg_cnt)                       \
        JS_ERROR_AND_RETURN(                                \
            mjs,                                            \
            MJS_BAD_ARGS_ERROR,                             \
            "expected %d arguments, got %d",                \
            _js_arg_cnt,                                    \
            mjs_nargs(mjs));                                \
    for(int _i = 0; _i < _js_arg_cnt; _i++)                 \
        *(_js_args[_i]) = mjs_arg(mjs, _i);

/**
 * @brief Returns the foreign pointer in `this["_"]`
 */
#define JS_GET_CONTEXT(mjs) mjs_get_ptr(mjs, mjs_get(mjs, mjs_get_this(mjs), INST_PROP_NAME, ~0))

/**
 * @brief Prepends an error, sets the JS return value to `undefined` and returns
 * from the C function
 * @warning This macro executes `return;` by design
 */
#define JS_ERROR_AND_RETURN(mjs, error_code, ...)         \
    do {                                                  \
        mjs_prepend_errorf(mjs, error_code, __VA_ARGS__); \
        mjs_return(mjs, MJS_UNDEFINED);                   \
        return;                                           \
    } while(0)

typedef struct JsModules JsModules;

typedef void* (*JsModuleConstructor)(struct mjs* mjs, mjs_val_t* object, JsModules* modules);
typedef void (*JsModuleDestructor)(void* inst);

typedef struct {
    char* name;
    JsModuleConstructor create;
    JsModuleDestructor destroy;
    const ElfApiInterface* api_interface;
} JsModuleDescriptor;

JsModules* js_modules_create(struct mjs* mjs, CompositeApiResolver* resolver);

void js_modules_destroy(JsModules* modules);

mjs_val_t js_module_require(JsModules* modules, const char* name, size_t name_len);

/**
 * @brief Gets a module instance by its name
 * This is useful when a module wants to access a stateful API of another
 * module.
 * @returns Pointer to module context, NULL if the module is not instantiated
 */
void* js_module_get(JsModules* modules, const char* name);
