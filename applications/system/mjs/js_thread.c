#include <common/cs_dbg.h>
#include <toolbox/stream/file_stream.h>
#include <loader/firmware_api/firmware_api.h>
#include <flipper_application/api_hashtable/api_hashtable.h>
#include <flipper_application/plugins/composite_resolver.h>
#include "addon_api/app_api_interface.h"
#include "js_thread.h"
#include "js_thread_i.h"
#include "js_modules.h"

#define TAG "JS"

struct JsThread {
    FuriThread* thread;
    FuriString* path;
    CompositeApiResolver* resolver;
    JsThreadCallback app_callback;
    void* context;
    JsModules* modules;
};

typedef enum {
    ThreadEventStop = (1 << 0),
    ThreadEventTest = (1 << 1),
} WorkerEventFlags;

// static void obj_type(mjs_val_t v) {
//     if(mjs_is_number(v)) {
//         FURI_LOG_I(TAG, "mjs_is_number");
//     } else if(mjs_is_boolean(v)) {
//         FURI_LOG_I(TAG, "mjs_is_boolean");
//     } else if(mjs_is_string(v)) {
//         FURI_LOG_I(TAG, "mjs_is_string");
//     } else if(mjs_is_array(v)) {
//         FURI_LOG_I(TAG, "mjs_is_array");
//     } else if(mjs_is_object(v)) {
//         FURI_LOG_I(TAG, "mjs_is_object");
//     } else if(mjs_is_foreign(v)) {
//         FURI_LOG_I(TAG, "mjs_is_foreign");
//     } else if(mjs_is_function(v)) {
//         FURI_LOG_I(TAG, "mjs_is_function");
//     } else if(mjs_is_null(v)) {
//         FURI_LOG_I(TAG, "mjs_is_null");
//     } else if(mjs_is_undefined(v)) {
//         FURI_LOG_I(TAG, "mjs_is_undefined");
//     } else {
//         FURI_LOG_I(TAG, "unknown");
//     }
// }

// TODO: mjs fix
void cs_log_printf(const char* fmt, ...) {
    UNUSED(fmt);
}

// TODO: mjs fix
int cs_log_print_prefix(enum cs_log_level level, const char* file, int ln) {
    UNUSED(level);
    UNUSED(file);
    UNUSED(ln);
    return 0;
}

char* cs_read_file(const char* path, size_t* size) {
    Storage* storage = furi_record_open(RECORD_STORAGE);
    Stream* stream = file_stream_alloc(storage);
    char* data = NULL;
    if(!file_stream_open(stream, path, FSAM_READ, FSOM_OPEN_EXISTING)) {
    } else {
        *size = stream_size(stream);
        data = (char*)malloc(*size + 1);
        if(data != NULL) {
            stream_rewind(stream);
            if(stream_read(stream, (uint8_t*)data, *size) != *size) {
                file_stream_close(stream);
                furi_record_close(RECORD_STORAGE);
                stream_free(stream);
                free(data);
                return NULL;
            }
            data[*size] = '\0';
        }
    }
    file_stream_close(stream);
    furi_record_close(RECORD_STORAGE);
    stream_free(stream);
    return data;
}

char* json_fread(const char* path) {
    UNUSED(path);
    return NULL;
}

int json_vfprintf(const char* file_name, const char* fmt, va_list ap) {
    UNUSED(file_name);
    UNUSED(fmt);
    UNUSED(ap);
    return 0;
}

int json_prettify_file(const char* file_name) {
    UNUSED(file_name);
    return 0;
}

static void mjs_str_print(FuriString* msg_str, struct mjs* mjs) {
    size_t num_args = mjs_nargs(mjs);
    for(size_t i = 0; i < num_args; i++) {
        char* name = NULL;
        size_t name_len = 0;
        int need_free = 0;
        mjs_val_t arg = mjs_arg(mjs, i);
        mjs_err_t err = mjs_to_string(mjs, &arg, &name, &name_len, &need_free);
        if(err != MJS_OK) {
            furi_string_cat_printf(msg_str, "err %s ", mjs_strerror(mjs, err));
        } else {
            furi_string_cat_printf(msg_str, "%s ", name);
        }
        if(need_free) {
            free(name);
            name = NULL;
        }
    }
}

