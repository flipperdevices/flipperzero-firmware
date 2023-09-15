#include <furi.h>
#include "elk.h"
#include <notification/notification_messages.h>
#include <storage/storage.h>
#include <dialogs/dialogs.h>
#include <loader/firmware_api/firmware_api.h>
#include <flipper_application/api_hashtable/api_hashtable.h>
#include "ffi/ffi.h"
#include <stdio.h>

#define TAG "JS"

typedef enum {
    ArgTypeVoid,
    ArgTypeUint8,
    ArgTypeSint8,
    ArgTypeUint16,
    ArgTypeSint16,
    ArgTypeUint32,
    ArgTypeSint32,
    ArgTypeUint64,
    ArgTypeSint64,
    ArgTypeFloat,
    ArgTypeDouble,
    ArgTypePointer,
    ArgTypeString,
} ArgType;

void test_ffi() {
    ffi_cif cif;
    ffi_type* args[1];
    void* values[1];
    char* s;
    ffi_arg rc;

    /* Initialize the argument info vectors */
    args[0] = &ffi_type_pointer;
    values[0] = &s;

    void (*fn)(void) = (void (*)(void))puts;

    /* Initialize the cif */
    if(ffi_prep_cif(&cif, FFI_DEFAULT_ABI, 1, &ffi_type_sint, args) == FFI_OK) {
        s = "Hello World!";
        ffi_call(&cif, fn, &rc, values);
        s = "This is cool!";
        ffi_call(&cif, fn, &rc, values);
    }
}

static ffi_type* type_from_argtype(ArgType argtype) {
    switch(argtype) {
    case ArgTypeVoid:
        return &ffi_type_void;
    case ArgTypeUint8:
        return &ffi_type_uint8;
    case ArgTypeSint8:
        return &ffi_type_sint8;
    case ArgTypeUint16:
        return &ffi_type_uint16;
    case ArgTypeSint16:
        return &ffi_type_sint16;
    case ArgTypeUint32:
        return &ffi_type_uint32;
    case ArgTypeSint32:
        return &ffi_type_sint32;
    case ArgTypeUint64:
        return &ffi_type_uint64;
    case ArgTypeSint64:
        return &ffi_type_sint64;
    case ArgTypeFloat:
        return &ffi_type_float;
    case ArgTypeDouble:
        return &ffi_type_double;
    case ArgTypePointer:
        return &ffi_type_pointer;
    case ArgTypeString:
        return &ffi_type_pointer;
    }

    return NULL;
}

static jsval_t js_global_fficall(struct js* js, jsval_t* args, int nargs) {
    Elf32_Addr addr;
    const char* name = js_getstr(js, args[1], NULL);
    uint32_t hash = elf_symbolname_hash(name);
    if(!firmware_api_interface->resolver_callback(firmware_api_interface, hash, &addr)) {
        FURI_LOG_E(TAG, "FFI: cannot find \"%s\"", name);
        return js_mkundef();
    }
    FURI_LOG_I(TAG, "FFI {");
    void (*fn)(void) = (void (*)(void))addr;
    FURI_LOG_I(TAG, "  \"%s\" = 0x%p", name, fn);

    const size_t arg_count = (nargs - 2) / 2;
    FURI_LOG_I(TAG, "  args count %u", arg_count);

    ffi_type* ffi_args[arg_count];
    uint64_t ffi_vals[arg_count];
    void* ffi_vals_p[arg_count];
    for(size_t i = 0; i < arg_count; i++) {
        ArgType type = (ArgType)js_getnum(args[2 + i * 2]);
        jsval_t arg = args[3 + i * 2];
        ffi_args[i] = type_from_argtype(type);
        switch(type) {
        case ArgTypeVoid:
            ffi_vals[i] = 0;
            FURI_LOG_I(TAG, "  arg %u void", i);
            break;
        case ArgTypeUint8:
        case ArgTypeSint8:
        case ArgTypeUint16:
        case ArgTypeSint16:
        case ArgTypeUint32:
        case ArgTypeSint32:
        case ArgTypeUint64:
        case ArgTypeSint64:
        case ArgTypeFloat:
        case ArgTypeDouble:
            ffi_vals[i] = js_getnum(arg);
            FURI_LOG_I(TAG, "  arg %u num = %f", i, (double)ffi_vals[i]);
            break;
        case ArgTypePointer:
            ffi_vals[i] = js_getnum(arg);
            FURI_LOG_I(TAG, "  arg %u ptr = 0x%p", i, (void*)(uint32_t)ffi_vals[i]);
            break;
        case ArgTypeString:
            ffi_vals[i] = (uint32_t)js_getstr(js, arg, NULL);
            FURI_LOG_I(TAG, "  arg %u str = \"%s\"", i, (const char*)(uint32_t)ffi_vals[i]);
            break;
        default:
            FURI_LOG_E(TAG, "FFI: invalid arg type %u", type);
            return js_mkundef();
        }

        ffi_vals_p[i] = &ffi_vals[i];
    }

    ArgType type = (ArgType)js_getnum(args[0]);
    ffi_arg rc;
    ffi_cif cif;

    if(ffi_prep_cif(&cif, FFI_DEFAULT_ABI, arg_count, type_from_argtype(type), ffi_args) ==
       FFI_OK) {
        ffi_call(&cif, fn, &rc, ffi_vals_p);
    } else {
        FURI_LOG_E(TAG, "FFI: ffi_prep_cif failed");
    }

    jsval_t ret;

    switch(type) {
    case ArgTypeVoid:
        FURI_LOG_I(TAG, "  ret void");
        ret = js_mknum(0);
        break;
    case ArgTypeUint8:
        ret = js_mknum((uint8_t)rc);
        break;
    case ArgTypeSint8:
        ret = js_mknum((int8_t)rc);
        break;
    case ArgTypeUint16:
        ret = js_mknum((uint16_t)rc);
        break;
    case ArgTypeSint16:
        ret = js_mknum((int16_t)rc);
        break;
    case ArgTypeUint32:
        ret = js_mknum((uint32_t)rc);
        break;
    case ArgTypeSint32:
        ret = js_mknum((int32_t)rc);
        break;
    case ArgTypeUint64:
        ret = js_mknum((uint64_t)rc);
        break;
    case ArgTypeSint64:
        ret = js_mknum((int64_t)rc);
        break;
    case ArgTypeFloat:
        FURI_LOG_I(TAG, "  ret double %f", (double)(float)rc);
        ret = js_mknum((float)rc);
        break;
    case ArgTypeDouble:
        FURI_LOG_I(TAG, "  ret double %f", (double)rc);
        ret = js_mknum((double)rc);
        break;
    case ArgTypePointer:
        FURI_LOG_I(TAG, "  ret pointer 0x%p", (void*)rc);
        ret = js_mknum((uint32_t)rc);
        break;
    case ArgTypeString:
        FURI_LOG_I(TAG, "  ret string \"%s\"", (char*)rc);
        ret = js_mkstr(js, (char*)rc, strlen((char*)rc));
        break;
    default:
        FURI_LOG_E(TAG, "FFI: ret error");
        ret = js_mkundef();
        break;
    }

    FURI_LOG_I(TAG, "}");

    return ret;
}

