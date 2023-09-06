// main.c
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <furi.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/view.h>
#include <gui/modules/text_box.h>
#include <gui/modules/dialog_ex.h>
#include <storage/storage.h>

#include "microvium.h"

#define JS_APP_PATH_FOLDER STORAGE_APP_DATA_PATH_PREFIX
#define TAG "microvium"

static int32_t js_run(void* context);

// A function in the host (this file) for the VM to call
#define IMPORT_FLIPPER_FURI_DELAY_MS 1
#define IMPORT_FLIPPER_CANVAS_STOP 2
#define IMPORT_FLIPPER_CANVAS_SET_FONT 3
#define IMPORT_FLIPPER_CANVAS_DRAW_STR 4
#define IMPORT_FLIPPER_CANVAS_DRAW_STR_ALIGNED 5
#define IMPORT_CONSOLE_CLEAR 6
#define IMPORT_CONSOLE_LOG 7
#define IMPORT_CONSOLE_WARN 8

// A function exported by VM to for the host to call
const mvm_VMExportID MAIN = 1;
/* Use when needed
const mvm_VMExportID INIT = 2;
*/

mvm_TeError resolveImport(mvm_HostFunctionID id, void*, mvm_TfHostFunction* out);
mvm_TeError flipper_furi_delay_ms(
    mvm_VM* vm,
    mvm_HostFunctionID funcID,
    mvm_Value* result,
    mvm_Value* args,
    uint8_t argCount);
mvm_TeError flipper_canvas_stop(
    mvm_VM* vm,
    mvm_HostFunctionID funcID,
    mvm_Value* result,
    mvm_Value* args,
    uint8_t argCount);
mvm_TeError flipper_canvas_set_font(
    mvm_VM* vm,
    mvm_HostFunctionID funcID,
    mvm_Value* result,
    mvm_Value* args,
    uint8_t argCount);
mvm_TeError flipper_canvas_draw_str(
    mvm_VM* vm,
    mvm_HostFunctionID funcID,
    mvm_Value* result,
    mvm_Value* args,
    uint8_t argCount);
mvm_TeError flipper_canvas_draw_str_aligned(
    mvm_VM* vm,
    mvm_HostFunctionID funcID,
    mvm_Value* result,
    mvm_Value* args,
    uint8_t argCount);
mvm_TeError console_clear(
    mvm_VM* vm,
    mvm_HostFunctionID funcID,
    mvm_Value* result,
    mvm_Value* args,
    uint8_t argCount);
mvm_TeError console_log(
    mvm_VM* vm,
    mvm_HostFunctionID funcID,
    mvm_Value* result,
    mvm_Value* args,
    uint8_t argCount);
mvm_TeError console_warn(
    mvm_VM* vm,
    mvm_HostFunctionID funcID,
    mvm_Value* result,
    mvm_Value* args,
    uint8_t argCount);

typedef enum {
    MyEventTypeKey,
    MyEventTypeDone,
} MyEventType;

typedef struct {
    MyEventType type; // The reason for this event.
    InputEvent input; // This data is specific to keypress data.
} MyEvent;

typedef enum {
    JSDisplay,
    JSConsole,
    //JSConfirm,
} ViewId;

FuriMessageQueue* queue;
ViewId current_view;

ViewDispatcher* view_dispatcher;

TextBox* text_box;

typedef struct {
    FuriThread* thread;
} JSRtThread;

typedef struct {
    FuriString* conLog;
} Console;

Console* console;

typedef enum {
    CNone,
    CDrawStr,
    CDrawStrAli,
} CDrawEvent;

typedef struct {
    CDrawEvent cEvent;
    Font font;
    const char* str;
    int x;
    int y;
    Align horizontal;
    Align vertical;
} Display;

Display* display;

size_t fileSize;
uint8_t* fileBuff;