static void mjs_print(struct mjs* mjs) {
    FuriString* msg_str = furi_string_alloc();
    mjs_str_print(msg_str, mjs);

    printf("%s\r\n", furi_string_get_cstr(msg_str));

    JsThread* worker = mjs->context;
    furi_assert(worker);
    if(worker->app_callback) {
        worker->app_callback(JsThreadEventPrint, furi_string_get_cstr(msg_str), worker->context);
    }

    furi_string_free(msg_str);

    mjs_return(mjs, MJS_UNDEFINED);
}

static void mjs_console_log(struct mjs* mjs) {
    FuriString* msg_str = furi_string_alloc();
    mjs_str_print(msg_str, mjs);
    FURI_LOG_I(TAG, "%s", furi_string_get_cstr(msg_str));
    furi_string_free(msg_str);
    mjs_return(mjs, MJS_UNDEFINED);
}

static void mjs_console_warn(struct mjs* mjs) {
    FuriString* msg_str = furi_string_alloc();
    mjs_str_print(msg_str, mjs);
    FURI_LOG_W(TAG, "%s", furi_string_get_cstr(msg_str));
    furi_string_free(msg_str);
    mjs_return(mjs, MJS_UNDEFINED);
}

static void mjs_console_error(struct mjs* mjs) {
    FuriString* msg_str = furi_string_alloc();
    mjs_str_print(msg_str, mjs);
    FURI_LOG_E(TAG, "%s", furi_string_get_cstr(msg_str));
    furi_string_free(msg_str);
    mjs_return(mjs, MJS_UNDEFINED);
}

static void mjs_console_debug(struct mjs* mjs) {
    FuriString* msg_str = furi_string_alloc();
    mjs_str_print(msg_str, mjs);
    FURI_LOG_D(TAG, "%s", furi_string_get_cstr(msg_str));
    furi_string_free(msg_str);
    mjs_return(mjs, MJS_UNDEFINED);
}

static void mjs_exit_flag_poll(struct mjs* mjs) {
    uint32_t flags = furi_thread_flags_wait(ThreadEventStop, FuriFlagWaitAny, 0);
    if(flags & FuriFlagError) {
        return;
    }
    if(flags & ThreadEventStop) {
        mjs_exit(mjs);
    }
}

bool js_delay_with_flags(struct mjs* mjs, uint32_t time) {
    uint32_t flags = furi_thread_flags_wait(ThreadEventStop, FuriFlagWaitAny, time);
    if(flags & FuriFlagError) {
        return false;
    }
    if(flags & ThreadEventStop) {
        mjs_exit(mjs);
        return true;
    }
    return false;
}

static void mjs_delay(struct mjs* mjs) {
    bool args_correct = false;
    int ms = 0;

    if(mjs_nargs(mjs) == 1) {
        mjs_val_t arg = mjs_arg(mjs, 0);
        if(mjs_is_number(arg)) {
            ms = mjs_get_int(mjs, arg);
            args_correct = true;
        }
    }
    if(!args_correct) {
        mjs_prepend_errorf(mjs, MJS_BAD_ARGS_ERROR, "");
        mjs_return(mjs, MJS_UNDEFINED);
        return;
    }
    js_delay_with_flags(mjs, ms);
    mjs_return(mjs, MJS_UNDEFINED);
}

static void* my_dlsym(void* handle, const char* name) {
    CompositeApiResolver* resolver = handle;
    Elf32_Addr addr = 0;
    uint32_t hash = elf_symbolname_hash(name);
    const ElfApiInterface* api = composite_api_resolver_get(resolver);

    if(!api->resolver_callback(api, hash, &addr)) {
        FURI_LOG_E(TAG, "FFI: cannot find \"%s\"", name);
        return NULL;
    }

    return (void*)addr;
}

static void mjs_ffi_address(struct mjs* mjs) {
    mjs_val_t name_v = mjs_arg(mjs, 0);
    size_t len;
    const char* name = mjs_get_string(mjs, &name_v, &len);
    void* addr = my_dlsym(mjs->dlsym_handle, name);
    mjs_return(mjs, mjs_mk_foreign(mjs, addr));
}

static void mjs_require(struct mjs* mjs) {
    mjs_val_t name_v = mjs_arg(mjs, 0);
    size_t len;
    const char* name = mjs_get_string(mjs, &name_v, &len);
    mjs_val_t req_object = MJS_UNDEFINED;
    if((len == 0) || (name == NULL)) {
        mjs_prepend_errorf(mjs, MJS_BAD_ARGS_ERROR, "String argument is expected");
    } else {
        JsThread* worker = mjs->context;
        furi_assert(worker);
        req_object = js_module_require(worker->modules, name, len);
        if(req_object == MJS_UNDEFINED) {
            mjs_prepend_errorf(mjs, MJS_BAD_ARGS_ERROR, "\"%s\" module not found", name);
        }
    }
    mjs_return(mjs, req_object);
}