static jsval_t js_global_ffires(struct js* js, jsval_t* args, int nargs) {
    UNUSED(js);
    UNUSED(nargs);
    Elf32_Addr addr;
    const char* name = js_getstr(js, args[0], NULL);
    uint32_t hash = elf_symbolname_hash(name);
    if(!firmware_api_interface->resolver_callback(firmware_api_interface, hash, &addr)) {
        FURI_LOG_E(TAG, "RES: cannot find %s", name);
        return js_mkundef();
    }

    FURI_LOG_I(TAG, "RES: \"%s\" = 0x%p", name, (void*)addr);
    return js_mknum((uint32_t)addr);
}

static void notify(const NotificationSequence* sequence) {
    NotificationApp* notification = furi_record_open(RECORD_NOTIFICATION);
    notification_message(notification, sequence);
    furi_record_close(RECORD_NOTIFICATION);
}

static jsval_t js_global_delay(struct js* js, jsval_t* args, int nargs) {
    UNUSED(js);
    UNUSED(nargs);
    furi_delay_ms(js_getnum(args[0]));
    return js_mkundef();
}

static jsval_t js_led_red(struct js* js, jsval_t* args, int nargs) {
    UNUSED(js);
    UNUSED(args);
    UNUSED(nargs);
    notify(&sequence_set_only_red_255);
    return js_mkundef();
}

static jsval_t js_led_green(struct js* js, jsval_t* args, int nargs) {
    UNUSED(js);
    UNUSED(args);
    UNUSED(nargs);
    notify(&sequence_set_only_green_255);
    return js_mkundef();
}

static jsval_t js_led_blue(struct js* js, jsval_t* args, int nargs) {
    UNUSED(js);
    UNUSED(args);
    UNUSED(nargs);
    notify(&sequence_set_only_blue_255);
    return js_mkundef();
}

static jsval_t js_global_print(struct js* js, jsval_t* args, int nargs) {
    for(int i = 0; i < nargs; i++) {
        const char* space = i == 0 ? "" : " ";
        printf("%s%s", space, js_str(js, args[i]));
    }
    printf("\r\n");
    return js_mkundef();
}

static jsval_t js_global_print_pointer(struct js* js, jsval_t* args, int nargs) {
    UNUSED(js);
    UNUSED(nargs);
    uint32_t p = (uint32_t)js_getnum(args[0]);
    printf("%lu\r\n", p);
    return js_mkundef();
}