static void draw_callback(Canvas* canvas, void* context) {
    UNUSED(context);
    canvas_set_font(canvas, display->font);
    if(display->cEvent == CDrawStr) {
        canvas_draw_str(canvas, display->x, display->y, display->str);
    } else if(display->cEvent == CDrawStrAli) {
        canvas_draw_str_aligned(
            canvas, display->x, display->y, display->horizontal, display->vertical, display->str);
    }
}

static bool input_callback(InputEvent* input_event, void* context) {
    UNUSED(context);
    bool handled = false;
    // we set our callback context to be the view_dispatcher.

    if(input_event->type == InputTypeShort) {
        if(input_event->key == InputKeyBack) {
            // Default back handler.
            handled = false;
        } else if(input_event->key == InputKeyOk) {
            // switch the view!
            view_dispatcher_send_custom_event(view_dispatcher, 42);
            handled = true;
        }
    }

    return handled;
}

bool navigation_event_callback(void* context) {
    UNUSED(context);
    // We did not handle the event, so return false.
    return false;
}

bool custom_event_callback(void* context, uint32_t event) {
    UNUSED(context);
    bool handled = false;

    if(event == 42) {
        if(current_view == JSDisplay) {
            current_view = JSConsole;
        }

        view_dispatcher_switch_to_view(view_dispatcher, current_view);
        handled = true;
    }

    // NOTE: The return value is not currently used by the ViewDispatcher.
    return handled;
}

static uint32_t exit_console_callback(void* context) {
    UNUSED(context);
    return JSDisplay;
}

static int32_t js_run(void* context) {
    UNUSED(context);

    mvm_TeError err;
    mvm_VM* vm;
    mvm_Value main;
    mvm_Value result;

    // Restore the VM from the snapshot
    err = mvm_restore(&vm, fileBuff, fileSize, NULL, resolveImport);
    if(err != MVM_E_SUCCESS) {
        FURI_LOG_E(TAG, "Error with restore: %d", err);
        return err;
    }

    // Find the "sayHello" function exported by the VM
    err = mvm_resolveExports(vm, &MAIN, &main, 1);
    if(err != MVM_E_SUCCESS) {
        FURI_LOG_E(TAG, "Error with exports: %d", err);
        return err;
    }

    // Call "main"
    err = mvm_call(vm, main, &result, NULL, 0);
    if(err != MVM_E_SUCCESS) {
        FURI_LOG_E(TAG, "Error with call: %d", err);
        return err;
    }

    // Clean up
    mvm_runGC(vm, true);

    return 0;
}

int32_t js_app() {
    JSRtThread* jsThread = malloc(sizeof(JSRtThread));

    Storage* storage = furi_record_open(RECORD_STORAGE);
    File* bytecode = storage_file_alloc(storage);
    storage_file_open(bytecode, APP_DATA_PATH("script.mvm-bc"), FSAM_READ, FSOM_OPEN_EXISTING);
    fileSize = storage_file_size(bytecode);
    FURI_LOG_I("microvium", "File Size: %d", fileSize);
    fileBuff = malloc(fileSize);
    storage_file_read(bytecode, fileBuff, fileSize);
    storage_file_close(bytecode);
    storage_file_free(bytecode);
    furi_record_close(RECORD_STORAGE);

    jsThread->thread = furi_thread_alloc_ex("microium", 1024, js_run, jsThread);

    view_dispatcher = view_dispatcher_alloc();

    // For this demo, we just use view_dispatcher as our application context.
    void* context = view_dispatcher;

    View* view1 = view_alloc();
    view_set_context(view1, context);
    view_set_draw_callback(view1, draw_callback);
    view_set_input_callback(view1, input_callback);
    view_set_orientation(view1, ViewOrientationHorizontal);

    text_box = text_box_alloc();
    text_box_set_font(text_box, TextBoxFontText);
    view_set_previous_callback(text_box_get_view(text_box), exit_console_callback);

    // set param 1 of custom event callback (impacts tick and navigation too).
    view_dispatcher_set_event_callback_context(view_dispatcher, context);
    view_dispatcher_set_navigation_event_callback(view_dispatcher, navigation_event_callback);
    view_dispatcher_set_custom_event_callback(view_dispatcher, custom_event_callback);
    view_dispatcher_enable_queue(view_dispatcher);
    view_dispatcher_add_view(view_dispatcher, JSDisplay, view1);
    view_dispatcher_add_view(view_dispatcher, JSConsole, text_box_get_view(text_box));

    Gui* gui = furi_record_open(RECORD_GUI);
    view_dispatcher_attach_to_gui(view_dispatcher, gui, ViewDispatcherTypeFullscreen);
    current_view = JSDisplay;
    view_dispatcher_switch_to_view(view_dispatcher, current_view);

    // console init
    console = malloc(sizeof(Console));
    console->conLog = furi_string_alloc();

    // display init and set defaults
    display = malloc(sizeof(Display));
    display->cEvent = CNone;
    display->font = FontSecondary;

    furi_thread_start(jsThread->thread);
    view_dispatcher_run(view_dispatcher);

    furi_thread_join(jsThread->thread);
    furi_thread_free(jsThread->thread);
    free(jsThread);

    furi_string_free(console->conLog);
    free(console);

    free(display);

    view_dispatcher_remove_view(view_dispatcher, JSDisplay);
    view_dispatcher_remove_view(view_dispatcher, JSConsole);
    furi_record_close(RECORD_GUI);
    view_dispatcher_free(view_dispatcher);

    return 0;
}

