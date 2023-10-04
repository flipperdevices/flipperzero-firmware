#include <furi.h>
#include <mjs_core_public.h>
#include <mjs_ffi_public.h>
#include <mjs_exec_public.h>
#include <mjs_object_public.h>
#include <mjs_string_public.h>
#include <mjs_conversion.h>
#include <mjs_primitive_public.h>
#include <common/cs_dbg.h>
#include <toolbox/stream/file_stream.h>
#include <loader/firmware_api/firmware_api.h>
#include <flipper_application/api_hashtable/api_hashtable.h>
#include <flipper_application/plugins/composite_resolver.h>
#include "addon_api/app_api_interface.h"
#include "mjs_thread.h"

#define TAG "MJS"

struct MjsThread {
    FuriThread* thread;
    FuriString* path;
    CompositeApiResolver* resolver;
    MjsThreadCallback app_callback;
    void* context;
};

typedef enum {
    ThreadEventStop = (1 << 0),
    ThreadEventTest = (1 << 1),
} WorkerEventFlags;

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

static void mjs_print(struct mjs* mjs) {
    FuriString* msg_str = furi_string_alloc();
    size_t i, num_args = mjs_nargs(mjs);
    for(i = 0; i < num_args; i++) {
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
    printf("%s\r\n", furi_string_get_cstr(msg_str));

    MjsThread* worker = mjs->context;
    furi_assert(worker);
    if(worker->app_callback) {
        worker->app_callback(MjsThreadEventPrint, furi_string_get_cstr(msg_str), worker->context);
    }

    // TODO: print callback
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

static void mjs_delay(struct mjs* mjs) {
    int ms = mjs_get_int(mjs, mjs_arg(mjs, 0));
    uint32_t flags = furi_thread_flags_wait(ThreadEventStop, FuriFlagWaitAny, ms);
    mjs_return(mjs, MJS_UNDEFINED);
    if(flags & FuriFlagError) {
        return;
    }
    if(flags & ThreadEventStop) {
        mjs_exit(mjs);
    }
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

#define MFS_MK_FN(fn) mjs_mk_foreign_func(mjs, (mjs_func_ptr_t)fn)

static int32_t mjs_thread(void* arg) {
    MjsThread* worker = arg;
    worker->resolver = composite_api_resolver_alloc();
    composite_api_resolver_add(worker->resolver, firmware_api_interface);
    composite_api_resolver_add(worker->resolver, application_api_interface);

    struct mjs* mjs = mjs_create(worker);
    // js_modules_create(worker);
    mjs_val_t global = mjs_get_global(mjs);
    mjs_set(mjs, global, "print", ~0, MFS_MK_FN(mjs_print));
    mjs_set(mjs, global, "delay", ~0, MFS_MK_FN(mjs_delay));
    mjs_set(mjs, global, "to_string", ~0, MFS_MK_FN(mjs_global_to_string));
    mjs_set(mjs, global, "to_hex_string", ~0, MFS_MK_FN(mjs_global_to_hex_string));
    mjs_set(mjs, global, "ffi_address", ~0, MFS_MK_FN(mjs_ffi_address));

    mjs_set_ffi_resolver(mjs, my_dlsym, worker->resolver);

    mjs_set_flags_poller(mjs, mjs_exit_flag_poll);

    mjs_err_t err = mjs_exec_file(mjs, furi_string_get_cstr(worker->path), NULL);

    if(err != MJS_OK) {
        FURI_LOG_E(TAG, "Exec error: %s", mjs_strerror(mjs, err));
        if(worker->app_callback) {
            worker->app_callback(MjsThreadEventError, mjs_strerror(mjs, err), worker->context);
        }
        if(mjs->stack_trace != NULL) {
            FURI_LOG_E(TAG, "Stack trace:\n%s", mjs->stack_trace);
            if(worker->app_callback) {
                worker->app_callback(MjsThreadEventErrorTrace, mjs->stack_trace, worker->context);
            }
        }
    } else {
        if(worker->app_callback) {
            worker->app_callback(MjsThreadEventDone, NULL, worker->context);
        }
    }

    // js_modules_destroy(worker);
    mjs_destroy(mjs);

    composite_api_resolver_free(worker->resolver);

    return 0;
}

MjsThread* mjs_thread_run(const char* script_path, MjsThreadCallback callback, void* context) {
    MjsThread* worker = malloc(sizeof(MjsThread));
    worker->path = furi_string_alloc_set(script_path);
    worker->thread = furi_thread_alloc_ex("MjsThread", 8 * 1024, mjs_thread, worker);
    worker->app_callback = callback;
    worker->context = context;
    furi_thread_start(worker->thread);
    return worker;
}

void mjs_thread_stop(MjsThread* worker) {
    furi_thread_flags_set(furi_thread_get_id(worker->thread), ThreadEventStop);
}

void mjs_thread_free(MjsThread* worker) {
    furi_thread_join(worker->thread);
    furi_thread_free(worker->thread);
    furi_string_free(worker->path);
    free(worker);
}
