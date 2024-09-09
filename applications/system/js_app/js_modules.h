#pragma once

#include <stdint.h>
#include "js_thread_i.h"
#include <flipper_application/flipper_application.h>
#include <flipper_application/plugins/plugin_manager.h>
#include <flipper_application/plugins/composite_resolver.h>

#define PLUGIN_APP_ID      "js"
#define PLUGIN_API_VERSION 1

// Are you tired of your silly little JS+C glue code functions being 75%
// argument validation code and 25% actual logic? Introducing: ASS (Argument
// Schema for Scripts)! ASS is a set of macros that reduce the typical
// boilerplate code of "check argument count, get arguments, validate arguments,
// extract C values from arguments" down to just one line!

/**
 * When passed as the second argument to `JS_FETCH_ARGS_OR_RETURN`, signifies
 * that the function requires exactly as many arguments as were specified.
 */
#define JS_EXACTLY  ==
/**
 * When passed as the second argument to `JS_FETCH_ARGS_OR_RETURN`, signifies
 * that the function requires at least as many arguments as were specified.
 */
#define JS_AT_LEAST >=

typedef struct {
    void* out;
    int (*validator)(mjs_val_t);
    void (*converter)(struct mjs*, mjs_val_t*, void*);
    const char* expected_type;
} _js_arg_decl;

static inline void _js_to_int32(struct mjs* mjs, mjs_val_t* in, void* out) {
    *(int32_t*)out = mjs_get_int32(mjs, *in);
}
#define JS_ARG_INT32(out) ((_js_arg_decl){out, mjs_is_number, _js_to_int32, "number"})

static inline void _js_to_ptr(struct mjs* mjs, mjs_val_t* in, void* out) {
    *(void**)out = mjs_get_ptr(mjs, *in);
}
#define JS_ARG_PTR(out) ((_js_arg_decl){out, mjs_is_foreign, _js_to_ptr, "opaque pointer"})

static inline void _js_to_string(struct mjs* mjs, mjs_val_t* in, void* out) {
    *(const char**)out = mjs_get_string(mjs, in, NULL);
}
#define JS_ARG_STR(out) ((_js_arg_decl){out, mjs_is_string, _js_to_string, "string"})

static inline void _js_to_bool(struct mjs* mjs, mjs_val_t* in, void* out) {
    *(bool*)out = !!mjs_get_bool(mjs, *in);
}
#define JS_ARG_BOOL(out) ((_js_arg_decl){out, mjs_is_boolean, _js_to_bool, "boolean"})

static inline void _js_passthrough(struct mjs* mjs, mjs_val_t* in, void* out) {
    UNUSED(mjs);
    *(mjs_val_t*)out = *in;
}
#define JS_ARG_ANY(out) ((_js_arg_decl){out, NULL, _js_passthrough, "any"})
#define JS_ARG_OBJ(out) ((_js_arg_decl){out, mjs_is_object, _js_passthrough, "any"})
#define JS_ARG_FN(out)  ((_js_arg_decl){out, mjs_is_function, _js_passthrough, "function"})
#define JS_ARG_ARR(out) ((_js_arg_decl){out, mjs_is_array, _js_passthrough, "array"})

//-V:JS_FETCH_ARGS_OR_RETURN:1008
/**
 * @brief Fetches and validates the arguments passed to a JS function
 * 
 * Example: `int32_t my_arg; JS_FETCH_ARGS_OR_RETURN(mjs, JS_EXACTLY, JS_ARG_INT32(&my_arg));`
 * 
 * @warning This macro executes `return;` by design in case of an argument count
 * mismatch or a validation failure
 */
#define JS_FETCH_ARGS_OR_RETURN(mjs, arg_operator, ...)                   \
    _js_arg_decl _js_args[] = {__VA_ARGS__};                              \
    int _js_arg_cnt = sizeof(_js_args) / sizeof(*_js_args);               \
    mjs_val_t _js_arg_vals[_js_arg_cnt];                                  \
    if(!(mjs_nargs(mjs) arg_operator _js_arg_cnt))                        \
        JS_ERROR_AND_RETURN(                                              \
            mjs,                                                          \
            MJS_BAD_ARGS_ERROR,                                           \
            "expected %s%d arguments, got %d",                            \
            #arg_operator,                                                \
            _js_arg_cnt,                                                  \
            mjs_nargs(mjs));                                              \
    for(int _i = 0; _i < _js_arg_cnt; _i++) {                             \
        _js_arg_vals[_i] = mjs_arg(mjs, _i);                              \
        if(_js_args[_i].validator)                                        \
            if(!_js_args[_i].validator(_js_arg_vals[_i]))                 \
                JS_ERROR_AND_RETURN(                                      \
                    mjs,                                                  \
                    MJS_BAD_ARGS_ERROR,                                   \
                    "argument %d: expected %s",                           \
                    _i,                                                   \
                    _js_args[_i].expected_type);                          \
        _js_args[_i].converter(mjs, &_js_arg_vals[_i], _js_args[_i].out); \
    }

/**
 * @brief Returns the foreign pointer in `obj["_"]`
 */
#define JS_GET_INST(mjs, obj) mjs_get_ptr(mjs, mjs_get(mjs, obj, INST_PROP_NAME, ~0))
/**
 * @brief Returns the foreign pointer in `this["_"]`
 */
#define JS_GET_CONTEXT(mjs)   JS_GET_INST(mjs, mjs_get_this(mjs))

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