static void mjs_global_to_string(struct mjs* mjs) {
    double num = mjs_get_int(mjs, mjs_arg(mjs, 0));
    char tmp_str[] = "-2147483648";
    itoa(num, tmp_str, 10);
    mjs_val_t ret = mjs_mk_string(mjs, tmp_str, ~0, true);
    mjs_return(mjs, ret);
}

static void mjs_global_to_hex_string(struct mjs* mjs) {
    double num = mjs_get_int(mjs, mjs_arg(mjs, 0));
    char tmp_str[] = "-FFFFFFFF";
    itoa(num, tmp_str, 16);
    mjs_val_t ret = mjs_mk_string(mjs, tmp_str, ~0, true);
    mjs_return(mjs, ret);
}

static int32_t js_thread(void* arg) {
    JsThread* worker = arg;
    worker->resolver = composite_api_resolver_alloc();
    composite_api_resolver_add(worker->resolver, firmware_api_interface);
    composite_api_resolver_add(worker->resolver, application_api_interface);

    struct mjs* mjs = mjs_create(worker);
    worker->modules = js_modules_create(mjs);
    mjs_val_t global = mjs_get_global(mjs);
    mjs_set(mjs, global, "print", ~0, MFS_MK_FN(mjs_print));
    mjs_set(mjs, global, "delay", ~0, MFS_MK_FN(mjs_delay));
    mjs_set(mjs, global, "to_string", ~0, MFS_MK_FN(mjs_global_to_string));
    mjs_set(mjs, global, "to_hex_string", ~0, MFS_MK_FN(mjs_global_to_hex_string));
    mjs_set(mjs, global, "ffi_address", ~0, MFS_MK_FN(mjs_ffi_address));
    mjs_set(mjs, global, "require", ~0, MFS_MK_FN(mjs_require));

    mjs_val_t console_obj = mjs_mk_object(mjs);
    mjs_set(mjs, console_obj, "log", ~0, MFS_MK_FN(mjs_console_log));
    mjs_set(mjs, console_obj, "warn", ~0, MFS_MK_FN(mjs_console_warn));
    mjs_set(mjs, console_obj, "error", ~0, MFS_MK_FN(mjs_console_error));
    mjs_set(mjs, console_obj, "debug", ~0, MFS_MK_FN(mjs_console_debug));
    mjs_set(mjs, global, "console", ~0, console_obj);

    mjs_set_ffi_resolver(mjs, my_dlsym, worker->resolver);

    mjs_set_flags_poller(mjs, mjs_exit_flag_poll);

    mjs_err_t err = mjs_exec_file(mjs, furi_string_get_cstr(worker->path), NULL);

    if(err != MJS_OK) {
        FURI_LOG_E(TAG, "Exec error: %s", mjs_strerror(mjs, err));
        if(worker->app_callback) {
            worker->app_callback(JsThreadEventError, mjs_strerror(mjs, err), worker->context);
        }
        if(mjs->stack_trace != NULL) {
            FURI_LOG_E(TAG, "Stack trace:\n%s", mjs->stack_trace);
            if(worker->app_callback) {
                worker->app_callback(JsThreadEventErrorTrace, mjs->stack_trace, worker->context);
            }
        }
    } else {
        if(worker->app_callback) {
            worker->app_callback(JsThreadEventDone, NULL, worker->context);
        }
    }

    mjs_destroy(mjs);
    js_modules_destroy(worker->modules);

    composite_api_resolver_free(worker->resolver);

    return 0;
}

JsThread* js_thread_run(const char* script_path, JsThreadCallback callback, void* context) {
    JsThread* worker = malloc(sizeof(JsThread));
    worker->path = furi_string_alloc_set(script_path);
    worker->thread = furi_thread_alloc_ex("JsThread", 8 * 1024, js_thread, worker);
    worker->app_callback = callback;
    worker->context = context;
    furi_thread_start(worker->thread);
    return worker;
}

void js_thread_stop(JsThread* worker) {
    furi_thread_flags_set(furi_thread_get_id(worker->thread), ThreadEventStop);
}

void js_thread_free(JsThread* worker) {
    furi_thread_join(worker->thread);
    furi_thread_free(worker->thread);
    furi_string_free(worker->path);
    free(worker);
}