void fatalError(void* vm, int e) {
    UNUSED(vm);
    FURI_LOG_E(TAG, "Error: %d\n", e);
    furi_crash("Microvium fatal error");
}

/*
 * This function is called by `mvm_restore` to search for host functions
 * imported by the VM based on their ID. Given an ID, it needs to pass back
 * a pointer to the corresponding C function to be used by the VM.
 */
mvm_TeError resolveImport(mvm_HostFunctionID funcID, void* context, mvm_TfHostFunction* out) {
    UNUSED(context);
    if(funcID == IMPORT_FLIPPER_FURI_DELAY_MS) {
        *out = flipper_furi_delay_ms;
    } else if(funcID == IMPORT_FLIPPER_CANVAS_SET_FONT) {
        *out = flipper_canvas_set_font;
        return MVM_E_SUCCESS;
    } else if(funcID == IMPORT_FLIPPER_CANVAS_DRAW_STR) {
        *out = flipper_canvas_draw_str;
        return MVM_E_SUCCESS;
    } else if(funcID == IMPORT_FLIPPER_CANVAS_DRAW_STR_ALIGNED) {
        *out = flipper_canvas_draw_str_aligned;
    } else if(funcID == IMPORT_CONSOLE_LOG) {
        *out = console_log;
        return MVM_E_SUCCESS;
    } else if(funcID == IMPORT_CONSOLE_CLEAR) {
        *out = console_clear;
        return MVM_E_SUCCESS;
    } else if(funcID == IMPORT_CONSOLE_WARN) {
        *out = console_warn;
        return MVM_E_SUCCESS;
    }
    return MVM_E_UNRESOLVED_IMPORT;
}

mvm_TeError flipper_furi_delay_ms(
    mvm_VM* vm,
    mvm_HostFunctionID funcID,
    mvm_Value* result,
    mvm_Value* args,
    uint8_t argCount) {
    UNUSED(funcID);
    UNUSED(result);
    furi_assert(argCount == 1);
    FURI_LOG_I(TAG, "delay_ms()");
    furi_delay_ms((int32_t)mvm_toInt32(vm, args[0]));
    return MVM_E_SUCCESS;
}

mvm_TeError flipper_canvas_stop(
    mvm_VM* vm,
    mvm_HostFunctionID funcID,
    mvm_Value* result,
    mvm_Value* args,
    uint8_t argCount) {
    UNUSED(vm);
    UNUSED(funcID);
    UNUSED(result);
    UNUSED(args);
    furi_assert(argCount == 0);
    FURI_LOG_I(TAG, "canvas_stop()");
    display->cEvent = CNone;
    return MVM_E_SUCCESS;
}