static jsval_t js_global_require(struct js* js, jsval_t* args, int nargs) {
    UNUSED(nargs);
    Storage* storage = furi_record_open(RECORD_STORAGE);
    jsval_t res = js_mkundef();
    File* file = storage_file_alloc(storage);
    char* data = NULL;
    const char* name = js_getstr(js, args[0], NULL);

    do {
        if(!storage_file_open(file, name, FSAM_READ, FSOM_OPEN_EXISTING)) {
            FURI_LOG_E("JS", "Cannot open %s", name);
            break;
        }

        size_t size = storage_file_size(file);
        data = (char*)malloc(size + 1);
        if(storage_file_read(file, data, size) != size) {
            FURI_LOG_E("JS", "Cannot read %s", name);
            break;
        }
        data[size] = '\0';
        res = js_eval(js, data, ~0U);

        if(js_type(res) == JS_ERR) {
            FURI_LOG_E("JS", "%s: %s", name, js_str(js, res));
        }

        free(data);
    } while(false);

    if(data) free(data);
    storage_file_free(file);

    furi_record_close(RECORD_STORAGE);
    return res;
}

static bool js_do(const char* text) {
    bool result = false;
    const size_t memory_size = 16 * 1024;
    uint8_t* memory = (uint8_t*)malloc(memory_size);
    struct js* js = js_create(memory, memory_size);

    jsval_t global = js_glob(js);
    jsval_t led = js_mkobj(js);
    jsval_t arg = js_mkobj(js);

    js_set(js, global, "arg", arg);
    js_set(js, arg, "none", js_mknum(ArgTypeVoid));
    js_set(js, arg, "uint8", js_mknum(ArgTypeUint8));
    js_set(js, arg, "sint8", js_mknum(ArgTypeSint8));
    js_set(js, arg, "uint16", js_mknum(ArgTypeUint16));
    js_set(js, arg, "sint16", js_mknum(ArgTypeSint16));
    js_set(js, arg, "uint32", js_mknum(ArgTypeUint32));
    js_set(js, arg, "sint32", js_mknum(ArgTypeSint32));
    js_set(js, arg, "uint64", js_mknum(ArgTypeUint64));
    js_set(js, arg, "sint64", js_mknum(ArgTypeSint64));
    js_set(js, arg, "float", js_mknum(ArgTypeFloat));
    js_set(js, arg, "double", js_mknum(ArgTypeDouble));
    js_set(js, arg, "pointer", js_mknum(ArgTypePointer));
    js_set(js, arg, "string", js_mknum(ArgTypeString));
    js_set(js, global, "fficall", js_mkfun(js_global_fficall));
    js_set(js, global, "ffires", js_mkfun(js_global_ffires));

    js_set(js, global, "delay", js_mkfun(js_global_delay));
    js_set(js, global, "print", js_mkfun(js_global_print));
    js_set(js, global, "print_pointer", js_mkfun(js_global_print_pointer));
    js_set(js, global, "require", js_mkfun(js_global_require));

    js_set(js, global, "led", led);
    js_set(js, led, "red", js_mkfun(js_led_red));
    js_set(js, led, "green", js_mkfun(js_led_green));
    js_set(js, led, "blue", js_mkfun(js_led_blue));

    jsval_t res = js_eval(js, text, ~0U);

    size_t total_ram = 0, min_ram = 0, stack = 0;
    js_stats(js, &total_ram, &min_ram, &stack);
    FURI_LOG_I("JS", "RAM: total %u, lowest free %u, C stack: %u", total_ram, min_ram, stack);

    if(js_type(res) == JS_ERR) {
        FURI_LOG_E("JS", "%s", js_str(js, res));
        result = false;
    } else {
        result = true;
    }

    free(memory);
    return result;
}

int32_t elk_js_app(void* arg) {
    Storage* storage = furi_record_open(RECORD_STORAGE);
    DialogsApp* dialogs = furi_record_open(RECORD_DIALOGS);
    FuriString* name = furi_string_alloc_set(APP_ASSETS_PATH());
    File* file = storage_file_alloc(storage);
    char* data = NULL;

    do {
        if(arg != NULL && strlen(arg) > 0) {
            furi_string_set(name, (const char*)arg);
        } else {
            DialogsFileBrowserOptions browser_options;
            dialog_file_browser_set_basic_options(&browser_options, ".js", NULL);
            if(!dialog_file_browser_show(dialogs, name, name, &browser_options)) break;
        }

        if(!storage_file_open(file, furi_string_get_cstr(name), FSAM_READ, FSOM_OPEN_EXISTING))
            break;
        size_t size = storage_file_size(file);
        data = (char*)malloc(size + 1);
        if(storage_file_read(file, data, size) != size) break;
        data[size] = '\0';
        js_do(data);
    } while(false);

    if(data) free(data);
    furi_string_free(name);
    storage_file_free(file);
    furi_record_close(RECORD_STORAGE);
    furi_record_close(RECORD_DIALOGS);
    return 0;
}