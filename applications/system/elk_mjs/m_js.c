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
#include <dialogs/dialogs.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/loading.h>
#include <flipper_application/api_hashtable/api_hashtable.h>
#include <flipper_application/plugins/composite_resolver.h>
#include "addon_api/app_api_interface.h"

#define TAG "MJS"

static CompositeApiResolver* resolver;

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
    size_t i, num_args = mjs_nargs(mjs);
    for(i = 0; i < num_args; i++) {
        char* name = NULL;
        size_t name_len = 0;
        int need_free = 0;
        mjs_val_t arg = mjs_arg(mjs, i);
        mjs_err_t err = mjs_to_string(mjs, &arg, &name, &name_len, &need_free);
        if(err != MJS_OK) {
            printf("err %s ", mjs_strerror(mjs, err));
        } else {
            printf("%s ", name);
        }

        if(need_free) {
            free(name);
            name = NULL;
        }
    }
    printf("\r\n");
    mjs_return(mjs, MJS_UNDEFINED);
}

static void mjs_delay(struct mjs* mjs) {
    double ms = mjs_get_int(mjs, mjs_arg(mjs, 0));
    furi_delay_ms(ms);
    mjs_return(mjs, MJS_UNDEFINED);
}

static void* my_dlsym(void* handle, const char* name) {
    UNUSED(handle);
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
    void* addr = my_dlsym(NULL, name);
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

static int js_do(const char* path) {
    struct mjs* mjs = mjs_create();
    mjs_val_t global = mjs_get_global(mjs);
    mjs_set(mjs, global, "print", ~0, MFS_MK_FN(mjs_print));
    mjs_set(mjs, global, "delay", ~0, MFS_MK_FN(mjs_delay));
    mjs_set(mjs, global, "to_string", ~0, MFS_MK_FN(mjs_global_to_string));
    mjs_set(mjs, global, "to_hex_string", ~0, MFS_MK_FN(mjs_global_to_hex_string));
    mjs_set(mjs, global, "ffi_address", ~0, MFS_MK_FN(mjs_ffi_address));

    mjs_set_ffi_resolver(mjs, my_dlsym);

    mjs_err_t err = mjs_exec_file(mjs, path, NULL);

    if(err != MJS_OK) {
        FURI_LOG_E(TAG, "mjs_exec_file: %s", mjs_strerror(mjs, err));
        if(mjs->stack_trace != NULL) {
            FURI_LOG_E(TAG, "stack: %s", mjs->stack_trace);
        }
    }

    mjs_destroy(mjs);
    return 0;
}

int32_t m_js_app(void* arg) {
    UNUSED(arg);
    FuriString* name = furi_string_alloc_set(APP_ASSETS_PATH());

    ViewDispatcher* view_dispatcher = view_dispatcher_alloc();
    Loading* loading = loading_alloc();
    resolver = composite_api_resolver_alloc();
    composite_api_resolver_add(resolver, firmware_api_interface);
    composite_api_resolver_add(resolver, application_api_interface);

    Gui* gui = furi_record_open("gui");
    view_dispatcher_enable_queue(view_dispatcher);
    view_dispatcher_attach_to_gui(view_dispatcher, gui, ViewDispatcherTypeFullscreen);
    view_dispatcher_add_view(view_dispatcher, 0, loading_get_view(loading));
    view_dispatcher_switch_to_view(view_dispatcher, 0);

    do {
        if(arg != NULL && strlen(arg) > 0) {
            furi_string_set(name, (const char*)arg);
        } else {
            DialogsFileBrowserOptions browser_options;
            dialog_file_browser_set_basic_options(&browser_options, ".js", NULL);
            DialogsApp* dialogs = furi_record_open(RECORD_DIALOGS);
            if(!dialog_file_browser_show(dialogs, name, name, &browser_options)) break;
            furi_record_close(RECORD_DIALOGS);
        }

        js_do(furi_string_get_cstr(name));
    } while(false);

    view_dispatcher_remove_view(view_dispatcher, 0);
    loading_free(loading);
    view_dispatcher_free(view_dispatcher);
    furi_record_close("gui");

    composite_api_resolver_free(resolver);

    furi_string_free(name);
    return 0;
}