mvm_TeError flipper_canvas_set_font(
    mvm_VM* vm,
    mvm_HostFunctionID funcID,
    mvm_Value* result,
    mvm_Value* args,
    uint8_t argCount) {
    UNUSED(funcID);
    UNUSED(result);
    furi_assert(argCount == 1);
    FURI_LOG_I(TAG, "canvas_set_font()");
    // display->cEvent = CSetFont;
    display->font = mvm_toInt32(vm, args[0]);
    // display->cEvent = CNone;
    return MVM_E_SUCCESS;
}

mvm_TeError flipper_canvas_draw_str(
    mvm_VM* vm,
    mvm_HostFunctionID funcID,
    mvm_Value* result,
    mvm_Value* args,
    uint8_t argCount) {
    UNUSED(funcID);
    UNUSED(result);
    furi_assert(argCount == 3);
    FURI_LOG_I(TAG, "canvas_draw_str()");
    display->x = (int32_t)mvm_toInt32(vm, args[0]);
    display->y = (int32_t)mvm_toInt32(vm, args[1]);
    display->str = (const char*)mvm_toStringUtf8(vm, args[2], NULL);
    display->cEvent = CDrawStr;
    return MVM_E_SUCCESS;
}

mvm_TeError flipper_canvas_draw_str_aligned(
    mvm_VM* vm,
    mvm_HostFunctionID funcID,
    mvm_Value* result,
    mvm_Value* args,
    uint8_t argCount) {
    UNUSED(funcID);
    UNUSED(result);
    furi_assert(argCount == 5);
    FURI_LOG_I(TAG, "canvas_draw_str_aligned()");
    display->x = (int32_t)mvm_toInt32(vm, args[0]);
    display->y = (int32_t)mvm_toInt32(vm, args[1]);
    display->horizontal = (int32_t)mvm_toInt32(vm, args[2]);
    display->vertical = (int32_t)mvm_toInt32(vm, args[3]);
    display->str = (const char*)mvm_toStringUtf8(vm, args[4], NULL);
    display->cEvent = CDrawStrAli;
    return MVM_E_SUCCESS;
}

mvm_TeError console_clear(
    mvm_VM* vm,
    mvm_HostFunctionID funcID,
    mvm_Value* result,
    mvm_Value* args,
    uint8_t argCount) {
    UNUSED(vm);
    UNUSED(funcID);
    UNUSED(result);
    UNUSED(args);
    furi_assert(argCount == 0);
    FURI_LOG_I(TAG, "console.clear()");
    furi_string_reset(console->conLog);
    text_box_set_text(text_box, furi_string_get_cstr(console->conLog));
    return MVM_E_SUCCESS;
}

mvm_TeError console_log(
    mvm_VM* vm,
    mvm_HostFunctionID funcID,
    mvm_Value* result,
    mvm_Value* args,
    uint8_t argCount) {
    UNUSED(funcID);
    UNUSED(result);
    furi_assert(argCount == 1);
    FURI_LOG_I(TAG, "console.log()");
    furi_string_cat_printf(
        console->conLog, "%s\n", (const char*)mvm_toStringUtf8(vm, args[0], NULL));
    text_box_set_text(text_box, furi_string_get_cstr(console->conLog));
    return MVM_E_SUCCESS;
}

mvm_TeError console_warn(
    mvm_VM* vm,
    mvm_HostFunctionID funcID,
    mvm_Value* result,
    mvm_Value* args,
    uint8_t argCount) {
    UNUSED(funcID);
    UNUSED(result);
    furi_assert(argCount == 1);
    FURI_LOG_I(TAG, "console.warn()");
    FURI_LOG_W(TAG, "%s\n", (const char*)mvm_toStringUtf8(vm, args[0], NULL));
    return MVM_E_SUCCESS;
}
